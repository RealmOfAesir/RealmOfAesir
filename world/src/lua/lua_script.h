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

#include <iostream>

struct lua_State;

namespace roa {
    class lua_script {
    public:
        lua_script(std::string library_script, std::string name, std::string script);
        lua_script(lua_script const &l) noexcept;
        lua_script(lua_script&& l) noexcept;
        ~lua_script() noexcept;

        bool load() noexcept;
        bool run() noexcept;
        void close() noexcept;

        void create_table() const noexcept;
        void create_nested_table(std::string const &name) const noexcept;
        void set_global(std::string const &name) const noexcept;

        void push_integer(std::string const &name, int64_t val) const noexcept;
        void push_boolean(std::string const &name, bool val) const noexcept;
        void push_string(std::string const &name, std::string const &val) const noexcept;
        void push_table() const noexcept;

        std::string name() const noexcept;
    private:
        lua_State *_L;
        bool _initialised;
        uint32_t _no_of_runs;
        std::string _library_script_text;
        std::string _name;
        std::string _script_text;
        int _library_chunk_ref;
        int _script_chunk_ref;
    };
}