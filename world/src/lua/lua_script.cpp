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

#include <macros.h>
#include "lua_interop.h"
#include "lua_script.h"
#include "easylogging++.h"

using namespace std;
using namespace roa;

lua_script::lua_script(lua_script const &l) noexcept
        : _L(nullptr), _initialised(false), _no_of_runs(0),
          _library_script_text(l._library_script_text), _name(l._name), _script_text(l._script_text),
          _library_chunk_ref(-1), _script_chunk_ref(-1) {
}

lua_script::lua_script(lua_script &&l) noexcept
        : _L(l._L), _initialised(l._initialised), _no_of_runs(l._no_of_runs),
          _library_script_text(l._library_script_text), _script_text(l._script_text),
          _library_chunk_ref(l._library_chunk_ref), _script_chunk_ref(l._script_chunk_ref) {
    l._L = nullptr;
    l._initialised = false;
    l._no_of_runs = 0;
    l._script_chunk_ref = -1;
    l._library_chunk_ref = -1;
}

lua_script::lua_script(std::string library_script, std::string name, std::string script) :
        _L(nullptr), _initialised(false), _no_of_runs(0), _library_script_text(library_script), _name(name),
        _script_text(script), _library_chunk_ref(-1), _script_chunk_ref(-1) {
}

lua_script::~lua_script() {
    if (_initialised) {
        lua_close(_L);
    }
}

bool lua_script::load() noexcept {
    int status;

    if (!_initialised) {
        LOG(TRACE) << NAMEOF(lua_script::lua_script) << " loading library script";
        _L = luaL_newstate();

        luaL_openlibs(_L);

        status = luaL_loadstring(_L, _library_script_text.c_str());
        if (unlikely(status != 0)) {
            LOG(ERROR) << NAMEOF(lua_script::lua_script) << " Couldn't load library script: " << lua_tostring(_L, -1);
            lua_close(_L);
            return false;
        }

        lua_pushvalue(_L, -1);
        _library_chunk_ref = luaL_ref(_L, LUA_REGISTRYINDEX);
    } else {
        LOG(TRACE) << NAMEOF(lua_script::lua_script) << " loading cached library script";
        lua_settop(_L, 0);
        lua_rawgeti(_L, LUA_REGISTRYINDEX, _library_chunk_ref);
    }

    status = lua_pcall(_L, 0, LUA_MULTRET, 0);
    if (unlikely(status != 0)) {
        LOG(ERROR) << NAMEOF(lua_script::lua_script) << " Couldn't run library script: " << lua_tostring(_L, -1);
        lua_close(_L);
        return false;
    }

    if (!_initialised) {
        LOG(TRACE) << NAMEOF(lua_script::lua_script) << " loading script";
        status = luaL_loadstring(_L, _script_text.c_str());
        if (status != 0) {
            LOG(ERROR) << NAMEOF(lua_script::lua_script) << " Couldn't load script: " << lua_tostring(_L, -1);
            lua_close(_L);
            return false;
        }

        lua_pushvalue(_L, -1);
        _script_chunk_ref = luaL_ref(_L, LUA_REGISTRYINDEX);
    } else {
        LOG(TRACE) << NAMEOF(lua_script::lua_script) << " loading cached script";
        lua_rawgeti(_L, LUA_REGISTRYINDEX, _script_chunk_ref);
    }

    _initialised = true;
    return true;
}

bool lua_script::run() noexcept {
    auto result = lua_pcall(_L, 0, LUA_MULTRET, 0);
    if (unlikely(result != 0)) {
        LOG(ERROR) << NAMEOF(script_system::update) << " Failed to run script: " << lua_tostring(_L, -1);
        close();
        return false;
    }
    _no_of_runs++;

    // In testing, after the 10,000th run, lua gave a table overflow error.
    // To prevent this, simply close & reload the script every 1001 times.
    if(_no_of_runs > 1000) {
        close();
    }

    return true;
}

void lua_script::close() noexcept {
    if(_initialised) {
        _no_of_runs = 0;
        _initialised = false;
        lua_close(_L);
    }
}

void lua_script::create_table() const noexcept {
    lua_newtable(_L);
}

void lua_script::create_nested_table(string const &name) const noexcept {
    lua_pushstring(_L, name.c_str());
    lua_newtable(_L);
}

void lua_script::set_global(string const &name) const noexcept {
    lua_setglobal(_L, name.c_str());
}

void lua_script::push_integer(string const &name, int64_t val) const noexcept {
    lua_pushstring(_L, name.c_str());
    lua_pushinteger(_L, val);
    lua_rawset(_L, -3);
}

void lua_script::push_boolean(string const &name, bool val) const noexcept {
    lua_pushstring(_L, name.c_str());
    lua_pushboolean(_L, val);
    lua_rawset(_L, -3);
}

void lua_script::push_string(string const &name, string const &val) const noexcept {
    lua_pushstring(_L, name.c_str());
    lua_pushstring(_L, val.c_str());
    lua_rawset(_L, -3);
}

void lua_script::push_table() const noexcept {
    lua_rawset(_L, -3);
}

std::string lua_script::name() const noexcept {
    return _name;
}
