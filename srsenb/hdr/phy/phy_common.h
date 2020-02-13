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

#ifndef SRSENB_PHCH_COMMON_H
#define SRSENB_PHCH_COMMON_H

#include "phy_interfaces.h"
#include "srslte/common/gen_mch_tables.h"
#include "srslte/common/interfaces_common.h"
#include "srslte/common/log.h"
#include "srslte/common/thread_pool.h"
#include "srslte/common/threads.h"
#include "srslte/interfaces/common_interfaces.h"
#include "srslte/interfaces/enb_interfaces.h"
#include "srslte/interfaces/enb_metrics_interface.h"
#include "srslte/phy/channel/channel.h"
#include "srslte/radio/radio.h"
#include <map>
#include <semaphore.h>
#include <string.h>

namespace srsenb {

class phy_common
{
public:
  phy_common(uint32_t nof_workers);
  ~phy_common();

  void set_nof_workers(uint32_t nof_workers);

  bool
       init(const phy_cell_cfg_list_t& cell_list_, srslte::radio_interface_phy* radio_handler, stack_interface_phy_lte* mac);
  void reset();
  void stop();

  void
  worker_end(uint32_t tx_mutex_cnt, cf_t* buffer[SRSLTE_MAX_PORTS], uint32_t nof_samples, srslte_timestamp_t tx_time);

  // Common objects
  phy_args_t params = {};

  uint32_t get_nof_carriers() { return static_cast<uint32_t>(cell_list.size()); };
  uint32_t get_nof_prb(uint32_t cc_idx)
  {
    uint32_t ret = 0;

    if (cc_idx < cell_list.size()) {
      ret = cell_list[cc_idx].cell.nof_prb;
    }

    return ret;
  };
  uint32_t get_nof_ports(uint32_t cc_idx)
  {
    uint32_t ret = 0;

    if (cc_idx < cell_list.size()) {
      ret = cell_list[cc_idx].cell.nof_ports;
    }

    return ret;
  };
  float get_ul_freq_hz(uint32_t cc_idx)
  {
    float ret = 0.0f;

    if (cc_idx < cell_list.size()) {
      ret = cell_list[cc_idx].ul_freq_hz;
    }

    return ret;
  };
  float get_dl_freq_hz(uint32_t cc_idx)
  {
    float ret = 0.0f;

    if (cc_idx < cell_list.size()) {
      ret = cell_list[cc_idx].dl_freq_hz;
    }

    return ret;
  };
  uint32_t get_rf_port(uint32_t cc_idx)
  {
    uint32_t ret = 0;

    if (cc_idx < cell_list.size()) {
      ret = cell_list[cc_idx].rf_port;
    }

    return ret;
  };
  srslte_cell_t get_cell(uint32_t cc_idx)
  {
    srslte_cell_t c = {};
    if (cc_idx < cell_list.size()) {
      c = cell_list[cc_idx].cell;
    }
    return c;
  };

  // Physical Uplink Config common
  srslte_ul_cfg_t ul_cfg_com = {};

  // Physical Downlink Config common
  srslte_dl_cfg_t dl_cfg_com = {};

  srslte::radio_interface_phy* radio      = nullptr;
  stack_interface_phy_lte*     stack      = nullptr;
  srslte::channel_ptr          dl_channel = nullptr;

  // Common objects for schedulign grants
  stack_interface_phy_lte::ul_sched_list_t ul_grants[TTIMOD_SZ] = {};
  stack_interface_phy_lte::dl_sched_list_t dl_grants[TTIMOD_SZ] = {};

  // Map of pending ACKs for each user
  typedef struct {
    srslte_uci_cfg_ack_t ack[SRSLTE_MAX_CARRIERS];
  } pending_ack_t;

  class common_ue
  {
  public:
    pending_ack_t                pending_ack[TTIMOD_SZ]        = {};
    uint8_t                      ri                            = 0;
    uint32_t                     pcell_idx                     = 0;
    srslte_ra_tb_t               last_tb[SRSLTE_MAX_HARQ_PROC] = {};
    std::map<uint32_t, uint32_t> scell_map;
  };

  std::map<uint16_t, common_ue> common_ue_db;

  /**
   * Adds or modifies a user in the UE database setting. This function requires a list of cells indexes for the UE. The
   * first element of the list must be the PCell and the rest will be SCell in the order
   *
   * @param rnti identifier of the user
   * @param cell_index_list List of the eNb cell indexes for carrier aggregation
   */
  void ue_db_addmod_rnti(uint16_t rnti, const std::vector<uint32_t>& cell_index_list);

  /**
   * Removes a whole UE entry from the UE database
   *
   * @param rnti identifier of the UE
   */
  void ue_db_rem_rnti(uint16_t rnti);

  /**
   * Removes all the pending ACKs of all the RNTIs for a given TTI
   *
   * @param tti is the given TTI to clear
   */
  void ue_db_clear_tti_pending_ack(uint32_t tti);

  /**
   * Sets the pending ACK for a given TTI in a given Component Carrier and user (RNTI is a member of the DCI)
   *
   * @param tti is the given TTI to fill
   * @param cc_idx the carrier where the DCI is scheduled
   * @param dci carries the Transport Block and required scheduling information
   *
   */
  void ue_db_set_ack_pending(uint32_t tti, uint32_t cc_idx, const srslte_dci_dl_t& dci);

  /**
   * Requests ACK information for a given RNTI that needs to acknowledge PDSCH transmissions in the cc_idx cell/carrier.
   *
   * @param tti is the given TTI to fill
   * @param cc_idx the carrier where the DCI is scheduled
   * @param rnti is the UE identifier
   * @param uci_cfg_ack vector pointing at the UCI configuration
   *
   */
  void ue_db_get_ack_pending(uint32_t             tti,
                             uint32_t             cc_idx,
                             uint16_t             rnti,
                             srslte_uci_cfg_ack_t uci_cfg_ack[SRSLTE_MAX_CARRIERS]);

  /**
   * Provides the number of aggregated cells for a given RNTI
   * @param rnti UE's RNTI
   * @return the number of aggregated cells if the RNTI exists, otherwise it returns 0
   */
  uint32_t ue_db_get_nof_ca_cells(uint16_t rnti);

  /**
   * Provides the PCell index of a given UE from its RNTI
   * @param rnti UE's RNTI
   * @return the index of the PCell if it exists, the number of cells otherwise
   */
  uint32_t ue_db_get_cc_pcell(uint16_t rnti);

  /**
   * Requests the eNb cell index of given RNTI from its scell_idx
   *
   * @param rnti the UE temporal ID
   * @param scell_idx the UE SCell index, use 0 for PCell index
   * @return the eNb cell index if found, the number of eNb cells otherwise
   *
   */
  uint32_t ue_db_get_cc_scell(uint16_t rnti, uint32_t scell_idx);

  /**
   *
   * @param rnti
   * @param ri
   */
  void ue_db_set_ri(uint16_t rnti, uint8_t ri);

  /**
   *
   * @param rnti
   * @return
   */
  uint8_t ue_db_get_ri(uint16_t rnti);

  void           ue_db_set_last_ul_tb(uint16_t rnti, uint32_t pid, srslte_ra_tb_t tb);
  srslte_ra_tb_t ue_db_get_last_ul_tb(uint16_t rnti, uint32_t pid);

  void configure_mbsfn(phy_interface_stack_lte::phy_cfg_mbsfn_t* cfg);
  void build_mch_table();
  void build_mcch_table();
  bool is_mbsfn_sf(srslte_mbsfn_cfg_t* cfg, uint32_t phy_tti);
  void set_mch_period_stop(uint32_t stop);

private:
  phy_cell_cfg_list_t cell_list;
  std::vector<sem_t>  tx_sem;
  bool                is_first_tx = false;

  uint32_t nof_workers = 0;
  uint32_t max_workers = 0;

  std::mutex user_mutex = {};

  bool                                     have_mtch_stop     = false;
  pthread_mutex_t                          mtch_mutex         = {};
  pthread_cond_t                           mtch_cvar          = {};
  phy_interface_stack_lte::phy_cfg_mbsfn_t mbsfn              = {};
  bool                                     sib13_configured   = false;
  bool                                     mcch_configured    = false;
  uint8_t                                  mch_table[40]      = {};
  uint8_t                                  mcch_table[10]     = {};
  uint32_t                                 mch_period_stop    = 0;
  uint8_t                                  mch_sf_idx_lut[10] = {};
  bool                                     is_mch_subframe(srslte_mbsfn_cfg_t* cfg, uint32_t phy_tti);
  bool                                     is_mcch_subframe(srslte_mbsfn_cfg_t* cfg, uint32_t phy_tti);

  void add_rnti(uint16_t rnti);
};

} // namespace srsenb

#endif // SRSENB_PHCH_COMMON_H
