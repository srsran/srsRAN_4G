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

/**
 * @brief Dummy X2AP implementation
 *
 * Dummy X2 adapter to facilitate communication between EUTRA RRC and NR RRC
 * for EN-DC procedures.
 *
 * The class uses direct function calls instead of real X2AP ASN1 encoded
 * messages. It mainly focuses on Sec 9.1.4 of TS 36.423 Rel. 15.11
 * for E-UTRAN-NR Dual Connectivity Procedures, i.e. SgNB-*
 *
 * It furthermore provide an interface for the GTPU adapter to
 * write DL PDUs, which it then forwards to the NR PDCP.
 *
 * It also provides a method to allow the eNB to foward timing
 * signal, i.e. TTI tics, to the NR stack.
 */

#ifndef SRSENB_X2_ADAPTER_H
#define SRSENB_X2_ADAPTER_H

#include "srsran/interfaces/enb_x2_interfaces.h"
#include "stack/enb_stack_lte.h"
#include "stack/gnb_stack_nr.h"

namespace srsenb {

class x2_adapter final : public x2_interface
{
public:
  x2_adapter() = default;

  // init functions to set handle to stacks
  void set_eutra_stack(enb_stack_lte* eutra_stack_) { eutra_stack = eutra_stack_; }

  void set_nr_stack(gnb_stack_nr* nr_stack_) { nr_stack = nr_stack_; }

  /// rrc_nr_interface_rrc
  int sgnb_addition_request(uint16_t eutra_rnti, const sgnb_addition_req_params_t& params)
  {
    if (nr_stack == nullptr) {
      return SRSRAN_ERROR;
    }
    return nr_stack->sgnb_addition_request(eutra_rnti, params);
  }
  int sgnb_reconfiguration_complete(uint16_t eutra_rnti, asn1::dyn_octstring reconfig_response)
  {
    if (nr_stack == nullptr) {
      return SRSRAN_ERROR;
    }
    return nr_stack->sgnb_reconfiguration_complete(eutra_rnti, reconfig_response);
  }

  /// rrc_eutra_interface_rrc_nr
  void sgnb_addition_ack(uint16_t eutra_rnti, sgnb_addition_ack_params_t params)
  {
    if (eutra_stack == nullptr) {
      return;
    }
    eutra_stack->sgnb_addition_ack(eutra_rnti, params);
  }
  void sgnb_addition_reject(uint16_t eutra_rnti)
  {
    if (eutra_stack == nullptr) {
      return;
    }
    eutra_stack->sgnb_addition_reject(eutra_rnti);
  }
  void sgnb_addition_complete(uint16_t eutra_rnti, uint16_t nr_rnti)
  {
    if (eutra_stack == nullptr) {
      return;
    }
    eutra_stack->sgnb_addition_complete(eutra_rnti, nr_rnti);
  }

  // stack_nr_interface_stack_eutra
  void tti_clock()
  {
    if (nr_stack == nullptr) {
      return;
    }
    nr_stack->tti_clock();
  }

  // pdcp_interface_gtpu
  void write_sdu(uint16_t rnti, uint32_t lcid, srsran::unique_byte_buffer_t sdu, int pdcp_sn = -1)
  {
    if (nr_stack == nullptr) {
      return;
    }
    nr_stack->write_sdu(rnti, lcid, std::move(sdu), pdcp_sn);
  }
  std::map<uint32_t, srsran::unique_byte_buffer_t> get_buffered_pdus(uint16_t rnti, uint32_t lcid)
  {
    if (nr_stack == nullptr) {
      return {};
    }
    return nr_stack->get_buffered_pdus(rnti, lcid);
  }

  // gtpu_interface_pdcp
  void write_pdu(uint16_t rnti, uint32_t bearer_id, srsran::unique_byte_buffer_t pdu)
  {
    if (eutra_stack == nullptr) {
      return;
    }
    eutra_stack->write_pdu(rnti, bearer_id, std::move(pdu));
  }

private:
  enb_stack_lte* eutra_stack = nullptr;
  gnb_stack_nr*  nr_stack    = nullptr;
};

} // namespace srsenb

#endif // SRSENB_X2_ADAPTER_H
