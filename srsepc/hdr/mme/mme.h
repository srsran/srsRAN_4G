/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2020 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

/******************************************************************************
 * File:        mme.h
 * Description: Top-level MME class. Creates and links all
 *              interfaces and helpers.
 *****************************************************************************/

#ifndef SRSEPC_MME_H
#define SRSEPC_MME_H

#include "s1ap.h"
#include "srslte/common/buffer_pool.h"
#include "srslte/common/log.h"
#include "srslte/common/log_filter.h"
#include "srslte/common/threads.h"
#include <cstddef>

namespace srsepc {

typedef struct {
  s1ap_args_t s1ap_args;
  // diameter_args_t diameter_args;
  // gtpc_args_t gtpc_args;
} mme_args_t;

typedef struct {
  int                 fd;
  uint64_t            imsi;
  enum nas_timer_type type;
} mme_timer_t;

class mme : public srslte::thread, public mme_interface_nas
{
public:
  static mme* get_instance(void);
  static void cleanup(void);

  int  init(mme_args_t* args);
  void stop();
  int  get_s1_mme();
  void run_thread();

  // Timer Methods
  virtual bool add_nas_timer(int timer_fd, enum nas_timer_type type, uint64_t imsi);
  virtual bool is_nas_timer_running(enum nas_timer_type type, uint64_t imsi);
  virtual bool remove_nas_timer(enum nas_timer_type type, uint64_t imsi);

private:
  mme();
  virtual ~mme();
  static mme* m_instance;
  s1ap*       m_s1ap;
  mme_gtpc*   m_mme_gtpc;

  bool                      m_running;
  srslte::byte_buffer_pool* m_pool;
  fd_set                    m_set;

  // Timer map
  std::vector<mme_timer_t> timers;

  // Timer Methods
  void handle_timer_expire(int timer_fd);

  // Logs
  srslog::basic_logger& m_s1ap_logger = srslog::fetch_basic_logger("S1AP");
};

} // namespace srsepc
#endif // SRSEPC_MME_H
