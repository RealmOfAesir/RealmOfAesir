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

#include <easylogging++.h>
#include "database_transaction.h"

using namespace std;
using namespace roa;
using namespace pqxx;

database_transaction::database_transaction(std::shared_ptr<connection> connection) noexcept
        : _connection(connection), _transaction(*_connection) {

}

database_transaction::~database_transaction() {
}

pqxx::result database_transaction::execute(string query) {
    LOG(DEBUG) << "executing query \"" << query << "\"";
    return _transaction.exec(query);
}

string database_transaction::escape(string element) {
    return _transaction.esc(element);
}

void database_transaction::commit() {
    _transaction.commit();
}
