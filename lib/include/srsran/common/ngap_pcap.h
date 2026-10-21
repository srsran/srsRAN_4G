// SPDX-FileCopyrightText: Copyright (C) 2013-2026 Software Radio Systems Limited
// SPDX-License-Identifier: LicenseRef-SRS-Custom

#ifndef SRSRAN_NGAP_PCAP_H
#define SRSRAN_NGAP_PCAP_H

#include "srsran/common/pcap.h"
#include <string>

namespace srsran {

class ngap_pcap
{
public:
  ngap_pcap();
  ~ngap_pcap();
  ngap_pcap(const ngap_pcap& other) = delete;
  ngap_pcap& operator=(const ngap_pcap& other) = delete;
  ngap_pcap(ngap_pcap&& other)                 = delete;
  ngap_pcap& operator=(ngap_pcap&& other) = delete;

  void enable();
  void open(const char* filename_);
  void close();
  void write_ngap(uint8_t* pdu, uint32_t pdu_len_bytes);

private:
  bool        enable_write = false;
  std::string filename;
  FILE*       pcap_file            = nullptr;
  int         emergency_handler_id = -1;
};

} // namespace srsran

#endif // SRSRAN_NGAP_PCAP_H
