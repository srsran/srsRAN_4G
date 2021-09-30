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

#include "srsenb/hdr/stack/mac/nr/sched_nr_signalling.h"

#define POS_IN_BURST_FIRST_BIT_IDX 0
#define POS_IN_BURST_SECOND_BIT_IDX 1
#define POS_IN_BURST_THIRD_BIT_IDX 2
#define POS_IN_BURST_FOURTH_BIT_IDX 3

#define DEFAULT_SSB_PERIODICITY 5

namespace srsenb {
namespace sched_nr_impl {

void sched_nzp_csi_rs(srsran::const_span<srsran_csi_rs_nzp_set_t> nzp_csi_rs_sets_cfg,
                      const srsran_slot_cfg_t&                    slot_cfg,
                      nzp_csi_rs_list&                            csi_rs_list)
{
  for (const srsran_csi_rs_nzp_set_t& set : nzp_csi_rs_sets_cfg) {
    // For each NZP-CSI-RS resource available in the set
    for (uint32_t i = 0; i < set.count; ++i) {
      // Select resource
      const srsran_csi_rs_nzp_resource_t& nzp_csi_resource = set.data[i];

      // Check if the resource is scheduled for this slot
      if (srsran_csi_rs_send(&nzp_csi_resource.periodicity, &slot_cfg)) {
        csi_rs_list.push_back(nzp_csi_resource);
      }
    }
  }
}

void sched_ssb_basic(const slot_point& sl_point, uint32_t ssb_periodicity, ssb_list& ssb_list)
{
  // If the periodicity is 0, it means that the parameter was not passed by the upper layers.
  // In that case, we use default value of 5ms (see Clause 4.1, TS 38.213)
  if (ssb_periodicity == 0)
    ssb_periodicity = DEFAULT_SSB_PERIODICITY;

  uint32_t sl_idx = sl_point.to_uint();
  // mod operation of slot index by ssb_periodicity. With current subcarrier spacing, 1 slot = 1ms
  uint32_t sl_point_mod = sl_point.to_uint() % ssb_periodicity;

  // code below is simplified
  if (sl_point_mod == 0) {
    ssb_t           ssb_msg = {};
    srsran_mib_nr_t mib_msg = {};
    mib_msg.sfn             = sl_point.sfn();
    mib_msg.hrf             = (sl_idx % SRSRAN_NSLOTS_PER_FRAME_NR(srsran_subcarrier_spacing_15kHz) <
                   SRSRAN_NSLOTS_PER_FRAME_NR(srsran_subcarrier_spacing_15kHz) / 2);
    // This corresponds to "Position in Burst" = 1000
    mib_msg.ssb_idx = 0;
    // Pack mib message to be sent to PHY
    srsran_pbch_msg_nr_mib_pack(&mib_msg, &ssb_msg.pbch_msg);
    ssb_list.push_back(ssb_msg);
  }
}

void sched_dl_signalling(const bwp_params& bwp_params,
                         slot_point        sl_pdcch,
                         ssb_list&         ssb_list,
                         nzp_csi_rs_list&  nzp_csi_rs)
{
  srsran_slot_cfg_t cfg;
  cfg.idx = sl_pdcch.to_uint();

  // Schedule SSB
  sched_ssb_basic(sl_pdcch, bwp_params.cell_cfg.ssb.periodicity_ms, ssb_list);

  // Schedule NZP-CSI-RS
  sched_nzp_csi_rs(bwp_params.cfg.pdsch.nzp_csi_rs_sets, cfg, nzp_csi_rs);

  // Schedule SIBs
  // TODO
}

} // namespace sched_nr_impl
} // namespace srsenb
