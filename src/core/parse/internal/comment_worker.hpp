#pragma once

#include "core/parse/details.hpp"
#include "core/parse/internal/tag_worker.hpp"

namespace docgen {
namespace core {
namespace parse {
namespace internal {

class CommentWorker : public worker_t
{
	public:
		using worker_t = typename parse::worker_t;
		using token_t = typename worker_t::token_t;

		CommentWorker(const token_t& open, const token_t& close, worker_arr_init_t workers={})
			: worker_t {
				TokenHandler(open),
				TokenHandler(symbol_t::END_OF_FILE, Routines::on_open_).neg(),
				TokenHandler(close, Routines::on_close_, workers)
			}
		{}

	private:
		static constexpr const char *DEFAULT_KEY = "desc";

		struct Routines : private routine_details_t
		{
			using token_t = typename routine_details_t::token_t;

			static constexpr const routine_t on_open_ = [](worker_t *, const token_t& t, dest_t& writer) {
				if (t == symbol_t::WHITESPACE || t == symbol_t::NEWLINE) {
					if (!writer.key_set()) {
						writer.set_key(DEFAULT_KEY);
					}
					writer.start_writing();
					writer.skip_write();
				}
				else {
					writer.stop_writing();
				}
			};
			static constexpr const routine_t on_close_ = [](worker_t *, const token_t&, dest_t& writer) {
				if (writer.writing()) {
					writer.stop_writing();
				}
				else {
					writer.start_writing();
				}
			};		
		};
};

} // namespace internal
} // namespace parse
} // namespace core
} // namespace docgen
