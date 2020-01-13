#pragma once
#include "lexer_routines.hpp"

namespace docgen {
namespace core {

struct Lexer
{
    using symbol_t = lexer_details::symbol_t;
    using file_reader = lexer_details::file_reader;
    using status_t = lexer_details::status_t;

    Lexer(FILE* file)
        : reader_(file)
    {
        status_.tokens.reserve(DEFAULT_TOKEN_ARR_SIZE);
    }

    void process()
    {
        lexer_details::process(reader_, status_);
    }
    
    const status_t::token_arr_t& get_tokens() const
    {
        return status_.tokens;
    }

private:
    static constexpr size_t DEFAULT_TOKEN_ARR_SIZE = 50;

    file_reader reader_;
    status_t status_; // keeps track of last token value (enum)
};

} // namespace core
} // namespace docgen
