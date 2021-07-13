/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSENB_MAC_NR_H
#define SRSENB_MAC_NR_H

#include "srsran/common/block_queue.h"
#include "srsran/common/mac_pcap.h"
#include "srsran/mac/mac_sch_pdu_nr.h"

#include "srsenb/hdr/stack/enb_stack_base.h"
#include "srsran/common/task_scheduler.h"
#include "srsran/interfaces/enb_metrics_interface.h"
#include "srsran/interfaces/gnb_interfaces.h"

namespace srsenb {

struct mac_nr_args_t {
  srsenb::pcap_args_t pcap;

  // params for the dummy user
  srsenb::sched_interface::sched_args_t sched;
  uint16_t                              rnti;
  uint32_t                              drb_lcid;

  // Add args
  std::string log_level;
  uint32_t    log_hex_limit;
  uint32_t    tb_size = 64;
};

class mac_nr final : public mac_interface_phy_nr, public mac_interface_rrc_nr, public mac_interface_rlc_nr
{
public:
  mac_nr(srsran::task_sched_handle task_sched_);
  ~mac_nr();

  int  init(const mac_nr_args_t&    args_,
            phy_interface_stack_nr* phy,
            stack_interface_mac*    stack_,
            rlc_interface_mac_nr*   rlc_,
            rrc_interface_mac_nr*   rrc_);
  void stop();

  void get_metrics(srsenb::mac_metrics_t& metrics);

  // MAC interface for RRC
  int cell_cfg(srsenb::sched_interface::cell_cfg_t* cell_cfg) override;
  int read_pdu_bcch_bch(uint8_t* payload);

  // MAC interface for RLC
  // TODO:
  int rlc_buffer_state(uint16_t rnti, uint32_t lc_id, uint32_t tx_queue, uint32_t retx_queue) override { return 0; }

  // Interface for PHY
  int sf_indication(const uint32_t tti);
  int rx_data_indication(stack_interface_phy_nr::rx_data_ind_t& grant);

  void process_pdus();
  int  slot_indication(const srsran_slot_cfg_t& slot_cfg) override;
  int  get_dl_sched(const srsran_slot_cfg_t& slot_cfg, dl_sched_t& dl_sched) override;
  int  get_ul_sched(const srsran_slot_cfg_t& slot_cfg, ul_sched_t& ul_sched) override;
  int  pucch_info(const srsran_slot_cfg_t& slot_cfg, const pucch_info_t& pucch_info) override;
  int  pusch_info(const srsran_slot_cfg_t& slot_cfg, const pusch_info_t& pusch_info) override;
  void rach_detected(const rach_info_t& rach_info) override;

private:
  // PDU processing
  int handle_pdu(srsran::unique_byte_buffer_t pdu);

  // Interaction with other components
  phy_interface_stack_nr* phy_h   = nullptr;
  stack_interface_mac*    stack_h = nullptr;
  rlc_interface_mac_nr*   rlc_h   = nullptr;
  rrc_interface_mac_nr*   rrc_h   = nullptr;

  // args
  srsran::task_sched_handle task_sched;

  std::unique_ptr<srsran::mac_pcap> pcap = nullptr;
  mac_nr_args_t                     args = {};
  srslog::basic_logger&             logger;

  bool started = false;

  srsenb::sched_interface::cell_cfg_t cfg = {};

  // BCH buffers
  struct sib_info_t {
    uint32_t                     index;
    uint32_t                     periodicity;
    srsran::unique_byte_buffer_t payload;
  };
  std::vector<sib_info_t>      bcch_dlsch_payload;
  srsran::unique_byte_buffer_t bcch_bch_payload = nullptr;

  // UE-specific buffer
  srsran::mac_sch_pdu_nr                    ue_tx_pdu;
  std::vector<srsran::unique_byte_buffer_t> ue_tx_buffer;
  srsran::block_queue<srsran::unique_byte_buffer_t>
      ue_rx_pdu_queue; ///< currently only DCH PDUs supported (add BCH, PCH, etc)

  srsran::unique_byte_buffer_t ue_rlc_buffer;

  srsran::mac_sch_pdu_nr ue_rx_pdu;
};

} // namespace srsenb

#endif // SRSENB_MAC_NR_H
