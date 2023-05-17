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
#ifndef SRSEPC_S1AP_PAGING_H
#define SRSEPC_S1AP_PAGING_H

#include "s1ap_common.h"
#include "srsran/common/buffer_pool.h"
#include "srsran/common/common.h"
#include "srsran/srslog/srslog.h"

namespace srsepc {

class s1ap;
class mme;

class s1ap_paging
{
  s1ap_paging() = default;

public:
  virtual ~s1ap_paging() = default;

  static s1ap_paging* get_instance();
  void                init();

  // Packing/unpacking helper functions
  bool send_paging(uint64_t imsi, uint16_t erab_to_setup);

private:
  mme*                  m_mme    = nullptr;
  s1ap*                 m_s1ap   = nullptr;
  srslog::basic_logger& m_logger = srslog::fetch_basic_logger("S1AP");

  s1ap_args_t m_s1ap_args;
};

} // namespace srsepc

#endif // SRSEPC_S1AP_PAGING_H
