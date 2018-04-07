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

#include "player_response.h"

#include <cereal/archives/binary.hpp>
#include <cereal/archives/json.hpp>
#include <cereal/types/string.hpp>

using namespace std;
using namespace roa;

template<class Archive>
void player_response::serialize(Archive &archive) {
    archive(cereal::make_nvp("player_id", this->player_id),
            cereal::make_nvp("player_name", this->player_name),
            cereal::make_nvp("map_name", this->map_name));
}

template void player_response::serialize(cereal::BinaryInputArchive &archive);
template void player_response::serialize(cereal::JSONInputArchive &archive);
template void player_response::serialize(cereal::BinaryOutputArchive &archive);
template void player_response::serialize(cereal::JSONOutputArchive &archive);