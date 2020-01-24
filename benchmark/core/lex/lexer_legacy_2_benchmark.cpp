#include <core/lex/legacy_2/lexer.hpp>
#include "lexer_base_fixture.hpp"

namespace docgen {
namespace core {
namespace lex {

struct lexer_legacy_2_fixture : lexer_base_fixture
{
    Lexer lexer;
};

BENCHMARK_F(lexer_legacy_2_fixture, data_1_test)(benchmark::State& st)
{
    for (auto _ : st) {
        std::unique_ptr<FILE, void (*)(FILE*)> file(fopen(data_1_path, "r"), 
                                                    [](FILE* file) {fclose(file);});
        int c = 0;
        while ((c = fgetc(file.get())) != EOF) {
            lexer.process(c);
        }
        benchmark::DoNotOptimize(lexer.next_token()); 
    }
}

BENCHMARK_F(lexer_legacy_2_fixture, data_2_test)(benchmark::State& st)
{
    for (auto _ : st) {
        std::unique_ptr<FILE, void (*)(FILE*)> file(fopen(data_2_path, "r"), 
                                                    [](FILE* file) {fclose(file);});
        int c = 0;
        while ((c = fgetc(file.get())) != EOF) {
            lexer.process(c);
        }
        benchmark::DoNotOptimize(lexer.next_token()); 
    }
}

BENCHMARK_F(lexer_legacy_2_fixture, data_3_test)(benchmark::State& st)
{
    for (auto _ : st) {
        std::unique_ptr<FILE, void (*)(FILE*)> file(fopen(data_3_path, "r"), 
                                                    [](FILE* file) {fclose(file);});
        int c = 0;
        while ((c = fgetc(file.get())) != EOF) {
            lexer.process(c);
        }
        benchmark::DoNotOptimize(lexer.next_token()); 
    }
}

BENCHMARK_F(lexer_legacy_2_fixture, data_4_test)(benchmark::State& st)
{
    for (auto _ : st) {
        std::unique_ptr<FILE, void (*)(FILE*)> file(fopen(data_4_path, "r"), 
                                                    [](FILE* file) {fclose(file);});
        int c = 0;
        while ((c = fgetc(file.get())) != EOF) {
            lexer.process(c);
        }
        benchmark::DoNotOptimize(lexer.next_token()); 
    }
}

} // namespace lex
} // namespace core
} // namespace docgen

