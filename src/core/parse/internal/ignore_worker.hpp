#pragma once

#include <initializer_list>
#include "core/parse/details.hpp"

namespace docgen {
namespace core {
namespace parse {
namespace internal {

class IgnoreWorker : public worker_t
{
	public:
		using token_t = typename worker_t::token_t;
		using token_arr_init_t = std::initializer_list<token_t>;

		IgnoreWorker(const token_t& from, const token_t& until)
			: worker_t {
				TokenHandler(from, Routines::on_start_),
				TokenHandler(until, Routines::on_stop_)
			}
		{}

		IgnoreWorker(const token_t& from, token_arr_init_t until)
			: worker_t {
				TokenHandler(from, Routines::on_start_),
				TokenHandler(until, Routines::on_stop_)
			}
		{}

		IgnoreWorker(token_arr_init_t from, const token_t& until)
			: worker_t {
				TokenHandler(from, Routines::on_start_),
				TokenHandler(until, Routines::on_stop_)
			}
		{}

		IgnoreWorker(token_arr_init_t from, token_arr_init_t until)
			: worker_t {
				TokenHandler(from, Routines::on_start_),
				TokenHandler(until, Routines::on_stop_)
			}
		{}

		IgnoreWorker& from_not() { handler_at(0).neg(); return *this; }
		IgnoreWorker& until_not() { handler_at(1).neg(); return *this; }
		IgnoreWorker& timeout(size_t count) { handler_at(0).timeout(count); return *this; }
		IgnoreWorker& ignore_last() { handler_at(1).on_match(Routines::on_stop_skip_); return *this; }
		IgnoreWorker& clear_key() { handler_at(0).on_match(Routines::on_start_clear_); return *this; }
		IgnoreWorker& balanced()
		{
			handler_at(1).on_match(Routines::on_stop_balanced_);
			handler_at(1).add_token(handler_at(0).token());
			on_cleanup(Routines::on_cleanup_balanced_);
			return *this;
		}

	private:
		struct Routines : private routine_details_t
		{
			using token_t = routine_details_t::token_t;

			static constexpr const routine_t on_start_ = [](worker_t *, const token_t&, dest_t& writer) {
				writer.stop_writing();
			};
			static constexpr const routine_t on_start_clear_ = [](worker_t *w, const token_t& t, dest_t& writer) {
				on_start_(w, t, writer);
				if (!writer.written()) {
					writer.clear_key();
				}
			};
			static constexpr const routine_t on_stop_ = [](worker_t *, const token_t&, dest_t& writer) {
				writer.start_writing();
			};
			static constexpr const routine_t on_stop_skip_ = [](worker_t *w, const token_t& t, dest_t& writer) {
				on_stop_(w, t, writer);
				writer.skip_write();
			};
			static constexpr const routine_t on_stop_balanced_ = [](worker_t *worker, const token_t& token, dest_t& d) {
				if (token == worker->handler_at(0).token()) {
					worker->handler().tolerance(worker->handler().tolerance() + 2);
					return;
				}
				else if (worker->handler().done()) {
					on_stop_(worker, token, d);
				}
			};
			static constexpr const routine_t on_cleanup_balanced_ = [](worker_t *worker, const token_t&, dest_t&) {
				worker->handler_at(1).tolerance(1);
			};
		};
};

} // namespace internal
} // namespace parse
} // namespace core
} // namespace docgen
