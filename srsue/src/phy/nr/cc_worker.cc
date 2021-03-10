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

#include "srsue/hdr/phy/nr/cc_worker.h"
#include "srslte/common/buffer_pool.h"
#include "srslte/srslte.h"

namespace srsue {
namespace nr {
cc_worker::cc_worker(uint32_t cc_idx_, srslog::basic_logger& log, state* phy_state_) :
  cc_idx(cc_idx_), phy(phy_state_), logger(log)
{
  cf_t* rx_buffer_c[SRSLTE_MAX_PORTS] = {};

  // Allocate buffers
  buffer_sz = SRSLTE_SF_LEN_PRB(phy->args.dl.nof_max_prb) * 5;
  for (uint32_t i = 0; i < phy_state_->args.dl.nof_rx_antennas; i++) {
    rx_buffer[i]   = srslte_vec_cf_malloc(buffer_sz);
    rx_buffer_c[i] = rx_buffer[i];
    tx_buffer[i]   = srslte_vec_cf_malloc(buffer_sz);
  }

  if (srslte_ue_dl_nr_init(&ue_dl, rx_buffer.data(), &phy_state_->args.dl) < SRSLTE_SUCCESS) {
    ERROR("Error initiating UE DL NR");
    return;
  }

  if (srslte_ue_ul_nr_init(&ue_ul, tx_buffer[0], &phy_state_->args.ul) < SRSLTE_SUCCESS) {
    ERROR("Error initiating UE DL NR");
    return;
  }

  if (srslte_softbuffer_rx_init_guru(&softbuffer_rx, SRSLTE_SCH_NR_MAX_NOF_CB_LDPC, SRSLTE_LDPC_MAX_LEN_ENCODED_CB) <
      SRSLTE_SUCCESS) {
    ERROR("Error init soft-buffer");
    return;
  }
}

cc_worker::~cc_worker()
{
  srslte_ue_dl_nr_free(&ue_dl);
  srslte_ue_ul_nr_free(&ue_ul);
  srslte_softbuffer_rx_free(&softbuffer_rx);
  for (cf_t* p : rx_buffer) {
    if (p != nullptr) {
      free(p);
    }
  }
  for (cf_t* p : tx_buffer) {
    if (p != nullptr) {
      free(p);
    }
  }
}

bool cc_worker::set_carrier(const srslte_carrier_nr_t* carrier)
{
  if (srslte_ue_dl_nr_set_carrier(&ue_dl, carrier) < SRSLTE_SUCCESS) {
    ERROR("Error setting carrier");
    return false;
  }

  if (srslte_ue_dl_nr_set_pdcch_config(&ue_dl, &phy->cfg.pdcch) < SRSLTE_SUCCESS) {
    ERROR("Error setting carrier");
    return false;
  }

  if (srslte_ue_ul_nr_set_carrier(&ue_ul, carrier) < SRSLTE_SUCCESS) {
    ERROR("Error setting carrier");
    return false;
  }

  // Set default PDSCH config
  phy->cfg.pdsch.rbg_size_cfg_1 = false;

  return true;
}

void cc_worker::set_tti(uint32_t tti)
{
  dl_slot_cfg.idx = tti;
  ul_slot_cfg.idx = TTI_TX(tti);
}

cf_t* cc_worker::get_rx_buffer(uint32_t antenna_idx)
{
  if (antenna_idx >= phy->args.dl.nof_rx_antennas) {
    return nullptr;
  }

  return rx_buffer.at(antenna_idx);
}

cf_t* cc_worker::get_tx_buffer(uint32_t antenna_idx)
{
  if (antenna_idx >= phy->args.dl.nof_rx_antennas) {
    return nullptr;
  }

  return tx_buffer.at(antenna_idx);
}

uint32_t cc_worker::get_buffer_len()
{
  return buffer_sz;
}

void cc_worker::decode_pdcch_dl()
{
  std::array<srslte_dci_dl_nr_t, SRSLTE_SEARCH_SPACE_MAX_NOF_CANDIDATES_NR> dci_rx = {};
  srsue::mac_interface_phy_nr::sched_rnti_t rnti = phy->stack->get_dl_sched_rnti_nr(dl_slot_cfg.idx);

  // Skip search if no valid RNTI is given
  if (rnti.id == SRSLTE_INVALID_RNTI) {
    return;
  }

  // Search for grants
  int n_dl =
      srslte_ue_dl_nr_find_dl_dci(&ue_dl, &dl_slot_cfg, rnti.id, rnti.type, dci_rx.data(), (uint32_t)dci_rx.size());
  if (n_dl < SRSLTE_SUCCESS) {
    logger.error("Error decoding DL NR-PDCCH");
    return;
  }

  // Iterate over all received grants
  for (int i = 0; i < n_dl; i++) {
    // Log found DCI
    if (logger.info.enabled()) {
      std::array<char, 512> str;
      srslte_dci_dl_nr_to_str(&dci_rx[i], str.data(), str.size());
      logger.info("PDCCH: cc=%d, %s", cc_idx, str.data());
    }

    // Enqueue UL grants
    phy->set_dl_pending_grant(dl_slot_cfg.idx, dci_rx[i]);
  }
}

void cc_worker::decode_pdcch_ul()
{
  std::array<srslte_dci_ul_nr_t, SRSLTE_SEARCH_SPACE_MAX_NOF_CANDIDATES_NR> dci_rx = {};
  srsue::mac_interface_phy_nr::sched_rnti_t rnti = phy->stack->get_ul_sched_rnti_nr(ul_slot_cfg.idx);

  // Skip search if no valid RNTI is given
  if (rnti.id == SRSLTE_INVALID_RNTI) {
    return;
  }

  // Search for grants
  int n_dl =
      srslte_ue_dl_nr_find_ul_dci(&ue_dl, &dl_slot_cfg, rnti.id, rnti.type, dci_rx.data(), (uint32_t)dci_rx.size());
  if (n_dl < SRSLTE_SUCCESS) {
    logger.error("Error decoding UL NR-PDCCH");
    return;
  }

  // Iterate over all received grants
  for (int i = 0; i < n_dl; i++) {
    // Log found DCI
    if (logger.info.enabled()) {
      std::array<char, 512> str;
      srslte_dci_ul_nr_to_str(&dci_rx[i], str.data(), str.size());
      logger.info("PDCCH: cc=%d, %s", cc_idx, str.data());
    }

    // Enqueue UL grants
    phy->set_ul_pending_grant(dl_slot_cfg.idx, dci_rx[i]);
  }
}

bool cc_worker::work_dl()
{
  // Run FFT
  srslte_ue_dl_nr_estimate_fft(&ue_dl, &dl_slot_cfg);

  // Decode PDCCH DL first
  decode_pdcch_dl();

  // Decode PDCCH UL after
  decode_pdcch_ul();

  // Get DL grant for this TTI, if available
  uint32_t                       pid          = 0;
  srslte_sch_cfg_nr_t            pdsch_cfg    = {};
  srslte_pdsch_ack_resource_nr_t ack_resource = {};
  if (phy->get_dl_pending_grant(dl_slot_cfg.idx, pdsch_cfg, ack_resource, pid)) {
    // Get data buffer
    srslte::unique_byte_buffer_t data = srslte::make_byte_buffer();
    data->N_bytes                     = pdsch_cfg.grant.tb[0].tbs / 8U;

    // Get soft-buffer from MAC
    // ...
    srslte_softbuffer_rx_reset(&softbuffer_rx);

    // Initialise PDSCH Result
    std::array<srslte_pdsch_res_nr_t, SRSLTE_MAX_CODEWORDS> pdsch_res = {};
    pdsch_res[0].payload                                              = data->msg;
    pdsch_cfg.grant.tb[0].softbuffer.rx                               = &softbuffer_rx;

    // Decode actual PDSCH transmission
    if (srslte_ue_dl_nr_decode_pdsch(&ue_dl, &dl_slot_cfg, &pdsch_cfg, pdsch_res.data()) < SRSLTE_SUCCESS) {
      ERROR("Error decoding PDSCH");
      return false;
    }

    // Logging
    if (logger.info.enabled()) {
      std::array<char, 512> str;
      srslte_ue_dl_nr_pdsch_info(&ue_dl, &pdsch_cfg, pdsch_res.data(), str.data(), str.size());
      logger.info(pdsch_res[0].payload, pdsch_cfg.grant.tb[0].tbs / 8, "PDSCH: cc=%d, %s", cc_idx, str.data());
    }

    // Enqueue PDSCH ACK information only if the RNTI is type C
    if (pdsch_cfg.grant.rnti_type == srslte_rnti_type_c) {
      phy->set_pending_ack(dl_slot_cfg.idx, ack_resource, pdsch_res[0].crc);
    }

    // Notify MAC about PDSCH decoding result
    if (pdsch_res[0].crc) {
      // Prepare grant
      mac_interface_phy_nr::mac_nr_grant_dl_t mac_nr_grant = {};
      mac_nr_grant.tb[0]                                   = std::move(data);
      mac_nr_grant.pid                                     = pid;
      mac_nr_grant.rnti                                    = pdsch_cfg.grant.rnti;
      mac_nr_grant.tti                                     = dl_slot_cfg.idx;

      if (pdsch_cfg.grant.rnti_type == srslte_rnti_type_ra) {
        phy->rar_grant_tti = dl_slot_cfg.idx;
      }

      // Send data to MAC
      phy->stack->tb_decoded(cc_idx, mac_nr_grant);
    }
  }

  return true;
}

bool cc_worker::work_ul()
{
  srslte_uci_data_nr_t uci_data = {};
  uint32_t             pid      = 0;

  // Gather PDSCH ACK information
  srslte_pdsch_ack_nr_t pdsch_ack  = {};
  bool                  has_ul_ack = phy->get_pending_ack(ul_slot_cfg.idx, pdsch_ack);

  // Request grant to PHY state for this transmit TTI
  srslte_sch_cfg_nr_t pusch_cfg       = {};
  bool                has_pusch_grant = phy->get_ul_pending_grant(ul_slot_cfg.idx, pusch_cfg, pid);

  // If PDSCH UL ACK is available, load into UCI
  if (has_ul_ack) {
    pdsch_ack.use_pusch = has_pusch_grant;
    if (srslte_ue_dl_nr_gen_ack(&phy->cfg.harq_ack, &pdsch_ack, &uci_data) < SRSLTE_SUCCESS) {
      ERROR("Filling UCI ACK bits");
      return false;
    }
  }

  // Add SR to UCI data if available
  phy->get_pending_sr(ul_slot_cfg.idx, uci_data);

  // Add CSI reports to UCI data if available
  phy->get_periodic_csi(ul_slot_cfg.idx, uci_data);

  if (has_pusch_grant) {
    // Notify MAC about PUSCH found grant
    mac_interface_phy_nr::tb_action_ul_t    ul_action    = {};
    mac_interface_phy_nr::mac_nr_grant_ul_t mac_ul_grant = {};
    mac_ul_grant.pid                                     = pid;
    mac_ul_grant.rnti                                    = pusch_cfg.grant.rnti;
    mac_ul_grant.tti                                     = ul_slot_cfg.idx;
    mac_ul_grant.tbs                                     = pusch_cfg.grant.tb[0].tbs;

    phy->stack->new_grant_ul(0, mac_ul_grant, &ul_action);

    // Assignning MAC provided values to PUSCH config structs
    pusch_cfg.grant.tb[0].softbuffer.tx = ul_action.tb.softbuffer;

    srslte_pusch_data_nr_t data = {};
    data.payload                = ul_action.tb.payload->msg;

    // Encode PUSCH transmission
    if (srslte_ue_ul_nr_encode_pusch(&ue_ul, &ul_slot_cfg, &pusch_cfg, &data) < SRSLTE_SUCCESS) {
      ERROR("Encoding PUSCH");
      return false;
    }

    // PUSCH Logging
    if (logger.info.enabled()) {
      std::array<char, 512> str;
      srslte_ue_ul_nr_pusch_info(&ue_ul, &pusch_cfg, str.data(), str.size());
      logger.info(ul_action.tb.payload->msg,
                  pusch_cfg.grant.tb[0].tbs / 8,
                  "PUSCH (NR): cc=%d, %s, tti_tx=%d",
                  cc_idx,
                  str.data(),
                  ul_slot_cfg.idx);
    }
  } else if (srslte_uci_nr_total_bits(&uci_data.cfg) > 0) {
    // Get PUCCH resource
    srslte_pucch_nr_resource_t resource = {};
    if (srslte_ra_ul_nr_pucch_resource(&phy->cfg.pucch, &uci_data.cfg, &resource) < SRSLTE_SUCCESS) {
      ERROR("Selecting PUCCH resource");
      return false;
    }

    // Encode PUCCH message
    if (srslte_ue_ul_nr_encode_pucch(&ue_ul, &ul_slot_cfg, &phy->cfg.pucch.common, &resource, &uci_data) <
        SRSLTE_SUCCESS) {
      ERROR("Encoding PUCCH");
      return false;
    }

    // PUCCH Logging
    if (logger.info.enabled()) {
      std::array<char, 512> str;
      srslte_ue_ul_nr_pucch_info(&resource, &uci_data, str.data(), str.size());
      logger.info("PUCCH: cc=%d, %s, tti_tx=%d", cc_idx, str.data(), ul_slot_cfg.idx);
    }
  } else {
    // No NR signal shall be transmitted
    srslte_vec_cf_zero(tx_buffer[0], ue_ul.ifft.sf_sz);
  }

  return true;
}

int cc_worker::read_pdsch_d(cf_t* pdsch_d)
{
  uint32_t nof_re = ue_dl.carrier.nof_prb * SRSLTE_NRE * 12;
  srslte_vec_cf_copy(pdsch_d, ue_dl.pdsch.d[0], nof_re);
  return nof_re;
}

} // namespace nr
} // namespace srsue
