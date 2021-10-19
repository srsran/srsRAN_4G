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

#ifndef SRSRAN_SCH_CFG_NR_H
#define SRSRAN_SCH_CFG_NR_H

#include "srsran/phy/fec/softbuffer.h"

typedef struct SRSRAN_API {
  srsran_mcs_table_t mcs_table; ///< @brief Indicates the MCS table the UE shall use for PDSCH and/or PUSCH without
                                ///< transform precoding

  srsran_xoverhead_t xoverhead; ///< @brief Accounts for overhead from CSI-RS, CORESET, etc. If the field is absent, the
                                ///< UE applies value xOh0 (see TS 38.214 [19], clause 5.1.3.2).

  bool limited_buffer_rm; ///< @brief Enables LBRM (Limited buffer rate-matching). Given by rateMatching parameter in
                          ///< PUSCH-ServingCellConfig or PDSCH-ServingCellConfig ASN1 sequences
} srsran_sch_cfg_t;

typedef struct SRSRAN_API {
  srsran_mod_t mod;
  uint32_t     N_L;      ///< the number of transmission layers that the transport block is mapped onto
  uint32_t     mcs;      ///< Modulation Code Scheme (MCS) for debug and trace purpose
  int          tbs;      ///< Payload size, TS 38.212 refers to it as A
  double       R;        ///< Target code rate
  double       R_prime;  ///< Actual code rate
  int          rv;       ///< Redundancy version
  int          ndi;      ///< New Data Indicator
  uint32_t     nof_re;   ///< Number of available resource elements to transmit ULSCH (data) and UCI (control)
  uint32_t     nof_bits; ///< Number of available bits to send ULSCH
  uint32_t     cw_idx;
  bool         enabled;

  /// Soft-buffers pointers
  union {
    srsran_softbuffer_tx_t* tx;
    srsran_softbuffer_rx_t* rx;
  } softbuffer;
} srsran_sch_tb_t;

#endif // SRSRAN_SCH_CFG_NR_H
