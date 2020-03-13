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

/*! \brief This class provides a common function for all TTCN3
 *  ports for SCTP socket creation, notification handling, etc.
 *
 */

#ifndef SRSUE_TTCN3_PORT_HANDLER_H
#define SRSUE_TTCN3_PORT_HANDLER_H

#include "srslte/common/epoll_helper.h"
#include "srslte/common/log.h"
#include "ttcn3_common.h"
#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/sctp.h>

class ttcn3_port_handler : public epoll_handler
{
public:
  ttcn3_port_handler() : rx_buf(unique_byte_array_t(new byte_array_t)) {}
  virtual ~ttcn3_port_handler() {}

  virtual int handle_message(const unique_byte_array_t& rx_buf, const uint32_t n) = 0;

  int handle_event(int fd, epoll_event e, int epoll_fd)
  {
    if (e.events & EPOLLIN) {
      struct sctp_sndrcvinfo sri       = {};
      socklen_t              fromlen   = sizeof(client_addr);
      int                    msg_flags = 0;
      int                    rd_sz =
          sctp_recvmsg(fd, rx_buf->begin(), RX_BUF_SIZE, (struct sockaddr*)&client_addr, &fromlen, &sri, &msg_flags);
      if (rd_sz == -1 && errno != EAGAIN) {
        log->error("Error reading from SCTP socket: %s", strerror(errno));
      } else if (rd_sz == -1 && errno == EAGAIN) {
        log->debug("Socket timeout reached");
      } else {
        if (msg_flags & MSG_NOTIFICATION) {
          // Received notification
          handle_notification(rx_buf->begin(), rd_sz);
        } else {
          // Received data
          rx_buf->at(rd_sz) = '\0'; // Terminate buffer
          handle_message(rx_buf, rd_sz);
        }
      }
    }
    return SRSLTE_SUCCESS;
  }

  int handle_notification(const uint8_t* payload, const uint32_t len)
  {
    union sctp_notification* notif             = (union sctp_notification*)payload;
    uint32_t                 notif_header_size = sizeof(((union sctp_notification*)NULL)->sn_header);
    if (notif_header_size > len) {
      printf("Error: Notification msg size is smaller than notification header size!\n");
      return SRSLTE_ERROR;
    }

    switch (notif->sn_header.sn_type) {
      case SCTP_ASSOC_CHANGE: {
        if (sizeof(struct sctp_assoc_change) > len) {
          printf("Error notification msg size is smaller than struct sctp_assoc_change size\n");
          return SRSLTE_ERROR;
        }

        const char*               state = NULL;
        struct sctp_assoc_change* n     = &notif->sn_assoc_change;

        switch (n->sac_state) {
          case SCTP_COMM_UP:
            state = "COMM UP";
            break;
          case SCTP_COMM_LOST:
            state = "COMM_LOST";
            break;
          case SCTP_RESTART:
            state = "RESTART";
            break;
          case SCTP_SHUTDOWN_COMP:
            state = "SHUTDOWN_COMP";
            break;
          case SCTP_CANT_STR_ASSOC:
            state = "CAN'T START ASSOC";
            break;
        }

        log->debug(
            "SCTP_ASSOC_CHANGE notif: state: %s, error code: %d, out streams: %d, in streams: %d, assoc id: %d\n",
            state,
            n->sac_error,
            n->sac_outbound_streams,
            n->sac_inbound_streams,
            n->sac_assoc_id);
        break;
      }

      case SCTP_SHUTDOWN_EVENT: {
        if (sizeof(struct sctp_shutdown_event) > len) {
          printf("Error notification msg size is smaller than struct sctp_assoc_change size\n");
          return SRSLTE_ERROR;
        }
        struct sctp_shutdown_event* n = &notif->sn_shutdown_event;
        log->debug("SCTP_SHUTDOWN_EVENT notif: assoc id: %d\n", n->sse_assoc_id);
        break;
      }

      default:
        log->warning("Unhandled notification type %d\n", notif->sn_header.sn_type);
        break;
    }

    return SRSLTE_SUCCESS;
  }

  ///< Send buffer to tester
  void send(const uint8_t* buffer, const uint32_t len)
  {
    if (sendto(sock_fd, buffer, len, 0, (struct sockaddr*)&client_addr, sizeof(client_addr)) == -1) {
      log->error("Error sending message to tester.\n");
    }
  }

  ///< Set socket to non-blocking-mode
  int set_non_blocking(uint32_t fd)
  {
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1) {
      perror("fcntl");
      return SRSLTE_ERROR;
    }

    flags |= O_NONBLOCK;
    int s = fcntl(fd, F_SETFL, flags);
    if (s == -1) {
      perror("fcntl");
      return SRSLTE_ERROR;
    }

    return SRSLTE_SUCCESS;
  }

  ///< Create, bind and listen on SCTP socket
  int port_listen()
  {
    int ret = SRSLTE_ERROR;

    sock_fd = socket(AF_INET, SOCK_SEQPACKET, IPPROTO_SCTP);
    if (sock_fd == -1) {
      log->console("Could not create SCTP socket\n");
      return ret;
    }

    // Sets the data_io_event to be able to use sendrecv_info
    // Subscribes to the SCTP_SHUTDOWN event, to handle graceful shutdown
    struct sctp_event_subscribe events = {};
    events.sctp_data_io_event          = 1;
    events.sctp_shutdown_event         = 1;
    events.sctp_association_event      = 1;
    if (setsockopt(sock_fd, IPPROTO_SCTP, SCTP_EVENTS, &events, sizeof(events))) {
      close(sock_fd);
      log->console("Subscribing to sctp_data_io_events failed\n");
      return SRSLTE_ERROR;
    }

    // Port bind
    struct sockaddr_in bind_addr = {};
    bind_addr.sin_family         = AF_INET;
    inet_pton(AF_INET, net_ip.c_str(), &(bind_addr.sin_addr));
    bind_addr.sin_port = htons(net_port);

    int one = 1;
    setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));
    ret = bind(sock_fd, (struct sockaddr*)&bind_addr, sizeof(bind_addr));
    if (ret != 0) {
      close(sock_fd);
      log->error("Error binding SCTP socket\n");
      log->console("Error binding SCTP socket\n");
      return SRSLTE_ERROR;
    }

    // Listen for connections
    ret = listen(sock_fd, SOMAXCONN);
    if (ret != SRSLTE_SUCCESS) {
      close(sock_fd);
      log->error("Error in SCTP socket listen\n");
      log->console("Error in SCTP socket listen\n");
      return SRSLTE_ERROR;
    }

    set_non_blocking(sock_fd);
    return sock_fd;
  }

  bool                initialized = false;
  std::string         net_ip      = "0.0.0.0";
  uint32_t            net_port    = 0;
  int                 sock_fd     = -1;
  struct sockaddr     client_addr = {};
  srslte::log*        log         = nullptr;
  unique_byte_array_t rx_buf; ///< Receive buffer for this port
};

#endif // SRSUE_TTCN3_PORT_HANDLER_H
