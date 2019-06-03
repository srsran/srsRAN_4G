/*
 * Copyright 2013-2019 Software Radio Systems Limited
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

#include "srsue/hdr/stack/ue_stack_lte.h"
#include "srslte/srslte.h"

using namespace srslte;

namespace srsue {

ue_stack_lte::ue_stack_lte() : running(false), args(), logger(nullptr), usim(nullptr), phy(nullptr) {}

ue_stack_lte::~ue_stack_lte()
{
  stop();
}

std::string ue_stack_lte::get_type()
{
  return "lte";
}

int ue_stack_lte::init(const stack_args_t& args_, srslte::logger* logger_, phy_interface_stack_lte* phy_)
{
  phy = phy_;

  if (init(args_, logger_)) {
    return SRSLTE_ERROR;
  }

  return SRSLTE_SUCCESS;
}

int ue_stack_lte::init(const stack_args_t& args_, srslte::logger* logger_)
{
  args   = args_;
  logger = logger_;

  // setup logging for each layer
  mac_log.init("MAC ", logger, true);
  rlc_log.init("RLC ", logger);
  pdcp_log.init("PDCP", logger);
  rrc_log.init("RRC ", logger);
  nas_log.init("NAS ", logger);
  gw_log.init("GW  ", logger);
  usim_log.init("USIM", logger);

  pool_log.init("POOL", logger);
  pool_log.set_level(srslte::LOG_LEVEL_ERROR);
  byte_buffer_pool::get_instance()->set_log(&pool_log);

  mac_log.set_level(args.log.mac_level);
  rlc_log.set_level(args.log.rlc_level);
  pdcp_log.set_level(args.log.pdcp_level);
  rrc_log.set_level(args.log.rrc_level);
  nas_log.set_level(args.log.nas_level);
  gw_log.set_level(args.log.gw_level);
  usim_log.set_level(args.log.usim_level);

  mac_log.set_hex_limit(args.log.mac_hex_limit);
  rlc_log.set_hex_limit(args.log.rlc_hex_limit);
  pdcp_log.set_hex_limit(args.log.pdcp_hex_limit);
  rrc_log.set_hex_limit(args.log.rrc_hex_limit);
  nas_log.set_hex_limit(args.log.nas_hex_limit);
  gw_log.set_hex_limit(args.log.gw_hex_limit);
  usim_log.set_hex_limit(args.log.usim_hex_limit);

  // Set up pcap
  if (args.pcap.enable) {
    mac_pcap.open(args.pcap.filename.c_str());
    mac.start_pcap(&mac_pcap);
  }
  if (args.pcap.nas_enable) {
    nas_pcap.open(args.pcap.nas_filename.c_str());
    nas.start_pcap(&nas_pcap);
  }

  // Init USIM first to allow early exit in case reader couldn't be found
  usim = usim_base::get_instance(&args.usim);
  if (usim->init(&args.usim, &usim_log)) {
    usim_log.console("Failed to initialize USIM.\n");
    return SRSLTE_ERROR;
  }

  mac.init(phy, &rlc, &rrc, &mac_log);
  rlc.init(&pdcp, &rrc, NULL, &rlc_log, &mac, 0 /* RB_ID_SRB0 */);
  pdcp.init(&rlc, &rrc, &gw, &pdcp_log, 0 /* RB_ID_SRB0 */, SECURITY_DIRECTION_UPLINK);
  nas.init(usim.get(), &rrc, &gw, &nas_log, args.nas);
  gw.init(&pdcp, &nas, &gw_log, args.gw);
  rrc.init(phy, &mac, &rlc, &pdcp, &nas, usim.get(), &gw, &mac, &rrc_log, args.rrc);

  running = true;

  return SRSLTE_SUCCESS;
}

void ue_stack_lte::stop()
{
  if (running) {
    usim->stop();
    nas.stop();
    rrc.stop();

    // Caution here order of stop is very important to avoid locks

    // Stop RLC and PDCP before GW to avoid locking on queue
    rlc.stop();
    pdcp.stop();
    gw.stop();
    mac.stop();

    if (args.pcap.enable) {
      mac_pcap.close();
    }
    if (args.pcap.nas_enable) {
      nas_pcap.close();
    }

    running = false;
  }
}

bool ue_stack_lte::switch_on()
{
  if (running) {
    return nas.attach_request();
  }

  return false;
}

bool ue_stack_lte::switch_off()
{
  // generate detach request
  nas.detach_request();

  // wait for max. 5s for it to be sent (according to TS 24.301 Sec 25.5.2.2)
  const uint32_t RB_ID_SRB1 = 1;
  int            cnt = 0, timeout = 5;
  while (rlc.has_data(RB_ID_SRB1) && ++cnt <= timeout) {
    sleep(1);
  }
  bool detach_sent = true;
  if (rlc.has_data(RB_ID_SRB1)) {
    nas_log.warning("Detach couldn't be sent after %ds.\n", timeout);
    detach_sent = false;
  }

  return detach_sent;
}

bool ue_stack_lte::get_metrics(stack_metrics_t* metrics)
{
  if (EMM_STATE_REGISTERED == nas.get_state()) {
    if (RRC_STATE_CONNECTED == rrc.get_state()) {
      mac.get_metrics(metrics->mac);
      rlc.get_metrics(metrics->rlc);
      gw.get_metrics(metrics->gw);
      nas.get_metrics(&metrics->nas);
      return true;
    }
  }
  return false;
}

bool ue_stack_lte::is_rrc_connected()
{
  return rrc.is_connected();
}

}