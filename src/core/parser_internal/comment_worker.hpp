#pragma once

#include "core/parser_internal/docgen_worker.hpp"
#include "core/parser_internal/tag_worker.hpp"

namespace docgen {
namespace core {
namespace parser_internal {

class CommentWorker : public worker_t
{
	public:
		using worker_t = typename parser_internal::worker_t;
		using token_t = typename worker_t::token_t;

		CommentWorker(token_t open, token_t close, worker_arr_init_t workers={})
			: worker_t {
				TokenHandler(open, Routines::on_open_),
				TokenHandler(close, Routines::on_close_, workers)
			}
		{}

	private:
		static constexpr const char *DEFAULT_KEY = "desc";

		struct Routines : private routine_details_t
		{
			using token_t = typename routine_details_t::token_t;

			static constexpr const routine_t on_open_ = [](worker_t *, const token_t& t, dest_t& writer) {
				if (!writer.key_set()) {
					writer.set_key(DEFAULT_KEY);
				}
				writer.start_writing();
				writer.skip_write();
			};
			static constexpr const routine_t on_close_ = [](worker_t *, const token_t&, dest_t& writer) {
				writer.stop_writing();
			};		
		};
};

} // namespace parser_internal
} // namespace core
} // namespace docgen
