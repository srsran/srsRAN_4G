/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2019 Software Radio Systems Limited
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

#include "srslte/common/live_mac_trace.h"

using namespace std;

namespace srslte{
  
live_mac_trace::live_mac_trace(){

}

void live_mac_trace::init(char * server_ip_addr_, uint16_t server_udp_port_, char * client_ip_addr_, uint16_t client_udp_port_){
  ueid = 0;
  socket_d = -1;
  socket_d = socket(AF_INET, SOCK_DGRAM, 0);
  if(socket_d < 0) {
    printf("Failed to create listener socket: %s\n", strerror(errno));
    return;
  }

  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = inet_addr(server_ip_addr_);
  server_addr.sin_port = htons(server_udp_port_);
  
  int ret = bind(socket_d, (struct sockaddr *) &server_addr, sizeof(server_addr));
  
  if (ret != 0){
    printf("Failed to bind socket to (%s:%u) %s\n", server_ip_addr_, server_udp_port_, strerror(errno));
    close(socket_d);
    socket_d = -1;
  } 

  client_addr.sin_family = AF_INET;
  client_addr.sin_addr.s_addr = inet_addr(client_ip_addr_);
  client_addr.sin_port = htons(client_udp_port_);
}

void live_mac_trace::stop(){
  close(socket_d);

}

void live_mac_trace::write_dl_crnti(uint8_t* pdu, uint32_t pdu_len_bytes, uint16_t rnti, bool crc_ok, uint32_t tti)
{
  send_mac_datagram(pdu, pdu_len_bytes, 0, crc_ok, tti, rnti, DIRECTION_DOWNLINK, C_RNTI);
}
void live_mac_trace::write_dl_ranti(uint8_t* pdu, uint32_t pdu_len_bytes, uint16_t rnti, bool crc_ok, uint32_t tti)
{
  send_mac_datagram(pdu, pdu_len_bytes, 0, crc_ok, tti, rnti, DIRECTION_DOWNLINK, RA_RNTI);
}
void live_mac_trace::write_ul_crnti(uint8_t* pdu, uint32_t pdu_len_bytes, uint16_t rnti, uint32_t reTX, uint32_t tti)
{
  send_mac_datagram(pdu, pdu_len_bytes, reTX, true, tti, rnti, DIRECTION_UPLINK, C_RNTI);
}
void live_mac_trace::write_dl_bch(uint8_t* pdu, uint32_t pdu_len_bytes, bool crc_ok, uint32_t tti)
{
  send_mac_datagram(pdu, pdu_len_bytes, 0, crc_ok, tti, 0, DIRECTION_DOWNLINK, NO_RNTI);
}
void live_mac_trace::write_dl_pch(uint8_t* pdu, uint32_t pdu_len_bytes, bool crc_ok, uint32_t tti)
{
  send_mac_datagram(pdu, pdu_len_bytes, 0, crc_ok, tti, SRSLTE_PRNTI, DIRECTION_DOWNLINK, P_RNTI);
}
void live_mac_trace::write_dl_mch(uint8_t* pdu, uint32_t pdu_len_bytes, bool crc_ok, uint32_t tti)
{
  send_mac_datagram(pdu, pdu_len_bytes, 0, crc_ok, tti, SRSLTE_MRNTI, DIRECTION_DOWNLINK, M_RNTI);
}
void live_mac_trace::write_dl_sirnti(uint8_t* pdu, uint32_t pdu_len_bytes, bool crc_ok, uint32_t tti)
{
  send_mac_datagram(pdu, pdu_len_bytes, 0, crc_ok, tti, SRSLTE_SIRNTI, DIRECTION_DOWNLINK, SI_RNTI);
}

void live_mac_trace::send_mac_datagram(uint8_t* pdu, uint32_t pdu_len_bytes, uint32_t reTX, bool crc_ok, uint32_t tti,
                              uint16_t rnti, uint8_t direction, uint8_t rnti_type){

  ssize_t bytes_sent;
  uint32_t offset = 0;
  uint16_t tmp16;

  uint32_t total_size = pdu_len_bytes + sizeof(MAC_Context_Info_t) + sizeof(MAC_LTE_START_STRING);
  
  if (total_size >= MAC_UDP_PDU_MAX_SIZE){
    printf("PDU size exceeds allocated byte buffer\n");
    return;
  }
   
  memset(buffer, 0, total_size);

  // MAC_LTE_START_STRING for UDP heuristics 
  memcpy(buffer + offset, MAC_LTE_START_STRING, strlen(MAC_LTE_START_STRING));
  offset += strlen(MAC_LTE_START_STRING);
  
  /*****************************************************************/
  /* Context information (same as written by UDP heuristic clients */
  buffer[offset++] = FDD_RADIO;
  buffer[offset++] = direction;
  buffer[offset++] = rnti_type;

  /* RNTI */
  buffer[offset++] = MAC_LTE_RNTI_TAG;
  tmp16 = htons(rnti);
  memcpy(buffer + offset, &tmp16, 2);
  offset += 2;

  /* UEId */
  buffer[offset++] = MAC_LTE_UEID_TAG;
  tmp16 = htons(ueid);
  memcpy(buffer + offset, &tmp16, 2);
  offset += 2;

  /* Subframe Number and System Frame Number */
  /* SFN is stored in 12 MSB and SF in 4 LSB */
  buffer[offset++] = MAC_LTE_FRAME_SUBFRAME_TAG;
  tmp16 = (((uint16_t)(tti/10)) << 4) | (uint16_t)(tti%10);
  tmp16 = htons(tmp16);
  memcpy(buffer + offset, &tmp16, 2);
  offset += 2;

  /* CRC Status */
  buffer[offset++] = MAC_LTE_CRC_STATUS_TAG;
  buffer[offset++] = crc_ok;

  /* Data tag immediately preceding PDU */
  buffer[offset++] = MAC_LTE_PAYLOAD_TAG;

  /* Write PDU data into buffer */
  if (pdu != NULL) {
    memcpy(buffer + offset, (void*)pdu, pdu_len_bytes);
    offset += pdu_len_bytes;
  }
  // Remote IP 
  bytes_sent = sendto(socket_d, buffer, offset, 0, (const struct sockaddr *) &client_addr, sizeof(client_addr));

  if (bytes_sent != offset) {
    printf("Failed to sent MAC UDP Frame (expected %d bytes, sent %d, (errno %d))\n", offset, bytes_sent, errno);
  }

}

}


