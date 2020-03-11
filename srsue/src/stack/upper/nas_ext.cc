/*
 * Copyright 2020 Software Radio Systems Limited
 * Author: Vadim Yanitskiy <axilirator@gmail.com>
 * Sponsored by Positive Technologies
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

#include "srslte/common/buffer_pool.h"
#include "srslte/common/common.h"
#include "srslte/common/log.h"
#include "srslte/common/nas_pcap.h"
#include "srslte/common/security.h"
#include "srslte/common/stack_procedure.h"
#include "srslte/interfaces/ue_interfaces.h"

#include "srsue/hdr/stack/upper/nas.h"
#include "srsue/hdr/stack/upper/nas_common.h"
#include "srsue/hdr/stack/upper/nas_ext.h"
#include "srsue/hdr/stack/upper/nas_metrics.h"

using namespace srslte;

namespace srsue {

void nas_ext::init(usim_interface_nas* usim_, rrc_interface_nas* rrc_, gw_interface_nas* gw_)
{
  usim = usim_;
  rrc  = rrc_;
  gw   = gw_;

  // TODO: parse the configuration
  // TODO: init the UNIX domain socket
}

void nas_ext::get_metrics(nas_metrics_t* m)
{
  nas_metrics_t metrics = {};
  // FIXME: is there anything we could fill in?
  *m = metrics;
}

void nas_ext::stop()
{
  // TODO: close the UNIX domain socket connection
}

/*******************************************************************************
 * UE interface (dummy)
 ******************************************************************************/

void nas_ext::start_attach_request(srslte::proc_state_t* result, srslte::establishment_cause_t cause_)
{
  nas_log->info("The UE has requested us to perform Attach Request, however we ignore it\n");
  if (result != nullptr) {
    result->set_val();
  }
}

bool nas_ext::detach_request(const bool switch_off)
{
  nas_log->info("The UE has requested us to perform Detach Request, however we ignore it\n");
  return false;
}

void nas_ext::timer_expired(uint32_t timeout_id)
{
  nas_log->info("Timer id=%u is expired, however we ignore it\n", timeout_id);
}

/*******************************************************************************
 * RRC interface
 ******************************************************************************/

// TODO: investigate the meaning of these signals
void nas_ext::set_barring(barring_t barring) {}
void nas_ext::run_tti(uint32_t tti) {}
void nas_ext::left_rrc_connected() {}

bool nas_ext::paging(srslte::s_tmsi_t* ue_identity)
{
  nas_log->info("Received paging from RRC\n");
  // TODO: send PAGING.ind to the external entity
  return false;
}

void nas_ext::write_pdu(uint32_t lcid, srslte::unique_byte_buffer_t pdu)
{
  nas_log->info_hex(pdu->msg, pdu->N_bytes, "Received DL %s PDU from RRC\n", rrc->get_rb_name(lcid).c_str());
  // TODO: send DATA.ind to the external entity
}

uint32_t nas_ext::get_k_enb_count()
{
  // FIXME: we probably need to maintain a security context
  return 0; // return a dummy value for now
}

bool nas_ext::is_attached()
{
  // FIXME: we probably need to maintain the state
  return false; // return a dummy value for now
}

bool nas_ext::get_k_asme(uint8_t* k_asme_, uint32_t n)
{
  // FIXME: we probably need to maintain a security context
  return false; // return a dummy value for now
}

uint32_t nas_ext::get_ipv4_addr()
{
  // FIXME: where can we get it? maybe from GW?
  return 0x00000000;
}

bool nas_ext::get_ipv6_addr(uint8_t* ipv6_addr)
{
  // FIXME: where can we get it? maybe from GW?
  return false;
}

void nas_ext::plmn_search_completed(
    const rrc_interface_nas::found_plmn_t found_plmns[rrc_interface_nas::MAX_FOUND_PLMNS],
    int                                   nof_plmns)
{
  nas_log->info("RRC has completed PLMN search, %d carriers found\n", nof_plmns);
  // TODO: send PLMN_SEARCH.res to the external entity
}

bool nas_ext::connection_request_completed(bool outcome)
{
  nas_log->info("RRC has %s connection establisment\n", outcome ? "completed" : "failed");
  // TODO: send CONN_ESTABLISH.res to the external entity
  return false;
}

} // namespace srsue
