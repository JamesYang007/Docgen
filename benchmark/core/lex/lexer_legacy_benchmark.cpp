#include <core/lex/legacy/lexer.hpp>
#include "lexer_base_fixture.hpp"

namespace docgen {
namespace core {
namespace lex {
namespace legacy {

struct lexer_legacy_fixture : lexer_base_fixture
{};

BENCHMARK_F(lexer_legacy_fixture, data_1_test)(benchmark::State& st)
{
    for (auto _ : st) {
        FILE* file = fopen(data_1_path, "r");
        Lexer lexer(file);
        lexer.process();
        benchmark::DoNotOptimize(lexer.get_tokens()[0]); 
        fclose(file);
    }
}

BENCHMARK_F(lexer_legacy_fixture, data_2_test)(benchmark::State& st)
{
    for (auto _ : st) {
        FILE* file = fopen(data_2_path, "r");
        Lexer lexer(file);
        lexer.process();
        benchmark::DoNotOptimize(lexer.get_tokens()[0]); 
        fclose(file);
    }
}

BENCHMARK_F(lexer_legacy_fixture, data_3_test)(benchmark::State& st)
{
    for (auto _ : st) {
        FILE* file = fopen(data_3_path, "r");
        Lexer lexer(file);
        lexer.process();
        benchmark::DoNotOptimize(lexer.get_tokens()[0]); 
        fclose(file);
    }
}

BENCHMARK_F(lexer_legacy_fixture, data_4_test)(benchmark::State& st)
{
    for (auto _ : st) {
        FILE* file = fopen(data_4_path, "r");
        Lexer lexer(file);
        lexer.process();
        benchmark::DoNotOptimize(lexer.get_tokens()[0]); 
        fclose(file);
    }
}

} // namespace legacy
} // namespace lex
} // namespace core
} // namespace docgen
