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

#include "srsenb/hdr/stack/mac/nr/mac_nr.h"
#include "srsran/common/buffer_pool.h"
#include "srsran/common/log_helper.h"
#include "srsran/common/phy_cfg_nr_default.h"
#include "srsran/common/rwlock_guard.h"
#include "srsran/common/standard_streams.h"
#include "srsran/common/string_helpers.h"
#include "srsran/common/time_prof.h"
#include "srsran/mac/mac_rar_pdu_nr.h"
#include <pthread.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>

namespace srsenb {

mac_nr::mac_nr(srsran::task_sched_handle task_sched_) :
  logger(srslog::fetch_basic_logger("MAC-NR")),
  task_sched(task_sched_),
  bcch_bch_payload(srsran::make_byte_buffer()),
  rar_pdu_buffer(srsran::make_byte_buffer())
{
  stack_task_queue = task_sched.make_task_queue();
}

mac_nr::~mac_nr()
{
  stop();
}

int mac_nr::init(const mac_nr_args_t&    args_,
                 phy_interface_stack_nr* phy_,
                 stack_interface_mac*    stack_,
                 rlc_interface_mac*      rlc_,
                 rrc_interface_mac_nr*   rrc_)
{
  args = args_;

  phy   = phy_;
  stack = stack_;
  rlc   = rlc_;
  rrc   = rrc_;

  if (args.pcap.enable) {
    pcap = std::unique_ptr<srsran::mac_pcap>(new srsran::mac_pcap());
    pcap->open(args.pcap.filename);
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

/// Called from metrics thread.
/// Note: This can contend for the same mutexes as the ones used by L1/L2 workers.
///       However, get_metrics is called infrequently enough to cause major halts in the L1/L2
void mac_nr::get_metrics(srsenb::mac_metrics_t& metrics)
{
  // TODO: We should comment on the logic we follow to get the metrics. Some of them are retrieved from MAC, some
  // others from the scheduler.
  get_metrics_nolock(metrics);
  sched.get_metrics(metrics);
}

void mac_nr::get_metrics_nolock(srsenb::mac_metrics_t& metrics)
{
  srsran::rwlock_read_guard lock(rwmutex);
  metrics.ues.reserve(ue_db.size());
  for (auto& u : ue_db) {
    metrics.ues.emplace_back();
    u.second->metrics_read(&metrics.ues.back());
  }
  metrics.cc_info.resize(detected_rachs.size());
  for (unsigned cc = 0, e = detected_rachs.size(); cc != e; ++cc) {
    metrics.cc_info[cc].cc_rach_counter = detected_rachs[cc];
    metrics.cc_info[cc].pci             = (cc < cell_config.size()) ? cell_config[cc].carrier.pci : 0;
  }
}

int mac_nr::cell_cfg(const std::vector<srsenb::sched_nr_interface::cell_cfg_t>& nr_cells)
{
  cell_config = nr_cells;
  sched.config(args.sched_cfg, nr_cells);
  detected_rachs.resize(nr_cells.size());

  // read SIBs from RRC (SIB1 for now only)
  for (int i = 0; i < 1 /* srsenb::sched_interface::MAX_SIBS */; i++) {
    // TODO: add flag for SIBs into cell config
    if (true) {
      sib_info_t sib  = {};
      sib.index       = i;
      sib.periodicity = 4; // TODO: read period_rf from config
      sib.payload     = srsran::make_byte_buffer();
      if (sib.payload == nullptr) {
        logger.error("Couldn't allocate PDU in %s().", __FUNCTION__);
        return SRSRAN_ERROR;
      }
      if (rrc->read_pdu_bcch_dlsch(sib.index, sib.payload) != SRSRAN_SUCCESS) {
        logger.error("Couldn't read SIB %d from RRC", sib.index);
      }

      logger.info("Including SIB %d into SI scheduling", sib.index);
      bcch_dlsch_payload.push_back(std::move(sib));
    }
  }

  return SRSRAN_SUCCESS;
}

int mac_nr::ue_cfg(uint16_t rnti, const sched_nr_interface::ue_cfg_t& ue_cfg)
{
  sched.ue_cfg(rnti, ue_cfg);
  return SRSRAN_SUCCESS;
}

uint16_t mac_nr::reserve_rnti(uint32_t enb_cc_idx, const sched_nr_ue_cfg_t& uecfg)
{
  uint16_t rnti = alloc_ue(enb_cc_idx);
  if (rnti == SRSRAN_INVALID_RNTI) {
    return rnti;
  }

  sched.ue_cfg(rnti, uecfg);

  return rnti;
}

void mac_nr::rach_detected(const rach_info_t& rach_info)
{
  static srsran::mutexed_tprof<srsran::avg_time_stats> rach_tprof("rach_tprof", "MAC-NR", 1);
  logger.set_context(rach_info.slot_index);
  auto rach_tprof_meas = rach_tprof.start();

  uint32_t enb_cc_idx = 0;
  stack_task_queue.push([this, rach_info, enb_cc_idx, rach_tprof_meas]() mutable {
    rach_tprof_meas.defer_stop();

    // Add new user to the scheduler so that it can RX/TX SRB0
    sched_nr_ue_cfg_t uecfg = {};
    uecfg.carriers.resize(1);
    uecfg.carriers[0].active      = true;
    uecfg.carriers[0].cc          = 0;
    uecfg.ue_bearers[0].direction = mac_lc_ch_cfg_t::BOTH;
    srsran::phy_cfg_nr_default_t::reference_cfg_t ref_args{};
    ref_args.duplex = cell_config[0].duplex.mode == SRSRAN_DUPLEX_MODE_TDD
                          ? srsran::phy_cfg_nr_default_t::reference_cfg_t::R_DUPLEX_TDD_CUSTOM_6_4
                          : srsran::phy_cfg_nr_default_t::reference_cfg_t::R_DUPLEX_FDD;
    uecfg.phy_cfg     = srsran::phy_cfg_nr_default_t{ref_args};
    uecfg.phy_cfg.csi = {}; // disable CSI until RA is complete

    uint16_t rnti = reserve_rnti(enb_cc_idx, uecfg);

    // Log this event.
    ++detected_rachs[enb_cc_idx];

    // Trigger scheduler RACH
    srsenb::sched_nr_interface::rar_info_t rar_info = {};
    rar_info.preamble_idx                           = rach_info.preamble;
    rar_info.temp_crnti                             = rnti;
    rar_info.ta_cmd                                 = rach_info.time_adv;
    rar_info.prach_slot                             = slot_point{NUMEROLOGY_IDX, rach_info.slot_index};
    // TODO: fill remaining fields as required
    sched.dl_rach_info(enb_cc_idx, rar_info);
    rrc->add_user(rnti, uecfg);

    logger.info("RACH:  slot=%d, cc=%d, preamble=%d, offset=%d, temp_crnti=0x%x",
                rach_info.slot_index,
                enb_cc_idx,
                rach_info.preamble,
                rach_info.time_adv,
                rnti);
    srsran::console("RACH:  slot=%d, cc=%d, preamble=%d, offset=%d, temp_crnti=0x%x\n",
                    rach_info.slot_index,
                    enb_cc_idx,
                    rach_info.preamble,
                    rach_info.time_adv,
                    rnti);
  });
}

uint16_t mac_nr::alloc_ue(uint32_t enb_cc_idx)
{
  ue_nr*   inserted_ue = nullptr;
  uint16_t rnti        = SRSRAN_INVALID_RNTI;

  do {
    // Assign new RNTI
    rnti = FIRST_RNTI + (ue_counter.fetch_add(1, std::memory_order_relaxed) % 60000);

    // Pre-check if rnti is valid
    {
      srsran::rwlock_read_guard read_lock(rwmutex);
      if (not is_rnti_valid_nolock(rnti)) {
        continue;
      }
    }

    // Allocate and initialize UE object
    std::unique_ptr<ue_nr> ue_ptr = std::unique_ptr<ue_nr>(new ue_nr(rnti, enb_cc_idx, &sched, rrc, rlc, phy, logger));

    // Add UE to rnti map
    srsran::rwlock_write_guard rw_lock(rwmutex);
    if (not is_rnti_valid_nolock(rnti)) {
      continue;
    }
    auto ret = ue_db.insert(rnti, std::move(ue_ptr));
    if (ret.has_value()) {
      inserted_ue = ret.value()->second.get();
    } else {
      logger.info("Failed to allocate rnti=0x%x. Attempting a different rnti.", rnti);
    }
  } while (inserted_ue == nullptr);

  return rnti;
}

// Remove UE from the perspective of L2/L3
int mac_nr::remove_ue(uint16_t rnti)
{
  srsran::rwlock_write_guard lock(rwmutex);
  if (is_rnti_active_nolock(rnti)) {
    sched.ue_rem(rnti);
    ue_db.erase(rnti);
  } else {
    logger.error("User rnti=0x%x not found", rnti);
    return SRSRAN_ERROR;
  }

  return SRSRAN_SUCCESS;
}

bool mac_nr::is_rnti_valid_nolock(uint16_t rnti)
{
  if (not started) {
    logger.info("RACH ignored as eNB is being shutdown");
    return false;
  }
  if (ue_db.full()) {
    logger.warning("Maximum number of connected UEs %zd connected to the eNB. Ignoring PRACH", SRSENB_MAX_UES);
    return false;
  }
  if (not ue_db.has_space(rnti)) {
    logger.info("Failed to allocate rnti=0x%x. Attempting a different rnti.", rnti);
    return false;
  }
  return true;
}

bool mac_nr::is_rnti_active_nolock(uint16_t rnti)
{
  if (not ue_db.contains(rnti)) {
    logger.error("User rnti=0x%x not found", rnti);
    return false;
  }
  return ue_db[rnti]->is_active();
}

int mac_nr::rlc_buffer_state(uint16_t rnti, uint32_t lc_id, uint32_t tx_queue, uint32_t retx_queue)
{
  sched.dl_buffer_state(rnti, lc_id, tx_queue, retx_queue);
  return SRSRAN_SUCCESS;
}

void mac_nr::ul_bsr(uint16_t rnti, uint32_t lcid, uint32_t bsr)
{
  sched.ul_bsr(rnti, lcid, bsr);
}

int mac_nr::slot_indication(const srsran_slot_cfg_t& slot_cfg)
{
  return 0;
}

int mac_nr::get_dl_sched(const srsran_slot_cfg_t& slot_cfg, dl_sched_t& dl_sched)
{
  slot_point pdsch_slot = srsran::slot_point{NUMEROLOGY_IDX, slot_cfg.idx};

  logger.set_context((pdsch_slot - TX_ENB_DELAY).to_uint());

  // Run Scheduler
  sched_nr_interface::sched_rar_list_t rar_list;
  sched_nr_interface::dl_res_t         dl_res(rar_list, dl_sched);

  int ret = sched.run_slot(pdsch_slot, 0, dl_res);
  if (ret != SRSRAN_SUCCESS) {
    return ret;
  }

  // Generate MAC DL PDUs
  uint32_t                  rar_count = 0;
  srsran::rwlock_read_guard rw_lock(rwmutex);
  for (pdsch_t& pdsch : dl_sched.pdsch) {
    if (pdsch.sch.grant.rnti_type == srsran_rnti_type_c) {
      uint16_t rnti = pdsch.sch.grant.rnti;
      if (not is_rnti_active_nolock(rnti)) {
        continue;
      }
      for (auto& tb_data : pdsch.data) {
        if (tb_data != nullptr and tb_data->N_bytes == 0) {
          // TODO: exclude retx from packing
          ue_db[rnti]->generate_pdu(tb_data, pdsch.sch.grant.tb->tbs / 8);

          if (pcap != nullptr) {
            uint32_t pid = 0; // TODO: get PID from PDCCH struct?
            pcap->write_dl_crnti_nr(tb_data->msg, tb_data->N_bytes, rnti, pid, slot_cfg.idx);
          }
          ue_db[rnti]->metrics_dl_mcs(pdsch.sch.grant.tb->mcs);
        }
      }
    } else if (pdsch.sch.grant.rnti_type == srsran_rnti_type_ra) {
      sched_nr_interface::rar_t& rar = dl_res.rar[rar_count++];
      // for RARs we could actually move the byte_buffer to the PHY, as there are no retx
      pdsch.data[0] = assemble_rar(rar.grants);
    }
  }
  for (auto& u : ue_db) {
    u.second->metrics_cnt();
  }
  return SRSRAN_SUCCESS;
}

int mac_nr::get_ul_sched(const srsran_slot_cfg_t& slot_cfg, ul_sched_t& ul_sched)
{
  int ret = 0;

  slot_point pusch_slot = srsran::slot_point{NUMEROLOGY_IDX, slot_cfg.idx};
  ret                   = sched.get_ul_sched(pusch_slot, 0, ul_sched);

  srsran::rwlock_read_guard rw_lock(rwmutex);
  for (auto& pusch : ul_sched.pusch) {
    if (ue_db.contains(pusch.sch.grant.rnti)) {
      ue_db[pusch.sch.grant.rnti]->metrics_ul_mcs(pusch.sch.grant.tb->mcs);
    }
  }
  return ret;
}

int mac_nr::pucch_info(const srsran_slot_cfg_t& slot_cfg, const mac_interface_phy_nr::pucch_info_t& pucch_info)
{
  if (not handle_uci_data(pucch_info.uci_data.cfg.pucch.rnti, pucch_info.uci_data.cfg, pucch_info.uci_data.value)) {
    logger.error("Error handling UCI data from PUCCH reception");
    return SRSRAN_ERROR;
  }

  // process PUCCH SNR
  uint16_t                  rnti = pucch_info.uci_data.cfg.pucch.rnti;
  srsran::rwlock_read_guard rw_lock(rwmutex);
  if (ue_db.contains(rnti)) {
    ue_db[rnti]->metrics_pucch_sinr(pucch_info.csi.snr_dB);
  }

  return SRSRAN_SUCCESS;
}

bool mac_nr::handle_uci_data(const uint16_t rnti, const srsran_uci_cfg_nr_t& cfg_, const srsran_uci_value_nr_t& value)
{
  // Process HARQ-ACK
  for (uint32_t i = 0; i < cfg_.ack.count; i++) {
    const srsran_harq_ack_bit_t* ack_bit = &cfg_.ack.bits[i];
    bool                         is_ok   = (value.ack[i] == 1) and value.valid;
    sched.dl_ack_info(rnti, 0, ack_bit->pid, 0, is_ok);
    srsran::rwlock_read_guard rw_lock(rwmutex);
    if (ue_db.contains(rnti)) {
      ue_db[rnti]->metrics_tx(is_ok, 0 /*TODO get size of packet from scheduler somehow*/);
    }
  }

  // Process SR
  if (value.valid and value.sr > 0) {
    sched.ul_sr_info(cfg_.pucch.rnti);
  }

  // Process CQI
  {
    srsran::rwlock_read_guard rw_lock(rwmutex);
    if (ue_db.contains(rnti) && value.valid) {
      ue_db[rnti]->metrics_dl_cqi(cfg_, value.csi->wideband_cri_ri_pmi_cqi.cqi);
    }
  }

  return true;
}

int mac_nr::pusch_info(const srsran_slot_cfg_t& slot_cfg, mac_interface_phy_nr::pusch_info_t& pusch_info)
{
  uint16_t rnti      = pusch_info.rnti;
  uint32_t nof_bytes = pusch_info.pdu->N_bytes;

  // Handle UCI data
  if (not handle_uci_data(rnti, pusch_info.uci_cfg, pusch_info.pusch_data.uci)) {
    logger.error("Error handling UCI data from PUCCH reception");
    return SRSRAN_ERROR;
  }

  sched.ul_crc_info(rnti, 0, pusch_info.pid, pusch_info.pusch_data.tb[0].crc);

  // process only PDUs with CRC=OK
  if (pusch_info.pusch_data.tb[0].crc) {
    if (pcap) {
      pcap->write_ul_crnti_nr(
          pusch_info.pdu->msg, pusch_info.pdu->N_bytes, pusch_info.rnti, pusch_info.pid, slot_cfg.idx);
    }

    auto process_pdu_task = [this, rnti](srsran::unique_byte_buffer_t& pdu) {
      srsran::rwlock_read_guard lock(rwmutex);
      if (is_rnti_active_nolock(rnti)) {
        ue_db[rnti]->process_pdu(std::move(pdu));
      } else {
        logger.debug("Discarding PDU rnti=0x%x", rnti);
      }
    };
    stack_task_queue.try_push(std::bind(process_pdu_task, std::move(pusch_info.pdu)));
  }
  srsran::rwlock_read_guard rw_lock(rwmutex);
  if (ue_db.contains(rnti)) {
    ue_db[rnti]->metrics_rx(pusch_info.pusch_data.tb[0].crc, nof_bytes);
    ue_db[rnti]->metrics_pusch_sinr(pusch_info.csi.snr_dB);
  }
  return SRSRAN_SUCCESS;
}

srsran::byte_buffer_t* mac_nr::assemble_rar(srsran::const_span<sched_nr_interface::msg3_grant_t> grants)
{
  srsran::mac_rar_pdu_nr rar_pdu;

  uint32_t pdsch_tbs = 10; // FIXME: how big is the PDSCH?
  rar_pdu.init_tx(rar_pdu_buffer.get(), pdsch_tbs);

  for (auto& rar_grant : grants) {
    srsran::mac_rar_subpdu_nr& rar_subpdu = rar_pdu.add_subpdu();

    // set values directly coming from scheduler
    rar_subpdu.set_ta(rar_grant.data.ta_cmd);
    rar_subpdu.set_rapid(rar_grant.data.preamble_idx);
    rar_subpdu.set_temp_crnti(rar_grant.data.temp_crnti);

    // convert Msg3 grant to raw UL grant
    srsran_dci_nr_t     dci     = {};
    srsran_dci_msg_nr_t dci_msg = {};
    if (srsran_dci_nr_ul_pack(&dci, &rar_grant.msg3_dci, &dci_msg) != SRSRAN_SUCCESS) {
      logger.error("Couldn't pack Msg3 UL grant");
      return nullptr;
    }

    if (logger.info.enabled()) {
      std::array<char, 512> str;
      srsran_dci_ul_nr_to_str(&dci, &rar_grant.msg3_dci, str.data(), str.size());
      logger.info("Setting RAR Grant %s", str.data());
    }

    // copy only the required bits
    std::array<uint8_t, SRSRAN_RAR_UL_GRANT_NBITS> packed_ul_grant = {};
    std::copy(
        std::begin(dci_msg.payload), std::begin(dci_msg.payload) + SRSRAN_RAR_UL_GRANT_NBITS, packed_ul_grant.begin());
    rar_subpdu.set_ul_grant(packed_ul_grant);
  }

  if (rar_pdu.pack() != SRSRAN_SUCCESS) {
    logger.error("Couldn't assemble RAR PDU");
    return nullptr;
  }

  fmt::memory_buffer buff;
  rar_pdu.to_string(buff);
  logger.info("DL %s", srsran::to_c_str(buff));

  return rar_pdu_buffer.get();
}

} // namespace srsenb
