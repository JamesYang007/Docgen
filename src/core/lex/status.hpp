#pragma once
#include <queue>

namespace docgen {
namespace core {
namespace lex {

template <class TokenType>
struct Status
{
    using token_t = TokenType;
    using token_arr_t = std::queue<token_t>;

    token_arr_t tokens;
};

} // namespace lex
} // namespace core
} // namespace docgen
