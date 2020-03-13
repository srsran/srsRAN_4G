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
#ifndef SRSEPC_S1AP_PAGING_H
#define SRSEPC_S1AP_PAGING_H

#include "s1ap_common.h"
#include "srslte/common/buffer_pool.h"
#include "srslte/common/common.h"
#include "srslte/common/log_filter.h"

namespace srsepc {

class s1ap;
class mme;

class s1ap_paging
{
public:
  static s1ap_paging* m_instance;
  static s1ap_paging* get_instance(void);
  static void         cleanup(void);
  void                init(void);

  // Packing/unpacking helper functions
  bool send_paging(uint64_t imsi, uint16_t erab_to_setup);

private:
  s1ap_paging();
  virtual ~s1ap_paging();

  mme*                m_mme;
  s1ap*               m_s1ap;
  srslte::log_filter* m_s1ap_log;

  s1ap_args_t               m_s1ap_args;
  srslte::byte_buffer_pool* m_pool;
};

} // namespace srsepc

#endif // SRSEPC_S1AP_PAGING_H
