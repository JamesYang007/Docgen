#pragma once
#include <cstdio>
#include <type_traits>
#include <memory>
#include "core/routine.hpp"
#include "core/cache.hpp"

namespace docgen {
namespace parse {
namespace details {

struct file_deleter
{
    void operator()(FILE* file) const
    {
        fclose(file);
    }
};

} // namespace details

// Parse a file given a filepath.
void parse_file(const char* filepath)
{
    constexpr size_t buf_size = 1000;   // TODO: experiment later optimal size
    constexpr size_t symbol_size = 10;  // TODO: figure out optimal symbol size statically later

    using Routine = core::Routine;
    using cache_t = core::Cache<symbol_size>;
    using routine_array_t = core::routine_array_t<cache_t>;

    static constexpr routine_array_t routines = core::make_routines<cache_t>();

    cache_t cache;
    Routine routine = Routine::READ;    // initially in read routine

    using file_ptr_t = std::unique_ptr<FILE, details::file_deleter>;
    file_ptr_t file(fopen(filepath, "r"), details::file_deleter());

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
    // TODO: implement
}

} // namespace parse
} // namespace docgen
