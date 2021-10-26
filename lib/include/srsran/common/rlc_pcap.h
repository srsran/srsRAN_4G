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

#ifndef RLCPCAP_H
#define RLCPCAP_H

#include "srsran/common/pcap.h"
#include "srsran/interfaces/rlc_interface_types.h"
#include <stdint.h>

namespace srsran {

class rlc_pcap
{
public:
  rlc_pcap() {}
  void enable(bool en);
  void open(const char* filename, const rlc_config_t& config);
  void close();

  void set_ue_id(uint16_t ue_id);

  void write_dl_ccch(uint8_t* pdu, uint32_t pdu_len_bytes);
  void write_ul_ccch(uint8_t* pdu, uint32_t pdu_len_bytes);

private:
  bool     enable_write = false;
  FILE*    pcap_file    = nullptr;
  uint32_t ue_id        = 0;
  uint8_t  mode         = 0;
  uint8_t  sn_length    = 0;
  void     pack_and_write(uint8_t* pdu,
                          uint32_t pdu_len_bytes,
                          uint8_t  mode,
                          uint8_t  direction,
                          uint8_t  priority,
                          uint8_t  seqnumberlength,
                          uint16_t ueid,
                          uint16_t channel_type,
                          uint16_t channel_id);
};

} // namespace srsran

#endif // RLCPCAP_H
