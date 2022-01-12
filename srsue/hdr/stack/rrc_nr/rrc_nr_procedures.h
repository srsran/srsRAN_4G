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

#include "srsran/srslog/srslog.h"
#include "srsue/hdr/stack/rrc_nr/rrc_nr.h"

#ifndef SRSRAN_RRC_NR_PROCEDURES_H
#define SRSRAN_RRC_NR_PROCEDURES_H

namespace srsue {

/********************************
 *         Procedures
 *******************************/

class rrc_nr::cell_selection_proc
{
public:
  using cell_selection_complete_ev = srsran::proc_result_t<cell_search_result_t>;

  explicit cell_selection_proc(rrc_nr& parent_);
  srsran::proc_outcome_t init();
  srsran::proc_outcome_t step();
  cell_search_result_t   get_result() const { return cell_search_ret; }
  static const char*     name() { return "Cell Selection"; }
  srsran::proc_outcome_t react(const rrc_interface_phy_nr::cell_search_result_t& result);
  srsran::proc_outcome_t react(const rrc_interface_phy_nr::cell_select_result_t& result);
  void                   then(const srsran::proc_result_t<cell_search_result_t>& proc_result) const;

private:
  bool                   is_serv_cell_suitable() const;
  srsran::proc_outcome_t set_proc_complete();

  // consts
  rrc_nr&                       rrc_handle;
  meas_cell_list<meas_cell_nr>& meas_cells;

  // state variables
  enum class search_state_t { cell_selection, serv_cell_camp, cell_config, cell_search };
  cell_search_result_t                                            cell_search_ret;
  search_state_t                                                  state;
  srsran::proc_future_t<rrc_interface_phy_lte::cell_search_ret_t> cell_search_fut;
  srsran::proc_future_t<void>                                     serv_cell_cfg_fut;
  phy_cell_t                                                      init_serv_cell;
};

class rrc_nr::setup_request_proc
{
public:
  explicit setup_request_proc(rrc_nr& parent_);
  srsran::proc_outcome_t init(srsran::nr_establishment_cause_t cause_,
                              srsran::unique_byte_buffer_t     dedicated_info_nas_);
  srsran::proc_outcome_t step();
  void                   then(const srsran::proc_state_t& result);
  srsran::proc_outcome_t react(const cell_selection_proc::cell_selection_complete_ev& e);
  static const char*     name() { return "Setup Request"; }

private:
  // const
  rrc_nr&               rrc_handle;
  srslog::basic_logger& logger;
  // args
  srsran::nr_establishment_cause_t cause;
  srsran::unique_byte_buffer_t     dedicated_info_nas;

  // state variables
  enum class state_t { cell_selection, config_serving_cell, wait_t300 } state;
  cell_search_result_t        cell_search_ret;
  srsran::proc_future_t<void> serv_cfg_fut;
};

class rrc_nr::connection_setup_proc
{
public:
  explicit connection_setup_proc(rrc_nr& parent_);
  srsran::proc_outcome_t init(const asn1::rrc_nr::radio_bearer_cfg_s radio_bearer_cfg_,
                              const asn1::rrc_nr::cell_group_cfg_s   cell_group_,
                              srsran::unique_byte_buffer_t           dedicated_info_nas_);
  srsran::proc_outcome_t step() { return srsran::proc_outcome_t::yield; }
  static const char*     name() { return "Connection Setup"; }
  srsran::proc_outcome_t react(const bool& config_complete);
  void                   then(const srsran::proc_state_t& result);

private:
  // const
  rrc_nr&               rrc_handle;
  srslog::basic_logger& logger;
  // args
  srsran::unique_byte_buffer_t dedicated_info_nas;
};

class rrc_nr::connection_reconf_no_ho_proc
{
public:
  explicit connection_reconf_no_ho_proc(rrc_nr& parent_);
  srsran::proc_outcome_t init(const reconf_initiator_t         initiator_,
                              const bool                       endc_release_and_add_r15,
                              const asn1::rrc_nr::rrc_recfg_s& rrc_nr_reconf);
  srsran::proc_outcome_t step() { return srsran::proc_outcome_t::yield; }
  static const char*     name() { return "NR Connection Reconfiguration"; }
  srsran::proc_outcome_t react(const bool& config_complete);
  void                   then(const srsran::proc_state_t& result);

private:
  // const
  rrc_nr&                        rrc_handle;
  reconf_initiator_t             initiator;
  asn1::rrc_nr::rrc_recfg_s      rrc_recfg;
  asn1::rrc_nr::cell_group_cfg_s cell_group_cfg;
};

} // namespace srsue

#endif // SRSRAN_RRC_NR_PROCEDURES_H
