/*
    RealmOfAesirWorld
    Copyright (C) 2017  Michael de Lang

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

#include "scripts_repository.h"

#include <easylogging++.h>

#include <macros.h>

using namespace std;
using namespace experimental;
using namespace roa;
using namespace pqxx;

scripts_repository::scripts_repository(shared_ptr<idatabase_pool> database_pool) : repository(database_pool) {

}

scripts_repository::scripts_repository(scripts_repository &repo) : repository(repo._database_pool) {

}

scripts_repository::scripts_repository(scripts_repository &&repo) : repository(repo._database_pool) {

}

scripts_repository::~scripts_repository() {

}

auto scripts_repository::create_transaction() -> decltype(repository::create_transaction()) {
    return repository::create_transaction();
}

bool scripts_repository::insert_or_update_script(script &scr,
                                                   std::unique_ptr<idatabase_transaction> const &transaction) {
    auto result = transaction->execute(
            "INSERT INTO scripts (script_name, script) VALUES ('" + transaction->escape(scr.name) +
            "', '" + transaction->escape(scr.text) + "') ON CONFLICT (script_name) DO UPDATE SET"
                    " script = '" + transaction->escape(scr.text) + "' RETURNING xmax");

    if(result[0][0].as<string>() == "0") {
        LOG(DEBUG) << NAMEOF(scripts_repository::insert_or_update_script) << " inserted script";
        return true;
    }
    LOG(DEBUG) << NAMEOF(scripts_repository::insert_or_update_script) << " updated script";
    return false;

}

STD_OPTIONAL<script>
scripts_repository::get_script(std::string const &name, std::unique_ptr<idatabase_transaction> const &transaction) {
    auto result = transaction->execute("SELECT s.script_name, s.script FROM scripts s WHERE script_name = '" + transaction->escape(name) + "'");

    LOG(DEBUG) << NAMEOF(scripts_repository::get_script) << " contains " << result.size() << " entries";

    if(result.size() == 0) {
        return {};
    }

    return make_optional<script>({result[0][0].as<string>(), result[0][1].as<string>()});
}
