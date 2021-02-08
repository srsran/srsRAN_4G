/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2020 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsue/hdr/stack/mac_nr/mac_nr.h"
#include "srslte/mac/mac_rar_pdu_nr.h"

namespace srsue {

mac_nr::mac_nr(srslte::ext_task_sched_handle task_sched_) :
  task_sched(task_sched_), logger(srslog::fetch_basic_logger("MAC"))
{
  tx_buffer  = srslte::make_byte_buffer();
  rlc_buffer = srslte::make_byte_buffer();
}

mac_nr::~mac_nr()
{
  stop();
}

int mac_nr::init(const mac_nr_args_t& args_, phy_interface_mac_nr* phy_, rlc_interface_mac* rlc_)
{
  args = args_;
  phy  = phy_;
  rlc  = rlc_;

  // Create Stack task dispatch queue
  stack_task_dispatch_queue = task_sched.make_task_queue();

  // Set up pcap
  if (args.pcap.enable) {
    pcap.reset(new srslte::mac_pcap(srslte::srslte_rat_t::nr));
    pcap->open(args.pcap.filename.c_str());
  }

  started = true;

  return SRSLTE_SUCCESS;
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

// Implement Section 5.9
void mac_nr::reset()
{
  logger.info("Resetting MAC-NR");
}

void mac_nr::run_tti(const uint32_t tti)
{
  // Step all procedures
  logger.debug("Running MAC tti=%d", tti);
}

uint16_t mac_nr::get_ul_sched_rnti(const uint32_t tti)
{
  return crnti;
}

bool mac_nr::is_si_opportunity()
{
  // TODO: ask RRC if we need SI
  return false;
}

bool mac_nr::is_paging_opportunity()
{
  return false;
}

uint16_t mac_nr::get_dl_sched_rnti(const uint32_t tti)
{
  // Priority: SI-RNTI, P-RNTI, RA-RNTI, Temp-RNTI, CRNTI
  if (is_si_opportunity()) {
    return SRSLTE_SIRNTI;
  }

  if (is_paging_opportunity()) {
    return SRSLTE_PRNTI;
  }

  // TODO: add new RA proc shortly
#if 0
  if (proc_ra->is_rar_opportunity()) {
    return proc_ra->get_rar_rnti();
  }

  if (proc_ra->has_temp_rnti() && has_crnti() == false) {
    return proc_ra->get_temp_rnti();
  }
#endif

  if (has_crnti()) {
    return get_crnti();
  }

  // turn off DCI search for this TTI
  return SRSLTE_INVALID_RNTI;
}

bool mac_nr::has_crnti()
{
  return crnti != SRSLTE_INVALID_RNTI;
}

uint16_t mac_nr::get_crnti()
{
  return crnti;
}

void mac_nr::bch_decoded_ok(uint32_t tti, srslte::unique_byte_buffer_t payload)
{
  // Send MIB to RLC
  rlc->write_pdu_bcch_bch(std::move(payload));

  if (pcap) {
    // pcap->write_dl_bch(payload, len, true, tti);
  }
}

int mac_nr::sf_indication(const uint32_t tti)
{
  run_tti(tti);
  return SRSLTE_SUCCESS;
}

void mac_nr::prach_sent(const uint32_t tti,
                        const uint32_t s_id,
                        const uint32_t t_id,
                        const uint32_t f_id,
                        const uint32_t ul_carrier_id)
{
  // TODO: indicate to RA proc
}

// This function handles all PCAP writing for a decoded DL TB
void mac_nr::write_pcap(const uint32_t cc_idx, mac_nr_grant_dl_t& grant)
{
  if (pcap) {
    for (uint32_t i = 0; i < SRSLTE_MAX_CODEWORDS; ++i) {
      if (grant.tb[i] != nullptr) {
        if (SRSLTE_RNTI_ISRAR(grant.rnti)) { // TODO: replace with proc_ra->get_rar_rnti()
          pcap->write_dl_ra_rnti_nr(grant.tb[i]->msg, grant.tb[i]->N_bytes, grant.rnti, true, grant.tti);
        } else if (grant.rnti == SRSLTE_PRNTI) {
          pcap->write_dl_pch_nr(grant.tb[i]->msg, grant.tb[i]->N_bytes, grant.rnti, true, grant.tti);
        } else {
          pcap->write_dl_crnti_nr(grant.tb[i]->msg, grant.tb[i]->N_bytes, grant.rnti, true, grant.tti);
        }
      }
    }
  }
}

/**
 * \brief Called from PHY after decoding a TB
 *
 * The TB can directly be used
 *
 * @param cc_idx
 * @param grant structure
 */
void mac_nr::tb_decoded(const uint32_t cc_idx, mac_nr_grant_dl_t& grant)
{
  write_pcap(cc_idx, grant);

  // handle PDU
  if (SRSLTE_RNTI_ISRAR(grant.rnti)) { // TODO: replace with proc_ra->get_rar_rnti()
    // TODO: pass to RA proc
    handle_rar_pdu(grant);
  } else {
    // Push DL PDUs to queue for back-ground processing
    for (uint32_t i = 0; i < SRSLTE_MAX_CODEWORDS; ++i) {
      if (grant.tb[i] != nullptr) {
        pdu_queue.push(std::move(grant.tb[i]));
      }
    }
  }

  metrics[cc_idx].rx_pkts++;
  stack_task_dispatch_queue.push([this]() { process_pdus(); });
}

// Temporary helper until RA proc is complete
void mac_nr::handle_rar_pdu(mac_nr_grant_dl_t& grant)
{
  for (uint32_t i = 0; i < SRSLTE_MAX_CODEWORDS; ++i) {
    if (grant.tb[i] != nullptr) {
      srslte::mac_rar_pdu_nr pdu;
      if (!pdu.unpack(grant.tb[i]->msg, grant.tb[i]->N_bytes)) {
        logger.warning("Error unpacking RAR PDU");
        return;
      }
      logger.info(pdu.to_string());

      for (auto& subpdu : pdu.get_subpdus()) {
        if (subpdu.has_rapid() && subpdu.get_rapid() == 0 /* selected preamble */) {
          phy->set_ul_grant(subpdu.get_ul_grant());
        }
      }
    }
  }
}

void mac_nr::new_grant_ul(const uint32_t cc_idx, const mac_nr_grant_ul_t& grant)
{
  phy_interface_stack_nr::tx_request_t tx_request = {};

  get_ul_data(grant, &tx_request);

  // send TX.request
  phy->tx_request(tx_request);

  metrics[cc_idx].tx_pkts++;
}

void mac_nr::get_ul_data(const mac_nr_grant_ul_t& grant, phy_interface_stack_nr::tx_request_t* tx_request)
{
  // Todo: delegate to mux class
  tx_request->tb_len = grant.tbs;

  // initialize MAC PDU
  tx_buffer->clear();
  tx_pdu.init_tx(tx_buffer.get(), grant.tbs, true);

  while (tx_pdu.get_remaing_len() >= MIN_RLC_PDU_LEN) {
    // read RLC PDU
    rlc_buffer->clear();
    uint8_t* rd      = rlc_buffer->msg;
    int      pdu_len = rlc->read_pdu(args.drb_lcid, rd, tx_pdu.get_remaing_len() - 2);

    // Add SDU if RLC has something to tx
    if (pdu_len > 0) {
      rlc_buffer->N_bytes = pdu_len;
      logger.info(rlc_buffer->msg, rlc_buffer->N_bytes, "Read %d B from RLC", rlc_buffer->N_bytes);

      // add to MAC PDU and pack
      if (tx_pdu.add_sdu(args.drb_lcid, rlc_buffer->msg, rlc_buffer->N_bytes) != SRSLTE_SUCCESS) {
        logger.error("Error packing MAC PDU");
      }
    } else {
      break;
    }
  }

  // Pack PDU
  tx_pdu.pack();

  logger.info(tx_buffer->msg, tx_buffer->N_bytes, "Generated MAC PDU (%d B)", tx_buffer->N_bytes);

  tx_request->data   = tx_buffer->msg;
  tx_request->tb_len = tx_buffer->N_bytes;

  if (pcap) {
    pcap->write_ul_crnti_nr(tx_request->data, tx_request->tb_len, grant.rnti, grant.pid, grant.tti);
  }
}

void mac_nr::timer_expired(uint32_t timer_id)
{
  // not implemented
}

void mac_nr::setup_lcid(const srslte::logical_channel_config_t& config)
{
  logger.info("Logical Channel Setup: LCID=%d, LCG=%d, priority=%d, PBR=%d, BSD=%dms, bucket_size=%d",
              config.lcid,
              config.lcg,
              config.priority,
              config.PBR,
              config.BSD,
              config.bucket_size);
  // mux_unit.setup_lcid(config);
  // bsr_procedure.setup_lcid(config.lcid, config.lcg, config.priority);
}

void mac_nr::set_config(const srslte::bsr_cfg_t& bsr_cfg)
{
  logger.info("BSR config periodic timer %d retx timer %d", bsr_cfg.periodic_timer, bsr_cfg.retx_timer);
  logger.warning("Not handling BSR config yet");
}

void mac_nr::set_config(const srslte::sr_cfg_t& sr_cfg)
{
  logger.info("Scheduling Request Config DSR tansmax %d", sr_cfg.dsr_transmax);
  logger.warning("Not Scheduling Request Config yet");
}

void mac_nr::set_contention_id(uint64_t ue_identity)
{
  contention_id = ue_identity;
}

bool mac_nr::set_crnti(const uint16_t crnti_)
{
  if (is_valid_crnti(crnti_)) {
    logger.info("Setting C-RNTI to 0x%X", crnti_);
    crnti = crnti_;
    return true;
  } else {
    logger.warning("Failed to set C-RNTI, 0x%X is not valid.", crnti_);
    return false;
  }
}

bool mac_nr::is_valid_crnti(const uint16_t crnti)
{
  // TS 38.321 15.3.0 Table 7.1-1
  return (crnti >= 0x0001 && crnti <= 0xFFEF);
}

void mac_nr::get_metrics(mac_metrics_t m[SRSLTE_MAX_CARRIERS]) {}

/**
 * Called from the main stack thread to process received PDUs
 */
void mac_nr::process_pdus()
{
  while (started and not pdu_queue.empty()) {
    srslte::unique_byte_buffer_t pdu = pdu_queue.wait_pop();
    // TODO: delegate to demux class
    handle_pdu(std::move(pdu));
  }
}

void mac_nr::handle_pdu(srslte::unique_byte_buffer_t pdu)
{
  logger.info(pdu->msg, pdu->N_bytes, "Handling MAC PDU (%d B)", pdu->N_bytes);

  rx_pdu.init_rx();
  rx_pdu.unpack(pdu->msg, pdu->N_bytes);

  for (uint32_t i = 0; i < rx_pdu.get_num_subpdus(); ++i) {
    srslte::mac_sch_subpdu_nr subpdu = rx_pdu.get_subpdu(i);
    logger.info("Handling subPDU %d/%d: lcid=%d, sdu_len=%d",
                i,
                rx_pdu.get_num_subpdus(),
                subpdu.get_lcid(),
                subpdu.get_sdu_length());

    if (subpdu.get_lcid() == args.drb_lcid) {
      rlc->write_pdu(subpdu.get_lcid(), subpdu.get_sdu(), subpdu.get_sdu_length());
    }
  }
}

} // namespace srsue
