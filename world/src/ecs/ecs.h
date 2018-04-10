/*
    RealmOfAesirWorld
    Copyright (C) 2017  Michael de Lang

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

#include <entt/entity/registry.hpp>
#include "components/ai_component.h"
#include "components/character_component.h"
#include "components/effect_component.h"
#include "components/map_component.h"
#include "components/player_component.h"
#include "components/position_component.h"
#include "components/stat_component.h"
#include "components/tile_component.h"

namespace roa {
    using EntityManager = entt::Registry<std::uint64_t>;

    using TimeDelta = uint32_t;
}