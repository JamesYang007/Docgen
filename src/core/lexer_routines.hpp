#pragma once
#include "token.hpp"
#include "status.hpp"
#include "symbol.hpp"
#include "io/file_reader.hpp"
#include "tag_set.hpp"
#include <iostream>

namespace docgen {
namespace core {

static constexpr size_t DEFAULT_STRING_RESERVE_SIZE = 50;

using file_reader = io::file_reader;
using symbol_t = Symbol;
using token_t = Token<symbol_t>;
using status_t = Status<token_t>;

// Reads and ignores chars until func(c) evaluates to false or reading terminates,
// where c is the current char read.
// Returns the last char read that terminated the function.
template <class Termination>
inline int ignore_until(file_reader& reader, Termination func)
{
    int c = 0;
    while (((c = reader.read()) != file_reader::termination) && func(c));
    return c;
}

// Reads and stores chars until func(c) evaluates to false or reading terminates,
// where c is the current char read.
// Returns the last char read that terminated the function.
template <class Termination>
inline int read_until(file_reader& reader, Termination func, std::string& line)
{
    int c = 0;
    line.reserve(DEFAULT_STRING_RESERVE_SIZE);
    while (((c = reader.read()) != file_reader::termination) && func(c)) {
        line.push_back(c);
    }
    return c;
}

// Trims all leading and trailing whitespaces (one of " \t\n\v\f\r") from line.
// Line is directly modified.
inline uint32_t trim(std::string& line)
{
    static constexpr const char* whitespaces = " \t\n\v\f\r";

    // find first non-whitespace
    const auto begin = line.find_first_not_of(whitespaces);

    // find last non-whitespace
    const auto end = line.find_last_not_of(whitespaces);

    // If substring invalid, simply clear line return length of string
    // By symmetry, begin and end will be npos if and only if the string only
    // consists of whitespaces. In this case, the leading whitespace count is 
    // simply the length of the string.
    if (begin == std::string::npos && end == std::string::npos) {
        uint32_t leading_ws_count = line.size();
        line.clear();
        return leading_ws_count;
    }

    // otherwise, replace with substring
    line = line.substr(begin, end - begin + 1);

    return begin; // number of leading whitespaces
}

// Trims text, tokenizes it, clears it, and reserve DEFAULT_STRING_RESERVE_SIZE.
inline void tokenize_text(std::string& text, status_t& status)
{
    // trim whitespaces from text first
    uint32_t leading_whitespace_count = trim(text);
    // tokenize current TEXT only if it is non-empty
    if (!text.empty()) {
        status.tokens.emplace_back(symbol_t::TEXT, std::move(text), leading_whitespace_count);
    }
    // clear and reserve 
    text.clear();
    text.reserve(DEFAULT_STRING_RESERVE_SIZE); 
}

// If c is one of single-char special tokens (see symbol.hpp),
// then text is tokenized and the single-char special token is tokenized afterwards.
// The tokens are appended to status.tokens.
// Otherwise, no operations are performed.
// Returns true if and only if a single-char special token created.
inline bool process_char(int c, std::string& text, status_t& status)
{
    switch (c) {
        case '\n':
            tokenize_text(text, status);
            status.tokens.emplace_back(symbol_t::NEWLINE);
            return true;
        case ';':
            tokenize_text(text, status);
            status.tokens.emplace_back(symbol_t::SEMICOLON);
            return true;
        case '{':
            tokenize_text(text, status);
            status.tokens.emplace_back(symbol_t::OPEN_BRACE);
            return true;
        case '}':
            tokenize_text(text, status);
            status.tokens.emplace_back(symbol_t::CLOSE_BRACE);
            return true;
        default:
            return false;
    }
}

// If tag name is not a valid one, assume it is simply text.
// It is expected that the caller immediately read "@" before calling.
inline void tokenize_tag_name(std::string& text, file_reader& reader, status_t& status)
{
    static constexpr const auto is_alpha = 
        [](char x) {return isalpha(x);};

    // parse tag
    std::string tagname;
    int c = read_until(reader, is_alpha, tagname);
    reader.back(c);

    // if valid tag, append token with tag name
    if (tag_set.find(tagname) != tag_set.end()) {
        tokenize_text(text, status);
        status.tokens.emplace_back(symbol_t::TAGNAME, std::move(tagname));
    }

    // otherwise, assume TEXT: tokenize "@" as TEXT first then tagname as a separate tag
    else {
        text.push_back('@');
        text.append(std::move(tagname));
        //status.tokens.emplace_back(symbol_t::TEXT, std::move(tagname));
    }
}

inline bool process_tag_name(int c, std::string& text, 
                             file_reader& reader, status_t& status)
{
    if (c == '@') {
        tokenize_tag_name(text, reader, status);
        return true;
    }
    return false;
}

// It is expected that caller has read the string "//" immediately before calling.
inline void process_line_comment(std::string& text, file_reader& reader, status_t& status)
{
    static constexpr const auto is_not_newline =
        [](char x) {return x != '\n';};

    int c = reader.read();

    if (c == '/') {
        c = reader.read();
        if (isspace(c)) {
            tokenize_text(text, status);
            status.tokens.emplace_back(symbol_t::BEGIN_LINE_COMMENT);
            reader.back(c); // in case it's a single-char token
        }
        else {
            // no need to read back since c cannot be a whitespace and we ignore anyway
            ignore_until(reader, is_not_newline);
        }
    }

    else {
        reader.back(c); // the character just read may be '\n'
        ignore_until(reader, is_not_newline);
    }
}

// It is expected that caller has read the string "/*" immediately before calling.
inline void process_block_comment(std::string& text, file_reader& reader, status_t& status)
{
    const auto is_not_end_block =
        [&](char x) {return (x != '*') || (reader.peek() != '/');};

    int c = reader.read();

    if (c == '!') {
        c = reader.read();
        // valid block comment: tokenize text then begin block comment symbol
        if (isspace(c)) {
            tokenize_text(text, status);
            status.tokens.emplace_back(symbol_t::BEGIN_BLOCK_COMMENT);
            reader.back(c); // may be special single-char token
        }
        // regular block comment: ignore text until end and stop tokenizing
        else {
            ignore_until(reader, is_not_end_block);
            reader.read(); // read the '/'
        }
    }

    // regular block comment
    else {
        ignore_until(reader, is_not_end_block);
        reader.read(); // read the '/'
    }
}

// If c is not '/', then no operation done and returns false.
// Otherwise, if it's a valid line comment ("/// ") then same as process_line_comment.
// If it's a valid block comment ("/*! ") then same as process_block_comment.
// Otherwise, text is updated to include all characters read.
// In any case, returns true since first char has been processed.
inline bool process_string(int c, std::string& text,
                           file_reader& reader, status_t& status)
{
    // possibly beginning of line or block comment
    if (c == '/') {
        c = reader.read();
        if (c == '/') {
            process_line_comment(text, reader, status);
        }
        else if (c == '*') {
            process_block_comment(text, reader, status);
        }
        else {
            text.push_back('/');
            text.push_back(c);
        }
        return true;
    }

    // possibly ending block comment or a star that can be ignored in the middle of a block comment
    else if (c == '*') {
        c = reader.read();
        if (c == '/') {
            tokenize_text(text, status);
            status.tokens.emplace_back(symbol_t::END_BLOCK_COMMENT);
        }
        else {
            tokenize_text(text, status);
            status.tokens.emplace_back(symbol_t::STAR);
            reader.back(c);
        }
        return true;
    }

    return false;
}

inline void process(file_reader& reader, status_t& status)
{
    std::string text;
    text.reserve(DEFAULT_STRING_RESERVE_SIZE);
    int c = 0;
    bool processed = false;

    while ((c = reader.read()) != file_reader::termination) {

        // process special single-char
        processed = process_char(c, text, status);
        if (processed) {
            continue;
        }

        // process tag name
        processed = process_tag_name(c, text, reader, status);
        if (processed) {
            continue;
        }

        // process string tokens
        processed = process_string(c, text, reader, status);
        if (processed) {
            continue;
        }

        // otherwise, no special symbol -> push to text
        text.push_back(c);
    }

    // tokenize last text then EOF
    tokenize_text(text, status);
    status.tokens.emplace_back(token_t::symbol_t::END_OF_FILE);
}

} // namespace core
} // namespace docgen
