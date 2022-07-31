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

#ifndef SRSRAN_RRC_PAGING_H
#define SRSRAN_RRC_PAGING_H

#include "srsran/adt/pool/cached_alloc.h"
#include "srsran/adt/span.h"
#include "srsran/asn1/rrc/paging.h"
#include "srsran/common/tti_point.h"

namespace srsenb {

/**
 * Class that handles the buffering of paging records and encoding of PCCH messages.
 * It's thread-safe, and, assuming that threads contend for pending PCCH messages using different subframe indexes,
 * should rarely blocking on mutexes
 */
class paging_manager
{
public:
  paging_manager(uint32_t default_paging_cycle_, float nb_) :
    T(default_paging_cycle_),
    Nb(static_cast<uint32_t>((float)T * nb_)),
    N(std::min(T, Nb)),
    Ns(std::max(1U, static_cast<uint32_t>(nb_))),
    logger(srslog::fetch_basic_logger("RRC"))
  {
    for (subframe_info& sf_obj : sf_pending_pcch) {
      sf_obj.pending_paging.resize(T);
      for (pcch_info& pcch : sf_obj.pending_paging) {
        pcch.pcch_msg.msg.set_c1().paging().paging_record_list_present = true;
      }
    }
  }

  /// add new IMSI paging record
  bool add_imsi_paging(uint32_t ueid, srsran::const_byte_span imsi);

  /// add new TMSI paging record
  bool add_tmsi_paging(uint32_t ueid, uint8_t mmec, srsran::const_byte_span m_tmsi);

  /// Get how many bytes are required to fit the pending PCCH message.
  size_t pending_pcch_bytes(tti_point tti_tx_dl);

  /**
   * Invoke "callable" for PCCH indexed by tti_tx_dl in a mutexed context.
   * Callable signature is bool(const_byte_span pdu, const pcch_msg& msg, bool is_first_tx)
   * - "pdu"         encoded ASN1 PCCH message
   * - "msg"         PCCH message in ASN1 form
   * - "is_first_tx" tells if the PDU hasn't been transmitted yet. This may be useful to log the PCCH only for one
   *                 of the carriers
   * - the return should be true if the PDU is being transmitted, and false otherwise
   */
  template <typename Callable>
  bool read_pdu_pcch(tti_point tti_tx_dl, const Callable& callable);

private:
  struct pcch_info {
    tti_point                    tti_tx_dl;
    asn1::rrc::pcch_msg_s        pcch_msg;
    srsran::unique_byte_buffer_t pdu;

    bool is_tx() const { return tti_tx_dl.is_valid(); }
    bool empty() const { return pdu == nullptr; }
    void clear()
    {
      tti_tx_dl = tti_point();
      pcch_msg.msg.c1().paging().paging_record_list.clear();
      pdu.reset();
    }
  };
  const static size_t nof_paging_subframes = 4;

  bool add_paging_record(uint32_t ueid, const asn1::rrc::paging_record_s& paging_record);

  static int get_sf_idx_key(uint32_t sf_idx)
  {
    switch (sf_idx) {
      case 0:
        return 0;
      case 4:
        return 1;
      case 5:
        return 2;
      case 9:
        return 3;
      default:
        break;
    }
    return -1;
  }

  // args
  uint32_t              T;
  uint32_t              Nb;
  uint32_t              N;
  uint32_t              Ns;
  srslog::basic_logger& logger;

  struct subframe_info {
    mutable std::mutex        mutex;
    srsran::deque<pcch_info*> transmitted_pcch;
    std::vector<pcch_info>    pending_paging;
  };

  std::array<subframe_info, nof_paging_subframes> sf_pending_pcch;
};

bool paging_manager::add_imsi_paging(uint32_t ueid, srsran::const_byte_span imsi)
{
  asn1::rrc::paging_record_s paging_elem;
  paging_elem.ue_id.set_imsi().resize(imsi.size());
  std::copy(imsi.begin(), imsi.end(), paging_elem.ue_id.imsi().begin());
  paging_elem.cn_domain = asn1::rrc::paging_record_s::cn_domain_e_::ps;
  return add_paging_record(ueid, paging_elem);
}

bool paging_manager::add_tmsi_paging(uint32_t ueid, uint8_t mmec, srsran::const_byte_span m_tmsi)
{
  asn1::rrc::paging_record_s paging_elem;
  paging_elem.ue_id.set_s_tmsi().mmec.from_number(mmec);
  uint32_t m_tmsi_val = 0;
  for (uint32_t i = 0; i < m_tmsi.size(); i++) {
    m_tmsi_val |= m_tmsi[i] << (8u * (m_tmsi.size() - i - 1u));
  }
  paging_elem.ue_id.s_tmsi().m_tmsi.from_number(m_tmsi_val);
  paging_elem.cn_domain = asn1::rrc::paging_record_s::cn_domain_e_::ps;
  return add_paging_record(ueid, paging_elem);
}

/// \remark See TS 36.304, Section 7
bool paging_manager::add_paging_record(uint32_t ueid, const asn1::rrc::paging_record_s& paging_record)
{
  constexpr static const int sf_pattern[4][4] = {{9, 4, -1, 0}, {-1, 9, -1, 4}, {-1, -1, -1, 5}, {-1, -1, -1, 9}};

  ueid         = ((uint32_t)ueid) % 1024;
  uint32_t i_s = (ueid / N) % Ns;

  int sf_idx = sf_pattern[i_s % 4][(Ns - 1) % 4];
  if (sf_idx < 0) {
    logger.error("SF pattern is N/A for Ns=%d, i_s=%d, imsi_decimal=%d", Ns, i_s, ueid);
    return false;
  }
  size_t sf_key = static_cast<size_t>(get_sf_idx_key(sf_idx));

  subframe_info&              locked_sf = sf_pending_pcch[sf_key];
  std::lock_guard<std::mutex> lock(locked_sf.mutex);

  size_t     sfn_cycle_idx = ((size_t)T / (size_t)N) * (size_t)(ueid % N);
  pcch_info& pending_pcch  = locked_sf.pending_paging[sfn_cycle_idx];
  auto&      record_list   = pending_pcch.pcch_msg.msg.c1().paging().paging_record_list;

  if (record_list.size() >= ASN1_RRC_MAX_PAGE_REC) {
    logger.warning("Failed to add new paging record for ueid=%d. Cause: no paging record space left.", ueid);
    return false;
  }
  if (pending_pcch.is_tx()) {
    logger.error("Adding Paging records to ueid=%d PCCH that has been already sent but not cleared.", ueid);
    pending_pcch.clear();
  }

  if (pending_pcch.pdu == nullptr) {
    pending_pcch.pdu = srsran::make_byte_buffer();
    if (pending_pcch.pdu == nullptr) {
      logger.warning("Failed to add new paging record for ueid=%d. Cause: No buffers available", ueid);
      return false;
    }
  }

  record_list.push_back(paging_record);

  asn1::bit_ref bref(pending_pcch.pdu->msg, pending_pcch.pdu->get_tailroom());
  if (pending_pcch.pcch_msg.pack(bref) == asn1::SRSASN_ERROR_ENCODE_FAIL) {
    logger.error("Failed to pack PCCH message");
    pending_pcch.clear();
    return false;
  }
  pending_pcch.pdu->N_bytes = (uint32_t)bref.distance_bytes();

  return true;
}

size_t paging_manager::pending_pcch_bytes(tti_point tti_tx_dl)
{
  int sf_key = get_sf_idx_key(tti_tx_dl.sf_idx());
  if (sf_key < 0) {
    // tti_tx_dl is not in a paging subframe
    return 0;
  }

  subframe_info&               locked_sf = sf_pending_pcch[static_cast<size_t>(sf_key)];
  std::unique_lock<std::mutex> lock(locked_sf.mutex, std::try_to_lock);
  if (not lock.owns_lock()) {
    // If the scheduler fails to lock, it will postpone the PCCH transmission to the next paging cycle
    return 0;
  }

  // clear old PCCH that has been transmitted at this point
  while (not locked_sf.transmitted_pcch.empty() and locked_sf.transmitted_pcch.front()->tti_tx_dl < tti_tx_dl) {
    locked_sf.transmitted_pcch.front()->clear();
    locked_sf.transmitted_pcch.pop_front();
  }

  const pcch_info& pending_pcch = locked_sf.pending_paging[tti_tx_dl.sfn() % T];
  if (pending_pcch.empty()) {
    return 0;
  }
  return pending_pcch.pdu->size();
}

template <typename Callable>
bool paging_manager::read_pdu_pcch(tti_point tti_tx_dl, const Callable& func)
{
  int sf_key = get_sf_idx_key(tti_tx_dl.sf_idx());
  if (sf_key < 0) {
    logger.warning("%s was called for tti=%d, which is not a paging subframe index", __FUNCTION__, tti_tx_dl.to_uint());
    return false;
  }

  subframe_info&              locked_sf = sf_pending_pcch[static_cast<size_t>(sf_key)];
  std::lock_guard<std::mutex> lock(locked_sf.mutex);

  pcch_info& pending_pcch = locked_sf.pending_paging[tti_tx_dl.sfn() % T];

  if (pending_pcch.empty()) {
    logger.warning("read_pdu_pdcch(...) called for tti=%d, but there is no pending pcch message", tti_tx_dl.to_uint());
    return false;
  }

  // Call callable for existing PCCH pdu
  if (func(*pending_pcch.pdu, pending_pcch.pcch_msg, not pending_pcch.is_tx())) {
    if (not pending_pcch.is_tx()) {
      // first tx. Enqueue in list of transmitted pcch. We do not erase the PCCH yet because it may be transmitted
      // by other carriers
      pending_pcch.tti_tx_dl = tti_tx_dl;
      locked_sf.transmitted_pcch.push_back(&pending_pcch);
    }
    return true;
  }
  return false;
}

} // namespace srsenb

#endif // SRSRAN_RRC_PAGING_H
