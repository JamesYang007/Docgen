#pragma once
#include <cstddef>
#include <type_traits>
#include <utility>
#include <array>

namespace docgen {
namespace core {
namespace utils {

template <class KeyType, class ValueType
        , size_t Size, bool IsSorted = false>
struct map;

// Define only when input will be sorted
template <class KeyType, class ValueType
        , size_t Size>
struct map<KeyType, ValueType, Size, true>
{
    using key_type = KeyType;
    using value_type = ValueType;
    using pair_type = std::pair<KeyType, ValueType>;

    constexpr map(const pair_type(&data)[Size]) noexcept
        : map(data, std::make_index_sequence<Size>())
    {}

    constexpr const pair_type* find(const KeyType& key) const
    {
        constexpr auto compute_mid = [](size_t begin, size_t end)
        {
            return begin + (end - begin) / 2;
        };

        size_t begin = 0;
        size_t end = mapping_.size();
        size_t mid = compute_mid(begin, end);
        while (begin < end) {
            if (key == mapping_[mid].first) {
                return &mapping_[mid];
            }
            else if (key < mapping_[mid].first) {
                end = mid;
                mid = compute_mid(begin, end);
            }
            else {
                begin = mid + 1;
                mid = compute_mid(begin, end);
            }
        }

        return this->end();
    }

    pair_type* find(const KeyType& key)
    {
        return const_cast<pair_type*>(static_cast<const map&>(*this).find(key));
    }

    pair_type* begin() 
    {
        return (&mapping_[0]);
    }

    constexpr const pair_type* begin() const
    {
        return (&mapping_[0]);
    }

    pair_type* end() 
    {
        return (&mapping_[0]) + mapping_.size();
    }

    constexpr const pair_type* end() const
    {
        return (&mapping_[0]) + mapping_.size();
    }

private:

    template <size_t... I>
    constexpr map(const pair_type(&data)[Size], 
                  std::index_sequence<I...>) noexcept
        : mapping_{data[I] ...}
    {}  
    
    std::array<pair_type, Size> mapping_;
};

template <class KeyType, class ValueType, bool IsSorted, size_t N>
inline constexpr auto make_map(const std::pair<KeyType, ValueType>(&data)[N])
{
    return map<KeyType, ValueType, N, IsSorted>(data); 
}

} // namespace utils
} // namespace core
} // namespace docgen
