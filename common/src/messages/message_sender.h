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

namespace roa {
    class message_sender {
    public:
        message_sender();
        message_sender(bool is_for_client, uint64_t client_id, uint32_t server_origin_id, uint32_t server_destination_id);
        message_sender(message_sender&&) = default;
        constexpr message_sender(message_sender const &) = default;

        template<class Archive>
        void serialize(Archive & archive);

        // if the message containing this sender is intended for a client
        // usually this means that the message is sent to a gateway and passed on the a client
        bool is_for_client;
        uint64_t client_id;
        uint32_t server_origin_id;
        uint32_t server_destination_id;

    };
}