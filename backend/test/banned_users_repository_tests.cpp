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
#include "../src/repositories/banned_users_repository.h"
#include "../src/repositories/repository_exceptions.h"
#include "test_helpers/startup_helper.h"

using namespace std;
using namespace roa;

TEST_CASE("banned repository users tests") {
    auto backend_injector = boost::di::make_injector(
            boost::di::bind<idatabase_transaction>.to<database_transaction>(),
            boost::di::bind<idatabase_connection>.to<database_connection>(),
            boost::di::bind<idatabase_pool>.to(db_pool),
            boost::di::bind<irepository>.to<repository>(),
            boost::di::bind<iusers_repository>.to<users_repository>(),
            boost::di::bind<ibanned_users_repository>.to<banned_users_repository>());

    users_repository user_repo = backend_injector.create<users_repository>();
    banned_users_repository banned_user_repo = backend_injector.create<banned_users_repository>();
    SECTION( "banned user inserted correctly" ) {
        auto transaction = banned_user_repo.create_transaction();
        banned_user usr{0, "192.168.0.1"s, {}, chrono::system_clock::now() += chrono::seconds(10)};
        banned_user_repo.insert_banned_user(usr, get<1>(transaction));
        REQUIRE(usr.id != 0);

        auto usr2 = banned_user_repo.get_banned_user(usr.id, get<1>(transaction));
        REQUIRE(usr2);
        REQUIRE(usr2->id == usr.id);
        REQUIRE(usr2->ip == usr.ip);
        REQUIRE(!usr2->_user);
        REQUIRE(usr2->until);

        auto time1 = chrono::duration_cast<chrono::seconds>(usr.until->time_since_epoch()).count();
        auto time2 = chrono::duration_cast<chrono::seconds>(usr2->until->time_since_epoch()).count();

        REQUIRE(time1 == time2);
    }

    SECTION( "is user or ip banned" ) {
        auto transaction = banned_user_repo.create_transaction();
        user usr{0, "banned_user", "", "", 0, 0};
        user_repo.insert_user_if_not_exists(usr, get<1>(transaction));
        REQUIRE(usr.id > 0);

        banned_user banned_usr{0, "192.168.0.1"s, usr, chrono::system_clock::now() += chrono::seconds(10)};
        banned_user_repo.insert_banned_user(banned_usr, get<1>(transaction));
        REQUIRE(banned_usr.id != 0);

        REQUIRE_THROWS_AS(banned_user_repo.is_username_or_ip_banned({}, {}, get<1>(transaction)), wrong_parameters_exception);
        auto usr2 = banned_user_repo.is_username_or_ip_banned({"banned_user"s}, {}, get<1>(transaction));
        REQUIRE(usr2);
        REQUIRE(usr2->id == banned_usr.id);
        REQUIRE(usr2->ip == banned_usr.ip);
        REQUIRE(!usr2->_user);
        REQUIRE(usr2->until);

        auto time1 = chrono::duration_cast<chrono::seconds>(banned_usr.until->time_since_epoch()).count();
        auto time2 = chrono::duration_cast<chrono::seconds>(usr2->until->time_since_epoch()).count();

        REQUIRE(time1 == time2);

        usr2 = banned_user_repo.is_username_or_ip_banned({}, {"192.168.0.1"}, get<1>(transaction));
        REQUIRE(usr2);
        REQUIRE(usr2->id == banned_usr.id);
        REQUIRE(usr2->ip == banned_usr.ip);
        REQUIRE(!usr2->_user);
        REQUIRE(usr2->until);

        time1 = chrono::duration_cast<chrono::seconds>(banned_usr.until->time_since_epoch()).count();
        time2 = chrono::duration_cast<chrono::seconds>(usr2->until->time_since_epoch()).count();

        REQUIRE(time1 == time2);

        usr2 = banned_user_repo.is_username_or_ip_banned({"banned_user"}, {"192.168.0.1"}, get<1>(transaction));
        REQUIRE(usr2);
        REQUIRE(usr2->id == banned_usr.id);
        REQUIRE(usr2->ip == banned_usr.ip);
        REQUIRE(!usr2->_user);
        REQUIRE(usr2->until);

        time1 = chrono::duration_cast<chrono::seconds>(banned_usr.until->time_since_epoch()).count();
        time2 = chrono::duration_cast<chrono::seconds>(usr2->until->time_since_epoch()).count();

        REQUIRE(time1 == time2);
    }

    SECTION( "user or ip was banned" ) {
        auto transaction = banned_user_repo.create_transaction();
        user usr{0, "banned_user", "", "", 0, 0};
        user_repo.insert_user_if_not_exists(usr, get<1>(transaction));
        REQUIRE(usr.id > 0);

        banned_user banned_usr{0, "192.168.0.1"s, usr, chrono::system_clock::now() -= chrono::hours(5)};
        banned_user_repo.insert_banned_user(banned_usr, get<1>(transaction));
        REQUIRE(banned_usr.id != 0);

        auto usr2 = banned_user_repo.is_username_or_ip_banned({"banned_user"s}, {}, get<1>(transaction));
        REQUIRE(!usr2);

        usr2 = banned_user_repo.is_username_or_ip_banned({}, {"192.168.0.1"}, get<1>(transaction));
        REQUIRE(!usr2);

        usr2 = banned_user_repo.is_username_or_ip_banned({"banned_user"}, {"192.168.0.1"}, get<1>(transaction));
        REQUIRE(!usr2);
    }

    SECTION( "update banned_user" ) {
        auto transaction = banned_user_repo.create_transaction();
        user usr{0, "banned_user", "", "", 0, 0};
        user_repo.insert_user_if_not_exists(usr, get<1>(transaction));
        REQUIRE(usr.id > 0);

        banned_user banned_usr{0, "192.168.0.1"s, {}, chrono::system_clock::now() += chrono::seconds(10)};
        banned_user_repo.insert_banned_user(banned_usr, get<1>(transaction));
        REQUIRE(banned_usr.id != 0);

        banned_usr._user = usr;
        banned_user_repo.update_banned_user(banned_usr, get<1>(transaction));

        auto usr2 = banned_user_repo.is_username_or_ip_banned({"banned_user"s}, {}, get<1>(transaction));
        REQUIRE(usr2);
        REQUIRE(usr2->id == banned_usr.id);
        REQUIRE(usr2->ip == banned_usr.ip);
        REQUIRE(!usr2->_user);
        REQUIRE(usr2->until);

        auto time1 = chrono::duration_cast<chrono::seconds>(banned_usr.until->time_since_epoch()).count();
        auto time2 = chrono::duration_cast<chrono::seconds>(usr2->until->time_since_epoch()).count();

        REQUIRE(time1 == time2);
    }
}