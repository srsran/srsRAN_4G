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

#include "srsenb/hdr/stack/mac/mac_nr.h"
#include "srsenb/test/mac/nr/sched_nr_cfg_generators.h"
#include "srsran/common/buffer_pool.h"
#include "srsran/common/log_helper.h"
#include "srsran/common/rwlock_guard.h"
#include "srsran/common/standard_streams.h"
#include "srsran/common/time_prof.h"
#include <pthread.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>

namespace srsenb {

mac_nr::mac_nr(srsran::task_sched_handle task_sched_) :
  logger(srslog::fetch_basic_logger("MAC-NR")),
  task_sched(task_sched_),
  sched(srsenb::sched_nr_interface::sched_cfg_t{})
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

  // configure scheduler for 1 carrier
  std::vector<srsenb::sched_nr_interface::cell_cfg_t> cells_cfg = srsenb::get_default_cells_cfg(1);
  sched.cell_cfg(cells_cfg);

  detected_rachs.resize(cells_cfg.size());

  bcch_bch_payload = srsran::make_byte_buffer();
  if (bcch_bch_payload == nullptr) {
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

int mac_nr::cell_cfg(srsenb::sched_interface::cell_cfg_t* cell_cfg)
{
  cfg = *cell_cfg;

  // read SIBs from RRC (SIB1 for now only)
  for (int i = 0; i < 1 /* srsenb::sched_interface::MAX_SIBS */; i++) {
    if (cell_cfg->sibs->len > 0) {
      sib_info_t sib  = {};
      sib.index       = i;
      sib.periodicity = cell_cfg->sibs->period_rf;
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

void mac_nr::rach_detected(const rach_info_t& rach_info)
{
  static srsran::mutexed_tprof<srsran::avg_time_stats> rach_tprof("rach_tprof", "MAC-NR", 1);
  logger.set_context(rach_info.slot_index);
  auto rach_tprof_meas = rach_tprof.start();

  uint32_t enb_cc_idx = 0;
  stack_task_queue.push([this, rach_info, enb_cc_idx, rach_tprof_meas]() mutable {
    uint16_t rnti = add_ue(enb_cc_idx);
    if (rnti == SRSRAN_INVALID_RNTI) {
      return;
    }

    rach_tprof_meas.defer_stop();

    // TODO: Generate RAR data
    // ..

    // Log this event.
    ++detected_rachs[enb_cc_idx];

    // Add new user to the scheduler so that it can RX/TX SRB0
    srsenb::sched_nr_interface::ue_cfg_t ue_cfg = srsenb::get_default_ue_cfg(1);
    ue_cfg.fixed_dl_mcs                         = args.fixed_dl_mcs;
    ue_cfg.fixed_ul_mcs                         = args.fixed_ul_mcs;
    sched.ue_cfg(rnti, ue_cfg);

    // Register new user in RRC
    if (rrc->add_user(rnti) == SRSRAN_ERROR) {
      // ue_rem(rnti);
      return;
    }

    // Trigger scheduler RACH
    srsenb::sched_nr_interface::dl_sched_rar_info_t rar_info = {};
    rar_info.preamble_idx                                    = rach_info.preamble;
    rar_info.temp_crnti                                      = rnti;
    rar_info.prach_slot                                      = slot_point(0, rach_info.slot_index);
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

uint16_t mac_nr::add_ue(uint32_t enb_cc_idx)
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
    // TODO: add sched interface
    std::unique_ptr<ue_nr> ue_ptr = std::unique_ptr<ue_nr>(new ue_nr(rnti, enb_cc_idx, nullptr, rrc, rlc, phy, logger));

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

  // Set PCAP if available
  // ..

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

uint16_t mac_nr::reserve_rnti()
{
  uint16_t rnti = add_ue(0);
  if (rnti == SRSRAN_INVALID_RNTI) {
    return rnti;
  }

  return rnti;
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
  if (not pdsch_slot.valid()) {
    pdsch_slot = srsran::slot_point{NUMEROLOGY_IDX, slot_cfg.idx};
  } else {
    pdsch_slot++;
  }

  int ret = sched.get_dl_sched(pdsch_slot, 0, dl_sched);
  for (pdsch_t& pdsch : dl_sched.pdsch) {
    // Set TBS
    // Select grant and set data
    pdsch.data[0] = nullptr; // FIXME: add ptr to PDU
    pdsch.data[1] = nullptr;
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

  int ret = sched.get_ul_sched(pusch_slot, 0, ul_sched);
  for (pusch_t& pusch : ul_sched.pusch) {
    pusch.data[0] = nullptr; // FIXME: add ptr to data to be filled
    pusch.data[1] = nullptr;
  }

  return SRSRAN_SUCCESS;
}
int mac_nr::pucch_info(const srsran_slot_cfg_t& slot_cfg, const mac_interface_phy_nr::pucch_info_t& pucch_info)
{
  // FIXME: provide CRC/ACK feedback
  // sched.dl_ack_info(rnti_, cc, pid, tb_idx, ack);
  // sched.ul_crc_info(rnti_, cc, pid, crc);
  return SRSRAN_SUCCESS;
}
int mac_nr::pusch_info(const srsran_slot_cfg_t& slot_cfg, const mac_interface_phy_nr::pusch_info_t& pusch_info)
{
  // FIXME: does the PUSCH info call include received PDUs?
  uint16_t                     rnti = pusch_info.rnti;
  srsran::unique_byte_buffer_t rx_pdu;
  auto                         process_pdu_task = [this, rnti](srsran::unique_byte_buffer_t& pdu) {
    srsran::rwlock_read_guard lock(rwlock);
    if (is_rnti_active_unsafe(rnti)) {
      ue_db[rnti]->process_pdu(std::move(pdu));
    } else {
      logger.debug("Discarding PDU rnti=0x%x", rnti);
    }
  };
  stack_task_queue.try_push(std::bind(process_pdu_task, std::move(rx_pdu)));

  return SRSRAN_SUCCESS;
}

} // namespace srsenb
