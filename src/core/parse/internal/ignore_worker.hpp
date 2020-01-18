#pragma once

#include "core/parse/details.hpp"

namespace docgen {
namespace core {
namespace parse {
namespace internal {

class IgnoreWorker : public worker_t
{
	public:
		using worker_t = parse::worker_t;
		using token_t = typename worker_t::token_t;

		IgnoreWorker(const token_t& from, const token_t& until)
			: worker_t {
				TokenHandler(from, Routines::on_start_),
				TokenHandler(until, Routines::on_stop_)
			}
		{}

		IgnoreWorker& balanced() { handler_at(1).on_match(Routines::on_stop_balanced_); handler_at(1).inject_token(handler_at(0).token()); return *this; }
		IgnoreWorker& until_not() { handler_at(1).neg(); return *this; }

	private:
		struct Routines : private routine_details_t
		{
			using token_t = routine_details_t::token_t;

			static constexpr const routine_t on_start_ = [](worker_t *, const token_t&, dest_t& writer) {
				writer.stop_writing();
			};
			static constexpr const routine_t on_stop_ = [](worker_t *, const token_t&, dest_t& writer) {
				writer.start_writing();
			};
			static constexpr const routine_t on_stop_balanced_ = [](worker_t *worker, const token_t& token, dest_t& d) {
				if (token == worker->handler_at(0).token()) {
					worker_t::handler_t& close_handler = worker->handler_at(1);
					close_handler.tolerance(close_handler.tolerance() + 2);
					return;
				}
				on_stop_(worker, token, d);
			};
		};
};

} // namespace internal
} // namespace parse
} // namespace core
} // namespace docgen
