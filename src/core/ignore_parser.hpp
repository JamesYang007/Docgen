#pragma once

#include "parse_feeder.hpp"
#include "parser.hpp"

namespace docgen {
namespace core {

struct IgnoreParser_routines {
	protected:
		static const routine_t on_start_, on_stop_;
};

const routine_t IgnoreParser_routines::on_start_ = [](const token_t& t) {
	ParseFeeder::stop();
};

const routine_t IgnoreParser_routines::on_stop_ = [](const token_t& t) {
	ParseFeeder::go();
};

class IgnoreParser : public ParseWorker, private IgnoreParser_routines {
	public:
		IgnoreParser(symbol_t from, symbol_t ignore)
			: ParseWorker {
				SymbolHandler(from, on_start_),
				SymbolHandler(ignore, on_stop_).neg()
			}
		{}
};

} // namespace core
} // namespace docgen
