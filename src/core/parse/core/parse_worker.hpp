#pragma once

#include <string>
#include <unordered_set>
#include <vector>

namespace docgen {
namespace core {
namespace parse {
namespace core {

/**
 * A ParseWorker object holds a sequential array of TokenHandler objects;
 * at any given point, only one of these TokenHandlers is being handled.
 *
 * On every call to proc(), the current handler is checked for a match;
 * so long as it isn't matched (waiting to match), proc() will call for
 * the handler to proc_workers_(), which will process through its array
 * of children ParseWorker objects. If the current handler has mismatched
 * enough s.t. it is "timed out", the worker will give up on this iteration
 * and restart. If the handler is matched, the hander's on_match_ routine
 * will be executed (if one is set), and if the handler has matched enough
 * s.t. it is "done", then this worker will move on to the next handler.
 * On the last handler, the worker will restart by wrapping around to
 * the first, for so long as the iterations limit hasn't been exceeded
 * (unlimited by default); if it is, calls to proc() no longer do anything. 
 *
 * Said to be in a "working" state when handling a TokenHandler at/after
 * the index dictated by working_at_ (by default, at the second handler);
 * this only matters to the parent worker, as only one child can be "working"
 * at a time.
 * Said to be "done" on having completed the current iteration of handlers,
 * and "finished" when the iterations limiter has been exceeded.
 * May be "stalled" s.t. does not move on to the next handler on present match.
 * May be set to "block" parent/grandparents/etc. from matching on any match.
 *
 * Object of type DestType is passed by reference to proc(), and is ultimately
 * passed down to handled by user-specified worker routines.
 *
 * TokenType must be compliant with the requirements of std::unordered_set
 **/
template <class TokenType, class DestType>
class ParseWorker
{
	public:
		using worker_t = ParseWorker;
		using worker_arr_t = std::vector<worker_t>;
		using worker_arr_init_t = std::initializer_list<worker_t>;

		using token_t = TokenType;
		using dest_t = DestType;

		/**
		 * RoutineDetails may be inherited for type aliases
		 * (routine function pointer and parameter types)
		 **/
		struct RoutineDetails
		{
			using worker_t = ParseWorker;
			using token_t = typename worker_t::token_t;
			using dest_t = typename worker_t::dest_t;
			using routine_t = void (*)(worker_t *, const token_t&, dest_t&);
		};

		/**
		 * A TokenHandler object holds an unordered set of tokens to match against (tokens_),
		 * a function pointer which references a routine to execute on match (on_match_), and
		 * an array of ParseWorker objects to process while still unmatched (workers_).
		 *
		 * The handler is said to be "done" when the worker may move on, and "timed out" when
		 * the worker should give up on the current iteration and restart.
		 **/
		class TokenHandler
		{
			friend class ParseWorker;
			public:	
				using token_set_t = std::unordered_set<token_t>;
				using token_arr_init_t = std::initializer_list<token_t>;

				using routine_t = typename RoutineDetails::routine_t;

				explicit TokenHandler(token_arr_init_t t, routine_t on_m=nullptr, worker_arr_init_t w={})
					: tokens_(t), on_match_(on_m), workers_(w)
				{}

				explicit TokenHandler(const token_t& t, routine_t on_m=nullptr, worker_arr_init_t w={})
					: tokens_{ t }, on_match_(on_m), workers_(w)
				{}

				explicit TokenHandler(token_t&& t, routine_t on_m=nullptr, worker_arr_init_t w={})
					: on_match_(on_m), workers_(w)
				{
					tokens_.insert(std::move(t));
				}

				explicit TokenHandler(worker_arr_init_t w)
					: TokenHandler(Symbol::END_OF_FILE, nullptr, w)
				{}

				/*
				 * Match to a symbol based on whether or not it is contained within tokens_;
				 * if neg_ is true, this will match when the symbol is not contained.
				 */
				bool match(const token_t& t);

				/*** OPERATORS ***/// setters which act like operators to be used inline on construction
				TokenHandler& neg() { neg_ = true; return *this; } // set handler to match against anything not in the set tokens_
				TokenHandler& tolerance(size_t t) { match_tolerance_ = t; return *this; } // set tolerance (# of matches before handler is done s.t. worker can move on)
				TokenHandler& timeout(size_t t) { mismatch_timeout_ = t; return *this; } // set timeout (# of matches missed before handler times out s.t. worker must restart)

				/*** PUBLIC HELPERS ***/// these are public so they may be used by routines or ParseWorker inheritors
				void on_match(routine_t on_m) { on_match_ = on_m; } // set routine to execute on match
				void inject_token(const token_t& t) { tokens_.insert(t); } // inject token into tokens_ set by const reference
				void inject_token(token_t&& t) { tokens_.insert(std::move(t)); } // inject token into tokens_ set by r-value
				const token_t& token() { return *tokens_.begin(); } // get token (only useful if there is just one token in tokens_)
				size_t tolerance() { return match_tolerance_; } // get handler's match tolerance
				size_t timeout() { return mismatch_timeout_; } // get handler's mismatch timeout

			private:
				/*
				 * Process all contained ParseWorker objects when none are "working";
				 * if one is, process only that one until it is done.
				 * Returns true if any workers are currently "blocking".
				 */
				bool proc_workers_(const token_t& t, dest_t& f);
				
				/*
				 * Reset all contained ParseWorker objects and all state indicators.
				 */
				void reset_();

				/*** MEMBERS ***/
				token_set_t tokens_; // set of tokens to match against
				routine_t on_match_; // routine to be executed on match
				worker_arr_t workers_; // set of children workers

				/*** STATE ***/
				size_t match_counter_ = 0; // tracks match count
				size_t mismatch_counter_ = 0; //  tracks mismatch count
				worker_t *working_ = nullptr; // points to the child worker that is presently "working"

				/*** SETTINGS ***/
				size_t match_tolerance_ = 1; // dictates how many matches are required before handler is "done"
				size_t mismatch_timeout_ = INF_ITERS; // dictates how many mismatches before handler is "timed out"
				bool neg_ = false; // if set true, a given symbol will only match if it is not in tokens_

				/*** HELPERS ***/
				bool done_() { return match_counter_ >= match_tolerance_; } // return if handler's match tolerance has been met
				bool time_() { return mismatch_timeout_ == INF_ITERS ? false : mismatch_counter_ >= mismatch_timeout_; } // return if handler has timed out
		};

		using handler_t = TokenHandler;
		using handler_arr_t = std::vector<handler_t>;
		using handler_arr_init_t = std::initializer_list<handler_t>;

		explicit ParseWorker(handler_arr_init_t handlers)
			: handlers_(std::move(handlers))
		{}

		explicit ParseWorker(worker_arr_init_t workers)
			: ParseWorker({ TokenHandler(std::move(workers)) })
		{}

		/*
		 * Processes based on the current handler.
		 * Returns true on "blocking".
		 */
		bool proc(const token_t& t, dest_t& f);

		/*** OPERATORS ***/// setters which act like operators to be used inline on construction
		ParseWorker& limit(size_t iters) { iters_ = iters; return *this; } // limit this worker's start-to-finish iterations
		ParseWorker& block() { blocker_ = true; return *this; } // set this worker to block parent, grandparents, etc. from matching on token match
		ParseWorker& working_at(size_t i) { working_at_ = i; return *this; } // set handler index at which this worker is "working" (to block sibling workers from processing)

		/*** HANDLER ACCESS ***//// public access for private array handlers_
		handler_t& handler_at(size_t i) { return handlers_[i % handlers_.size()]; } // safely access handler by index (wraparound if out of bounds)
		handler_t& handler(size_t offset) { return handler_at(handler_i_ + offset); } // access handler at offset from current handler
		handler_t& handler() { return handler(0); } // access current handler
		handler_t& handler_next() { return handler(1); } // access next handler

		/*** PUBLIC HELPERS ***/// these are public so that they may be used by worker routines
		void stall() { stalling_ = true; } // "stall" such that currently matched handler doesn't proceed to next
		void rewind() { handler().reset_(); handler_i_ = 0; } // rewind to first handler
		void restart() { ++itered_; rewind(); } // rewind and count iteration
		void reset() { itered_ = 0; stalling_ = false; rewind(); } // rewind reset state indicators

	private:
		/*** MEMBERS ***/
		handler_arr_t handlers_; // sequential array of token handlers

		/*** STATE ***/
		unsigned int handler_i_ = 0; // index of the current handler
		bool stalling_ = false; // dictates if worker is "stalled" from moving to next handler
		size_t itered_ = 0; // the number of iterations completed/attempted

		/*** SETTINGS ***/
		size_t working_at_ = 1; // handler index at which said to be "working"
		bool blocker_ = false; // whether or not will be "blocking" ancestors from matching on match
		size_t iters_ = INF_ITERS; // the limit on iterations

		/*** CONSTANTS ***/
		static constexpr size_t INF_ITERS = 0;

		/*** HELPERS ***/
		bool indefinite_() const { return iters_ == INF_ITERS; } // check if iterations are limited
		bool finished_() const { return !indefinite_() && itered_ >= iters_; } // check if iteration limit has been exceeded
		bool done_() const { return handler_i_ == handlers_.size(); } // check if current iteration is done
		bool working_() const { return handler_i_ >= working_at_ && !finished_(); } // check if presently "working"
};

template <class TokenType, class DestType>
inline bool ParseWorker<TokenType, DestType>::proc(const token_t& t, dest_t& d)
{
	// do nothing if iteration limit has been exceeded
	if (finished_()) {
		return false;
	}

	// process all children workers first
	if (handler().proc_workers_(t, d)) {
		return true;
	}

	// check for match with current token handler
	if (handler().match(t)) {	
		// execute handler's on_match_ routine
		if (handler().on_match_) {
			handler().on_match_(this, t, d);
		}

		// if current handler is "done" and worker isn't being "stalled", move on to next handler
		if (handler().done_() && !stalling_) {
			handler().reset_();
			++handler_i_;

			// restart if current iteration is complete (i.e. currently past the last handler)
			if (done_()) {
				restart();
			}
		}
		stalling_ = false;

		// if worker is set "blocking", return true to block ancestors from matching
		return blocker_;
	}
	else if (handler().time_()) {
		// restart on handler timeout
		restart();
	}

	return false;
}

template <class TokenType, class DestType>
inline bool ParseWorker<TokenType, DestType>::TokenHandler::match(const token_t& t)
{
	// check if token is/isn't in set tokens_; return based on if neg_ is false/true
	if (!neg_ ? tokens_.find(t) != tokens_.end() : tokens_.find(t) == tokens_.end()) {
		++match_counter_;
		return true;
	}
	++mismatch_counter_;
	return false;
}

template <class TokenType, class DestType>
inline bool ParseWorker<TokenType, DestType>::TokenHandler::proc_workers_(const token_t& t, dest_t& d)
{
	bool blocked = false;

	// if working_ is set, process only that worker
	if (working_) {
		blocked = working_->proc(t, d);
		if (!working_->working_()) {
			for (worker_t& w : workers_) {
				w.rewind();	
			}
			working_ = nullptr;
		}
	}

	// working_ may change (no longer be "working") with the former conditional,
	// in which case continue processing all workers
	if (!working_) {
		for (worker_t& p : workers_) {
			blocked = p.proc(t, d) || blocked;
			if (p.working_()) {
				working_ = &p;
				break;
			}
		}
	}

	return blocked;
}

template <class TokenType, class DestType>
inline void ParseWorker<TokenType, DestType>::TokenHandler::reset_()
{
	for (worker_t& w : workers_) {
		w.reset();
	}
	match_counter_ = 0;
	mismatch_counter_ = 0;
	working_ = nullptr;
}

} // namespace core
} // namespace parse
} // namespace core
} // namespace docgen
