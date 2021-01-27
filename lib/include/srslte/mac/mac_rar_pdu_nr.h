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

#ifndef SRSLTE_MAC_RAR_PDU_NR_H
#define SRSLTE_MAC_RAR_PDU_NR_H

#include "srslte/common/common.h"
#include "srslte/config.h"
#include "srslte/srslog/srslog.h"
#include <memory>
#include <stdint.h>
#include <vector>

namespace srslte {

class mac_rar_pdu_nr;

// 3GPP 38.321 v15.3.0 Sec 6.1.5
class mac_rar_subpdu_nr
{
public:
  // Possible types of RAR subpdus (same like EUTRA)
  typedef enum { BACKOFF = 0, RAPID } rar_subh_type_t;

  mac_rar_subpdu_nr(mac_rar_pdu_nr* parent_);

  // RAR content length in bits (38.321 Sec 6.2.3)
  static const uint32_t UL_GRANT_NBITS   = 27;
  static const uint32_t TA_COMMAND_NBITS = 12;

  // getter
  bool     read_subpdu(const uint8_t* ptr);
  bool     has_more_subpdus();
  uint32_t get_total_length();
  bool     has_rapid();
  uint8_t  get_rapid();
  uint16_t get_temp_crnti();
  uint32_t get_ta();
  void     get_ul_grant(std::array<uint8_t, UL_GRANT_NBITS>& grant);

  bool    has_backoff();
  uint8_t get_backoff();

  // setter
  uint32_t write_subpdu(const uint8_t* start_);
  void     set_backoff(const uint8_t backoff_indicator_);

  std::string to_string();

private:
  int header_length  = 1; // RAR PDU subheader is always 1 B
  int payload_length = 0; // only used if MAC RAR is included

  std::array<uint8_t, UL_GRANT_NBITS> ul_grant          = {};
  uint16_t                            ta                = 0; // 12bit TA
  uint16_t                            temp_crnti        = 0;
  uint16_t                            rapid             = 0;
  uint8_t                             backoff_indicator = 0;
  rar_subh_type_t                     type              = BACKOFF;
  bool                                E_bit             = 0;

  srslog::basic_logger& logger;

  mac_rar_pdu_nr* parent = nullptr;
};

class mac_rar_pdu_nr
{
public:
  mac_rar_pdu_nr();
  ~mac_rar_pdu_nr() = default;

  bool                     pack();
  bool                     unpack(const uint8_t* payload, const uint32_t& len);
  uint32_t                 get_num_subpdus();
  const mac_rar_subpdu_nr& get_subpdu(const uint32_t& index);

  uint32_t get_remaining_len();

  void set_si_rapid(uint16_t si_rapid_); // configured through SIB1 for on-demand SI request (See 38.331 Sec 5.2.1)
  bool has_si_rapid();

  std::string to_string();

private:
  std::vector<mac_rar_subpdu_nr> subpdus;
  uint32_t                       remaining_len = 0;
  uint16_t                       si_rapid      = 0;
  bool                           si_rapid_set  = false;
  srslog::basic_logger&          logger;
};

} // namespace srslte

#endif // SRSLTE_MAC_RAR_PDU_NR_H
