/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsenb/hdr/stack/mac/nr/mac_nr.h"
#include "srsenb/test/mac/nr/sched_nr_cfg_generators.h"
#include "srsran/common/buffer_pool.h"
#include "srsran/common/log_helper.h"
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

mac_nr::mac_nr(srsran::task_sched_handle task_sched_, const sched_nr_interface::sched_cfg_t& sched_cfg) :
  logger(srslog::fetch_basic_logger("MAC-NR")),
  task_sched(task_sched_),
  sched(sched_cfg),
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

void mac_nr::get_metrics(srsenb::mac_metrics_t& metrics)
{
  srsran::rwlock_read_guard lock(rwlock);
  metrics.ues.reserve(ue_db.size());
  for (auto& u : ue_db) {
    if (not sched.ue_exists(u.first)) {
      continue;
    }
    metrics.ues.emplace_back();
    u.second->metrics_read(&metrics.ues.back());
  }
  metrics.cc_info.resize(detected_rachs.size());
  for (unsigned cc = 0, e = detected_rachs.size(); cc != e; ++cc) {
    metrics.cc_info[cc].cc_rach_counter = detected_rachs[cc];
    metrics.cc_info[cc].pci             = (cc < cell_config.size()) ? cell_config[cc].carrier.pci : 0;
  }
}

int mac_nr::cell_cfg(const sched_interface::cell_cfg_t&                 eutra_cell_config,
                     srsran::const_span<sched_nr_interface::cell_cfg_t> nr_cells)
{
  cell_config = nr_cells;
  sched.cell_cfg(nr_cells);
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

uint16_t mac_nr::reserve_rnti(uint32_t enb_cc_idx)
{
  uint16_t rnti = alloc_ue(enb_cc_idx);
  if (rnti == SRSRAN_INVALID_RNTI) {
    return rnti;
  }

  // Add new user to the scheduler so that it can RX/TX SRB0
  srsenb::sched_nr_interface::ue_cfg_t ue_cfg = srsenb::get_default_ue_cfg(1);
  ue_cfg.fixed_dl_mcs                         = args.fixed_dl_mcs;
  ue_cfg.fixed_ul_mcs                         = args.fixed_ul_mcs;
  sched.ue_cfg(rnti, ue_cfg);

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
    uint16_t rnti = reserve_rnti(enb_cc_idx);

    // Log this event.
    ++detected_rachs[enb_cc_idx];

    // Trigger scheduler RACH
    srsenb::sched_nr_interface::dl_sched_rar_info_t rar_info = {};
    rar_info.preamble_idx                                    = rach_info.preamble;
    rar_info.temp_crnti                                      = rnti;
    rar_info.ta_cmd                                          = rach_info.time_adv;
    rar_info.prach_slot                                      = slot_point{NUMEROLOGY_IDX, rach_info.slot_index};
    // TODO: fill remaining fields as required
    sched.dl_rach_info(enb_cc_idx, rar_info);

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
      srsran::rwlock_read_guard read_lock(rwlock);
      if (not is_rnti_valid_unsafe(rnti)) {
        continue;
      }
    }

    // Allocate and initialize UE object
    std::unique_ptr<ue_nr> ue_ptr = std::unique_ptr<ue_nr>(new ue_nr(rnti, enb_cc_idx, &sched, rrc, rlc, phy, logger));

    // Add UE to rnti map
    srsran::rwlock_write_guard rw_lock(rwlock);
    if (not is_rnti_valid_unsafe(rnti)) {
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
  srsran::rwlock_write_guard lock(rwlock);
  if (is_rnti_active_unsafe(rnti)) {
    ue_db.erase(rnti);
  } else {
    logger.error("User rnti=0x%x not found", rnti);
    return SRSRAN_ERROR;
  }

  return SRSRAN_SUCCESS;
}

bool mac_nr::is_rnti_valid_unsafe(uint16_t rnti)
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

bool mac_nr::is_rnti_active_unsafe(uint16_t rnti)
{
  if (not ue_db.contains(rnti)) {
    logger.error("User rnti=0x%x not found", rnti);
    return false;
  }
  return ue_db[rnti]->is_active();
}

int mac_nr::slot_indication(const srsran_slot_cfg_t& slot_cfg)
{
  return 0;
}

int mac_nr::get_dl_sched(const srsran_slot_cfg_t& slot_cfg, dl_sched_t& dl_sched)
{
  logger.set_context(slot_cfg.idx);

  if (not pdsch_slot.valid()) {
    pdsch_slot = srsran::slot_point{NUMEROLOGY_IDX, slot_cfg.idx};
  } else {
    pdsch_slot++;
  }

  sched_nr_interface::dl_sched_res_t dl_res;
  int                                ret = sched.get_dl_sched(pdsch_slot, 0, dl_res);
  if (ret != SRSRAN_SUCCESS) {
    return ret;
  }
  dl_sched = dl_res.dl_sched;

  uint32_t                  rar_count = 0;
  srsran::rwlock_read_guard rw_lock(rwlock);
  for (pdsch_t& pdsch : dl_sched.pdsch) {
    if (pdsch.sch.grant.rnti_type == srsran_rnti_type_c) {
      uint16_t rnti = pdsch.sch.grant.rnti;
      if (not is_rnti_active_unsafe(rnti)) {
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
        }
      }
    } else if (pdsch.sch.grant.rnti_type == srsran_rnti_type_ra) {
      sched_nr_interface::sched_rar_t& rar = dl_res.rar[rar_count++];
      // for RARs we could actually move the byte_buffer to the PHY, as there are no retx
      pdsch.data[0]                        = assemble_rar(rar.grants);
    }
  }
  return SRSRAN_SUCCESS;
}

int mac_nr::get_ul_sched(const srsran_slot_cfg_t& slot_cfg, ul_sched_t& ul_sched)
{
  if (not pusch_slot.valid()) {
    pusch_slot = srsran::slot_point{NUMEROLOGY_IDX, slot_cfg.idx};
  } else {
    pusch_slot++;
  }

  return sched.get_ul_sched(pusch_slot, 0, ul_sched);
}

int mac_nr::pucch_info(const srsran_slot_cfg_t& slot_cfg, const mac_interface_phy_nr::pucch_info_t& pucch_info)
{
  if (not handle_uci_data(pucch_info.uci_data.cfg.pucch.rnti, pucch_info.uci_data.cfg, pucch_info.uci_data.value)) {
    logger.error("Error handling UCI data from PUCCH reception");
    return SRSRAN_ERROR;
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
  }
  return true;
}

int mac_nr::pusch_info(const srsran_slot_cfg_t& slot_cfg, mac_interface_phy_nr::pusch_info_t& pusch_info)
{
  uint16_t rnti = pusch_info.rnti;

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
      srsran::rwlock_read_guard lock(rwlock);
      if (is_rnti_active_unsafe(rnti)) {
        ue_db[rnti]->process_pdu(std::move(pdu));
      } else {
        logger.debug("Discarding PDU rnti=0x%x", rnti);
      }
    };
    stack_task_queue.try_push(std::bind(process_pdu_task, std::move(pusch_info.pdu)));
  }

  return SRSRAN_SUCCESS;
}

srsran::byte_buffer_t* mac_nr::assemble_rar(srsran::const_span<sched_nr_interface::sched_rar_grant_t> grants)
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
    std::copy(std::begin(dci_msg.payload), std::begin(dci_msg.payload)+SRSRAN_RAR_UL_GRANT_NBITS, packed_ul_grant.begin());
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
