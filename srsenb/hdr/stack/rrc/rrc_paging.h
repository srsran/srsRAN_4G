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

#ifndef SRSRAN_RRC_PAGING_H
#define SRSRAN_RRC_PAGING_H

#include "srsran/adt/span.h"
#include "srsran/asn1/rrc/paging.h"
#include "srsran/common/tti_point.h"

namespace srsenb {

/**
 * Class that handles the buffering of paging records and encoding of PCCH messages. It's thread-safe
 */
class paging_manager
{
public:
  paging_manager(uint32_t default_paging_cycle_, uint32_t nb_) :
    T(default_paging_cycle_),
    Nb(T * nb_),
    N(std::min(T, Nb)),
    Ns(std::max(nb_, 1u)),
    pending_paging(T),
    logger(srslog::fetch_basic_logger("RRC"))
  {
    for (auto& sfn_pcch_msgs : pending_paging) {
      for (pcch_info& pcch : sfn_pcch_msgs) {
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
   * - "is_first_tx" tells if the PDU hasn't been transmitted yet.
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

  bool       add_paging_record(uint32_t ueid, const asn1::rrc::paging_record_s& paging_record);
  pcch_info* get_pcch_info(tti_point tti_tx_dl)
  {
    int sf_key = get_sf_idx_key(tti_tx_dl.sf_idx());
    if (sf_key < 0) {
      return nullptr;
    }
    return &pending_paging[tti_tx_dl.sfn() % T][sf_key];
  }
  const pcch_info* get_pcch_info(tti_point tti_tx_dl) const
  {
    int sf_key = get_sf_idx_key(tti_tx_dl.sf_idx());
    if (sf_key < 0) {
      return nullptr;
    }
    return &pending_paging[tti_tx_dl.sfn() % T][sf_key];
  }
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

  mutable std::array<std::mutex, nof_paging_subframes>      sf_idx_mutex;
  std::vector<std::array<pcch_info, nof_paging_subframes> > pending_paging;
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

  size_t     sfn_cycle_idx = (T / N) * (ueid % N);
  pcch_info& pending_pcch  = pending_paging[sfn_cycle_idx][sf_key];
  auto&      record_list   = pending_pcch.pcch_msg.msg.c1().paging().paging_record_list;

  std::lock_guard<std::mutex> lock(sf_idx_mutex[sf_key]);

  if (record_list.size() >= ASN1_RRC_MAX_PAGE_REC) {
    logger.warning("Failed to add new paging record for ueid=%d. Cause: no paging record space left.", ueid);
    return false;
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

  std::lock_guard<std::mutex> lock(sf_idx_mutex[static_cast<size_t>(sf_key)]);

  // clear old PCCH that has been transmitted at this point
  pcch_info* old_pcch = get_pcch_info(tti_tx_dl - SRSRAN_NOF_SF_X_FRAME);
  if (old_pcch != nullptr and not old_pcch->empty()) {
    old_pcch->clear();
  }

  const pcch_info* pending_pcch = get_pcch_info(tti_tx_dl);
  if (pending_pcch->empty()) {
    return 0;
  }
  return pending_pcch->pdu->size();
}

template <typename Callable>
bool paging_manager::read_pdu_pcch(tti_point tti_tx_dl, const Callable& func)
{
  int sf_key = get_sf_idx_key(tti_tx_dl.sf_idx());
  if (sf_key < 0) {
    logger.warning("%s was called for tti=%d, which is not a paging subframe index", __FUNCTION__, tti_tx_dl.to_uint());
    return false;
  }

  std::lock_guard<std::mutex> lock(sf_idx_mutex[static_cast<size_t>(sf_key)]);

  pcch_info* pending_pcch = get_pcch_info(tti_tx_dl);

  if (pending_pcch->empty()) {
    logger.warning("read_pdu_pdcch(...) called for tti=%d, but there is no pending pcch message", tti_tx_dl.to_uint());
    return false;
  }

  // Call callable for existing PCCH pdu
  if (func(*pending_pcch->pdu, pending_pcch->pcch_msg, not pending_pcch->is_tx())) {
    pending_pcch->tti_tx_dl = tti_tx_dl;
    return true;
  }
  return false;
}

} // namespace srsenb

#endif // SRSRAN_RRC_PAGING_H
