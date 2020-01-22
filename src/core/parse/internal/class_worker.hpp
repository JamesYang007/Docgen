#pragma once

#include "core/parse/details.hpp"
#include "core/parse/internal/general_worker.hpp"

namespace docgen {
namespace core {
namespace parse {
namespace internal {

class ClassWorker : public worker_t
{
	public:
		ClassWorker()
			: worker_t ({
				TokenHandler({ symbol_t::CLASS, symbol_t::STRUCT }, Routines::on_keyword_),
				TokenHandler({ symbol_t::OPEN_BRACE, symbol_t::SEMICOLON }, Routines::on_open_),
				TokenHandler({ symbol_t::OPEN_BRACE, symbol_t::CLOSE_BRACE }, Routines::on_close_, { GeneralWorker() }),
				TokenHandler(symbol_t::SEMICOLON, Routines::on_done_)
			}, Routines::on_cleanup_)
		{}

	private:
		static constexpr const char * const CLASSES_KEY = "classes";
		static constexpr const char * const DEC_KEY = "declaration";

		struct Routines : private routine_details_t
		{
			static const routine_t on_open_; // defined out-of-line since this requires ClassWorker definition

			static constexpr const routine_t on_keyword_ = [](worker_t *, const token_t&, dest_t& writer) {
				if (writer.anything_written()) {
					writer.set_key(DEC_KEY);
					writer.start_writing();
				}
			};
			static constexpr const routine_t on_close_ = [](worker_t *worker, const token_t& token, dest_t&) {
				if (token == symbol_t::OPEN_BRACE) {
					worker->handler().tolerance(worker->handler().tolerance() + 2);
				}
			};
			static constexpr const routine_t on_done_ = [](worker_t *worker, const token_t& t, dest_t& writer) {
				worker->cleanup(t, writer);
				writer.store_to_pushback(CLASSES_KEY);
			};
			static constexpr const routine_t on_cleanup_ = [](worker_t *worker, const token_t&, dest_t&) {
				worker->handler_at(2).tolerance(1);
			};
			static constexpr const routine_t on_cleanup_within_ = [](worker_t *worker, const token_t& t, dest_t& writer) {
				writer.back_out();
				worker->on_cleanup(on_cleanup_);
				worker->cleanup(t, writer);
			};
		};
};

const routine_details_t::routine_t ClassWorker::Routines::on_open_ = [](worker_t *worker, const token_t& token, dest_t& writer) { 
	writer.stop_writing();
	if (token == symbol_t::SEMICOLON) {
		writer.store_to_pushback(CLASSES_KEY);
		worker->restart(token, writer);
		return;
	}
	if (writer.anything_written()) {
		writer.go_into_this();
		worker->handler_at(2).worker_at(0).handler_at(0).inject_worker(ClassWorker());	
		worker->on_cleanup(on_cleanup_within_);
	}
};

} // namespace internal
} // namespace parse
} // namespace core
} // namespace docgen
