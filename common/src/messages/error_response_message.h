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

#include "message.h"

#define BANNED_ERROR_CODE -2
#define GENERIC_ERROR_CODE -1

namespace roa {
    template <bool UseJson>
    class error_response_message : public message<UseJson> {
    public:
        error_response_message(message_sender sender, int error_number, std::string error_str) noexcept;

        ~error_response_message() noexcept override;

        std::string const serialize() const override;

        int error_number;
        std::string error_str;
        static constexpr uint32_t id = 8;
    };

    using json_error_response_message = error_response_message<true>;
    using binary_error_response_message = error_response_message<false>;
}