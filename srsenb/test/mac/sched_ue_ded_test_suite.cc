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

#include "sched_ue_ded_test_suite.h"
#include "lib/include/srsran/mac/pdu.h"
#include "srsenb/hdr/stack/mac/sched_helpers.h"
#include "srsenb/hdr/stack/mac/sched_phy_ch/sf_cch_allocator.h"
#include "srsran/common/test_common.h"

namespace srsenb {

using phich_t = sched_interface::ul_sched_phich_t;
using pusch_t = sched_interface::ul_sched_data_t;
using pdsch_t = sched_interface::dl_sched_data_t;

const sched_interface::ue_cfg_t::cc_cfg_t* sim_ue_ctxt_t::get_cc_cfg(uint32_t enb_cc_idx) const
{
  // TODO: verify SCELL Act was received
  auto it =
      std::find_if(ue_cfg.supported_cc_list.begin(),
                   ue_cfg.supported_cc_list.end(),
                   [enb_cc_idx](const sched_interface::ue_cfg_t::cc_cfg_t& cc) { return cc.enb_cc_idx == enb_cc_idx; });
  return (it == ue_cfg.supported_cc_list.end()) ? nullptr : &(*it);
}

int sim_ue_ctxt_t::enb_to_ue_cc_idx(uint32_t enb_cc_idx) const
{
  auto it =
      std::find_if(ue_cfg.supported_cc_list.begin(),
                   ue_cfg.supported_cc_list.end(),
                   [enb_cc_idx](const sched_interface::ue_cfg_t::cc_cfg_t& cc) { return cc.enb_cc_idx == enb_cc_idx; });
  return it == ue_cfg.supported_cc_list.end() ? -1 : std::distance(ue_cfg.supported_cc_list.begin(), it);
}

const phich_t* find_phich_grant(uint16_t rnti, const sched_interface::ul_sched_res_t& ul_cc_res)
{
  const phich_t* phich_ptr = std::find_if(
      ul_cc_res.phich.begin(), ul_cc_res.phich.end(), [rnti](const phich_t& phich) { return phich.rnti == rnti; });
  return phich_ptr == ul_cc_res.phich.end() ? nullptr : phich_ptr;
}

const pusch_t* find_pusch_grant(uint16_t rnti, const sched_interface::ul_sched_res_t& ul_cc_res)
{
  const pusch_t* ptr = std::find_if(
      ul_cc_res.pusch.begin(), ul_cc_res.pusch.end(), [rnti](const pusch_t& pusch) { return pusch.dci.rnti == rnti; });
  return ptr == ul_cc_res.pusch.end() ? nullptr : ptr;
}

const pdsch_t* find_pdsch_grant(uint16_t rnti, const sched_interface::dl_sched_res_t& dl_cc_res)
{
  const pdsch_t* ptr = std::find_if(
      dl_cc_res.data.begin(), dl_cc_res.data.end(), [rnti](const pdsch_t& pdsch) { return pdsch.dci.rnti == rnti; });
  return ptr == dl_cc_res.data.end() ? nullptr : ptr;
}

int test_pdsch_grant(const sim_enb_ctxt_t&                   enb_ctxt,
                     const sf_output_res_t&                  sf_out,
                     uint32_t                                enb_cc_idx,
                     const sched_interface::dl_sched_data_t& pdsch)
{
  tti_point                                  tti_rx      = sf_out.tti_rx;
  const sim_ue_ctxt_t&                       ue_ctxt     = *enb_ctxt.ue_db.at(pdsch.dci.rnti);
  const sched_interface::ue_cfg_t::cc_cfg_t* cc_cfg      = ue_ctxt.get_cc_cfg(enb_cc_idx);
  const sched_cell_params_t&                 cell_params = enb_ctxt.cell_params[enb_cc_idx];
  bool has_pusch_grant = find_pusch_grant(pdsch.dci.rnti, sf_out.ul_cc_result[enb_cc_idx]) != nullptr;

  // TEST: Check if CC is configured and active
  CONDERROR(cc_cfg == nullptr or not cc_cfg->active, "PDSCH allocation for disabled or unavailable cc");
  CONDERROR(pdsch.dci.ue_cc_idx != (uint32_t)std::distance(&ue_ctxt.ue_cfg.supported_cc_list.front(), cc_cfg),
            "Inconsistent enb_cc_idx -> ue_cc_idx mapping");

  // TEST: DCI is consistent with current UE DL harq state
  auto&    h        = ue_ctxt.cc_list[pdsch.dci.ue_cc_idx].dl_harqs[pdsch.dci.pid];
  uint32_t nof_retx = get_nof_retx(pdsch.dci.tb[0].rv); // 0..3
  if (h.nof_txs == 0 or h.ndi != pdsch.dci.tb[0].ndi) {
    // It is newtx
    CONDERROR(nof_retx != 0, "Invalid rv index for new DL tx");
    CONDERROR(h.active, "DL newtx for already active DL harq pid=%d", h.pid);
  } else {
    // it is retx
    CONDERROR(get_rvidx(h.nof_retxs + 1) != (uint32_t)pdsch.dci.tb[0].rv, "Invalid rv index for retx");
    CONDERROR(not h.active, "retx for inactive dl harq pid=%d", h.pid);
    CONDERROR(to_tx_dl_ack(h.last_tti_rx) > tti_rx, "harq pid=%d reused too soon", h.pid);
    CONDERROR(h.nof_retxs + 1 > ue_ctxt.ue_cfg.maxharq_tx,
              "The number of retx=%d exceeded its max=%d",
              h.nof_retxs + 1,
              ue_ctxt.ue_cfg.maxharq_tx);
    //    CONDERROR(h.dci_loc.L != pdsch.dci.location.L, "Harq DCI aggregation level changed.");
    CONDERROR(h.tbs != pdsch.tbs[0], "TBS changed during HARQ retx");
  }

  // TEST: max coderate is not exceeded
  if (h.nof_txs == 0 or h.ndi != pdsch.dci.tb[0].ndi) {
    // it is newtx
    srsran_pdsch_grant_t grant = {};
    srsran_dl_sf_cfg_t   dl_sf = {};
    dl_sf.cfi                  = sf_out.dl_cc_result[enb_cc_idx].cfi;
    dl_sf.tti                  = to_tx_dl(tti_rx).to_uint();
    srsran_ra_dl_grant_to_grant_prb_allocation(&pdsch.dci, &grant, cell_params.nof_prb());
    uint32_t     nof_re   = srsran_ra_dl_grant_nof_re(&cell_params.cfg.cell, &dl_sf, &grant);
    float        coderate = srsran_coderate(pdsch.tbs[0] * 8, nof_re);
    srsran_mod_t mod      = srsran_ra_dl_mod_from_mcs(pdsch.dci.tb[0].mcs_idx, ue_ctxt.ue_cfg.use_tbs_index_alt);
    uint32_t     max_Qm   = ue_ctxt.ue_cfg.use_tbs_index_alt ? 8 : 6;
    uint32_t     Qm       = std::min(max_Qm, srsran_mod_bits_x_symbol(mod));
    CONDERROR(coderate > 0.932f * Qm, "Max coderate was exceeded");
  }

  // TEST: PUCCH-ACK will not collide with SR
  CONDERROR(not has_pusch_grant and is_pucch_sr_collision(ue_ctxt.ue_cfg.pucch_cfg,
                                                          to_tx_dl_ack(sf_out.tti_rx),
                                                          pdsch.dci.location.ncce + cell_params.cfg.n1pucch_an),
            "Collision detected between UE PUCCH-ACK and SR");

  return SRSRAN_SUCCESS;
}

int test_dl_sched_result(const sim_enb_ctxt_t& enb_ctxt, const sf_output_res_t& sf_out)
{
  for (uint32_t cc = 0; cc < enb_ctxt.cell_params.size(); ++cc) {
    for (uint32_t i = 0; i < sf_out.dl_cc_result[cc].data.size(); ++i) {
      const sched_interface::dl_sched_data_t& data = sf_out.dl_cc_result[cc].data[i];
      CONDERROR(
          enb_ctxt.ue_db.count(data.dci.rnti) == 0, "Allocated DL grant for non-existent rnti=0x%x", data.dci.rnti);
      TESTASSERT(test_pdsch_grant(enb_ctxt, sf_out, cc, data) == SRSRAN_SUCCESS);
    }
  }
  return SRSRAN_SUCCESS;
}

bool is_in_measgap(srsran::tti_point tti, uint32_t period, uint32_t offset)
{
  if (period == 0) {
    return false;
  }
  uint32_t T = period / 10;
  for (uint32_t i = 0; i < 6; ++i) {
    tti_point tti_gap_start = tti - i;
    bool      is_gap_start  = (tti_gap_start.sfn() % T == offset / 10) and (tti_gap_start.sf_idx() == offset % 10);
    if (is_gap_start) {
      return true;
    }
  }
  return false;
}

int test_ul_sched_result(const sim_enb_ctxt_t& enb_ctxt, const sf_output_res_t& sf_out)
{
  uint32_t pid = to_tx_ul(sf_out.tti_rx).to_uint() % (FDD_HARQ_DELAY_UL_MS + FDD_HARQ_DELAY_DL_MS);

  for (size_t cc = 0; cc < enb_ctxt.cell_params.size(); ++cc) {
    const auto* phich_begin = sf_out.ul_cc_result[cc].phich.begin();
    const auto* phich_end   = sf_out.ul_cc_result[cc].phich.end();
    const auto* pusch_begin = sf_out.ul_cc_result[cc].pusch.begin();
    const auto* pusch_end   = sf_out.ul_cc_result[cc].pusch.end();

    // TEST: rnti must exist for all PHICH
    CONDERROR(std::any_of(phich_begin,
                          phich_end,
                          [&enb_ctxt](const phich_t& phich) { return enb_ctxt.ue_db.count(phich.rnti) == 0; }),
              "Scheduled PHICH does not have associated rnti");

    // TEST: rnti must exist for all PUSCH
    CONDERROR(std::any_of(pusch_begin,
                          pusch_end,
                          [&enb_ctxt](const pusch_t& pusch) { return enb_ctxt.ue_db.count(pusch.dci.rnti) == 0; }),
              "Scheduled PUSCH does not have associated rnti.");

    for (const auto& ue_pair : enb_ctxt.ue_db) {
      const auto& ue        = *ue_pair.second;
      uint16_t    rnti      = ue.rnti;
      int         ue_cc_idx = ue.enb_to_ue_cc_idx(cc);

      const phich_t* phich_ptr = find_phich_grant(rnti, sf_out.ul_cc_result[cc]);
      const pusch_t* pusch_ptr = find_pusch_grant(rnti, sf_out.ul_cc_result[cc]);

      // TEST: Check that idle CCs do not receive PUSCH grants or PHICH
      if (ue_cc_idx < 0 or not ue.ue_cfg.supported_cc_list[ue_cc_idx].active) {
        CONDERROR(phich_ptr != nullptr, "PHICH cannot be allocated in idle cells");
        CONDERROR(pusch_ptr != nullptr, "PUSCH cannot be allocated in idle cells");
        continue;
      }

      const auto& h                 = ue.cc_list[ue_cc_idx].ul_harqs[pid];
      bool        phich_ack         = phich_ptr != nullptr and phich_ptr->phich == phich_t::ACK;
      bool        is_msg3           = h.first_tti_rx == ue.msg3_tti_rx and h.nof_txs == h.nof_retxs + 1;
      uint32_t    max_nof_retxs     = is_msg3 ? sf_out.cc_params[0].cfg.maxharq_msg3tx : ue.ue_cfg.maxharq_tx;
      bool        last_retx         = h.nof_retxs + 1 >= max_nof_retxs;
      tti_point   tti_tx_phich      = to_tx_dl(sf_out.tti_rx);
      bool        phich_in_meas_gap = is_in_measgap(tti_tx_phich, ue.ue_cfg.measgap_period, ue.ue_cfg.measgap_offset);
      bool        pusch_in_meas_gap =
          is_in_measgap(to_tx_ul(sf_out.tti_rx), ue.ue_cfg.measgap_period, ue.ue_cfg.measgap_offset);
      bool h_cleared = (not h.active) or (phich_ack or last_retx);

      // TEST: Already active UL HARQs have to receive PHICH (unless MeasGap collision)
      CONDERROR(h.active and phich_ptr == nullptr and not phich_in_meas_gap,
                "PHICH not received for rnti=0x%x active UL HARQ pid=%d",
                rnti,
                pid);
      CONDERROR(not h.active and phich_ptr != nullptr,
                "PHICH for rnti=0x%x corresponds to inactive UL HARQ pid=%d",
                rnti,
                pid);

      // TEST: absent PUSCH grants for active UL HARQs must be either ACKs, last retx, or interrupted HARQs
      if (phich_ptr != nullptr) {
        CONDERROR(not h_cleared and pusch_ptr == nullptr and not pusch_in_meas_gap,
                  "PHICH NACK received for rnti=0x%x but no PUSCH retx reallocated",
                  rnti);
      }

      if (pusch_ptr != nullptr) {
        CONDERROR(pusch_ptr->dci.ue_cc_idx != (uint32_t)ue_cc_idx, "Inconsistent enb_cc_idx -> ue_cc_idx mapping");

        // TEST: DCI is consistent with current UE UL harq state
        uint32_t nof_retx = get_nof_retx(pusch_ptr->dci.tb.rv); // 0..3

        if (h.nof_txs == 0 or h.ndi != pusch_ptr->dci.tb.ndi) {
          // newtx
          CONDERROR(nof_retx != 0, "Invalid rv index for new UL tx");
          CONDERROR(pusch_ptr->current_tx_nb != 0, "UL HARQ retxs need to have been previously transmitted");
          CONDERROR(not h_cleared, "New tx for already active UL HARQ");
          CONDERROR(not pusch_ptr->needs_pdcch and ue.msg3_tti_rx.is_valid() and sf_out.tti_rx > ue.msg3_tti_rx,
                    "In case of newtx, PDCCH allocation is required, unless it is Msg3");
        } else {
          CONDERROR(pusch_ptr->current_tx_nb == 0, "UL retx has to have nof tx > 0");
          CONDERROR(h.nof_retxs >= max_nof_retxs, "UL max nof retxs exceeded");
          CONDERROR(pusch_ptr->current_tx_nb != h.nof_retxs + 1, "UL HARQ nof_retx mismatch");
          if (not h.active) {
            // the HARQ is being resumed. PDCCH must be active with the exception of Msg3
            CONDERROR(ue.msg4_tti_rx.is_valid() and not pusch_ptr->needs_pdcch,
                      "Resumed UL HARQs need to be signalled in PDCCH");
          } else {
            if (pusch_ptr->needs_pdcch) {
              CONDERROR(pusch_ptr->dci.type2_alloc.riv == h.riv, "Adaptive retx must change riv");
            } else {
              // non-adaptive retx
              CONDERROR(pusch_ptr->dci.type2_alloc.riv != h.riv, "Non-adaptive retx must keep the same riv");
            }
          }
          CONDERROR(get_rvidx(h.nof_retxs + 1) != (uint32_t)pusch_ptr->dci.tb.rv, "Invalid rv index for UL retx");
          CONDERROR(h.tbs != pusch_ptr->tbs, "TBS changed during HARQ retx");
          CONDERROR(to_tx_ul(h.last_tti_rx) > sf_out.tti_rx, "UL harq pid=%d was reused too soon", h.pid);
        }
      }
    }
  }

  return SRSRAN_SUCCESS;
}

int test_ra(const sim_enb_ctxt_t& enb_ctxt, const sf_output_res_t& sf_out)
{
  for (uint32_t cc = 0; cc < enb_ctxt.cell_params.size(); ++cc) {
    const auto& dl_cc_res = sf_out.dl_cc_result[cc];
    const auto& ul_cc_res = sf_out.ul_cc_result[cc];
    for (const auto& ue_pair : enb_ctxt.ue_db) {
      const auto& ue        = *ue_pair.second;
      uint16_t    rnti      = ue.rnti;
      uint32_t    ue_cc_idx = ue.enb_to_ue_cc_idx(cc);

      if (ue_cc_idx != 0) {
        // only check for RAR/Msg3/Msg4 presence for a UE's PCell
        continue;
      }

      // TEST: RAR allocation
      uint32_t             rar_win_size = enb_ctxt.cell_params[cc].cfg.prach_rar_window;
      srsran::tti_interval rar_window{ue.prach_tti_rx + 3, ue.prach_tti_rx + 3 + rar_win_size};
      srsran::tti_point    tti_tx_dl = to_tx_dl(sf_out.tti_rx);

      if (not rar_window.contains(tti_tx_dl)) {
        CONDERROR(not ue.rar_tti_rx.is_valid() and tti_tx_dl > rar_window.stop(),
                  "rnti=0x%x RAR not scheduled within the RAR Window",
                  rnti);
        for (uint32_t i = 0; i < sf_out.dl_cc_result[cc].rar.size(); ++i) {
          CONDERROR(sf_out.dl_cc_result[cc].rar[i].dci.rnti == rnti,
                    "No RAR allocations allowed outside of user RAR window");
        }
      } else {
        // Inside RAR window
        uint32_t nof_rars = ue.rar_tti_rx.is_valid() ? 1 : 0;
        for (uint32_t i = 0; i < dl_cc_res.rar.size(); ++i) {
          for (const auto& grant : dl_cc_res.rar[i].msg3_grant) {
            const auto& data = grant.data;
            if (data.prach_tti == (uint32_t)ue.prach_tti_rx.to_uint() and data.preamble_idx == ue.preamble_idx) {
              CONDERROR(rnti != data.temp_crnti, "RAR grant C-RNTI does not match the expected.");
              nof_rars++;
            }
          }
        }
        CONDERROR(nof_rars > 1, "There was more than one RAR for the same user");
      }

      // TEST: Msg3 was allocated
      if (ue.rar_tti_rx.is_valid() and not ue.msg3_tti_rx.is_valid()) {
        // RAR scheduled, Msg3 not yet scheduled
        srsran::tti_point expected_msg3_tti_rx = ue.rar_tti_rx + MSG3_DELAY_MS;
        CONDERROR(expected_msg3_tti_rx < sf_out.tti_rx, "No UL msg3 alloc was made");

        if (expected_msg3_tti_rx == sf_out.tti_rx) {
          // Msg3 should exist
          uint32_t msg3_count = 0;
          for (uint32_t i = 0; i < ul_cc_res.pusch.size(); ++i) {
            if (ul_cc_res.pusch[i].dci.rnti == rnti) {
              msg3_count++;
              CONDERROR(ul_cc_res.pusch[i].needs_pdcch, "Msg3 allocations do not require PDCCH");
              CONDERROR(ue.msg3_riv != ul_cc_res.pusch[i].dci.type2_alloc.riv,
                        "The Msg3 was not allocated in the expected PRBs.");
            }
          }
          CONDERROR(msg3_count == 0, "Msg3 was not transmitted.");
          CONDERROR(msg3_count > 1, "Only one Msg3 allower per user.");
        }
      }

      // TEST: Check Msg4
      if (ue.msg3_tti_rx.is_valid() and not ue.msg4_tti_rx.is_valid()) {
        // Msg3 scheduled, but Msg4 not yet scheduled
        uint32_t msg4_count = 0;
        for (uint32_t i = 0; i < dl_cc_res.data.size(); ++i) {
          if (dl_cc_res.data[i].dci.rnti == rnti) {
            CONDERROR(to_tx_dl(sf_out.tti_rx) < to_tx_ul(ue.msg3_tti_rx),
                      "Msg4 cannot be scheduled without Msg3 being tx");
            for (uint32_t j = 0; j < dl_cc_res.data[i].nof_pdu_elems[0]; ++j) {
              if (dl_cc_res.data[i].pdu[0][j].lcid == (uint32_t)srsran::dl_sch_lcid::CON_RES_ID) {
                // ConRes found
                CONDERROR(dl_cc_res.data[i].dci.format != SRSRAN_DCI_FORMAT1 and
                              dl_cc_res.data[i].dci.format != SRSRAN_DCI_FORMAT1A,
                          "ConRes must be format1/1a");
                msg4_count++;
              }
            }
            CONDERROR(msg4_count == 0, "No ConRes CE was scheduled in Msg4");
          }
        }
        CONDERROR(msg4_count > 1, "Duplicate ConRes CE for the same rnti");
      }

      if (not ue.msg4_tti_rx.is_valid()) {
        // TEST: No UL allocs except for Msg3 before Msg4
        for (uint32_t i = 0; i < ul_cc_res.pusch.size(); ++i) {
          if (ul_cc_res.pusch[i].dci.rnti == rnti) {
            tti_point rar_tti_rx = ue.rar_tti_rx;
            if (not rar_tti_rx.is_valid()) {
              for (uint32_t j = 0; j < dl_cc_res.rar.size(); ++j) {
                for (const auto& grant : dl_cc_res.rar[i].msg3_grant) {
                  if (grant.data.temp_crnti == ue.rnti) {
                    rar_tti_rx = sf_out.tti_rx;
                    break;
                  }
                }
              }
            }
            CONDERROR(not rar_tti_rx.is_valid(), "No UL allocs before RAR allowed");
            srsran::tti_point expected_msg3_tti = rar_tti_rx + MSG3_DELAY_MS;
            CONDERROR(expected_msg3_tti > sf_out.tti_rx, "No UL allocs before Msg3 is scheduled");
            if (expected_msg3_tti < sf_out.tti_rx) {
              bool msg3_retx =
                  ((ue.msg3_tti_rx - expected_msg3_tti) % (FDD_HARQ_DELAY_UL_MS + FDD_HARQ_DELAY_DL_MS)) == 0;
              CONDERROR(not msg3_retx, "No UL txs allowed except for Msg3 before user received Msg4");
            }
          }
        }

        // TEST: No DL allocs before Msg3
        if (not ue.msg3_tti_rx.is_valid()) {
          for (uint32_t i = 0; i < dl_cc_res.data.size(); ++i) {
            CONDERROR(dl_cc_res.data[i].dci.rnti == rnti, "No DL data allocs allowed before Msg3 is scheduled");
          }
        }
      }
    }

    // TEST: Ensure there are no spurious RARs that do not belong to any user
    for (uint32_t i = 0; i < dl_cc_res.rar.size(); ++i) {
      for (uint32_t j = 0; j < dl_cc_res.rar[i].msg3_grant.size(); ++j) {
        uint32_t prach_tti    = dl_cc_res.rar[i].msg3_grant[j].data.prach_tti;
        uint32_t preamble_idx = dl_cc_res.rar[i].msg3_grant[j].data.preamble_idx;
        auto     it           = std::find_if(
            enb_ctxt.ue_db.begin(), enb_ctxt.ue_db.end(), [&](const std::pair<uint16_t, const sim_ue_ctxt_t*>& u) {
              const auto& ctxt = *u.second;
              return ctxt.preamble_idx == preamble_idx and ((uint32_t)ctxt.prach_tti_rx.to_uint() == prach_tti);
            });
        CONDERROR(it == enb_ctxt.ue_db.end(), "There was a RAR allocation with no associated user");
        CONDERROR(it->second->ue_cfg.supported_cc_list[0].enb_cc_idx != cc, "The allocated RAR is in the wrong cc");
      }
    }
  }

  return SRSRAN_SUCCESS;
}

int test_meas_gaps(const sim_enb_ctxt_t& enb_ctxt, const sf_output_res_t& sf_out)
{
  for (uint32_t cc = 0; cc < enb_ctxt.cell_params.size(); ++cc) {
    const auto& dl_cc_res = sf_out.dl_cc_result[cc];
    const auto& ul_cc_res = sf_out.ul_cc_result[cc];
    for (const auto& ue_pair : enb_ctxt.ue_db) {
      const auto&       ue        = *ue_pair.second;
      uint16_t          rnti      = ue.rnti;
      uint32_t          ue_cc_idx = ue.enb_to_ue_cc_idx(cc);
      srsran::tti_point tti_tx_ul = to_tx_ul(sf_out.tti_rx), tti_tx_dl = to_tx_dl(sf_out.tti_rx),
                        tti_tx_dl_ack = to_tx_dl_ack(sf_out.tti_rx);

      if (ue_cc_idx != 0 or ue.ue_cfg.measgap_period == 0) {
        continue;
      }

      if (is_in_measgap(tti_tx_dl, ue.ue_cfg.measgap_period, ue.ue_cfg.measgap_offset)) {
        const phich_t* phich_ptr = find_phich_grant(rnti, ul_cc_res);
        CONDERROR(phich_ptr != nullptr, "PHICH grants cannot fall in UE measGap");
      }
      if (is_in_measgap(tti_tx_ul, ue.ue_cfg.measgap_period, ue.ue_cfg.measgap_offset)) {
        const pusch_t* pusch_ptr = find_pusch_grant(rnti, ul_cc_res);
        CONDERROR(pusch_ptr != nullptr, "PUSCH grants cannot fall in UE measGap");
      }
      if (is_in_measgap(tti_tx_dl, ue.ue_cfg.measgap_period, ue.ue_cfg.measgap_offset) or
          is_in_measgap(tti_tx_dl_ack, ue.ue_cfg.measgap_period, ue.ue_cfg.measgap_offset)) {
        const pdsch_t* pdsch_ptr = find_pdsch_grant(rnti, dl_cc_res);
        CONDERROR(pdsch_ptr != nullptr, "PDSCH grants and respective ACKs cannot fall in UE measGap");
      }
    }
  }
  return SRSRAN_SUCCESS;
}

int test_all_ues(const sim_enb_ctxt_t& enb_ctxt, const sf_output_res_t& sf_out)
{
  TESTASSERT(test_dl_sched_result(enb_ctxt, sf_out) == SRSRAN_SUCCESS);

  TESTASSERT(test_ul_sched_result(enb_ctxt, sf_out) == SRSRAN_SUCCESS);

  TESTASSERT(test_ra(enb_ctxt, sf_out) == SRSRAN_SUCCESS);

  TESTASSERT(test_meas_gaps(enb_ctxt, sf_out) == SRSRAN_SUCCESS);

  return SRSRAN_SUCCESS;
}

} // namespace srsenb
