/*
 * Copyright 2013-2020 Software Radio Systems Limited
 *
 * This file is part of srsLTE.
 *
 * srsLTE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsLTE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
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
