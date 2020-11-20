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

/******************************************************************************
 *  File:         sch_nr.h
 *
 *  Description:  Common UL and DL shared channel encode/decode functions for NR.
 *
 *  Reference:    3GPP TS 38.212 V15.9.0
 *****************************************************************************/

#ifndef SRSLTE_SCH_NR_H
#define SRSLTE_SCH_NR_H

#include "srslte/config.h"
#include "srslte/phy/common/phy_common_nr.h"
#include "srslte/phy/fec/crc.h"
#include "srslte/phy/fec/ldpc/ldpc_decoder.h"
#include "srslte/phy/fec/ldpc/ldpc_encoder.h"
#include "srslte/phy/fec/ldpc/ldpc_rm.h"
#include "srslte/phy/phch/pdsch_cfg_nr.h"

#define SRSLTE_SCH_NR_MAX_NOF_CB_LDPC                                                                                  \
  ((SRSLTE_SLOT_MAX_NOF_BITS_NR + (SRSLTE_LDPC_BG2_MAX_LEN_CB - 1)) / SRSLTE_LDPC_BG2_MAX_LEN_CB)

typedef struct SRSLTE_API {
  srslte_carrier_nr_t carrier;

  /// Temporal data buffers
  uint8_t* temp_cb;

  /// CRC generators
  srslte_crc_t crc_tb_24;
  srslte_crc_t crc_tb_16;
  srslte_crc_t crc_cb;

  /// LDPC encoders
  srslte_ldpc_encoder_t* encoder_bg1[MAX_LIFTSIZE + 1];
  srslte_ldpc_encoder_t* encoder_bg2[MAX_LIFTSIZE + 1];

  /// LDPC decoders
  srslte_ldpc_decoder_t* decoder_bg1[MAX_LIFTSIZE + 1];
  srslte_ldpc_decoder_t* decoder_bg2[MAX_LIFTSIZE + 1];

  /// LDPC Rate matcher
  srslte_ldpc_rm_t tx_rm;
  srslte_ldpc_rm_t rx_rm;
} srslte_sch_nr_t;

/**
 * @brief SCH encoder and decoder initialization arguments
 */
typedef struct SRSLTE_API {
  bool  disable_simd;
  bool  decoder_use_flooded;
  float decoder_scaling_factor;
} srslte_sch_nr_args_t;

/**
 * @brief Common SCH configuration
 */
typedef struct {
  srslte_basegraph_t     bg;   ///< @brief Base graph
  uint32_t               Qm;   ///< @brief Modulation order
  uint32_t               G;    ///< Number of available bits
  uint32_t               A;    ///< @brief Payload size, TBS
  uint32_t               L_tb; ///< @brief the number of the transport block parity bits (16 or 24 bits)
  uint32_t               L_cb; ///< @brief the number of the code block parity bits (0 or 24 bits)
  uint32_t               B;    ///< @brief the number of bits in the transport block including TB CRC
  uint32_t               Bp;   ///< @brief the number of bits in the transport block including CB and TB CRCs
  uint32_t               Kp;   ///< @brief Number of payload bits of the code block including CB CRC
  uint32_t               Kr;   ///< @brief Number of payload bits of the code block including CB CRC and filler bits
  uint32_t               F;    ///< @brief Number of filler bits
  uint32_t               Nref; ///< @brief N_ref parameter described in TS 38.212 V15.9.0 5.4.2.1
  uint32_t               Z;    ///< @brief LDPC lifting size
  uint32_t               Nl;   ///< @brief Number of transmission layers that the transport block is mapped onto
  bool                   mask[SRSLTE_SCH_NR_MAX_NOF_CB_LDPC]; ///< Indicates what codeblocks shall be encoded/decoded
  uint32_t               C;                                   ///< Number of codeblocks
  uint32_t               Cp;                                  ///< Number of codeblocks that are actually transmitted
  srslte_crc_t*          crc_tb;                              ///< Selected CRC for transport block
  srslte_ldpc_encoder_t* encoder;                             ///< @brief Points to the selected encoder (if valid)
  srslte_ldpc_decoder_t* decoder;                             ///< @brief Points to the selected decoder (if valid)
} srslte_sch_nr_common_cfg_t;

/**
 * @brief Base graph selection from a provided transport block size and target rate
 *
 * @remark Defined for DL-SCH in TS 38.212 V15.9.0 section 7.2.2 LDPC base graph selection
 * @remark Defined for UL-SCH in TS 38.212 V15.9.0 section 6.2.2 LDPC base graph selection
 *
 * @param tbs the payload size as described in Clause 7.2.1 for DL-SCH and 6.2.1 for UL-SCH.
 * @param R Target rate
 * @return it returns the selected base graph
 */
SRSLTE_API srslte_basegraph_t srslte_sch_nr_select_basegraph(uint32_t tbs, double R);

/**
 * @brief Calculates all the parameters required for performing TS 38.212 V15.9.0 5.4 General procedures for LDPC
 * @param sch_cfg Provides higher layers configuration
 * @param tb Provides transport block configuration
 * @param cfg SCH object
 * @return
 */
SRSLTE_API int srslte_dlsch_nr_fill_cfg(srslte_sch_nr_t*            q,
                                        const srslte_sch_cfg_t*     sch_cfg,
                                        const srslte_sch_tb_t*      tb,
                                        srslte_sch_nr_common_cfg_t* cfg);

SRSLTE_API int srslte_sch_nr_init_tx(srslte_sch_nr_t* q, const srslte_sch_nr_args_t* cfg);

SRSLTE_API int srslte_sch_nr_init_rx(srslte_sch_nr_t* q, const srslte_sch_nr_args_t* cfg);

SRSLTE_API int srslte_sch_nr_set_carrier(srslte_sch_nr_t* q, const srslte_carrier_nr_t* carrier);

SRSLTE_API void srslte_sch_nr_free(srslte_sch_nr_t* q);

SRSLTE_API int srslte_dlsch_nr_encode(srslte_sch_nr_t*        q,
                                      const srslte_sch_cfg_t* cfg,
                                      const srslte_sch_tb_t*  tb,
                                      const uint8_t*          data,
                                      uint8_t*                e_bits);

SRSLTE_API int srslte_sch_nr_decoder_set_carrier(srslte_sch_nr_t* q, const srslte_carrier_nr_t* carrier);

SRSLTE_API int srslte_dlsch_nr_decode(srslte_sch_nr_t*        q,
                                      const srslte_sch_cfg_t* sch_cfg,
                                      const srslte_sch_tb_t*  tb,
                                      int8_t*                 e_bits,
                                      uint8_t*                data,
                                      bool*                   crc_ok);

SRSLTE_API int srslte_sch_nr_tb_info(const srslte_sch_tb_t* tb, char* str, uint32_t str_len);

#endif // SRSLTE_SCH_NR_H