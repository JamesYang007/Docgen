#pragma once

#include "core/parser_internal/docgen_worker.hpp"
#include "core/parser_internal/ignore_worker.hpp"

namespace docgen {
namespace core {
namespace parser_internal {

class FuncWorker : public worker_t
{
	public:
		using worker_t = parser_internal::worker_t;

		FuncWorker()
			: worker_t {
				TokenHandler(symbol_t::TEXT, Routines::on_dec_),
				TokenHandler({ symbol_t::SEMICOLON, symbol_t::OPEN_BRACE }, Routines::on_done_, {
					IgnoreWorker(symbol_t::NEWLINE, symbol_t::TEXT)
				}),
				TokenHandler(symbol_t::CLOSE_BRACE, nullptr, {
					IgnoreWorker(symbol_t::OPEN_BRACE, symbol_t::CLOSE_BRACE).recursive().block()
				})
			}
		{}

	private:
		static constexpr const char *FUNCS_KEY = "functions";
		static constexpr const char *DEC_KEY = "declaration";

		struct Routines : private routine_details_t
		{
			using token_t = routine_details_t::token_t;

			static constexpr const routine_t on_dec_ = [](worker_t *, const token_t&, dest_t& writer) {
				writer.set_key(DEC_KEY);	
				writer.start_writing();
			};
			static constexpr const routine_t on_done_ = [](worker_t *worker, const token_t& token, dest_t& writer) {
				writer.stop_writing();
				writer.clear_key();
				writer.store(FUNCS_KEY);
				if (token.name == symbol_t::SEMICOLON) {
					worker->reset();
				}
			};
		};
};

} // namespace parser_internal
} // namespace core
} // namespace docgen
