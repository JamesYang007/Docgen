#pragma once

#include "docgen_worker.hpp"
#include "ignore_worker.hpp"

namespace docgen {
namespace core {
namespace parser_internal {

struct FuncWorker_routines : private routine_details_internal_t
{
	private:
		static constexpr const char *FUNCS_KEY = "functions";
		static constexpr const char *DEC_KEY = "declaration";

	protected:
		static constexpr const routine_t on_dec_ = [](worker_t *, const tok_t&, dest_t& writer) {
			writer.set_key(DEC_KEY);	
			writer.start_writing();
		};
		static constexpr const routine_t on_done_ = [](worker_t *worker, const tok_t& token, dest_t& writer) {
			writer.stop_writing();
			writer.clear_key();
			writer.store(FUNCS_KEY);
			if (token.name == symbol_internal_t::SEMICOLON) {
				worker->reset();
			}
		};
};

class FuncWorker : public worker_internal_t, private FuncWorker_routines
{
	public:
		FuncWorker()
			: ParseWorker {
				TokenHandler(symbol_internal_t::TEXT, on_dec_),
				TokenHandler({ symbol_internal_t::SEMICOLON, symbol_internal_t::OPEN_BRACE }, on_done_, {
					IgnoreWorker(symbol_internal_t::NEWLINE, symbol_internal_t::TEXT)
				}),
				TokenHandler(symbol_internal_t::CLOSE_BRACE, nullptr, {
					IgnoreWorker(symbol_internal_t::OPEN_BRACE, symbol_internal_t::CLOSE_BRACE).recursive().block()
				})
			}
		{}
};

} // namespace parser_internal
} // namespace core
} // namespace docgen
