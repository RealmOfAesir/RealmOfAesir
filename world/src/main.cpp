/*
    RealmOfAesirWorld
    Copyright (C) 2017  Michael de Lang

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <easylogging++.h>
#include <nlohmann/json.hpp>
#include <kafka_consumer.h>
#include <kafka_producer.h>
#include <admin_messages/admin_quit_message.h>
#include <readerwriterqueue.h>

#include <signal.h>
#include <string>
#include <fstream>
#include <streambuf>
#include <vector>
#include <thread>
#include <unordered_map>
#include <atomic>
#include <exceptions.h>
#include <roa_di.h>
#include <macros.h>
#include <database_pool.h>
#include <repositories/locations_repository.h>
#include <repositories/maps_repository.h>
#include <repositories/settings_repository.h>
#include <message_handlers/world/world_get_characters_handler.h>
#include <world/world.h>
#include "message_handlers/message_dispatcher.h"
#include "message_handlers/world/world_create_character_handler.h"
#include "message_handlers/world/world_play_character_handler.h"
#include "database_transaction.h"
#include "config.h"
#include <events/event.h>

using namespace std;
using namespace roa;
using namespace pqxx;

#ifdef EXPERIMENTAL_OPTIONAL
using namespace experimental;
#endif

using json = nlohmann::json;


INITIALIZE_EASYLOGGINGPP

atomic<bool> quit{false};

void on_sigint(int sig) {
    quit = true;
}

void init_extras() noexcept {
    ios::sync_with_stdio(false);
    signal(SIGINT, on_sigint);
}

void init_logger(Config const config) noexcept {
    el::Configurations defaultConf;
    defaultConf.setGlobally(el::ConfigurationType::Format, "%datetime %level: %msg");
    defaultConf.setGlobally(el::ConfigurationType::Filename, "logs/world-%datetime{%Y%M%d%h%m}.log");
    if(!config.debug_level.empty()) {
        if(config.debug_level == "error") {
            defaultConf.set(el::Level::Warning, el::ConfigurationType::Enabled, "false");
            defaultConf.set(el::Level::Info, el::ConfigurationType::Enabled, "false");
            defaultConf.set(el::Level::Debug, el::ConfigurationType::Enabled, "false");
            defaultConf.set(el::Level::Trace, el::ConfigurationType::Enabled, "false");
        } else if(config.debug_level == "warning") {
            defaultConf.set(el::Level::Info, el::ConfigurationType::Enabled, "false");
            defaultConf.set(el::Level::Debug, el::ConfigurationType::Enabled, "false");
            defaultConf.set(el::Level::Trace, el::ConfigurationType::Enabled, "false");
        } else if(config.debug_level == "info") {
            defaultConf.set(el::Level::Debug, el::ConfigurationType::Enabled, "false");
            defaultConf.set(el::Level::Trace, el::ConfigurationType::Enabled, "false");
        } else if(config.debug_level == "debug") {
            defaultConf.set(el::Level::Trace, el::ConfigurationType::Enabled, "false");
        }
    }
    el::Loggers::reconfigureAllLoggers(defaultConf);
    LOG(INFO) << "debug level: " << config.debug_level;
}



STD_OPTIONAL<Config> parse_env_file() {
    string env_contents;
    ifstream env(".env_world");

    if(!env) {
        LOG(ERROR) << "[main] no .env_world file found. Please make one.";
        return {};
    }

    env.seekg(0, ios::end);
    env_contents.resize(env.tellg());
    env.seekg(0, ios::beg);
    env.read(&env_contents[0], env_contents.size());
    env.close();

    auto env_json = json::parse(env_contents);
    Config config;

    try {
        config.broker_list = env_json["BROKER_LIST"];
    } catch (const std::exception& e) {
        LOG(ERROR) << "[main] BROKER_LIST missing in .env_world file.";
        return {};
    }

    try {
        config.group_id = env_json["GROUP_ID"];
    } catch (const std::exception& e) {
        LOG(ERROR) << "[main] GROUP_ID missing in .env_world file.";
        return {};
    }

    try {
        config.server_id = env_json["SERVER_ID"];
    } catch (const std::exception& e) {
        LOG(ERROR) << "[main] SERVER_ID missing in .env_world file.";
        return {};
    }

    if(config.server_id == 0) {
        LOG(ERROR) << "[main] SERVER_ID has to be greater than 0";
        return {};
    }

    try {
        config.connection_string = env_json["CONNECTION_STRING"];
    } catch (const std::exception& e) {
        LOG(ERROR) << "[main] CONNECTION_STRING missing in .env_world file.";
        return {};
    }

    // TODO the following settings should be moved into the database

    try {
        config.debug_level = env_json["DEBUG_LEVEL"];
    } catch (const std::exception& e) {
        LOG(ERROR) << "[main] DEBUG_LEVEL missing in .env_world file.";
        return {};
    }

    try {
        config.world_name = env_json["WORLD_NAME"];
    } catch (const std::exception& e) {
        LOG(ERROR) << "[main] WORLD_NAME missing in .env_world file.";
        return {};
    }

    try {
        config.tick_length = env_json["TICK_LENGTH"];
    } catch (const std::exception& e) {
        LOG(ERROR) << "[main] TICK_LENGTH missing in .env_world file.";
        return {};
    }

    config.debug_roa_library = false;

    return config;
}

unique_ptr<thread> create_consumer_thread(Config& config, moodycamel::ReaderWriterQueue<shared_ptr<player_event>>& player_event_queue, shared_ptr<database_pool> db_pool,
                                          shared_ptr<ikafka_consumer<false>> consumer, shared_ptr<ikafka_producer<false>> producer) {
    if (!consumer || !producer) {
        LOG(ERROR) << NAMEOF(create_consumer_thread) << " one of the arguments are null";
        throw runtime_error("[main:consumer] one of the arguments are null");
    }

    return make_unique<thread>([=, &config, &player_event_queue] {
        LOG(INFO) << NAMEOF(create_consumer_thread) << " starting consumer thread";

        auto backend_injector = boost::di::make_injector(
                boost::di::bind<idatabase_transaction>.to<database_transaction>(),
                boost::di::bind<idatabase_connection>.to<database_connection>(),
                boost::di::bind<idatabase_pool>.to(db_pool),
                boost::di::bind<ilocations_repository>.to<locations_repository>(),
                boost::di::bind<imaps_repository>.to<maps_repository>(),
                boost::di::bind<iplayers_repository>.to<players_repository>(),
                boost::di::bind<isettings_repository>.to<settings_repository>()
        );

        auto players_repo = backend_injector.create<players_repository>();

        message_dispatcher<false> world_server_msg_dispatcher;

        world_server_msg_dispatcher.register_handler<world_create_character_handler, Config&, moodycamel::ReaderWriterQueue<std::shared_ptr<player_event>>&, iplayers_repository&, shared_ptr<ikafka_producer<false>>>(config, player_event_queue, players_repo, producer);
        world_server_msg_dispatcher.register_handler<world_play_character_handler, Config&, moodycamel::ReaderWriterQueue<std::shared_ptr<player_event>>&, iplayers_repository&, shared_ptr<ikafka_producer<false>>>(config, player_event_queue, players_repo, producer);
        world_server_msg_dispatcher.register_handler<world_get_characters_handler, Config&, iplayers_repository&, shared_ptr<ikafka_producer<false>>>(config, players_repo, producer);

        consumer->start(config.broker_list, config.group_id, std::vector<std::string>{"server-" + to_string(config.server_id), "world_messages", "broadcast"}, 50);

        LOG(INFO) << NAMEOF(create_consumer_thread) << " started consumer thread";

        while (!quit) {
            try {
                auto msg = consumer->try_get_message(50);
                if (get<1>(msg)) {
                    LOG(INFO) << "got msg";
                    world_server_msg_dispatcher.trigger_handler(msg);
                }
            } catch (serialization_exception &e) {
                LOG(ERROR) << NAMEOF(create_consumer_thread) << " received serialization exception " << e.what();
            } catch(exception &e) {
                LOG(ERROR) << NAMEOF(create_consumer_thread) << " received exception " << e.what();
            }
        }

        consumer->close();
    });
}

unique_ptr<thread> create_world_thread(Config& config, moodycamel::ReaderWriterQueue<shared_ptr<player_event>>& player_event_queue, shared_ptr<database_pool> db_pool,
                                       shared_ptr<ikafka_producer<false>> producer) {
    if (!producer) {
        LOG(ERROR) << NAMEOF(create_world_thread) << " one of the arguments are null";
        throw runtime_error("[main:world] one of the arguments are null");
    }

    return make_unique<thread>([=, &config, &player_event_queue] {
        LOG(INFO) << NAMEOF(create_world_thread) << " starting world thread";

        world w;
        w.load_from_database(db_pool, config, player_event_queue, producer);
        auto next_tick = chrono::system_clock::now() + chrono::milliseconds(config.tick_length);
        auto second_next_tick = next_tick + chrono::milliseconds(config.tick_length);
        while (!quit) {
            auto now = chrono::system_clock::now();
            if(now < next_tick) {
                this_thread::sleep_until(next_tick);
            }
            w.do_tick(config.tick_length);
            next_tick = second_next_tick;
            second_next_tick = second_next_tick + chrono::milliseconds(config.tick_length);
        }
    });
}

int main() {
    Config config;
    try {
        auto config_opt = parse_env_file();
        if(!config_opt) {
            return 1;
        }
        config = config_opt.value();
    } catch (const std::exception& e) {
        LOG(ERROR) << "[main] .env file is malformed json.";
        return 1;
    }

    init_logger(config);
    init_extras();

    auto db_pool = make_shared<database_pool>();
    db_pool->create_connections(config.connection_string, 2);
    auto common_injector = create_common_di_injector();
    moodycamel::ReaderWriterQueue<shared_ptr<player_event>> player_event_queue;

    auto producer = common_injector.create<shared_ptr<ikafka_producer<false>>>();
    auto consumer = common_injector.create<shared_ptr<ikafka_consumer<false>>>();

    producer->start(config.broker_list, 50);

    try {
        LOG(INFO) << "[main] starting main thread";

        auto consumer_thread = create_consumer_thread(config, player_event_queue, db_pool, consumer, producer);
        auto world_thread = create_world_thread(config, player_event_queue, db_pool, producer);

        while (!quit) {
            try {
                producer->poll(50);
            } catch (serialization_exception &e) {
                LOG(ERROR) << NAMEOF(main) << " received serialization exception " << e.what();
            } catch(exception &e) {
                LOG(ERROR) << NAMEOF(main) << " received exception " << e.what();
            }
        }

        LOG(INFO) << "[main] closing";

        producer->close();
        consumer_thread->join();
        world_thread->join();
    } catch (const runtime_error& e) {
        LOG(ERROR) << "[main] error: " << typeid(e).name() << "-" << e.what();
    }

    return 0;
}
