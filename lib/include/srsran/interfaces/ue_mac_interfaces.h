/**
 * Copyright 2013-2022 Software Radio Systems Limited
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

#ifndef SRSRAN_UE_MAC_INTERFACES_H
#define SRSRAN_UE_MAC_INTERFACES_H

#include "mac_interface_types.h"

namespace srsue {

class mac_interface_phy_lte
{
public:
  typedef struct {
    uint32_t nof_mbsfn_services;
  } mac_phy_cfg_mbsfn_t;

  typedef struct {
    uint32_t tbs;
    bool     ndi;
    bool     ndi_present;
    int      rv;
  } mac_tb_t;

  typedef struct {
    mac_tb_t tb[SRSRAN_MAX_TB];
    uint32_t pid;
    uint16_t rnti;
    bool     is_sps_release;
    bool     is_pdcch_order;
    uint32_t preamble_idx;
    uint32_t prach_mask_idx;
    uint32_t tti;
  } mac_grant_dl_t;

  typedef struct {
    mac_tb_t tb;
    uint32_t pid;
    uint16_t rnti;
    bool     phich_available;
    bool     hi_value;
    bool     is_rar;
    uint32_t tti_tx;
  } mac_grant_ul_t;

  typedef struct {
    bool     enabled;
    uint32_t rv;
    uint8_t* payload;
    union {
      srsran_softbuffer_rx_t* rx;
      srsran_softbuffer_tx_t* tx;
    } softbuffer;
  } tb_action_t;

  typedef struct {
    tb_action_t tb[SRSRAN_MAX_TB];

    bool generate_ack;
  } tb_action_dl_t;

  typedef struct {
    tb_action_t tb;
    uint32_t    current_tx_nb;
    bool        expect_ack;
    bool        is_rar;
  } tb_action_ul_t;

  /* Query the MAC for the current RNTI to look for
   */
  virtual uint16_t get_dl_sched_rnti(uint32_t tti) = 0;
  virtual uint16_t get_ul_sched_rnti(uint32_t tti) = 0;

  /* Indicate reception of UL dci.
   * payload_ptr points to memory where MAC PDU must be written by MAC layer */
  virtual void new_grant_ul(uint32_t cc_idx, mac_grant_ul_t grant, tb_action_ul_t* action) = 0;

  /* Indicate reception of DL dci. */
  virtual void new_grant_dl(uint32_t cc_idx, mac_grant_dl_t grant, tb_action_dl_t* action) = 0;

  /* Indicate successful decoding of PDSCH AND PCH TB. */
  virtual void tb_decoded(uint32_t cc_idx, mac_grant_dl_t grant, bool ack[SRSRAN_MAX_CODEWORDS]) = 0;

  /* Indicate successful decoding of BCH TB through PBCH */
  virtual void bch_decoded_ok(uint32_t cc_idx, uint8_t* payload, uint32_t len) = 0;

  /* Indicate successful decoding of MCH TB through PMCH */
  virtual void mch_decoded(uint32_t len, bool crc, uint8_t* payload) = 0;

  /* Communicate the number of mbsfn services available  */
  virtual void set_mbsfn_config(uint32_t nof_mbsfn_services) = 0;
};

class mac_interface_rrc
{
public:
  /* Instructs the MAC to start receiving BCCH */
  virtual void bcch_start_rx(int si_window_start, int si_window_length) = 0;
  virtual void bcch_stop_rx()                                           = 0;

  /* Instructs the MAC to start receiving PCCH */
  virtual void pcch_start_rx() = 0;

  /* RRC configures a logical channel */
  virtual void setup_lcid(uint32_t lcid, uint32_t lcg, uint32_t priority, int PBR_x_tti, uint32_t BSD) = 0;

  /* Instructs the MAC to start receiving an MCH */
  virtual void mch_start_rx(uint32_t lcid) = 0;

  /* Set entire MAC config */
  virtual void set_config(srsran::mac_cfg_t& mac_cfg) = 0;

  /* Update SR config only for PUCCH release */
  virtual void set_config(srsran::sr_cfg_t& sr_cfg) = 0;

  virtual void set_rach_ded_cfg(uint32_t preamble_index, uint32_t prach_mask) = 0;

  virtual uint16_t get_crnti()                                      = 0;
  virtual void     set_contention_id(uint64_t uecri)                = 0;
  virtual void     set_ho_rnti(uint16_t crnti, uint16_t target_pci) = 0;

  virtual void reconfiguration(const uint32_t& cc_idx, const bool& enable) = 0;
  virtual void reset()                                                     = 0;
};

} // namespace srsue

#endif // SRSRAN_UE_MAC_INTERFACES_H
