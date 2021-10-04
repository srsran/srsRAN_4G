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

#include "srsran/interfaces/enb_gtpu_interfaces.h"
#include "srsran/interfaces/enb_pdcp_interfaces.h"
#include "srsran/interfaces/enb_rrc_interface_types.h"

#ifndef SRSRAN_ENB_X2_INTERFACES_H
#define SRSRAN_ENB_X2_INTERFACES_H

namespace srsenb {

/**
 * @brief Set of X2AP inspired interfaces to support 5G NSA
 *
 */

/// X2AP inspired interface to allow EUTRA RRC to call NR RRC
class rrc_nr_interface_rrc
{
public:
  struct sgnb_addition_req_params_t {
    uint32_t eps_bearer_id;
    // add configuration check
    // E-RAB Parameters, Tunnel address (IP address, TEID)
    // QCI, security, etc
  };

  /// Request addition of NR carrier for UE
  virtual int sgnb_addition_request(uint16_t eutra_rnti, const sgnb_addition_req_params_t& params) = 0;

  /// Provide information whether the requested configuration was applied successfully by the UE
  virtual int sgnb_reconfiguration_complete(uint16_t eutra_rnti, asn1::dyn_octstring reconfig_response) = 0;
};

/// X2AP inspired interface for response from NR RRC to EUTRA RRC
class rrc_eutra_interface_rrc_nr
{
public:
  /**
   * @brief List of parameters included in the SgNB addition Ack message
   * @param nr_secondary_cell_group_cfg_r15 Encoded part of the RRC Reconfiguration
   * @param nr_radio_bearer_cfg1_r15 Encoded part of the RRC Reconfiguration
   * @param eps_bearer_id ID of the transfered bearer
   */
  struct sgnb_addition_ack_params_t {
    uint16_t            nr_rnti = SRSRAN_INVALID_RNTI; // RNTI that was assigned to the UE
    asn1::dyn_octstring nr_secondary_cell_group_cfg_r15;
    asn1::dyn_octstring nr_radio_bearer_cfg1_r15;
    uint32_t            eps_bearer_id = 0; // (list of) successfully transfered EPS bearers
  };

  /**
   * @brief Signal successful addition of UE
   *
   * @param eutra_rnti The RNTI that the EUTRA RRC used to request the SgNB addition
   * @param params Parameter list
   */
  virtual void sgnb_addition_ack(uint16_t eutra_rnti, sgnb_addition_ack_params_t params) = 0;

  /**
   * @brief Signal unsuccessful SgNB addition
   *
   * @param eutra_rnti The RNTI that the EUTRA RRC used to request the SgNB addition
   */
  virtual void sgnb_addition_reject(uint16_t eutra_rnti) = 0;

  /**
   * @brief Signal completion of SgNB addition after UE (with new NR identity) has attached
   *
   * @param eutra_rnti The RNTI that the EUTRA RRC used to request the SgNB addition
   * @param nr_rnti    The RNTI that has been assigned to the UE on the SgNB
   */
  virtual void sgnb_addition_complete(uint16_t eutra_rnti, uint16_t nr_rnti) = 0;

  /**
   * @brief Signal user activity (i.e. DL/UL traffic) for given RNTI
   *
   * @param eutra_rnti The RNTI that the EUTRA RRC uses
   */
  virtual void set_activity_user(uint16_t eutra_rnti) = 0;
};

// combined interface used by X2 adapter
class x2_interface : public rrc_nr_interface_rrc,
                     public rrc_eutra_interface_rrc_nr,
                     public pdcp_interface_gtpu, // allow GTPU to access PDCP in DL direction
                     public gtpu_interface_pdcp  // allow PDCP to access GTPU in UL direction
{};

} // namespace srsenb

#endif // SRSRAN_ENB_X2_INTERFACES_H
