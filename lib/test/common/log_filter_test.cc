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

#define NTHREADS 100
#define NMSGS 100

#include "srslte/common/log_filter.h"
#include "srslte/common/logger_srslog_wrapper.h"
#include "srslte/common/logmap.h"
#include "srslte/common/test_common.h"
#include "srslte/srslog/srslog.h"
#include <stdio.h>

using namespace srslte;

typedef struct {
  logger* l;
  int     thread_id;
} args_t;

void* thread_loop(void* a)
{
  args_t* args = (args_t*)a;
  char    buf[100];

  sprintf(buf, "LAYER%d", args->thread_id);
  log_filter filter(buf, args->l);
  filter.set_level(LOG_LEVEL_INFO);

  for (int i = 0; i < NMSGS; i++) {
    filter.error("Thread %d: %d", args->thread_id, i);
    filter.warning("Thread %d: %d", args->thread_id, i);
    filter.info("Thread %d: %d", args->thread_id, i);
    filter.debug("Thread %d: %d", args->thread_id, i);
  }
  return NULL;
}

void* thread_loop_hex(void* a)
{
  args_t* args = (args_t*)a;
  char    buf[100];
  uint8_t hex[100];

  for (int i = 0; i < 100; i++) {
    hex[i] = i & 0xFF;
  }
  sprintf(buf, "LAYER%d", args->thread_id);
  log_filter filter(buf, args->l);
  filter.set_level(LOG_LEVEL_DEBUG);
  filter.set_hex_limit(32);

  for (int i = 0; i < NMSGS; i++) {
    filter.error_hex(hex, 100, "Thread %d: %d", args->thread_id, i);
    filter.warning_hex(hex, 100, "Thread %d: %d", args->thread_id, i);
    filter.info_hex(hex, 100, "Thread %d: %d", args->thread_id, i);
    filter.debug_hex(hex, 100, "Thread %d: %d", args->thread_id, i);
  }
  return NULL;
}

void write(std::string filename)
{
  srslog::sink*          s    = srslog::create_file_sink(filename);
  srslog::log_channel*   chan = srslog::create_log_channel("write", *s);
  srslte::srslog_wrapper l(*chan);

  pthread_t threads[NTHREADS];
  args_t    args[NTHREADS];
  for (int i = 0; i < NTHREADS; i++) {
    args[i].l         = &l;
    args[i].thread_id = i;
    pthread_create(&threads[i], NULL, &thread_loop_hex, &args[i]);
  }
  for (int i = 0; i < NTHREADS; i++) {
    pthread_join(threads[i], NULL);
  }
}

bool read(std::string filename)
{
  bool pass = true;
  bool written[NTHREADS][NMSGS];
  int  thread, msg;

  for (int i = 0; i < NTHREADS; i++) {
    for (int j = 0; j < NMSGS; j++) {
      written[i][j] = false;
    }
  }
  FILE* f = fopen(filename.c_str(), "r");
  if (f != NULL) {
    while (fscanf(f, "Thread %d: %d\n", &thread, &msg)) {
      written[thread][msg] = true;
    }
    fclose(f);
  }
  for (int i = 0; i < NTHREADS; i++) {
    for (int j = 0; j < NMSGS; j++) {
      if (!written[i][j])
        pass = false;
    }
  }
  return pass;
}

int basic_hex_test()
{
  srslog::sink* s = srslog::find_sink("stdout");
  if (!s) {
    return SRSLTE_ERROR;
  }
  srslog::log_channel* chan = srslog::create_log_channel("basic_hex_test", *s);
  if (!chan) {
    return SRSLTE_ERROR;
  }
  srslte::srslog_wrapper l(*chan);

  log_filter filter("layer", &l);
  filter.set_level(LOG_LEVEL_DEBUG);
  filter.set_hex_limit(500);

  const uint32_t hex_len = 497;

  uint8_t hex[hex_len];
  for (uint32_t i = 0; i < hex_len; i++) {
    hex[i] = i & 0xFF;
  }

  filter.debug_hex(hex, hex_len, "This is the long hex msg (%d B)\n", hex_len);
  filter.debug("This is a message after the long hex msg that should not be cut\n");

  return SRSLTE_SUCCESS;
}

int test_log_singleton()
{
  srslte::logmap::set_default_log_level(LOG_LEVEL_DEBUG);

  // TEST: Check if default setters are working
  srslte::log_ref log1 = srslte::logmap::get("LAYER1");
  TESTASSERT(log1->get_service_name() == "LAYER1");
  TESTASSERT(log1->get_level() == LOG_LEVEL_DEBUG);

  // TEST: register logger manually. Verify log_ref stays valid after overwrite
  std::unique_ptr<srslte::log_filter> log_ptr2(new srslte::log_filter("LAYER2"));
  log_ptr2->set_level(LOG_LEVEL_WARNING);
  srslte::log_ref old_ref = srslte::logmap::get("LAYER2");
  TESTASSERT(old_ref->get_level() == LOG_LEVEL_DEBUG);
  srslte::logmap::register_log(std::move(log_ptr2));
  srslte::log_ref new_ref = srslte::logmap::get("LAYER2");
  TESTASSERT(new_ref->get_level() == LOG_LEVEL_WARNING);
  TESTASSERT(old_ref == new_ref);

  // TEST: padding working correctly
  TESTASSERT(srslte::logmap::get("MAC").get() == srslte::logmap::get("MAC ").get());

  log1->info("logmap test finished successfully\n");
  return SRSLTE_SUCCESS;
}

int test_log_ref()
{
  // Check if trailing whitespaces are correctly removed
  srslte::log_ref t1_log{"T1"};
  TESTASSERT(t1_log->get_service_name() == "T1");
  TESTASSERT(t1_log.get() == srslte::logmap::get("T1").get());
  TESTASSERT(t1_log.get() == srslte::logmap::get("T1  ").get());
  {
    scoped_log<srslte::nullsink_log> null_log{"T2"};
    TESTASSERT(null_log->get_service_name() == "T2");
  }

  return SRSLTE_SUCCESS;
}

int full_test()
{
  std::string f("log.txt");
  write(f);
#if 0
  bool result = read(f);
  remove(f.c_str());
  if(result) {
    printf("Passed\n");
    exit(0);
  }else{
    printf("Failed\n;");
   exit(1);
  }
#endif

  return SRSLTE_SUCCESS;
}

int main(int argc, char** argv)
{
  // Setup logging.
  srslog::sink* log_sink = srslog::create_stdout_sink();
  if (!log_sink) {
    return SRSLTE_ERROR;
  }

  TESTASSERT(basic_hex_test() == SRSLTE_SUCCESS);
  TESTASSERT(full_test() == SRSLTE_SUCCESS);
  TESTASSERT(test_log_singleton() == SRSLTE_SUCCESS);
  TESTASSERT(test_log_ref() == SRSLTE_SUCCESS);

  return SRSLTE_SUCCESS;
}
