/**
 * Copyright 2013-2023 Software Radio Systems Limited
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

#ifndef SRSRAN_UCI_NR_H
#define SRSRAN_UCI_NR_H

#include "srsran/phy/common/phy_common_nr.h"
#include "srsran/phy/fec/crc.h"
#include "srsran/phy/fec/polar/polar_code.h"
#include "srsran/phy/fec/polar/polar_decoder.h"
#include "srsran/phy/fec/polar/polar_encoder.h"
#include "srsran/phy/fec/polar/polar_rm.h"
#include "srsran/phy/phch/phch_cfg_nr.h"
#include "srsran/phy/phch/pucch_cfg_nr.h"
#include "uci_cfg_nr.h"
#include <stdbool.h>
#include <stdint.h>

/**
 * @brief NR-UCI Encoder/decoder initialization arguments
 */
typedef struct {
  bool  disable_simd;         ///< Disable Polar code SIMD
  float block_code_threshold; ///< Set normalised block code threshold (receiver only)
  float one_bit_threshold;    ///< Decode threshold for 1 bit (receiver only)
} srsran_uci_nr_args_t;

typedef struct {
  srsran_polar_rm_t      rm_tx;
  srsran_polar_rm_t      rm_rx;
  srsran_polar_encoder_t encoder;
  srsran_polar_decoder_t decoder;
  srsran_crc_t           crc6;
  srsran_crc_t           crc11;
  srsran_polar_code_t    code;
  uint8_t*               bit_sequence;         ///< UCI bit sequence
  uint8_t*               c;                    ///< UCI code-block prior encoding or after decoding
  uint8_t*               allocated;            ///< Polar code intermediate
  uint8_t*               d;                    ///< Polar code encoded intermediate
  float                  block_code_threshold; ///< Decode threshold for block code (3-11 bits)
  float                  one_bit_threshold;    ///< Decode threshold for 1 bit
} srsran_uci_nr_t;

/**
 * @brief Calculates the number of bits carried by PUCCH formats 2, 3 and 4 from the PUCCH resource
 * @remark Defined in TS 38.212 Table 6.3.1.4-1: Total rate matching output sequence length Etot
 * @param resource PUCCH format 2, 3 or 4 Resource provided by upper layers
 * @return The number of bits if the provided resource is valid, SRSRAN_ERROR code otherwise
 */
SRSRAN_API int srsran_uci_nr_pucch_format_2_3_4_E(const srsran_pucch_nr_resource_t* resource);

/**
 * @brief Calculates in advance how many CRC bits will be appended for a given amount of UCI bits (A)
 * @remark Defined in TS 38.212 section 6.3.1.2 Code block segmentation and CRC attachment
 * @param A Number of UCI bits to transmit
 */
SRSRAN_API uint32_t srsran_uci_nr_crc_len(uint32_t A);

/**
 * @brief Initialises NR-UCI encoder/decoder object
 * @param[in,out] q NR-UCI object
 * @param[in] args Configuration arguments
 * @return SRSRAN_SUCCESS if initialization is successful, SRSRAN_ERROR code otherwise
 */
SRSRAN_API int srsran_uci_nr_init(srsran_uci_nr_t* q, const srsran_uci_nr_args_t* args);

/**
 * @brief Deallocates NR-UCI encoder/decoder object
 * @param[in,out] q NR-UCI object
 */
SRSRAN_API void srsran_uci_nr_free(srsran_uci_nr_t* q);

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
 * @return Number of encoded bits if encoding is successful, SRSRAN_ERROR code otherwise
 */
SRSRAN_API int srsran_uci_nr_encode_pucch(srsran_uci_nr_t*                  q,
                                          const srsran_pucch_nr_resource_t* pucch_resource,
                                          const srsran_uci_cfg_nr_t*        uci_cfg,
                                          const srsran_uci_value_nr_t*      value,
                                          uint8_t*                          o);

/**
 * @brief Decoder UCI bits
 *
 * @attention Compatible only with PUCCH formats 2, 3 and 4
 *
 * @param[in,out] q NR-UCI object
 * @param[in] pucch_resource_cfg
 * @param[in] uci_cfg
 * @param[in] llr
 * @param[out] value
 * @return SRSRAN_SUCCESSFUL if it is successful, SRSRAN_ERROR code otherwise
 */
SRSRAN_API int srsran_uci_nr_decode_pucch(srsran_uci_nr_t*                  q,
                                          const srsran_pucch_nr_resource_t* pucch_resource_cfg,
                                          const srsran_uci_cfg_nr_t*        uci_cfg,
                                          int8_t*                           llr,
                                          srsran_uci_value_nr_t*            value);

/**
 * @brief Calculates total number of encoded bits for HARQ-ACK multiplexing in PUSCH
 * @param[in] cfg PUSCH transmission configuration
 * @return The number of encoded bits if successful, SRSRAN_ERROR code otherwise
 */
SRSRAN_API int srsran_uci_nr_pusch_ack_nof_bits(const srsran_uci_nr_pusch_cfg_t* cfg, uint32_t O_ack);

/**
 * @brief Encodes HARQ-ACK bits for PUSCH transmission
 * @param[in,out] q NR-UCI object
 * @param[in] cfg UCI configuration
 * @param[in] value UCI value
 * @param[out] o_ack Encoded ack bits
 * @return The number of encoded bits if successful, SRSRAN_ERROR code otherwise
 */
SRSRAN_API int srsran_uci_nr_encode_pusch_ack(srsran_uci_nr_t*             q,
                                              const srsran_uci_cfg_nr_t*   cfg,
                                              const srsran_uci_value_nr_t* value,
                                              uint8_t*                     o_ack);

/**
 * @brief Decodes HARQ-ACK bits for PUSCH transmission
 * @param[in,out] q NR-UCI object
 * @param[in] cfg UCI configuration
 * @param[in] llr Provides softbits LLR
 * @param[out] value UCI value
 * @return SRSRAN_SUCCESS if the decoding process was successful, SRSRAN_ERROR code otherwise
 */
SRSRAN_API int srsran_uci_nr_decode_pusch_ack(srsran_uci_nr_t*           q,
                                              const srsran_uci_cfg_nr_t* cfg,
                                              int8_t*                    llr,
                                              srsran_uci_value_nr_t*     value);

/**
 * @brief Calculates total number of encoded bits for CSI part 1 multiplexing in PUSCH
 * @param[in] cfg UCI configuration
 * @return The number of encoded bits if valid, SRSRAN_ERROR code otherwise
 */
SRSRAN_API int srsran_uci_nr_pusch_csi1_nof_bits(const srsran_uci_cfg_nr_t* cfg);

/**
 * @brief Encodes CSI part 1 bits for PUSCH transmission
 * @param[in,out] q NR-UCI object
 * @param[in] cfg UCI configuration
 * @param[in] value UCI value
 * @param[out] o_ack Encoded CSI part 1 bits
 * @return The number of encoded bits if successful, SRSRAN_ERROR code otherwise
 */
SRSRAN_API int srsran_uci_nr_encode_pusch_csi1(srsran_uci_nr_t*             q,
                                               const srsran_uci_cfg_nr_t*   cfg,
                                               const srsran_uci_value_nr_t* value,
                                               uint8_t*                     o);

/**
 * @brief Decodes CSI part 1 bits for PUSCH transmission
 * @param[in,out] q NR-UCI object
 * @param[in] cfg UCI configuration
 * @param[in] llr Provides softbits LLR
 * @param[out] value UCI value
 * @return SRSRAN_SUCCESS if the decoding process was successful, SRSRAN_ERROR code otherwise
 */
SRSRAN_API int srsran_uci_nr_decode_pusch_csi1(srsran_uci_nr_t*           q,
                                               const srsran_uci_cfg_nr_t* cfg,
                                               int8_t*                    llr,
                                               srsran_uci_value_nr_t*     value);

/**
 * @brief Calculates the total number of UCI bits
 * @param uci_cfg UCI configuration
 * @return The number of total bits carrying HARQ ACK, CSI reports and SR bits
 */
SRSRAN_API uint32_t srsran_uci_nr_total_bits(const srsran_uci_cfg_nr_t* uci_cfg);

/**
 * @brief Converts to string an UCI data structure
 * @param uci_data UCO data structure
 * @param str Destination string
 * @param str_len String length
 * @return Resultant string length
 */
SRSRAN_API uint32_t srsran_uci_nr_info(const srsran_uci_data_nr_t* uci_data, char* str, uint32_t str_len);

#endif // SRSRAN_UCI_NR_H
