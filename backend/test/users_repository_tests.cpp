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
#include "../src/repositories/users_repository.h"
#include "../src/repositories/repository_exceptions.h"
#include "test_helpers/startup_helper.h"

using namespace std;
using namespace roa;

TEST_CASE("user repository tests") {
    auto backend_injector = boost::di::make_injector(
            boost::di::bind<idatabase_transaction>.to<database_transaction>(),
            boost::di::bind<idatabase_connection>.to<database_connection>(),
            boost::di::bind<idatabase_pool>.to(db_pool),
            boost::di::bind<irepository>.to<repository>(),
            boost::di::bind<iusers_repository>.to<users_repository>());

    users_repository user_repo = backend_injector.create<users_repository>();
    SECTION( "user inserted correctly" ) {
        auto transaction = user_repo.create_transaction();
        user usr{0, "user"s, "pass"s, "email"s, 2, 3, 4};
        user_repo.insert_user_if_not_exists(usr, get<1>(transaction));
        REQUIRE(usr.id != 0);

        auto usr2 = user_repo.get_user(usr.id, get<1>(transaction));
        REQUIRE(usr2);
        REQUIRE(usr2->id == usr.id);
        REQUIRE(usr2->username == usr.username);
        REQUIRE(usr2->password == usr.password);
        REQUIRE(usr2->email == usr.email);
        REQUIRE(usr2->login_attempts == usr.login_attempts);
        REQUIRE(usr2->admin == usr.admin);
        REQUIRE(usr2->no_of_players == usr.no_of_players);

        uint64_t old_id = usr.id;
        user_repo.insert_user_if_not_exists(usr, get<1>(transaction));
        REQUIRE(usr.id == old_id);
    }

    SECTION( "update user" ) {
        auto transaction = user_repo.create_transaction();
        user usr{0, "user"s, "pass"s, "email"s, 2, 3, 4};
        user_repo.insert_user_if_not_exists(usr, get<1>(transaction));
        REQUIRE(usr.id != 0);

        usr.username = "user2";
        usr.password = "pass2";
        usr.email = "email2";
        usr.login_attempts = 5;
        usr.admin = 6;
        usr.no_of_players = 7;
        user_repo.update_user(usr, get<1>(transaction));

        auto usr2 = user_repo.get_user(usr.username, get<1>(transaction));
        REQUIRE(usr2);
        REQUIRE(usr2->id == usr.id);
        REQUIRE(usr2->username == usr.username);
        REQUIRE(usr2->password == usr.password);
        REQUIRE(usr2->email == usr.email);
        REQUIRE(usr2->login_attempts == usr.login_attempts);
        REQUIRE(usr2->admin == usr.admin);
        REQUIRE(usr2->no_of_players == usr.no_of_players);
    }
}