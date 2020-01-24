#include <core/lex/lexer.hpp>
#include "lexer_base_fixture.hpp"

namespace docgen {
namespace core {
namespace lex {
namespace details {

struct LexTrieSubParamsGenerator
{
private:
    static constexpr auto string_symbol_arr = 
        make_pair_array<std::string_view, Symbol>({
            {"\n", Symbol::NEWLINE},
            {";", Symbol::SEMICOLON},
            {"*", Symbol::STAR},
            {"{", Symbol::OPEN_BRACE},
            {"}", Symbol::CLOSE_BRACE},
            {"///", Symbol::BEGIN_SLINE_COMMENT},
            {"/*!", Symbol::BEGIN_SBLOCK_COMMENT},
            {"*/", Symbol::END_BLOCK_COMMENT},
            {"@tparam", Symbol::TPARAM},
            {"@param", Symbol::PARAM},
        });

public:
    using type = utils::trie_params_t<
        std::decay_t<decltype(make_trie_params_input<string_symbol_arr>())>
    >;
};

} // namespace details

struct lexer_fixture : lexer_base_fixture
{
    using lex_trie_t = LexTrie<
        typename details::LexTrieSubParamsGenerator::type
    >;
    LexerGeneric<lex_trie_t> lexer;
    Lexer lexer_original;
};

BENCHMARK_F(lexer_fixture, data_1_test)(benchmark::State& st)
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

BENCHMARK_F(lexer_fixture, data_2_test)(benchmark::State& st)
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

BENCHMARK_F(lexer_fixture, data_3_test)(benchmark::State& st)
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

BENCHMARK_F(lexer_fixture, data_4_test)(benchmark::State& st)
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

BENCHMARK_F(lexer_fixture, data_1_test_original)(benchmark::State& st)
{
    for (auto _ : st) {
        std::unique_ptr<FILE, void (*)(FILE*)> file(fopen(data_1_path, "r"), 
                                                    [](FILE* file) {fclose(file);});
        int c = 0;
        while ((c = fgetc(file.get())) != EOF) {
            lexer_original.process(c);
        }
        benchmark::DoNotOptimize(lexer_original.next_token()); 
    }
}

BENCHMARK_F(lexer_fixture, data_2_test_original)(benchmark::State& st)
{
    for (auto _ : st) {
        std::unique_ptr<FILE, void (*)(FILE*)> file(fopen(data_2_path, "r"), 
                                                    [](FILE* file) {fclose(file);});
        int c = 0;
        while ((c = fgetc(file.get())) != EOF) {
            lexer_original.process(c);
        }
        benchmark::DoNotOptimize(lexer_original.next_token()); 
    }
}

BENCHMARK_F(lexer_fixture, data_3_test_original)(benchmark::State& st)
{
    for (auto _ : st) {
        std::unique_ptr<FILE, void (*)(FILE*)> file(fopen(data_3_path, "r"), 
                                                    [](FILE* file) {fclose(file);});
        int c = 0;
        while ((c = fgetc(file.get())) != EOF) {
            lexer_original.process(c);
        }
        benchmark::DoNotOptimize(lexer_original.next_token()); 
    }
}

BENCHMARK_F(lexer_fixture, data_4_test_original)(benchmark::State& st)
{
    for (auto _ : st) {
        std::unique_ptr<FILE, void (*)(FILE*)> file(fopen(data_4_path, "r"), 
                                                    [](FILE* file) {fclose(file);});
        int c = 0;
        while ((c = fgetc(file.get())) != EOF) {
            lexer_original.process(c);
        }
        benchmark::DoNotOptimize(lexer_original.next_token()); 
    }
}

} // namespace lex
} // namespace core
} // namespace docgen

