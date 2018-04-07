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

#include "enter_map_event.h"
#include <easylogging++.h>
#include <macros.h>
#include <messages/error_response_message.h>
#include <cereal/archives/json.hpp>
#include <helpers/tiled_converter.h>
#include <messages/game/send_map_message.h>
#include <experimental/optional>
#include <messages/message.h>

using namespace std;
using namespace experimental;
using namespace roa;

std::unique_ptr<binary_message> enter_world_event::process(EntityManager &es) const {
    return player_event::process(es);
}
