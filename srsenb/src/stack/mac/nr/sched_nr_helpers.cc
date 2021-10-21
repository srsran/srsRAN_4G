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

#include "srsenb/hdr/stack/mac/nr/sched_nr_helpers.h"
#include "srsenb/hdr/stack/mac/nr/sched_nr_grant_allocator.h"
#include "srsenb/hdr/stack/mac/nr/sched_nr_harq.h"
#include "srsenb/hdr/stack/mac/nr/sched_nr_ue.h"
#include "srsran/common/string_helpers.h"

namespace srsenb {
namespace sched_nr_impl {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename DciDlOrUl>
void fill_dci_common(const slot_ue& ue, const bwp_params_t& bwp_cfg, DciDlOrUl& dci)
{
  const static uint32_t rv_idx[4] = {0, 2, 3, 1};

  dci.bwp_id = ue.cfg->active_bwp().bwp_id;
  dci.cc_id  = ue.cc;
  dci.tpc    = 1;
  // harq
  harq_proc* h = std::is_same<DciDlOrUl, srsran_dci_dl_nr_t>::value ? static_cast<harq_proc*>(ue.h_dl)
                                                                    : static_cast<harq_proc*>(ue.h_ul);
  dci.pid = h->pid;
  dci.ndi = h->ndi();
  dci.mcs = h->mcs();
  dci.rv  = rv_idx[h->nof_retx() % 4];
  // PRB assignment
  const prb_grant& grant = h->prbs();
  if (grant.is_alloc_type0()) {
    dci.freq_domain_assigment = grant.rbgs().to_uint64();
  } else {
    dci.freq_domain_assigment =
        srsran_ra_nr_type1_riv(bwp_cfg.cfg.rb_width, grant.prbs().start(), grant.prbs().length());
  }
  dci.time_domain_assigment = 0;
}

bool fill_dci_rar(prb_interval interv, uint16_t ra_rnti, const bwp_params_t& bwp_cfg, srsran_dci_dl_nr_t& dci)
{
  dci.mcs                   = 5;
  dci.ctx.format            = srsran_dci_format_nr_1_0;
  dci.ctx.ss_type           = srsran_search_space_type_common_1;
  dci.ctx.rnti_type         = srsran_rnti_type_ra;
  dci.ctx.rnti              = ra_rnti;
  dci.ctx.coreset_id        = bwp_cfg.cfg.pdcch.ra_search_space.coreset_id;
  dci.freq_domain_assigment = srsran_ra_nr_type1_riv(bwp_cfg.cfg.rb_width, interv.start(), interv.length());
  dci.time_domain_assigment = 0;
  dci.tpc                   = 1;
  dci.bwp_id                = bwp_cfg.bwp_id;
  dci.cc_id                 = bwp_cfg.cc;
  // TODO: Fill

  return true;
}

bool fill_dci_msg3(const slot_ue& ue, const bwp_params_t& bwp_cfg, srsran_dci_ul_nr_t& msg3_dci)
{
  fill_dci_common(ue, bwp_cfg, msg3_dci);
  msg3_dci.ctx.coreset_id = ue.cfg->phy().pdcch.ra_search_space.coreset_id;
  msg3_dci.ctx.rnti_type  = srsran_rnti_type_tc;
  msg3_dci.ctx.rnti       = ue.rnti;
  msg3_dci.ctx.ss_type    = srsran_search_space_type_rar;
  if (ue.h_ul->nof_retx() == 0) {
    msg3_dci.ctx.format = srsran_dci_format_nr_rar;
  } else {
    msg3_dci.ctx.format = srsran_dci_format_nr_0_0;
  }

  return true;
}

void fill_dl_dci_ue_fields(const slot_ue&        ue,
                           const bwp_params_t&   bwp_cfg,
                           uint32_t              ss_id,
                           srsran_dci_location_t dci_pos,
                           srsran_dci_dl_nr_t&   dci)
{
  // Note: DCI location may not be the final one, as scheduler may rellocate the UE PDCCH. However, the remaining DCI
  //       params are independent of the exact DCI location
  bool ret = ue.cfg->phy().get_dci_ctx_pdsch_rnti_c(ss_id, dci_pos, ue.rnti, dci.ctx);
  srsran_assert(ret, "Invalid DL DCI format");

  fill_dci_common(ue, bwp_cfg, dci);
  if (dci.ctx.format == srsran_dci_format_nr_1_0) {
    dci.harq_feedback = (ue.uci_slot - ue.pdsch_slot) - 1;
  } else {
    dci.harq_feedback = ue.pdsch_slot.slot_idx();
  }
}

void fill_ul_dci_ue_fields(const slot_ue&        ue,
                           const bwp_params_t&   bwp_cfg,
                           uint32_t              ss_id,
                           srsran_dci_location_t dci_pos,
                           srsran_dci_ul_nr_t&   dci)
{
  bool ret = ue.cfg->phy().get_dci_ctx_pusch_rnti_c(ss_id, dci_pos, ue.rnti, dci.ctx);
  srsran_assert(ret, "Invalid DL DCI format");

  fill_dci_common(ue, bwp_cfg, dci);
}

void log_sched_slot_ues(srslog::basic_logger& logger, slot_point pdcch_slot, uint32_t cc, const slot_ue_map_t& slot_ues)
{
  if (not logger.debug.enabled() or slot_ues.empty()) {
    return;
  }

  fmt::memory_buffer fmtbuf;
  fmt::format_to(fmtbuf, "SCHED: UE candidates, pdcch_tti={}, cc={}: [", pdcch_slot, cc);

  const char* use_comma = "";
  for (const auto& ue_pair : slot_ues) {
    auto& ue = ue_pair->second;

    fmt::format_to(
        fmtbuf, "{}{{rnti=0x{:x}, dl_bs={}, ul_bs={}}}", use_comma, ue.rnti, ue.dl_pending_bytes, ue.ul_pending_bytes);
    use_comma = ", ";
  }

  logger.debug("%s]", srsran::to_c_str(fmtbuf));
}

void log_sched_bwp_result(srslog::basic_logger& logger,
                          slot_point            pdcch_slot,
                          const bwp_res_grid&   res_grid,
                          const slot_ue_map_t&  slot_ues)
{
  const bwp_slot_grid& bwp_slot  = res_grid[pdcch_slot];
  size_t               rar_count = 0;
  for (const pdcch_dl_t& pdcch : bwp_slot.dl_pdcchs) {
    fmt::memory_buffer fmtbuf;
    if (pdcch.dci.ctx.rnti_type == srsran_rnti_type_c) {
      const slot_ue& ue = slot_ues[pdcch.dci.ctx.rnti];
      fmt::format_to(fmtbuf,
                     "SCHED: DL {}, cc={}, rnti=0x{:x}, pid={}, cs={}, f={}, prbs={}, nrtx={}, dai={}, "
                     "tbs={}, bs={}, pdsch_slot={}, tti_ack={}",
                     ue.h_dl->nof_retx() == 0 ? "tx" : "retx",
                     res_grid.cfg->cc,
                     ue.rnti,
                     pdcch.dci.pid,
                     pdcch.dci.ctx.coreset_id,
                     srsran_dci_format_nr_string(pdcch.dci.ctx.format),
                     ue.h_dl->prbs(),
                     ue.h_dl->nof_retx(),
                     pdcch.dci.dai,
                     ue.h_dl->tbs() / 8u,
                     ue.dl_pending_bytes,
                     ue.pdsch_slot,
                     ue.uci_slot);
    } else if (pdcch.dci.ctx.rnti_type == srsran_rnti_type_ra) {
      const pdsch_t&           pdsch = bwp_slot.pdschs[std::distance(bwp_slot.dl_pdcchs.data(), &pdcch)];
      srsran::const_span<bool> prbs{pdsch.sch.grant.prb_idx, pdsch.sch.grant.prb_idx + pdsch.sch.grant.nof_prb};
      uint32_t                 start_idx = std::distance(prbs.begin(), std::find(prbs.begin(), prbs.end(), true));
      uint32_t end_idx = std::distance(prbs.begin(), std::find(prbs.begin() + start_idx, prbs.end(), false));
      fmt::format_to(fmtbuf,
                     "SCHED: RAR, cc={}, ra-rnti=0x{:x}, prbs={}, pdsch_slot={}, msg3_slot={}, nof_grants={}",
                     res_grid.cfg->cc,
                     pdcch.dci.ctx.rnti,
                     srsran::interval<uint32_t>{start_idx, end_idx},
                     pdcch_slot,
                     pdcch_slot + res_grid.cfg->pusch_ra_list[0].msg3_delay,
                     bwp_slot.rar[rar_count].grants.size());
      rar_count++;
    } else {
      fmt::format_to(fmtbuf, "SCHED: unknown format");
    }

    logger.info("%s", srsran::to_c_str(fmtbuf));
  }
  for (const pdcch_ul_t& pdcch : bwp_slot.ul_pdcchs) {
    fmt::memory_buffer fmtbuf;
    if (pdcch.dci.ctx.rnti_type == srsran_rnti_type_c) {
      const slot_ue& ue = slot_ues[pdcch.dci.ctx.rnti];
      fmt::format_to(fmtbuf,
                     "SCHED: UL {}, cc={}, rnti=0x{:x}, pid={}, cs={}, f={}, nrtx={}, tbs={}, bs={}, tti_pusch={}",
                     ue.h_ul->nof_retx() == 0 ? "tx" : "retx",
                     res_grid.cfg->cc,
                     ue.rnti,
                     pdcch.dci.pid,
                     pdcch.dci.ctx.coreset_id,
                     srsran_dci_format_nr_string(pdcch.dci.ctx.format),
                     ue.h_ul->nof_retx(),
                     ue.h_ul->tbs() / 8u,
                     ue.ul_pending_bytes,
                     ue.pusch_slot);
    } else if (pdcch.dci.ctx.rnti_type == srsran_rnti_type_tc) {
      const slot_ue& ue = slot_ues[pdcch.dci.ctx.rnti];
      fmt::format_to(fmtbuf,
                     "SCHED: UL Msg3, cc={}, tc-rnti=0x{:x}, pid={}, nrtx={}, f={}, tti_pusch={}",
                     res_grid.cfg->cc,
                     ue.rnti,
                     pdcch.dci.pid,
                     ue.h_ul->nof_retx(),
                     srsran_dci_format_nr_string(pdcch.dci.ctx.format),
                     ue.pusch_slot);
    } else {
      fmt::format_to(fmtbuf, "SCHED: unknown rnti format");
    }

    logger.info("%s", srsran::to_c_str(fmtbuf));
  }
}

} // namespace sched_nr_impl
} // namespace srsenb
