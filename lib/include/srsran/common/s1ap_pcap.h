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
#include <string>

namespace srsran {

class s1ap_pcap
{
public:
  s1ap_pcap();
  s1ap_pcap(const s1ap_pcap& other) = delete;
  s1ap_pcap& operator=(const s1ap_pcap& other) = delete;
  s1ap_pcap(s1ap_pcap&& other)                 = delete;
  s1ap_pcap& operator=(s1ap_pcap&& other) = delete;

  void enable();
  void open(const char* filename_);
  void close();
  void write_s1ap(uint8_t* pdu, uint32_t pdu_len_bytes);

private:
  bool        enable_write = false;
  std::string filename;
  FILE*       pcap_file = nullptr;
};

} // namespace srsran

#endif // SRSRAN_NAS_PCAP_H
