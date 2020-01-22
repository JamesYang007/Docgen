#pragma once
#include <type_traits>
#include <utility>

// TODO: needed?
//#define ERR_NOT_VALUELIST \
//    "List must be of type valuelist."

#define ERR_EMPTY_VALUELIST \
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

//////////////////////////////////////////////////
// Non-member functions
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
// get_v
//////////////////////////////////////////////////

namespace details {

template <class List, size_t Idx>
inline constexpr auto get()
{
    static_assert(!is_empty_v<List>, ERR_EMPTY_VALUELIST);
    static_assert((Idx < size_v<List>), "Index must be less than the size of List.");
    if constexpr (Idx != 0) {
        return get<typename List::next_type, Idx - 1>();
    }
    else {
        return List::value;
    }
}
    
} // namespace details

template <class List, size_t Idx>
inline constexpr auto get_v = details::get<List, Idx>();

// Note: the following assumes List is our implementation of valuelist specifically

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
// subset_idx
//////////////////////////////////////////////////

namespace details {

template <class List, class Condition
        , size_t Idx = 0
        , size_t ListSize = size_v<List>
        , bool = (Idx < ListSize)>
struct subset_idx
{
private:
    using next_subset_t = typename subset_idx<
        typename List::next_type, Condition, Idx + 1, ListSize
    >::type;

public:
    using type = std::conditional_t<
        Condition::evaluate(List::value),
        concat_t<valuelist<size_t, Idx>, next_subset_t>,
        next_subset_t
    >;
};

// at the end of list
template <class List, class Condition
        , size_t Idx, size_t ListSize>
struct subset_idx<List, Condition, Idx, ListSize, false>
{
    using type = valuelist<size_t>;
};

} // namespace details

template <class List, class Condition>
using subset_idx_t = typename details::subset_idx<List, Condition>::type;

//////////////////////////////////////////////////
// subset
//////////////////////////////////////////////////

namespace details {

template <class List, class IdxList>
struct subset
{
private:
    template <size_t... I>
    static constexpr auto evaluate(std::index_sequence<I...>)
    {
        using value_type = typename List::value_type;
        return valuelist<value_type, get_v<List, get_v<IdxList, I>>...>();
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
// zip
//
// combine two valuelists into tuple of valuelists of 2 values
//////////////////////////////////////////////////

namespace details {

template <class List1, class List2>
struct zip
{
private:
    static_assert(size_v<List1> == size_v<List2>);
    static_assert(std::is_same_v<
            typename List1::value_type,
            typename List2::value_type
            >);

    template <size_t... I>
    static constexpr auto evaluate(std::index_sequence<I...>)
    {
        using value_type = typename List1::value_type;
        return std::tuple<valuelist<value_type, get_v<List1, I>, get_v<List2, I>>...>();
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

} // namespace utils
} // namespace core
} // namespace docgen
