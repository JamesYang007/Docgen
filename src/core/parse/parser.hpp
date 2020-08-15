#pragma once

#include <initializer_list>
#include <nlohmann/json.hpp>
#include "core/parse/details.hpp"
#include "core/parse/internal/general_worker.hpp"
#include "core/parse/internal/class_worker.hpp"

namespace docgen {
namespace core {
namespace parse {

class Parser
{
	public:
		void process(const token_t& token);

		nlohmann::json& parsed() { return writer_.stored(); }
		const nlohmann::json& parsed() const { return writer_.stored(); }

	private:	
		worker_t worker_ = internal::GeneralWorker().inject_worker_at(0, internal::ClassWorker());
		writer_t writer_;
};

inline void Parser::process(const token_t& token)
{
	worker_.proc(token, writer_);
	writer_.feed(token.c_str());
}

} // namespace parse
} // namespace docgen
} // namespace core
