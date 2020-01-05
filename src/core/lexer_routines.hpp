#pragma once
#include "token.hpp"
#include "file_io/file_reader.hpp"

namespace docgen {
namespace core {

static constexpr size_t DEFAULT_STRING_RESERVE_SIZE = 20;

enum class DocSymbol {
    line_comment,
    block_comment,
    func_declaration,
    class_declaration,
    eof
};

using file_reader = file_io::file_reader;
using token_t = Token<DocSymbol>;

// TODO: not entirely correct
inline token_t declaration(file_reader& reader)
{
    std::string line;
    line.reserve(DEFAULT_STRING_RESERVE_SIZE);
    char c = 0;

    while ((c = reader.read()) && 
           ((c != ';') || (c != '{'))) {
        line.push_back(c);
    }

    // class declaration
    if ((line.find("class") != std::string::npos) ||
        (line.find("struct") != std::string::npos))
    {
        return token_t(DocSymbol::class_declaration, std::move(line)); 
    }

    // function declaration
    return token_t(DocSymbol::func_declaration, std::move(line));
}

inline token_t line_comment(file_reader& reader)
{
    std::string line;
    line.reserve(DEFAULT_STRING_RESERVE_SIZE);
    char c = 0;

    while ((c = reader.read()) && (c != '\n')) {
        line.push_back(c);
    }

    return token_t(DocSymbol::line_comment, std::move(line));
}

inline token_t block_comment(file_reader& reader)
{
    std::string line;
    line.reserve(DEFAULT_STRING_RESERVE_SIZE);
    char c = 0;

    while ((c = reader.read())) {
        if ((c == '*') && (reader.peek() == '/')) {
            break;
        }
        line.push_back(c);
    }

    return token_t(DocSymbol::block_comment, std::move(line));
}

inline token_t process(file_reader& reader)
{
    // 1 if last symbol was declaration, 0 otherwise
    // set to -1 by default in the beginning.
    static int last_sym_decl = -1; 
    char c = 0;

    while ((c = reader.read())) {

        if (c == '/') {
            c = reader.read();
            if (c == '/') {
                c = reader.read();
                if (c == '/') {
                    return line_comment(reader);
                }
            }

            else if (c == '*') {
                c = reader.read();
                if (c == '!') {
                    return block_comment(reader);
                }
            }
        }

        // if last symbol were a comment and
        // c is alpha, assume it is the beginning of a declaration
        if (last_sym_decl == 0 && std::isalpha(c)) {
            last_sym_decl = 1; 
            return declaration(reader); 
        }
    }

    return token_t(DocSymbol::eof);
}

} // namespace core
} // namespace docgen
