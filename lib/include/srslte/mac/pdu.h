/*
 * Copyright 2013-2020 Software Radio Systems Limited
 *
 * This file is part of srsLTE.
 *
 * srsLTE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsLTE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#ifndef SRSLTE_PDU_H
#define SRSLTE_PDU_H

#include "srslte/common/interfaces_common.h"
#include "srslte/common/logmap.h"
#include <stdint.h>
#include <stdio.h>
#include <vector>

/* MAC PDU Packing/Unpacking functions. Section 6 of 36.321 */
class subh;

namespace srslte {

/* 3GPP 36.321 Table 6.2.1-1 */
enum class dl_sch_lcid {
  CCCH     = 0b00000,
  RESERVED = 0b10001,
  //...
  SCELL_ACTIVATION_4_OCTET = 0b11000,
  SCELL_ACTIVATION         = 0b11011,
  CON_RES_ID               = 0b11100,
  TA_CMD                   = 0b11101,
  DRX_CMD                  = 0b11110,
  PADDING                  = 0b11111
};
const char*    to_string(dl_sch_lcid v);
uint32_t       ce_size(dl_sch_lcid v);
uint32_t       ce_subheader_size(dl_sch_lcid v);
uint32_t       ce_total_size(dl_sch_lcid v);
constexpr bool is_mac_ce(dl_sch_lcid v)
{
  return v > dl_sch_lcid::RESERVED;
}
constexpr bool is_sdu(dl_sch_lcid v)
{
  return v <= dl_sch_lcid::RESERVED;
}

/* 3GPP 36.321 Table 6.2.1-2 */
enum class ul_sch_lcid {
  CCCH     = 0b00000,
  RESERVED = 0b10001,
  //...
  PHR_REPORT_EXT = 0b11001,
  PHR_REPORT     = 0b11010,
  CRNTI          = 0b11011,
  TRUNC_BSR      = 0b11100,
  SHORT_BSR      = 0b11101,
  LONG_BSR       = 0b11110,
  PADDING        = 0b11111
};
const char*    to_string(ul_sch_lcid v);
uint32_t       ce_size(ul_sch_lcid v);
constexpr bool is_mac_ce(ul_sch_lcid v)
{
  return v > ul_sch_lcid::RESERVED;
}
constexpr bool is_sdu(ul_sch_lcid v)
{
  return v <= ul_sch_lcid::RESERVED;
}

/* 3GPP 36.321 Table 6.2.1-4 */
enum class mch_lcid {
  MCCH = 0b00000,
  //...
  MTCH_MAX_LCID  = 0b11100,
  MCH_SCHED_INFO = 0b11110,
  PADDING        = 0b11111
};
const char*    to_string(mch_lcid v);
constexpr bool is_mac_ce(mch_lcid v)
{
  return v >= mch_lcid::MCH_SCHED_INFO;
}
constexpr bool is_sdu(mch_lcid v)
{
  return v < mch_lcid::MCH_SCHED_INFO;
}

/* Common LCID type */
struct lcid_t {
  enum class ch_type { dl_sch, ul_sch, mch } type;
  union {
    uint32_t    lcid;
    dl_sch_lcid dl_sch;
    ul_sch_lcid ul_sch;
    mch_lcid    mch;
  };
  lcid_t(ch_type t, uint32_t lcid_) : type(t), lcid(lcid_) {}
  lcid_t(dl_sch_lcid lcid_) : type(ch_type::dl_sch), dl_sch(lcid_) {}
  lcid_t(ul_sch_lcid lcid_) : type(ch_type::ul_sch), ul_sch(lcid_) {}
  lcid_t(mch_lcid lcid_) : type(ch_type::mch), mch(lcid_) {}
  const char* to_string() const;
  bool        is_sch() const { return type == ch_type::dl_sch or type == ch_type::ul_sch; }
  bool        is_sdu() const;
  bool        operator==(lcid_t t) const { return type == t.type and lcid == t.lcid; }
};

template <class SubH>
class pdu
{
public:
  pdu(uint32_t max_subheaders_, log_ref log_h_) :
    max_subheaders(max_subheaders_),
    subheaders(max_subheaders_),
    log_h(log_h_),
    nof_subheaders(0),
    cur_idx(-1),
    pdu_len(0),
    rem_len(0),
    last_sdu_idx(-1),
    pdu_is_ul(false),
    buffer_tx(nullptr)
  {}
  virtual ~pdu() = default;

  void fprint(FILE* stream)
  {
    fprintf(stream, "Number of Subheaders: %d\n", nof_subheaders);
    for (int i = 0; i < nof_subheaders; i++) {
      fprintf(stream, " -- Subheader %d: ", i);
      subheaders[i].fprint(stream);
    }
  }

  /* Resets the Read/Write position and remaining PDU length */
  void reset()
  {
    cur_idx      = -1;
    last_sdu_idx = -1;
    rem_len      = pdu_len;
  }

  void init_rx(uint32_t pdu_len_bytes, bool is_ulsch = false) { init_(NULL, pdu_len_bytes, is_ulsch); }

  void init_tx(byte_buffer_t* buffer, uint32_t pdu_len_bytes, bool is_ulsch = false)
  {
    init_(buffer, pdu_len_bytes, is_ulsch);
  }

  uint32_t nof_subh() { return nof_subheaders; }

  bool new_subh()
  {
    if (nof_subheaders < (int)max_subheaders - 1 && rem_len > 0 && buffer_tx->get_headroom() > 1) {
      nof_subheaders++;
      return next();
    } else {
      return false;
    }
  }

  bool next()
  {
    if (cur_idx < nof_subheaders - 1) {
      cur_idx++;
      return true;
    } else {
      return false;
    }
  }

  void del_subh()
  {
    if (nof_subheaders > 0) {
      nof_subheaders--;
    }
    if (cur_idx >= 0) {
      cur_idx--;
    }
  }

  SubH* get()
  {
    if (cur_idx >= 0) {
      return &subheaders[cur_idx];
    } else {
      return nullptr;
    }
  }

  bool is_ul() { return pdu_is_ul; }

  uint8_t* get_current_sdu_ptr() { return &buffer_tx->msg[buffer_tx->N_bytes]; }

  void add_sdu(uint32_t sdu_sz) { buffer_tx->N_bytes += sdu_sz; }

  // Section 6.1.2
  void parse_packet(uint8_t* ptr)
  {
    uint8_t* init_ptr = ptr;
    nof_subheaders    = 0;
    bool ret          = false;
    do {
      if (nof_subheaders < (int)max_subheaders) {
        ret = subheaders[nof_subheaders].read_subheader(&ptr);
        nof_subheaders++;
      }

      if (ret && ((ptr - init_ptr) >= (int32_t)pdu_len)) {
        // stop processing last subheader indicates another one but all bytes are consume
        nof_subheaders = 0;
        INFO("Corrupted MAC PDU - all bytes have been consumed (pdu_len=%d)\n", pdu_len);
        if (log_h) {
          log_h->info_hex(
              init_ptr, pdu_len, "Corrupted MAC PDU - all bytes have been consumed (pdu_len=%d)\n", pdu_len);
        }
      }
    } while (ret && (nof_subheaders + 1) < (int)max_subheaders && ((int32_t)pdu_len > (ptr - init_ptr)));

    for (int i = 0; i < nof_subheaders; i++) {
      subheaders[i].read_payload(&ptr);
    }
  }

protected:
  std::vector<SubH> subheaders;
  uint32_t          pdu_len;
  uint32_t          rem_len;
  int               cur_idx;
  int               nof_subheaders;
  uint32_t          max_subheaders;
  bool              pdu_is_ul;
  byte_buffer_t*    buffer_tx = nullptr;
  int               last_sdu_idx;
  srslte::log_ref   log_h;

  /* Prepares the PDU for parsing or writing by setting the number of subheaders to 0 and the pdu length */
  virtual void init_(byte_buffer_t* buffer_tx_, uint32_t pdu_len_bytes, bool is_ulsch)
  {
    nof_subheaders = 0;
    pdu_len        = pdu_len_bytes;
    rem_len        = pdu_len;
    pdu_is_ul      = is_ulsch;
    buffer_tx      = buffer_tx_;
    last_sdu_idx   = -1;
    reset();
    for (uint32_t i = 0; i < max_subheaders; i++) {
      subheaders[i].parent = this;
      subheaders[i].init();
    }
  }
};

typedef enum { SCH_SUBH_TYPE = 0, MCH_SUBH_TYPE = 1 } subh_type;

template <class SubH>
class subh
{
public:
  virtual ~subh() {}

  virtual bool read_subheader(uint8_t** ptr)                = 0;
  virtual void read_payload(uint8_t** ptr)                  = 0;
  virtual void write_subheader(uint8_t** ptr, bool is_last) = 0;
  virtual void write_payload(uint8_t** ptr)                 = 0;
  virtual void fprint(FILE* stream)                         = 0;

  pdu<SubH>* parent = nullptr;

private:
  virtual void init() = 0;
};

class sch_subh : public subh<sch_subh>
{
public:
  sch_subh(subh_type type_ = SCH_SUBH_TYPE) : type(type_) {}

  virtual ~sch_subh() {}

  // Size of MAC CEs
  const static int      MAC_CE_CONTRES_LEN = 6;
  const static uint32_t MTCH_STOP_EMPTY    = 0b11111111111;
  const static uint32_t SDU                = 0b00000;

  // Reading functions
  bool        is_sdu();
  bool        is_var_len_ce();
  uint32_t    lcid_value() { return lcid; }
  ul_sch_lcid ul_sch_ce_type();
  dl_sch_lcid dl_sch_ce_type();
  mch_lcid    mch_ce_type();
  uint32_t    size_plus_header();
  void        set_payload_size(uint32_t size);

  bool read_subheader(uint8_t** ptr);
  void read_payload(uint8_t** ptr);

  uint32_t get_sdu_lcid();
  uint32_t get_payload_size();
  uint32_t get_header_size(bool is_last);
  uint8_t* get_sdu_ptr();

  uint16_t get_c_rnti();
  uint64_t get_con_res_id();
  uint8_t  get_ta_cmd();
  uint8_t  get_activation_deactivation_cmd();
  float    get_phr();
  int      get_bsr(uint32_t buff_size[4]);

  bool get_next_mch_sched_info(uint8_t* lcid, uint16_t* mtch_stop);

  // Writing functions
  void write_subheader(uint8_t** ptr, bool is_last);
  void write_payload(uint8_t** ptr);

  int  set_sdu(uint32_t lcid, uint32_t nof_bytes, uint8_t* payload);
  int  set_sdu(uint32_t lcid, uint32_t requested_bytes, read_pdu_interface* sdu_itf);
  bool set_c_rnti(uint16_t crnti);
  bool set_bsr(uint32_t buff_size[4], ul_sch_lcid format);
  bool set_con_res_id(uint64_t con_res_id);
  bool set_ta_cmd(uint8_t ta_cmd);
  bool set_scell_activation_cmd(const std::array<bool, SRSLTE_MAX_CARRIERS>& active_scell_idxs);
  bool set_phr(float phr);
  void set_padding();
  void set_padding(uint32_t padding_len);
  void set_type(subh_type type_);

  void init();
  void fprint(FILE* stream);

  bool set_next_mch_sched_info(uint8_t lcid, uint16_t mtch_stop);

protected:
  static const int MAX_CE_PAYLOAD_LEN = 8;
  uint32_t         lcid               = 0;
  int              nof_bytes          = 0;
  uint8_t*         payload            = nullptr;
  uint8_t          w_payload_ce[64]   = {};
  uint8_t          nof_mch_sched_ce   = 0;
  uint8_t          cur_mch_sched_ce   = 0;
  bool             F_bit              = false;
  subh_type        type               = SCH_SUBH_TYPE;

private:
  uint32_t       sizeof_ce(uint32_t lcid, bool is_ul);
  static uint8_t buff_size_table(uint32_t buffer_size);
  static uint8_t phr_report_table(float phr_value);
};

class sch_pdu : public pdu<sch_subh>
{
public:
  sch_pdu(uint32_t max_subh, const log_ref& log_h_) : pdu(max_subh, log_h_) {}

  void     parse_packet(uint8_t* ptr);
  uint8_t* write_packet();
  uint8_t* write_packet(srslte::log_ref log);
  bool     has_space_ce(uint32_t nbytes, bool var_len = false);
  bool     has_space_sdu(uint32_t nbytes);
  int      get_pdu_len();
  int      rem_size();
  int      get_sdu_space();

  static uint32_t size_header_sdu(uint32_t nbytes);
  bool            update_space_ce(uint32_t nbytes, bool var_len = false);
  bool            update_space_sdu(uint32_t nbytes);
  void            fprint(FILE* stream);
};

class rar_subh : public subh<rar_subh>
{
public:
  typedef enum { BACKOFF = 0, RAPID } rar_subh_type_t;

  rar_subh()
  {
    bzero(&grant, sizeof(grant));
    ta        = 0;
    temp_rnti = 0;
    preamble  = 0;
    parent    = NULL;
    type      = BACKOFF;
  }

  static const uint32_t RAR_GRANT_LEN = 20;

  // Reading functions
  bool     read_subheader(uint8_t** ptr);
  void     read_payload(uint8_t** ptr);
  bool     has_rapid();
  uint32_t get_rapid();
  uint32_t get_ta_cmd();
  uint16_t get_temp_crnti();
  void     get_sched_grant(uint8_t grant[RAR_GRANT_LEN]);

  // Writing functoins
  void write_subheader(uint8_t** ptr, bool is_last);
  void write_payload(uint8_t** ptr);
  void set_rapid(uint32_t rapid);
  void set_ta_cmd(uint32_t ta);
  void set_temp_crnti(uint16_t temp_rnti);
  void set_sched_grant(uint8_t grant[RAR_GRANT_LEN]);

  void init();
  void fprint(FILE* stream);

private:
  uint8_t         grant[RAR_GRANT_LEN];
  uint32_t        ta;
  uint16_t        temp_rnti;
  uint32_t        preamble;
  rar_subh_type_t type;
};

class rar_pdu : public pdu<rar_subh>
{
public:
  rar_pdu(uint32_t max_rars = 16, srslte::log_ref log_ = {});

  void    set_backoff(uint8_t bi);
  bool    has_backoff();
  uint8_t get_backoff();

  bool write_packet(uint8_t* ptr);
  void fprint(FILE* stream);

private:
  bool    has_backoff_indicator;
  uint8_t backoff_indicator;
};

class mch_pdu : public sch_pdu
{
public:
  mch_pdu(uint32_t max_subh, const log_ref& log_h_) : sch_pdu(max_subh, log_h_) {}

private:
  /* Prepares the PDU for parsing or writing by setting the number of subheaders to 0 and the pdu length */
  virtual void init_(byte_buffer_t* buffer_tx_, uint32_t pdu_len_bytes, bool is_ulsch)
  {
    nof_subheaders = 0;
    pdu_len        = pdu_len_bytes;
    rem_len        = pdu_len;
    pdu_is_ul      = is_ulsch;
    buffer_tx      = buffer_tx_;
    last_sdu_idx   = -1;
    reset();
    for (uint32_t i = 0; i < max_subheaders; i++) {
      subheaders[i].set_type(MCH_SUBH_TYPE);
      subheaders[i].parent = this;
      subheaders[i].init();
    }
  }
};

} // namespace srslte

#endif // MACPDU_H
