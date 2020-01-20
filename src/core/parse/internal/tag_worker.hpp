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
				}, Routines::on_tag_),
				TokenHandler(symbol_t::TEXT, nullptr)
			}
		{}

	private:
		static constexpr const char * const PARAMS_KEY = "params";
		static constexpr const char * const PARAM_NAME_KEY = "name";
		static constexpr const char * const PARAM_DESC_KEY = "desc";

		struct Routines : private routine_details_t
		{
			using token_t = routine_details_t::token_t;

			static constexpr const routine_t on_param_first_text_  = [](worker_t *, const token_t& token, dest_t& writer) {
				writer.stow_for(PARAMS_KEY);
				writer.set_key_active(PARAM_NAME_KEY);
				writer.write(token.c_str());
				writer.set_key_active(PARAM_DESC_KEY);
				writer.start_writing();
				writer.skip_write();
			};
			static constexpr const routine_t on_tag_  = [](worker_t *worker, const token_t& token, dest_t& writer) {
				if (token == symbol_t::PARAM) {
					worker->handler(1).on_match(on_param_first_text_);
					if (writer.writing()) {
						writer.skip_write();
					}
					return;
				}
				writer.set_key(token.c_str());
				writer.start_writing();
				writer.skip_write();
				worker->restart();
			};
		};
};

} // namespace internal
} // namespace parse
} // namespace core
} // namespace docgen
