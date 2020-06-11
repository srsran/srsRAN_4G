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

#include "srsenb/hdr/stack/mac/mac_nr.h"
#include "srslte/common/buffer_pool.h"
#include "srslte/common/log_helper.h"
#include <pthread.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>

namespace srsenb {

mac_nr::mac_nr() : pool(srslte::byte_buffer_pool::get_instance()), log_h("MAC")
{
  bcch_bch_payload = srslte::allocate_unique_buffer(*pool);

  // allocate 8 tx buffers for UE (TODO: as we don't handle softbuffers why do we need so many buffers)
  for (int i = 0; i < SRSLTE_FDD_NOF_HARQ; i++) {
    ue_tx_buffer.emplace_back(srslte::allocate_unique_buffer(*pool));
  }

  ue_rlc_buffer = srslte::allocate_unique_buffer(*pool);
}

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

  log_h->set_level(args.log_level);
  log_h->set_hex_limit(args.log_hex_limit);

  if (args.pcap.enable) {
    pcap = std::unique_ptr<srslte::mac_nr_pcap>(new srslte::mac_nr_pcap());
    pcap->open(args.pcap.filename);
  }

  log_h->info("Started\n");

  started = true;

  return SRSLTE_SUCCESS;
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

void mac_nr::get_metrics(srsenb::mac_metrics_t* metrics) {}

// Fills both, DL_CONFIG.request and TX.request structs
void mac_nr::get_dl_config(const uint32_t                               tti,
                           phy_interface_stack_nr::dl_config_request_t& config_request,
                           phy_interface_stack_nr::tx_request_t&        tx_request)
{
  // send MIB over BCH every 80ms
  if (tti % 80 == 0) {
    // try to read BCH PDU from RRC
    if (rrc_h->read_pdu_bcch_bch(tti, bcch_bch_payload) == SRSLTE_SUCCESS) {
      log_h->info("Adding BCH in TTI=%d\n", tti);
      tx_request.pdus[tx_request.nof_pdus].pbch.mib_present = true;
      tx_request.pdus[tx_request.nof_pdus].data[0]          = bcch_bch_payload->msg;
      tx_request.pdus[tx_request.nof_pdus].length           = bcch_bch_payload->N_bytes;
      tx_request.pdus[tx_request.nof_pdus].index            = tx_request.nof_pdus;
      tx_request.nof_pdus++;

      if (pcap) {
        pcap->write_dl_bch(bcch_bch_payload->msg, bcch_bch_payload->N_bytes, 0xffff, 0, tti);
      }
    } else {
      log_h->error("Couldn't read BCH payload from RRC\n");
    }
  }

  // Schedule SIBs
  for (auto& sib : bcch_dlsch_payload) {
    if (sib.payload->N_bytes > 0) {
      if (tti % (sib.periodicity * 10) == 0) {
        log_h->info("Adding SIB %d in TTI=%d\n", sib.index, tti);

        tx_request.pdus[tx_request.nof_pdus].data[0] = sib.payload->msg;
        tx_request.pdus[tx_request.nof_pdus].length  = sib.payload->N_bytes;
        tx_request.pdus[tx_request.nof_pdus].index   = tx_request.nof_pdus;

        if (pcap) {
          pcap->write_dl_si_rnti(sib.payload->msg, sib.payload->N_bytes, 0xffff, 0, tti);
        }

        tx_request.nof_pdus++;
      }
    }
  }

  // Add MAC padding if TTI is empty
  if (tx_request.nof_pdus == 0) {
    uint32_t buffer_index = tti % SRSLTE_FDD_NOF_HARQ;

    ue_tx_buffer.at(buffer_index)->clear();
    ue_tx_pdu.init_tx(ue_tx_buffer.at(buffer_index).get(), args.tb_size);

    // read RLC PDU
    ue_rlc_buffer->clear();
    int pdu_len = rlc_h->read_pdu(args.rnti, 4, ue_rlc_buffer->msg, args.tb_size - 2);

    // Only create PDU if RLC has something to tx
    if (pdu_len > 0) {
      log_h->info("Adding MAC PDU for RNTI=%d\n", args.rnti);
      ue_rlc_buffer->N_bytes = pdu_len;
      log_h->info_hex(ue_rlc_buffer->msg, ue_rlc_buffer->N_bytes, "Read %d B from RLC\n", ue_rlc_buffer->N_bytes);

      // add to MAC PDU and pack
      ue_tx_pdu.add_sdu(4, ue_rlc_buffer->msg, ue_rlc_buffer->N_bytes);
      ue_tx_pdu.pack();

      log_h->debug_hex(ue_tx_buffer.at(buffer_index)->msg,
                       ue_tx_buffer.at(buffer_index)->N_bytes,
                       "Generated MAC PDU (%d B)\n",
                       ue_tx_buffer.at(buffer_index)->N_bytes);

      tx_request.pdus[tx_request.nof_pdus].data[0] = ue_tx_buffer.at(buffer_index)->msg;
      tx_request.pdus[tx_request.nof_pdus].length  = ue_tx_buffer.at(buffer_index)->N_bytes;
      tx_request.pdus[tx_request.nof_pdus].index   = tx_request.nof_pdus;

      if (pcap) {
        pcap->write_dl_crnti(tx_request.pdus[tx_request.nof_pdus].data[0],
                             tx_request.pdus[tx_request.nof_pdus].length,
                             args.rnti,
                             buffer_index,
                             tti);
      }

      tx_request.nof_pdus++;
    }
  }

  config_request.tti = tti;
  tx_request.tti     = tti;
}

int mac_nr::sf_indication(const uint32_t tti)
{
  phy_interface_stack_nr::dl_config_request_t config_request = {};
  phy_interface_stack_nr::tx_request_t        tx_request     = {};

  // step MAC TTI
  log_h->step(tti);

  get_dl_config(tti, config_request, tx_request);

  // send DL_CONFIG.request
  phy_h->dl_config_request(config_request);

  // send TX.request
  phy_h->tx_request(tx_request);

  return SRSLTE_SUCCESS;
}

int mac_nr::rx_data_indication(stack_interface_phy_nr::rx_data_ind_t& rx_data)
{
  // push received PDU on queue
  if (rx_data.tb != nullptr) {
    if (pcap) {
      pcap->write_ul_crnti(rx_data.tb->msg, rx_data.tb->N_bytes, rx_data.rnti, true, rx_data.tti);
    }
    ue_rx_pdu_queue.push(std::move(rx_data.tb));
  }

  // inform stack that new PDUs may have been received
  stack_h->process_pdus();

  return SRSLTE_SUCCESS;
}

/**
 * Called from the main stack thread to process received PDUs
 */
void mac_nr::process_pdus()
{
  while (started and not ue_rx_pdu_queue.empty()) {
    srslte::unique_byte_buffer_t pdu = ue_rx_pdu_queue.wait_pop();
    /// TODO; delegate to demux class
    handle_pdu(std::move(pdu));
  }
}

int mac_nr::handle_pdu(srslte::unique_byte_buffer_t pdu)
{
  log_h->info_hex(pdu->msg, pdu->N_bytes, "Handling MAC PDU (%d B)\n", pdu->N_bytes);

  ue_rx_pdu.init_rx(true);
  ue_rx_pdu.unpack(pdu->msg, pdu->N_bytes);

  for (uint32_t i = 0; i < ue_rx_pdu.get_num_subpdus(); ++i) {
    srslte::mac_nr_sch_subpdu subpdu = ue_rx_pdu.get_subpdu(i);
    log_h->info("Handling subPDU %d/%d: lcid=%d, sdu_len=%d\n",
                i,
                ue_rx_pdu.get_num_subpdus(),
                subpdu.get_lcid(),
                subpdu.get_sdu_length());

    if (subpdu.get_lcid() == args.drb_lcid) {
      rlc_h->write_pdu(args.rnti, subpdu.get_lcid(), subpdu.get_sdu(), subpdu.get_sdu_length());
    }
  }
  return SRSLTE_SUCCESS;
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
      sib.payload     = srslte::allocate_unique_buffer(*pool);
      if (rrc_h->read_pdu_bcch_dlsch(sib.index, sib.payload) != SRSLTE_SUCCESS) {
        log_h->error("Couldn't read SIB %d from RRC\n", sib.index);
      }

      log_h->info("Including SIB %d into SI scheduling\n", sib.index);
      bcch_dlsch_payload.push_back(std::move(sib));
    }
  }

  return SRSLTE_SUCCESS;
}

} // namespace srsenb
