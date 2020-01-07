#pragma once
#include <string>

namespace docgen {
namespace io {

struct string_reader
{
    string_reader(std::string&& str)
        : str_(std::move(str))
        , end_(str.length())
    {}

    char read()
    {
        char c = str_[begin_];
        if (begin_ != end_) {
            ++begin_;
        }
        return c;
    }

    void back(char c)
    {
        if (begin_ == end_) {
            --begin_;
            return;
        }
        str_[begin_--] = c;
    }

    char peek() 
    {
        return str_[begin_];
    }

    static constexpr int termination = 0;

private:
    std::string str_;
    int begin_ = 0;
    int end_ = 0;
};
    
} // namespace io
} // namespace docgen
