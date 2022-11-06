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

#ifndef SRSUE_LTE_CC_WORKER_H
#define SRSUE_LTE_CC_WORKER_H

#include "srsran/interfaces/ue_interfaces.h"
#include "srsran/srsran.h"
#include "srsue/hdr/phy/phy_common.h"

namespace srsue {
namespace lte {

class cc_worker
{
public:
  cc_worker(uint32_t cc_idx, uint32_t max_prb, phy_common* phy, srslog::basic_logger& logger);
  ~cc_worker();

  /* Functions used by main PHY thread */
  cf_t*    get_rx_buffer(uint32_t antenna_idx);
  cf_t*    get_tx_buffer(uint32_t antenna_idx);
  uint32_t get_buffer_len();

  void  set_tti(uint32_t tti);
  void  set_cfo_nolock(float cfo);
  float get_ref_cfo() const;

  // Functions to set configuration.
  // Warning: all these functions are unlocked and must be called while the worker is not processing data
  void reset_cell_nolock();
  bool set_cell_nolock(srsran_cell_t cell_);
  void set_tdd_config_nolock(srsran_tdd_config_t config);
  void set_config_nolock(const srsran::phy_cfg_t& phy_cfg);
  void upd_config_dci_nolock(const srsran_dci_cfg_t& dci_cfg);

  void set_uci_periodic_cqi(srsran_uci_data_t* uci_data);

  bool work_dl_regular();
  bool work_dl_mbsfn(srsran_mbsfn_cfg_t mbsfn_cfg);
  bool work_ul(srsran_uci_data_t* uci_data);

  int read_ce_abs(float* ce_abs, uint32_t tx_antenna, uint32_t rx_antenna);
  int read_pdsch_d(cf_t* pdsch_d);

  void update_measurements(std::vector<phy_meas_t>& serving_cells, cf_t* rssi_power_buffer = nullptr);

private:
  void reset();

  void dl_phy_to_mac_grant(srsran_pdsch_grant_t*                  phy_grant,
                           srsran_dci_dl_t*                       dl_dci,
                           mac_interface_phy_lte::mac_grant_dl_t* mac_grant);
  void ul_phy_to_mac_grant(srsran_pusch_grant_t*                  phy_grant,
                           srsran_dci_ul_t*                       ul_dci,
                           uint32_t                               pid,
                           bool                                   ul_grant_available,
                           mac_interface_phy_lte::mac_grant_ul_t* mac_grant);

  /* Methods for DL... */
  int decode_pdcch_ul();
  int decode_pdcch_dl();

  void decode_phich();
  int  decode_pdsch(srsran_pdsch_ack_resource_t            ack_resource,
                    mac_interface_phy_lte::tb_action_dl_t* action,
                    bool                                   acks[SRSRAN_MAX_CODEWORDS]);
  int  decode_pmch(mac_interface_phy_lte::tb_action_dl_t* action, srsran_mbsfn_cfg_t* mbsfn_cfg);
  void new_mch_dl(mac_interface_phy_lte::tb_action_dl_t*);
  /* Methods for UL */
  bool     encode_uplink(mac_interface_phy_lte::tb_action_ul_t* action, srsran_uci_data_t* uci_data);
  void     set_uci_sr(srsran_uci_data_t* uci_data);
  void     set_uci_aperiodic_cqi(srsran_uci_data_t* uci_data);
  void     set_uci_ack(srsran_uci_data_t* uci_data, bool is_grant_available, uint32_t dai_ul, bool is_pusch_available);
  uint32_t get_wideband_cqi();

  /* Common objects */
  phy_common*           phy = nullptr;
  srslog::basic_logger& logger;

  srsran_cell_t      cell      = {};
  srsran_dl_sf_cfg_t sf_cfg_dl = {};
  srsran_ul_sf_cfg_t sf_cfg_ul = {};

  uint32_t               cc_idx                             = 0;
  bool                   cell_initiated                     = false;
  cf_t*                  signal_buffer_rx[SRSRAN_MAX_PORTS] = {};
  cf_t*                  signal_buffer_tx[SRSRAN_MAX_PORTS] = {};
  uint32_t               signal_buffer_max_samples          = 0;
  const static uint32_t  mch_payload_buffer_sz              = SRSRAN_MAX_BUFFER_SIZE_BYTES;
  uint8_t                mch_payload_buffer[mch_payload_buffer_sz];
  srsran_softbuffer_rx_t mch_softbuffer = {};

  /* Objects for DL */
  srsran_ue_dl_t     ue_dl     = {};
  srsran_ue_dl_cfg_t ue_dl_cfg = {};
  srsran_pmch_cfg_t  pmch_cfg  = {};

  srsran_chest_dl_cfg_t chest_mbsfn_cfg   = {};
  srsran_chest_dl_cfg_t chest_default_cfg = {};

  /* Objects for UL */
  srsran_ue_ul_t     ue_ul     = {};
  srsran_ue_ul_cfg_t ue_ul_cfg = {};
};

} // namespace lte
} // namespace srsue

#endif // SRSUE_LTE_CC_WORKER_H
