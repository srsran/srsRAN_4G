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
#include "srsran/common/buffer_pool.h"
#include "srsran/common/string_helpers.h"
#include "srsran/interfaces/gnb_interfaces.h"

namespace srsenb {

ue_nr::ue_nr(uint16_t                rnti_,
             uint32_t                enb_cc_idx,
             sched_nr_interface*     sched_,
             rrc_interface_mac_nr*   rrc_,
             rlc_interface_mac*      rlc_,
             phy_interface_stack_nr* phy_,
             srslog::basic_logger&   logger_) :
  rnti(rnti_),
  sched(sched_),
  rrc(rrc_),
  rlc(rlc_),
  phy(phy_),
  logger(logger_),
  ue_rlc_buffer(srsran::make_byte_buffer())
{}

ue_nr::~ue_nr() {}

void ue_nr::reset()
{
  {
    std::lock_guard<std::mutex> lock(metrics_mutex);
    ue_metrics = {};
  }
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
  logger.debug(pdu->msg, pdu->N_bytes, "Handling MAC PDU (%d B)", pdu->N_bytes);

  mac_pdu_ul.init_rx(true);
  if (mac_pdu_ul.unpack(pdu->msg, pdu->N_bytes) != SRSRAN_SUCCESS) {
    return SRSRAN_ERROR;
  }

  if (logger.info.enabled()) {
    fmt::memory_buffer str_buffer;
    mac_pdu_ul.to_string(str_buffer);
    logger.info("Rx PDU: rnti=0x%x, %s", rnti, srsran::to_c_str(str_buffer));
  }

  // First, process MAC CEs in reverse order (CE like C-RNTI get handled first)
  for (uint32_t n = mac_pdu_ul.get_num_subpdus(), i = mac_pdu_ul.get_num_subpdus() - 1; n > 0; --n, i = n - 1) {
    srsran::mac_sch_subpdu_nr subpdu = mac_pdu_ul.get_subpdu(i);
    if (not subpdu.is_sdu()) {
      if (process_ce_subpdu(subpdu) != SRSRAN_SUCCESS) {
        return SRSRAN_ERROR;
      }
    }
  }

  // Second, handle all SDUs in order to avoid unnecessary reordering at higher layers
  for (uint32_t i = 0; i < mac_pdu_ul.get_num_subpdus(); ++i) {
    srsran::mac_sch_subpdu_nr subpdu = mac_pdu_ul.get_subpdu(i);
    if (subpdu.is_sdu()) {
      rrc->set_activity_user(rnti);
      rlc->write_pdu(rnti, subpdu.get_lcid(), subpdu.get_sdu(), subpdu.get_sdu_length());
    }
  }

  return SRSRAN_SUCCESS;
}

int ue_nr::process_ce_subpdu(srsran::mac_sch_subpdu_nr& subpdu)
{
  // Handle MAC CEs
  switch (subpdu.get_lcid()) {
    case srsran::mac_sch_subpdu_nr::nr_lcid_sch_t::CRNTI: {
      uint16_t c_rnti = subpdu.get_c_rnti();
      if (true /*sched->ue_exists(c_crnti)*/) {
        rrc->update_user(rnti, c_rnti);
        rnti = c_rnti;
        sched->ul_sr_info(rnti); // provide UL grant regardless of other BSR content for UE to complete RA
      } else {
        logger.warning("Updating user C-RNTI: rnti=0x%x already released.", c_rnti);
        // Disable scheduling for all bearers. The new rnti will be removed on msg3 timer expiry in the RRC
        for (uint32_t lcid = 0; lcid < sched_interface::MAX_LC; ++lcid) {
          // sched->bearer_ue_rem(rnti, lcid);
        }
      }
    } break;
    case srsran::mac_sch_subpdu_nr::nr_lcid_sch_t::SHORT_BSR:
    case srsran::mac_sch_subpdu_nr::nr_lcid_sch_t::SHORT_TRUNC_BSR: {
      srsran::mac_sch_subpdu_nr::lcg_bsr_t sbsr = subpdu.get_sbsr();
      uint32_t buffer_size_bytes                = buff_size_field_to_bytes(sbsr.buffer_size, srsran::SHORT_BSR);
      // Assume all LCGs are 0 if reported SBSR is 0
      if (buffer_size_bytes == 0) {
        for (uint32_t j = 0; j <= SCHED_NR_MAX_LC_GROUP; j++) {
          sched->ul_bsr(rnti, j, 0);
        }
      } else {
        sched->ul_bsr(rnti, sbsr.lcg_id, buffer_size_bytes);
      }
    } break;
    case srsran::mac_sch_subpdu_nr::nr_lcid_sch_t::LONG_BSR:
    case srsran::mac_sch_subpdu_nr::nr_lcid_sch_t::LONG_TRUNC_BSR: {
      srsran::mac_sch_subpdu_nr::lbsr_t lbsr = subpdu.get_lbsr();
      for (auto& lb : lbsr.list) {
        sched->ul_bsr(rnti, lb.lcg_id, buff_size_field_to_bytes(lb.buffer_size, srsran::LONG_BSR));
      }
    } break;
    case srsran::mac_sch_subpdu_nr::nr_lcid_sch_t::PADDING:
      break;
    default:
      logger.warning("Unhandled subPDU with LCID=%d", subpdu.get_lcid());
  }

  return SRSRAN_SUCCESS;
}

uint32_t ue_nr::read_pdu(uint32_t lcid, uint8_t* payload, uint32_t requested_bytes)
{
  return rlc->read_pdu(rnti, lcid, payload, requested_bytes);
}

int ue_nr::generate_pdu(srsran::byte_buffer_t* pdu, uint32_t grant_size)
{
  std::lock_guard<std::mutex> lock(mutex);

  if (mac_pdu_dl.init_tx(pdu, grant_size) != SRSRAN_SUCCESS) {
    logger.error("Couldn't initialize MAC PDU buffer");
    return SRSRAN_ERROR;
  }

  bool drb_activity = false; // inform RRC about user activity if true
  int  lcid         = 4;     // only supporting single DRB right now

  int32_t remaining_len = mac_pdu_dl.get_remaing_len();

  logger.debug("Adding MAC PDU for RNTI=%d (max %d B)", rnti, remaining_len);
  while (remaining_len >= MIN_RLC_PDU_LEN) {
    // clear read buffer
    ue_rlc_buffer->clear();

    // Determine space for RLC
    remaining_len -= remaining_len >= srsran::mac_sch_subpdu_nr::MAC_SUBHEADER_LEN_THRESHOLD ? 3 : 2;

    // read RLC PDU
    int pdu_len = rlc->read_pdu(rnti, lcid, ue_rlc_buffer->msg, remaining_len);

    if (pdu_len > remaining_len) {
      logger.error("Can't add SDU of %d B. Available space %d B", pdu_len, remaining_len);
      break;
    } else {
      // Add SDU if RLC has something to tx
      if (pdu_len > 0) {
        ue_rlc_buffer->N_bytes = pdu_len;
        logger.debug(ue_rlc_buffer->msg, ue_rlc_buffer->N_bytes, "Read %d B from RLC", ue_rlc_buffer->N_bytes);

        // add to MAC PDU and pack
        if (mac_pdu_dl.add_sdu(lcid, ue_rlc_buffer->msg, ue_rlc_buffer->N_bytes) != SRSRAN_SUCCESS) {
          logger.error("Error packing MAC PDU");
          break;
        }

        // set DRB activity flag but only notify RRC once
        if (lcid > 3) {
          drb_activity = true;
        }
      } else {
        break;
      }

      remaining_len -= pdu_len;
      logger.debug("%d B remaining PDU", remaining_len);
    }
  }

  mac_pdu_dl.pack();

  if (drb_activity) {
    // Indicate DRB activity in DL to RRC
    rrc->set_activity_user(rnti);
    logger.debug("DL activity rnti=0x%x", rnti);
  }

  if (logger.info.enabled()) {
    fmt::memory_buffer str_buffer;
    mac_pdu_dl.to_string(str_buffer);
    logger.info("0x%x %s", rnti, srsran::to_c_str(str_buffer));
  }
  return SRSRAN_SUCCESS;
}

/******* METRICS interface ***************/
void ue_nr::metrics_read(mac_ue_metrics_t* metrics_)
{
  uint32_t ul_buffer = 0; // sched->get_ul_buffer(rnti);
  uint32_t dl_buffer = 0; // sched->get_dl_buffer(rnti);

  std::lock_guard<std::mutex> lock(metrics_mutex);
  ue_metrics.rnti      = rnti;
  ue_metrics.ul_buffer = ul_buffer;
  ue_metrics.dl_buffer = dl_buffer;

  // set PCell sector id
  std::array<int, SRSRAN_MAX_CARRIERS> cc_list; //= sched->get_enb_ue_cc_map(rnti);
  auto                                 it = std::find(cc_list.begin(), cc_list.end(), 0);
  ue_metrics.cc_idx                       = std::distance(cc_list.begin(), it);

  *metrics_            = ue_metrics;
  phr_counter          = 0;
  dl_cqi_valid_counter = 0;
  pucch_sinr_counter   = 0;
  pusch_sinr_counter   = 0;
  ue_metrics           = {};
}

void ue_nr::metrics_dl_cqi(const srsran_uci_cfg_nr_t& cfg_, uint32_t dl_cqi)
{
  std::lock_guard<std::mutex> lock(metrics_mutex);

  // Process CQI
  for (uint32_t i = 0; i < cfg_.nof_csi; i++) {
    // Skip if invalid or not supported CSI report
    if (cfg_.csi[i].cfg.quantity != SRSRAN_CSI_REPORT_QUANTITY_CRI_RI_PMI_CQI or
        cfg_.csi[i].cfg.freq_cfg != SRSRAN_CSI_REPORT_FREQ_WIDEBAND) {
      continue;
    }

    // Add statistics
    ue_metrics.dl_cqi = SRSRAN_VEC_SAFE_CMA(dl_cqi, ue_metrics.dl_cqi, dl_cqi_valid_counter);
    dl_cqi_valid_counter++;
  }
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

void ue_nr::metrics_dl_mcs(uint32_t mcs)
{
  std::lock_guard<std::mutex> lock(metrics_mutex);
  ue_metrics.dl_mcs = SRSRAN_VEC_CMA((float)mcs, ue_metrics.dl_mcs, ue_metrics.dl_mcs_samples);
  ue_metrics.dl_mcs_samples++;
}

void ue_nr::metrics_ul_mcs(uint32_t mcs)
{
  std::lock_guard<std::mutex> lock(metrics_mutex);
  ue_metrics.ul_mcs = SRSRAN_VEC_CMA((float)mcs, ue_metrics.ul_mcs, ue_metrics.ul_mcs_samples);
  ue_metrics.ul_mcs_samples++;
}

void ue_nr::metrics_cnt()
{
  std::lock_guard<std::mutex> lock(metrics_mutex);
  ue_metrics.nof_tti++;
}

void ue_nr::metrics_pucch_sinr(float sinr)
{
  std::lock_guard<std::mutex> lock(metrics_mutex);
  // discard nan or inf values for average SINR
  if (!std::isinf(sinr) && !std::isnan(sinr)) {
    ue_metrics.pucch_sinr = SRSRAN_VEC_SAFE_CMA((float)sinr, ue_metrics.pucch_sinr, pucch_sinr_counter);
    pucch_sinr_counter++;
  }
}

void ue_nr::metrics_pusch_sinr(float sinr)
{
  std::lock_guard<std::mutex> lock(metrics_mutex);
  // discard nan or inf values for average SINR
  if (!std::isinf(sinr) && !std::isnan(sinr)) {
    ue_metrics.pusch_sinr = SRSRAN_VEC_SAFE_CMA((float)sinr, ue_metrics.pusch_sinr, pusch_sinr_counter);
    pusch_sinr_counter++;
  }
}

/** Converts the buffer size field of a BSR (5 or 8-bit Buffer Size field) into Bytes
 * @param buff_size_field The buffer size field contained in the MAC PDU
 * @param format          The BSR format that determines the buffer size field length
 * @return uint32_t       The actual buffer size level in Bytes
 */
uint32_t ue_nr::buff_size_field_to_bytes(uint32_t buff_size_index, const srsran::bsr_format_nr_t& format)
{
  using namespace srsran;

  // early exit
  if (buff_size_index == 0) {
    return 0;
  }

  const uint32_t max_offset = 1; // make the reported value bigger than the 2nd biggest

  switch (format) {
    case SHORT_BSR:
    case SHORT_TRUNC_BSR:
      if (buff_size_index >= buffer_size_levels_5bit_max_idx) {
        return buffer_size_levels_5bit[buffer_size_levels_5bit_max_idx] + max_offset;
      } else {
        return buffer_size_levels_5bit[buff_size_index];
      }
      break;
    case LONG_BSR:
    case LONG_TRUNC_BSR:
      if (buff_size_index > buffer_size_levels_8bit_max_idx) {
        return buffer_size_levels_8bit[buffer_size_levels_8bit_max_idx] + max_offset;
      } else {
        return buffer_size_levels_8bit[buff_size_index];
      }
      break;
  }
  return 0;
}

} // namespace srsenb
