#pragma once

#include "parse_feeder.hpp"
#include "parse_worker.hpp"

namespace docgen {
namespace core {

struct TagParser_routines
{
	protected:
		using routine_t = ParseWorker::SymbolHandler::routine_t;

		static constexpr const routine_t on_tag_  = [](const token_t& t, ParseFeeder& f) {
			f.at(t.content);
			f.go();
			f.skip();
		};
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
