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

#include <pqxx/pqxx>
#include <easylogging++.h>

#include <sql_exceptions.h>
#include "database_pool.h"
#include "repository.h"

using namespace std;
using namespace roa;
using namespace pqxx;

repository::repository(shared_ptr<idatabase_pool> database_pool)
        : _database_pool(database_pool) {

}

repository::~repository() {

}

tuple<unique_ptr<idatabase_connection>, unique_ptr<idatabase_transaction>> repository::create_transaction()  {
    auto connection = _database_pool->get_connection();
    auto transaction = connection->create_transaction();

    return make_tuple(move(connection), move(transaction));
}
