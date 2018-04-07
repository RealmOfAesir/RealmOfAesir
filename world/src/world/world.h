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

#pragma once

#include <vector>
#include <database_pool.h>
#include <config.h>
#include <ecs/systems/ecs_system.h>
#include <readerwriterqueue.h>
#include <events/event.h>
#include <repositories/models/player_model.h>
#include <kafka_producer.h>
#include <events/player_events/player_event.h>

namespace roa {
    class world {
    public:

        ~world();
        void do_tick(uint32_t tick_length);
        void load_from_database(std::shared_ptr<idatabase_pool> db_pool, Config& config,
                                moodycamel::ReaderWriterQueue<std::shared_ptr<player_event>> &player_event_queue, std::shared_ptr<ikafka_producer<false>> producer);

    private:
        EntityManager _ex;
        std::vector<std::unique_ptr<ecs_system>> _systems;
    };
}