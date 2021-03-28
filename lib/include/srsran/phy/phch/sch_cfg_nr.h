/**
 * Copyright 2013-2021 Software Radio Systems Limited
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

#ifndef SRSRAN_SCH_CFG_NR_H
#define SRSRAN_SCH_CFG_NR_H

#include "srsran/phy/fec/softbuffer.h"

typedef struct SRSRAN_API {
  srsran_mcs_table_t mcs_table; ///< @brief Indicates the MCS table the UE shall use for PDSCH and/or PUSCH without
                                ///< transform precoding

  srsran_xoverhead_t xoverhead; ///< Accounts for overhead from CSI-RS, CORESET, etc. If the field is absent, the UE
                                ///< applies value xOh0 (see TS 38.214 [19], clause 5.1.3.2).
} srsran_sch_cfg_t;

typedef struct SRSRAN_API {
  srsran_mod_t mod;
  uint32_t     N_L; ///< the number of transmission layers that the transport block is mapped onto
  int          tbs; ///< Payload size, TS 38.212 refers to it as A
  double       R;   ///< Target LDPC rate
  int          rv;
  uint32_t     nof_re;   ///< Number of available resource elements to send, known as N_RE
  uint32_t     nof_bits; ///< Number of available bits to send, known as G
  uint32_t     cw_idx;
  bool         enabled;

  /// Soft-buffers pointers
  union {
    srsran_softbuffer_tx_t* tx;
    srsran_softbuffer_rx_t* rx;
  } softbuffer;
} srsran_sch_tb_t;

#endif // SRSRAN_SCH_CFG_NR_H
