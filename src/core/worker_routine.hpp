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
	using token_t = typename worker_t::token_t;
	using dest_t = typename worker_t::dest_t;
	using routine_t = void (*)(worker_t *, const token_t&, dest_t&);
};

} // namespace core
} // namespace docgen
