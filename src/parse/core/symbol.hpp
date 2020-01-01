#pragma once 
#include <cstddef> // size_t
#include <vector>

namespace docgen {
namespace parse {
namespace core {

// Symbol wraps an array that is used to store
// a word that needs to be remembered across batches.
// N is the size of the array.
class Symbol
{
public:

    explicit Symbol(uint32_t symbol_size) noexcept
        : buf_(symbol_size + 1, 0), max_size_(symbol_size)
    {}

    // insert char in the next unassigned spot (O(1))
    void push_back(char x) 
    {
        assert(end_ < max_size_);
        buf_[end_] = x;
        ++end_;
        buf_[end_] = 0; // null-terminate
    }

    // null-terminate string and return address of first char (O(1))
    const char* get() const 
    {
        return &buf_[0];
    }

    // clear string (O(1))
    void clear()
    {
        end_ = 0;
        buf_[end_] = 0; // null-terminate
    }

private:
    using buf_t = std::vector<char>;
    buf_t buf_;
    uint32_t max_size_;
    uint32_t end_ = 0;
};

} // namespace core
} // namespace parse
} // namespace docgen
