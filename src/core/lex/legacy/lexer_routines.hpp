#pragma once
#include "core/lex/legacy/token.hpp"
#include "core/lex/legacy/status.hpp"
#include "core/lex/legacy/symbol.hpp"
#include "io/file_reader.hpp"
#include "core/tag_set.hpp"

namespace docgen {
namespace core {
namespace lex {
namespace legacy {

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
// Returns leading whitespace count of original line.
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
// (Trimmed) text is only tokenized if it is non-empty.
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
// then text is first tokenized then the single-char special token.
// The tokens are appended to status.tokens in this order.
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

    // if valid tag, append text token then token with tag name
    if (tag_set.find(tagname) != tag_set.end()) {
        tokenize_text(text, status);
        status.tokens.emplace_back(symbol_t::TAGNAME, std::move(tagname));
    }

    // otherwise, assume part of text: append "@" then tag name to text 
    else {
        text.push_back('@');
        text.append(tagname);
    }
}

// If c is '@', try to tokenize tag name.
// Behavior is the same as tokenize_tag_name.
// Returns true if and only if c is '@'.
inline bool process_tag_name(int c, std::string& text, 
                             file_reader& reader, status_t& status)
{
    if (c == '@') {
        tokenize_tag_name(text, reader, status);
        return true;
    }
    return false;
}

void process_line_comment(std::string& text, file_reader& reader, status_t& status);
void process_block_comment(std::string& text, file_reader& reader, status_t& status);
bool process_string(int c, std::string& text,
                           file_reader& reader, status_t& status);
void process(file_reader& reader, status_t& status);

} // namespace legacy
} // namespace lex
} // namespace core
} // namespace docgen
