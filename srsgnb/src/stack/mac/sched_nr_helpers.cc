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

#include "srsgnb/hdr/stack/mac/sched_nr_helpers.h"
#include "srsgnb/hdr/stack/mac/sched_nr_grant_allocator.h"
#include "srsgnb/hdr/stack/mac/sched_nr_harq.h"
#include "srsgnb/hdr/stack/mac/sched_nr_ue.h"
#include "srsran/common/string_helpers.h"

namespace srsenb {
namespace sched_nr_impl {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

    fmt::format_to(fmtbuf, "{}{{rnti=0x{:x}", use_comma, ue->rnti);
    if (ue.dl_active) {
      fmt::format_to(fmtbuf, ", dl_bs={}", ue.dl_bytes);
    }
    if (ue.ul_active) {
      fmt::format_to(fmtbuf, ", ul_bs={}", ue.ul_bytes);
    }
    fmt::format_to(fmtbuf, "}}");
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
  size_t               rar_count = 0, si_count = 0, data_count = 0;
  for (const pdcch_dl_t& pdcch : bwp_slot.dl.phy.pdcch_dl) {
    fmt::memory_buffer fmtbuf;
    if (pdcch.dci.ctx.rnti_type == srsran_rnti_type_c) {
      const slot_ue& ue = slot_ues[pdcch.dci.ctx.rnti];
      fmt::format_to(fmtbuf,
                     "SCHED: DL {}, cc={}, rnti=0x{:x}, pid={}, cs={}, f={}, prbs={}, nrtx={}, dai={}, "
                     "lcids=[{}], tbs={}, bs={}, pdsch_slot={}, ack_slot={}",
                     ue.h_dl->nof_retx() == 0 ? "tx" : "retx",
                     res_grid.cfg->cc,
                     ue->rnti,
                     pdcch.dci.pid,
                     pdcch.dci.ctx.coreset_id,
                     srsran_dci_format_nr_string(pdcch.dci.ctx.format),
                     ue.h_dl->prbs(),
                     ue.h_dl->nof_retx(),
                     pdcch.dci.dai,
                     fmt::join(bwp_slot.dl.data[data_count].subpdus, ", "),
                     ue.h_dl->tbs() / 8u,
                     ue.dl_bytes,
                     ue.pdsch_slot,
                     ue.uci_slot);
      data_count++;
    } else if (pdcch.dci.ctx.rnti_type == srsran_rnti_type_ra) {
      const pdsch_t&           pdsch = bwp_slot.dl.phy.pdsch[std::distance(bwp_slot.dl.phy.pdcch_dl.data(), &pdcch)];
      srsran::const_span<bool> prbs{pdsch.sch.grant.prb_idx, pdsch.sch.grant.prb_idx + SRSRAN_MAX_PRB_NR};
      uint32_t                 start_idx = std::distance(prbs.begin(), std::find(prbs.begin(), prbs.end(), true));
      uint32_t                 end_idx   = start_idx + pdsch.sch.grant.nof_prb;
      fmt::format_to(fmtbuf,
                     "SCHED: RAR, cc={}, ra-rnti=0x{:x}, prbs={}, pdsch_slot={}, msg3_slot={}, nof_grants={}",
                     res_grid.cfg->cc,
                     pdcch.dci.ctx.rnti,
                     srsran::interval<uint32_t>{start_idx, end_idx},
                     pdcch_slot,
                     pdcch_slot + res_grid.cfg->pusch_ra_list[0].msg3_delay,
                     bwp_slot.dl.rar[rar_count].grants.size());
      rar_count++;
    } else if (pdcch.dci.ctx.rnti_type == srsran_rnti_type_si) {
      if (logger.debug.enabled()) {
        const pdsch_t&           pdsch = bwp_slot.dl.phy.pdsch[std::distance(bwp_slot.dl.phy.pdcch_dl.data(), &pdcch)];
        srsran::const_span<bool> prbs{pdsch.sch.grant.prb_idx, pdsch.sch.grant.prb_idx + SRSRAN_MAX_PRB_NR};
        uint32_t                 start_idx = std::distance(prbs.begin(), std::find(prbs.begin(), prbs.end(), true));
        uint32_t                 end_idx   = start_idx + pdsch.sch.grant.nof_prb;
        fmt::format_to(fmtbuf,
                       "SCHED: SI{}, cc={}, prbs={}, pdsch_slot={}",
                       pdcch.dci.sii == 0 ? "B" : " message",
                       res_grid.cfg->cc,
                       srsran::interval<uint32_t>{start_idx, end_idx},
                       pdcch_slot);
        si_count++;
      }
    }

    if (fmtbuf.size() > 0) {
      if (pdcch.dci.ctx.rnti_type == srsran_rnti_type_si) {
        logger.debug("%s", srsran::to_c_str(fmtbuf));
      } else {
        logger.info("%s", srsran::to_c_str(fmtbuf));
      }
    }
  }
  for (const pdcch_ul_t& pdcch : bwp_slot.dl.phy.pdcch_ul) {
    fmt::memory_buffer fmtbuf;
    if (pdcch.dci.ctx.rnti_type == srsran_rnti_type_c) {
      const slot_ue& ue = slot_ues[pdcch.dci.ctx.rnti];
      fmt::format_to(fmtbuf,
                     "SCHED: UL {}, cc={}, rnti=0x{:x}, pid={}, cs={}, f={}, nrtx={}, tbs={}, bs={}, pusch_slot={}",
                     ue.h_ul->nof_retx() == 0 ? "tx" : "retx",
                     res_grid.cfg->cc,
                     ue->rnti,
                     pdcch.dci.pid,
                     pdcch.dci.ctx.coreset_id,
                     srsran_dci_format_nr_string(pdcch.dci.ctx.format),
                     ue.h_ul->nof_retx(),
                     ue.h_ul->tbs() / 8u,
                     ue.ul_bytes,
                     ue.pusch_slot);
    } else if (pdcch.dci.ctx.rnti_type == srsran_rnti_type_tc) {
      const slot_ue& ue = slot_ues[pdcch.dci.ctx.rnti];
      fmt::format_to(fmtbuf,
                     "SCHED: UL Msg3, cc={}, tc-rnti=0x{:x}, pid={}, nrtx={}, f={}, tti_pusch={}",
                     res_grid.cfg->cc,
                     ue->rnti,
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
