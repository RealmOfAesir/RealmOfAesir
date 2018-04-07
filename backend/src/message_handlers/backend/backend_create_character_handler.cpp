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

#include "backend_create_character_handler.h"
#include <easylogging++.h>
#include <messages/user_access_control/register_response_message.h>
#include <messages/error_response_message.h>
#include <macros.h>

using namespace std;
using namespace roa;

static inline binary_create_character_message create_message(uint64_t client_id, uint32_t server_id, uint32_t gateway_id, uint64_t user_id, string player_name) {
    return binary_create_character_message {
            {false, client_id, server_id, gateway_id},
            user_id,
            player_name
    };
}

static inline binary_error_response_message create_error_message(uint64_t client_id, uint32_t server_id, int error_no, string error_str) {
    return binary_error_response_message {
            {false, client_id, server_id, 0 /* ANY */},
            error_no,
            error_str
    };
}

backend_create_character_handler::backend_create_character_handler(Config config, iusers_repository &users_repository, isettings_repository& settings_respository, std::shared_ptr<ikafka_producer<false>> producer)
        : _config(config), _users_repository(users_repository), _settings_repository(settings_respository), _producer(producer) {

}

void backend_create_character_handler::handle_message(unique_ptr<binary_message const> const &msg) {
    string gateway_queue = "server-" + to_string(msg->sender.server_origin_id);
    try {
        if (auto casted_msg = dynamic_cast<create_character_message<false> const *>(msg.get())) {
            // TODO use a repeatable read transaction, rather than the default read committed.
            auto transaction = _users_repository.create_transaction();
            auto usr = _users_repository.get_user(casted_msg->user_id, get<1>(transaction));
            auto max_characters = _settings_repository.get_setting("max_characters_per_user", get<1>(transaction));
            auto world_id = _settings_repository.get_setting("starting_world_id", get<1>(transaction));


            if(!usr) {
                LOG(ERROR) << NAMEOF(backend_create_character_handler::handle_message) << " Couldn't find user_id " << casted_msg->user_id << " - queue: " << gateway_queue;
                this->_producer->enqueue_message(gateway_queue, create_error_message(msg->sender.client_id, _config.server_id, -1, "Something went wrong."));
                return;
            }

            if(!max_characters) {
                LOG(ERROR) << NAMEOF(backend_create_character_handler::handle_message) << " Couldn't find max_characters - queue: " << gateway_queue;
                this->_producer->enqueue_message(gateway_queue, create_error_message(msg->sender.client_id, _config.server_id, -1, "Something went wrong."));
                return;
            }

            if(!world_id) {
                LOG(ERROR) << NAMEOF(backend_create_character_handler::handle_message) << " Couldn't find world_id - queue: " << gateway_queue;
                this->_producer->enqueue_message(gateway_queue, create_error_message(msg->sender.client_id, _config.server_id, -1, "Something went wrong."));
                return;
            }

            if(stoi(max_characters->value) <= usr->no_of_players) {
                LOG(ERROR) << NAMEOF(backend_create_character_handler::handle_message) << " user " << casted_msg->user_id << " has too many players - queue: " << gateway_queue;
                this->_producer->enqueue_message(gateway_queue, create_error_message(msg->sender.client_id, _config.server_id, -1, "You already have too many players. Max: " + max_characters->value));
                return;
            }

            string world_queue = "server-" + world_id->value;
            usr->no_of_players++;
            _users_repository.update_user(usr.value(), get<1>(transaction));

            get<1>(transaction)->commit();

            LOG(INFO) << NAMEOF(backend_create_character_handler::handle_message) << " sending create character message to queue: " << world_queue;

            this->_producer->enqueue_message(world_queue, create_message(msg->sender.client_id, _config.server_id, casted_msg->sender.server_origin_id, casted_msg->user_id, casted_msg->player_name));
        } else {
            LOG(ERROR) << NAMEOF(backend_create_character_handler::handle_message) << " Couldn't cast message to create_character_message - queue: " << gateway_queue;
            this->_producer->enqueue_message(gateway_queue, create_error_message(msg->sender.client_id, _config.server_id, -1, "Something went wrong."));
        }
    } catch (std::runtime_error const &e) {
        LOG(ERROR) << NAMEOF(backend_create_character_handler::handle_message) << " error: " << typeid(e).name() << "-" << e.what() << " - queue: " << gateway_queue;
        this->_producer->enqueue_message(gateway_queue, create_error_message(msg->sender.client_id, _config.server_id, -1, "Something went wrong."));
    }
}

uint32_t constexpr backend_create_character_handler::message_id;