#include "lexer.hpp"
#include "parser.hpp"
#include "lex_N_parse.hpp"

using namespace docgen::core;

void parse_file(const char *path, nlohmann::json *parsed)
{
	Lexer lexer(path);
	lexer.process();

	Parser parser;
	parser.process(lexer.get_tokens());

	parsed->push_back(std::move(parser.parsed()));
	parsed->back()["name"] = path;
}
