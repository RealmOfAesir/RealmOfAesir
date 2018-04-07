START TRANSACTION;

INSERT INTO licenses (license_name, author, license) VALUES ('http://pousse.rapiere.free.fr/tome/tome-tiles.htm',  'David Gervais', 'https://creativecommons.org/licenses/by/3.0/');

--INSERT INTO gods ("name") VALUES ('Andhrimnir');
--INSERT INTO gods ("name") VALUES ('Loki');
--INSERT INTO gods ("name") VALUES ('Odin');
--INSERT INTO gods ("name") VALUES ('Thor');
--INSERT INTO gods ("name") VALUES ('Freya');

INSERT INTO settings (setting_name, value) VALUES ('maintenance_mode', '0');
INSERT INTO settings (setting_name, value) VALUES ('max_characters_per_user', '1');
INSERT INTO settings (setting_name, value) VALUES ('starting_world_id', '0');

INSERT INTO users (username, password, email, admin) VALUES ('admin', '$argon2i$v=19$m=131072,t=6,p=1$qkpFHxW828QgvN0gucL+UA$oVeHz9xRUZbqFHfMVJLpISvoIvVQ12dLQuqGZIu9kXc', 'realm.of.aesir@gmail.com', 1);

INSERT INTO schema_information(file_name, date) VALUES ('seed.sql', CURRENT_TIMESTAMP);

COMMIT;