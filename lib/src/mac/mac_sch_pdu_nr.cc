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

#include "srslte/mac/mac_sch_pdu_nr.h"

namespace srslte {

mac_sch_subpdu_nr::mac_sch_subpdu_nr(mac_sch_pdu_nr* parent_) :
  parent(parent_), logger(srslog::fetch_basic_logger("MAC"))
{}

mac_sch_subpdu_nr::nr_lcid_sch_t mac_sch_subpdu_nr::get_type()
{
  if (lcid >= 32) {
    return (nr_lcid_sch_t)lcid;
  }

  return CCCH;
}

bool mac_sch_subpdu_nr::is_sdu()
{
  // for UL-SCH LCID 52 is also valid for carrying SDUs
  return (lcid <= 32 || (parent->is_ulsch() && lcid == 52));
}

// returns false for all reserved values in Table 6.2.1-1 and 6.2.1-2
bool mac_sch_subpdu_nr::is_valid_lcid()
{
  return (lcid <= 63 && ((parent->is_ulsch() && (lcid <= 32 || lcid >= 52)) || (lcid <= 32 || lcid >= 47)));
}

bool mac_sch_subpdu_nr::is_var_len_ce()
{
  return false;
}

// return length of PDU (or SRSLTE_ERROR otherwise)
int32_t mac_sch_subpdu_nr::read_subheader(const uint8_t* ptr)
{
  // Skip R, read F bit and LCID
  F_bit = (bool)(*ptr & 0x40) ? true : false;
  lcid  = (uint8_t)*ptr & 0x3f;
  ptr++;
  header_length = 1;

  if (is_valid_lcid()) {
    if ((is_sdu() || is_var_len_ce()) && not is_ul_ccch()) {
      // Read first length byte
      sdu_length = (uint32_t)*ptr;
      ptr++;
      header_length++;

      if (F_bit) {
        // add second length byte
        sdu_length = sdu_length << 8 | ((uint32_t)*ptr & 0xff);
        ptr++;
        header_length++;
      }
    } else {
      sdu_length = sizeof_ce(lcid, parent->is_ulsch());
    }
    sdu = (uint8_t*)ptr;
  } else {
    logger.warning("Invalid LCID (%d) in MAC PDU", lcid);
    return SRSLTE_ERROR;
  }
  return header_length;
}

void mac_sch_subpdu_nr::set_sdu(const uint32_t lcid_, const uint8_t* payload_, const uint32_t len_)
{
  lcid          = lcid_;
  sdu           = const_cast<uint8_t*>(payload_);
  header_length = is_ul_ccch() ? 1 : 2;
  sdu_length    = len_;
  if (is_ul_ccch()) {
    F_bit      = false;
    sdu_length = sizeof_ce(lcid, parent->is_ulsch());
    if (len_ != static_cast<uint32_t>(sdu_length)) {
      logger.warning("Invalid SDU length of UL-SCH SDU (%d != %d)", len_, sdu_length);
    }
  }

  if (sdu_length >= 256) {
    F_bit = true;
    header_length += 1;
  }
}

void mac_sch_subpdu_nr::set_padding(const uint32_t len_)
{
  lcid = PADDING;
  // 1 Byte R/LCID MAC subheader
  sdu_length    = len_ - 1;
  header_length = 1;
}

// Section 6.1.2
uint32_t mac_sch_subpdu_nr::write_subpdu(const uint8_t* start_)
{
  uint8_t* ptr = const_cast<uint8_t*>(start_);
  *ptr         = (uint8_t)((F_bit ? 1 : 0) << 6) | ((uint8_t)lcid & 0x3f);
  ptr += 1;

  if (header_length == 3) {
    // 3 Byte R/F/LCID/L MAC subheader with 16-bit L field
    *ptr = ((sdu_length & 0xff00) >> 8);
    ptr += 1;
    *ptr = static_cast<uint8_t>(sdu_length);
    ptr += 1;

  } else if (header_length == 2) {
    // 2 Byte R/F/LCID/L MAC subheader with 8-bit L field
    *ptr = static_cast<uint8_t>(sdu_length);
    ptr += 1;
  } else if (header_length == 1) {
    // do nothing
  } else {
    logger.warning("Error while packing PDU. Unsupported header length (%d)", header_length);
  }

  // copy SDU payload
  if (sdu) {
    memcpy(ptr, sdu, sdu_length);
  } else {
    // clear memory
    memset(ptr, 0, sdu_length);
  }

  ptr += sdu_length;

  // return total length of subpdu
  return ptr - start_;
}

uint32_t mac_sch_subpdu_nr::get_total_length()
{
  return (header_length + sdu_length);
}

uint32_t mac_sch_subpdu_nr::get_sdu_length()
{
  return sdu_length;
}

uint32_t mac_sch_subpdu_nr::get_lcid()
{
  return lcid;
}

uint8_t* mac_sch_subpdu_nr::get_sdu()
{
  return sdu;
}

uint32_t mac_sch_subpdu_nr::sizeof_ce(uint32_t lcid, bool is_ul)
{
  if (is_ul) {
    switch (lcid) {
      case CCCH_SIZE_48:
        return 6;
      case CCCH_SIZE_64:
        return 8;
      case CRNTI:
        return 2;
      case SHORT_TRUNC_BSR:
        return 1;
      case SHORT_BSR:
        return 1;
      case PADDING:
        return 0;
    }
  } else {
    switch (lcid) {
      case CON_RES_ID:
        return 6;
      case TA_CMD:
        return 1;
      case DRX_CMD:
        return 0;
      case PADDING:
        return 0;
    }
  }
  return 0;
}

inline bool mac_sch_subpdu_nr::is_ul_ccch()
{
  return (parent->is_ulsch() && (lcid == CCCH_SIZE_48 || lcid == CCCH_SIZE_64));
}

void mac_sch_pdu_nr::pack()
{
  // SDUs are written in place, only add padding if needed
  if (remaining_len) {
    mac_sch_subpdu_nr padding_subpdu(this);
    padding_subpdu.set_padding(remaining_len);
    padding_subpdu.write_subpdu(buffer->msg + buffer->N_bytes);

    // update length
    buffer->N_bytes += padding_subpdu.get_total_length();
    remaining_len -= padding_subpdu.get_total_length();
    subpdus.push_back(padding_subpdu);
  }
}

void mac_sch_pdu_nr::unpack(const uint8_t* payload, const uint32_t& len)
{
  uint32_t offset = 0;
  while (offset < len) {
    mac_sch_subpdu_nr sch_pdu(this);
    if (sch_pdu.read_subheader(payload + offset) == SRSLTE_ERROR) {
      fprintf(stderr, "Error parsing NR MAC PDU (len=%d, offset=%d)\n", len, offset);
      return;
    }
    offset += sch_pdu.get_total_length();
    if (sch_pdu.get_lcid() == mac_sch_subpdu_nr::PADDING) {
      // set SDU length to rest of PDU
      sch_pdu.set_padding(len - offset + 1); // One byte for Padding header will be substracted again
      // skip remaining bytes
      offset = len;
    }
    subpdus.push_back(sch_pdu);
  }
  if (offset != len) {
    fprintf(stderr, "Error parsing NR MAC PDU (len=%d, offset=%d)\n", len, offset);
  }
}

uint32_t mac_sch_pdu_nr::get_num_subpdus()
{
  return subpdus.size();
}

const mac_sch_subpdu_nr& mac_sch_pdu_nr::get_subpdu(const uint32_t& index)
{
  return subpdus.at(index);
}

bool mac_sch_pdu_nr::is_ulsch()
{
  return ulsch;
}

void mac_sch_pdu_nr::init_tx(byte_buffer_t* buffer_, uint32_t pdu_len_, bool ulsch_)
{
  buffer = buffer_;
  subpdus.clear();
  pdu_len       = pdu_len_;
  remaining_len = pdu_len_;
  ulsch         = ulsch_;
}

void mac_sch_pdu_nr::init_rx(bool ulsch_)
{
  buffer = nullptr;
  subpdus.clear();
  pdu_len       = 0;
  remaining_len = 0;
  ulsch         = ulsch_;
}

uint32_t mac_sch_pdu_nr::size_header_sdu(const uint32_t lcid, const uint32_t nbytes)
{
  if (ulsch && (lcid == mac_sch_subpdu_nr::CCCH_SIZE_48 || lcid == mac_sch_subpdu_nr::CCCH_SIZE_64)) {
    return 1;
  } else {
    if (nbytes < 256) {
      return 2;
    } else {
      return 3;
    }
  }
}

uint32_t mac_sch_pdu_nr::get_remaing_len()
{
  return remaining_len;
}

uint32_t mac_sch_pdu_nr::add_sdu(const uint32_t lcid_, const uint8_t* payload_, const uint32_t len_)
{
  int header_size = size_header_sdu(lcid_, len_);

  if (header_size + len_ > remaining_len) {
    printf("Header and SDU exceed space in PDU (%d > %d).\n", header_size + len_, remaining_len);
    return SRSLTE_ERROR;
  }

  mac_sch_subpdu_nr sch_pdu(this);
  sch_pdu.set_sdu(lcid_, payload_, len_);
  uint32_t length = sch_pdu.write_subpdu(buffer->msg + buffer->N_bytes);

  if (length != sch_pdu.get_total_length()) {
    fprintf(stderr, "Error writing subPDU (Length error: %d != %d)\n", length, sch_pdu.get_total_length());
    return SRSLTE_ERROR;
  }

  // update length and advance payload pointer
  buffer->N_bytes += length;
  remaining_len -= length;

  subpdus.push_back(sch_pdu);

  return SRSLTE_SUCCESS;
}

} // namespace srslte
