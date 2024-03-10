/**
 * Copyright 2013-2023 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
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

#include "sched.h"
#include "sched_interface.h"
#include "srsenb/hdr/common/rnti_pool.h"
#include "srsenb/hdr/stack/mac/schedulers/sched_time_rr.h"
#include "srsran/adt/circular_map.h"
#include "srsran/adt/pool/batch_mem_pool.h"
#include "srsran/common/mac_pcap.h"
#include "srsran/common/mac_pcap_net.h"
#include "srsran/common/task_scheduler.h"
#include "srsran/common/threads.h"
#include "srsran/common/tti_sync_cv.h"
#include "srsran/interfaces/enb_mac_interfaces.h"
#include "srsran/interfaces/enb_metrics_interface.h"
#include "srsran/interfaces/enb_rrc_interface_types.h"
#include "srsran/srslog/srslog.h"
#include "ta.h"
#include "ue.h"
#include <vector>

namespace srsenb {

class mac final : public mac_interface_phy_lte, public mac_interface_rlc, public mac_interface_rrc
{
public:
  mac(srsran::ext_task_sched_handle task_sched_, srslog::basic_logger& logger);
  ~mac();
  bool init(const mac_args_t&        args_,
            const cell_list_t&       cells_,
            phy_interface_stack_lte* phy,
            rlc_interface_mac*       rlc,
            rrc_interface_mac*       rrc);
  void stop();

  void start_pcap(srsran::mac_pcap* pcap_);
  void start_pcap_net(srsran::mac_pcap_net* pcap_net_);

  /******** Interface from PHY (PHY -> MAC) ****************/
  int  sr_detected(uint32_t tti, uint16_t rnti) final;
  void rach_detected(uint32_t tti, uint32_t enb_cc_idx, uint32_t preamble_idx, uint32_t time_adv) final;

  int ri_info(uint32_t tti, uint16_t rnti, uint32_t enb_cc_idx, uint32_t ri_value) override;
  int pmi_info(uint32_t tti, uint16_t rnti, uint32_t enb_cc_idx, uint32_t pmi_value) override;
  int cqi_info(uint32_t tti, uint16_t rnti, uint32_t enb_cc_idx, uint32_t cqi_value) override;
  int sb_cqi_info(uint32_t tti, uint16_t rnti, uint32_t enb_cc_idx, uint32_t sb_idx, uint32_t cqi_value) override;
  int snr_info(uint32_t tti, uint16_t rnti, uint32_t enb_cc_idx, float snr, ul_channel_t ch) override;
  int ta_info(uint32_t tti, uint16_t rnti, float ta_us) override;
  int ack_info(uint32_t tti, uint16_t rnti, uint32_t enb_cc_idx, uint32_t tb_idx, bool ack) override;
  int crc_info(uint32_t tti, uint16_t rnti, uint32_t enb_cc_idx, uint32_t nof_bytes, bool crc_res) override;
  int push_pdu(uint32_t tti, uint16_t rnti, uint32_t enb_cc_idx, uint32_t nof_bytes, bool crc_res, uint32_t ul_nof_prbs)
      override;

  int  get_dl_sched(uint32_t tti_tx_dl, dl_sched_list_t& dl_sched_res) override;
  int  get_ul_sched(uint32_t tti_tx_ul, ul_sched_list_t& ul_sched_res) override;
  int  get_mch_sched(uint32_t tti, bool is_mcch, dl_sched_list_t& dl_sched_res) override;
  void set_sched_dl_tti_mask(uint8_t* tti_mask, uint32_t nof_sfs) override
  {
    scheduler.set_dl_tti_mask(tti_mask, nof_sfs);
  }
  void build_mch_sched(uint32_t tbs);

  /******** Interface from RRC (RRC -> MAC) ****************/
  /* Provides cell configuration including SIB periodicity, etc. */
  int cell_cfg(const std::vector<sched_interface::cell_cfg_t>& cell_cfg) override;

  /* Manages UE scheduling context */
  int ue_cfg(uint16_t rnti, const sched_interface::ue_cfg_t* cfg) override;
  int ue_rem(uint16_t rnti) override;
  int ue_set_crnti(uint16_t temp_crnti, uint16_t crnti, const sched_interface::ue_cfg_t& cfg) override;

  // Indicates that the PHY config dedicated has been enabled or not
  void phy_config_enabled(uint16_t rnti, bool enabled) override;

  /* Manages UE bearers and associated configuration */
  int bearer_ue_cfg(uint16_t rnti, uint32_t lc_id, mac_lc_ch_cfg_t* cfg) override;
  int bearer_ue_rem(uint16_t rnti, uint32_t lc_id) override;
  int rlc_buffer_state(uint16_t rnti, uint32_t lc_id, uint32_t tx_queue, uint32_t retx_queue) override;

  /* Handover-related */
  uint16_t reserve_new_crnti(const sched_interface::ue_cfg_t& ue_cfg) override;

  void get_metrics(mac_metrics_t& metrics);

  void toggle_padding();

  void add_padding();

  void write_mcch(const srsran::sib2_mbms_t* sib2_,
                  const srsran::sib13_t*     sib13_,
                  const srsran::mcch_msg_t*  mcch_,
                  const uint8_t*             mcch_payload,
                  const uint8_t              mcch_payload_length) override;

private:
  /// \brief Reads the SIB message to be scheduled from the RRC and advances the current SIB segment if required.
  ///
  /// \param[out]    grant        DL grant where the SIB message will be stored.
  /// \param[out,in] sib_config   SIB parameters required for scheduling.
  /// \param[in]     sched_result Scheduling parameters of the SIB allocation.
  /// \param[in]     cc_idx       ENB Carrier index.
  void read_sib(dl_sched_grant_t&           grant,
                sched::cell_cfg_sib&        sib_config,
                const sched::dl_sched_bc_t& sched_result,
                unsigned                    cc_idx);

  bool     check_ue_active(uint16_t rnti);
  uint16_t allocate_ue(uint32_t enb_cc_idx);
  bool     is_valid_rnti_unprotected(uint16_t rnti);

  /* helper function for PDCCH orders */
  /**
   * @brief Checks if the current RACH is a RACH triggered by a PDCCH order.
   *
   * @param[in] preamble_idx RACH preamble idx
   * @param rnti is the rnti where the crnti of the RACH is written
   * @return true if this is a RACH triggered by a PDCCH order, otherwise it returns false
   */
  bool is_pending_pdcch_order_prach(const uint32_t preamble_idx, uint16_t& rnti);

  srslog::basic_logger& logger;

  // We use a rwlock in MAC to allow multiple workers to access MAC simultaneously. No conflicts will happen since
  // access for different TTIs
  pthread_rwlock_t rwlock = {};

  // Interaction with PHY
  phy_interface_stack_lte*      phy_h = nullptr;
  rlc_interface_mac*            rlc_h = nullptr;
  rrc_interface_mac*            rrc_h = nullptr;
  srsran::ext_task_sched_handle task_sched;

  cell_list_t cells = {};
  mac_args_t  args  = {};

  // derived from args
  srsran::task_multiqueue::queue_handle stack_task_queue;

  bool started = false;

  /* Scheduler unit */
  sched                                    scheduler;
  std::vector<sched_interface::cell_cfg_t> cell_config;

  sched_interface::dl_pdu_mch_t mch = {};

  /* Map of active UEs */
  static const uint16_t            FIRST_RNTI = 0x46;
  rnti_map_t<unique_rnti_ptr<ue> > ue_db;
  std::atomic<uint16_t>            ue_counter{0};

  uint8_t* assemble_rar(sched_interface::dl_sched_rar_grant_t* grants,
                        uint32_t                               enb_cc_idx,
                        uint32_t                               nof_grants,
                        uint32_t                               rar_idx,
                        uint32_t                               pdu_len,
                        uint32_t                               tti);

  const static int                                           rar_payload_len = 128;
  std::array<srsran::rar_pdu, sched_interface::MAX_RAR_LIST> rar_pdu_msg;
  srsran::byte_buffer_t rar_payload[SRSRAN_MAX_CARRIERS][sched_interface::MAX_RAR_LIST];

  const static int NOF_BCCH_DLSCH_MSG = sched_interface::MAX_SIBS;

  const static int pcch_payload_buffer_len = 1024;
  struct common_buffers_t {
    uint8_t                pcch_payload_buffer[pcch_payload_buffer_len] = {};
    srsran_softbuffer_tx_t bcch_softbuffer_tx[NOF_BCCH_DLSCH_MSG]       = {};
    srsran_softbuffer_tx_t pcch_softbuffer_tx                           = {};
    srsran_softbuffer_tx_t rar_softbuffer_tx                            = {};
  };

  std::vector<common_buffers_t> common_buffers;

  const static int    mcch_payload_len                      = 3000; // TODO FIND OUT MAX LENGTH
  int                 current_mcch_length                   = 0;
  uint8_t             mcch_payload_buffer[mcch_payload_len] = {};
  srsran::mcch_msg_t  mcch;
  srsran::sib2_mbms_t sib2;
  srsran::sib13_t     sib13;
  const static int    mtch_payload_len                      = 10000;
  uint8_t             mtch_payload_buffer[mtch_payload_len] = {};

  // pointer to MAC PCAP object
  srsran::mac_pcap*     pcap       = nullptr;
  srsran::mac_pcap_net* pcap_net   = nullptr;
  bool                  do_padding = false;

  // Number of rach preambles detected for a cc.
  std::vector<uint32_t> detected_rachs;

  // PDCCH order
  std::vector<sched_interface::dl_sched_po_info_t> pending_po_prachs = {};

  // Softbuffer pool
  std::unique_ptr<srsran::obj_pool_itf<ue_cc_softbuffers> > softbuffer_pool;
};

} // namespace srsenb

#endif // SRSENB_MAC_H
