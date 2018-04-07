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

#include <catch.hpp>
#include <roa_di.h>
#include <easylogging++.h>
#include <database_transaction.h>
#include "../src/repositories/repository_exceptions.h"
#include "test_helpers/startup_helper.h"
#include "../src/repositories/settings_repository.h"

using namespace std;
using namespace roa;

TEST_CASE("settings repository tests") {
    auto backend_injector = boost::di::make_injector(
            boost::di::bind<idatabase_transaction>.to<database_transaction>(),
            boost::di::bind<idatabase_connection>.to<database_connection>(),
            boost::di::bind<idatabase_pool>.to(db_pool),
            boost::di::bind<irepository>.to<repository>(),
            boost::di::bind<isettings_repository>.to<settings_repository>());

    settings_repository settings_repo = backend_injector.create<settings_repository>();

    SECTION( "setting inserted correctly" ) {
        auto transaction = settings_repo.create_transaction();
        setting sett{"test_name", "test_value"};
        bool inserted = settings_repo.insert_or_update_setting(sett, get<1>(transaction));
        REQUIRE(inserted == true);

        auto sett2 = settings_repo.get_setting("test_name", get<1>(transaction));
        REQUIRE(sett2);
        REQUIRE(sett2->name == sett.name);
        REQUIRE(sett2->value == sett.value);
    }

    SECTION( "setting updated correctly" ) {
        auto transaction = settings_repo.create_transaction();
        setting sett{"test_name", "test_value"};
        settings_repo.insert_or_update_setting(sett, get<1>(transaction));
        sett.value = "test_value2";
        bool inserted = settings_repo.insert_or_update_setting(sett, get<1>(transaction));
        REQUIRE(inserted == false);

        auto sett2 = settings_repo.get_setting("test_name", get<1>(transaction));
        REQUIRE(sett2);
        REQUIRE(sett2->name == sett.name);
        REQUIRE(sett2->value == sett.value);
    }
}