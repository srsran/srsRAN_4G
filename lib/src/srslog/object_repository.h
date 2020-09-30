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

#ifndef SRSLOG_OBJECT_REPOSITORY_H
#define SRSLOG_OBJECT_REPOSITORY_H

#include "srslte/srslog/detail/support/thread_utils.h"
#include <unordered_map>
#include <vector>

namespace srslog {

/// This template class implements a very basic object repository with arbitrary
/// key and value types. It allows registering new objects of type V indexed by
/// key K, no element removal supported.
/// NOTE: Thread safe class.
template <typename K, typename V>
class object_repository
{
  mutable detail::mutex m;
  std::unordered_map<K, V> repo;

public:
  /// Inserts a new entry into the repository. A pointer to the new
  /// entry is returned or nullptr when the key already exists.
  V* insert(const K& key, V&& value)
  {
    detail::scoped_lock lock(m);
    const auto& insertion = repo.emplace(key, std::move(value));
    if (!insertion.second)
      return nullptr;
    return &insertion.first->second;
  }

  /// Inserts a new entry in-place into the repository if there is no element
  /// with the specified key. Returns a reference to the inserted element or to
  /// the already existing element if no insertion happened.
  template <typename... Args>
  V& emplace(Args&&... args)
  {
    detail::scoped_lock lock(m);
    auto insertion = repo.emplace(std::forward<Args>(args)...);
    return insertion.first->second;
  }

  /// Finds a value with the specified key in the repository. Returns a pointer
  /// to the value, otherwise nullptr if not found.
  V* find(const K& key)
  {
    detail::scoped_lock lock(m);
    auto it = repo.find(key);
    return (it != repo.end()) ? &it->second : nullptr;
  }
  const V* find(const K& key) const
  {
    detail::scoped_lock lock(m);
    const auto it = repo.find(key);
    return (it != repo.cend()) ? &it->second : nullptr;
  }

  /// Returns a copy of the contents of the repository.
  std::vector<V*> contents()
  {
    detail::scoped_lock lock(m);

    std::vector<V*> data;
    data.reserve(repo.size());
    for (auto& elem : repo) {
      data.push_back(&elem.second);
    }

    return data;
  }
  std::vector<const V*> contents() const
  {
    detail::scoped_lock lock(m);

    std::vector<const V*> data;
    data.reserve(repo.size());
    for (const auto& elem : repo) {
      data.push_back(&elem.second);
    }

    return data;
  }
};

} // namespace srslog

#endif // SRSLOG_OBJECT_REPOSITORY_H
