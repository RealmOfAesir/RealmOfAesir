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

#include "chat_receive_message.h"

#include <cereal/archives/binary.hpp>
#include <cereal/archives/json.hpp>
#include <cereal/types/string.hpp>
#include <sstream>

using namespace std;
using namespace roa;

template <bool UseJson>
chat_receive_message<UseJson>::chat_receive_message(message_sender sender, std::string from_username, std::string target, std::string message) noexcept
        : message<UseJson>(sender), from_username(from_username), target(target), message(message) {}

template <bool UseJson>
chat_receive_message<UseJson>::~chat_receive_message() noexcept {

}

template <bool UseJson>
string const chat_receive_message<UseJson>::serialize() const {
    stringstream ss;
    {
        typename conditional<UseJson, cereal::JSONOutputArchive, cereal::BinaryOutputArchive>::type archive(ss);

        archive(cereal::make_nvp("id", chat_receive_message<UseJson>::id),
                cereal::make_nvp("sender", this->sender),
                cereal::make_nvp("from_username", this->from_username),
                cereal::make_nvp("target", this->target),
                cereal::make_nvp("message", this->message));
    }

    return ss.str();
}

template<bool UseJson> uint32_t constexpr chat_receive_message<UseJson>::id;
template class chat_receive_message<false>;
template class chat_receive_message<true>;