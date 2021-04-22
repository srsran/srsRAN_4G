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

#ifndef SRSENB_PHY_UE_DB_H_
#define SRSENB_PHY_UE_DB_H_

#include "phy_interfaces.h"
#include "srsran/interfaces/enb_mac_interfaces.h"
#include "srsran/interfaces/enb_phy_interfaces.h"
#include <map>
#include <mutex>
#include <srsran/adt/circular_array.h>

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
   * Consider that once the secondary serving cell is configured, the state transition between active and deactivated
   * shall be performed by stack calls activation/deactivation and not re-configuration.
   *
   *  +---------+   Set SCell Configuration   +-------------+   SCell activation   +--------+
   *  | Default | --------------------------->| Deactivated |--------------------->| Active |
   *  +---------+                             +-------------+                      +--------+
   *     ^ ^                                         ^                                 | |
   *     | |                                         |                                 | |
   *   --+ |                                         |         SCell deactivation      | |
   *       |                                         +---------------------------------+ |
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
  struct cell_info_t {
    cell_state_t state      = cell_state_none; ///< Configuration state
    uint32_t     enb_cc_idx = 0;               ///< Corresponding eNb cell/carrier index
    uint8_t      last_ri    = 0;               ///< Last reported rank indicator
    srsran::circular_array<srsran_ra_tb_t, SRSRAN_MAX_HARQ_PROC> last_tb =
        {}; ///< Stores last PUSCH Resource allocation
    bool                                    stash_use_tbs_index_alt = false;
    srsran::phy_cfg_t                       phy_cfg;            ///< Configuration, it has a default constructor
    srsran::circular_array<bool, TTIMOD_SZ> is_grant_available; ///< Indicates whether there is an available grant
  };

  /**
   * UE object stored in the PHY common database
   */
  struct common_ue {
    bool                                                  stashed_multiple_csi_request_enabled = false;
    srsran::circular_array<srsran_pdsch_ack_t, TTIMOD_SZ> pdsch_ack = {}; ///< Pending acknowledgements for this Cell
    std::array<cell_info_t, SRSRAN_MAX_CARRIERS>          cell_info = {}; ///< Cell information, indexed by ue_cell_idx
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
   * @return SRSRAN_SUCCESS if the RNTI is not duplicated and is added successfully, SRSRAN_ERROR code if it exists
   */
  inline int _add_rnti(uint16_t rnti);

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
  inline void _set_common_config_rnti(uint16_t rnti, srsran::phy_cfg_t& phy_cfg) const;

  /**
   * Gets the SCell index for a given RNTI and a eNb cell/carrier. It returns the SCell index (0 if PCell) if the cc_idx
   * is found among the configured cells/carriers. Otherwise, it returns SRSRAN_MAX_CARRIERS.
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
   * @return SRSRAN_SUCCESS if the indicated RNTI exists, otherwise it returns SRSRAN_ERROR
   */
  inline int _assert_rnti(uint16_t rnti) const;

  /**
   * Checks if an RNTI is configured to use an specified eNb cell/carrier as PCell or SCell
   * @param rnti provides UE identifier
   * @param enb_cc_idx provides eNb cell/carrier
   * @return SRSRAN_SUCCESS if the indicated RNTI exists, otherwise it returns SRSRAN_ERROR
   */
  inline int _assert_enb_cc(uint16_t rnti, uint32_t enb_cc_idx) const;

  /**
   * Checks if an RNTI uses a given eNb cell/carrier as PCell
   * @param rnti provides UE identifier
   * @param enb_cc_idx provides eNb cell/carrier index
   * @return SRSRAN_SUCCESS if the indicated eNb cell/carrier of the RNTI is a PCell, otherwise it returns SRSRAN_ERROR
   */
  inline int _assert_enb_pcell(uint16_t rnti, uint32_t enb_cc_idx) const;

  /**
   * Checks if an RNTI is configured to use an specified UE cell/carrier as PCell or SCell
   * @param rnti provides UE identifier
   * @param ue_cc_idx UE cell/carrier index that is asserted
   * @return SRSRAN_SUCCESS if the indicated cell/carrier index is valid, otherwise it returns SRSRAN_ERROR
   */
  inline int _assert_ue_cc(uint16_t rnti, uint32_t ue_cc_idx) const;

  /**
   * Checks if an RNTI is configured to use an specified eNb cell/carrier as PCell or SCell and it is active
   * @param rnti provides UE identifier
   * @param enb_cc_idx UE cell/carrier index that is asserted
   * @return SRSRAN_SUCCESS if the indicated eNb cell/carrier is active, otherwise it returns SRSRAN_ERROR
   */
  inline int _assert_active_enb_cc(uint16_t rnti, uint32_t enb_cc_idx) const;

  /**
   * Internal eNb stack assertion
   * @return SRSRAN_SUCCESS if available, otherwise it returns SRSRAN_ERROR
   */
  inline int _assert_stack() const;

  /**
   * Internal eNb Cell list assertion
   * @return SRSRAN_SUCCESS if available, otherwise it returns SRSRAN_ERROR
   */
  inline int _assert_cell_list_cfg() const;

  /**
   * Internal eNb general configuration getter, returns default configuration if the UE does not exist in the given cell
   *
   * @param rnti provides UE identifier
   * @param enb_cc_idx eNb cell index
   * @param[out] phy_cfg The PHY configuration of the indicated UE for the indicated eNb carrier/call index.
   * @return SRSRAN_SUCCESS if provided context is correct, SRSRAN_ERROR code otherwise
   */
  inline int _get_rnti_config(uint16_t rnti, uint32_t enb_cc_idx, srsran::phy_cfg_t& phy_cfg) const;

  /**
   * Count number of configured secondary serving cells
   *
   * @param rnti provides UE identifier
   * @return The number of configured secondary cells
   */
  inline uint32_t _count_nof_configured_scell(uint16_t rnti);

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
   * @return SRSRAN_SUCCESS if provided RNTI exists, SRSRAN_ERROR code otherwise
   */
  int rem_rnti(uint16_t rnti);

  /**
   * Stack callback for indicating the completion of the configuration process and apply the stashed configuration in
   * the primary cell.
   *
   * @param rnti identifier of the user
   * @return SRSRAN_SUCCESS if provided RNTI exists, SRSRAN_ERROR code otherwise
   */
  int complete_config(uint16_t rnti);

  /**
   * Activates or deactivates configured secondary cells for a given RNTI and SCell index (UE SCell index), index 0 is
   * reserved for primary cell
   * @param rnti identifier of the UE
   * @param scell_idx
   * @param activate
   * @return SRSRAN_SUCCESS if provided RNTI exists in the given cell, SRSRAN_ERROR code otherwise
   */
  int activate_deactivate_scell(uint16_t rnti, uint32_t ue_cc_idx, bool activate);

  /**
   * Asserts a given eNb cell is PCell of the given RNTI
   * @param rnti identifier of the UE
   * @param enb_cc_idx eNb cell/carrier index
   * @return It returns true if it is the primary cell, otherwise it returns false
   */
  bool is_pcell(uint16_t rnti, uint32_t enb_cc_idx) const;

  /**
   * Asserts a given eNb cell is part of the given RNTI
   * @param rnti identifier of the UE
   * @param enb_cc_idx eNb cell/carrier index
   * @return It returns true if the cell is part of the UE, othwerwise it returns false
   */
  bool ue_has_cell(uint16_t rnti, uint32_t enb_cc_idx) const;

  /**
   * Get the current down-link physical layer configuration for an RNTI and an eNb cell/carrier
   *
   * @param rnti identifier of the UE
   * @param cc_idx the eNb cell/carrier identifier
   * @param[out] dl_cfg Current DL PHY configuration
   * @return SRSRAN_SUCCESS if provided RNTI exists in the given cell, SRSRAN_ERROR code otherwise
   */
  int get_dl_config(uint16_t rnti, uint32_t enb_cc_idx, srsran_dl_cfg_t& dl_cfg) const;

  /**
   * Get the current DCI configuration for PDSCH physical layer configuration for an RNTI and an eNb cell/carrier
   *
   * @param rnti identifier of the UE
   * @param cc_idx the eNb cell/carrier identifier
   * @param[out] dci_cfg Current DL-DCI configuration
   * @return SRSRAN_SUCCESS if provided RNTI exists in the given cell, SRSRAN_ERROR code otherwise
   */
  int get_dci_dl_config(uint16_t rnti, uint32_t enb_cc_idx, srsran_dci_cfg_t& dci_cfg) const;

  /**
   * Get the current PUCCH physical layer configuration for an RNTI and an eNb cell/carrier.
   *
   * @param rnti identifier of the UE
   * @param cc_idx the eNb cell/carrier identifier
   * @param[out] ul_cfg Current UL PHY configuration
   * @return SRSRAN_SUCCESS if provided RNTI exists in the given cell, SRSRAN_ERROR code otherwise
   */
  int get_ul_config(uint16_t rnti, uint32_t enb_cc_idx, srsran_ul_cfg_t& ul_cfg) const;

  /**
   * Get the current DCI configuration for PUSCH physical layer configuration for an RNTI and an eNb cell/carrier
   *
   * @param rnti identifier of the UE
   * @param cc_idx the eNb cell/carrier identifier
   * @param[out] dci_cfg Current UL-DCI configuration
   * @return SRSRAN_SUCCESS if provided RNTI exists in the given cell, SRSRAN_ERROR code otherwise
   */
  int get_dci_ul_config(uint16_t rnti, uint32_t enb_cc_idx, srsran_dci_cfg_t& dci_cfg) const;

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
  bool set_ack_pending(uint32_t tti, uint32_t enb_cc_idx, const srsran_dci_dl_t& dci);

  /**
   * Fills the Uplink Control Information (UCI) configuration and returns true/false idicating if UCI bits are required.
   * @param tti the current UL reception TTI
   * @param cc_idx the eNb cell/carrier where the UL receiption is happening
   * @param rnti is the UE identifier
   * @param aperiodic_cqi_request indicates if aperiodic CQI was requested
   * @param uci_cfg brings the UCI configuration
   * @return 1 if UCI decoding is required, 0 if not, -1 if error
   */
  int fill_uci_cfg(uint32_t          tti,
                   uint32_t          enb_cc_idx,
                   uint16_t          rnti,
                   bool              aperiodic_cqi_request,
                   bool              is_pusch_available,
                   srsran_uci_cfg_t& uci_cfg);

  /**
   * Sends the decoded Uplink Control Information by PUCCH or PUSCH to MAC
   * @param tti the current TTI
   * @param rnti is the UE identifier
   * @param uci_cfg is the UCI configuration
   * @param uci_value is the UCI received value
   * @return SRSRAN_SUCCESS if provided RNTI exists in the given cell, SRSRAN_ERROR code otherwise
   */
  int send_uci_data(uint32_t                  tti,
                    uint16_t                  rnti,
                    uint32_t                  enb_cc_idx,
                    const srsran_uci_cfg_t&   uci_cfg,
                    const srsran_uci_value_t& uci_value);

  /**
   * Set the latest UL Transport Block resource allocation for a given RNTI, eNb cell/carrier and UL HARQ process
   * identifier.
   *
   * @param rnti the UE temporal ID
   * @param enb_cc_idx the cell/carrier origin of the transmission
   * @param pid HARQ process identifier
   * @param tb the Resource Allocation for the PUSCH transport block
   * @return SRSRAN_SUCCESS if provided RNTI exists in the given cell, SRSRAN_ERROR code otherwise
   */
  int set_last_ul_tb(uint16_t rnti, uint32_t enb_cc_idx, uint32_t pid, srsran_ra_tb_t tb);

  /**
   * Get the latest UL Transport Block resource allocation for a given RNTI, eNb cell/carrier and UL HARQ process
   * identifier. It returns the resource allocation if the RNTI and cell/eNb are valid, otherwise it will return an
   * default Resource allocation (all zeros by default).
   *
   * @param rnti the UE temporal ID
   * @param cc_idx the cell/carrier origin of the transmission
   * @param pid HARQ process identifier
   * @param[out] ra_tb the Resource Allocation for the PUSCH transport block
   * @return SRSRAN_SUCCESS if the provided context is valid
   */
  int get_last_ul_tb(uint16_t rnti, uint32_t enb_cc_idx, uint32_t pid, srsran_ra_tb_t& ra_tb) const;

  /**
   * Flags to true the UL grant available for a given TTI, RNTI and eNb cell/carrier index
   * @param tti the current TTI
   * @param rnti
   * @param enb_cc_idx
   */
  int set_ul_grant_available(uint32_t tti, const stack_interface_phy_lte::ul_sched_list_t& ul_sched_list);
};

} // namespace srsenb
#endif // SRSENB_PHY_UE_DB_H_
