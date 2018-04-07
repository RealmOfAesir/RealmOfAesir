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

#include <repositories/models/player_model.h>
#include "player_event.h"

namespace roa {
    struct enter_world_event : public player_event {
        enter_world_event() = default;
        enter_world_event(uint32_t player_client_id, uint32_t player_gateway_id, player _player)
                : player_event(enter_world_event::type, player_client_id, player_gateway_id), _player(_player), _debug(false) {}
        virtual ~enter_world_event() {}

        static constexpr uint32_t type = 1001;
        player _player;
        bool _debug;

        std::unique_ptr<binary_message> process(EntityManager &es) const override;
    };
}

