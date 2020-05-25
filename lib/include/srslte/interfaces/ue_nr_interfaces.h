/*
 * Copyright 2013-2020 Software Radio Systems Limited
 *
 * This file is part of srsLTE.
 *
 * srsLTE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsLTE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#ifndef SRSLTE_UE_NR_INTERFACES_H
#define SRSLTE_UE_NR_INTERFACES_H

#include "srslte/common/interfaces_common.h"
#include <string>

namespace srsue {

class rrc_interface_phy_nr
{
public:
  virtual void in_sync()                   = 0;
  virtual void out_of_sync()               = 0;
  virtual void run_tti(const uint32_t tti) = 0;
};

class mac_interface_phy_nr
{
public:
  typedef struct {
    srslte::unique_byte_buffer_t tb[SRSLTE_MAX_TB];
    uint32_t                     pid;
    uint16_t                     rnti;
    uint32_t                     tti;
  } mac_nr_grant_dl_t;

  typedef struct {
    uint32_t pid;
    uint16_t rnti;
    uint32_t tti;
    uint32_t tbs;
  } mac_nr_grant_ul_t;

  virtual int sf_indication(const uint32_t tti) = 0; ///< FIXME: rename to slot indication

  /// Indicate succussfully received TB to MAC. The TB buffer is allocated in the PHY and handed as unique_ptr to MAC
  virtual void tb_decoded(const uint32_t cc_idx, mac_nr_grant_dl_t& grant) = 0;

  /// Indicate reception of UL grant (only TBS is provided). Buffer for resulting MAC PDU is provided by MAC and is
  /// passed as pointer to PHY during tx_reuqest
  virtual void new_grant_ul(const uint32_t cc_idx, const mac_nr_grant_ul_t& grant) = 0;
};

class mac_interface_rrc_nr
{};

class phy_interface_mac_nr
{
public:
  typedef struct {
    uint32_t tti;
    uint32_t tb_len;
    uint8_t* data; // always a pointer in our case
  } tx_request_t;

  virtual int tx_request(const tx_request_t& request) = 0;
};

class phy_interface_rrc_nr
{};

// Combined interface for PHY to access stack (MAC and RRC)
class stack_interface_phy_nr : public mac_interface_phy_nr,
                               public rrc_interface_phy_nr,
                               public srslte::stack_interface_phy_nr
{};

// Combined interface for stack (MAC and RRC) to access PHY
class phy_interface_stack_nr : public phy_interface_mac_nr, public phy_interface_rrc_nr
{};

} // namespace srsue

#endif // SRSLTE_UE_NR_INTERFACES_H
