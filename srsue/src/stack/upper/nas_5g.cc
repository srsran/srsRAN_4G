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

#include "srsue/hdr/stack/upper/nas_5g.h"
#include "srsran/asn1/nas_5g_ies.h"
#include "srsran/asn1/nas_5g_msg.h"
#include "srsran/common/bcd_helpers.h"
#include "srsran/common/security.h"
#include "srsran/common/standard_streams.h"
#include "srsran/common/string_helpers.h"
#include "srsran/interfaces/ue_gw_interfaces.h"
#include "srsran/interfaces/ue_rrc_interfaces.h"
#include "srsran/interfaces/ue_usim_interfaces.h"
#include "srsue/hdr/stack/upper/nas_5g_procedures.h"

#include <fstream>
#include <iomanip>
#include <iostream>
#include <unistd.h>

using namespace srsran;
using namespace srsran::nas_5g;

namespace srsue {

/*********************************************************************
 *   NAS 5G (NR)
 ********************************************************************/

nas_5g::nas_5g(srslog::basic_logger& logger_, srsran::task_sched_handle task_sched_) :
  nas_base(logger_),
  task_sched(task_sched_),
  t3502(task_sched_.get_unique_timer()),
  t3510(task_sched_.get_unique_timer()),
  t3511(task_sched_.get_unique_timer()),
  t3521(task_sched_.get_unique_timer()),
  reregistration_timer(task_sched_.get_unique_timer()),
  registration_proc(this)
{
  // Configure timers
  t3502.set(t3502_duration_ms, [this](uint32_t tid) { timer_expired(tid); });
  t3510.set(t3510_duration_ms, [this](uint32_t tid) { timer_expired(tid); });
  t3511.set(t3511_duration_ms, [this](uint32_t tid) { timer_expired(tid); });
  t3521.set(t3521_duration_ms, [this](uint32_t tid) { timer_expired(tid); });
  reregistration_timer.set(reregistration_timer_duration_ms, [this](uint32_t tid) { timer_expired(tid); });
}

nas_5g::~nas_5g() {}

void nas_5g::stop()
{
  running = false;
}

int nas_5g::init(usim_interface_nas*      usim_,
                 rrc_nr_interface_nas_5g* rrc_nr_,
                 gw_interface_nas*        gw_,
                 const nas_args_t&        cfg_)
{
  usim   = usim_;
  rrc_nr = rrc_nr_;
  gw     = gw_;
  cfg    = cfg_;

  // parse and sanity check EIA list
  if (parse_security_algorithm_list(cfg_.ia5g, ia5g_caps) != SRSRAN_SUCCESS) {
    logger.warning("Failed to parse integrity algorithm list: Defaulting to 5G-EI1-128, 5G-EI2-128, 5G-EI3-128");
    ia5g_caps[0] = false;
    ia5g_caps[1] = true;
    ia5g_caps[2] = true;
    ia5g_caps[3] = true;
  }

  // parse and sanity check EEA list
  if (parse_security_algorithm_list(cfg_.ea5g, ea5g_caps) != SRSRAN_SUCCESS) {
    logger.warning(
        "Failed to parse encryption algorithm list: Defaulting to 5G-EA0, 5G-EA1-128, 5G-EA2-128, 5G-EA3-128");
    ea5g_caps[0] = true;
    ea5g_caps[1] = true;
    ea5g_caps[2] = true;
    ea5g_caps[3] = true;
  }

  running = true;
  return SRSRAN_SUCCESS;
}

void nas_5g::run_tti()
{
  // Process PLMN selection ongoing procedures
  callbacks.run();

  // Transmit intiating messages if necessary
  switch (state.get_state()) {
    case mm5g_state_t::state_t::deregistered:
      // TODO Make sure cell selection is finished after transitioning from another state (if required)
      // Make sure the RRC is finished transitioning to RRC Idle
      if (reregistration_timer.is_running()) {
        logger.debug("Waiting for re-attach timer to expire to attach again.");
        return;
      }
      switch (state.get_deregistered_substate()) {
        case mm5g_state_t::deregistered_substate_t::plmn_search:
        case mm5g_state_t::deregistered_substate_t::normal_service:
        case mm5g_state_t::deregistered_substate_t::initial_registration_needed:
          registration_proc.launch();
          break;
        case mm5g_state_t::deregistered_substate_t::attempting_to_registration:
        case mm5g_state_t::deregistered_substate_t::no_supi:
        case mm5g_state_t::deregistered_substate_t::no_cell_available:
        case mm5g_state_t::deregistered_substate_t::e_call_inactive:
          logger.debug("Attempting to registration (not implemented) %s", state.get_full_state_text().c_str());
        default:
          break;
      }
    case mm5g_state_t::state_t::registered:
      break;
    case mm5g_state_t::state_t::deregistered_initiated:
      logger.debug("UE detaching...");
      break;
    default:
      break;
  }
}

/*******************************************************************************
 * Senders
 ******************************************************************************/

int nas_5g::send_registration_request()
{
  unique_byte_buffer_t pdu = srsran::make_byte_buffer();
  if (!pdu) {
    logger.error("Couldn't allocate PDU in %s().", __FUNCTION__);
    return SRSRAN_ERROR;
  }

  logger.info("Generating registration request");

  nas_5gs_msg             nas_msg;
  nas_msg.hdr.extended_protocol_discriminator =
      nas_5gs_hdr::extended_protocol_discriminator_opts::extended_protocol_discriminator_5gmm;
  registration_request_t& reg_req = nas_msg.set_registration_request();

  reg_req.registration_type_5gs.follow_on_request_bit =
      registration_type_5gs_t::follow_on_request_bit_type_::options::no_follow_on_request_pending;
  reg_req.registration_type_5gs.registration_type =
      registration_type_5gs_t::registration_type_type_::options::initial_registration;
  mobile_identity_5gs_t::suci_s& suci = reg_req.mobile_identity_5gs.set_suci();
  suci.supi_format                    = mobile_identity_5gs_t::suci_s::supi_format_type_::options::imsi;
  usim->get_home_mcc_bytes(suci.mcc.data(), suci.mcc.size());
  usim->get_home_mcc_bytes(suci.mnc.data(), suci.mnc.size());

  suci.scheme_output.resize(5);
  usim->get_home_msin_bcd(suci.scheme_output.data(), 5);
  logger.info("Requesting IMSI attach (IMSI=%s)", usim->get_imsi_str().c_str());

  if (nas_msg.pack(pdu) != SRSASN_SUCCESS) {
    logger.error("Failed to pack registration request");
    return SRSRAN_ERROR;
  }

  if (pcap != nullptr) {
    pcap->write_nas(pdu.get()->msg, pdu.get()->N_bytes);
  }

  // start T3510
  logger.debug("Starting T3410. Timeout in %d ms.", t3510.duration());
  t3510.run();

  state.set_registered_initiated();

  return SRSRAN_SUCCESS;
}

/*******************************************************************************
 * UE Stack and RRC common Interface
 ******************************************************************************/
bool nas_5g::is_registered()
{
  return state.get_state() == mm5g_state_t::state_t::registered;
}

/*******************************************************************************
 * NAS Timers
 ******************************************************************************/
void nas_5g::timer_expired(uint32_t timeout_id)
{
  // TODO
}

/*******************************************************************************
 * UE Stack Interface
 ******************************************************************************/
int nas_5g::switch_on()
{
  logger.info("Switching on");
  state.set_deregistered(mm5g_state_t::deregistered_substate_t::plmn_search);
  return SRSRAN_SUCCESS;
}

int nas_5g::switch_off()
{
  logger.info("Switching off");
  // TODO
  return SRSRAN_SUCCESS;
}

int nas_5g::enable_data()
{
  logger.info("Enabling data services");
  return switch_on();
}

int nas_5g::disable_data()
{
  logger.info("Disabling data services");
  // TODO
  return SRSRAN_SUCCESS;
}

int nas_5g::start_service_request()
{
  logger.info("Service Request");
  // TODO
  return SRSRAN_SUCCESS;
}

} // namespace srsue