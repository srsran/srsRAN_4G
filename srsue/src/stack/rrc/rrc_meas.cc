/**
 * Copyright 2013-2023 Software Radio Systems Limited
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

#include "srsue/hdr/stack/rrc/rrc_meas.h"
#include "srsran/asn1/obj_id_cmp_utils.h"
#include "srsran/asn1/rrc/dl_dcch_msg.h"
#include "srsran/interfaces/ue_phy_interfaces.h"
#include "srsue/hdr/stack/rrc/rrc.h"

/************************************************************************
 *
 *
 * RRC Measurements
 *
 *
 ************************************************************************/
using namespace srsran;
using namespace asn1::rrc;

namespace srsue {

meas_obj_to_add_mod_s* find_meas_obj_map(std::map<uint32_t, meas_obj_to_add_mod_s>& l, uint32_t earfcn)
{
  auto same_earfcn = [earfcn](const std::pair<uint32_t, meas_obj_to_add_mod_s>& c) {
    return (int)earfcn == srsran::get_carrier_freq(c.second);
  };
  auto it = std::find_if(l.begin(), l.end(), same_earfcn);
  if (it == l.end()) {
    return nullptr;
  } else {
    return &(*it).second;
  }
}

void rrc::rrc_meas::init(rrc* rrc_ptr_)
{
  rrc_ptr = rrc_ptr_;
  meas_report_list.init(rrc_ptr_);
  meas_cfg.init(rrc_ptr_);
  reset();
}

void rrc::rrc_meas::reset()
{
  meas_cfg.reset();
  rrc_ptr->phy->meas_stop();
}

// Perform Layer 3 filtering 5.5.3.2
float rrc::rrc_meas::rsrp_filter(const float new_value, const float avg_value)
{
  phy_quant_t f = meas_cfg.get_filter_a();
  return std::isnormal(avg_value) ? SRSRAN_VEC_EMA(new_value, avg_value, f.rsrp) : new_value;
}

// Perform Layer 3 filtering 5.5.3.2
float rrc::rrc_meas::rsrq_filter(const float new_value, const float avg_value)
{
  phy_quant_t f = meas_cfg.get_filter_a();
  return std::isnormal(avg_value) ? SRSRAN_VEC_EMA(new_value, avg_value, f.rsrq) : new_value;
}

/* Instruct PHY to start measurement on every configured frequency */
void rrc::rrc_meas::update_phy()
{
  std::list<meas_obj_to_add_mod_s> objects = meas_cfg.get_active_objects();
  rrc_ptr->phy->meas_stop();
  rrc_ptr->rrc_nr->phy_meas_stop();
  for (const auto& obj : objects) {
    switch (obj.meas_obj.type().value) {
      case meas_obj_to_add_mod_s::meas_obj_c_::types_opts::meas_obj_eutra: {
        std::set<uint32_t> neighbour_pcis = rrc_ptr->get_cells(obj.meas_obj.meas_obj_eutra().carrier_freq);
        for (const auto& cell : obj.meas_obj.meas_obj_eutra().cells_to_add_mod_list) {
          neighbour_pcis.insert(cell.pci);
        }
        // Instruct PHY to look for cells IDs on this frequency. If neighbour_pcis is empty it will look for new cells
        rrc_ptr->phy->set_cells_to_meas(obj.meas_obj.meas_obj_eutra().carrier_freq, neighbour_pcis);
        break;
      }
      case meas_obj_to_add_mod_s::meas_obj_c_::types_opts::meas_obj_nr_r15: {
        rrc_ptr->rrc_nr->phy_set_cells_to_meas(obj.meas_obj.meas_obj_nr_r15().carrier_freq_r15);
        break;
      }
      default:
        logger.error("Not supported");
        break;
    }
    // Concatenate cells indicated by enodeb with discovered neighbours
  }
}

/* Parses MeasConfig object from RRCConnectionReconfiguration message and applies configuration
 * as per section 5.5.2
 */
bool rrc::rrc_meas::parse_meas_config(const rrc_conn_recfg_r8_ies_s* mob_reconf_r8,
                                      bool                           is_ho_reest,
                                      uint32_t                       src_earfcn)
{
  std::lock_guard<std::mutex> lock(meas_cfg_mutex);
  bool                        ret = true;
  if (mob_reconf_r8->meas_cfg_present) {
    ret = meas_cfg.parse_meas_config(&mob_reconf_r8->meas_cfg, is_ho_reest, src_earfcn);
    update_phy();
  }
  return ret;
}

// Section 5.5.6.1 Actions upon handover and re-establishment
void rrc::rrc_meas::ho_reest_actions(const uint32_t src_earfcn, const uint32_t dst_earfcn)
{
  meas_cfg.ho_reest_finish(src_earfcn, dst_earfcn);
}

void rrc::rrc_meas::run_tti()
{
  std::lock_guard<std::mutex> lock(meas_cfg_mutex);

  // Evaluate triggers and send reports for events Section 5.5.4
  meas_cfg.eval_triggers();
  meas_cfg.report_triggers();
}

// For thresholds, the actual value is (field value – 156) dBm, except for field value 127, in which case the actual
// value is infinity.
float rrc::rrc_meas::range_to_value_nr(const asn1::rrc::thres_nr_r15_c::types_opts::options type, const uint8_t range)
{
  float val = 0;
  switch (type) {
    case asn1::rrc::thres_nr_r15_c::types_opts::options::nr_rsrp_r15:
      if (range == 127)
        val = std::numeric_limits<float>::infinity();
      else {
        val = -156 + (float)range;
      }
      break;
    case asn1::rrc::thres_nr_r15_c::types_opts::options::nr_rsrq_r15:
      val = -87 + (float)range / 2;
      break;
    case asn1::rrc::thres_nr_r15_c::types_opts::options::nr_sinr_r15:
      val = -46 + (float)range / 2;
      break;
    default:
      break;
  }
  return val;
}

uint8_t rrc::rrc_meas::value_to_range_nr(const asn1::rrc::thres_nr_r15_c::types_opts::options type, const float value)
{
  uint8_t range = 0;
  switch (type) {
    case asn1::rrc::thres_nr_r15_c::types_opts::options::nr_rsrp_r15:
      if (value == std::numeric_limits<float>::infinity()) {
        range = 127;
      } else {
        range = (uint8_t)(value + 156);
      }
      break;
    case asn1::rrc::thres_nr_r15_c::types_opts::options::nr_rsrq_r15:
      range = (uint8_t)(2 * (value + 87));
      break;
    case asn1::rrc::thres_nr_r15_c::types_opts::options::nr_sinr_r15:
      range = (uint8_t)(2 * (value + 46));
      break;
    default:
      break;
  }
  return range;
}

uint8_t rrc::rrc_meas::offset_val(const meas_obj_eutra_s& meas_obj)
{
  return meas_obj.offset_freq_present ? meas_obj.offset_freq.to_number() : 0;
}

asn1::dyn_array<cells_to_add_mod_s>::iterator rrc::rrc_meas::find_pci_in_meas_obj(meas_obj_eutra_s& meas_obj,
                                                                                  const uint32_t    pci)
{
  return std::find_if(meas_obj.cells_to_add_mod_list.begin(),
                      meas_obj.cells_to_add_mod_list.end(),
                      [&pci](const cells_to_add_mod_s& c) { return c.pci == pci; });
}

/*
 *
 * var_meas_report_list subclass
 *
 */

void rrc::rrc_meas::var_meas_report_list::init(rrc* rrc_ptr_)
{
  rrc_ptr = rrc_ptr_;
}

void rrc::rrc_meas::var_meas_report_list::generate_report_eutra(meas_results_s* report, const uint32_t measId)
{
  meas_cell_eutra* serv_cell = rrc_ptr->get_serving_cell();
  if (serv_cell == nullptr) {
    logger.warning("MEAS:  Serving cell not set when evaluating triggers");
    return;
  }

  meas_result_list_eutra_l& neigh_list = report->meas_result_neigh_cells.set_meas_result_list_eutra();
  var_meas_report&          var_meas   = varMeasReportList.at(measId);

  // sort cells by RSRP
  std::sort(
      var_meas.cell_triggered_list.begin(), var_meas.cell_triggered_list.end(), [this](phy_cell_t a, phy_cell_t b) {
        return rrc_ptr->get_cell_rsrp(a.earfcn, a.pci) > rrc_ptr->get_cell_rsrp(b.earfcn, b.pci);
      });

  // set the measResultNeighCells to include the best neighbouring cells up to maxReportCells in accordance with
  // the following
  for (auto& cell : var_meas.cell_triggered_list) {
    // report neighbour cells only
    if (cell.pci == serv_cell->get_pci() && cell.earfcn == serv_cell->get_earfcn()) {
      logger.info("MEAS:  skipping serving cell in report neighbour=%d, pci=%d, earfcn=%d",
                  neigh_list.size(),
                  cell.pci,
                  var_meas.carrier_freq);
      continue;
    }
    if (neigh_list.size() <= var_meas.report_cfg_eutra.max_report_cells) {
      float rsrp_value = rrc_ptr->get_cell_rsrp(var_meas.carrier_freq, cell.pci);
      float rsrq_value = rrc_ptr->get_cell_rsrq(var_meas.carrier_freq, cell.pci);

      meas_result_eutra_s rc = {};
      // Set quantity to report
      switch (var_meas.report_cfg_eutra.report_quant.value) {
        case report_cfg_eutra_s::report_quant_opts::both:
          rc.meas_result.rsrp_result_present = true;
          rc.meas_result.rsrq_result_present = true;
          break;
        case report_cfg_eutra_s::report_quant_opts::same_as_trigger_quant:
          switch (var_meas.report_cfg_eutra.trigger_quant.value) {
            case report_cfg_eutra_s::trigger_quant_opts::rsrp:
              rc.meas_result.rsrp_result_present = true;
              break;
            case report_cfg_eutra_s::trigger_quant_opts::rsrq:
              rc.meas_result.rsrp_result_present = true;
              break;
            default:
              break;
          }
          break;
        default:
          break;
      }
      rc.pci                     = (uint16_t)cell.pci;
      rc.meas_result.rsrp_result = rrc_value_to_range(quant_rsrp, rsrp_value);
      rc.meas_result.rsrq_result = rrc_value_to_range(quant_rsrq, rsrq_value);

      logger.info("MEAS:  Adding to report neighbour=%d, pci=%d, earfcn=%d, rsrp=%+.1f, rsrq=%+.1f",
                  neigh_list.size(),
                  rc.pci,
                  var_meas.carrier_freq,
                  rsrp_value,
                  rsrq_value);

      neigh_list.push_back(rc);
    }
  }
  report->meas_result_neigh_cells_present = neigh_list.size() > 0;

  var_meas.nof_reports_sent++;
  if (var_meas.periodic_timer.is_valid()) {
    var_meas.periodic_timer.stop();
  }

  // if the numberOfReportsSent as defined within the VarMeasReportList for this measId is less than the
  // reportAmount as defined within the corresponding reportConfig for this measId (also includes case where amount is
  // infinity)
  if (var_meas.nof_reports_sent < var_meas.report_cfg_eutra.report_amount.to_number() ||
      var_meas.report_cfg_eutra.report_amount.to_number() == -1) {
    // start the periodical reporting timer with the value of reportInterval as defined within the corresponding
    // reportConfig for this measId
    if (var_meas.periodic_timer.is_valid()) {
      var_meas.periodic_timer.run();
    }
  } else {
    if (var_meas.periodic_timer.is_valid()) {
      var_meas.periodic_timer.release();
    }
    // else if the triggerType is set to ‘periodical’:
    if (var_meas.report_cfg_eutra.trigger_type.type().value == report_cfg_eutra_s::trigger_type_c_::types::periodical) {
      // remove the entry within the VarMeasReportList for this measId
      remove_varmeas_report(measId);
      meas_cfg->remove_measId(measId);
    }
  }
}
void rrc::rrc_meas::var_meas_report_list::generate_report_interrat(meas_results_s* report, const uint32_t measId)
{
  meas_result_cell_list_nr_r15_l& neigh_list = report->meas_result_neigh_cells.set_meas_result_neigh_cell_list_nr_r15();

  var_meas_report& var_meas = varMeasReportList.at(measId);

  // sort cells by RSRP
  std::sort(
      var_meas.cell_triggered_list.begin(), var_meas.cell_triggered_list.end(), [this](phy_cell_t a, phy_cell_t b) {
        return rrc_ptr->get_cell_rsrq_nr(a.earfcn, a.pci) > rrc_ptr->get_cell_rsrq_nr(b.earfcn, b.pci);
      });

  // set the measResultNeighCells to include the best neighbouring cells up to maxReportCells in accordance with
  // the following

  for (auto& cell : var_meas.cell_triggered_list) {
    if (neigh_list.size() <= var_meas.report_cfg_inter.max_report_cells) {
      meas_result_cell_nr_r15_s rc = {};

      float rsrp_value = rrc_ptr->get_cell_rsrp_nr(var_meas.carrier_freq, cell.pci);
      float rsrq_value = rrc_ptr->get_cell_rsrq_nr(var_meas.carrier_freq, cell.pci);
      // float sinr_value = rrc_pts->get_cell_sinr_nr(var_meas.carrier_freq, cell.pci); TODO
      rc.pci_r15 = (uint16_t)cell.pci;

      // Set quantity to report
      if (var_meas.report_cfg_inter.report_quant_cell_nr_r15.is_present()) {
        if (var_meas.report_cfg_inter.report_quant_cell_nr_r15->ss_rsrp == true) {
          rc.meas_result_cell_r15.rsrp_result_r15_present = true;
          rc.meas_result_cell_r15.rsrp_result_r15 =
              value_to_range_nr(asn1::rrc::thres_nr_r15_c::types_opts::options::nr_rsrp_r15, rsrp_value);
        }
        if (var_meas.report_cfg_inter.report_quant_cell_nr_r15->ss_rsrq == true) {
          rc.meas_result_cell_r15.rsrq_result_r15_present = true;
          rc.meas_result_cell_r15.rsrq_result_r15 =
              value_to_range_nr(asn1::rrc::thres_nr_r15_c::types_opts::options::nr_rsrq_r15, rsrq_value);
        }
        if (var_meas.report_cfg_inter.report_quant_cell_nr_r15->ss_sinr == true) {
          rc.meas_result_cell_r15.rs_sinr_result_r15_present = true;
          rc.meas_result_cell_r15.rs_sinr_result_r15 =
              value_to_range_nr(asn1::rrc::thres_nr_r15_c::types_opts::options::nr_sinr_r15, 1.0);
        }
      } else {
        logger.warning("Report quantity for NR cells not present in measurement config. Sending RSRP anyway.");
        rc.meas_result_cell_r15.rsrp_result_r15_present = true;
        rc.meas_result_cell_r15.rsrp_result_r15 =
            value_to_range_nr(asn1::rrc::thres_nr_r15_c::types_opts::options::nr_rsrp_r15, rsrp_value);
      }

      logger.info("MEAS:  Adding to report neighbour=%d, pci=%d, earfcn=%d, rsrp=%+.1f, rsrq=%+.1f",
                  neigh_list.size(),
                  rc.pci_r15,
                  var_meas.carrier_freq,
                  rsrp_value,
                  rsrq_value);

      neigh_list.push_back(rc);
    }
  }
  report->meas_result_neigh_cells_present = neigh_list.size() > 0;

  var_meas.nof_reports_sent++;
  if (var_meas.periodic_timer.is_valid()) {
    var_meas.periodic_timer.stop();
  }

  // if the numberOfReportsSent as defined within the VarMeasReportList for this measId is less than the
  // reportAmount as defined within the corresponding reportConfig for this measId (also includes case where amount is
  // infinity)
  if (var_meas.nof_reports_sent < var_meas.report_cfg_inter.report_amount.to_number() ||
      var_meas.report_cfg_inter.report_amount.to_number() == -1) {
    // start the periodical reporting timer with the value of reportInterval as defined within the corresponding
    // reportConfig for this measId
    if (var_meas.periodic_timer.is_valid()) {
      var_meas.periodic_timer.run();
    }
  } else {
    if (var_meas.periodic_timer.is_valid()) {
      var_meas.periodic_timer.release();
    }
    // else if the triggerType is set to ‘periodical’:
    if (var_meas.report_cfg_inter.trigger_type.type().value ==
        report_cfg_inter_rat_s::trigger_type_c_::types::periodical) {
      // remove the entry within the VarMeasReportList for this measId
      remove_varmeas_report(measId);
      meas_cfg->remove_measId(measId);
    }
  }
}
/* Generate report procedure 5.5.5 */
void rrc::rrc_meas::var_meas_report_list::generate_report(const uint32_t measId)
{
  meas_cell_eutra* serv_cell = rrc_ptr->get_serving_cell();
  if (serv_cell == nullptr) {
    logger.warning("MEAS:  Serving cell not set when evaluating triggers");
    return;
  }

  ul_dcch_msg_s ul_dcch_msg;
  ul_dcch_msg.msg.set_c1().set_meas_report().crit_exts.set_c1().set_meas_report_r8();

  meas_results_s* report = &ul_dcch_msg.msg.c1().meas_report().crit_exts.c1().meas_report_r8().meas_results;

  report->meas_id                       = (uint8_t)measId;
  report->meas_result_pcell.rsrp_result = rrc_value_to_range(quant_rsrp, serv_cell->get_rsrp());
  report->meas_result_pcell.rsrq_result = rrc_value_to_range(quant_rsrq, serv_cell->get_rsrq());

  logger.info("MEAS:  Generate report MeasId=%d, Pcell rsrp=%f rsrq=%f",
              report->meas_id,
              serv_cell->get_rsrp(),
              serv_cell->get_rsrq());

  meas_result_list_eutra_l& neigh_list = report->meas_result_neigh_cells.set_meas_result_list_eutra();
  var_meas_report&          var_meas   = varMeasReportList.at(measId);

  switch (var_meas.report_type) {
    case eutra: {
      logger.debug("MEAS: Generate EUTRA report");
      generate_report_eutra(report, measId);
      break;
    }
    case inter_rat: {
      logger.debug("MEAS: Generate INTER RAT NR report");
      generate_report_interrat(report, measId);
      break;
    }
    default:
      logger.debug("MEAS: Not supported");
      break;
  }

  // Send to lower layers
  rrc_ptr->send_srb1_msg(ul_dcch_msg);
}

void rrc::rrc_meas::var_meas_report_list::remove_all_varmeas_reports()
{
  varMeasReportList.clear();
}

void rrc::rrc_meas::var_meas_report_list::remove_varmeas_report(const uint32_t measId)
{
  if (varMeasReportList.count(measId)) {
    varMeasReportList.erase(measId);
  }
}

bool rrc::rrc_meas::var_meas_report_list::is_timer_expired(const uint32_t measId)
{
  if (varMeasReportList.count(measId)) {
    if (varMeasReportList.at(measId).periodic_timer.is_valid()) {
      return varMeasReportList.at(measId).periodic_timer.is_expired();
    }
  }
  return false;
}

void rrc::rrc_meas::var_meas_report_list::set_measId(const uint32_t            measId,
                                                     const uint32_t            carrier_freq,
                                                     const report_cfg_eutra_s& report_cfg,
                                                     const cell_triggered_t&   cell_triggered_list)
{
  // Create entry if it doesn't exist.
  if (!varMeasReportList.count(measId)) {
    varMeasReportList[measId].nof_reports_sent = 0;
  }

  // The ReportInterval is applicable if the UE performs periodical reporting (i.e. when reportAmount exceeds 1), for
  // triggerType ‘event’ as well as for triggerType ‘periodical’
  if (!varMeasReportList.at(measId).periodic_timer.is_valid() &&
      (report_cfg.report_amount.to_number() > 1 || report_cfg.report_amount.to_number() == -1)) {
    varMeasReportList.at(measId).periodic_timer = rrc_ptr->task_sched.get_unique_timer();
    varMeasReportList.at(measId).periodic_timer.set(report_cfg.report_interv.to_number());
  }
  varMeasReportList.at(measId).report_cfg_eutra = std::move(report_cfg);
  varMeasReportList.at(measId).carrier_freq     = carrier_freq;
  varMeasReportList.at(measId).nof_reports_sent = 0;
  upd_measId(measId, cell_triggered_list);
}

void rrc::rrc_meas::var_meas_report_list::set_measId(const uint32_t                measId,
                                                     const uint32_t                carrier_freq,
                                                     const report_cfg_inter_rat_s& report_cfg,
                                                     const cell_triggered_t&       cell_triggered_list)
{
  // Create entry if it doesn't exist.
  if (!varMeasReportList.count(measId)) {
    varMeasReportList[measId].nof_reports_sent = 0;
  }

  // The ReportInterval is applicable if the UE performs periodical reporting (i.e. when reportAmount exceeds 1), for
  // triggerType ‘event’ as well as for triggerType ‘periodical’
  if (!varMeasReportList.at(measId).periodic_timer.is_valid() &&
      (report_cfg.report_amount.to_number() > 1 || report_cfg.report_amount.to_number() == -1)) {
    varMeasReportList.at(measId).periodic_timer = rrc_ptr->task_sched.get_unique_timer();
    varMeasReportList.at(measId).periodic_timer.set(report_cfg.report_interv.to_number());
  }
  varMeasReportList.at(measId).report_type      = inter_rat;
  varMeasReportList.at(measId).report_cfg_inter = std::move(report_cfg);
  varMeasReportList.at(measId).carrier_freq     = carrier_freq;
  varMeasReportList.at(measId).nof_reports_sent = 0;
  upd_measId(measId, cell_triggered_list);
}

void rrc::rrc_meas::var_meas_report_list::upd_measId(const uint32_t measId, const cell_triggered_t& cell_triggered_list)
{
  if (varMeasReportList.count(measId)) {
    varMeasReportList.at(measId).cell_triggered_list = std::move(cell_triggered_list);
  }
}

cell_triggered_t rrc::rrc_meas::var_meas_report_list::get_measId_cells(const uint32_t measId)
{
  if (varMeasReportList.count(measId)) {
    return varMeasReportList.at(measId).cell_triggered_list;
  } else {
    return {};
  }
}

void rrc::rrc_meas::var_meas_cfg::report_triggers_eutra_check_new(int32_t             meas_id,
                                                                  report_cfg_eutra_s& report_cfg,
                                                                  meas_obj_eutra_s&   meas_obj)
{
  bool             new_cell_trigger     = false;
  cell_triggered_t cells_triggered_list = meas_report->get_measId_cells(meas_id);
  for (auto& cell : trigger_state[meas_id]) {
    if (cell.second.is_enter_equal(report_cfg.trigger_type.event().time_to_trigger.to_number())) {
      // Do not add if already exists
      if (std::find_if(cells_triggered_list.begin(), cells_triggered_list.end(), [&cell](const phy_cell_t& c) {
            return cell.first == c.pci;
          }) == cells_triggered_list.end()) {
        cells_triggered_list.push_back({cell.first, meas_obj.carrier_freq});
        new_cell_trigger = true;
      }
    }
  }

  if (new_cell_trigger) {
    // include a measurement reporting entry within the VarMeasReportList for this measId (nof_reports reset
    // inside) include the concerned cell(s) in the cellsTriggeredList defined within the VarMeasReportList
    meas_report->set_measId(meas_id, meas_obj.carrier_freq, report_cfg, cells_triggered_list);

    // initiate the measurement reporting procedure, as specified in 5.5.5;
    meas_report->generate_report(meas_id);
  }
}

void rrc::rrc_meas::var_meas_cfg::report_triggers_eutra_check_leaving(int32_t meas_id, report_cfg_eutra_s& report_cfg)
{
  // if the triggerType is set to ‘event’ and if the leaving condition applicable for this event is fulfilled ...
  cell_triggered_t cells_triggered_list = meas_report->get_measId_cells(meas_id);

  // remove the concerned cell(s) in the cellsTriggeredList defined within the VarMeasReportList
  auto it = cells_triggered_list.begin();
  while (it != cells_triggered_list.end()) {
    if (trigger_state[meas_id][it->pci].is_exit_equal(report_cfg.trigger_type.event().time_to_trigger.to_number())) {
      it = cells_triggered_list.erase(it);
      meas_report->upd_measId(meas_id, cells_triggered_list);

      // if reportOnLeave is set to TRUE for the corresponding reporting configuration
      if (report_cfg.trigger_type.event().event_id.type() == eutra_event_s::event_id_c_::types::event_a3 &&
          report_cfg.trigger_type.event().event_id.event_a3().report_on_leave) {
        // initiate the measurement reporting procedure, as specified in 5.5.5;
        meas_report->generate_report(meas_id);
      }

      // if the cellsTriggeredList defined within the VarMeasReportList for this measId is empty:
      if (cells_triggered_list.empty()) {
        remove_varmeas_report(meas_id);
      }
    } else {
      it++;
    }
  }
}

void rrc::rrc_meas::var_meas_cfg::report_triggers_eutra_removing_trigger(int32_t meas_id)
{
  meas_cell_eutra* serv_cell = rrc_ptr->get_serving_cell();
  if (serv_cell == nullptr) {
    logger.warning("MEAS:  Serving cell not set when reporting triggers");
    return;
  }
  uint32_t serving_pci = serv_cell->get_pci();

  // remove all cells in the cellsTriggeredList that are no neighbor cells anymore
  cell_triggered_t cells_triggered_list = meas_report->get_measId_cells(meas_id);
  auto             it                   = cells_triggered_list.begin();
  while (it != cells_triggered_list.end()) {
    if (not rrc_ptr->has_neighbour_cell(it->earfcn, it->pci) and it->pci != serving_pci) {
      logger.debug("MEAS:  Removing unknown PCI=%d from event trigger list", it->pci);
      it = cells_triggered_list.erase(it);
      meas_report->upd_measId(meas_id, cells_triggered_list);

      // if the cellsTriggeredList defined within the VarMeasReportList for this measId is empty:
      if (cells_triggered_list.empty()) {
        remove_varmeas_report(meas_id);
      }
    } else {
      it++;
    }
  }
}

void rrc::rrc_meas::var_meas_cfg::report_triggers_eutra(uint32_t            meas_id,
                                                        report_cfg_eutra_s& report_cfg,
                                                        meas_obj_eutra_s&   meas_obj)
{
  if (report_cfg.trigger_type.type() == report_cfg_eutra_s::trigger_type_c_::types::event) {
    // if the triggerType is set to ‘event’ and if the entry condition applicable for this event,
    report_triggers_eutra_check_new(meas_id, report_cfg, meas_obj);
    report_triggers_eutra_check_leaving(meas_id, report_cfg);
    report_triggers_eutra_removing_trigger(meas_id);
  }
}

void rrc::rrc_meas::var_meas_cfg::report_triggers_interrat_check_new(int32_t                 meas_id,
                                                                     report_cfg_inter_rat_s& report_cfg,
                                                                     meas_obj_nr_r15_s&      meas_obj)
{
  bool             new_cell_trigger     = false;
  cell_triggered_t cells_triggered_list = meas_report->get_measId_cells(meas_id);
  for (auto& cell : trigger_state_nr[meas_id]) {
    if (cell.second.is_enter_equal(report_cfg.trigger_type.event().time_to_trigger.to_number())) {
      // Do not add if already exists
      if (std::find_if(cells_triggered_list.begin(), cells_triggered_list.end(), [&cell](const phy_cell_t& c) {
            return cell.first == c.pci;
          }) == cells_triggered_list.end()) {
        cells_triggered_list.push_back({cell.first, meas_obj.carrier_freq_r15});
        new_cell_trigger = true;
      }
    }
  }

  if (new_cell_trigger) {
    // include a measurement reporting entry within the VarMeasReportList for this measId (nof_reports reset
    // inside) include the concerned cell(s) in the cellsTriggeredList defined within the VarMeasReportList
    meas_report->set_measId(meas_id, meas_obj.carrier_freq_r15, report_cfg, cells_triggered_list);

    // initiate the measurement reporting procedure, as specified in 5.5.5;
    meas_report->generate_report(meas_id);
  }
}
void rrc::rrc_meas::var_meas_cfg::report_triggers_interrat_check_leaving(int32_t                 meas_id,
                                                                         report_cfg_inter_rat_s& report_cfg)
{
  // if the triggerType is set to ‘event’ and if the leaving condition applicable for this event is fulfilled ...
  cell_triggered_t cells_triggered_list = meas_report->get_measId_cells(meas_id);

  // remove the concerned cell(s) in the cellsTriggeredList defined within the VarMeasReportList
  auto it = cells_triggered_list.begin();
  while (it != cells_triggered_list.end()) {
    if (trigger_state_nr[meas_id][it->pci].is_exit_equal(report_cfg.trigger_type.event().time_to_trigger.to_number())) {
      it = cells_triggered_list.erase(it);
      meas_report->upd_measId(meas_id, cells_triggered_list);

      // if reportOnLeave is set to TRUE for the corresponding reporting configuration
      if (report_cfg.trigger_type.event().event_id.type() ==
              report_cfg_inter_rat_s::trigger_type_c_::event_s_::event_id_c_::types_opts::options::event_b1_nr_r15 &&
          report_cfg.trigger_type.event().event_id.event_b1_nr_r15().report_on_leave_r15 == true) {
        // initiate the measurement reporting procedure, as specified in 5.5.5;
        meas_report->generate_report(meas_id);
      }

      // if the cellsTriggeredList defined within the VarMeasReportList for this measId is empty:
      if (cells_triggered_list.empty()) {
        remove_varmeas_report(meas_id);
      }
    } else {
      it++;
    }
  }
}

void rrc::rrc_meas::var_meas_cfg::report_triggers_interrat_removing_trigger(int32_t meas_id)
{
  // remove all cells in the cellsTriggeredList that are no neighbor cells anymore
  cell_triggered_t cells_triggered_list = meas_report->get_measId_cells(meas_id);
  auto             it                   = cells_triggered_list.begin();
  while (it != cells_triggered_list.end()) {
    if (not rrc_ptr->has_neighbour_cell_nr(it->earfcn, it->pci)) {
      logger.debug("MEAS:  Removing unknown PCI=%d from event trigger list", it->pci);
      it = cells_triggered_list.erase(it);
      meas_report->upd_measId(meas_id, cells_triggered_list);

      // if the cellsTriggeredList defined within the VarMeasReportList for this measId is empty:
      if (cells_triggered_list.empty()) {
        remove_varmeas_report(meas_id);
      }
    } else {
      it++;
    }
  }
}

void rrc::rrc_meas::var_meas_cfg::report_triggers_interrat_nr(uint32_t                meas_id,
                                                              report_cfg_inter_rat_s& report_cfg,
                                                              meas_obj_nr_r15_s&      meas_obj)
{
  if (report_cfg.trigger_type.type() == report_cfg_inter_rat_s::trigger_type_c_::types::event) {
    // if the triggerType is set to ‘event’ and if the entry condition applicable for this event,
    report_triggers_interrat_check_new(meas_id, report_cfg, meas_obj);
    report_triggers_interrat_check_leaving(meas_id, report_cfg);
    report_triggers_interrat_removing_trigger(meas_id);
  }
}
void rrc::rrc_meas::var_meas_cfg::report_triggers()
{
  // for each measId included in the measIdList within VarMeasConfig
  for (auto& m : measIdList) {
    if (!reportConfigList.count(m.second.report_cfg_id) || !measObjectsList.count(m.second.meas_obj_id)) {
      logger.error("MEAS:  Computing report triggers. MeasId=%d has invalid report or object settings", m.first);
      continue;
    }

    report_cfg_to_add_mod_s& report_cfg = reportConfigList.at(m.second.report_cfg_id);
    meas_obj_to_add_mod_s&   meas_obj   = measObjectsList.at(m.second.meas_obj_id);

    logger.debug("MEAS:  Calculating reports for MeasId=%d, ObjectId=%d (Type %s), ReportId=%d (Type %s)",
                 m.first,
                 m.second.meas_obj_id,
                 report_cfg.report_cfg.type().to_string(),
                 m.second.report_cfg_id,
                 meas_obj.meas_obj.type().to_string());

    if (meas_obj.meas_obj.type().value == meas_obj_to_add_mod_s::meas_obj_c_::types_opts::meas_obj_eutra &&
        report_cfg.report_cfg.type().value == report_cfg_to_add_mod_s::report_cfg_c_::types::report_cfg_eutra) {
      report_triggers_eutra(m.first, report_cfg.report_cfg.report_cfg_eutra(), meas_obj.meas_obj.meas_obj_eutra());
    } else if (meas_obj.meas_obj.type().value == meas_obj_to_add_mod_s::meas_obj_c_::types_opts::meas_obj_nr_r15 &&
               report_cfg.report_cfg.type().value ==
                   report_cfg_to_add_mod_s::report_cfg_c_::types::report_cfg_inter_rat) {
      report_triggers_interrat_nr(
          m.first, report_cfg.report_cfg.report_cfg_inter_rat(), meas_obj.meas_obj.meas_obj_nr_r15());
    } else {
      logger.error("Unsupported combination of measurement object type %s and report config type %s ",
                   meas_obj.meas_obj.type().to_string(),
                   report_cfg.report_cfg.type().to_string());
    }

    // upon expiry of the periodical reporting timer for this measId
    if (meas_report->is_timer_expired(m.first)) {
      meas_report->generate_report(m.first);
    }
  }
}

bool rrc::rrc_meas::var_meas_cfg::is_rsrp(report_cfg_eutra_s::trigger_quant_opts::options q)
{
  return q == report_cfg_eutra_s::trigger_quant_opts::rsrp;
}

void rrc::rrc_meas::var_meas_cfg::eval_triggers_eutra(uint32_t            meas_id,
                                                      report_cfg_eutra_s& report_cfg,
                                                      meas_obj_eutra_s&   meas_obj,
                                                      meas_cell_eutra*    serv_cell,
                                                      float               Ofs,
                                                      float               Ocs)
{
  auto asn1_quant_convert = [](report_cfg_eutra_s::trigger_quant_e_ q) {
    if (q == report_cfg_eutra_s::trigger_quant_opts::rsrp) {
      return quant_rsrp;
    } else {
      return quant_rsrq;
    }
  };

  eutra_event_s::event_id_c_ event_id = report_cfg.trigger_type.event().event_id;

  // For A1/A2 events, get serving cell from current carrier
  if (event_id.type().value < eutra_event_s::event_id_c_::types::event_a3 &&
      meas_obj.carrier_freq != serv_cell->get_earfcn()) {
    uint32_t scell_pci = 0;
    if (!rrc_ptr->meas_cells.get_scell_cc_idx(meas_obj.carrier_freq, scell_pci)) {
      logger.error("MEAS:  Could not find serving cell for carrier earfcn=%d", meas_obj.carrier_freq);
      return;
    }
    serv_cell = rrc_ptr->meas_cells.get_neighbour_cell_handle(meas_obj.carrier_freq, scell_pci);
    if (!serv_cell) {
      logger.error(
          "MEAS:  Could not find serving cell for carrier earfcn=%d and pci=%d", meas_obj.carrier_freq, scell_pci);
      return;
    }
  }

  double hyst = 0.5 * report_cfg.trigger_type.event().hysteresis;
  float  Ms   = is_rsrp(report_cfg.trigger_quant.value) ? serv_cell->get_rsrp() : serv_cell->get_rsrq();
  if (!std::isnormal(Ms)) {
    logger.debug("MEAS:  Serving cell Ms=%f invalid when evaluating triggers", Ms);
    return;
  }

  if (report_cfg.trigger_type.type() == report_cfg_eutra_s::trigger_type_c_::types::event) {
    // A1 & A2 are for serving cell only
    if (event_id.type().value < eutra_event_s::event_id_c_::types::event_a3) {
      float thresh          = 0.0;
      bool  enter_condition = false;
      bool  exit_condition  = false;
      if (event_id.type() == eutra_event_s::event_id_c_::types::event_a1) {
        if (event_id.event_a1().a1_thres.type().value == thres_eutra_c::types::thres_rsrp) {
          thresh = rrc_range_to_value(asn1_quant_convert(report_cfg.trigger_quant),
                                      event_id.event_a1().a1_thres.thres_rsrp());
        } else {
          thresh = rrc_range_to_value(asn1_quant_convert(report_cfg.trigger_quant),
                                      event_id.event_a1().a1_thres.thres_rsrq());
        }
        enter_condition = Ms - hyst > thresh;
        exit_condition  = Ms + hyst < thresh;
      } else {
        if (event_id.event_a2().a2_thres.type() == thres_eutra_c::types::thres_rsrp) {
          thresh = rrc_range_to_value(asn1_quant_convert(report_cfg.trigger_quant),
                                      event_id.event_a2().a2_thres.thres_rsrp());
        } else {
          thresh = rrc_range_to_value(asn1_quant_convert(report_cfg.trigger_quant),
                                      event_id.event_a2().a2_thres.thres_rsrq());
        }
        enter_condition = Ms + hyst < thresh;
        exit_condition  = Ms - hyst > thresh;
      }

      trigger_state[meas_id][serv_cell->get_pci()].event_condition(enter_condition, exit_condition);

      logger.debug("MEAS:  eventId=%s, Ms=%.2f, hyst=%.2f, Thresh=%.2f, enter_condition=%d, exit_condition=%d",
                   event_id.type().to_string(),
                   Ms,
                   hyst,
                   thresh,
                   enter_condition,
                   exit_condition);

      // Rest are evaluated for every cell in frequency
    } else {
      auto cells = rrc_ptr->get_cells(meas_obj.carrier_freq);
      for (auto& pci : cells) {
        logger.debug("MEAS:  eventId=%s, pci=%d, earfcn=%d", event_id.type().to_string(), pci, meas_obj.carrier_freq);
        float Ofn = offset_val(meas_obj);
        float Ocn = 0;
        // If the cell was provided by the configuration, check if it has an individual q_offset
        auto n = find_pci_in_meas_obj(meas_obj, pci);
        if (n != meas_obj.cells_to_add_mod_list.end()) {
          Ocn = n->cell_individual_offset.to_number();
        }
        float Mn = 0;
        if (is_rsrp(report_cfg.trigger_quant.value)) {
          Mn = rrc_ptr->get_cell_rsrp(meas_obj.carrier_freq, pci);
        } else {
          Mn = rrc_ptr->get_cell_rsrq(meas_obj.carrier_freq, pci);
        }
        double  Off    = 0;
        float   thresh = 0, th1 = 0, th2 = 0;
        bool    enter_condition = false;
        bool    exit_condition  = false;
        uint8_t range, range2;
        switch (event_id.type().value) {
          case eutra_event_s::event_id_c_::types::event_a3:
            Off             = 0.5 * event_id.event_a3().a3_offset;
            enter_condition = Mn + Ofn + Ocn - hyst > Ms + Ofs + Ocs + Off;
            exit_condition  = Mn + Ofn + Ocn + hyst < Ms + Ofs + Ocs + Off;
            break;
          case eutra_event_s::event_id_c_::types::event_a4:
            if (event_id.event_a4().a4_thres.type() == thres_eutra_c::types::thres_rsrp) {
              range = event_id.event_a4().a4_thres.thres_rsrp();
            } else {
              range = event_id.event_a4().a4_thres.thres_rsrq();
            }
            thresh          = rrc_range_to_value(asn1_quant_convert(report_cfg.trigger_quant.value), range);
            enter_condition = Mn + Ofn + Ocn - hyst > thresh;
            exit_condition  = Mn + Ofn + Ocn + hyst < thresh;
            break;
          case eutra_event_s::event_id_c_::types::event_a5:
            if (event_id.event_a5().a5_thres1.type() == thres_eutra_c::types::thres_rsrp) {
              range = event_id.event_a5().a5_thres1.thres_rsrp();
            } else {
              range = event_id.event_a5().a5_thres1.thres_rsrq();
            }
            if (event_id.event_a5().a5_thres2.type() == thres_eutra_c::types::thres_rsrp) {
              range2 = event_id.event_a5().a5_thres2.thres_rsrp();
            } else {
              range2 = event_id.event_a5().a5_thres2.thres_rsrq();
            }
            th1             = rrc_range_to_value(asn1_quant_convert(report_cfg.trigger_quant.value), range);
            th2             = rrc_range_to_value(asn1_quant_convert(report_cfg.trigger_quant.value), range2);
            enter_condition = (Ms + hyst < th1) && (Mn + Ofn + Ocn - hyst > th2);
            exit_condition  = (Ms - hyst > th1) && (Mn + Ofn + Ocn + hyst < th2);
            break;
          default:
            logger.error("Error event %s not implemented", event_id.type().to_string());
        }

        trigger_state[meas_id][pci].event_condition(enter_condition, exit_condition);

        logger.debug(
            "MEAS:  eventId=%s, pci=%d, Ms=%.2f, hyst=%.2f, Thresh=%.2f, enter_condition=%d, exit_condition=%d",
            event_id.type().to_string(),
            pci,
            Ms,
            hyst,
            thresh,
            enter_condition,
            exit_condition);
      }
    }
  }
}

void rrc::rrc_meas::var_meas_cfg::eval_triggers_interrat_nr(uint32_t                meas_id,
                                                            report_cfg_inter_rat_s& report_cfg,
                                                            meas_obj_nr_r15_s&      meas_obj)
{
  if (!(report_cfg.trigger_type.type() == report_cfg_inter_rat_s::trigger_type_c_::types::event)) {
    logger.error("Unsupported trigger type for interrat nr eval");
    return;
  }

  report_cfg_inter_rat_s::trigger_type_c_::event_s_::event_id_c_ event_id = report_cfg.trigger_type.event().event_id;

  double hyst = (double)report_cfg.trigger_type.event().hysteresis;

  auto cells = rrc_ptr->get_cells_nr(meas_obj.carrier_freq_r15);

  for (auto& pci : cells) {
    float thresh          = 0.0;
    bool  enter_condition = false;
    bool  exit_condition  = false;
    float Mn              = 0.0;

    logger.debug("MEAS:  eventId=%s, pci=%d, earfcn=%d", event_id.type().to_string(), pci, meas_obj.carrier_freq_r15);

    if (event_id.event_b1_nr_r15().b1_thres_nr_r15.type().value == thres_nr_r15_c::types::nr_rsrp_r15) {
      Mn     = rrc_ptr->get_cell_rsrp_nr(meas_obj.carrier_freq_r15, pci);
      thresh = range_to_value_nr(asn1::rrc::thres_nr_r15_c::types_opts::options::nr_rsrp_r15,
                                 event_id.event_b1_nr_r15().b1_thres_nr_r15.nr_rsrp_r15());
    } else {
      logger.warning("Other threshold values are not supported yet!");
    }

    enter_condition = Mn - hyst > thresh;
    exit_condition  = Mn + hyst < thresh;

    trigger_state_nr[meas_id][pci].event_condition(enter_condition, exit_condition);

    logger.debug("MEAS (NR):  eventId=%s, Mn=%.2f, hyst=%.2f, Thresh=%.2f, enter_condition=%d, exit_condition=%d",
                 event_id.type().to_string(),
                 Mn,
                 hyst,
                 thresh,
                 enter_condition,
                 exit_condition);
  }
}
/* Evaluate event trigger conditions for each cell 5.5.4 */
void rrc::rrc_meas::var_meas_cfg::eval_triggers()
{
  meas_cell_eutra* serv_cell = rrc_ptr->get_serving_cell();

  if (serv_cell == nullptr) {
    logger.warning("MEAS:  Serving cell not set when evaluating triggers");
    return;
  }

  uint32_t serving_earfcn = serv_cell->get_earfcn();
  uint32_t serving_pci    = serv_cell->get_pci();

  // Obtain serving cell specific offset
  float Ofs = 0;
  float Ocs = 0;

  meas_obj_to_add_mod_s* serving_obj = find_meas_obj_map(measObjectsList, serving_earfcn);
  if (serving_obj != nullptr &&
      serving_obj->meas_obj.type().value == meas_obj_to_add_mod_s::meas_obj_c_::types_opts::meas_obj_eutra) {
    Ofs                   = offset_val(serving_obj->meas_obj.meas_obj_eutra());
    auto serving_cell_off = find_pci_in_meas_obj(serving_obj->meas_obj.meas_obj_eutra(), serving_pci);
    if (serving_cell_off != serving_obj->meas_obj.meas_obj_eutra().cells_to_add_mod_list.end()) {
      Ocs = serving_cell_off->cell_individual_offset.to_number();
    }
  }

  for (auto& m : measIdList) {
    if (!reportConfigList.count(m.second.report_cfg_id) || !measObjectsList.count(m.second.meas_obj_id)) {
      logger.error("MEAS:  Computing report triggers. MeasId=%d has invalid report or object settings", m.first);
      continue;
    }

    report_cfg_to_add_mod_s& report_cfg = reportConfigList.at(m.second.report_cfg_id);
    meas_obj_to_add_mod_s&   meas_obj   = measObjectsList.at(m.second.meas_obj_id);

    logger.debug("MEAS:  Calculating trigger for MeasId=%d, ObjectId=%d (Type %s), ReportId=%d (Type %s)",
                 m.first,
                 m.second.meas_obj_id,
                 report_cfg.report_cfg.type().to_string(),
                 m.second.report_cfg_id,
                 meas_obj.meas_obj.type().to_string());

    if (meas_obj.meas_obj.type().value == meas_obj_to_add_mod_s::meas_obj_c_::types_opts::meas_obj_eutra &&
        report_cfg.report_cfg.type().value == report_cfg_to_add_mod_s::report_cfg_c_::types::report_cfg_eutra) {
      eval_triggers_eutra(
          m.first, report_cfg.report_cfg.report_cfg_eutra(), meas_obj.meas_obj.meas_obj_eutra(), serv_cell, Ofs, Ocs);
    } else if (meas_obj.meas_obj.type().value == meas_obj_to_add_mod_s::meas_obj_c_::types_opts::meas_obj_nr_r15 &&
               report_cfg.report_cfg.type().value ==
                   report_cfg_to_add_mod_s::report_cfg_c_::types::report_cfg_inter_rat)
      eval_triggers_interrat_nr(
          m.first, report_cfg.report_cfg.report_cfg_inter_rat(), meas_obj.meas_obj.meas_obj_nr_r15());
    else {
      logger.error("Unsupported combination of measurement object type %s and report config type %s ",
                   meas_obj.meas_obj.type().to_string(),
                   report_cfg.report_cfg.type().to_string());
    }
  }
}

/***
 *
 * varMeasConfig class
 *
 *
 */

void rrc::rrc_meas::var_meas_cfg::init(rrc* rrc_ptr_)
{
  rrc_ptr = rrc_ptr_;
  rrc_ptr = rrc_ptr_;
}

void rrc::rrc_meas::var_meas_cfg::reset()
{
  measIdList.clear();
  measObjectsList.clear();
  reportConfigList.clear();
}

rrc::rrc_meas::phy_quant_t rrc::rrc_meas::var_meas_cfg::get_filter_a()
{
  return filter_a;
}

// stop the periodical reporting timer or timer T321, whichever one is running, and reset the associated
// information (e.g. timeToTrigger) for this measId
// All these is done automatically by the destructor
void rrc::rrc_meas::var_meas_cfg::remove_measId(const uint32_t measId)
{
  measIdList.erase(measId);
}

void rrc::rrc_meas::var_meas_cfg::remove_varmeas_report(const uint32_t meas_id)
{
  meas_report->remove_varmeas_report(meas_id);
  trigger_state.erase(meas_id);
  trigger_state_nr.erase(meas_id);
}

std::list<meas_obj_to_add_mod_s> rrc::rrc_meas::var_meas_cfg::get_active_objects()
{
  std::list<meas_obj_to_add_mod_s> r;
  for (auto& m : measIdList) {
    if (measObjectsList.count(m.second.meas_obj_id)) {
      r.push_back(measObjectsList.at(m.second.meas_obj_id));
    }
  }
  if (logger.debug.enabled()) {
    logger.debug("MEAS:  Returning %zd active objects", r.size());
    for (auto& o : r) {
      switch (o.meas_obj.type().value) {
        case meas_obj_to_add_mod_s::meas_obj_c_::types_opts::meas_obj_eutra:
          logger.debug("MEAS:      carrier_freq=%d, %u cells",
                       o.meas_obj.meas_obj_eutra().carrier_freq,
                       o.meas_obj.meas_obj_eutra().cells_to_add_mod_list.size());
          break;
        case meas_obj_to_add_mod_s::meas_obj_c_::types_opts::meas_obj_nr_r15:
          logger.debug("MEAS:      NR: carrier_freq=%d", o.meas_obj.meas_obj_nr_r15().carrier_freq_r15);
        default:
          break;
      }
    }
  }
  // we do a copy of all the structs here but this function is only called during reconfiguration
  return r;
}

// Procedure upon handover or reestablishment 5.5.6.1
void rrc::rrc_meas::var_meas_cfg::ho_reest_finish(const uint32_t src_earfcn, const uint32_t dst_earfcn)
{
  logger.info("MEAS:  Actions upon handover and reestablishment, src_earfcn=%d, dst_earfcn=%d", src_earfcn, dst_earfcn);

  // for each measId included in the measIdList within VarMeasConfig, if the triggerType is set to ‘periodical’, remove
  // this measId from the measIdList within VarMeasConfig
  {
    auto it = measIdList.begin();
    while (it != measIdList.end()) {
      if (reportConfigList.count(it->second.report_cfg_id) &&
          reportConfigList.at(it->second.report_cfg_id).report_cfg.type().value ==
              report_cfg_to_add_mod_s::report_cfg_c_::types_opts::report_cfg_eutra &&
          reportConfigList.at(it->second.report_cfg_id).report_cfg.report_cfg_eutra().trigger_type.type().value ==
              report_cfg_eutra_s::trigger_type_c_::types_opts::periodical) {
        it = measIdList.erase(it);
      } else {
        it++;
      }
    }
  }

  if (logger.debug.enabled()) {
    logger.debug("MEAS:  measId before HO");
    for (auto& m : measIdList) {
      logger.debug("MEAS:    measId=%d, measObjectId=%d", m.first, m.second.meas_obj_id);
    }
  }

  // if the procedure was triggered due to inter-frequency handover or successful re-establishment to an inter-
  // frequency cell
  if (src_earfcn != dst_earfcn) {
    auto src_obj = std::find_if(measObjectsList.begin(),
                                measObjectsList.end(),
                                [&src_earfcn](const std::pair<uint32_t, meas_obj_to_add_mod_s>& c) {
                                  return c.second.meas_obj.meas_obj_eutra().carrier_freq == src_earfcn;
                                });
    auto dst_obj = std::find_if(measObjectsList.begin(),
                                measObjectsList.end(),
                                [&dst_earfcn](const std::pair<uint32_t, meas_obj_to_add_mod_s>& c) {
                                  return c.second.meas_obj.meas_obj_eutra().carrier_freq == dst_earfcn;
                                });
    if (dst_obj != measObjectsList.end()) {
      for (auto& m : measIdList) {
        // if the measId value is linked to the measObjectId value corresponding to the source carrier frequency
        if (m.second.meas_obj_id == src_obj->first) {
          // link this measId value to the measObjectId value corresponding to the target carrier frequency
          m.second.meas_obj_id = dst_obj->first;
          // else if the measId value is linked to the measObjectId value corresponding to the target carrier
          // frequency
        } else if (m.second.meas_obj_id == dst_obj->first) {
          // link this measId value to the measObjectId value corresponding to the source carrier frequency
          m.second.meas_obj_id = src_obj->first;
        }
      }
    } else {
      // remove all measId values that are linked to the measObjectId value corresponding to the source carrier
      // frequency
      auto it = measIdList.begin();
      while (it != measIdList.end()) {
        if (it->second.meas_obj_id == src_obj->first) {
          it = measIdList.erase(it);
        } else {
          it++;
        }
      }
    }
  }

  if (logger.debug.enabled()) {
    logger.debug("MEAS:  measId after HO");
    for (auto& m : measIdList) {
      logger.debug("MEAS:    measId=%d, measObjectId=%d", m.first, m.second.meas_obj_id);
    }
  }

  meas_report->remove_all_varmeas_reports();
  trigger_state.clear();
}

// Measurement object removal 5.5.2.4
void rrc::rrc_meas::var_meas_cfg::measObject_removal(const meas_obj_to_rem_list_l& list)
{
  for (auto& l : list) {
    if (measObjectsList.count(l)) {
      // Remove entry from measObjectList
      measObjectsList.erase(l);
      // Remove all entries in measIdList associated with this objectId
      auto it = measIdList.begin();
      while (it != measIdList.end()) {
        if (it->second.meas_obj_id == l) {
          logger.info("MEAS:  Removed measId=%d", it->first);
          remove_varmeas_report(it->first); // Remove report before `it` is updated with the next pointer
          it = measIdList.erase(it);
        } else {
          it++;
        }
      }
      logger.info("MEAS: Removed measObjectId=%d", l);
    }
  }
}

void rrc::rrc_meas::var_meas_cfg::measObject_addmod_eutra(const meas_obj_to_add_mod_s& l)
{
  bool entry_exists = measObjectsList.count(l.meas_obj_id) > 0;
  if (!entry_exists) {
    // add a new entry for the received measObject to the measObjectList within VarMeasConfig
    measObjectsList.emplace(l.meas_obj_id, l);
  }

  meas_obj_eutra_s cfg_obj                      = l.meas_obj.meas_obj_eutra();
  measObjectsList.at(l.meas_obj_id).meas_obj_id = l.meas_obj_id;
  // Assert choice will check if existing meas object is of type eutra
  meas_obj_eutra_s& local_obj = measObjectsList.at(l.meas_obj_id).meas_obj.meas_obj_eutra();

  // if an entry with the matching measObjectId exists in the measObjectList within the VarMeasConfig
  if (entry_exists) {
    // Update carrier frequency0
    local_obj.carrier_freq    = cfg_obj.carrier_freq;
    local_obj.allowed_meas_bw = cfg_obj.allowed_meas_bw;

    // Combine the new cells with the existing ones and remove the cells indicated in config
    {
      // Remove cells
      if (cfg_obj.cells_to_rem_list_present) {
        logger.debug("MEAS:     Removing %d cells", cfg_obj.cells_to_rem_list.size());
        apply_remlist_diff(local_obj.cells_to_add_mod_list, cfg_obj.cells_to_rem_list, local_obj.cells_to_add_mod_list);

        if (logger.debug.enabled()) {
          for (auto& c : local_obj.cells_to_add_mod_list) {
            logger.debug("MEAS:       cell idx=%d, pci=%d, q_offset=%d",
                         c.cell_idx,
                         c.pci,
                         c.cell_individual_offset.to_number());
          }
        }
      }
      if (cfg_obj.cells_to_add_mod_list_present) {
        apply_addmodlist_diff(
            local_obj.cells_to_add_mod_list, cfg_obj.cells_to_add_mod_list, local_obj.cells_to_add_mod_list);
      }
    }

    // Do the same with excluded list
    {
      if (cfg_obj.excluded_cells_to_rem_list_present) {
        apply_remlist_diff(local_obj.excluded_cells_to_add_mod_list,
                           cfg_obj.excluded_cells_to_rem_list,
                           local_obj.excluded_cells_to_add_mod_list);
      }
      if (cfg_obj.excluded_cells_to_add_mod_list_present) {
        apply_addmodlist_diff(local_obj.excluded_cells_to_add_mod_list,
                              cfg_obj.excluded_cells_to_add_mod_list,
                              local_obj.excluded_cells_to_add_mod_list);
      }
    }

    // for each measId associated with this measObjectId in the measIdList within the VarMeasConfig
    for (auto& m : measIdList) {
      if (m.second.meas_obj_id == l.meas_obj_id) {
        remove_varmeas_report(m.first);
      }
    }
  }

  logger.info("MEAS:  %s objectId=%d, carrier_freq=%d, %u cells, %u excluded-listed cells",
              !entry_exists ? "Added" : "Modified",
              l.meas_obj_id,
              local_obj.carrier_freq,
              local_obj.cells_to_add_mod_list.size(),
              local_obj.excluded_cells_to_add_mod_list.size());
  if (logger.debug.enabled()) {
    for (auto& c : local_obj.cells_to_add_mod_list) {
      logger.debug(
          "MEAS:       cell idx=%d, pci=%d, q_offset=%d", c.cell_idx, c.pci, c.cell_individual_offset.to_number());
    }
    for (auto& b : local_obj.excluded_cells_to_add_mod_list) {
      logger.debug("MEAS:       excluded-listed cell idx=%d", b.cell_idx);
    }
  }
}

void rrc::rrc_meas::var_meas_cfg::measObject_addmod_nr_r15(const meas_obj_to_add_mod_s& l)
{
  bool entry_exists = measObjectsList.count(l.meas_obj_id) > 0;
  if (!entry_exists) {
    // add a new entry for the received measObject to the measObjectList within VarMeasConfig
    measObjectsList.emplace(l.meas_obj_id, l);
  }

  meas_obj_nr_r15_s cfg_obj                     = l.meas_obj.meas_obj_nr_r15();
  measObjectsList.at(l.meas_obj_id).meas_obj_id = l.meas_obj_id;
  meas_obj_nr_r15_s& local_obj                  = measObjectsList.at(l.meas_obj_id).meas_obj.meas_obj_nr_r15();
  // if an entry with the matching measObjectId exists in the measObjectList within the VarMeasConfig
  if (entry_exists) {
    // Update carrier frequency0
    local_obj.carrier_freq_r15 = cfg_obj.carrier_freq_r15;

    // Combine the new cells with the existing ones and remove the cells indicated in config
    // Do the same with excluded list
    {
      if (cfg_obj.excluded_cells_to_rem_list_r15_present) {
        apply_remlist_diff(local_obj.excluded_cells_to_add_mod_list_r15,
                           cfg_obj.excluded_cells_to_rem_list_r15,
                           local_obj.excluded_cells_to_add_mod_list_r15);
      }
      if (cfg_obj.excluded_cells_to_add_mod_list_r15_present) {
        apply_addmodlist_diff(local_obj.excluded_cells_to_add_mod_list_r15,
                              cfg_obj.excluded_cells_to_add_mod_list_r15,
                              local_obj.excluded_cells_to_add_mod_list_r15);
      }
    }
    // for each measId associated with this measObjectId in the measIdList within the VarMeasConfig
    for (auto& m : measIdList) {
      if (m.second.meas_obj_id == l.meas_obj_id) {
        remove_varmeas_report(m.first);
      }
    }
  }

  logger.info("MEAS (NR R15):  %s objectId=%d, carrier_freq=%d, %u excluded-listed cells",
              !entry_exists ? "Added" : "Modified",
              l.meas_obj_id,
              local_obj.carrier_freq_r15,
              local_obj.excluded_cells_to_add_mod_list_r15.size());
  if (logger.debug.enabled()) {
    for (auto& b : local_obj.excluded_cells_to_add_mod_list_r15) {
      logger.debug("MEAS:       excluded-listed cell idx=%d", b.cell_idx_r15);
    }
  }
}

// Measurement object addition/modification Section 5.5.2.5
void rrc::rrc_meas::var_meas_cfg::measObject_addmod(const meas_obj_to_add_mod_list_l& list)
{
  for (auto& l : list) {
    switch (l.meas_obj.type().value) {
      case meas_obj_to_add_mod_s::meas_obj_c_::types_opts::meas_obj_eutra:
        measObject_addmod_eutra(l);
        break;
      case meas_obj_to_add_mod_s::meas_obj_c_::types_opts::meas_obj_nr_r15:
        measObject_addmod_nr_r15(l);
        break;
      default:
        logger.error("Unsupported measObject type: %s", l.meas_obj.type().to_string());
        break;
    }
  }
}

// perform the reporting configuration removal procedure as specified in 5.5.2.6;
void rrc::rrc_meas::var_meas_cfg::reportConfig_removal(const report_cfg_to_rem_list_l& list)
{
  for (auto& l : list) {
    if (reportConfigList.count(l)) {
      // Remove entry from measObjectList
      reportConfigList.erase(l);
      // Remove all entries in measIdList associated with this objectId
      auto it = measIdList.begin();
      while (it != measIdList.end()) {
        if (it->second.report_cfg_id == l) {
          logger.info("MEAS:  Removed measId=%d", it->first);
          remove_varmeas_report(it->first); // Remove report before `it` is updated with the next pointer
          it = measIdList.erase(it);
        } else {
          it++;
        }
      }
      logger.info("MEAS:  Removed reportObjectId=%d", l);
    }
  }
}

bool rrc::rrc_meas::var_meas_cfg::reportConfig_addmod_to_reportConfigList(const report_cfg_to_add_mod_s& l)
{
  bool entry_exists = reportConfigList.count(l.report_cfg_id) > 0;
  if (entry_exists) {
    reportConfigList.at(l.report_cfg_id) = l;
    // for each measId associated with this reportConfigId in the measIdList within the VarMeasConfig
    for (auto& m : measIdList) {
      if (m.second.report_cfg_id == l.report_cfg_id) {
        remove_varmeas_report(m.first);
      }
    }
  } else {
    reportConfigList.emplace(l.report_cfg_id, l);
  }
  return entry_exists;
}

void rrc::rrc_meas::var_meas_cfg::reportConfig_addmod_eutra(const report_cfg_to_add_mod_s& l)
{
  const report_cfg_eutra_s& report_cfg = l.report_cfg.report_cfg_eutra();

  if (!(report_cfg.trigger_type.type().value == report_cfg_eutra_s::trigger_type_c_::types_opts::event)) {
    logger.error("MEAS:  Periodical reports not supported. Received in reportConfigId=%d", l.report_cfg_id);
    return;
  }
  bool entry_exists = reportConfig_addmod_to_reportConfigList(l);
  logger.info("MEAS:  %s reportConfig id=%d, event-type=%s, time-to-trigger=%d ms, reportInterval=%d",
              !entry_exists ? "Added" : "Modified",
              l.report_cfg_id,
              report_cfg.trigger_type.event().event_id.type().to_string(),
              report_cfg.trigger_type.event().time_to_trigger.to_number(),
              report_cfg.report_interv.to_number());
  if (entry_exists) {
    log_debug_trigger_value_eutra(report_cfg.trigger_type.event().event_id);
  }
}

void rrc::rrc_meas::var_meas_cfg::reportConfig_addmod_interrat(const report_cfg_to_add_mod_s& l)
{
  const report_cfg_inter_rat_s& report_cfg = l.report_cfg.report_cfg_inter_rat();
  if (!(report_cfg.trigger_type.type().value == report_cfg_inter_rat_s::trigger_type_c_::types_opts::event)) {
    logger.error("MEAS:  Periodical reports not supported. Received in reportConfigId=%d", l.report_cfg_id);
    return;
  }
  bool entry_exists = reportConfig_addmod_to_reportConfigList(l);
  logger.info("MEAS: Inter RAT %s reportConfig id=%d, event-type=%s, time-to-trigger=%d ms, reportInterval=%d",
              !entry_exists ? "Added" : "Modified",
              l.report_cfg_id,
              report_cfg.trigger_type.event().event_id.type().to_string(),
              report_cfg.trigger_type.event().time_to_trigger.to_number(),
              report_cfg.report_interv.to_number());
  log_debug_trigger_value_interrat(report_cfg.trigger_type.event().event_id);
}

// perform the reporting configuration addition/ modification procedure as specified in 5.5.2.7
void rrc::rrc_meas::var_meas_cfg::reportConfig_addmod(const report_cfg_to_add_mod_list_l& list)
{
  for (auto& l : list) {
    switch (l.report_cfg.type()) {
      case report_cfg_to_add_mod_s::report_cfg_c_::types_opts::report_cfg_eutra:
        reportConfig_addmod_eutra(l);
        break;
      case report_cfg_to_add_mod_s::report_cfg_c_::types_opts::report_cfg_inter_rat:
        reportConfig_addmod_interrat(l);
        break;
      default:
        logger.error("MEAS: Unsupported reportConfig type: %s", l.report_cfg.type().to_string());
        break;
    }
  }
}

// Warning: Use for Test debug purposes only. Assumes thresholds in RSRP
void rrc::rrc_meas::var_meas_cfg::log_debug_trigger_value_interrat(
    const report_cfg_inter_rat_s::trigger_type_c_::event_s_::event_id_c_& e)
{
  if (logger.debug.enabled()) {
    switch (e.type()) {
      case report_cfg_inter_rat_s::trigger_type_c_::event_s_::event_id_c_::types_opts::event_b1_nr_r15: {
        logger.debug("MEAS:   B1-NR-R15-threashold (%d)=%.1f dBm",
                     e.event_b1_nr_r15().b1_thres_nr_r15.nr_rsrp_r15(),
                     range_to_value_nr(asn1::rrc::thres_nr_r15_c::types_opts::options::nr_rsrp_r15,
                                       e.event_b1_nr_r15().b1_thres_nr_r15.nr_rsrp_r15()));
        break;
      }
      default:
        logger.debug("MEAS:     Unsupported inter rat trigger type %s", e.type().to_string());
        break;
    }
  }
}

// Warning: Use for Test debug purposes only. Assumes thresholds in RSRP
void rrc::rrc_meas::var_meas_cfg::log_debug_trigger_value_eutra(const eutra_event_s::event_id_c_& e)
{
  if (logger.debug.enabled()) {
    switch (e.type()) {
      case eutra_event_s::event_id_c_::types_opts::event_a1:
        logger.debug("MEAS:     A1-threshold=%.1f dBm",
                     rrc_range_to_value(quant_rsrp, e.event_a1().a1_thres.thres_rsrp()));
        break;
      case eutra_event_s::event_id_c_::types_opts::event_a2:
        logger.debug("MEAS:     A2-threshold=%.1f dBm",
                     rrc_range_to_value(quant_rsrp, e.event_a2().a2_thres.thres_rsrp()));
        break;
      case eutra_event_s::event_id_c_::types_opts::event_a3:
        logger.debug("MEAS:     A3-offset=%.1f dB", rrc_range_to_value(quant_rsrp, e.event_a3().a3_offset));
        break;
      case eutra_event_s::event_id_c_::types_opts::event_a4:
        logger.debug("MEAS:     A4-threshold=%.1f dBm",
                     rrc_range_to_value(quant_rsrp, e.event_a4().a4_thres.thres_rsrp()));
        break;
      default:
        logger.debug("MEAS:     Unsupported");
        break;
    }
  }
}

// perform the measurement identity removal procedure as specified in 5.5.2.2
void rrc::rrc_meas::var_meas_cfg::measId_removal(const meas_id_to_rem_list_l& list)
{
  for (auto& l : list) {
    // for each measId included in the received measIdToRemoveList that is part of the current UE configuration in
    // varMeasConfig
    if (measIdList.count(l)) {
      measIdList.erase(l);
      remove_varmeas_report(l);
      logger.info("MEAS:  Removed measId=%d", l);
    }
  }
}

// perform the measurement identity addition/ modification procedure as specified in 5.5.2.3
void rrc::rrc_meas::var_meas_cfg::measId_addmod(const meas_id_to_add_mod_list_l& list)
{
  for (auto& l : list) {
    // configure only if report and object exists
    if (reportConfigList.count(l.report_cfg_id) == 0) {
      logger.error("MEAS:  Adding measId=%d, reportConfigId=%d doesn't exist", l.meas_id, l.report_cfg_id);
      continue;
    }
    if (measObjectsList.count(l.meas_obj_id) == 0) {
      logger.error("MEAS:  Adding measId=%d, measObjectId=%d doesn't exist", l.meas_id, l.meas_obj_id);
      continue;
    }
    // add/replace entry in VarMeasConfig. Safe to use [] operator here
    measIdList[l.meas_id] = l;

    // remove the measurement reporting entry for this measId from the VarMeasReportList, if included
    remove_varmeas_report(l.meas_id);

    logger.info(
        "MEAS:  AddMod measId=%d, measObjectId=%d, reportConfigId=%d", l.meas_id, l.meas_obj_id, l.report_cfg_id);
  }
}

// perform the quantity configuration procedure as specified in 5.5.2.8;
void rrc::rrc_meas::var_meas_cfg::quantity_config(const quant_cfg_s& cfg)
{
  // set the parameter quantityConfig within VarMeasConfig to the received value of quantityConfig
  // we compute filter coefficients here
  if (cfg.quant_cfg_eutra_present) {
    uint32_t k_rsrp = 0, k_rsrq = 0;
    if (cfg.quant_cfg_eutra.filt_coef_rsrp_present) {
      k_rsrp = cfg.quant_cfg_eutra.filt_coef_rsrp.to_number();
    } else {
      k_rsrp = filt_coef_e(filt_coef_e::fc4).to_number();
    }
    if (cfg.quant_cfg_eutra.filt_coef_rsrq_present) {
      k_rsrq = cfg.quant_cfg_eutra.filt_coef_rsrq.to_number();
    } else {
      k_rsrq = filt_coef_e(filt_coef_e::fc4).to_number();
    }
    filter_a.rsrp = powf(0.5f, (float)k_rsrp / 4.0f);
    filter_a.rsrq = powf(0.5f, (float)k_rsrq / 4.0f);

    logger.info("MEAS:  Quantity configuration k_rsrp=%d, k_rsrq=%d", k_rsrp, k_rsrq);

    // for each measId included in the measIdList within VarMeasConfig
    for (auto& m : measIdList) {
      remove_varmeas_report(m.first);
    }
  }
}

bool rrc::rrc_meas::var_meas_cfg::parse_meas_config(const meas_cfg_s* cfg, bool is_ho_reest, uint32_t src_earfcn)
{
  // if the received measConfig includes the measObjectToRemoveList
  if (cfg->meas_obj_to_rem_list_present) {
    measObject_removal(cfg->meas_obj_to_rem_list);
  }
  // if the received measConfig includes the measObjectToAddModList
  if (cfg->meas_obj_to_add_mod_list_present) {
    measObject_addmod(cfg->meas_obj_to_add_mod_list);
  }

  // if the received measConfig includes the reportConfigToRemoveList
  if (cfg->report_cfg_to_rem_list_present) {
    reportConfig_removal(cfg->report_cfg_to_rem_list);
  }
  // if the received measConfig includes the reportConfigToAddModList
  if (cfg->report_cfg_to_add_mod_list_present) {
    reportConfig_addmod(cfg->report_cfg_to_add_mod_list);
  }

  // if the received measConfig includes the quantityConfig
  if (cfg->quant_cfg_present) {
    quantity_config(cfg->quant_cfg);
  }

  // if the received measConfig includes the measIdToRemoveList
  if (cfg->meas_id_to_rem_list_present) {
    measId_removal(cfg->meas_id_to_rem_list);
  }
  // if the received measConfig includes the measIdToAddModList
  if (cfg->meas_id_to_add_mod_list_present) {
    measId_addmod(cfg->meas_id_to_add_mod_list);
  }

  // if the received measConfig includes the s-Measure
  if (cfg->s_measure_present) {
    // set the parameter s-Measure within VarMeasConfig to the lowest value of the RSRP ranges indicated by the
    // received value of s-Measure
    if (cfg->s_measure) {
      s_measure_value = rrc_range_to_value(quant_rsrp, cfg->s_measure);
    }
  }

  // According to 5.5.6.1, if the new configuration after a HO/Reest does not configure the target frequency, we need
  // to swap frequencies with source
  if (is_ho_reest) {
    meas_cell_eutra* serv_cell = rrc_ptr->get_serving_cell();
    if (serv_cell) {
      // Check if the target frequency is configured
      uint32_t target_earfcn = serv_cell->get_earfcn();
      if (std::find_if(measIdList.begin(), measIdList.end(), [&](const std::pair<uint32_t, meas_id_to_add_mod_s>& c) {
            return measObjectsList.count(c.second.meas_obj_id) &&
                   measObjectsList.at(c.second.meas_obj_id).meas_obj.meas_obj_eutra().carrier_freq == target_earfcn;
          }) == measIdList.end()) {
        // Run HO procedure
        ho_reest_finish(src_earfcn, target_earfcn);
      }
    } else {
      logger.warning("MEAS:  Could not get serving cell earfcn");
    }
  }
  return true;
}

void rrc::rrc_meas::var_meas_cfg::cell_trigger_state::event_condition(const bool enter, const bool exit)
{
  if (enter) {
    nof_tti_enter++;
    nof_tti_exit = 0;
  } else if (exit) {
    nof_tti_enter = 0;
    nof_tti_exit++;
  } else {
    nof_tti_enter = 0;
    nof_tti_exit  = 0;
  }
}

bool rrc::rrc_meas::var_meas_cfg::cell_trigger_state::is_enter_equal(const uint32_t nof_tti)
{
  return nof_tti < nof_tti_enter;
}

bool rrc::rrc_meas::var_meas_cfg::cell_trigger_state::is_exit_equal(const uint32_t nof_tti)
{
  return nof_tti < nof_tti_exit;
}

} // namespace srsue
