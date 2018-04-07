START TRANSACTION;

INSERT INTO maps (map_name) VALUES ('test_map');
INSERT INTO map_tiles (map_id, tile_id, x, y, z) VALUES (1, 0, 0, 0, 0), (1, 0, 0, 1, 0), (1, 0, 1, 0, 0), (1, 0, 1, 1, 0);

-- player start location
INSERT INTO script_zones (map_id, x, y, width, height) VALUES (1, 0, 0, 1, 1);
UPDATE settings SET value = LASTVAL() WHERE setting_name = 'player_start_script_zone';

INSERT INTO schema_information(file_name, date) VALUES ('test_data.sql', CURRENT_TIMESTAMP);

COMMIT;