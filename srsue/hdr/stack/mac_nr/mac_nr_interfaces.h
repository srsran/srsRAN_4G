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

#ifndef SRSUE_MAC_NR_INTERFACES_H
#define SRSUE_MAC_NR_INTERFACES_H

#include "srsran/common/interfaces_common.h"

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
};

/**
 * @brief Interface from MAC NR parent class to mux ubclass
 */
class mac_interface_mux_nr
{
public:
  // MUX can query MAC for current C-RNTI for Msg3 transmission
  virtual uint16_t get_crnti() = 0;
};

} // namespace srsue

#endif // SRSUE_MAC_NR_INTERFACES_H