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

#include "map_tiles_repository.h"

#include <easylogging++.h>

#include "repository_exceptions.h"
#include <macros.h>

using namespace std;
using namespace experimental;
using namespace roa;
using namespace pqxx;

map_tiles_repository::map_tiles_repository(shared_ptr<idatabase_pool> database_pool) : repository(database_pool) {

}

map_tiles_repository::map_tiles_repository(map_tiles_repository &repo) : repository(repo._database_pool) {

}

map_tiles_repository::map_tiles_repository(map_tiles_repository &&repo) : repository(repo._database_pool) {

}

map_tiles_repository::~map_tiles_repository() {

}

auto map_tiles_repository::create_transaction() -> decltype(repository::create_transaction()) {
    return repository::create_transaction();
}

void map_tiles_repository::insert_tile(map_tile &tile, std::unique_ptr<idatabase_transaction> const &transaction) {
    auto result = transaction->execute(
            "INSERT INTO map_tiles (map_id, tile_id, x, y, z) VALUES (" + to_string(tile.map_id) +
            ", " + to_string(tile.tile_id) + ", " + to_string(tile.x) + ", " + to_string(tile.y) + ", " + to_string(tile.z) +
            ") RETURNING id");

    if(unlikely(result.size() == 0)) {
        throw unexpected_result_error("expected exactly one result"s);
    }

    tile.id = result[0][0].as<uint32_t>();

    LOG(DEBUG) << NAMEOF(map_tiles_repository::insert_tile) << " inserted map_tile with id " << tile.id;
}

void map_tiles_repository::update_tile(map_tile &tile, std::unique_ptr<idatabase_transaction> const &transaction) {
    transaction->execute(
            "UPDATE map_tiles set map_id = " + to_string(tile.map_id) + ", tile_id = " + to_string(tile.tile_id) +
            ", x = " + to_string(tile.x) + ", y = " + to_string(tile.y) + ", z = " + to_string(tile.z) +
            " WHERE id = " + to_string(tile.id));

    LOG(DEBUG) << NAMEOF(map_tiles_repository::update_tile) << " updated map_tile with id " << tile.id;
}

optional <map_tile> map_tiles_repository::get_tile(uint64_t id, std::unique_ptr<idatabase_transaction> const &transaction) {
    auto result = transaction->execute("SELECT mt.id, mt.map_id, mt.tile_id, mt.x, mt.y, mt.z FROM map_tiles mt WHERE id = " + to_string(id));

    if(result.size() == 0) {
        LOG(DEBUG) << NAMEOF(map_tiles_repository::get_tile) << " found no tile by id " << id;
        return {};
    }

    auto ret = make_optional<map_tile>({result[0][0].as<uint64_t>(), result[0][1].as<uint32_t>(), result[0][2].as<uint32_t>(),
                                        result[0][3].as<uint32_t>(), result[0][4].as<uint32_t>(), result[0][5].as<uint32_t>()});

    LOG(DEBUG) << NAMEOF(map_tiles_repository::get_tile) << " found tile with id " << ret->id;

    return ret;
}

std::vector<map_tile>
map_tiles_repository::get_tiles_for_map(uint32_t map_id, std::unique_ptr<idatabase_transaction> const &transaction) {
    auto result = transaction->execute("SELECT mt.id, mt.map_id, mt.tile_id, mt.x, mt.y, mt.z FROM map_tiles mt WHERE map_id = " + to_string(map_id));

    if(result.size() == 0) {
        LOG(DEBUG) << NAMEOF(map_tiles_repository::get_tiles_for_map) << " found no tiles by id " << map_id;
        return {};
    }

    vector<map_tile> tiles;
    tiles.reserve(result.size());

    for(auto const & res : result) {
        tiles.emplace_back<map_tile>({result[0][0].as<uint64_t>(), result[0][1].as<uint32_t>(), result[0][2].as<uint32_t>(),
                                      result[0][3].as<uint32_t>(), result[0][4].as<uint32_t>(), result[0][5].as<uint32_t>()});
    }

    LOG(DEBUG) << NAMEOF(map_tiles_repository::get_tiles_for_map) << " found " << tiles.size() << " tiles for map_id " << map_id;

    return tiles;
}
