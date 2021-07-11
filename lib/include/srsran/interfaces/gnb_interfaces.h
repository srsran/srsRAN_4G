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

#ifndef SRSRAN_GNB_INTERFACES_H
#define SRSRAN_GNB_INTERFACES_H

#include "srsran/srsran.h"

#include "srsran/common/interfaces_common.h"
#include "srsran/common/security.h"
#include "srsran/interfaces/pdcp_interface_types.h"
#include "srsran/interfaces/rlc_interface_types.h"
#include "srsran/interfaces/rrc_interface_types.h"
#include "srsran/interfaces/sched_interface.h"

namespace srsenb {

/*****************************
 *      MAC INTERFACES
 ****************************/
class mac_interface_rrc_nr
{
public:
  // Provides cell configuration including SIB periodicity, etc.
  virtual int cell_cfg(srsenb::sched_interface::cell_cfg_t* cell_cfg) = 0;
};

class mac_interface_rlc_nr
{
public:
  virtual int rlc_buffer_state(uint16_t rnti, uint32_t lc_id, uint32_t tx_queue, uint32_t retx_queue) = 0;
};

/*****************************
 *      RLC INTERFACES
 ****************************/
class rlc_interface_mac_nr
{
public:
  /* MAC calls RLC to get RLC segment of nof_bytes length.
   * Segmentation happens in this function. RLC PDU is stored in payload. */
  virtual int  read_pdu(uint16_t rnti, uint32_t lcid, uint8_t* payload, uint32_t nof_bytes) = 0;
  virtual void read_pdu_pcch(uint8_t* payload, uint32_t buffer_size)                        = 0;

  /* MAC calls RLC to push an RLC PDU. This function is called from an independent MAC thread.
   * PDU gets placed into the buffer and higher layer thread gets notified. */
  virtual void write_pdu(uint16_t rnti, uint32_t lcid, uint8_t* payload, uint32_t nof_bytes) = 0;
};
class rlc_interface_pdcp_nr
{
public:
  /* PDCP calls RLC to push an RLC SDU. SDU gets placed into the RLC buffer and MAC pulls
   * RLC PDUs according to TB size. */
  virtual void write_sdu(uint16_t rnti, uint32_t lcid, srsran::unique_byte_buffer_t sdu) = 0;
  virtual bool rb_is_um(uint16_t rnti, uint32_t lcid)                                    = 0;
  virtual bool sdu_queue_is_full(uint16_t rnti, uint32_t lcid)                           = 0;
};
class rlc_interface_rrc_nr
{
public:
  virtual void clear_buffer(uint16_t rnti)                                               = 0;
  virtual void add_user(uint16_t rnti)                                                   = 0;
  virtual void rem_user(uint16_t rnti)                                                   = 0;
  virtual void add_bearer(uint16_t rnti, uint32_t lcid, srsran::rlc_config_t cnfg)       = 0;
  virtual void add_bearer_mrb(uint16_t rnti, uint32_t lcid)                              = 0;
  virtual void write_sdu(uint16_t rnti, uint32_t lcid, srsran::unique_byte_buffer_t sdu) = 0;
};

/*****************************
 *      PDCP INTERFACES
 ****************************/
class pdcp_interface_rlc_nr
{
public:
  /* RLC calls PDCP to push a PDCP PDU. */
  virtual void write_pdu(uint16_t rnti, uint32_t lcid, srsran::unique_byte_buffer_t sdu)               = 0;
  virtual void notify_delivery(uint16_t rnti, uint32_t lcid, const srsran::pdcp_sn_vector_t& pdcp_sns) = 0;
  virtual void notify_failure(uint16_t rnti, uint32_t lcid, const srsran::pdcp_sn_vector_t& pdcp_sns)  = 0;
};

class pdcp_interface_rrc_nr
{
public:
  virtual void reset(uint16_t rnti)                                                                = 0;
  virtual void add_user(uint16_t rnti)                                                             = 0;
  virtual void rem_user(uint16_t rnti)                                                             = 0;
  virtual void write_sdu(uint16_t rnti, uint32_t lcid, srsran::unique_byte_buffer_t sdu)           = 0;
  virtual void add_bearer(uint16_t rnti, uint32_t lcid, srsran::pdcp_config_t cnfg)                = 0;
  virtual void config_security(uint16_t rnti, uint32_t lcid, srsran::as_security_config_t sec_cfg) = 0;
  virtual void enable_integrity(uint16_t rnti, uint32_t lcid)                                      = 0;
  virtual void enable_encryption(uint16_t rnti, uint32_t lcid)                                     = 0;
};

class pdcp_interface_sdap_nr
{
public:
  virtual void write_sdu(uint16_t rnti, uint32_t lcid, srsran::unique_byte_buffer_t pdu) = 0;
};

/*****************************
 *      SDAP INTERFACES
 ****************************/
class sdap_interface_pdcp_nr
{
public:
  virtual void write_pdu(uint16_t rnti, uint32_t lcid, srsran::unique_byte_buffer_t pdu) = 0;
};
class sdap_interface_gtpu_nr
{
public:
  virtual void write_sdu(uint16_t rnti, uint32_t lcid, srsran::unique_byte_buffer_t pdu) = 0;
};

/*****************************
 *      GTPU INTERFACES
 ****************************/
class gtpu_interface_rrc_nr
{
public:
};

class gtpu_interface_sdap_nr
{
public:
  virtual void write_pdu(uint16_t rnti, uint32_t lcid, srsran::unique_byte_buffer_t pdu) = 0;
};

/*****************************
 *      RRC INTERFACES
 ****************************/
class rrc_interface_phy_nr
{};
class rrc_interface_mac_nr
{
public:
  // Provides MIB packed message
  virtual int read_pdu_bcch_bch(const uint32_t tti, srsran::unique_byte_buffer_t& buffer)   = 0;
  virtual int read_pdu_bcch_dlsch(uint32_t sib_index, srsran::unique_byte_buffer_t& buffer) = 0;
};
class rrc_interface_rlc_nr
{
public:
  virtual void        read_pdu_pcch(uint8_t* payload, uint32_t payload_size)                    = 0;
  virtual void        max_retx_attempted(uint16_t rnti)                                         = 0;
  virtual void        protocol_failure(uint16_t rnti)                                           = 0;
  virtual void        write_pdu(uint16_t rnti, uint32_t lcid, srsran::unique_byte_buffer_t sdu) = 0;
  virtual const char* get_rb_name(uint32_t lcid)                                                = 0;
};
class rrc_interface_pdcp_nr
{
public:
  virtual void write_pdu(uint16_t rnti, uint32_t lcid, srsran::unique_byte_buffer_t pdu) = 0;
  virtual void notify_pdcp_integrity_error(uint16_t rnti, uint32_t lcid)                 = 0;
};

class phy_interface_stack_nr
{
public:
  const static int MAX_DL_GRANTS = 4;

  typedef struct {
    // TODO: include NR related fields
  } dl_sched_grant_t;

  typedef struct {
    bool mib_present;
  } bch_sched_t;

  typedef struct {
    uint32_t         tti;
    uint32_t         nof_grants;
    dl_sched_grant_t pdsch[MAX_DL_GRANTS];
    int              beam_id;
  } dl_config_request_t;

  typedef struct {
    bch_sched_t pbch;
    uint16_t    length;
    uint16_t    index;               // index indicated in dl_config
    uint8_t*    data[SRSRAN_MAX_TB]; // always a pointer in our case
  } tx_request_pdu_t;

  typedef struct {
    uint32_t         tti;
    uint32_t         tb_len;
    uint32_t         nof_pdus;
    tx_request_pdu_t pdus[MAX_DL_GRANTS];
  } tx_request_t;

  virtual int dl_config_request(const dl_config_request_t& request) = 0;
  virtual int tx_request(const tx_request_t& request)               = 0;
};

class stack_interface_mac
{
public:
  ///< MAC calls stack to inform about new PDUs having arrived to be further processes in stack thread
  virtual void process_pdus() = 0;
};

class mac_interface_phy_nr
{
public:
  const static int MAX_SSB        = 4;
  const static int MAX_GRANTS     = 64;
  const static int MAX_NZP_CSI_RS = 4;

  struct pdcch_dl_t {
    srsran_dci_cfg_nr_t dci_cfg = {};
    srsran_dci_dl_nr_t  dci     = {};
  };

  struct pdcch_ul_t {
    srsran_dci_cfg_nr_t dci_cfg = {};
    srsran_dci_ul_nr_t  dci     = {};
  };

  struct pdsch_t {
    srsran_sch_cfg_nr_t                 sch  = {}; ///< PDSCH configuration
    std::array<uint8_t*, SRSRAN_MAX_TB> data = {}; ///< Data pointer
  };

  struct ssb_t {
    srsran_pbch_msg_nr_t pbch_msg = {};
  };

  struct dl_sched_t {
    srsran::bounded_vector<ssb_t, MAX_SSB>                               ssb;
    srsran::bounded_vector<pdcch_dl_t, MAX_GRANTS>                       pdcch_dl;
    srsran::bounded_vector<pdcch_ul_t, MAX_GRANTS>                       pdcch_ul;
    srsran::bounded_vector<pdsch_t, MAX_GRANTS>                          pdsch;
    srsran::bounded_vector<srsran_csi_rs_nzp_resource_t, MAX_NZP_CSI_RS> nzp_csi_rs;
  };

  struct pusch_t {
    uint32_t                            pid  = 0;  ///< HARQ process ID
    srsran_sch_cfg_nr_t                 sch  = {}; ///< PUSCH configuration
    std::array<uint8_t*, SRSRAN_MAX_TB> data = {}; ///< Data pointer
  };

  struct pucch_t {
    srsran_uci_cfg_nr_t          uci_cfg;   ///< UCI configuration
    srsran_pucch_nr_common_cfg_t pucch_cfg; ///< UE dedicated PUCCH configuration
    srsran_pucch_nr_resource_t   resource;  ///< PUCCH resource
  };

  struct ul_sched_t {
    srsran::bounded_vector<pusch_t, MAX_GRANTS> pusch;
    srsran::bounded_vector<pucch_t, MAX_GRANTS> pucch;
  };

  struct pucch_info_t {
    srsran_uci_data_nr_t uci_data; ///< RNTI is available under cfg->pucch->rnti
    // ... add signal measurements here
  };

  struct pusch_info_t {
    uint16_t              rnti;
    uint32_t              pid = 0; ///< HARQ process ID
    srsran_pusch_res_nr_t pusch_data;
    srsran_uci_cfg_nr_t   uci_cfg; ///< Provides UCI configuration, so stack does not need to keep the pending state
    // ... add signal measurements here
  };

  virtual int slot_indication(const srsran_slot_cfg_t& slot_cfg)                            = 0;
  virtual int get_dl_sched(const srsran_slot_cfg_t& slot_cfg, dl_sched_t& dl_sched)         = 0;
  virtual int get_ul_sched(const srsran_slot_cfg_t& slot_cfg, ul_sched_t& ul_sched)         = 0;
  virtual int pucch_info(const srsran_slot_cfg_t& slot_cfg, const pucch_info_t& pucch_info) = 0;
  virtual int pusch_info(const srsran_slot_cfg_t& slot_cfg, const pusch_info_t& pusch_info) = 0;
};

class stack_interface_phy_nr : public mac_interface_phy_nr, public srsran::stack_interface_phy_nr
{
public:
  struct rx_data_ind_t {
    uint32_t                     tti;
    uint16_t                     rnti;
    srsran::unique_byte_buffer_t tb;
  };

  virtual int rx_data_indication(rx_data_ind_t& grant) = 0;
};

} // namespace srsenb

#endif // SRSRAN_GNB_INTERFACES_H
