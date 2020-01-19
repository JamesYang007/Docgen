#pragma once

#include "core/parse/details.hpp"

namespace docgen {
namespace core {
namespace parse {
namespace internal {

class TagWorker : public worker_t
{
	public:
		TagWorker()
			: worker_t {
				TokenHandler({
					symbol_t::SDESC,
					symbol_t::TPARAM,
					symbol_t::PARAM,
					symbol_t::RETURN
				}, Routines::on_tag_)
			}
		{}

	private:
		struct Routines : private routine_details_t
		{
			using token_t = routine_details_t::token_t;

			static constexpr const routine_t on_tag_  = [](worker_t *, const token_t& t, dest_t& writer) {
				writer.set_key(t.c_str());
				writer.start_writing();
				writer.skip_write();
			};
		};
};

} // namespace internal
} // namespace parse
} // namespace core
} // namespace docgen
