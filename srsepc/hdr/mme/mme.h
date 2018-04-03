/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2017 Software Radio Systems Limited
 *
 * \section LICENSE
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

/******************************************************************************
 * File:        mme.h
 * Description: Top-level MME class. Creates and links all
 *              interfaces and helpers.
 *****************************************************************************/

#ifndef SRSEPC_MME_H
#define SRSEPC_MME_H

#include <cstddef>
#include "srslte/common/log.h"
#include "srslte/common/logger_file.h"
#include "srslte/common/log_filter.h"
#include "srslte/common/buffer_pool.h"
#include "srslte/common/threads.h"
#include "s1ap.h"


namespace srsepc{

/*
typedef struct {
  std::string   s1ap_level;
  std::string   all_level;
  int           s1ap_hex_limit;
  std::string   filename;
}log_args_t;
*/

typedef struct{
  s1ap_args_t s1ap_args;
  //diameter_args_t diameter_args;
  //gtpc_args_t gtpc_args;
} mme_args_t;


class mme:
  public thread
{
public:
  static mme* get_instance(void);
  static void cleanup(void);
  int init(mme_args_t* args, srslte::log_filter *s1ap_log, srslte::log_filter *mme_gtpc_log, hss_interface_s1ap * hss_);
  void stop();
  int get_s1_mme();
  void run_thread();

private:

  mme();
  virtual ~mme();
  static mme *m_instance;
  s1ap *m_s1ap;
  mme_gtpc *m_mme_gtpc;

  bool m_running;
  srslte::byte_buffer_pool *m_pool;

  /*Logs*/
  srslte::log_filter  *m_s1ap_log;
  srslte::log_filter  *m_mme_gtpc_log;
};

} // namespace srsepc

#endif // SRSEPC_MME_H
