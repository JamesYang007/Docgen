#pragma once

#include <nlohmann/json.hpp>

void parse_file(const char *path, nlohmann::json *parsed);
