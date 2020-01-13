#pragma once

#include "parse_feeder.hpp"
#include "parse_worker.hpp"
#include "tags_parser.hpp"

namespace docgen {
namespace core {

struct CommentParser_routines
{
	private:
		static constexpr const char *DEFAULT_TAG = "desc";

	protected:
		using routine_t = ParseWorker::SymbolHandler::routine_t;

		static constexpr const routine_t on_open_ = [](const token_t&, ParseFeeder& f) {
			if (!f.at()) {
				f.at(DEFAULT_TAG);
			}
			f.go();
			f.skip();
		};
		static constexpr const routine_t on_close_ = [](const token_t&, ParseFeeder& f) {
			f.stop();
		};
};

class CommentParser : public ParseWorker, private CommentParser_routines
{
	public:
		CommentParser(symbol_t open, symbol_t close, worker_arr_init_t extra_workers={})
			: ParseWorker {
				SymbolHandler(open, on_open_),
				SymbolHandler(close, on_close_, {
					TagParser(),
					extra_workers
				})
			}
		{}
};

} // namespace core
} // namespace docgen
