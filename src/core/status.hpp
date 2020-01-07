#pragma once
#include <vector>

namespace docgen {
namespace core {

template <class TokenType>
struct Status
{
    using token_t = TokenType;
    using token_arr_t = std::vector<token_t>;

    const token_t& get_last_token() const
    {
        return tokens[tokens.size() - 1];
    }

    token_arr_t tokens;
};

} // namespace core
} // namespace docgen
