#pragma once

#include <nlohmann/json.hpp>
#include "core/lexer.hpp"
#include "core/parser.hpp"

namespace docgen {

void parse_file(const char *path, nlohmann::json& parsed);

} // namespace docgen
