/**
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#ifndef SRSRAN_INTRUSIVE_LIST_H
#define SRSRAN_INTRUSIVE_LIST_H

#include <iterator>
#include <type_traits>

namespace srsran {

struct default_intrusive_tag;

/// Base class of T, where T is a node of intrusive_forward_list<T>
template <typename Tag = default_intrusive_tag>
struct intrusive_forward_list_element {
  intrusive_forward_list_element<Tag>* next_node = nullptr;
};

/**
 * Forward linked list of pointers of type "T" that doesn't rely on allocations.
 * It leverages each node's internal pointer (thus intrusive) to store the next node of the list.
 * It supports push_front/pop_front, iteration, clear, etc.
 * @tparam T node type. It must be a subclass of intrusive_forward_list_element<Tag>
 * @tparam Tag useful to differentiate multiple intrusive lists in the same node
 */
template <typename T, typename Tag = default_intrusive_tag>
class intrusive_forward_list
{
  using node_t = intrusive_forward_list_element<Tag>;

  template <typename U>
  class iterator_impl
  {
    using elem_t = typename std::conditional<std::is_const<U>::value, const node_t, node_t>::type;

  public:
    using iterator_category = std::forward_iterator_tag;
    using value_type        = U;
    using difference_type   = std::ptrdiff_t;
    using pointer           = U*;
    using reference         = U&;

    explicit iterator_impl(elem_t* node_ = nullptr) : node(node_) {}
    iterator_impl<U>& operator++()
    {
      node = node->next_node;
      return *this;
    }
    pointer   operator->() { return static_cast<pointer>(node); }
    reference operator*() { return static_cast<reference>(*node); }

    bool operator==(const iterator_impl<U>& other) const { return node == other.node; }
    bool operator!=(const iterator_impl<U>& other) const { return node != other.node; }

  private:
    elem_t* node;
  };

public:
  using iterator       = iterator_impl<T>;
  using const_iterator = iterator_impl<const T>;

  intrusive_forward_list()
  {
    static_assert(std::is_base_of<node_t, T>::value,
                  "Provided template argument T must have intrusive_forward_list_element<Tag> as base class");
  }
  intrusive_forward_list(const intrusive_forward_list&) = default;
  intrusive_forward_list(intrusive_forward_list&& other) noexcept : node(other.node) { other.node = nullptr; }
  intrusive_forward_list& operator=(const intrusive_forward_list&) = default;
  intrusive_forward_list& operator                                 =(intrusive_forward_list&& other) noexcept
  {
    node       = other.node;
    other.node = nullptr;
    return *this;
  }

  T& front() const { return *static_cast<T*>(node); }

  void push_front(T* t)
  {
    node_t* new_head    = static_cast<node_t*>(t);
    new_head->next_node = node;
    node                = new_head;
  }
  T* pop_front()
  {
    node_t* ret = node;
    node        = node->next_node;
    return static_cast<T*>(ret);
  }
  void clear()
  {
    while (node != nullptr) {
      node_t* torem    = node;
      node             = node->next_node;
      torem->next_node = nullptr;
    }
  }

  bool empty() const { return node == nullptr; }

  iterator       begin() { return iterator(node); }
  iterator       end() { return iterator(nullptr); }
  const_iterator begin() const { return const_iterator(node); }
  const_iterator end() const { return const_iterator(nullptr); }

private:
  node_t* node = nullptr;
};

template <typename Tag = default_intrusive_tag>
struct intrusive_double_linked_list_element {
  intrusive_double_linked_list_element<Tag>* next_node = nullptr;
  intrusive_double_linked_list_element<Tag>* prev_node = nullptr;
};

/**
 * Double Linked List of pointers of type "T" that doesn't rely on allocations.
 * Instead, it leverages T's internal pointers to store the next and previous nodes
 * @tparam T node type. Must be a subclass of intrusive_double_linked_list_element<Tag>
 * @tparam Tag tag of nodes. Useful to differentiate separate intrusive lists inside the same T node
 */
template <typename T, typename Tag = default_intrusive_tag>
class intrusive_double_linked_list
{
  using node_t = intrusive_double_linked_list_element<Tag>;

  template <typename U>
  class iterator_impl
  {
    using elem_t = typename std::conditional<std::is_const<U>::value, const node_t, node_t>::type;

  public:
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type        = U;
    using difference_type   = std::ptrdiff_t;
    using pointer           = U*;
    using reference         = U&;

    explicit iterator_impl(elem_t* node_ = nullptr) : node(node_) {}
    iterator_impl<U>& operator++()
    {
      node = node->next_node;
      return *this;
    }
    iterator_impl<U>& operator--()
    {
      node = node->prev_node;
      return *this;
    }
    pointer   operator->() { return static_cast<pointer>(node); }
    reference operator*() { return static_cast<reference>(*node); }

    bool operator==(const iterator_impl<U>& other) const { return node == other.node; }
    bool operator!=(const iterator_impl<U>& other) const { return node != other.node; }

  private:
    elem_t* node;
  };

public:
  using iterator       = iterator_impl<T>;
  using const_iterator = iterator_impl<const T>;

  intrusive_double_linked_list()
  {
    static_assert(std::is_base_of<node_t, T>::value,
                  "Provided template argument T must have intrusive_forward_list_element<Tag> as base class");
  }
  intrusive_double_linked_list(const intrusive_double_linked_list&) = default;
  intrusive_double_linked_list(intrusive_double_linked_list&& other) noexcept : node(other.node)
  {
    other.node = nullptr;
  }
  intrusive_double_linked_list& operator=(const intrusive_double_linked_list&) = default;
  intrusive_double_linked_list& operator=(intrusive_double_linked_list&& other) noexcept
  {
    node       = other.node;
    other.node = nullptr;
    return *this;
  }
  ~intrusive_double_linked_list() { clear(); }

  T& front() const { return *static_cast<T*>(node); }

  void push_front(T* t)
  {
    node_t* new_head    = static_cast<node_t*>(t);
    new_head->prev_node = nullptr;
    new_head->next_node = node;
    if (node != nullptr) {
      node->prev_node = new_head;
    }
    node = new_head;
  }
  void pop(T* t)
  {
    node_t* to_rem = static_cast<node_t*>(t);
    if (to_rem == node) {
      node = to_rem->next_node;
    }
    if (to_rem->prev_node != nullptr) {
      to_rem->prev_node->next_node = to_rem->next_node;
    }
    if (to_rem->next_node != nullptr) {
      to_rem->next_node->prev_node = to_rem->prev_node;
    }
    to_rem->next_node = nullptr;
    to_rem->prev_node = nullptr;
  }
  void pop_front() { pop(static_cast<T*>(node)); }
  void clear()
  {
    while (node != nullptr) {
      node_t* torem    = node;
      node             = node->next_node;
      torem->next_node = nullptr;
      torem->prev_node = nullptr;
    }
  }

  bool empty() const { return node == nullptr; }

  iterator       begin() { return iterator(node); }
  iterator       end() { return iterator(nullptr); }
  const_iterator begin() const { return const_iterator(node); }
  const_iterator end() const { return const_iterator(nullptr); }

private:
  node_t* node = nullptr;
};

} // namespace srsran

#endif // SRSRAN_INTRUSIVE_LIST_H
