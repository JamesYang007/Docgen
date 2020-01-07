#pragma once
#include <string>

namespace docgen {
namespace core {

template <class EnumType>
struct Token
{
    using value_type = EnumType;

    Token(value_type name)
        : name(name)
    {}

    Token(value_type name, const std::string& content)
        : name(name)
        , content(content)
    {}

    Token(value_type name, std::string&& content)
        : name(name)
        , content(std::move(content))
    {}

    value_type name;
    std::string content;
};

} // namespace core
} // namespace docgen
