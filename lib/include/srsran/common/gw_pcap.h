/**
 * @file gw_pcap.h
 * @author Zishuai CHENG (5786734+WingPig99@users.noreply.github.com)
 * @brief
 * @version 0.1
 * @date 2022-06-23
 *
 * @copyright Copyright (c) 2022
 *
 */

#ifndef SRSUE_GW_PCAP_H
#define SRSUE_GW_PCAP_H

#include <cstdio>
#include <stdint.h>
#include <string>

namespace srsran {
class gw_pcap
{
public:
  gw_pcap();
  ~gw_pcap();
  uint32_t open(const char* filename);
  void     close();

  void enable();

  void write_dl_pdu(uint32_t lcid, uint8_t* pdu, uint32_t pdu_len_bytes);
  void write_dl_pdu_mch(uint32_t lcid, uint8_t* pdu, uint32_t pdu_len_bytes);
  void write_ul_pdu(uint32_t lcid, uint8_t* pdu, uint32_t pdu_len_bytes);

private:
  std::string pcap_filename;
  bool        enable_write;
  FILE*       pcap_file;
};
} // namespace srsran
#endif