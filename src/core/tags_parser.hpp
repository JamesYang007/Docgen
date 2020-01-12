#pragma once

#include "parse_feeder.hpp"
#include "parser.hpp"

namespace docgen {
namespace core {

struct TagParser_routines
{
	protected:
		static const routine_t on_tag_;
};

const routine_t TagParser_routines::on_tag_ = [](const token_t& t) {
	ParseFeeder::at(t.content);
	ParseFeeder::go();
	ParseFeeder::skip();
};

class TagParser : public ParseWorker, private TagParser_routines
{
	public:
		TagParser()
			: ParseWorker({ SymbolHandler(Symbol::TAGNAME, on_tag_) })
		{}
};

} // namespace core
} // namespace docgen
