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

#include "srsenb/hdr/stack/mac/nr/sched_nr_phy_helpers.h"
#include "srsenb/hdr/stack/mac/nr/sched_nr_harq.h"
#include "srsenb/hdr/stack/mac/nr/sched_nr_ue.h"

namespace srsenb {
namespace sched_nr_impl {

/// Table 6.1.2.2.1-1 - Nominal RBG size P
uint32_t get_P(uint32_t bwp_nof_prb, bool config_1_or_2)
{
  srsran_assert(bwp_nof_prb > 0 and bwp_nof_prb <= 275, "Invalid BWP size");
  if (bwp_nof_prb <= 36) {
    return config_1_or_2 ? 2 : 4;
  }
  if (bwp_nof_prb <= 72) {
    return config_1_or_2 ? 4 : 8;
  }
  if (bwp_nof_prb <= 144) {
    return config_1_or_2 ? 8 : 16;
  }
  return 16;
}

uint32_t get_nof_rbgs(uint32_t bwp_nof_prb, uint32_t bwp_start, bool config1_or_2)
{
  uint32_t P = get_P(bwp_nof_prb, config1_or_2);
  return srsran::ceil_div(bwp_nof_prb + (bwp_start % P), P);
}

uint32_t get_rbg_size(uint32_t bwp_nof_prb, uint32_t bwp_start, bool config1_or_2, uint32_t rbg_idx)
{
  uint32_t P        = get_P(bwp_nof_prb, config1_or_2);
  uint32_t nof_rbgs = get_nof_rbgs(bwp_nof_prb, bwp_start, config1_or_2);
  if (rbg_idx == 0) {
    return P - (bwp_start % P);
  }
  if (rbg_idx == nof_rbgs - 1) {
    uint32_t ret = (bwp_start + bwp_nof_prb) % P;
    return ret > 0 ? ret : P;
  }
  return P;
}

void bitmap_to_prb_array(const rbgmask_t& bitmap, uint32_t bwp_nof_prb, srsran_sch_grant_nr_t& grant)
{
  uint32_t count = 0;
  grant.nof_prb  = bwp_nof_prb;
  for (uint32_t rbg = 0; rbg < bitmap.size(); ++rbg) {
    bool     val      = bitmap.test(rbg);
    uint32_t rbg_size = get_rbg_size(bwp_nof_prb, 0, true, rbg);
    for (uint32_t prb = count; prb < count + rbg_size; ++prb) {
      grant.prb_idx[prb] = val;
    }
  }
}

template <typename DciDlOrUl>
void fill_dci_common(const slot_ue& ue, DciDlOrUl& dci)
{
  dci.bwp_id        = ue.bwp_id;
  dci.cc_id         = ue.cc;
  dci.ctx.rnti      = ue.rnti;
  dci.ctx.rnti_type = srsran_rnti_type_c;
  dci.tpc           = 1;
  // harq
  harq_proc* h = std::is_same<DciDlOrUl, srsran_dci_dl_nr_t>::value ? ue.h_dl : ue.h_ul;
  dci.pid      = h->pid;
  dci.ndi      = h->ndi();
  dci.mcs      = h->mcs();
}

void fill_dci_ue_cfg(const slot_ue& ue, srsran_dci_dl_nr_t& dci)
{
  fill_dci_common(ue, dci);
}

void fill_dci_ue_cfg(const slot_ue& ue, srsran_dci_ul_nr_t& dci)
{
  fill_dci_common(ue, dci);
}

void fill_sch_ue(const slot_ue& ue, const rbgmask_t& rbgmask, const sched_cell_params& cc_cfg, srsran_sch_cfg_nr_t& sch)
{
  sch.grant.rnti_type  = srsran_rnti_type_c;
  sch.grant.rnti       = ue.rnti;
  sch.grant.nof_layers = 1;
  bitmap_to_prb_array(rbgmask, cc_cfg.cell_cfg.nof_prb, sch.grant);
}

} // namespace sched_nr_impl
} // namespace srsenb