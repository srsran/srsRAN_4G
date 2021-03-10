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

#ifndef SRSENB_SDAP_H
#define SRSENB_SDAP_H

#include "srslte/common/buffer_pool.h"
#include "srslte/common/common.h"
#include "srslte/interfaces/gnb_interfaces.h"
#include "srslte/interfaces/ue_gw_interfaces.h"

namespace srsenb {

class sdap final : public sdap_interface_pdcp_nr, public sdap_interface_gtpu_nr
{
public:
  explicit sdap();
  bool init(pdcp_interface_sdap_nr* pdcp_, gtpu_interface_sdap_nr* gtpu_, srsue::gw_interface_pdcp* gw_);
  void stop();

  // Interface for PDCP
  void write_pdu(uint16_t rnti, uint32_t lcid, srslte::unique_byte_buffer_t pdu) final;

  // Interface for GTPU
  void write_sdu(uint16_t rnti, uint32_t lcid, srslte::unique_byte_buffer_t pdu) final;

private:
  gtpu_interface_sdap_nr*   m_gtpu = nullptr;
  pdcp_interface_sdap_nr*   m_pdcp = nullptr;
  srsue::gw_interface_pdcp* m_gw   = nullptr;

  // state
  bool running = false;
};

} // namespace srsenb

#endif // SRSENB_SDAP_H
