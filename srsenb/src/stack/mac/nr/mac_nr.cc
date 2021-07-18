/**
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#include "srsenb/hdr/stack/mac/mac_nr.h"
#include "srsran/common/buffer_pool.h"
#include "srsran/common/log_helper.h"
#include <pthread.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>

namespace srsenb {

mac_nr::mac_nr(srsran::task_sched_handle task_sched_) :
  logger(srslog::fetch_basic_logger("MAC-NR")), task_sched(task_sched_)
{}

mac_nr::~mac_nr()
{
  stop();
}

int mac_nr::init(const mac_nr_args_t&    args_,
                 phy_interface_stack_nr* phy_,
                 stack_interface_mac*    stack_,
                 rlc_interface_mac_nr*   rlc_,
                 rrc_interface_mac_nr*   rrc_)
{
  args = args_;

  phy_h   = phy_;
  stack_h = stack_;
  rlc_h   = rlc_;
  rrc_h   = rrc_;

  logger.set_level(srslog::str_to_basic_level(args.log_level));
  logger.set_hex_dump_max_size(args.log_hex_limit);

  if (args.pcap.enable) {
    pcap = std::unique_ptr<srsran::mac_pcap>(new srsran::mac_pcap());
    pcap->open(args.pcap.filename);
  }

  bcch_bch_payload = srsran::make_byte_buffer();
  if (bcch_bch_payload == nullptr) {
    return SRSRAN_ERROR;
  }

  // allocate 8 tx buffers for UE (TODO: as we don't handle softbuffers why do we need so many buffers)
  for (int i = 0; i < SRSRAN_FDD_NOF_HARQ; i++) {
    srsran::unique_byte_buffer_t buffer = srsran::make_byte_buffer();
    if (buffer == nullptr) {
      return SRSRAN_ERROR;
    }
    ue_tx_buffer.emplace_back(std::move(buffer));
  }

  ue_rlc_buffer = srsran::make_byte_buffer();
  if (ue_rlc_buffer == nullptr) {
    return SRSRAN_ERROR;
  }

  logger.info("Started");

  started = true;

  return SRSRAN_SUCCESS;
}

void mac_nr::stop()
{
  if (started) {
    if (pcap != nullptr) {
      pcap->close();
    }

    started = false;
  }
}

void mac_nr::get_metrics(srsenb::mac_metrics_t& metrics) {}

int mac_nr::rx_data_indication(stack_interface_phy_nr::rx_data_ind_t& rx_data)
{
  // push received PDU on queue
  if (rx_data.tb != nullptr) {
    if (pcap) {
      pcap->write_ul_crnti_nr(rx_data.tb->msg, rx_data.tb->N_bytes, rx_data.rnti, true, rx_data.tti);
    }
    ue_rx_pdu_queue.push(std::move(rx_data.tb));
  }

  // inform stack that new PDUs may have been received
  stack_h->process_pdus();

  return SRSRAN_SUCCESS;
}

/**
 * Called from the main stack thread to process received PDUs
 */
void mac_nr::process_pdus()
{
  while (started and not ue_rx_pdu_queue.empty()) {
    srsran::unique_byte_buffer_t pdu = ue_rx_pdu_queue.wait_pop();
    /// TODO; delegate to demux class
    handle_pdu(std::move(pdu));
  }
}

int mac_nr::handle_pdu(srsran::unique_byte_buffer_t pdu)
{
  logger.info(pdu->msg, pdu->N_bytes, "Handling MAC PDU (%d B)", pdu->N_bytes);

  ue_rx_pdu.init_rx(true);
  if (ue_rx_pdu.unpack(pdu->msg, pdu->N_bytes) != SRSRAN_SUCCESS) {
    return SRSRAN_ERROR;
  }

  for (uint32_t i = 0; i < ue_rx_pdu.get_num_subpdus(); ++i) {
    srsran::mac_sch_subpdu_nr subpdu = ue_rx_pdu.get_subpdu(i);
    logger.info("Handling subPDU %d/%d: lcid=%d, sdu_len=%d",
                i,
                ue_rx_pdu.get_num_subpdus(),
                subpdu.get_lcid(),
                subpdu.get_sdu_length());

    // rlc_h->write_pdu(args.rnti, subpdu.get_lcid(), subpdu.get_sdu(), subpdu.get_sdu_length());
  }
  return SRSRAN_SUCCESS;
}

int mac_nr::cell_cfg(srsenb::sched_interface::cell_cfg_t* cell_cfg)
{
  cfg = *cell_cfg;

  // read SIBs from RRC (SIB1 for now only)
  for (int i = 0; i < srsenb::sched_interface::MAX_SIBS; i++) {
    if (cell_cfg->sibs->len > 0) {
      sib_info_t sib  = {};
      sib.index       = i;
      sib.periodicity = cell_cfg->sibs->period_rf;
      sib.payload     = srsran::make_byte_buffer();
      if (rrc_h->read_pdu_bcch_dlsch(sib.index, sib.payload) != SRSRAN_SUCCESS) {
        logger.error("Couldn't read SIB %d from RRC", sib.index);
      }

      logger.info("Including SIB %d into SI scheduling", sib.index);
      bcch_dlsch_payload.push_back(std::move(sib));
    }
  }

  return SRSRAN_SUCCESS;
}

int mac_nr::slot_indication(const srsran_slot_cfg_t& slot_cfg)
{
  return 0;
}

int mac_nr::get_dl_sched(const srsran_slot_cfg_t& slot_cfg, dl_sched_t& dl_sched)
{
  return 0;
}
int mac_nr::get_ul_sched(const srsran_slot_cfg_t& slot_cfg, ul_sched_t& ul_sched)
{
  return 0;
}
int mac_nr::pucch_info(const srsran_slot_cfg_t& slot_cfg, const mac_interface_phy_nr::pucch_info_t& pucch_info)
{
  return 0;
}
int mac_nr::pusch_info(const srsran_slot_cfg_t& slot_cfg, const mac_interface_phy_nr::pusch_info_t& pusch_info)
{
  return 0;
}
void mac_nr::rach_detected(const mac_interface_phy_nr::rach_info_t& rach_info) {}

} // namespace srsenb
