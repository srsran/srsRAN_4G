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
#ifndef S1AP_NAS_TRANSPORT_H
#define S1AP_NAS_TRANSPORT_H

#include "srslte/asn1/liblte_s1ap.h"
#include "srslte/common/common.h"
#include "mme/s1ap_common.h"

namespace srsepc{

class s1ap_nas_transport
{
public:
  s1ap_nas_transport();
  virtual ~s1ap_nas_transport();

  void set_log(srslte::log *s1ap_logger);
  bool unpack_initial_ue_message(LIBLTE_S1AP_MESSAGE_S1SETUPREQUEST_STRUCT *msg, uint64_t *imsi);
  bool pack_authentication_request(uint8_t *autn,uint8_t *rand);

private:
  srslte::log *m_s1ap_log;

};

} //namespace srsepc

#endif //S1AP_NAS_TRANSPORT
