#pragma once

#include "docgen_worker.hpp"

namespace docgen {
namespace core {
namespace parser_internal {

struct TagWorker_routines : private routine_details_t
{
	protected:
		static constexpr const routine_t on_tag_  = [](worker_t *, const tok_t& t, dest_t& writer) {
			writer.set_key(t.content);
			writer.start_writing();
			writer.skip_write();
		};
};

class TagWorker : public worker_class_t, private TagWorker_routines
{
	public:
		TagWorker()
			: ParseWorker({ TokenHandler(Symbol::TAGNAME, on_tag_) })
		{}
};

} // namespace parser_internal
} // namespace core
} // namespace docgen
