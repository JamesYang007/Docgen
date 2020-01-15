#pragma once

#include "core/parser_internal/docgen_worker.hpp"

namespace docgen {
namespace core {
namespace parser_internal {

struct IgnoreWorker_routines : private routine_details_internal_t
{
	protected:
		static constexpr const routine_t on_start_ = [](worker_t *, const tok_t&, dest_t& writer) {
			writer.stop_writing();
		};
		static constexpr const routine_t on_start_recurse_ = [](worker_t *worker, const tok_t& t, dest_t& d) {
			on_start_(worker, t, d);
			worker->inject_worker(ParseWorker(*worker).reset());
		};
		static constexpr const routine_t on_stop_ = [](worker_t *, const tok_t&, dest_t& writer) {
			writer.start_writing();
		};
};

class IgnoreWorker : public worker_internal_t, private IgnoreWorker_routines
{
	public:
		IgnoreWorker(token_t from, token_t until)
			: worker_internal_t {
				TokenHandler(from, on_start_),
				TokenHandler(until, on_stop_)
			}
		{}

		IgnoreWorker& recursive() { handlers_[0].on_match(on_start_recurse_); return *this; }
		IgnoreWorker& until_not() { handlers_[1].neg(); return *this; }
};

} // namespace parser_internal
} // namespace core
} // namespace docgen
