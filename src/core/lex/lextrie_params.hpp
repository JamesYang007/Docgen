#pragma once
#include <type_traits>
#include <string_view>
#include <utility>
#include <core/symbol.hpp>
#include <core/utils/trie_params.hpp>
#include <array>

namespace docgen {
namespace core {
namespace lex {

/////////////////////////////////////////////////////////////////
// Lexer type setup
/////////////////////////////////////////////////////////////////

template <class FirstType, class SecondType, size_t N, size_t... I>
static constexpr auto make_pair_array_helper(
        const std::pair<FirstType, SecondType> (&list)[N],
        std::index_sequence<I...>
        )
{
    using pair_t = std::pair<FirstType, SecondType>;
    return std::array<pair_t, N>{{list[I]...}};
}

// Variadic number of pairs to array of pairs
template <class FirstType, class SecondType, size_t N>
static constexpr auto make_pair_array(
        const std::pair<FirstType, SecondType> (&list)[N]
        )
{
    return make_pair_array_helper(list, std::make_index_sequence<N>());
}

template <const auto& arr, size_t I>
struct get_array_element
{
    using value_type = std::decay_t<typename std::decay_t<decltype(arr)>::value_type>;
    static constexpr value_type value = arr[I];
};

template <const auto& arr, size_t I>
inline constexpr auto get_array_element_v = get_array_element<arr, I>::value;

template <const auto& pair, size_t I>
struct get_pair_element
{
    using value_type = std::conditional_t<
        I == 0,
        std::decay_t<typename std::decay_t<decltype(pair)>::first_type>,
        std::decay_t<typename std::decay_t<decltype(pair)>::second_type>
    >;
    static constexpr value_type value = std::get<I>(pair);
};

template <const auto& pair, size_t I>
inline constexpr auto get_pair_element_v = get_pair_element<pair, I>::value;

template <const auto& string_symbol_arr, size_t... I>
static constexpr auto make_trie_params_input(std::index_sequence<I...>)
{
    return utils::typelist<
        utils::typelist<
            utils::sv_to_valuelist_t<
                get_pair_element_v<
                    get_array_element_v<string_symbol_arr, I>, 
                    0
                >
            >,
            std::integral_constant<
                Symbol, 
                get_pair_element_v<
                    get_array_element_v<string_symbol_arr, I>, 
                    1 
                >
            >
        >...
    >();
}

template <const auto& string_symbol_arr>
static constexpr auto make_trie_params_input()
{
    return make_trie_params_input<string_symbol_arr>(
        std::make_index_sequence<string_symbol_arr.size()>()
    );
}

struct LexTrieParamsGenerator 
{
private:
    static constexpr auto string_symbol_arr = 
        make_pair_array<std::string_view, Symbol>({
            {"\n", Symbol::NEWLINE},
            {" ", Symbol::WHITESPACE},
            {"\t", Symbol::WHITESPACE},
            {"\v", Symbol::WHITESPACE},
            {"\r", Symbol::WHITESPACE},
            {"\f", Symbol::WHITESPACE},
            {";", Symbol::SEMICOLON},
            {"#", Symbol::HASHTAG},
            {"*", Symbol::STAR},
            {"{", Symbol::OPEN_BRACE},
            {"}", Symbol::CLOSE_BRACE},
	    {"<", Symbol::OPEN_ANGLE_BRACKET },
	    {">", Symbol::CLOSE_ANGLE_BRACKET },
            {"/", Symbol::FORWARD_SLASH},
            {"///", Symbol::BEGIN_SLINE_COMMENT},
            {"/*!", Symbol::BEGIN_SBLOCK_COMMENT},
            {"//", Symbol::BEGIN_NLINE_COMMENT},
            {"/*", Symbol::BEGIN_NBLOCK_COMMENT},
            {"*/", Symbol::END_BLOCK_COMMENT},
	    {"template", Symbol::TEMPLATE },
	    {"class", Symbol::CLASS },
	    {"struct", Symbol::STRUCT },
            {"@sdesc", Symbol::SDESC},
            {"@tparam", Symbol::TPARAM},
            {"@param", Symbol::PARAM},
            {"@return", Symbol::RETURN}
        });

public:
    using type = utils::trie_params_t<
        std::decay_t<decltype(make_trie_params_input<string_symbol_arr>())>
    >;
};

////////////////////////////////////////////////////////////
// LexTrieParams Typedef 
////////////////////////////////////////////////////////////
using lextrie_params_t = typename LexTrieParamsGenerator::type;

} // namespace lex
} // namespace core
} // namespace docgen
