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

#include "../event.h"

namespace roa {
    struct update_tile_event : public event_type {
        update_tile_event() = default;
        update_tile_event(uint32_t id, uint16_t tile_id) : event_type(update_tile_event::type), id(id), tile_id(tile_id) {}
        virtual ~update_tile_event() {}

        static constexpr uint32_t type = 1;
        uint32_t id;
        uint16_t tile_id;
    };
}

