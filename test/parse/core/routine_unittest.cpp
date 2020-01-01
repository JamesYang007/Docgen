#include <parse/core/routine.hpp>
#include <parse/core/cache.hpp>
#include <gtest/gtest.h>

namespace docgen {
namespace parse {
namespace core {

struct routine_fixture : ::testing::Test
{
protected:
    using cache_t = Cache<10>;
    using routine_array_t = routine_array_t<cache_t>;
    static constexpr routine_array_t routines = make_routines<cache_t>();
    cache_t cache;
    std::string doc = 
        "#include <some_dir/some_header.hpp>\n"
        "\n"
        "  //   some comment"
        ;
};

// make_routines TEST
    
TEST_F(routine_fixture, routine_array_constexprness)
{
    static_assert(routines.size() == static_cast<size_t>(Routine::NUM_ROUTINES),
                  "routines not constructed at compile-time");
}

TEST_F(routine_fixture, routine_array_ordering)
{
    static constexpr routine_array_t routines = make_routines<cache_t, 3>();
    EXPECT_EQ(routines[0], &routine<static_cast<Routine>(0)>::template run<cache_t>);
    EXPECT_EQ(routines[1], &routine<static_cast<Routine>(1)>::template run<cache_t>);
    EXPECT_EQ(routines[2], &routine<static_cast<Routine>(2)>::template run<cache_t>);
}

// process_slash TEST
TEST_F(routine_fixture, slash_end_batch)
{
    using namespace details;
    cache.symbol.clear();
    cache.symbol.push_back('/');
    const char* begin = doc.c_str() + doc.length();
    const char* end = begin;
    Routine actual = process_slash(cache, begin, end); 
    Routine expected = Routine::READ;
    EXPECT_EQ(actual, expected);
}

} // namespace core
} // namespace parse
} // namespace docgen
