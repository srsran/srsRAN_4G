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

#include "srsgnb/hdr/stack/mac/sched_nr_sch.h"
#include "srsran/common/string_helpers.h"

namespace srsenb {
namespace sched_nr_impl {

template <typename... Args>
void log_alloc_failure(srslog::log_channel& log_ch, const char* cause_fmt, Args&&... args)
{
  if (not log_ch.enabled()) {
    return;
  }

  // Log allocation failure
  fmt::memory_buffer fmtbuf;
  fmt::format_to(fmtbuf, "SCHED: Failure to allocate PDSCH. Cause: ");
  fmt::format_to(fmtbuf, cause_fmt, std::forward<Args>(args)...);
  log_ch("%s", srsran::to_c_str(fmtbuf));
}

pdsch_allocator::pdsch_allocator(const bwp_params_t& cfg_, uint32_t slot_index, pdsch_list_t& pdsch_lst) :
  bwp_cfg(cfg_),
  slot_idx(slot_index),
  pdschs(pdsch_lst),
  dl_prbs(bwp_cfg.cfg.rb_width, bwp_cfg.cfg.start_rb, bwp_cfg.cfg.pdsch.rbg_size_cfg_1)
{}

void pdsch_allocator::reset()
{
  pdschs.clear();
  dl_prbs.reset();
}

alloc_result pdsch_allocator::is_grant_valid_common(srsran_search_space_type_t ss_type,
                                                    srsran_dci_format_nr_t     dci_fmt,
                                                    uint32_t                   coreset_id,
                                                    const prb_grant&           grant) const
{
  // DL must be active in given slot
  if (not bwp_cfg.slots[slot_idx].is_dl) {
    log_alloc_failure(bwp_cfg.logger.error, "DL is disabled for slot={}", slot_idx);
    return alloc_result::no_sch_space;
  }

  // No space in Scheduler PDSCH output list
  if (pdschs.full()) {
    log_alloc_failure(bwp_cfg.logger.warning, "Maximum number of PDSCHs={} reached.", pdschs.size());
    return alloc_result::no_sch_space;
  }

  // TS 38.214, 5.1.2.2 - "The UE shall assume that when the scheduling grant is received with DCI format 1_0, then
  //                       downlink resource allocation type 1 is used."
  if (dci_fmt == srsran_dci_format_nr_1_0 and not grant.is_alloc_type1()) {
    log_alloc_failure(bwp_cfg.logger.warning, "DL Resource Allocation type 1 must be used in case of DCI format 1_0.");
    return alloc_result::invalid_grant_params;
  }

  // TS 38.214 - 5.1.2.2 - For DCI format 1_0 and Common Search Space, the list of available PRBs is limited by the
  //                       rb_start and bandwidth of the coreset
  if (dci_fmt == srsran_dci_format_nr_1_0 and SRSRAN_SEARCH_SPACE_IS_COMMON(ss_type)) {
    // Grant PRBs do not collide with CORESET PRB limits (in case of common SearchSpace)
    if (bwp_cfg.dci_fmt_1_0_excluded_prbs(coreset_id).collides(grant)) {
      log_alloc_failure(
          bwp_cfg.logger.debug, "Provided PRB grant={:x} falls outside common CORESET PRB boundaries.", grant);
      return alloc_result::sch_collision;
    }
  }

  // Grant PRBs do not collide with previous PDSCH allocations
  if (dl_prbs.collides(grant)) {
    log_alloc_failure(
        bwp_cfg.logger.debug, "Provided PRB grant={:x} collides with allocations previously made.", grant);
    return alloc_result::sch_collision;
  }

  return alloc_result::success;
}

alloc_result pdsch_allocator::is_si_grant_valid(uint32_t ss_id, const prb_grant& grant) const
{
  // Verify SearchSpace validity
  const srsran_search_space_t* ss = bwp_cfg.get_ss(ss_id);
  if (ss == nullptr) {
    // Couldn't find SearchSpace
    log_alloc_failure(bwp_cfg.logger.error, "SearchSpace has not been configured.");
    return alloc_result::invalid_grant_params;
  }
  return is_grant_valid_common(ss->type, srsran_dci_format_nr_1_0, ss->coreset_id, grant);
}

alloc_result pdsch_allocator::is_rar_grant_valid(const prb_grant& grant) const
{
  srsran_sanity_check(bwp_cfg.cfg.pdcch.ra_search_space_present,
                      "Attempting RAR allocation in BWP with no raSearchSpace");
  return is_grant_valid_common(bwp_cfg.cfg.pdcch.ra_search_space.type,
                               srsran_dci_format_nr_1_0,
                               bwp_cfg.cfg.pdcch.ra_search_space.coreset_id,
                               grant);
}

alloc_result pdsch_allocator::is_ue_grant_valid(const ue_carrier_params_t& ue,
                                                uint32_t                   ss_id,
                                                srsran_dci_format_nr_t     dci_fmt,
                                                const prb_grant&           grant) const
{
  const srsran_search_space_t* ss = ue.get_ss(ss_id);
  if (ss == nullptr) {
    // Couldn't find SearchSpace
    log_alloc_failure(bwp_cfg.logger.error, "rnti=0x%x,SearchSpaceId={} has not been configured.", ue.rnti, ss_id);
    return alloc_result::invalid_grant_params;
  }
  alloc_result ret = is_grant_valid_common(ss->type, dci_fmt, ss->coreset_id, grant);
  if (ret != alloc_result::success) {
    return ret;
  }

  // TS 38.214, 5.1.2.2 - "the UE shall use the downlink frequency resource allocation type as defined by the higher
  //                       layer parameter resourceAllocation"
  if (ue.phy().pdsch.alloc != srsran_resource_alloc_dynamic) {
    if ((ue.phy().pdsch.alloc == srsran_resource_alloc_type0) != grant.is_alloc_type0()) {
      log_alloc_failure(bwp_cfg.logger.warning,
                        "UE rnti=0x{:x} PDSCH RA configuration type {} doesn't match grant type",
                        ue.rnti,
                        grant.is_alloc_type0() ? 0 : 1);
      return alloc_result::invalid_grant_params;
    }
  }

  return alloc_result::success;
}

pdsch_alloc_result pdsch_allocator::alloc_si_pdsch(uint32_t ss_id, const prb_grant& grant, srsran_dci_dl_nr_t& dci)
{
  alloc_result code = is_si_grant_valid(ss_id, grant);
  if (code != alloc_result::success) {
    return code;
  }
  return {&alloc_si_pdsch_unchecked(ss_id, grant, dci)};
}

pdsch_t& pdsch_allocator::alloc_si_pdsch_unchecked(uint32_t ss_id, const prb_grant& grant, srsran_dci_dl_nr_t& dci)
{
  // Verify SearchSpace validity
  const srsran_search_space_t* ss = bwp_cfg.get_ss(ss_id);
  srsran_sanity_check(ss != nullptr, "SearchSpace has not been configured");
  return alloc_pdsch_unchecked(ss->coreset_id, ss->type, srsran_dci_format_nr_1_0, grant, dci);
}

pdsch_alloc_result pdsch_allocator::alloc_rar_pdsch(const prb_grant& grant, srsran_dci_dl_nr_t& dci)
{
  alloc_result code = is_rar_grant_valid(grant);
  if (code != alloc_result::success) {
    return code;
  }
  return {&alloc_rar_pdsch_unchecked(grant, dci)};
}

pdsch_t& pdsch_allocator::alloc_rar_pdsch_unchecked(const prb_grant& grant, srsran_dci_dl_nr_t& dci)
{
  // TS 38.213, 8.2 - "In response to a PRACH transmission, a UE attempts to detect a DCI format 1_0"
  const static srsran_dci_format_nr_t dci_fmt = srsran_dci_format_nr_1_0;

  return alloc_pdsch_unchecked(
      bwp_cfg.cfg.pdcch.ra_search_space.coreset_id, bwp_cfg.cfg.pdcch.ra_search_space.type, dci_fmt, grant, dci);
}

pdsch_alloc_result pdsch_allocator::alloc_ue_pdsch(uint32_t                   ss_id,
                                                   srsran_dci_format_nr_t     dci_fmt,
                                                   const prb_grant&           grant,
                                                   const ue_carrier_params_t& ue,
                                                   srsran_dci_dl_nr_t&        dci)
{
  alloc_result code = is_ue_grant_valid(ue, ss_id, dci_fmt, grant);
  if (code != alloc_result::success) {
    return code;
  }
  return {&alloc_ue_pdsch_unchecked(ss_id, dci_fmt, grant, ue, dci)};
}

pdsch_t& pdsch_allocator::alloc_ue_pdsch_unchecked(uint32_t                   ss_id,
                                                   srsran_dci_format_nr_t     dci_fmt,
                                                   const prb_grant&           grant,
                                                   const ue_carrier_params_t& ue,
                                                   srsran_dci_dl_nr_t&        dci)
{
  const srsran_search_space_t* ss = ue.get_ss(ss_id);
  srsran_sanity_check(ss != nullptr, "SearchSpace has not been configured");
  return alloc_pdsch_unchecked(ss->coreset_id, ss->type, dci_fmt, grant, dci);
}

pdsch_t& pdsch_allocator::alloc_pdsch_unchecked(uint32_t                   coreset_id,
                                                srsran_search_space_type_t ss_type,
                                                srsran_dci_format_nr_t     dci_fmt,
                                                const prb_grant&           grant,
                                                srsran_dci_dl_nr_t&        out_dci)
{
  // Create new PDSCH entry in output PDSCH list
  pdschs.emplace_back();
  pdsch_t& pdsch = pdschs.back();

  // Register allocated PRBs in accumulated bitmap
  dl_prbs |= grant;

  // Fill DCI with PDSCH freq/time allocation information
  out_dci.time_domain_assigment = 0;
  if (grant.is_alloc_type0()) {
    out_dci.freq_domain_assigment = grant.rbgs().to_uint64();
  } else {
    uint32_t rb_start = grant.prbs().start(), nof_prb = bwp_cfg.nof_prb;
    if (SRSRAN_SEARCH_SPACE_IS_COMMON(ss_type)) {
      prb_interval lims = bwp_cfg.coreset_prb_range(coreset_id);
      if (dci_fmt == srsran_dci_format_nr_1_0) {
        srsran_sanity_check(rb_start >= lims.start(), "Invalid PRB grant");
        rb_start -= lims.start();
      }
      if (coreset_id == 0) {
        nof_prb = lims.length();
      }
    }
    srsran_sanity_check(rb_start + grant.prbs().length() <= nof_prb, "Invalid PRB grant");
    out_dci.freq_domain_assigment = srsran_ra_nr_type1_riv(nof_prb, rb_start, grant.prbs().length());
  }

  return pdsch;
}

void pdsch_allocator::cancel_last_pdsch()
{
  srsran_assert(not pdschs.empty(), "Trying to abort PDSCH allocation that does not exist");
  pdschs.pop_back();
  // TODO: clear bitmap allocated RBs
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename... Args>
void log_pusch_alloc_failure(srslog::log_channel& log_ch, const char* cause_fmt, Args&&... args)
{
  if (not log_ch.enabled()) {
    return;
  }

  // Log allocation failure
  fmt::memory_buffer fmtbuf;
  fmt::format_to(fmtbuf, "SCHED: Failure to allocate PUSCH. Cause: ");
  fmt::format_to(fmtbuf, cause_fmt, std::forward<Args>(args)...);
  log_ch("%s", srsran::to_c_str(fmtbuf));
}

pusch_allocator::pusch_allocator(const bwp_params_t& cfg_, uint32_t sl_index, pusch_list_t& pusch_lst) :
  bwp_cfg(cfg_),
  slot_idx(sl_index),
  puschs(pusch_lst),
  ul_prbs(bwp_cfg.cfg.rb_width, bwp_cfg.cfg.start_rb, bwp_cfg.cfg.pdsch.rbg_size_cfg_1)
{}

void pusch_allocator::reset()
{
  puschs.clear();
  ul_prbs.reset();
}

alloc_result pusch_allocator::has_grant_space(uint32_t nof_grants, bool verbose) const
{
  // UL must be active in given slot
  if (not bwp_cfg.slots[slot_idx].is_ul) {
    if (verbose) {
      log_pusch_alloc_failure(bwp_cfg.logger.error, "UL is disabled for slot={}", slot_idx);
    }
    return alloc_result::no_sch_space;
  }

  // No space in Scheduler PDSCH output list
  if (puschs.size() + nof_grants > puschs.capacity()) {
    if (verbose) {
      log_pusch_alloc_failure(bwp_cfg.logger.warning, "Maximum number of PUSCHs={} reached.", puschs.capacity());
    }
    return alloc_result::no_sch_space;
  }

  return alloc_result::success;
}

alloc_result
pusch_allocator::is_grant_valid(srsran_search_space_type_t ss_type, const prb_grant& grant, bool verbose) const
{
  alloc_result ret = has_grant_space(1, verbose);
  if (ret != alloc_result::success) {
    return ret;
  }

  if (SRSRAN_SEARCH_SPACE_IS_COMMON(ss_type)) {
    // In case of common SearchSpaces, the PRBs must be contiguous
    if (grant.is_alloc_type0()) {
      log_pusch_alloc_failure(bwp_cfg.logger.warning, "AllocType0 not allowed in common SearchSpace.");
      return alloc_result::invalid_grant_params;
    }
  }

  // Grant PRBs do not collide with previous PDSCH allocations
  if (ul_prbs.collides(grant)) {
    if (verbose) {
      log_pusch_alloc_failure(bwp_cfg.logger.debug, "SCHED: Provided UL PRB mask collides with previous allocations.");
    }
    return alloc_result::sch_collision;
  }

  return alloc_result::success;
}

pusch_alloc_result
pusch_allocator::alloc_pusch(const srsran_search_space_type_t ss_type, const prb_grant& grant, srsran_dci_ul_nr_t& dci)
{
  alloc_result code = is_grant_valid(ss_type, grant);
  if (code != alloc_result::success) {
    return code;
  }

  return {&alloc_pusch_unchecked(grant, dci)};
}

pusch_t& pusch_allocator::alloc_pusch_unchecked(const prb_grant& grant, srsran_dci_ul_nr_t& out_dci)
{
  // Create new PUSCH entry in output PUSCH list
  puschs.emplace_back();
  pusch_t& pusch = puschs.back();

  // Register allocated PRBs in accumulated bitmap
  ul_prbs |= grant;

  // Fill DCI with PUSCH freq/time allocation information
  out_dci.time_domain_assigment = 0;
  if (grant.is_alloc_type0()) {
    out_dci.freq_domain_assigment = grant.rbgs().to_uint64();
  } else {
    uint32_t nof_prb              = bwp_cfg.nof_prb;
    out_dci.freq_domain_assigment = srsran_ra_nr_type1_riv(nof_prb, grant.prbs().start(), grant.prbs().length());
  }

  return pusch;
}

void pusch_allocator::cancel_last_pusch()
{
  srsran_assert(not puschs.empty(), "Trying to abort PUSCH allocation that does not exist");
  puschs.pop_back();
}

} // namespace sched_nr_impl
} // namespace srsenb