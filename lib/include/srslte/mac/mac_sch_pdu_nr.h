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

#ifndef SRSLTE_MAC_SCH_PDU_NR_H
#define SRSLTE_MAC_SCH_PDU_NR_H

#include "srslte/common/common.h"
#include "srslte/common/logmap.h"
#include "srslte/config.h"
#include "srslte/srslog/srslog.h"
#include <memory>
#include <stdint.h>
#include <vector>

namespace srslte {

class mac_sch_pdu_nr;

class mac_sch_subpdu_nr
{
public:
  // 3GPP 38.321 v15.3.0 Combined Tables 6.2.1-1, 6.2.1-2
  typedef enum {
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
  } nr_lcid_sch_t;

  mac_sch_subpdu_nr(mac_sch_pdu_nr* parent_);

  nr_lcid_sch_t get_type();
  bool          is_sdu();
  bool          is_valid_lcid();
  bool          is_var_len_ce();
  bool          is_ul_ccch();

  int32_t  read_subheader(const uint8_t* ptr);
  uint32_t get_total_length();
  uint32_t get_sdu_length();
  uint32_t get_lcid();
  uint8_t* get_sdu();
  uint16_t get_c_rnti();

  // both return the reported values as per TS 38.321, mapping to dB according to TS 38.133 Sec 10.1.17 not done here
  uint8_t get_phr();
  uint8_t get_pcmax();

  // BSR
  struct lcg_bsr_t {
    uint8_t lcg_id;
    uint8_t buffer_size;
  };
  lcg_bsr_t                               get_sbsr();
  static const uint8_t                    max_num_lcg_lbsr = 8;
  std::array<lcg_bsr_t, max_num_lcg_lbsr> get_lbsr();

  void set_sdu(const uint32_t lcid_, const uint8_t* payload_, const uint32_t len_);

  void set_padding(const uint32_t len_);

  uint32_t write_subpdu(const uint8_t* start_);

private:
  uint32_t sizeof_ce(uint32_t lcid, bool is_ul);

  // protected:
  uint32_t lcid          = 0;
  int      header_length = 0;
  int      sdu_length    = 0;
  bool     F_bit         = false;
  uint8_t* sdu           = nullptr;

  mac_sch_pdu_nr*       parent = nullptr;
  srslog::basic_logger& logger;
};

class mac_sch_pdu_nr
{
public:
  mac_sch_pdu_nr(bool ulsch_ = false) : ulsch(ulsch_) {}

  void                     pack();
  void                     unpack(const uint8_t* payload, const uint32_t& len);
  uint32_t                 get_num_subpdus();
  const mac_sch_subpdu_nr& get_subpdu(const uint32_t& index);
  bool                     is_ulsch();

  void init_tx(byte_buffer_t* buffer_, uint32_t pdu_len_, bool is_ulsch_ = false);
  void init_rx(bool ulsch_ = false);

  uint32_t add_sdu(const uint32_t lcid_, const uint8_t* payload_, const uint32_t len_);

  uint32_t get_remaing_len();

private:
  uint32_t size_header_sdu(const uint32_t lcid_, const uint32_t nbytes);

  bool                           ulsch = false;
  std::vector<mac_sch_subpdu_nr> subpdus;

  byte_buffer_t* buffer        = nullptr;
  uint32_t       pdu_len       = 0;
  uint32_t       remaining_len = 0;
};

} // namespace srslte

#endif // SRSLTE_MAC_SCH_PDU_NR_H
