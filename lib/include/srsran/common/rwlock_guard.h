// SPDX-FileCopyrightText: Copyright (C) 2013-2026 Software Radio Systems Limited
// SPDX-License-Identifier: LicenseRef-SRS-Custom

#ifndef SRSRAN_RWLOCK_GUARD_H
#define SRSRAN_RWLOCK_GUARD_H

#include <pthread.h>

namespace srsran {

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

} // namespace srsran

#endif // SRSRAN_RWLOCK_GUARD_H
