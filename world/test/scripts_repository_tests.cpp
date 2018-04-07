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

#include <easylogging++.h>
#include <catch.hpp>
#include <roa_di.h>
#include <database_transaction.h>
#include "test_helpers/startup_helper.h"
#include "../src/repositories/scripts_repository.h"

using namespace std;
using namespace roa;

TEST_CASE("scripts repository tests") {
    auto backend_injector = boost::di::make_injector(
            boost::di::bind<idatabase_transaction>.to<database_transaction>(),
            boost::di::bind<idatabase_connection>.to<database_connection>(),
            boost::di::bind<idatabase_pool>.to(db_pool),
            boost::di::bind<irepository>.to<repository>(),
            boost::di::bind<iscripts_repository>.to<scripts_repository>());

    scripts_repository scripts_repo = backend_injector.create<scripts_repository>();
    auto transaction = scripts_repo.create_transaction();

    SECTION("script inserted correctly") {
        script scr{"test_name", "test_value"};
        bool inserted = scripts_repo.insert_or_update_script(scr, get<1>(transaction));
        REQUIRE(inserted == true);

        auto scr2 = scripts_repo.get_script("test_name", get<1>(transaction));
        REQUIRE(scr2);
        REQUIRE(scr2->name == scr.name);
        REQUIRE(scr2->text == scr.text);
    }

    SECTION("script updated correctly") {
        script scr{"test_name", "test_value"};
        scripts_repo.insert_or_update_script(scr, get<1>(transaction));
        scr.text = "test_value2";
        bool inserted = scripts_repo.insert_or_update_script(scr, get<1>(transaction));
        REQUIRE(inserted == false);

        auto scr2 = scripts_repo.get_script("test_name", get<1>(transaction));
        REQUIRE(scr2);
        REQUIRE(scr2->name == scr.name);
        REQUIRE(scr2->text == scr.text);
    }
}