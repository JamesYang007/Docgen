#include <core/utils/trie.hpp>
//#include <core/symbol.hpp>
#include <gtest/gtest.h>

namespace docgen {
namespace core {
namespace utils {

template <const std::string_view& x>
struct A
{
    static constexpr auto yo = x;
};

enum class MockSymbol {
    symbol_0,
    symbol_1
};

struct trie_fixture : ::testing::Test
{
protected:
    using symbol_t = MockSymbol;
};

TEST_F(trie_fixture, get_first_chars)
{
    using namespace std::literals;
    static constexpr auto arr = make_array<std::string_view>({
        "a"sv,
        "abc"sv,
        "bc"sv
    });
    using array_t = make_array_constant_t<arr>;
    using first_chars_t = get_first_chars_t<array_t>;

    static_assert(first_chars_t::value[0] == 'a');
    static_assert(first_chars_t::value[1] == 'a');
    static_assert(first_chars_t::value[2] == 'b');
}

TEST_F(trie_fixture, remove_first_chars)
{
    using namespace std::literals;
    static constexpr auto arr = make_array<std::string_view>({
        "a"sv,
        "abc"sv,
        "bc"sv
    });
    using array_t = make_array_constant_t<arr>;
    using removed_first_chars_t = remove_first_chars_t<array_t>;

    static_assert(removed_first_chars_t::value[0] == "");
    static_assert(removed_first_chars_t::value[1] == "bc");
    static_assert(removed_first_chars_t::value[2] == "c");
}

TEST_F(trie_fixture, trie_params_empty)
{
    using namespace details;
    using namespace std::literals;
    using pair_t = std::pair<std::string_view, MockSymbol>;
    static constexpr std::array<pair_t, 0> pair_arr = {};
    using arr_t = make_array_constant_t<pair_arr>;
    static_assert(std::is_same_v<typename trie_params<arr_t>::type,
                                 utils::leaf_tag>);
}

TEST_F(trie_fixture, just_testing)
{
    using namespace std::literals;
    using type = A<"hello"sv>;
}

//TEST_F(trie_fixture, trie_params_depth_1)
//{
//    using namespace details;
//    using namespace std::literals;
//    using pair_t = std::pair<std::string_view, symbol_t>;
//    static constexpr auto pair_arr = make_array<pair_t>({
//        {"a"sv, symbol_t::symbol_0},
//        {"b"sv, symbol_t::symbol_1}
//    });
//    using arr_t = make_array_constant_t<pair_arr>;
//    using param_t = typename trie_params<arr_t>::type;
//
//    using first_t = typename param_t::first_type;
//    using second_t = typename param_t::second_type;
//
//    static_assert(first_t::value.size() == 2);
//    static_assert(first_t::value[0] == 'a');
//    static_assert(first_t::value[1] == 'b');
//
//    static_assert(std::is_same_v<
//            std::tuple_element_t<0, std::tuple_element_t<0, second_t>>,
//            utils::leaf_tag
//            >);
//
//    static_assert(
//            std::tuple_element_t<1, std::tuple_element_t<0, second_t>>::value 
//            ==
//            true
//            );
//
//    static_assert(
//            std::tuple_element_t<2, std::tuple_element_t<0, second_t>>::value 
//            ==
//            symbol_t::symbol_0
//            );
//
//    static_assert(std::is_same_v<
//            std::tuple_element_t<0, std::tuple_element_t<1, second_t>>,
//            utils::leaf_tag
//            >);
//
//    static_assert(
//            std::tuple_element_t<1, std::tuple_element_t<1, second_t>>::value 
//            ==
//            true
//            );
//
//    static_assert(
//            std::tuple_element_t<2, std::tuple_element_t<1, second_t>>::value 
//            ==
//            symbol_t::symbol_1
//            );
//}
//
//TEST_F(trie_fixture, trie_params_depth_2)
//{
//    using namespace details;
//    using namespace std::literals;
//    using pair_t = std::pair<std::string_view, symbol_t>;
//    static constexpr auto pair_arr = make_array<pair_t>({
//        {"a"sv, symbol_t::symbol_0},
//        {"ab"sv, symbol_t::symbol_1}
//    });
//
//    using arr_t = make_array_constant_t<pair_arr>;
//    using param_t = typename trie_params<arr_t>::type;
//
//    using first_t = typename param_t::first_type;
//    using second_t = typename param_t::second_type;
//
//    static_assert(first_t::value.size() == 1);
//    static_assert(first_t::value[0] == 'a');
//
//    using depth_1_t = std::tuple_element_t<0, std::tuple_element_t<0, second_t>>;
//    using depth_1_first_t = typename depth_1_t::first_type;
//    using depth_1_second_t = typename depth_1_t::second_type;
//
//    static_assert(depth_1_first_t::value.size() == 1);
//    static_assert(depth_1_first_t::value[0] == 'b');
//
//    static_assert(std::is_same_v<
//            std::tuple_element_t<0, std::tuple_element_t<0, depth_1_second_t>>,
//            utils::leaf_tag
//            >);
//
//    static_assert(
//            std::tuple_element_t<1, std::tuple_element_t<0, depth_1_second_t>>::value 
//            ==
//            true
//            );
//
//    static_assert(
//            std::tuple_element_t<2, std::tuple_element_t<0, depth_1_second_t>>::value 
//            ==
//            symbol_t::symbol_1
//            );
//
//    static_assert(
//            std::tuple_element_t<1, std::tuple_element_t<0, second_t>>::value 
//            ==
//            true
//            );
//
//    static_assert(
//            std::tuple_element_t<2, std::tuple_element_t<0, second_t>>::value 
//            ==
//            symbol_t::symbol_0
//            );
//
//}
//
//TEST_F(trie_fixture, trie_params_real)
//{
//    using namespace details;
//    using namespace std::literals;
//    using pair_t = std::pair<std::string_view, Symbol>;
//    static constexpr auto pair_arr = make_array<pair_t>({
//         {"\n", Symbol::NEWLINE},
//         {" ", Symbol::WHITESPACE},
//         {"\t", Symbol::WHITESPACE},
//         {"\v", Symbol::WHITESPACE},
//         {"\r", Symbol::WHITESPACE},
//         {"\f", Symbol::WHITESPACE},
//         {";", Symbol::SEMICOLON},
//         {"#", Symbol::HASHTAG},
//         {"*", Symbol::STAR},
//         {"{", Symbol::OPEN_BRACE},
//         {"}", Symbol::CLOSE_BRACE},
//         {"///", Symbol::BEGIN_SLINE_COMMENT},
//         {"/*!", Symbol::BEGIN_SBLOCK_COMMENT},
//         {"//", Symbol::BEGIN_NLINE_COMMENT},
//         {"/*", Symbol::BEGIN_NBLOCK_COMMENT},
//         {"*/", Symbol::END_BLOCK_COMMENT},
//         {"@sdesc", Symbol::SDESC},
//         {"@tparam", Symbol::TPARAM},
//         {"@param", Symbol::PARAM},
//         {"@return", Symbol::RETURN}
//    });
//
//    using arr_t = make_array_constant_t<pair_arr>;
//    using param_t = typename trie_params<arr_t>::type;
//
//    //using first_t = typename param_t::first_type;
//    //using second_t = typename param_t::second_type;
//
//    //static_assert(first_t::value.size() == 1);
//    //static_assert(first_t::value[0] == 'a');
//
//    //using depth_1_t = std::tuple_element_t<0, std::tuple_element_t<0, second_t>>;
//    //using depth_1_first_t = typename depth_1_t::first_type;
//    //using depth_1_second_t = typename depth_1_t::second_type;
//
//    //static_assert(depth_1_first_t::value.size() == 1);
//    //static_assert(depth_1_first_t::value[0] == 'b');
//
//    //static_assert(std::is_same_v<
//    //        std::tuple_element_t<0, std::tuple_element_t<0, depth_1_second_t>>,
//    //        utils::leaf_tag
//    //        >);
//
//    //static_assert(
//    //        std::tuple_element_t<1, std::tuple_element_t<0, depth_1_second_t>>::value 
//    //        ==
//    //        true
//    //        );
//
//    //static_assert(
//    //        std::tuple_element_t<2, std::tuple_element_t<0, depth_1_second_t>>::value 
//    //        ==
//    //        symbol_t::symbol_1
//    //        );
//
//    //static_assert(
//    //        std::tuple_element_t<1, std::tuple_element_t<0, second_t>>::value 
//    //        ==
//    //        true
//    //        );
//
//    //static_assert(
//    //        std::tuple_element_t<2, std::tuple_element_t<0, second_t>>::value 
//    //        ==
//    //        symbol_t::symbol_0
//    //        );
//
//}
    
} // namespace utils
} // namespace core
} // namespace docgen
