#pragma once

#include "parse_feeder.hpp"
#include "parse_worker.hpp"
#include "tag_worker.hpp"

namespace docgen {
namespace core {

struct CommentWorker_routines : private WorkerRoutine
{
	private:
		static constexpr const char *DEFAULT_TAG = "desc";

	protected:
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

class CommentWorker : public ParseWorker, private CommentWorker_routines
{
	public:
		CommentWorker(symbol_t open, symbol_t close, worker_arr_init_t extra_workers={})
			: ParseWorker {
				SymbolHandler(open, on_open_),
				SymbolHandler(close, on_close_, {
					TagWorker(),
					extra_workers
				})
			}
		{}
};

} // namespace core
} // namespace docgen
