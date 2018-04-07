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

#include "../message.h"

namespace roa {
    template <bool UseJson>
    class get_characters_message : public message<UseJson> {
    public:
        get_characters_message(message_sender sender, uint64_t user_id) noexcept;

        ~get_characters_message() noexcept override;

        std::string const serialize() const override;

        uint64_t user_id;
        static constexpr uint32_t id = 12;
    };

    using json_get_characters_message = get_characters_message<true>;
    using binary_get_characters_message = get_characters_message<false>;
}