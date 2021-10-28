/**
 * Copyright 2013-2021 Software Radio Systems Limited
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

#ifndef SRSRAN_UE_NR_INTERFACES_H
#define SRSRAN_UE_NR_INTERFACES_H

#include "srsran/common/interfaces_common.h"
#include "srsran/common/phy_cfg_nr.h"
#include "srsran/interfaces/mac_interface_types.h"
#include <array>
#include <set>
#include <string>

namespace srsue {

class rrc_interface_phy_nr
{
public:
  virtual void in_sync()                            = 0;
  virtual void out_of_sync()                        = 0;
  virtual void run_tti(const uint32_t tti)          = 0;
  virtual void set_phy_config_complete(bool status) = 0;
};

class mac_interface_phy_nr
{
public:
  /// For DL, PDU buffer is allocated and passed to MAC in tb_decoded()
  typedef struct {
    bool                    enabled;    /// Whether or not PHY should attempt to decode PDSCH
    srsran_softbuffer_rx_t* softbuffer; /// Pointer to softbuffer to use
  } tb_dl_t;

  /// Struct provided by MAC with all necessary information for PHY
  typedef struct {
    tb_dl_t tb; // only single TB in DL
  } tb_action_dl_t;

  typedef struct {
    uint16_t rnti;
    uint32_t tti;
    uint8_t  pid; // HARQ process ID
    uint8_t  rv;  // Redundancy Version
    uint8_t  ndi; // Raw new data indicator extracted from DCI
    uint32_t tbs; // Transport block size in Bytes
  } mac_nr_grant_dl_t;

  typedef struct {
    uint32_t                     rx_slot_idx; // Slot when DL TB has been decoded
    srsran::unique_byte_buffer_t payload;     // TB when decoded successfully, nullptr otherwise
    bool                         ack;         // HARQ information
  } tb_action_dl_result_t;

  // UL grant as conveyed between PHY and MAC
  typedef struct {
    uint16_t rnti;
    uint32_t tti;
    uint8_t  pid;          // HARQ process ID
    uint32_t tbs;          // transport block size in Bytes
    uint8_t  ndi;          // Raw new data indicator extracted from DCI
    uint8_t  rv;           // Redundancy Version
    bool     is_rar_grant; // True if grant comes from RAR
  } mac_nr_grant_ul_t;

  /// For UL, payload buffer remains in MAC
  typedef struct {
    bool                    enabled;
    uint32_t                rv;
    srsran::byte_buffer_t*  payload;
    srsran_softbuffer_tx_t* softbuffer;
  } tb_ul_t;

  /// Struct provided by MAC with all necessary information for PHY
  typedef struct {
    tb_ul_t tb; // only single TB in UL
  } tb_action_ul_t;

  virtual int sf_indication(const uint32_t tti) = 0; ///< TODO: rename to slot indication

  // Query the MAC for the current RNTI to look for
  struct sched_rnti_t {
    uint16_t           id;
    srsran_rnti_type_t type;
  };
  virtual sched_rnti_t get_dl_sched_rnti_nr(const uint32_t tti) = 0;
  virtual sched_rnti_t get_ul_sched_rnti_nr(const uint32_t tti) = 0;

  /**
   * @brief Indicate reception of DL grant to MAC
   *
   * The TB buffer is allocated in the PHY and handed as unique_ptr to MAC.
   *
   * @param cc_idx The carrier index on which the grant has been received
   * @param grant  Reference to the grant
   * @param action Pointer to the TB action to be filled by MAC
   */
  virtual void new_grant_dl(const uint32_t cc_idx, const mac_nr_grant_dl_t& grant, tb_action_dl_t* action) = 0;

  /**
   * Indicate decoding of PDSCH
   *
   * @param cc_idx The index of the carrier for which the PDSCH has been decoded
   * @param grant  The original DL grant
   * @param result Payload (if any) and ack information
   */
  virtual void tb_decoded(const uint32_t cc_idx, const mac_nr_grant_dl_t& grant, tb_action_dl_result_t result) = 0;

  /**
   * @brief Indicate reception of UL grant to MAC
   *
   * Buffer for resulting MAC PDU is provided and managed (owned) by MAC and is passed as pointer in ul_action
   *
   * @param cc_idx The carrier index on which the grant has been received
   * @param grant  Reference to the grant
   * @param action Pointer to the TB action to be filled by MAC
   */
  virtual void new_grant_ul(const uint32_t cc_idx, const mac_nr_grant_ul_t& grant, tb_action_ul_t* action) = 0;

  /**
   * @brief Indicate the successful transmission of a PRACH.
   * @param tti  The TTI from the PHY viewpoint at which the PRACH was sent over-the-air (not to the radio).
   * @param s_id The index of the first OFDM symbol of the specified PRACH (0 <= s_id < 14).
   * @param t_id The index of the first slot of the specified PRACH (0 <= t_id < 80).
   * @param f_id The index of the specified PRACH in the frequency domain (0 <= f_id < 8).
   * @param ul_carrier_id The UL carrier used for Msg1 transmission (0 for NUL carrier, and 1 for SUL carrier).
   */
  virtual void prach_sent(uint32_t tti, uint32_t s_id, uint32_t t_id, uint32_t f_id, uint32_t ul_carrier_id) = 0;

  /**
   * @brief Indicate a valid SR transmission occasion on the valid PUCCH resource for SR configured; and the SR
   * transmission occasion does not overlap with a measurement gap; and the PUCCH resource for the SR transmission
   * occasion does not overlap with a UL-SCH resource;
   * @param tti  The TTI from the PHY viewpoint at which the SR occasion was sent over-the-air (not to the radio).
   */
  virtual bool sr_opportunity(uint32_t tti, uint32_t sr_id, bool meas_gap, bool ul_sch_tx) = 0;
};

class mac_interface_rrc_nr
{
public:
  virtual void reset() = 0;
  // Config calls that return SRSRAN_SUCCESS or SRSRAN_ERROR
  virtual int  setup_lcid(const srsran::logical_channel_config_t& config) = 0;
  virtual int  set_config(const srsran::bsr_cfg_nr_t& bsr_cfg)            = 0;
  virtual int  set_config(const srsran::sr_cfg_nr_t& sr_cfg)              = 0;
  virtual int  set_config(const srsran::dl_harq_cfg_nr_t& dl_hrq_cfg)     = 0;
  virtual void set_config(const srsran::rach_nr_cfg_t& rach_cfg)          = 0;
  virtual int  add_tag_config(const srsran::tag_cfg_nr_t& tag_cfg)        = 0;
  virtual int  set_config(const srsran::phr_cfg_nr_t& phr_cfg)            = 0;
  virtual int  remove_tag_config(const uint32_t tag_id)                   = 0;

  // RRC triggers MAC ra procedure
  virtual void start_ra_procedure() = 0;

  // RRC informs MAC about the (randomly) selected ID used for contention-based RA
  virtual void set_contention_id(const uint64_t ue_identity) = 0;

  // RRC informs MAC about new UE identity for contention-free RA
  virtual bool set_crnti(const uint16_t crnti) = 0;
};

struct phy_args_nr_t {
  uint32_t               rf_channel_offset  = 0; ///< Specifies the RF channel the NR carrier shall fill
  uint32_t               nof_carriers       = 1;
  uint32_t               max_nof_prb        = 106;
  uint32_t               nof_phy_threads    = 3;
  uint32_t               worker_cpu_mask    = 0;
  srsran::phy_log_args_t log                = {};
  srsran_ue_dl_nr_args_t dl                 = {};
  srsran_ue_ul_nr_args_t ul                 = {};
  std::set<uint32_t>     fixed_sr           = {1};
  uint32_t               fix_wideband_cqi   = 15; // Set to a non-zero value for fixing the wide-band CQI report
  bool                   store_pdsch_ko     = false;
  float                  trs_epre_ema_alpha = 0.1f; ///< EPRE measurement exponential average alpha
  float                  trs_rsrp_ema_alpha = 0.1f; ///< RSRP measurement exponential average alpha
  float                  trs_sinr_ema_alpha = 0.1f; ///< SINR measurement exponential average alpha
  float                  trs_cfo_ema_alpha  = 0.1f; ///< RSRP measurement exponential average alpha
  bool                   enable_worker_cfo  = true; ///< Enable/Disable open loop CFO correction at the workers

  phy_args_nr_t()
  {
    dl.nof_rx_antennas        = 1;
    dl.nof_max_prb            = 106;
    dl.pdsch.max_prb          = 106;
    dl.pdsch.max_layers       = 1;
    dl.pdsch.measure_evm      = true;
    dl.pdsch.measure_time     = true;
    dl.pdsch.sch.disable_simd = false;
    dl.pdsch.sch.max_nof_iter = 10;
    ul.nof_max_prb            = 106;
    ul.pusch.max_prb          = 106;
    ul.pusch.max_layers       = 1;
    ul.pusch.measure_time     = true;
    ul.pusch.sch.disable_simd = false;

    // fixed_sr.insert(0); // Enable SR_id = 0 by default for testing purposes
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
  virtual int set_ul_grant(uint32_t                                       rar_slot_idx,
                           std::array<uint8_t, SRSRAN_RAR_UL_GRANT_NBITS> packed_ul_grant,
                           uint16_t                                       rnti,
                           srsran_rnti_type_t                             rnti_type) = 0;

  // MAC instructs PHY to transmit MAC TB at the given TTI
  virtual int tx_request(const tx_request_t& request) = 0;

  /// Instruct PHY to send PRACH in the next occasion.
  virtual void send_prach(const uint32_t prach_occasion,
                          const int      preamble_index,
                          const float    preamble_received_target_power,
                          const float    ta_base_sec = 0.0f) = 0;

  /**
   * @brief Query PHY if there is a valid PUCCH SR resource configured for a given SR identifier
   * @param sr_id SR identifier
   * @return True if there is a valid PUCCH resource configured
   */
  virtual bool has_valid_sr_resource(uint32_t sr_id) = 0;

  /**
   * @brief Clear any configured downlink assignments and uplink grants
   */
  virtual void clear_pending_grants() = 0;
};

class phy_interface_rrc_nr
{
public:
  virtual bool set_config(const srsran::phy_cfg_nr_t& cfg) = 0;
};

// Combined interface for PHY to access stack (MAC and RRC)
class stack_interface_phy_nr : public mac_interface_phy_nr,
                               public rrc_interface_phy_nr,
                               public srsran::stack_interface_phy_nr
{};

// Combined interface for stack (MAC and RRC) to access PHY
class phy_interface_stack_nr : public phy_interface_mac_nr, public phy_interface_rrc_nr
{};

} // namespace srsue

#endif // SRSRAN_UE_NR_INTERFACES_H
