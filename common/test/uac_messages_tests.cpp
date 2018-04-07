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
#include <messages/user_access_control/register_message.h>
#include <messages/user_access_control/register_response_message.h>
#include <messages/user_access_control/logout_message.h>
#include <messages/user_access_control/create_character_message.h>
#include <messages/user_access_control/play_character_message.h>
#include <messages/user_access_control/get_characters_message.h>
#include <messages/user_access_control/get_characters_response_message.h>
#include <messages/error_response_message.h>
#include "messages/message.h"
#include "messages/user_access_control/login_message.h"
#include "messages/user_access_control/login_response_message.h"
#include "exceptions.h"
#include "test_helpers.h"

using namespace std;
using namespace roa;

TEST_CASE("serialize/deserialize login_message happy flow") {
    auto new_json_message = test_happy_flow<login_message, true, string, string, string>(json_login_message::id, "user"s, "pass"s, "ip"s);
    REQUIRE(new_json_message != nullptr);
    REQUIRE(new_json_message->username == "user");
    REQUIRE(new_json_message->password == "pass");
    REQUIRE(new_json_message->ip == "ip");

    auto new_binary_message = test_happy_flow<login_message, false, string, string, string>(binary_login_message::id, "user"s, "pass"s, "ip"s);
    REQUIRE(new_binary_message != nullptr);
    REQUIRE(new_binary_message->username == "user");
    REQUIRE(new_binary_message->password == "pass");
    REQUIRE(new_binary_message->ip == "ip");
}

TEST_CASE("deserialize garbage") {
    REQUIRE_THROWS_AS(message<false>::template deserialize<false>(""), serialization_exception&);
    REQUIRE_THROWS_AS(message<false>::template deserialize<false>("garbage"), cereal::Exception&);
}

TEST_CASE("serialize/deserialize login_message errors") {
    {
        stringstream ss;
        cereal::BinaryOutputArchive archive(ss);
        archive((uint32_t) json_login_message::id, string("garbage"));
        ss.flush();
        REQUIRE_THROWS_AS(message<false>::template deserialize<false>(ss.str()), cereal::Exception &);
    }
    {
        stringstream ss;
        cereal::BinaryOutputArchive archive(ss);
        message_sender sender(true, 1, 2, 3);
        archive((uint32_t) json_login_message::id, sender, string("garbage"));
        ss.flush();
        REQUIRE_THROWS_AS(message<false>::template deserialize<false>(ss.str()), cereal::Exception &);
    }
}

TEST_CASE("serialize/deserialize login_response_message happy flow") {
    auto new_json_message = test_happy_flow<login_response_message, true, int, uint64_t>(json_login_response_message::id, 2, 3);
    REQUIRE(new_json_message != nullptr);
    REQUIRE(new_json_message->admin_status == 2);
    REQUIRE(new_json_message->user_id == 3);

    auto new_binary_message = test_happy_flow<login_response_message, false, int, uint64_t>(binary_login_response_message::id, 2, 3);
    REQUIRE(new_binary_message != nullptr);
    REQUIRE(new_binary_message->admin_status == 2);
    REQUIRE(new_binary_message->user_id == 3);
}

TEST_CASE("serialize/deserialize register_message happy flow") {
    auto new_json_message = test_happy_flow<register_message, true, string, string, string, string>(json_register_message::id, "user"s, "pass"s, "email"s, "ip"s);
    REQUIRE(new_json_message != nullptr);
    REQUIRE(new_json_message->username == "user");
    REQUIRE(new_json_message->password == "pass");
    REQUIRE(new_json_message->email == "email");
    REQUIRE(new_json_message->ip == "ip");

    auto new_binary_message = test_happy_flow<register_message, false, string, string, string, string>(binary_register_message::id, "user"s, "pass"s, "email"s, "ip"s);
    REQUIRE(new_binary_message != nullptr);
    REQUIRE(new_binary_message->username == "user");
    REQUIRE(new_binary_message->password == "pass");
    REQUIRE(new_binary_message->email == "email");
    REQUIRE(new_binary_message->ip == "ip");
}

TEST_CASE("serialize/deserialize register_response_message happy flow") {
    auto new_json_message = test_happy_flow<register_response_message, true, int, uint64_t>(json_register_response_message::id, 2, 3);
    REQUIRE(new_json_message != nullptr);
    REQUIRE(new_json_message->admin_status == 2);
    REQUIRE(new_json_message->user_id == 3);

    auto new_binary_message = test_happy_flow<register_response_message, false, int, uint64_t>(binary_register_response_message::id, 2, 3);
    REQUIRE(new_binary_message != nullptr);
    REQUIRE(new_binary_message->admin_status == 2);
    REQUIRE(new_binary_message->user_id == 3);
}

TEST_CASE("serialize/deserialize logout_message happy flow") {
    auto new_json_message = test_happy_flow<logout_message, true>(json_logout_message::id);
    REQUIRE(new_json_message != nullptr);

    auto new_binary_message = test_happy_flow<logout_message, false>(binary_logout_message::id);
    REQUIRE(new_binary_message != nullptr);
}

TEST_CASE("serialize/deserialize create_character_message happy flow") {
    auto new_json_message = test_happy_flow<create_character_message, true>(json_create_character_message::id, 3, "player"s);
    REQUIRE(new_json_message != nullptr);
    REQUIRE(new_json_message->user_id == 3);
    REQUIRE(new_json_message->player_name == "player"s);

    auto new_binary_message = test_happy_flow<create_character_message, false>(binary_create_character_message::id, 3, "player"s);
    REQUIRE(new_binary_message != nullptr);
    REQUIRE(new_binary_message->user_id == 3);
    REQUIRE(new_binary_message->player_name == "player"s);
}

TEST_CASE("serialize/deserialize create_character_response_message happy flow") {
    auto new_json_message = test_happy_flow<error_response_message, true, int, string>(json_error_response_message::id, 1, "test"s);
    REQUIRE(new_json_message != nullptr);
    REQUIRE(new_json_message->error_number == 1);
    REQUIRE(new_json_message->error_str == "test");

    auto new_binary_message = test_happy_flow<error_response_message, false, int, string>(binary_error_response_message::id, 1, "test"s);
    REQUIRE(new_binary_message != nullptr);
    REQUIRE(new_binary_message->error_number == 1);
    REQUIRE(new_binary_message->error_str == "test");
}

TEST_CASE("serialize/deserialize play_character_message happy flow") {
    auto new_json_message = test_happy_flow<play_character_message, true>(json_play_character_message::id, 3, "player"s);
    REQUIRE(new_json_message != nullptr);
    REQUIRE(new_json_message->user_id == 3);
    REQUIRE(new_json_message->player_name == "player"s);

    auto new_binary_message = test_happy_flow<play_character_message, false>(binary_play_character_message::id, 3, "player"s);
    REQUIRE(new_binary_message != nullptr);
    REQUIRE(new_binary_message->user_id == 3);
    REQUIRE(new_binary_message->player_name == "player"s);
}

TEST_CASE("serialize/deserialize get_characters_message happy flow") {
    auto new_json_message = test_happy_flow<get_characters_message, true>(json_get_characters_message::id, 3);
    REQUIRE(new_json_message != nullptr);
    REQUIRE(new_json_message->user_id == 3);

    auto new_binary_message = test_happy_flow<get_characters_message, false>(binary_get_characters_message::id, 3);
    REQUIRE(new_binary_message != nullptr);
    REQUIRE(new_binary_message->user_id == 3);
}

TEST_CASE("serialize/deserialize get_characters_response_message happy flow") {
    auto new_json_message = test_happy_flow<get_characters_response_message, true, vector<player_response>, string>(json_get_characters_response_message::id, {{3, "player_name"s, "map_name"s}, {4, "player2_name"s, "map2_name"s}}, "test"s);
    REQUIRE(new_json_message != nullptr);
    REQUIRE(new_json_message->players.size() == 2);
    REQUIRE(new_json_message->players[0].player_id == 3);
    REQUIRE(new_json_message->players[0].map_name == "map_name"s);
    REQUIRE(new_json_message->players[0].player_name == "player_name"s);
    REQUIRE(new_json_message->players[1].player_id == 4);
    REQUIRE(new_json_message->players[1].map_name == "map2_name"s);
    REQUIRE(new_json_message->players[1].player_name == "player2_name"s);
    REQUIRE(new_json_message->world_name == "test"s);

    auto new_binary_message = test_happy_flow<get_characters_response_message, false, vector<player_response>, string>(binary_get_characters_response_message::id, {{3, "player_name"s, "map_name"s}}, "test"s);
    REQUIRE(new_binary_message != nullptr);
    REQUIRE(new_binary_message->players.size() == 1);
    REQUIRE(new_binary_message->players[0].player_id == 3);
    REQUIRE(new_binary_message->players[0].map_name == "map_name"s);
    REQUIRE(new_binary_message->players[0].player_name == "player_name"s);
    REQUIRE(new_binary_message->world_name == "test"s);
}