#pragma once

#include "parse_feeder.hpp"
#include "parse_worker.hpp"

namespace docgen {
namespace core {

struct TagWorker_routines : private WorkerRoutine
{
	protected:
		static constexpr const routine_t on_tag_  = [](const token_t& t, ParseFeeder& f) {
			f.at(t.content);
			f.go();
			f.skip();
		};
};

class TagWorker : public ParseWorker, private TagWorker_routines
{
	public:
		TagWorker()
			: ParseWorker({ SymbolHandler(Symbol::TAGNAME, on_tag_) })
		{}
};

} // namespace core
} // namespace docgen
