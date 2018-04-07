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
#include <repositories/scripts_repository.h>
#include "lua_interop.h"
#include "easylogging++.h"

using namespace std;

namespace roa {
    string library_script;
    shared_ptr<iscripts_repository> scripts_repo;

    lua_script load_script_with_libraries(std::string name, std::string script) {
        return lua_script(library_script, name, script);
    }

    void set_library_script(std::string script) {
        library_script = script;
    }

    void set_scripts_repository(shared_ptr<iscripts_repository> scripts_repository) {
        scripts_repo = scripts_repository;
    }

    shared_ptr<script> load_script(string name) {
#ifdef USE_LOCAL_FILES
        LOG(TRACE) << NAMEOF(load_script) << " loading \"scripts/" + name + ".lua\"";
        string script_src;
        std::ifstream src_file_stream("scripts/"  + name + ".lua");

        src_file_stream.seekg(0, std::ios::end);
        script_src.reserve(src_file_stream.tellg());
        src_file_stream.seekg(0, std::ios::beg);

        script_src.assign((std::istreambuf_iterator<char>(src_file_stream)),
                        std::istreambuf_iterator<char>());

        return make_shared<script>(name, script_src);
#else
        LOG(TRACE) << NAMEOF(load_script) << " loading \"" + name + "\" from db";
        auto transaction = scripts_repo->create_transaction();
        return scripts_repo->get_script(name, get<1>(transaction));
#endif
    }
}