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

#include "error_response_message.h"

#include <cereal/archives/binary.hpp>
#include <cereal/archives/json.hpp>
#include <cereal/types/string.hpp>
#include <sstream>

using namespace std;
using namespace roa;

template <bool UseJson>
error_response_message<UseJson>::error_response_message(message_sender sender, int error_number, std::string error_str) noexcept
        : message<UseJson>(sender), error_number(error_number), error_str(error_str) {}

template <bool UseJson>
error_response_message<UseJson>::~error_response_message() noexcept {

}

template <bool UseJson>
string const error_response_message<UseJson>::serialize() const {
    stringstream ss;
    {
        typename conditional<UseJson, cereal::JSONOutputArchive, cereal::BinaryOutputArchive>::type archive(ss);

        archive(cereal::make_nvp("id", error_response_message<UseJson>::id),
                cereal::make_nvp("sender", this->sender),
                cereal::make_nvp("error_number", this->error_number),
                cereal::make_nvp("error_str", this->error_str));
    }

    return ss.str();
}

template<bool UseJson> uint32_t constexpr error_response_message<UseJson>::id;
template class error_response_message<false>;
template class error_response_message<true>;