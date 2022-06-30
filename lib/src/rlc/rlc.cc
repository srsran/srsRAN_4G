/**
 * Copyright 2013-2022 Software Radio Systems Limited
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

#include "srsran/rlc/rlc.h"
#include "srsran/common/rwlock_guard.h"
#include "srsran/rlc/rlc_am_base.h"
#include "srsran/rlc/rlc_tm.h"
#include "srsran/rlc/rlc_um_lte.h"
#include "srsran/rlc/rlc_um_nr.h"

namespace srsran {

rlc::rlc(const char* logname) : logger(srslog::fetch_basic_logger(logname)), pool(byte_buffer_pool::get_instance())
{
  pthread_rwlock_init(&rwlock, NULL);
}

rlc::~rlc()
{
  // destroy all remaining entities
  {
    srsran::rwlock_write_guard lock(rwlock);
    rlc_array.clear();
    rlc_array_mrb.clear();
  }
  pthread_rwlock_destroy(&rwlock);
}

void rlc::init(srsue::pdcp_interface_rlc* pdcp_,
               srsue::rrc_interface_rlc*  rrc_,
               srsran::timer_handler*     timers_,
               uint32_t                   lcid_)
{
  pdcp         = pdcp_;
  rrc          = rrc_;
  timers       = timers_;
  default_lcid = lcid_;

  reset_metrics();

  // create default RLC_TM bearer for SRB0
  add_bearer(default_lcid, rlc_config_t());
}

void rlc::init(srsue::pdcp_interface_rlc* pdcp_,
               srsue::rrc_interface_rlc*  rrc_,
               srsran::timer_handler*     timers_,
               uint32_t                   lcid_,
               bsr_callback_t             bsr_callback_)
{
  bsr_callback = bsr_callback_;
  init(pdcp_, rrc_, timers_, lcid_);
}

void rlc::reset_metrics()
{
  for (rlc_map_t::iterator it = rlc_array.begin(); it != rlc_array.end(); ++it) {
    it->second->reset_metrics();
  }

  for (rlc_map_t::iterator it = rlc_array_mrb.begin(); it != rlc_array_mrb.end(); ++it) {
    it->second->reset_metrics();
  }

  metrics_tp = std::chrono::high_resolution_clock::now();
}

void rlc::stop()
{
  for (rlc_map_t::iterator it = rlc_array.begin(); it != rlc_array.end(); ++it) {
    it->second->stop();
  }
  for (rlc_map_t::iterator it = rlc_array_mrb.begin(); it != rlc_array_mrb.end(); ++it) {
    it->second->stop();
  }
}

void rlc::get_metrics(rlc_metrics_t& m, const uint32_t nof_tti)
{
  std::chrono::duration<double> secs = std::chrono::high_resolution_clock::now() - metrics_tp;

  for (rlc_map_t::iterator it = rlc_array.begin(); it != rlc_array.end(); ++it) {
    rlc_bearer_metrics_t metrics = it->second->get_metrics();

    // Rx/Tx rate based on real time
    double rx_rate_mbps_real_time = (metrics.num_rx_pdu_bytes * 8 / (double)1e6) / secs.count();
    double tx_rate_mbps_real_time = (metrics.num_tx_pdu_bytes * 8 / (double)1e6) / secs.count();

    // Rx/Tx rate based on number of TTIs
    double rx_rate_mbps = (nof_tti > 0) ? ((metrics.num_rx_pdu_bytes * 8 / (double)1e6) / (nof_tti / 1000.0)) : 0.0;
    double tx_rate_mbps = (nof_tti > 0) ? ((metrics.num_tx_pdu_bytes * 8 / (double)1e6) / (nof_tti / 1000.0)) : 0.0;

    logger.debug("lcid=%d, rx_rate_mbps=%4.2f (real=%4.2f), tx_rate_mbps=%4.2f (real=%4.2f)",
                 it->first,
                 rx_rate_mbps,
                 rx_rate_mbps_real_time,
                 tx_rate_mbps,
                 tx_rate_mbps_real_time);
    m.bearer[it->first] = metrics;
  }

  // Add multicast metrics
  for (rlc_map_t::iterator it = rlc_array_mrb.begin(); it != rlc_array_mrb.end(); ++it) {
    rlc_bearer_metrics_t metrics = it->second->get_metrics();
    logger.debug("MCH_LCID=%d, rx_rate_mbps=%4.2f",
                 it->first,
                 (metrics.num_rx_pdu_bytes * 8 / static_cast<double>(1e6)) / secs.count());
    m.bearer[it->first] = metrics;
  }

  reset_metrics();
}

// Reestablish all RLC bearer
void rlc::reestablish()
{
  for (rlc_map_t::iterator it = rlc_array.begin(); it != rlc_array.end(); ++it) {
    it->second->reestablish();
  }

  for (rlc_map_t::iterator it = rlc_array_mrb.begin(); it != rlc_array_mrb.end(); ++it) {
    it->second->reestablish();
  }

  reset_metrics();
}

// Reestablish a specific RLC bearer
void rlc::reestablish(uint32_t lcid)
{
  if (valid_lcid(lcid)) {
    logger.info("Reestablishing LCID %d", lcid);
    rlc_array.at(lcid)->reestablish();
  } else {
    logger.warning("RLC LCID %d doesn't exist.", lcid);
  }
}

// Resetting the RLC layer returns the object to the state after the call to init():
// All LCIDs are removed, except SRB0
void rlc::reset()
{
  {
    rwlock_write_guard lock(rwlock);
    rlc_array.clear();
    // the multicast bearer (MRB) is not removed here because eMBMS services continue to be streamed in idle mode (3GPP
    // TS 23.246 version 14.1.0 Release 14 section 8)
  }

  // Add SRB0 again
  add_bearer(default_lcid, rlc_config_t());
}

void rlc::empty_queue()
{
  // Empty Tx queue, not needed for MCH bearers
  for (rlc_map_t::iterator it = rlc_array.begin(); it != rlc_array.end(); ++it) {
    it->second->empty_queue();
  }
}

/*******************************************************************************
  PDCP interface (called from Stack thread and therefore no lock required)
*******************************************************************************/

void rlc::write_sdu(uint32_t lcid, unique_byte_buffer_t sdu)
{
  // TODO: rework build PDU logic to allow large SDUs (without concatenation)
  if (sdu->N_bytes > RLC_MAX_SDU_SIZE) {
    logger.warning("Dropping too long SDU of size %d B (Max. size %d B).", sdu->N_bytes, RLC_MAX_SDU_SIZE);
    return;
  }

  if (valid_lcid(lcid)) {
    rlc_array.at(lcid)->write_sdu_s(std::move(sdu));
    update_bsr(lcid);
  } else {
    logger.warning("RLC LCID %d doesn't exist. Deallocating SDU", lcid);
  }
}

void rlc::write_sdu_mch(uint32_t lcid, unique_byte_buffer_t sdu)
{
  if (valid_lcid_mrb(lcid)) {
    rlc_array_mrb.at(lcid)->write_sdu(std::move(sdu));
    update_bsr_mch(lcid);
  } else {
    logger.warning("RLC LCID %d doesn't exist. Deallocating SDU", lcid);
  }
}

bool rlc::rb_is_um(uint32_t lcid)
{
  bool ret = false;

  if (valid_lcid(lcid)) {
    ret = rlc_array.at(lcid)->get_mode() == rlc_mode_t::um;
  } else {
    logger.warning("LCID %d doesn't exist.", lcid);
  }

  return ret;
}

void rlc::discard_sdu(uint32_t lcid, uint32_t discard_sn)
{
  if (valid_lcid(lcid)) {
    rlc_array.at(lcid)->discard_sdu(discard_sn);
    update_bsr(lcid);
  } else {
    logger.warning("RLC LCID %d doesn't exist. Ignoring discard SDU", lcid);
  }
}

bool rlc::sdu_queue_is_full(uint32_t lcid)
{
  if (valid_lcid(lcid)) {
    return rlc_array.at(lcid)->sdu_queue_is_full();
  }
  logger.warning("RLC LCID %d doesn't exist. Ignoring queue check", lcid);
  return false;
}

/*******************************************************************************
  MAC interface (mostly called from PHY workers, lock needs to be hold)
*******************************************************************************/
bool rlc::has_data_locked(const uint32_t lcid)
{
  rwlock_read_guard lock(rwlock);
  return has_data(lcid);
}

void rlc::get_buffer_state(uint32_t lcid, uint32_t& tx_queue, uint32_t& prio_tx_queue)
{
  rwlock_read_guard lock(rwlock);
  if (valid_lcid(lcid)) {
    if (rlc_array.at(lcid)->is_suspended()) {
      tx_queue      = 0;
      prio_tx_queue = 0;
    } else {
      rlc_array.at(lcid)->get_buffer_state(tx_queue, prio_tx_queue);
    }
  }
}

uint32_t rlc::get_buffer_state(uint32_t lcid)
{
  uint32_t tx_queue = 0, prio_tx_queue = 0;
  get_buffer_state(lcid, tx_queue, prio_tx_queue);
  return tx_queue + prio_tx_queue;
}

uint32_t rlc::get_total_mch_buffer_state(uint32_t lcid)
{
  uint32_t ret = 0;

  rwlock_read_guard lock(rwlock);
  if (valid_lcid_mrb(lcid)) {
    ret = rlc_array_mrb.at(lcid)->get_buffer_state();
  }

  return ret;
}

uint32_t rlc::read_pdu(uint32_t lcid, uint8_t* payload, uint32_t nof_bytes)
{
  uint32_t ret = 0;

  rwlock_read_guard lock(rwlock);
  if (valid_lcid(lcid)) {
    ret = rlc_array.at(lcid)->read_pdu(payload, nof_bytes);
    update_bsr(lcid);
  } else {
    logger.warning("LCID %d doesn't exist.", lcid);
  }

  srsran_expect(ret <= nof_bytes, "Created too big RLC PDU (%d > %d)", ret, nof_bytes);

  return ret;
}

uint32_t rlc::read_pdu_mch(uint32_t lcid, uint8_t* payload, uint32_t nof_bytes)
{
  uint32_t ret = 0;

  rwlock_read_guard lock(rwlock);
  if (valid_lcid_mrb(lcid)) {
    ret = rlc_array_mrb.at(lcid)->read_pdu(payload, nof_bytes);
    update_bsr_mch(lcid);
  } else {
    logger.warning("LCID %d doesn't exist.", lcid);
  }

  srsran_expect(ret <= nof_bytes, "Created too big RLC PDU for MCH (%d > %d)", ret, nof_bytes);

  return ret;
}

// Write PDU methods are called from Stack thread context, no need to acquire the lock
void rlc::write_pdu(uint32_t lcid, uint8_t* payload, uint32_t nof_bytes)
{
  if (valid_lcid(lcid)) {
    rlc_array.at(lcid)->write_pdu_s(payload, nof_bytes);
    update_bsr(lcid);
  } else {
    logger.warning("LCID %d doesn't exist. Dropping PDU.", lcid);
  }
}

// Pass directly to PDCP, no DL througput counting done
void rlc::write_pdu_bcch_bch(srsran::unique_byte_buffer_t pdu)
{
  logger.info(pdu->msg, pdu->N_bytes, "BCCH BCH message received.");
  pdcp->write_pdu_bcch_bch(std::move(pdu));
}

// Pass directly to PDCP, no DL througput counting done
void rlc::write_pdu_bcch_dlsch(uint8_t* payload, uint32_t nof_bytes)
{
  logger.info(payload, nof_bytes, "BCCH TXSCH message received.");
  unique_byte_buffer_t buf = make_byte_buffer();
  if (buf != NULL) {
    memcpy(buf->msg, payload, nof_bytes);
    buf->N_bytes = nof_bytes;
    buf->set_timestamp();
    pdcp->write_pdu_bcch_dlsch(std::move(buf));
  } else {
    logger.error("Fatal error: Out of buffers from the pool in write_pdu_bcch_dlsch()");
  }
}

// Pass directly to PDCP, no DL througput counting done
void rlc::write_pdu_pcch(srsran::unique_byte_buffer_t pdu)
{
  logger.info(pdu->msg, pdu->N_bytes, "PCCH message received.");
  pdcp->write_pdu_pcch(std::move(pdu));
}

void rlc::write_pdu_mch(uint32_t lcid, uint8_t* payload, uint32_t nof_bytes)
{
  if (valid_lcid_mrb(lcid)) {
    rlc_array_mrb.at(lcid)->write_pdu(payload, nof_bytes);
  }
}

/*******************************************************************************
  RRC interface (write-lock ONLY needs to be hold for all calls modifying the RLC array)
*******************************************************************************/
bool rlc::is_suspended(const uint32_t lcid)
{
  bool ret = false;

  if (valid_lcid(lcid)) {
    ret = rlc_array.at(lcid)->is_suspended();
  }

  return ret;
}

bool rlc::has_data(uint32_t lcid)
{
  bool has_data = false;

  if (valid_lcid(lcid)) {
    has_data = rlc_array.at(lcid)->has_data();
  }

  return has_data;
}

// Methods modifying the RLC array need to acquire the write-lock
int rlc::add_bearer(uint32_t lcid, const rlc_config_t& cnfg)
{
  rwlock_write_guard lock(rwlock);

  if (valid_lcid(lcid)) {
    logger.warning("LCID %d already exists", lcid);
    return SRSRAN_ERROR;
  }

  std::unique_ptr<rlc_common> rlc_entity;

  switch (cnfg.rlc_mode) {
    case rlc_mode_t::tm:
      rlc_entity = std::unique_ptr<rlc_common>(new rlc_tm(logger, lcid, pdcp, rrc));
      break;
    case rlc_mode_t::am:
      switch (cnfg.rat) {
        case srsran_rat_t::lte:
          rlc_entity = std::unique_ptr<rlc_common>(new rlc_am(cnfg.rat, logger, lcid, pdcp, rrc, timers));
          break;
        case srsran_rat_t::nr:
          rlc_entity = std::unique_ptr<rlc_common>(new rlc_am(cnfg.rat, logger, lcid, pdcp, rrc, timers));
          break;
        default:
          logger.error("AM not supported for this RAT");
          return SRSRAN_ERROR;
      }
      break;
    case rlc_mode_t::um:
      switch (cnfg.rat) {
        case srsran_rat_t::lte:
          rlc_entity = std::unique_ptr<rlc_common>(new rlc_um_lte(logger, lcid, pdcp, rrc, timers));
          break;
        case srsran_rat_t::nr:
          rlc_entity = std::unique_ptr<rlc_common>(new rlc_um_nr(logger, lcid, pdcp, rrc, timers));
          break;
        default:
          logger.error("UM not supported for this RAT");
          return SRSRAN_ERROR;
      }
      break;
    default:
      logger.error("Cannot add RLC entity - invalid mode");
      return SRSRAN_ERROR;
  }

  // make sure entity has been created
  if (rlc_entity == nullptr) {
    logger.error("Couldn't allocate new RLC entity");
    return SRSRAN_ERROR;
  }

  // configure entity
  if (cnfg.rlc_mode != rlc_mode_t::tm) {
    if (not rlc_entity->configure(cnfg)) {
      logger.error("Error configuring RLC entity.");
      return SRSRAN_ERROR;
    }
  }

  rlc_entity->set_bsr_callback(bsr_callback);

  if (not rlc_array.insert(rlc_map_pair_t(lcid, std::move(rlc_entity))).second) {
    logger.error("Error inserting RLC entity in to array.");
    return SRSRAN_ERROR;
  }

  logger.info("Added %s radio bearer with LCID %d in %s", to_string(cnfg.rat), lcid, to_string(cnfg.rlc_mode));

  return SRSRAN_SUCCESS;
}

int rlc::add_bearer_mrb(uint32_t lcid)
{
  rwlock_write_guard lock(rwlock);
  if (not valid_lcid_mrb(lcid)) {
    std::unique_ptr<rlc_common> rlc_entity =
        std::unique_ptr<rlc_common>(new rlc_um_lte(logger, lcid, pdcp, rrc, timers));
    // configure and add to array
    if (not rlc_entity or rlc_entity->configure(rlc_config_t::mch_config()) == false) {
      logger.error("Error configuring RLC entity.");
      return SRSRAN_ERROR;
    }
    rlc_entity->set_bsr_callback(bsr_callback);
    if (rlc_array_mrb.count(lcid) == 0) {
      if (not rlc_array_mrb.insert(rlc_map_pair_t(lcid, std::move(rlc_entity))).second) {
        logger.error("Error inserting RLC entity in to array.");
        return SRSRAN_ERROR;
      }
    }
    logger.info("Added bearer MRB%d with mode RLC_UM", lcid);
  } else {
    logger.warning("Bearer MRB%d already created.", lcid);
  }

  return SRSRAN_SUCCESS;
}

void rlc::del_bearer(uint32_t lcid)
{
  rwlock_write_guard lock(rwlock);

  if (valid_lcid(lcid)) {
    rlc_map_t::iterator it = rlc_array.find(lcid);
    it->second->stop();
    rlc_array.erase(it);
    logger.info("Deleted RLC bearer with LCID %d", lcid);
  } else {
    logger.error("Can't delete bearer with LCID %d. Bearer doesn't exist.", lcid);
  }
}

void rlc::del_bearer_mrb(uint32_t lcid)
{
  rwlock_write_guard lock(rwlock);

  if (valid_lcid_mrb(lcid)) {
    rlc_map_t::iterator it = rlc_array_mrb.find(lcid);
    it->second->stop();
    rlc_array_mrb.erase(it);
    logger.info("Deleted RLC MRB bearer with LCID %d", lcid);
  } else {
    logger.error("Can't delete bearer with LCID %d. Bearer doesn't exist.", lcid);
  }
}

void rlc::change_lcid(uint32_t old_lcid, uint32_t new_lcid)
{
  rwlock_write_guard lock(rwlock);

  // make sure old LCID exists and new LCID is still free
  if (valid_lcid(old_lcid) && not valid_lcid(new_lcid)) {
    // insert old rlc entity into new LCID
    rlc_map_t::iterator         it         = rlc_array.find(old_lcid);
    std::unique_ptr<rlc_common> rlc_entity = std::move(it->second);
    if (not rlc_array.insert(rlc_map_pair_t(new_lcid, std::move(rlc_entity))).second) {
      logger.error("Error inserting RLC entity into array.");
      return;
    }
    // erase from old position
    rlc_array.erase(it);

    if (valid_lcid(new_lcid) && not valid_lcid(old_lcid)) {
      logger.info("Successfully changed LCID of RLC bearer from %d to %d", old_lcid, new_lcid);
    } else {
      logger.error("Error during LCID change of RLC bearer from %d to %d", old_lcid, new_lcid);
    }
  } else {
    logger.error("Can't change LCID of bearer LCID %d to %d. Bearer doesn't exist or new LCID already occupied.",
                 old_lcid,
                 new_lcid);
  }
}

// Further RRC calls executed from Stack thread, no need to hold lock
void rlc::suspend_bearer(uint32_t lcid)
{
  if (valid_lcid(lcid)) {
    if (rlc_array.at(lcid)->suspend()) {
      logger.info("Suspended radio bearer with LCID %d", lcid);
    } else {
      logger.error("Error suspending RLC entity: bearer already suspended.");
    }
  } else {
    logger.error("Suspending bearer: bearer with LCID %d not configured.", lcid);
  }
}

void rlc::resume_bearer(uint32_t lcid)
{
  logger.info("Resuming radio LCID %d", lcid);
  if (valid_lcid(lcid)) {
    if (rlc_array.at(lcid)->resume()) {
      logger.info("Resumed radio LCID %d", lcid);
    } else {
      logger.error("Error resuming RLC entity: bearer not suspended.");
    }
  } else {
    logger.error("Resuming bearer: bearer with LCID %d not configured.", lcid);
  }
}

bool rlc::has_bearer(uint32_t lcid)
{
  bool ret = valid_lcid(lcid);
  return ret;
}

/*******************************************************************************
  Helpers (Lock must be hold when calling those)
*******************************************************************************/
bool rlc::valid_lcid(uint32_t lcid)
{
  if (lcid >= SRSRAN_N_RADIO_BEARERS) {
    logger.error("Radio bearer id must be in [0:%d] - %d", SRSRAN_N_RADIO_BEARERS, lcid);
    return false;
  }

  if (rlc_array.find(lcid) == rlc_array.end()) {
    return false;
  }

  return true;
}

bool rlc::valid_lcid_mrb(uint32_t lcid)
{
  if (lcid >= SRSRAN_N_MCH_LCIDS) {
    logger.error("Radio bearer id must be in [0:%d] - %d", SRSRAN_N_RADIO_BEARERS, lcid);
    return false;
  }

  if (rlc_array_mrb.find(lcid) == rlc_array_mrb.end()) {
    return false;
  }

  return true;
}

void rlc::update_bsr(uint32_t lcid)
{
  if (bsr_callback) {
    uint32_t tx_queue = 0, prio_tx_queue = 0;
    get_buffer_state(lcid, tx_queue, prio_tx_queue);
  }
}

void rlc::update_bsr_mch(uint32_t lcid)
{
  if (bsr_callback) {
    uint32_t tx_queue = get_total_mch_buffer_state(lcid);
  }
}

void rlc_bearer_metrics_print(const rlc_bearer_metrics_t& metrics)
{
  std::cout << "num_tx_sdus=" << metrics.num_tx_sdus << "\n";
  std::cout << "num_rx_sdus=" << metrics.num_rx_sdus << "\n";
  std::cout << "num_tx_sdu_bytes=" << metrics.num_tx_sdu_bytes << "\n";
  std::cout << "num_rx_sdu_bytes=" << metrics.num_rx_sdu_bytes << "\n";
  std::cout << "num_tx_pdus=" << metrics.num_tx_pdus << "\n";
  std::cout << "num_rx_pdus=" << metrics.num_rx_pdus << "\n";
  std::cout << "num_tx_pdu_bytes=" << metrics.num_tx_pdu_bytes << "\n";
  std::cout << "num_rx_pdu_bytes=" << metrics.num_rx_pdu_bytes << "\n";
  std::cout << "num_lost_pdus=" << metrics.num_lost_pdus << "\n";
  std::cout << "num_lost_sdus=" << metrics.num_lost_sdus << "\n";
}

} // namespace srsran
