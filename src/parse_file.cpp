#include "parse_file.hpp"

namespace docgen {

using namespace core;

void parse_file(const char *path, nlohmann::json& parsed)
{
	Lexer lexer(path);
	lexer.process();

	Parser parser;
	parser.process(lexer.get_tokens());

	parsed.push_back(std::move(parser.parsed()));
	parsed.back()["name"] = path;
}

} // namespace docgen
