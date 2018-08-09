/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2015 Software Radio Systems Limited
 *
 * \section LICENSE
 *
 * This file is part of the srsUE library.
 *
 * srsUE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsUE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#ifndef SRSLTE_RLC_ENTITY_H
#define SRSLTE_RLC_ENTITY_H

#include "srslte/common/log.h"
#include "srslte/common/common.h"
#include "srslte/interfaces/ue_interfaces.h"
#include "srslte/upper/rlc_common.h"
#include "srslte/upper/rlc_tm.h"
#include "srslte/upper/rlc_um.h"
#include "srslte/upper/rlc_am.h"

namespace srslte {



/****************************************************************************
 * RLC Entity
 * Common container for all RLC entities
 ***************************************************************************/
class rlc_entity
{
public:
  rlc_entity();
  void init(rlc_mode_t                  mode,
            log                        *rlc_entity_log_,
            uint32_t                    lcid_,
            srsue::pdcp_interface_rlc  *pdcp_,
            srsue::rrc_interface_rlc   *rrc_,
            mac_interface_timers       *mac_timers_,
            int                         buffer_size = -1); // use -1 for default buffer sizes

  void configure(srslte_rlc_config_t cnfg);
  void reestablish();
  void stop();
  void empty_queue();
  bool active();

  rlc_mode_t    get_mode();
  uint32_t      get_bearer();

  // PDCP interface
  void write_sdu(byte_buffer_t *sdu);
  void write_sdu_nb(byte_buffer_t *sdu);

  // MAC interface
  uint32_t get_buffer_state();
  uint32_t get_total_buffer_state();
  int      read_pdu(uint8_t *payload, uint32_t nof_bytes);
  void     write_pdu(uint8_t *payload, uint32_t nof_bytes);
  

private:
  rlc_mode_t mode;
  uint32_t   lcid;
  rlc_common *rlc;
};

} // namespace srslte


#endif // SRSLTE_RLC_ENTITY_H
