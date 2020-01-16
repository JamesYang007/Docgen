#pragma once

#include "core/symbol.hpp"
#include "core/token.hpp"
#include "core/json_writer.hpp"
#include "core/parse_worker.hpp"
#include "core/worker_routine.hpp"

namespace docgen {
namespace core {
namespace parser_internal {

using symbol_t = Symbol;

using token_t = Token<symbol_t>;

using writer_t = JSONWriter;

using worker_t = ParseWorker<token_t, writer_t>;

using routine_details_t = WorkerRoutine<worker_t>;

} // namespace parser_internal
} // namespace core
} // namespace docgen
