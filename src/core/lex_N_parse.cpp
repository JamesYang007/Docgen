#include "lexer.hpp"
#include "parser.hpp"
#include "lex_N_parse.hpp"

using namespace docgen;

void parse_file(const char *path, nlohmann::json *parsed)
{
	core::Lexer lexer(path);
	lexer.process();

	core::Parser parser;
	parser.process(lexer.get_tokens());

	parsed->push_back(std::move(parser.parsed()));
	parsed->back()["name"] = path;
}
