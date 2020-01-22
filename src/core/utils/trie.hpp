#pragma once
#include <core/utils/array_constant.hpp>
#include <string_view>
#include <mapbox/eternal.hpp>

namespace docgen {
namespace core {
namespace utils {
namespace details {

struct IsEmpty
{
    static constexpr bool evaluate(const std::string_view& str)
    {
        return str.empty();
    }
};

struct IsNonEmpty
{
    static constexpr bool evaluate(const std::string_view& str)
    {
        return !IsEmpty::evaluate(str);
    }
};

template <class CharConstant>
struct BeginsWith
{
    static constexpr bool evaluate(const std::string_view& str)
    {
        return str[0] == CharConstant::value;
    }
};

} // namespace details

////////////////////////////////////////////////////////////
// get_first_chars
////////////////////////////////////////////////////////////

namespace details {

template <class ArrayConstant>
struct get_first_chars
{
private:
    static_assert(is_array_constant_v<ArrayConstant>);

    using raw_arr_t = std::decay_t<typename ArrayConstant::value_type>; 
    static_assert(std::is_same_v<typename raw_arr_t::value_type, std::string_view>);

    template <size_t... I>
    static constexpr auto evaluate(std::index_sequence<I...>)
    {
        static_assert(((ArrayConstant::value[I].size() > 0) && ...));
        return utils::make_array<char>({ArrayConstant::value[I][0] ...});
    }

    static constexpr auto evaluate()
    {
        return evaluate(std::make_index_sequence<ArrayConstant::value.size()>());
    }

    static constexpr auto value = evaluate();

public:
    using type = make_array_constant_t<value>;
};

} // namespace details

template <class ArrayConstant>
using get_first_chars_t = typename details::get_first_chars<ArrayConstant>::type;

////////////////////////////////////////////////////////////
// remove_first_chars
////////////////////////////////////////////////////////////

namespace details {

template <class ArrayConstant>
struct remove_first_chars
{
private:
    static_assert(is_array_constant_v<ArrayConstant>);

    using raw_arr_t = std::decay_t<typename ArrayConstant::value_type>; 
    static_assert(std::is_same_v<typename raw_arr_t::value_type, std::string_view>);

    template <size_t... I>
    static constexpr auto evaluate(std::index_sequence<I...>)
    {
        static_assert(((ArrayConstant::value[I].size() > 0) && ...));
        return utils::make_array<std::string_view>({ArrayConstant::value[I].substr(1) ...});
    }

    static constexpr auto evaluate()
    {
        return evaluate(std::make_index_sequence<ArrayConstant::value.size()>());
    }

    static constexpr auto value = evaluate();

public:
    using type = make_array_constant_t<value>;
};

} // namespace details

template <class ArrayConstant>
using remove_first_chars_t = typename details::remove_first_chars<ArrayConstant>::type;

////////////////////////////////////////////////////////////
// has_empty_v
////////////////////////////////////////////////////////////

template <class StringArrayConstant>
inline constexpr bool has_empty_string_v = 
    subset_idx_t<StringArrayConstant, details::IsNonEmpty>::value.size() !=
    StringArrayConstant::value.size();

////////////////////////////////////////////////////////////
// LexTrie 
//
// This is the core logic of lexer tree.
// PairArrayConstant is an array of pairs of char constant types
// and a variant type representing all possible children LexTrie types.
////////////////////////////////////////////////////////////

template <class PairArrayConstant>
struct LexTrie
{
    explicit constexpr LexTrie(bool is_accept) noexcept
        : is_accept_(is_accept)
    {}

private:
    using children_map_t = std::decay_t<
        decltype(mapbox::eternal::map(PairArrayConstant::value))
        >;
    const bool is_accept_ = false;
    // add optional symbol member
    // no need to add parent pointer
    // add is_root_?
    children_map_t children_ = mapbox::eternal::map(PairArrayConstant::value);
    typename children_map_t::value_type* active_child = nullptr;
};

////////////////////////////////////////////////////////////
// make_trie 
//
// Compiler error is raised if the first type of the pair is not std::string_view.
////////////////////////////////////////////////////////////

// A tag to indicate empty Trie structure.
struct leaf_tag
{};

namespace details {

// This is used in a very special case when we know
// IdxArrayConstant is either size 0 or 1,
// and ArrayConstant is non-empty.
template <class ArrayConstant, class IdxArrayConstant
        , bool = (IdxArrayConstant::value.size() > 0)>
struct choose_symbol
{
    // if condition true, then IdxArrayConstant has only 1 element!
    using type = get_element_t<
        ArrayConstant, 
        get_element_t<IdxArrayConstant, 0>::value
    >;
};

template <class ArrayConstant, class IdxArrayConstant>
struct choose_symbol<ArrayConstant, IdxArrayConstant, false>
{
    // this is an arbitrary, but always well-defined, choice
    using type = get_element_t<ArrayConstant, 0>;
};

template <class ArrayConstant, class IdxArrayConstant>
using choose_symbol_t = 
    typename choose_symbol<ArrayConstant, IdxArrayConstant>::type;

// Makes parameters to construct LexTrie.
// The expected final type is of the form 
// std::tuple<
//  unique set of chars, 
//  tuple of each corresponding final type, 
//  boolean indicating accept state
// >
// The final type corresponds to a leaf in the LexTrie iff set of chars is empty.
// PairArrayConstant: array of pair<string view, user-defined class>
template <class PairArrayConstant
        , bool = !PairArrayConstant::value.empty()>
struct trie_params
{
//private:

    static_assert(is_array_constant_v<PairArrayConstant>);

    // get string and value array (value is the second type in pair)
    using string_arr_t = unzip_t<PairArrayConstant, 0>;
    using value_arr_t = unzip_t<PairArrayConstant, 1>;

    // find array of first chars (well-defined)
    using first_char_array_t = get_first_chars_t<string_arr_t>;

    // unique set of first chars (order not preserved)
    using char_set_t = set_t<first_char_array_t>;

    // get subset index where strings begin with Ith (unique) char in set
    template <size_t I>
    using subset_idx_by_char_t = subset_idx_t<string_arr_t, BeginsWith<get_element_t<char_set_t, I>>>;

    // get subset of string and value for ith char in set
    template <size_t I>
    using subset_string_by_char_t = subset_t<string_arr_t, subset_idx_by_char_t<I>>;
    template <size_t I>
    using subset_value_by_char_t = subset_t<value_arr_t, subset_idx_by_char_t<I>>;

    template <size_t I>
    using remove_first_char_t = remove_first_chars_t<subset_string_by_char_t<I>>;

    template <size_t I>
    using pruned_subset_string_t = subset_t<
        remove_first_char_t<I>, 
        subset_idx_t<remove_first_char_t<I>, IsNonEmpty>
            >;

    template <size_t I>
    using pruned_subset_value_t = subset_t<
        subset_value_by_char_t<I>, 
        subset_idx_t<remove_first_char_t<I>, IsNonEmpty>
            >;

    // get new subset of pairs with ith char in set removed from beginning of the strings
    // Note: strings may now be empty after removing first char.
    // If this is the case, current LexTrie is accepting and the second boolean constant is true.
    // Otherwise, it is not accepting and the second boolean constant is false.
    // In either case, we remove all empty strings.    
    template <size_t I>
    using subtrie_params_t = std::tuple<
        typename trie_params<
            zip_t<
                pruned_subset_string_t<I>,
                pruned_subset_value_t<I>
            >
        >::type,
        std::integral_constant<
            bool, 
            has_empty_string_v<remove_first_char_t<I>>
        >,
        choose_symbol_t<
            subset_value_by_char_t<I>, 
            subset_idx_t<remove_first_char_t<I>, details::IsEmpty>
        >
    >;
    
    template <size_t... I>
    static constexpr auto make_subtrie_params_tuple(
                std::index_sequence<I...>)
    {
        return std::tuple<subtrie_params_t<I>...>();
    }

    static constexpr auto make_subtrie_params_tuple()
    {
        return make_subtrie_params_tuple(
                std::make_index_sequence<char_set_t::value.size()>()
                );
    }

    using subtrie_params_tuple_t = std::decay_t<
        decltype(make_subtrie_params_tuple())
    >;

public:
    using type = std::pair<char_set_t, subtrie_params_tuple_t>;
};

template <class PairArrayConstant>
struct trie_params<PairArrayConstant, false>
{
private:
    static_assert(is_array_constant_v<PairArrayConstant>);
public:
    using type = leaf_tag;
};

} // namespace details

template <class PairArrayConstant>
inline constexpr auto make_trie()
{
    using raw_pair_arr_t = std::decay_t<typename PairArrayConstant::value_type>;
    using pair_t = typename raw_pair_arr_t::value_type;
    using first_t = typename pair_t::first_type;

    // check that first type in pair is std::string_view
    static_assert(std::is_same_v<first_t, std::string_view>,
            "First pair type must be std::string_view");

    // check that array is non-empty
    static_assert(!PairArrayConstant::value.empty());

    using string_arr_t = unzip_t<PairArrayConstant>;

    // check that all strings are unique
    static_assert(is_unique_v<string_arr_t>);    
}

} // namespace utils
} // namespace core
} // namespace docgen
