#pragma once

#include <cstdio>

namespace docgen {
namespace utils {

struct file_deleter
{
    void operator()(FILE* file) const
    {
        fclose(file);
    }
};

} // namespace utils
} // namespace docgen
