/*
    Realm of Aesir backend
    Copyright (C) 2016  Michael de Lang

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
#include <uWS.h>
#include <nlohmann/json.hpp>
#include <kafka_consumer.h>
#include <kafka_producer.h>
#include <admin_messages/admin_quit_message.h>

#include <csignal>
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
#include <libcuckoo/cuckoohash_map.hh>
#include <src/message_handlers/client/client_create_character_handler.h>
#include <src/message_handlers/client/client_get_characters_handler.h>
#include <src/message_handlers/client/client_play_character_handler.h>
#include <src/message_handlers/gateway/gateway_send_map_handler.h>
#include <src/message_handlers/gateway/gateway_get_characters_response_handler.h>
#include "src/message_handlers/gateway/gateway_chat_send_handler.h"
#include "src/message_handlers/gateway/gateway_login_response_handler.h"
#include "src/message_handlers/gateway/gateway_register_response_handler.h"
#include "src/message_handlers/gateway/gateway_error_response_handler.h"
#include "src/message_handlers/gateway/gateway_quit_handler.h"
#include "message_handlers/client/client_admin_quit_handler.h"
#include "message_handlers/client/client_login_handler.h"
#include "message_handlers/client/client_register_handler.h"
#include "message_handlers/client/client_chat_send_handler.h"
#include "user_connection.h"
#include "config.h"

using namespace std;
using namespace roa;

#ifdef EXPERIMENTAL_OPTIONAL
using namespace experimental;
#endif

using json = nlohmann::json;


INITIALIZE_EASYLOGGINGPP

atomic<bool> quit{false};
atomic<bool> uwsQuit{false};

void on_sigint(int sig) {
    quit = true;
}

void init_extras() noexcept {
    ios::sync_with_stdio(false);
    signal(SIGINT, on_sigint);
}

void init_logger(Config const & config) noexcept {
    el::Configurations defaultConf;
    defaultConf.setGlobally(el::ConfigurationType::Format, "%datetime %level: %msg");
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
    LOG(INFO) << NAMEOF(init_logger) << " debug level: " << config.debug_level;
}



STD_OPTIONAL<Config> parse_env_file() {
    string env_contents;
    ifstream env(".env_gateway");

    if(!env) {
        LOG(ERROR) << NAMEOF(parse_env_file) << " no .env_gateway file found. Please make one.";
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
        LOG(ERROR) << NAMEOF(parse_env_file) << " BROKER_LIST missing in .env_gateway file.";
        return {};
    }

    try {
        config.group_id = env_json["GROUP_ID"];
    } catch (const std::exception& e) {
        LOG(ERROR) << NAMEOF(parse_env_file) << " GROUP_ID missing in .env_gateway file.";
        return {};
    }

    try {
        config.server_id = env_json["SERVER_ID"];
    } catch (const std::exception& e) {
        LOG(ERROR) << NAMEOF(parse_env_file) << " SERVER_ID missing in .env_gateway file.";
        return {};
    }

    if(config.server_id == 0) {
        LOG(ERROR) << NAMEOF(parse_env_file) << " SERVER_ID has to be greater than 0";
        return {};
    }

    try {
        config.connection_string = env_json["CONNECTION_STRING"];
    } catch (const std::exception& e) {
        LOG(ERROR) << NAMEOF(parse_env_file) << " CONNECTION_STRING missing in .env_gateway file.";
        return {};
    }

    try {
        config.debug_level = env_json["DEBUG_LEVEL"];
    } catch (const std::exception& e) {
        LOG(ERROR) << NAMEOF(parse_env_file) << " DEBUG_LEVEL missing in .env_gateway file.";
        return {};
    }

    return config;
}

unique_ptr<thread> create_uws_thread(Config config, uWS::Hub &h, shared_ptr<ikafka_producer<false>> producer, shared_ptr<cuckoohash_map<string, user_connection>> connections) {
    if(!producer) {
        LOG(ERROR) << NAMEOF(create_uws_thread) << " one of the arguments are null";
        throw runtime_error("[main:uws] one of the arguments are null");
    }

    return make_unique<thread>([=, &h]{
        try {
            message_dispatcher<false> client_msg_dispatcher;

            client_msg_dispatcher.register_handler<client_admin_quit_handler>(config, producer);
            client_msg_dispatcher.register_handler<client_login_handler>(config, producer);
            client_msg_dispatcher.register_handler<client_register_handler>(config, producer);
            client_msg_dispatcher.register_handler<client_chat_send_handler>(config, producer);
            client_msg_dispatcher.register_handler<client_create_character_handler>(config, producer);
            client_msg_dispatcher.register_handler<client_get_characters_handler>(config, producer);
            client_msg_dispatcher.register_handler<client_play_character_handler>(config, producer);

            h.onMessage([&](uWS::WebSocket<uWS::SERVER> *ws, char *recv_msg, size_t length, uWS::OpCode opCode) {
                LOG(DEBUG) << NAMEOF(create_uws_thread) << " Got message from wss";
                if(opCode == uWS::OpCode::TEXT) {
                    LOG(INFO) << NAMEOF(create_uws_thread) << " Got message from wss";
                    string str(recv_msg, length);
                    LOG(DEBUG) << NAMEOF(create_uws_thread) << " " << str;
                    user_connection connection;

                    string key = user_connection::AddressToString(ws->getAddress());

                    if(unlikely(!connections->find(key, connection))) {
                        LOG(ERROR) << NAMEOF(create_uws_thread) << " got message from " << key << " without connection";
                        ws->terminate();
                        return;
                    }

                    try {
                        auto msg = message<true>::deserialize<false>(str);
                        if (get<1>(msg)) {
                            client_msg_dispatcher.trigger_handler(msg, make_optional(ref(connection)));
                        }
                    } catch(const std::exception& e) {
                        LOG(ERROR) << NAMEOF(create_uws_thread)
                                   << " exception when deserializing message, disconnecting " << connection.state
                                   << ":" << connection.username << ":exception: " << typeid(e).name() << "-" << e.what();

                        connections->erase(key);
                        ws->terminate();
                    }
                } else {
                    ws->send(recv_msg, length, opCode);
                }
            });

            h.onConnection([&connections](uWS::WebSocket<uWS::SERVER> *ws, uWS::HttpRequest request) {
                LOG(WARNING) << NAMEOF(create_uws_thread) << " Got a connection";
                string key = user_connection::AddressToString(ws->getAddress());
                if(connections->contains(key)) {
                    LOG(WARNING) << NAMEOF(create_uws_thread) << " Connection already present, closing this one";
                    ws->terminate();
                    return;
                }
                connections->insert(key, user_connection(ws));
            });

            h.onDisconnection([&connections](uWS::WebSocket<uWS::SERVER> *ws, int code, char *message, size_t length) {

                string key = user_connection::AddressToString(ws->getAddress());
                connections->erase(key);

                LOG(WARNING) << NAMEOF(create_uws_thread) << " Got a disconnect, " << connections->size() << " connections remaining";
            });

            h.onError([](int type) {
                LOG(WARNING) << NAMEOF(create_uws_thread) << " Got error:" << type;
            });

            //auto context = uS::TLS::createContext("cert.pem", "key.pem", "test");
            //h.getDefaultGroup<uWS::SERVER>().addAsync();
            if(!h.listen(3000/*, nullptr, 0, group.get()*/)) {
                LOG(ERROR) << NAMEOF(create_uws_thread) << " h.listen failed";
                return;
            }

            LOG(INFO) << NAMEOF(create_uws_thread) << " starting create_uws_thread";

            while(!quit) {
                h.poll();
            }

            uwsQuit = true;
        } catch (const runtime_error& e) {
            LOG(ERROR) << NAMEOF(create_uws_thread) << " error: " << typeid(e).name() << "-" << e.what();
        }
    });
}

unique_ptr<thread> create_consumer_thread(Config config, shared_ptr<ikafka_consumer<false>> consumer, shared_ptr<cuckoohash_map<string, user_connection>> connections) {
    if(!consumer) {
        LOG(ERROR) << NAMEOF(create_consumer_thread) << " one of the arguments are null";
        throw runtime_error("[main:consumer] one of the arguments are null");
    }

    return make_unique<thread>([=] {
        consumer->start(config.broker_list, config.group_id, std::vector<std::string>{
                "server-" + to_string(config.server_id),
                "chat_messages",
                "broadcast"},
                50);
        message_dispatcher<false> server_gateway_msg_dispatcher;

        server_gateway_msg_dispatcher.register_handler<gateway_quit_handler>(&quit);
        server_gateway_msg_dispatcher.register_handler<gateway_login_response_handler>(config);
        server_gateway_msg_dispatcher.register_handler<gateway_register_response_handler>(config);
        server_gateway_msg_dispatcher.register_handler<gateway_chat_send_handler>(config, connections);
        server_gateway_msg_dispatcher.register_handler<gateway_error_response_handler>(config);
        server_gateway_msg_dispatcher.register_handler<gateway_send_map_handler>(config);
        server_gateway_msg_dispatcher.register_handler<gateway_get_characters_response_handler>(config);

        LOG(INFO) << NAMEOF(create_consumer_thread) << " starting consumer thread";

        while (!quit) {
            try {
                auto msg = consumer->try_get_message(50);
                if (get<1>(msg)) {
                    LOG(INFO) << NAMEOF(create_consumer_thread) << " Got message from kafka";

                    auto id = get<1>(msg)->sender.client_id;
                    auto locked_table = (*connections).lock_table();
                    auto connection = find_if(begin(locked_table), end(locked_table), [id](auto &t) {
                        return get<1>(t).connection_id == id;
                    });

                    if (connection == end(locked_table)) {
                        LOG(DEBUG) << NAMEOF(create_consumer_thread) << " Got message for client_id " << id << " but no connection found";
                        continue;
                    }

                    locked_table.unlock();

                    server_gateway_msg_dispatcher.trigger_handler(msg, make_optional(ref(get<1>(*connection))));

                    LOG(DEBUG) << NAMEOF(create_consumer_thread) << " done handling message";
                }
            } catch (serialization_exception &e) {
                LOG(ERROR) << NAMEOF(create_consumer_thread) << " received serialization exception " << e.what();
            } catch(exception &e) {
                LOG(ERROR) << NAMEOF(create_consumer_thread) << " received exception " << e.what();
            }
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
        LOG(ERROR) << NAMEOF(main) << " .env file is malformed json.";
        return 1;
    }

    init_logger(config);
    init_extras();

    auto common_injector = create_common_di_injector();

    auto producer = common_injector.create<shared_ptr<ikafka_producer<false>>>();
    auto consumer = common_injector.create<shared_ptr<ikafka_consumer<false>>>();

    auto connections = make_shared<cuckoohash_map<string, user_connection>>();
    uWS::Hub h;

    try {
        LOG(INFO) << NAMEOF(main) << " starting main thread";
        producer->start(config.broker_list, 50);
        auto uws_thread = create_uws_thread(config, h, producer, connections);
        auto consumer_thread = create_consumer_thread(config, consumer, connections);

        while (!quit) {
            try {
                producer->poll(50);
            } catch (serialization_exception &e) {
                LOG(ERROR) << NAMEOF(main) << " received exception " << e.what();
            }
        }

        LOG(INFO) << NAMEOF(main) << " closing";


        producer->close();
        consumer->close();
        LOG(INFO) << NAMEOF(main) << " closed kafka connections";

        auto now = chrono::system_clock::now().time_since_epoch().count();
        auto wait_until = (chrono::system_clock::now() += 2000ms).time_since_epoch().count();

        while (!uwsQuit && now < wait_until) {
            LOG(INFO) << NAMEOF(main) << " waiting for uws";
            this_thread::sleep_for(100ms);
            now = chrono::system_clock::now().time_since_epoch().count();
        }

        LOG(INFO) << NAMEOF(main) << " joining consumer thread";
        consumer_thread->join();

        if(!uwsQuit) {
            LOG(INFO) << NAMEOF(main) << " detaching uws thread";
            uws_thread->detach();
        } else {
            LOG(INFO) << NAMEOF(main) << " joining uws thread";
            uws_thread->join();
        }
    } catch (const runtime_error& e) {
        LOG(ERROR) << NAMEOF(main) << " error: " << typeid(e).name() << "-" << e.what();
    }

    LOG(INFO) << NAMEOF(main) << " goodbye";

    return 0;
}
