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

#include "players_repository.h"

#include <easylogging++.h>

#include "repository_exceptions.h"
#include <macros.h>

using namespace std;
using namespace experimental;
using namespace roa;
using namespace pqxx;

players_repository::players_repository(shared_ptr<idatabase_pool> database_pool) : repository(database_pool) {

}

players_repository::players_repository(players_repository &repo) : repository(repo._database_pool) {

}

players_repository::players_repository(players_repository &&repo) : repository(repo._database_pool) {

}

players_repository::~players_repository() {

}

auto players_repository::create_transaction() -> decltype(repository::create_transaction()) {
    return repository::create_transaction();
}

bool players_repository::insert_or_update_player(player &plyr, unique_ptr<idatabase_transaction> const &transaction) {

    auto result = transaction->execute(
            "INSERT INTO players (user_id, location_id, player_name) VALUES (" + to_string(plyr.user_id) +
            ", " + to_string(plyr.location_id) + ", '" + transaction->escape(plyr.name) + "') ON CONFLICT (player_name) DO UPDATE SET"
            " user_id = " + to_string(plyr.user_id) + ", location_id = " + to_string(plyr.location_id) +
            " RETURNING xmax, id");

    if(unlikely(result.size() == 0)) {
        throw unexpected_result_error("expected exactly one result"s);
    }

    plyr.id = result[0][1].as<uint64_t>();

    if(result[0][0].as<string>() == "0") {
        LOG(DEBUG) << NAMEOF(players_repository::insert_or_update_player) << " inserted player with id " << plyr.id;
        return true;
    }
    LOG(DEBUG) << NAMEOF(players_repository::insert_or_update_player) << " updated player with id " << plyr.id;
    return false;
}

void players_repository::update_player(player &plyr, unique_ptr<idatabase_transaction> const &transaction) {

    transaction->execute(
        "UPDATE players SET user_id = " + to_string(plyr.user_id) + ", location_id = " + to_string(plyr.location_id) +
        " WHERE id = " + to_string(plyr.id));

    LOG(DEBUG) << NAMEOF(players_repository::update_player) << " updated player with id " << plyr.id;
}

STD_OPTIONAL<player> players_repository::get_player(string const &name, included_tables includes,
                                                    unique_ptr<idatabase_transaction> const &transaction) {
    pqxx::result result;

    if(includes == included_tables::none) {
        result = transaction->execute("SELECT p.id, p.user_id, p.location_id, p.player_name FROM players p WHERE player_name = '" + transaction->escape(name) + "'");
    } else if (includes == included_tables::location) {
        result = transaction->execute("SELECT p.id, p.user_id, p.location_id, p.player_name, l.x, l.y, m.id, m.map_name FROM players p "
                                              "INNER JOIN locations l ON l.id = p.location_id "
                                              "INNER JOIN maps m ON m.id = l.map_id "
                                              "WHERE p.player_name = '" + transaction->escape(name) + "'");
    } else {
        LOG(ERROR) << NAMEOF(players_repository::get_player) << " included_tables value " << static_cast<int>(includes) << " not implemented";
        throw unexpected_result_error("included_tables value not implemented");
    }

    if(result.size() == 0) {
        LOG(DEBUG) << NAMEOF(players_repository::get_player) << " found no player by name " << name;
        return {};
    }

    auto ret = make_optional<player>({result[0][0].as<uint64_t>(), result[0][1].as<uint64_t>(),
                                      result[0][2].as<uint64_t>(), result[0][3].as<string>()});

    if(includes == included_tables::location) {
        ret->location.emplace(result[0][4].as<uint32_t>(), result[0][5].as<uint32_t>(), result[0][6].as<uint32_t>(), result[0][7].as<string>());
    }

    LOG(DEBUG) << NAMEOF(players_repository::get_player) << " found player by name with id " << ret->id;

    return ret;
}

STD_OPTIONAL<player> players_repository::get_player(uint64_t id, included_tables includes,
                                                    unique_ptr<idatabase_transaction> const &transaction) {
    auto result = transaction->execute("SELECT p.id, p.user_id, p.location_id, p.player_name FROM players p WHERE id = " + to_string(id));

    if(result.size() == 0) {
        LOG(DEBUG) << NAMEOF(players_repository::get_player) << " found no player by id " << id;
        return {};
    }

    auto ret = make_optional<player>({result[0][0].as<uint64_t>(), result[0][1].as<uint64_t>(),
                                      result[0][2].as<uint64_t>(), result[0][3].as<string>()});

    LOG(DEBUG) << NAMEOF(players_repository::get_player) << " found player by id with id " << ret->id;

    return ret;
}

vector<player> players_repository::get_players_by_user_id(uint64_t user_id, included_tables includes,
                                                          unique_ptr<idatabase_transaction> const &transaction) {
    pqxx::result result;
    if(includes == included_tables::none) {
        result = transaction->execute("SELECT p.id, p.user_id, p.location_id, p.player_name FROM players p WHERE user_id = " + to_string(user_id));
    } else if (includes == included_tables::location) {
        result = transaction->execute("SELECT p.id, p.user_id, p.location_id, p.player_name, l.x, l.y, m.id, m.map_name FROM players p "
                                      "INNER JOIN locations l ON l.id = p.location_id "
                                      "INNER JOIN maps m ON m.id = l.map_id "
                                      "WHERE p.user_id = " + to_string(user_id));
    } else {
        LOG(ERROR) << NAMEOF(players_repository::get_players_by_user_id) << " included_tables value " << static_cast<int>(includes) << " not implemented";
        throw unexpected_result_error("included_tables value not implemented");
    }

    LOG(DEBUG) << NAMEOF(players_repository::get_players_by_user_id) << " contains " << result.size() << " entries";

    vector<player> players;
    players.reserve(result.size());

    for(auto const & res : result) {
        player plyr{res[0].as<uint64_t>(), res[1].as<uint64_t>(), res[2].as<uint64_t>(), res[3].as<string>()};
        if(includes == included_tables::location) {
            plyr.location.emplace(res[4].as<uint32_t>(), res[5].as<uint32_t>(), res[6].as<uint32_t>(), res[7].as<string>());
        }
        players.push_back(move(plyr));
    }

    return players;
}
