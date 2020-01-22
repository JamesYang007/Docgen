#pragma once
#include <type_traits>
#include <array>

namespace docgen {
namespace core {
namespace utils {

////////////////////////////////////////////////////////////
// make_array 
////////////////////////////////////////////////////////////

namespace details {

template <class T, size_t N, size_t... I>
inline constexpr auto make_array(
       const T (&arr)[N],
       std::index_sequence<I...>)
{
   return std::array<T, N>({arr[I]...});
}

} // namespace details

template <class T, size_t N>
inline constexpr auto make_array(const T (&arr)[N])
{
   return details::make_array(arr, std::make_index_sequence<N>());
}

////////////////////////////////////////////////////////////
// make_pair_array 
////////////////////////////////////////////////////////////

namespace details {

template <class FirstType, class SecondType, size_t N, size_t... I>
inline constexpr auto make_pair_array(
        const std::pair<FirstType, SecondType> (&list)[N],
        std::index_sequence<I...>
        )
{
    using pair_t = std::pair<FirstType, SecondType>;
    return std::array<pair_t, N>{{list[I]...}};
}

} // namespace details

// Variadic number of pairs to array of pairs
template <class FirstType, class SecondType, size_t N>
inline constexpr auto make_pair_array(
        const std::pair<FirstType, SecondType> (&list)[N]
        )
{
    return details::make_pair_array(list, std::make_index_sequence<N>());
}
    
////////////////////////////////////////////////////////////
// is_array 
////////////////////////////////////////////////////////////

namespace details {
 
template <class T>
struct is_array : std::false_type
{};

template <class T, size_t N>
struct is_array<std::array<T, N>> : std::true_type
{};

} // namespace details

template <class T>
inline constexpr bool is_array_v = details::is_array<T>::value;

////////////////////////////////////////////////////////////
// make_constant_t
////////////////////////////////////////////////////////////

template <auto element>
using make_constant_t = std::integral_constant<
    std::decay_t<decltype(element)>,
    element
    >;

////////////////////////////////////////////////////////////
// is_constant 
////////////////////////////////////////////////////////////

namespace details {
 
template <class T>
struct is_constant 
    : std::false_type
{};

template <class T, T x>
struct is_constant<std::integral_constant<T, x>> 
    : std::true_type
{};

} // namespace details

template <class T>
inline constexpr bool is_constant_v = details::is_constant<T>::value;

} // namespace utils
} // namespace core
} // namespace docgen
