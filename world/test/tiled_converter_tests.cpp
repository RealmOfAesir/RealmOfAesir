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

#include <easylogging++.h>
#include <catch.hpp>
#include <ecs/ecs.h>
#include <helpers/tiled_converter.h>
#include <nlohmann/json.hpp>
#include <base64.h>
#include <lz4.h>
#include <unistd.h>
#include <chrono>

using namespace std;
using namespace roa;
using namespace nlohmann;

size_t get_current_rss() {
    /* Linux ---------------------------------------------------- */
    long rss = 0L;
    FILE* fp = NULL;
    if ( (fp = fopen( "/proc/self/statm", "r" )) == NULL )
        return (size_t)0L;      /* Can't open? */
    if ( fscanf( fp, "%*s%ld", &rss ) != 1 )
    {
        fclose( fp );
        return (size_t)0L;      /* Can't read? */
    }
    fclose( fp );
    return (size_t)rss * (size_t)sysconf( _SC_PAGESIZE);
}

TEST_CASE("convert_map_to_json tests") {
    EntityManager _ex;

    const int layers = 100;
    const int width_tiles = 500;
    const int height_tiles = 500;

    auto start_mem = get_current_rss();
    auto start = chrono::high_resolution_clock::now();
    auto map_entity = _ex.create();
    auto& mc = _ex.assign<map_component>(map_entity, 0u, 64u, 64u, 640u, 640u, 1u, 1u, 101u);
    mc.tilesets.emplace_back(1, "terrain.png"s, 64, 64, 1536, 2560);
    for(uint32_t z = 0; z < layers; z++) {
        mc.layers.emplace_back(vector<uint64_t>{}, 0, 0, width_tiles, height_tiles);
        mc.layers[z].tiles.reserve(width_tiles * height_tiles);

        for(uint32_t x = 0; x < width_tiles; x++) {
            for(uint32_t y = 0; y < height_tiles; y++) {
                auto tile_entity = _ex.create();
                _ex.assign<tile_component>(tile_entity, map_entity, static_cast<uint16_t>(y+1));
                mc.layers[z].tiles.push_back(tile_entity);
            }
        }
    }

    auto end = chrono::high_resolution_clock::now();
    auto end_mem = get_current_rss();

    LOG(INFO) << " startup time required to run: " << chrono::duration_cast<chrono::milliseconds>((end - start)).count() << " ms";
    LOG(INFO) << " mem: " << start_mem / 1024 / 1024 << " - " << (end_mem - start_mem) / 1024 / 1024;

    start = chrono::high_resolution_clock::now();
    string ret;
    REQUIRE_NOTHROW(ret = tiled_converter::convert_map_to_json(_ex, mc));
    end = chrono::high_resolution_clock::now();

    LOG(INFO) << " convert to json time required to run: " << chrono::duration_cast<chrono::milliseconds>((end - start)).count() << " ms";

    _ex.reset();

    auto json_map = json::parse(ret);

    for(uint32_t z = 0; z < layers; z++) {
        std::string base64data = json_map["layers"][z]["data"];
        std::string data = base64_decode(base64data);

        int tiles_size = width_tiles * height_tiles * 4;
        char *tiles = new char[tiles_size];

        auto lz4_ret = LZ4_decompress_safe(data.c_str(), tiles, data.size(), tiles_size);
        CHECK(lz4_ret > 0);

        if (lz4_ret > 0) {
            for (uint32_t x = 0; x < width_tiles; x++) {
                for (uint32_t y = 0; y < height_tiles; y++) {
                    uint32_t tocheck = reinterpret_cast<uint32_t *>(tiles)[x * width_tiles + y];
                    CHECK(tocheck == y + 1);
                }
            }
        }

        delete tiles;
    }
}