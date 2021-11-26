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

#ifndef SRSRAN_GTPU_PDCP_ADAPTER_H
#define SRSRAN_GTPU_PDCP_ADAPTER_H

#include "srsran/common/bearer_manager.h"
#include "srsran/interfaces/enb_gtpu_interfaces.h"
#include "srsran/srslog/logger.h"

namespace srsenb {

class gtpu_pdcp_adapter final : public gtpu_interface_pdcp, public pdcp_interface_gtpu
{
public:
  gtpu_pdcp_adapter(srslog::basic_logger& logger_,
                    pdcp_interface_gtpu*  pdcp_lte,
                    pdcp_interface_gtpu*  pdcp_nr,
                    gtpu*                 gtpu_,
                    enb_bearer_manager&   bearers_) :
    logger(logger_), pdcp_lte_obj(pdcp_lte), pdcp_nr_obj(pdcp_nr), gtpu_obj(gtpu_), bearers(&bearers_)
  {}

  /// Converts LCID to EPS-BearerID and sends corresponding PDU to GTPU
  void write_pdu(uint16_t rnti, uint32_t lcid, srsran::unique_byte_buffer_t pdu) override
  {
    auto bearer = bearers->get_lcid_bearer(rnti, lcid);
    if (not bearer.is_valid()) {
      logger.error("Bearer rnti=0x%x, lcid=%d not found", rnti, lcid);
      return;
    }
    gtpu_obj->write_pdu(rnti, bearer.eps_bearer_id, std::move(pdu));
  }
  void write_sdu(uint16_t rnti, uint32_t eps_bearer_id, srsran::unique_byte_buffer_t sdu, int pdcp_sn = -1) override
  {
    auto bearer = bearers->get_radio_bearer(rnti, eps_bearer_id);
    // route SDU to PDCP entity
    if (bearer.rat == srsran::srsran_rat_t::lte) {
      pdcp_lte_obj->write_sdu(rnti, bearer.lcid, std::move(sdu), pdcp_sn);
    } else if (bearer.rat == srsran::srsran_rat_t::nr) {
      pdcp_nr_obj->write_sdu(rnti, bearer.lcid, std::move(sdu), pdcp_sn);
    } else {
      logger.warning("Can't deliver SDU for EPS bearer %d. Dropping it.", eps_bearer_id);
    }
  }
  std::map<uint32_t, srsran::unique_byte_buffer_t> get_buffered_pdus(uint16_t rnti, uint32_t eps_bearer_id) override
  {
    auto bearer = bearers->get_radio_bearer(rnti, eps_bearer_id);
    // route SDU to PDCP entity
    if (bearer.rat == srsran::srsran_rat_t::lte) {
      return pdcp_lte_obj->get_buffered_pdus(rnti, bearer.lcid);
    } else if (bearer.rat == srsran::srsran_rat_t::nr) {
      return pdcp_nr_obj->get_buffered_pdus(rnti, bearer.lcid);
    }
    logger.error("Bearer rnti=0x%x, eps-BearerID=%d not found", rnti, eps_bearer_id);
    return {};
  }

private:
  srslog::basic_logger& logger;
  gtpu*                 gtpu_obj     = nullptr;
  pdcp_interface_gtpu*  pdcp_lte_obj = nullptr;
  pdcp_interface_gtpu*  pdcp_nr_obj  = nullptr;
  enb_bearer_manager*   bearers      = nullptr;
};

} // namespace srsenb

#endif // SRSRAN_GTPU_PDCP_ADAPTER_H
