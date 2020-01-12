#pragma once

#include <initializer_list>
#include <nlohmann/json.hpp>
#include "parse_feeder.hpp"
#include "parse_worker.hpp"
#include "comments_parser.hpp"
#include "ignore_parser.hpp"

namespace docgen {
namespace core {

class Parser : private ParseWorker
{
	public:
		Parser()
			: ParseWorker {
				CommentParser(Symbol::BEGIN_LINE_COMMENT, Symbol::NEWLINE),
				CommentParser(Symbol::BEGIN_BLOCK_COMMENT, Symbol::END_BLOCK_COMMENT, {
					TagParser(),
					IgnoreParser(Symbol::NEWLINE, Symbol::STAR)
				})
			}
		{}

		void process(const std::vector<token_t>& tokens);

		const nlohmann::json& parsed() const { return parsed_; }

	private:
		nlohmann::json parsed_;
};

void Parser::process(const std::vector<token_t>& tokens)
{
	ParseFeeder::reset();

	for (const token_t& token : tokens) {
		proc(token);
		ParseFeeder::feed(token);
	}

	parsed_ = ParseFeeder::move();
}

} // namespace docgen
} // namespace core
