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

#ifndef SRSLTE_UE_NR_INTERFACES_H
#define SRSLTE_UE_NR_INTERFACES_H

#include "srslte/common/interfaces_common.h"
#include "srslte/interfaces/mac_interface_types.h"
#include "srslte/interfaces/rrc_nr_interface_types.h"
#include <array>
#include <string>

namespace srsue {

class rrc_interface_phy_nr
{
public:
  virtual void in_sync()                   = 0;
  virtual void out_of_sync()               = 0;
  virtual void run_tti(const uint32_t tti) = 0;
};

class mac_interface_phy_nr
{
public:
  typedef struct {
    srslte::unique_byte_buffer_t tb[SRSLTE_MAX_TB];
    uint32_t                     pid;
    uint16_t                     rnti;
    uint32_t                     tti;
  } mac_nr_grant_dl_t;

  typedef struct {
    uint32_t pid;
    uint16_t rnti;
    uint32_t tti;
    uint32_t tbs;
  } mac_nr_grant_ul_t;

  virtual int sf_indication(const uint32_t tti) = 0; ///< FIXME: rename to slot indication

  // Query the MAC for the current RNTI to look for
  struct sched_rnti_t {
    uint16_t           id;
    srslte_rnti_type_t type;
  };
  virtual sched_rnti_t get_dl_sched_rnti_nr(const uint32_t tti) = 0;
  virtual sched_rnti_t get_ul_sched_rnti_nr(const uint32_t tti) = 0;

  /// Indicate succussfully received TB to MAC. The TB buffer is allocated in the PHY and handed as unique_ptr to MAC
  virtual void tb_decoded(const uint32_t cc_idx, mac_nr_grant_dl_t& grant) = 0;

  /// Indicate reception of UL grant (only TBS is provided). Buffer for resulting MAC PDU is provided by MAC and is
  /// passed as pointer to PHY during tx_reuqest
  virtual void
  new_grant_ul(const uint32_t cc_idx, const mac_nr_grant_ul_t& grant, srslte::unique_byte_buffer_t phy_tx_pdu) = 0;

  /**
   * @brief Indicate the successful transmission of a PRACH.
   * @param tti  The TTI from the PHY viewpoint at which the PRACH was sent over-the-air (not to the radio).
   * @param s_id The index of the first OFDM symbol of the specified PRACH (0 <= s_id < 14).
   * @param t_id The index of the first slot of the specified PRACH (0 <= t_id < 80).
   * @param f_id The index of the specified PRACH in the frequency domain (0 <= f_id < 8).
   * @param ul_carrier_id The UL carrier used for Msg1 transmission (0 for NUL carrier, and 1 for SUL carrier).
   */
  virtual void prach_sent(uint32_t tti, uint32_t s_id, uint32_t t_id, uint32_t f_id, uint32_t ul_carrier_id) = 0;
};

class mac_interface_rrc_nr
{
public:
  virtual void setup_lcid(const srslte::logical_channel_config_t& config) = 0;
  virtual void set_config(const srslte::bsr_cfg_t& bsr_cfg)               = 0;
  virtual void set_config(const srslte::sr_cfg_t& sr_cfg)                 = 0;
  virtual void set_config(const srslte::rach_nr_cfg_t& rach_cfg)          = 0;

  // RRC triggers MAC ra procedure
  virtual void start_ra_procedure() = 0;

  // RRC informs MAC about the (randomly) selected ID used for contention-based RA
  virtual void set_contention_id(const uint64_t ue_identity) = 0;

  // RRC informs MAC about new UE identity for contention-free RA
  virtual bool set_crnti(const uint16_t crnti) = 0;
};

struct phy_args_nr_t {
  uint32_t               nof_carriers;
  uint32_t               nof_prb;
  uint32_t               nof_phy_threads;
  uint32_t               worker_cpu_mask;
  srslte::phy_log_args_t log;
  srslte_ue_dl_nr_args_t dl;
  srslte_ue_ul_nr_args_t ul;

  phy_args_nr_t()
  {
    dl.nof_rx_antennas        = 1;
    dl.nof_max_prb            = 100;
    dl.pdsch.measure_evm      = true;
    dl.pdsch.measure_time     = true;
    dl.pdsch.sch.disable_simd = false;
    ul.nof_max_prb            = 100;
    ul.pusch.measure_time     = true;
    ul.pusch.sch.disable_simd = false;
  }
};

class phy_interface_mac_nr
{
public:
  typedef struct {
    uint32_t tti;
    uint32_t tb_len;
    uint8_t* data; // always a pointer in our case
  } tx_request_t;

  // MAC informs PHY about UL grant included in RAR PDU
  virtual int set_ul_grant(std::array<uint8_t, SRSLTE_RAR_UL_GRANT_NBITS> packed_ul_grant,
                           uint16_t                                       rnti,
                           srslte_rnti_type_t                             rnti_type) = 0;

  // MAC instructs PHY to transmit MAC TB at the given TTI
  virtual int tx_request(const tx_request_t& request) = 0;

  /// Instruct PHY to send PRACH in the next occasion.
  virtual void send_prach(const uint32_t prach_occasion,
                          const int      preamble_index,
                          const float    preamble_received_target_power,
                          const float    ta_base_sec = 0.0f) = 0;
};

class phy_interface_rrc_nr
{
public:
  virtual bool set_config(const srslte::phy_cfg_nr_t& cfg) = 0;
};

// Combined interface for PHY to access stack (MAC and RRC)
class stack_interface_phy_nr : public mac_interface_phy_nr,
                               public rrc_interface_phy_nr,
                               public srslte::stack_interface_phy_nr
{};

// Combined interface for stack (MAC and RRC) to access PHY
class phy_interface_stack_nr : public phy_interface_mac_nr, public phy_interface_rrc_nr
{};

} // namespace srsue

#endif // SRSLTE_UE_NR_INTERFACES_H
