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

#include "srsepc/hdr/mme/mme.h"
#include <arpa/inet.h>
#include <inttypes.h> // for printing uint64_t
#include <netinet/sctp.h>
#include <sys/socket.h>
#include <sys/types.h>

namespace srsepc {

mme*            mme::m_instance    = NULL;
pthread_mutex_t mme_instance_mutex = PTHREAD_MUTEX_INITIALIZER;

mme::mme() : m_running(false), thread("MME")
{
  m_pool = srslte::byte_buffer_pool::get_instance();
  return;
}

mme::~mme()
{
  return;
}

mme* mme::get_instance(void)
{
  pthread_mutex_lock(&mme_instance_mutex);
  if (NULL == m_instance) {
    m_instance = new mme();
  }
  pthread_mutex_unlock(&mme_instance_mutex);
  return (m_instance);
}

void mme::cleanup(void)
{
  pthread_mutex_lock(&mme_instance_mutex);
  if (NULL != m_instance) {
    delete m_instance;
    m_instance = NULL;
  }
  pthread_mutex_unlock(&mme_instance_mutex);
}

int mme::init(mme_args_t*         args,
              srslte::log_filter* nas_log,
              srslte::log_filter* s1ap_log,
              srslte::log_filter* mme_gtpc_log)
{
  /*Init logger*/
  m_nas_log      = nas_log;
  m_s1ap_log     = s1ap_log;
  m_mme_gtpc_log = mme_gtpc_log;

  /*Init S1AP*/
  m_s1ap = s1ap::get_instance();
  if (m_s1ap->init(args->s1ap_args, nas_log, s1ap_log)) {
    m_s1ap_log->error("Error initializing MME S1APP\n");
    exit(-1);
  }

  /*Init GTP-C*/
  m_mme_gtpc = mme_gtpc::get_instance();
  if (!m_mme_gtpc->init(m_mme_gtpc_log)) {
    m_s1ap_log->console("Error initializing GTP-C\n");
    exit(-1);
  }

  /*Log successful initialization*/
  m_s1ap_log->info("MME Initialized. MCC: 0x%x, MNC: 0x%x\n", args->s1ap_args.mcc, args->s1ap_args.mnc);
  m_s1ap_log->console("MME Initialized. MCC: 0x%x, MNC: 0x%x\n", args->s1ap_args.mcc, args->s1ap_args.mnc);
  return 0;
}

void mme::stop()
{
  if (m_running) {
    m_s1ap->stop();
    m_s1ap->cleanup();
    m_running = false;
    thread_cancel();
    wait_thread_finish();
  }
  return;
}

void mme::run_thread()
{
  srslte::byte_buffer_t* pdu = m_pool->allocate("mme::run_thread");
  uint32_t               sz  = SRSLTE_MAX_BUFFER_SIZE_BYTES - SRSLTE_BUFFER_HEADER_OFFSET;

  struct sockaddr_in     enb_addr;
  struct sctp_sndrcvinfo sri;
  socklen_t              fromlen = sizeof(enb_addr);
  bzero(&enb_addr, sizeof(enb_addr));
  int rd_sz;
  int msg_flags = 0;

  // Mark the thread as running
  m_running = true;

  // Get S1-MME and S11 sockets
  int s1mme = m_s1ap->get_s1_mme();
  int s11   = m_mme_gtpc->get_s11();

  while (m_running) {
    pdu->clear();
    int max_fd = std::max(s1mme, s11);

    FD_ZERO(&m_set);
    FD_SET(s1mme, &m_set);
    FD_SET(s11, &m_set);

    // Add timers to select
    for (std::vector<mme_timer_t>::iterator it = timers.begin(); it != timers.end(); ++it) {
      FD_SET(it->fd, &m_set);
      max_fd = std::max(max_fd, it->fd);
      m_s1ap_log->debug("Adding Timer fd %d to fd_set\n", it->fd);
    }

    m_s1ap_log->debug("Waiting for S1-MME or S11 Message\n");
    int n = select(max_fd + 1, &m_set, NULL, NULL, NULL);
    if (n == -1) {
      m_s1ap_log->error("Error from select\n");
    } else if (n) {
      // Handle S1-MME
      if (FD_ISSET(s1mme, &m_set)) {
        rd_sz = sctp_recvmsg(s1mme, pdu->msg, sz, (struct sockaddr*)&enb_addr, &fromlen, &sri, &msg_flags);
        if (rd_sz == -1 && errno != EAGAIN) {
          m_s1ap_log->error("Error reading from SCTP socket: %s", strerror(errno));
        } else if (rd_sz == -1 && errno == EAGAIN) {
          m_s1ap_log->debug("Socket timeout reached");
        } else {
          if (msg_flags & MSG_NOTIFICATION) {
            // Received notification
            union sctp_notification* notification = (union sctp_notification*)pdu->msg;
            m_s1ap_log->debug("SCTP Notification %d\n", notification->sn_header.sn_type);
            if (notification->sn_header.sn_type == SCTP_SHUTDOWN_EVENT) {
              m_s1ap_log->info("SCTP Association Shutdown. Association: %d\n", sri.sinfo_assoc_id);
              m_s1ap_log->console("SCTP Association Shutdown. Association: %d\n", sri.sinfo_assoc_id);
              m_s1ap->delete_enb_ctx(sri.sinfo_assoc_id);
            }
          } else {
            // Received data
            pdu->N_bytes = rd_sz;
            m_s1ap_log->info("Received S1AP msg. Size: %d\n", pdu->N_bytes);
            m_s1ap->handle_s1ap_rx_pdu(pdu, &sri);
          }
        }
      }
      // Handle S11
      if (FD_ISSET(s11, &m_set)) {
        pdu->N_bytes = recvfrom(s11, pdu->msg, SRSLTE_MAX_BUFFER_SIZE_BYTES, 0, NULL, NULL);
        m_mme_gtpc->handle_s11_pdu(pdu);
      }
      // Handle NAS Timers
      for (std::vector<mme_timer_t>::iterator it = timers.begin(); it != timers.end();) {
        if (FD_ISSET(it->fd, &m_set)) {
          m_s1ap_log->info("Timer expired\n");
          uint64_t exp;
          rd_sz = read(it->fd, &exp, sizeof(uint64_t));
          m_s1ap->expire_nas_timer(it->type, it->imsi);
          close(it->fd);
          timers.erase(it);
        } else {
          ++it;
        }
      }
    } else {
      m_s1ap_log->debug("No data from select.\n");
    }
  }
  return;
}

/*
 * Timer Handling
 */
bool mme::add_nas_timer(int timer_fd, nas_timer_type type, uint64_t imsi)
{
  m_s1ap_log->debug("Adding NAS timer to MME. IMSI %" PRIu64 ", Type %d, Fd: %d\n", imsi, type, timer_fd);

  mme_timer_t timer;
  timer.fd   = timer_fd;
  timer.type = type;
  timer.imsi = imsi;

  timers.push_back(timer);
  return true;
}

bool mme::is_nas_timer_running(nas_timer_type type, uint64_t imsi)
{
  std::vector<mme_timer_t>::iterator it;
  for (it = timers.begin(); it != timers.end(); ++it) {
    if (it->type == type && it->imsi == imsi) {
      return true; // found timer
    }
  }
  return false;
}

bool mme::remove_nas_timer(nas_timer_type type, uint64_t imsi)
{
  std::vector<mme_timer_t>::iterator it;
  for (it = timers.begin(); it != timers.end(); ++it) {
    if (it->type == type && it->imsi == imsi) {
      break; // found timer to remove
    }
  }
  if (it == timers.end()) {
    m_s1ap_log->warning("Could not find timer to remove. IMSI %" PRIu64 ", Type %d\n", imsi, type);
    return false;
  }

  // removing timer
  m_s1ap_log->debug("Removing NAS timer from MME. IMSI %" PRIu64 ", Type %d, Fd: %d\n", imsi, type, it->fd);
  FD_CLR(it->fd, &m_set);
  close(it->fd);
  timers.erase(it);
  return true;
}

} // namespace srsepc
