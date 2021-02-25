/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2020 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSUE_MAC_NR_H
#define SRSUE_MAC_NR_H

#include "mac_nr_interfaces.h"
#include "proc_ra_nr.h"
#include "srslte/common/block_queue.h"
#include "srslte/common/mac_pcap.h"
#include "srslte/interfaces/mac_interface_types.h"
#include "srslte/interfaces/ue_nr_interfaces.h"
#include "srslte/interfaces/ue_rlc_interfaces.h"
#include "srslte/mac/mac_sch_pdu_nr.h"
#include "srslte/srslog/srslog.h"
#include "srsue/hdr/stack/mac_nr/mux_nr.h"
#include "srsue/hdr/stack/ue_stack_base.h"

namespace srsue {

class rlc_interface_mac;

struct mac_nr_args_t {
};

class mac_nr final : public mac_interface_phy_nr, public mac_interface_rrc_nr, public mac_interface_proc_ra_nr
{
public:
  mac_nr(srslte::ext_task_sched_handle task_sched_);
  ~mac_nr();

  int  init(const mac_nr_args_t& args_, phy_interface_mac_nr* phy, rlc_interface_mac* rlc);
  void stop();

  void reset();
  void run_tti(const uint32_t tti);

  void start_pcap(srslte::mac_pcap* pcap_);

  void bch_decoded_ok(uint32_t tti, srslte::unique_byte_buffer_t payload);

  /// Interface for PHY
  sched_rnti_t get_dl_sched_rnti_nr(const uint32_t tti);
  sched_rnti_t get_ul_sched_rnti_nr(const uint32_t tti);

  int  sf_indication(const uint32_t tti);
  void tb_decoded(const uint32_t cc_idx, mac_nr_grant_dl_t& grant);
  void new_grant_ul(const uint32_t cc_idx, const mac_nr_grant_ul_t& grant, tb_action_ul_t* action);
  void prach_sent(const uint32_t tti,
                  const uint32_t s_id,
                  const uint32_t t_id,
                  const uint32_t f_id,
                  const uint32_t ul_carrier_id);

  /// Stack interface
  void timer_expired(uint32_t timer_id);
  void get_metrics(mac_metrics_t* metrics);

  /// Interface for RRC (RRC -> MAC)
  void setup_lcid(const srslte::logical_channel_config_t& config);
  void set_config(const srslte::bsr_cfg_t& bsr_cfg);
  void set_config(const srslte::sr_cfg_t& sr_cfg);
  void set_config(const srslte::rach_nr_cfg_t& rach_cfg);
  void set_contention_id(const uint64_t ue_identity);
  bool set_crnti(const uint16_t crnti);
  void start_ra_procedure();

  /// procedure ra nr interface
  uint64_t get_contention_id();
  uint16_t get_c_rnti();
  void     set_c_rnti(uint64_t c_rnti_);

  void msg3_flush() { mux.msg3_flush(); }
  bool msg3_is_transmitted() { return mux.msg3_is_transmitted(); }
  void msg3_prepare() { mux.msg3_prepare(); }
  bool msg3_is_pending() { return mux.msg3_is_pending(); }
  bool msg3_is_empty() { return mux.msg3_is_empty(); }

  /// stack interface
  void process_pdus();

  static bool is_in_window(uint32_t tti, int* start, int* len);

  // PHY Interface
  void prach_sent(const uint32_t tti);
  void tb_decoded_ok(const uint8_t cc_idx, const uint32_t tti);

private:
  void write_pcap(const uint32_t cc_idx, mac_nr_grant_dl_t& grant); // If PCAPs are enabled for this MAC
  void handle_pdu(srslte::unique_byte_buffer_t pdu);
  void get_ul_data(const mac_nr_grant_ul_t& grant, srslte::byte_buffer_t* tx_pdu);

  // temporary helper
  void handle_rar_pdu(mac_nr_grant_dl_t& grant);

  bool is_si_opportunity();
  bool is_paging_opportunity();

  bool     has_crnti();
  uint16_t get_crnti();
  bool     is_valid_crnti(const uint16_t crnti);

  /// Interaction with rest of the stack
  phy_interface_mac_nr*         phy = nullptr;
  rlc_interface_mac*            rlc = nullptr;
  srslte::ext_task_sched_handle task_sched;

  srslte::mac_pcap*     pcap = nullptr;
  srslog::basic_logger& logger;
  mac_nr_args_t         args = {};

  bool started = false;

  uint16_t c_rnti        = SRSLTE_INVALID_RNTI;
  uint64_t contention_id = 0;

  static constexpr uint32_t MIN_RLC_PDU_LEN =
      5; ///< minimum bytes that need to be available in a MAC PDU for attempting to add another RLC SDU

  srslte::block_queue<srslte::unique_byte_buffer_t>
      pdu_queue; ///< currently only DCH PDUs supported (add BCH, PCH, etc)

  mac_metrics_t metrics[SRSLTE_MAX_CARRIERS] = {};

  /// Rx buffer
  srslte::mac_sch_pdu_nr rx_pdu;

  /// Tx buffer
  srslte::mac_sch_pdu_nr       tx_pdu;
  srslte::unique_byte_buffer_t tx_buffer     = nullptr;
  srslte::unique_byte_buffer_t rlc_buffer    = nullptr;
  srslte_softbuffer_tx_t       softbuffer_tx = {}; /// UL HARQ (temporal)

  srslte::task_multiqueue::queue_handle stack_task_dispatch_queue;

  // MAC Uplink-related procedures
  proc_ra_nr proc_ra;
  mux_nr     mux;
};

} // namespace srsue

#endif // SRSUE_MAC_NR_H
