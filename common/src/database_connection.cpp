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

#include "database_connection.h"
#include "database_pool.h"
#include "database_transaction.h"

using namespace std;
using namespace roa;
using namespace pqxx;

database_connection::database_connection(database_pool *database_pool, tuple<uint32_t, shared_ptr<pqxx::connection>> connection) noexcept
        : _database_pool(database_pool), _connection(connection) {

}

database_connection::~database_connection() {
    _database_pool->release_connection(get<0>(_connection));
}

std::unique_ptr<idatabase_transaction> database_connection::create_transaction() {
    return make_unique<database_transaction>(get<1>(_connection));
}