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

#include "client_create_character_handler.h"
#include <macros.h>
#include <easylogging++.h>
#include <messages/error_response_message.h>

using namespace std;
using namespace roa;

client_create_character_handler::client_create_character_handler(Config config,
                                           shared_ptr<ikafka_producer<false>> producer)
        : _config(config), _producer(producer) {

}

void client_create_character_handler::handle_message(unique_ptr<binary_message const> const &msg, STD_OPTIONAL<std::reference_wrapper<user_connection>> connection) {
    if(unlikely(!connection)) {
        LOG(ERROR) << NAMEOF(client_create_character_handler::handle_message) << " received empty connection";
        return;
    }

    if(connection->get().state != user_connection_state::LOGGED_IN) {
        LOG(DEBUG) << NAMEOF(client_create_character_handler::handle_message) << " not logged in.";
        json_error_response_message response{{false, 0, 0, 0}, -1, "Need to login."};
        auto response_str = response.serialize();
        connection->get().ws->send(response_str.c_str(), response_str.length(), uWS::OpCode::TEXT);
        return;
    }

    if (auto message = dynamic_cast<binary_create_character_message const *>(msg.get())) {
        LOG(DEBUG) << NAMEOF(client_create_character_handler::handle_message) << " Got binary_create_character_message from wss";
        this->_producer->enqueue_message("backend_messages", binary_create_character_message {
                {
                        false,
                        connection->get().connection_id,
                        _config.server_id,
                        0 // ANY
                },
                connection->get().user_id,
                message->player_name
        });
    } else {
        LOG(ERROR) << NAMEOF(client_create_character_handler::handle_message) << " Couldn't cast message to binary_create_character_message";
        json_error_response_message response{{false, 0, 0, 0}, -1, "Something went wrong."};
        auto response_str = response.serialize();
        connection->get().ws->send(response_str.c_str(), response_str.length(), uWS::OpCode::TEXT);
    }
}

uint32_t constexpr client_create_character_handler::message_id;