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

#ifndef SRSLTE_SINGLETON_H
#define SRSLTE_SINGLETON_H

#include <memory>

// CRTP pattern to create class singletons
template <typename T>
class singleton_t
{
public:
  singleton_t(const singleton_t<T>&) = delete;
  singleton_t(singleton_t<T>&&)      = delete;
  singleton_t& operator=(const singleton_t<T>&) = delete;
  singleton_t& operator=(singleton_t<T>&&) = delete;

  static T* get_instance()
  {
    // this variable initialization is thread-safe since C++11
    // instance dtor is called on program exit
    static std::unique_ptr<T> instance{new T_instance{}};
    return instance.get();
  }

protected:
  singleton_t() = default;

private:
  // this class makes the protected T::T() accessible to the singleton
  struct T_instance : public T {
    T_instance() : T() {}
  };
};

#endif // SRSLTE_SINGLETON_H
