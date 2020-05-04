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

#ifndef SRSENB_RRC_MOBILITY_H
#define SRSENB_RRC_MOBILITY_H

#include "rrc.h"
#include "srslte/common/logmap.h"
#include <map>

namespace srsenb {

/**
 * This class is responsible for storing the UE Measurement Configuration at the eNB side.
 * Has the same fields as asn1::rrc::var_meas_cfg but stored in data structs that are easier to handle
 */
class var_meas_cfg_t
{
public:
  using meas_cell_t  = asn1::rrc::cells_to_add_mod_s;
  using meas_id_t    = asn1::rrc::meas_id_to_add_mod_s;
  using meas_obj_t   = asn1::rrc::meas_obj_to_add_mod_s;
  using report_cfg_t = asn1::rrc::report_cfg_to_add_mod_s;

  var_meas_cfg_t() : rrc_log(srslte::logmap::get("RRC")) {}
  std::tuple<bool, meas_obj_t*, meas_cell_t*> add_cell_cfg(const meas_cell_cfg_t& cellcfg);
  report_cfg_t*                               add_report_cfg(const asn1::rrc::report_cfg_eutra_s& reportcfg);
  meas_id_t*                                  add_measid_cfg(uint8_t measobjid, uint8_t repid);
  asn1::rrc::quant_cfg_s*                     add_quant_cfg(const asn1::rrc::quant_cfg_eutra_s& quantcfg);

  bool compute_diff_meas_cfg(const var_meas_cfg_t& target_cfg, asn1::rrc::meas_cfg_s* meas_cfg) const;
  void compute_diff_meas_objs(const var_meas_cfg_t& target_cfg, asn1::rrc::meas_cfg_s* meas_cfg) const;
  void compute_diff_cells(const asn1::rrc::meas_obj_eutra_s& target_it,
                          const asn1::rrc::meas_obj_eutra_s& src_it,
                          asn1::rrc::meas_obj_to_add_mod_s*  added_obj) const;
  void compute_diff_report_cfgs(const var_meas_cfg_t& target_cfg, asn1::rrc::meas_cfg_s* meas_cfg) const;
  void compute_diff_meas_ids(const var_meas_cfg_t& target_cfg, asn1::rrc::meas_cfg_s* meas_cfg) const;
  void compute_diff_quant_cfg(const var_meas_cfg_t& target_cfg, asn1::rrc::meas_cfg_s* meas_cfg_msg) const;

  // getters
  const asn1::rrc::meas_obj_to_add_mod_list_l&   meas_objs() const { return var_meas.meas_obj_list; }
  const asn1::rrc::report_cfg_to_add_mod_list_l& rep_cfgs() const { return var_meas.report_cfg_list; }
  const asn1::rrc::meas_id_to_add_mod_list_l&    meas_ids() const { return var_meas.meas_id_list; }
  asn1::rrc::meas_obj_to_add_mod_list_l&         meas_objs() { return var_meas.meas_obj_list; }
  asn1::rrc::report_cfg_to_add_mod_list_l&       rep_cfgs() { return var_meas.report_cfg_list; }
  asn1::rrc::meas_id_to_add_mod_list_l&          meas_ids() { return var_meas.meas_id_list; }

  static var_meas_cfg_t make(const asn1::rrc::meas_cfg_s& meas_cfg);

private:
  asn1::rrc::var_meas_cfg_s var_meas;
  srslte::log_ref           rrc_log;
};

class rrc::enb_mobility_handler
{
public:
  explicit enb_mobility_handler(rrc* rrc_);

  //! Variable used to store the MeasConfig expected for each cell.
  // Note: Made const to forbid silent updates and enable comparison based on addr
  std::vector<std::shared_ptr<const var_meas_cfg_t> > cell_meas_cfg_list;

  rrc *get_rrc() { return rrc_ptr; }
  const rrc *get_rrc() const { return rrc_ptr; }

private:
  // args
  rrc*             rrc_ptr = nullptr;
  const rrc_cfg_t* cfg     = nullptr;
};

class rrc::ue::rrc_mobility
{
public:
  explicit rrc_mobility(srsenb::rrc::ue* outer_ue);
  bool fill_conn_recfg_msg(asn1::rrc::rrc_conn_recfg_r8_ies_s* conn_recfg);
  void handle_ue_meas_report(const asn1::rrc::meas_report_s& msg);
  void handle_ho_preparation_complete(bool is_success, srslte::unique_byte_buffer_t container);

private:
  enum class ho_interface_t { S1, X2, interSector };

  bool start_ho_preparation(uint32_t target_eci, uint8_t measobj_id, bool fwd_direct_path_available);
  bool start_enb_status_transfer();

  bool update_ue_var_meas_cfg(const asn1::rrc::meas_cfg_s& source_meas_cfg,
                              uint32_t                     target_enb_cc_idx,
                              asn1::rrc::meas_cfg_s*       diff_meas_cfg);
  bool update_ue_var_meas_cfg(const var_meas_cfg_t&  source_var_meas_cfg,
                              uint32_t               target_enb_cc_idx,
                              asn1::rrc::meas_cfg_s* diff_meas_cfg);

  rrc::ue*                   rrc_ue  = nullptr;
  rrc*                       rrc_enb = nullptr;
  rrc::enb_mobility_handler* cfg     = nullptr;
  srslte::byte_buffer_pool*  pool    = nullptr;
  srslte::log_ref            rrc_log;

  // vars
  std::shared_ptr<const var_meas_cfg_t> ue_var_meas;

  class sourceenb_ho_proc_t
  {
  public:
    struct ho_prep_result {
      bool                         is_success;
      srslte::unique_byte_buffer_t rrc_container;
    };

    explicit sourceenb_ho_proc_t(rrc_mobility* ue_mobility_);
    srslte::proc_outcome_t init(const asn1::rrc::meas_id_to_add_mod_s&    measid_,
                                const asn1::rrc::meas_obj_to_add_mod_s&   measobj_,
                                const asn1::rrc::report_cfg_to_add_mod_s& repcfg_,
                                const asn1::rrc::cells_to_add_mod_s&      cell_,
                                const asn1::rrc::meas_result_eutra_s&     meas_res_,
                                uint32_t                                  target_eci_);
    srslte::proc_outcome_t step() { return srslte::proc_outcome_t::yield; }
    srslte::proc_outcome_t react(ho_prep_result);
    static const char*     name() { return "Handover"; }

  private:
    // args
    rrc_mobility* parent = nullptr;
    // run args
    const asn1::rrc::meas_id_to_add_mod_s*    measid  = nullptr;
    const asn1::rrc::meas_obj_to_add_mod_s*   measobj = nullptr;
    const asn1::rrc::report_cfg_to_add_mod_s* repcfg  = nullptr;
    const asn1::rrc::cells_to_add_mod_s*      cell    = nullptr;
    asn1::rrc::meas_result_eutra_s            meas_res;
    uint32_t                                  target_eci = 0;

    enum class state_t { ho_preparation, ho_execution } state{};
    ho_interface_t ho_interface{};
    bool           fwd_direct_path_available = false;
  };
  srslte::proc_t<sourceenb_ho_proc_t> source_ho_proc;
};

} // namespace srsenb
#endif // SRSENB_RRC_MOBILITY_H
