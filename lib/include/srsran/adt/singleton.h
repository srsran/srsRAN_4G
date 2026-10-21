// SPDX-FileCopyrightText: Copyright (C) 2013-2026 Software Radio Systems Limited
// SPDX-License-Identifier: LicenseRef-SRS-Custom

#ifndef SRSRAN_SINGLETON_H
#define SRSRAN_SINGLETON_H

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

#endif // SRSRAN_SINGLETON_H
