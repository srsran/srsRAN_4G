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

#include "srsenb/hdr/stack/mac/nr/sched_nr_phy.h"
#include "srsenb/hdr/stack/mac/nr/sched_nr_harq.h"
#include "srsenb/hdr/stack/mac/nr/sched_nr_ue.h"

namespace srsenb {
namespace sched_nr_impl {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool fill_dci_rar(prb_interval interv, const bwp_params& cell, srsran_dci_dl_nr_t& dci)
{
  dci.mcs = 5;
  return true;
}

template <typename DciDlOrUl>
void fill_dci_common(const slot_ue& ue, const bwp_params& bwp_cfg, DciDlOrUl& dci)
{
  const static uint32_t rv_idx[4] = {0, 2, 3, 1};

  dci.bwp_id = ue.bwp_id;
  dci.cc_id  = ue.cc;
  dci.tpc    = 1;
  // harq
  harq_proc* h = std::is_same<DciDlOrUl, srsran_dci_dl_nr_t>::value ? ue.h_dl : ue.h_ul;
  dci.pid      = h->pid;
  dci.ndi      = h->ndi();
  dci.mcs      = h->mcs();
  dci.rv       = rv_idx[h->nof_retx() % 4];
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
    dci.harq_feedback = ue.cfg->phy().harq_ack.dl_data_to_ul_ack[ue.pdsch_tti.sf_idx()] - 1;
  } else {
    dci.harq_feedback = ue.pdsch_tti.sf_idx();
  }
}

void fill_ul_dci_ue_fields(const slot_ue&        ue,
                           const bwp_params&     bwp_cfg,
                           uint32_t              ss_id,
                           srsran_dci_location_t dci_pos,
                           srsran_dci_ul_nr_t&   dci)
{
  bool ret = ue.cfg->phy().get_dci_ctx_pdsch_rnti_c(ss_id, dci_pos, ue.rnti, dci.ctx);
  srsran_assert(ret, "Invalid DL DCI format");

  fill_dci_common(ue, bwp_cfg, dci);
}

} // namespace sched_nr_impl
} // namespace srsenb