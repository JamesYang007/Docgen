#pragma once
#include "lexer_routines.hpp"

namespace docgen {
namespace core {

static constexpr size_t DEFAULT_TOKEN_ARR_SIZE = 50;

struct Lexer
{
    // token_t, status_t defined in lexer_routines.hpp
    using value_type = typename token_t::value_type;

    Lexer(const char* filepath)
        : reader_(filepath)
    {
        status_.tokens.reserve(DEFAULT_TOKEN_ARR_SIZE);
    }

    void process()
    {
        while (reader_.peek() != EOF) {
            status_.tokens.emplace_back(core::process(reader_, status_));
        }

        if (status_.tokens[status_.tokens.size() - 1].name != value_type::eof) {
            status_.tokens.emplace_back(value_type::eof);
        }
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
