/*
 * Copyright 2013-2020 Software Radio Systems Limited
 *
 * This file is part of srsLTE.
 *
 * srsLTE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsLTE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#ifndef SRSLOG_DETAIL_SUPPORT_ANY_H
#define SRSLOG_DETAIL_SUPPORT_ANY_H

#include <memory>
#include <type_traits>

namespace srslog {

namespace detail {

/// Tag for in place construction of the any class.
template <typename T>
struct in_place_type_t {
  explicit in_place_type_t() = default;
};

/// Type trait to detect if T is a in_place_type_t tag.
template <typename T>
struct is_in_place_type_t : std::false_type {};
template <typename T>
struct is_in_place_type_t<in_place_type_t<T>> : std::true_type {};

/// This is a very minimalist and non compliant implementation of std::any which
/// is included in C++17.
/// From the standard: "The class any describes a type-safe container for single
/// values of any type".
class any
{
public:
  //:TODO: Clang 3.8 does not compile when default constructing a const object
  // due to DR253. Declare the defaulted constructor out of the class.
  any();

  /// Disallow copies for simplicity.
  any(const any& other) = delete;
  any& operator=(const any& other) = delete;

  /// Constructs an object of type decayed T with the provided argument.
  /// This constructor only participates in overload resolution when the decayed
  /// T meets all of the following conditions:
  ///   a) is not of class any.
  ///   b) is move constructible.
  ///   c) is not an specialization of in_place_type_t.
  /// Otherwise the rest of special member functions are considered.
  template <
      typename T,
      typename std::enable_if<
          !std::is_same<typename std::decay<T>::type, any>{} &&
              std::is_move_constructible<typename std::decay<T>::type>{} &&
              !is_in_place_type_t<typename std::decay<T>::type>{},
          int>::type = 0>
  explicit any(T&& t) :
    storage(new storage_impl<typename std::decay<T>::type>(std::forward<T>(t)))
  {}

  /// Constructs an object of type decayed T directly into the internal storage
  /// forwarding the provided arguments.
  /// This constructor only participates in overload resolution when the decayed
  /// T meets all of the following conditions:
  ///   a) T(args...) is constructible.
  /// Otherwise the rest of special member functions are considered.
  template <typename T,
            typename... Args,
            typename std::enable_if<
                std::is_constructible<typename std::decay<T>::type, Args...>{},
                int>::type = 0>
  explicit any(in_place_type_t<T>, Args&&... args) :
    storage(new storage_impl<typename std::decay<T>::type>(
        std::forward<Args>(args)...))
  {}

  any(any&& other) : storage(std::move(other.storage)) {}

  any& operator=(any&& other)
  {
    storage = std::move(other.storage);
    return *this;
  }

  /// Checks whether the object contains a value.
  bool has_value() const { return (storage != nullptr); }

  /// If not empty, destroys the contained object.
  void reset() { storage.reset(); }

  /// Swaps the content of two any objects.
  void swap(any& other)
  {
    using std::swap;
    swap(storage, other.storage);
  }

private:
  template <typename T>
  friend T* any_cast(any* operand);
  template <typename T>
  friend const T* any_cast(const any* operand);

  /// Type erased interface for type identification purposes.
  struct type_interface {
    virtual ~type_interface() = default;

    virtual const void* type() const = 0;
  };

  /// Concrete type implementation with data storage.
  template <typename T>
  struct storage_impl : public type_interface {
    template <typename... Args>
    explicit storage_impl(Args&&... args) : data(std::forward<Args>(args)...)
    {}

    storage_impl(const storage_impl& other) = delete;
    storage_impl& operator=(const storage_impl& other) = delete;

    const void* type() const override { return &type_tag<T>::tag; }

    T data;
  };

  /// Discriminant tag for type identification.
  template <typename T>
  struct type_tag {
    static const char tag;
  };

private:
  std::unique_ptr<type_interface> storage;
};

//:TODO: declared out of line, see TODO above.
inline any::any() = default;

/// Constructs an any object containing an object of type T, passing the
/// provided arguments to T's constructor.
template <typename T, typename... Args>
inline any make_any(Args&&... args)
{
  return any(in_place_type_t<T>{}, std::forward<Args>(args)...);
}

/// When the requested T matches that of the contents of operand, a pointer to
/// the value contained by operand is returned, otherwise returns a nullptr.
/// Non-const overload.
template <typename T>
inline T* any_cast(any* operand)
{
  if (!operand || !operand->storage)
    return nullptr;

  using U =
      typename std::remove_cv<typename std::remove_reference<T>::type>::type;
  if (operand->storage->type() != &any::type_tag<U>::tag)
    return nullptr;

  return &static_cast<any::storage_impl<U>*>(operand->storage.get())->data;
}

/// When the requested T matches that of the contents of operand, a pointer to
/// the value contained by operand is returned, otherwise returns a nullptr.
/// Const overload.
template <typename T>
inline const T* any_cast(const any* operand)
{
  if (!operand || !operand->storage)
    return nullptr;

  using U =
      typename std::remove_cv<typename std::remove_reference<T>::type>::type;
  if (operand->storage->type() != &any::type_tag<U>::tag)
    return nullptr;

  return &static_cast<any::storage_impl<U>*>(operand->storage.get())->data;
}

inline void swap(any& lhs, any& rhs)
{
  lhs.swap(rhs);
}

template <typename T>
const char any::type_tag<T>::tag = 0;

} // namespace detail

} // namespace srslog

#endif // SRSLOG_DETAIL_SUPPORT_ANY_H
