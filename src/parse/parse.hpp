#pragma once
#include <cstdio>
#include <type_traits>
#include <memory>
#include "core/routine.hpp"

namespace docgen {
namespace parse {

// Parse a file given a filepath.
void parse_file(const char* filepath)
{
    constexpr size_t buf_size = 1000;   // TODO: experiment later optimal size
    constexpr size_t symbol_size = 10;  // TODO: figure out optimal symbol size statically later

    using Routine = core::Routine;
    using Status = core::Status;

    Status status(symbol_size);
    Routine routine = Routine::READ;    // initially in read routine

    FILE* file = fopen(filepath, "r");

    char buf[buf_size] = {0};
    size_t nread = 0;

    while ((nread = fread(buf, sizeof(buf[0]), buf_size, file)) > 0) {
        const char* begin = buf; 
        const char* end = buf + nread;
        while (begin != end) {
            switch(routine) {
                case Routine::READ:
                    routine = core::routine<Routine::READ>::run(status, begin, end);
                    break;

                // TODO: other routines
                default:
                    break;
            }
        }
    }

    fclose(file);
}

} // namespace parse
} // namespace docgen
