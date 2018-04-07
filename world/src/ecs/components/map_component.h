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
#include <ecs/ecs.h>

namespace roa {
    struct layer {
        layer(std::vector<uint64_t> tiles, uint32_t x, uint32_t y, uint32_t width, uint32_t height)
                : tiles(tiles), x(x), y(y), width(width), height(height) {}

        std::vector<uint64_t> tiles;
        uint32_t x;
        uint32_t y;
        uint32_t width;
        uint32_t height;
    };

    struct tileset {
        tileset(uint32_t first_gid, std::string image_path, uint32_t tile_width, uint32_t tile_height,
                uint32_t width, uint32_t height) : first_gid(first_gid), image_path(image_path), tile_width(tile_width),
                                                   tile_height(tile_height), width(width), height(height) {}

        uint32_t first_gid;
        std::string image_path;
        uint32_t tile_width;
        uint32_t tile_height;
        uint32_t width;
        uint32_t height;
    };

    struct map_component {
        map_component(uint64_t id, uint32_t tile_height, uint32_t tile_width,
                      uint32_t width, uint32_t height, uint32_t no_layers, uint32_t first_tile_id, uint32_t max_tile_id)
                : id(id), tile_height(tile_height), tile_width(tile_width),
                  width(width), height(height), no_layers(no_layers), first_tile_id(first_tile_id), max_tile_id(max_tile_id)
        {}

        uint64_t id;
        uint32_t tile_height;
        uint32_t tile_width;
        uint32_t width;
        uint32_t height;
        uint32_t no_layers;
        uint32_t first_tile_id;
        uint32_t max_tile_id;
        std::vector<tileset> tilesets;
        std::vector<layer> layers;
        std::vector<uint64_t> npcs;
        std::vector<uint64_t> pcs;
        std::vector<uint64_t> items;
    };
}