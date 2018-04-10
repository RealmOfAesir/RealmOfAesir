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

#include <boost/di.hpp>
#include "../ecs/ecs.h"
#include "world.h"
#include <easylogging++.h>
#include <macros.h>
#include <config.h>
#include <ecs/systems/player_event_system.h>
#include <database_connection.h>
#include <database_transaction.h>

using namespace std;
using namespace roa;

world::~world() {
    _systems.clear();
    _ex.reset();
}

void world::do_tick(uint32_t tick_length) {
    LOG(TRACE) << NAMEOF(world::do_tick) << " starting tick";

    for(auto& system : _systems) {
        system->update(_ex, tick_length);
    }
    LOG(TRACE) << NAMEOF(world::do_tick) << " finished tick";
}

void world::load_from_database(shared_ptr<idatabase_pool> db_pool, Config& config,
                               moodycamel::ReaderWriterQueue<std::shared_ptr<player_event>> &player_event_queue, shared_ptr<ikafka_producer<false>> producer) {

    LOG(INFO) << NAMEOF(world::load_from_database) << " loading world from db";

    auto repo_injector = boost::di::make_injector(
            boost::di::bind<idatabase_transaction>.to<database_transaction>(),
            boost::di::bind<idatabase_connection>.to<database_connection>(),
            boost::di::bind<idatabase_pool>.to(db_pool)
    );

    _systems.emplace_back(make_unique<player_event_system>(config, player_event_queue, producer));

    auto map_entity = _ex.create();
    auto& mc = _ex.assign<map_component>(map_entity, 1u, 64u, 64u, 640u, 640u, 1u, 1u, 10u);

    mc.tilesets.emplace_back(1, "terrain.png"s, 64, 64, 1536, 2560);

    mc.layers.emplace_back(vector<uint64_t>{}, 0, 0, 100, 100);
    mc.layers[0].tiles.reserve(100*100);

    for(uint32_t x = 0; x < 100; x++) {
        for(uint32_t y = 0; y < 100; y++) {
            auto tile_entity = _ex.create();
            _ex.assign<tile_component>(tile_entity, map_entity, static_cast<uint16_t>(y+1));
            mc.layers[0].tiles.push_back(tile_entity);
        }
    }

    for(uint32_t x = 0; x < 10; x++) {
        auto npc_entity = _ex.create();
        _ex.assign<position_component>(npc_entity, x, 0u, mc.id);
        mc.npcs.push_back(npc_entity);
    }

    LOG(INFO) << NAMEOF(world::load_from_database) << " loaded world from db with " << _ex.size() << " entities";
}
