#pragma once
#include <optional>
#include <variant>
#include <core/utils/trie_params.hpp>
#include <core/utils/map.hpp>

namespace docgen {
namespace core {
namespace utils {

////////////////////////////////////////////////////////////
// LexTrie 
//
// TrieParams is expected to be a result of calling trie_params_t.
////////////////////////////////////////////////////////////

namespace details {

// Helper for LexTrie to determine the symbol type given trie_params type.
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
struct LexTrie
{
    using symbol_t = SymbolType;

    LexTrie(bool is_accept, symbol_t symbol) 
        : is_root_(false), is_accept_(is_accept), symbol_(symbol)
    {}

    LexTrie() 
        : is_root_(true), is_accept_(false)
    {}

    // Transition from current most active child with char.
    // On transition, call functor of type OnTransition.
    // If transition was successful, return true.
    // Otherwise, no changes are made and returns false.
    template <class OnTransition>
    bool transition(char, OnTransition);

    // Back transition from most active child.
    // By default, back transition once.
    size_t back_transition(size_t = 1);
    
    // Returns true if and only if current active child is accepting.
    bool is_accept() const;

    // Returns true if root trie child is inactive.
    bool is_reset() const;

    // Sets all active children as inactive.
    void reset();

    const std::optional<symbol_t>& get_symbol() const;

private:

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
        using sorted_char_list_idx_t = sort_idx_t<char_list_t>;
        // note that sorted_char_list_idx_t is used to construct the map
        // such that the keys are already in sorted order
        // This optimizes construction of the map at run-time and
        // allows us to remove the need to sort it during construction.
        return make_map<char, variant_t, true>({
            {
                get_v<char_list_t, get_v<sorted_char_list_idx_t, I>>, 
                variant_t(
                        std::in_place_index<get_v<sorted_char_list_idx_t, I>>, 
                        // is accepting?
                        get_t<get_t<unzip_t<subtrie_info_t>, 1>, get_v<sorted_char_list_idx_t, I>>::value,
                        // possible symbol if accepting
                        get_t<get_t<unzip_t<subtrie_info_t>, 2>, get_v<sorted_char_list_idx_t, I>>::value
                )
            }...
        });
    }

    static constexpr auto make_children()
    {
        return make_children(std::make_index_sequence<size_v<char_list_t>>());
    }

    using children_map_t = std::decay_t<decltype(make_children())>;

    using variant_t = typename children_map_t::value_type;

    ////////////////////////////////////////
    // Private member variables
    ////////////////////////////////////////

    const bool is_root_;                      // indicates whether current trie is the root
    const bool is_accept_;                    // indicates whether current trie (node) is accepting
    const std::optional<symbol_t> symbol_;    // only meaningful if is_accept_ is true
                                              // optional since root of tree may not be given symbol

    children_map_t children_ = make_children(); // children mapping from char to variant
    variant_t* active_child_ = nullptr;    // active child
};

// Specialization: leaf
template <class SymbolType>
class LexTrie<leaf_tag, SymbolType>
{
public:
    using symbol_t = SymbolType;

    LexTrie(bool is_accept, symbol_t symbol)
        : is_accept_(is_accept), symbol_(symbol)
    {}

    // This function will always return false since it never contains any children.
    template <class OnTransition>
    bool transition(char, OnTransition);

    // Simply returns 0 as receipt.
    size_t back_transition(size_t = 1);

    bool is_accept() const;

    // Returns true since it is vacuously true.
    bool is_reset() const;

    void reset();
    
    const std::optional<symbol_t>& get_symbol() const;

private:

    bool is_root_ = false;
    bool is_accept_ = true; 
    std::optional<symbol_t> symbol_; 
};

////////////////////////////////////////////////////////////
// LexTrie Implementation (Primary)
////////////////////////////////////////////////////////////

template <class TrieParams, class SymbolType>
template <class OnTransition>
inline bool 
LexTrie<TrieParams, SymbolType>::transition(char c, OnTransition transfunc)
{
    // delegate if a child is active
    if (active_child_) {
        return std::visit(
                [=](auto&& arg) {
                    return arg.transition(c, transfunc);
                },
                *active_child_
                );
    }

    auto it = children_.find(c);
    if (it != children_.end()) {
        active_child_ = &(it->second);
        transfunc();
        return true;
    }

    return false;
}

template <class TrieParams, class SymbolType>
inline size_t LexTrie<TrieParams, SymbolType>::back_transition(size_t num)
{
    if (active_child_) {
        // back_num is the number of times back transitioned after calling from here.
        // If it is the same as num, simply return back_num as a receipt.
        // Otherwise, deactivate the child to nullptr.
        size_t back_num = std::visit(
                [=](auto&& arg) {
                    return arg.back_transition(num);
                },
                *active_child_
                );
        if (back_num == num) {
            return back_num;
        }
        active_child_ = nullptr;
        return back_num + 1;
    }

    // if not active, the base case is that we return 0
    return 0;
}

template <class TrieParams, class SymbolType>
inline bool LexTrie<TrieParams, SymbolType>::is_accept() const
{
    if (active_child_) {
        return std::visit(
                [](auto&& arg) -> bool {
                    return arg.is_accept();
                },
                *active_child_
                ); 
    }
    return is_accept_;
}

template <class TrieParams, class SymbolType>
inline bool LexTrie<TrieParams, SymbolType>::is_reset() const
{
    return active_child_ == nullptr;
}
    
template <class TrieParams, class SymbolType>
inline void LexTrie<TrieParams, SymbolType>::reset()
{
    if (active_child_) {
        std::visit(
                [](auto&& arg) {
                    arg.reset();
                },
                *active_child_
                );
        active_child_ = nullptr;
    }
}

template <class TrieParams, class SymbolType>
inline const std::optional<typename LexTrie<TrieParams, SymbolType>::symbol_t>& 
LexTrie<TrieParams, SymbolType>::get_symbol() const
{
    using opt_symb_ref_t = 
        const std::optional<typename LexTrie<TrieParams, SymbolType>::symbol_t>&;
    if (active_child_) {
        return std::visit(
                [](auto&& arg) -> opt_symb_ref_t {
                    return arg.get_symbol();
                },
                *active_child_
                );
    }
    return symbol_;
}

////////////////////////////////////////////////////////////
// LexTrie Implementation (Specialization)
////////////////////////////////////////////////////////////

template <class SymbolType>
template <class OnTransition>
inline bool 
LexTrie<leaf_tag, SymbolType>::transition(char, OnTransition)
{
    return false;
}

template <class SymbolType>
inline size_t
LexTrie<leaf_tag, SymbolType>::back_transition(size_t)
{
    return 0;
}

template <class SymbolType>
inline bool LexTrie<leaf_tag, SymbolType>::is_accept() const
{
    return is_accept_;
}

template <class SymbolType>
inline bool LexTrie<leaf_tag, SymbolType>::is_reset() const
{
    return true;
}

template <class SymbolType>
inline void LexTrie<leaf_tag, SymbolType>::reset()
{}

template <class SymbolType>
inline const std::optional<typename LexTrie<leaf_tag, SymbolType>::symbol_t>& 
LexTrie<leaf_tag, SymbolType>::get_symbol() const
{
    return symbol_;
}

} // namespace utils
} // namespace core
} // namespace docgen
