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

#include "backend_login_handler.h"
#include <messages/user_access_control/login_response_message.h>
#include <messages/error_response_message.h>
#include <easylogging++.h>
#include <sodium.h>
#include <macros.h>
#include <on_leaving_scope.h>

using namespace std;
using namespace roa;

static inline binary_login_response_message create_message(uint64_t client_id, uint32_t server_id, int8_t admin_status, uint64_t user_id) {
    return binary_login_response_message {
            {false, client_id, server_id, 0 /* ANY */},
            admin_status,
            user_id
    };
}

static inline binary_error_response_message create_error_message(uint64_t client_id, uint32_t server_id, int error_no, string error_str) {
    return binary_error_response_message {
            {false, client_id, server_id, 0 /* ANY */},
            error_no,
            error_str
    };
}

backend_login_handler::backend_login_handler(Config config, iusers_repository &users_repository, ibanned_users_repository& banned_users_repository, std::shared_ptr<ikafka_producer<false>> producer)
        : _config(config), _users_repository(users_repository), _banned_users_repository(banned_users_repository), _producer(producer) {

}

void backend_login_handler::handle_message(unique_ptr<binary_message const> const &msg) {
    string queue_name = "server-" + to_string(msg->sender.server_origin_id);
    try {
        if (auto casted_msg = dynamic_cast<binary_login_message const *>(msg.get())) {
            sodium_mlock(reinterpret_cast<unsigned char *>(&const_cast<binary_login_message*>(casted_msg)->password[0]), casted_msg->password.size());
            auto scope_guard = on_leaving_scope([&] {
                sodium_munlock(reinterpret_cast<unsigned char *>(&const_cast<binary_login_message*>(casted_msg)->password[0]), casted_msg->password.size());
            });
            auto transaction = _users_repository.create_transaction();
            auto banned_user = _banned_users_repository.is_username_or_ip_banned(casted_msg->username, casted_msg->ip, get<1>(transaction));

            if(banned_user) {
                LOG(INFO) << NAMEOF(backend_login_handler::handle_message) << " logging in user, but is banned - queue: " << queue_name;
                this->_producer->enqueue_message(queue_name, create_error_message(msg->sender.client_id, _config.server_id, -2, "You are banned"));
                return;
            }

            STD_OPTIONAL<user> usr = _users_repository.get_user(casted_msg->username, get<1>(transaction));
            if(!usr) {
                LOG(DEBUG) << NAMEOF(backend_login_handler::handle_message) << " Login " << casted_msg->username << " doesn't exist - queue: " << queue_name;
                this->_producer->enqueue_message(queue_name, create_error_message(msg->sender.client_id, _config.server_id, -1, "User doesn't exist"));
            } else {
                if(crypto_pwhash_str_verify(usr->password.c_str(), casted_msg->password.c_str(), casted_msg->password.size()) != 0) {
                    LOG(ERROR) << NAMEOF(backend_login_handler::handle_message) << " logging in user, but wrong password - queue: " << queue_name;
                    this->_producer->enqueue_message(queue_name, create_error_message(msg->sender.client_id, _config.server_id, -1, "Wrong combination of user + password"));
                    return;
                }

                LOG(DEBUG) << NAMEOF(backend_login_handler::handle_message) << " Login " << casted_msg->username << " - " << usr->id << " - queue: " << queue_name;
                this->_producer->enqueue_message(queue_name, create_message(msg->sender.client_id, _config.server_id, usr->admin, usr->id));
            }
        } else {
            LOG(ERROR) << NAMEOF(backend_login_handler::handle_message) << " Couldn't cast message to login_message - queue: " << queue_name;
            this->_producer->enqueue_message(queue_name, create_error_message(msg->sender.client_id, _config.server_id, -1, "Something went wrong"));
        }
    } catch (std::runtime_error const &e) {
        LOG(ERROR) << NAMEOF(backend_login_handler::handle_message) << " error: " << typeid(e).name() << "-" << e.what() << " - queue: " << queue_name;
        this->_producer->enqueue_message(queue_name, create_error_message(msg->sender.client_id, _config.server_id, -1, "Something went wrong"));
    }
}

uint32_t constexpr backend_login_handler::message_id;