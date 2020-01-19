#include <core/lex/lexer.hpp>
#include <benchmark/benchmark.h>

namespace docgen {
namespace core {
namespace lex {

struct lexer_fixture : benchmark::Fixture 
{
    static constexpr const char* data_1_path = "data/data_1.txt";
    static constexpr const char* data_2_path = "data/data_2.txt";
    static constexpr const char* data_3_path = "data/data_3.txt";

    Lexer lexer;

    void SetUp(const ::benchmark::State& state) 
    {
    }

    void TearDown(const ::benchmark::State& state)
    {
    }
};

BENCHMARK_F(lexer_fixture, data_1_test)(benchmark::State& st)
{
    std::unique_ptr<FILE, void (*)(FILE*)> file(fopen(data_1_path, "r"), 
                                                [](FILE* file) {fclose(file);});
    int c = 0;
    for (auto _ : st) {
        while ((c = getc(file.get())) != EOF) {
            lexer.process(c);
        }
    }
}

} // namespace lex
} // namespace core
} // namespace docgen

