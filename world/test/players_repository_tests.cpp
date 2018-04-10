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
#include "../src/repositories/players_repository.h"
#include "../src/repositories/maps_repository.h"
#include "../src/repositories/locations_repository.h"
#include "../src/repositories/settings_repository.h"

using namespace std;
using namespace roa;

TEST_CASE("players repository tests") {
    auto backend_injector = boost::di::make_injector(
            boost::di::bind<idatabase_transaction>.to<database_transaction>(),
            boost::di::bind<idatabase_connection>.to<database_connection>(),
            boost::di::bind<idatabase_pool>.to(db_pool),
            boost::di::bind<irepository>.to<repository>(),
            boost::di::bind<iplayers_repository>.to<players_repository>(),
            boost::di::bind<imaps_repository>.to<maps_repository>(),
            boost::di::bind<ilocations_repository>.to<locations_repository>(),
            boost::di::bind<isettings_repository>.to<settings_repository>());

    players_repository players_repo = backend_injector.create<players_repository>();
    maps_repository maps_repo = backend_injector.create<maps_repository>();
    locations_repository locations_repo = backend_injector.create<locations_repository>();
    auto transaction = players_repo.create_transaction();

    roa::map _map{0, "map_name"s};
    maps_repo.insert_map(_map, get<1>(transaction));

    SECTION( "player inserted correctly" ) {
        location loc{0, _map.id, 0, 0};
        locations_repo.insert_location(loc, get<1>(transaction));

        player plyr{0, 1, loc.id, "john doe"s};
        players_repo.insert_or_update_player(plyr, get<1>(transaction));

        auto plyr2 = players_repo.get_player(plyr.id, included_tables::none, get<1>(transaction));
        REQUIRE(plyr2);
        REQUIRE(plyr2->location_id == plyr.location_id);
        REQUIRE(plyr2->user_id == plyr.user_id);
        REQUIRE(plyr2->name == plyr.name);
    }

    SECTION( "multiple players retrieved correctly" ) {
        location loc{0, _map.id, 0, 0};
        locations_repo.insert_location(loc, get<1>(transaction));

        player plyr{0, 2, loc.id, "john doe"s};
        player plyr2{0, 2, loc.id, "john doe2"s};
        players_repo.insert_or_update_player(plyr, get<1>(transaction));
        players_repo.insert_or_update_player(plyr2, get<1>(transaction));

        auto players = players_repo.get_players_by_user_id(plyr.user_id, included_tables::none, get<1>(transaction));
        REQUIRE(players.size() == 2);
        REQUIRE(!players[0].location);
        REQUIRE(players[0].items.size() == 0);
        REQUIRE(players[0].stats.size() == 0);

        players = players_repo.get_players_by_user_id(plyr.user_id, included_tables::location, get<1>(transaction));
        REQUIRE(players.size() == 2);
        REQUIRE(players[0].location);
        REQUIRE(players[0].location->map_name == _map.name);
        REQUIRE(players[0].location->x == loc.x);
        REQUIRE(players[0].location->y == loc.y);
        REQUIRE(players[0].items.size() == 0);
        REQUIRE(players[0].stats.size() == 0);
    }
}