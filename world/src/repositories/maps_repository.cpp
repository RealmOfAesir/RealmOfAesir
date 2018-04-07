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

#include "maps_repository.h"

#include <easylogging++.h>

#include "repository_exceptions.h"
#include <macros.h>

using namespace std;
using namespace experimental;
using namespace roa;
using namespace pqxx;

maps_repository::maps_repository(shared_ptr<idatabase_pool> database_pool) : repository(database_pool) {

}

maps_repository::maps_repository(maps_repository &repo) : repository(repo._database_pool) {

}

maps_repository::maps_repository(maps_repository &&repo) : repository(repo._database_pool) {

}

maps_repository::~maps_repository() {

}

auto maps_repository::create_transaction() -> decltype(repository::create_transaction()) {
    return repository::create_transaction();
}

void maps_repository::insert_map(roa::map &_map, std::unique_ptr<idatabase_transaction> const &transaction) {
    auto result = transaction->execute(
            "INSERT INTO maps (map_name) VALUES ('" + transaction->escape(_map.name) +
            "') RETURNING id");

    if(unlikely(result.size() == 0)) {
        throw unexpected_result_error("expected exactly one result"s);
    }

    _map.id = result[0][0].as<uint32_t>();

    LOG(DEBUG) << NAMEOF(maps_repository::insert_map) << " inserted map with id " << _map.id;
}

void maps_repository::update_map(roa::map &_map, std::unique_ptr<idatabase_transaction> const &transaction) {
    transaction->execute(
            "UPDATE maps set map_name = '" + transaction->escape(_map.name) +
            "' WHERE id = " + to_string(_map.id));

    LOG(DEBUG) << NAMEOF(maps_repository::update_map) << " updated map with id " << _map.id;
}

optional <roa::map> maps_repository::get_map(uint32_t id, std::unique_ptr<idatabase_transaction> const &transaction) {
    auto result = transaction->execute("SELECT m.id, m.map_name FROM maps m WHERE id = " + to_string(id));

    if(result.size() == 0) {
        LOG(DEBUG) << NAMEOF(maps_repository::get_map) << " found no map by id " << id;
        return {};
    }

    auto ret = make_optional<map>({result[0][0].as<uint32_t>(), result[0][1].as<string>()});

    LOG(DEBUG) << NAMEOF(maps_repository::get_map) << " found map with id " << ret->id;

    return ret;
}

optional <roa::map>
maps_repository::get_map(std::string const &name, std::unique_ptr<idatabase_transaction> const &transaction) {
    auto result = transaction->execute("SELECT m.id, m.map_name FROM maps m WHERE map_name = " + transaction->escape(name));

    if(result.size() == 0) {
        LOG(DEBUG) << NAMEOF(maps_repository::get_map) << " found no map by name " << name;
        return {};
    }

    auto ret = make_optional<map>({result[0][0].as<uint32_t>(), result[0][1].as<string>()});

    LOG(DEBUG) << NAMEOF(maps_repository::get_map) << " found map with name " << name;

    return ret;
}
