# building

### build the cpp lib first

```bash
mkdir build && cd build
cmake ..
cmake --build . --config Release
```

lib will be built to lib-cpp, from here it will be usable in rust

### rust

just add the dependency to your cargo.toml

```toml
[dependencies]
elden-ring-utils-rs = {git="https://github.com/Tacotakedown/elden-ring-util-rs/tree/master"}
```

##

many of the functions use `std::string` or `std::vec` so there is a c wrapper for the rust wrapper since rust cannot directly interface with cpp classes, vec and string
