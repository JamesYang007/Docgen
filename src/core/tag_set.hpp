#pragma once
#include <unordered_set>
#include <string>

namespace docgen {
namespace core {

static const std::unordered_set<std::string> tag_set = {
            "sdesc",
            "param",
            "tparam",
            "return",
            "section",
            "end_section",
            "example",
            "end_example"
    };

} // namespace core
} // namespace docgen
