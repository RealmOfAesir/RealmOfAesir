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

#include "world_create_character_handler.h"
#include <easylogging++.h>
#include <messages/game/send_map_message.h>
#include <messages/error_response_message.h>
#include <macros.h>
#include <events/player_events/enter_map_event.h>

using namespace std;
using namespace roa;

static inline binary_error_response_message create_error_message(uint64_t client_id, uint32_t server_id, int error_no, string error_str) {
    return binary_error_response_message {
            {false, client_id, server_id, 0 /* ANY */},
            error_no,
            error_str
    };
}

world_create_character_handler::world_create_character_handler(Config& config, moodycamel::ReaderWriterQueue<std::shared_ptr<player_event>>& player_event_queue,
                                                               iplayers_repository& players_repository, std::shared_ptr<ikafka_producer<false>> producer)
        : _config(config), _player_event_queue(player_event_queue), _players_repository(players_repository), _producer(producer) {

}

void world_create_character_handler::handle_message(unique_ptr<binary_message const> const &msg) {
    // send message to gateway id instead of backend id
    string queue_name = "server-" + to_string(msg->sender.server_destination_id);

    // TODO send error message to backend as well as gateway, so the backend can undo the create_character in its db

    try {
        if (auto casted_msg = dynamic_cast<create_character_message<false> const * const>(msg.get())) {
            auto transaction = _players_repository.create_transaction();

            auto existing_player = _players_repository.get_player(casted_msg->player_name, included_tables::none, get<1>(transaction));

            if(existing_player) {
                LOG(ERROR) << NAMEOF(world_create_character_handler::handle_message) << " Player already exists but received create message from backend";
                _producer->enqueue_message(queue_name, create_error_message(msg->sender.client_id, _config.server_id, -1, "Something went wrong."));
                return;
            }

            player plyr{0, casted_msg->user_id, 0, casted_msg->player_name};
            // TODO
            //_players_repository.insert_player_at_start_location(plyr, get<1>(transaction));
            get<1>(transaction)->commit();

            _player_event_queue.enqueue(make_shared<enter_world_event>(msg->sender.client_id, msg->sender.server_destination_id, plyr));
        } else {
            LOG(ERROR) << NAMEOF(world_create_character_handler::handle_message) << " Couldn't cast message to create_character_message";
            _producer->enqueue_message(queue_name, create_error_message(msg->sender.client_id, _config.server_id, -1, "Something went wrong."));
        }
    } catch (std::runtime_error const &e) {
        LOG(ERROR) << NAMEOF(world_create_character_handler::handle_message) << " error: " << typeid(e).name() << "-" << e.what();
        _producer->enqueue_message(queue_name, create_error_message(msg->sender.client_id, _config.server_id, -1, "Something went wrong."));
    }
}

uint32_t constexpr world_create_character_handler::message_id;