#pragma once
#include <type_traits>
#include <utility>
#include <cstddef>

#define ERR_EMPTY_LIST \
    "List must be non-empty."

namespace docgen {
namespace core {
namespace utils {

// Tag used to indicate a class is considered an empty list
struct empty_tag
{};

// List of values of type T
template <class T, T... values>
struct valuelist;

template <class T, T _value, T... _values>
struct valuelist<T, _value, _values...>
{
    using next_type = valuelist<T, _values...>;
    using value_type = T;
    static constexpr value_type value = _value;
};

template <class T>
struct valuelist<T> : empty_tag
{
    using value_type = T;
};

// List of types
template <class... Ts>
struct typelist;

template <class T, class... Ts>
struct typelist<T, Ts...>
{
    using next_type = typelist<Ts...>;
    using type = T;
};

template <>
struct typelist<> : empty_tag
{};

//////////////////////////////////////////////////
// Non-member functions
//////////////////////////////////////////////////

//////////////////////////////////////////////////
// Intended for ANY list
//////////////////////////////////////////////////

//////////////////////////////////////////////////
// is_empty
//////////////////////////////////////////////////

namespace details {

template <class List, bool = std::is_convertible_v<List, empty_tag>>
struct is_empty : std::true_type
{};

template <class List>
struct is_empty<List, false> : std::false_type
{};

} // namespace details

template <class List>
inline constexpr bool is_empty_v = details::is_empty<List>::value;

//////////////////////////////////////////////////
// size_v
//////////////////////////////////////////////////

namespace details {

template <class List, bool = !is_empty_v<List>>
struct size
{
    static constexpr size_t value = 
        1 + size<typename List::next_type>::value;
};

// empty list
template <class List>
struct size<List, false>
{
    static constexpr size_t value = 0;
};

} // namespace details

template <class List>
inline constexpr size_t size_v = details::size<List>::value;

//////////////////////////////////////////////////
// Intended for ONLY valuelist
//////////////////////////////////////////////////

//////////////////////////////////////////////////
// is_valuelist
//////////////////////////////////////////////////

namespace details {

template <class List>
struct is_valuelist : std::false_type
{};

template <class T, T... values>
struct is_valuelist<valuelist<T, values...>> : std::true_type
{};

} // namespace details

template <class List>
inline constexpr bool is_valuelist_v = details::is_valuelist<List>::value;

//////////////////////////////////////////////////
// get_v
//////////////////////////////////////////////////

namespace details {

template <class List, size_t Idx>
inline constexpr auto get_value()
{
    static_assert(!is_empty_v<List>, ERR_EMPTY_LIST);
    static_assert((Idx < size_v<List>), "Index must be less than the size of List.");
    if constexpr (Idx != 0) {
        return get_value<typename List::next_type, Idx - 1>();
    }
    else {
        return List::value;
    }
}
    
} // namespace details

template <class List, size_t Idx>
inline constexpr auto get_v = details::get_value<List, Idx>();

//////////////////////////////////////////////////
// concat
//////////////////////////////////////////////////

namespace details {

template <class T, class U>
struct concat;

// concatenate two valuelists
template <class T, T... values_1, T... values_2>
struct concat<valuelist<T, values_1...>, valuelist<T, values_2...>>
{
    using type = valuelist<T, values_1..., values_2...>;
};

} // namespace details

template <class List1, class List2>
using concat_t = typename details::concat<List1, List2>::type;

//////////////////////////////////////////////////
// is_in
//////////////////////////////////////////////////

namespace details {

template <class List, typename List::value_type x>
struct is_in
{
private:
    static_assert(is_valuelist_v<List>);

    template <size_t... I>
    static constexpr bool evaluate(std::index_sequence<I...>)
    {
        return ((get_v<List, I> == x) || ...);
    }

    static constexpr bool evaluate()
    {
        return evaluate(std::make_index_sequence<size_v<List>>());
    }
public:
    static constexpr bool value = evaluate();
};

} // namespace details

template <class List, typename List::value_type x>
inline constexpr bool is_in_v = details::is_in<List, x>::value;

//////////////////////////////////////////////////
// set
//////////////////////////////////////////////////

namespace details {

template <class List, bool = !is_empty_v<List>>
struct set
{
private:
    static_assert(is_valuelist_v<List>);
    using next_set_t = typename set<typename List::next_type>::type;
    using value_type = typename List::value_type;

public:
    using type = std::conditional_t<
        is_in_v<next_set_t, List::value>,
        next_set_t,
        concat_t<valuelist<value_type, List::value>, next_set_t>
    >;
};

template <class List>
struct set<List, false>
{
    using type = valuelist<typename List::value_type>;
};

} // namespace details

template <class List>
using set_t = typename details::set<List>::type;

//////////////////////////////////////////////////
// left_shift_t
//
// Shifts all values to the left by one, where
// the first value will be discarded.
// If list is empty, the result is the same empty list.
//////////////////////////////////////////////////

namespace details {

template <class List, bool = !is_empty_v<List>>
struct left_shift
{
private:
    static_assert(is_valuelist_v<List>);
public:
    using type = typename List::next_type;
};

template <class List>
struct left_shift<List, false>
{
    using type = List;
};

} // namespace details

template <class List>
using left_shift_t = typename details::left_shift<List>::type;

//////////////////////////////////////////////////
// Intended for ONLY typelist
//////////////////////////////////////////////////

//////////////////////////////////////////////////
// is_typelist
//////////////////////////////////////////////////

namespace details {

template <class List>
struct is_typelist : std::false_type
{};

template <class... Ts>
struct is_typelist<typelist<Ts...>> : std::true_type
{};

} // namespace details

template <class List>
inline constexpr bool is_typelist_v = details::is_typelist<List>::value;


//////////////////////////////////////////////////
// get_t
//////////////////////////////////////////////////

namespace details {

template <class List, size_t Idx
        , bool = (Idx != 0)>
struct get_type
{
    static_assert((Idx < size_v<List>));
    using type = typename get_type<typename List::next_type, Idx-1>::type;
};

template <class List, size_t Idx>
struct get_type<List, Idx, false>
{
    using type = typename List::type;
};
    
} // namespace details

template <class List, size_t Idx>
using get_t = typename details::get_type<List, Idx>::type;

// Note: the following assumes List is our implementation of valuelist specifically

//////////////////////////////////////////////////
// zip
//
// Intended for two typelist<...> to form
// => typelist< typelist<first, second> ...>
//////////////////////////////////////////////////

namespace details {

template <class List1, class List2>
struct zip
{
private:
    static_assert(is_typelist_v<List1>);
    static_assert(is_typelist_v<List2>);
    static_assert(size_v<List1> == size_v<List2>);

    template <size_t... I>
    static constexpr auto evaluate(std::index_sequence<I...>)
    {
        return typelist<typelist<get_t<List1, I>, get_t<List2, I>>...>();
    }

    static constexpr auto evaluate()
    {
        return evaluate(std::make_index_sequence<size_v<List1>>());
    }

public:
    using type = std::decay_t<decltype(evaluate())>;
};

} // namespace details

template <class List1, class List2>
using zip_t = typename details::zip<List1, List2>::type;

//////////////////////////////////////////////////
// unzip
//
// Intended for typelist< typelist<T1, T2>, ...>
// => 
// typelist<T1,...> (first types only)
// or
// typelist<T2,...> (second types only)
// Every typelist must be of same size.
//////////////////////////////////////////////////

namespace details {

template <class List, bool = !is_empty_v<List>>
struct unzip
{
private:
    static_assert(is_typelist_v<List>);

    // get Jth type of the ith typelist in List
    template <size_t J, size_t... I>
    using column_j_t = typelist<get_t<get_t<List, I>, J>...>;

    template <size_t... I, size_t... J>
    static constexpr auto evaluate(
            std::index_sequence<I...>,
            std::index_sequence<J...>)
    {
        return typelist<column_j_t<J, I...>...>();
    }

    static constexpr auto evaluate()
    {
        return evaluate(
                // number of typelists
                std::make_index_sequence<size_v<List>>(),
                // number of columns
                std::make_index_sequence<size_v<get_t<List, 0>>>()
                );
    }

public:
    using type = std::decay_t<decltype(evaluate())>;
};

template <class List>
struct unzip<List, false>
{
private:
    static_assert(is_typelist_v<List>);
public:
    using type = List;
};

} // namespace details

template <class List>
using unzip_t = typename details::unzip<List>::type;

//////////////////////////////////////////////////
// transform
// 
// Transforms every type into Transformer<type>::type
// and creates a new typelist.
//////////////////////////////////////////////////

namespace details {

template <class List, template <class> class Transformer>
struct transform
{
private:
    static_assert(is_typelist_v<List>);

    template <size_t... I>
    static constexpr auto evaluate(std::index_sequence<I...>)
    {
        return typelist<typename Transformer<get_t<List, I>>::type... >();
    }

    static constexpr auto evaluate()
    {
        return evaluate(std::make_index_sequence<size_v<List>>());
    }

public:
    using type = std::decay_t<decltype(evaluate())>;
};

} // namespace details

template <class List, template <class> class Transformer>
using transform_t = typename details::transform<List, Transformer>::type;

//////////////////////////////////////////////////
// Intended for combination of typelist and valuelist
//////////////////////////////////////////////////

//////////////////////////////////////////////////
// subset_idx
//
// Intended for typelist
// TODO: may need for valuelist
// Returns a valuelist<size_t, ...> of indices where
// Condition<type>::value is true.
//////////////////////////////////////////////////

namespace details {

template <class List, template <class> class Condition
        , size_t Idx = 0
        , size_t ListSize = size_v<List>
        , bool = (Idx < ListSize)>
struct subset_idx
{
private:
    static_assert(is_typelist_v<List>);

    using next_subset_t = typename subset_idx<
        typename List::next_type, Condition, Idx + 1, ListSize
    >::type;

public:
    using type = std::conditional_t<
        Condition<typename List::type>::value,
        concat_t<valuelist<size_t, Idx>, next_subset_t>,
        next_subset_t
    >;
};

// at the end of list
template <class List, template <class> class Condition
        , size_t Idx, size_t ListSize>
struct subset_idx<List, Condition, Idx, ListSize, false>
{
    using type = valuelist<size_t>;
};

} // namespace details

template <class List, template <class> class Condition>
using subset_idx_t = typename details::subset_idx<List, Condition>::type;

//////////////////////////////////////////////////
// subset
//
// Intended for typelist
// TODO: may need for valuelist
// Returns a subset of original typelist with given valuelist<size_t,...>
// list of indices.
//////////////////////////////////////////////////

namespace details {

template <class List, class IdxList>
struct subset
{
private:
    static_assert(is_typelist_v<List>);

    template <size_t... I>
    static constexpr auto evaluate(std::index_sequence<I...>)
    {
        return typelist<get_t<List, get_v<IdxList, I>>...>();
    }

    static constexpr auto evaluate()
    {
        return evaluate(std::make_index_sequence<size_v<IdxList>>());
    }

public:
    using type = std::decay_t<decltype(evaluate())>;
};

} // namespace details

template <class List, class IdxList>
using subset_t = typename details::subset<List, IdxList>::type;

//////////////////////////////////////////////////
// get_first_value_t
//
// Intended for typelist<valuelist<...>, valuelist<...>,...>
// Extracts the first character from every list into a new valuelist.
// If a valuelist is empty, it is ignored.
// If typelist is empty, compiler error since it is impossible to deduce
// the type for the valuelist.
//////////////////////////////////////////////////

namespace details {

template <class List>
struct is_not_empty
{
    static constexpr bool value = !is_empty_v<List>;
};

template <class List>
struct get_first_value
{
private:
    static_assert(is_typelist_v<List>);

    using pruned_list_t = subset_t<List, subset_idx_t<List, is_not_empty>>;

    static_assert(!is_empty_v<pruned_list_t>, 
            "Typelist of valuelists must be non-empty and contain at least one non-empty valuelist.");

    template <size_t... I>
    static constexpr auto evaluate(std::index_sequence<I...>)
    {
        // every type in pruned_list_t must be a valuelist
        static_assert((is_valuelist_v<get_t<pruned_list_t, I>> && ...));

        using value_type = typename get_t<pruned_list_t, 0>::value_type;
        // every type (valuelist) must have same value type
        static_assert(
            (std::is_same_v<typename get_t<pruned_list_t, I>::value_type, value_type> && ...)
        );

        return valuelist<value_type, get_v<get_t<pruned_list_t, I>, 0>...>();
    }

    static constexpr auto evaluate()
    {
        return evaluate(std::make_index_sequence<size_v<pruned_list_t>>());
    }

public:
    using type = std::decay_t<decltype(evaluate())>;
};

} // namespace details

template <class List>
using get_first_value_t = typename details::get_first_value<List>::type;

} // namespace utils
} // namespace core
} // namespace docgen
