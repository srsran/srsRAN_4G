/*
 * Copyright 2013-2019 Software Radio Systems Limited
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

#ifndef SRSENB_MAC_H
#define SRSENB_MAC_H

#include "scheduler.h"
#include "scheduler_metric.h"
#include "srslte/common/log.h"
#include "srslte/common/mac_pcap.h"
#include "srslte/common/threads.h"
#include "srslte/common/tti_sync_cv.h"
#include "srslte/interfaces/enb_interfaces.h"
#include "srslte/interfaces/enb_metrics_interface.h"
#include "srslte/interfaces/sched_interface.h"
#include "ue.h"
#include <vector>

namespace srsenb {

class mac final : public mac_interface_phy_lte, public mac_interface_rlc, public mac_interface_rrc
{
public:
  mac();
  ~mac();
  bool init(const mac_args_t&        args_,
            srslte_cell_t*           cell,
            phy_interface_stack_lte* phy,
            rlc_interface_mac*       rlc,
            rrc_interface_mac*       rrc,
            stack_interface_mac_lte* stack_,
            srslte::log*             log_h);
  void stop();

  void start_pcap(srslte::mac_pcap* pcap_);

  /******** Interface from PHY (PHY -> MAC) ****************/
  int sr_detected(uint32_t tti, uint16_t rnti) final;
  int rach_detected(uint32_t tti, uint32_t primary_cc_idx, uint32_t preamble_idx, uint32_t time_adv) final;

  int ri_info(uint32_t tti, uint16_t rnti, uint32_t ri_value);
  int pmi_info(uint32_t tti, uint16_t rnti, uint32_t pmi_value);
  int cqi_info(uint32_t tti, uint16_t rnti, uint32_t cqi_value);
  int snr_info(uint32_t tti, uint16_t rnti, float snr);
  int ack_info(uint32_t tti, uint16_t rnti, uint32_t tb_idx, bool ack);
  int crc_info(uint32_t tti, uint16_t rnti, uint32_t nof_bytes, bool crc_res);

  int  get_dl_sched(uint32_t tti, dl_sched_list_t& dl_sched_res);
  int  get_ul_sched(uint32_t tti, ul_sched_list_t& ul_sched_res);
  int  get_mch_sched(uint32_t tti, bool is_mcch, dl_sched_list_t& dl_sched_res);
  void set_sched_dl_tti_mask(uint8_t* tti_mask, uint32_t nof_sfs) final
  {
    scheduler.set_dl_tti_mask(tti_mask, nof_sfs);
  }
  void build_mch_sched(uint32_t tbs);
  void rl_failure(uint16_t rnti);
  void rl_ok(uint16_t rnti);

  /******** Interface from RRC (RRC -> MAC) ****************/
  /* Provides cell configuration including SIB periodicity, etc. */
  int  cell_cfg(const std::vector<sched_interface::cell_cfg_t>& cell_cfg) override;
  void reset() override;

  /* Manages UE scheduling context */
  int ue_cfg(uint16_t rnti, sched_interface::ue_cfg_t* cfg);
  int ue_rem(uint16_t rnti);

  // Indicates that the PHY config dedicated has been enabled or not
  void phy_config_enabled(uint16_t rnti, bool enabled);

  /* Manages UE bearers and associated configuration */
  int bearer_ue_cfg(uint16_t rnti, uint32_t lc_id, sched_interface::ue_bearer_cfg_t* cfg);
  int bearer_ue_rem(uint16_t rnti, uint32_t lc_id);
  int rlc_buffer_state(uint16_t rnti, uint32_t lc_id, uint32_t tx_queue, uint32_t retx_queue);

  bool process_pdus();

  uint32_t get_current_tti();
  void     get_metrics(mac_metrics_t metrics[ENB_METRICS_MAX_USERS]);
  void     write_mcch(asn1::rrc::sib_type2_s* sib2, asn1::rrc::sib_type13_r9_s* sib13, asn1::rrc::mcch_msg_s* mcch);

private:
  static const int      MAX_LOCATIONS            = 20;
  static const uint32_t cfi                      = 3;
  srslte_dci_location_t locations[MAX_LOCATIONS] = {};

  static const int MAC_PDU_THREAD_PRIO = 60;

  // We use a rwlock in MAC to allow multiple workers to access MAC simultaneously. No conflicts will happen since
  // access for different TTIs
  pthread_rwlock_t rwlock = {};

  // Interaction with PHY
  phy_interface_stack_lte* phy_h = nullptr;
  rlc_interface_mac*       rlc_h = nullptr;
  rrc_interface_mac*       rrc_h = nullptr;
  stack_interface_mac_lte* stack = nullptr;
  srslte::log*             log_h = nullptr;

  srslte_cell_t cell;
  mac_args_t    args = {};

  bool started = false;

  /* Scheduler unit */
  sched                                    scheduler;
  std::vector<sched_interface::cell_cfg_t> cell_config;

  sched_interface::dl_pdu_mch_t mch;

  /* Map of active UEs */
  std::map<uint16_t, ue*> ue_db;
  uint16_t                last_rnti;

  uint8_t* assemble_rar(sched_interface::dl_sched_rar_grant_t* grants,
                        uint32_t                               nof_grants,
                        int                                    rar_idx,
                        uint32_t                               pdu_len,
                        uint32_t                               tti);
  uint8_t* assemble_si(uint32_t index);

  const static int             rar_payload_len = 128;
  std::vector<srslte::rar_pdu> rar_pdu_msg;
  srslte::byte_buffer_t        rar_payload[sched_interface::MAX_RAR_LIST];

  const static int NOF_BCCH_DLSCH_MSG = sched_interface::MAX_SIBS;
  uint8_t          bcch_dlsch_payload[sched_interface::MAX_SIB_PAYLOAD_LEN];

  const static int       pcch_payload_buffer_len = 1024;
  uint8_t                pcch_payload_buffer[pcch_payload_buffer_len];
  srslte_softbuffer_tx_t bcch_softbuffer_tx[NOF_BCCH_DLSCH_MSG];
  srslte_softbuffer_tx_t pcch_softbuffer_tx;
  srslte_softbuffer_tx_t rar_softbuffer_tx;

  const static int           mcch_payload_len = 3000; // TODO FIND OUT MAX LENGTH
  int                        current_mcch_length;
  uint8_t                    mcch_payload_buffer[mcch_payload_len];
  asn1::rrc::mcch_msg_s      mcch;
  asn1::rrc::sib_type2_s     sib2;
  asn1::rrc::sib_type13_r9_s sib13;

  const static int mtch_payload_len = 10000;
  uint8_t          mtch_payload_buffer[mtch_payload_len];

  // pointer to MAC PCAP object
  srslte::mac_pcap* pcap = nullptr;
};

} // namespace srsenb

#endif // SRSENB_MAC_H
