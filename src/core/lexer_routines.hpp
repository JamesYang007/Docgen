#pragma once
#include "token.hpp"
#include "io/file_reader.hpp"
#include "io/string_reader.hpp"
#include "status.hpp"
#include <mapbox/eternal.hpp>

#include <iostream>

namespace docgen {
namespace core {

static constexpr size_t DEFAULT_STRING_RESERVE_SIZE = 20;

enum class LexerContext {
    NONE, 
    BEGIN_DECL
    //LINE_COMMENT,
    //BLOCK_COMMENT,
    //FUNC_DECLARATION,
    //CLASS_DECLARATION
};

enum class Symbol {
    END_OF_FILE,
    // tag names
    DESC,
    SDESC,
    TPARAM,
    PARAM,
    RETURN,
    TYPE,
    SECTION,
    EXAMPLE,
    // tag information
    TAGINFO
};

// Compile-time mapping of strings to corresponding symbol
MAPBOX_ETERNAL_CONSTEXPR const auto tag_map = 
    mapbox::eternal::map<mapbox::eternal::string, Symbol>({
            {"desc", Symbol::DESC},
            {"sdesc", Symbol::SDESC},
            {"tparam", Symbol::TPARAM},
            {"param", Symbol::PARAM},
            {"return", Symbol::RETURN},
            {"type", Symbol::TYPE},
            {"section", Symbol::SECTION},
            {"example", Symbol::EXAMPLE}
    });

using file_reader = io::file_reader;
using string_reader = io::string_reader;
using token_t = Token<Symbol>;
using context_t = LexerContext;
using status_t = Status<token_t, context_t>;

// ignores read chars until func(c) false or reading terminates.
// returns the last char read that terminated the function
template <class Reader, class Termination>
int ignore_until(Reader& reader, Termination func)
{
    int c = 0;
    while (((c = reader.read()) != Reader::termination) && func(c));
    return c;
}

// ignores read chars until func(c) false or reading terminates.
template <class Reader, class Termination>
std::string read_until(Reader& reader, Termination func)
{
    int c = 0;
    std::string line;
    line.reserve(DEFAULT_STRING_RESERVE_SIZE);
    while (((c = reader.read()) != Reader::termination) && func(c)) {
        line.push_back(c);
    }
    return line;
}

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
//        return token_t(token_t::symbol_t::class_declaration, std::move(line)); 
//    }
//
//    // function declaration
//    return token_t(token_t::symbol_t::func_declaration, std::move(line));
//}

// If tag name is not a valid one and there exists a previous tag, 
// we assume current "tag name" is part of previous tag information.
template <class Reader>
inline void process_tag_name(Reader& reader, status_t& status)
{
    auto is_not_space = [](char x) {return x != ' ';};

    // parse tag
    std::string&& tagname = read_until(reader, is_not_space);

    // if valid tag, append token with tag name
    if (tag_map.contains(tagname.c_str())) {
        // Note: this also signifies the beginning of some declaration to document
        // if context has not been set yet.
        if (status.context == context_t::NONE) {
            status.context = context_t::BEGIN_DECL;
        }
        status.tokens.emplace_back(tag_map.at(tagname.c_str()), std::move(tagname));
    }

    // otherwise, assume part of previous tag's info only if context is in some declaration
    else if (status.context == context_t::BEGIN_DECL){
        tagname.insert(0, "@");
        tagname.push_back(' ');
        status.tokens.emplace_back(token_t::symbol_t::TAGINFO, std::move(tagname));
    }
}

template <class Reader>
inline void process_tag_info(Reader& reader, status_t& status)
{
    auto is_not_at = [](char x) {return x != '@';};

    // search for @ symbol
    // if no need to save initial portion of string before @ 
    if (status.context == status_t::context_t::NONE) {
        ignore_until(reader, is_not_at);
        return;
    }

    std::string&& info = read_until(reader, is_not_at);
    // make a token only if info is nonempty
    if (!info.empty()) {
        status.tokens.emplace_back(token_t::symbol_t::TAGINFO, std::move(info));
    }
}

// reader should read a string that has been trimmed, i.e.,
// preceeding and succeeding whitespaces have been removed.
template <class Reader>
inline void process_tags(Reader& reader, status_t& status)
{
    // process (possibly trailing) tag information 
    process_tag_info(reader, status);

    // if terminated, no need to read further
    if (reader.peek() == Reader::termination) {
        return;
    }

    // process tag name
    process_tag_name(reader, status);

    // if terminated, no need to read further
    if (reader.peek() == Reader::termination) {
        return;
    }

    // recurse to process tags in rest of the line
    process_tags(reader, status);
}

template <class Reader>
inline void line_comment(Reader& reader, status_t& status)
{
    auto is_not_newline = [](char x) {return (x != '\n');};
    auto is_space_not_newline = [](char x) {return isspace(x) && (x != '\n');};

    // ignore leading whitespace except newline
    int c = ignore_until(reader, is_space_not_newline);

    // move back one char
    reader.back(c); 

    // current char is whitespace, do nothing
    if (isspace(c)) {
        return; 
    }

    // read line until newline (non-inclusive)
    std::string&& line = read_until(reader, is_not_newline);

    // ignore trailing whitespace
    const auto end = line.find_last_not_of(" \t\n\v\f\r");
    string_reader str_reader(line.substr(0, end + 1));

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
//    return token_t(token_t::symbol_t::block_comment, std::move(line));
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
                    line_comment(reader, status);
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

    status.tokens.emplace_back(token_t::symbol_t::END_OF_FILE);
}

} // namespace core
} // namespace docgen
