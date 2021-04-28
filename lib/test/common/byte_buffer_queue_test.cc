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

#define NMSGS 1000000

#include "srsran/common/buffer_pool.h"
#include "srsran/upper/byte_buffer_queue.h"
#include <stdio.h>

using namespace srsran;

typedef struct {
  byte_buffer_queue* q;
} args_t;

void write_thread(byte_buffer_queue* q)
{
  unique_byte_buffer_t b;
  for (uint32_t i = 0; i < NMSGS; i++) {
    do {
      b = srsran::make_byte_buffer();
      if (b == nullptr) {
        // wait until pool is not depleted
        std::this_thread::yield();
      }
    } while (b == nullptr);
    memcpy(b->msg, &i, 4);
    b->N_bytes = 4;
    q->write(std::move(b));
  }
}

int test_concurrent_writeread()
{
  byte_buffer_queue    q;
  unique_byte_buffer_t b;
  int                  result = 0;

  std::thread t([&q]() { write_thread(&q); });

  for (uint32_t i = 0; i < NMSGS; i++) {
    b          = q.read();
    uint32_t r = 0;
    memcpy(&r, b->msg, 4);
    if (r != i) {
      result = -1;
      break;
    }
  }

  t.join();

  if (q.size() != 0 || q.size_bytes() != 0) {
    result = -1;
  }

  if (result == 0) {
    printf("Passed\n");
  } else {
    printf("Failed\n;");
  }
  return result;
}

int main()
{
  return test_concurrent_writeread();
}
