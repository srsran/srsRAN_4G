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

#ifndef SRSENB_PHY_NR_UE_DB_H_
#define SRSENB_PHY_NR_UE_DB_H_

#include <map>
#include <mutex>
#include <srsenb/hdr/phy/phy_interfaces.h>
#include <srsran/adt/circular_array.h>
#include <srsran/interfaces/gnb_interfaces.h>
#include <srsran/interfaces/rrc_nr_interface_types.h>

namespace srsenb {
namespace nr {

class phy_nr_state
{
private:
  /**
   * UE object stored in the PHY common database
   */
  struct common_ue {
    srsran::phy_cfg_nr_t cfg;
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
  stack_interface_phy_nr& stack;

  /**
   * Cell list
   */
  const phy_cell_cfg_list_nr_t& cell_cfg_list;

  /**
   * Internal RNTI addition, it is not thread safe protected
   *
   * @param rnti identifier of the UE
   * @return SRSRAN_SUCCESS if the RNTI is not duplicated and is added successfully, SRSRAN_ERROR code if it exists
   */
  inline int _add_rnti(uint16_t rnti);

  /**
   * Checks if a given RNTI exists in the database
   * @param rnti provides UE identifier
   * @return SRSRAN_SUCCESS if the indicated RNTI exists, otherwise it returns SRSRAN_ERROR
   */
  inline int _assert_rnti(uint16_t rnti) const;

  /**
   * Internal eNb general configuration getter, returns default configuration if the UE does not exist in the given cell
   *
   * @param rnti provides UE identifier
   * @param[out] phy_cfg The PHY configuration of the indicated UE for the indicated eNb carrier/call index.
   * @return SRSRAN_SUCCESS if provided context is correct, SRSRAN_ERROR code otherwise
   */
  inline int _get_rnti_config(uint16_t rnti, srsran::phy_cfg_nr_t& phy_cfg) const;

public:
  phy_nr_state(const phy_cell_cfg_list_nr_t& cell_cfg_list_, stack_interface_phy_nr& stack_);

  void addmod_rnti(uint16_t rnti, const srsran::phy_cfg_nr_t& phy_cfg);

  /**
   * Removes a whole UE entry from the UE database
   *
   * @param rnti identifier of the UE
   * @return SRSRAN_SUCCESS if provided RNTI exists, SRSRAN_ERROR code otherwise
   */
  int rem_rnti(uint16_t rnti);

  const phy_cell_cfg_list_nr_t& get_carrier_list() const { return cell_cfg_list; }

  stack_interface_phy_nr& get_stack() { return stack; }
};

} // namespace nr
} // namespace srsenb

#endif // SRSENB_PHY_NR_UE_DB_H_
