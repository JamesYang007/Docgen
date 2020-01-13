#pragma once

#include <string>
#include <unordered_set>
#include <vector>
#include <nlohmann/json.hpp>
#include "symbol.hpp"
#include "token.hpp"
#include "parse_feeder.hpp"

namespace docgen {
namespace core {

/*
 * A ParseWorker object holds an ordered array of SymbolHandler objects;
 * at any given point, only one of these SymbolHandlers is being handled.
 *
 * On every call to proc(), the current handler is checked for a match;
 * so long as it isn't matched (waiting to match), proc() will call for
 * the handler to handle_workers(), which will process based on
 * its array of ParseWorker objects. When the current handler is matched,
 * proc() will call for the handler to handle_token(), which executes
 * the routine set by handler's passed function pointer; then, set to
 * move on to the next handler. On the last handler, this will wrap around
 * to the first, for so long as the passed "iters" limiter isn't
 * exceeded (unlimited by default); once it is, this ParseWorker is said
 * to be in a "done" state, on which proc() will no longer execute.
 *
 * Said to be in a "working" state when handling a SymbolHandler after
 * the first; this only matters to a SymbolHandler which is holding
 * this ParseWorker.
 */
class ParseWorker 
{
	public:
		using worker_t = ParseWorker;
		using worker_arr_t = std::vector<worker_t>;
		using worker_arr_init_t = std::initializer_list<worker_t>;

		using symbol_t = Symbol;
		using token_t = Token<symbol_t>;

		/*
		 * A SymbolHandler object holds an unordered set of symbols to match against (symbols_),
		 * a function pointer which references a routine to execute on match (on_symbol_), and
		 * an array of ParseWorker objects to process while still unmatched (workers_).
		 *
		 * If neg_ is set true, a given symbol will only match if it is not in symbols_.
		 *
		 * If working_ is not a nullptr, it indicates that one of the workers is currently in
		 * the "working" state, and points to this worker; until it is done, only this worker
		 * will be processed.
		 */
		class SymbolHandler
		{
		public:	
			using symbol_set_t = std::unordered_set<symbol_t>;
			using symbol_arr_init_t = std::initializer_list<symbol_t>;

			using routine_t = void (*)(const token_t&, ParseFeeder&);

			SymbolHandler(symbol_arr_init_t s, routine_t on_s, worker_arr_init_t w={})
				: symbols_(s), on_symbol_(on_s), workers_(w)
			{}

			SymbolHandler(symbol_t s, routine_t on_s, worker_arr_init_t w={})
				: symbols_{ s }, on_symbol_(on_s), workers_(w)
			{}

			SymbolHandler(worker_arr_init_t w)
				: SymbolHandler(Symbol::END_OF_FILE, nullptr, w)
			{}

			SymbolHandler& neg() { neg_ = true; return *this; }

			bool match(symbol_t s);
			void handle_token(const token_t& t, ParseFeeder& f);
			void handle_workers(const token_t& t, ParseFeeder& f);
			void reset_workers();

		private:
			symbol_set_t symbols_;
			routine_t on_symbol_;
			worker_arr_t workers_;

			worker_t *working_ = nullptr;
			bool neg_ = false;
		};

		using handler_t = SymbolHandler;
		using handler_arr_t = std::vector<handler_t>;
		using handler_arr_init_t = std::initializer_list<handler_t>;

		ParseWorker(handler_arr_init_t handlers, size_t iters=INF_ITERS)
			: handlers_(handlers), iters_(iters)
		{}

		explicit ParseWorker(handler_t&& handler, size_t iters=INF_ITERS)
			: handlers_{ std::move(handler) }, iters_(iters)
		{}

		ParseWorker(worker_arr_init_t workers, size_t iters=INF_ITERS)
			: ParseWorker(SymbolHandler(workers), iters)
		{}

		void proc(const token_t& t, ParseFeeder& f);
		void reset();

	private:
		handler_arr_t handlers_;
		size_t iters_;

		unsigned int handler_i_ = 0;
		size_t itered_ = 0;

		static constexpr size_t INF_ITERS = 0;

		handler_t& handler_() { return handlers_[handler_i_]; }
		void rewind_() { handler_i_ = 0; }
		bool done_() const { return handler_i_ == handlers_.size(); }
		bool working_() const { return handler_i_ != 0 && !done_(); }
		bool indefinite_() const { return iters_ == INF_ITERS; }
};

/*
 * Processes based on the current handler.
 */
inline void ParseWorker::proc(const token_t& t, ParseFeeder& f)
{
	if (done_()) {
		return;
	}

	if (handler_().match(t.name)) {
		handler_().handle_token(t, f);

		handler_().reset_workers();

		++handler_i_;

		if (done_()) {
			if (indefinite_() || ++itered_ < iters_) {
				rewind_();
			}
			return;
		}
	}
	else {
		handler_().handle_workers(t, f);
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
 * Execute a routine by the on_symbol_ function pointer, if present.
 */
inline void ParseWorker::SymbolHandler::handle_token(const token_t& t, ParseFeeder& f)
{
	if (on_symbol_) {
		on_symbol_(t, f);
	}
}

/*
 * Process all contained ParseWorker objects when none are "working";
 * if one is, process only that one until it is done.
 */
inline void ParseWorker::SymbolHandler::handle_workers(const token_t& t, ParseFeeder& f)
{
	if (working_) {
		working_->proc(t, f);
		if (!working_->working_()) {
			working_ = nullptr;
			for (worker_t& p : workers_) {
				p.reset();
			}
		}
	}
	if (!working_) {
		for (worker_t& p : workers_) {
			p.proc(t, f);
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

/*
 * Match to a symbol based on whether or not it is contained within symbols_;
 * if neg_ is true, this will match when the symbol is not contained.
 */
inline bool ParseWorker::SymbolHandler::match(symbol_t s)
{
	return !neg_ ?
		symbols_.find(s) != symbols_.end() : symbols_.find(s) == symbols_.end();
}

} // namespace core
} // namespace docgen
