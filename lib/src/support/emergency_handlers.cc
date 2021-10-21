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

#include "srsran/support/emergency_handlers.h"
#include "srsran/support/srsran_assert.h"

namespace {

/// Holds the callback function pointer and the associated user provided data pointer.
struct handler_instance {
  std::atomic<void*>                      data{};
  std::atomic<emergency_cleanup_callback> callback{};
};

} // namespace

// Handlers are added in a thread safe manner without using locks to avoid possible issues if a signal is emitted while
// modifying the callback array.
static constexpr unsigned    max_handlers = 12;
static handler_instance      registered_handlers[max_handlers];
static std::atomic<unsigned> num_handlers;

void add_emergency_cleanup_handler(emergency_cleanup_callback callback, void* data)
{
  // Reserve a slot in the array.
  auto pos = num_handlers.fetch_add(1);

  // Check if we have space in the array.
  if (pos >= max_handlers) {
    srsran_assert(0, "Exceeded the emergency cleanup handler registered limit");
    return;
  }

  // Order is important here: write last the callback member as it is used to signal that the handler is valid when
  // reading the array.
  registered_handlers[pos].data.store(data);
  registered_handlers[pos].callback.store(callback);
}

void execute_emergency_cleanup_handlers()
{
  for (unsigned i = 0, e = num_handlers; i != e; ++i) {
    auto callback = registered_handlers[i].callback.load();
    // Test the validity of the callback as it may have not been written yet into the array even if num_callbacks has
    // been updated.
    if (callback) {
      callback(registered_handlers[i].data.load());
    }
  }
}
