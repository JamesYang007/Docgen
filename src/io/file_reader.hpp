#pragma once
#include <cassert>
#include <cstdio>
#include <vector>
#include <exceptions/exceptions.hpp>

namespace docgen {
namespace io {

struct file_reader
{
    file_reader(const char* filename)
        : file_(fopen(filename, "r"))
    {
        if (!file_) {
            throw exceptions::system_error(filename);
        }
    }

    ~file_reader()
    {
        fclose(file_);
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
