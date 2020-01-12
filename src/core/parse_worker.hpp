#pragma once

#include <string>
#include <unordered_set>
#include <vector>
#include <nlohmann/json.hpp>
#include "symbol.hpp"
#include "token.hpp"

namespace docgen {
namespace core {

class ParseWorker;

using token_t = Token<symbol_t>;
using routine_t = void (*)(const token_t& t);
using workers_t = std::vector<ParseWorker>;
using workers_init_t = std::initializer_list<ParseWorker>;

class ParseWorker 
{
	public:
		struct SymbolHandler
		{
			using symbol_t = Symbol;
			using symbols_t = std::unordered_set<symbol_t>;
			using symbols_init_t = std::initializer_list<symbol_t>;

			symbols_t symbols;
			routine_t on_symbol;
			workers_t workers;

			SymbolHandler(symbols_init_t s, routine_t on_s, workers_init_t w={})
				: symbols(s), on_symbol(on_s), workers(w)
			{}
			SymbolHandler(const SymbolHandler& s)
			{}

			SymbolHandler(symbol_t s, routine_t on_s, workers_init_t w={})
				: symbols(), on_symbol(on_s), workers(w)
			{
				symbols.insert(s);
			}

			SymbolHandler(workers_init_t w)
				: SymbolHandler(Symbol::END_OF_FILE, nullptr, w)
			{}

			SymbolHandler& neg() { neg_ = true; return *this; }

			bool match(symbol_t s);
			void handle_workers(const token_t& t);
			void reset_workers();

			private:
				ParseWorker *working_ = nullptr;
				bool neg_ = false;
		};

		using handler_t = SymbolHandler;
		using handlers_t = std::vector<handler_t>;
		using handlers_init_t = std::initializer_list<handler_t>;

		ParseWorker(handlers_init_t handlers, size_t iters=INF_ITERS)
			: handlers_(handlers), iters_(iters)
		{}

		explicit ParseWorker(handler_t&& handler, size_t iters=INF_ITERS)
			: iters_(iters)
		{
			handlers_.push_back(handler);
			handler_ = handlers_.begin();
		}

		ParseWorker(workers_init_t workers, size_t iters=INF_ITERS)
			: ParseWorker(SymbolHandler(workers), iters)
		{}

		void proc(const token_t& t);
		void reset();

		static constexpr size_t INF_ITERS = 0;

	private:
		handlers_t handlers_;
		size_t iters_;

		handlers_t::iterator handler_ = handlers_.begin();
		size_t itered_ = 0;

		void rewind_() { handler_ = handlers_.begin(); }
		bool done_() const { return handler_ == handlers_.end(); }
		bool working_() const { return handler_ != handlers_.begin() && !done_(); }
		bool indefinite_() const { return iters_ == INF_ITERS; }
};

void ParseWorker::proc(const token_t& t)
{
	if (done_()) {
		return;
	}

	if (handler_->match(t.name)) {
		if (handler_->on_symbol) {
			handler_->on_symbol(t);
		}

		handler_->reset_workers();

		++handler_;

		if (done_()) {
			if (indefinite_() || itered_++ < iters_) {
				rewind_();
			}
			return;
		}
	}
	else {
		handler_->handle_workers(t);
	}

	return;
}

void ParseWorker::reset()
{
	rewind_();
	itered_ = 0;
}

bool ParseWorker::SymbolHandler::match(symbol_t s)
{
	return !neg_ ?
		symbols.find(s) != symbols.end() : symbols.find(s) == symbols.end();
}

void ParseWorker::SymbolHandler::handle_workers(const token_t& t)
{
	if (working_) {
		working_->proc(t);
		if (!working_->working_()) {
			working_ = nullptr;
			for (ParseWorker& p : workers) {
				p.reset();
			}
		}
	}
	if (!working_) {
		for (ParseWorker& p : workers) {
			p.proc(t);
			if (p.working_()) {
				working_ = &p;
				break;
			}
		}
	}
}

void ParseWorker::SymbolHandler::reset_workers()
{
	for (ParseWorker& p : workers) {
		p.reset();
	}
}

/* workers_t operator+(const workers_t& w1, const workers_t& w2) */
/* { */
/* 	workers_t w; */
/* 	w.reserve(w1.size() + w2.size()); */
/* 	w.insert(w.end(), w1.begin(), w1.end()); */
/* 	w.insert(w.end(), w2.begin(), w2.end()); */
/* 	return w; */
/* } */

} // namespace core
} // namespace docgen
