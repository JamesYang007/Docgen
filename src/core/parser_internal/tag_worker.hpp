#pragma once

#include "core/parser_internal/docgen_worker.hpp"

namespace docgen {
namespace core {
namespace parser_internal {

class TagWorker : public worker_t
{
	public:
		using worker_t = parser_internal::worker_t;

		TagWorker()
			: worker_t {
				TokenHandler({
					symbol_t::TAGNAME
				}, Routines::on_tag_)
			}
		{}

	private:
		struct Routines : private routine_details_t
		{
			using token_t = routine_details_t::token_t;

			static constexpr const routine_t on_tag_  = [](worker_t *, const token_t& t, dest_t& writer) {
				writer.set_key(t.content);
				writer.start_writing();
				writer.skip_write();
			};
		};
};

} // namespace parser_internal
} // namespace core
} // namespace docgen
