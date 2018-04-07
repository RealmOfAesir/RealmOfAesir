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

#include "script_system.h"
#include <queue>
#include <easylogging++.h>
#include <macros.h>
#include <lua/lua_interop.h>
#include <custom_optional.h>
#include <lua/lua_script.h>
#include <events/lua_events/update_tile_event.h>
#include <events/lua_events/create_script_event.h>
#include <events/lua_events/destroy_script_event.h>
#include <chrono>

using namespace std;
using namespace experimental;
using namespace roa;

queue<shared_ptr<event_type>> _script_event_queue;

enum entity_types {
    tile = 1,
    character = 2,
    player = 3
};

STD_OPTIONAL<uint64_t> get_map_entity_from_id(EntityManager &es, uint64_t map_id) {
    STD_OPTIONAL<uint64_t> ret;

    for(auto entity : es.view<map_component>()) {
        auto& map = es.get<map_component>(entity);
        if(map.id == map_id) {
            ret = make_optional(entity);
        }
    }

    return ret;
}

void script_system::update(EntityManager &es, TimeDelta dt) {
    LOG(TRACE) << NAMEOF(script_system::update) << " starting tick";
    int updated_scripts = 0;
    int checked_entities = 0;
    int checked_scripts = 0;
    auto start = chrono::high_resolution_clock::now();
    for(auto entity : es.view<script_container_component>()) {
        checked_entities++;
        auto& script_container = es.get<script_container_component>(entity);
        LOG(TRACE) << NAMEOF(script_system::update) << " entity " << entity << " with " << script_container.scripts.size() << " scripts";
        for(auto& script_tuple : script_container.scripts) {
            auto& script = get<1>(script_tuple);

            LOG(TRACE) << NAMEOF(script_system::update) << " script " << script.id;
            checked_scripts++;

            if (script.execute_in_ms <= dt) {
                lua_script &lscript = script.script;

                if (unlikely(!lscript.load())) {
                    return;
                }

                lscript.create_table();

                lscript.push_boolean("debug", script.debug);
                lscript.push_boolean("library_debug", _config.debug_roa_library);

                lscript.set_global("roa_settings");

                lscript.create_table();

                if (es.has<tile_component>(entity)) {
                    auto& tile_handle = es.get<tile_component>(entity);
                    LOG(TRACE) << NAMEOF(script_system::update) << " tile handle " << tile_handle.tile_id;
                    lscript.push_integer("tile_id", tile_handle.tile_id);
                    lscript.push_integer("id", entity);
                    lscript.push_integer("type", entity_types::tile);
                } else if (es.has<character_component>(entity)) {
                    auto& stats_handle = es.get<stat_component>(entity);

                    if (es.has<player_component>(entity)) {
                        auto& player_handle = es.get<player_component>(entity);
                        LOG(TRACE) << NAMEOF(script_system::update) << " player handle " << player_handle.user_id;
                        lscript.push_integer("user_id", player_handle.user_id);
                        lscript.push_integer("type", entity_types::player);
                    } else {
                        lscript.push_integer("type", entity_types::character);
                    }
                    lscript.push_integer("id", entity);

                    lscript.create_nested_table("stats");

                    for (auto &stat : stats_handle.stats) {
                        if (!stat.is_growth) {
                            lscript.push_integer(stat.name, stat.static_value);
                        }
                    }

                    lscript.push_table();
                }

                lscript.set_global("roa_entity");

                if (!script.global) {
                    uint64_t map_id;

                    if (es.has<tile_component>(entity)) {
                        auto& tile_handle = es.get<tile_component>(entity);
                        map_id = tile_handle.map_id;
                    } else if (es.has<character_component>(entity)) {
                        auto& character_handle = es.get<character_component>(entity);
                        map_id = character_handle.map_id;
                    }

                    LOG(TRACE) << NAMEOF(script_system::update) << " map handle " << map_id;

                    auto map_entity = get_map_entity_from_id(es, map_id);

                    if (!map_entity) {
                        LOG(ERROR) << "non-global script without map entity " << map_id;
                        lscript.close();
                        return;
                    }

                    auto& map_handle = es.get<map_component>(map_entity.value());

                    lscript.create_table();

                    lscript.push_integer("id", map_entity.value());
                    lscript.push_integer("first_tile_id", map_handle.first_tile_id);
                    lscript.push_integer("max_tile_id", map_handle.max_tile_id);
                    lscript.push_integer("width", map_handle.width);
                    lscript.push_integer("height", map_handle.height);

                    lscript.set_global("roa_map");
                }

                lscript.create_table();

                lscript.push_integer("id", script.id);
                lscript.push_integer("attached_entity_id", entity);
                lscript.push_string("name", script.script.name());

                lscript.set_global("roa_script");

                if (unlikely(!lscript.run())) {
                    return;
                }

                updated_scripts++;
                script.execute_in_ms = script.loop_every_ms - (dt - script.execute_in_ms);
            } else {
                script.execute_in_ms -= dt;
            }
        }
    }
    auto end = chrono::high_resolution_clock::now();
    LOG(INFO) << NAMEOF(script_system::update) << " checked " << checked_entities << " entities, " << checked_scripts << " scripts and ran " << updated_scripts << " scripts in "
              << chrono::duration_cast<chrono::milliseconds>((end-start)).count() << " ms";

    start = chrono::high_resolution_clock::now();
    int total_events = 0;
    int update_tile_events = 0;
    int destroy_script_events = 0;
    int create_script_events = 0;
    while(!_script_event_queue.empty()) {
        auto& event = _script_event_queue.front();
        total_events++;

        switch(event->type) {
            case update_tile_event::type: {
                update_tile_events++;
                update_tile_event *update_event = dynamic_cast<update_tile_event *>(event.get());

                if (unlikely(update_event == nullptr)) {
                    LOG(WARNING) << NAMEOF(script_system::update) << " expected event type update_tile_event but couldn't cast";
                    break;
                }

                if(unlikely(!es.valid(update_event->id))) {
                    LOG(ERROR) << NAMEOF(script_system::update) << " tried updating non-existing entity";
                    break;
                }

                auto& tile_handle = es.get<tile_component>(update_event->id);
                LOG(TRACE) << NAMEOF(script_system::update) << " updating tile " << update_event->id
                           << " map_id " << tile_handle.map_id << " new tile_id " << update_event->tile_id;
                tile_handle.tile_id = update_event->tile_id;
            }
                break;
            case destroy_script_event::type: {
                destroy_script_events++;
                destroy_script_event *destroy_event = dynamic_cast<destroy_script_event *>(event.get());

                if (unlikely(destroy_event == nullptr)) {
                    LOG(WARNING) << NAMEOF(script_system::update) << " expected event type destroy_script_event but couldn't cast";
                    break;
                }

                if(unlikely(!es.valid(destroy_event->id))) {
                    LOG(ERROR) << NAMEOF(script_system::update) << " tried destroying non-existing script entity";
                    break;
                }

                if(unlikely(!es.valid(destroy_event->attached_entity_id) || !es.has<script_container_component>(destroy_event->attached_entity_id))) {
                    LOG(ERROR) << NAMEOF(script_system::update) << " tried destroying script entity but wrong attached entity_id";
                    break;
                }

                LOG(TRACE) << NAMEOF(script_system::update) << " destroying script " << destroy_event->id;
                auto& container = es.get<script_container_component>(destroy_event->attached_entity_id);
                container.remove_by_id(destroy_event->id);
                es.destroy(destroy_event->id);
            }
                break;
            case create_script_event::type: {
                create_script_events++;
                create_script_event *create_event = dynamic_cast<create_script_event *>(event.get());

                if (unlikely(create_event == nullptr)) {
                    LOG(WARNING) << NAMEOF(script_system::update) << " expected event type create_script_event but couldn't cast";
                    break;
                }

                if(unlikely(!es.valid(create_event->entity_id))) {
                    LOG(ERROR) << NAMEOF(script_system::update) << " tried attaching to non-existing entity";
                    break;
                }

                auto script_entity = es.create();
                auto& script_container_handle = es.get<script_container_component>(create_event->entity_id);
                bool global = create_event->entity_id == 0;

                LOG(TRACE) << NAMEOF(script_system::update) << " creating script " << create_event->name
                           << ":" << script_entity << " attaching to " << create_event->entity_id;

                script_container_handle.scripts.emplace(script_entity, script_component{
                        script_entity, load_script_with_libraries(create_event->name, create_event->script),
                        create_event->execute_in_ms, create_event->loop_every_ms, create_event->trigger_type, global, create_event->debug});
            }
                break;
            default:
                break;
        }

        _script_event_queue.pop();
    }

    end = chrono::high_resolution_clock::now();
    LOG(INFO) << NAMEOF(script_system::update) << " pumped " << total_events << " events, " << update_tile_events << " update events, "
              << destroy_script_events << " destroy events and " << create_script_events << " create events in "
              << chrono::duration_cast<chrono::milliseconds>((end-start)).count() << " ms";
}
