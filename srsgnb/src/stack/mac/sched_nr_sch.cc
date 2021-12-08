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

#include "srsgnb/hdr/stack/mac/sched_nr_sch.h"
#include "srsran/common/string_helpers.h"

namespace srsenb {
namespace sched_nr_impl {

template <typename... Args>
void log_alloc_failure(srslog::log_channel& log_ch, uint32_t ss_id, const char* cause_fmt, Args&&... args)
{
  if (not log_ch.enabled()) {
    return;
  }

  // Log allocation failure
  fmt::memory_buffer fmtbuf;
  fmt::format_to(fmtbuf, "SCHED: Failure to allocate PDSCH in SS#{}. Cause: ", ss_id);
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

alloc_result pdsch_allocator::is_grant_valid(uint32_t               ss_id,
                                             srsran_dci_format_nr_t dci_fmt,
                                             const prb_grant&       grant,
                                             ue_carrier_params_t*   ue) const
{
  // DL must be active in given slot
  if (not bwp_cfg.slots[slot_idx].is_dl) {
    log_alloc_failure(bwp_cfg.logger.error, ss_id, "DL is disabled for slot={}", slot_idx);
    return alloc_result::no_sch_space;
  }

  // No space in Scheduler PDSCH output list
  if (pdschs.full()) {
    log_alloc_failure(bwp_cfg.logger.warning, ss_id, "Maximum number of PDSCHs={} reached.", pdschs.size());
    return alloc_result::no_sch_space;
  }

  // Verify SearchSpace validity
  const srsran_search_space_t* ss = (ue == nullptr) ? bwp_cfg.get_ss(ss_id) : ue->get_ss(ss_id);
  if (ss == nullptr) {
    // Couldn't find SearchSpace
    log_alloc_failure(bwp_cfg.logger.error, ss_id, "SearchSpace has not been configured.");
    return alloc_result::invalid_grant_params;
  }

  if (SRSRAN_SEARCH_SPACE_IS_COMMON(ss->type)) {
    // In case of common SearchSpaces, the PRBs must be contiguous
    if (grant.is_alloc_type0()) {
      log_alloc_failure(bwp_cfg.logger.warning, ss_id, "AllocType0 not allowed in common SearchSpace.");
      return alloc_result::invalid_grant_params;
    }

    // Grant PRBs do not collide with CORESET PRB limits (in case of common SearchSpace)
    if (bwp_cfg.coreset_prb_limits(ss_id, dci_fmt).collides(grant)) {
      bwp_cfg.logger.debug("SCHED: Provided RBG mask falls outside common CORESET PRB boundaries.");
      return alloc_result::sch_collision;
    }
  }

  // Grant PRBs do not collide with previous PDSCH allocations
  if (dl_prbs.collides(grant)) {
    bwp_cfg.logger.debug("SCHED: Provided RBG mask collides with allocation previously made.");
    return alloc_result::sch_collision;
  }

  return alloc_result::success;
}

srsran::expected<pdsch_t*, alloc_result> pdsch_allocator::alloc_pdsch(const srsran_dci_ctx_t& dci_ctx,
                                                                      uint32_t                ss_id,
                                                                      const prb_grant&        grant,
                                                                      srsran_dci_dl_nr_t&     dci)
{
  alloc_result code = is_grant_valid(ss_id, dci_ctx.format, grant);
  if (code != alloc_result::success) {
    return code;
  }

  return {&alloc_pdsch_unchecked(dci_ctx, grant, dci)};
}

pdsch_t& pdsch_allocator::alloc_pdsch_unchecked(const srsran_dci_ctx_t& dci_ctx,
                                                const prb_grant&        grant,
                                                srsran_dci_dl_nr_t&     out_dci)
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
    uint32_t rb_start = grant.prbs().start(), nof_prb = bwp_cfg.nof_prb();
    if (SRSRAN_SEARCH_SPACE_IS_COMMON(dci_ctx.ss_type)) {
      if (dci_ctx.format == srsran_dci_format_nr_1_0) {
        rb_start -= dci_ctx.coreset_start_rb;
      }
      if (dci_ctx.coreset_id == 0) {
        nof_prb = bwp_cfg.coreset_bw(0);
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
pusch_allocator::alloc_pusch(const srsran_dci_ctx_t& dci_ctx, const prb_grant& grant, srsran_dci_ul_nr_t& dci)
{
  alloc_result code = is_grant_valid(dci_ctx.ss_type, grant);
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
    uint32_t nof_prb              = bwp_cfg.nof_prb();
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