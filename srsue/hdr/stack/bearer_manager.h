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

#ifndef SRSUE_BEARER_MANAGER_H
#define SRSUE_BEARER_MANAGER_H

#include "srsran/common/common.h"
#include "srsran/common/rwlock_guard.h"
#include "srsran/srslog/srslog.h"
#include <map>
#include <stdint.h>

namespace srsue {

/**
 * @brief Helper class to manage the mapping between EPS bearer and radio bearer
 *
 * The class maps EPS bearers that are known to NAS and GW and radio bearer (RB) that
 * are only known to RRC. Since the lifetime of a EPS bearer is usually longer
 * than the lifetime of a RB, the GW needs to query the Stack to check whether a
 * given EPS bearer is active, i.e. a DRB is established, or not.
 *
 * The class also maps between RATs since each LCID can exist on either EUTRA or NR RATs, or both.
 *
 * Since the access of this class is happening from two different threads (GW+RRC/Stack)
 * it's public interface is protected.
 *
 */
class bearer_manager
{
public:
  bearer_manager();
  ~bearer_manager();

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
  struct radio_bearer_t {
    srsran::srsran_rat_t rat;
    uint32_t             lcid;
  };
  radio_bearer_t& get_radio_bearer(uint32_t eps_bearer_id);

private:
  pthread_rwlock_t                   rwlock = {}; /// RW lock to protect access from RRC/GW threads
  srslog::basic_logger&              logger;
  std::map<uint32_t, radio_bearer_t> eps_rb_map;
  radio_bearer_t                     invalid_rb = {srsran::srsran_rat_t::nulltype, 0};
};

} // namespace srsue

#endif // SRSUE_BEARER_MANAGER_H