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

#include "srsran/interfaces/rrc_interface_types.h"
#include "srsran/interfaces/sched_interface.h"

#ifndef SRSRAN_ENB_MAC_INTERFACES_H
#define SRSRAN_ENB_MAC_INTERFACES_H

namespace srsenb {

struct mac_args_t {
  uint32_t                      nof_prb; ///< Needed to dimension MAC softbuffers for all cells
  sched_interface::sched_args_t sched;
  int                           nr_tb_size = -1;
  uint32_t                      max_nof_ues;
  uint32_t                      max_nof_kos;
};

/* Interface PHY -> MAC */
class mac_interface_phy_lte
{
public:
  const static int MAX_GRANTS = 64;

  /**
   * DL grant structure per UE
   */
  struct dl_sched_grant_t {
    srsran_dci_dl_t         dci                          = {};
    uint8_t*                data[SRSRAN_MAX_TB]          = {};
    srsran_softbuffer_tx_t* softbuffer_tx[SRSRAN_MAX_TB] = {};
  };

  /**
   * DL Scheduling result per cell/carrier
   */
  typedef struct {
    dl_sched_grant_t pdsch[MAX_GRANTS]; //< DL Grants
    uint32_t         nof_grants;        //< Number of DL grants
    uint32_t         cfi;               //< Current CFI of the cell, it can vary across cells
  } dl_sched_t;

  /**
   * List of DL scheduling results, one entry per cell/carrier
   */
  using dl_sched_list_t = srsran::bounded_vector<dl_sched_t, SRSRAN_MAX_CARRIERS>;

  typedef struct {
    uint16_t rnti;
    bool     ack;
  } ul_sched_ack_t;

  /**
   * UL grant information per UE
   */
  typedef struct {
    srsran_dci_ul_t         dci;
    uint32_t                pid;
    uint32_t                current_tx_nb;
    uint8_t*                data;
    bool                    needs_pdcch;
    srsran_softbuffer_rx_t* softbuffer_rx;
  } ul_sched_grant_t;

  /**
   * UL Scheduling result per cell/carrier
   */
  typedef struct {
    ul_sched_grant_t pusch[MAX_GRANTS];
    ul_sched_ack_t   phich[MAX_GRANTS];
    uint32_t         nof_grants;
    uint32_t         nof_phich;
  } ul_sched_t;

  /**
   * List of UL scheduling results, one entry per cell/carrier
   */
  using ul_sched_list_t = srsran::bounded_vector<ul_sched_t, SRSRAN_MAX_CARRIERS>;

  virtual int  sr_detected(uint32_t tti, uint16_t rnti)                                                       = 0;
  virtual void rach_detected(uint32_t tti, uint32_t primary_cc_idx, uint32_t preamble_idx, uint32_t time_adv) = 0;

  /**
   * PHY callback for giving MAC the Rank Indicator information of a given RNTI for an eNb cell/carrier.
   *
   * @param tti the given TTI
   * @param rnti the UE identifier in the eNb
   * @param cc_idx The eNb Cell/Carrier where the measurement corresponds
   * @param ri_value the actual Rank Indicator value, 0 for 1 layer, 1 for two layers and so on.
   * @return SRSRAN_SUCCESS if no error occurs, SRSRAN_ERROR* if an error occurs
   */
  virtual int ri_info(uint32_t tti, uint16_t rnti, uint32_t cc_idx, uint32_t ri_value) = 0;

  /**
   * PHY callback for giving MAC the Pre-coding Matrix Indicator information of a given RNTI for an eNb cell/carrier.
   *
   * @param tti the given TTI
   * @param rnti the UE identifier in the eNb
   * @param cc_idx The eNb Cell/Carrier where the measurement corresponds
   * @param pmi_value the actual PMI value
   * @return SRSRAN_SUCCESS if no error occurs, SRSRAN_ERROR* if an error occurs
   */
  virtual int pmi_info(uint32_t tti, uint16_t rnti, uint32_t cc_idx, uint32_t pmi_value) = 0;

  /**
   * PHY callback for for giving MAC the Channel Quality information of a given RNTI, TTI and eNb cell/carrier
   * @param tti the given TTI
   * @param rnti the UE identifier in the eNb
   * @param cc_idx The eNb Cell/Carrier where the measurement corresponds
   * @param cqi_value the corresponding Channel Quality Information
   * @return SRSRAN_SUCCESS if no error occurs, SRSRAN_ERROR* if an error occurs
   */
  virtual int cqi_info(uint32_t tti, uint16_t rnti, uint32_t cc_idx, uint32_t cqi_value) = 0;

  typedef enum { PUSCH = 0, PUCCH, SRS } ul_channel_t;

  /**
   * PHY callback for giving MAC the SNR in dB of an UL transmission for a given RNTI at a given carrier
   *
   * @param tti The measurement was made
   * @param rnti The UE identifier in the eNb
   * @param cc_idx The eNb Cell/Carrier where the UL transmission was received
   * @param snr_db The actual SNR of the received signal
   * @param ch Indicates uplink channel (PUSCH, PUCCH or SRS)
   * @return SRSRAN_SUCCESS if no error occurs, SRSRAN_ERROR* if an error occurs
   */
  virtual int snr_info(uint32_t tti, uint16_t rnti, uint32_t cc_idx, float snr_db, ul_channel_t ch) = 0;

  /**
   * PHY callback for giving MAC the Time Aligment information in microseconds of a given RNTI during a TTI processing
   *
   * @param tti The measurement was made
   * @param rnti The UE identifier in the eNb
   * @param ta_us The actual time alignment in microseconds
   * @return SRSRAN_SUCCESS if no error occurs, SRSRAN_ERROR* if an error occurs
   */
  virtual int ta_info(uint32_t tti, uint16_t rnti, float ta_us) = 0;

  /**
   * PHY callback for giving MAC the HARQ DL ACK/NACK feedback information for a given RNTI, TTI, eNb cell/carrier and
   * Transport block.
   *
   * @param tti the given TTI
   * @param rnti the UE identifier in the eNb
   * @param cc_idx the eNb Cell/Carrier identifier
   * @param tb_idx the transport block index
   * @param ack true for ACK, false for NACK, do not call for DTX
   * @return SRSRAN_SUCCESS if no error occurs, SRSRAN_ERROR* if an error occurs
   */
  virtual int ack_info(uint32_t tti, uint16_t rnti, uint32_t cc_idx, uint32_t tb_idx, bool ack) = 0;

  /**
   * Informs MAC about a received PUSCH transmission for given RNTI, TTI and eNb Cell/carrier.
   *
   * This function does not deallocate the uplink buffer. The function push_pdu() must be called after this
   * to inform the MAC that the uplink buffer can be discarded or pushed to the stack
   *
   * @param tti the given TTI
   * @param rnti the UE identifier in the eNb
   * @param cc_idx the eNb Cell/Carrier identifier
   * @param nof_bytes the number of grants carrierd by the PUSCH message
   * @param crc_res the CRC check, set to true if the message was decoded succesfully
   * @return SRSRAN_SUCCESS if no error occurs, SRSRAN_ERROR* if an error occurs
   */
  virtual int crc_info(uint32_t tti, uint16_t rnti, uint32_t cc_idx, uint32_t nof_bytes, bool crc_res) = 0;

  /**
   * Pushes an uplink PDU through the stack if crc_res==true or discards it if crc_res==false
   *
   * @param tti the given TTI
   * @param rnti the UE identifier in the eNb
   * @param enb_cc_idx the eNb Cell/Carrier identifier
   * @param nof_bytes the number of grants carrierd by the PUSCH message
   * @param crc_res the CRC check, set to true if the message was decoded succesfully
   * @return SRSRAN_SUCCESS if no error occurs, SRSRAN_ERROR* if an error occurs
   */
  virtual int push_pdu(uint32_t tti_rx, uint16_t rnti, uint32_t enb_cc_idx, uint32_t nof_bytes, bool crc_res) = 0;

  virtual int  get_dl_sched(uint32_t tti, dl_sched_list_t& dl_sched_res)                = 0;
  virtual int  get_mch_sched(uint32_t tti, bool is_mcch, dl_sched_list_t& dl_sched_res) = 0;
  virtual int  get_ul_sched(uint32_t tti, ul_sched_list_t& ul_sched_res)                = 0;
  virtual void set_sched_dl_tti_mask(uint8_t* tti_mask, uint32_t nof_sfs)               = 0;
};

class mac_interface_rlc
{
public:
  virtual int rlc_buffer_state(uint16_t rnti, uint32_t lc_id, uint32_t tx_queue, uint32_t retx_queue) = 0;
};

class mac_interface_rrc
{
public:
  /* Provides cell configuration including SIB periodicity, etc. */
  virtual int  cell_cfg(const std::vector<sched_interface::cell_cfg_t>& cell_cfg) = 0;
  virtual void reset()                                                            = 0;

  /* Manages UE configuration context */
  virtual int ue_cfg(uint16_t rnti, sched_interface::ue_cfg_t* cfg) = 0;
  virtual int ue_rem(uint16_t rnti)                                 = 0;

  /**
   * Called after Msg3 reception to set the UE C-RNTI, resolve contention, and alter the UE's configuration in the
   * scheduler and phy.
   *
   * @param temp_crnti temporary C-RNTI of the UE
   * @param crnti chosen C-RNTI for the UE
   * @param cfg new UE scheduler configuration
   */
  virtual int ue_set_crnti(uint16_t temp_crnti, uint16_t crnti, sched_interface::ue_cfg_t* cfg) = 0;

  /* Manages UE bearers and associated configuration */
  virtual int  bearer_ue_cfg(uint16_t rnti, uint32_t lc_id, sched_interface::ue_bearer_cfg_t* cfg) = 0;
  virtual int  bearer_ue_rem(uint16_t rnti, uint32_t lc_id)                                        = 0;
  virtual void phy_config_enabled(uint16_t rnti, bool enabled)                                     = 0;
  virtual void write_mcch(const srsran::sib2_mbms_t* sib2_,
                          const srsran::sib13_t*     sib13_,
                          const srsran::mcch_msg_t*  mcch_,
                          const uint8_t*             mcch_payload,
                          const uint8_t              mcch_payload_length)                                       = 0;

  /**
   * Allocate a C-RNTI for a new user, without adding it to the phy layer and scheduler yet
   * @return value of the allocated C-RNTI
   */
  virtual uint16_t reserve_new_crnti(const sched_interface::ue_cfg_t& ue_cfg) = 0;
};

// Combined interface for PHY to access stack (MAC and RRC)
class stack_interface_phy_lte : public mac_interface_phy_lte
{
public:
  virtual void tti_clock() = 0;
};

} // namespace srsenb

#endif // SRSRAN_ENB_MAC_INTERFACES_H
