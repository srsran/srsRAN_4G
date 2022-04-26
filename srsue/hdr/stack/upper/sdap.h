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

#ifndef SRSUE_SDAP_H
#define SRSUE_SDAP_H

#include "srsran/common/buffer_pool.h"
#include "srsran/common/common.h"
#include "srsran/common/common_nr.h"
#include "srsran/interfaces/ue_gw_interfaces.h"
#include "srsran/interfaces/ue_pdcp_interfaces.h"
#include "srsran/interfaces/ue_sdap_interfaces.h"

namespace srsue {

class sdap final : public sdap_interface_pdcp_nr, public sdap_interface_gw_nr, public sdap_interface_rrc
{
public:
  explicit sdap(const char* logname);
  bool init(pdcp_interface_sdap_nr* pdcp_, srsue::gw_interface_pdcp* gw_);
  void stop();

  // Interface for GW
  void write_pdu(uint32_t lcid, srsran::unique_byte_buffer_t pdu) final;

  // Interface for PDCP
  void write_sdu(uint32_t lcid, srsran::unique_byte_buffer_t pdu) final;

  // Interface for RRC
  bool set_bearer_cfg(uint32_t lcid, const sdap_interface_rrc::bearer_cfg_t& cfg) final;

private:
  pdcp_interface_sdap_nr* m_pdcp = nullptr;
  gw_interface_pdcp*      m_gw   = nullptr;

  // state
  bool running = false;

  // configuration
  std::array<sdap_interface_rrc::bearer_cfg_t, srsran::MAX_NR_NOF_BEARERS> bearers = {};

  srslog::basic_logger& logger;
};

} // namespace srsue

#endif // SRSUE_SDAP_H
