#pragma once

#include "parse_feeder.hpp"
#include "parse_worker.hpp"

namespace docgen {
namespace core {

struct IgnoreWorker_routines : private WorkerRoutine {
	protected:
		static constexpr const routine_t on_start_ = [](const token_t&, ParseFeeder& f) {
			f.stop();
		};
		static constexpr const routine_t on_stop_ = [](const token_t&, ParseFeeder& f) {
			f.go();
		};
};

class IgnoreWorker : public ParseWorker, private IgnoreWorker_routines {
	public:
		IgnoreWorker(symbol_t from, symbol_t ignore)
			: ParseWorker {
				SymbolHandler(from, on_start_),
				SymbolHandler(ignore, on_stop_).neg()
			}
		{}
};

} // namespace core
} // namespace docgen
