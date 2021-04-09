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

#include "srsran/upper/rlc.h"
#include "srsran/common/rwlock_guard.h"
#include "srsran/upper/rlc_am_lte.h"
#include "srsran/upper/rlc_tm.h"
#include "srsran/upper/rlc_um_lte.h"
#include "srsran/upper/rlc_um_nr.h"

namespace srsran {

rlc::rlc(const char* logname) : logger(srslog::fetch_basic_logger(logname))
{
  pool = byte_buffer_pool::get_instance();
  pthread_rwlock_init(&rwlock, NULL);
}

rlc::~rlc()
{
  // destroy all remaining entities
  {
    rwlock_write_guard lock(rwlock);

    for (rlc_map_t::iterator it = rlc_array.begin(); it != rlc_array.end(); ++it) {
      delete (it->second);
    }
    rlc_array.clear();

    for (rlc_map_t::iterator it = rlc_array_mrb.begin(); it != rlc_array_mrb.end(); ++it) {
      delete (it->second);
    }
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
  init(pdcp_, rrc_, timers_, lcid_);
  bsr_callback = bsr_callback_;
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

    logger.info("lcid=%d, rx_rate_mbps=%4.2f (real=%4.2f), tx_rate_mbps=%4.2f (real=%4.2f)",
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
    logger.info("MCH_LCID=%d, rx_rate_mbps=%4.2f",
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
    logger.info("Reestablishing %s", rrc->get_rb_name(lcid));
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

    for (rlc_map_t::iterator it = rlc_array.begin(); it != rlc_array.end(); ++it) {
      it->second->stop();
      delete (it->second);
    }
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

uint32_t rlc::get_buffer_state(uint32_t lcid)
{
  uint32_t ret = 0;

  rwlock_read_guard lock(rwlock);
  if (valid_lcid(lcid)) {
    if (rlc_array.at(lcid)->is_suspended()) {
      ret = 0;
    } else {
      ret = rlc_array.at(lcid)->get_buffer_state();
    }
  }

  return ret;
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

int rlc::read_pdu(uint32_t lcid, uint8_t* payload, uint32_t nof_bytes)
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

int rlc::read_pdu_mch(uint32_t lcid, uint8_t* payload, uint32_t nof_bytes)
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
void rlc::add_bearer(uint32_t lcid, const rlc_config_t& cnfg)
{
  rwlock_write_guard lock(rwlock);

  rlc_common* rlc_entity = nullptr;

  if (cnfg.rlc_mode != rlc_mode_t::tm and rlc_array.find(lcid) != rlc_array.end()) {
    if (rlc_array[lcid]->get_mode() != cnfg.rlc_mode) {
      logger.info("Switching RLC entity type. Recreating it.");
      rlc_array.erase(lcid);
    }
  }

  if (not valid_lcid(lcid)) {
    if (cnfg.rat == srsran_rat_t::lte) {
      switch (cnfg.rlc_mode) {
        case rlc_mode_t::tm:
          rlc_entity = new rlc_tm(logger, lcid, pdcp, rrc);
          break;
        case rlc_mode_t::am:
          rlc_entity = new rlc_am_lte(logger, lcid, pdcp, rrc, timers);
          break;
        case rlc_mode_t::um:
          rlc_entity = new rlc_um_lte(logger, lcid, pdcp, rrc, timers);
          break;
        default:
          logger.error("Cannot add RLC entity - invalid mode");
          return;
      }
      if (rlc_entity != nullptr) {
        rlc_entity->set_bsr_callback(bsr_callback);
      }
    } else if (cnfg.rat == srsran_rat_t::nr) {
      switch (cnfg.rlc_mode) {
        case rlc_mode_t::tm:
          rlc_entity = new rlc_tm(logger, lcid, pdcp, rrc);
          break;
        case rlc_mode_t::um:
          rlc_entity = new rlc_um_nr(logger, lcid, pdcp, rrc, timers);
          break;
        default:
          logger.error("Cannot add RLC entity - invalid mode");
          return;
      }
    } else {
      logger.error("RAT not supported");
      return;
    }

    if (not rlc_array.insert(rlc_map_pair_t(lcid, rlc_entity)).second) {
      logger.error("Error inserting RLC entity in to array.");
      goto delete_and_exit;
    }
    logger.info("Added radio bearer %s in %s", rrc->get_rb_name(lcid), to_string(cnfg.rlc_mode).c_str());
    rlc_entity = NULL;
  }

  // configure and add to array
  if (cnfg.rlc_mode != rlc_mode_t::tm and rlc_array.find(lcid) != rlc_array.end()) {
    if (not rlc_array.at(lcid)->configure(cnfg)) {
      logger.error("Error configuring RLC entity.");
      goto delete_and_exit;
    }
  }

  logger.info("Configured radio bearer %s in %s", rrc->get_rb_name(lcid), to_string(cnfg.rlc_mode).c_str());

delete_and_exit:
  if (rlc_entity) {
    delete (rlc_entity);
  }
}

void rlc::add_bearer_mrb(uint32_t lcid)
{
  rwlock_write_guard lock(rwlock);
  rlc_common*        rlc_entity = NULL;

  if (not valid_lcid_mrb(lcid)) {
    rlc_entity = new rlc_um_lte(logger, lcid, pdcp, rrc, timers);
    // configure and add to array
    if (not rlc_entity->configure(rlc_config_t::mch_config())) {
      logger.error("Error configuring RLC entity.");
      goto delete_and_exit;
    }
    if (rlc_array_mrb.count(lcid) == 0) {
      if (not rlc_array_mrb.insert(rlc_map_pair_t(lcid, rlc_entity)).second) {
        logger.error("Error inserting RLC entity in to array.");
        goto delete_and_exit;
      }
    }
    logger.warning("Added bearer MRB%d with mode RLC_UM", lcid);
    return;
  } else {
    logger.warning("Bearer MRB%d already created.", lcid);
  }

delete_and_exit:
  if (rlc_entity != NULL) {
    delete (rlc_entity);
  }
}

void rlc::del_bearer(uint32_t lcid)
{
  rwlock_write_guard lock(rwlock);

  if (valid_lcid(lcid)) {
    rlc_map_t::iterator it = rlc_array.find(lcid);
    it->second->stop();
    delete (it->second);
    rlc_array.erase(it);
    logger.warning("Deleted RLC bearer %s", rrc->get_rb_name(lcid));
  } else {
    logger.error("Can't delete bearer %s. Bearer doesn't exist.", rrc->get_rb_name(lcid));
  }
}

void rlc::del_bearer_mrb(uint32_t lcid)
{
  rwlock_write_guard lock(rwlock);

  if (valid_lcid_mrb(lcid)) {
    rlc_map_t::iterator it = rlc_array_mrb.find(lcid);
    it->second->stop();
    delete (it->second);
    rlc_array_mrb.erase(it);
    logger.warning("Deleted RLC MRB bearer %s", rrc->get_rb_name(lcid));
  } else {
    logger.error("Can't delete bearer %s. Bearer doesn't exist.", rrc->get_rb_name(lcid));
  }
}

void rlc::change_lcid(uint32_t old_lcid, uint32_t new_lcid)
{
  rwlock_write_guard lock(rwlock);

  // make sure old LCID exists and new LCID is still free
  if (valid_lcid(old_lcid) && not valid_lcid(new_lcid)) {
    // insert old rlc entity into new LCID
    rlc_map_t::iterator it         = rlc_array.find(old_lcid);
    rlc_common*         rlc_entity = it->second;
    if (not rlc_array.insert(rlc_map_pair_t(new_lcid, rlc_entity)).second) {
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
    logger.error("Can't change LCID of bearer %s from %d to %d. Bearer doesn't exist or new LCID already occupied.",
                 rrc->get_rb_name(old_lcid),
                 old_lcid,
                 new_lcid);
  }
}

// Further RRC calls executed from Stack thread, no need to hold lock
void rlc::suspend_bearer(uint32_t lcid)
{
  if (valid_lcid(lcid)) {
    if (rlc_array.at(lcid)->suspend()) {
      logger.info("Suspended radio bearer %s", rrc->get_rb_name(lcid));
    } else {
      logger.error("Error suspending RLC entity: bearer already suspended.");
    }
  } else {
    logger.error("Suspending bearer: bearer %s not configured.", rrc->get_rb_name(lcid));
  }
}

void rlc::resume_bearer(uint32_t lcid)
{
  logger.info("Resuming radio bearer %s", rrc->get_rb_name(lcid));
  if (valid_lcid(lcid)) {
    if (rlc_array.at(lcid)->resume()) {
      logger.info("Resumed radio bearer %s", rrc->get_rb_name(lcid));
    } else {
      logger.error("Error resuming RLC entity: bearer not suspended.");
    }
  } else {
    logger.error("Resuming bearer: bearer %s not configured.", rrc->get_rb_name(lcid));
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
    uint32_t tx_queue   = get_buffer_state(lcid);
    uint32_t retx_queue = 0; // todo: separate tx_queue and retx_queue
    bsr_callback(lcid, tx_queue, retx_queue);
  }
}

void rlc::update_bsr_mch(uint32_t lcid)
{
  if (bsr_callback) {
    uint32_t tx_queue   = get_total_mch_buffer_state(lcid);
    uint32_t retx_queue = 0; // todo: separate tx_queue and retx_queue
    bsr_callback(lcid, tx_queue, retx_queue);
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
