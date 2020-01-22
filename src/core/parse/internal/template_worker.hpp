#pragma once

#include "core/parse/details.hpp"

namespace docgen {
namespace core {
namespace parse {
namespace internal {

class TemplateWorker : public worker_t
{
	public:
		TemplateWorker()
			: worker_t ({
				TokenHandler(symbol_t::TEMPLATE, nullptr),
				TokenHandler(symbol_t::OPEN_ANGLE_BRACKET, Routines::on_open_),
				TokenHandler({ symbol_t::OPEN_ANGLE_BRACKET, symbol_t::CLOSE_ANGLE_BRACKET }, Routines::on_close_)
			}, Routines::on_cleanup_)
		{}

	private:
		static constexpr const char * const TEMPLATES_KEY = "tparamlists";

		struct Routines : private routine_details_t
		{
			using token_t = routine_details_t::token_t;

			static constexpr const routine_t on_open_ = [](worker_t *, const token_t&, dest_t& writer) {
				writer.set_key(TEMPLATES_KEY);
				writer.start_writing();
				writer.skip_write();
				writer.pushback_write();
			};
			static constexpr const routine_t on_close_ = [](worker_t *worker, const token_t& token, dest_t& writer) {
				if (token == symbol_t::OPEN_ANGLE_BRACKET) {
					worker->handler().tolerance(worker->handler().tolerance() + 2);
					return;
				}
				else if (worker->handler().done()) {
					writer.stop_writing();
				}
			};
			static constexpr const routine_t on_cleanup_ = [](worker_t *worker, const token_t&, dest_t&) {
				worker->handler_at(2).tolerance(1);
			};
		};
};

} // namespace internal
} // namespace parse
} // namespace core
} // namespace docgen
