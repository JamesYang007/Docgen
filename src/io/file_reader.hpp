#pragma once
#include <cassert>
#include <cstdio>
#include <vector>
#include <exceptions/exceptions.hpp>

namespace docgen {
namespace io {

struct file_reader
{
    file_reader(FILE* file)
        : file_(file)
    {
        if (!file_) {
            throw exceptions::system_error("file not valid");
        }
    }

    int read()
    {
        return fgetc(file_);
    }

    void back(int c)
    {
        ungetc(c, file_);
    }

    int peek() 
    {
        int c = read();
        back(c);
        return c;
    }

    static constexpr int termination = EOF;

private:
    FILE* file_ = nullptr;
};

} // namespace io
} // namespace docgen
