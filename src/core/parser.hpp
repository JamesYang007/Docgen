#pragma once

#include <vector>
#include <initializer_list>
#include <nlohmann/json.hpp>
#include "core/json_writer.hpp"
#include "core/parse_worker.hpp"
#include "core/parser_internal/docgen_worker.hpp"
#include "core/parser_internal/comment_worker.hpp"
#include "core/parser_internal/func_worker.hpp"
#include "core/parser_internal/ignore_worker.hpp"
#include "core/parser_internal/tag_worker.hpp"

namespace docgen {
namespace core {

class Parser
{
	public:
		using CommentWorker = parser_internal::CommentWorker;
		using FuncWorker = parser_internal::FuncWorker;
		using IgnoreWorker = parser_internal::IgnoreWorker;
		using TagWorker = parser_internal::TagWorker;
		using symbol_t = parser_internal::symbol_internal_t;

		Parser()
			: worker_ {
				CommentWorker(symbol_t::BEGIN_LINE_COMMENT, symbol_t::NEWLINE, { TagWorker() }),
				CommentWorker(symbol_t::BEGIN_BLOCK_COMMENT, symbol_t::END_BLOCK_COMMENT, {
					TagWorker(),
					IgnoreWorker(symbol_t::NEWLINE, symbol_t::STAR).until_not()
				}),
				FuncWorker()
			}
		{}

		using token_t = parser_internal::token_internal_t;
		using token_arr_t = std::vector<token_t>;

		void process(const token_arr_t& tokens);

		const nlohmann::json& parsed() { return writer_.stored(); }

	private:	
		parser_internal::worker_internal_t worker_;
		parser_internal::writer_internal_t writer_;
};

inline void Parser::process(const token_arr_t& tokens)
{
	writer_.reset();

	for (const token_t& token : tokens) {
		worker_.proc(token, writer_);
		writer_.feed(token);
	}
}

} // namespace docgen
} // namespace core
