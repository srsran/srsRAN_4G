/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSRAN_TYPE_STORAGE_H
#define SRSRAN_TYPE_STORAGE_H

#include <type_traits>
#include <utility>

namespace srsran {

template <typename T>
struct type_storage {
  template <typename... Args>
  void construct(Args&&... args)
  {
    new (&buffer) T(std::forward<Args>(args)...);
  }
  void destroy() { get().~T(); }
  void copy_ctor(const type_storage<T>& other) { buffer.get() = other.get(); }
  void move_ctor(type_storage<T>&& other) { buffer.get() = std::move(other.get()); }
  void copy_assign(const type_storage<T>& other)
  {
    if (this == &other) {
      return;
    }
    get() = other.get();
  }
  void move_assign(type_storage<T>&& other) { get() = std::move(other.get()); }

  T&       get() { return reinterpret_cast<T&>(buffer); }
  const T& get() const { return reinterpret_cast<const T&>(buffer); }

  typename std::aligned_storage<sizeof(T), alignof(T)>::type buffer;
};

template <typename T>
void copy_if_present_helper(type_storage<T>& lhs, const type_storage<T>& rhs, bool lhs_present, bool rhs_present)
{
  if (lhs_present and rhs_present) {
    lhs.get() = rhs.get();
  }
  if (lhs_present) {
    lhs.destroy();
  }
  if (rhs_present) {
    lhs.template construct(rhs.get());
  }
}

template <typename T>
void move_if_present_helper(type_storage<T>& lhs, type_storage<T>& rhs, bool lhs_present, bool rhs_present)
{
  if (lhs_present and rhs_present) {
    lhs.move_assign(std::move(rhs));
  }
  if (lhs_present) {
    lhs.destroy();
  }
  if (rhs_present) {
    lhs.template construct(std::move(rhs.get()));
  }
}

} // namespace srsran

#endif // SRSRAN_TYPE_STORAGE_H
