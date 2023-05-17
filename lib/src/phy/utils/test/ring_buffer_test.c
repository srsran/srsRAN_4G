/**
 * Copyright 2013-2023 Software Radio Systems Limited
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

#include "srsran/support/srsran_test.h"
#include <complex.h>
#include <math.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <time.h>
#include <unistd.h>

#include "srsran/phy/utils/ringbuffer.h"
#include "srsran/phy/utils/vector.h"

struct thread_args_t {
  int                  len;
  uint8_t*             in;
  uint8_t*             out;
  srsran_ringbuffer_t* buf;
  int                  res;
};

int N = 200;
int M = 10;

void usage(char* prog)
{
  printf("Usage: %s\n", prog);
  printf("\t-N size of blocks in  [Default 200]\n");
  printf("\t-M Number of blocks  [Default 10]\n");
}

void parse_args(int argc, char** argv)
{
  int opt;
  while ((opt = getopt(argc, argv, "Nd")) != -1) {
    switch (opt) {
      case 'N':
        N = (int)strtol(argv[optind], NULL, 10);
        break;
      case 'M':
        M = (int)strtol(argv[optind], NULL, 10);
        break;
      default:
        usage(argv[0]);
        exit(-1);
    }
  }
}

int test_normal_read_write(srsran_ringbuffer_t* q, uint8_t* in, uint8_t* out, int len)
{
  srsran_ringbuffer_write(q, in, len / 2);
  srsran_ringbuffer_write(q, &in[len / 2], len / 2);

  for (int i = 0; i < 4; i++) {
    srsran_ringbuffer_read(q, &out[(len / 4) * i], len / 4);
  }

  TESTASSERT(!memcmp(in, out, len));
  return 0;
}

int test_overflow_write(srsran_ringbuffer_t* q, uint8_t* in, uint8_t* out, int len)
{
  int ret = srsran_ringbuffer_write(q, in, len / 2);
  ret     = srsran_ringbuffer_write(q, &in[len / 2], len / 2 + 2);
  if (ret != (len / 2 + 2)) {
    ret = -1;
  }
  return ret;
}

void* write_thread(void* args_)
{
  int                   res  = 0;
  struct thread_args_t* args = (struct thread_args_t*)args_;
  for (int i = 0; i < M; i++) {
    res = srsran_ringbuffer_write_block(args->buf, args->in, args->len);
  }
  if (res < 0) {
    args->res = res;
  }
  return NULL;
}

void* read_thread(void* args_)
{
  int                   res  = 0;
  struct thread_args_t* args = (struct thread_args_t*)args_;
  for (int i = 0; i < M; i++) {
    res = srsran_ringbuffer_read(args->buf, &args->out[args->len * i], args->len);
  }
  if (res < 0) {
    args->res = res;
  }
  return NULL;
}

int threaded_blocking_test(struct thread_args_t* args)
{

  pthread_t threads[2];
  if (pthread_create(&threads[0], NULL, write_thread, args)) {
    fprintf(stderr, "Error creating thread\n");
    return SRSRAN_ERROR;
  }
  usleep(10000);
  if (pthread_create(&threads[1], NULL, read_thread, args)) {
    fprintf(stderr, "Error creating thread\n");
    return SRSRAN_ERROR;
  }

  for (int i = 0; i < 2; i++) {
    if (pthread_join(threads[i], NULL)) {
      fprintf(stderr, "Error joining thread\n");
      return SRSRAN_ERROR;
    }
  }

  for (int i = 0; i < M; i++) {
    uint8_t* out_ptr = &args->out[N * i];
    TESTASSERT(!memcmp(args->in, out_ptr, N));
  }

  if (args->res < 0) {
    return SRSRAN_ERROR;
  }

  return SRSRAN_SUCCESS;
}

int main(int argc, char** argv)
{
  int ret = SRSRAN_SUCCESS;
  parse_args(argc, argv);
  struct thread_args_t thread_in;

  uint8_t*            in  = srsran_vec_u8_malloc(N * 2);
  uint8_t*            out = srsran_vec_u8_malloc(N * 10);
  srsran_ringbuffer_t ring_buf;
  srsran_ringbuffer_init(&ring_buf, N);

  thread_in.in  = in;
  thread_in.out = out;
  thread_in.buf = &ring_buf;
  thread_in.len = N;
  thread_in.res = ret;

  for (int i = 0; i < N * 2; i++) {
    in[i] = i % 255;
  }

  if (test_normal_read_write(&ring_buf, in, out, N) < 0) {
    printf("Normal read write test failed\n");
    ret = SRSRAN_ERROR;
  }
  bzero(out, N * 10);
  srsran_ringbuffer_reset(&ring_buf);

  if (test_overflow_write(&ring_buf, in, out, N) != -1) {
    printf("Overflow detection not working correctly\n");
    ret = SRSRAN_ERROR;
  }
  bzero(out, N * 10);
  srsran_ringbuffer_reset(&ring_buf);

  if (threaded_blocking_test((void*)&thread_in)) {
    printf("Error in multithreaded blocking ringbuffer test\n");
    ret = SRSRAN_ERROR;
  }
  srsran_ringbuffer_stop(&ring_buf);
  srsran_ringbuffer_free(&ring_buf);
  free(in);
  free(out);
  printf("Done\n");
  return ret;
}
