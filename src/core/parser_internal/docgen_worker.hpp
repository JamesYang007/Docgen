#pragma once

#include "../symbol.hpp"
#include "../token.hpp"
#include "../json_writer.hpp"
#include "../parse_worker.hpp"
#include "../worker_routine.hpp"

namespace docgen {
namespace core {
namespace parser_internal {

using token_class_t = Token<Symbol>;

using writer_class_t = JSONWriter<token_class_t>;

using worker_class_t = ParseWorker<token_class_t, writer_class_t>;

using routine_details_t = WorkerRoutine<worker_class_t>;

} // namespace parser_internal

inline bool operator==(const Token<Symbol>& t1, const Token<Symbol>& t2) {
	return t1.name == t2.name;
}

} // namespace core
} // namespace docgen

namespace std {

template <>
struct hash<docgen::core::Token<docgen::core::Symbol>>
{
	size_t operator()(const docgen::core::Token<docgen::core::Symbol>& t) const
	{
		return hash<size_t>()(static_cast<size_t>(t.name));
	}
};

} // namespace std
