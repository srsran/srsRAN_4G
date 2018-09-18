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

#include <stdio.h>
#include <assert.h>

#include "srslte/common/live_mac_trace.h"
#include "srslte/common/threads.h"


using namespace srslte;


uint8_t pdu[] = {0x23, 0x38, 0x1f, 0xa0, 0x45, 0x80, 0x74, 0x21, 0xb1, 0xc4, 0xd4, 0x1f, 0x25, 0x02, 0xd8, 0x6a,
0xfe, 0xfc, 0x1b, 0x4f, 0x01, 0xfc, 0x24, 0xb7, 0xce, 0x59, 0xc4, 0xde, 0xa7, 0x52, 0x07, 0x00,
0x41, 0x75, 0x29, 0xc0, 0x73, 0x0c, 0x57, 0x9f, 0x44, 0x1d, 0x27, 0xcb, 0xb3, 0xd1, 0xf6, 0xb7,
0x7e, 0x4f, 0x11, 0xfa, 0x53, 0x9c, 0xeb, 0xec, 0x1e, 0x9a, 0x2b, 0x4a, 0x87, 0xeb, 0x7b, 0x07,
0x46, 0x1e, 0x21};

#define EXPECTED_UDP_SIZE 89

class test_thread : public thread
{
public:
  test_thread(): thread("TEST_THREAD"){
  }

  void init(char * client_ip_addr_, uint16_t client_udp_port_){
    // Creating socket file descriptor 
    if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) { 
      perror("socket creation failed"); 
      return;
    } 

    client_addr.sin_family = AF_INET;
    client_addr.sin_addr.s_addr = inet_addr(client_ip_addr_);
    client_addr.sin_port = htons(client_udp_port_);

    int ret = bind(sockfd, (struct sockaddr*) &client_addr, sizeof(client_addr));

    if (ret != 0) {
      printf("Failed to bind socket to (%s:%u)\n",
            inet_ntoa(client_addr.sin_addr),
            ntohs(client_addr.sin_port));
      close(sockfd);
      return;
    }
   
    running = true;
    start();
  }

  void run_thread(){;
    int n, len; 
    socklen_t socklen = sizeof(client_addr);
    while(running){
      n = recvfrom(sockfd, (char *)buffer, 1024,  
                  0, (struct sockaddr *) &client_addr, 
                  &socklen);
      assert(EXPECTED_UDP_SIZE == n);
      // TODO: Check buffer...
    }
    return; 
  }

  void stop(){
    running = false;
    close(sockfd); 
  }

private:
    int sockfd; 
    char buffer[1024]; 
    struct sockaddr_in     client_addr; 
    bool running;
};


int main(int argc, char **argv) {
  char server_ip_addr[] = "127.0.0.1";
  uint16_t server_udp_port = 5687;
  char client_ip_addr[] = "127.0.0.1";
  uint16_t client_udp_port = 5847;
  live_mac_trace mac_trace;
  mac_trace.init(server_ip_addr, server_udp_port, client_ip_addr, client_udp_port);
  test_thread test_thread_a;
  test_thread_a.init(client_ip_addr, client_udp_port);
  mac_trace.write_dl_crnti(pdu, sizeof(pdu), 60128, true, 2133);
  sleep(0.05);
  test_thread_a.stop();
  mac_trace.stop();
}