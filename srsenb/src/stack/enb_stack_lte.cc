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

#include "srsenb/hdr/stack/enb_stack_lte.h"
#include "srsenb/hdr/enb.h"
#include "srslte/srslte.h"
#include <srslte/interfaces/enb_metrics_interface.h>

using namespace srslte;

namespace srsenb {

enb_stack_lte::enb_stack_lte(srslte::logger* logger_) : logger(logger_), pdcp(&pdcp_log), timers(128) {}

enb_stack_lte::~enb_stack_lte()
{
  stop();
}

std::string enb_stack_lte::get_type()
{
  return "lte";
}

int enb_stack_lte::init(const stack_args_t& args_, const rrc_cfg_t& rrc_cfg_, phy_interface_stack_lte* phy_)
{
  phy = phy_;
  if (init(args_, rrc_cfg_)) {
    return SRSLTE_ERROR;
  }
  return SRSLTE_SUCCESS;
}

int enb_stack_lte::init(const stack_args_t& args_, const rrc_cfg_t& rrc_cfg_)
{
  args    = args_;
  rrc_cfg = rrc_cfg_;

  // setup logging for each layer
  mac_log.init("MAC ", logger, true);
  rlc_log.init("RLC ", logger);
  pdcp_log.init("PDCP", logger);
  rrc_log.init("RRC ", logger);
  gtpu_log.init("GTPU", logger);
  s1ap_log.init("S1AP", logger);

  // Init logs
  mac_log.set_level(args.log.mac_level);
  rlc_log.set_level(args.log.rlc_level);
  pdcp_log.set_level(args.log.pdcp_level);
  rrc_log.set_level(args.log.rrc_level);
  gtpu_log.set_level(args.log.gtpu_level);
  s1ap_log.set_level(args.log.s1ap_level);

  mac_log.set_hex_limit(args.log.mac_hex_limit);
  rlc_log.set_hex_limit(args.log.rlc_hex_limit);
  pdcp_log.set_hex_limit(args.log.pdcp_hex_limit);
  rrc_log.set_hex_limit(args.log.rrc_hex_limit);
  gtpu_log.set_hex_limit(args.log.gtpu_hex_limit);
  s1ap_log.set_hex_limit(args.log.s1ap_hex_limit);

  // Set up pcap and trace
  if (args.pcap.enable) {
    mac_pcap.open(args.pcap.filename.c_str());
    mac.start_pcap(&mac_pcap);
  }

  // verify configuration correctness
  uint32_t       prach_freq_offset = rrc_cfg.sibs[1].sib2().rr_cfg_common.prach_cfg.prach_cfg_info.prach_freq_offset;
  srslte_cell_t& cell_cfg          = rrc_cfg.cell;
  if (cell_cfg.nof_prb > 10) {
    uint32_t lower_bound = SRSLTE_MAX(rrc_cfg.sr_cfg.nof_prb, rrc_cfg.cqi_cfg.nof_prb);
    uint32_t upper_bound = cell_cfg.nof_prb - lower_bound;
    if (prach_freq_offset + 6 > upper_bound or prach_freq_offset < lower_bound) {
      fprintf(stderr,
              "ERROR: Invalid PRACH configuration - prach_freq_offset=%d collides with PUCCH.\n",
              prach_freq_offset);
      fprintf(stderr,
              "       Consider changing \"prach_freq_offset\" in sib.conf to a value between %d and %d.\n",
              lower_bound,
              upper_bound);
      return SRSLTE_ERROR;
    }
  } else { // 6 PRB case
    if (prach_freq_offset + 6 > cell_cfg.nof_prb) {
      fprintf(stderr,
              "ERROR: Invalid PRACH configuration - prach=(%d, %d) does not fit into the eNB PRBs=(0, %d).\n",
              prach_freq_offset,
              prach_freq_offset + 6,
              cell_cfg.nof_prb);
      fprintf(
          stderr,
          "       Consider changing the \"prach_freq_offset\" value to 0 in the sib.conf file when using 6 PRBs.\n");
      return SRSLTE_ERROR;
    }
  }

  // Init all layers
  mac.init(args.mac, &cell_cfg, phy, &rlc, &rrc, &mac_log);
  rlc.init(&pdcp, &rrc, &mac, &timers, &rlc_log);
  pdcp.init(&rlc, &rrc, &gtpu);
  rrc.init(&rrc_cfg, phy, &mac, &rlc, &pdcp, &s1ap, &gtpu, &rrc_log);
  s1ap.init(args.s1ap, &rrc, &s1ap_log);
  gtpu.init(args.s1ap.gtp_bind_addr,
            args.s1ap.mme_addr,
            args.embms.m1u_multiaddr,
            args.embms.m1u_if_addr,
            &pdcp,
            &gtpu_log,
            args.embms.enable);

  started = true;

  return SRSLTE_SUCCESS;
}

void enb_stack_lte::stop()
{
  if (started) {
    s1ap.stop();
    gtpu.stop();
    mac.stop();
    usleep(50000);

    rlc.stop();
    pdcp.stop();
    rrc.stop();

    usleep(10000);
    if (args.pcap.enable) {
      mac_pcap.close();
    }
    started = false;
  }
}

bool enb_stack_lte::get_metrics(stack_metrics_t* metrics)
{
  mac.get_metrics(metrics->mac);
  rrc.get_metrics(metrics->rrc);
  s1ap.get_metrics(metrics->s1ap);
  return true;
}

} // namespace srsenb
