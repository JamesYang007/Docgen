#include <cstdio>
#include <string>
#include "exceptions/exceptions.hpp"
#include "core/lexer.hpp"
#include "core/parser.hpp"
#include "parse_file.hpp"

namespace docgen {

void parse_file(const char *path, nlohmann::json& parsed)
{
	FILE *file = fopen(path, "r");
	if (file == nullptr) {
		throw exceptions::system_error("failed to open file at path '" + std::string(path) + '\'');
	}

	core::Lexer lexer(file);
	lexer.process();

	core::Parser parser;
	parser.process(lexer.get_tokens());

	parsed.push_back(std::move(parser.parsed()));
	parsed.back()["name"] = path;

	fclose(file);
}

} // namespace docgen
