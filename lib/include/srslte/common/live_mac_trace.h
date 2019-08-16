/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2018 Software Radio Systems Limited
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


#ifndef SRSLTE_LIVE_MAC_TRACE_H
#define SRSLTE_LIVE_MAC_TRACE_H


#include "srslte/srslte.h"
#include "srslte/common/pcap.h"
#include "srslte/common/threads.h"
#include "srslte/common/buffer_pool.h"
#include "srslte/common/block_queue.h"
#include "srslte/interfaces/enb_interfaces.h"

namespace srslte {

class live_mac_trace : public thread, public srsenb::trace_interface_mac{
public:
  live_mac_trace();
  void init(const char * server_ip_addr_, uint16_t server_udp_port_, const char * client_ip_addr_, uint16_t client_udp_port_);
  void stop();
  void set_ue_id(uint16_t ue_id);

  void write_ul_crnti(uint8_t *pdu, uint32_t pdu_len_bytes, uint16_t crnti, uint32_t reTX, uint32_t tti);
  void write_dl_crnti(uint8_t *pdu, uint32_t pdu_len_bytes, uint16_t crnti, bool crc_ok, uint32_t tti);
  void write_dl_ranti(uint8_t *pdu, uint32_t pdu_len_bytes, uint16_t ranti, bool crc_ok, uint32_t tti);
  
  // SI and BCH only for DL 
  void write_dl_sirnti(uint8_t *pdu, uint32_t pdu_len_bytes, bool crc_ok, uint32_t tti);
  void write_dl_bch(uint8_t *pdu, uint32_t pdu_len_bytes, bool crc_ok, uint32_t tti);
  void write_dl_pch(uint8_t *pdu, uint32_t pdu_len_bytes, bool crc_ok, uint32_t tti);
  void write_dl_mch(uint8_t *pdu, uint32_t pdu_len_bytes, bool crc_ok, uint32_t tti);

private:
  byte_buffer_pool        *pool;
  bool running;
  byte_buffer_t udp_datagram;

  uint16_t ue_id;
  uint16_t udp_port;

  typedef struct{
    MAC_Context_Info_t  context;
    srslte::byte_buffer_t*  pdu;
  }mac_trace_pdu_t;

  srslte::block_queue<mac_trace_pdu_t> mac_trace_pdu_queue;

  struct sockaddr_in server_addr;
  struct sockaddr_in client_addr;

  int socket_d;
  void pack_and_queue(uint8_t* pdu, uint32_t pdu_len_bytes, uint32_t reTX, bool crc_ok, uint32_t tti,
                              uint16_t crnti_, uint8_t direction, uint8_t rnti_type);
  void send_mac_datagram(uint8_t* pdu, uint32_t pdu_len_bytes, MAC_Context_Info_t *context);
  void run_thread();
};

}

#endif // SRSLTE_LIVE_MAC_TRACE_H