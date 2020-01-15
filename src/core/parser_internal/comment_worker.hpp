#pragma once

#include "docgen_worker.hpp"
#include "tag_worker.hpp"

namespace docgen {
namespace core {
namespace parser_internal {

struct CommentWorker_routines : private routine_details_internal_t
{
	private:
		static constexpr const char *DEFAULT_KEY = "desc";

	protected:
		static constexpr const routine_t on_open_ = [](worker_t *, const tok_t& t, dest_t& writer) {
			if (!writer.key_set()) {
				writer.set_key(DEFAULT_KEY);
			}
			writer.start_writing();
			writer.skip_write();
		};
		static constexpr const routine_t on_close_ = [](worker_t *, const tok_t&, dest_t& writer) {
			writer.stop_writing();
		};
};

class CommentWorker : public worker_internal_t, private CommentWorker_routines
{
	public:
		CommentWorker(token_t open, token_t close, worker_arr_init_t workers={})
			: ParseWorker {
				TokenHandler(open, on_open_),
				TokenHandler(close, on_close_, workers)
			}
		{}
};

} // namespace parser_internal
} // namespace core
} // namespace docgen
