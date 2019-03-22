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

#include "srsepc/hdr/spgw/spgw.h"
#include "srsepc/hdr/mme/mme_gtpc.h"
#include "srsepc/hdr/spgw/gtpc.h"
#include "srsepc/hdr/spgw/gtpu.h"
#include "srslte/upper/gtpu.h"
#include <inttypes.h> // for printing uint64_t

namespace srsepc {

spgw*           spgw::m_instance    = NULL;
pthread_mutex_t spgw_instance_mutex = PTHREAD_MUTEX_INITIALIZER;

spgw::spgw() : m_running(false)
{
  m_gtpc = new spgw::gtpc;
  m_gtpu = new spgw::gtpu;
  return;
}

spgw::~spgw()
{
  delete m_gtpc;
  delete m_gtpu;
  return;
}

spgw* spgw::get_instance()
{
  pthread_mutex_lock(&spgw_instance_mutex);
  if (NULL == m_instance) {
    m_instance = new spgw();
  }
  pthread_mutex_unlock(&spgw_instance_mutex);
  return (m_instance);
}

void spgw::cleanup()
{
  pthread_mutex_lock(&spgw_instance_mutex);
  if (NULL != m_instance) {
    delete m_instance;
    m_instance = NULL;
  }
  pthread_mutex_unlock(&spgw_instance_mutex);
}

int spgw::init(spgw_args_t*        args,
               srslte::log_filter* gtpu_log,
               srslte::log_filter* gtpc_log,
               srslte::log_filter* spgw_log)
{
  srslte::error_t err;
  m_pool = srslte::byte_buffer_pool::get_instance();

  // Init log
  m_spgw_log = spgw_log;
  m_mme_gtpc = mme_gtpc::get_instance();

  // Init GTP-U
  if (m_gtpu->init(args, this, gtpu_log) != 0) {
    m_spgw_log->console("Could not initialize the SPGW's GTP-U.\n");
    return -1;
  }

  // Init GTP-C
  if (m_gtpc->init(args, this, m_gtpu, gtpc_log) != 0) {
    m_spgw_log->console("Could not initialize the S1-U interface.\n");
    return -1;
  }

  m_spgw_log->info("SP-GW Initialized.\n");
  m_spgw_log->console("SP-GW Initialized.\n");
  return 0;
}

void spgw::stop()
{
  if (m_running) {
    m_running = false;
    thread_cancel();
    wait_thread_finish();
  }

  m_gtpu->stop();
  m_gtpc->stop();
  return;
}

void spgw::run_thread()
{
  // Mark the thread as running
  m_running = true;
  srslte::byte_buffer_t* msg;
  msg = m_pool->allocate();

  struct sockaddr_in src_addr_in;
  struct sockaddr_un src_addr_un;
  socklen_t       addrlen;
  struct iphdr*   ip_pkt;

  int sgi = m_gtpu->get_sgi();
  int s1u = m_gtpu->get_s1u();
  int s11 = m_gtpc->get_s11();

  size_t buf_len = SRSLTE_MAX_BUFFER_SIZE_BYTES - SRSLTE_BUFFER_HEADER_OFFSET;

  fd_set set;
  int max_fd = std::max(s1u, sgi);
  max_fd = std::max(max_fd, s11);
  while (m_running) {
    msg->reset();
    FD_ZERO(&set);
    FD_SET(s1u, &set);
    FD_SET(sgi, &set);
    FD_SET(s11, &set);

    int n = select(max_fd + 1, &set, NULL, NULL, NULL);
    if (n == -1) {
      m_spgw_log->error("Error from select\n");
    } else if (n) {
      if (FD_ISSET(s1u, &set)) {
        msg->N_bytes = recvfrom(s1u, msg->msg, buf_len, 0, (struct sockaddr*)&src_addr_in, &addrlen);
        m_gtpu->handle_s1u_pdu(msg);
      }
      if (FD_ISSET(sgi, &set)) {
        msg->N_bytes = read(sgi, msg->msg, buf_len);
        m_gtpu->handle_sgi_pdu(msg);
      }
      if (FD_ISSET(s11, &set)) {
        m_spgw_log->debug("Message received at SPGW: S11 Message\n");
        msg->N_bytes = recvfrom(s11, msg->msg, buf_len, 0, (struct sockaddr*)&src_addr_un, &addrlen);
        m_gtpc->handle_s11_pdu(msg);
      }
    } else {
      m_spgw_log->debug("No data from select.\n");
    }
  }
  m_pool->deallocate(msg);
  return;
}

} // namespace srsepc
