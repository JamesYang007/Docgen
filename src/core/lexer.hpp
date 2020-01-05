#pragma once
#include <vector>
#include "lexer_routines.hpp"

namespace docgen {
namespace core {

static constexpr size_t DEFAULT_TOKEN_ARR_SIZE = 50;

struct Lexer
{
    using token_arr_t = std::vector<token_t>;
    using symbol_t = typename token_t::symbol_t;

    Lexer(const char* filepath, 
          size_t buf_size = file_io::DEFAULT_BUF_SIZE)
        : reader(filepath, buf_size)
    {
        tokens.reserve(DEFAULT_TOKEN_ARR_SIZE);
    }

    void process()
    {
        while (reader.peek()) {
            tokens.emplace_back(core::process(reader));
        }

        if (tokens[tokens.size() - 1].name != symbol_t::eof) {
            tokens.emplace_back(symbol_t::eof);
        }
    }
    
    const token_arr_t& get_tokens() const
    {
        return tokens;
    }

private:
    file_io::file_reader reader;
    token_arr_t tokens;
};

} // namespace core
} // namespace docgen
