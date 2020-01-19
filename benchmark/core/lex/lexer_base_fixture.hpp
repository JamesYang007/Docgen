#pragma once
#include <benchmark/benchmark.h>

namespace docgen {
namespace core {
namespace lex {

struct lexer_base_fixture : benchmark::Fixture 
{
    static constexpr const char* data_1_path = "data/data_1.txt";
    static constexpr const char* data_2_path = "data/data_2.txt";
    static constexpr const char* data_3_path = "data/data_3.txt";
    static constexpr const char* data_4_path = "data/data_3.txt";

    void SetUp(const ::benchmark::State& state) 
    {}

    void TearDown(const ::benchmark::State& state)
    {}
};

} // namespace lex
} // namespace core
} // namespace docgen
