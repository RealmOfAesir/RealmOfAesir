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

local ffi = require("ffi")

ffi.cdef[[
void roa_log(int level, char const * message);
void set_tile_properties(uint32_t id, uint16_t tile_id);
void destroy_script(uint32_t id);
void create_script(char const * name, uint32_t id, uint32_t execute_in_ms, uint32_t loop_every_ms, uint32_t trigger_type, bool debug);
]]

local roa = {}

roa.entity_types = {
    Tile = 1,
    Character = 2
}

roa.debug_level = {
    Debug = 0,
    Info = 1,
    Warning = 2,
    Error = 3
}

roa.class_type = {
    Settings = 0,
    Map = 1,
    Tile = 2,
    Script = 3
}

roa.script_trigger_type = {
    Once = 0,
    Looped = 1,
    Chat = 2,
    Movement = 3
}

function SettingsClass(debug, library_debug)
    local s = {}
    s.class_type = roa.class_type.Settings
    s.debug = debug
    s.library_debug = library_debug
    return s
end

function MapClass(id, first_tile_id, max_tile_id, width, height)
    local s = {}
    s.class_type = roa.class_type.Map

    local id = id
    local first_tile_id = first_tile_id
    local max_tile_id = max_tile_id
    local width = width
    local height = height

    function s.get_id()
        return id
    end

    function s.get_first_tile_id()
        return first_tile_id
    end

    function s.get_max_tile_id()
        return max_tile_id
    end

    function s.get_width()
        return width
    end

    function s.get_height()
        return height
    end

    return s
end

function TileClass(id, tile_id, map)
    if map == nil then
        error("tile requires a map")
    end

    local s = {}
    s.class_type = roa.class_type.Tile
    local id = id
    local tile_id = tile_id
    local map = map

    function s.set_tile_id(new_id)
        if roa_settings["library_debug"] then
            ffi.C.roa_log(0, "setting tile " .. id .. " to tile_id " .. new_id)
        end

        ffi.C.set_tile_properties(id, new_id)
        tile_id = new_id
    end

    function s.get_tile_id()
        return tile_id
    end

    function s.get_id()
        return id
    end

    function s.get_map()
        return map
    end

    return s
end

function ScriptClass(id, name)
    local s = {}
    s.class_type = roa.class_type.Script

    local id = id
    local name = name

    function s.get_id()
        return id
    end

    function s.get_name()
        return name
    end

    function s.destroy()
        ffi.C.destroy_script(id)
        id = -1
    end

    return s
end

function roa.log(level, message)
    if level == roa.debug_level.Debug then
        ffi.C.roa_log(0, message)
    elseif level == roa.debug_level.Info then
        ffi.C.roa_log(1, message)
    elseif level == roa.debug_level.Warning then
        ffi.C.roa_log(2, message)
    elseif level == roa.debug_level.Error then
        ffi.C.roa_log(3, message)
    else
        ffi.C.roa_log(3, debug.traceback())
        error("enum type debug_level expected")
    end
end

function roa.get_triggering_entity()
    if(roa_entity == nil or roa_map == nil) then
        roa.log(roa.debug_level.Info, "Entity or map nil")
        return nil
    end

    if(roa_entity["type"] == roa.entity_types.Tile) then
        local map = MapClass(roa_map["id"], roa_map["first_tile_id"], roa_map["max_tile_id"], roa_map["width"], roa_map["height"])
        return TileClass(roa_entity["id"], roa_entity["tile_id"], map)
    elseif roa_settings["library_debug"] then
        roa.log(roa.debug_level.Debug, "type unknown")
    end

    return nil
end

function roa.get_self_script()
    return ScriptClass(roa_script["id"], roa_script["name"])
end

function roa.get_settings()
    if(roa_settings == nil) then
        ffi.C.roa_log(3, debug.traceback())
        error("no settings available")
    end

    return SettingsClass(roa_settings["debug"], roa_settings["library_debug"])
end

function roa.display_stack_parameters()
    roa.log(roa.debug_level.Debug, "settings: ")
    for k, v in pairs( roa_settings ) do
        roa.log(roa.debug_level.Debug, string.format("%s-%s", k, v))
    end

    roa.log(roa.debug_level.Debug, "entity: ")
    for k, v in pairs( roa_entity ) do
        roa.log(roa.debug_level.Debug, string.format("%s-%s", k, v))
    end

    roa.log(roa.debug_level.Debug, "map: ")
    for k, v in pairs( roa_map ) do
        roa.log(roa.debug_level.Debug, string.format("%s-%s", k, v))
    end
end

function roa.create_script(name, entity_id, execute_in_ms, loop_every_ms, trigger_type, debug)
    ffi.C.create_script(name, entity_id, execute_in_ms, loop_every_ms, trigger_type, debug)
end

package.loaded["roa"] = roa