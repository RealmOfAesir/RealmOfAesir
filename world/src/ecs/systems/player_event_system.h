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

#include <config.h>
#include <kafka_producer.h>
#include <events/player_events/player_event.h>
#include <readerwriterqueue.h>
#include <repositories/models/player_model.h>
#include "ecs_system.h"

namespace roa {
    class player_event_system : public ecs_system {
    public:
        explicit player_event_system(Config& config, moodycamel::ReaderWriterQueue<std::shared_ptr<player_event>> &player_event_queue,
                                     std::shared_ptr<ikafka_producer<false>> producer)
                : _config(config), _player_event_queue(player_event_queue), _producer(producer) {}
        virtual ~player_event_system() override {};

        void update(EntityManager &es, TimeDelta dt) override;
    private:
        Config& _config;
        moodycamel::ReaderWriterQueue<std::shared_ptr<player_event>> &_player_event_queue;
        std::shared_ptr<ikafka_producer<false>> _producer;

        STD_OPTIONAL<map_component> get_map(EntityManager &es, player const &plyr) const noexcept;
        void send_error(player_event const * const event) const noexcept;
    };
}
