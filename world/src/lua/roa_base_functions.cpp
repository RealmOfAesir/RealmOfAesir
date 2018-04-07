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

#include <queue>
#include <macros.h>
#include "lua_interop.h"
#include "easylogging++.h"
#include "../../test/test_helpers/startup_helper.h"
#include <cstdio>
#include <repositories/scripts_repository.h>
#include <boost/di.hpp>
#include <events/lua_events/update_tile_event.h>
#include <events/lua_events/create_script_event.h>
#include <events/lua_events/destroy_script_event.h>

using namespace std;
using namespace roa;

extern queue<shared_ptr<event_type>> _script_event_queue;

extern "C" void roa_log(int level, char const *message) {
    switch (level) {
        case 0:
            LOG(DEBUG) << message;
            break;
        case 1:
            LOG(INFO) << message;
            break;
        case 2:
            LOG(WARNING) << message;
            break;
        default:
            LOG(ERROR) << message;
            break;
    }
}

extern "C" void set_tile_properties(uint32_t id, uint16_t tile_id) {
    _script_event_queue.emplace(make_shared<update_tile_event>(id, tile_id));
}

extern "C" void destroy_script(uint32_t id, uint32_t attached_entity_id) {
    _script_event_queue.emplace(make_shared<destroy_script_event>(id, attached_entity_id));
}

extern "C" void create_script(const char * name, uint32_t id, uint32_t execute_in_ms,
                              uint32_t loop_every_ms, uint32_t trigger_type, bool debug) {
    trigger_type_enum type;
    if(trigger_type == 0) {
        type = trigger_type_enum::once;
    } else if(trigger_type == 1) {
        type = trigger_type_enum::looped;
    } else if(trigger_type == 2) {
        type = trigger_type_enum::chat;
    } else if(trigger_type == 3) {
        type = trigger_type_enum::movement;
    } else {
        LOG(ERROR) << NAMEOF(roa_base_functions::create_script) << " trigger_type undefined";
        return;
    }

    auto script = load_script(name);

    _script_event_queue.emplace(make_shared<create_script_event>(name, script->text, id, execute_in_ms, loop_every_ms, type, debug));
}
