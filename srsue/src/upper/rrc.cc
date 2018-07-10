/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2015 Software Radio Systems Limited
 *
 * \section LICENSE
 *
 * This file is part of the srsUE library.
 *
 * srsUE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsUE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */


#include <unistd.h>
#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <time.h>
#include <inttypes.h> // for printing uint64_t
#include <srslte/asn1/liblte_rrc.h>
#include "srsue/hdr/upper/rrc.h"
#include "srslte/asn1/liblte_rrc.h"
#include "srslte/common/security.h"
#include "srslte/common/bcd_helpers.h"

using namespace srslte;

namespace srsue {

const static uint32_t NOF_REQUIRED_SIBS = 4;
const static uint32_t required_sibs[NOF_REQUIRED_SIBS] = {0,1,2,12}; // SIB1, SIB2, SIB3 and SIB13 (eMBMS)

/*******************************************************************************
  Base functions 
*******************************************************************************/

rrc::rrc()
  :state(RRC_STATE_IDLE)
  ,drb_up(false)
  ,serving_cell(NULL)
{
  n310_cnt       = 0;
  n311_cnt       = 0;
  serving_cell = new cell_t();
  neighbour_cells.reserve(NOF_NEIGHBOUR_CELLS);
  initiated = false;
  running = false;
  go_idle = false;
  go_rlf  = false;
}

rrc::~rrc()
{
  if (serving_cell) {
    delete(serving_cell);
  }

  std::vector<cell_t*>::iterator it;
  for (it = neighbour_cells.begin(); it != neighbour_cells.end(); ++it) {
    delete(*it);
  }
}

static void liblte_rrc_handler(void *ctx, char *str) {
  rrc *r = (rrc *) ctx;
  r->liblte_rrc_log(str);
}

void rrc::liblte_rrc_log(char *str) {
  if (rrc_log) {
    rrc_log->warning("[ASN]: %s\n", str);
  } else {
    printf("[ASN]: %s\n", str);
  }
}
void rrc::print_mbms()
{
  if(rrc_log) {
    if(serving_cell->has_mcch) {
      LIBLTE_RRC_MCCH_MSG_STRUCT msg;
      memcpy(&msg, &serving_cell->mcch, sizeof(LIBLTE_RRC_MCCH_MSG_STRUCT));
      std::stringstream ss;
      for(uint32_t i=0;i<msg.pmch_infolist_r9_size; i++){
        ss << "PMCH: " << i << std::endl;
        LIBLTE_RRC_PMCH_INFO_R9_STRUCT *pmch = &msg.pmch_infolist_r9[i];
        for(uint32_t j=0;j<pmch->mbms_sessioninfolist_r9_size; j++) {
          LIBLTE_RRC_MBMS_SESSION_INFO_R9_STRUCT *sess = &pmch->mbms_sessioninfolist_r9[j];
          ss << "  Service ID: " << sess->tmgi_r9.serviceid_r9;
          if(sess->sessionid_r9_present) {
            ss << ", Session ID: " << (uint32_t)sess->sessionid_r9;
          }
          if(sess->tmgi_r9.plmn_id_explicit) {
            std::string tmp;
            if(mcc_to_string(sess->tmgi_r9.plmn_id_r9.mcc, &tmp)) {
              ss << ", MCC: " << tmp;
            }
            if(mnc_to_string(sess->tmgi_r9.plmn_id_r9.mnc, &tmp)) {
              ss << ", MNC: " << tmp;
            }
          } else {
            ss << ", PLMN index: " << (uint32_t)sess->tmgi_r9.plmn_index_r9;
          }
          ss << ", LCID: " << (uint32_t)sess->logicalchannelid_r9;
          ss << std::endl;
        }
      }
      //rrc_log->console(ss.str());
      std::cout << ss.str();
    } else {
      rrc_log->console("MCCH not available for current cell\n");
    }
  }
}

bool rrc::mbms_service_start(uint32_t serv, uint32_t port)
{
  bool ret = false;
  
  if(serving_cell->has_mcch) {
    LIBLTE_RRC_MCCH_MSG_STRUCT msg;
    memcpy(&msg, &serving_cell->mcch, sizeof(LIBLTE_RRC_MCCH_MSG_STRUCT));
    for(uint32_t i=0;i<msg.pmch_infolist_r9_size; i++){
      LIBLTE_RRC_PMCH_INFO_R9_STRUCT *pmch = &msg.pmch_infolist_r9[i];
      for(uint32_t j=0;j<pmch->mbms_sessioninfolist_r9_size; j++) {
        LIBLTE_RRC_MBMS_SESSION_INFO_R9_STRUCT *sess = &pmch->mbms_sessioninfolist_r9[j];
        if(serv == sess->tmgi_r9.serviceid_r9) {
          rrc_log->console("MBMS service started. Service id:%d, port: %d\n", serv, port);
          ret = true;
          add_mrb(sess->logicalchannelid_r9, port);
        }
      }
    }
  }
  return ret;
}


void rrc::init(phy_interface_rrc *phy_,
               mac_interface_rrc *mac_,
               rlc_interface_rrc *rlc_,
               pdcp_interface_rrc *pdcp_,
               nas_interface_rrc *nas_,
               usim_interface_rrc *usim_,
               gw_interface_rrc *gw_,
               mac_interface_timers *mac_timers_,
               srslte::log *rrc_log_) {
  pool = byte_buffer_pool::get_instance();
  phy = phy_;
  mac = mac_;
  rlc = rlc_;
  pdcp = pdcp_;
  nas = nas_;
  usim = usim_;
  gw = gw_;
  rrc_log = rrc_log_;

  // Use MAC timers
  mac_timers = mac_timers_;
  state = RRC_STATE_IDLE;
  plmn_is_selected = false;

  security_is_activated = false;

  pthread_mutex_init(&mutex, NULL);

  args.ue_category = SRSLTE_UE_CATEGORY;
  args.supported_bands[0] = 7;
  args.nof_supported_bands = 1;
  args.feature_group = 0xe6041000;

  t300 = mac_timers->timer_get_unique_id();
  t301 = mac_timers->timer_get_unique_id();
  t302 = mac_timers->timer_get_unique_id();
  t310 = mac_timers->timer_get_unique_id();
  t311 = mac_timers->timer_get_unique_id();
  t304 = mac_timers->timer_get_unique_id();

  dedicatedInfoNAS = NULL;
  ueIdentity_configured = false;

  transaction_id = 0;

  // Register logging handler with liblte_rrc
  liblte_rrc_log_register_handler(this, liblte_rrc_handler);

  cell_clean_cnt = 0;

  ho_start = false;

  pending_mob_reconf = false;

  // Set default values for all layers
  set_rrc_default();
  set_phy_default();
  set_mac_default();

  measurements.init(this);
  // set seed for rand (used in attach)
  srand(time(NULL));

  running = true;
  start();
  initiated = true;
}

void rrc::stop() {
  running = false;
  cmd_msg_t msg;
  msg.command = cmd_msg_t::STOP;
  cmd_q.push(msg);
  wait_thread_finish();
}

rrc_state_t rrc::get_state() {
  return state;
}

bool rrc::is_connected() {
  return (RRC_STATE_CONNECTED == state);
}

bool rrc::have_drb() {
  return drb_up;
}

void rrc::set_args(rrc_args_t *args) {
  memcpy(&this->args, args, sizeof(rrc_args_t));
}

/*
 * Low priority thread to run functions that can not be executed from main thread
 */
void rrc::run_thread() {
  while(running) {
    cmd_msg_t msg = cmd_q.wait_pop();
    switch(msg.command) {
      case cmd_msg_t::STOP:
        return;
      case cmd_msg_t::PCCH:
        process_pcch(msg.pdu);
        break;
    }
  }
}


/*
 *
 * RRC State Machine
 *
 */
void rrc::run_tti(uint32_t tti) {

  if (!initiated) {
    return;
  }

  /* We can not block in this thread because it is called from
   * the MAC TTI timer and needs to return immediatly to perform other
   * tasks. Therefore in this function we use trylock() instead of lock() and
   * skip function if currently locked, since none of the functions here is urgent
   */
  if (!pthread_mutex_trylock(&mutex)) {

    // Process pending PHY measurements in IDLE/CONNECTED
    process_phy_meas();

    // Run state machine
    rrc_log->debug("State %s\n", rrc_state_text[state]);
    switch (state) {
      case RRC_STATE_IDLE:

        /* CAUTION: The execution of cell_search() and cell_selection() take more than 1 ms
         * and will slow down MAC TTI ticks. This has no major effect at the moment because
         * the UE is in IDLE but we could consider splitting MAC and RRC threads to avoid this
         */

        // If attached but not camping on the cell, perform cell reselection
        if (nas->is_attached()) {
          rrc_log->debug("Running cell selection and reselection in IDLE\n");
          switch(cell_selection()) {
            case rrc::CHANGED_CELL:
              // New cell has been selected, start receiving PCCH
              mac->pcch_start_rx();
              break;
            case rrc::NO_CELL:
              rrc_log->warning("Could not find any cell to camp on\n");
              break;
            case rrc::SAME_CELL:
              if (!phy->cell_is_camping()) {
                rrc_log->warning("Did not reselect cell but serving cell is out-of-sync.\n");
                serving_cell->in_sync = false;
              }
              break;
          }
        }
        break;
      case RRC_STATE_CONNECTED:
        if (ho_start) {
          ho_start = false;
          if (!ho_prepare()) {
            con_reconfig_failed();
          }
        }
        measurements.run_tti(tti);
        if (go_idle) {
          go_idle = false;
          leave_connected();
        }
        if (go_rlf) {
          go_rlf = false;
          // Initiate connection re-establishment procedure after RLF
          send_con_restablish_request(LIBLTE_RRC_CON_REEST_REQ_CAUSE_OTHER_FAILURE);
        }
        break;
      default:break;
    }

    // Clean old neighbours
    cell_clean_cnt++;
    if (cell_clean_cnt == 1000) {
      clean_neighbours();
      cell_clean_cnt = 0;
    }
    pthread_mutex_unlock(&mutex);
  } // Skip TTI if mutex is locked
}









/*******************************************************************************
*
*
*
* NAS interface: PLMN search and RRC connection establishment
*
*
*
*******************************************************************************/

uint16_t rrc::get_mcc() {
  return serving_cell->get_mcc();
}

uint16_t rrc::get_mnc() {
  return serving_cell->get_mnc();
}

/* NAS interface to search for available PLMNs.
 * It goes through all known frequencies, synchronizes and receives SIB1 for each to extract PLMN.
 * The function is blocking and waits until all frequencies have been
 * searched and PLMNs are obtained.
 *
 * This function is thread-safe with connection_request()
 */
int rrc::plmn_search(found_plmn_t found_plmns[MAX_FOUND_PLMNS])
{
  // Mutex with connect
  pthread_mutex_lock(&mutex);

  rrc_log->info("Starting PLMN search\n");
  uint32_t nof_plmns = 0;
  phy_interface_rrc::cell_search_ret_t ret;
  do {
    ret = cell_search();
    if (ret.found == phy_interface_rrc::cell_search_ret_t::CELL_FOUND) {
      if (serving_cell->has_sib1()) {
        // Save PLMN and TAC to NAS
        for (uint32_t i = 0; i < serving_cell->nof_plmns(); i++) {
          if (nof_plmns < MAX_FOUND_PLMNS) {
            found_plmns[nof_plmns].plmn_id = serving_cell->get_plmn(i);
            found_plmns[nof_plmns].tac = serving_cell->get_tac();
            nof_plmns++;
          } else {
            rrc_log->error("No more space for plmns (%d)\n", nof_plmns);
          }
        }
      } else {
        rrc_log->error("SIB1 not acquired\n");
      }
    }
  } while (ret.last_freq == phy_interface_rrc::cell_search_ret_t::MORE_FREQS &&
           ret.found     != phy_interface_rrc::cell_search_ret_t::ERROR);

  // Process all pending measurements before returning
  process_phy_meas();

  pthread_mutex_unlock(&mutex);

  if (ret.found == phy_interface_rrc::cell_search_ret_t::ERROR) {
    return -1; 
  } else {
    return nof_plmns;
  }
}

/* This is the NAS interface. When NAS requests to select a PLMN we have to
 * connect to either register or because there is pending higher layer traffic.
 */
void rrc::plmn_select(LIBLTE_RRC_PLMN_IDENTITY_STRUCT plmn_id) {
  plmn_is_selected = true;
  selected_plmn_id = plmn_id;

  rrc_log->info("PLMN Selected %s\n", plmn_id_to_string(plmn_id).c_str());
}

/* 5.3.3.2 Initiation of RRC Connection Establishment procedure
 *
 * Higher layers request establishment of RRC connection while UE is in RRC_IDLE
 *
 * This procedure selects a suitable cell for transmission of RRCConnectionRequest and configures
 * it. Sends connectionRequest message and returns if message transmitted successfully.
 * It does not wait until completition of Connection Establishment procedure
 */
bool rrc::connection_request(LIBLTE_RRC_CON_REQ_EST_CAUSE_ENUM cause,
                             srslte::byte_buffer_t *dedicatedInfoNAS)
{

  if (!plmn_is_selected) {
    rrc_log->error("Trying to connect but PLMN not selected.\n");
    return false;
  }

  if (state != RRC_STATE_IDLE) {
    rrc_log->warning("Requested RRC connection establishment while not in IDLE\n");
    return false;
  }

  if (mac_timers->timer_get(t302)->is_running()) {
    rrc_log->info("Requested RRC connection establishment while T302 is running\n");
    nas->set_barring(nas_interface_rrc::BARRING_MO_DATA);
    return false;
  }

  bool ret = false;

  pthread_mutex_lock(&mutex);

  rrc_log->info("Initiation of Connection establishment procedure\n");

  // Perform cell selection & reselection for the selected PLMN
  cs_ret_t cs_ret = cell_selection();

  // .. and SI acquisition
  if (phy->cell_is_camping()) {

    // Set default configurations
    set_phy_default();
    set_mac_default();

    // CCCH configuration applied already at start
    // timeAlignmentCommon applied in configure_serving_cell

    rrc_log->info("Configuring serving cell...\n");
    if (configure_serving_cell()) {

      mac_timers->timer_get(t300)->reset();
      mac_timers->timer_get(t300)->run();

      // Send connectionRequest message to lower layers
      send_con_request(cause);

      // Save dedicatedInfoNAS SDU
      if (this->dedicatedInfoNAS) {
        rrc_log->warning("Received a new dedicatedInfoNAS SDU but there was one still in queue. Removing it\n");
        pool->deallocate(this->dedicatedInfoNAS);
      }
      this->dedicatedInfoNAS = dedicatedInfoNAS;

      // Wait until t300 stops due to RRCConnectionSetup/Reject or expiry
      while (mac_timers->timer_get(t300)->is_running()) {
        usleep(1000);
      }

      if (state == RRC_STATE_CONNECTED) {
        // Received ConnectionSetup
        ret = true;
      } else if (mac_timers->timer_get(t300)->is_expired()) {
        // T300 is expired: 5.3.3.6
        rrc_log->info("Timer T300 expired: ConnectionRequest timed out\n");
        mac->reset();
        set_mac_default();
        rlc->reestablish();
      } else {
        // T300 is stopped but RRC not Connected is because received Reject: Section 5.3.3.8
        rrc_log->info("Timer T300 stopped: Received ConnectionReject\n");
        mac->reset();
        set_mac_default();
      }

    } else {
      rrc_log->error("Configuring serving cell\n");
    }
  } else {
    switch(cs_ret) {
      case SAME_CELL:
        rrc_log->warning("Did not reselect cell but serving cell is out-of-sync.\n");
        serving_cell->in_sync = false;
      break;
      case CHANGED_CELL:
        rrc_log->warning("Selected a new cell but could not camp on. Setting out-of-sync.\n");
        serving_cell->in_sync = false;
        break;
      default:
        rrc_log->warning("Could not find any suitable cell to connect\n");
    }
  }

  if (!ret) {
    rrc_log->warning("Could not estblish connection. Deallocating dedicatedInfoNAS PDU\n");
    pool->deallocate(this->dedicatedInfoNAS);
    this->dedicatedInfoNAS = NULL;
  }

  pthread_mutex_unlock(&mutex);
  return ret;
}

void rrc::set_ue_idenity(LIBLTE_RRC_S_TMSI_STRUCT s_tmsi) {
  ueIdentity_configured = true;
  ueIdentity = s_tmsi;
  rrc_log->info("Set ue-Identity to 0x%x:0x%x\n", ueIdentity.mmec, ueIdentity.m_tmsi);
}

/* Retrieves all required SIB or configures them if already retrieved before
 */
bool rrc::configure_serving_cell() {

  if (!phy->cell_is_camping()) {
    rrc_log->error("Trying to configure Cell while not camping on it\n");
    return false;
  }
  serving_cell->has_mcch = false;
  // Obtain the SIBs if not available or apply the configuration if available
  for (uint32_t i = 0; i < NOF_REQUIRED_SIBS; i++) {
    if (!serving_cell->has_sib(required_sibs[i])) {
      rrc_log->info("Cell has no SIB%d. Obtaining SIB%d\n", required_sibs[i]+1, required_sibs[i]+1);
      if (!si_acquire(required_sibs[i])) {
        rrc_log->info("Timeout while acquiring SIB%d\n", required_sibs[i]+1);
        if (required_sibs[i] < 2) {
          return false;
        }
      }
    } else {
      rrc_log->info("Cell has SIB%d\n", required_sibs[i]+1);
      switch(required_sibs[i]) {
        case 1:
          apply_sib2_configs(serving_cell->sib2ptr());
          break;
        case 12:
          apply_sib13_configs(serving_cell->sib13ptr());
          break;
      }
    }
  }
  return true;
}






/*******************************************************************************
*
*
*
* PHY interface: neighbour and serving cell measurements and out-of-sync/in-sync
*
*
*
*******************************************************************************/

/* This function is called from a PHY worker thus must return very quickly.
 * Queue the values of the measurements and process them from the RRC thread
 */
void rrc::new_phy_meas(float rsrp, float rsrq, uint32_t tti, int earfcn_i, int pci_i) {
  uint32_t pci    = 0;
  uint32_t earfcn = 0;
  if (earfcn_i < 0) {
    earfcn = (uint32_t) serving_cell->get_earfcn();
  } else {
    earfcn = (uint32_t) earfcn_i;
  }
  if (pci_i < 0) {
    pci    = (uint32_t) serving_cell->get_pci();
  } else {
    pci    = (uint32_t) pci_i;
  }
  phy_meas_t new_meas = {rsrp, rsrq, tti, earfcn, pci};
  phy_meas_q.push(new_meas);
  rrc_log->info("MEAS:  New measurement pci=%d, rsrp=%.1f dBm.\n", pci, rsrp);
}

/* Processes all pending PHY measurements in queue. Must be called from a mutexed function
 */
void rrc::process_phy_meas() {
  phy_meas_t m;
  while(phy_meas_q.try_pop(&m)) {
    rrc_log->debug("MEAS:  Processing measurement. %lu measurements in queue\n", phy_meas_q.size());
    process_new_phy_meas(m);
  }
}

void rrc::process_new_phy_meas(phy_meas_t meas)
{
  float rsrp   = meas.rsrp;
  float rsrq   = meas.rsrq;
  uint32_t tti = meas.tti;
  uint32_t earfcn = meas.earfcn;
  uint32_t pci    = meas.pci;

  // Measurements in RRC_CONNECTED go through measurement class to log reports etc.
  if (state != RRC_STATE_IDLE) {
    measurements.new_phy_meas(earfcn, pci, rsrp, rsrq, tti);

    // Measurements in RRC_IDLE update serving cell
  } else {

    // Update serving cell
    if (serving_cell->equals(earfcn, pci)) {
      serving_cell->set_rsrp(rsrp);
      // Or update/add neighbour cell
    } else {
      add_neighbour_cell(earfcn, pci, rsrp);
    }
  }
}

// Detection of physical layer problems in RRC_CONNECTED (5.3.11.1)
void rrc::out_of_sync()
{

  // CAUTION: We do not lock in this function since they are called from real-time threads

  serving_cell->in_sync = false;
  rrc_log->info("Received out-of-sync while in state %s. n310=%d, t311=%s, t310=%s\n",
                rrc_state_text[state], n310_cnt,
                mac_timers->timer_get(t311)->is_running()?"running":"stop",
                mac_timers->timer_get(t310)->is_running()?"running":"stop");
  if (state == RRC_STATE_CONNECTED) {
    if (!mac_timers->timer_get(t311)->is_running() && !mac_timers->timer_get(t310)->is_running()) {
      n310_cnt++;
      if (n310_cnt == N310) {
        rrc_log->info("Detected %d out-of-sync from PHY. Trying to resync. Starting T310 timer %d ms\n",
                      N310, mac_timers->timer_get(t310)->get_timeout());
        mac_timers->timer_get(t310)->reset();
        mac_timers->timer_get(t310)->run();
        n310_cnt = 0;
      }
    }
  }
}

// Recovery of physical layer problems (5.3.11.2)
void rrc::in_sync()
{

  // CAUTION: We do not lock in this function since they are called from real-time threads

  serving_cell->in_sync = true;
  if (mac_timers->timer_get(t310)->is_running()) {
    n311_cnt++;
    if (n311_cnt == N311) {
      mac_timers->timer_get(t310)->stop();
      n311_cnt = 0;
      rrc_log->info("Detected %d in-sync from PHY. Stopping T310 timer\n", N311);
    }
  }
}
















/*******************************************************************************
*
*
*
* System Information Acquisition procedure
*
*
*
*******************************************************************************/


// Determine SI messages scheduling as in 36.331 5.2.3 Acquisition of an SI message
uint32_t rrc::sib_start_tti(uint32_t tti, uint32_t period, uint32_t offset, uint32_t sf) {
  return (period*10*(1+tti/(period*10))+(offset*10)+sf)%10240; // the 1 means next opportunity
}

/* Implemnets the SI acquisition procedure
 * Configures the MAC/PHY scheduling to retrieve SI messages. The function is blocking and will not
 * return until SIB is correctly received or timeout
 */
bool rrc::si_acquire(uint32_t sib_index)
{
  uint32_t tti;
  uint32_t si_win_start=0, si_win_len=0;
  uint16_t period;
  uint32_t sched_index;
  uint32_t x, sf, offset;

  uint32_t last_win_start = 0;
  uint32_t timeout = 0;

  while(timeout < SIB_SEARCH_TIMEOUT_MS && !serving_cell->has_sib(sib_index)) {

    bool instruct_phy = false;

    if (sib_index == 0) {

      // Instruct MAC to look for SIB1
      tti = mac->get_current_tti();
      si_win_start = sib_start_tti(tti, 2, 0, 5);
      if (last_win_start == 0 ||
          (srslte_tti_interval(tti, last_win_start) >= 20 && srslte_tti_interval(tti, last_win_start) < 1000)) {

        last_win_start = si_win_start;
        si_win_len = 1;
        instruct_phy = true;
      }
      period = 20;
      sched_index = 0;
    } else {
      // Instruct MAC to look for SIB2..13
      if (serving_cell->has_sib1()) {

        LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_1_STRUCT *sib1 = serving_cell->sib1ptr();

        // SIB2 scheduling
        if (sib_index == 1) {
          period      = liblte_rrc_si_periodicity_num[sib1->sched_info[0].si_periodicity];
          sched_index = 0;
        } else {
          // SIB3+ scheduling Section 5.2.3
          if (sib_index >= 2) {
            bool found = false;
            for (uint32_t i=0;i<sib1->N_sched_info && !found;i++) {
              for (uint32_t j=0;j<sib1->sched_info[i].N_sib_mapping_info && !found;j++) {
                if ((uint32_t) sib1->sched_info[i].sib_mapping_info[j].sib_type == sib_index - 2) {
                  period      = liblte_rrc_si_periodicity_num[sib1->sched_info[i].si_periodicity];
                  sched_index = i;
                  found       = true;
                }
              }
            }
            if (!found) {
              rrc_log->info("Could not find SIB%d scheduling in SIB1\n", sib_index+1);
              return false;
            }
          }
        }
        si_win_len   = liblte_rrc_si_window_length_num[sib1->si_window_length];
        x            = sched_index*si_win_len;
        sf           = x%10;
        offset       = x/10;

        tti          = mac->get_current_tti();
        si_win_start = sib_start_tti(tti, period, offset, sf);
        si_win_len = liblte_rrc_si_window_length_num[sib1->si_window_length];

        if (last_win_start == 0 ||
            (srslte_tti_interval(tti, last_win_start) > period*5 && srslte_tti_interval(tti, last_win_start) < 1000))
        {
          last_win_start = si_win_start;
          instruct_phy = true;
        }
      } else {
        rrc_log->error("Trying to receive SIB%d but SIB1 not received\n", sib_index+1);
      }
    }

    // Instruct MAC to decode SIB
    if (instruct_phy && !serving_cell->has_sib(sib_index)) {
      mac->bcch_start_rx(si_win_start, si_win_len);
      rrc_log->info("Instructed MAC to search for SIB%d, win_start=%d, win_len=%d, period=%d, sched_index=%d\n",
                    sib_index+1, si_win_start, si_win_len, period, sched_index);
    }
    usleep(1000);
    timeout++;
  }
  return serving_cell->has_sib(sib_index);
}









/*******************************************************************************
*
*
*
* Cell selection, reselection and neighbour cell database management
*
*
*
*******************************************************************************/

/* Searches for a cell in the current frequency and retrieves SIB1 if not retrieved yet
 */
phy_interface_rrc::cell_search_ret_t rrc::cell_search()
{
  phy_interface_rrc::phy_cell_t new_cell;

  phy_interface_rrc::cell_search_ret_t ret = phy->cell_search(&new_cell);

  switch(ret.found) {
    case phy_interface_rrc::cell_search_ret_t::CELL_FOUND:
      rrc_log->info("Cell found in this frequency. Setting new serving cell...\n");

      // Create cell with NaN RSRP. Will be updated by new_phy_meas() during SIB search.
      if (!add_neighbour_cell(new_cell, NAN)) {
        rrc_log->info("No more space for neighbour cells\n");
        break;
      }
      set_serving_cell(new_cell);

      if (phy->cell_is_camping()) {
        if (!serving_cell->has_sib1()) {
          rrc_log->info("Cell has no SIB1. Obtaining SIB1\n");
          if (!si_acquire(0)) {
            rrc_log->error("Timeout while acquiring SIB1\n");
          }
        } else {
          rrc_log->info("Cell has SIB1\n");
        }
      } else {
        rrc_log->warning("Could not camp on found cell. Trying next one...\n");
      }
      break;
    case phy_interface_rrc::cell_search_ret_t::CELL_NOT_FOUND:
      rrc_log->info("No cells found.\n");
      break;
    case phy_interface_rrc::cell_search_ret_t::ERROR:
      rrc_log->error("In cell search. Finishing PLMN search\n");
      break;
  }
  return ret;
}

/* Cell selection procedure 36.304 5.2.3
 * Select the best cell to camp on among the list of known cells
 */
rrc::cs_ret_t rrc::cell_selection()
{
  // Neighbour cells are sorted in descending order of RSRP
  for (uint32_t i = 0; i < neighbour_cells.size(); i++) {
    if (/*TODO: CHECK that PLMN matches. Currently we don't receive SIB1 of neighbour cells
         * neighbour_cells[i]->plmn_equals(selected_plmn_id) && */
        neighbour_cells[i]->in_sync) // matches S criteria
    {
      // If currently connected, verify cell selection criteria
      if (!serving_cell->in_sync ||
          (cell_selection_criteria(neighbour_cells[i]->get_rsrp())  &&
              neighbour_cells[i]->get_rsrp() > serving_cell->get_rsrp() + 5))
      {
        // Try to select Cell
        set_serving_cell(i);
        rrc_log->info("Selected cell idx=%d, PCI=%d, EARFCN=%d\n",
                      i, serving_cell->get_pci(), serving_cell->get_earfcn());
        rrc_log->console("Selected cell PCI=%d, EARFCN=%d\n",
                         serving_cell->get_pci(), serving_cell->get_earfcn());

        if (phy->cell_select(&serving_cell->phy_cell)) {
          if (configure_serving_cell()) {
            rrc_log->info("Selected and configured cell successfully\n");
            return CHANGED_CELL;
          } else {
            rrc_log->error("While configuring serving cell\n");
          }
        } else {
          serving_cell->in_sync = false;
          rrc_log->warning("Could not camp on selected cell\n");
        }
      }
    }
  }
  if (serving_cell->in_sync) {
    if (!phy->cell_is_camping()) {
      rrc_log->info("Serving cell is in-sync but not camping. Selecting it...\n");
      if (phy->cell_select(&serving_cell->phy_cell)) {
        rrc_log->info("Selected serving cell OK.\n");
      } else {
        serving_cell->in_sync = false;
        rrc_log->error("Could not camp on serving cell.\n");
      }
    }
    return SAME_CELL;
  }
  // If can not find any suitable cell, search again
  rrc_log->info("Cell selection and reselection in IDLE did not find any suitable cell. Searching again\n");
  // If can not camp on any cell, search again for new cells
  phy_interface_rrc::cell_search_ret_t ret = cell_search();

  return (ret.found == phy_interface_rrc::cell_search_ret_t::CELL_FOUND)?CHANGED_CELL:NO_CELL;
}

// Cell selection criteria Section 5.2.3.2 of 36.304
bool rrc::cell_selection_criteria(float rsrp, float rsrq)
{
  if (get_srxlev(rsrp) > 0 || !serving_cell->has_sib3()) {
    return true;
  } else {
    return false;
  }
}

float rrc::get_srxlev(float Qrxlevmeas) {
  // TODO: Do max power limitation
  float Pcompensation = 0;
  return Qrxlevmeas - (cell_resel_cfg.Qrxlevmin + cell_resel_cfg.Qrxlevminoffset) - Pcompensation;
}

float rrc::get_squal(float Qqualmeas) {
  return Qqualmeas - (cell_resel_cfg.Qqualmin + cell_resel_cfg.Qqualminoffset);
}

// Cell reselection in IDLE Section 5.2.4 of 36.304
void rrc::cell_reselection(float rsrp, float rsrq)
{
  // Intra-frequency cell-reselection criteria

  if (get_srxlev(rsrp) > cell_resel_cfg.s_intrasearchP && rsrp > -95.0) {
    // UE may not perform intra-frequency measurements.
    phy->meas_reset();
    // keep measuring serving cell
    phy->meas_start(phy->get_current_earfcn(), phy->get_current_pci());
  } else {
    // UE must start intra-frequency measurements
    phy->meas_start(phy->get_current_earfcn(), -1);
  }

  // TODO: Inter-frequency cell reselection
}

// Set new serving cell
void rrc::set_serving_cell(phy_interface_rrc::phy_cell_t phy_cell) {
  int cell_idx = find_neighbour_cell(phy_cell.earfcn, phy_cell.cell.id);
  if (cell_idx >= 0) {
    set_serving_cell(cell_idx);
  } else {
    rrc_log->error("Setting serving cell: Unkonwn cell with earfcn=%d, PCI=%d\n", phy_cell.earfcn, phy_cell.cell.id);
  }
}

// Set new serving cell
void rrc::set_serving_cell(uint32_t cell_idx) {

  if (cell_idx < neighbour_cells.size())
  {
    // Remove future serving cell from neighbours to make space for current serving cell
    cell_t *new_serving_cell = neighbour_cells[cell_idx];
    if (!new_serving_cell) {
      rrc_log->error("Setting serving cell. Index %d is empty\n", cell_idx);
      return;
    }
    neighbour_cells.erase(std::remove(neighbour_cells.begin(), neighbour_cells.end(), neighbour_cells[cell_idx]), neighbour_cells.end());

    // Move serving cell to neighbours list
    if (serving_cell->is_valid()) {
      // Make sure it does not exist already
      int serving_idx = find_neighbour_cell(serving_cell->get_earfcn(), serving_cell->get_pci());
      if (serving_idx >= 0 && (uint32_t) serving_idx < neighbour_cells.size()) {
        printf("Error serving cell is already in the neighbour list. Removing it\n");
        neighbour_cells.erase(std::remove(neighbour_cells.begin(), neighbour_cells.end(), neighbour_cells[serving_idx]), neighbour_cells.end());
      }
      // If not in the list, add it to the list of neighbours (sorted inside the function)
      if (!add_neighbour_cell(serving_cell)) {
        rrc_log->info("Serving cell not added to list of neighbours. Worse than current neighbours\n");
      }
    }

    // Set new serving cell
    serving_cell = new_serving_cell;

    rrc_log->info("Setting serving cell idx=%d, earfcn=%d, PCI=%d, nof_neighbours=%lu\n",
                  cell_idx, serving_cell->get_earfcn(), serving_cell->get_pci(), neighbour_cells.size());

  } else {
    rrc_log->error("Setting invalid serving cell idx %d\n", cell_idx);
  }
}

bool sort_rsrp(cell_t *u1, cell_t *u2) {
  return u1->greater(u2);
}

void rrc::delete_neighbour(uint32_t cell_idx) {
  measurements.delete_report(neighbour_cells[cell_idx]->get_earfcn(), neighbour_cells[cell_idx]->get_pci());
  delete neighbour_cells[cell_idx];
  neighbour_cells.erase(std::remove(neighbour_cells.begin(), neighbour_cells.end(), neighbour_cells[cell_idx]), neighbour_cells.end());
}

std::vector<cell_t*>::iterator rrc::delete_neighbour(std::vector<cell_t*>::iterator it) {
  measurements.delete_report((*it)->get_earfcn(), (*it)->get_pci());
  delete (*it);
  return neighbour_cells.erase(it);
}

/* Called by main RRC thread to remove neighbours from which measurements have not been received in a while
 */
void rrc::clean_neighbours()
{
  struct timeval now;
  gettimeofday(&now, NULL);

  std::vector<cell_t*>::iterator it = neighbour_cells.begin();
  while(it != neighbour_cells.end()) {
    if ((*it)->timeout_secs(now) > NEIGHBOUR_TIMEOUT) {
      rrc_log->info("Neighbour PCI=%d timed out. Deleting\n", (*it)->get_pci());
      it = delete_neighbour(it);
    } else {
      ++it;
    }
  }
}

// Sort neighbour cells by decreasing order of RSRP
void rrc::sort_neighbour_cells()
{
  // Remove out-of-sync cells
  std::vector<cell_t*>::iterator it = neighbour_cells.begin();
  while(it != neighbour_cells.end()) {
    if ((*it)->in_sync == false) {
      rrc_log->info("Neighbour PCI=%d is out-of-sync. Deleting\n", (*it)->get_pci());
      it = delete_neighbour(it);
    } else {
      ++it;
    }
  }

  std::sort(neighbour_cells.begin(), neighbour_cells.end(), sort_rsrp);

  if (neighbour_cells.size() > 0) {
    char ordered[512];
    int n=0;
    n += snprintf(ordered, 512, "[pci=%d, rsrp=%.2f", neighbour_cells[0]->phy_cell.cell.id, neighbour_cells[0]->get_rsrp());
    for (uint32_t i=1;i<neighbour_cells.size();i++) {
      n += snprintf(&ordered[n], 512-n, " | pci=%d, rsrp=%.2f", neighbour_cells[i]->get_pci(), neighbour_cells[i]->get_rsrp());
    }
    rrc_log->info("Neighbours: %s]\n", ordered);
  } else {
    rrc_log->info("Neighbours: Empty\n");
  }
}

bool rrc::add_neighbour_cell(cell_t *new_cell) {
  bool ret = false;
  if (neighbour_cells.size() < NOF_NEIGHBOUR_CELLS) {
    ret = true;
  } else if (new_cell->greater(neighbour_cells[neighbour_cells.size()-1])) {
    // Replace old one by new one
    delete_neighbour(neighbour_cells.size()-1);
    ret = true;
  }
  if (ret) {
    neighbour_cells.push_back(new_cell);
  }
  rrc_log->info("Added neighbour cell EARFCN=%d, PCI=%d, nof_neighbours=%zd\n",
                new_cell->get_earfcn(), new_cell->get_pci(), neighbour_cells.size());
  sort_neighbour_cells();
  return ret;
}

// If only neighbour PCI is provided, copy full cell from serving cell
bool rrc::add_neighbour_cell(uint32_t earfcn, uint32_t pci, float rsrp) {
  phy_interface_rrc::phy_cell_t phy_cell;
  phy_cell = serving_cell->phy_cell;
  phy_cell.earfcn = earfcn;
  phy_cell.cell.id = pci;
  return add_neighbour_cell(phy_cell, rsrp);
}

bool rrc::add_neighbour_cell(phy_interface_rrc::phy_cell_t phy_cell, float rsrp) {
  if (phy_cell.earfcn == 0) {
    phy_cell.earfcn = serving_cell->get_earfcn();
  }

  // First check if already exists
  int cell_idx = find_neighbour_cell(phy_cell.earfcn, phy_cell.cell.id);

  rrc_log->info("Adding PCI=%d, earfcn=%d, cell_idx=%d\n", phy_cell.cell.id, phy_cell.earfcn, cell_idx);

  // If exists, update RSRP if provided, sort again and return
  if (cell_idx >= 0 && isnormal(rsrp)) {
    neighbour_cells[cell_idx]->set_rsrp(rsrp);
    sort_neighbour_cells();
    return true;
  }

  // If not, create a new one
  cell_t *new_cell = new cell_t(phy_cell, rsrp);

  return add_neighbour_cell(new_cell);
}

int rrc::find_neighbour_cell(uint32_t earfcn, uint32_t pci) {
  for (uint32_t i = 0; i < neighbour_cells.size(); i++) {
    if (neighbour_cells[i]->equals(earfcn, pci)) {
      return (int) i;
    }
  }
  return -1;
}


/*******************************************************************************
*
*
*
* Other functions
*
*
*
*******************************************************************************/

/* Detection of radio link failure (5.3.11.3)
 * Upon T310 expiry, RA problem or RLC max retx
 */
void rrc::radio_link_failure() {
  // TODO: Generate and store failure report
  rrc_log->warning("Detected Radio-Link Failure\n");
  rrc_log->console("Warning: Detected Radio-Link Failure\n");
  if (state == RRC_STATE_CONNECTED) {
    go_rlf = true;
  }
}

/* Reception of PUCCH/SRS release procedure (Section 5.3.13) */
void rrc::release_pucch_srs() {
  // Apply default configuration for PUCCH (CQI and SR) and SRS (release)
  set_phy_default_pucch_srs();

  // Configure RX signals without pregeneration because default option is release
  phy->configure_ul_params(true);
}

void rrc::ra_problem() {
  radio_link_failure();
}

void rrc::max_retx_attempted() {
  //TODO: Handle the radio link failure
  rrc_log->warning("Max RLC reTx attempted\n");
  radio_link_failure();
}

void rrc::timer_expired(uint32_t timeout_id) {
  if (timeout_id == t310) {
    rrc_log->info("Timer T310 expired: Radio Link Failure\n");
    radio_link_failure();
  } else if (timeout_id == t311) {
    rrc_log->info("Timer T311 expired: Going to RRC IDLE\n");
    go_idle = true;
  } else if (timeout_id == t301) {
    if (state == RRC_STATE_IDLE) {
      rrc_log->info("Timer T301 expired: Already in IDLE.\n");
    } else {
      rrc_log->info("Timer T301 expired: Going to RRC IDLE\n");
      go_idle = true;
    }
  } else if (timeout_id == t302) {
    rrc_log->info("Timer T302 expired. Informing NAS about barrier alleviation\n");
    nas->set_barring(nas_interface_rrc::BARRING_NONE);
  } else if (timeout_id == t300) {
    // Do nothing, handled in connection_request()
  } else if (timeout_id == t304) {
    rrc_log->console("Timer T304 expired: Handover failed\n");
    ho_failed();
  // fw to measurement
  } else if (!measurements.timer_expired(timeout_id)) {
    rrc_log->error("Timeout from unknown timer id %d\n", timeout_id);
  }
}








/*******************************************************************************
*
*
*
* Connection Control: Establishment, Reconfiguration, Reestablishment and Release
*
*
*
*******************************************************************************/

void rrc::send_con_request(LIBLTE_RRC_CON_REQ_EST_CAUSE_ENUM cause) {
  rrc_log->debug("Preparing RRC Connection Request\n");
  bzero(&ul_ccch_msg, sizeof(LIBLTE_RRC_UL_CCCH_MSG_STRUCT));

  // Prepare ConnectionRequest packet
  ul_ccch_msg.msg_type = LIBLTE_RRC_UL_CCCH_MSG_TYPE_RRC_CON_REQ;

  if (ueIdentity_configured) {
    ul_ccch_msg.msg.rrc_con_req.ue_id_type = LIBLTE_RRC_CON_REQ_UE_ID_TYPE_S_TMSI;
    ul_ccch_msg.msg.rrc_con_req.ue_id.s_tmsi.m_tmsi = ueIdentity.m_tmsi;
    ul_ccch_msg.msg.rrc_con_req.ue_id.s_tmsi.mmec   = ueIdentity.mmec;
  } else {
    ul_ccch_msg.msg.rrc_con_req.ue_id_type = LIBLTE_RRC_CON_REQ_UE_ID_TYPE_RANDOM_VALUE;
    // TODO use proper RNG
    uint64_t random_id = 0;
    for (uint i = 0; i < 5; i++) { // fill random ID bytewise, 40 bits = 5 bytes
      random_id |= ( (uint64_t)rand() & 0xFF ) << i*8;
    }
    ul_ccch_msg.msg.rrc_con_req.ue_id.random = random_id;
  }

  ul_ccch_msg.msg.rrc_con_req.cause = cause;

  send_ul_ccch_msg();
}

/* RRC connection re-establishment procedure (5.3.7) */
void rrc::send_con_restablish_request(LIBLTE_RRC_CON_REEST_REQ_CAUSE_ENUM cause)
{
  bzero(&ul_ccch_msg, sizeof(LIBLTE_RRC_UL_CCCH_MSG_STRUCT));

  uint16_t crnti;
  uint16_t pci;
  uint32_t cellid;
  if (cause == LIBLTE_RRC_CON_REEST_REQ_CAUSE_HANDOVER_FAILURE) {
    crnti  = ho_src_rnti;
    pci    = ho_src_cell.get_pci();
    cellid = ho_src_cell.get_cell_id();
  } else {
    mac_interface_rrc::ue_rnti_t uernti;
    mac->get_rntis(&uernti);
    crnti  = uernti.crnti;
    pci    = serving_cell->get_pci();
    cellid = serving_cell->get_cell_id();
  }

  // Compute shortMAC-I
  uint8_t varShortMAC[128], varShortMAC_packed[16];
  bzero(varShortMAC, 128);
  bzero(varShortMAC_packed, 16);
  uint8_t *msg_ptr = varShortMAC;

  // ASN.1 encode VarShortMAC-Input
  liblte_rrc_pack_cell_identity_ie(cellid, &msg_ptr);
  liblte_rrc_pack_phys_cell_id_ie(pci, &msg_ptr);
  liblte_rrc_pack_c_rnti_ie(crnti, &msg_ptr);

  // byte align (already zero-padded)
  uint32_t N_bits  = (uint32_t) (msg_ptr-varShortMAC);
  uint32_t N_bytes = ((N_bits-1)/8+1);
  srslte_bit_pack_vector(varShortMAC, varShortMAC_packed, N_bytes*8);

  rrc_log->info("Encoded varShortMAC: cellId=0x%x, PCI=%d, rnti=0x%x (%d bytes, %d bits)\n",
                cellid, pci, crnti, N_bytes, N_bits);

  // Compute MAC-I
  uint8_t mac_key[4];
  switch(integ_algo) {
    case INTEGRITY_ALGORITHM_ID_128_EIA1:
      security_128_eia1(&k_rrc_int[16],
                        0xffffffff,    // 32-bit all to ones
                        0x1f,          // 5-bit all to ones
                        1,             // 1-bit to one
                        varShortMAC_packed,
                        N_bytes,
                        mac_key);
      break;
    case INTEGRITY_ALGORITHM_ID_128_EIA2:
      security_128_eia2(&k_rrc_int[16],
                        0xffffffff,    // 32-bit all to ones
                        0x1f,          // 5-bit all to ones
                        1,             // 1-bit to one
                        varShortMAC_packed,
                        N_bytes,
                        mac_key);
      break;
    default:
      rrc_log->info("Unsupported integrity algorithm during reestablishment\n");
  }

  // Prepare ConnectionRestalishmentRequest packet
  ul_ccch_msg.msg_type = LIBLTE_RRC_UL_CCCH_MSG_TYPE_RRC_CON_REEST_REQ;
  ul_ccch_msg.msg.rrc_con_reest_req.ue_id.c_rnti = crnti;
  ul_ccch_msg.msg.rrc_con_reest_req.ue_id.phys_cell_id = pci;
  ul_ccch_msg.msg.rrc_con_reest_req.ue_id.short_mac_i = mac_key[2] << 8 | mac_key[3];
  ul_ccch_msg.msg.rrc_con_reest_req.cause = cause;

  rrc_log->info("Initiating RRC Connection Reestablishment Procedure\n");
  rrc_log->console("RRC Connection Reestablishment\n");
  mac_timers->timer_get(t310)->stop();
  mac_timers->timer_get(t311)->reset();
  mac_timers->timer_get(t311)->run();

  phy->reset();
  set_phy_default();
  mac->reset();
  set_mac_default();

  // Perform cell selection in accordance to 36.304
  if (cell_selection_criteria(serving_cell->get_rsrp()) && serving_cell->in_sync) {
    if (phy->cell_select(&serving_cell->phy_cell)) {

      if (mac_timers->timer_get(t311)->is_running()) {
        // Actions following cell reselection while T311 is running 5.3.7.3
        rrc_log->info("Cell Selection finished. Initiating transmission of RRC Connection Reestablishment Request\n");
        liblte_rrc_pack_ul_ccch_msg(&ul_ccch_msg, (LIBLTE_BIT_MSG_STRUCT *) &bit_buf);

        mac_timers->timer_get(t301)->reset();
        mac_timers->timer_get(t301)->run();
        mac_timers->timer_get(t311)->stop();
        send_ul_ccch_msg();
      } else {
        rrc_log->info("T311 expired while selecting cell. Going to IDLE\n");
        go_idle = true;
      }
    } else {
      rrc_log->warning("Could not re-synchronize with cell.\n");
      go_idle = true;
    }
  } else {
    rrc_log->info("Selected cell no longer suitable for camping (in_sync=%s). Going to IDLE\n", serving_cell->in_sync?"yes":"no");
    go_idle = true;
  }
}

void rrc::send_con_restablish_complete() {
  bzero(&ul_dcch_msg, sizeof(LIBLTE_RRC_UL_DCCH_MSG_STRUCT));

  rrc_log->debug("Preparing RRC Connection Reestablishment Complete\n");

  rrc_log->console("RRC Connected\n");

  // Prepare ConnectionSetupComplete packet
  ul_dcch_msg.msg_type = LIBLTE_RRC_UL_DCCH_MSG_TYPE_RRC_CON_REEST_COMPLETE;
  ul_dcch_msg.msg.rrc_con_reest_complete.rrc_transaction_id = transaction_id;

  send_ul_dcch_msg();
}

void rrc::send_con_setup_complete(byte_buffer_t *nas_msg) {
  bzero(&ul_dcch_msg, sizeof(LIBLTE_RRC_UL_DCCH_MSG_STRUCT));
  rrc_log->debug("Preparing RRC Connection Setup Complete\n");

  // Prepare ConnectionSetupComplete packet
  ul_dcch_msg.msg_type = LIBLTE_RRC_UL_DCCH_MSG_TYPE_RRC_CON_SETUP_COMPLETE;
  ul_dcch_msg.msg.rrc_con_setup_complete.registered_mme_present = false;
  ul_dcch_msg.msg.rrc_con_setup_complete.rrc_transaction_id = transaction_id;
  ul_dcch_msg.msg.rrc_con_setup_complete.selected_plmn_id = 1;
  memcpy(ul_dcch_msg.msg.rrc_con_setup_complete.dedicated_info_nas.msg, nas_msg->msg, nas_msg->N_bytes);
  ul_dcch_msg.msg.rrc_con_setup_complete.dedicated_info_nas.N_bytes = nas_msg->N_bytes;

  pool->deallocate(nas_msg);
  
  send_ul_dcch_msg();
}

void rrc::send_ul_info_transfer(byte_buffer_t *nas_msg) {
  bzero(&ul_dcch_msg, sizeof(LIBLTE_RRC_UL_DCCH_MSG_STRUCT));

  rrc_log->debug("Preparing RX Info Transfer\n");

  // Prepare RX INFO packet
  ul_dcch_msg.msg_type = LIBLTE_RRC_UL_DCCH_MSG_TYPE_UL_INFO_TRANSFER;
  ul_dcch_msg.msg.ul_info_transfer.dedicated_info_type = LIBLTE_RRC_UL_INFORMATION_TRANSFER_TYPE_NAS;
  memcpy(ul_dcch_msg.msg.ul_info_transfer.dedicated_info.msg, nas_msg->msg, nas_msg->N_bytes);
  ul_dcch_msg.msg.ul_info_transfer.dedicated_info.N_bytes = nas_msg->N_bytes;

  pool->deallocate(nas_msg);

  send_ul_dcch_msg();
}

void rrc::send_security_mode_complete() {
  bzero(&ul_dcch_msg, sizeof(LIBLTE_RRC_UL_DCCH_MSG_STRUCT));
  rrc_log->debug("Preparing Security Mode Complete\n");

  ul_dcch_msg.msg_type = LIBLTE_RRC_UL_DCCH_MSG_TYPE_SECURITY_MODE_COMPLETE;
  ul_dcch_msg.msg.security_mode_complete.rrc_transaction_id = transaction_id;

  send_ul_dcch_msg();
}

void rrc::send_rrc_con_reconfig_complete() {
  bzero(&ul_dcch_msg, sizeof(LIBLTE_RRC_UL_DCCH_MSG_STRUCT));
  rrc_log->debug("Preparing RRC Connection Reconfig Complete\n");

  ul_dcch_msg.msg_type = LIBLTE_RRC_UL_DCCH_MSG_TYPE_RRC_CON_RECONFIG_COMPLETE;
  ul_dcch_msg.msg.rrc_con_reconfig_complete.rrc_transaction_id = transaction_id;

  send_ul_dcch_msg();
}

bool rrc::ho_prepare() {
  if (pending_mob_reconf) {
    rrc_log->info("Processing HO command to target PCell=%d\n", mob_reconf.mob_ctrl_info.target_pci);

    int target_cell_idx = find_neighbour_cell(serving_cell->get_earfcn(), mob_reconf.mob_ctrl_info.target_pci);
    if (target_cell_idx < 0) {
      rrc_log->console("Received HO command to unknown PCI=%d\n", mob_reconf.mob_ctrl_info.target_pci);
      rrc_log->error("Could not find target cell earfcn=%d, pci=%d\n",
                     serving_cell->get_earfcn(),
                     mob_reconf.mob_ctrl_info.target_pci);
      return false;
    }

    // Section 5.3.5.4
    mac_timers->timer_get(t310)->stop();
    mac_timers->timer_get(t304)->set(this, liblte_rrc_t304_num[mob_reconf.mob_ctrl_info.t304]);
    if (mob_reconf.mob_ctrl_info.carrier_freq_eutra_present &&
        mob_reconf.mob_ctrl_info.carrier_freq_eutra.dl_carrier_freq != serving_cell->get_earfcn()) {
      rrc_log->error("Received mobilityControlInfo for inter-frequency handover\n");
      return false;
    }

    // Save serving cell and current configuration
    ho_src_cell = *serving_cell;
    mac_interface_rrc::ue_rnti_t uernti;
    mac->get_rntis(&uernti);
    ho_src_rnti = uernti.crnti;

    // Reset/Reestablish stack
    mac->clear_rntis();
    phy->meas_reset();
    mac->wait_uplink();
    pdcp->reestablish();
    rlc->reestablish();
    mac->reset();
    phy->reset();

    mac->set_ho_rnti(mob_reconf.mob_ctrl_info.new_ue_id, mob_reconf.mob_ctrl_info.target_pci);
    apply_rr_config_common_dl(&mob_reconf.mob_ctrl_info.rr_cnfg_common);

    if (!phy->cell_select(&neighbour_cells[target_cell_idx]->phy_cell)) {
      rrc_log->error("Could not synchronize with target cell pci=%d. Trying to return to source PCI\n",
                     neighbour_cells[target_cell_idx]->get_pci());
      return false;
    }

    set_serving_cell(target_cell_idx);

    if (mob_reconf.mob_ctrl_info.rach_cnfg_ded_present) {
      rrc_log->info("Starting non-contention based RA with preamble_idx=%d, mask_idx=%d\n",
                    mob_reconf.mob_ctrl_info.rach_cnfg_ded.preamble_index,
                    mob_reconf.mob_ctrl_info.rach_cnfg_ded.prach_mask_index);
      mac->start_noncont_ho(mob_reconf.mob_ctrl_info.rach_cnfg_ded.preamble_index,
                            mob_reconf.mob_ctrl_info.rach_cnfg_ded.prach_mask_index);
    } else {
      rrc_log->info("Starting contention-based RA\n");
      mac->start_cont_ho();
    }

    int ncc = -1;
    if (mob_reconf.sec_cnfg_ho_present) {
      ncc = mob_reconf.sec_cnfg_ho.intra_lte.next_hop_chaining_count;
      if (mob_reconf.sec_cnfg_ho.intra_lte.key_change_ind) {
        rrc_log->console("keyChangeIndicator in securityConfigHO not supported\n");
        return false;
      }
      if (mob_reconf.sec_cnfg_ho.intra_lte.sec_alg_cnfg_present) {
        cipher_algo = (CIPHERING_ALGORITHM_ID_ENUM) mob_reconf.sec_cnfg_ho.intra_lte.sec_alg_cnfg.cipher_alg;
        integ_algo  = (INTEGRITY_ALGORITHM_ID_ENUM) mob_reconf.sec_cnfg_ho.intra_lte.sec_alg_cnfg.int_alg;
        rrc_log->info("Changed Ciphering to %s and Integrity to %s\n",
                      ciphering_algorithm_id_text[cipher_algo],
                      integrity_algorithm_id_text[integ_algo]);
      }
    }

    usim->generate_as_keys_ho(mob_reconf.mob_ctrl_info.target_pci, phy->get_current_earfcn(),
                              ncc,
                              k_rrc_enc, k_rrc_int, k_up_enc, k_up_int, cipher_algo, integ_algo);

    pdcp->config_security_all(k_rrc_enc, k_rrc_int, cipher_algo, integ_algo);
    send_rrc_con_reconfig_complete();
  }
  return true;
}

void rrc::ho_ra_completed(bool ra_successful) {
  if (pending_mob_reconf) {

    if (ra_successful) {
      measurements.ho_finish();

      if (mob_reconf.meas_cnfg_present) {
        measurements.parse_meas_config(&mob_reconf.meas_cnfg);
      }

      mac_timers->timer_get(t304)->stop();

      apply_rr_config_common_ul(&mob_reconf.mob_ctrl_info.rr_cnfg_common);
      if (mob_reconf.rr_cnfg_ded_present) {
        apply_rr_config_dedicated(&mob_reconf.rr_cnfg_ded);
      }
    }
    // T304 will expiry and send ho_failure

    rrc_log->info("HO %ssuccessful\n", ra_successful?"":"un");
    rrc_log->console("HO %ssuccessful\n", ra_successful?"":"un");

    pending_mob_reconf = false;
  } else {
    rrc_log->error("Received HO random access completed but no pending mobility reconfiguration info\n");
  }
}

bool rrc::con_reconfig_ho(LIBLTE_RRC_CONNECTION_RECONFIGURATION_STRUCT *reconfig)
{
  if (reconfig->mob_ctrl_info.target_pci == phy->get_current_pci()) {
    rrc_log->console("Warning: Received HO command to own cell\n");
    rrc_log->warning("Received HO command to own cell\n");
    return false;
  }

  rrc_log->info("Received HO command to target PCell=%d\n", reconfig->mob_ctrl_info.target_pci);
  rrc_log->console("Received HO command to target PCell=%d, NCC=%d\n",
                   reconfig->mob_ctrl_info.target_pci, reconfig->sec_cnfg_ho.intra_lte.next_hop_chaining_count);

  // store mobilityControlInfo
  memcpy(&mob_reconf, reconfig, sizeof(LIBLTE_RRC_CONNECTION_RECONFIGURATION_STRUCT));
  pending_mob_reconf = true;

  ho_start = true;

  return true;
}

// Handle RRC Reconfiguration without MobilityInformation Section 5.3.5.3
bool rrc::con_reconfig(LIBLTE_RRC_CONNECTION_RECONFIGURATION_STRUCT *reconfig) {
  if (reconfig->rr_cnfg_ded_present) {
    if (!apply_rr_config_dedicated(&reconfig->rr_cnfg_ded)) {
      return false;
    }
  }
  if (reconfig->meas_cnfg_present) {
    if (!measurements.parse_meas_config(&reconfig->meas_cnfg)) {
      return false;
    }
  }

  send_rrc_con_reconfig_complete();

  byte_buffer_t *nas_sdu;
  for (uint32_t i = 0; i < reconfig->N_ded_info_nas; i++) {
    nas_sdu = pool_allocate;
    if (nas_sdu) {
      memcpy(nas_sdu->msg, &reconfig->ded_info_nas_list[i].msg, reconfig->ded_info_nas_list[i].N_bytes);
      nas_sdu->N_bytes = reconfig->ded_info_nas_list[i].N_bytes;
      nas->write_pdu(RB_ID_SRB1, nas_sdu);
    } else {
      rrc_log->error("Fatal Error: Couldn't allocate PDU in handle_rrc_con_reconfig().\n");
      return false;
    }
  }
  return true;
}

// HO failure from T304 expiry 5.3.5.6
void rrc::ho_failed() {
  send_con_restablish_request(LIBLTE_RRC_CON_REEST_REQ_CAUSE_HANDOVER_FAILURE);
}

// Reconfiguration failure or Section 5.3.5.5
void rrc::con_reconfig_failed()
{
  // Set previous PHY/MAC configuration
  phy->set_config(&previous_phy_cfg);
  mac->set_config(&previous_mac_cfg);

  if (security_is_activated) {
    // Start the Reestablishment Procedure
    send_con_restablish_request(LIBLTE_RRC_CON_REEST_REQ_CAUSE_RECONFIG_FAILURE);
  } else {
    go_idle = true;
  }
}

void rrc::handle_rrc_con_reconfig(uint32_t lcid, LIBLTE_RRC_CONNECTION_RECONFIGURATION_STRUCT *reconfig)
{
  phy->get_config(&previous_phy_cfg);
  mac->get_config(&previous_mac_cfg);

  if (reconfig->mob_ctrl_info_present) {
    if (!con_reconfig_ho(reconfig)) {
      con_reconfig_failed();
    }
  } else {
    if (!con_reconfig(reconfig)) {
      con_reconfig_failed();
    }
  }
}

/* Actions upon reception of RRCConnectionRelease 5.3.8.3 */
void rrc::rrc_connection_release() {
  // Save idleModeMobilityControlInfo, etc.
  rrc_log->console("Received RRC Connection Release\n");
  go_idle = true;
}

/* Actions upon leaving RRC_CONNECTED 5.3.12 */
void rrc::leave_connected()
{
  rrc_log->console("RRC IDLE\n");
  rrc_log->info("Leaving RRC_CONNECTED state\n");
  state = RRC_STATE_IDLE;
  drb_up = false;
  security_is_activated = false;
  measurements.reset();
  pdcp->reset();
  rlc->reset();
  phy->reset();
  mac->reset();
  set_phy_default();
  set_mac_default();
  mac_timers->timer_get(t301)->stop();
  mac_timers->timer_get(t310)->stop();
  mac_timers->timer_get(t311)->stop();
  mac_timers->timer_get(t304)->stop();
  rrc_log->info("Going RRC_IDLE\n");
  if (phy->cell_is_camping()) {
    // Receive paging
    mac->pcch_start_rx();
    // Instruct PHY to measure serving cell for cell reselection
    phy->meas_start(phy->get_current_earfcn(), phy->get_current_pci());
  }
}






/*******************************************************************************
*
*
*
* Reception of Broadcast messages (MIB and SIBs)
*
*
*
*******************************************************************************/
void rrc::write_pdu_bcch_bch(byte_buffer_t *pdu) {
  // Do we need to do something with BCH?
  rrc_log->info_hex(pdu->msg, pdu->N_bytes, "BCCH BCH message received.");
  pool->deallocate(pdu);
}

void rrc::write_pdu_bcch_dlsch(byte_buffer_t *pdu) {
  mac->clear_rntis();

  rrc_log->info_hex(pdu->msg, pdu->N_bytes, "BCCH DLSCH message received.");
  rrc_log->info("BCCH DLSCH message Stack latency: %ld us\n", pdu->get_latency_us());
  LIBLTE_RRC_BCCH_DLSCH_MSG_STRUCT dlsch_msg;
  ZERO_OBJECT(dlsch_msg);

  srslte_bit_unpack_vector(pdu->msg, bit_buf.msg, pdu->N_bytes * 8);
  bit_buf.N_bits = pdu->N_bytes * 8;
  pool->deallocate(pdu);
  liblte_rrc_unpack_bcch_dlsch_msg((LIBLTE_BIT_MSG_STRUCT *) &bit_buf, &dlsch_msg);

  for(uint32_t i=0; i<dlsch_msg.N_sibs; i++) {
    rrc_log->info("Processing SIB%d (%d/%d)\n", liblte_rrc_sys_info_block_type_num[dlsch_msg.sibs[i].sib_type], i, dlsch_msg.N_sibs);

    if (LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_1 == dlsch_msg.sibs[i].sib_type) {
      serving_cell->set_sib1(&dlsch_msg.sibs[i].sib.sib1);
      handle_sib1();
    } else if (LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_2 == dlsch_msg.sibs[i].sib_type && !serving_cell->has_sib2()) {
      serving_cell->set_sib2(&dlsch_msg.sibs[i].sib.sib2);
      handle_sib2();
    } else if (LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_3 == dlsch_msg.sibs[i].sib_type && !serving_cell->has_sib3()) {
      serving_cell->set_sib3(&dlsch_msg.sibs[i].sib.sib3);
      handle_sib3();
    }else if (LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_13 == dlsch_msg.sibs[i].sib_type && !serving_cell->has_sib13()) {
      serving_cell->set_sib13(&dlsch_msg.sibs[i].sib.sib13);
      handle_sib13();
    }
  }
}

void rrc::handle_sib1()
{
  LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_1_STRUCT *sib1 = serving_cell->sib1ptr();
  rrc_log->info("SIB1 received, CellID=%d, si_window=%d, sib2_period=%d\n",
                serving_cell->get_cell_id()&0xfff,
                liblte_rrc_si_window_length_num[sib1->si_window_length],
                liblte_rrc_si_periodicity_num[sib1->sched_info[0].si_periodicity]);

  // Print SIB scheduling info
  uint32_t i,j;
  for(i=0;i<sib1->N_sched_info;i++){
    for(j=0;j<sib1->sched_info[i].N_sib_mapping_info;j++){
      LIBLTE_RRC_SIB_TYPE_ENUM t       = sib1->sched_info[i].sib_mapping_info[j].sib_type;
      LIBLTE_RRC_SI_PERIODICITY_ENUM p = sib1->sched_info[i].si_periodicity;
      rrc_log->debug("SIB scheduling info, sib_type=%d, si_periodicity=%d\n",
                    liblte_rrc_sib_type_num[t],
                    liblte_rrc_si_periodicity_num[p]);
    }
  }

  // Set TDD Config
  if(sib1->tdd) {
    phy->set_config_tdd(&sib1->tdd_cnfg);
  }
}

void rrc::handle_sib2()
{
  rrc_log->info("SIB2 received\n");

  apply_sib2_configs(serving_cell->sib2ptr());

}

void rrc::handle_sib3()
{
  rrc_log->info("SIB3 received\n");

  LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_3_STRUCT *sib3 = serving_cell->sib3ptr();

  // cellReselectionInfoCommon
  cell_resel_cfg.q_hyst = liblte_rrc_q_hyst_num[sib3->q_hyst];

  // cellReselectionServingFreqInfo
  cell_resel_cfg.threshservinglow = sib3->thresh_serving_low;

  // intraFreqCellReselectionInfo
  cell_resel_cfg.Qrxlevmin       = sib3->q_rx_lev_min;
  if (sib3->s_intra_search_present) {
    cell_resel_cfg.s_intrasearchP  = sib3->s_intra_search;
  } else {
    cell_resel_cfg.s_intrasearchP  = INFINITY;
  }

}

void rrc::handle_sib13()
{
  rrc_log->info("SIB13 received\n");

//  mac->set_config_mbsfn_sib13(&serving_cell->sib13.mbsfn_area_info_list_r9[0],
//                              serving_cell->sib13.mbsfn_area_info_list_r9_size,
//                              &serving_cell->sib13.mbsfn_notification_config);
}




/*******************************************************************************
*
*
*
* Reception of Paging messages
*
*
*
*******************************************************************************/
void rrc::write_pdu_pcch(byte_buffer_t *pdu) {
  cmd_msg_t msg;
  msg.pdu = pdu;
  msg.command = cmd_msg_t::PCCH;
  cmd_q.push(msg);
}

void rrc::process_pcch(byte_buffer_t *pdu) {
  if (pdu->N_bytes > 0 && pdu->N_bytes < SRSLTE_MAX_BUFFER_SIZE_BITS) {
    rrc_log->info_hex(pdu->msg, pdu->N_bytes, "PCCH message received %d bytes\n", pdu->N_bytes);
    rrc_log->info("PCCH message Stack latency: %ld us\n", pdu->get_latency_us());

    LIBLTE_RRC_PCCH_MSG_STRUCT pcch_msg;
    ZERO_OBJECT(pcch_msg);
    srslte_bit_unpack_vector(pdu->msg, bit_buf.msg, pdu->N_bytes * 8);
    bit_buf.N_bits = pdu->N_bytes * 8;
    pool->deallocate(pdu);
    liblte_rrc_unpack_pcch_msg((LIBLTE_BIT_MSG_STRUCT *) &bit_buf, &pcch_msg);

    if (pcch_msg.paging_record_list_size > LIBLTE_RRC_MAX_PAGE_REC) {
      pcch_msg.paging_record_list_size = LIBLTE_RRC_MAX_PAGE_REC;
    }

    if (!ueIdentity_configured) {
      rrc_log->warning("Received paging message but no ue-Identity is configured\n");
      return;
    }
    LIBLTE_RRC_S_TMSI_STRUCT *s_tmsi_paged;
    for (uint32_t i = 0; i < pcch_msg.paging_record_list_size; i++) {
      s_tmsi_paged = &pcch_msg.paging_record_list[i].ue_identity.s_tmsi;
      rrc_log->info("Received paging (%d/%d) for UE %x:%x\n", i + 1, pcch_msg.paging_record_list_size,
                    pcch_msg.paging_record_list[i].ue_identity.s_tmsi.mmec,
                    pcch_msg.paging_record_list[i].ue_identity.s_tmsi.m_tmsi);
      if (ueIdentity.mmec == s_tmsi_paged->mmec && ueIdentity.m_tmsi == s_tmsi_paged->m_tmsi) {
        if (RRC_STATE_IDLE == state) {
          rrc_log->info("S-TMSI match in paging message\n");
          rrc_log->console("S-TMSI match in paging message\n");
          nas->paging(s_tmsi_paged);
        } else {
          rrc_log->warning("Received paging while in CONNECT\n");
        }
      } else {
        rrc_log->info("Received paging for unknown identity\n");
      }
    }
  }
}


void rrc::write_pdu_mch(uint32_t lcid, srslte::byte_buffer_t *pdu)
{
  if (pdu->N_bytes > 0 && pdu->N_bytes < SRSLTE_MAX_BUFFER_SIZE_BITS) {
    rrc_log->info_hex(pdu->msg, pdu->N_bytes, "MCH message received %d bytes on lcid:%d\n", pdu->N_bytes, lcid);
    rrc_log->info("MCH message Stack latency: %ld us\n", pdu->get_latency_us());
    //TODO: handle MCCH notifications and update MCCH
    if(0 == lcid && !serving_cell->has_mcch) {
      srslte_bit_unpack_vector(pdu->msg, bit_buf.msg, pdu->N_bytes * 8);
      bit_buf.N_bits = pdu->N_bytes * 8;
      liblte_rrc_unpack_mcch_msg((LIBLTE_BIT_MSG_STRUCT *) &bit_buf, &serving_cell->mcch);
      serving_cell->has_mcch = true;
      phy->set_config_mbsfn_mcch(&serving_cell->mcch);
    }

    pool->deallocate(pdu);
  }
}








/*******************************************************************************
*
*
*
* Packet processing
*
*
*******************************************************************************/
byte_buffer_t* rrc::byte_align_and_pack()
{
  // Byte align and pack the message bits for PDCP
  if ((bit_buf.N_bits % 8) != 0) {
    for (uint32_t i = 0; i < 8 - (bit_buf.N_bits % 8); i++)
      bit_buf.msg[bit_buf.N_bits + i] = 0;
    bit_buf.N_bits += 8 - (bit_buf.N_bits % 8);
  }

  // Reset and reuse sdu buffer if provided
  byte_buffer_t *pdcp_buf = pool_allocate;
  if (pdcp_buf) {
    srslte_bit_pack_vector(bit_buf.msg, pdcp_buf->msg, bit_buf.N_bits);
    pdcp_buf->N_bytes = bit_buf.N_bits / 8;
    pdcp_buf->set_timestamp();
  } else {
    rrc_log->error("Fatal Error: Couldn't allocate PDU in byte_align_and_pack().\n");
  }
  return pdcp_buf;
}

void rrc::send_ul_ccch_msg()
{
  liblte_rrc_pack_ul_ccch_msg(&ul_ccch_msg, (LIBLTE_BIT_MSG_STRUCT *) &bit_buf);
  byte_buffer_t *pdu = byte_align_and_pack();
  if (pdu) {
    // Set UE contention resolution ID in MAC
    uint64_t uecri = 0;
    uint8_t *ue_cri_ptr = (uint8_t *) &uecri;
    uint32_t nbytes = 6;
    for (uint32_t i = 0; i < nbytes; i++) {
      ue_cri_ptr[nbytes - i - 1] = pdu->msg[i];
    }

    rrc_log->debug("Setting UE contention resolution ID: %" PRIu64 "\n", uecri);
    mac->set_contention_id(uecri);

    rrc_log->info("Sending %s\n", liblte_rrc_ul_ccch_msg_type_text[ul_ccch_msg.msg_type]);
    pdcp->write_sdu(RB_ID_SRB0, pdu);
  }
}

void rrc::send_ul_dcch_msg()
{
  liblte_rrc_pack_ul_dcch_msg(&ul_dcch_msg, (LIBLTE_BIT_MSG_STRUCT *) &bit_buf);
  byte_buffer_t *pdu = byte_align_and_pack();
  if (pdu) {
    rrc_log->info("Sending %s\n", liblte_rrc_ul_dcch_msg_type_text[ul_dcch_msg.msg_type]);
    pdcp->write_sdu(RB_ID_SRB1, pdu);
  }
}

void rrc::write_sdu(uint32_t lcid, byte_buffer_t *sdu) {

  if (state == RRC_STATE_IDLE) {
    rrc_log->warning("Received ULInformationTransfer SDU when in IDLE\n");
    return;
  }
  rrc_log->info_hex(sdu->msg, sdu->N_bytes, "TX %s SDU", get_rb_name(lcid).c_str());
  send_ul_info_transfer(sdu);
}

void rrc::write_pdu(uint32_t lcid, byte_buffer_t *pdu) {
  rrc_log->info_hex(pdu->msg, pdu->N_bytes, "RX %s PDU", get_rb_name(lcid).c_str());

  switch (lcid) {
    case RB_ID_SRB0:
      parse_dl_ccch(pdu);
      break;
    case RB_ID_SRB1:
    case RB_ID_SRB2:
      parse_dl_dcch(lcid, pdu);
      break;
    default:
      rrc_log->error("RX PDU with invalid bearer id: %d", lcid);
      break;
  }
}

void rrc::parse_dl_ccch(byte_buffer_t *pdu) {
  srslte_bit_unpack_vector(pdu->msg, bit_buf.msg, pdu->N_bytes * 8);
  bit_buf.N_bits = pdu->N_bytes * 8;
  pool->deallocate(pdu);
  bzero(&dl_ccch_msg, sizeof(LIBLTE_RRC_DL_CCCH_MSG_STRUCT));
  liblte_rrc_unpack_dl_ccch_msg((LIBLTE_BIT_MSG_STRUCT *) &bit_buf, &dl_ccch_msg);

  rrc_log->info("SRB0 - Received %s\n",
                liblte_rrc_dl_ccch_msg_type_text[dl_ccch_msg.msg_type]);

  switch (dl_ccch_msg.msg_type) {
    case LIBLTE_RRC_DL_CCCH_MSG_TYPE_RRC_CON_REJ:
      // 5.3.3.8
      rrc_log->info("Received ConnectionReject. Wait time: %d\n",
                    dl_ccch_msg.msg.rrc_con_rej.wait_time);
      rrc_log->console("Received ConnectionReject. Wait time: %d\n",
                    dl_ccch_msg.msg.rrc_con_rej.wait_time);

      mac_timers->timer_get(t300)->stop();

      if (dl_ccch_msg.msg.rrc_con_rej.wait_time) {
        nas->set_barring(nas_interface_rrc::BARRING_ALL);
        mac_timers->timer_get(t302)->set(this, dl_ccch_msg.msg.rrc_con_rej.wait_time*1000);
        mac_timers->timer_get(t302)->run();
      } else {
        // Perform the actions upon expiry of T302 if wait time is zero
        nas->set_barring(nas_interface_rrc::BARRING_NONE);
        go_idle = true;
      }
      break;
    case LIBLTE_RRC_DL_CCCH_MSG_TYPE_RRC_CON_SETUP:
      rrc_log->info("ConnectionSetup received\n");
      transaction_id = dl_ccch_msg.msg.rrc_con_setup.rrc_transaction_id;
      handle_con_setup(&dl_ccch_msg.msg.rrc_con_setup);
      break;
    case LIBLTE_RRC_DL_CCCH_MSG_TYPE_RRC_CON_REEST:
      rrc_log->info("ConnectionReestablishment received\n");
      rrc_log->console("Reestablishment OK\n");
      transaction_id = dl_ccch_msg.msg.rrc_con_reest.rrc_transaction_id;
      handle_con_reest(&dl_ccch_msg.msg.rrc_con_reest);
      break;
      /* Reception of RRCConnectionReestablishmentReject 5.3.7.8 */
    case LIBLTE_RRC_DL_CCCH_MSG_TYPE_RRC_CON_REEST_REJ:
      rrc_log->info("ConnectionReestablishmentReject received\n");
      rrc_log->console("Reestablishment Reject\n");
      go_idle = true;
      break;
    default:
      break;
  }
}

void rrc::parse_dl_dcch(uint32_t lcid, byte_buffer_t *pdu) {
  srslte_bit_unpack_vector(pdu->msg, bit_buf.msg, pdu->N_bytes * 8);
  bit_buf.N_bits = pdu->N_bytes * 8;
  liblte_rrc_unpack_dl_dcch_msg((LIBLTE_BIT_MSG_STRUCT *) &bit_buf, &dl_dcch_msg);

  rrc_log->info("%s - Received %s\n",
                get_rb_name(lcid).c_str(),
                liblte_rrc_dl_dcch_msg_type_text[dl_dcch_msg.msg_type]);

  pool->deallocate(pdu);

  switch (dl_dcch_msg.msg_type) {
    case LIBLTE_RRC_DL_DCCH_MSG_TYPE_DL_INFO_TRANSFER:
      pdu = pool_allocate;
      if (!pdu) {
        rrc_log->error("Fatal error: out of buffers in pool\n");
        return;
      }
      memcpy(pdu->msg, dl_dcch_msg.msg.dl_info_transfer.dedicated_info.msg,
             dl_dcch_msg.msg.dl_info_transfer.dedicated_info.N_bytes);
      pdu->N_bytes = dl_dcch_msg.msg.dl_info_transfer.dedicated_info.N_bytes;
      nas->write_pdu(lcid, pdu);
      break;
    case LIBLTE_RRC_DL_DCCH_MSG_TYPE_SECURITY_MODE_COMMAND:
      transaction_id = dl_dcch_msg.msg.security_mode_cmd.rrc_transaction_id;

      cipher_algo = (CIPHERING_ALGORITHM_ID_ENUM) dl_dcch_msg.msg.security_mode_cmd.sec_algs.cipher_alg;
      integ_algo = (INTEGRITY_ALGORITHM_ID_ENUM) dl_dcch_msg.msg.security_mode_cmd.sec_algs.int_alg;

      rrc_log->info("Received Security Mode Command eea: %s, eia: %s\n",
                    ciphering_algorithm_id_text[cipher_algo],
                    integrity_algorithm_id_text[integ_algo]);

      // Generate AS security keys
      uint8_t k_asme[32];
      nas->get_k_asme(k_asme, 32);
      usim->generate_as_keys(k_asme, nas->get_ul_count(), k_rrc_enc, k_rrc_int, k_up_enc, k_up_int, cipher_algo, integ_algo);
      rrc_log->info_hex(k_rrc_enc, 32, "RRC encryption key - k_rrc_enc");
      rrc_log->info_hex(k_rrc_int, 32, "RRC integrity key  - k_rrc_int");
      rrc_log->info_hex(k_up_enc, 32,  "UP encryption key  - k_up_enc");

      security_is_activated = true;

      // Configure PDCP for security
      pdcp->config_security(lcid, k_rrc_enc, k_rrc_int, cipher_algo, integ_algo);
      pdcp->enable_integrity(lcid);
      send_security_mode_complete();
      pdcp->enable_encryption(lcid);
      break;
    case LIBLTE_RRC_DL_DCCH_MSG_TYPE_RRC_CON_RECONFIG:
      transaction_id = dl_dcch_msg.msg.rrc_con_reconfig.rrc_transaction_id;
      handle_rrc_con_reconfig(lcid, &dl_dcch_msg.msg.rrc_con_reconfig);
      break;
    case LIBLTE_RRC_DL_DCCH_MSG_TYPE_UE_CAPABILITY_ENQUIRY:
      transaction_id = dl_dcch_msg.msg.ue_cap_enquiry.rrc_transaction_id;
      for (uint32_t i = 0; i < dl_dcch_msg.msg.ue_cap_enquiry.N_ue_cap_reqs; i++) {
        if (LIBLTE_RRC_RAT_TYPE_EUTRA == dl_dcch_msg.msg.ue_cap_enquiry.ue_capability_request[i]) {
          send_rrc_ue_cap_info();
          break;
        }
      }
      break;
    case LIBLTE_RRC_DL_DCCH_MSG_TYPE_RRC_CON_RELEASE:
      rrc_connection_release();
      break;
    default:
      break;
  }
}









/*******************************************************************************
*
*
*
* Capabilities Message
*
*
*
*******************************************************************************/
void rrc::enable_capabilities() {
  bool enable_ul_64 = args.ue_category >= 5 && serving_cell->sib2ptr()->rr_config_common_sib.pusch_cnfg.enable_64_qam;
  rrc_log->info("%s 64QAM PUSCH\n", enable_ul_64 ? "Enabling" : "Disabling");
  phy->set_config_64qam_en(enable_ul_64);
}

void rrc::send_rrc_ue_cap_info() {
  rrc_log->debug("Preparing UE Capability Info\n");

  ul_dcch_msg.msg_type = LIBLTE_RRC_UL_DCCH_MSG_TYPE_UE_CAPABILITY_INFO;
  ul_dcch_msg.msg.ue_capability_info.rrc_transaction_id = transaction_id;

  LIBLTE_RRC_UE_CAPABILITY_INFORMATION_STRUCT *info = &ul_dcch_msg.msg.ue_capability_info;
  info->N_ue_caps = 1;
  info->ue_capability_rat[0].rat_type = LIBLTE_RRC_RAT_TYPE_EUTRA;

  LIBLTE_RRC_UE_EUTRA_CAPABILITY_STRUCT *cap = &info->ue_capability_rat[0].eutra_capability;
  cap->access_stratum_release = LIBLTE_RRC_ACCESS_STRATUM_RELEASE_REL8;
  cap->ue_category = args.ue_category;

  cap->pdcp_params.max_rohc_ctxts_present = false;
  cap->pdcp_params.supported_rohc_profiles[0] = false;
  cap->pdcp_params.supported_rohc_profiles[1] = false;
  cap->pdcp_params.supported_rohc_profiles[2] = false;
  cap->pdcp_params.supported_rohc_profiles[3] = false;
  cap->pdcp_params.supported_rohc_profiles[4] = false;
  cap->pdcp_params.supported_rohc_profiles[5] = false;
  cap->pdcp_params.supported_rohc_profiles[6] = false;
  cap->pdcp_params.supported_rohc_profiles[7] = false;
  cap->pdcp_params.supported_rohc_profiles[8] = false;

  cap->phy_params.specific_ref_sigs_supported = false;
  cap->phy_params.tx_antenna_selection_supported = false;

  cap->rf_params.N_supported_band_eutras = args.nof_supported_bands;
  cap->meas_params.N_band_list_eutra     = args.nof_supported_bands;
  for (uint32_t i=0;i<args.nof_supported_bands;i++) {
    cap->rf_params.supported_band_eutra[i].band_eutra = args.supported_bands[i];
    cap->rf_params.supported_band_eutra[i].half_duplex = false;
    cap->meas_params.band_list_eutra[i].N_inter_freq_need_for_gaps = 1;
    cap->meas_params.band_list_eutra[i].inter_freq_need_for_gaps[0] = true;
  }

  cap->feature_group_indicator_present = true;
  cap->feature_group_indicator = args.feature_group;
  cap->inter_rat_params.utra_fdd_present = false;
  cap->inter_rat_params.utra_tdd128_present = false;
  cap->inter_rat_params.utra_tdd384_present = false;
  cap->inter_rat_params.utra_tdd768_present = false;
  cap->inter_rat_params.geran_present = false;
  cap->inter_rat_params.cdma2000_hrpd_present = false;
  cap->inter_rat_params.cdma2000_1xrtt_present = false;

  send_ul_dcch_msg();
}










/*******************************************************************************
*
*
*
* PHY and MAC Radio Resource configuration
*
*
*
*******************************************************************************/

void rrc::apply_rr_config_common_dl(LIBLTE_RRC_RR_CONFIG_COMMON_STRUCT *config) {
  mac_interface_rrc::mac_cfg_t mac_cfg;
  mac->get_config(&mac_cfg);
  if (config->rach_cnfg_present) {
    memcpy(&mac_cfg.rach, &config->rach_cnfg, sizeof(LIBLTE_RRC_RACH_CONFIG_COMMON_STRUCT));
    mac_cfg.ul_harq_params.max_harq_msg3_tx = config->rach_cnfg.max_harq_msg3_tx;
  }
  mac_cfg.prach_config_index = config->prach_cnfg.root_sequence_index;

  mac->set_config(&mac_cfg);

  phy_interface_rrc::phy_cfg_t phy_cfg;
  phy->get_config(&phy_cfg);
  phy_interface_rrc::phy_cfg_common_t *common = &phy_cfg.common;

  if (config->pdsch_cnfg_present) {
    memcpy(&common->pdsch_cnfg, &config->pdsch_cnfg, sizeof(LIBLTE_RRC_PDSCH_CONFIG_COMMON_STRUCT));
  }
  common->prach_cnfg.root_sequence_index = config->prach_cnfg.root_sequence_index;
  if (config->prach_cnfg.prach_cnfg_info_present) {
    memcpy(&common->prach_cnfg.prach_cnfg_info, &config->prach_cnfg.prach_cnfg_info, sizeof(LIBLTE_RRC_PRACH_CONFIG_INFO_STRUCT));
  }

  phy->set_config_common(common);
}

void rrc::apply_rr_config_common_ul(LIBLTE_RRC_RR_CONFIG_COMMON_STRUCT *config) {
  phy_interface_rrc::phy_cfg_t phy_cfg;
  phy->get_config(&phy_cfg);
  phy_interface_rrc::phy_cfg_common_t *common = &phy_cfg.common;

  memcpy(&common->pusch_cnfg, &config->pusch_cnfg, sizeof(LIBLTE_RRC_PUSCH_CONFIG_COMMON_STRUCT));
  if (config->pucch_cnfg_present) {
    memcpy(&common->pucch_cnfg, &config->pucch_cnfg, sizeof(LIBLTE_RRC_PUCCH_CONFIG_COMMON_STRUCT));
  }
  if (config->ul_pwr_ctrl_present) {
    memcpy(&common->ul_pwr_ctrl, &config->ul_pwr_ctrl, sizeof(LIBLTE_RRC_UL_POWER_CONTROL_COMMON_STRUCT));
  }
  if (config->srs_ul_cnfg.present) {
    memcpy(&common->srs_ul_cnfg, &config->srs_ul_cnfg, sizeof(LIBLTE_RRC_SRS_UL_CONFIG_COMMON_STRUCT));
  } else {
    // default is release
    common->srs_ul_cnfg.present = false;
  }
  phy->set_config_common(common);
  phy->configure_ul_params();
}

void rrc::apply_sib2_configs(LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_2_STRUCT *sib2) {

  // Apply RACH timeAlginmentTimer configuration
  mac_interface_rrc::mac_cfg_t cfg;
  mac->get_config(&cfg);

  cfg.main.time_alignment_timer = sib2->time_alignment_timer;
  memcpy(&cfg.rach, &sib2->rr_config_common_sib.rach_cnfg, sizeof(LIBLTE_RRC_RACH_CONFIG_COMMON_STRUCT));
  cfg.prach_config_index = sib2->rr_config_common_sib.prach_cnfg.root_sequence_index;
  cfg.ul_harq_params.max_harq_msg3_tx = cfg.rach.max_harq_msg3_tx;
  // Apply MBSFN configuration
//  cfg.mbsfn_subfr_cnfg_list_size = sib2->mbsfn_subfr_cnfg_list_size;
//  for(uint8_t i=0;i<sib2->mbsfn_subfr_cnfg_list_size;i++) {
//    memcpy(&cfg.mbsfn_subfr_cnfg_list[i], &sib2->mbsfn_subfr_cnfg_list[i], sizeof(LIBLTE_RRC_MBSFN_SUBFRAME_CONFIG_STRUCT));
//  }
  
    // Set MBSFN configs
  phy->set_config_mbsfn_sib2(sib2);

  mac->set_config(&cfg);

  rrc_log->info("Set RACH ConfigCommon: NofPreambles=%d, ResponseWindow=%d, ContentionResolutionTimer=%d ms\n",
                liblte_rrc_number_of_ra_preambles_num[sib2->rr_config_common_sib.rach_cnfg.num_ra_preambles],
                liblte_rrc_ra_response_window_size_num[sib2->rr_config_common_sib.rach_cnfg.ra_resp_win_size],
                liblte_rrc_mac_contention_resolution_timer_num[sib2->rr_config_common_sib.rach_cnfg.mac_con_res_timer]);

  // Apply PHY RR Config Common
  phy_interface_rrc::phy_cfg_common_t common;
  memcpy(&common.pdsch_cnfg, &sib2->rr_config_common_sib.pdsch_cnfg, sizeof(LIBLTE_RRC_PDSCH_CONFIG_COMMON_STRUCT));
  memcpy(&common.pusch_cnfg, &sib2->rr_config_common_sib.pusch_cnfg, sizeof(LIBLTE_RRC_PUSCH_CONFIG_COMMON_STRUCT));
  memcpy(&common.pucch_cnfg, &sib2->rr_config_common_sib.pucch_cnfg, sizeof(LIBLTE_RRC_PUCCH_CONFIG_COMMON_STRUCT));
  memcpy(&common.ul_pwr_ctrl, &sib2->rr_config_common_sib.ul_pwr_ctrl,
         sizeof(LIBLTE_RRC_UL_POWER_CONTROL_COMMON_STRUCT));
  memcpy(&common.prach_cnfg, &sib2->rr_config_common_sib.prach_cnfg, sizeof(LIBLTE_RRC_PRACH_CONFIG_SIB_STRUCT));
  if (sib2->rr_config_common_sib.srs_ul_cnfg.present) {
    memcpy(&common.srs_ul_cnfg, &sib2->rr_config_common_sib.srs_ul_cnfg,
           sizeof(LIBLTE_RRC_SRS_UL_CONFIG_COMMON_STRUCT));
  } else {
    // default is release
    common.srs_ul_cnfg.present = false;
  }
  phy->set_config_common(&common);

  phy->configure_ul_params();

  rrc_log->info("Set PUSCH ConfigCommon: HopOffset=%d, RSGroup=%d, RSNcs=%d, N_sb=%d\n",
                sib2->rr_config_common_sib.pusch_cnfg.pusch_hopping_offset,
                sib2->rr_config_common_sib.pusch_cnfg.ul_rs.group_assignment_pusch,
                sib2->rr_config_common_sib.pusch_cnfg.ul_rs.cyclic_shift,
                sib2->rr_config_common_sib.pusch_cnfg.n_sb);

  rrc_log->info("Set PUCCH ConfigCommon: DeltaShift=%d, CyclicShift=%d, N1=%d, NRB=%d\n",
                liblte_rrc_delta_pucch_shift_num[sib2->rr_config_common_sib.pucch_cnfg.delta_pucch_shift],
                sib2->rr_config_common_sib.pucch_cnfg.n_cs_an,
                sib2->rr_config_common_sib.pucch_cnfg.n1_pucch_an,
                sib2->rr_config_common_sib.pucch_cnfg.n_rb_cqi);

  rrc_log->info("Set PRACH ConfigCommon: SeqIdx=%d, HS=%s, FreqOffset=%d, ZC=%d, ConfigIndex=%d\n",
                sib2->rr_config_common_sib.prach_cnfg.root_sequence_index,
                sib2->rr_config_common_sib.prach_cnfg.prach_cnfg_info.high_speed_flag ? "yes" : "no",
                sib2->rr_config_common_sib.prach_cnfg.prach_cnfg_info.prach_freq_offset,
                sib2->rr_config_common_sib.prach_cnfg.prach_cnfg_info.zero_correlation_zone_config,
                sib2->rr_config_common_sib.prach_cnfg.prach_cnfg_info.prach_config_index);

  rrc_log->info("Set SRS ConfigCommon: BW-Configuration=%d, SF-Configuration=%d, ACKNACK=%s\n",
                liblte_rrc_srs_bw_config_num[sib2->rr_config_common_sib.srs_ul_cnfg.bw_cnfg],
                liblte_rrc_srs_subfr_config_num[sib2->rr_config_common_sib.srs_ul_cnfg.subfr_cnfg],
                sib2->rr_config_common_sib.srs_ul_cnfg.ack_nack_simul_tx ? "yes" : "no");

  mac_timers->timer_get(t300)->set(this, liblte_rrc_t300_num[sib2->ue_timers_and_constants.t300]);
  mac_timers->timer_get(t301)->set(this, liblte_rrc_t301_num[sib2->ue_timers_and_constants.t301]);
  mac_timers->timer_get(t310)->set(this, liblte_rrc_t310_num[sib2->ue_timers_and_constants.t310]);
  mac_timers->timer_get(t311)->set(this, liblte_rrc_t311_num[sib2->ue_timers_and_constants.t311]);
  N310 = liblte_rrc_n310_num[sib2->ue_timers_and_constants.n310];
  N311 = liblte_rrc_n311_num[sib2->ue_timers_and_constants.n311];

  rrc_log->info("Set Constants and Timers: N310=%d, N311=%d, t300=%d, t301=%d, t310=%d, t311=%d\n",
                N310, N311, mac_timers->timer_get(t300)->get_timeout(), mac_timers->timer_get(t301)->get_timeout(),
                mac_timers->timer_get(t310)->get_timeout(), mac_timers->timer_get(t311)->get_timeout());

}

void rrc::apply_sib13_configs(LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_13_STRUCT *sib13)
{
  phy->set_config_mbsfn_sib13(&serving_cell->sib13);
  add_mrb(0, 0); // Add MRB0
}

// Go through all information elements and apply defaults (9.2.4) if not defined
void rrc::apply_phy_config_dedicated(LIBLTE_RRC_PHYSICAL_CONFIG_DEDICATED_STRUCT *phy_cnfg, bool apply_defaults) {
  // Get current configuration
  LIBLTE_RRC_PHYSICAL_CONFIG_DEDICATED_STRUCT *current_cfg;
  phy_interface_rrc::phy_cfg_t c;
  phy->get_config(&c);
  current_cfg = &c.dedicated;

  if (phy_cnfg->pucch_cnfg_ded_present) {
    memcpy(&current_cfg->pucch_cnfg_ded, &phy_cnfg->pucch_cnfg_ded, sizeof(LIBLTE_RRC_PUCCH_CONFIG_DEDICATED_STRUCT));
  } else if (apply_defaults) {
    current_cfg->pucch_cnfg_ded.tdd_ack_nack_feedback_mode = LIBLTE_RRC_TDD_ACK_NACK_FEEDBACK_MODE_BUNDLING;
    current_cfg->pucch_cnfg_ded.ack_nack_repetition_setup_present = false;
  }
  if (phy_cnfg->pusch_cnfg_ded_present) {
    memcpy(&current_cfg->pusch_cnfg_ded, &phy_cnfg->pusch_cnfg_ded, sizeof(LIBLTE_RRC_PUSCH_CONFIG_DEDICATED_STRUCT));
  } else if (apply_defaults) {
    current_cfg->pusch_cnfg_ded.beta_offset_ack_idx = 10;
    current_cfg->pusch_cnfg_ded.beta_offset_ri_idx = 12;
    current_cfg->pusch_cnfg_ded.beta_offset_cqi_idx = 15;
  }
  if (phy_cnfg->ul_pwr_ctrl_ded_present) {
    memcpy(&current_cfg->ul_pwr_ctrl_ded, &phy_cnfg->ul_pwr_ctrl_ded,
           sizeof(LIBLTE_RRC_UL_POWER_CONTROL_DEDICATED_STRUCT));
  } else if (apply_defaults) {
    current_cfg->ul_pwr_ctrl_ded.p0_ue_pusch     = 0;
    current_cfg->ul_pwr_ctrl_ded.delta_mcs_en    = LIBLTE_RRC_DELTA_MCS_ENABLED_EN0;
    current_cfg->ul_pwr_ctrl_ded.accumulation_en = true;
    current_cfg->ul_pwr_ctrl_ded.p0_ue_pucch     = 0;
    current_cfg->ul_pwr_ctrl_ded.p_srs_offset    = 7;
  }
  if (phy_cnfg->ul_pwr_ctrl_ded.filter_coeff_present) {
    current_cfg->ul_pwr_ctrl_ded.filter_coeff = phy_cnfg->ul_pwr_ctrl_ded.filter_coeff;
  } else {
    current_cfg->ul_pwr_ctrl_ded.filter_coeff = LIBLTE_RRC_FILTER_COEFFICIENT_FC4;
  }
  if (phy_cnfg->tpc_pdcch_cnfg_pucch_present) {
    memcpy(&current_cfg->tpc_pdcch_cnfg_pucch, &phy_cnfg->tpc_pdcch_cnfg_pucch,
           sizeof(LIBLTE_RRC_TPC_PDCCH_CONFIG_STRUCT));
  } else if (apply_defaults) {
    current_cfg->tpc_pdcch_cnfg_pucch.setup_present = false;
  }
  if (phy_cnfg->tpc_pdcch_cnfg_pusch_present) {
    memcpy(&current_cfg->tpc_pdcch_cnfg_pusch, &phy_cnfg->tpc_pdcch_cnfg_pusch,
           sizeof(LIBLTE_RRC_TPC_PDCCH_CONFIG_STRUCT));
  } else {
    current_cfg->tpc_pdcch_cnfg_pusch.setup_present = false;
  }
  if (phy_cnfg->cqi_report_cnfg_present) {
    if (phy_cnfg->cqi_report_cnfg.report_periodic_present) {
      memcpy(&current_cfg->cqi_report_cnfg.report_periodic, &phy_cnfg->cqi_report_cnfg.report_periodic,
             sizeof(LIBLTE_RRC_CQI_REPORT_PERIODIC_STRUCT));
      current_cfg->cqi_report_cnfg.report_periodic_setup_present = phy_cnfg->cqi_report_cnfg.report_periodic_setup_present;
    } else if (apply_defaults) {
      current_cfg->cqi_report_cnfg.report_periodic_setup_present = false;
    }
    if (phy_cnfg->cqi_report_cnfg.report_mode_aperiodic_present) {
      current_cfg->cqi_report_cnfg.report_mode_aperiodic = phy_cnfg->cqi_report_cnfg.report_mode_aperiodic;
      current_cfg->cqi_report_cnfg.report_mode_aperiodic_present = phy_cnfg->cqi_report_cnfg.report_mode_aperiodic_present;
    } else if (apply_defaults) {
      current_cfg->cqi_report_cnfg.report_mode_aperiodic_present = false;
    }
    current_cfg->cqi_report_cnfg.nom_pdsch_rs_epre_offset = phy_cnfg->cqi_report_cnfg.nom_pdsch_rs_epre_offset;
  }
  if (phy_cnfg->srs_ul_cnfg_ded_present && phy_cnfg->srs_ul_cnfg_ded.setup_present) {
    memcpy(&current_cfg->srs_ul_cnfg_ded, &phy_cnfg->srs_ul_cnfg_ded,
           sizeof(LIBLTE_RRC_SRS_UL_CONFIG_DEDICATED_STRUCT));
  } else if (apply_defaults) {
    current_cfg->srs_ul_cnfg_ded.setup_present = false;
  }
  if (phy_cnfg->antenna_info_present) {
    if (!phy_cnfg->antenna_info_default_value) {
      if (phy_cnfg->antenna_info_explicit_value.tx_mode != LIBLTE_RRC_TRANSMISSION_MODE_1 &&
          phy_cnfg->antenna_info_explicit_value.tx_mode != LIBLTE_RRC_TRANSMISSION_MODE_2 &&
	  phy_cnfg->antenna_info_explicit_value.tx_mode != LIBLTE_RRC_TRANSMISSION_MODE_3 &&
	  phy_cnfg->antenna_info_explicit_value.tx_mode != LIBLTE_RRC_TRANSMISSION_MODE_4) {
        rrc_log->error("Transmission mode TM%s not currently supported by srsUE\n",
                       liblte_rrc_transmission_mode_text[phy_cnfg->antenna_info_explicit_value.tx_mode]);
      }
      memcpy(&current_cfg->antenna_info_explicit_value, &phy_cnfg->antenna_info_explicit_value,
             sizeof(LIBLTE_RRC_ANTENNA_INFO_DEDICATED_STRUCT));
    } else if (apply_defaults) {
      current_cfg->antenna_info_explicit_value.tx_mode = LIBLTE_RRC_TRANSMISSION_MODE_2;
      current_cfg->antenna_info_explicit_value.codebook_subset_restriction_present = false;
      current_cfg->antenna_info_explicit_value.ue_tx_antenna_selection_setup_present = false;
    }
  } else if (apply_defaults) {
    current_cfg->antenna_info_explicit_value.tx_mode = LIBLTE_RRC_TRANSMISSION_MODE_2;
    current_cfg->antenna_info_explicit_value.codebook_subset_restriction_present = false;
    current_cfg->antenna_info_explicit_value.ue_tx_antenna_selection_setup_present = false;
  }
  if (phy_cnfg->sched_request_cnfg_present) {
    memcpy(&current_cfg->sched_request_cnfg, &phy_cnfg->sched_request_cnfg,
           sizeof(LIBLTE_RRC_SCHEDULING_REQUEST_CONFIG_STRUCT));
  } else if (apply_defaults) {
    current_cfg->sched_request_cnfg.setup_present = false;
  }
  if (phy_cnfg->pdsch_cnfg_ded_present) {
    current_cfg->pdsch_cnfg_ded = phy_cnfg->pdsch_cnfg_ded;
    rrc_log->info("Set PDSCH-Config=%s (present)\n", liblte_rrc_pdsch_config_p_a_text[(int) current_cfg->pdsch_cnfg_ded]);
  } else if (apply_defaults) {
    current_cfg->pdsch_cnfg_ded = LIBLTE_RRC_PDSCH_CONFIG_P_A_DB_0;
    rrc_log->info("Set PDSCH-Config=%s (default)\n", liblte_rrc_pdsch_config_p_a_text[(int) current_cfg->pdsch_cnfg_ded]);
  }

  if (phy_cnfg->cqi_report_cnfg_present) {
    if (phy_cnfg->cqi_report_cnfg.report_periodic_present) {
      rrc_log->info("Set cqi-PUCCH-ResourceIndex=%d, cqi-pmi-ConfigIndex=%d, cqi-FormatIndicatorPeriodic=%s\n",
                    current_cfg->cqi_report_cnfg.report_periodic.pucch_resource_idx,
                    current_cfg->cqi_report_cnfg.report_periodic.pmi_cnfg_idx,
                    liblte_rrc_cqi_format_indicator_periodic_text[current_cfg->cqi_report_cnfg.report_periodic.format_ind_periodic]);
    }
    if (phy_cnfg->cqi_report_cnfg.report_mode_aperiodic_present) {
      rrc_log->info("Set cqi-ReportModeAperiodic=%s\n",
                    liblte_rrc_cqi_report_mode_aperiodic_text[current_cfg->cqi_report_cnfg.report_mode_aperiodic]);
    }

  }

  if (phy_cnfg->sched_request_cnfg_present) {
    rrc_log->info("Set PHY config ded: SR-n_pucch=%d, SR-ConfigIndex=%d, SR-TransMax=%d\n",
                  current_cfg->sched_request_cnfg.sr_pucch_resource_idx,
                  current_cfg->sched_request_cnfg.sr_cnfg_idx,
                  liblte_rrc_dsr_trans_max_num[current_cfg->sched_request_cnfg.dsr_trans_max]);
  }

  if (current_cfg->srs_ul_cnfg_ded_present) {
    rrc_log->info("Set PHY config ded: SRS-ConfigIndex=%d, SRS-bw=%s, SRS-Nrcc=%d, SRS-hop=%s, SRS-Ncs=%s\n",
                  current_cfg->srs_ul_cnfg_ded.srs_cnfg_idx,
                  liblte_rrc_srs_bandwidth_text[current_cfg->srs_ul_cnfg_ded.srs_bandwidth],
                  current_cfg->srs_ul_cnfg_ded.freq_domain_pos,
                  liblte_rrc_srs_hopping_bandwidth_text[current_cfg->srs_ul_cnfg_ded.srs_hopping_bandwidth],
                  liblte_rrc_cyclic_shift_text[current_cfg->srs_ul_cnfg_ded.cyclic_shift]);
  }

  phy->set_config_dedicated(current_cfg);

  // Apply changes to PHY
  phy->configure_ul_params();
}

void rrc::apply_mac_config_dedicated(LIBLTE_RRC_MAC_MAIN_CONFIG_STRUCT *mac_cnfg, bool apply_defaults) {
  // Set Default MAC main configuration (9.2.2)
  LIBLTE_RRC_MAC_MAIN_CONFIG_STRUCT default_cfg;
  bzero(&default_cfg, sizeof(LIBLTE_RRC_MAC_MAIN_CONFIG_STRUCT));
  default_cfg.ulsch_cnfg.max_harq_tx = LIBLTE_RRC_MAX_HARQ_TX_N5;
  default_cfg.ulsch_cnfg.periodic_bsr_timer = LIBLTE_RRC_PERIODIC_BSR_TIMER_INFINITY;
  default_cfg.ulsch_cnfg.retx_bsr_timer = LIBLTE_RRC_RETRANSMISSION_BSR_TIMER_SF2560;
  default_cfg.ulsch_cnfg.tti_bundling = false;
  default_cfg.drx_cnfg.setup_present = false;
  default_cfg.phr_cnfg.setup_present = false;
  default_cfg.time_alignment_timer = LIBLTE_RRC_TIME_ALIGNMENT_TIMER_INFINITY;


  if (!apply_defaults) {
    if (mac_cnfg->ulsch_cnfg_present) {
      if (mac_cnfg->ulsch_cnfg.max_harq_tx_present) {
        default_cfg.ulsch_cnfg.max_harq_tx = mac_cnfg->ulsch_cnfg.max_harq_tx;
        default_cfg.ulsch_cnfg.max_harq_tx_present = true;
      }
      if (mac_cnfg->ulsch_cnfg.periodic_bsr_timer_present) {
        default_cfg.ulsch_cnfg.periodic_bsr_timer = mac_cnfg->ulsch_cnfg.periodic_bsr_timer;
        default_cfg.ulsch_cnfg.periodic_bsr_timer_present = true;
      }
      default_cfg.ulsch_cnfg.retx_bsr_timer = mac_cnfg->ulsch_cnfg.retx_bsr_timer;
      default_cfg.ulsch_cnfg.tti_bundling = mac_cnfg->ulsch_cnfg.tti_bundling;
    }
    if (mac_cnfg->drx_cnfg_present) {
      memcpy(&default_cfg.drx_cnfg, &mac_cnfg->drx_cnfg, sizeof(LIBLTE_RRC_DRX_CONFIG_STRUCT));
      default_cfg.drx_cnfg_present = true;
    }
    if (mac_cnfg->phr_cnfg_present) {
      memcpy(&default_cfg.phr_cnfg, &mac_cnfg->phr_cnfg, sizeof(LIBLTE_RRC_PHR_CONFIG_STRUCT));
      default_cfg.phr_cnfg_present = true;
    }
    default_cfg.time_alignment_timer = mac_cnfg->time_alignment_timer;
  }

  // Setup MAC configuration
  mac->set_config_main(&default_cfg);

  // Update UL HARQ config
  mac_interface_rrc::mac_cfg_t cfg;
  mac->get_config(&cfg);
  cfg.ul_harq_params.max_harq_tx = liblte_rrc_max_harq_tx_num[default_cfg.ulsch_cnfg.max_harq_tx];
  mac->set_config(&cfg);

  rrc_log->info("Set MAC main config: harq-MaxReTX=%d, bsr-TimerReTX=%d, bsr-TimerPeriodic=%d\n",
                liblte_rrc_max_harq_tx_num[default_cfg.ulsch_cnfg.max_harq_tx],
                liblte_rrc_retransmission_bsr_timer_num[default_cfg.ulsch_cnfg.retx_bsr_timer],
                liblte_rrc_periodic_bsr_timer_num[default_cfg.ulsch_cnfg.periodic_bsr_timer]);
  if (default_cfg.phr_cnfg_present) {
    rrc_log->info("Set MAC PHR config: periodicPHR-Timer=%d, prohibitPHR-Timer=%d, dl-PathlossChange=%d\n",
                  liblte_rrc_periodic_phr_timer_num[default_cfg.phr_cnfg.periodic_phr_timer],
                  liblte_rrc_prohibit_phr_timer_num[default_cfg.phr_cnfg.prohibit_phr_timer],
                  liblte_rrc_dl_pathloss_change_num[default_cfg.phr_cnfg.dl_pathloss_change]);
  }
}

bool rrc::apply_rr_config_dedicated(LIBLTE_RRC_RR_CONFIG_DEDICATED_STRUCT *cnfg) {
  if (cnfg->phy_cnfg_ded_present) {
    apply_phy_config_dedicated(&cnfg->phy_cnfg_ded, false);
    // Apply SR configuration to MAC
    if (cnfg->phy_cnfg_ded.sched_request_cnfg_present) {
      mac->set_config_sr(&cnfg->phy_cnfg_ded.sched_request_cnfg);
    }
  }

  if (cnfg->mac_main_cnfg_present) {
    apply_mac_config_dedicated(&cnfg->mac_main_cnfg.explicit_value, cnfg->mac_main_cnfg.default_value);
  }

  if (cnfg->sps_cnfg_present) {
    //TODO
  }
  if (cnfg->rlf_timers_and_constants_present) {
    mac_timers->timer_get(t301)->set(this, liblte_rrc_t301_num[cnfg->rlf_timers_and_constants.t301]);
    mac_timers->timer_get(t310)->set(this, liblte_rrc_t310_num[cnfg->rlf_timers_and_constants.t310]);
    mac_timers->timer_get(t311)->set(this, liblte_rrc_t311_num[cnfg->rlf_timers_and_constants.t311]);
    N310 = liblte_rrc_n310_num[cnfg->rlf_timers_and_constants.n310];
    N311 = liblte_rrc_n311_num[cnfg->rlf_timers_and_constants.n311];

    rrc_log->info("Updated Constants and Timers: N310=%d, N311=%d, t300=%u, t301=%u, t310=%u, t311=%u\n",
                  N310, N311, mac_timers->timer_get(t300)->get_timeout(), mac_timers->timer_get(t301)->get_timeout(),
                  mac_timers->timer_get(t310)->get_timeout(), mac_timers->timer_get(t311)->get_timeout());
  }
  for (uint32_t i = 0; i < cnfg->srb_to_add_mod_list_size; i++) {
    // TODO: handle SRB modification
    add_srb(&cnfg->srb_to_add_mod_list[i]);
  }
  for (uint32_t i = 0; i < cnfg->drb_to_release_list_size; i++) {
    release_drb(cnfg->drb_to_release_list[i]);
  }
  for (uint32_t i = 0; i < cnfg->drb_to_add_mod_list_size; i++) {
    // TODO: handle DRB modification
    add_drb(&cnfg->drb_to_add_mod_list[i]);
  }
  return true;
}

void rrc::handle_con_setup(LIBLTE_RRC_CONNECTION_SETUP_STRUCT *setup) {
  // Apply the Radio Resource configuration
  apply_rr_config_dedicated(&setup->rr_cnfg);

  // Must enter CONNECT before stopping T300
  state = RRC_STATE_CONNECTED;

  rrc_log->console("RRC Connected\n");
  mac_timers->timer_get(t300)->stop();
  mac_timers->timer_get(t302)->stop();
  nas->set_barring(nas_interface_rrc::BARRING_NONE);

  if (dedicatedInfoNAS) {
    send_con_setup_complete(dedicatedInfoNAS);
    dedicatedInfoNAS = NULL; // deallocated Inside!
  } else {
    rrc_log->error("Pending to transmit a ConnectionSetupComplete but no dedicatedInfoNAS was in queue\n");
  }
}

/* Reception of RRCConnectionReestablishment by the UE 5.3.7.5 */
void rrc::handle_con_reest(LIBLTE_RRC_CONNECTION_REESTABLISHMENT_STRUCT *setup) {

  mac_timers->timer_get(t301)->stop();

  pdcp->reestablish();
  rlc->reestablish();

  // Apply the Radio Resource configuration
  apply_rr_config_dedicated(&setup->rr_cnfg);

  // Send ConnectionSetupComplete message
  send_con_restablish_complete();
}


void rrc::add_srb(LIBLTE_RRC_SRB_TO_ADD_MOD_STRUCT *srb_cnfg) {
  // Setup PDCP
  pdcp->add_bearer(srb_cnfg->srb_id, srslte_pdcp_config_t(true)); // Set PDCP config control flag
  if(RB_ID_SRB2 == srb_cnfg->srb_id) {
    pdcp->config_security(srb_cnfg->srb_id, k_rrc_enc, k_rrc_int, cipher_algo, integ_algo);
    pdcp->enable_integrity(srb_cnfg->srb_id);
    pdcp->enable_encryption(srb_cnfg->srb_id);
  }

  // Setup RLC
  if (srb_cnfg->rlc_cnfg_present) {
    if (srb_cnfg->rlc_default_cnfg_present) {
      rlc->add_bearer(srb_cnfg->srb_id);
    }else{
      rlc->add_bearer(srb_cnfg->srb_id, srslte_rlc_config_t(&srb_cnfg->rlc_explicit_cnfg));
    }
  }

  // Setup MAC
  uint8_t log_chan_group = 0;
  uint8_t priority = 1;
  int prioritized_bit_rate = -1;
  int bucket_size_duration = -1;

  if (srb_cnfg->lc_cnfg_present) {
    if (srb_cnfg->lc_default_cnfg_present) {
      if (RB_ID_SRB2 == srb_cnfg->srb_id)
        priority = 3;
    } else {
      if (srb_cnfg->lc_explicit_cnfg.log_chan_sr_mask_present) {
        //TODO
      }
      if (srb_cnfg->lc_explicit_cnfg.ul_specific_params_present) {
        if (srb_cnfg->lc_explicit_cnfg.ul_specific_params.log_chan_group_present)
          log_chan_group = srb_cnfg->lc_explicit_cnfg.ul_specific_params.log_chan_group;

        priority = srb_cnfg->lc_explicit_cnfg.ul_specific_params.priority;
        prioritized_bit_rate = liblte_rrc_prioritized_bit_rate_num[srb_cnfg->lc_explicit_cnfg.ul_specific_params.prioritized_bit_rate];
        bucket_size_duration = liblte_rrc_bucket_size_duration_num[srb_cnfg->lc_explicit_cnfg.ul_specific_params.bucket_size_duration];
      }
    }
    mac->setup_lcid(srb_cnfg->srb_id, log_chan_group, priority, prioritized_bit_rate, bucket_size_duration);
  }

  srbs[srb_cnfg->srb_id] = *srb_cnfg;
  rrc_log->info("Added radio bearer %s\n", get_rb_name(srb_cnfg->srb_id).c_str());
}

void rrc::add_drb(LIBLTE_RRC_DRB_TO_ADD_MOD_STRUCT *drb_cnfg) {

  if (!drb_cnfg->pdcp_cnfg_present ||
      !drb_cnfg->rlc_cnfg_present ||
      !drb_cnfg->lc_cnfg_present) {
    rrc_log->error("Cannot add DRB - incomplete configuration\n");
    return;
  }
  uint32_t lcid = 0;
  if (drb_cnfg->lc_id_present) {
    lcid = drb_cnfg->lc_id;
  } else {
    lcid = RB_ID_SRB2 + drb_cnfg->drb_id;
    rrc_log->warning("LCID not present, using %d\n", lcid);
  }

  // Setup PDCP
  srslte_pdcp_config_t pdcp_cfg;
  pdcp_cfg.is_data = true;
  if (drb_cnfg->pdcp_cnfg.rlc_um_pdcp_sn_size_present) {
    if (LIBLTE_RRC_PDCP_SN_SIZE_7_BITS == drb_cnfg->pdcp_cnfg.rlc_um_pdcp_sn_size) {
      pdcp_cfg.sn_len = 7;
    }
  }
  pdcp->add_bearer(lcid, pdcp_cfg);
  pdcp->config_security(lcid, k_up_enc, k_up_int, cipher_algo, integ_algo);
  pdcp->enable_encryption(lcid);

  // Setup RLC
  rlc->add_bearer(lcid, srslte_rlc_config_t(&drb_cnfg->rlc_cnfg));

  // Setup MAC
  uint8_t log_chan_group = 0;
  uint8_t priority = 1;
  int prioritized_bit_rate = -1;
  int bucket_size_duration = -1;
  if (drb_cnfg->lc_cnfg.ul_specific_params_present) {
    if (drb_cnfg->lc_cnfg.ul_specific_params.log_chan_group_present) {
      log_chan_group = drb_cnfg->lc_cnfg.ul_specific_params.log_chan_group;
    } else {
      rrc_log->warning("LCG not present, setting to 0\n");
    }
    priority = drb_cnfg->lc_cnfg.ul_specific_params.priority;
    prioritized_bit_rate = liblte_rrc_prioritized_bit_rate_num[drb_cnfg->lc_cnfg.ul_specific_params.prioritized_bit_rate];

    if (prioritized_bit_rate > 0) {
      rrc_log->warning("PBR>0 currently not supported. Setting it to Inifinty\n");
      prioritized_bit_rate = -1;
    }

    bucket_size_duration = liblte_rrc_bucket_size_duration_num[drb_cnfg->lc_cnfg.ul_specific_params.bucket_size_duration];
  }
  mac->setup_lcid(lcid, log_chan_group, priority, prioritized_bit_rate, bucket_size_duration);

  drbs[lcid] = *drb_cnfg;
  drb_up     = true;
  rrc_log->info("Added radio bearer %s\n", get_rb_name(lcid).c_str());
}

void rrc::release_drb(uint8_t lcid) {
  // TODO
}

void rrc::add_mrb(uint32_t lcid, uint32_t port)
{
  gw->add_mch_port(lcid, port);
  rlc->add_bearer_mrb(lcid);
  mac->mch_start_rx(lcid);
  rrc_log->info("Added MRB bearer for lcid:%d\n", lcid);
}

// PHY CONFIG DEDICATED Defaults (3GPP 36.331 v10 9.2.4)
void rrc::set_phy_default_pucch_srs() {

  phy_interface_rrc::phy_cfg_t current_cfg;
  phy->get_config(&current_cfg);

  // Set defaults to CQI, SRS and SR
  current_cfg.dedicated.cqi_report_cnfg_present = false;
  current_cfg.dedicated.srs_ul_cnfg_ded_present = false;
  current_cfg.dedicated.sched_request_cnfg_present = false;

  apply_phy_config_dedicated(&current_cfg.dedicated, true);

  // Release SR configuration from MAC
  LIBLTE_RRC_SCHEDULING_REQUEST_CONFIG_STRUCT cfg;
  bzero(&cfg, sizeof(LIBLTE_RRC_SCHEDULING_REQUEST_CONFIG_STRUCT));
  mac->set_config_sr(&cfg);
}

void rrc::set_phy_default() {
  LIBLTE_RRC_PHYSICAL_CONFIG_DEDICATED_STRUCT defaults;
  bzero(&defaults, sizeof(LIBLTE_RRC_PHYSICAL_CONFIG_DEDICATED_STRUCT));
  apply_phy_config_dedicated(&defaults, true);
}

void rrc::set_mac_default() {
  apply_mac_config_dedicated(NULL, true);
  LIBLTE_RRC_SCHEDULING_REQUEST_CONFIG_STRUCT sr_cfg;
  bzero(&sr_cfg, sizeof(LIBLTE_RRC_SCHEDULING_REQUEST_CONFIG_STRUCT));
  sr_cfg.setup_present = false;
  mac->set_config_sr(&sr_cfg);
}

void rrc::set_rrc_default() {
  N310 = 1;
  N311 = 1;
  mac_timers->timer_get(t310)->set(this, 1000);
  mac_timers->timer_get(t311)->set(this, 1000);
}



















/************************************************************************
 *
 *
 * RRC Measurements
 *
 *
 ************************************************************************/

void rrc::rrc_meas::init(rrc *parent) {
  this->parent      = parent;
  this->log_h       = parent->rrc_log;
  this->phy         = parent->phy;
  this->mac_timers  = parent->mac_timers;
  s_measure_enabled = false;
  reset();
}

void rrc::rrc_meas::reset()
{
  filter_k_rsrp = liblte_rrc_filter_coefficient_num[LIBLTE_RRC_FILTER_COEFFICIENT_FC4];
  filter_k_rsrq = liblte_rrc_filter_coefficient_num[LIBLTE_RRC_FILTER_COEFFICIENT_FC4];

  // FIXME: Turn struct into a class and use destructor
  std::map<uint32_t, meas_t>::iterator iter = active.begin();
  while (iter != active.end()) {
    remove_meas_id(iter++);
  }

  // These objects do not need destructor
  objects.clear();
  reports_cfg.clear();
  phy->meas_reset();
  bzero(&pcell_measurement, sizeof(meas_value_t));
}

/* L3 filtering 5.5.3.2 */
void rrc::rrc_meas::L3_filter(meas_value_t *value, float values[NOF_MEASUREMENTS])
{
  for (int i=0;i<NOF_MEASUREMENTS;i++) {
    if (value->ms[i]) {
      value->ms[i] = SRSLTE_VEC_EMA(values[i], value->ms[i], filter_a[i]);
    } else {
      value->ms[i] = values[i];
    }
  }
}

void rrc::rrc_meas::new_phy_meas(uint32_t earfcn, uint32_t pci, float rsrp, float rsrq, uint32_t tti)
{
  float values[NOF_MEASUREMENTS] = {rsrp, rsrq};

  // This indicates serving cell
  if (parent->serving_cell->equals(earfcn, pci)) {

    log_h->debug("MEAS:  New measurement serving cell, rsrp=%f, rsrq=%f, tti=%d\n", rsrp, rsrq, tti);

    L3_filter(&pcell_measurement, values);

    // Update serving cell measurement
    parent->serving_cell->set_rsrp(rsrp);

  } else {

    // Add to list of neighbour cells
    bool added = parent->add_neighbour_cell(earfcn, pci, rsrp);

    log_h->debug("MEAS:  New measurement %s earfcn=%d, pci=%d, rsrp=%f, rsrq=%f, tti=%d\n",
                added?"added":"not added", earfcn, pci, rsrp, rsrq, tti);

    // Only report measurements of 8th strongest cells
    if (added) {
      // Save PHY measurement for all active measurements whose earfcn/pci matches
      for(std::map<uint32_t, meas_t>::iterator iter=active.begin(); iter!=active.end(); ++iter) {
        meas_t *m = &iter->second;
        if (objects[m->object_id].earfcn == earfcn) {
          // If it's a newly discovered cell, add it to objects
          if (!m->cell_values.count(pci)) {
            uint32_t cell_idx = objects[m->object_id].cells.size();
            objects[m->object_id].cells[cell_idx].pci      = pci;
            objects[m->object_id].cells[cell_idx].q_offset = 0;
          }
          // Update or add cell
          L3_filter(&m->cell_values[pci], values);
          return;
        }
      }
    }
  }
}

// Remove all stored measurements for a given cell
void rrc::rrc_meas::delete_report(uint32_t earfcn, uint32_t pci) {
  for(std::map<uint32_t, meas_t>::iterator iter=active.begin(); iter!=active.end(); ++iter) {
    meas_t *m = &iter->second;
    if (objects[m->object_id].earfcn == earfcn) {
      if (m->cell_values.count(pci)) {
        m->cell_values.erase(pci);
        log_h->info("Deleting report PCI=%d from cell_values\n", pci);
      }
    }
  }
}

void rrc::rrc_meas::run_tti(uint32_t tti) {
  // Measurement Report Triggering Section 5.5.4
  calculate_triggers(tti);
}

bool rrc::rrc_meas::find_earfcn_cell(uint32_t earfcn, uint32_t pci, meas_obj_t **object, int *cell_idx) {
  if (object) {
    *object = NULL;
  }
  for (std::map<uint32_t, meas_obj_t>::iterator obj = objects.begin(); obj != objects.end(); ++obj) {
    if (obj->second.earfcn == earfcn) {
      if (object) {
        *object = &obj->second;
      }
      for (std::map<uint32_t, meas_cell_t>::iterator c = obj->second.cells.begin(); c != obj->second.cells.end(); ++c) {
        if (c->second.pci == pci) {
          if (cell_idx) {
            *cell_idx = c->first;
            return true;
          }
        }
      }
      // return true if cell idx not found but frequency is found
      if (cell_idx) {
        *cell_idx = -1;
      }
      return true;
    }
  }
  return false;
}

/* Generate report procedure 5.5.5 */
void rrc::rrc_meas::generate_report(uint32_t meas_id)
{
  parent->ul_dcch_msg.msg_type = LIBLTE_RRC_UL_DCCH_MSG_TYPE_MEASUREMENT_REPORT;
  LIBLTE_RRC_MEASUREMENT_REPORT_STRUCT *report = &parent->ul_dcch_msg.msg.measurement_report;

  bzero(report, sizeof(LIBLTE_RRC_MEASUREMENT_REPORT_STRUCT));

  meas_t       *m   = &active[meas_id];
  report_cfg_t *cfg = &reports_cfg[m->report_id];

  report->meas_id = meas_id;
  report->pcell_rsrp_result = value_to_range(RSRP, pcell_measurement.ms[RSRP]);
  report->pcell_rsrq_result = value_to_range(RSRQ, pcell_measurement.ms[RSRQ]);

  log_h->info("MEAS:  Generate report MeasId=%d, nof_reports_send=%d, Pcell rsrp=%f rsrq=%f\n",
              report->meas_id, m->nof_reports_sent, pcell_measurement.ms[RSRP], pcell_measurement.ms[RSRQ]);

  // TODO: report up to 8 best cells
  for (std::map<uint32_t, meas_value_t>::iterator cell = m->cell_values.begin(); cell != m->cell_values.end(); ++cell)
  {
    if (cell->second.triggered && report->meas_result_neigh_cells.eutra.n_result < 8)
    {
      LIBLTE_RRC_MEAS_RESULT_EUTRA_STRUCT *rc = &report->meas_result_neigh_cells.eutra.result_eutra_list[report->meas_result_neigh_cells.eutra.n_result];

      rc->phys_cell_id = cell->first;
      rc->meas_result.have_rsrp   = cfg->report_quantity==RSRP || cfg->report_quantity==BOTH;
      rc->meas_result.have_rsrq   = cfg->report_quantity==RSRQ || cfg->report_quantity==BOTH;
      rc->meas_result.rsrp_result = value_to_range(RSRP, cell->second.ms[RSRP]);
      rc->meas_result.rsrq_result = value_to_range(RSRQ, cell->second.ms[RSRQ]);

      log_h->info("MEAS:  Adding to report neighbour=%d, pci=%d, rsrp=%f, rsrq=%f\n",
                     report->meas_result_neigh_cells.eutra.n_result, rc->phys_cell_id,
                     cell->second.ms[RSRP], cell->second.ms[RSRQ]);

      report->meas_result_neigh_cells.eutra.n_result++;
    }
  }
  report->have_meas_result_neigh_cells = report->meas_result_neigh_cells.eutra.n_result > 0;

  m->nof_reports_sent++;
  mac_timers->timer_get(m->periodic_timer)->stop();

  if (m->nof_reports_sent < cfg->amount) {
    mac_timers->timer_get(m->periodic_timer)->reset();
    mac_timers->timer_get(m->periodic_timer)->run();
  } else {
    if (cfg->trigger_type == report_cfg_t::PERIODIC) {
      m->triggered = false;
    }
  }

  // Send to lower layers
  parent->send_ul_dcch_msg();
}

/* Handle entering/leaving event conditions 5.5.4.1 */
bool rrc::rrc_meas::process_event(LIBLTE_RRC_EVENT_EUTRA_STRUCT *event, uint32_t tti,
                                  bool enter_condition, bool exit_condition,
                                  meas_t *m, meas_value_t *cell)
{
  bool generate_report = false;
  if (enter_condition && (!m->triggered || !cell->triggered)) {
    if (!cell->timer_enter_triggered) {
      cell->timer_enter_triggered = true;
      cell->enter_tti     = tti;
    } else if (srslte_tti_interval(tti, cell->enter_tti) >= event->time_to_trigger) {
      m->triggered        = true;
      cell->triggered     = true;
      m->nof_reports_sent = 0;
      generate_report     = true;
    }
  } else if (exit_condition) {
    if (!cell->timer_exit_triggered) {
      cell->timer_exit_triggered = true;
      cell->exit_tti      = tti;
    } else if (srslte_tti_interval(tti, cell->exit_tti) >= event->time_to_trigger) {
      m->triggered        = false;
      cell->triggered     = false;
      mac_timers->timer_get(m->periodic_timer)->stop();
      if (event) {
        if (event->event_id == LIBLTE_RRC_EVENT_ID_EUTRA_A3 && event->event_a3.report_on_leave) {
          generate_report = true;
        }
      }
    }
  }
  if (!enter_condition) {
    cell->timer_enter_triggered = false;
  }
  if (!enter_condition) {
    cell->timer_exit_triggered = false;
  }
  return generate_report;
}

/* Calculate trigger conditions for each cell 5.5.4 */
void rrc::rrc_meas::calculate_triggers(uint32_t tti)
{
  float Ofp = 0, Ocp = 0;
  meas_obj_t *serving_object   = NULL;
  int         serving_cell_idx = 0;

  // Get serving cell
  if (active.size()) {
    if (find_earfcn_cell(phy->get_current_earfcn(), phy->get_current_pci(), &serving_object, &serving_cell_idx)) {
      Ofp = serving_object->q_offset;
      if (serving_cell_idx >= 0) {
        Ocp = serving_object->cells[serving_cell_idx].q_offset;
      }
    } else {
      log_h->warning("Can't find current eafcn=%d, pci=%d in objects list. Using Ofp=0, Ocp=0\n",
                     phy->get_current_earfcn(), phy->get_current_pci());
    }
  }



  for (std::map<uint32_t, meas_t>::iterator m = active.begin(); m != active.end(); ++m) {
    report_cfg_t *cfg = &reports_cfg[m->second.report_id];
    float hyst = 0.5*cfg->event.hysteresis;
    float Mp   = pcell_measurement.ms[cfg->trigger_quantity];

    LIBLTE_RRC_EVENT_ID_EUTRA_ENUM event_id = cfg->event.event_id;
    const char *event_str = liblte_rrc_event_id_eutra_text[event_id];

    bool gen_report = false;

    if (cfg->trigger_type == report_cfg_t::EVENT) {

      // A1 & A2 are for serving cell only
      if (event_id < LIBLTE_RRC_EVENT_ID_EUTRA_A3) {

        bool enter_condition;
        bool exit_condition;
        if (event_id == LIBLTE_RRC_EVENT_ID_EUTRA_A1) {
          enter_condition = Mp - hyst > range_to_value(cfg->trigger_quantity, cfg->event.event_a1.eutra.range);
          exit_condition  = Mp + hyst < range_to_value(cfg->trigger_quantity, cfg->event.event_a1.eutra.range);
        } else {
          enter_condition = Mp + hyst < range_to_value(cfg->trigger_quantity, cfg->event.event_a1.eutra.range);
          exit_condition  = Mp - hyst > range_to_value(cfg->trigger_quantity, cfg->event.event_a1.eutra.range);
        }

        // check only if
        gen_report |= process_event(&cfg->event, tti, enter_condition, exit_condition,
                                        &m->second, &pcell_measurement);

        if (gen_report) {
          log_h->info("Triggered by A1/A2 event\n");
        }
      // Rest are evaluated for every cell in frequency
      } else {
        meas_obj_t *obj = &objects[m->second.object_id];
        for (std::map<uint32_t, meas_cell_t>::iterator cell = obj->cells.begin(); cell != obj->cells.end(); ++cell) {
          if (m->second.cell_values.count(cell->second.pci)) {
            float Ofn = obj->q_offset;
            float Ocn = cell->second.q_offset;
            float Mn = m->second.cell_values[cell->second.pci].ms[cfg->trigger_quantity];
            float Off=0, th=0, th1=0, th2=0;
            bool enter_condition = false;
            bool exit_condition  = false;
            switch (event_id) {
              case LIBLTE_RRC_EVENT_ID_EUTRA_A3:
                Off = 0.5*cfg->event.event_a3.offset;
                enter_condition = Mn + Ofn + Ocn - hyst > Mp + Ofp + Ocp + Off;
                exit_condition  = Mn + Ofn + Ocn + hyst < Mp + Ofp + Ocp + Off;
                break;
              case LIBLTE_RRC_EVENT_ID_EUTRA_A4:
                th = range_to_value(cfg->trigger_quantity, cfg->event.event_a4.eutra.range);
                enter_condition = Mn + Ofn + Ocn - hyst > th;
                exit_condition  = Mn + Ofn + Ocn + hyst < th;
                break;
              case LIBLTE_RRC_EVENT_ID_EUTRA_A5:
                th1 = range_to_value(cfg->trigger_quantity, cfg->event.event_a5.eutra1.range);
                th2 = range_to_value(cfg->trigger_quantity, cfg->event.event_a5.eutra2.range);
                enter_condition = (Mp + hyst < th1) && (Mn + Ofn + Ocn - hyst > th2);
                exit_condition  = (Mp - hyst > th1) && (Mn + Ofn + Ocn + hyst < th2);
                break;
              default:
                log_h->error("Error event %s not implemented\n", event_str);
            }
            gen_report |= process_event(&cfg->event, tti, enter_condition, exit_condition,
                                        &m->second, &m->second.cell_values[cell->second.pci]);
          }
        }
      }
    }
    if (gen_report) {
      log_h->info("Generate report MeasId=%d, from event\n", m->first);
      generate_report(m->first);
    }
  }
}

// Procedure upon handover or reestablishment 5.5.6.1
void rrc::rrc_meas::ho_finish() {
  // Remove all measId with trigger periodic
  std::map<uint32_t, meas_t>::iterator iter = active.begin();
  while (iter != active.end()) {
    if (reports_cfg[iter->second.report_id].trigger_type == report_cfg_t::PERIODIC) {
      remove_meas_id(iter++);
    } else {
      ++iter;
    }
  }

  //TODO: Inter-frequency handover

  // Stop all reports
  for (std::map<uint32_t, meas_t>::iterator iter = active.begin(); iter != active.end(); ++iter) {
    stop_reports(&iter->second);
  }
}

// 5.5.4.1 expiry of periodical reporting timer
bool rrc::rrc_meas::timer_expired(uint32_t timer_id) {
  for (std::map<uint32_t, meas_t>::iterator iter = active.begin(); iter != active.end(); ++iter) {
    if (iter->second.periodic_timer == timer_id) {
      log_h->info("Generate report MeasId=%d, from timerId=%d\n", iter->first, timer_id);
      generate_report(iter->first);
      return true;
    }
  }
  return false;
}

void rrc::rrc_meas::stop_reports(meas_t *m) {
  mac_timers->timer_get(m->periodic_timer)->stop();
  m->triggered = false;
}

void rrc::rrc_meas::stop_reports_object(uint32_t object_id) {
  for (std::map<uint32_t, meas_t>::iterator iter = active.begin(); iter != active.end(); ++iter) {
    if (iter->second.object_id == object_id) {
      stop_reports(&iter->second);
    }
  }
}

void rrc::rrc_meas::remove_meas_object(uint32_t object_id) {
  std::map<uint32_t, meas_t>::iterator iter = active.begin();
  while (iter != active.end()) {
    if (iter->second.object_id == object_id) {
      remove_meas_id(iter++);
    } else {
      ++iter;
    }
  }
}

void rrc::rrc_meas::remove_meas_report(uint32_t report_id) {
  std::map<uint32_t, meas_t>::iterator iter = active.begin();
  while (iter != active.end()) {
    if (iter->second.report_id == report_id) {
      remove_meas_id(iter++);
    } else {
      ++iter;
    }
  }
}

void rrc::rrc_meas::remove_meas_id(uint32_t measId) {
  if (active.count(measId)) {
    mac_timers->timer_get(active[measId].periodic_timer)->stop();
    mac_timers->timer_release_id(active[measId].periodic_timer);
    log_h->info("MEAS: Removed measId=%d\n", measId);
    active.erase(measId);
  } else {
    log_h->warning("MEAS: Removing unexistent measId=%d\n", measId);
  }
}

void rrc::rrc_meas::remove_meas_id(std::map<uint32_t, meas_t>::iterator it) {
  mac_timers->timer_get(it->second.periodic_timer)->stop();
  mac_timers->timer_release_id(it->second.periodic_timer);
  log_h->info("MEAS: Removed measId=%d\n", it->first);
  active.erase(it);
}

/* Parses MeasConfig object from RRCConnectionReconfiguration message and applies configuration
 * as per section 5.5.2
 */
bool rrc::rrc_meas::parse_meas_config(LIBLTE_RRC_MEAS_CONFIG_STRUCT *cfg)
{

  // Measurement object removal 5.5.2.4
  for (uint32_t i=0;i<cfg->N_meas_obj_to_remove;i++) {
    objects.erase(cfg->meas_obj_to_remove_list[i]);
    remove_meas_object(cfg->meas_obj_to_remove_list[i]);
    log_h->info("MEAS: Removed measObjectId=%d\n", cfg->meas_obj_to_remove_list[i]);
  }

  // Measurement object addition/modification Section 5.5.2.5
  if (cfg->meas_obj_to_add_mod_list_present) {
    for (uint32_t i=0;i<cfg->meas_obj_to_add_mod_list.N_meas_obj;i++) {
      if (cfg->meas_obj_to_add_mod_list.meas_obj_list[i].meas_obj_type == LIBLTE_RRC_MEAS_OBJECT_TYPE_EUTRA) {
        LIBLTE_RRC_MEAS_OBJECT_EUTRA_STRUCT *src_obj = &cfg->meas_obj_to_add_mod_list.meas_obj_list[i].meas_obj_eutra;

        // Access the object if exists or create it
        meas_obj_t *dst_obj = &objects[cfg->meas_obj_to_add_mod_list.meas_obj_list[i].meas_obj_id];

        dst_obj->earfcn   = src_obj->carrier_freq;;
        if (src_obj->offset_freq_not_default) {
          dst_obj->q_offset = liblte_rrc_q_offset_range_num[src_obj->offset_freq];
        } else {
          dst_obj->q_offset = 0;
        }

        if (src_obj->black_cells_to_remove_list_present) {
          for (uint32_t j=0;j<src_obj->black_cells_to_remove_list.N_cell_idx;j++) {
            dst_obj->cells.erase(src_obj->black_cells_to_remove_list.cell_idx[j]);
          }
        }

        for (uint32_t j=0;j<src_obj->N_cells_to_add_mod;j++) {
          dst_obj->cells[src_obj->cells_to_add_mod_list[j].cell_idx].q_offset = liblte_rrc_q_offset_range_num[src_obj->cells_to_add_mod_list[j].cell_offset];
          dst_obj->cells[src_obj->cells_to_add_mod_list[j].cell_idx].pci      = src_obj->cells_to_add_mod_list[j].pci;

          log_h->info("MEAS: Added measObjectId=%d, earfcn=%d, q_offset=%f, pci=%d, offset_cell=%f\n",
                      cfg->meas_obj_to_add_mod_list.meas_obj_list[i].meas_obj_id, dst_obj->earfcn, dst_obj->q_offset,
                      dst_obj->cells[src_obj->cells_to_add_mod_list[j].cell_idx].pci,
                      dst_obj->cells[src_obj->cells_to_add_mod_list[j].cell_idx].q_offset);

        }

        // Untrigger reports and stop timers
        stop_reports_object(cfg->meas_obj_to_add_mod_list.meas_obj_list[i].meas_obj_id);

        // TODO: Blackcells
        // TODO: meassubframepattern

      } else {
        log_h->warning("MEAS: Unsupported MeasObject type %s\n",
                       liblte_rrc_meas_object_type_text[cfg->meas_obj_to_add_mod_list.meas_obj_list[i].meas_obj_type]);
      }
    }
  }

  // Reporting configuration removal 5.5.2.6
  for (uint32_t i=0;i<cfg->N_rep_cnfg_to_remove;i++) {
    reports_cfg.erase(cfg->rep_cnfg_to_remove_list[i]);
    remove_meas_report(cfg->rep_cnfg_to_remove_list[i]);
    log_h->info("MEAS: Removed reportConfigId=%d\n", cfg->rep_cnfg_to_remove_list[i]);
  }

  // Reporting configuration addition/modification 5.5.2.7
  if (cfg->rep_cnfg_to_add_mod_list_present) {
    for (uint32_t i=0;i<cfg->rep_cnfg_to_add_mod_list.N_rep_cnfg;i++) {
      if (cfg->rep_cnfg_to_add_mod_list.rep_cnfg_list[i].rep_cnfg_type == LIBLTE_RRC_REPORT_CONFIG_TYPE_EUTRA) {
        LIBLTE_RRC_REPORT_CONFIG_EUTRA_STRUCT *src_rep = &cfg->rep_cnfg_to_add_mod_list.rep_cnfg_list[i].rep_cnfg_eutra;
        // Access the object if exists or create it
        report_cfg_t *dst_rep = &reports_cfg[cfg->rep_cnfg_to_add_mod_list.rep_cnfg_list[i].rep_cnfg_id];

        dst_rep->trigger_type = src_rep->trigger_type==LIBLTE_RRC_TRIGGER_TYPE_EUTRA_EVENT?report_cfg_t::EVENT:report_cfg_t::PERIODIC;
        dst_rep->event    = src_rep->event;
        dst_rep->amount   = liblte_rrc_report_amount_num[src_rep->report_amount];
        dst_rep->interval = liblte_rrc_report_interval_num[src_rep->report_interval];
        dst_rep->max_cell = src_rep->max_report_cells;
        dst_rep->trigger_quantity = (quantity_t) src_rep->trigger_quantity;
        dst_rep->report_quantity  = src_rep->report_quantity==LIBLTE_RRC_REPORT_QUANTITY_SAME_AS_TRIGGER_QUANTITY?dst_rep->trigger_quantity:BOTH;

        log_h->info("MEAS: Added reportConfigId=%d, event=%s, amount=%d, interval=%d\n",
                    cfg->rep_cnfg_to_add_mod_list.rep_cnfg_list[i].rep_cnfg_id,
                    liblte_rrc_event_id_eutra_text[dst_rep->event.event_id],
                    dst_rep->amount, dst_rep->interval);

        // Reset reports counter
        for(std::map<uint32_t, meas_t>::iterator iter=active.begin(); iter!=active.end(); ++iter) {
          if (iter->second.report_id == cfg->rep_cnfg_to_add_mod_list.rep_cnfg_list[i].rep_cnfg_id) {
            iter->second.nof_reports_sent = 0;
            stop_reports(&iter->second);
          }
        }
      } else {
        log_h->warning("MEAS: Unsupported reportConfigType %s\n", liblte_rrc_report_config_type_text[cfg->rep_cnfg_to_add_mod_list.rep_cnfg_list[i].rep_cnfg_type]);
      }
    }
  }

  // Quantity configuration 5.5.2.8
  if (cfg->quantity_cnfg_present && cfg->quantity_cnfg.qc_eutra_present) {
    if (cfg->quantity_cnfg.qc_eutra.fc_rsrp_not_default) {
      filter_k_rsrp = liblte_rrc_filter_coefficient_num[cfg->quantity_cnfg.qc_eutra.fc_rsrp];
    } else {
      filter_k_rsrp = liblte_rrc_filter_coefficient_num[LIBLTE_RRC_FILTER_COEFFICIENT_FC4];
    }
    if (cfg->quantity_cnfg.qc_eutra.fc_rsrq_not_default) {
      filter_k_rsrq = liblte_rrc_filter_coefficient_num[cfg->quantity_cnfg.qc_eutra.fc_rsrq];
    } else {
      filter_k_rsrq = liblte_rrc_filter_coefficient_num[LIBLTE_RRC_FILTER_COEFFICIENT_FC4];
    }
    filter_a[RSRP] = pow(0.5, (float) filter_k_rsrp/4);
    filter_a[RSRQ] = pow(0.5, (float) filter_k_rsrq/4);

    log_h->info("MEAS: Quantity configuration k_rsrp=%d, k_rsrq=%d\n", filter_k_rsrp, filter_k_rsrq);
  }

  // Measurement identity removal 5.5.2.2
  for (uint32_t i=0;i<cfg->N_meas_id_to_remove;i++) {
    remove_meas_id(cfg->meas_id_to_remove_list[i]);
  }

  log_h->info("nof active measId=%zd\n", active.size());

  // Measurement identity addition/modification 5.5.2.3
  if (cfg->meas_id_to_add_mod_list_present) {
    for (uint32_t i=0;i<cfg->meas_id_to_add_mod_list.N_meas_id;i++) {
      LIBLTE_RRC_MEAS_ID_TO_ADD_MOD_STRUCT *measId = &cfg->meas_id_to_add_mod_list.meas_id_list[i];
      // Stop the timer if the entry exists or create the timer if not
      bool is_new = false;
      if (active.count(measId->meas_id)) {
        mac_timers->timer_get(active[measId->meas_id].periodic_timer)->stop();
      } else {
        is_new = true;
        active[measId->meas_id].periodic_timer   = mac_timers->timer_get_unique_id();
      }
      active[measId->meas_id].object_id = measId->meas_obj_id;
      active[measId->meas_id].report_id = measId->rep_cnfg_id;
      log_h->info("MEAS: %s measId=%d, measObjectId=%d, reportConfigId=%d, timer_id=%u, nof_values=%lu\n",
                  is_new?"Added":"Updated", measId->meas_id, measId->meas_obj_id, measId->rep_cnfg_id,
                  active[measId->meas_id].periodic_timer,
                  active[measId->meas_id].cell_values.size());
    }
  }

  // S-Measure
  if (cfg->s_meas_present) {
    if (cfg->s_meas) {
      s_measure_enabled = true;
      s_measure_value   = range_to_value(RSRP, cfg->s_meas);
    } else {
      s_measure_enabled = false;
    }
  }

  update_phy();

  return true;
}

/* Instruct PHY to start measurement */
void rrc::rrc_meas::update_phy()
{
  phy->meas_reset();
  for(std::map<uint32_t, meas_t>::iterator iter=active.begin(); iter!=active.end(); ++iter) {
    meas_t m = iter->second;
    meas_obj_t o = objects[m.object_id];
    // Instruct PHY to look for neighbour cells on this frequency
    phy->meas_start(o.earfcn);
    for(std::map<uint32_t, meas_cell_t>::iterator iter=o.cells.begin(); iter!=o.cells.end(); ++iter) {
      // Instruct PHY to look for cells IDs on this frequency
      phy->meas_start(o.earfcn, iter->second.pci);
    }
  }
}


uint8_t rrc::rrc_meas::value_to_range(quantity_t quant, float value) {
  uint8_t range = 0;
  switch(quant) {
    case RSRP:
      if (value < -140) {
        range = 0;
      } else if (-140 <= value && value < -44) {
        range = 1 + (uint8_t) (value + 140);
      } else {
        range = 97;
      }
      break;
    case RSRQ:
      if (value < -19.5) {
        range = 0;
      } else if (-19.5 <= value && value < -3) {
        range = 1 + (uint8_t) (2*(value + 19.5));
      } else {
        range = 34;
      }
      break;
    case BOTH:
      printf("Error quantity both not supported in value_to_range\n");
      break;
  }
  return range;
}

float rrc::rrc_meas::range_to_value(quantity_t quant, uint8_t range) {
  float val = 0;
  switch(quant) {
    case RSRP:
      val = -140+(float) range;
      break;
    case RSRQ:
      val = -19.5+(float) range/2;
      break;
    case BOTH:
      printf("Error quantity both not supported in range_to_value\n");
      break;
  }
  return val;
}

const std::string rrc::rb_id_str[] = {"SRB0", "SRB1", "SRB2",
                                      "DRB1", "DRB2", "DRB3",
                                      "DRB4", "DRB5", "DRB6",
                                      "DRB7", "DRB8"};

} // namespace srsue
