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

#ifndef SRSRAN_RLC_COMMON_H
#define SRSRAN_RLC_COMMON_H

#include "srsran/adt/circular_buffer.h"
#include "srsran/adt/circular_map.h"
#include "srsran/adt/intrusive_list.h"
#include "srsran/interfaces/rlc_interface_types.h"
#include "srsran/rlc/bearer_mem_pool.h"
#include "srsran/rlc/rlc_metrics.h"
#include <cstdlib>
#include <list>

namespace srsran {

/****************************************************************************
 * Structs and Defines
 * Ref: 3GPP TS 36.322 v10.0.0
 ***************************************************************************/

#define MOD 1024
#define RLC_AM_WINDOW_SIZE 512
#define RLC_MAX_SDU_SIZE ((1 << 11) - 1) // Length of LI field is 11bits
#define RLC_AM_MIN_DATA_PDU_SIZE (3)     // AMD PDU with 10 bit SN (length of LI field is 11 bits) (No LI)

#define RLC_AM_NR_TYP_NACKS 512  // Expected number of NACKs in status PDU before expanding space by alloc
#define RLC_AM_NR_MAX_NACKS 2048 // Maximum number of NACKs in status PDU

#define RlcDebug(fmt, ...) logger.debug("%s: " fmt, rb_name, ##__VA_ARGS__)
#define RlcInfo(fmt, ...) logger.info("%s: " fmt, rb_name, ##__VA_ARGS__)
#define RlcWarning(fmt, ...) logger.warning("%s: " fmt, rb_name, ##__VA_ARGS__)
#define RlcError(fmt, ...) logger.error("%s: " fmt, rb_name, ##__VA_ARGS__)

#define RlcHexDebug(msg, bytes, fmt, ...) logger.debug(msg, bytes, "%s: " fmt, rb_name, ##__VA_ARGS__)
#define RlcHexInfo(msg, bytes, fmt, ...) logger.info(msg, bytes, "%s: " fmt, rb_name, ##__VA_ARGS__)
#define RlcHexWarning(msg, bytes, fmt, ...) logger.warning(msg, bytes, "%s: " fmt, rb_name, ##__VA_ARGS__)
#define RlcHexError(msg, bytes, fmt, ...) logger.error(msg, bytes, "%s: " fmt, rb_name, ##__VA_ARGS__)

typedef enum {
  RLC_FI_FIELD_START_AND_END_ALIGNED = 0,
  RLC_FI_FIELD_NOT_END_ALIGNED,
  RLC_FI_FIELD_NOT_START_ALIGNED,
  RLC_FI_FIELD_NOT_START_OR_END_ALIGNED,
  RLC_FI_FIELD_N_ITEMS,
} rlc_fi_field_t;
static const char rlc_fi_field_text[RLC_FI_FIELD_N_ITEMS][32] = {"Start and end aligned",
                                                                 "Not end aligned",
                                                                 "Not start aligned",
                                                                 "Not start or end aligned"};

enum class rlc_nr_si_field_t : unsigned {
  full_sdu                       = 0b00,
  first_segment                  = 0b01,
  last_segment                   = 0b10,
  neither_first_nor_last_segment = 0b11,
  nulltype
};
inline std::string to_string(const rlc_nr_si_field_t& si)
{
  constexpr static const char* options[] = {"Data field contains full SDU",
                                            "Data field contains first segment of SDU",
                                            "Data field contains last segment of SDU",
                                            "Data field contains neither first nor last segment of SDU"};
  return enum_to_text(options, (uint32_t)rlc_nr_si_field_t::nulltype, (uint32_t)si);
}

inline std::string to_string_short(const rlc_nr_si_field_t& si)
{
  constexpr static const char* options[] = {"full", "first", "last", "middle"};
  return enum_to_text(options, (uint32_t)rlc_nr_si_field_t::nulltype, (uint32_t)si);
}

static inline uint8_t operator&(rlc_nr_si_field_t lhs, int rhs)
{
  return static_cast<uint8_t>(static_cast<std::underlying_type<rlc_nr_si_field_t>::type>(lhs) &
                              static_cast<std::underlying_type<rlc_nr_si_field_t>::type>(rhs));
}

enum class rlc_am_nr_control_pdu_type_t : unsigned { status_pdu = 0b000, nulltype };
inline std::string to_string(const rlc_am_nr_control_pdu_type_t& type)
{
  constexpr static const char* options[] = {"Control PDU"};
  return enum_to_text(options, (uint32_t)rlc_am_nr_control_pdu_type_t::nulltype, (uint32_t)type);
}

typedef enum {
  RLC_DC_FIELD_CONTROL_PDU = 0,
  RLC_DC_FIELD_DATA_PDU,
  RLC_DC_FIELD_N_ITEMS,
} rlc_dc_field_t;
static const char rlc_dc_field_text[RLC_DC_FIELD_N_ITEMS][20] = {"Control PDU", "Data PDU"};

// UMD PDU Header
typedef struct {
  uint8_t           fi;                     // Framing info
  rlc_umd_sn_size_t sn_size;                // Sequence number size (5 or 10 bits)
  uint16_t          sn;                     // Sequence number
  uint32_t          N_li;                   // Number of length indicators
  uint16_t          li[RLC_AM_WINDOW_SIZE]; // Array of length indicators
} rlc_umd_pdu_header_t;

typedef struct {
  rlc_nr_si_field_t   si;      // Segmentation info
  rlc_um_nr_sn_size_t sn_size; // Sequence number size (6 or 12 bits)
  uint16_t            sn;      // Sequence number
  uint16_t            so;      // Segment offset
} rlc_um_nr_pdu_header_t;

// AMD PDU Header
class rlc_amd_pdu_header_t
{
public:
  rlc_amd_pdu_header_t() = default;
  rlc_amd_pdu_header_t(const rlc_amd_pdu_header_t& h) { copy(h); }
  rlc_amd_pdu_header_t(rlc_amd_pdu_header_t&& h) noexcept { copy(h); }
  rlc_amd_pdu_header_t& operator=(const rlc_amd_pdu_header_t& h)
  {
    if (this == &h) {
      return *this;
    }
    copy(h);
    return *this;
  }
  rlc_amd_pdu_header_t& operator=(rlc_amd_pdu_header_t&& h) noexcept
  {
    copy(h);
    return *this;
  }
  void copy(const rlc_amd_pdu_header_t& h)
  {
    dc   = h.dc;
    rf   = h.rf;
    p    = h.p;
    fi   = h.fi;
    sn   = h.sn;
    lsf  = h.lsf;
    so   = h.so;
    N_li = h.N_li;
    for (uint32_t i = 0; i < h.N_li; i++) {
      li[i] = h.li[i];
    }
  }

  rlc_dc_field_t dc                     = RLC_DC_FIELD_CONTROL_PDU;           // Data or control
  uint8_t        rf                     = 0;                                  // Resegmentation flag
  uint8_t        p                      = 0;                                  // Polling bit
  uint8_t        fi                     = RLC_FI_FIELD_START_AND_END_ALIGNED; // Framing info
  uint16_t       sn                     = 0;                                  // Sequence number
  uint8_t        lsf                    = 0;                                  // Last segment flag
  uint16_t       so                     = 0;                                  // Segment offset
  uint32_t       N_li                   = 0;                                  // Number of length indicators
  uint16_t       li[RLC_AM_WINDOW_SIZE] = {0};                                // Array of length indicators
};

// NACK helper (for LTE and NR)
struct rlc_status_nack_t {
  const static uint16_t so_end_of_sdu = 0xFFFF;

  uint32_t nack_sn;        // Sequence Number (SN) of first missing SDU
  bool     has_so;         // NACKs continuous sequence of bytes [so_start..so_end]
  uint16_t so_start;       // First missing byte in SDU with SN=nack_sn
  uint16_t so_end;         // Last missing byte in SDU with SN=nack_sn or SN=nack_sn+nack_range-1 if has_nack_range.
  bool     has_nack_range; // NACKs continuous sequence of SDUs
  uint8_t  nack_range;     // Number of SDUs being NACKed (including SN=nack_sn)

  rlc_status_nack_t()
  {
    has_so         = false;
    nack_sn        = 0;
    so_start       = 0;
    so_end         = so_end_of_sdu;
    has_nack_range = false;
    nack_range     = 0;
  }

  bool equals(const rlc_status_nack_t& other) const
  {
    return nack_sn == other.nack_sn && has_so == other.has_so && so_start == other.so_start && so_end == other.so_end &&
           has_nack_range == other.has_nack_range && nack_range == other.nack_range;
  }

  bool operator==(const rlc_status_nack_t& other) const { return equals(other); }
  bool operator!=(const rlc_status_nack_t& other) const { return not equals(other); }
};

// STATUS PDU
struct rlc_status_pdu_t {
  uint16_t          ack_sn; // SN of the next not received RLC Data PDU
  uint32_t          N_nack;
  rlc_status_nack_t nacks[RLC_AM_WINDOW_SIZE];

  rlc_status_pdu_t()
  {
    N_nack = 0;
    ack_sn = 0;
  }
};

typedef std::function<void(uint32_t, uint32_t, uint32_t)> bsr_callback_t;

/****************************************************************************
 * RLC Common interface
 * Common interface for all RLC entities
 ***************************************************************************/
class rlc_common
{
public:
  explicit rlc_common(srslog::basic_logger& logger_) : logger(logger_) {}
  virtual ~rlc_common()                            = default;
  virtual bool configure(const rlc_config_t& cnfg) = 0;
  virtual void stop()                              = 0;
  virtual void reestablish()                       = 0;
  virtual void empty_queue()                       = 0;

  bool suspend()
  {
    if (suspended) {
      return false;
    }
    suspended = true;
    return true;
  }

  // Pops all PDUs from queue and calls write_pdu() method for the bearer type
  bool resume()
  {
    if (!suspended) {
      return false;
    }
    unique_byte_buffer_t rx_pdu;
    // Do not block
    while (rx_pdu_resume_queue.try_pop(rx_pdu)) {
      write_pdu(rx_pdu->msg, rx_pdu->N_bytes);
    }

    unique_byte_buffer_t tx_sdu;
    while (tx_sdu_resume_queue.try_pop(tx_sdu)) {
      write_sdu(std::move(tx_sdu));
    }
    suspended = false;
    return true;
  }

  void write_pdu_s(uint8_t* payload, uint32_t nof_bytes)
  {
    if (suspended) {
      queue_rx_pdu(payload, nof_bytes);
    } else {
      write_pdu(payload, nof_bytes);
    }
  }

  void write_sdu_s(unique_byte_buffer_t sdu)
  {
    if (suspended) {
      queue_tx_sdu(std::move(sdu));
    } else {
      write_sdu(std::move(sdu));
    }
  }

  virtual rlc_mode_t get_mode() = 0;
  virtual uint32_t   get_lcid() = 0;

  virtual rlc_bearer_metrics_t get_metrics()   = 0;
  virtual void                 reset_metrics() = 0;

  // PDCP interface
  virtual void write_sdu(unique_byte_buffer_t sdu) = 0;
  virtual void discard_sdu(uint32_t discard_sn)    = 0;
  virtual bool sdu_queue_is_full()                 = 0;

  // MAC interface
  virtual bool     has_data() = 0;
  bool             is_suspended() { return suspended; };
  virtual uint32_t get_buffer_state()                                            = 0;
  virtual void     get_buffer_state(uint32_t& tx_queue, uint32_t& prio_tx_queue) = 0;
  virtual uint32_t read_pdu(uint8_t* payload, uint32_t nof_bytes)                = 0;
  virtual void     write_pdu(uint8_t* payload, uint32_t nof_bytes)               = 0;

  virtual void set_bsr_callback(bsr_callback_t callback) = 0;

  void* operator new(size_t sz) { return allocate_rlc_bearer(sz); }
  void  operator delete(void* p) { return deallocate_rlc_bearer(p); }

protected:
  std::string           rb_name = {};
  srslog::basic_logger& logger;

private:
  bool suspended = false;

  // Enqueues the Rx PDU in the resume queue
  void queue_rx_pdu(uint8_t* payload, uint32_t nof_bytes)
  {
    unique_byte_buffer_t rx_pdu = srsran::make_byte_buffer();
    if (rx_pdu == nullptr) {
      srslog::fetch_basic_logger("RLC").warning("Couldn't allocate PDU in %s().", __FUNCTION__);
      return;
    }

    if (rx_pdu->get_tailroom() < nof_bytes) {
      srslog::fetch_basic_logger("RLC").warning("Not enough space to store PDU.");
      return;
    }

    memcpy(rx_pdu->msg, payload, nof_bytes);
    rx_pdu->N_bytes = nof_bytes;

    // Do not block ever
    if (!rx_pdu_resume_queue.try_push(std::move(rx_pdu))) {
      srslog::fetch_basic_logger("RLC").warning("Dropping SDUs while bearer suspended.");
      return;
    }
  }

  // Enqueues the Tx SDU in the resume queue
  void queue_tx_sdu(unique_byte_buffer_t sdu)
  {
    // Do not block ever
    if (not tx_sdu_resume_queue.try_push(std::move(sdu))) {
      srslog::fetch_basic_logger("RLC").warning("Dropping SDUs while bearer suspended.");
      return;
    }
  }

  static_blocking_queue<unique_byte_buffer_t, 256> rx_pdu_resume_queue;
  static_blocking_queue<unique_byte_buffer_t, 256> tx_sdu_resume_queue;
};

} // namespace srsran

#endif // SRSRAN_RLC_COMMON_H
