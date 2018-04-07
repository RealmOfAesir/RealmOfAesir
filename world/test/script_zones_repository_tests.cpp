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
#include "../src/repositories/script_zones_repository.h"

using namespace std;
using namespace roa;

TEST_CASE("script zones repository tests") {
    auto backend_injector = boost::di::make_injector(
            boost::di::bind<idatabase_transaction>.to<database_transaction>(),
            boost::di::bind<idatabase_connection>.to<database_connection>(),
            boost::di::bind<idatabase_pool>.to(db_pool),
            boost::di::bind<irepository>.to<repository>(),
            boost::di::bind<imaps_repository>.to<maps_repository>(),
            boost::di::bind<iscript_zones_repository>.to<script_zones_repository>());

    maps_repository maps_repo = backend_injector.create<maps_repository>();
    script_zones_repository script_zones_repo = backend_injector.create<script_zones_repository>();
    auto transaction = script_zones_repo.create_transaction();

    SECTION( "script_zone inserted correctly" ) {
        roa::map _map{0, "map_name"s};

        maps_repo.insert_map(_map, get<1>(transaction));

        script_zone z{0, "zone_name", _map.id, 1, 2, 3, 4};
        script_zones_repo.insert_script_zone(z, get<1>(transaction));
        REQUIRE(z.id > 0);

        auto z2 = script_zones_repo.get_script_zone(z.id, get<1>(transaction));
        REQUIRE(z2);
        REQUIRE(z2->id == z.id);
        REQUIRE(z2->zone_name == z.zone_name);
        REQUIRE(z2->map_id == z.map_id);
        REQUIRE(z2->x == z.x);
        REQUIRE(z2->y == z.y);
        REQUIRE(z2->width == z.width);
        REQUIRE(z2->height == z.height);
    }
}