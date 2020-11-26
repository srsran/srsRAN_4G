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

#ifndef SRSLTE_MAC_NR_PCAP_H
#define SRSLTE_MAC_NR_PCAP_H

#include <string>

namespace srslte {

class mac_nr_pcap
{
public:
  mac_nr_pcap();
  ~mac_nr_pcap();
  void enable(const bool& enable_);
  void open(const std::string& filename, const uint16_t& ue_id = 0);
  void close();

  void set_ue_id(const uint16_t& ue_id);

  void write_dl_crnti(uint8_t* pdu, uint32_t pdu_len_bytes, uint16_t crnti, uint8_t harqid, uint32_t tti);
  void write_ul_crnti(uint8_t* pdu, uint32_t pdu_len_bytes, uint16_t rnti, uint8_t harqid, uint32_t tti);
  void write_dl_ra_rnti(uint8_t* pdu, uint32_t pdu_len_bytes, uint16_t rnti, uint8_t harqid, uint32_t tti);
  void write_dl_bch(uint8_t* pdu, uint32_t pdu_len_bytes, uint16_t rnti, uint8_t harqid, uint32_t tti);
  void write_dl_pch(uint8_t* pdu, uint32_t pdu_len_bytes, uint16_t rnti, uint8_t harqid, uint32_t tti);
  void write_dl_si_rnti(uint8_t* pdu, uint32_t pdu_len_bytes, uint16_t rnti, uint8_t harqid, uint32_t tti);

private:
  bool        enable_write = false;
  std::string filename;
  FILE*       pcap_file = nullptr;
  uint32_t    ue_id     = 0;
  void        pack_and_write(uint8_t* pdu,
                             uint32_t pdu_len_bytes,
                             uint32_t tti,
                             uint16_t crnti_,
                             uint8_t  harqid,
                             uint8_t  direction,
                             uint8_t  rnti_type);
};

} // namespace srslte

#endif // SRSLTE_MAC_NR_PCAP_H
