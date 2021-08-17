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

#ifndef SRSRAN_OPTIONAL_H
#define SRSRAN_OPTIONAL_H

#include "detail/type_storage.h"
#include "srsran/common/srsran_assert.h"

namespace srsran {

template <typename T>
class optional
{
public:
  using value_type = T;

  optional() : has_val_(false) {}
  optional(const T& t) : has_val_(true) { storage.emplace(t); }
  optional(T&& t) : has_val_(true) { storage.emplace(std::move(t)); }
  optional(const optional<T>& other) : has_val_(other.has_value())
  {
    if (other.has_value()) {
      storage.copy_ctor(other.storage);
    }
  }
  optional(optional<T>&& other) noexcept : has_val_(other.has_value())
  {
    if (other.has_value()) {
      storage.move_ctor(std::move(other.storage));
    }
  }
  optional& operator=(const optional<T>& other)
  {
    if (this == &other) {
      return *this;
    }
    copy_if_present_helper(storage, other.storage, has_value(), other.has_value());
    has_val_ = other.has_value();
    return *this;
  }
  optional& operator=(optional<T>&& other) noexcept
  {
    move_if_present_helper(storage, other.storage, has_value(), other.has_value());
    has_val_ = other.has_value();
    return *this;
  }
  ~optional()
  {
    if (has_value()) {
      storage.destroy();
    }
  }

  bool     has_value() const { return has_val_; }
  explicit operator bool() const { return has_value(); }

  T*       operator->() { return &value(); }
  const T* operator->() const { return &value(); }
  T&       operator*() { return value(); }
  const T& operator*() const { return value(); }
  T&       value()
  {
    srsran_assert(has_val_, "Invalid optional<T> access");
    return storage.get();
  }
  const T& value() const
  {
    srsran_assert(has_val_, "Invalid optional<T> access");
    return storage.get();
  }

  template <typename... Args>
  void emplace(Args&&... args)
  {
    if (has_value()) {
      storage.destroy();
    }
    storage.emplace(std::forward<Args>(args)...);
    has_val_ = true;
  }

  void reset()
  {
    if (has_value()) {
      storage.destroy();
      has_val_ = false;
    }
  }

private:
  bool                    has_val_;
  detail::type_storage<T> storage;
};

template <typename T>
bool operator==(const optional<T>& lhs, const optional<T>& rhs)
{
  return lhs.has_value() == rhs.has_value() and (not lhs.has_value() or lhs.value() == rhs.value());
}

template <typename T>
bool operator==(const optional<T>& lhs, const T& rhs)
{
  return lhs.has_value() and lhs.value() == rhs;
}

template <typename T>
bool operator!=(const optional<T>& lhs, const optional<T>& rhs)
{
  return not(lhs == rhs);
}

template <typename T>
bool operator!=(const optional<T>& lhs, const T& rhs)
{
  return not(lhs == rhs);
}

template <typename T>
bool operator<(const optional<T>& lhs, const optional<T>& rhs)
{
  return rhs.has_value() and ((lhs.has_value() and lhs.value() < rhs.value()) or (not lhs.has_value()));
}

} // namespace srsran

#endif // SRSRAN_OPTIONAL_H
