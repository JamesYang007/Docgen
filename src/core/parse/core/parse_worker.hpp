#pragma once

#include <string>
#include <unordered_set>
#include <vector>

namespace docgen {
namespace core {
namespace parse {
namespace core {

/*
 * A ParseWorker object holds an ordered array of TokenHandler objects;
 * at any given point, only one of these TokenHandlers is being handled.
 *
 * On every call to proc(), the current handler is checked for a match;
 * so long as it isn't matched (waiting to match), proc() will call for
 * the handler to proc_workers_(), which will process based on
 * its array of ParseWorker objects. When the current handler is matched,
 * proc() will execute the routine set by handler's passed function pointer;
 * then, set to move on to the next handler. On the last handler,
 * this will wrap around to the first, for so long as the passed "iters" limiter
 * isn't exceeded (unlimited by default); once it is, this ParseWorker is said
 * to be in a "done" state, on which proc() will no longer execute.
 *
 * Said to be in a "working" state when handling a TokenHandler at/after
 * the index specified by working_at() (by default, the second);
 * this only matters to a TokenHandler which is holding this ParseWorker.
 *
 * Object of type DestType is passed by reference to proc(), and is ultimately
 * passed down to specified worker routines.
 *
 * TokenType must be compliant with the requirements of std::unordered_set
 */
template <class TokenType, class DestType>
class ParseWorker
{
	public:
		using worker_t = ParseWorker;
		using worker_arr_t = std::vector<worker_t>;
		using worker_arr_init_t = std::initializer_list<worker_t>;

		using token_t = TokenType;
		using dest_t = DestType;

		/*
		 * RoutineDetails may be inherited for type aliases
		 */
		struct RoutineDetails
		{
			using worker_t = ParseWorker;
			using token_t = typename worker_t::token_t;
			using dest_t = typename worker_t::dest_t;
			using routine_t = void (*)(worker_t *, const token_t&, dest_t&);
		};

		/*
		 * A TokenHandler object holds an unordered set of symbols to match against (tokens_),
		 * a function pointer which references a routine to execute on match (on_match_), and
		 * an array of ParseWorker objects to process while still unmatched (workers_).
		 *
		 * If neg_ is set true, a given symbol will only match if it is not in tokens_.
		 *
		 * If working_ is not a nullptr, it indicates that one of the workers is currently in
		 * the "working" state, and points to this worker; until it is done, only this worker
		 * will be processed.
		 */
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

				bool match(const token_t& t);

				TokenHandler& neg() { neg_ = true; return *this; }
				TokenHandler& tolerance(size_t t) { match_tolerance_ = t; return *this; }
				TokenHandler& timeout(size_t t) { nomatch_timeout_ = t; return *this; }
				const token_t& token() { return *tokens_.begin(); }
				size_t tolerance() { return match_tolerance_; }
				size_t timeout() { return nomatch_timeout_; }
				void on_match(routine_t on_m) { on_match_ = on_m; }
				bool done() { return match_counter_ >= match_tolerance_; }
				bool time() { return nomatch_timeout_ == INF_ITERS ? false : nomatch_counter_ >= nomatch_timeout_; }
				void inject_token(const token_t& t) { tokens_.insert(t); }
				void inject_token(token_t&& t) { tokens_.insert(std::move(t)); }

			private:
				bool proc_workers_(const token_t& t, dest_t& f);
				void reset_();

				token_set_t tokens_;
				routine_t on_match_;
				worker_arr_t workers_;
				size_t match_counter_ = 0;
				size_t nomatch_counter_ = 0;

				size_t match_tolerance_ = 1;
				size_t nomatch_timeout_ = INF_ITERS;
				worker_t *working_ = nullptr;
				bool neg_ = false;
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

		bool proc(const token_t& t, dest_t& f);

		ParseWorker& rewind() { handler().reset_(); handler_i_ = 0; return *this; }
		ParseWorker& reset() { itered_ = 0; return rewind(); }	
		ParseWorker& block() { blocker_ = true; return *this; }
		ParseWorker& limit(size_t iters) { iters_ = iters; return *this; }
		ParseWorker& working_at(size_t i) { working_at_ = i; return *this; }
		handler_t& handler_at(size_t i) { return handlers_[i % handlers_.size()]; }
		handler_t& handler(size_t offset) { return handler_at(handler_i_ + offset); }
		handler_t& handler() { return handler(0); }
		handler_t& handler_next() { return handler(1); }
		void stall() { stalling_ = true; }

	protected:
		handler_arr_t handlers_;

	private:
		size_t iters_ = INF_ITERS;
		size_t working_at_ = 1;
		bool blocker_ = false;

		unsigned int handler_i_ = 0;
		size_t itered_ = 0;
		bool stalling_ = false;

		static constexpr size_t INF_ITERS = 0;

		void restart_() { ++itered_; rewind(); }
		bool indefinite_() const { return iters_ == INF_ITERS; }
		bool finished_() const { return !indefinite_() && itered_ >= iters_; }
		bool done_() const { return handler_i_ == handlers_.size(); }
		bool working_() const { return handler_i_ >= working_at_ && !finished_(); }
};

/*
 * Processes based on the current handler.
 * Returns true on blocking.
 */
template <class TokenType, class DestType>
inline bool ParseWorker<TokenType, DestType>::proc(const token_t& t, dest_t& d)
{
	if (finished_()) {
		return false;
	}

	if (handler().proc_workers_(t, d)) {
		return true;
	}

	if (handler().match(t)) {	
		if (handler().on_match_) {
			handler().on_match_(this, t, d);
		}

		if (handler().done() && !stalling_) {
			handler().reset_();

			++handler_i_;

			if (done_()) {
				restart_();
			}
		}
		stalling_ = false;
		return blocker_;
	}
	else if (handler().time()) {
		restart_();
	}

	return false;
}

/*
 * Match to a symbol based on whether or not it is contained within tokens_;
 * if neg_ is true, this will match when the symbol is not contained.
 */
template <class TokenType, class DestType>
inline bool ParseWorker<TokenType, DestType>::TokenHandler::match(const token_t& t)
{
	if (!neg_ ? tokens_.find(t) != tokens_.end() : tokens_.find(t) == tokens_.end()) {
		++match_counter_;
		return true;
	}
	++nomatch_counter_;
	return false;
}

/*
 * Process all contained ParseWorker objects when none are "working";
 * if one is, process only that one until it is done.
 * Returns true if any workers are blocking.
 */
template <class TokenType, class DestType>
inline bool ParseWorker<TokenType, DestType>::TokenHandler::proc_workers_(const token_t& t, dest_t& d)
{
	bool blocked = false;

	if (working_) {
		blocked = working_->proc(t, d);
		if (!working_->working_()) {
			for (worker_t& w : workers_) {
				w.rewind();	
			}
			working_ = nullptr;
		}
	}
	if (!working_) {
		for (worker_t& p : workers_) {
			blocked = p.proc(t, d) ? true : blocked;
			if (p.working_()) {
				working_ = &p;
				break;
			}
		}
	}

	return blocked;
}

/*
 * Reset all contained ParseWorker objects.
 */
template <class TokenType, class DestType>
inline void ParseWorker<TokenType, DestType>::TokenHandler::reset_()
{
	for (worker_t& w : workers_) {
		w.reset();
	}
	match_counter_ = 0;
	nomatch_counter_ = 0;
}

} // namespace core
} // namespace parse
} // namespace core
} // namespace docgen
