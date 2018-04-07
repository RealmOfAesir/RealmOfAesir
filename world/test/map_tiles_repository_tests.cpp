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
#include "../src/repositories/map_tiles_repository.h"
#include "../src/repositories/maps_repository.h"

using namespace std;
using namespace roa;

TEST_CASE("map_tiles repository tests") {
    auto backend_injector = boost::di::make_injector(
            boost::di::bind<idatabase_transaction>.to<database_transaction>(),
            boost::di::bind<idatabase_connection>.to<database_connection>(),
            boost::di::bind<idatabase_pool>.to(db_pool),
            boost::di::bind<irepository>.to<repository>(),
            boost::di::bind<imaps_repository>.to<maps_repository>(),
            boost::di::bind<imap_tiles_repository>.to<map_tiles_repository>());

    maps_repository maps_repo = backend_injector.create<maps_repository>();
    map_tiles_repository map_tiles_repo = backend_injector.create<map_tiles_repository>();
    auto transaction = map_tiles_repo.create_transaction();
    roa::map _map{0, "test"};
    maps_repo.insert_map(_map, get<1>(transaction));

    SECTION("map_tile inserted correctly") {
        map_tile tile{0, _map.id, 3, 4, 5, 6};
        map_tiles_repo.insert_tile(tile, get<1>(transaction));

        auto tile2 = map_tiles_repo.get_tile(tile.id, get<1>(transaction));
        REQUIRE(tile2);
        REQUIRE(tile2->id == tile.id);
        REQUIRE(tile2->map_id == tile.map_id);
        REQUIRE(tile2->tile_id == tile.tile_id);
        REQUIRE(tile2->x == tile.x);
        REQUIRE(tile2->y == tile.y);
        REQUIRE(tile2->z == tile.z);
    }

    SECTION("map_tile updated correctly") {
        map_tile tile{0, _map.id, 3, 4, 5, 6};
        map_tiles_repo.insert_tile(tile, get<1>(transaction));

        roa::map _map2{0, "test"};
        maps_repo.insert_map(_map2, get<1>(transaction));

        tile.map_id = _map2.id;
        tile.tile_id = 13;
        tile.x = 14;
        tile.y = 15;
        tile.z = 16;
        map_tiles_repo.update_tile(tile, get<1>(transaction));

        auto tile2 = map_tiles_repo.get_tile(tile.id, get<1>(transaction));
        REQUIRE(tile2);
        REQUIRE(tile2->id == tile.id);
        REQUIRE(tile2->map_id == tile.map_id);
        REQUIRE(tile2->tile_id == tile.tile_id);
        REQUIRE(tile2->x == tile.x);
        REQUIRE(tile2->y == tile.y);
        REQUIRE(tile2->z == tile.z);
    }
}