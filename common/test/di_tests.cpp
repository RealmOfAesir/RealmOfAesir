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
#include <roa_di.h>
#include <easylogging++.h>
#include "kafka_producer.h"
#include "kafka_consumer.h"

using namespace std;
using namespace roa;

TEST_CASE("di test") {
    auto injector = create_common_di_injector();
    auto producer = injector.create<unique_ptr<kafka_producer<false>>>();
    auto producer2 = injector.create<unique_ptr<kafka_producer<true>>>();
    auto consumer = injector.create<unique_ptr<kafka_consumer<false>>>();
    auto consumer2 = injector.create<unique_ptr<kafka_consumer<true>>>();
    REQUIRE(producer != nullptr);
    REQUIRE_THROWS(producer->poll(1));
    REQUIRE(producer2 != nullptr);
    REQUIRE_THROWS(producer2->poll(1));
    REQUIRE(consumer != nullptr);
    REQUIRE_THROWS(consumer->try_get_message(1));
    REQUIRE(consumer2 != nullptr);
    REQUIRE_THROWS(consumer2->try_get_message(1));
}