#pragma once
#include <cstdio>
#include <type_traits>
#include <memory>
#include "core/routine.hpp"
#include "core/cache.hpp"

namespace docgen {
namespace parse {

// Parse a file given a filepath.
void parse_file(const char* filepath)
{
    constexpr size_t buf_size = 1000;   // TODO: experiment later optimal size
    constexpr size_t symbol_size = 10;  // TODO: figure out optimal symbol size statically later

    using Routine = core::Routine;
    using cache_t = core::Cache<symbol_size>;

    static constexpr core::RoutineArray<cache_t> routines = {{
        core::routine<Routine::READ>::template run<cache_t>,
        core::routine<Routine::IGNORE_WS>::template run<cache_t>,
        core::routine<Routine::PROCESS>::template run<cache_t>
    }};

    static_assert(core::is_properly_ordered(routines), 
            "RoutineArray is initialized improperly. "
            "It must be intialized in the same order as core::Routine."
            );

    cache_t cache;
    Routine routine = Routine::READ;    // initially in read routine

    std::shared_ptr<FILE> file(fopen(filepath, "r"), 
                               [](FILE* file) { fclose(file); }
                               );
    char buf[buf_size] = {0};
    size_t nread = 0;

    while ((nread = fread(buf, sizeof(buf[0]), buf_size, file.get())) > 0) {
        const char* begin = buf; 
        const char* end = buf + nread;
        while (begin != end) {
            routine = routines[static_cast<size_t>(routine)](cache, begin, end);
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
