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
#include <custom_optional.h>
#include <vector>

namespace roa {
    struct player_location {
        uint32_t x;
        uint32_t y;
        uint32_t map_id;
        std::string map_name;

        player_location() = default;
        player_location(uint32_t x, uint32_t y, uint32_t map_id, std::string map_name) : x(x), y(y), map_id(map_id), map_name(map_name) {}
    };

    struct player_stat {
        uint64_t id;
        std::string name;
        uint64_t value;

        player_stat() = default;
        player_stat(uint64_t id, std::string name, uint64_t value) : id(id), name(name), value(value) {}
    };

    struct player_item {
        uint64_t id;
        std::string name;

        player_item() = default;
        player_item(uint64_t id, std::string name) : id(id), name(name) {}
    };

    struct player {
        uint64_t id;
        uint64_t user_id;
        uint64_t location_id;
        std::string name;
        STD_OPTIONAL<player_location> location;
        std::vector<player_stat> stats;
        std::vector<player_item> items;
    };
}

