#pragma once

#include <initializer_list>
#include <nlohmann/json.hpp>
#include "core/parse/details.hpp"
#include "core/parse/internal/comment_worker.hpp"
#include "core/parse/internal/func_worker.hpp"
#include "core/parse/internal/ignore_worker.hpp"
#include "core/parse/internal/tag_worker.hpp"

namespace docgen {
namespace core {
namespace parse {

class Parser
{
	public:
		using CommentWorker = internal::CommentWorker;
		using FuncWorker = internal::FuncWorker;
		using IgnoreWorker = internal::IgnoreWorker;
		using TagWorker = internal::TagWorker;

		Parser()
			: worker_ {
				IgnoreWorker(symbol_t::HASHTAG, symbol_t::NEWLINE),
				IgnoreWorker(symbol_t::BEGIN_NLINE_COMMENT, symbol_t::NEWLINE),
				IgnoreWorker(symbol_t::BEGIN_NBLOCK_COMMENT, symbol_t::END_BLOCK_COMMENT).ignore_last(),
				CommentWorker(symbol_t::BEGIN_SLINE_COMMENT, symbol_t::NEWLINE, {
					TagWorker(),
					IgnoreWorker(
						{ symbol_t::BEGIN_SLINE_COMMENT, symbol_t::BEGIN_NLINE_COMMENT, symbol_t::FORWARD_SLASH },
						symbol_t::END_OF_FILE
					).clear_key().timeout(1).limit(1)
				}),
				CommentWorker(symbol_t::BEGIN_SBLOCK_COMMENT, symbol_t::END_BLOCK_COMMENT, {
					TagWorker(),
					IgnoreWorker(symbol_t::NEWLINE, { symbol_t::STAR, symbol_t::WHITESPACE, symbol_t::NEWLINE }).until_not()
				}),
				FuncWorker()
			}
		{}

		void process(const token_t& token);

		nlohmann::json& parsed() { return writer_.stored(); }

	private:	
		worker_t worker_;
		writer_t writer_;
};

inline void Parser::process(const token_t& token)
{
	worker_.proc(token, writer_);
	writer_.feed(token.c_str());
}

}
} // namespace docgen
} // namespace core
