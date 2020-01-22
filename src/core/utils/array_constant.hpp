#pragma once
#include <core/utils/type_traits.hpp>
#include <variant>

namespace docgen {
namespace core {
namespace utils {

////////////////////////////////////////////////////////////
// array_constant 
////////////////////////////////////////////////////////////

template <class ArrayType, const ArrayType& x>
struct array_constant
{
    static_assert(is_array_v<ArrayType>);
    static constexpr ArrayType value = x;
};

////////////////////////////////////////////////////////////
// make_array_constant_t
////////////////////////////////////////////////////////////

template <const auto& arr>
using make_array_constant_t = array_constant<
    std::decay_t<decltype(arr)>, arr
    >;

////////////////////////////////////////////////////////////
// is_array_constant
////////////////////////////////////////////////////////////

namespace details {

template <class ArrayConstant>
struct is_array_constant : std::false_type
{};

template <class ArrayType, const ArrayType& x>
struct is_array_constant<array_constant<ArrayType, x>> : std::true_type
{};

} // namespace details

template <class ArrayConstant>
inline constexpr bool is_array_constant_v = 
    details::is_array_constant<ArrayConstant>::value;

////////////////////////////////////////////////////////////
// get_element_t
//
// retrieves an array_constant element at index Idx as 
// std::integral_constant<const T&, x> where T is the underlying value type
// of the array and x is a copy of the Idx'th element.
////////////////////////////////////////////////////////////

namespace details {

template <class ArrayConstant, size_t Idx>
struct get_element
{
private:
    static_assert(is_array_constant_v<ArrayConstant>);
    static constexpr auto value = ArrayConstant::value[Idx];
public:
    using type = make_constant_t<value>;
};

} // namespace details

template <class ArrayConstant, size_t Idx>
using get_element_t = typename details::get_element<ArrayConstant, Idx>::type;

////////////////////////////////////////////////////////////
// unpack 
////////////////////////////////////////////////////////////

template <class... Ts>
struct unpacked
{};

namespace details {

template <class ArrayConstant>
struct unpack
{
private:
    static_assert(is_array_constant_v<ArrayConstant>);

    template <size_t... I>
    static constexpr auto evaluate(std::index_sequence<I...>)
    {
        return unpacked<get_element_t<ArrayConstant, I>...>();
    }

    static constexpr auto evaluate()
    {
        return evaluate(std::make_index_sequence<ArrayConstant::value.size()>());
    }

public:
    using type = std::decay_t<decltype(evaluate())>;
};

} // namespace details

template <class ArrayConstant>
using unpack_t = typename details::unpack<ArrayConstant>::type;

////////////////////////////////////////////////////////////
// concat 
////////////////////////////////////////////////////////////

namespace details {

template <class T, class U>
struct concat;

// Specialization: two array constants
template <class ArrayType1, const ArrayType1& arr1
        , class ArrayType2, const ArrayType2& arr2>
struct concat<array_constant<ArrayType1, arr1>, array_constant<ArrayType2, arr2>>
{
private: 
    template <size_t... I, size_t... J>
    static constexpr auto evaluate(std::index_sequence<I...>, std::index_sequence<J...>) 
    {
        using value_t = std::common_type_t<typename ArrayType1::value_type, typename ArrayType2::value_type>;
        if constexpr (sizeof...(I) == 0 && sizeof...(J) == 0) {
            return std::array<value_t, 0>({});
        }
        else {
            return utils::make_array<value_t>({arr1[I]..., arr2[J]...});
        }
    }
    static constexpr auto evaluate()
    {
        return evaluate(std::make_index_sequence<arr1.size()>(),
                       std::make_index_sequence<arr2.size()>());
    }

    static constexpr auto value = evaluate();

public:
    using type = make_array_constant_t<value>;
};

// Specialization: array constant, element constant
template <class ArrayType, const ArrayType& arr
        , const typename ArrayType::value_type& x>
struct concat<array_constant<ArrayType, arr>, 
              constant<typename ArrayType::value_type, x>>
{
private:
    using arr1_t = array_constant<ArrayType, arr>;
    using value_t = typename ArrayType::value_type;
    static constexpr auto arr2 = utils::make_array<value_t>({x});
    using arr2_t = make_array_constant_t<arr2>;
public:
    using type = typename concat<arr1_t, arr2_t>::type;
};

// Specialization: element constant, array constant
template <class ArrayType, const ArrayType& arr
        , const typename ArrayType::value_type& x>
struct concat<constant<typename ArrayType::value_type, x>,
              array_constant<ArrayType, arr>>
{
private:
    using value_t = typename ArrayType::value_type;
    static constexpr auto arr1 = utils::make_array<value_t>({x});
    using arr1_t = make_array_constant_t<arr1>;
    using arr2_t = array_constant<ArrayType, arr>;
public:
    using type = typename concat<arr1_t, arr2_t>::type;
};

} // namespace details

template <class T, class U>
using concat_t = typename details::concat<T, U>::type;

////////////////////////////////////////////////////////////
// subset_idx
////////////////////////////////////////////////////////////

namespace details {

// Specialization: I < N
template <class ArrayConstant, class Condition
        , size_t I = 0, bool = (I < ArrayConstant::value.size())>
struct subset_idx
{
private:
    static_assert(is_array_constant_v<ArrayConstant>);
    using next_subset_t = typename subset_idx<ArrayConstant, Condition, I+1>::type;
    static constexpr size_t current_idx = I;

public:
    using type = std::conditional_t<
        Condition::evaluate(get_element_t<ArrayConstant, I>::value),
        concat_t<make_constant_t<current_idx>, next_subset_t>,
        next_subset_t
            >;
};

// Specialization: I == N
template <class ArrayConstant, class Condition, size_t I>
struct subset_idx<ArrayConstant, Condition, I, false>
{
private:
    static_assert(is_array_constant_v<ArrayConstant>);

    using raw_arr_t = std::array<size_t, 0>;
    static constexpr raw_arr_t value = {};

public:
    using type = make_array_constant_t<value>;
}; 

} // namespace details

// Given array of objects, return array of indices 
// where Condition::evaluate returns true. 
// Note that Condition::evaluate must be static constexpr.
template <class ArrayConstant, class Condition>
using subset_idx_t = typename details::subset_idx<ArrayConstant, Condition>::type;

////////////////////////////////////////////////////////////
// subset
////////////////////////////////////////////////////////////

namespace details {

template <class ArrayConstant, class IdxArrayConstant>
struct subset
{
private:
    static_assert(is_array_constant_v<ArrayConstant>);
    static_assert(is_array_constant_v<IdxArrayConstant>);

    template <size_t... I>
    static constexpr auto evaluate(std::index_sequence<I...>)
    {
        using value_t = typename std::decay_t<typename ArrayConstant::value_type>::value_type;
        if constexpr (sizeof... (I) == 0) {
            return std::array<value_t, 0>({});
        }
        else {
            return utils::make_array<value_t>({ArrayConstant::value[IdxArrayConstant::value[I]]...});
        }
    }

    static constexpr auto evaluate()
    {
        return evaluate(std::make_index_sequence<IdxArrayConstant::value.size()>());
    }

    static constexpr auto value = evaluate();

public:
    using type = make_array_constant_t<value>;
};

} // namespace details

template <class ArrayConstant, class IdxArrayConstant>
using subset_t = typename details::subset<ArrayConstant, IdxArrayConstant>::type;

////////////////////////////////////////////////////////////
// zip 
////////////////////////////////////////////////////////////

namespace details {

template <class FirstArrayConstant, class SecondArrayConstant>
struct zip
{
private:
    static_assert(is_array_constant_v<FirstArrayConstant>);
    static_assert(is_array_constant_v<SecondArrayConstant>);
    static_assert((FirstArrayConstant::value.size() == SecondArrayConstant::value.size()));

    template <size_t... I>
    static constexpr auto evaluate(std::index_sequence<I...>)
    {
        using first_raw_arr_t = std::decay_t<typename FirstArrayConstant::value_type>;
        using second_raw_arr_t = std::decay_t<typename SecondArrayConstant::value_type>;
        using first_type = typename first_raw_arr_t::value_type;
        using second_type = typename second_raw_arr_t::value_type;
        using pair_t = std::pair<first_type, second_type>;

        if constexpr (sizeof... (I) == 0) {
            return std::array<pair_t, 0>({});
        }
        else {
            return utils::make_array<pair_t>({{FirstArrayConstant::value[I], SecondArrayConstant::value[I]}...});
        }
    }

    static constexpr auto evaluate()
    {
        return evaluate(std::make_index_sequence<FirstArrayConstant::value.size()>());
    }

    static constexpr auto value = evaluate();

public:
    using type = make_array_constant_t<value>;
};

} // namespace details

template <class FirstArrayConstant, class SecondArrayConstant>
using zip_t = typename details::zip<FirstArrayConstant, SecondArrayConstant>::type;

////////////////////////////////////////////////////////////
// unzip 
////////////////////////////////////////////////////////////

namespace details {

template <class PairArrayConstant, size_t Idx>
struct unzip
{
private:
    static_assert(is_array_constant_v<PairArrayConstant>);

    template <size_t... I>
    static constexpr auto evaluate(std::index_sequence<I...>)
    {
        using first_type = std::common_type_t<
            std::decay_t<decltype(std::get<Idx>(PairArrayConstant::value[I]))>...
            >;
        if constexpr (sizeof... (I) == 0) {
            return std::array<first_type, 0>({});
        }
        else {
            return utils::make_array<first_type>({std::get<Idx>(PairArrayConstant::value[I])...});
        }
    }

    static constexpr auto evaluate()
    {
        return evaluate(std::make_index_sequence<PairArrayConstant::value.size()>());
    }

    static constexpr auto value = evaluate();

public:
    using type = make_array_constant_t<value>;
};

} // namespace details

template <class PairArrayConstant, size_t Idx = 0>
using unzip_t = typename details::unzip<PairArrayConstant, Idx>::type;

////////////////////////////////////////////////////////////
// is_in 
////////////////////////////////////////////////////////////

namespace details {

template <class ElementConstant, class ArrayConstant>
struct is_in
{
private:    
    static_assert(is_constant_v<ElementConstant>);
    static_assert(is_array_constant_v<ArrayConstant>);

    template <size_t... I>
    static constexpr bool evaluate(std::index_sequence<I...>)
    {
        return ((ElementConstant::value == ArrayConstant::value[I]) || ...);
    }

    static constexpr bool evaluate()
    {
        return evaluate(std::make_index_sequence<ArrayConstant::value.size()>());
    }

public:
    static constexpr bool value = evaluate();
};

} // namespace details

template <class ElementConstant, class ArrayConstant>
inline constexpr bool is_in_v = 
    details::is_in<ElementConstant, ArrayConstant>::value;

////////////////////////////////////////////////////////////
// set 
////////////////////////////////////////////////////////////

namespace details {

template <class ArrayConstant, size_t I
        , bool = (I < ArrayConstant::value.size())>
struct set
{
private:
    static_assert(is_array_constant_v<ArrayConstant>);

    using current_element_constant_t = get_element_t<ArrayConstant, I>;
    using next_set_t = typename set<ArrayConstant, I+1>::type;

public:
    using type = std::conditional_t<
        is_in_v<current_element_constant_t, next_set_t>,
        next_set_t,
        concat_t<current_element_constant_t, next_set_t>
            >;
};

template <class ArrayConstant, size_t I>
struct set<ArrayConstant, I, false>
{
private:
    static_assert(is_array_constant_v<ArrayConstant>);
    using value_t = typename std::decay_t<typename ArrayConstant::value_type>::value_type;
    static constexpr std::array<value_t, 0> value = {};
public:
    using type = make_array_constant_t<value>;
};

} // namespace details

template <class ArrayConstant, size_t I = 0>
using set_t = typename details::set<ArrayConstant, I>::type;

////////////////////////////////////////////////////////////
// is_unique 
////////////////////////////////////////////////////////////

template <class ArrayConstant>
inline constexpr bool is_unique_v = 
    set_t<ArrayConstant>::value.size() == ArrayConstant::value.size();

////////////////////////////////////////////////////////////
// for_each 
//
// TODO: remove!
////////////////////////////////////////////////////////////

namespace details {

template <class RangeArrayConstant
        , template <class ElementConstant> class Functor>
struct for_each
{
private:
    static_assert(is_array_constant_v<RangeArrayConstant>);

    template <size_t Idx>
    using get_functor_t = Functor<get_element_t<RangeArrayConstant, Idx>>;

    template <size_t Idx>
    using functor_return_t = std::decay_t<
        decltype(get_functor_t<Idx>::evaluate())
        >;

    template <size_t... I>
    static constexpr auto evaluate(std::index_sequence<I...>)
    {
        using variant_t = std::variant<functor_return_t<I>...>;
        if constexpr (sizeof... (I) == 0) {
            return std::array<variant_t, 0>({});
        }
        else {
            return utils::make_array<variant_t>({
                    variant_t(std::in_place_index<I>, get_functor_t<I>::evaluate())...
                    });
        }
    }

    static constexpr auto evaluate()
    {
        return evaluate(std::make_index_sequence<RangeArrayConstant::value.size()>());
    }

    static constexpr auto value = evaluate();

public:
    using type = make_array_constant_t<value>;
};

} // namespace details

template <class RangeArrayConstant
        , template <class ElementConstant> class Functor>
using for_each_t = typename details::for_each<RangeArrayConstant, Functor>::type;

} // namespace utils
} // namespace core
} // namespace docgen
