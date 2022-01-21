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

#ifndef SRSRAN_RRC_NR_DU_MANAGER_H
#define SRSRAN_RRC_NR_DU_MANAGER_H

#include "rrc_nr_config.h"
#include "srsgnb/hdr/stack/mac/sched_nr_interface.h"
#include "srsran/asn1/rrc_nr.h"

namespace srsenb {

class du_cell_config
{
public:
  uint32_t cc;
  uint32_t pci;

  asn1::rrc_nr::mib_s          mib;
  srsran::unique_byte_buffer_t packed_mib;

  asn1::rrc_nr::sib1_s         sib1;
  srsran::unique_byte_buffer_t packed_sib1;

  /// SI messages (index=0 for SIB1)
  srsran::const_byte_span packed_si_msg(uint32_t idx) { return srsran::make_span(packed_sib1); }
  size_t                  nof_si_msgs() const { return 1; }
};

class du_config_manager
{
public:
  du_config_manager(const rrc_nr_cfg_t& cfg);
  ~du_config_manager();

  int add_cell();

  const du_cell_config& cell(uint32_t cc) const
  {
    srsran_assert(cc < cells.size(), "Unknown DU Cell Index=%d", cc);
    return *cells[cc];
  }

private:
  const rrc_nr_cfg_t&   cfg;
  srslog::basic_logger& logger;

  std::vector<std::unique_ptr<du_cell_config> > cells;
};

} // namespace srsenb

#endif // SRSRAN_RRC_NR_DU_MANAGER_H
