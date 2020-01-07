#pragma once
#include <vector>

namespace docgen {
namespace core {

template <class TokenType, class ContextType>
struct Status
{
    using token_t = TokenType;
    using context_t = ContextType;
    using token_arr_t = std::vector<token_t>;

    const token_t& get_last_token() const
    {
        return tokens[tokens.size() - 1];
    }

    token_arr_t tokens;
    context_t context = context_t::NONE;
};

} // namespace core
} // namespace docgen
