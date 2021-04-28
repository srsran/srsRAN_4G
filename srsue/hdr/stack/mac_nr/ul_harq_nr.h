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

#ifndef SRSUE_UL_HARQ_NR_H
#define SRSUE_UL_HARQ_NR_H

#include "mux_nr.h"
#include "proc_ra_nr.h"
#include "srsran/common/interfaces_common.h"
#include "srsran/common/timers.h"
#include "srsran/interfaces/ue_nr_interfaces.h"

using namespace srsran;

namespace srsue {

class ul_harq_entity_nr
{
public:
  ul_harq_entity_nr(const uint8_t cc_idx_, mac_interface_harq_nr* mac_, proc_ra_nr* ra_proc__, mux_nr* mux_);

  int init();

  void reset();
  void reset_ndi();
  void set_config(srsran::ul_harq_cfg_t& harq_cfg);

  /***************** PHY->MAC interface for UL processes **************************/
  void new_grant_ul(const mac_interface_phy_nr::mac_nr_grant_ul_t& grant, mac_interface_phy_nr::tb_action_ul_t* action);

  int get_current_tbs(uint32_t pid);

  // HARQ specific metrics interface that is combined for all processees
  struct ul_harq_metrics_t {
    uint32_t tx_ok;
    uint32_t tx_ko;
    uint32_t tx_brate;
  };
  ul_harq_metrics_t get_metrics();

private:
  class ul_harq_process_nr
  {
  public:
    ul_harq_process_nr();
    ~ul_harq_process_nr();

    bool init(uint32_t pid_, ul_harq_entity_nr* entity_);
    void reset();
    void reset_ndi();
    uint8_t get_ndi();
    bool    has_grant();

    uint32_t get_nof_retx();
    int      get_current_tbs();

    /**
     * Implements Section 5.4.2.1
     *
     * @param grant       The unmodified grant as received from PHY
     * @param ndi_toggled The NDI toggled state determined by the entity
     * @param action      The resulting UL action structure to be filled.
     */
    void new_grant_ul(const mac_interface_phy_nr::mac_nr_grant_ul_t& grant,
                      const bool&                                    ndi_toggled,
                      mac_interface_phy_nr::tb_action_ul_t*          action);

  private:
    mac_interface_phy_nr::mac_nr_grant_ul_t current_grant = {};
    bool                                    grant_configured = false;

    uint32_t pid          = 0;
    uint32_t nof_retx     = 0;
    bool     is_initiated = false;

    srslog::basic_logger&  logger;
    ul_harq_entity_nr*     harq_entity = nullptr;
    srsran_softbuffer_tx_t softbuffer;

    std::unique_ptr<byte_buffer_t> harq_buffer = nullptr;

    void generate_tx(mac_interface_phy_nr::tb_action_ul_t* action);
    void generate_new_tx(const mac_interface_phy_nr::mac_nr_grant_ul_t& grant,
                         mac_interface_phy_nr::tb_action_ul_t*          action);
    void generate_retx(const mac_interface_phy_nr::mac_nr_grant_ul_t& grant,
                       mac_interface_phy_nr::tb_action_ul_t*          action);
  };

  std::array<ul_harq_process_nr, SRSRAN_MAX_HARQ_PROC_UL_NR> harq_procs;

  mac_interface_harq_nr* mac = nullptr;
  mux_nr*                mux = nullptr;
  srslog::basic_logger&  logger;

  srsran::ul_harq_cfg_t harq_cfg = {};
  ul_harq_metrics_t     metrics  = {};
};

typedef std::unique_ptr<ul_harq_entity_nr>                     ul_harq_entity_nr_ptr;
typedef std::array<ul_harq_entity_nr_ptr, SRSRAN_MAX_CARRIERS> ul_harq_entity_nr_vector;

} // namespace srsue

#endif // SRSUE_UL_HARQ_NR_H
