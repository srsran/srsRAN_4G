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
