#pragma once
#include <cstdio>
#include <vector>

namespace docgen {
namespace io {

struct file_reader
{
    file_reader(const char* filename)
        : file_(fopen(filename, "r"))
    {
        assert(file_ != nullptr);
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
