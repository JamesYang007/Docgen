#pragma once
#include <mapbox/eternal.hpp>
#include <unordered_set>
#include <string>

namespace docgen {
namespace core {

enum class Symbol {
    // single-char tokens
    END_OF_FILE,
    NEWLINE,
    WHITESPACE,
    SEMICOLON,
    STAR,
    OPEN_BRACE,
    CLOSE_BRACE,
    // string tokens
    BEGIN_SLINE_COMMENT,
    BEGIN_SBLOCK_COMMENT,
    BEGIN_NLINE_COMMENT,
    BEGIN_NBLOCK_COMMENT,
    END_BLOCK_COMMENT,
    // special tags
    SDESC,
    // default
    TEXT
};

// Compile-time mapping of strings to corresponding symbol
static MAPBOX_ETERNAL_CONSTEXPR const auto symbol_map = 
    mapbox::eternal::map<Symbol, mapbox::eternal::string>({
            {Symbol::SEMICOLON, ";"},
            {Symbol::STAR, "*"},
            {Symbol::OPEN_BRACE, "{"},
            {Symbol::CLOSE_BRACE, "}"},
            {Symbol::BEGIN_SLINE_COMMENT, "///"},
            {Symbol::BEGIN_SBLOCK_COMMENT, "/*!"},
            {Symbol::BEGIN_NLINE_COMMENT, "//"},
            {Symbol::BEGIN_NBLOCK_COMMENT, "/*"},
            {Symbol::END_BLOCK_COMMENT, "*/"},
    });

} // namespace core
} // namespace docgen
