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

INITIALIZE_EASYLOGGINGPP

using namespace std;

void init_stuff() {
    ios::sync_with_stdio(false);

    el::Configurations defaultConf;
    defaultConf.setGlobally(el::ConfigurationType::Format, "%datetime %level: %msg");
    el::Loggers::reconfigureAllLoggers(defaultConf);
}

int main(int argc, char const * const * argv) {
    init_stuff();

    int result = Catch::Session().run( argc, argv );

    // global clean-up...

    return ( result < 0xff ? result : 0xff );
}