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

#ifndef SRSLTE_RWLOCK_GUARD_H
#define SRSLTE_RWLOCK_GUARD_H

#include <pthread.h>

namespace srslte {

class rwlock_write_guard
{
public:
  rwlock_write_guard(pthread_rwlock_t& rwlock_) : rwlock(&rwlock_) { pthread_rwlock_wrlock(rwlock); }
  rwlock_write_guard(const rwlock_write_guard&) = delete;
  rwlock_write_guard(rwlock_write_guard&&)      = delete;
  rwlock_write_guard& operator=(const rwlock_write_guard&) = delete;
  rwlock_write_guard& operator=(rwlock_write_guard&&) = delete;
  ~rwlock_write_guard() { pthread_rwlock_unlock(rwlock); }

private:
  pthread_rwlock_t* rwlock;
};

// Shared lock guard that automatically unlocks rwlock on exit
class rwlock_read_guard
{
public:
  rwlock_read_guard(pthread_rwlock_t& rwlock_) : rwlock(&rwlock_) { pthread_rwlock_rdlock(rwlock); }
  rwlock_read_guard(const rwlock_read_guard&) = delete;
  rwlock_read_guard(rwlock_read_guard&&)      = delete;
  rwlock_read_guard& operator=(const rwlock_read_guard&) = delete;
  rwlock_read_guard& operator=(rwlock_read_guard&&) = delete;
  ~rwlock_read_guard() { pthread_rwlock_unlock(rwlock); }

private:
  pthread_rwlock_t* rwlock;
};

} // namespace srslte

#endif // SRSLTE_RWLOCK_GUARD_H
