#pragma once

#include <initializer_list>
#include <nlohmann/json.hpp>
#include "parse_feeder.hpp"
#include "parse_worker.hpp"
#include "comment_worker.hpp"
#include "ignore_worker.hpp"

namespace docgen {
namespace core {

class Parser
{
	public:
		Parser()
			: worker_ {
				CommentWorker(Symbol::BEGIN_LINE_COMMENT, Symbol::NEWLINE),
				CommentWorker(Symbol::BEGIN_BLOCK_COMMENT, Symbol::END_BLOCK_COMMENT, {
					IgnoreWorker(Symbol::NEWLINE, Symbol::STAR)
				})
			}
		{}

		void process(const std::vector<token_t>& tokens);

		const nlohmann::json& parsed() { return feeder_.parsed(); }

	private:
		ParseWorker worker_;
		ParseFeeder feeder_;
};

inline void Parser::process(const std::vector<token_t>& tokens)
{
	feeder_.reset();

	for (const token_t& token : tokens) {
		worker_.proc(token, feeder_);
		feeder_.feed(token);
	}
}

} // namespace docgen
} // namespace core
