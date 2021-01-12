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

#ifndef SRSLTE_UCI_NR_H
#define SRSLTE_UCI_NR_H

#include "srslte/phy/fec/crc.h"
#include "srslte/phy/fec/polar/polar_code.h"
#include "srslte/phy/fec/polar/polar_encoder.h"
#include "srslte/phy/fec/polar/polar_rm.h"
#include "srslte/phy/phch/pucch_nr.h"
#include "uci_cfg.h"
#include "uci_cfg_nr.h"
#include <stdbool.h>
#include <stdint.h>

typedef struct {
  bool disable_simd;
} srslte_uci_nr_args_t;

typedef struct {

} srslte_uci_nr_cfg_t;

typedef struct {
  srslte_polar_rm_t      rm;
  srslte_polar_encoder_t encoder;
  srslte_crc_t           crc6;
  srslte_crc_t           crc11;
  srslte_polar_code_t    code;
  uint8_t*               bit_sequence; ///< UCI bit sequence
  uint8_t*               c;            ///< UCI code-block prior encoding or after decoding
  uint8_t*               allocated;    ///< Polar code intermediate
  uint8_t*               d;            ///< Polar code encoded intermediate
} srslte_uci_nr_t;

/**
 * @brief Initialises NR-UCI encoder/decoder object
 * @param[in,out] q NR-UCI object
 * @param[in] args Configuration arguments
 * @return SRSLTE_SUCCESS if initialization is successful, SRSLTE_ERROR code otherwise
 */
SRSLTE_API int srslte_uci_nr_init(srslte_uci_nr_t* q, const srslte_uci_nr_args_t* args);

/**
 * @brief Deallocates NR-UCI encoder/decoder object
 * @param[in,out] q NR-UCI object
 */
SRSLTE_API void srslte_uci_nr_free(srslte_uci_nr_t* q);

/**
 * @brief Encodes UCI bits
 *
 * @attention Compatible only with PUCCH formats 2, 3 and 4
 *
 * @remark Defined in TS 38.212 section 6.3.1.1
 *
 * @param[in,out] q NR-UCI object
 * @param[in] pucch_cfg Higher layers PUCCH configuration
 * @param[in] uci_cfg UCI configuration
 * @param[in] uci_value UCI values
 * @param[out] o Output encoded bits
 * @return Number of encoded bits if encoding is successful, SRSLTE_ERROR code otherwise
 */
SRSLTE_API int srslte_uci_nr_encode_pucch(srslte_uci_nr_t*             q,
                                          const srslte_pucch_nr_cfg_t* pucch_cfg,
                                          const srslte_uci_cfg_nr_t*   uci_cfg,
                                          const srslte_uci_value_nr_t* value,
                                          uint8_t*                     o);

#endif // SRSLTE_UCI_NR_H
