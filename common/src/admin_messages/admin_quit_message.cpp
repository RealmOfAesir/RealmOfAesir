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

#include <sstream>
#include <cereal/archives/binary.hpp>
#include <cereal/archives/json.hpp>
#include "admin_quit_message.h"

using namespace std;
using namespace roa;

template <bool UseJson>
admin_quit_message<UseJson>::admin_quit_message(message_sender sender) noexcept
        : message<UseJson>(sender) {

}

template <bool UseJson>
admin_quit_message<UseJson>::~admin_quit_message() {

}

template <bool UseJson>
string const admin_quit_message<UseJson>::serialize() const {
    stringstream ss;
    {
        typename conditional<UseJson, cereal::JSONOutputArchive, cereal::BinaryOutputArchive>::type archive(ss);

        archive(cereal::make_nvp("id", admin_quit_message<UseJson>::id), cereal::make_nvp("sender", this->sender));
    }
    return ss.str();
}

template<bool UseJson> uint32_t constexpr admin_quit_message<UseJson>::id;
template class admin_quit_message<false>;
template class admin_quit_message<true>;