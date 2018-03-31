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

#ifndef SRSUE_GW_H
#define SRSUE_GW_H

#include "srslte/common/buffer_pool.h"
#include "srslte/common/log.h"
#include "srslte/common/common.h"
#include "srslte/common/msg_queue.h"
#include "srslte/common/interfaces_common.h"
#include "srslte/interfaces/ue_interfaces.h"
#include "srslte/common/threads.h"
#include "gw_metrics.h"

#include <linux/if.h>

namespace srsue {

class gw
    :public gw_interface_pdcp
    ,public gw_interface_nas
    ,public thread
{
public:
  gw();
  void init(pdcp_interface_gw *pdcp_, nas_interface_gw *nas_, srslte::log *gw_log_, srslte::srslte_gw_config_t);
  void stop();

  void get_metrics(gw_metrics_t &m);

  // PDCP interface
  void write_pdu(uint32_t lcid, srslte::byte_buffer_t *pdu);

  // NAS interface
  srslte::error_t setup_if_addr(uint32_t ip_addr, char *err_str);

  void set_netmask(std::string netmask);

private:

  bool default_netmask;
  std::string netmask;

  static const int GW_THREAD_PRIO = 7;

  pdcp_interface_gw  *pdcp;
  nas_interface_gw   *nas;

  srslte::byte_buffer_pool   *pool;

  srslte::log                *gw_log;
  srslte::srslte_gw_config_t cfg;
  bool                running;
  bool                run_enable;
  int32               tun_fd;
  struct ifreq        ifr;
  int32               sock;
  bool                if_up;

  uint32_t            current_ip_addr;

  long                ul_tput_bytes;
  long                dl_tput_bytes;
  struct timeval      metrics_time[3];

  void                run_thread();
  srslte::error_t     init_if(char *err_str);
};

} // namespace srsue


#endif // SRSUE_GW_H
