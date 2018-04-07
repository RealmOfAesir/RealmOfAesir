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

#include "user_connection.h"
#include <easylogging++.h>
#include <macros.h>

using namespace std;
using namespace roa;

atomic<uint64_t> user_connection::idCounter;

user_connection::user_connection()
        : state(UNKNOWN), admin_status(0), ws(nullptr), connection_id(0), username(), user_id(), player_id(), player_characters() {
    LOG(DEBUG) << NAMEOF(user_connection::user_connection) << " new connection " << connection_id;
}

user_connection::user_connection(uWS::WebSocket<uWS::SERVER> *ws)
        : state(UNKNOWN), admin_status(0), ws(ws), connection_id(idCounter.fetch_add(1, std::memory_order_relaxed)), username(), user_id(), player_id(), player_characters() {
    LOG(DEBUG) << NAMEOF(user_connection::user_connection) << " new connection " << connection_id;
}

user_connection::user_connection(user_connection const &conn)
        : state(conn.state), admin_status(conn.admin_status), ws(conn.ws), connection_id(conn.connection_id), username(conn.username), user_id(conn.user_id), player_id(conn.player_id), player_characters(conn.player_characters) {
}

std::string user_connection::AddressToString(uS::Socket::Address &&a) {
    return std::string(a.address + std::to_string(a.port));
}
