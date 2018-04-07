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

#include "world_get_characters_handler.h"
#include <easylogging++.h>
#include <messages/user_access_control/get_characters_response_message.h>
#include <messages/error_response_message.h>
#include <macros.h>

using namespace std;
using namespace roa;

static inline binary_get_characters_response_message create_message(uint64_t client_id, uint32_t server_id, std::string world_name, vector<player> &players) {
    vector<player_response> responses;

    for(auto& plyr : players) {
        responses.emplace_back(plyr.id, plyr.name, plyr.location->map_name);
    }

    return binary_get_characters_response_message {
            {false, client_id, server_id, 0 /* ANY */},
            responses,
            world_name
    };
}

static inline binary_error_response_message create_error_message(uint64_t client_id, uint32_t server_id, int error_no, string error_str) {
    return binary_error_response_message {
            {false, client_id, server_id, 0 /* ANY */},
            error_no,
            error_str
    };
}

world_get_characters_handler::world_get_characters_handler(Config& config,
                                                           iplayers_repository& players_repository, std::shared_ptr<ikafka_producer<false>> producer)
        : _config(config), _players_repository(players_repository), _producer(producer) {

}

void world_get_characters_handler::handle_message(unique_ptr<binary_message const> const &msg) {
    string queue_name = "server-" + to_string(msg->sender.server_origin_id);

    try {
        if (auto casted_msg = dynamic_cast<get_characters_message<false> const * const>(msg.get())) {
            auto transaction = _players_repository.create_transaction();

            auto players = _players_repository.get_players_by_user_id(casted_msg->user_id, included_tables::location,
                                                                      get<1>(transaction));

            LOG(INFO) << NAMEOF(world_get_characters_handler::handle_message) << " found " << players.size() << " players for user_id " << casted_msg->user_id << " - queue: " << queue_name;
            if(players.size() > 0) {
                _producer->enqueue_message(queue_name, create_message(msg->sender.client_id, _config.server_id, _config.world_name, players));
            }
        } else {
            LOG(ERROR) << NAMEOF(world_get_characters_handler::handle_message) << " Couldn't cast message to get_characters_message";
            _producer->enqueue_message(queue_name, create_error_message(msg->sender.client_id, _config.server_id, -1, "Something went wrong."));
        }
    } catch (std::runtime_error const &e) {
        LOG(ERROR) << NAMEOF(world_get_characters_handler::handle_message) << " error: " << typeid(e).name() << "-" << e.what();
        _producer->enqueue_message(queue_name, create_error_message(msg->sender.client_id, _config.server_id, -1, "Something went wrong."));
    }
}

uint32_t constexpr world_get_characters_handler::message_id;