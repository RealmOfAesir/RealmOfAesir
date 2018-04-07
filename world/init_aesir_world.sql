START TRANSACTION;

CREATE TABLE scripts (
    id SERIAL PRIMARY KEY,
    script_name citext NOT NULL,
    script text NOT NULL
);

CREATE TABLE script_triggers (
    id SERIAL PRIMARY KEY,
    trigger_type citext NOT NULL,
    script_id INT NOT NULL,
    script_zone_id INT NULL,
    global BOOLEAN NOT NULL
);

CREATE TABLE script_zones (
    id SERIAL PRIMARY KEY,
    zone_name text NULL,
    map_id INT NOT NULL,
    x INT NOT NULL,
    y INT NOT NULL,
    width INT NOT NULL,
    height INT NOT NULL
);

CREATE TABLE stats (
    id SERIAL PRIMARY KEY,
    stat_name citext NOT NULL
);

CREATE TABLE locations (
    id BIGSERIAL PRIMARY KEY,
    map_id INT NOT NULL,
    x INT NOT NULL,
    y INT NOT NULL
);

CREATE TABLE maps (
    id SERIAL PRIMARY KEY,
    map_name citext NOT NULL
);

CREATE TABLE map_tiles (
    id BIGSERIAL PRIMARY KEY,
    map_id INT NOT NULL,
    tile_id INT NOT NULL,
    x INT NOT NULL,
    y INT NOT NULL,
    z INT NOT NULL
);

CREATE TABLE players (
    id BIGSERIAL PRIMARY KEY,
    user_id BIGINT NOT NULL,
    location_id BIGINT NOT NULL,
    player_name citext NOT NULL
);

CREATE TABLE player_stats (
    id BIGSERIAL PRIMARY KEY,
    player_id BIGINT NOT NULL,
    stat_id INT NOT NULL,
    is_growth BOOLEAN NOT NULL,
    static_value BIGINT NOT NULL,
    dice INT NULL,
    die_face INT NULL
);

CREATE TABLE items (
    id BIGSERIAL PRIMARY KEY,
    player_id BIGINT NULL,
    npc_id INT NULL,
    location_id BIGINT NULL,
    item_name citext NOT NULL
);

CREATE TABLE item_stats (
    id BIGSERIAL PRIMARY KEY,
    item_id BIGINT NOT NULL,
    stat_id INT NOT NULL,
    is_growth BOOLEAN NOT NULL,
    static_value BIGINT NOT NULL,
    dice INT NULL,
    die_face INT NULL
);

CREATE TABLE npcs (
    id SERIAL PRIMARY KEY,
    location_id BIGINT NOT NULL,
    npc_name citext NOT NULL
);

CREATE TABLE npc_stats (
    id BIGSERIAL PRIMARY KEY,
    npc_id INT NOT NULL,
    stat_id INT NOT NULL,
    is_growth BOOLEAN NOT NULL,
    static_value BIGINT NOT NULL,
    dice INT NULL,
    die_face INT NULL
);

CREATE TABLE objects (
    id BIGSERIAL PRIMARY KEY,
    location_id BIGINT NOT NULL,
    object_name citext NOT NULL
);

CREATE TABLE settings (
    setting_name citext NOT NULL,
    value text not null
);

CREATE TABLE schema_information (
    file_name text NOT NULL,
    date TIMESTAMPTZ NOT NULL
);

ALTER TABLE scripts ADD CONSTRAINT "scripts_name_unique" UNIQUE (script_name);
ALTER TABLE script_triggers ADD CONSTRAINT "script_triggers_scripts_id_fkey" FOREIGN KEY (script_id) REFERENCES scripts(id);
ALTER TABLE script_triggers ADD CONSTRAINT "script_triggers_script_zones_id_fkey" FOREIGN KEY (script_zone_id) REFERENCES script_zones(id);
ALTER TABLE script_zones ADD CONSTRAINT "script_zones_map_id_fkey" FOREIGN KEY (map_id) REFERENCES maps(id);
ALTER TABLE locations ADD CONSTRAINT "locations_maps_id_fkey" FOREIGN KEY (map_id) REFERENCES maps(id);
ALTER TABLE map_tiles ADD CONSTRAINT "map_tiles_maps_id_fkey" FOREIGN KEY (map_id) REFERENCES maps(id);
ALTER TABLE players ADD CONSTRAINT "players_locations_id_fkey" FOREIGN KEY (location_id) REFERENCES locations(id);
ALTER TABLE players ADD CONSTRAINT "players_name_unique" UNIQUE (player_name);
ALTER TABLE player_stats ADD CONSTRAINT "player_stats_players_id_fkey" FOREIGN KEY (player_id) REFERENCES players(id);
ALTER TABLE player_stats ADD CONSTRAINT "player_stats_stats_id_fkey" FOREIGN KEY (stat_id) REFERENCES stats(id);
ALTER TABLE player_stats ADD CONSTRAINT "player_stats_die_ck" CHECK ((dice IS NOT NULL AND die_face IS NOT NULL) OR (dice IS NULL AND die_face IS NULL));
ALTER TABLE items ADD CONSTRAINT "items_players_id_fkey" FOREIGN KEY (player_id) REFERENCES players(id);
ALTER TABLE items ADD CONSTRAINT "items_npcs_id_fkey" FOREIGN KEY (npc_id) REFERENCES npcs(id);
ALTER TABLE items ADD CONSTRAINT "items_locations_id_fkey" FOREIGN KEY (location_id) REFERENCES locations(id);
ALTER TABLE item_stats ADD CONSTRAINT "item_stats_items_id_fkey" FOREIGN KEY (item_id) REFERENCES items(id);
ALTER TABLE item_stats ADD CONSTRAINT "item_stats_stats_id_fkey" FOREIGN KEY (stat_id) REFERENCES stats(id);
ALTER TABLE item_stats ADD CONSTRAINT "item_stats_die_ck" CHECK ((dice IS NOT NULL AND die_face IS NOT NULL) OR (dice IS NULL AND die_face IS NULL));
ALTER TABLE npcs ADD CONSTRAINT "npcs_location_id_fkey" FOREIGN KEY (location_id) REFERENCES locations(id);
ALTER TABLE npc_stats ADD CONSTRAINT "npc_stats_items_id_fkey" FOREIGN KEY (npc_id) REFERENCES npcs(id);
ALTER TABLE npc_stats ADD CONSTRAINT "npc_stats_stats_id_fkey" FOREIGN KEY (stat_id) REFERENCES stats(id);
ALTER TABLE npc_stats ADD CONSTRAINT "npc_stats_die_ck" CHECK ((dice IS NOT NULL AND die_face IS NOT NULL) OR (dice IS NULL AND die_face IS NULL));
ALTER TABLE objects ADD CONSTRAINT "objects_locations_id_fkey" FOREIGN KEY (location_id) REFERENCES locations(id);
ALTER TABLE settings ADD CONSTRAINT "settings_name_unique" UNIQUE (setting_name);
ALTER TABLE schema_information ADD CONSTRAINT "schema_information_name_unique" UNIQUE (file_name);

INSERT INTO schema_information(file_name, date) VALUES ('init_aesir_world.sql', CURRENT_TIMESTAMP);

COMMIT;