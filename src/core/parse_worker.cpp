#include "parse_worker.hpp"

namespace docgen {
namespace core {

/*
 * Processes based on the current handler.
 */
void ParseWorker::proc(const token_t& t)
{
	if (done_()) {
		return;
	}

	if (handler_().match(t.name)) {
		handler_().handle_token(t);

		handler_().reset_workers();

		++handler_i_;

		if (done_()) {
			if (indefinite_() || itered_++ < iters_) {
				rewind_();
			}
			return;
		}
	}
	else {
		handler_().handle_workers(t);
	}

	return;
}

/*
 * Reset to the originally constructed state
 */
inline void ParseWorker::reset()
{
	handler_().reset_workers();
	rewind_();
	itered_ = 0;
}

/*
 * Match to a symbol based on whether or not it is contained within symbols_;
 * if neg_ is true, this will match when the symbol is not contained.
 */
inline bool ParseWorker::SymbolHandler::match(symbol_t s)
{
	return !neg_ ?
		symbols_.find(s) != symbols_.end() : symbols_.find(s) == symbols_.end();
}

/*
 * Execute a routine by the on_symbol_ function pointer, if present.
 */
inline void ParseWorker::SymbolHandler::handle_token(const token_t& t)
{
	if (on_symbol_) {
		on_symbol_(t);
	}
}

/*
 * Process all contained ParseWorker objects when none are "working";
 * if one is, process only that one until it is done.
 */
void ParseWorker::SymbolHandler::handle_workers(const token_t& t)
{
	if (working_) {
		working_->proc(t);
		if (!working_->working_()) {
			working_ = nullptr;
			for (worker_t& p : workers_) {
				p.reset();
			}
		}
	}
	if (!working_) {
		for (worker_t& p : workers_) {
			p.proc(t);
			if (p.working_()) {
				working_ = &p;
				break;
			}
		}
	}
}

/*
 * Reset all contained ParseWorker objects
 */
inline void ParseWorker::SymbolHandler::reset_workers()
{
	for (worker_t& p : workers_) {
		p.reset();
	}
}

} // namespace core
} // namespace docgen
