Realm of Aesir client

# compiling

Compiling requires cmake 3.1 or higher and g++ 5 or higher, because it uses C++17 stuff.

Debug builds:
```bash
cmake -DCMAKE_BUILD_TYPE=Debug .
```

Release builds:
```bash
cmake .
```

then finish it with
```bash
make && bin/RealmOfAesir
```
