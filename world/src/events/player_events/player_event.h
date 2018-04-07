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

#include "../event.h"
#include <messages/message.h>
#include <memory>
#include <ecs/ecs.h>

#pragma once

namespace roa {
    class player_event : public event_type {
    public:
        explicit player_event(uint32_t type, uint32_t player_client_id, uint32_t player_gateway_id) : event_type(type)
                , _player_client_id(player_client_id), _player_gateway_id(player_gateway_id) {}
        virtual ~player_event() {};
        uint32_t _player_client_id;
        uint32_t _player_gateway_id;

        virtual std::unique_ptr<binary_message> process(EntityManager &es) const { return {}; };
    };
}

