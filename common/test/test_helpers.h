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

#pragma once

#include "messages/message_sender.h"

namespace roa {
    /**
     * Setup a message of type T, add a sender and initialize with args, serialize and deserialize it, check if the
     * sender converted properly and return a unique ptr to the result
     * @tparam T
     * @tparam UseJson
     * @tparam Args
     * @param id
     * @param args
     * @return
     */
    template<template<bool> class T, bool UseJson, class... Args>
    std::unique_ptr<T<UseJson> const> test_happy_flow(uint32_t id, Args... args) {
        message_sender sender(true, 1, 2, 3);
        T<UseJson> T_inst(sender, args...);
        auto serialized_message = T_inst.serialize();
        REQUIRE(serialized_message.length() > 0);
        auto deserialized_message = message<UseJson>::template deserialize<UseJson>(serialized_message);
        REQUIRE(std::get<0>(deserialized_message) == id);
        REQUIRE(std::get<1>(deserialized_message) != nullptr);

        REQUIRE(std::get<1>(deserialized_message)->sender.is_for_client == true);
        REQUIRE(std::get<1>(deserialized_message)->sender.client_id == 1);
        REQUIRE(std::get<1>(deserialized_message)->sender.server_origin_id == 2);
        REQUIRE(std::get<1>(deserialized_message)->sender.server_destination_id == 3);

        return std::unique_ptr<T<UseJson> const>(dynamic_cast<T<UseJson> const *>(std::get<1>(deserialized_message).release()));
    }
}