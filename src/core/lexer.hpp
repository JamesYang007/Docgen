#pragma once
#include "lexer_routines.hpp"

namespace docgen {
namespace core {

static constexpr size_t DEFAULT_TOKEN_ARR_SIZE = 50;

struct Lexer
{
    // token_t, status_t defined in lexer_routines.hpp
    using symbol_t = typename token_t::symbol_t;

    Lexer(const char* filepath)
        : reader_(filepath)
    {
        status_.tokens.reserve(DEFAULT_TOKEN_ARR_SIZE);
    }

    void process()
    {
        core::process(reader_, status_);
    }
    
    const status_t::token_arr_t& get_tokens() const
    {
        return status_.tokens;
    }

private:
    io::file_reader reader_;
    status_t status_; // keeps track of last token value (enum)
};

} // namespace core
} // namespace docgen
