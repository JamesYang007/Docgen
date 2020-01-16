#pragma once

#include "core/parser_internal/docgen_worker.hpp"

namespace docgen {
namespace core {
namespace parser_internal {

class IgnoreWorker : public worker_t
{
	public:
		using worker_t = parser_internal::worker_t;
		using token_t = typename worker_t::token_t;

		IgnoreWorker(token_t from, token_t until)
			: worker_t {
				TokenHandler(from, Routines::on_start_),
				TokenHandler(until, Routines::on_stop_)
			}
		{}

		IgnoreWorker& recursive() { handlers_[0].on_match(Routines::on_start_recurse_); return *this; }
		IgnoreWorker& until_not() { handlers_[1].neg(); return *this; }

	private:
		struct Routines : private routine_details_t
		{
			using token_t = routine_details_t::token_t;

			static constexpr const routine_t on_start_ = [](worker_t *, const token_t&, dest_t& writer) {
				writer.stop_writing();
			};
			static constexpr const routine_t on_start_recurse_ = [](worker_t *worker, const token_t& t, dest_t& d) {
				on_start_(worker, t, d);
				worker->inject_worker(ParseWorker(*worker).reset());
			};
			static constexpr const routine_t on_stop_ = [](worker_t *, const token_t&, dest_t& writer) {
				writer.start_writing();
			};
		};
};

} // namespace parser_internal
} // namespace core
} // namespace docgen
