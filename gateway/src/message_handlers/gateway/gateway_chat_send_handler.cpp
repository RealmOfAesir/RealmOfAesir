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

#include "gateway_chat_send_handler.h"
#include <macros.h>
#include <easylogging++.h>
#include <messages/chat/chat_receive_message.h>

using namespace std;
using namespace roa;

gateway_chat_send_handler::gateway_chat_send_handler(Config config, shared_ptr<cuckoohash_map<string, user_connection>> connections)
        : _config(config), _connections(connections) {
    if(!_connections) {
        LOG(ERROR) << NAMEOF(gateway_chat_send_handler::handle_message) << " one of the arguments are null";
        throw runtime_error("one of the arguments are null");
    }
}

void gateway_chat_send_handler::handle_message(std::unique_ptr<binary_message const> const &msg,
                                                  STD_OPTIONAL<reference_wrapper<user_connection>> connection) {
    if (auto response_msg = dynamic_cast<binary_chat_send_message const *>(msg.get())) {
        LOG(DEBUG) << NAMEOF(gateway_chat_send_handler::handle_message) << " Got response message from backend";

        json_chat_receive_message chat_msg{{false, 0, 0, 0}, response_msg->from_username, response_msg->target, response_msg->message};
        auto response_str = chat_msg.serialize();

        auto locked_table = (*_connections).lock_table();
        if(response_msg->target == "all") {
            for(auto const& conn : locked_table) {
                if(get<1>(conn).state == user_connection_state::LOGGED_IN) {
                    get<1>(conn).ws->send(response_str.c_str(), response_str.length(), uWS::OpCode::TEXT);
                }
            }
        } else {
            auto user_connection = find_if(cbegin(locked_table), cend(locked_table), [&](auto& t) {
                return get<1>(t).username == response_msg->target && get<1>(t).state == user_connection_state ::LOGGED_IN;
            });

            if(user_connection != cend(locked_table)) {
                user_connection->second.ws->send(response_str.c_str(), response_str.length(), uWS::OpCode::TEXT);
            }
        }
        locked_table.unlock();
    } else {
        LOG(ERROR) << NAMEOF(gateway_chat_send_handler::handle_message) << " Couldn't cast message to chat_send_message";
    }
}

uint32_t constexpr gateway_chat_send_handler::message_id;
