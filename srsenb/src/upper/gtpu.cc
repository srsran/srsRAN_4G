/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2017 Software Radio Systems Limited
 *
 * \section LICENSE
 *
 * This file is part of srsLTE.
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
#include "srslte/upper/gtpu.h"
#include "srsenb/hdr/upper/gtpu.h"
#include <unistd.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <errno.h>

using namespace srslte;
namespace srsenb {

gtpu::gtpu():mchthread()
{
  pdcp          = NULL;
  gtpu_log      = NULL;
  pool          = NULL;

  pthread_mutex_init(&mutex, NULL);

}

bool gtpu::init(std::string gtp_bind_addr_, std::string mme_addr_, std::string m1u_multiaddr_, std::string m1u_if_addr_, srsenb::pdcp_interface_gtpu* pdcp_, srslte::log* gtpu_log_, bool enable_mbsfn)
{
  pdcp          = pdcp_;
  gtpu_log      = gtpu_log_;
  gtp_bind_addr = gtp_bind_addr_;
  mme_addr      = mme_addr_;
  pool          = byte_buffer_pool::get_instance();

  // Set up socket
  fd = socket(AF_INET, SOCK_DGRAM, 0);
  if (fd < 0) {
    gtpu_log->error("Failed to create socket\n");
    return false;
  }
  int enable = 1;
#if defined (SO_REUSEADDR)
  if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
      gtpu_log->error("setsockopt(SO_REUSEADDR) failed\n");
#endif
#if defined (SO_REUSEPORT)
  if (setsockopt(fd, SOL_SOCKET, SO_REUSEPORT, &enable, sizeof(int)) < 0)
      gtpu_log->error("setsockopt(SO_REUSEPORT) failed\n");
#endif

  struct sockaddr_in bindaddr;
  bzero(&bindaddr, sizeof(struct sockaddr_in));
  bindaddr.sin_family      = AF_INET;
  bindaddr.sin_addr.s_addr = inet_addr(gtp_bind_addr.c_str());
  bindaddr.sin_port        = htons(GTPU_PORT);

  if (bind(fd, (struct sockaddr *)&bindaddr, sizeof(struct sockaddr_in))) {
    gtpu_log->error("Failed to bind on address %s, port %d\n", gtp_bind_addr.c_str(), GTPU_PORT);
    gtpu_log->console("Failed to bind on address %s, port %d\n", gtp_bind_addr.c_str(), GTPU_PORT);
    return false;
  }

  // Setup a thread to receive packets from the src socket
  start(THREAD_PRIO);

  // Start MCH thread if enabled
  this->enable_mbsfn = enable_mbsfn;
  if(enable_mbsfn) {
    mchthread.init(m1u_multiaddr_, m1u_if_addr_, pdcp, gtpu_log);
  }
  return true;
}

void gtpu::stop()
{
  if(enable_mbsfn){
    mchthread.stop();
  }

  if (run_enable) {
    run_enable = false;
    // Wait thread to exit gracefully otherwise might leave a mutex locked
    int cnt=0;
    while(running && cnt<100) {
      usleep(10000);
      cnt++;
    }
    if (running) {
      thread_cancel();
    }
    wait_thread_finish();
  }

  if (fd) {
    close(fd);
  }
}

// gtpu_interface_pdcp
void gtpu::write_pdu(uint16_t rnti, uint32_t lcid, srslte::byte_buffer_t* pdu)
{
  gtpu_log->info_hex(pdu->msg, pdu->N_bytes, "TX PDU, RNTI: 0x%x, LCID: %d, n_bytes=%d", rnti, lcid, pdu->N_bytes);
  gtpu_header_t header;
  header.flags        = GTPU_FLAGS_VERSION_V1 | GTPU_FLAGS_GTP_PROTOCOL;
  header.message_type = GTPU_MSG_DATA_PDU;
  header.length       = pdu->N_bytes;
  header.teid         = rnti_bearers[rnti].teids_out[lcid];

  struct sockaddr_in servaddr;
  servaddr.sin_family      = AF_INET;
  servaddr.sin_addr.s_addr = htonl(rnti_bearers[rnti].spgw_addrs[lcid]);
  servaddr.sin_port        = htons(GTPU_PORT);

  if(!gtpu_write_header(&header, pdu, gtpu_log)){
    gtpu_log->error("Error writing GTP-U Header. Flags 0x%x, Message Type 0x%x\n", header.flags, header.message_type);
    return;
  }
  if (sendto(fd, pdu->msg, pdu->N_bytes, MSG_EOR, (struct sockaddr*)&servaddr, sizeof(struct sockaddr_in))<0) {
    perror("sendto");
  }

  pool->deallocate(pdu);
}

/* Warning: This function is called before calling gtpu::init() during MCCH initialization.
 * If access to any element created in init (such as gtpu_log) is required, it must be considered
 * the case of it being NULL.
 */
void gtpu::add_bearer(uint16_t rnti, uint32_t lcid, uint32_t addr, uint32_t teid_out, uint32_t *teid_in)
{
  // Allocate a TEID for the incoming tunnel
  rntilcid_to_teidin(rnti, lcid, teid_in);
  if (gtpu_log) {
    gtpu_log->info("Adding bearer for rnti: 0x%x, lcid: %d, addr: 0x%x, teid_out: 0x%x, teid_in: 0x%x\n", rnti, lcid, addr, teid_out, *teid_in);
  }

  // Initialize maps if it's a new RNTI
  if(rnti_bearers.count(rnti) == 0) {
    for(int i=0;i<SRSENB_N_RADIO_BEARERS;i++) {
      rnti_bearers[rnti].teids_in[i]  = 0;
      rnti_bearers[rnti].teids_out[i] = 0;
      rnti_bearers[rnti].spgw_addrs[i] = 0;
    }
  }

  rnti_bearers[rnti].teids_in[lcid]  = *teid_in;
  rnti_bearers[rnti].teids_out[lcid] = teid_out;
  rnti_bearers[rnti].spgw_addrs[lcid] = addr;
}

void gtpu::rem_bearer(uint16_t rnti, uint32_t lcid)
{
  pthread_mutex_lock(&mutex);
  gtpu_log->info("Removing bearer for rnti: 0x%x, lcid: %d\n", rnti, lcid);

  rnti_bearers[rnti].teids_in[lcid]  = 0;
  rnti_bearers[rnti].teids_out[lcid] = 0;

  // Remove RNTI if all bearers are removed
  bool rem = true;
  for(int i=0;i<SRSENB_N_RADIO_BEARERS; i++) {
    if(rnti_bearers[rnti].teids_in[i] != 0) {
      rem = false;
    }
  }
  if(rem) {
    rnti_bearers.erase(rnti);
  }
  pthread_mutex_unlock(&mutex);
}

void gtpu::rem_user(uint16_t rnti)
{
  pthread_mutex_lock(&mutex);
  rnti_bearers.erase(rnti);
  pthread_mutex_unlock(&mutex);
}

void gtpu::run_thread()
{
  byte_buffer_t *pdu = pool_allocate;

  if (!pdu) {
    gtpu_log->error("Fatal Error: Couldn't allocate buffer in gtpu::run_thread().\n");
    return;
  }
  run_enable = true;

  sockaddr_in client;
  socklen_t   client_len = sizeof(client);
  size_t      buflen = SRSENB_MAX_BUFFER_SIZE_BYTES - SRSENB_BUFFER_HEADER_OFFSET;

  running=true;
  while(run_enable) {

    pdu->reset();
    gtpu_log->debug("Waiting for read...\n");
    int n = 0;
    do{
      n = recvfrom(fd, pdu->msg, buflen, 0, (struct sockaddr *)&client, &client_len);
    } while (n == -1 && errno == EAGAIN);

    if (n < 0) {
        gtpu_log->error("Failed to read from socket\n");
    }

    gtpu_log->debug("Received %d bytes from S1-U interface\n", n);
    pdu->N_bytes = (uint32_t) n;

    gtpu_header_t header;
    if(!gtpu_read_header(pdu, &header,gtpu_log)){
      continue;
    }

    switch(header.message_type) {

      case GTPU_MSG_ECHO_REQUEST:
        // Echo request - send response
        echo_response(client.sin_addr.s_addr, client.sin_port, header.seq_number);
        break;

      case GTPU_MSG_DATA_PDU:

        uint16_t rnti = 0;
        uint16_t lcid = 0;
        teidin_to_rntilcid(header.teid, &rnti, &lcid);

        pthread_mutex_lock(&mutex);
        bool user_exists = (rnti_bearers.count(rnti) > 0);
        pthread_mutex_unlock(&mutex);

        if(!user_exists) {
          gtpu_log->error("Unrecognized RNTI for DL PDU: 0x%x - dropping packet\n", rnti);
          continue;
        }

        if(lcid < SRSENB_N_SRB || lcid >= SRSENB_N_RADIO_BEARERS) {
          gtpu_log->error("Invalid LCID for DL PDU: %d - dropping packet\n", lcid);
          continue;
        }

        gtpu_log->info_hex(pdu->msg, pdu->N_bytes, "RX GTPU PDU rnti=0x%x, lcid=%d, n_bytes=%d", rnti, lcid, pdu->N_bytes);

        pdcp->write_sdu(rnti, lcid, pdu);

        do {
          pdu = pool_allocate;
          if (!pdu) {
            gtpu_log->console("GTPU Buffer pool empty. Trying again...\n");
            usleep(10000);
          }
        } while(!pdu);
        break;
    }
  }
  running = false;
}

void gtpu::echo_response(in_addr_t addr, in_port_t port, uint16_t seq)
{
  gtpu_log->info("TX GTPU Echo Response, Seq: %d\n", seq);

  gtpu_header_t header;
  srslte::byte_buffer_t *pdu = pool_allocate;

  //header
  header.flags = GTPU_FLAGS_VERSION_V1 | GTPU_FLAGS_GTP_PROTOCOL | GTPU_FLAGS_SEQUENCE;
  header.message_type = GTPU_MSG_ECHO_RESPONSE;
  header.teid = 0;
  header.length = 4;
  header.seq_number = seq;
  header.n_pdu = 0;
  header.next_ext_hdr_type = 0;

  gtpu_write_header(&header,pdu,gtpu_log);

  struct sockaddr_in servaddr;
  servaddr.sin_family      = AF_INET;
  servaddr.sin_addr.s_addr = addr;
  servaddr.sin_port        = port;

  sendto(fd, pdu->msg, 12, MSG_EOR, (struct sockaddr*)&servaddr, sizeof(struct sockaddr_in));
  pool->deallocate(pdu);
}

/****************************************************************************
* TEID to RNIT/LCID helper functions
***************************************************************************/
void gtpu::teidin_to_rntilcid(uint32_t teidin, uint16_t *rnti, uint16_t *lcid)
{
  *lcid = teidin & 0xFFFF;
  *rnti = (teidin >> 16) & 0xFFFF;
}

void gtpu::rntilcid_to_teidin(uint16_t rnti, uint16_t lcid, uint32_t *teidin)
{
  *teidin = (rnti << 16) | lcid;
}


/****************************************************************************
* Class to run the MCH thread
***************************************************************************/
bool gtpu::mch_thread::init(std::string m1u_multiaddr_, std::string m1u_if_addr_, pdcp_interface_gtpu *pdcp, srslte::log *gtpu_log)
{
  pool           = byte_buffer_pool::get_instance();
  this->pdcp     = pdcp;
  this->gtpu_log = gtpu_log;
  m1u_multiaddr = m1u_multiaddr_;
  m1u_if_addr   = m1u_if_addr_;

  struct sockaddr_in bindaddr;

  // Set up sink socket
  m1u_sd = socket(AF_INET, SOCK_DGRAM, 0);
  if (m1u_sd < 0) {
    gtpu_log->error("Failed to create M1-U sink socket\n");
    return false;
  }

  /* Bind socket */
  bzero((char *)&bindaddr, sizeof(struct sockaddr_in));
  bindaddr.sin_family = AF_INET;
  bindaddr.sin_addr.s_addr = htonl(INADDR_ANY); //Multicast sockets require bind to INADDR_ANY
  bindaddr.sin_port = htons(GTPU_PORT+1);
  size_t addrlen = sizeof(bindaddr);

  if (bind(m1u_sd, (struct sockaddr *) &bindaddr, sizeof(bindaddr)) < 0) {
    gtpu_log->error("Failed to bind multicast socket\n");
    return false;
  }

  /* Send an ADD MEMBERSHIP message via setsockopt */
  struct ip_mreq mreq;
  mreq.imr_multiaddr.s_addr = inet_addr(m1u_multiaddr.c_str()); //Multicast address of the service
  mreq.imr_interface.s_addr = inet_addr(m1u_if_addr.c_str());           //Address of the IF the socket will listen to.
  if (setsockopt(m1u_sd, IPPROTO_IP, IP_ADD_MEMBERSHIP,
                 &mreq, sizeof(mreq)) < 0) {
    gtpu_log->error("Register musticast group for M1-U\n");
    gtpu_log->error("M1-U infterface IP: %s, M1-U Multicast Address %s\n", m1u_if_addr.c_str(),m1u_multiaddr.c_str());
    return false;
  }
  gtpu_log->info("M1-U initialized\n");

  initiated = true;
  lcid_counter = 1;

  // Start thread
  start(MCH_THREAD_PRIO);
  return true;
}

void gtpu::mch_thread::run_thread()
{
  if (!initiated) {
    fprintf(stderr, "Fatal error running mch_thread without initialization\n");
    return;
  }

  byte_buffer_t *pdu;
  int n;
  socklen_t addrlen;
  sockaddr_in src_addr;

  bzero((char *)&src_addr, sizeof(src_addr));
  src_addr.sin_family = AF_INET;
  src_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  src_addr.sin_port = htons(GTPU_PORT+1);
  addrlen = sizeof(src_addr);

  run_enable = true;
  running=true;

  pdu = pool->allocate();

  // Warning: Use mutex here if creating multiple services each with a different thread
  uint16_t lcid = lcid_counter;
  lcid_counter++;

  while(run_enable) {

    pdu->reset();
    do{
      n =  recvfrom(m1u_sd, pdu->msg, SRSENB_MAX_BUFFER_SIZE_BYTES - SRSENB_BUFFER_HEADER_OFFSET, 0, (struct sockaddr *) &src_addr, &addrlen);
    } while (n == -1 && errno == EAGAIN);
    gtpu_log->debug("Received %d bytes from M1-U interface\n", n);

    pdu->N_bytes = (uint32_t) n;

    gtpu_header_t header;
    gtpu_read_header(pdu, &header, gtpu_log);
    pdcp->write_sdu(SRSLTE_MRNTI, lcid, pdu);
    do {
      pdu = pool_allocate;
      if (!pdu) {
        gtpu_log->console("GTPU Buffer pool empty. Trying again...\n");
        usleep(10000);
      }
    } while(!pdu);
  }
  running = false;
}

void gtpu::mch_thread::stop()
{
  if (run_enable) {
    run_enable = false;
    // Wait thread to exit gracefully otherwise might leave a mutex locked
    int cnt = 0;
    while(running && cnt < 100) {
      usleep(10000);
      cnt++;
    }
    if (running) {
      thread_cancel();
    }
    wait_thread_finish();
  } 
}

} // namespace srsenb
