#pragma once
#include "token.hpp"
#include "io/file_reader.hpp"
#include "io/string_reader.hpp"
#include "status.hpp"

namespace docgen {
namespace core {

static constexpr size_t DEFAULT_STRING_RESERVE_SIZE = 20;

enum class LexerContext {
    line_comment,
    block_comment,
    func_declaration,
    class_declaration
};

enum class Symbol {
    tag,
    taginfo, 
    eof
};

using file_reader = io::file_reader;
using string_reader = io::string_reader;
using token_t = Token<Symbol>;
using status_t = Status<token_t>;

// TODO: not entirely correct
//inline token_t declaration(file_reader& reader)
//{
//    std::string line;
//    line.reserve(DEFAULT_STRING_RESERVE_SIZE);
//    char c = 0;
//
//    while ((c = reader.read()) && 
//           ((c != ';') || (c != '{'))) {
//        line.push_back(c);
//    }
//
//    // class declaration
//    if ((line.find("class") != std::string::npos) ||
//        (line.find("struct") != std::string::npos))
//    {
//        return token_t(token_t::value_type::class_declaration, std::move(line)); 
//    }
//
//    // function declaration
//    return token_t(token_t::value_type::func_declaration, std::move(line));
//}

template <class Reader>
inline void process_tagname(Reader& reader, status_t& status)
{
    std::string tagname;
    tagname.reserve(DEFAULT_STRING_RESERVE_SIZE);
    while(((c = reader.read()) != Reader::termination) && (c != ' ')) {
        tagname.push_back(c);
    }

    switch ()
}

template <class Reader>
inline void process_tags(Reader& reader, status_t& status)
{
    int c = 0; 

    // search for @ symbol
    while (((c = reader.read()) != Reader::termination) && (c != '@'));

    // if reader terminated
    if (c == Reader::termination) {
        return; 
    }

    // parse tag
}

template <class Reader>
inline void line_comment(Reader& reader, status_t& status)
{
    std::string line;
    line.reserve(DEFAULT_STRING_RESERVE_SIZE);
    int c = 0;

    // ignore leading whitespace except newline
    while (((c = reader.read()) != Reader::termination) && isspace(c) && (c != '\n'));

    // every char in line is a whitespace, nothing to do
    if (c == '\n') {
        return; 
    }

    reader.back(c); // next char to read is now first non-space

    // read line until newline (non-inclusive)
    while (((c = reader.read()) != Reader::termination) && (c != '\n')) {
        line.push_back(c);
    }

    // ignore trailing whitespace
    const auto end = line.find_last_not_of(" \t\n\v\f\r");
    string_reader str_reader(line.substr(0, end));

    // lex tags in current trimmed line
    process_tags(str_reader, status);
}

//inline token_t block_comment(file_reader& reader)
//{
//    std::string line;
//    line.reserve(DEFAULT_STRING_RESERVE_SIZE);
//    int c = 0;
//
//    while ((c = reader.read())) {
//        if ((c == '*') && (reader.peek() == '/')) {
//            break;
//        }
//        line.push_back(c);
//    }
//
//    return token_t(token_t::value_type::block_comment, std::move(line));
//}

template <class Reader>
inline void process(Reader& reader, status_t& status)
{
    // 1 if last symbol was declaration, 0 otherwise
    // set to -1 by default in the beginning.
    // static int last_sym_decl = -1; 
    int c = 0;

    while ((c = reader.read()) != Reader::termination) {

        if (c == '/') {
            c = reader.read();
            if (c == '/') {
                c = reader.read();
                if (c == '/') {
                    status.tokens.emplace_back(line_comment(reader, status));
                }
            }

            // TODO: block comment
            //else if (c == '*') {
            //    c = reader.read();
            //    if (c == '!') {
            //        return block_comment(reader, status);
            //    }
            //}
        }

        // TODO: declaration
        //// if last symbol were a comment and
        //// c is alpha, assume it is the beginning of a declaration
        //if (last_sym_decl == 0 && std::isalpha(c)) {
        //    last_sym_decl = 1; 
        //    return declaration(reader); 
        //}
    }

    status.tokens.emplace_back(token_t::value_type::eof);
}

} // namespace core
} // namespace docgen
