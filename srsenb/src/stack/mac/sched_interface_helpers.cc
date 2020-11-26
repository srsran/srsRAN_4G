/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2020 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsenb/hdr/stack/mac/sched_interface_helpers.h"
#include "srslte/mac/pdu.h"
#include "srslte/srslog/bundled/fmt/format.h"
#include <array>

namespace srsenb {

using dl_sched_res_t    = sched_interface::dl_sched_res_t;
using dl_sched_data_t   = sched_interface::dl_sched_data_t;
using custom_mem_buffer = fmt::basic_memory_buffer<char, 1024>;

const char* to_string_short(srslte_dci_format_t dcifmt)
{
  switch (dcifmt) {
    case SRSLTE_DCI_FORMAT0:
      return "0";
    case SRSLTE_DCI_FORMAT1:
      return "1";
    case SRSLTE_DCI_FORMAT1A:
      return "1A";
    case SRSLTE_DCI_FORMAT1B:
      return "1B";
    case SRSLTE_DCI_FORMAT2:
      return "2";
    case SRSLTE_DCI_FORMAT2A:
      return "2A";
    case SRSLTE_DCI_FORMAT2B:
      return "2B";
    default:
      return "unknown";
  }
}

void fill_dl_cc_result_info(custom_mem_buffer& strbuf, const dl_sched_data_t& data)
{
  uint32_t first_ce = sched_interface::MAX_RLC_PDU_LIST;
  for (uint32_t i = 0; i < data.nof_pdu_elems[0]; ++i) {
    if (srslte::is_mac_ce(static_cast<srslte::dl_sch_lcid>(data.pdu[i]->lcid))) {
      first_ce = i;
      break;
    }
  }
  if (first_ce == sched_interface::MAX_RLC_PDU_LIST) {
    return;
  }
  const char* prefix = strbuf.size() > 0 ? " | " : "";
  fmt::format_to(strbuf, "{}rnti={:0x}: [", prefix, data.dci.rnti);
  bool ces_found = false;
  for (uint32_t i = 0; i < data.nof_pdu_elems[0]; ++i) {
    const auto& pdu          = data.pdu[0][i];
    prefix                   = (ces_found) ? " | " : "";
    srslte::dl_sch_lcid lcid = static_cast<srslte::dl_sch_lcid>(pdu.lcid);
    if (srslte::is_mac_ce(lcid)) {
      fmt::format_to(strbuf, "{}MAC CE \"{}\"", prefix, srslte::to_string_short(lcid));
      ces_found = true;
    }
  }
  fmt::format_to(strbuf, "]");
}

void fill_dl_cc_result_debug(custom_mem_buffer& strbuf, const dl_sched_data_t& data)
{
  if (data.nof_pdu_elems[0] == 0 and data.nof_pdu_elems[1] == 0) {
    return;
  }
  fmt::format_to(strbuf,
                 "  > rnti={:0x}, tbs={}, f={}, mcs={}: [",
                 data.dci.rnti,
                 data.tbs[0],
                 to_string_short(data.dci.format),
                 data.dci.tb[0].mcs_idx);
  for (uint32_t tb = 0; tb < SRSLTE_MAX_TB; ++tb) {
    for (uint32_t i = 0; i < data.nof_pdu_elems[tb]; ++i) {
      const auto&         pdu    = data.pdu[tb][i];
      const char*         prefix = (i == 0) ? "" : " | ";
      srslte::dl_sch_lcid lcid   = static_cast<srslte::dl_sch_lcid>(pdu.lcid);
      if (srslte::is_mac_ce(lcid)) {
        fmt::format_to(strbuf, "{}MAC CE \"{}\"", prefix, srslte::to_string_short(lcid));
      } else {
        fmt::format_to(strbuf, "{}MAC SDU lcid={}, tb={}, len={} B", prefix, pdu.lcid, tb, pdu.nbytes);
      }
    }
  }
  fmt::format_to(strbuf, "]\n");
}

void log_dl_cc_results(srslte::log_ref log_h, uint32_t enb_cc_idx, const sched_interface::dl_sched_res_t& result)
{
  if (log_h->get_level() < srslte::LOG_LEVEL_INFO) {
    return;
  }
  custom_mem_buffer strbuf;
  for (uint32_t i = 0; i < result.nof_data_elems; ++i) {
    const dl_sched_data_t& data = result.data[i];
    if (log_h->get_level() == srslte::LOG_LEVEL_INFO) {
      fill_dl_cc_result_info(strbuf, data);
    } else if (log_h->get_level() == srslte::LOG_LEVEL_DEBUG) {
      fill_dl_cc_result_debug(strbuf, data);
    }
  }
  if (strbuf.size() != 0) {
    if (log_h->get_level() == srslte::LOG_LEVEL_DEBUG) {
      log_h->debug("SCHED: MAC LCID allocs cc=%d:\n%s", enb_cc_idx, fmt::to_string(strbuf).c_str());
    } else {
      log_h->info("SCHED: MAC CE allocs cc=%d: %s", enb_cc_idx, fmt::to_string(strbuf).c_str());
    }
  }
}

} // namespace srsenb