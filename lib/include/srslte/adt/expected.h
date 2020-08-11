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

#ifndef SRSLTE_EXPECTED_H
#define SRSLTE_EXPECTED_H

#include "adt_utils.h"
#include <memory>
#include <system_error>

namespace srslte {

struct default_error_t {};

template <typename T, typename E = default_error_t>
class expected
{
  static_assert(not std::is_same<T, E>::value, "Expected and unexpected types cannot be of the same type");

public:
  expected() : has_val(true), val(T{}) {}
  expected(T&& t) : has_val(true), val(std::forward<T>(t)) {}
  expected(E&& e) : has_val(false), unexpected(std::forward<E>(e)) {}
  expected(const expected& other)
  {
    if (other.has_val) {
      construct_val(other.val);
    } else {
      construct_error(other.unexpected);
    }
  }
  expected(expected&& other) noexcept
  {
    if (other.has_val) {
      construct_val(std::move(other.val));
    } else {
      construct_error(std::move(other.unexpected));
    }
  }
  expected& operator=(const expected& other)
  {
    if (this != &other) {
      expected(other).swap(*this);
    }
    return *this;
  }
  expected& operator=(expected&& other) noexcept
  {
    expected(std::move(other)).swap(*this);
    return *this;
  }
  expected& operator=(const T& other) noexcept
  {
    if (not has_value()) {
      unexpected.~E();
      has_val = true;
    }
    val = other;
    return *this;
  }
  expected& operator=(T&& other) noexcept
  {
    if (not has_value()) {
      unexpected.~E();
      has_val = true;
    }
    val = std::move(other);
    return *this;
  }
  ~expected() { destroy(); }

  void set_error()
  {
    if (has_value()) {
      val.~T();
      construct_error(E{});
    } else {
      unexpected = E{};
    }
  }
  template <typename U>
  void set_error(U&& other)
  {
    if (has_value()) {
      val.~T();
      construct_error(std::forward<U>(other));
    } else {
      unexpected = std::forward<U>(other);
    }
  }

  explicit operator bool() const { return has_value(); }
  bool     has_value() const { return has_val; }
  bool     is_error() const { return not has_value(); }
  const T& value() const
  {
    if (not has_val) {
      THROW_BAD_ACCESS("Bad expected value access");
    }
    return val;
  }
  T& value()
  {
    if (not has_val) {
      THROW_BAD_ACCESS("Bad expected value access");
    }
    return val;
  }
  const E& error() const
  {
    if (has_val) {
      THROW_BAD_ACCESS("Bad expected error access");
    }
    return unexpected;
  }
  E& error()
  {
    if (has_val) {
      THROW_BAD_ACCESS("Bad expected error access");
    }
    return unexpected;
  }

  void swap(expected& other) noexcept
  {
    using std::swap;

    if (has_value() and other.has_value()) {
      swap(val, other.val);
    } else if (not has_value() and not other.has_value()) {
      swap(unexpected, other.unexpected);
    } else if (has_value() and not other.has_value()) {
      E err(std::move(other.unexpected));
      other.unexpected.~E();
      other.construct_val(std::move(val));
      val.~T();
      construct_error(std::move(err));
    } else if (!bool(*this) && bool(other)) {
      other.swap(*this);
    }
  }

private:
  void construct_val(const T& v) noexcept
  {
    has_val = true;
    new (&val) T(v);
  }
  void construct_val(T&& v) noexcept
  {
    has_val = true;
    new (&val) T(std::move(v));
  }
  void construct_error(const E& e) noexcept
  {
    has_val = false;
    new (&unexpected) E(e);
  }
  void construct_error(E&& e) noexcept
  {
    has_val = false;
    new (&unexpected) E(std::move(e));
  }
  void destroy()
  {
    if (has_value()) {
      val.~T();
    } else {
      unexpected.~E();
    }
  }

  bool has_val = false;
  union {
    T val;
    E unexpected;
  };
};

template <typename E>
using error_type = expected<std::true_type, E>;

} // namespace srslte

#endif // SRSLTE_EXPECTED_H
