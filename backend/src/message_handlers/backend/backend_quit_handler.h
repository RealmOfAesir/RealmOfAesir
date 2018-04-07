/*
    Realm of Aesir backend
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

#include "../message_dispatcher.h"
#include <atomic>
#include <admin_messages/admin_quit_message.h>

namespace roa {
    class backend_quit_handler : public imessage_handler<false> {
    public:
        explicit backend_quit_handler(std::atomic<bool> *quit);
        ~backend_quit_handler() override = default;

        void handle_message(std::unique_ptr<binary_message const> const &msg) override;

        static constexpr uint32_t message_id = admin_quit_message<false>::id;
    private:
        std::atomic<bool> *_quit;
    };
}
