#pragma once

#include "core/parser_internal/docgen_worker.hpp"

namespace docgen {
namespace core {
namespace parser_internal {

struct TagWorker_routines : private routine_details_t
{
	protected:
		using token_t = routine_details_t::token_t;

		static constexpr const routine_t on_tag_  = [](worker_t *, const token_t& t, dest_t& writer) {
			writer.set_key(t.content);
			writer.start_writing();
			writer.skip_write();
		};
};

class TagWorker : public worker_t, private TagWorker_routines
{
	public:
		using worker_t = parser_internal::worker_t;

		TagWorker()
			: worker_t {
				TokenHandler({
					symbol_t::TAGNAME
				}, on_tag_)
			}
		{}
};

} // namespace parser_internal
} // namespace core
} // namespace docgen
