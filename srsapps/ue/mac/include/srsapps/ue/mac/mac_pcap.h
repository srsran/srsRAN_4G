/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2015 The srsLTE Developers. See the
 * COPYRIGHT file at the top-level directory of this distribution.
 *
 * \section LICENSE
 *
 * This file is part of the srsLTE library.
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


#include <stdint.h>
#include "srsapps/ue/mac/pcap.h"

#ifndef MACPCAP_H
#define MACPCAP_H

namespace srslte {
namespace ue {

class mac_pcap
{
public: 
  mac_pcap() {enable_write=false; ue_id=0;}; 
  void enable(bool en);
  bool open(const char *filename, uint32_t ue_id = 0);
  void close(); 
  void write_ul_crnti(uint8_t *pdu, uint32_t pdu_len_bytes, uint16_t crnti, uint32_t reTX, uint32_t tti);
  void write_dl_crnti(uint8_t *pdu, uint32_t pdu_len_bytes, uint16_t crnti, bool crc_ok, uint32_t tti);
  void write_dl_ranti(uint8_t *pdu, uint32_t pdu_len_bytes, uint16_t ranti, bool crc_ok, uint32_t tti);
  
  // SI and BCH only for DL 
  void write_dl_sirnti(uint8_t *pdu, uint32_t pdu_len_bytes, bool crc_ok, uint32_t tti);
  void write_dl_bch(uint8_t *pdu, uint32_t pdu_len_bytes, bool crc_ok, uint32_t tti);
  
private:
  bool enable_write; 
  FILE *pcap_file; 
  uint32_t ue_id; 
  void pack_and_write(uint8_t* pdu, uint32_t pdu_len_bytes, uint32_t reTX, bool crc_ok, uint32_t tti, 
                              uint16_t crnti_, uint8_t direction, uint8_t rnti_type);
};

}
}

#endif