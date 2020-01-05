#pragma once
#include <string>

namespace docgen {
namespace core {

template <class EnumType>
struct Token
{
    using symbol_t = EnumType;

    Token(symbol_t name)
        : name(name)
    {}

    Token(symbol_t name, const std::string& content)
        : name(name)
        , content(content)
    {}

    Token(symbol_t name, std::string&& content)
        : name(name)
        , content(std::move(content))
    {}

    symbol_t name;
    std::string content;
};

} // namespace core
} // namespace docgen
