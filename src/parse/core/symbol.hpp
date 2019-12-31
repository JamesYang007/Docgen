#pragma once 
#include <cstddef> // size_t
#include <array>

namespace docgen {
namespace parse {
namespace core {

// Symbol wraps an array that is used to store
// a word that needs to be remembered across batches.
// N is the size of the array.
template <size_t N>
class Symbol
{
public:
    // insert char in the next unassigned spot (O(1))
    void push_back(char x) 
    {
        assert(end_ < N);
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
    }

private:
    using buf_t = std::array<char, N + 1>;
    buf_t buf_;
    uint32_t end_ = 0;
};

} // namespace core
} // namespace parse
} // namespace docgen
