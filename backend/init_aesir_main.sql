START TRANSACTION;

CREATE TABLE users (
    id BIGSERIAL PRIMARY KEY,
    username citext NOT NULL,
    password VARCHAR(100) NOT NULL,
    email citext NOT NULL,
    login_attempts SMALLINT NOT NULL DEFAULT 0,
    verification_code text DEFAULT NULL,
    no_of_players SMALLINT NOT NULL DEFAULT 0,
    admin SMALLINT NOT NULL DEFAULT 0
);

CREATE TABLE licenses (
    id BIGSERIAL PRIMARY KEY,
    license_name text NOT NULL,
    author text NOT NULL,
    license text NOT NULL
);

CREATE TABLE banned_users (
    id BIGSERIAL PRIMARY KEY,
    ip text NULL,
    user_id BIGINT NULL,
    until TIMESTAMPTZ NULL
);

CREATE TABLE settings (
    setting_name text NOT NULL,
    value text NOT NULL
);

CREATE TABLE schema_information (
    file_name text NOT NULL,
    date TIMESTAMPTZ NOT NULL
);

ALTER TABLE users ADD CONSTRAINT "users_username_unique" UNIQUE (username);
ALTER TABLE banned_users ADD CONSTRAINT "banned_users_user_id_fkey" FOREIGN KEY (user_id) REFERENCES users(id);
ALTER TABLE settings ADD CONSTRAINT "settings_name_unique" UNIQUE (setting_name);
ALTER TABLE schema_information ADD CONSTRAINT "schema_information_name_unique" UNIQUE (file_name);

INSERT INTO schema_information(file_name, date) VALUES ('init_aesir_main.sql', CURRENT_TIMESTAMP);

COMMIT;