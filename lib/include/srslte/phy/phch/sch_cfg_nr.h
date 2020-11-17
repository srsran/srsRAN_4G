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

#ifndef SRSLTE_SCH_CFG_NR_H
#define SRSLTE_SCH_CFG_NR_H

#include "srslte/phy/fec/softbuffer.h"

typedef struct SRSLTE_API {
  srslte_mcs_table_t mcs_table; ///< @brief Indicates the MCS table the UE shall use for PDSCH and/or PUSCH without
                                ///< transform precoding

  srslte_xoverhead_t xoverhead; ///< Accounts for overhead from CSI-RS, CORESET, etc. If the field is absent, the UE
                                ///< applies value xOh0 (see TS 38.214 [19], clause 5.1.3.2).
} srslte_sch_cfg_t;

typedef struct SRSLTE_API {
  srslte_mod_t mod;
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
    srslte_softbuffer_tx_t* tx;
    srslte_softbuffer_rx_t* rx;
  } softbuffer;
} srslte_sch_tb_t;

#endif // SRSLTE_SCH_CFG_NR_H
