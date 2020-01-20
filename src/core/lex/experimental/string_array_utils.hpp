#pragma once
#include <array>
#include <type_traits>
#include <utility>
#include <string_view>

namespace docgen {
namespace core {
namespace lex {
namespace experimental {

////////////////////////////////////////////////////////////
// concat 
////////////////////////////////////////////////////////////

namespace details {

// Take a variadic number of objects and return
// an array of their copies. The underlying data type is the
// commont type among all objects.
template <class... Ts>
inline constexpr auto
concat_many(const Ts&... xs)
{
    using common_t = std::common_type_t<Ts...>;
    return std::array<common_t, sizeof...(Ts)>{{xs...}};
}

template <class T, size_t N, size_t M, size_t... I, size_t... J>
inline constexpr auto
concat(const std::array<T, N>& arr1, const std::array<T, M>& arr2, 
       std::index_sequence<I...>, std::index_sequence<J...>)
{
    return concat_many(arr1[I]..., arr2[J]...);
}

} // namespace details

// Given T object x and an array of T objects arr,
// concatenate to form a new array where the first object is a copy
// of x and the rest of the elements is a copy of arr.
template <class T, size_t N, size_t M>
inline constexpr auto
concat(const std::array<T, N>& arr1, const std::array<T, M>& arr2) 
{
    return details::concat(arr1, arr2, 
            std::make_index_sequence<N>(),
            std::make_index_sequence<M>());
}

template <class T, size_t N>
inline constexpr auto
concat(const T& x, const std::array<T, N>& arr) 
{
    return concat(std::array<T, 1>{{x}}, arr);
}

template <class T, size_t N>
inline constexpr auto
concat(const std::array<T, N>& arr, const T& x) 
{
    return concat(arr, std::array<T, 1>{{x}});
}

////////////////////////////////////////////////////////////
// subset_idx
////////////////////////////////////////////////////////////

namespace details {

// Overload for when I < N: concatenate current index (if condition holds)
// to the result of calling subset on the rest of the array from I+1
template <class Condition, const auto& arr, size_t I=0
        , bool = (I < arr.size())>
struct subset_idx
{
    static constexpr auto execute()
    {
        auto next_subset = details::subset_idx<Condition, arr, I+1>::execute();
        if constexpr (Condition::check(arr[I])) {
            return experimental::concat(I, next_subset); 
        }
        else {
            return next_subset;
        }
    }
};

// Overload for when I == N: return empty array of size_t
template <class Condition, const auto& arr, size_t I>
struct subset_idx<Condition, arr, I, false>
{
    static constexpr auto execute() 
    {
        return std::array<size_t, 0>();
    }
}; 

} // namespace details

// Given array of objects, return array of indices 
// where Condition::check returns true. 
// Note that Condition::check must be static constexpr.
template <class Condition, const auto& arr>
inline constexpr auto subset_idx()
{
    return details::subset_idx<Condition, arr>::execute();
}

////////////////////////////////////////////////////////////
// subset
////////////////////////////////////////////////////////////

namespace details {

// I... is an enumeration of indices up to arr_idx.size()
// Compiler error if this condition is not met.
template <class T1, size_t N1, size_t N2, size_t... I>
inline constexpr auto subset(
        const std::array<T1, N1>& arr_obj, 
        const std::array<size_t, N2>& arr_idx,
        std::index_sequence<I...>)
{
    return std::array<T1, N2>{{arr_obj[arr_idx[I]]...}};
}

} // namespace details

// Given array of objects and array of indices 
// return a subset with those indices. 
template <class T1, size_t N1, size_t N2>
inline constexpr auto subset(
        const std::array<T1, N1>& arr_obj, 
        const std::array<size_t, N2>& arr_idx)
{
    return details::subset(arr_obj, arr_idx,
            std::make_index_sequence<N2>());
}

////////////////////////////////////////////////////////////
// shrink_array
////////////////////////////////////////////////////////////

// Shrinks array to new array of pairs where the first element (string_view)
// is always non-empty.
template <class SecondType, size_t NumPairs>
inline constexpr auto
shrink_array(const std::array<std::pair<std::string_view, SecondType>, NumPairs>& arr)
{
    static_cast<void>(arr);
}

// Variadic number of pairs to array of pairs
template <class... FirstTypes, class... SecondTypes>
inline constexpr auto
make_pair_array(const std::pair<FirstTypes, SecondTypes>&... pairs)
{
    using first_t = std::common_type_t<FirstTypes...>;
    using second_t = std::common_type_t<SecondTypes...>;
    using pair_t = std::pair<first_t, second_t>;
    constexpr size_t num_pairs = sizeof... (FirstTypes);
    return std::array<pair_t, num_pairs>{{pairs...}};
}

template <class... Pairs>
inline constexpr auto make_trie(const Pairs&... pairs)
{
    auto pair_arr = make_pair_array(pairs...);
    return pair_arr;
}

} // namespace experimental
} // namespace lex
} // namespace core
} // namespace docgen
