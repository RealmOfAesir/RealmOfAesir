/*
    Realm of Aesir
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

#include "message.h"
#include "user_access_control/login_message.h"
#include "exceptions.h"
#include "user_access_control/login_response_message.h"
#include "misc.h"
#include "error_response_message.h"

#include <cereal/archives/binary.hpp>
#include <cereal/archives/json.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/vector.hpp>
#include <sstream>
#include <easylogging++.h>
#include <admin_messages/admin_quit_message.h>
#include <messages/user_access_control/register_message.h>
#include <messages/user_access_control/register_response_message.h>
#include <messages/chat/chat_receive_message.h>
#include <messages/chat/chat_send_message.h>
#include <messages/user_access_control/logout_message.h>
#include <messages/user_access_control/create_character_message.h>
#include <messages/user_access_control/play_character_message.h>
#include <messages/game/send_map_message.h>
#include <messages/user_access_control/get_characters_message.h>
#include <messages/user_access_control/get_characters_response_message.h>

using namespace std;
using namespace roa;

template <bool UseJson>
message<UseJson>::message(message_sender sender) : sender(sender) {

}

template <bool UseJson>
template <bool UseJsonAsReturnType>
tuple<uint32_t, unique_ptr<message<UseJsonAsReturnType> const>> message<UseJson>::deserialize(std::string buffer) {
    if(buffer.empty() || buffer.length() < 4) {
        LOG(WARNING) << "[message] deserialize encountered empty buffer";
        throw serialization_exception("empty buffer " + to_string(buffer.length()));
    }

    uint32_t message_id;
    message_sender sender;
    stringstream ss;
    ss << buffer;
    ss.flush();
    typename conditional<UseJson, cereal::JSONInputArchive, cereal::BinaryInputArchive>::type archive(ss);
    archive(cereal::make_nvp("id", message_id), cereal::make_nvp("sender", sender));

    LOG(INFO) << "[message] type " << message_id << " with length " << buffer.size();

    // scopes in cases are required for the cereal archive to go out of scope properly
    switch(message_id) {

        case error_response_message<UseJson>::id:
        {
            int error_number;
            std::string error_str;
            archive(cereal::make_nvp("error_number", error_number), cereal::make_nvp("error_str", error_str));
            return make_tuple(message_id, make_unique<error_response_message<UseJsonAsReturnType>>(sender, error_number, error_str));
        }

        // ---- uac messages ----

        case login_message<UseJson>::id:
        {
            std::string username;
            std::string password;
            std::string ip;
            archive(cereal::make_nvp("username", username), cereal::make_nvp("password", password), cereal::make_nvp("ip", ip));
            return make_tuple(message_id, make_unique<login_message<UseJsonAsReturnType>>(sender, username, password, ip));
        }
        case login_response_message<UseJson>::id:
        {
            int16_t admin_status;
            uint64_t user_id;
            archive(cereal::make_nvp("admin_status", admin_status), cereal::make_nvp("user_id", user_id));
            return make_tuple(message_id, make_unique<login_response_message<UseJsonAsReturnType>>(sender, admin_status, user_id));
        }
        case register_message<UseJson>::id:
        {
            std::string username;
            std::string password;
            std::string email;
            std::string ip;
            archive(cereal::make_nvp("username", username), cereal::make_nvp("password", password), cereal::make_nvp("email", email), cereal::make_nvp("ip", ip));
            return make_tuple(message_id, make_unique<register_message<UseJsonAsReturnType>>(sender, username, password, email, ip));
        }
        case register_response_message<UseJson>::id:
        {
            int16_t admin_status;
            uint64_t user_id;
            archive(cereal::make_nvp("admin_status", admin_status), cereal::make_nvp("user_id", user_id));
            return make_tuple(message_id, make_unique<register_response_message<UseJsonAsReturnType>>(sender, admin_status, user_id));
        }
        case logout_message<UseJson>::id:
        {
            return make_tuple(message_id, make_unique<logout_message<UseJsonAsReturnType>>(sender));
        }
        case create_character_message<UseJson>::id:
        {
            uint64_t user_id;
            string player_name;
            archive(cereal::make_nvp("user_id", user_id), cereal::make_nvp("player_name", player_name));
            return make_tuple(message_id, make_unique<create_character_message<UseJsonAsReturnType>>(sender, user_id, player_name));
        }
        case play_character_message<UseJson>::id:
        {
            uint64_t user_id;
            string player_name;
            archive(cereal::make_nvp("user_id", user_id), cereal::make_nvp("player_name", player_name));
            return make_tuple(message_id, make_unique<play_character_message<UseJsonAsReturnType>>(sender, user_id, player_name));
        }
        case get_characters_message<UseJson>::id:
        {
            uint64_t user_id;
            archive(cereal::make_nvp("user_id", user_id));
            return make_tuple(message_id, make_unique<get_characters_message<UseJsonAsReturnType>>(sender, user_id));
        }
        case get_characters_response_message<UseJson>::id:
        {
            vector<player_response> players;
            string world_name;
            archive(cereal::make_nvp("players", players), cereal::make_nvp("world_name", world_name));
            return make_tuple(message_id, make_unique<get_characters_response_message<UseJsonAsReturnType>>(sender, players, world_name));
        }

        // ---- chat messages ----

        case chat_send_message<UseJson>::id:
        {
            string from_username;
            string target;
            string message;
            archive(cereal::make_nvp("from_username", from_username), cereal::make_nvp("target", target), cereal::make_nvp("message", message));
            return make_tuple(message_id, make_unique<chat_send_message<UseJsonAsReturnType>>(sender, from_username, target, message));
        }
        case chat_receive_message<UseJson>::id:
        {
            string from_username;
            string target;
            string msg;
            archive(cereal::make_nvp("from_username", from_username), cereal::make_nvp("target", target), cereal::make_nvp("message", msg));
            return make_tuple(message_id, make_unique<chat_receive_message<UseJsonAsReturnType>>(sender, from_username, target, msg));
        }

        // ---- game messages ----

        case send_map_message<UseJson>::id:
        {
            std::string map_data;
            archive( cereal::make_nvp("map_data", map_data));
            return make_tuple(message_id, make_unique<send_map_message<UseJsonAsReturnType>>(sender, map_data));
        }

        // ---- admin messages ----

        case admin_quit_message<UseJson>::id:
        {
            return make_tuple(message_id, make_unique<admin_quit_message<UseJsonAsReturnType>>(sender));
        }
        default:
        {
            LOG(WARNING) << "[message] deserialize encountered unknown message type: " << message_id;

            throw serialization_exception("unknown message type " + to_string(message_id));
        }
    }
}

template class message<false>;
template class message<true>;
template tuple<uint32_t, unique_ptr<message<false> const>> message<false>::deserialize<false>(std::string buffer);
template tuple<uint32_t, unique_ptr<message<false> const>> message<true>::deserialize<false>(std::string buffer);
template tuple<uint32_t, unique_ptr<message<true> const>> message<false>::deserialize<true>(std::string buffer);
template tuple<uint32_t, unique_ptr<message<true> const>> message<true>::deserialize<true>(std::string buffer);