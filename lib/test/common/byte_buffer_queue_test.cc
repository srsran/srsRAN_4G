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

void* write_thread(void* a)
{
  args_t* args = (args_t*)a;
  for (uint32_t i = 0; i < NMSGS; i++) {
    unique_byte_buffer_t b = srsran::make_byte_buffer();
    if (b == nullptr) {
      return nullptr;
    }
    memcpy(b->msg, &i, 4);
    b->N_bytes = 4;
    args->q->write(std::move(b));
  }
  return NULL;
}

int main(int argc, char** argv)
{
  bool                 result;
  byte_buffer_queue    q;
  unique_byte_buffer_t b;
  pthread_t            thread;
  args_t               args;
  u_int32_t            r;

  result = true;
  args.q = &q;

  pthread_create(&thread, NULL, &write_thread, &args);

  for (uint32_t i = 0; i < NMSGS; i++) {
    b = q.read();
    memcpy(&r, b->msg, 4);
    if (r != i)
      result = false;
  }

  pthread_join(thread, NULL);

  if (q.size() != 0 || q.size_bytes() != 0) {
    result = false;
  }

  if (result) {
    printf("Passed\n");
    exit(0);
  } else {
    printf("Failed\n;");
    exit(1);
  }
}
