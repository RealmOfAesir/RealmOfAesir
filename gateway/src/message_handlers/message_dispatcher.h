/*
    Realm of Aesir Backend
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

#include <unordered_map>
#include <vector>
#include "src/user_connection.h"
#include <messages/message.h>
#include <custom_optional.h>

namespace roa {

    template <bool UseJson>
    class imessage_handler {
    public:
        virtual ~imessage_handler() = default;
        virtual void handle_message(std::unique_ptr<message<UseJson> const> const &msg, STD_OPTIONAL<std::reference_wrapper<user_connection>> connection) = 0;
    };

    template <bool UseJson>
    class message_dispatcher {
    public:
        message_dispatcher() : _handlers() {}

        template <template <bool> class handler, class... Args>
        void register_handler(Args... args) {
            // https://stackoverflow.com/questions/15783342/should-i-use-c11-emplace-back-with-pointers-containters
            // We're sticking with push_back(make_unique)
            _handlers[handler<UseJson>::message_id].push_back(std::make_unique<handler<UseJson>>(args...));
        }

        template <class handler, class... Args>
        void register_handler(Args... args) {
            _handlers[handler::message_id].push_back(std::make_unique<handler>(args...));
        }

        void trigger_handler(std::tuple<uint32_t, std::unique_ptr<message<UseJson> const>> const &msg, STD_OPTIONAL<std::reference_wrapper<user_connection>> connection) {
            auto iterator = _handlers.find(std::get<0>(msg));

            if(iterator == std::end(_handlers)) {
                return;
            }

            for(auto &msg_handler : iterator->second) {
                msg_handler->handle_message(std::get<1>(msg), connection);
            }
        }
    private:
        std::unordered_map<uint32_t, std::vector<std::unique_ptr<imessage_handler<UseJson>>>> _handlers;
    };
}