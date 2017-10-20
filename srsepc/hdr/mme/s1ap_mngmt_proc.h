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
#ifndef S1AP_MNGMT_PROC_H
#define S1AP_MNGMT_PROC_H

#include "srslte/asn1/liblte_s1ap.h"
#include "srslte/common/common.h"
#include "mme/s1ap_common.h"

namespace srsepc{

class s1ap_mngmt_proc
{
public:
  s1ap_mngmt_proc();
  virtual ~s1ap_mngmt_proc();

  bool unpack_s1_setup_request(LIBLTE_S1AP_MESSAGE_S1SETUPREQUEST_STRUCT *msg, enb_ctx_t* enb_ctx);
  bool pack_s1_setup_failure(LIBLTE_S1AP_CAUSEMISC_ENUM cause, srslte::byte_buffer_t* msg);
  bool pack_s1_setup_response(s1ap_args_t s1ap_args, srslte::byte_buffer_t* msg);
};

} //namespace srsepc

#endif //S1AP_MNGMT_PROC
