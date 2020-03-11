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

#ifndef SRSUE_NAS_EXT_H
#define SRSUE_NAS_EXT_H

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

using srslte::byte_buffer_t;

namespace srsue {

class nas_ext : public nas_base
{
public:
  nas_ext(srslte::log* log_, srslte::timer_handler* timers_, const nas_ext_args_t& cfg_) :
    nas_base::nas_base(log_, timers_), cfg(cfg_){};

  void init(usim_interface_nas* usim_, rrc_interface_nas* rrc_, gw_interface_nas* gw_);
  void get_metrics(nas_metrics_t* m);
  void stop();

  // RRC interface
  void     left_rrc_connected();
  bool     paging(srslte::s_tmsi_t* ue_identity);
  void     set_barring(barring_t barring);
  void     write_pdu(uint32_t lcid, srslte::unique_byte_buffer_t pdu);
  uint32_t get_k_enb_count();
  bool     is_attached();
  bool     get_k_asme(uint8_t* k_asme_, uint32_t n);
  uint32_t get_ipv4_addr();
  bool     get_ipv6_addr(uint8_t* ipv6_addr);

  void plmn_search_completed(const rrc_interface_nas::found_plmn_t found_plmns[rrc_interface_nas::MAX_FOUND_PLMNS],
                             int                                   nof_plmns) final;
  bool connection_request_completed(bool outcome) final;
  void run_tti(uint32_t tti) final;

  // UE interface
  void start_attach_request(srslte::proc_state_t* result, srslte::establishment_cause_t cause_) final;
  bool detach_request(const bool switch_off) final;

  // timer callback
  void timer_expired(uint32_t timeout_id);

private:
  nas_ext_args_t cfg = {};
};

} // namespace srsue

#endif // SRSUE_NAS_EXT_H
