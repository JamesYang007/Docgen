#pragma once

#include "parse_feeder.hpp"
#include "parser.hpp"
#include "tags_parser.hpp"

namespace docgen {
namespace core {

struct CommentParser_routines
{
	protected:
		static const routine_t on_open_, on_close_;
};

const routine_t CommentParser_routines::on_open_ = [](const token_t& t) {
	if (!ParseFeeder::at()) {
		ParseFeeder::at("desc");
	}
	ParseFeeder::go();
	ParseFeeder::skip();
};

const routine_t CommentParser_routines::on_close_ = [](const token_t& t) {
	ParseFeeder::stop();
};

class CommentParser : public ParseWorker, private CommentParser_routines
{
	public:
		CommentParser(symbol_t open, symbol_t close, workers_init_t workers={})
			: ParseWorker {
				SymbolHandler(open, on_open_),
				SymbolHandler(close, on_close_, workers)
			}
		{}
};

} // namespace core
} // namespace docgen
