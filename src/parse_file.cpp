#include <cstdio>
#include <string>
#include "exceptions/exceptions.hpp"
#include "core/lex/lexer.hpp"
#include "core/parse/parser.hpp"
#include "parse_file.hpp"

namespace docgen {

static constexpr const char * const FILENAME_KEY = "name";

static constexpr size_t BUF_SZ = 4096;

void parse_file(const char *path, nlohmann::json& parsed)
{
	// open file
	FILE *file = fopen(path, "r");
	if (file == nullptr) {
		throw exceptions::file_open_error(path);
	}

	// initialize lexer and parser
	core::lex::Lexer lexer;
	core::parse::Parser parser;

	// read file in by chunks
	char buf[BUF_SZ];
	size_t r, i;
	std::optional<core::lex::Lexer::token_t> token;
	while ((r = fread(buf, 1, BUF_SZ, file))) {
		for (i = 0; i < r; ++i) {
			// process lexer on every read character
			lexer.process(buf[i]);
			// process parser on every resolved token
			while ((token = lexer.next_token())) {
				parser.process(*token);
			}
		}
	}
	if (ferror(file)) {
		throw exceptions::system_error("fread() failed on file at path \"" + std::string(path) + '\"');
	}
	fclose(file);

	// flush lexer of remaining tokens process them with parser
	lexer.flush();
	while ((token = lexer.next_token())) {
		parser.process(*token);
	}

	// if anything useful was parsed, move it into the json
	if (!parser.parsed().is_null()) {
		parsed.push_back(std::move(parser.parsed()));
		parsed.back()[FILENAME_KEY] = path;
	}
}

} // namespace docgen
