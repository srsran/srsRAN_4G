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

#ifndef SRSRAN_ENB_RRC_INTERFACE_MAC_H
#define SRSRAN_ENB_RRC_INTERFACE_MAC_H

#include "srsenb/hdr/stack/mac/sched_interface.h"

namespace srsenb {

/// RRC interface for MAC
class rrc_interface_mac
{
public:
  /* Radio Link failure */
  virtual int  add_user(uint16_t rnti, const sched_interface::ue_cfg_t& init_ue_cfg)      = 0;
  virtual void upd_user(uint16_t new_rnti, uint16_t old_rnti)                             = 0;
  virtual void set_activity_user(uint16_t rnti)                                           = 0;
  virtual void set_radiolink_dl_state(uint16_t rnti, bool crc_res)                        = 0;
  virtual void set_radiolink_ul_state(uint16_t rnti, bool crc_res)                        = 0;
  virtual bool is_paging_opportunity(uint32_t tti_tx_dl, uint32_t* payload_len)           = 0;
  virtual void read_pdu_pcch(uint32_t tti_tx_dl, uint8_t* payload, uint32_t payload_size) = 0;

  ///< Provide packed SIB to MAC (buffer is managed by RRC)
  virtual uint8_t* read_pdu_bcch_dlsch(const uint8_t enb_cc_idx, const uint32_t sib_index) = 0;

  /// \brief Provides the packed SIBs in a System Information (SI) message consisting of different segments to the MAC.
  ///
  /// This method must be used when the desired SIB in a SI message is segmented, i.e., when the contents of the SIB are
  /// split across different versions of the SIB. In this case, the SIB segments are scheduled in sequence, as per
  /// TS36.331 Sections 5.2.1.4 and 5.2.1.5.
  ///
  /// \param[in] enb_cc_idx        Index of the ENB CC.
  /// \param[in] sib_index         Index of the System Information (SI) message containing the SIB. It corresponds with
  ///                              the ordering of the SI messages in the SIB1 scheduling information list.
  /// \param[in] sib_segment_index Index of the SIB segment to retrieve.
  ///
  /// \return A pointer to the desired SIB message segment.
  /// \remark An assertion is thrown if the SIB message is not segmented.
  virtual uint8_t*
  read_pdu_bcch_dlsch(const uint8_t enb_cc_idx, const uint32_t sib_index, const uint32_t sib_segment_index) = 0;
};

} // namespace srsenb

#endif // SRSRAN_ENB_RRC_INTERFACE_MAC_H
