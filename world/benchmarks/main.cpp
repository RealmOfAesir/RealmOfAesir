/*
    Realm of Aesir backend
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

#include <easylogging++.h>
#include <lua/lua_interop.h>
#include <chrono>
#include <macros.h>

#include <unistd.h>

INITIALIZE_EASYLOGGINGPP

using namespace std;
using namespace roa;

void print_process_mem_usage(string stuff) {
    using std::ios_base;
    using std::ifstream;
    using std::string;

    // 'file' stat seems to give the most reliable results
    //
    ifstream stat_stream("/proc/self/stat", ios_base::in);

    // dummy vars for leading entries in stat that we don't care about
    //
    string pid, comm, state, ppid, pgrp, session, tty_nr;
    string tpgid, flags, minflt, cminflt, majflt, cmajflt;
    string utime, stime, cutime, cstime, priority, nice;
    string O, itrealvalue, starttime;

    // the two fields we want
    //
    unsigned long vsize;
    long rss;

    stat_stream >> pid >> comm >> state >> ppid >> pgrp >> session >> tty_nr
                >> tpgid >> flags >> minflt >> cminflt >> majflt >> cmajflt
                >> utime >> stime >> cutime >> cstime >> priority >> nice
                >> O >> itrealvalue >> starttime >> vsize >> rss; // don't care about the rest

    stat_stream.close();

    long page_size_kb = sysconf(_SC_PAGE_SIZE) / 1024; // in case x86-64 is configured to use 2MB pages
    LOG(INFO) << stuff << " VM: " << (vsize / 1024.0) << " - RSS: " << rss * page_size_kb;
}

void benchmark_loading_and_running_lua() {
    std::string lib;
    std::string src_file;

    {
        std::ifstream lib_stream("scripts/roa_library.lua");
        lib_stream.seekg(0, std::ios::end);
        lib.reserve(lib_stream.tellg());
        lib_stream.seekg(0, std::ios::beg);

        lib.assign((std::istreambuf_iterator<char>(lib_stream)),
                   std::istreambuf_iterator<char>());
    }

    {
        std::ifstream src_file_stream("scripts/tile_id_upwards.lua");
        src_file_stream.seekg(0, std::ios::end);
        src_file.reserve(src_file_stream.tellg());
        src_file_stream.seekg(0, std::ios::beg);

        src_file.assign((std::istreambuf_iterator<char>(src_file_stream)),
                        std::istreambuf_iterator<char>());
    }

    set_library_script(lib);
    lua_script L = load_script_with_libraries("test", src_file);

    print_process_mem_usage("start mem: ");

    auto start = chrono::high_resolution_clock::now();
    for (int i = 0; i < 100'000; i++) {
        L.load();

        L.create_table();

        L.push_boolean("debug", false);
        L.push_boolean("library_debug", false);

        L.set_global("roa_settings");

        L.create_table();

        L.push_integer("tile_id", 1);
        L.push_integer("id", 1);
        L.push_integer("type", 1);

        L.create_nested_table("stats");

        L.push_integer("test", 15);
        L.push_integer("test2", 16);

        L.push_table();

        L.set_global("roa_entity");

        L.create_table();

        L.push_integer("tile_id", 2);
        L.push_integer("first_tile_id", 2);
        L.push_integer("max_tile_id", 2);
        L.push_integer("width", 2);
        L.push_integer("height", 2);

        L.set_global("roa_map");

        L.create_table();

        L.push_integer("id", 20);
        L.push_string("name", "script");

        L.set_global("roa_script");
        L.run();
    }
    auto end = chrono::high_resolution_clock::now();

    print_process_mem_usage("end mem: ");

    LOG(INFO) << NAMEOF(benchmark_loading_and_running_lua) << " time required to run: "
              << chrono::duration_cast<chrono::milliseconds>((end - start)).count() << " ms";
}

void init_stuff() {
    ios::sync_with_stdio(false);

    el::Configurations defaultConf;
    defaultConf.setGlobally(el::ConfigurationType::Format, "%datetime %level: %msg");
    defaultConf.set(el::Level::Debug, el::ConfigurationType::Enabled, "false");
    defaultConf.set(el::Level::Trace, el::ConfigurationType::Enabled, "false");
    el::Loggers::reconfigureAllLoggers(defaultConf);
}

int main(int argc, char const *const *argv) {
    init_stuff();

    benchmark_loading_and_running_lua();

    return 0;
}
