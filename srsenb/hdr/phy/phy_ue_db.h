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

#ifndef SRSENB_PHY_UE_DB_H_
#define SRSENB_PHY_UE_DB_H_

#include "phy_interfaces.h"
#include <map>
#include <mutex>
#include <srslte/interfaces/enb_interfaces.h>
#include <srslte/srslte.h>

namespace srsenb {

class phy_ue_db
{
private:
  /**
   * SCell Configuration state indicates whether the SCell has been configured from the RRC set and activated from the
   * MAC layer.
   *
   * Initially the the state is default and it goes to deactivated as soon as it is configured from RRC, then it can
   * transition to active as soon as the MAC indicates so.
   *
   *  +---------+   Set SCell Configuration   +-------------+   SCell activation   +--------+
   *  | Default | --------------------------->| Deactivated |--------------------->| Active |
   *  +---------+                             +-------------+                      +--------+
   *     ^ ^                                         ^                                 | |
   *     | |                                         |                                 | |
   *   --+ |                                         |         SCell deactivation      | |
   *       |                                         +---------------------------------+ |
   *       |                                                   Reconfigure SCell         |
   *       |       Remove SCell configuration                                            |
   *       +-----------------------------------------------------------------------------+
   */
  typedef enum {
    scell_state_default = 0, ///< Default values, not configured, not active
    scell_state_deactivated, ///< Configured from RRC but not activated
    scell_state_active       ///< PCell or activated from MAC
  } scell_state_t;

  /**
   * SCell information for the UE database
   */
  typedef struct {
    scell_state_t     state                         = scell_state_default; ///< Configuration state
    uint32_t          cc_idx                        = 0;                   ///< Corresponding eNb cell/carrier index
    uint8_t           last_ri                       = 0;                   ///< Last reported rank indicator
    srslte_ra_tb_t    last_tb[SRSLTE_MAX_HARQ_PROC] = {};                  ///< Stores last PUSCH Resource allocation
    srslte::phy_cfg_t phy_cfg; ///< Configuration, it has a default constructor
  } scell_info_t;

  /**
   * UE object stored in the PHY common database
   */
  struct common_ue {
    srslte_pdsch_ack_t pdsch_ack[TTIMOD_SZ] = {};       ///< Pending acknowledgements for this SCell
    scell_info_t       scell_info[SRSLTE_MAX_CARRIERS]; ///< SCell information, indexed by scell_idx
  };

  /**
   * UE database indexed by RNTI
   */
  std::map<uint16_t, common_ue> ue_db;

  /**
   * Concurrency protection mutex, allowed modifications from const methods.
   */
  mutable std::mutex mutex;

  /**
   * Stack interface
   */
  stack_interface_phy_lte* stack = nullptr;

  /**
   * PHY arguments pointer, used for loading the common UE parameters when a new configuration is set
   */
  const phy_args_t* phy_args = {};

  /**
   * Cell list
   */
  const phy_cell_cfg_list_t* cell_cfg_list = nullptr;

  /**
   * Internal RNTI addition, it is not thread safe protected
   *
   * @param rnti identifier of the UE
   */
  inline void _add_rnti(uint16_t rnti);

  /**
   * Internal pending ACK clear for a given RNTI and TTI, it is not thread safe protected
   *
   * @param tti is the given TTI (requires assertion prior to call)
   * @param rnti identifier of the UE (requires assertion prior to call)
   */
  inline void _clear_tti_pending_rnti(uint32_t tti, uint16_t rnti);

  /**
   * Helper method to set the constant attributes of a given RNTI after the configuration is set
   *
   * @param rnti identifier of the UE (requires assertion prior to call)
   */
  inline void _set_common_config_rnti(uint16_t rnti);

  /**
   * Gets the SCell index for a given RNTI and a eNb cell/carrier. It returns the SCell index (0 if PCell) if the cc_idx
   * is found among the configured cells/carriers. Otherwise, it returns SRSLTE_MAX_CARRIERS.
   *
   * @param rnti identifier of the UE (requires assertion prior to call)
   * @param cc_idx the eNb cell/carrier index to look for in the RNTI.
   * @return the SCell index as described above.
   */
  inline uint32_t _get_scell_idx(uint16_t rnti, uint32_t cc_idx) const;

public:
  /**
   * Initialises the UE database with the stack and cell list
   * @param stack_ptr points to the stack (read/write)
   * @param cell_cfg_list_ points to the cell configuration list (read only)
   */
  void init(stack_interface_phy_lte* stack_ptr, const phy_args_t& phy_args_, const phy_cell_cfg_list_t& cell_cfg_list_);

  /**
   * Adds or modifies a user in the UE database setting. This function requires the physical layer configuration coming
   * from the RRC in order to extract cross carrier information such as channel quality reporting configuration. The
   * first element of the list must be the PCell and the rest will be SCell in the order
   *
   * @param rnti identifier of the user
   * @param phy_rrc_dedicated_list List of the eNb physical layer configuration coming for the RRC
   */
  void addmod_rnti(uint16_t rnti, const phy_interface_rrc_lte::phy_rrc_dedicated_list_t& phy_rrc_dedicated_list);

  /**
   * Removes a whole UE entry from the UE database
   *
   * @param rnti identifier of the UE
   */
  void rem_rnti(uint16_t rnti);

  /**
   * Activates or deactivates configured SCells for a given RNTI and SCell index (UE SCell index), index 0 is reserved
   * for PCell
   * @param rnti identifier of the UE
   * @param scell_idx
   * @param activate
   */
  void activate_deactivate_scell(uint16_t rnti, uint32_t scell_idx, bool activate);

  /**
   * Get the current physical layer configuration for an RNTI and an eNb cell/carrier
   *
   * @param rnti identifier of the UE
   * @param cc_idx the eNb cell/carrier identifier
   */
  srslte::phy_cfg_t get_config(uint16_t rnti, uint32_t cc_idx) const;

  /**
   * Removes all the pending ACKs of all the RNTIs for a given TTI
   *
   * @param tti is the given TTI to clear
   */
  void clear_tti_pending_ack(uint32_t tti);

  /**
   * Sets the pending ACK for a given TTI in a given Component Carrier and user (RNTI is a member of the DCI)
   *
   * @param tti is the given TTI to fill
   * @param cc_idx the carrier where the DCI is scheduled
   * @param dci carries the Transport Block and required scheduling information
   *
   */
  void set_ack_pending(uint32_t tti, uint32_t cc_idx, const srslte_dci_dl_t& dci);

  /**
   * Fills the Uplink Control Information (UCI) configuration and returns true/false idicating if UCI bits are required.
   * @param tti the current UL reception TTI
   * @param cc_idx the eNb cell/carrier where the UL receiption is happening
   * @param rnti is the UE identifier
   * @param aperiodic_cqi_request indicates if aperiodic CQI was requested
   * @param uci_cfg brings the UCI configuration
   * @return true if UCI decoding is required and false otherwise
   */
  bool fill_uci_cfg(uint32_t          tti,
                    uint32_t          cc_idx,
                    uint16_t          rnti,
                    bool              aperiodic_cqi_request,
                    srslte_uci_cfg_t& uci_cfg) const;

  /**
   * Sends the decoded Uplink Control Information by PUCCH or PUSCH to MAC
   * @param tti the current TTI
   * @param rnti is the UE identifier
   * @param uci_cfg is the UCI configuration
   * @param uci_value is the UCI received value
   */
  void send_uci_data(uint32_t                  tti,
                     uint16_t                  rnti,
                     uint32_t                  cc_idx,
                     const srslte_uci_cfg_t&   uci_cfg,
                     const srslte_uci_value_t& uci_value);

  /**
   * Set the latest UL Transport Block resource allocation for a given RNTI, eNb cell/carrier and UL HARQ process
   * identifier.
   *
   * @param rnti the UE temporal ID
   * @param cc_idx the cell/carrier origin of the transmission
   * @param pid HARQ process identifier
   * @param tb the Resource Allocation for the PUSCH transport block
   */
  void set_last_ul_tb(uint16_t rnti, uint32_t cc_idx, uint32_t pid, srslte_ra_tb_t tb);

  /**
   * Get the latest UL Transport Block resource allocation for a given RNTI, eNb cell/carrier and UL HARQ process
   * identifier. It returns the resource allocation if the RNTI and cell/eNb are valid, otherwise it will return an
   * default Resource allocation (all zeros by default).
   *
   * @param rnti the UE temporal ID
   * @param cc_idx the cell/carrier origin of the transmission
   * @param pid HARQ process identifier
   * @return the Resource Allocation for the PUSCH transport block
   */
  srslte_ra_tb_t get_last_ul_tb(uint16_t rnti, uint32_t cc_idx, uint32_t pid) const;
};

} // namespace srsenb
#endif // SRSENB_PHY_UE_DB_H_
