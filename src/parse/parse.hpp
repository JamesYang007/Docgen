#pragma once
#include <cstdio>
#include <type_traits>
#include <memory>
#include "core/routine.hpp"
#include "core/cache.hpp"
#include "utility/file_deleter.hpp"

namespace docgen {
namespace parse {

// Parse a file given a filepath.
void parse_file(const char* filepath)
{
    constexpr size_t buf_size = 1000;   // TODO: experiment later optimal size
    constexpr size_t symbol_size = 10;  // TODO: figure out optimal symbol size statically later

    using Routine = core::Routine;
    using cache_t = core::Cache<symbol_size>;

    cache_t cache;
    Routine routine = Routine::READ;    // initially in read routine

    using file_ptr_t = std::unique_ptr<FILE, utils::file_deleter>;
    file_ptr_t file(fopen(filepath, "r"), utils::file_deleter());

    char buf[buf_size] = {0};
    size_t nread = 0;

    while ((nread = fread(buf, sizeof(buf[0]), buf_size, file.get())) > 0) {
        const char* begin = buf; 
        const char* end = buf + nread;
        while (begin != end) {
            switch(routine) {
                case Routine::READ:
                    routine = core::routine<Routine::READ>::template run(cache, begin, end);
                    break;

                // TODO: other routines
                default:
                    break;
            }
        }
    }
}

} // namespace parse
} // namespace docgen
