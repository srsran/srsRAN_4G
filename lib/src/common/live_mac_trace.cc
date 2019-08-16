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
  
live_mac_trace::live_mac_trace() : thread("MAC_LIVE_THREAD")
{
  pool = byte_buffer_pool::get_instance();
}


void live_mac_trace::init(const char * server_ip_addr_, uint16_t server_udp_port_, const char * client_ip_addr_, uint16_t client_udp_port_){
  ue_id = 0;
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
  mac_trace_pdu_queue.clear();
  start();
}



void live_mac_trace::run_thread(){
  running = true;
 
  while(running){
    mac_trace_pdu_t mac_trace_pdu = mac_trace_pdu_queue.wait_pop();
    if(mac_trace_pdu.pdu != NULL){
      send_mac_datagram(mac_trace_pdu.pdu->msg, mac_trace_pdu.pdu->N_bytes, &mac_trace_pdu.context);
      pool->deallocate(mac_trace_pdu.pdu);
    }
  }
}

void live_mac_trace::set_ue_id(uint16_t ue_id) {
  this->ue_id = ue_id;
}


void live_mac_trace::stop(){
  running = false;
  mac_trace_pdu_t mac_trace_pdu;
  mac_trace_pdu.pdu = NULL;
  mac_trace_pdu_queue.push(mac_trace_pdu);
  sleep(0.01); // not sexy 
  wait_thread_finish();
  close(socket_d);
}

void live_mac_trace::write_dl_crnti(uint8_t* pdu, uint32_t pdu_len_bytes, uint16_t rnti, bool crc_ok, uint32_t tti)
{
  pack_and_queue(pdu, pdu_len_bytes, 0, crc_ok, tti, rnti, DIRECTION_DOWNLINK, C_RNTI);
}
void live_mac_trace::write_dl_ranti(uint8_t* pdu, uint32_t pdu_len_bytes, uint16_t rnti, bool crc_ok, uint32_t tti)
{
  pack_and_queue(pdu, pdu_len_bytes, 0, crc_ok, tti, rnti, DIRECTION_DOWNLINK, RA_RNTI);
}
void live_mac_trace::write_ul_crnti(uint8_t* pdu, uint32_t pdu_len_bytes, uint16_t rnti, uint32_t reTX, uint32_t tti)
{
  pack_and_queue(pdu, pdu_len_bytes, reTX, true, tti, rnti, DIRECTION_UPLINK, C_RNTI);
}
void live_mac_trace::write_dl_bch(uint8_t* pdu, uint32_t pdu_len_bytes, bool crc_ok, uint32_t tti)
{
  pack_and_queue(pdu, pdu_len_bytes, 0, crc_ok, tti, 0, DIRECTION_DOWNLINK, NO_RNTI);
}
void live_mac_trace::write_dl_pch(uint8_t* pdu, uint32_t pdu_len_bytes, bool crc_ok, uint32_t tti)
{
  pack_and_queue(pdu, pdu_len_bytes, 0, crc_ok, tti, SRSLTE_PRNTI, DIRECTION_DOWNLINK, P_RNTI);
}
void live_mac_trace::write_dl_mch(uint8_t* pdu, uint32_t pdu_len_bytes, bool crc_ok, uint32_t tti)
{
  pack_and_queue(pdu, pdu_len_bytes, 0, crc_ok, tti, SRSLTE_MRNTI, DIRECTION_DOWNLINK, M_RNTI);
}
void live_mac_trace::write_dl_sirnti(uint8_t* pdu, uint32_t pdu_len_bytes, bool crc_ok, uint32_t tti)
{
  pack_and_queue(pdu, pdu_len_bytes, 0, crc_ok, tti, SRSLTE_SIRNTI, DIRECTION_DOWNLINK, SI_RNTI);
}

void live_mac_trace::pack_and_queue(uint8_t* pdu, uint32_t pdu_len_bytes, uint32_t reTX, bool crc_ok, uint32_t tti,
                              uint16_t rnti, uint8_t direction, uint8_t rnti_type){
  byte_buffer_t*  udp_pdu;
  mac_trace_pdu_t mac_trace_pdu;
  
  udp_pdu = pool->allocate();

  if(pdu != NULL){
    memcpy(udp_pdu->msg, pdu, pdu_len_bytes);
  }
 
  udp_pdu->N_bytes = pdu_len_bytes;

  mac_trace_pdu.pdu = udp_pdu;

  mac_trace_pdu.context.radioType = FDD_RADIO;
  mac_trace_pdu.context.direction = direction;
  mac_trace_pdu.context.rntiType = rnti_type;
  mac_trace_pdu.context.rnti = rnti;
  mac_trace_pdu.context.ueid = ue_id;
  mac_trace_pdu.context.isRetx = reTX;
  mac_trace_pdu.context.crcStatusOK = crc_ok;
  mac_trace_pdu.context.sysFrameNumber = (uint16_t)(tti/10);
  mac_trace_pdu.context.subFrameNumber = (uint16_t)(tti%10);
  mac_trace_pdu_queue.push(mac_trace_pdu);
}

void live_mac_trace::send_mac_datagram(uint8_t* pdu, uint32_t pdu_len_bytes, MAC_Context_Info_t *context){

  ssize_t bytes_sent;
  uint32_t offset = 0;
  uint16_t tmp16;
  
  uint32_t total_size = pdu_len_bytes + sizeof(MAC_Context_Info_t) + sizeof(MAC_LTE_START_STRING);
  
  if( SRSLTE_MAX_BUFFER_SIZE_BYTES <= total_size){
    printf("Send length does exceed max buffer length\n");
    return;
  }

  // MAC_LTE_START_STRING for UDP heuristics 
  memcpy(udp_datagram.msg + offset, MAC_LTE_START_STRING, strlen(MAC_LTE_START_STRING));
  offset += strlen(MAC_LTE_START_STRING);
  
  /*****************************************************************/
  /* Context information (same as written by UDP heuristic clients */
  udp_datagram.msg[offset++] = context->radioType;
  udp_datagram.msg[offset++] = context->direction;
  udp_datagram.msg[offset++] = context->rntiType;

  /* RNTI */
  udp_datagram.msg[offset++] = MAC_LTE_RNTI_TAG;
  tmp16 = htons(context->rnti);
  memcpy(udp_datagram.msg + offset, &tmp16, 2);
  offset += 2;

  /* UEId */
  udp_datagram.msg[offset++] = MAC_LTE_UEID_TAG;
  tmp16 = htons(context->ueid);
  memcpy(udp_datagram.msg + offset, &tmp16, 2);
  offset += 2;

  /* Subframe Number and System Frame Number */
  /* SFN is stored in 12 MSB and SF in 4 LSB */

  udp_datagram.msg[offset++] = MAC_LTE_FRAME_SUBFRAME_TAG;
  tmp16 = (context->sysFrameNumber << 4) | context->subFrameNumber;
  tmp16 = htons(tmp16);
  memcpy(udp_datagram.msg + offset, &tmp16, 2);
  offset += 2;

  /* CRC Status */
  udp_datagram.msg[offset++] = MAC_LTE_CRC_STATUS_TAG;
  udp_datagram.msg[offset++] = context->crcStatusOK;

  /* Data tag immediately preceding PDU */
  udp_datagram.msg[offset++] = MAC_LTE_PAYLOAD_TAG;

  /* Write PDU data into buffer */
  if (pdu != NULL) {
    memcpy(udp_datagram.msg + offset, (void*)pdu, pdu_len_bytes);
    offset += pdu_len_bytes;
  }
  // Remote IP 
  // TODO: sendto thread safe with mutex
  bytes_sent = sendto(socket_d, udp_datagram.msg, offset, 0, (const struct sockaddr *) &client_addr, sizeof(client_addr));

  if (bytes_sent != offset) {
    printf("Failed to sent MAC UDP Frame (expected %d bytes, sent %zu, (errno %d))\n", offset, bytes_sent, errno);
  }
  bzero(udp_datagram.msg, offset);
  udp_datagram.clear();
}

} //namespace