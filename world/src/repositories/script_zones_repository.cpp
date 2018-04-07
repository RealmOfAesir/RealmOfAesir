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

#include "script_zones_repository.h"

#include <easylogging++.h>

#include "repository_exceptions.h"
#include <macros.h>

using namespace std;
using namespace experimental;
using namespace roa;
using namespace pqxx;

script_zones_repository::script_zones_repository(shared_ptr<idatabase_pool> database_pool) : repository(database_pool) {

}

script_zones_repository::script_zones_repository(script_zones_repository &repo) : repository(repo._database_pool) {

}

script_zones_repository::script_zones_repository(script_zones_repository &&repo) : repository(repo._database_pool) {

}

script_zones_repository::~script_zones_repository() {

}

auto script_zones_repository::create_transaction() -> decltype(repository::create_transaction()) {
    return repository::create_transaction();
}

void script_zones_repository::insert_script_zone(script_zone &zone,
                                                 unique_ptr<idatabase_transaction> const &transaction) {
    auto result = transaction->execute(
            "INSERT INTO script_zones (zone_name, map_id, x, y, width, height) VALUES ('" + transaction->escape(zone.zone_name) + "', " +
            to_string(zone.map_id) + ", " + to_string(zone.x) + ", " + to_string(zone.y) + ", " +
                    to_string(zone.width) + ", " + to_string(zone.height) + ") RETURNING id");

    if(unlikely(result.size() == 0)) {
        throw unexpected_result_error("expected exactly one result"s);
    }

    zone.id = result[0][0].as<uint32_t>();

    LOG(DEBUG) << NAMEOF(script_zones_repository::insert_script_zone) << " inserted zone with id " << zone.id;
}

void script_zones_repository::update_script_zone(script_zone &zone,
                                                 unique_ptr<idatabase_transaction> const &transaction) {
    transaction->execute(
            "UPDATE script_zones SET zone_name = '" + transaction->escape(zone.zone_name) + "', map_id = " +
            to_string(zone.map_id) + ", x = " + to_string(zone.x) + ", y = " + to_string(zone.y) +
            ", width = " + to_string(zone.width) + ", height = " + to_string(zone.height) + " WHERE id = " + to_string(zone.id));

    LOG(DEBUG) << NAMEOF(script_zones_repository::update_script_zone) << " updated zone with id " << zone.id;
}

STD_OPTIONAL<script_zone>
script_zones_repository::get_script_zone(uint32_t id, unique_ptr<idatabase_transaction> const &transaction) {
    auto result = transaction->execute("SELECT sz.id, sz.zone_name, sz.map_id, sz.x, sz.y, sz.width, sz.height FROM script_zones sz WHERE id = " + to_string(id));

    if(result.size() == 0) {
        LOG(DEBUG) << NAMEOF(script_zones_repository::get_script_zone) << " found no zone by id " << id;
        return {};
    }

    auto ret = make_optional<script_zone>({result[0][0].as<uint32_t>(), result[0][1].as<string>(),
                                           result[0][2].as<uint32_t>(), result[0][3].as<uint32_t>(),
                                           result[0][4].as<uint32_t>(), result[0][5].as<uint32_t>(),
                                           result[0][6].as<uint32_t>()});

    LOG(DEBUG) << NAMEOF(script_zones_repository::get_script_zone) << " found zone with id " << ret->id;

    return ret;
}

