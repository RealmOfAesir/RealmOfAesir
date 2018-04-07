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

#include <string>
#include <memory>
#include <tuple>
#include <sstream>

#include "message_sender.h"

namespace roa {
    template <bool UseJson>
    class message {
    public:
        message(message_sender sender);
        virtual ~message() noexcept {};

        /**
         * serialize to binary if UseJson = false, Json if UseJson = true.
         * @return serialized string
         * @throws whatever cereal might throw
         */
        virtual std::string const serialize() const = 0;

        /**
         * deserialize string to message
         * @tparam UseJsonAsReturnType
         * @param buffer
         * @return tuple of message type and a unique pointer to the message
         * @throws serialization_exception if buffer is empty or if message type is not recognized or cereal exception if message incomplete
         */
        template <bool UseJsonAsReturnType>
        static std::tuple<uint32_t, std::unique_ptr<message<UseJsonAsReturnType> const>> deserialize(std::string buffer);

        message_sender sender;
    };

    using json_message = message<true>;
    using binary_message = message<false>;
}