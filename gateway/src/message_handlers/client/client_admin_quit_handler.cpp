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

#include "client_admin_quit_handler.h"
#include <macros.h>
#include <easylogging++.h>

using namespace std;
using namespace roa;

client_admin_quit_handler::client_admin_quit_handler(Config config, std::shared_ptr<ikafka_producer<false>> producer)
        : _config(config), _producer(producer) {

}

void client_admin_quit_handler::handle_message(const unique_ptr<const binary_message> &msg, STD_OPTIONAL<std::reference_wrapper<user_connection>> connection) {
    if(unlikely(!connection)) {
        LOG(ERROR) << NAMEOF(client_admin_quit_handler::handle_message) << " received empty connection";
        return;
    }

    if(connection->get().admin_status != 1) {
        LOG(WARNING) << NAMEOF(client_admin_quit_handler::handle_message) << " received unauthorized quit message";
        return;
    }

    if (auto quit_msg = dynamic_cast<binary_quit_message const *>(msg.get())) {
        LOG(WARNING) << NAMEOF(client_admin_quit_handler::handle_message) << " Got authorized binary_quit_message from wss, sending quit message to kafka";
        this->_producer->enqueue_message("broadcast", quit_msg);
    }
}

uint32_t constexpr client_admin_quit_handler::message_id;
