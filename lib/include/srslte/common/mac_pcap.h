/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2015 Software Radio Systems Limited
 *
 * \section LICENSE
 *
 * This file is part of the srsUE library.
 *
 * srsUE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsUE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#ifndef SRSLTE_MAC_PCAP_H
#define SRSLTE_MAC_PCAP_H

#include <stdint.h>
#include "srslte/common/pcap.h"

namespace srslte {

class mac_pcap
{
public: 
  mac_pcap() {enable_write=false; ue_id=0; pcap_file = NULL; };
  void enable(bool en);
  void open(const char *filename, uint32_t ue_id = 0);
  void close();

  void set_ue_id(uint16_t ue_id);

  void write_ul_crnti(uint8_t *pdu, uint32_t pdu_len_bytes, uint16_t crnti, uint32_t reTX, uint32_t tti);
  void write_dl_crnti(uint8_t *pdu, uint32_t pdu_len_bytes, uint16_t crnti, bool crc_ok, uint32_t tti);
  void write_dl_ranti(uint8_t *pdu, uint32_t pdu_len_bytes, uint16_t ranti, bool crc_ok, uint32_t tti);
  
  // SI and BCH only for DL 
  void write_dl_sirnti(uint8_t *pdu, uint32_t pdu_len_bytes, bool crc_ok, uint32_t tti);
  void write_dl_bch(uint8_t *pdu, uint32_t pdu_len_bytes, bool crc_ok, uint32_t tti);
  void write_dl_pch(uint8_t *pdu, uint32_t pdu_len_bytes, bool crc_ok, uint32_t tti);
  
private:
  bool enable_write; 
  FILE *pcap_file; 
  uint32_t ue_id;
  void pack_and_write(uint8_t* pdu, uint32_t pdu_len_bytes, uint32_t reTX, bool crc_ok, uint32_t tti,
                              uint16_t crnti_, uint8_t direction, uint8_t rnti_type);
};

} // namespace srslte

#endif // SRSLTE_MAC_PCAP_H
