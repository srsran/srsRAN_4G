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

#include <pthread.h>
#include <string.h>

#include "srsenb/hdr/stack/mac/mac.h"
#include "srsran/adt/pool/obj_pool.h"
#include "srsran/common/rwlock_guard.h"
#include "srsran/common/standard_streams.h"
#include "srsran/common/time_prof.h"
#include "srsran/interfaces/enb_phy_interfaces.h"
#include "srsran/interfaces/enb_rlc_interfaces.h"
#include "srsran/interfaces/enb_rrc_interfaces.h"
#include "srsran/srslog/event_trace.h"

// #define WRITE_SIB_PCAP
using namespace asn1::rrc;

namespace srsenb {

mac::mac(srsran::ext_task_sched_handle task_sched_, srslog::basic_logger& logger) :
  logger(logger), rar_payload(), common_buffers(SRSRAN_MAX_CARRIERS), task_sched(task_sched_)
{
  pthread_rwlock_init(&rwlock, nullptr);
}

mac::~mac()
{
  stop();
  pthread_rwlock_destroy(&rwlock);
}

bool mac::init(const mac_args_t&        args_,
               const cell_list_t&       cells_,
               phy_interface_stack_lte* phy,
               rlc_interface_mac*       rlc,
               rrc_interface_mac*       rrc)
{
  started = false;

  if (not phy or not rlc) {
    return false;
  }
  phy_h = phy;
  rlc_h = rlc;
  rrc_h = rrc;

  args  = args_;
  cells = cells_;

  stack_task_queue = task_sched.make_task_queue();

  scheduler.init(rrc, args.sched);

  // Init softbuffer for SI messages
  common_buffers.resize(cells.size());
  for (auto& cc : common_buffers) {
    for (int i = 0; i < NOF_BCCH_DLSCH_MSG; i++) {
      srsran_softbuffer_tx_init(&cc.bcch_softbuffer_tx[i], args.nof_prb);
    }
    // Init softbuffer for PCCH
    srsran_softbuffer_tx_init(&cc.pcch_softbuffer_tx, args.nof_prb);

    // Init softbuffer for RAR
    srsran_softbuffer_tx_init(&cc.rar_softbuffer_tx, args.nof_prb);
  }

  reset();

  // Initiate common pool of softbuffers
  uint32_t nof_prb          = args.nof_prb;
  auto     init_softbuffers = [nof_prb](void* ptr) {
    new (ptr) ue_cc_softbuffers(nof_prb, SRSRAN_FDD_NOF_HARQ, SRSRAN_FDD_NOF_HARQ);
  };
  auto recycle_softbuffers = [](ue_cc_softbuffers& softbuffers) { softbuffers.clear(); };
  softbuffer_pool.reset(new srsran::background_obj_pool<ue_cc_softbuffers>(
      8, 8, args.max_nof_ues, init_softbuffers, recycle_softbuffers));

  // Pre-alloc UE objects for first attaching users
  prealloc_ue(10);

  detected_rachs.resize(cells.size());

  started = true;
  return true;
}

void mac::stop()
{
  srsran::rwlock_write_guard lock(rwlock);
  if (started) {
    started = false;

    ue_db.clear();
    for (auto& cc : common_buffers) {
      for (int i = 0; i < NOF_BCCH_DLSCH_MSG; i++) {
        srsran_softbuffer_tx_free(&cc.bcch_softbuffer_tx[i]);
      }
      srsran_softbuffer_tx_free(&cc.pcch_softbuffer_tx);
      srsran_softbuffer_tx_free(&cc.rar_softbuffer_tx);
    }
    ue_pool.stop();
  }
}

// Implement Section 5.9
void mac::reset()
{
  logger.info("Resetting MAC");

  last_rnti = 70;

  /* Setup scheduler */
  scheduler.reset();
}

void mac::start_pcap(srsran::mac_pcap* pcap_)
{
  pcap = pcap_;
  // Set pcap in all UEs for UL messages
  for (auto& u : ue_db) {
    u.second->start_pcap(pcap);
  }
}

void mac::start_pcap_net(srsran::mac_pcap_net* pcap_net_)
{
  pcap_net = pcap_net_;
  // Set pcap in all UEs for UL messages
  for (auto& u : ue_db) {
    u.second->start_pcap_net(pcap_net);
  }
}

/********************************************************
 *
 * RLC interface
 *
 *******************************************************/
int mac::rlc_buffer_state(uint16_t rnti, uint32_t lc_id, uint32_t tx_queue, uint32_t retx_queue)
{
  srsran::rwlock_read_guard lock(rwlock);
  int                       ret = -1;
  if (ue_db.contains(rnti)) {
    if (rnti != SRSRAN_MRNTI) {
      ret = scheduler.dl_rlc_buffer_state(rnti, lc_id, tx_queue, retx_queue);
    } else {
      for (uint32_t i = 0; i < mch.num_mtch_sched; i++) {
        if (lc_id == mch.mtch_sched[i].lcid) {
          mch.mtch_sched[i].lcid_buffer_size = tx_queue;
        }
      }
      ret = 0;
    }
  } else {
    logger.error("User rnti=0x%x not found", rnti);
  }
  return ret;
}

int mac::bearer_ue_cfg(uint16_t rnti, uint32_t lc_id, sched_interface::ue_bearer_cfg_t* cfg)
{
  int                       ret = -1;
  srsran::rwlock_read_guard lock(rwlock);
  if (ue_db.contains(rnti)) {
    ret = scheduler.bearer_ue_cfg(rnti, lc_id, *cfg);
  } else {
    logger.error("User rnti=0x%x not found", rnti);
  }
  return ret;
}

int mac::bearer_ue_rem(uint16_t rnti, uint32_t lc_id)
{
  srsran::rwlock_read_guard lock(rwlock);
  int                       ret = -1;
  if (ue_db.contains(rnti)) {
    ret = scheduler.bearer_ue_rem(rnti, lc_id);
  } else {
    logger.error("User rnti=0x%x not found", rnti);
  }
  return ret;
}

void mac::phy_config_enabled(uint16_t rnti, bool enabled)
{
  scheduler.phy_config_enabled(rnti, enabled);
}

// Update UE configuration
int mac::ue_cfg(uint16_t rnti, sched_interface::ue_cfg_t* cfg)
{
  srsran::rwlock_read_guard lock(rwlock);

  auto it     = ue_db.find(rnti);
  ue*  ue_ptr = nullptr;
  if (it == ue_db.end()) {
    logger.error("User rnti=0x%x not found", rnti);
    return SRSRAN_ERROR;
  }
  ue_ptr = it->second.get();

  // Start TA FSM in UE entity
  ue_ptr->start_ta();

  // Update Scheduler configuration
  if (cfg != nullptr and scheduler.ue_cfg(rnti, *cfg) == SRSRAN_ERROR) {
    logger.error("Registering new UE rnti=0x%x to SCHED", rnti);
    return SRSRAN_ERROR;
  }
  return SRSRAN_SUCCESS;
}

// Removes UE from DB
int mac::ue_rem(uint16_t rnti)
{
  // Remove UE from the perspective of L2/L3
  {
    srsran::rwlock_write_guard lock(rwlock);
    if (ue_db.contains(rnti)) {
      ues_to_rem[rnti] = std::move(ue_db[rnti]);
      ue_db.erase(rnti);
    } else {
      logger.error("User rnti=0x%x not found", rnti);
      return SRSRAN_ERROR;
    }
  }
  scheduler.ue_rem(rnti);

  // Remove UE from the perspective of L1
  // Note: Let any pending retx ACK to arrive, so that PHY recognizes rnti
  task_sched.defer_callback(FDD_HARQ_DELAY_DL_MS + FDD_HARQ_DELAY_UL_MS, [this, rnti]() {
    phy_h->rem_rnti(rnti);
    ues_to_rem.erase(rnti);
    logger.info("User rnti=0x%x removed from MAC/PHY", rnti);
  });
  return SRSRAN_SUCCESS;
}

// Called after Msg3
int mac::ue_set_crnti(uint16_t temp_crnti, uint16_t crnti, sched_interface::ue_cfg_t* cfg)
{
  srsran::rwlock_read_guard lock(rwlock);
  if (temp_crnti != crnti) {
    // if C-RNTI is changed, it corresponds to older user. Handover scenario.
    ue_db[crnti]->reset();
  } else {
    // Schedule ConRes Msg4
    scheduler.dl_mac_buffer_state(crnti, (uint32_t)srsran::dl_sch_lcid::CON_RES_ID);
  }
  int ret = ue_cfg(crnti, cfg);
  if (ret != SRSRAN_SUCCESS) {
    return ret;
  }
  return ret;
}

int mac::cell_cfg(const std::vector<sched_interface::cell_cfg_t>& cell_cfg_)
{
  cell_config = cell_cfg_;
  return scheduler.cell_cfg(cell_config);
}

void mac::get_metrics(mac_metrics_t& metrics)
{
  srsran::rwlock_read_guard lock(rwlock);
  metrics.ues.reserve(ue_db.size());
  for (auto& u : ue_db) {
    if (not scheduler.ue_exists(u.first)) {
      continue;
    }
    metrics.ues.emplace_back();
    u.second->metrics_read(&metrics.ues.back());
  }
  metrics.cc_rach_counter = detected_rachs;
}

/********************************************************
 *
 * PHY interface
 *
 *******************************************************/

int mac::ack_info(uint32_t tti_rx, uint16_t rnti, uint32_t enb_cc_idx, uint32_t tb_idx, bool ack)
{
  logger.set_context(tti_rx);
  srsran::rwlock_read_guard lock(rwlock);

  if (not check_ue_exists(rnti)) {
    return SRSRAN_ERROR;
  }

  int nof_bytes = scheduler.dl_ack_info(tti_rx, rnti, enb_cc_idx, tb_idx, ack);
  ue_db[rnti]->metrics_tx(ack, nof_bytes);

  rrc_h->set_radiolink_dl_state(rnti, ack);

  return SRSRAN_SUCCESS;
}

int mac::crc_info(uint32_t tti_rx, uint16_t rnti, uint32_t enb_cc_idx, uint32_t nof_bytes, bool crc)
{
  logger.set_context(tti_rx);
  srsran::rwlock_read_guard lock(rwlock);

  if (not check_ue_exists(rnti)) {
    return SRSRAN_ERROR;
  }

  ue_db[rnti]->set_tti(tti_rx);
  ue_db[rnti]->metrics_rx(crc, nof_bytes);

  rrc_h->set_radiolink_ul_state(rnti, crc);

  // Scheduler uses eNB's CC mapping
  return scheduler.ul_crc_info(tti_rx, rnti, enb_cc_idx, crc);
}

int mac::push_pdu(uint32_t tti_rx, uint16_t rnti, uint32_t enb_cc_idx, uint32_t nof_bytes, bool crc)
{
  srsran::rwlock_read_guard lock(rwlock);

  if (not check_ue_exists(rnti)) {
    return SRSRAN_ERROR;
  }

  std::array<int, SRSRAN_MAX_CARRIERS> enb_ue_cc_map = scheduler.get_enb_ue_cc_map(rnti);
  if (enb_ue_cc_map[enb_cc_idx] < 0) {
    logger.error("User rnti=0x%x is not activated for carrier %d", rnti, enb_cc_idx);
    return SRSRAN_ERROR;
  }
  uint32_t ue_cc_idx = enb_ue_cc_map[enb_cc_idx];

  // push the pdu through the queue if received correctly
  if (crc) {
    logger.info("Pushing PDU rnti=0x%x, tti_rx=%d, nof_bytes=%d", rnti, tti_rx, nof_bytes);
    ue_db[rnti]->push_pdu(tti_rx, ue_cc_idx, nof_bytes);
    stack_task_queue.push([this]() { process_pdus(); });
  } else {
    logger.debug("Discarting PDU rnti=0x%x, tti_rx=%d, nof_bytes=%d", rnti, tti_rx, nof_bytes);
    ue_db[rnti]->deallocate_pdu(tti_rx, ue_cc_idx);
  }
  return SRSRAN_SUCCESS;
}

int mac::ri_info(uint32_t tti, uint16_t rnti, uint32_t enb_cc_idx, uint32_t ri_value)
{
  logger.set_context(tti);
  srsran::rwlock_read_guard lock(rwlock);

  if (not check_ue_exists(rnti)) {
    return SRSRAN_ERROR;
  }

  scheduler.dl_ri_info(tti, rnti, enb_cc_idx, ri_value);
  ue_db[rnti]->metrics_dl_ri(ri_value);

  return SRSRAN_SUCCESS;
}

int mac::pmi_info(uint32_t tti, uint16_t rnti, uint32_t enb_cc_idx, uint32_t pmi_value)
{
  logger.set_context(tti);
  srsran::rwlock_read_guard lock(rwlock);

  if (not check_ue_exists(rnti)) {
    return SRSRAN_ERROR;
  }

  scheduler.dl_pmi_info(tti, rnti, enb_cc_idx, pmi_value);
  ue_db[rnti]->metrics_dl_pmi(pmi_value);

  return SRSRAN_SUCCESS;
}

int mac::cqi_info(uint32_t tti, uint16_t rnti, uint32_t enb_cc_idx, uint32_t cqi_value)
{
  logger.set_context(tti);
  srsran::rwlock_read_guard lock(rwlock);

  if (not check_ue_exists(rnti)) {
    return SRSRAN_ERROR;
  }

  scheduler.dl_cqi_info(tti, rnti, enb_cc_idx, cqi_value);
  ue_db[rnti]->metrics_dl_cqi(cqi_value);

  return SRSRAN_SUCCESS;
}

int mac::snr_info(uint32_t tti_rx, uint16_t rnti, uint32_t enb_cc_idx, float snr, ul_channel_t ch)
{
  logger.set_context(tti_rx);
  srsran::rwlock_read_guard lock(rwlock);

  if (not check_ue_exists(rnti)) {
    return SRSRAN_ERROR;
  }

  return scheduler.ul_snr_info(tti_rx, rnti, enb_cc_idx, snr, (uint32_t)ch);
}

int mac::ta_info(uint32_t tti, uint16_t rnti, float ta_us)
{
  srsran::rwlock_read_guard lock(rwlock);

  if (not check_ue_exists(rnti)) {
    return SRSRAN_ERROR;
  }

  uint32_t nof_ta_count = ue_db[rnti]->set_ta_us(ta_us);
  if (nof_ta_count) {
    scheduler.dl_mac_buffer_state(rnti, (uint32_t)srsran::dl_sch_lcid::TA_CMD, nof_ta_count);
  }
  return SRSRAN_SUCCESS;
}

int mac::sr_detected(uint32_t tti, uint16_t rnti)
{
  logger.set_context(tti);
  srsran::rwlock_read_guard lock(rwlock);

  if (not check_ue_exists(rnti)) {
    return SRSRAN_ERROR;
  }

  return scheduler.ul_sr_info(tti, rnti);
}

uint16_t mac::allocate_rnti()
{
  std::lock_guard<std::mutex> lock(rnti_mutex);

  // Assign a c-rnti
  uint16_t rnti = last_rnti++;
  if (last_rnti >= 60000) {
    last_rnti = 70;
  }

  return rnti;
}

uint16_t mac::allocate_ue()
{
  ue* inserted_ue = nullptr;
  do {
    // Get pre-allocated UE object
    std::unique_ptr<ue> ue_ptr;
    if (not ue_pool.try_pop(ue_ptr)) {
      logger.error("UE pool empty. Ignoring RACH attempt.");
      return SRSRAN_INVALID_RNTI;
    }
    uint16_t rnti = ue_ptr->get_rnti();

    // Add UE to map
    {
      srsran::rwlock_write_guard lock(rwlock);
      if (not started) {
        logger.info("RACH ignored as eNB is being shutdown");
        return SRSRAN_INVALID_RNTI;
      }
      if (ue_db.size() >= args.max_nof_ues) {
        logger.warning("Maximum number of connected UEs %zd connected to the eNB. Ignoring PRACH", args.max_nof_ues);
        return SRSRAN_INVALID_RNTI;
      }
      auto ret = ue_db.insert(rnti, std::move(ue_ptr));
      if (ret) {
        inserted_ue = ret.value()->second.get();
      } else {
        logger.info("Failed to allocate rnti=0x%x. Attempting a different rnti.", rnti);
      }
    }

    // Allocate one new UE object in advance
    srsran::get_background_workers().push_task([this]() { prealloc_ue(1); });

  } while (inserted_ue == nullptr);

  // RNTI allocation was successful
  uint16_t rnti = inserted_ue->get_rnti();

  // Set PCAP if available
  if (pcap != nullptr) {
    inserted_ue->start_pcap(pcap);
  }

  if (pcap_net != nullptr) {
    inserted_ue->start_pcap_net(pcap_net);
  }

  return rnti;
}

uint16_t mac::reserve_new_crnti(const sched_interface::ue_cfg_t& ue_cfg)
{
  uint16_t rnti = allocate_ue();
  if (rnti == SRSRAN_INVALID_RNTI) {
    return rnti;
  }

  // Add new user to the scheduler so that it can RX/TX SRB0
  if (scheduler.ue_cfg(rnti, ue_cfg) != SRSRAN_SUCCESS) {
    logger.error("Registering new user rnti=0x%x to SCHED", rnti);
    return SRSRAN_INVALID_RNTI;
  }

  return rnti;
}

void mac::rach_detected(uint32_t tti, uint32_t enb_cc_idx, uint32_t preamble_idx, uint32_t time_adv)
{
  static srsran::mutexed_tprof<srsran::avg_time_stats> rach_tprof("rach_tprof", "MAC", 1);
  logger.set_context(tti);
  auto rach_tprof_meas = rach_tprof.start();

  uint16_t rnti = allocate_ue();
  if (rnti == SRSRAN_INVALID_RNTI) {
    return;
  }

  stack_task_queue.push([this, rnti, tti, enb_cc_idx, preamble_idx, time_adv, rach_tprof_meas]() mutable {
    rach_tprof_meas.defer_stop();
    // Generate RAR data
    sched_interface::dl_sched_rar_info_t rar_info = {};
    rar_info.preamble_idx                         = preamble_idx;
    rar_info.ta_cmd                               = time_adv;
    rar_info.temp_crnti                           = rnti;
    rar_info.msg3_size                            = 7;
    rar_info.prach_tti                            = tti;

    // Log this event.
    ++detected_rachs[enb_cc_idx];

    // Add new user to the scheduler so that it can RX/TX SRB0
    sched_interface::ue_cfg_t ue_cfg = {};
    ue_cfg.supported_cc_list.emplace_back();
    ue_cfg.supported_cc_list.back().active     = true;
    ue_cfg.supported_cc_list.back().enb_cc_idx = enb_cc_idx;
    ue_cfg.ue_bearers[0].direction             = srsenb::sched_interface::ue_bearer_cfg_t::BOTH;
    ue_cfg.supported_cc_list[0].dl_cfg.tm      = SRSRAN_TM1;
    if (scheduler.ue_cfg(rnti, ue_cfg) != SRSRAN_SUCCESS) {
      logger.error("Registering new user rnti=0x%x to SCHED", rnti);
      ue_rem(rnti);
      return;
    }

    // Register new user in RRC
    if (rrc_h->add_user(rnti, ue_cfg) == SRSRAN_ERROR) {
      ue_rem(rnti);
      return;
    }

    // Trigger scheduler RACH
    scheduler.dl_rach_info(enb_cc_idx, rar_info);

    logger.info(
        "RACH:  tti=%d, cc=%d, preamble=%d, offset=%d, temp_crnti=0x%x", tti, enb_cc_idx, preamble_idx, time_adv, rnti);
    srsran::console("RACH:  tti=%d, cc=%d, preamble=%d, offset=%d, temp_crnti=0x%x\n",
                    tti,
                    enb_cc_idx,
                    preamble_idx,
                    time_adv,
                    rnti);
  });
}

void mac::prealloc_ue(uint32_t nof_ue)
{
  for (uint32_t i = 0; i < nof_ue; i++) {
    std::unique_ptr<ue> ptr = std::unique_ptr<ue>(new ue(
        allocate_rnti(), args.nof_prb, &scheduler, rrc_h, rlc_h, phy_h, logger, cells.size(), softbuffer_pool.get()));
    if (not ue_pool.try_push(std::move(ptr))) {
      logger.info("Cannot preallocate more UEs as pool is full");
      return;
    }
  }
}

int mac::get_dl_sched(uint32_t tti_tx_dl, dl_sched_list_t& dl_sched_res_list)
{
  if (!started) {
    return 0;
  }

  trace_complete_event("mac::get_dl_sched", "total_time");
  logger.set_context(TTI_SUB(tti_tx_dl, FDD_HARQ_DELAY_UL_MS));

  for (uint32_t enb_cc_idx = 0; enb_cc_idx < cell_config.size(); enb_cc_idx++) {
    // Run scheduler with current info
    sched_interface::dl_sched_res_t sched_result = {};
    if (scheduler.dl_sched(tti_tx_dl, enb_cc_idx, sched_result) < 0) {
      logger.error("Running scheduler");
      return SRSRAN_ERROR;
    }

    int         n            = 0;
    dl_sched_t* dl_sched_res = &dl_sched_res_list[enb_cc_idx];

    {
      srsran::rwlock_read_guard lock(rwlock);

      // Copy data grants
      for (uint32_t i = 0; i < sched_result.data.size(); i++) {
        uint32_t tb_count = 0;

        // Get UE
        uint16_t rnti = sched_result.data[i].dci.rnti;

        if (ue_db.contains(rnti)) {
          // Copy dci info
          dl_sched_res->pdsch[n].dci = sched_result.data[i].dci;

          for (uint32_t tb = 0; tb < SRSRAN_MAX_TB; tb++) {
            dl_sched_res->pdsch[n].softbuffer_tx[tb] =
                ue_db[rnti]->get_tx_softbuffer(sched_result.data[i].dci.ue_cc_idx, sched_result.data[i].dci.pid, tb);

            // If the Rx soft-buffer is not given, abort transmission
            if (dl_sched_res->pdsch[n].softbuffer_tx[tb] == nullptr) {
              continue;
            }

            if (sched_result.data[i].nof_pdu_elems[tb] > 0) {
              /* Get PDU if it's a new transmission */
              dl_sched_res->pdsch[n].data[tb] = ue_db[rnti]->generate_pdu(sched_result.data[i].dci.ue_cc_idx,
                                                                          sched_result.data[i].dci.pid,
                                                                          tb,
                                                                          sched_result.data[i].pdu[tb],
                                                                          sched_result.data[i].nof_pdu_elems[tb],
                                                                          sched_result.data[i].tbs[tb]);

              if (!dl_sched_res->pdsch[n].data[tb]) {
                logger.error("Error! PDU was not generated (rnti=0x%04x, tb=%d)", rnti, tb);
              }

              if (pcap) {
                pcap->write_dl_crnti(
                    dl_sched_res->pdsch[n].data[tb], sched_result.data[i].tbs[tb], rnti, true, tti_tx_dl, enb_cc_idx);
              }
              if (pcap_net) {
                pcap_net->write_dl_crnti(
                    dl_sched_res->pdsch[n].data[tb], sched_result.data[i].tbs[tb], rnti, true, tti_tx_dl, enb_cc_idx);
              }
            } else {
              /* TB not enabled OR no data to send: set pointers to NULL  */
              dl_sched_res->pdsch[n].data[tb] = nullptr;
            }

            tb_count++;
          }

          // Count transmission if at least one TB has succesfully added
          if (tb_count > 0) {
            n++;
          }
        } else {
          logger.warning("Invalid DL scheduling result. User 0x%x does not exist", rnti);
        }
      }

      // No more uses of shared ue_db beyond here
    }

    // Copy RAR grants
    for (uint32_t i = 0; i < sched_result.rar.size(); i++) {
      // Copy dci info
      dl_sched_res->pdsch[n].dci = sched_result.rar[i].dci;

      // Set softbuffer (there are no retx in RAR but a softbuffer is required)
      dl_sched_res->pdsch[n].softbuffer_tx[0] = &common_buffers[enb_cc_idx].rar_softbuffer_tx;

      // Assemble PDU
      dl_sched_res->pdsch[n].data[0] = assemble_rar(sched_result.rar[i].msg3_grant.data(),
                                                    enb_cc_idx,
                                                    sched_result.rar[i].msg3_grant.size(),
                                                    i,
                                                    sched_result.rar[i].tbs,
                                                    tti_tx_dl);

      if (pcap) {
        pcap->write_dl_ranti(dl_sched_res->pdsch[n].data[0],
                             sched_result.rar[i].tbs,
                             dl_sched_res->pdsch[n].dci.rnti,
                             true,
                             tti_tx_dl,
                             enb_cc_idx);
      }
      if (pcap_net) {
        pcap_net->write_dl_ranti(dl_sched_res->pdsch[n].data[0],
                                 sched_result.rar[i].tbs,
                                 dl_sched_res->pdsch[n].dci.rnti,
                                 true,
                                 tti_tx_dl,
                                 enb_cc_idx);
      }
      n++;
    }

    // Copy SI and Paging grants
    for (uint32_t i = 0; i < sched_result.bc.size(); i++) {
      // Copy dci info
      dl_sched_res->pdsch[n].dci = sched_result.bc[i].dci;

      // Set softbuffer
      if (sched_result.bc[i].type == sched_interface::dl_sched_bc_t::BCCH) {
        dl_sched_res->pdsch[n].softbuffer_tx[0] =
            &common_buffers[enb_cc_idx].bcch_softbuffer_tx[sched_result.bc[i].index];
        dl_sched_res->pdsch[n].data[0] = rrc_h->read_pdu_bcch_dlsch(enb_cc_idx, sched_result.bc[i].index);
#ifdef WRITE_SIB_PCAP
        if (pcap) {
          pcap->write_dl_sirnti(dl_sched_res->pdsch[n].data[0], sched_result.bc[i].tbs, true, tti_tx_dl, enb_cc_idx);
        }
        if (pcap_net) {
          pcap_net->write_dl_sirnti(
              dl_sched_res->pdsch[n].data[0], sched_result.bc[i].tbs, true, tti_tx_dl, enb_cc_idx);
        }
#endif
      } else {
        dl_sched_res->pdsch[n].softbuffer_tx[0] = &common_buffers[enb_cc_idx].pcch_softbuffer_tx;
        dl_sched_res->pdsch[n].data[0]          = common_buffers[enb_cc_idx].pcch_payload_buffer;
        rlc_h->read_pdu_pcch(common_buffers[enb_cc_idx].pcch_payload_buffer, pcch_payload_buffer_len);

        if (pcap) {
          pcap->write_dl_pch(dl_sched_res->pdsch[n].data[0], sched_result.bc[i].tbs, true, tti_tx_dl, enb_cc_idx);
        }
        if (pcap_net) {
          pcap_net->write_dl_pch(dl_sched_res->pdsch[n].data[0], sched_result.bc[i].tbs, true, tti_tx_dl, enb_cc_idx);
        }
      }

      n++;
    }

    dl_sched_res->nof_grants = n;

    // Number of CCH symbols
    dl_sched_res->cfi = sched_result.cfi;
  }

  // Count number of TTIs for all active users
  {
    srsran::rwlock_read_guard lock(rwlock);
    for (auto& u : ue_db) {
      u.second->metrics_cnt();
    }
  }

  return SRSRAN_SUCCESS;
}

void mac::build_mch_sched(uint32_t tbs)
{
  int sfs_per_sched_period = mcch.pmch_info_list[0].sf_alloc_end;
  int bytes_per_sf         = tbs / 8 - 6; // leave 6 bytes for header

  int total_space_avail_bytes = sfs_per_sched_period * bytes_per_sf;

  int total_bytes_to_tx = 0;

  // calculate total bytes to be scheduled
  for (uint32_t i = 0; i < mch.num_mtch_sched; i++) {
    total_bytes_to_tx += mch.mtch_sched[i].lcid_buffer_size;
    mch.mtch_sched[i].stop = 0;
  }

  int last_mtch_stop = 0;

  if (total_bytes_to_tx > 0 && total_bytes_to_tx >= total_space_avail_bytes) {
    for (uint32_t i = 0; i < mch.num_mtch_sched; i++) {
      double ratio           = mch.mtch_sched[i].lcid_buffer_size / total_bytes_to_tx;
      float  assigned_sfs    = floor(sfs_per_sched_period * ratio);
      mch.mtch_sched[i].stop = last_mtch_stop + (uint32_t)assigned_sfs;
      last_mtch_stop         = mch.mtch_sched[i].stop;
    }
  } else {
    for (uint32_t i = 0; i < mch.num_mtch_sched; i++) {
      float assigned_sfs     = ceil(((float)mch.mtch_sched[i].lcid_buffer_size) / ((float)bytes_per_sf));
      mch.mtch_sched[i].stop = last_mtch_stop + (uint32_t)assigned_sfs;
      last_mtch_stop         = mch.mtch_sched[i].stop;
    }
  }
}

int mac::get_mch_sched(uint32_t tti, bool is_mcch, dl_sched_list_t& dl_sched_res_list)
{
  dl_sched_t* dl_sched_res = &dl_sched_res_list[0];
  logger.set_context(tti);
  srsran_ra_tb_t mcs      = {};
  srsran_ra_tb_t mcs_data = {};
  mcs.mcs_idx             = enum_to_number(this->sib13.mbsfn_area_info_list[0].mcch_cfg.sig_mcs);
  mcs_data.mcs_idx        = this->mcch.pmch_info_list[0].data_mcs;
  srsran_dl_fill_ra_mcs(&mcs, 0, cell_config[0].cell.nof_prb, false);
  srsran_dl_fill_ra_mcs(&mcs_data, 0, cell_config[0].cell.nof_prb, false);
  if (is_mcch) {
    build_mch_sched(mcs_data.tbs);
    mch.mcch_payload              = mcch_payload_buffer;
    mch.current_sf_allocation_num = 1;
    logger.info("MCH Sched Info: LCID: %d, Stop: %d, tti is %d ",
                mch.mtch_sched[0].lcid,
                mch.mtch_sched[mch.num_mtch_sched - 1].stop,
                tti);
    phy_h->set_mch_period_stop(mch.mtch_sched[mch.num_mtch_sched - 1].stop);
    for (uint32_t i = 0; i < mch.num_mtch_sched; i++) {
      mch.pdu[i].lcid = (uint32_t)srsran::mch_lcid::MCH_SCHED_INFO;
      // m1u.mtch_sched[i].lcid = 1+i;
    }

    mch.pdu[mch.num_mtch_sched].lcid   = 0;
    mch.pdu[mch.num_mtch_sched].nbytes = current_mcch_length;
    dl_sched_res->pdsch[0].dci.rnti    = SRSRAN_MRNTI;

    // we use TTI % HARQ to make sure we use different buffers for consecutive TTIs to avoid races between PHY workers
    ue_db[SRSRAN_MRNTI]->metrics_tx(true, mcs.tbs);
    dl_sched_res->pdsch[0].data[0] =
        ue_db[SRSRAN_MRNTI]->generate_mch_pdu(tti % SRSRAN_FDD_NOF_HARQ, mch, mch.num_mtch_sched + 1, mcs.tbs / 8);

  } else {
    uint32_t current_lcid = 1;
    uint32_t mtch_index   = 0;
    uint32_t mtch_stop    = mch.mtch_sched[mch.num_mtch_sched - 1].stop;

    for (uint32_t i = 0; i < mch.num_mtch_sched; i++) {
      if (mch.current_sf_allocation_num <= mch.mtch_sched[i].stop) {
        current_lcid = mch.mtch_sched[i].lcid;
        mtch_index   = i;
        break;
      }
    }
    if (mch.current_sf_allocation_num <= mtch_stop) {
      int requested_bytes = (mcs_data.tbs / 8 > (int)mch.mtch_sched[mtch_index].lcid_buffer_size)
                                ? (mch.mtch_sched[mtch_index].lcid_buffer_size)
                                : ((mcs_data.tbs / 8) - 2);
      int bytes_received = ue_db[SRSRAN_MRNTI]->read_pdu(current_lcid, mtch_payload_buffer, requested_bytes);
      mch.pdu[0].lcid    = current_lcid;
      mch.pdu[0].nbytes  = bytes_received;
      mch.mtch_sched[0].mtch_payload  = mtch_payload_buffer;
      dl_sched_res->pdsch[0].dci.rnti = SRSRAN_MRNTI;
      if (bytes_received) {
        ue_db[SRSRAN_MRNTI]->metrics_tx(true, mcs.tbs);
        dl_sched_res->pdsch[0].data[0] =
            ue_db[SRSRAN_MRNTI]->generate_mch_pdu(tti % SRSRAN_FDD_NOF_HARQ, mch, 1, mcs_data.tbs / 8);
      }
    } else {
      dl_sched_res->pdsch[0].dci.rnti = 0;
      dl_sched_res->pdsch[0].data[0]  = nullptr;
    }
    mch.current_sf_allocation_num++;
  }

  // Count number of TTIs for all active users
  for (auto& u : ue_db) {
    u.second->metrics_cnt();
  }
  return SRSRAN_SUCCESS;
}

uint8_t* mac::assemble_rar(sched_interface::dl_sched_rar_grant_t* grants,
                           uint32_t                               enb_cc_idx,
                           uint32_t                               nof_grants,
                           uint32_t                               rar_idx,
                           uint32_t                               pdu_len,
                           uint32_t                               tti)
{
  uint8_t grant_buffer[64] = {};
  if (pdu_len < rar_payload_len && rar_idx < rar_pdu_msg.size()) {
    srsran::rar_pdu* pdu = &rar_pdu_msg[rar_idx];
    rar_payload[enb_cc_idx][rar_idx].clear();
    pdu->init_tx(&rar_payload[enb_cc_idx][rar_idx], pdu_len);
    for (uint32_t i = 0; i < nof_grants; i++) {
      srsran_dci_rar_pack(&grants[i].grant, grant_buffer);
      if (pdu->new_subh()) {
        pdu->get()->set_rapid(grants[i].data.preamble_idx);
        pdu->get()->set_ta_cmd(grants[i].data.ta_cmd);
        pdu->get()->set_temp_crnti(grants[i].data.temp_crnti);
        pdu->get()->set_sched_grant(grant_buffer);
      }
    }
    if (pdu->write_packet(rar_payload[enb_cc_idx][rar_idx].msg)) {
      return rar_payload[enb_cc_idx][rar_idx].msg;
    }
  }

  logger.error("Assembling RAR: rar_idx=%d, pdu_len=%d, rar_payload_len=%d, nof_grants=%d",
               rar_idx,
               pdu_len,
               int(rar_payload_len),
               nof_grants);
  return nullptr;
}

int mac::get_ul_sched(uint32_t tti_tx_ul, ul_sched_list_t& ul_sched_res_list)
{
  if (!started) {
    return SRSRAN_SUCCESS;
  }

  logger.set_context(TTI_SUB(tti_tx_ul, FDD_HARQ_DELAY_UL_MS + FDD_HARQ_DELAY_DL_MS));

  // Execute UE FSMs (e.g. TA)
  for (auto& ue : ue_db) {
    ue.second->tic();
  }

  for (uint32_t enb_cc_idx = 0; enb_cc_idx < cell_config.size(); enb_cc_idx++) {
    ul_sched_t* phy_ul_sched_res = &ul_sched_res_list[enb_cc_idx];

    // Run scheduler with current info
    sched_interface::ul_sched_res_t sched_result = {};
    if (scheduler.ul_sched(tti_tx_ul, enb_cc_idx, sched_result) < 0) {
      logger.error("Running scheduler");
      return SRSRAN_ERROR;
    }

    {
      srsran::rwlock_read_guard lock(rwlock);

      // Copy DCI grants
      phy_ul_sched_res->nof_grants = 0;
      int n                        = 0;
      for (uint32_t i = 0; i < sched_result.pusch.size(); i++) {
        if (sched_result.pusch[i].tbs > 0) {
          // Get UE
          uint16_t rnti = sched_result.pusch[i].dci.rnti;

          if (ue_db.contains(rnti)) {
            // Copy grant info
            phy_ul_sched_res->pusch[n].current_tx_nb = sched_result.pusch[i].current_tx_nb;
            phy_ul_sched_res->pusch[n].pid           = TTI_RX(tti_tx_ul) % SRSRAN_FDD_NOF_HARQ;
            phy_ul_sched_res->pusch[n].needs_pdcch   = sched_result.pusch[i].needs_pdcch;
            phy_ul_sched_res->pusch[n].dci           = sched_result.pusch[i].dci;
            phy_ul_sched_res->pusch[n].softbuffer_rx =
                ue_db[rnti]->get_rx_softbuffer(sched_result.pusch[i].dci.ue_cc_idx, tti_tx_ul);

            // If the Rx soft-buffer is not given, abort reception
            if (phy_ul_sched_res->pusch[n].softbuffer_rx == nullptr) {
              continue;
            }

            if (sched_result.pusch[n].current_tx_nb == 0) {
              srsran_softbuffer_rx_reset_tbs(phy_ul_sched_res->pusch[n].softbuffer_rx, sched_result.pusch[i].tbs * 8);
            }
            phy_ul_sched_res->pusch[n].data =
                ue_db[rnti]->request_buffer(tti_tx_ul, sched_result.pusch[i].dci.ue_cc_idx, sched_result.pusch[i].tbs);
            if (phy_ul_sched_res->pusch[n].data) {
              phy_ul_sched_res->nof_grants++;
            } else {
              logger.error("Grant for rnti=0x%x could not be allocated due to lack of buffers", rnti);
            }
            n++;
          } else {
            logger.warning("Invalid UL scheduling result. User 0x%x does not exist", rnti);
          }

        } else {
          logger.warning("Grant %d for rnti=0x%x has zero TBS", i, sched_result.pusch[i].dci.rnti);
        }
      }

      // No more uses of ue_db beyond here
    }

    // Copy PHICH actions
    for (uint32_t i = 0; i < sched_result.phich.size(); i++) {
      phy_ul_sched_res->phich[i].ack  = sched_result.phich[i].phich == sched_interface::ul_sched_phich_t::ACK;
      phy_ul_sched_res->phich[i].rnti = sched_result.phich[i].rnti;
    }
    phy_ul_sched_res->nof_phich = sched_result.phich.size();
  }
  // clear old buffers from all users
  for (auto& u : ue_db) {
    u.second->clear_old_buffers(tti_tx_ul);
  }
  return SRSRAN_SUCCESS;
}

bool mac::process_pdus()
{
  srsran::rwlock_read_guard lock(rwlock);
  bool                      ret = false;
  for (auto& u : ue_db) {
    ret |= u.second->process_pdus();
  }
  return ret;
}

void mac::write_mcch(const srsran::sib2_mbms_t* sib2_,
                     const srsran::sib13_t*     sib13_,
                     const srsran::mcch_msg_t*  mcch_,
                     const uint8_t*             mcch_payload,
                     const uint8_t              mcch_payload_length)
{
  mcch               = *mcch_;
  mch.num_mtch_sched = this->mcch.pmch_info_list[0].nof_mbms_session_info;
  for (uint32_t i = 0; i < mch.num_mtch_sched; ++i) {
    mch.mtch_sched[i].lcid = this->mcch.pmch_info_list[0].mbms_session_info_list[i].lc_ch_id;
  }
  sib2  = *sib2_;
  sib13 = *sib13_;
  memcpy(mcch_payload_buffer, mcch_payload, mcch_payload_length * sizeof(uint8_t));
  current_mcch_length = mcch_payload_length;
  ue_db[SRSRAN_MRNTI] = std::unique_ptr<ue>{
      new ue(SRSRAN_MRNTI, args.nof_prb, &scheduler, rrc_h, rlc_h, phy_h, logger, cells.size(), softbuffer_pool.get())};

  rrc_h->add_user(SRSRAN_MRNTI, {});
}

bool mac::check_ue_exists(uint16_t rnti)
{
  if (not ue_db.contains(rnti)) {
    if (not ues_to_rem.count(rnti)) {
      logger.error("User rnti=0x%x not found", rnti);
    }
    return false;
  }
  return true;
}

} // namespace srsenb
