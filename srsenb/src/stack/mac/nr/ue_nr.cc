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

#include <bitset>
#include <inttypes.h>
#include <iostream>
#include <string.h>

#include "srsenb/hdr/stack/mac/nr/ue_nr.h"
#include "srsran/common/string_helpers.h"
#include "srsran/interfaces/gnb_interfaces.h"

namespace srsenb {

ue_nr::ue_nr(uint16_t                rnti_,
             uint32_t                enb_cc_idx,
             sched_interface*        sched_,
             rrc_interface_mac_nr*   rrc_,
             rlc_interface_mac*      rlc_,
             phy_interface_stack_nr* phy_,
             srslog::basic_logger&   logger_) :
  rnti(rnti_), sched(sched_), rrc(rrc_), rlc(rlc_), phy(phy_), logger(logger_)
{}

ue_nr::~ue_nr() {}

void ue_nr::reset()
{
  ue_metrics   = {};
  nof_failures = 0;
}

void ue_nr::ue_cfg(const sched_interface::ue_cfg_t& ue_cfg)
{
  // nop
}

void ue_nr::set_tti(uint32_t tti)
{
  last_tti = tti;
}

int ue_nr::process_pdu(srsran::unique_byte_buffer_t pdu)
{
  logger.info(pdu->msg, pdu->N_bytes, "Handling MAC PDU (%d B)", pdu->N_bytes);

  mac_pdu_ul.init_rx(true);
  if (mac_pdu_ul.unpack(pdu->msg, pdu->N_bytes) != SRSRAN_SUCCESS) {
    return SRSRAN_ERROR;
  }

  if (logger.info.enabled()) {
    fmt::memory_buffer str_buffer;
    // mac_pdu_ul.to_string(str_buffer);
    logger.info("0x%x %s", rnti, srsran::to_c_str(str_buffer));
  }

  for (uint32_t i = 0; i < mac_pdu_ul.get_num_subpdus(); ++i) {
    srsran::mac_sch_subpdu_nr subpdu = mac_pdu_ul.get_subpdu(i);
    logger.info("Handling subPDU %d/%d: lcid=%d, sdu_len=%d",
                i,
                mac_pdu_ul.get_num_subpdus(),
                subpdu.get_lcid(),
                subpdu.get_sdu_length());

    rlc->write_pdu(rnti, subpdu.get_lcid(), subpdu.get_sdu(), subpdu.get_sdu_length());
  }
  return SRSRAN_SUCCESS;
}

uint32_t ue_nr::read_pdu(uint32_t lcid, uint8_t* payload, uint32_t requested_bytes)
{
  return rlc->read_pdu(rnti, lcid, payload, requested_bytes);
}

uint8_t* ue_nr::generate_pdu(uint32_t                              enb_cc_idx,
                             uint32_t                              harq_pid,
                             uint32_t                              tb_idx,
                             const sched_interface::dl_sched_pdu_t pdu[sched_interface::MAX_RLC_PDU_LIST],
                             uint32_t                              nof_pdu_elems,
                             uint32_t                              grant_size)
{
  std::lock_guard<std::mutex> lock(mutex);
  uint8_t*                    ret = nullptr;
  if (enb_cc_idx < SRSRAN_MAX_CARRIERS && harq_pid < SRSRAN_FDD_NOF_HARQ && tb_idx < SRSRAN_MAX_TB) {
    srsran::byte_buffer_t* buffer = nullptr; // TODO: read from scheduler output
    buffer->clear();

    mac_pdu_dl.init_tx(buffer, grant_size);

    // read RLC PDU
    ue_rlc_buffer->clear();
    int lcid    = 4;
    int pdu_len = rlc->read_pdu(rnti, lcid, ue_rlc_buffer->msg, grant_size - 2);

    // Only create PDU if RLC has something to tx
    if (pdu_len > 0) {
      logger.info("Adding MAC PDU for RNTI=%d", rnti);
      ue_rlc_buffer->N_bytes = pdu_len;
      logger.info(ue_rlc_buffer->msg, ue_rlc_buffer->N_bytes, "Read %d B from RLC", ue_rlc_buffer->N_bytes);

      // add to MAC PDU and pack
      mac_pdu_dl.add_sdu(4, ue_rlc_buffer->msg, ue_rlc_buffer->N_bytes);
      mac_pdu_dl.pack();
    }

    if (logger.info.enabled()) {
      fmt::memory_buffer str_buffer;
      // mac_pdu_dl.to_string(str_buffer);
      logger.info("0x%x %s", rnti, srsran::to_c_str(str_buffer));
    }
  } else {
    logger.error(
        "Invalid parameters calling generate_pdu: cc_idx=%d, harq_pid=%d, tb_idx=%d", enb_cc_idx, harq_pid, tb_idx);
  }
  return ret;
}

/******* METRICS interface ***************/
void ue_nr::metrics_read(mac_ue_metrics_t* metrics_)
{
  uint32_t ul_buffer = sched->get_ul_buffer(rnti);
  uint32_t dl_buffer = sched->get_dl_buffer(rnti);

  std::lock_guard<std::mutex> lock(metrics_mutex);
  ue_metrics.rnti      = rnti;
  ue_metrics.ul_buffer = ul_buffer;
  ue_metrics.dl_buffer = dl_buffer;

  // set PCell sector id
  std::array<int, SRSRAN_MAX_CARRIERS> cc_list = sched->get_enb_ue_cc_map(rnti);
  auto                                 it      = std::find(cc_list.begin(), cc_list.end(), 0);
  ue_metrics.cc_idx                            = std::distance(cc_list.begin(), it);

  *metrics_ = ue_metrics;

  phr_counter    = 0;
  dl_cqi_counter = 0;
  ue_metrics     = {};
}

void ue_nr::metrics_dl_cqi(uint32_t dl_cqi)
{
  std::lock_guard<std::mutex> lock(metrics_mutex);
  ue_metrics.dl_cqi = SRSRAN_VEC_CMA((float)dl_cqi, ue_metrics.dl_cqi, dl_cqi_counter);
  dl_cqi_counter++;
}

void ue_nr::metrics_rx(bool crc, uint32_t tbs)
{
  std::lock_guard<std::mutex> lock(metrics_mutex);
  if (crc) {
    ue_metrics.rx_brate += tbs * 8;
  } else {
    ue_metrics.rx_errors++;
  }
  ue_metrics.rx_pkts++;
}

void ue_nr::metrics_tx(bool crc, uint32_t tbs)
{
  std::lock_guard<std::mutex> lock(metrics_mutex);
  if (crc) {
    ue_metrics.tx_brate += tbs * 8;
  } else {
    ue_metrics.tx_errors++;
  }
  ue_metrics.tx_pkts++;
}

void ue_nr::metrics_cnt()
{
  std::lock_guard<std::mutex> lock(metrics_mutex);
  ue_metrics.nof_tti++;
}

} // namespace srsenb
