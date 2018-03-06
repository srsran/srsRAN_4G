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
#ifndef S1AP_UE_CAP_INFO_H
#define S1AP_UE_CAP_INFO_H

#include "srslte/asn1/liblte_s1ap.h"
#include "srslte/common/common.h"
#include "mme/s1ap_common.h"
#include "srslte/common/log_filter.h"

namespace srsepc{

class s1ap;

class s1ap_ue_cap_info
{
public:

  static s1ap_ue_cap_info *m_instance;

  static s1ap_ue_cap_info* get_instance(void);
  static void cleanup(void);
  void init(void);

  bool handle_ue_capability_info_indication(LIBLTE_S1AP_MESSAGE_UECAPABILITYINFOINDICATION_STRUCT *msg, sctp_sndrcvinfo *enb_sri, srslte::byte_buffer_t *reply_buffer, bool *reply_flag);

private:
  s1ap_ue_cap_info();
  virtual ~s1ap_ue_cap_info();

  s1ap* m_s1ap;
  srslte::log_filter *m_s1ap_log;
};
} //namespace srsepc
#endif 
