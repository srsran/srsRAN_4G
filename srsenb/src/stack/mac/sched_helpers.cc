/**
 * Copyright 2013-2022 Software Radio Systems Limited
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

#include "srsenb/hdr/stack/mac/sched_helpers.h"
#include "srsran/common/standard_streams.h"
#include "srsran/common/string_helpers.h"
#include "srsran/mac/pdu.h"
#include "srsran/srslog/bundled/fmt/format.h"
#include <array>

#define Debug(fmt, ...) get_mac_logger().debug(fmt, ##__VA_ARGS__)
#define Info(fmt, ...) get_mac_logger().info(fmt, ##__VA_ARGS__)
#define Warning(fmt, ...) get_mac_logger().warning(fmt, ##__VA_ARGS__)
#define Error(fmt, ...) get_mac_logger().error(fmt, ##__VA_ARGS__)

namespace srsenb {

using dl_sched_res_t    = sched_interface::dl_sched_res_t;
using dl_sched_data_t   = sched_interface::dl_sched_data_t;
using custom_mem_buffer = fmt::basic_memory_buffer<char, 1024>;

static srslog::basic_logger& get_mac_logger()
{
  static srslog::basic_logger& mac_logger = srslog::fetch_basic_logger("MAC");
  return mac_logger;
}

const char* to_string_short(srsran_dci_format_t dcifmt)
{
  switch (dcifmt) {
    case SRSRAN_DCI_FORMAT0:
      return "0";
    case SRSRAN_DCI_FORMAT1:
      return "1";
    case SRSRAN_DCI_FORMAT1A:
      return "1A";
    case SRSRAN_DCI_FORMAT1B:
      return "1B";
    case SRSRAN_DCI_FORMAT2:
      return "2";
    case SRSRAN_DCI_FORMAT2A:
      return "2A";
    case SRSRAN_DCI_FORMAT2B:
      return "2B";
    default:
      return "unknown";
  }
}

void fill_dl_cc_result_info(custom_mem_buffer& strbuf, const dl_sched_data_t& data)
{
  uint32_t first_ce = sched_interface::MAX_RLC_PDU_LIST;
  for (uint32_t i = 0; i < data.nof_pdu_elems[0]; ++i) {
    if (srsran::is_mac_ce(static_cast<srsran::dl_sch_lcid>(data.pdu[i]->lcid))) {
      first_ce = i;
      break;
    }
  }
  if (first_ce == sched_interface::MAX_RLC_PDU_LIST) {
    return;
  }
  const char* prefix = strbuf.size() > 0 ? " | " : "";
  fmt::format_to(strbuf, "{}rnti=0x{:0x}: [", prefix, data.dci.rnti);
  bool ces_found = false;
  for (uint32_t i = 0; i < data.nof_pdu_elems[0]; ++i) {
    const auto& pdu          = data.pdu[0][i];
    prefix                   = (ces_found) ? " | " : "";
    srsran::dl_sch_lcid lcid = static_cast<srsran::dl_sch_lcid>(pdu.lcid);
    if (srsran::is_mac_ce(lcid)) {
      fmt::format_to(strbuf, "{}CE \"{}\"", prefix, srsran::to_string_short(lcid));
      ces_found = true;
    }
  }
  fmt::format_to(strbuf, "]");
}

void fill_dl_cc_result_debug(custom_mem_buffer& strbuf, const dl_sched_data_t& data)
{
  if (data.nof_pdu_elems[0] == 0 and data.nof_pdu_elems[1] == 0) {
    return;
  }
  fmt::format_to(strbuf,
                 "  > rnti=0x{:0x}, tbs={}, f={}, mcs={}: [",
                 data.dci.rnti,
                 data.tbs[0],
                 to_string_short(data.dci.format),
                 data.dci.tb[0].mcs_idx);
  for (uint32_t tb = 0; tb < SRSRAN_MAX_TB; ++tb) {
    for (uint32_t i = 0; i < data.nof_pdu_elems[tb]; ++i) {
      const auto&         pdu    = data.pdu[tb][i];
      const char*         prefix = (i == 0) ? "" : " | ";
      srsran::dl_sch_lcid lcid   = static_cast<srsran::dl_sch_lcid>(pdu.lcid);
      if (srsran::is_mac_ce(lcid)) {
        fmt::format_to(strbuf, "{}CE \"{}\"", prefix, srsran::to_string_short(lcid));
      } else {
        fmt::format_to(strbuf, "{}SDU lcid={}, tb={}, len={} B", prefix, pdu.lcid, tb, pdu.nbytes);
      }
    }
  }
  fmt::format_to(strbuf, "]");
}

void log_dl_cc_results(srslog::basic_logger& logger, uint32_t enb_cc_idx, const sched_interface::dl_sched_res_t& result)
{
  if (!logger.info.enabled() && !logger.debug.enabled()) {
    return;
  }

  custom_mem_buffer strbuf;
  for (const auto& data : result.data) {
    if (logger.debug.enabled()) {
      fill_dl_cc_result_debug(strbuf, data);
    } else {
      fill_dl_cc_result_info(strbuf, data);
    }
  }
  if (strbuf.size() != 0) {
    if (logger.debug.enabled()) {
      logger.debug("SCHED: DL MAC PDU payload cc=%d:\n%s", enb_cc_idx, srsran::to_c_str(strbuf));
    } else {
      logger.info("SCHED: DL MAC CEs cc=%d: %s", enb_cc_idx, srsran::to_c_str(strbuf));
    }
  }
}

void log_phich_cc_results(srslog::basic_logger&                  logger,
                          uint32_t                               enb_cc_idx,
                          const sched_interface::ul_sched_res_t& result)
{
  using phich_t = sched_interface::ul_sched_phich_t;
  if (!logger.debug.enabled()) {
    return;
  }
  custom_mem_buffer strbuf;
  for (uint32_t i = 0; i < result.phich.size(); ++i) {
    const phich_t& phich  = result.phich[i];
    const char*    prefix = strbuf.size() > 0 ? " | " : "";
    const char*    val    = phich.phich == phich_t::ACK ? "ACK" : "NACK";
    fmt::format_to(strbuf, "{}rnti=0x{:0x}, val={}", prefix, phich.rnti, val);
  }
  if (strbuf.size() != 0) {
    logger.debug("SCHED: Allocated PHICHs, cc=%d: [%s]", enb_cc_idx, srsran::to_c_str(strbuf));
  }
}

/*******************************************************
 *                 Sched Params
 *******************************************************/

sched_cell_params_t::dl_nof_re_table generate_nof_re_table(const srsran_cell_t& cell)
{
  sched_cell_params_t::dl_nof_re_table table(cell.nof_prb);

  srsran_dl_sf_cfg_t dl_sf    = {};
  dl_sf.sf_type               = SRSRAN_SF_NORM;
  dl_sf.tdd_config.configured = false;

  for (uint32_t cfi = 0; cfi < SRSRAN_NOF_CFI; ++cfi) {
    dl_sf.cfi = cfi + 1;
    for (uint32_t sf_idx = 0; sf_idx < SRSRAN_NOF_SF_X_FRAME; ++sf_idx) {
      dl_sf.tti = sf_idx;
      for (uint32_t s = 0; s < SRSRAN_NOF_SLOTS_PER_SF; ++s) {
        for (uint32_t n = 0; n < cell.nof_prb; ++n) {
          table[n][sf_idx][s][cfi] = ra_re_x_prb(&cell, &dl_sf, s, n);
        }
      }
    }
  }
  return table;
}

sched_cell_params_t::dl_lb_nof_re_table get_lb_nof_re_x_prb(const sched_cell_params_t::dl_nof_re_table& table)
{
  sched_cell_params_t::dl_lb_nof_re_table ret;
  for (uint32_t sf_idx = 0; sf_idx < SRSRAN_NOF_SF_X_FRAME; ++sf_idx) {
    ret[sf_idx].resize(table.size());
    srsran::bounded_vector<uint32_t, SRSRAN_MAX_PRB> re_prb_vec(table.size());
    for (uint32_t p = 0; p < table.size(); ++p) {
      for (uint32_t s = 0; s < SRSRAN_NOF_SLOTS_PER_SF; ++s) {
        // assume max CFI to compute lower bound
        re_prb_vec[p] += table[p][sf_idx][s][SRSRAN_NOF_CFI - 1];
      }
    }

    srsran::bounded_vector<uint32_t, SRSRAN_MAX_PRB> re_prb_vec2(re_prb_vec.size());
    std::copy(re_prb_vec.begin(), re_prb_vec.end(), re_prb_vec2.begin());
    // pick intervals of PRBs with the lowest sum of REs
    ret[sf_idx][0] = *std::min_element(re_prb_vec2.begin(), re_prb_vec2.end());
    for (uint32_t p = 1; p < table.size(); ++p) {
      std::transform(re_prb_vec2.begin(),
                     re_prb_vec2.end() - 1,
                     re_prb_vec.begin() + p,
                     re_prb_vec2.begin(),
                     std::plus<uint32_t>());
      re_prb_vec2.pop_back();
      ret[sf_idx][p] = *std::min_element(re_prb_vec2.begin(), re_prb_vec2.end());
    }
  }
  return ret;
}

void sched_cell_params_t::regs_deleter::operator()(srsran_regs_t* p)
{
  if (p != nullptr) {
    srsran_regs_free(p);
    delete p;
  }
}

bool sched_cell_params_t::set_cfg(uint32_t                             enb_cc_idx_,
                                  const sched_interface::cell_cfg_t&   cfg_,
                                  const sched_interface::sched_args_t& sched_args)
{
  enb_cc_idx = enb_cc_idx_;
  cfg        = cfg_;
  sched_cfg  = &sched_args;

  // Basic cell config checks
  if (cfg.si_window_ms == 0) {
    Error("SCHED: Invalid si-window length 0 ms");
    return false;
  }

  bool invalid_prach;
  if (cfg.cell.nof_prb == 6) {
    // PUCCH has to allow space for Msg3
    if (cfg.nrb_pucch > 1) {
      srsran::console("Invalid PUCCH configuration: nrb_pucch=%d does not allow space for Msg3 transmission..\n",
                      cfg.nrb_pucch);
      return false;
    }
    // PRACH has to fit within the PUSCH+PUCCH space
    invalid_prach = cfg.prach_freq_offset + 6 > cfg.cell.nof_prb;
  } else {
    // PRACH has to fit within the PUSCH space
    invalid_prach = (cfg.prach_freq_offset + 6) > (cfg.cell.nof_prb - cfg.nrb_pucch) or
                    ((int)cfg.prach_freq_offset < cfg.nrb_pucch);
  }
  if (invalid_prach) {
    Error("Invalid PRACH configuration: frequency offset=%d outside bandwidth limits", cfg.prach_freq_offset);
    srsran::console("Invalid PRACH configuration: frequency offset=%d outside bandwidth limits\n",
                    cfg.prach_freq_offset);
    return false;
  }

  // Set derived sched parameters

  // init regs
  regs.reset(new srsran_regs_t{});
  if (srsran_regs_init(regs.get(), cfg.cell) != SRSRAN_SUCCESS) {
    Error("Getting DCI locations");
    return false;
  }

  // Compute Common locations for DCI for each CFI
  for (uint32_t cfix = 0; cfix < SRSRAN_NOF_CFI; cfix++) {
    generate_cce_location(regs.get(), common_locations[cfix], cfix + 1);
  }
  if (common_locations[sched_cfg->max_nof_ctrl_symbols - 1][2].empty()) {
    Error("SCHED: Current cfi=%d is not valid for broadcast (check scheduler.max_nof_ctrl_symbols in conf file).",
          sched_cfg->max_nof_ctrl_symbols);
    srsran::console(
        "SCHED: Current cfi=%d is not valid for broadcast (check scheduler.max_nof_ctrl_symbols in conf file).\n",
        sched_cfg->max_nof_ctrl_symbols);
    return false;
  }

  // Compute UE locations for RA-RNTI
  for (uint32_t cfi = 0; cfi < SRSRAN_NOF_CFI; cfi++) {
    for (uint32_t sf_idx = 0; sf_idx < SRSRAN_NOF_SF_X_FRAME; sf_idx++) {
      generate_cce_location(regs.get(), rar_locations[sf_idx][cfi], cfi + 1, sf_idx);
    }
  }

  // precompute nof cces in PDCCH for each CFI
  for (uint32_t cfix = 0; cfix < nof_cce_table.size(); ++cfix) {
    int ret = srsran_regs_pdcch_ncce(regs.get(), cfix + 1);
    if (ret < 0) {
      Error("SCHED: Failed to calculate the number of CCEs in the PDCCH");
      return false;
    }
    nof_cce_table[cfix] = (uint32_t)ret;
  }

  // PUCCH config struct for PUCCH position derivation
  pucch_cfg_common.format            = SRSRAN_PUCCH_FORMAT_1;
  pucch_cfg_common.delta_pucch_shift = cfg.delta_pucch_shift;
  pucch_cfg_common.n_rb_2            = cfg.nrb_cqi;
  pucch_cfg_common.N_cs              = cfg.ncs_an;
  pucch_cfg_common.N_pucch_1         = cfg.n1pucch_an;

  P        = srsran_ra_type0_P(cfg.cell.nof_prb);
  nof_rbgs = srsran::ceil_div(cfg.cell.nof_prb, P);

  nof_re_table    = generate_nof_re_table(cfg.cell);
  nof_re_lb_table = get_lb_nof_re_x_prb(nof_re_table);

  return true;
}

uint32_t sched_cell_params_t::get_dl_lb_nof_re(tti_point tti_tx_dl, uint32_t nof_prbs_alloc) const
{
  assert(nof_prbs_alloc <= nof_prb());
  if (nof_prbs_alloc == 0) {
    return 0;
  }
  uint32_t sf_idx = tti_tx_dl.sf_idx();
  uint32_t nof_re = nof_re_lb_table[sf_idx][nof_prbs_alloc - 1];

  // sanity check
  assert(nof_re <= srsran_ra_dl_approx_nof_re(&cfg.cell, nof_prbs_alloc, SRSRAN_NOF_CFI));
  return nof_re;
}

uint32_t
sched_cell_params_t::get_dl_nof_res(srsran::tti_point tti_tx_dl, const srsran_dci_dl_t& dci, uint32_t cfi) const
{
  assert(cfi > 0 && "CFI has to be within (1..3)");
  srsran_pdsch_grant_t grant = {};
  srsran_dl_sf_cfg_t   dl_sf = {};
  dl_sf.cfi                  = cfi;
  dl_sf.tti                  = tti_tx_dl.to_uint();
  srsran_ra_dl_grant_to_grant_prb_allocation(&dci, &grant, nof_prb());

  uint32_t nof_re = 0;
  for (uint32_t p = 0; p < nof_prb(); ++p) {
    for (uint32_t s = 0; s < SRSRAN_NOF_SLOTS_PER_SF; ++s) {
      if (grant.prb_idx[s][p]) {
        nof_re += nof_re_table[p][tti_tx_dl.sf_idx()][s][cfi - 1];
      }
    }
  }

  return nof_re;
}

cce_frame_position_table generate_cce_location_table(uint16_t rnti, const sched_cell_params_t& cell_cfg)
{
  cce_frame_position_table dci_locations = {};
  // Generate allowed CCE locations
  for (int cfi = 0; cfi < SRSRAN_NOF_CFI; cfi++) {
    for (int sf_idx = 0; sf_idx < SRSRAN_NOF_SF_X_FRAME; sf_idx++) {
      generate_cce_location(cell_cfg.regs.get(), dci_locations[sf_idx][cfi], cfi + 1, sf_idx, rnti);
    }
  }
  return dci_locations;
}

void generate_cce_location(srsran_regs_t*          regs_,
                           cce_cfi_position_table& locations,
                           uint32_t                cfi,
                           uint32_t                sf_idx,
                           uint16_t                rnti)
{
  locations = {};

  srsran_dci_location_t loc[64];
  uint32_t              nloc = 0;
  if (rnti == 0) {
    nloc = srsran_pdcch_common_locations_ncce(srsran_regs_pdcch_ncce(regs_, cfi), loc, 64);
  } else {
    nloc = srsran_pdcch_ue_locations_ncce(srsran_regs_pdcch_ncce(regs_, cfi), loc, 64, sf_idx, rnti);
  }

  // Save to different format
  for (uint32_t i = 0; i < nloc; i++) {
    uint32_t l = loc[i].L;
    locations[l].push_back(loc[i].ncce);
  }
}

/*******************************************************
 *            DCI-specific helper functions
 *******************************************************/

uint32_t get_aggr_level(uint32_t nof_bits,
                        uint32_t dl_cqi,
                        uint32_t min_aggr_lvl,
                        uint32_t max_aggr_lvl,
                        uint32_t cell_nof_prb,
                        bool     use_tbs_index_alt)
{
  float    max_coderate = srsran_cqi_to_coderate(dl_cqi, use_tbs_index_alt);
  float    factor       = 1.5;
  uint32_t l_max        = 3;
  if (cell_nof_prb == 6) {
    factor = 1.0;
    l_max  = 2;
  }
  l_max = std::min(max_aggr_lvl, l_max);

  uint32_t l        = std::min(min_aggr_lvl, l_max);
  float    coderate = srsran_pdcch_coderate(nof_bits, l);
  while (factor * coderate > max_coderate and l < l_max) {
    l++;
    coderate = srsran_pdcch_coderate(nof_bits, l);
  }

  Debug("SCHED: CQI=%d, l=%d, nof_bits=%d, coderate=%.2f, max_coderate=%.2f",
        dl_cqi,
        l,
        nof_bits,
        coderate,
        max_coderate);
  return l;
}

/*******************************************************
 *          sched_interface helper functions
 *******************************************************/

/// sanity check the UE CC configuration
int check_ue_cfg_correctness(const sched_interface::ue_cfg_t& ue_cfg)
{
  using cc_t             = sched_interface::ue_cfg_t::cc_cfg_t;
  const auto& cc_list    = ue_cfg.supported_cc_list;
  bool        has_scells = std::count_if(cc_list.begin(), cc_list.end(), [](const cc_t& c) { return c.active; }) > 1;
  int         ret        = SRSRAN_SUCCESS;

  if (has_scells) {
    // In case of CA, CQI configs must exist and cannot collide in the PUCCH
    for (uint32_t i = 0; i < cc_list.size(); ++i) {
      const auto& cc1 = cc_list[i];
      if (not cc1.active) {
        continue;
      }
      if (not cc1.dl_cfg.cqi_report.periodic_configured and not cc1.dl_cfg.cqi_report.aperiodic_configured) {
        Warning("SCHED: No CQI configuration was provided for UE scell index=%d", i);
        ret = SRSRAN_ERROR;
      } else if (cc1.dl_cfg.cqi_report.periodic_configured) {
        for (uint32_t j = i + 1; j < cc_list.size(); ++j) {
          if (cc_list[j].active and cc_list[j].dl_cfg.cqi_report.periodic_configured and
              cc_list[j].dl_cfg.cqi_report.pmi_idx == cc1.dl_cfg.cqi_report.pmi_idx) {
            Warning("SCHED: The provided CQI configurations for UE scells %d and %d collide in time resources.", i, j);
            ret = SRSRAN_ERROR;
          }
        }
      }
    }
  }
  return ret;
}

} // namespace srsenb
