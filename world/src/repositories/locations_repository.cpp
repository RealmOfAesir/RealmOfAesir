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

#include "locations_repository.h"

#include <easylogging++.h>

#include "repository_exceptions.h"
#include <macros.h>

using namespace std;
using namespace experimental;
using namespace roa;
using namespace pqxx;

locations_repository::locations_repository(shared_ptr<idatabase_pool> database_pool) : repository(database_pool) {

}

locations_repository::locations_repository(locations_repository &repo) : repository(repo._database_pool) {

}

locations_repository::locations_repository(locations_repository &&repo) : repository(repo._database_pool) {

}

locations_repository::~locations_repository() {

}

auto locations_repository::create_transaction() -> decltype(repository::create_transaction()) {
    return repository::create_transaction();
}

void locations_repository::insert_location(location &loc, std::unique_ptr<idatabase_transaction> const &transaction) {
    auto result = transaction->execute(
            "INSERT INTO locations (map_id, x, y) VALUES (" + to_string(loc.map_id) +
            ", " + to_string(loc.x) + ", " + to_string(loc.y) + ") RETURNING id");

    if(unlikely(result.size() == 0)) {
        throw unexpected_result_error("expected exactly one result"s);
    }

    loc.id = result[0][0].as<uint32_t>();

    LOG(DEBUG) << NAMEOF(locations_repository::insert_location) << " inserted location with id " << loc.id;
}

void locations_repository::update_location(location &loc, std::unique_ptr<idatabase_transaction> const &transaction) {
    transaction->execute(
            "UPDATE locations SET map_id = " + to_string(loc.map_id) + ", x = " + to_string(loc.x) +
            ", y = " + to_string(loc.y) + " WHERE id = " + to_string(loc.id));

    LOG(DEBUG) << NAMEOF(locations_repository::insert_location) << " updated location with id " << loc.id;
}

STD_OPTIONAL<location>
locations_repository::get_location(uint64_t id, std::unique_ptr<idatabase_transaction> const &transaction) {
    auto result = transaction->execute("SELECT l.id, l.map_id, l.x, l.y FROM locations l WHERE id = " + to_string(id));

    if(result.size() == 0) {
        LOG(DEBUG) << NAMEOF(locations_repository::get_location) << " found no location by id " << id;
        return {};
    }

    auto ret = make_optional<location>({result[0][0].as<uint64_t>(), result[0][1].as<uint32_t>(),
                                        result[0][2].as<uint32_t>(), result[0][3].as<uint32_t>()});

    LOG(DEBUG) << NAMEOF(locations_repository::get_location) << " found location with id " << ret->id;

    return ret;
}

std::vector<location> locations_repository::get_locations_by_map_id(uint32_t map_id,
                                                                    std::unique_ptr<idatabase_transaction> const &transaction) {
    auto result = transaction->execute("SELECT l.id, l.map_id, l.x, l.y FROM locations l WHERE map_id = " + to_string(map_id));

    LOG(DEBUG) << NAMEOF(locations_repository::get_locations_by_map_id) << " contains " << result.size() << " entries";

    vector<location> locations;
    locations.reserve(result.size());

    for(auto const & res : result) {
        locations.emplace_back<location>({result[0][0].as<uint32_t>(), result[0][1].as<uint32_t>(),
                                          result[0][2].as<uint32_t>(), result[0][3].as<uint32_t>()});
    }

    return locations;
}
