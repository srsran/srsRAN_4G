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

#include "srsenb/hdr/stack/mac/nr/sched_nr_helpers.h"
#include "srsenb/hdr/stack/mac/nr/sched_nr_grant_allocator.h"
#include "srsenb/hdr/stack/mac/nr/sched_nr_harq.h"
#include "srsenb/hdr/stack/mac/nr/sched_nr_ue.h"
#include "srsran/common/string_helpers.h"

namespace srsenb {
namespace sched_nr_impl {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename DciDlOrUl>
void fill_dci_common(const slot_ue& ue, const bwp_params& bwp_cfg, DciDlOrUl& dci)
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

bool fill_dci_rar(prb_interval interv, uint16_t ra_rnti, const bwp_params& bwp_cfg, srsran_dci_dl_nr_t& dci)
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

bool fill_dci_msg3(const slot_ue& ue, const bwp_params& bwp_cfg, srsran_dci_ul_nr_t& msg3_dci)
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
                           const bwp_params&     bwp_cfg,
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
    dci.harq_feedback = ue.cfg->phy().harq_ack.dl_data_to_ul_ack[ue.pdsch_slot.slot_idx()] - 1;
  } else {
    dci.harq_feedback = ue.pdsch_slot.slot_idx();
  }
}

void fill_ul_dci_ue_fields(const slot_ue&        ue,
                           const bwp_params&     bwp_cfg,
                           uint32_t              ss_id,
                           srsran_dci_location_t dci_pos,
                           srsran_dci_ul_nr_t&   dci)
{
  bool ret = ue.cfg->phy().get_dci_ctx_pusch_rnti_c(ss_id, dci_pos, ue.rnti, dci.ctx);
  srsran_assert(ret, "Invalid DL DCI format");

  fill_dci_common(ue, bwp_cfg, dci);
}

void log_sched_bwp_result(srslog::basic_logger& logger,
                          slot_point            pdcch_slot,
                          const bwp_res_grid&   res_grid,
                          const slot_ue_map_t&  slot_ues)
{
  const bwp_slot_grid& bwp_slot = res_grid[pdcch_slot];
  for (const pdcch_dl_t& pdcch : bwp_slot.dl_pdcchs) {
    fmt::memory_buffer fmtbuf;
    if (pdcch.dci.ctx.rnti_type == srsran_rnti_type_c) {
      const slot_ue& ue = slot_ues[pdcch.dci.ctx.rnti];
      fmt::format_to(
          fmtbuf,
          "SCHED: DL {}, cc={}, rnti=0x{:x}, pid={}, f={}, nrtx={}, dai={}, tbs={}, pdsch_slot={}, tti_ack={}",
          ue.h_dl->nof_retx() == 0 ? "tx" : "retx",
          res_grid.cfg->cc,
          ue.rnti,
          pdcch.dci.pid,
          srsran_dci_format_nr_string(pdcch.dci.ctx.format),
          ue.h_dl->nof_retx(),
          pdcch.dci.dai,
          ue.h_dl->tbs(),
          ue.pdsch_slot,
          ue.uci_slot);
    } else if (pdcch.dci.ctx.rnti_type == srsran_rnti_type_ra) {
      fmt::format_to(fmtbuf,
                     "SCHED: DL RAR, cc={}, ra-rnti=0x{:x}, pdsch_slot={}, msg3_slot={}",
                     res_grid.cfg->cc,
                     pdcch.dci.ctx.rnti,
                     pdcch_slot,
                     pdcch_slot + res_grid.cfg->pusch_ra_list[0].msg3_delay);
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
                     "SCHED: UL {}, cc={}, rnti=0x{:x}, pid={}, f={}, nrtx={}, tbs={}, tti_pusch={}",
                     ue.h_dl->nof_retx() == 0 ? "tx" : "retx",
                     res_grid.cfg->cc,
                     ue.rnti,
                     pdcch.dci.pid,
                     srsran_dci_format_nr_string(pdcch.dci.ctx.format),
                     ue.h_dl->nof_retx(),
                     ue.h_ul->tbs(),
                     ue.pusch_slot);
    } else if (pdcch.dci.ctx.rnti_type == srsran_rnti_type_tc) {
      const slot_ue& ue = slot_ues[pdcch.dci.ctx.rnti];
      fmt::format_to(fmtbuf,
                     "SCHED: UL Msg3, cc={}, tc-rnti=0x{:x}, pid={}, nrtx={}, f={}, tti_pusch={}",
                     res_grid.cfg->cc,
                     ue.rnti,
                     pdcch.dci.pid,
                     ue.h_dl->nof_retx(),
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
