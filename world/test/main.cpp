/*
    Realm of Aesir
    Copyright (C) 2016  Michael de Lang

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

#define CATCH_CONFIG_RUNNER

#include <catch.hpp>
#include <easylogging++.h>
#include <nlohmann/json.hpp>

#include "test_helpers/startup_helper.h"

INITIALIZE_EASYLOGGINGPP

using namespace std;
using namespace roa;
using json = nlohmann::json;

Config parse_env_file() {
    string env_contents;
    ifstream env(".env_world");

    if(!env) {
        LOG(ERROR) << "[main] no .env_world file found. Please make one.";
        exit(1);
    }

    env.seekg(0, ios::end);
    env_contents.resize(env.tellg());
    env.seekg(0, ios::beg);
    env.read(&env_contents[0], env_contents.size());
    env.close();

    auto env_json = json::parse(env_contents);
    Config config;

    try {
        config.connection_string = env_json["CONNECTION_STRING"];
    } catch (const std::exception& e) {
        LOG(ERROR) << "[main] CONNECTION_STRING missing in .env_world file.";
        exit(1);
    }

    return config;
}

void init_stuff() {
    ios::sync_with_stdio(false);

    el::Configurations defaultConf;
    defaultConf.setGlobally(el::ConfigurationType::Format, "%datetime %level: %msg");
    el::Loggers::reconfigureAllLoggers(defaultConf);

    config = parse_env_file();
    config.connection_string.size();
    db_pool = make_shared<database_pool>();
    db_pool->create_connections(config.connection_string, 2);
}

int main(int argc, char **argv) {
    init_stuff();

    int result = Catch::Session().run( argc, argv );

    // global clean-up...

    return ( result < 0xff ? result : 0xff );
}