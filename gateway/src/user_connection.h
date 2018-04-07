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

#pragma once

#include <uWS.h>
#include <string>
#include <atomic>

namespace roa {
    enum user_connection_state {
        UNKNOWN,
        REGISTERING_OR_LOGGING_IN,
        LOGGED_IN
    };

    struct player_character {
        uint64_t id;
        uint32_t server_id;
        std::string player_name;
        std::string map_name;
        std::string world_name;
    };

    struct user_connection {
        user_connection_state state;
        int8_t admin_status;
        uWS::WebSocket<uWS::SERVER> *ws;
        uint64_t connection_id;
        std::string username;
        uint64_t user_id;
        uint64_t player_id;
        std::vector<player_character> player_characters;
        static std::atomic<uint64_t> idCounter;

        explicit user_connection();
        explicit user_connection(uWS::WebSocket<uWS::SERVER> *ws);
        user_connection(user_connection const &conn);

        static std::string AddressToString(uS::Socket::Address &&a);
    };
}