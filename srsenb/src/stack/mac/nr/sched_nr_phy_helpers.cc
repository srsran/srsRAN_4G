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

void bitmap_to_prb_array(const rbgmask_t& bitmap, uint32_t bwp_nof_prb, srsran::span<bool> prbs)
{
  uint32_t count = 0;
  for (uint32_t rbg = 0; rbg < bitmap.size(); ++rbg) {
    bool     val      = bitmap.test(rbg);
    uint32_t rbg_size = get_rbg_size(bwp_nof_prb, 0, true, rbg);
    for (uint32_t prb_idx = count; prb_idx < count + rbg_size; ++prb_idx) {
      prbs[prb_idx] = val;
    }
  }
}

srsran::interval<uint32_t> find_first_interval(const rbgmask_t& mask)
{
  int rb_start = mask.find_lowest(0, mask.size());
  if (rb_start != -1) {
    int rb_end = mask.find_lowest(rb_start + 1, mask.size(), false);
    return {(uint32_t)rb_start, (uint32_t)(rb_end < 0 ? mask.size() : rb_end)};
  }
  return {};
}

int bitmap_to_riv(const rbgmask_t& bitmap, uint32_t cell_nof_prb)
{
  srsran::interval<uint32_t> interv = find_first_interval(bitmap);
  srsran_assert(interv.length() == bitmap.count(), "Trying to acquire riv for non-contiguous bitmap");
  return srsran_ra_nr_type1_riv(cell_nof_prb, interv.start(), interv.length());
}

template <typename DciDlOrUl>
void fill_dci_common(const slot_ue& ue, const rbgmask_t& bitmap, const sched_cell_params& cc_cfg, DciDlOrUl& dci)
{
  // Note: PDCCH DCI position already filled at this point
  dci.bwp_id                = ue.bwp_id;
  dci.cc_id                 = ue.cc;
  dci.freq_domain_assigment = bitmap_to_riv(bitmap, cc_cfg.cell_cfg.nof_prb);
  dci.ctx.rnti              = ue.rnti;
  dci.ctx.rnti_type         = srsran_rnti_type_c;
  dci.tpc                   = 1;
  // harq
  harq_proc* h = std::is_same<DciDlOrUl, srsran_dci_dl_nr_t>::value ? ue.h_dl : ue.h_ul;
  dci.pid      = h->pid;
  dci.ndi      = h->ndi();
  dci.mcs      = h->mcs();
}

void fill_dci_ue_cfg(const slot_ue&           ue,
                     const rbgmask_t&         rbgmask,
                     const sched_cell_params& cc_cfg,
                     srsran_dci_dl_nr_t&      dci)
{
  fill_dci_common(ue, rbgmask, cc_cfg, dci);
}

void fill_dci_ue_cfg(const slot_ue&           ue,
                     const rbgmask_t&         rbgmask,
                     const sched_cell_params& cc_cfg,
                     srsran_dci_ul_nr_t&      dci)
{
  fill_dci_common(ue, rbgmask, cc_cfg, dci);
}

void fill_sch_ue_common(const slot_ue&           ue,
                        const rbgmask_t&         rbgmask,
                        const sched_cell_params& cc_cfg,
                        srsran_sch_cfg_nr_t&     sch)
{
  sch.grant.rnti_type  = srsran_rnti_type_c;
  sch.grant.rnti       = ue.rnti;
  sch.grant.nof_layers = 1;
  sch.grant.nof_prb    = cc_cfg.cell_cfg.nof_prb;
}

void fill_pdsch_ue(const slot_ue&           ue,
                   const rbgmask_t&         rbgmask,
                   const sched_cell_params& cc_cfg,
                   srsran_sch_cfg_nr_t&     sch)
{
  fill_sch_ue_common(ue, rbgmask, cc_cfg, sch);
  sch.grant.k          = ue.cc_cfg->pdsch_res_list[0].k0;
  sch.grant.dci_format = srsran_dci_format_nr_1_0;
}

void fill_pusch_ue(const slot_ue&           ue,
                   const rbgmask_t&         rbgmask,
                   const sched_cell_params& cc_cfg,
                   srsran_sch_cfg_nr_t&     sch)
{
  fill_sch_ue_common(ue, rbgmask, cc_cfg, sch);
  sch.grant.k          = ue.cc_cfg->pusch_res_list[0].k2;
  sch.grant.dci_format = srsran_dci_format_nr_0_1;
}

} // namespace sched_nr_impl
} // namespace srsenb