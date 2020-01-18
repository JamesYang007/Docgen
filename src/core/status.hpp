#pragma once
#include <queue>
#include <string>

namespace docgen {
namespace core {

template <class TokenType>
struct Status
{
    using token_t = TokenType;
    using token_arr_t = std::queue<token_t>;

    token_arr_t tokens;
};

} // namespace core
} // namespace docgen
