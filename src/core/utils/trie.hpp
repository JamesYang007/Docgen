#pragma once
#include <core/utils/list.hpp>
#include <string_view>
#include <optional>
#include <variant>
#include <mapbox/eternal.hpp>

namespace docgen {
namespace core {
namespace utils {

////////////////////////////////////////////////////////////
// sv_to_valuelist
////////////////////////////////////////////////////////////

namespace details {

template <const std::string_view& str>
struct sv_to_valuelist
{
private:

    template <size_t... I>
    static constexpr auto evaluate(std::index_sequence<I...>)
    {
        return valuelist<char, str[I]...>();
    }

    static constexpr auto evaluate()
    {
        return evaluate(std::make_index_sequence<str.size()>());
    }

public:
    using type = std::decay_t<decltype(evaluate())>;    
};

} // namespace details

template <const std::string_view& str>
using sv_to_valuelist_t = typename details::sv_to_valuelist<str>::type;

////////////////////////////////////////////////////////////
// has_empty_valuelist_v
//
// Intended for typelist< valuelist<...>, valuelist<...>, ...>
////////////////////////////////////////////////////////////

template <class List>
inline constexpr bool has_empty_valuelist_v = 
    size_v<subset_idx_t<List, details::is_not_empty>> !=
    size_v<List>;

// A tag to indicate empty Trie structure.
struct leaf_tag
{};

////////////////////////////////////////////////////////////
// choose_symbol
//
// IdxList is either size 0 or 1, and List is non-empty.
// If IdxList is non-empty (size == 1), resulting type is List 
// at IdxList's only value.
// Otherwise, undefined type, but one of List types, is returned.
////////////////////////////////////////////////////////////

namespace details {

template <class List, class IdxList
        , bool = !is_empty_v<IdxList>>
struct choose_symbol
{
private:
    static_assert(size_v<IdxList> == 1); // if non-empty, must only have 1 element
    static_assert(!is_empty_v<List>);
    static_assert(is_typelist_v<List>);
    static_assert(is_valuelist_v<IdxList>);

public:
    using type = get_t<List, get_v<IdxList, 0>>;
};

template <class List, class IdxList>
struct choose_symbol<List, IdxList, false>
{
    static_assert(is_typelist_v<List>);
    static_assert(!is_empty_v<List>);
    // this is an arbitrary, but always well-defined, choice
    using type = get_t<List, 0>;
};

} // namespace details

template <class List, class IdxList>
using choose_symbol_t = typename details::choose_symbol<List, IdxList>::type;

////////////////////////////////////////////////////////////
// trie_params
//
// Makes parameters to construct LexTrie.
// The expected final type is of the form 
// typelist <
//  valuelist <unique set of chars>, 
//  typelist <
//      typelist < 
//          child's corresponding final sub-(trie_params::type), 
//          boolean indicating child's accept state,
//          child's symbol if in accept state (otherwise undefined value)
//      >
//      ... (for every child)
//  >
// >
// The final type corresponds to a leaf in the LexTrie iff set of chars is empty.
////////////////////////////////////////////////////////////

namespace details {

// Class template to check if a valuelist has first value equal to c.
template <char c>
struct BeginsWith
{
    template <class List>
    struct Evaluator 
    {
        static_assert(!is_empty_v<List>, ERR_EMPTY_LIST);
        static_assert(is_valuelist_v<List>);
        static constexpr bool value = (List::value == c);
    };
};

// Removes first element from a given valuelist
// Wraps left_shift to only take in one template parameter.
template <class List>
struct remove_first_element
{
    using type = left_shift_t<List>;
};

// Checks if List is empty.
// Wraps is_empty to only take in one template parameter.
template <class List>
struct is_empty_wrapper
{
    static constexpr bool value = is_empty_v<List>;
};

template <class PairList
        , bool = !is_empty_v<PairList>>
struct trie_params
{
private: 

    static_assert(is_typelist_v<PairList>);

    // get string and value array (value is the second type in pair)
    using unzipped_t = unzip_t<PairList>;
    using valuelists_t = get_t<unzipped_t, 0>;
    using constants_t = get_t<unzipped_t, 1>;

    // find array of first chars (well-defined)
    using first_char_list_t = get_first_value_t<valuelists_t>;

    // unique set of first chars (order not preserved)
    using char_set_t = set_t<first_char_list_t>;

    // get subset index where strings begin with Ith (unique) char in set
    template <size_t I>
    using subset_idx_by_char_t = subset_idx_t<
        valuelists_t, 
        BeginsWith<get_v<char_set_t, I>>::template Evaluator
    >;

    // get subset of valuelists for ith char in set
    template <size_t I>
    using subset_valuelists_by_char_t = subset_t<valuelists_t, subset_idx_by_char_t<I>>;

    // get subset of constants for ith char in set
    template <size_t I>
    using subset_constants_by_char_t = subset_t<constants_t, subset_idx_by_char_t<I>>;

    // remove first char from all valuelists associated with ith char in set
    template <size_t I>
    using remove_first_char_t = transform_t<
        subset_valuelists_by_char_t<I>,
        remove_first_element
    >;

    // get subset of non-empty strings after removing first char
    template <size_t I>
    using pruned_subset_string_t = subset_t<
        remove_first_char_t<I>, 
        subset_idx_t<remove_first_char_t<I>, is_not_empty>
    >;

    // get subset of constants associated with non-empty strings after removing first char
    template <size_t I>
    using pruned_subset_value_t = subset_t<
        subset_constants_by_char_t<I>, 
        subset_idx_t<remove_first_char_t<I>, is_not_empty>
    >;

    // get new subset of pairs with ith char in set removed from beginning of the strings
    // Note: strings may now be empty after removing first char.
    // If this is the case, current LexTrie is accepting and the second boolean constant is true.
    // Otherwise, it is not accepting and the second boolean constant is false.
    // In either case, we remove all empty strings.    
    template <size_t I>
    using subtrie_params_t = typelist<
        typename trie_params<
            zip_t<
                pruned_subset_string_t<I>,
                pruned_subset_value_t<I>
            >
        >::type,
        std::integral_constant<
            bool, 
            has_empty_valuelist_v<remove_first_char_t<I>>
        >,
        choose_symbol_t<
            subset_constants_by_char_t<I>, 
            subset_idx_t<remove_first_char_t<I>, is_empty_wrapper>
        >
    >;
    
    template <size_t... I>
    static constexpr auto make_subtrie_params_list(
                std::index_sequence<I...>)
    {
        return typelist<subtrie_params_t<I>...>();
    }

    static constexpr auto make_subtrie_params_list()
    {
        return make_subtrie_params_list(
                std::make_index_sequence<size_v<char_set_t>>()
                );
    }

    using subtrie_params_list_t = std::decay_t<
        decltype(make_subtrie_params_list())
    >;

public:
    using type = typelist<char_set_t, subtrie_params_list_t>;
};

template <class PairList>
struct trie_params<PairList, false>
{
private:
    static_assert(is_typelist_v<PairList>);
public:
    using type = leaf_tag;
};

} // namespace details

template <class PairList>
using trie_params_t = typename details::trie_params<PairList>::type;

////////////////////////////////////////////////////////////
// LexTrie 
//
// This is the core logic of lexer tree.
// TrieParams is expected to be a result of calling trie_params_t.
////////////////////////////////////////////////////////////

namespace details {

// Very specific helper for LexTrie to determine the symbol type
// given trie_params type.
template <class TrieParams>
using get_symbol_type_t = typename get_t<
    get_t<unzip_t<get_t<TrieParams, 1>>, 2>, 
    0
>::value_type;

} // namespace details

// Primary definition assumes non-empty children.
// The specialization for leaf_tag is for empty children.
template <class TrieParams
        , class SymbolType = details::get_symbol_type_t<TrieParams>
        >
class LexTrie
{
    ////////////////////////////////////////
    // Helper alias and functions
    ////////////////////////////////////////
    
    using char_list_t = get_t<TrieParams, 0>;
    static_assert(!is_empty_v<char_list_t>);
    static_assert(is_valuelist_v<char_list_t>);

    using subtrie_info_t = get_t<TrieParams, 1>;
    static_assert(!is_empty_v<subtrie_info_t>);
    static_assert(is_typelist_v<subtrie_info_t>);

    // assert first child information has 3 components
    // TODO: this should really check that every child information has 3 components
    static_assert(size_v<get_t<subtrie_info_t, 0>> == 3);

    // helper functions to create children mapping
    template <size_t I>
    using subtrie_params_t = get_t<get_t<unzip_t<subtrie_info_t>, 0>, I>;
    
    template <size_t... I>
    static constexpr auto make_children(std::index_sequence<I...>)
    {
        using variant_t = std::variant<LexTrie<subtrie_params_t<I>, SymbolType>...>;
        return mapbox::eternal::map<char, variant_t>({
            {
                get_v<char_list_t, I>, 
                variant_t(
                        std::in_place_index<I>, 
                        LexTrie<subtrie_params_t<I>, SymbolType>(
                            // is accepting?
                            get_t<get_t<unzip_t<subtrie_info_t>, 1>, I>::value,
                            // possible symbol if accepting
                            get_t<get_t<unzip_t<subtrie_info_t>, 2>, I>::value
                        )
                )
            }...
        });
    }

    static constexpr auto make_children()
    {
        return make_children(std::make_index_sequence<size_v<char_list_t>>());
    }

    using children_map_t = std::decay_t<decltype(make_children())>;

    using symbol_t = typename get_t<get_t<unzip_t<subtrie_info_t>, 2>, 0>::value_type;


    ////////////////////////////////////////
    // Private member variables
    ////////////////////////////////////////

    bool is_root_;                // indicates whether current trie is the root
    bool is_accept_;              // indicates whether current trie (node) is accepting
    std::optional<symbol_t> symbol_;    // only meaningful if is_accept_ is true
                                        // optional since root of tree may not be given symbol

    children_map_t children_ = make_children(); // children mapping from char to variant
    typename children_map_t::value_type* active_child = nullptr;    // active child

public:

    LexTrie(bool is_accept, symbol_t symbol) 
        : is_root_(false), is_accept_(is_accept), symbol_(symbol)
    {}

    LexTrie() 
        : is_root_(true), is_accept_(false)
    {}

};

template <class SymbolType>
class LexTrie<leaf_tag, SymbolType>
{
public:
    using symbol_t = SymbolType;

    LexTrie(bool is_accept, symbol_t symbol)
        : is_accept_(is_accept), symbol_(symbol)
    {}

private:
    bool is_root_ = false;
    bool is_accept_ = true; 
    std::optional<symbol_t> symbol_; 
};

////////////////////////////////////////////////////////////
// make_trie 
//
// Compiler error is raised if the first type of the pair is not std::string_view.
////////////////////////////////////////////////////////////

//template <class PairArrayConstant>
//inline constexpr auto make_trie()
//{
//    using raw_pair_arr_t = std::decay_t<typename PairArrayConstant::value_type>;
//    using pair_t = typename raw_pair_arr_t::value_type;
//    using first_t = typename pair_t::first_type;
//
//    // check that first type in pair is std::string_view
//    static_assert(std::is_same_v<first_t, std::string_view>,
//            "First pair type must be std::string_view");
//
//    // check that array is non-empty
//    static_assert(!PairArrayConstant::value.empty());
//
//    using string_arr_t = unzip_t<PairArrayConstant>;
//
//    // check that all strings are unique
//    static_assert(is_unique_v<string_arr_t>);    
//}

} // namespace utils
} // namespace core
} // namespace docgen
