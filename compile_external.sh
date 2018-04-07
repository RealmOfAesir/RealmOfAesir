#!/bin/bash
DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
CPUS="$( grep -c ^processor /proc/cpuinfo )"

cd $DIR/external/libsodium
./autogen.sh
./configure
make -j$CPUS

cd $DIR/external/libpqxx
./configure --disable-documentation
make -j$CPUS

cd $DIR/external/librdkafka
./configure
make -j$CPUS

cd $DIR/external/uWebSockets
make

cd $DIR/external/LuaJIT
make -j$CPUS

cd $DIR/external/lz4
make -j$CPUS

cd $DIR/external/Catch2
cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo .
make -j$CPUS

cd $DIR/external/cpprestsdk/Release
cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo .
make -j$CPUS

cd $DIR
