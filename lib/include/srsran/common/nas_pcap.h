// SPDX-FileCopyrightText: Copyright (C) 2013-2026 Software Radio Systems Limited
// SPDX-License-Identifier: LicenseRef-SRS-Custom

#ifndef SRSRAN_NAS_PCAP_H
#define SRSRAN_NAS_PCAP_H

#include "srsran/common/common.h"
#include "srsran/common/pcap.h"
#include <string>

namespace srsran {

class nas_pcap
{
public:
  nas_pcap();
  ~nas_pcap();
  void     enable();
  uint32_t open(std::string filename_, uint32_t ue_id = 0, srsran_rat_t rat_type = srsran_rat_t::lte);
  void     close();
  void     write_nas(uint8_t* pdu, uint32_t pdu_len_bytes);

private:
  bool        enable_write = false;
  std::string filename;
  FILE*       pcap_file            = nullptr;
  uint32_t    ue_id                = 0;
  int         emergency_handler_id = -1;
  void        pack_and_write(uint8_t* pdu, uint32_t pdu_len_bytes);
};

} // namespace srsran

#endif // SRSRAN_NAS_PCAP_H
