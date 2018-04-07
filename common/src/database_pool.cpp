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

#include "database_pool.h"
#include "database_connection.h"
#include <macros.h>
#include <mutex>
#include <condition_variable>
#include <algorithm>
#include <easylogging++.h>
#include <thread>

using namespace std;
using namespace roa;
using namespace pqxx;

database_pool::database_pool() noexcept : _connection_string(), _min_connections(), _connections(), _connections_mutex() {

}

database_pool::~database_pool() {
    lock_guard<mutex> cl(_connections_mutex);
    _connections.clear();
    _connection_string.clear();
}

void database_pool::create_connections(string connection_string, uint32_t min_connections) {
    lock_guard<mutex> cl(_connections_mutex);
    _connection_string = connection_string;
    _min_connections = min_connections;
    for(uint32_t i = 0; i < min_connections; i++) {
        auto conn = make_shared<connection>(connection_string);
        conn->activate();
        _connections.push_back(make_tuple(true, i, conn));
    }

}

std::unique_ptr<idatabase_connection> database_pool::get_connection() {
    if(unlikely(_connection_string.empty())) {
        throw runtime_error("pool not initialized yet");
    }

    uint32_t id = _min_connections+1;
    shared_ptr<connection> conn;

    while(id > _min_connections) {
        {
            lock_guard<mutex> cl(_connections_mutex);
            for (auto &c : _connections) {
                // true == available for use, false = current in use
                if (get<0>(c) == true) {

                    get<0>(c) = false;
                    id = get<1>(c);
                    conn = get<2>(c);

                    LOG(DEBUG) << "got connection " << id;
                }
            };
        }

        if(unlikely(id > _min_connections)) {
            this_thread::sleep_for(1ms);
        }
    }

    if(unlikely(!conn)) {
        throw runtime_error("Unexpected error, conn should never be null.");
    }

    return make_unique<database_connection>(this, make_tuple(id, conn));
}

void database_pool::release_connection(uint32_t id) {
    lock_guard<mutex> cl(_connections_mutex);

    LOG(DEBUG) << "releasing connection " << id;

    auto result = find_if(begin(_connections), end(_connections), [&](tuple<bool, uint32_t, shared_ptr<connection>> &t) {
       return get<1>(t) == id;
    });

    if(unlikely(result == end(_connections))) {
        runtime_error("Couldn't find connection with id " + to_string(id));
    }

    if(unlikely(get<0>(*result))) {
        throw runtime_error("Trying to release connection that's already released");
    }

    get<0>(*result) = true;
}
