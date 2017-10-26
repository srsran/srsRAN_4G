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
 * File:        hss.h
 * Description: Top-level HSS class. Creates and links all
 *              interfaces and helpers.
 *****************************************************************************/

#ifndef HSS_H
#define HSS_H

#include <cstddef>
#include "srslte/common/log.h"
#include "srslte/common/logger_file.h"
#include "srslte/common/log_filter.h"
#include "srslte/common/buffer_pool.h"


namespace srsepc{

typedef struct{
  std::string ue_file;
}hss_args_t;



class hss
{
public:
  static hss* get_instance(void);
  static void cleanup(void);
  int init(hss_args_t *hss_args, srslte::logger* logger);

  //bool gen_auth_info_answer(uint64_t imsi);
  
  //bool get_k_amf_op(uint64_t imsi, *k, *amf, *op);
  
private:

  hss();
  virtual ~hss();
  static hss *m_instance;

  srslte::byte_buffer_pool *m_pool;

  /*Logs*/
  srslte::logger_stdout m_logger_stdout;
  srslte::logger_file   m_logger_file;
  srslte::logger        *m_logger;

  srslte::log_filter  m_hss_log;
 
};

} // namespace srsepc

#endif // MME_H
