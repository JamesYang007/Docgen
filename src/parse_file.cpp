#include <cstdio>
#include <string>
#include "exceptions/exceptions.hpp"
#include "core/lexer/lexer.hpp"
#include "core/parse/parser.hpp"
#include "parse_file.hpp"

namespace docgen {

static constexpr const char * const FILENAME_KEY = "name";

static constexpr size_t BUF_SZ = 4096;

void parse_file(const char *path, nlohmann::json& parsed)
{
	FILE *file = fopen(path, "r");
	if (file == nullptr) {
		throw exceptions::file_open_error(path);
	}

	core::lexer::Lexer lexer;
	core::parse::Parser parser;

	char buf[BUF_SZ];
	size_t r, i;
	std::optional<core::lexer::Lexer::token_t> token;
	while ((r = fread(buf, 1, BUF_SZ, file))) {
		for (i = 0; i < r; ++i) {
			lexer.process(buf[i]);
			if ((token = lexer.next_token())) {
				parser.process(*token);
			}
		}
	}
	if (ferror(file)) {
		throw exceptions::system_error("fread() failed on file at path \"" + std::string(path) + '\"');
	}
	fclose(file);

	if (!parser.parsed().is_null()) {
		parsed.push_back(std::move(parser.parsed()));
		parsed.back()[FILENAME_KEY] = path;
	}
}

} // namespace docgen
