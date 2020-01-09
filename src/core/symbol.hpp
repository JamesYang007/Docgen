#pragma once
#include <mapbox/eternal.hpp>

namespace docgen {
namespace core {

enum class Symbol {
    // single-char tokens
    END_OF_FILE,
    NEWLINE,
    SEMICOLON,
    OPEN_BRACE,
    CLOSE_BRACE,
    // string tokens
    BEGIN_LINE_COMMENT,
    END_LINE_COMMENT = NEWLINE,   // may not be used
    BEGIN_BLOCK_COMMENT,
    END_BLOCK_COMMENT,
    // special tags
    TAGNAME,
    // default
    TEXT
};

// Compile-time mapping of strings to corresponding symbol
static MAPBOX_ETERNAL_CONSTEXPR const auto symbol_map = 
    mapbox::eternal::map<Symbol, mapbox::eternal::string>({
            {Symbol::NEWLINE, "\n"},
            {Symbol::SEMICOLON, ";"},
            {Symbol::OPEN_BRACE, "{"},
            {Symbol::CLOSE_BRACE, "}"},
            {Symbol::BEGIN_LINE_COMMENT, "///"},
            {Symbol::END_LINE_COMMENT, "\n"},
            {Symbol::BEGIN_BLOCK_COMMENT, "/*!"},
            {Symbol::END_BLOCK_COMMENT, "*/"},
    });
 
} // namespace core
} // namespace docgen
