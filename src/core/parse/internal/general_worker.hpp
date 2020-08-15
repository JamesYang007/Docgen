#pragma once

#include "core/parse/details.hpp"
#include "core/parse/internal/comment_worker.hpp"
#include "core/parse/internal/func_worker.hpp"
#include "core/parse/internal/template_worker.hpp"
#include "core/parse/internal/ignore_worker.hpp"
#include "core/parse/internal/tag_worker.hpp"

namespace docgen {
namespace core {
namespace parse {
namespace internal {

class GeneralWorker : public worker_t
{
	public:
		GeneralWorker()
			: worker_t {
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
				TemplateWorker(),
				FuncWorker()
			}
		{}
};

} // namespace internal
} // namespace parse
} // namespace core
} // namespace docgen
