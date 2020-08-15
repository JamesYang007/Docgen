#pragma once
#include <mapbox/eternal.hpp>

namespace docgen {
namespace core {

enum class Symbol {
    // single-char tokens
    END_OF_FILE,
    NEWLINE,
    WHITESPACE,
    SEMICOLON,
    HASHTAG,
    STAR,
    OPEN_BRACE,
    CLOSE_BRACE,
    OPEN_ANGLE_BRACKET,
    CLOSE_ANGLE_BRACKET,
    FORWARD_SLASH,
    // string tokens
    BEGIN_SLINE_COMMENT,
    BEGIN_SBLOCK_COMMENT,
    BEGIN_NLINE_COMMENT,
    BEGIN_NBLOCK_COMMENT,
    END_BLOCK_COMMENT,
    // language keywords
    TEMPLATE,
    CLASS,
    STRUCT,
    // special tags
    SDESC,
    TPARAM,
    PARAM,
    RETURN,
    // default
    TEXT
};

// Compile-time mapping of strings to corresponding symbol
static MAPBOX_ETERNAL_CONSTEXPR const auto symbol_map = 
    mapbox::eternal::map<Symbol, mapbox::eternal::string>({
            {Symbol::WHITESPACE, " "},
            {Symbol::SEMICOLON, ";"},
            {Symbol::HASHTAG, "#"},
            {Symbol::STAR, "*"},
            {Symbol::OPEN_BRACE, "{"},
            {Symbol::CLOSE_BRACE, "}"},
            {Symbol::OPEN_ANGLE_BRACKET, "<"},
            {Symbol::CLOSE_ANGLE_BRACKET, ">"},
            {Symbol::FORWARD_SLASH, "/"},
            {Symbol::BEGIN_SLINE_COMMENT, "///"},
            {Symbol::BEGIN_SBLOCK_COMMENT, "/*!"},
            {Symbol::BEGIN_NLINE_COMMENT, "//"},
            {Symbol::BEGIN_NBLOCK_COMMENT, "/*"},
            {Symbol::END_BLOCK_COMMENT, "*/"},
            {Symbol::TEMPLATE, "template"},
            {Symbol::CLASS, "class"},
            {Symbol::STRUCT, "struct"},
            {Symbol::SDESC, "sdesc"},
            {Symbol::TPARAM, "tparam"},
            {Symbol::PARAM, "param"},
            {Symbol::RETURN, "return"},
    });

} // namespace core
} // namespace docgen
