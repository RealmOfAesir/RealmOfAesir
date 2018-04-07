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
#include "../src/repositories/maps_repository.h"
#include "../src/repositories/locations_repository.h"

using namespace std;
using namespace roa;

TEST_CASE("locations repository tests") {
    auto backend_injector = boost::di::make_injector(
            boost::di::bind<idatabase_transaction>.to<database_transaction>(),
            boost::di::bind<idatabase_connection>.to<database_connection>(),
            boost::di::bind<idatabase_pool>.to(db_pool),
            boost::di::bind<irepository>.to<repository>(),
            boost::di::bind<imaps_repository>.to<maps_repository>(),
            boost::di::bind<ilocations_repository>.to<locations_repository>());

    maps_repository maps_repo = backend_injector.create<maps_repository>();
    locations_repository locations_repo = backend_injector.create<locations_repository>();
    auto transaction = locations_repo.create_transaction();

    SECTION( "location inserted correctly" ) {
        roa::map _map{0, "map_name"s};

        maps_repo.insert_map(_map, get<1>(transaction));

        location loc{0, _map.id, 1, 2};
        locations_repo.insert_location(loc, get<1>(transaction));
        REQUIRE(loc.id > 0);

        auto loc2 = locations_repo.get_location(loc.id, get<1>(transaction));
        REQUIRE(loc2);
        REQUIRE(loc2->id == loc.id);
        REQUIRE(loc2->map_id == loc.map_id);
        REQUIRE(loc2->x == loc.x);
        REQUIRE(loc2->y == loc.y);
    }
}