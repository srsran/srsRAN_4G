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

#ifndef SRSRAN_MAC_SCH_PDU_NR_H
#define SRSRAN_MAC_SCH_PDU_NR_H

#include "srsran/common/byte_buffer.h"
#include "srsran/common/common.h"
#include "srsran/config.h"
#include "srsran/srslog/srslog.h"
#include <memory>
#include <stdint.h>
#include <vector>

namespace srsran {

class mac_sch_pdu_nr;

class mac_sch_subpdu_nr
{
public:
  // 3GPP 38.321 v15.3.0 Combined Tables 6.2.1-1, 6.2.1-2
  enum nr_lcid_sch_t {
    // Values for DL-SCH
    CCCH       = 0b000000,
    DRX_CMD    = 0b111100,
    TA_CMD     = 0b111101,
    CON_RES_ID = 0b111110,

    // Values for UL-SCH
    CRNTI           = 0b111010,
    SHORT_TRUNC_BSR = 0b111011,
    LONG_TRUNC_BSR  = 0b111100,
    CCCH_SIZE_48    = 0b110100,
    CCCH_SIZE_64    = 0b000000,
    SE_PHR          = 0b111001, // Single Entry PHR

    SHORT_BSR = 0b111101,
    LONG_BSR  = 0b111110,

    // Common
    PADDING = 0b111111,
  };

  // SDUs up to 256 B can use the short 8-bit L field
  static const int32_t MAC_SUBHEADER_LEN_THRESHOLD = 256;

  mac_sch_subpdu_nr(mac_sch_pdu_nr* parent_) : parent(parent_), logger(&srslog::fetch_basic_logger("MAC-NR")){};

  nr_lcid_sch_t get_type();
  bool          is_sdu() const;
  bool          is_valid_lcid();
  bool          is_var_len_ce(uint32_t lcid);
  bool          is_ul_ccch() const;

  int32_t        read_subheader(const uint8_t* ptr);
  uint32_t       get_total_length() const;
  uint32_t       get_sdu_length() const;
  uint32_t       get_lcid() const;
  uint8_t*       get_sdu();
  const uint8_t* get_sdu() const;
  uint16_t       get_c_rnti() const;

  // both return the reported values as per TS 38.321, mapping to dB according to TS 38.133 Sec 10.1.17 not done here
  uint8_t get_phr();
  uint8_t get_pcmax();

  // BSR
  struct lcg_bsr_t {
    uint8_t lcg_id;
    uint8_t buffer_size;
  };
  lcg_bsr_t            get_sbsr() const;
  static const uint8_t max_num_lcg_lbsr = 8;
  struct lbsr_t {
    uint8_t                bitmap; // the first octet of LBSR and Long Trunc BSR
    std::vector<lcg_bsr_t> list;   // one entry for each reported LCG
  };
  lbsr_t get_lbsr() const;

  // TA
  struct ta_t {
    uint8_t tag_id;
    uint8_t ta_command;
  };
  ta_t get_ta();

  // UE contention resolution identity CE
  static const uint8_t                           ue_con_res_id_len = 6;
  typedef std::array<uint8_t, ue_con_res_id_len> ue_con_res_id_t;
  ue_con_res_id_t                                get_ue_con_res_id_ce();
  uint64_t                                       get_ue_con_res_id_ce_packed();

  // setters
  void set_sdu(const uint32_t lcid_, const uint8_t* payload_, const uint32_t len_);
  void set_padding(const uint32_t len_);
  void set_c_rnti(const uint16_t crnti_);
  void set_se_phr(const uint8_t phr_, const uint8_t pcmax_);
  void set_sbsr(const lcg_bsr_t bsr_);
  void set_lbsr(const std::array<mac_sch_subpdu_nr::lcg_bsr_t, max_num_lcg_lbsr> bsr_);
  void set_ue_con_res_id_ce(const ue_con_res_id_t id);

  uint32_t write_subpdu(const uint8_t* start_);

  // Used by BSR procedure to determine size of BSR types
  static uint32_t sizeof_ce(uint32_t lcid, bool is_ul);

  void to_string(fmt::memory_buffer& buffer);

private:
  srslog::basic_logger* logger;

  // internal helpers
  bool has_length_field();

  uint32_t lcid          = 0;
  int      header_length = 0;
  int      sdu_length    = 0;
  bool     F_bit         = false;

  /// This helper class manages a SDU pointer that can point to either a user provided external buffer or to a small
  /// internal buffer, useful for storing very short SDUs.
  class sdu_buffer
  {
    static const uint8_t mac_ce_payload_len                 = 8 + 1; // Long BSR has max. 9 octets (see sizeof_ce() too)
    std::array<uint8_t, mac_ce_payload_len> ce_write_buffer = {};    // Buffer for CE payload
    uint8_t*                                sdu             = nullptr;

  public:
    sdu_buffer() = default;

    sdu_buffer(const sdu_buffer& other) : ce_write_buffer(other.ce_write_buffer)
    {
      // First check if we need to use internal storage.
      if (other.sdu == other.ce_write_buffer.data()) {
        sdu = ce_write_buffer.data();
        return;
      }
      sdu = other.sdu;
    }

    sdu_buffer& operator=(const sdu_buffer& other)
    {
      if (this == &other) {
        return *this;
      }
      ce_write_buffer = other.ce_write_buffer;
      if (other.sdu == other.ce_write_buffer.data()) {
        sdu = ce_write_buffer.data();
        return *this;
      }
      sdu = other.sdu;
      return *this;
    }

    explicit operator bool() const { return sdu; }

    /// Set the SDU pointer to use the internal buffer.
    uint8_t* use_internal_storage()
    {
      sdu = ce_write_buffer.data();
      return sdu;
    }

    /// Set the SDU pointer to point to the provided buffer.
    uint8_t* set_storage_to(uint8_t* p)
    {
      sdu = p;
      return sdu;
    }

    /// Returns the SDU pointer.
    const uint8_t* ptr() const { return sdu; }
    uint8_t*       ptr() { return sdu; }
  };

  sdu_buffer sdu;

  mac_sch_pdu_nr* parent = nullptr;
};

class mac_sch_pdu_nr
{
public:
  mac_sch_pdu_nr(bool ulsch_ = false) : ulsch(ulsch_), logger(srslog::fetch_basic_logger("MAC-NR")) {}

  void                     pack();
  int                      unpack(const uint8_t* payload, const uint32_t& len);
  uint32_t                 get_num_subpdus() const { return subpdus.size(); }
  const mac_sch_subpdu_nr& get_subpdu(const uint32_t& index) const;
  mac_sch_subpdu_nr&       get_subpdu(uint32_t index);
  bool                     is_ulsch();

  int  init_tx(byte_buffer_t* buffer_, uint32_t pdu_len_, bool is_ulsch_ = false);
  void init_rx(bool ulsch_ = false);

  // Add SDU or CEs to PDU
  // All functions will return SRSRAN_SUCCESS on success, and SRSRAN_ERROR otherwise
  uint32_t add_sdu(const uint32_t lcid_, const uint8_t* payload_, const uint32_t len_);
  uint32_t add_crnti_ce(const uint16_t crnti_);
  uint32_t add_se_phr_ce(const uint8_t phr_, const uint8_t pcmax_);
  uint32_t add_sbsr_ce(const mac_sch_subpdu_nr::lcg_bsr_t bsr_);
  uint32_t add_lbsr_ce(const std::array<mac_sch_subpdu_nr::lcg_bsr_t, mac_sch_subpdu_nr::max_num_lcg_lbsr> bsr_);
  uint32_t add_ue_con_res_id_ce(const mac_sch_subpdu_nr::ue_con_res_id_t id);

  uint32_t get_remaing_len();

  void to_string(fmt::memory_buffer& buffer);

  uint32_t size_header_sdu(const uint32_t lcid_, const uint32_t nbytes);

private:
  /// Private helper that adds a subPDU to the MAC PDU
  uint32_t add_sudpdu(mac_sch_subpdu_nr& subpdu);

  bool                           ulsch = false;
  std::vector<mac_sch_subpdu_nr> subpdus;

  byte_buffer_t*        buffer        = nullptr;
  uint32_t              pdu_len       = 0;
  uint32_t              remaining_len = 0;
  srslog::basic_logger& logger;
};

} // namespace srsran

#endif // SRSRAN_MAC_SCH_PDU_NR_H
