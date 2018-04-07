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
#include <cereal/details/helpers.hpp>
#include "messages/message.h"
#include "admin_messages/admin_quit_message.h"
#include "test_helpers.h"

using namespace std;
using namespace roa;

TEST_CASE("serialize/deserialize admin_quit_message happy flow") {
    auto new_json_message = test_happy_flow<admin_quit_message, true>(json_quit_message::id);
    REQUIRE(new_json_message != nullptr);

    auto new_binary_message = test_happy_flow<admin_quit_message, false>(binary_quit_message::id);
    REQUIRE(new_binary_message != nullptr);
}