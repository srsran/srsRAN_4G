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
#include "mme/s1ap_ue_cap_info.h"

namespace srsepc{

s1ap_ue_cap_info*          s1ap_ue_cap_info::m_instance = NULL;
boost::mutex   s1ap_ue_cap_info_instance_mutex;


s1ap_ue_cap_info::s1ap_ue_cap_info()
{
}

s1ap_ue_cap_info::~s1ap_ue_cap_info()
{
}

s1ap_ue_cap_info*
s1ap_ue_cap_info::get_instance(void)
{
  boost::mutex::scoped_lock lock(s1ap_ue_cap_info_instance_mutex);
  if(NULL == m_instance) {
    m_instance = new s1ap_ue_cap_info();
  }
  return(m_instance);
}

void
s1ap_ue_cap_info::cleanup(void)
{
  boost::mutex::scoped_lock lock(s1ap_ue_cap_info_instance_mutex);
  if(NULL != m_instance) {
    delete m_instance;
    m_instance = NULL;
  }
}

void
s1ap_ue_cap_info::init(void)
{
  m_s1ap     = s1ap::get_instance();
  m_s1ap_log = m_s1ap->m_s1ap_log;
}

bool
s1ap_ue_cap_info::handle_ue_capability_info_indication(LIBLTE_S1AP_MESSAGE_UECAPABILITYINFOINDICATION_STRUCT *msg, struct sctp_sndrcvinfo *enb_sri, srslte::byte_buffer_t *reply_buffer, bool *reply_flag)
{
  m_s1ap_log->info("UE Capability Info Indication\n");
  return true;
}

} //namespace srsepc
