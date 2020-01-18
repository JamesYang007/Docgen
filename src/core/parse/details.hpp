#pragma once

#include "core/symbol.hpp"
#include "core/token.hpp"
#include "core/parse/core/json_writer.hpp"
#include "core/parse/core/parse_worker.hpp"

namespace docgen {
namespace core {
namespace parse {

using symbol_t = Symbol;

using token_t = Token<symbol_t>;

using writer_t = core::JSONWriter;

using worker_t = core::ParseWorker<token_t, writer_t>;

using routine_details_t = worker_t::RoutineDetails;

} // namespace parse
} // namespace core
} // namespace docgen
