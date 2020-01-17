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
		throw exceptions::file_open_error(path);
	}

	core::Lexer lexer(file);
	lexer.process();

	fclose(file);

	core::Parser parser;
	parser.process(lexer.get_tokens());

	if (!parser.parsed().is_null()) {
		parsed.push_back(std::move(parser.parsed()));
		parsed.back()["name"] = path;
	}
}

} // namespace docgen
