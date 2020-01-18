#pragma once
#include <unordered_map>
#include <string_view>
#include <type_traits>
#include <cassert>
#include <optional>
#include <exceptions/exceptions.hpp>

namespace docgen {
namespace core {
namespace lexer {

template <class SymbolType>
struct Trie
{
private:
    struct TrieNode; // forward declaration

public:
    using pair_t = std::pair<std::string_view, SymbolType>;

    // Constructs trie node from a list of pairs of string and symbol.
    // The string must be of type std::string_view and it must not be empty.
    // The symbol must be of type SymbolType.
    Trie(const std::initializer_list<pair_t>&);

    // Delete compiler-generated copy/move ctor/assignment
    // This ensures that Trie objects are only (default) constructible.
    Trie(const Trie&) =delete;
    Trie(Trie&&) =delete;
    Trie& operator=(const Trie&) =delete;
    Trie& operator=(Trie&&) =delete;

    void transition(char c);
    void back_transition();
    bool is_accept() const;
    std::unordered_map<char, TrieNode>& get_children();
    bool is_reset() const;
    void reset();
    const std::optional<SymbolType>& get_symbol() const;

private:

    struct TrieNode
    {
        // Insert str from current node to update the trie structure.
        // The string str is read starting from idx.
        void insert(const std::pair<std::string_view, SymbolType>&, size_t = 0);

        // Returns if current node is an accepting state.
        bool is_accept() const;

        // Returns the optional symbol associated with current node.
        // Symbol will be active if is_accept is true.
        const std::optional<SymbolType>& get_symbol() const;

        std::unordered_map<char, TrieNode>& get_children();

        std::optional<std::reference_wrapper<TrieNode>> get_parent();

    private:

        enum class State : bool {
            accept,
            non_accept
        };

        State state_ = State::non_accept;               // indicates accepting node or not
        std::optional<SymbolType> symbol_;              // symbol for accepting node
        std::unordered_map<char, TrieNode> children_;   // current node's children
        TrieNode* parent_ptr_;                          // current node's parent
    };

    TrieNode root_;                                       // root of Trie
    std::reference_wrapper<TrieNode> curr_node_ = root_;  // current node
};

////////////////////////////////////////////////////////////////
// TrieNode Implementation
////////////////////////////////////////////////////////////////

template <class SymbolType>
inline void 
Trie<SymbolType>::TrieNode::insert(const pair_t& pair, size_t idx)
{
    const auto& str = std::get<0>(pair);

    // if string starting from idx is empty, then accepting state
    if (str[idx] == '\0') {
        state_ = State::accept;
        symbol_ = std::get<1>(pair);
    }

    else {
        auto& child = children_[str[idx]];
        child.parent_ptr_ = this;
        child.insert(pair, idx + 1);
    }
}

template <class SymbolType>
inline bool 
Trie<SymbolType>::TrieNode::is_accept() const
{
    return state_ == State::accept;
}

template <class SymbolType>
inline const std::optional<SymbolType>&
Trie<SymbolType>::TrieNode::get_symbol() const
{
    return symbol_;
}

template <class SymbolType>
inline std::unordered_map<char, typename Trie<SymbolType>::TrieNode>&
Trie<SymbolType>::TrieNode::get_children()
{
    return children_;
}

template <class SymbolType>
inline std::optional<std::reference_wrapper<typename Trie<SymbolType>::TrieNode>>
Trie<SymbolType>::TrieNode::get_parent()
{
    if (parent_ptr_) {
        return *parent_ptr_;
    }
    return {};
}

////////////////////////////////////////////////////////////////
// Trie Implementation
////////////////////////////////////////////////////////////////

template <class SymbolType>
inline 
Trie<SymbolType>::Trie(const std::initializer_list<pair_t>& pairs)
    : root_()
{
    for (auto it = pairs.begin(); it != pairs.end(); ++it) {
        if (it->first.empty()) {
            throw exceptions::control_flow_error("strings must be non-empty");
        }
        root_.insert(*it);
    }
}

template <class SymbolType>
inline void
Trie<SymbolType>::transition(char c)
{
    curr_node_ = curr_node_.get().get_children().at(c);
}

template <class SymbolType>
inline bool
Trie<SymbolType>::is_accept() const
{
    return curr_node_.get().is_accept();
}

template <class SymbolType>
inline std::unordered_map<char, typename Trie<SymbolType>::TrieNode>&
Trie<SymbolType>::get_children() 
{
    return curr_node_.get().get_children();
}

template <class SymbolType>
inline bool
Trie<SymbolType>::is_reset() const
{
    return &(curr_node_.get()) == &root_;
}

template <class SymbolType>
inline void
Trie<SymbolType>::reset() 
{
    curr_node_ = root_;
}

template <class SymbolType>
inline void
Trie<SymbolType>::back_transition() 
{
    auto&& opt_parent = curr_node_.get().get_parent();
    if (!opt_parent) {
        throw exceptions::control_flow_error("Attempt to back transition past the root");
    }
    curr_node_ = *opt_parent;
}

template <class SymbolType>
inline const std::optional<SymbolType>& 
Trie<SymbolType>::get_symbol() const
{
    return curr_node_.get().get_symbol();
}

} // namespace lexer
} // namespace core
} // namespace docgen
