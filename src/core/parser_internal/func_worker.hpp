#pragma once

#include "docgen_worker.hpp"
#include "ignore_worker.hpp"

namespace docgen {
namespace core {
namespace parser_internal {

struct FuncWorker_routines : private routine_details_t
{
	private:
		static constexpr const char *FUNCS_KEY = "functions";
		static constexpr const char *DEC_KEY = "declaration";

	protected:
		static constexpr const routine_t on_dec_ = [](worker_t *, const tok_t&, dest_t& writer) {
			writer.set_key(DEC_KEY);	
			writer.start_writing();
		};
		static constexpr const routine_t on_done_ = [](worker_t *worker, const tok_t& t, dest_t& writer) {
			writer.stop_writing();
			writer.clear_key();
			writer.store(FUNCS_KEY);
			if (t.name == Symbol::SEMICOLON) {
				worker->reset();
			}
		};
};

class FuncWorker : public worker_class_t, private FuncWorker_routines
{
	public:
		FuncWorker()
			: ParseWorker {
				TokenHandler(Symbol::TEXT, on_dec_),
				TokenHandler({ Symbol::SEMICOLON, Symbol::OPEN_BRACE }, on_done_, {
					IgnoreWorker(Symbol::NEWLINE, Symbol::TEXT)
				}),
				TokenHandler(Symbol::CLOSE_BRACE, nullptr, {
					IgnoreWorker(Symbol::OPEN_BRACE, Symbol::CLOSE_BRACE).recursive().block()
				})
			}
		{}
};

} // namespace parser_internal
} // namespace core
} // namespace docgen
