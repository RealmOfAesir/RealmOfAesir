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

#include <lua.hpp>
#include <iostream>
#include <ecs/components/script_component.h>
#include "lua_script.h"
#include <memory>

#define USE_LOCAL_FILES 1

namespace roa {
    class iscripts_repository;
    class script;

    lua_script load_script_with_libraries(std::string name, std::string script);
    void set_library_script(std::string script);
    void set_scripts_repository(std::shared_ptr<iscripts_repository> scripts_repo);
    std::shared_ptr<script> load_script(std::string name);


}