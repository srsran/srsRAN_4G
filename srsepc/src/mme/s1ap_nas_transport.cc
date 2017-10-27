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

s1ap_nas_transport::s1ap_nas_transport(srslte::logger *s1ap_logger)
{
  m_s1ap_logger=s1ap_logger;
}

s1ap_nas_transport::~s1ap_nas_transport()
{
}

bool 
s1ap_nas_transport::initial_ue_message(LIBLTE_S1AP_MESSAGE_S1SETUPREQUEST_STRUCT *msg, uint64_t *imsi)
{
  return true;
}

  
} //namespace srsepc
