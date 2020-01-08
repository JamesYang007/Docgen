#pragma once
#include <mapbox/eternal.hpp>

namespace docgen {
namespace core {

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
    mapbox::eternal::map<Symbol, mapbox::eternal::string>({
            {Symbol::DESC, "desc"},
            {Symbol::SDESC, "sdesc"},
            {Symbol::TPARAM, "tparam"},
            {Symbol::PARAM, "param"},
            {Symbol::RETURN, "return"},
            {Symbol::TYPE, "type"},
            {Symbol::SECTION, "section"},
            {Symbol::EXAMPLE, "example"}
    });
 
} // namespace core
} // namespace docgen
