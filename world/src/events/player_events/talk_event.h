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

namespace roa {
    struct talk_event : public player_event {
    public:
        talk_event() = default;
        talk_event(std::string name, std::string script, uint32_t entity_id, uint32_t execute_in_ms, uint32_t loop_every_ms, trigger_type_enum trigger_type, bool debug)
                : player_event(create_script_event::type), name(name), script(script), entity_id(entity_id), execute_in_ms(execute_in_ms), loop_every_ms(loop_every_ms), trigger_type(trigger_type), debug(debug) {}
        virtual ~talk_event() {}

        static constexpr uint32_t type = 1003;
        std::string name;
        std::string script;
        uint32_t entity_id;
        uint32_t execute_in_ms;
        uint32_t loop_every_ms;
        trigger_type_enum trigger_type;
        bool debug;
    };
}

