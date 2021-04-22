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

#ifndef SRSUE_MAC_NR_INTERFACES_H
#define SRSUE_MAC_NR_INTERFACES_H

#include "srsran/common/interfaces_common.h"
#include "srsran/mac/mac_sch_pdu_nr.h"

namespace srsue {
/**
 * @brief Interface from MAC NR parent class to subclass random access procedure
 */
class mac_interface_proc_ra_nr
{
public:
  // Functions for identity handling, e.g., contention id and c-rnti
  virtual uint64_t get_contention_id()         = 0;
  virtual uint16_t get_crnti()                 = 0;
  virtual bool     set_crnti(uint16_t c_rnti)  = 0;

  // Functions for msg3 manipulation which shall be transparent to the procedure
  virtual bool msg3_is_transmitted() = 0;
  virtual void msg3_flush()          = 0;
  virtual void msg3_prepare()        = 0;
  virtual bool msg3_is_empty()       = 0;

  // RRC functions
  virtual void rrc_ra_problem() = 0;
};

/**
 * @brief Interface from MAC NR parent class to SR subclass
 */
class mac_interface_sr_nr
{
public:
  // SR can query MAC (as proxy for RA) to start RA procedure
  virtual void start_ra() = 0;
};

/**
 * @brief Interface from MAC NR parent class to mux subclass
 */
class mac_interface_mux_nr
{
public:
  // MUX can query MAC for current C-RNTI for Msg3 transmission
  virtual uint16_t get_crnti() = 0;

  // MUX queries MAC to return LCG state for SBSR
  virtual srsran::mac_sch_subpdu_nr::lcg_bsr_t generate_sbsr() = 0;
};

/**
 * @brief Interface from MAC NR parent class to HARQ subclass
 */
class mac_interface_harq_nr
{
public:
  // HARQ can query MAC for current C-RNTI
  virtual uint16_t get_crnti() = 0;

  // MAC also provides Temp C-RNTI (through RA proc)
  virtual uint16_t get_temp_crnti() = 0;

  // MAC provides the Currently Scheduled RNTI (for SPS)
  virtual uint16_t get_csrnti() = 0;
};

/**
 * @brief Interface from HARQ class to demux class
 */
class demux_interface_harq_nr
{
public:
  /// Inform demux unit about a newly decoded TB.
  virtual void push_pdu(srsran::unique_byte_buffer_t pdu, uint32_t tti) = 0;
};

} // namespace srsue

#endif // SRSUE_MAC_NR_INTERFACES_H