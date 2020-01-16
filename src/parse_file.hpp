#pragma once

#include <nlohmann/json.hpp>

namespace docgen {

void parse_file(const char *path, nlohmann::json& parsed);

} // namespace docgen
