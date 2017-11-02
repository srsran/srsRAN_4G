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

#include "mme/s1ap.h"
#include "mme/s1ap_nas_transport.h"

namespace srsepc{

s1ap_nas_transport::s1ap_nas_transport()
{
  return;
}

s1ap_nas_transport::~s1ap_nas_transport()
{
  return;
}

void
s1ap_nas_transport::set_log(srslte::log *s1ap_log)
{
  m_s1ap_log=s1ap_log;
  return;
}

bool 
s1ap_nas_transport::unpack_initial_ue_message(LIBLTE_S1AP_MESSAGE_S1SETUPREQUEST_STRUCT *msg, uint64_t *imsi)
{
  return true;
}

bool
s1ap_nas_transport::pack_authentication_request(uint8_t *autn,uint8_t *rand)
{

  LIBLTE_MME_AUTHENTICATION_REQUEST_MSG_STRUCT pdu;
  memcpy(pdu.autn , autn, 16);
  memcpy(pdu.rand, rand, 16); 
  pdu.nas_ksi.tsc_flag=LIBLTE_MME_TYPE_OF_SECURITY_CONTEXT_FLAG_NATIVE;
  pdu.nas_ksi.nas_ksi=0;
  
  return true;
}

} //namespace srsepc
