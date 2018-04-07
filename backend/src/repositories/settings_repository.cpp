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

#include "settings_repository.h"

#include <pqxx/pqxx>
#include <easylogging++.h>

#include <sql_exceptions.h>
#include <macros.h>

using namespace std;
using namespace experimental;
using namespace roa;
using namespace pqxx;

settings_repository::settings_repository(std::shared_ptr<idatabase_pool> database_pool) : repository(database_pool) {

}

settings_repository::settings_repository(settings_repository &repo) : repository(repo._database_pool) {

}

settings_repository::settings_repository(settings_repository &&repo) : repository(repo._database_pool) {

}

settings_repository::~settings_repository() {

}

auto settings_repository::create_transaction() -> decltype(repository::create_transaction()) {
    return repository::create_transaction();
}

bool settings_repository::insert_or_update_setting(setting &sett,
                                                   std::unique_ptr<idatabase_transaction> const &transaction) {
    auto result = transaction->execute(
            "INSERT INTO settings (setting_name, value) VALUES ('" + transaction->escape(sett.name) +
            "', '" + transaction->escape(sett.value) + "') ON CONFLICT (setting_name) DO UPDATE SET"
                    " value = '" + transaction->escape(sett.value) + "' RETURNING xmax");

    return result[0]["xmax"].as<string>() == "0";
}

STD_OPTIONAL<setting>
settings_repository::get_setting(std::string const &name, std::unique_ptr<idatabase_transaction> const &transaction) {
    auto result = transaction->execute("SELECT * FROM settings WHERE setting_name = '" + transaction->escape(name) + "'");

    LOG(DEBUG) << NAMEOF(settings_repository::get_setting) << " contains " << result.size() << " entries";

    if(result.size() == 0) {
        return {};
    }

    return make_optional<setting>({result[0]["setting_name"].as<string>(), result[0]["value"].as<string>()});
}
