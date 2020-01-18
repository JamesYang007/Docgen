#pragma once
#include <queue>

namespace docgen {
namespace core {
namespace lexer {

template <class TokenType>
struct Status
{
    using token_t = TokenType;
    using token_arr_t = std::queue<token_t>;

    token_arr_t tokens;
};

} // namespace lexer
} // namespace core
} // namespace docgen
