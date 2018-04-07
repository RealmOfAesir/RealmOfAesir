/*
    Realm of Aesir
    Copyright (C) 2016  Michael de Lang

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

#include <catch.hpp>
#include <iostream>
#include <sstream>
#include <cereal/archives/binary.hpp>
#include <cereal/types/string.hpp>
#include <easylogging++.h>
#include "messages/message.h"
#include "messages/game/send_map_message.h"
#include "exceptions.h"
#include "test_helpers.h"

using namespace std;
using namespace roa;

TEST_CASE("serialize/deserialize send_map_message happy flow") {
    auto new_json_message = test_happy_flow<send_map_message, true, string>(json_send_map_message::id, "map_data"s);
    REQUIRE(new_json_message != nullptr);
    REQUIRE(new_json_message->map_data == "map_data"s);

    auto new_binary_message = test_happy_flow<send_map_message, false, string>(binary_send_map_message::id, "map_data"s);
    REQUIRE(new_binary_message != nullptr);
    REQUIRE(new_binary_message->map_data == "map_data"s);
}
