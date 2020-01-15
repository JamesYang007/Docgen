#pragma once

namespace docgen {
namespace core {

/*
 * WorkerType must have type aliases for token_t and dest_t
 */
template <class WorkerType>
struct WorkerRoutine
{
	using worker_t = WorkerType;
	using tok_t = typename WorkerType::token_t;
	using dest_t = typename WorkerType::dest_t;
	using routine_t = void (*)(WorkerType *, const token_t&, dest_t&);
};

} // namespace core
} // namespace docgen
