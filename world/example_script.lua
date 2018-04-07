--[[
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
--]]

local roa = require("roa")

local entity = roa.get_triggering_entity()
local self_script = roa.get_self_script()

if entity.class_type == roa.class_type.Map then
    self_script.destroy()
elseif entity.class_type == roa.class_type.Tile then
    local map_entity = entity.get_map()
    local max_tile_id = map_entity.get_max_tile_id()
    if entity.get_tile_id() >= max_tile_id then
        roa.create_script("tile_id_downwards", entity.get_id(), 200, 200, roa.script_trigger_type.Looped, false)
        self_script.destroy()
    else
        entity.set_tile_id(entity.get_tile_id() + 1)
        if entity.get_tile_id() >= max_tile_id then
            roa.create_script("tile_id_downwards", entity.get_id(), 200, 200, roa.script_trigger_type.Looped, false)
            self_script.destroy()
        end
    end
end
