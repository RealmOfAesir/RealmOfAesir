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

#include "get_characters_response_message.h"

#include <cereal/archives/binary.hpp>
#include <cereal/archives/json.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/vector.hpp>
#include <sstream>

using namespace std;
using namespace roa;


template <bool UseJson>
get_characters_response_message<UseJson>::get_characters_response_message(message_sender sender, std::vector<player_response> players, std::string world_name) noexcept
        : message<UseJson>(sender), players(players), world_name(world_name) {}

template <bool UseJson>
get_characters_response_message<UseJson>::~get_characters_response_message() noexcept {

}

template <bool UseJson>
string const get_characters_response_message<UseJson>::serialize() const {
    stringstream ss;
    {
        typename conditional<UseJson, cereal::JSONOutputArchive, cereal::BinaryOutputArchive>::type archive(ss);

        archive(cereal::make_nvp("id", get_characters_response_message<UseJson>::id),
                cereal::make_nvp("sender", this->sender),
                cereal::make_nvp("players", this->players),
                cereal::make_nvp("world_name", this->world_name));
    }

    return ss.str();
}

template<bool UseJson> uint32_t constexpr get_characters_response_message<UseJson>::id;
template class get_characters_response_message<false>;
template class get_characters_response_message<true>;