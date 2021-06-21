/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsenb/hdr/phy/nr/phy_nr_state.h"

namespace srsenb {
namespace nr {

int phy_nr_state::_add_rnti(uint16_t rnti)
{
  if (ue_db.count(rnti) > 0) {
    return SRSRAN_ERROR;
  }

  // Access UE to create
  // Set defaults
  ue_db[rnti] = {};

  return SRSRAN_SUCCESS;
}

int phy_nr_state::_assert_rnti(uint16_t rnti) const
{
  return ue_db.count(rnti) > 0 ? SRSRAN_SUCCESS : SRSRAN_ERROR;
}

int phy_nr_state::_get_rnti_config(uint16_t rnti, srsran::phy_cfg_nr_t& phy_cfg) const
{
  if (_assert_rnti(rnti) < SRSRAN_SUCCESS) {
    return SRSRAN_ERROR;
  }

  phy_cfg = ue_db.at(rnti).cfg;

  return SRSRAN_SUCCESS;
}

phy_nr_state::phy_nr_state(const phy_cell_cfg_list_nr_t& cell_cfg_list_, stack_interface_phy_nr& stack_) :
  cell_cfg_list(cell_cfg_list_),
  stack(stack_)
{}

void phy_nr_state::addmod_rnti(uint16_t rnti, const srsran::phy_cfg_nr_t& phy_cfg)
{
  std::lock_guard<std::mutex> lock(mutex);

  // Create UE if it does not exist
  if (_assert_rnti(rnti) < SRSRAN_SUCCESS) {
    _add_rnti(rnti);
  }

  // Set UE configuration
  ue_db[rnti].cfg = phy_cfg;
}
int phy_nr_state::rem_rnti(uint16_t rnti)
{
  std::lock_guard<std::mutex> lock(mutex);

  if (_assert_rnti(rnti) < SRSRAN_SUCCESS) {
    return SRSRAN_ERROR;
  }

  ue_db.erase(rnti);

  return SRSRAN_SUCCESS;
}

int phy_nr_state::get_config(uint16_t rnti, srsran::phy_cfg_nr_t& phy_cfg)
{
  std::lock_guard<std::mutex> lock(mutex);

  return _get_rnti_config(rnti, phy_cfg);
}

} // namespace nr
} // namespace srsenb