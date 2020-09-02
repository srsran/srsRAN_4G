/*
 * Copyright 2013-2020 Software Radio Systems Limited
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
#include <srslte/adt/circular_array.h>
#include <srslte/interfaces/enb_interfaces.h>
#include <srslte/srslte.h>

namespace srsenb {

class phy_ue_db
{
private:
  /**
   * Primary serving cell configuration flow
   * ----------------------------------------
   * Initially UEs are created with a default configuration. When it receives a new configuration for a UE from the
   * stack, it does not apply the primary serving cell straightforward. The primary cell configuration is stashed and
   * not applied. However, some parameters are copied to the actual configuration order to allow PHY transmitting the
   * upper layers messages and receiving the messages before applying the full configuration.
   *
   * The stashed primary cell configuration is applied as soon as the stack indicates the completion of the
   * configuration procedure.
   *
   * +---------+ Stack set config +--------------+ Stack Config complete +--------------------+
   * | Default |----------------->| Stash config |---------------------->| Full configuration |
   * +---------+                  +--------------+                       +--------------------+
   *                                     ^                                          |
   *                                     |           User reconfiguration           |
   *                                     +------------------------------------------+
   *
   * Secondary serving cell configuration flow
   * ------------------------------------------
   * Secondary serving cell configuration uses the cell_state attribute for indicating whether the they have been
   * configured from the stack set and activated/deactivated.
   *
   * Initially the the state is default (none) and it goes to inactive as soon as it is configured from the stack, then
   * it can transition to active as soon as the stack indicates so.
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
    cell_state_none = 0,           ///< Uninitialized
    cell_state_primary,            ///< PCell
    cell_state_secondary_inactive, ///< Configured from RRC but not activated
    cell_state_secondary_active    ///< Configured and activated from MAC
  } cell_state_t;

  /**
   * Cell information for the UE database
   */
  typedef struct {
    cell_state_t                                     state      = cell_state_none; ///< Configuration state
    uint32_t                                         enb_cc_idx = 0;  ///< Corresponding eNb cell/carrier index
    uint8_t                                          last_ri    = 0;  ///< Last reported rank indicator
    srslte::circular_array<srslte_ra_tb_t, SRSLTE_MAX_HARQ_PROC> last_tb =
        {};                                                           ///< Stores last PUSCH Resource allocation
    srslte::phy_cfg_t                                phy_cfg;         ///< Configuration, it has a default constructor
    srslte::circular_array<bool, TTIMOD_SZ> is_grant_available;       ///< Indicates whether there is an available grant
  } cell_info_t;

  /**
   * UE object stored in the PHY common database
   */
  struct common_ue {
    srslte::circular_array<srslte_pdsch_ack_t, TTIMOD_SZ> pdsch_ack = {}; ///< Pending acknowledgements for this Cell
    std::array<cell_info_t, SRSLTE_MAX_CARRIERS>          cell_info = {}; ///< Cell information, indexed by ue_cell_idx
    srslte::phy_cfg_t                                     pcell_cfg_stash = {}; ///< Stashed Cell information
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
   * Helper method to set the constant attributes of a given RNTI after the configuration is set, it does not modify
   * internal states.
   *
   * @param rnti identifier of the UE (requires assertion prior to call)
   * @param phy_cfg points to the PHY configuration for a given cell/carrier
   */
  inline void _set_common_config_rnti(uint16_t rnti, srslte::phy_cfg_t& phy_cfg) const;

  /**
   * Gets the SCell index for a given RNTI and a eNb cell/carrier. It returns the SCell index (0 if PCell) if the cc_idx
   * is found among the configured cells/carriers. Otherwise, it returns SRSLTE_MAX_CARRIERS.
   *
   * @param rnti identifier of the UE (requires assertion prior to call)
   * @param enb_cc_idx the eNb cell/carrier index to look for in the RNTI.
   * @return the SCell index as described above.
   */
  inline uint32_t _get_ue_cc_idx(uint16_t rnti, uint32_t enb_cc_idx) const;

  /**
   * Gets the eNb Cell/Carrier index in which the UCI shall be carried. This corresponds to the serving cell with lowest
   * index that has an UL grant available.
   *
   * If no grant is available in the indicated TTI, it returns the number of the eNb Cells/Carriers.
   *
   * @param tti The UL processing TTI
   * @param rnti Temporal UE ID
   * @return the eNb Cell/Carrier with lowest serving cell index that has an UL grant
   */
  uint32_t _get_uci_enb_cc_idx(uint32_t tti, uint16_t rnti) const;

  /**
   * Checks if a given RNTI exists in the database
   * @param rnti provides UE identifier
   * @return SRSLTE_SUCCESS if the indicated RNTI exists, otherwise it returns SRSLTE_ERROR
   */
  inline int _assert_rnti(uint16_t rnti) const;

  /**
   * Checks if an RNTI is configured to use an specified eNb cell/carrier as PCell or SCell
   * @param rnti provides UE identifier
   * @param enb_cc_idx provides eNb cell/carrier
   * @return SRSLTE_SUCCESS if the indicated RNTI exists, otherwise it returns SRSLTE_ERROR
   */
  inline int _assert_enb_cc(uint16_t rnti, uint32_t enb_cc_idx) const;

  /**
   * Checks if an RNTI uses a given eNb cell/carrier as PCell
   * @param rnti provides UE identifier
   * @param enb_cc_idx provides eNb cell/carrier index
   * @return SRSLTE_SUCCESS if the indicated eNb cell/carrier of the RNTI is a PCell, otherwise it returns SRSLTE_ERROR
   */
  inline int _assert_enb_pcell(uint16_t rnti, uint32_t enb_cc_idx) const;

  /**
   * Checks if an RNTI is configured to use an specified UE cell/carrier as PCell or SCell
   * @param rnti provides UE identifier
   * @param ue_cc_idx UE cell/carrier index that is asserted
   * @return SRSLTE_SUCCESS if the indicated cell/carrier index is valid, otherwise it returns SRSLTE_ERROR
   */
  inline int _assert_ue_cc(uint16_t rnti, uint32_t ue_cc_idx) const;

  /**
   * Checks if an RNTI is configured to use an specified eNb cell/carrier as PCell or SCell and it is active
   * @param rnti provides UE identifier
   * @param enb_cc_idx UE cell/carrier index that is asserted
   * @return SRSLTE_SUCCESS if the indicated eNb cell/carrier is active, otherwise it returns SRSLTE_ERROR
   */
  inline int _assert_active_enb_cc(uint16_t rnti, uint32_t enb_cc_idx) const;

  /**
   * Internal eNb stack assertion
   * @return SRSLTE_SUCCESS if available, otherwise it returns SRSLTE_ERROR
   */
  inline int _assert_stack() const;

  /**
   * Internal eNb Cell list assertion
   * @return SRSLTE_SUCCESS if available, otherwise it returns SRSLTE_ERROR
   */
  inline int _assert_cell_list_cfg() const;

  /**
   * Internal eNb general configuration getter, returns default configuration if the UE does not exist in the given cell
   *
   * @param rnti provides UE identifier
   * @param enb_cc_idx eNb cell index
   * @param stashed if it is true, it returns the stashed configuration. Otherwise, it return the current configuration.
   * @return The PHY configuration of the indicated UE for the indicated eNb carrier/call index.
   */
  inline srslte::phy_cfg_t _get_rnti_config(uint16_t rnti, uint32_t enb_cc_idx, bool stashed) const;

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
   * @param phy_cfg_list List of the eNb physical layer configuration coming for the RRC
   */
  void addmod_rnti(uint16_t rnti, const phy_interface_rrc_lte::phy_rrc_cfg_list_t& phy_cfg_list);

  /**
   * Removes a whole UE entry from the UE database
   *
   * @param rnti identifier of the UE
   */
  void rem_rnti(uint16_t rnti);

  /**
   * Stack callback for indicating the completion of the configuration process and apply the stashed configuration in
   * the primary cell.
   *
   * @param rnti identifier of the user
   */
  void complete_config(uint16_t rnti);

  /**
   * Activates or deactivates configured secondary cells for a given RNTI and SCell index (UE SCell index), index 0 is
   * reserved for primary cell
   * @param rnti identifier of the UE
   * @param scell_idx
   * @param activate
   */
  void activate_deactivate_scell(uint16_t rnti, uint32_t ue_cc_idx, bool activate);

  /**
   * Asserts a given eNb cell is PCell of the given RNTI
   * @param rnti identifier of the UE
   * @param enb_cc_idx eNb cell/carrier index
   * @return It returns true if it is the primmary cell, othwerwise it returns false
   */
  bool is_pcell(uint16_t rnti, uint32_t enb_cc_idx) const;

  /**
   * Get the current down-link physical layer configuration for an RNTI and an eNb cell/carrier
   *
   * @param rnti identifier of the UE
   * @param cc_idx the eNb cell/carrier identifier
   */
  srslte_dl_cfg_t get_dl_config(uint16_t rnti, uint32_t enb_cc_idx) const;

  /**
   * Get the current DCI configuration for PDSCH physical layer configuration for an RNTI and an eNb cell/carrier
   *
   * @param rnti identifier of the UE
   * @param cc_idx the eNb cell/carrier identifier
   */
  srslte_dci_cfg_t get_dci_dl_config(uint16_t rnti, uint32_t enb_cc_idx) const;

  /**
   * Get the current PUCCH physical layer configuration for an RNTI and an eNb cell/carrier.
   *
   * @param rnti identifier of the UE
   * @param cc_idx the eNb cell/carrier identifier
   */
  srslte_ul_cfg_t get_ul_config(uint16_t rnti, uint32_t enb_cc_idx) const;

  /**
   * Get the current DCI configuration for PUSCH physical layer configuration for an RNTI and an eNb cell/carrier
   *
   * @param rnti identifier of the UE
   * @param cc_idx the eNb cell/carrier identifier
   */
  srslte_dci_cfg_t get_dci_ul_config(uint16_t rnti, uint32_t enb_cc_idx) const;

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
  void set_ack_pending(uint32_t tti, uint32_t enb_cc_idx, const srslte_dci_dl_t& dci);

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
                    uint32_t          enb_cc_idx,
                    uint16_t          rnti,
                    bool              aperiodic_cqi_request,
                    bool              is_pusch_available,
                    srslte_uci_cfg_t& uci_cfg);

  /**
   * Sends the decoded Uplink Control Information by PUCCH or PUSCH to MAC
   * @param tti the current TTI
   * @param rnti is the UE identifier
   * @param uci_cfg is the UCI configuration
   * @param uci_value is the UCI received value
   */
  void send_uci_data(uint32_t                  tti,
                     uint16_t                  rnti,
                     uint32_t                  enb_cc_idx,
                     const srslte_uci_cfg_t&   uci_cfg,
                     const srslte_uci_value_t& uci_value);

  /**
   * Set the latest UL Transport Block resource allocation for a given RNTI, eNb cell/carrier and UL HARQ process
   * identifier.
   *
   * @param rnti the UE temporal ID
   * @param enb_cc_idx the cell/carrier origin of the transmission
   * @param pid HARQ process identifier
   * @param tb the Resource Allocation for the PUSCH transport block
   */
  void set_last_ul_tb(uint16_t rnti, uint32_t enb_cc_idx, uint32_t pid, srslte_ra_tb_t tb);

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
  srslte_ra_tb_t get_last_ul_tb(uint16_t rnti, uint32_t enb_cc_idx, uint32_t pid) const;

  /**
   * Flags to true the UL grant available for a given TTI, RNTI and eNb cell/carrier index
   * @param tti the current TTI
   * @param rnti
   * @param enb_cc_idx
   */
  void set_ul_grant_available(uint32_t tti, const stack_interface_phy_lte::ul_sched_list_t& ul_sched_list);
};

} // namespace srsenb
#endif // SRSENB_PHY_UE_DB_H_
