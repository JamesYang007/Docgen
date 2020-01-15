#pragma once

#include <vector>
#include <initializer_list>
#include <nlohmann/json.hpp>
#include "json_writer.hpp"
#include "parse_worker.hpp"
#include "parser_internal/docgen_worker.hpp"
#include "parser_internal/comment_worker.hpp"
#include "parser_internal/func_worker.hpp"
#include "parser_internal/ignore_worker.hpp"
#include "parser_internal/tag_worker.hpp"

namespace docgen {
namespace core {

class Parser
{
	public:
		using CommentWorker = parser_internal::CommentWorker;
		using FuncWorker = parser_internal::FuncWorker;
		using IgnoreWorker = parser_internal::IgnoreWorker;
		using TagWorker = parser_internal::TagWorker;

		Parser()
			: worker_ {
				CommentWorker(Symbol::BEGIN_LINE_COMMENT, Symbol::NEWLINE, { TagWorker() }),
				CommentWorker(Symbol::BEGIN_BLOCK_COMMENT, Symbol::END_BLOCK_COMMENT, {
					TagWorker(),
					IgnoreWorker(Symbol::NEWLINE, Symbol::STAR).until_not()
				}),
				FuncWorker()
			}
		{}

		using token_t = parser_internal::token_class_t;
		using token_arr_t = std::vector<token_t>;

		void process(const token_arr_t& tokens);

		const nlohmann::json& parsed() { return writer_.stored(); }

	private:	
		parser_internal::worker_class_t worker_;
		parser_internal::writer_class_t writer_;
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
