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

#ifndef SRSRAN_S1AP_PCAP_H
#define SRSRAN_S1AP_PCAP_H

#include "srsran/common/pcap.h"

namespace srsran {

class s1ap_pcap
{
public:
  s1ap_pcap()
  {
    enable_write = false;
    pcap_file    = NULL;
  }
  void enable();
  void open(const char* filename);
  void close();
  void write_s1ap(uint8_t* pdu, uint32_t pdu_len_bytes);

private:
  bool  enable_write;
  FILE* pcap_file;
};

} // namespace srsran

#endif // SRSRAN_NAS_PCAP_H
