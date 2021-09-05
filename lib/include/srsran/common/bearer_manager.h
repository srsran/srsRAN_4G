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

#ifndef SRSRAN_BEARER_MANAGER_H
#define SRSRAN_BEARER_MANAGER_H

#include "srsran/common/common.h"
#include "srsran/common/rwlock_guard.h"
#include "srsran/srslog/srslog.h"
#include <map>
#include <stdint.h>

namespace srsran {

/**
 * @brief Helper class to manage the mapping between EPS bearer and radio bearer
 *
 * The class maps EPS bearers that are known to NAS and GW (UE) or GTPU (eNB)
 * to radio bearer (RB) that are only known to RRC.
 * Since the lifetime of a EPS bearer is usually longer than the lifetime of a RB,
 * the GW/GTPU needs to query the Stack to check whether a
 * given EPS bearer is active, i.e. a DRB is established, or not.
 *
 * The class also maps between RATs since each LCID can exist on either EUTRA or NR RATs, or both.
 *
 * Since the access of this class is happening from two different threads (GW+RRC/Stack)
 * it's public interface is protected.
 *
 * The class provides two interfaces to be used with RNTI or without. The version without
 * RNTI is used by the UE. The version with RNTI in the interface is intented to be
 * used by the eNB.
 *
 */
class bearer_manager
{
public:
  bearer_manager();
  ~bearer_manager();

  struct radio_bearer_t {
    srsran::srsran_rat_t rat;
    uint32_t             lcid;
    uint32_t             eps_bearer_id;
    bool                 is_valid() const { return rat != srsran_rat_t::nulltype; }
  };

  /// Single user interface (for UE)

  // RRC interface
  /// Registers EPS bearer with PDCP RAT type and LCID
  void add_eps_bearer(uint8_t eps_bearer_id, srsran::srsran_rat_t rat, uint32_t lcid);

  /// Single EPS bearer is removed from map when the associated DRB is deleted
  void remove_eps_bearer(uint8_t eps_bearer_id);

  /// All registered bearer are removed (e.g. after connection release)
  void reset();

  // GW interface
  bool has_active_radio_bearer(uint32_t eps_bearer_id);

  // Stack interface to retrieve active RB
  radio_bearer_t get_radio_bearer(uint32_t eps_bearer_id);

  radio_bearer_t get_lcid_bearer(uint16_t rnti, uint32_t lcid);

  /// Multi-user interface (see comments above)
  void           add_eps_bearer(uint16_t rnti, uint8_t eps_bearer_id, srsran::srsran_rat_t rat, uint32_t lcid);
  void           remove_eps_bearer(uint16_t rnti, uint8_t eps_bearer_id);
  void           reset(uint16_t rnti);
  bool           has_active_radio_bearer(uint16_t rnti, uint32_t eps_bearer_id);
  radio_bearer_t get_radio_bearer(uint16_t rnti, uint32_t eps_bearer_id);

private:
  pthread_rwlock_t      rwlock = {}; /// RW lock to protect access from RRC/GW threads
  srslog::basic_logger& logger;

  typedef std::map<uint32_t, radio_bearer_t> eps_rb_map_t;
  struct user_bearers {
    eps_rb_map_t                 bearers;
    std::map<uint32_t, uint32_t> lcid_to_eps_bearer_id;
  };
  std::map<uint16_t, user_bearers> users_map;

  const uint16_t default_key = 0xffff; // dummy RNTI used for public interface without explicit RNTI
  radio_bearer_t invalid_rb  = {srsran::srsran_rat_t::nulltype, 0, 0};
};

} // namespace srsran

#endif // SRSRAN_BEARER_MANAGER_H