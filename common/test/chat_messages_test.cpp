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
#include "messages/chat/chat_send_message.h"
#include "messages/chat/chat_receive_message.h"
#include "exceptions.h"
#include "test_helpers.h"

using namespace std;
using namespace roa;

TEST_CASE("serialize/deserialize chat_send_message happy flow") {
auto new_json_message = test_happy_flow<chat_send_message, true, string, string, string>(json_chat_send_message::id, "user"s, "target"s, "msg"s);
REQUIRE(new_json_message != nullptr);
REQUIRE(new_json_message->from_username == "user"s);
REQUIRE(new_json_message->target == "target"s);
REQUIRE(new_json_message->message == "msg"s);

auto new_binary_message = test_happy_flow<chat_send_message, false, string, string, string>(binary_chat_send_message::id, "user"s, "target"s, "msg"s);
REQUIRE(new_binary_message != nullptr);
REQUIRE(new_binary_message->from_username == "user"s);
REQUIRE(new_binary_message->target == "target"s);
REQUIRE(new_binary_message->message == "msg"s);
}

TEST_CASE("serialize/deserialize chat_receive_message happy flow") {
auto new_json_message = test_happy_flow<chat_receive_message, true, string, string, string>(json_chat_receive_message::id, "user"s, "target"s, "msg"s);
REQUIRE(new_json_message != nullptr);
REQUIRE(new_json_message->from_username == "user"s);
REQUIRE(new_json_message->target == "target"s);
REQUIRE(new_json_message->message == "msg"s);

auto new_binary_message = test_happy_flow<chat_receive_message, false, string, string, string>(binary_chat_receive_message::id, "user"s, "target"s, "msg"s);
REQUIRE(new_binary_message != nullptr);
REQUIRE(new_binary_message->from_username == "user"s);
REQUIRE(new_binary_message->target == "target"s);
REQUIRE(new_binary_message->message == "msg"s);
}
