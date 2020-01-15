#pragma once

#include "../symbol.hpp"
#include "../token.hpp"
#include "../json_writer.hpp"
#include "../parse_worker.hpp"
#include "../worker_routine.hpp"

namespace docgen {
namespace core {
namespace parser_internal {

using symbol_internal_t = Symbol;

using token_internal_t = Token<symbol_internal_t>;

using writer_internal_t = JSONWriter<token_internal_t>;

using worker_internal_t = ParseWorker<token_internal_t, writer_internal_t>;

using routine_details_internal_t = WorkerRoutine<worker_internal_t>;

} // namespace parser_internal
} // namespace core
} // namespace docgen
