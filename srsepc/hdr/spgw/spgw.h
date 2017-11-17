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
 * File:        spgw.h
 * Description: Top-level SP-GW class. Creates and links all
 *              interfaces and helpers.
 *****************************************************************************/

#ifndef SPGW_H
#define SPGW_H

#include <cstddef>
#include "srslte/common/log.h"
#include "srslte/common/logger_file.h"
#include "srslte/common/log_filter.h"
#include "srslte/common/buffer_pool.h"
#include "srslte/common/threads.h"

namespace srsepc{

typedef struct {
  std::string gtpc_bind_addr;
} spgw_args_t;


class spgw:
  public thread
{
public:
  static spgw* get_instance(void);
  static void cleanup(void);
  int init(spgw_args_t* args, srslte::log_filter *spgw_log);
  void stop();
  void run_thread();

private:

  spgw();
  virtual ~spgw();
  static spgw *m_instance;

  srslte::error_t init_sgi_if();

  bool m_running;
  srslte::byte_buffer_pool *m_pool;

  bool m_if_up;
  int m_sgi_if;
  int m_sgi_sock;
  /*Logs*/
  srslte::log_filter  *m_spgw_log;
 
};

} // namespace srsepc

#endif // SGW_H
