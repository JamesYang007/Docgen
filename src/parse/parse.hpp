#pragma once
#include <cstdio>
#include "core/routine.hpp"
#include "core/cache.hpp"

namespace docgen {
namespace parse {

// Parse a file given a filepath.
void parse_file(const char* filepath)
{
    using Routine = core::Routine;
    using State = core::State;

    constexpr size_t buf_size = 1000;   // TODO: experiment later optimal size
    constexpr size_t symbol_size = 10;  // TODO: figure out optimal symbol size statically later

    core::Cache<symbol_size> cache;
    cache.state = State::DEFAULT;       // initially in default state

    Routine routine = Routine::READ;    // initially in read routine

    FILE* file = fopen(filepath, "r");
    char buf[buf_size] = {0};
    size_t nread = 0;

    while ((nread = fread(buf, sizeof(buf[0]), buf_size, file)) > 0) {
        const char* begin = buf; 
        const char* end = buf + nread;

        while (begin != end) {
            switch (routine) {
                case Routine::READ:
                    routine = core::routine<Routine::READ>::run(cache, begin, end);
                    break;

                // TODO: other routines
                
                default:
                    break;
            }
        }
    }
}

// Parse files in a given directory
void parse_dir(const char* dirpath)
{
    //for (auto& file_path : std::directory_iterator(dirpath)) {
    //        parse_file(file_path);
    //}   
}

} // namespace parse
} // namespace docgen
