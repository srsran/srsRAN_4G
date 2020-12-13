/**
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

#include "srsenb/hdr/stack/mac/sched_helpers.h"
#include "srslte/mac/pdu.h"
#include "srslte/srslog/bundled/fmt/format.h"
#include <array>

#define Info(fmt, ...) srslte::logmap::get("MAC")->error(fmt, ##__VA_ARGS__)
#define Error(fmt, ...) srslte::logmap::get("MAC")->error(fmt, ##__VA_ARGS__)

namespace srsenb {

using dl_sched_res_t    = sched_interface::dl_sched_res_t;
using dl_sched_data_t   = sched_interface::dl_sched_data_t;
using custom_mem_buffer = fmt::basic_memory_buffer<char, 1024>;

const char* to_string_short(srslte_dci_format_t dcifmt)
{
  switch (dcifmt) {
    case SRSLTE_DCI_FORMAT0:
      return "0";
    case SRSLTE_DCI_FORMAT1:
      return "1";
    case SRSLTE_DCI_FORMAT1A:
      return "1A";
    case SRSLTE_DCI_FORMAT1B:
      return "1B";
    case SRSLTE_DCI_FORMAT2:
      return "2";
    case SRSLTE_DCI_FORMAT2A:
      return "2A";
    case SRSLTE_DCI_FORMAT2B:
      return "2B";
    default:
      return "unknown";
  }
}

void fill_dl_cc_result_info(custom_mem_buffer& strbuf, const dl_sched_data_t& data)
{
  uint32_t first_ce = sched_interface::MAX_RLC_PDU_LIST;
  for (uint32_t i = 0; i < data.nof_pdu_elems[0]; ++i) {
    if (srslte::is_mac_ce(static_cast<srslte::dl_sch_lcid>(data.pdu[i]->lcid))) {
      first_ce = i;
      break;
    }
  }
  if (first_ce == sched_interface::MAX_RLC_PDU_LIST) {
    return;
  }
  const char* prefix = strbuf.size() > 0 ? " | " : "";
  fmt::format_to(strbuf, "{}rnti={:0x}: [", prefix, data.dci.rnti);
  bool ces_found = false;
  for (uint32_t i = 0; i < data.nof_pdu_elems[0]; ++i) {
    const auto& pdu          = data.pdu[0][i];
    prefix                   = (ces_found) ? " | " : "";
    srslte::dl_sch_lcid lcid = static_cast<srslte::dl_sch_lcid>(pdu.lcid);
    if (srslte::is_mac_ce(lcid)) {
      fmt::format_to(strbuf, "{}MAC CE \"{}\"", prefix, srslte::to_string_short(lcid));
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
                 "  > rnti={:0x}, tbs={}, f={}, mcs={}: [",
                 data.dci.rnti,
                 data.tbs[0],
                 to_string_short(data.dci.format),
                 data.dci.tb[0].mcs_idx);
  for (uint32_t tb = 0; tb < SRSLTE_MAX_TB; ++tb) {
    for (uint32_t i = 0; i < data.nof_pdu_elems[tb]; ++i) {
      const auto&         pdu    = data.pdu[tb][i];
      const char*         prefix = (i == 0) ? "" : " | ";
      srslte::dl_sch_lcid lcid   = static_cast<srslte::dl_sch_lcid>(pdu.lcid);
      if (srslte::is_mac_ce(lcid)) {
        fmt::format_to(strbuf, "{}MAC CE \"{}\"", prefix, srslte::to_string_short(lcid));
      } else {
        fmt::format_to(strbuf, "{}MAC SDU lcid={}, tb={}, len={} B", prefix, pdu.lcid, tb, pdu.nbytes);
      }
    }
  }
  fmt::format_to(strbuf, "]\n");
}

void log_dl_cc_results(srslte::log_ref log_h, uint32_t enb_cc_idx, const sched_interface::dl_sched_res_t& result)
{
  if (log_h->get_level() < srslte::LOG_LEVEL_INFO) {
    return;
  }
  custom_mem_buffer strbuf;
  for (uint32_t i = 0; i < result.nof_data_elems; ++i) {
    const dl_sched_data_t& data = result.data[i];
    if (log_h->get_level() == srslte::LOG_LEVEL_INFO) {
      fill_dl_cc_result_info(strbuf, data);
    } else if (log_h->get_level() == srslte::LOG_LEVEL_DEBUG) {
      fill_dl_cc_result_debug(strbuf, data);
    }
  }
  if (strbuf.size() != 0) {
    if (log_h->get_level() == srslte::LOG_LEVEL_DEBUG) {
      log_h->debug("SCHED: MAC LCID allocs cc=%d:\n%s", enb_cc_idx, fmt::to_string(strbuf).c_str());
    } else {
      log_h->info("SCHED: MAC CE allocs cc=%d: %s", enb_cc_idx, fmt::to_string(strbuf).c_str());
    }
  }
}

rbg_interval rbg_interval::prbs_to_rbgs(const prb_interval& prbs, uint32_t P)
{
  return rbg_interval{srslte::ceil_div(prbs.start(), P), srslte::ceil_div(prbs.stop(), P)};
}

prb_interval prb_interval::rbgs_to_prbs(const rbg_interval& rbgs, uint32_t P)
{
  return prb_interval{rbgs.start() * P, rbgs.stop() * P};
}

rbg_interval rbg_interval::rbgmask_to_rbgs(const rbgmask_t& mask)
{
  int rb_start = -1;
  for (uint32_t i = 0; i < mask.size(); i++) {
    if (rb_start == -1) {
      if (mask.test(i)) {
        rb_start = i;
      }
    } else {
      if (!mask.test(i)) {
        return rbg_interval(rb_start, i);
      }
    }
  }
  if (rb_start != -1) {
    return rbg_interval(rb_start, mask.size());
  } else {
    return rbg_interval();
  }
}

prb_interval prb_interval::riv_to_prbs(uint32_t riv, uint32_t nof_prbs, int nof_vrbs)
{
  if (nof_vrbs < 0) {
    nof_vrbs = nof_prbs;
  }
  uint32_t rb_start, l_crb;
  srslte_ra_type2_from_riv(riv, &l_crb, &rb_start, nof_prbs, (uint32_t)nof_vrbs);
  return {rb_start, rb_start + l_crb};
}

/*******************************************************
 *                 Sched Params
 *******************************************************/

void sched_cell_params_t::regs_deleter::operator()(srslte_regs_t* p)
{
  if (p != nullptr) {
    srslte_regs_free(p);
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
    Error("SCHED: Invalid si-window length 0 ms\n");
    return false;
  }

  bool invalid_prach;
  if (cfg.cell.nof_prb == 6) {
    // PUCCH has to allow space for Msg3
    if (cfg.nrb_pucch > 1) {
      srslte::console("Invalid PUCCH configuration: nrb_pucch=%d does not allow space for Msg3 transmission..\n",
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
    Error("Invalid PRACH configuration: frequency offset=%d outside bandwidth limits\n", cfg.prach_freq_offset);
    srslte::console("Invalid PRACH configuration: frequency offset=%d outside bandwidth limits\n",
                    cfg.prach_freq_offset);
    return false;
  }

  // Set derived sched parameters

  // init regs
  regs.reset(new srslte_regs_t{});
  if (srslte_regs_init(regs.get(), cfg.cell) != SRSLTE_SUCCESS) {
    Error("Getting DCI locations\n");
    return false;
  }

  // Compute Common locations for DCI for each CFI
  for (uint32_t cfix = 0; cfix < SRSLTE_NOF_CFI; cfix++) {
    generate_cce_location(regs.get(), &common_locations[cfix], cfix + 1);
  }
  if (common_locations[sched_cfg->max_nof_ctrl_symbols - 1].nof_loc[2] == 0) {
    Error("SCHED: Current cfi=%d is not valid for broadcast (check scheduler.max_nof_ctrl_symbols in conf file).\n",
          sched_cfg->max_nof_ctrl_symbols);
    srslte::console(
        "SCHED: Current cfi=%d is not valid for broadcast (check scheduler.max_nof_ctrl_symbols in conf file).\n",
        sched_cfg->max_nof_ctrl_symbols);
    return false;
  }

  // Compute UE locations for RA-RNTI
  for (uint32_t cfi = 0; cfi < 3; cfi++) {
    for (uint32_t sf_idx = 0; sf_idx < 10; sf_idx++) {
      generate_cce_location(regs.get(), &rar_locations[cfi][sf_idx], cfi + 1, sf_idx);
    }
  }

  // precompute nof cces in PDCCH for each CFI
  for (uint32_t cfix = 0; cfix < nof_cce_table.size(); ++cfix) {
    int ret = srslte_regs_pdcch_ncce(regs.get(), cfix + 1);
    if (ret < 0) {
      Error("SCHED: Failed to calculate the number of CCEs in the PDCCH\n");
      return false;
    }
    nof_cce_table[cfix] = (uint32_t)ret;
  }

  P        = srslte_ra_type0_P(cfg.cell.nof_prb);
  nof_rbgs = srslte::ceil_div(cfg.cell.nof_prb, P);

  return true;
}

void generate_cce_location(srslte_regs_t*   regs_,
                           sched_dci_cce_t* location,
                           uint32_t         cfi,
                           uint32_t         sf_idx,
                           uint16_t         rnti)
{
  *location = {};

  srslte_dci_location_t loc[64];
  uint32_t              nloc = 0;
  if (rnti == 0) {
    nloc = srslte_pdcch_common_locations_ncce(srslte_regs_pdcch_ncce(regs_, cfi), loc, 64);
  } else {
    nloc = srslte_pdcch_ue_locations_ncce(srslte_regs_pdcch_ncce(regs_, cfi), loc, 64, sf_idx, rnti);
  }

  // Save to different format
  for (uint32_t i = 0; i < nloc; i++) {
    uint32_t l                                   = loc[i].L;
    location->cce_start[l][location->nof_loc[l]] = loc[i].ncce;
    location->nof_loc[l]++;
  }
}

} // namespace srsenb
