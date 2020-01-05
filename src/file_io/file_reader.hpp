#pragma once
#include <cstdio>
#include <vector>

namespace docgen {
namespace file_io {

static constexpr size_t DEFAULT_BUF_SIZE = 1024;

struct file_reader
{
    file_reader(const char* filename, size_t buf_size = DEFAULT_BUF_SIZE)
        : file_(fopen(filename, "r"))
        , buf_(buf_size, 0)
    {
        assert(buf_size > 0);
        assert(file_ != nullptr);
        refill();
    }

    ~file_reader()
    {
        fclose(file_);
    }

    char read()
    {
        char x = peek();
        // do not increment if '\0'
        if (x) {
            ++begin_;
        }
        return x;
    }

    char peek() 
    {
        if (begin_ == end_) {
            refill();
        }

        // begin_ == end_ again after refilling, 
        // hit EOF and nothing more to read in buffer
        if (begin_ == end_) {
            return 0;
        }

        return buf_[begin_];
    }

private:

    void refill()
    {
        size_t n_read = fread(buf_.data(), sizeof(char), buf_.size(), file_);
        begin_ = 0;
        end_ = begin_ + n_read;
    }

    FILE* file_ = nullptr;
    std::vector<char> buf_;
    size_t begin_ = 0;
    size_t end_ = 0;
};

} // namespace file_io
} // namespace docgen
