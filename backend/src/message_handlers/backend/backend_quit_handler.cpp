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

#include <macros.h>
#include <easylogging++.h>
#include "backend_quit_handler.h"

using namespace roa;

backend_quit_handler::backend_quit_handler(std::atomic<bool> *quit) : _quit(quit) {

}

void backend_quit_handler::handle_message(std::unique_ptr<binary_message const> const &msg) {
    LOG(DEBUG) << NAMEOF(backend_quit_handler::handle_message) << " Received quit";
    *this->_quit = true;
}

uint32_t constexpr backend_quit_handler::message_id;