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

#include "srsgnb/hdr/stack/mac/sched_nr_signalling.h"
#include "srsgnb/hdr/stack/mac/sched_nr_grant_allocator.h"

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
        if (csi_rs_list.full()) {
          srslog::fetch_basic_logger("MAC-NR").error("SCHED: Failed to allocate NZP-CSI RS");
          return;
        }
        csi_rs_list.push_back(nzp_csi_resource);
      }
    }
  }
}

void sched_ssb_basic(const slot_point& sl_point, uint32_t ssb_periodicity, ssb_list& ssb_list)
{
  if (ssb_list.full()) {
    srslog::fetch_basic_logger("MAC-NR").error("SCHED: Failed to allocate SSB");
    return;
  }
  // If the periodicity is 0, it means that the parameter was not passed by the upper layers.
  // In that case, we use default value of 5ms (see Clause 4.1, TS 38.213)
  if (ssb_periodicity == 0) {
    ssb_periodicity = DEFAULT_SSB_PERIODICITY;
  }

  uint32_t sl_cnt = sl_point.to_uint();
  // Perform mod operation of slot index by ssb_periodicity;
  // "ssb_periodicity * nof_slots_per_subframe" gives the number of slots in 1 ssb_periodicity time interval
  uint32_t sl_point_mod = sl_cnt % (ssb_periodicity * (uint32_t)sl_point.nof_slots_per_subframe());

  // code below is simplified, it assumes 15kHz subcarrier spacing and sub 3GHz carrier
  if (sl_point_mod == 0) {
    ssb_t           ssb_msg = {};
    srsran_mib_nr_t mib_msg = {};
    mib_msg.sfn             = sl_point.sfn();
    mib_msg.hrf             = (sl_point.slot_idx() % SRSRAN_NSLOTS_PER_FRAME_NR(srsran_subcarrier_spacing_15kHz) >=
                   SRSRAN_NSLOTS_PER_FRAME_NR(srsran_subcarrier_spacing_15kHz) / 2);
    // This corresponds to "Position in Burst" = 1000
    mib_msg.ssb_idx = 0;
    // Setting the following 4 parameters is redundant, but it makes it explicit what values are passed to PHY
    mib_msg.dmrs_typeA_pos = srsran_dmrs_sch_typeA_pos_2;
    mib_msg.scs_common     = srsran_subcarrier_spacing_15kHz;
    mib_msg.coreset0_idx   = 0;
    mib_msg.ss0_idx        = 0;

    // Pack mib message to be sent to PHY
    int packing_ret_code = srsran_pbch_msg_nr_mib_pack(&mib_msg, &ssb_msg.pbch_msg);
    srsran_assert(packing_ret_code == SRSRAN_SUCCESS, "SSB packing returned en error");
    ssb_list.push_back(ssb_msg);
  }
}

void sched_dl_signalling(bwp_slot_allocator& bwp_alloc)
{
  const bwp_params_t& bwp_params = bwp_alloc.cfg;
  slot_point          sl_pdcch   = bwp_alloc.get_pdcch_tti();
  bwp_slot_grid&      sl_grid    = bwp_alloc.tx_slot_grid();

  srsran_slot_cfg_t cfg;
  cfg.idx = sl_pdcch.to_uint();

  // Schedule SSB
  sched_ssb_basic(sl_pdcch, bwp_params.cell_cfg.ssb.periodicity_ms, sl_grid.dl.phy.ssb);

  // Schedule NZP-CSI-RS
  sched_nzp_csi_rs(bwp_params.cfg.pdsch.nzp_csi_rs_sets, cfg, sl_grid.dl.phy.nzp_csi_rs);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool fill_dci_sib(prb_interval interv, uint32_t sib_id, const bwp_params_t& bwp_cfg, srsran_dci_dl_nr_t& dci)
{
  dci.mcs                   = 5;
  dci.ctx.format            = srsran_dci_format_nr_1_0;
  dci.ctx.ss_type           = srsran_search_space_type_common_0;
  dci.ctx.rnti_type         = srsran_rnti_type_si;
  dci.ctx.rnti              = SRSRAN_SIRNTI;
  dci.ctx.coreset_id        = 0;
  dci.freq_domain_assigment = srsran_ra_nr_type1_riv(bwp_cfg.cfg.rb_width, interv.start(), interv.length());
  dci.time_domain_assigment = 0;
  dci.tpc                   = 1;
  dci.bwp_id                = bwp_cfg.bwp_id;
  dci.cc_id                 = bwp_cfg.cc;
  dci.rv                    = 0;
  dci.sii                   = sib_id == 1 ? 0 : 1;

  return true;
}

si_sched::si_sched(const bwp_params_t& bwp_cfg_) :
  bwp_cfg(&bwp_cfg_), logger(srslog::fetch_basic_logger(bwp_cfg_.sched_cfg.logger_name))
{
  // TODO: Get SIB1 other SI msgs config from RRC
  pending_sis.emplace_back();
  pending_sis[0].n       = 0;
  pending_sis[0].len     = 77;
  pending_sis[0].period  = 160;
  pending_sis[0].win_len = 1;
}

void si_sched::run_slot(bwp_slot_allocator& bwp_alloc)
{
  if (true) {
    // CORESET#0 must be present, otherwise SIs are not allocated
    // TODO: provide proper config
    return;
  }
  const uint32_t    si_aggr_level = 2;
  slot_point        sl_pdcch      = bwp_alloc.get_pdcch_tti();
  const prb_bitmap& prbs          = bwp_alloc.res_grid()[sl_pdcch].dl_prbs.prbs();

  // Update SI windows
  uint32_t N = bwp_cfg->slots.size();
  for (si_msg_ctxt_t& si : pending_sis) {
    uint32_t x = (si.n - 1) * si.win_len;

    if (not si.win_start.valid()) {
      bool start_window;
      if (si.n == 0) {
        // SIB1
        start_window = sl_pdcch.to_uint() % si.period == 0;
      } else {
        // SI messages
        start_window = (sl_pdcch.sfn() % si.period == x / N) and sl_pdcch.slot_idx() == x % bwp_cfg->slots.size();
      }
      if (start_window) {
        // If start of SI message window
        si.win_start = sl_pdcch;
      }
    } else if (si.win_start + si.win_len >= sl_pdcch) {
      // If end of SI message window
      if (si.n == 0) {
        logger.error("SCHED: Could not allocate SIB1, len=%d. Cause: %s", si.n, si.len, to_string(si.result));
      } else {
        logger.warning(
            "SCHED: Could not allocate SI message idx=%d, len=%d. Cause: %s", si.n, si.len, to_string(si.result));
      }
      si.win_start.clear();
    }
  }

  // Schedule pending SIBs
  if (not bwp_cfg->slots[sl_pdcch.slot_idx()].is_dl) {
    return;
  }
  for (si_msg_ctxt_t& si : pending_sis) {
    if (not si.win_start.valid()) {
      continue;
    }

    // TODO: NOTE 2: The UE is not required to monitor PDCCH monitoring occasion(s) corresponding to each transmitted
    // SSB in SI-window.

    // Attempt grants with increasing number of PRBs (if the number of PRBs is too low, the coderate is invalid)
    si.result              = alloc_result::invalid_coderate;
    uint32_t prb_start_idx = 0;
    for (uint32_t nprbs = 4; nprbs < bwp_cfg->cfg.rb_width and si.result == alloc_result::invalid_coderate; ++nprbs) {
      prb_interval grant = find_empty_interval_of_length(prbs, nprbs, prb_start_idx);
      prb_start_idx      = grant.start();
      if (grant.length() != nprbs) {
        si.result = alloc_result::no_sch_space;
        break;
      }
      si.result = bwp_alloc.alloc_si(si_aggr_level, si.n, si.n_tx, grant);
      if (si.result == alloc_result::success) {
        // SIB scheduled successfully
        si.win_start.clear();
        si.n_tx++;
        if (si.n == 0) {
          logger.debug("SCHED: Allocated SIB1, len=%d.", si.n, si.len);
        } else {
          logger.debug("SCHED: Allocated SI message idx=%d, len=%d.", si.n, si.len);
        }
      }
    }
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

} // namespace sched_nr_impl
} // namespace srsenb
