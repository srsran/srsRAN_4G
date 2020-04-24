/*
 * Copyright 2013-2020 Software Radio Systems Limited
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

#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include "prb_dl.h"
#include "srslte/phy/common/phy_common.h"
#include "srslte/phy/phch/dci_nbiot.h"
#include "srslte/phy/phch/npdcch.h"
#include "srslte/phy/utils/bit.h"
#include "srslte/phy/utils/debug.h"
#include "srslte/phy/utils/vector.h"

#define DUMP_SIGNALS 0
#define RE_EXT_DEBUG 0

/** Initializes the NPDCCH transmitter and receiver */
int srslte_npdcch_init(srslte_npdcch_t* q)
{
  int ret = SRSLTE_ERROR_INVALID_INPUTS;

  if (q != NULL) {
    ret = SRSLTE_ERROR;
    bzero(q, sizeof(srslte_npdcch_t));
    q->nof_cce = 2; // One Format1 NPDCCH occupying both NCCEs

    // Allocate memory for the maximum number of NPDCCH bits, i.e. one full PRB
    q->max_bits  = SRSLTE_CP_NORM_SF_NSYMB * SRSLTE_NRE * 2;
    q->ncce_bits = q->max_bits / 2;

    INFO("Init NPDCCH: Max bits: %d, %d ports.\n", q->max_bits, q->cell.nof_ports);

    if (srslte_modem_table_lte(&q->mod, SRSLTE_MOD_QPSK)) {
      goto clean;
    }
    if (srslte_crc_init(&q->crc, SRSLTE_LTE_CRC16, 16)) {
      goto clean;
    }

    int poly[3] = {0x6D, 0x4F, 0x57};
    if (srslte_viterbi_init(&q->decoder, SRSLTE_VITERBI_37, poly, SRSLTE_NBIOT_DCI_MAX_SIZE + 16, true)) {
      goto clean;
    }

    q->e = srslte_vec_u8_malloc(q->max_bits);
    if (!q->e) {
      goto clean;
    }

    for (int i = 0; i < 2; i++) {
      q->llr[i] = srslte_vec_f_malloc(q->max_bits);
      if (!q->llr[i]) {
        goto clean;
      }
      srslte_vec_f_zero(q->llr[i], q->max_bits);
    }

    q->d = srslte_vec_cf_malloc(q->max_bits / 2);
    if (!q->d) {
      goto clean;
    }

    for (uint32_t i = 0; i < SRSLTE_MAX_PORTS; i++) {
      q->ce[i] = srslte_vec_cf_malloc(q->max_bits / 2);
      if (!q->ce[i]) {
        goto clean;
      }
      for (uint32_t k = 0; k < q->max_bits / 2; k++) {
        q->ce[i][k] = 1;
      }
      q->x[i] = srslte_vec_cf_malloc(q->max_bits / 2);
      if (!q->x[i]) {
        goto clean;
      }
      q->symbols[i] = srslte_vec_cf_malloc(q->max_bits / 2);
      if (!q->symbols[i]) {
        goto clean;
      }
      memset(q->symbols[i], 0, sizeof(cf_t) * q->max_bits / 2);
    }

    ret = SRSLTE_SUCCESS;
  }
clean:
  if (ret == SRSLTE_ERROR) {
    srslte_npdcch_free(q);
  }
  return ret;
}

void srslte_npdcch_free(srslte_npdcch_t* q)
{
  if (q->e) {
    free(q->e);
  }

  for (uint32_t i = 0; i < 2; i++) {
    if (q->llr[i]) {
      free(q->llr[i]);
    }
  }

  if (q->d) {
    free(q->d);
  }
  for (uint32_t i = 0; i < SRSLTE_MAX_PORTS; i++) {
    if (q->ce[i]) {
      free(q->ce[i]);
    }
    if (q->x[i]) {
      free(q->x[i]);
    }
    if (q->symbols[i]) {
      free(q->symbols[i]);
    }
  }

  for (uint32_t i = 0; i < SRSLTE_NOF_SF_X_FRAME; i++) {
    srslte_sequence_free(&q->seq[i]);
  }

  srslte_modem_table_free(&q->mod);
  srslte_viterbi_free(&q->decoder);

  bzero(q, sizeof(srslte_npdcch_t));
}

int srslte_npdcch_set_cell(srslte_npdcch_t* q, srslte_nbiot_cell_t cell)
{
  int ret = SRSLTE_ERROR_INVALID_INPUTS;

  if (q != NULL && srslte_nbiot_cell_isvalid(&cell)) {
    ret = SRSLTE_ERROR;

    if (q->cell.n_id_ncell != cell.n_id_ncell || q->cell.base.nof_prb == 0) {
      q->cell = cell;

      if (q->cell.mode == SRSLTE_NBIOT_MODE_INBAND_SAME_PCI || q->cell.mode == SRSLTE_NBIOT_MODE_INBAND_DIFFERENT_PCI) {
        q->i_n_start = 3;
      } else {
        q->i_n_start = 0;
      }

      // TODO: Add case for LTE cell with 4 ports
      if (q->cell.nof_ports == 1) {
        q->nof_nbiot_refs = 2;
      } else {
        q->nof_nbiot_refs = 4;
      }

      if (q->cell.base.nof_ports == 1) {
        q->nof_lte_refs = 2;
      } else {
        q->nof_lte_refs = 4;
      }

      // Update the maximum number of NPDCCH bits, i.e. one PRB minus the starting offset minus the reference symbols
      q->max_bits  = srslte_ra_nbiot_dl_grant_nof_re(q->cell, q->i_n_start) * 2;
      q->ncce_bits = q->max_bits / 2;

      for (int i = 0; i < SRSLTE_NOF_SF_X_FRAME; i++) {
        if (srslte_sequence_npdcch(&q->seq[i], 2 * i, q->cell.n_id_ncell, q->max_bits)) {
          return SRSLTE_ERROR;
        }
      }
    }
    ret = SRSLTE_SUCCESS;
  }
  return ret;
}

int srslte_npdcch_dci_decode(srslte_npdcch_t* q, float* e, uint8_t* data, uint32_t E, uint32_t nof_bits, uint16_t* crc)
{
  uint16_t p_bits, crc_res;
  uint8_t* x;

  if (q != NULL) {
    if (data != NULL && E <= q->max_bits && nof_bits <= SRSLTE_DCI_MAX_BITS) {
      srslte_vec_f_zero(q->rm_f, 3 * (SRSLTE_DCI_MAX_BITS + 16));

      uint32_t coded_len = 3 * (nof_bits + 16);

      // unrate matching
      srslte_rm_conv_rx(e, E, q->rm_f, coded_len);

      // viterbi decoder
      srslte_viterbi_decode_f(&q->decoder, q->rm_f, data, nof_bits + 16);

      x       = &data[nof_bits];
      p_bits  = (uint16_t)srslte_bit_pack(&x, 16);
      crc_res = ((uint16_t)srslte_crc_checksum(&q->crc, data, nof_bits) & 0xffff);

      if (crc) {
        *crc = p_bits ^ crc_res;
      }

      return SRSLTE_SUCCESS;
    } else {
      fprintf(stderr, "Invalid parameters: E: %d, max_bits: %d, nof_bits: %d\n", E, q->max_bits, nof_bits);
      return SRSLTE_ERROR_INVALID_INPUTS;
    }
  } else {
    return SRSLTE_ERROR_INVALID_INPUTS;
  }
}

/** Tries to decode a DCI message from the LLRs stored in the srslte_npdcch_t structure by the function
 * srslte_npdcch_extract_llr(). This function can be called multiple times.
 * The decoded message is stored in msg and the CRC remainder in crc_rem pointer
 *
 */
int srslte_npdcch_decode_msg(srslte_npdcch_t*       q,
                             srslte_dci_msg_t*      msg,
                             srslte_dci_location_t* location,
                             srslte_dci_format_t    format,
                             uint16_t*              crc_rem)
{
  int ret                 = SRSLTE_ERROR_INVALID_INPUTS;
  int num_decoded_symbols = 0;
  if (q != NULL && msg != NULL && srslte_nbiot_dci_location_isvalid(location)) {
    uint32_t nof_bits = (format == SRSLTE_DCI_FORMATN2) ? 15 : 23;
    uint32_t e_bits   = q->ncce_bits * location->L;

    // Get the right softbits for this aggregation level
    float* llr = (location->L == 1) ? q->llr[0] : q->llr[1];
    if (SRSLTE_VERBOSE_ISDEBUG()) {
      DEBUG("LLR:\n");
      srslte_vec_fprint_f(stdout, llr, q->max_bits);
    }

    double mean = 0;
    for (int i = 0; i < e_bits; i++) {
      mean += fabsf(llr[location->ncce * q->ncce_bits + i]);
    }
    mean /= e_bits;
    if (mean > 0.3) {
      ret = srslte_npdcch_dci_decode(q, &llr[location->ncce * q->ncce_bits], msg->payload, e_bits, nof_bits, crc_rem);
      if (ret == SRSLTE_SUCCESS) {
        num_decoded_symbols = e_bits / 2;
        msg->nof_bits       = nof_bits;
        // Check format differentiation
        if (format == SRSLTE_DCI_FORMATN0 || format == SRSLTE_DCI_FORMATN1) {
          msg->format = (msg->payload[0] == 0) ? SRSLTE_DCI_FORMATN0 : SRSLTE_DCI_FORMATN1;
        } else {
          msg->format = format;
        }
      } else {
        fprintf(stderr, "Error calling npdcch_dci_decode\n");
      }
      if (crc_rem) {
        DEBUG("Decoded DCI: nCCE=%d, L=%d, format=%s, msg_len=%d, mean=%f, crc_rem=0x%x\n",
              location->ncce,
              location->L,
              srslte_dci_format_string(msg->format),
              nof_bits,
              mean,
              *crc_rem);
      }
    } else {
      DEBUG("Skipping DCI:  nCCE=%d, L=%d, msg_len=%d, mean=%f\n", location->ncce, location->L, nof_bits, mean);
    }
    ret = SRSLTE_SUCCESS;
  } else {
    fprintf(stderr, "Invalid parameters, location=%d,%d\n", location->ncce, location->L);
  }

  q->num_decoded_symbols = num_decoded_symbols;
  return ret;
}

/** Extracts the LLRs from srslte_dci_location_t location of the subframe and stores them in the srslte_npdcch_t
 * structure. DCI messages can be extracted from this location calling the function srslte_npdcch_decode_msg(). Every
 * time this function is called (with a different location), the last demodulated symbols are overwritten and new
 * messages from other locations can be decoded
 */
int srslte_npdcch_extract_llr(srslte_npdcch_t* q,
                              cf_t*            sf_symbols,
                              cf_t*            ce[SRSLTE_MAX_PORTS],
                              float            noise_estimate,
                              uint32_t         sf_idx)
{
  int ret = SRSLTE_ERROR_INVALID_INPUTS;

  /* Set pointers for layermapping & precoding */
  uint32_t num_symbols, e_bits;
  float*   llr;
  cf_t*    x[SRSLTE_MAX_LAYERS];

  if (q != NULL && sf_idx < 10) {
    ret = SRSLTE_ERROR;

    for (int i = 0; i < SRSLTE_NPDCCH_FORMAT_NITEMS; i++) {
      // set parameters according to NPDCCH format
      switch (i) {
        case SRSLTE_NPDCCH_FORMAT0_LOWER_HALF:
          e_bits = q->ncce_bits;
          llr    = q->llr[0];
          break;
        case SRSLTE_NPDCCH_FORMAT0_UPPER_HALF:
          e_bits = q->ncce_bits;
          llr    = &q->llr[0][q->ncce_bits];
          break;
        case SRSLTE_NPDCCH_FORMAT1:
          e_bits = q->ncce_bits * 2;
          llr    = q->llr[1];
          break;
        default:
          return ret;
      }
      num_symbols = e_bits / 2;

      DEBUG("Extracting LLRs for NPDCCH %s: E: %d, SF: %d\n", srslte_npdcch_format_text[i], e_bits, sf_idx);

      if (i != SRSLTE_NPDCCH_FORMAT0_UPPER_HALF) {
        // don't overwrite lower half LLRs
        srslte_vec_f_zero(llr, q->max_bits);
      }

      // number of layers equals number of ports
      for (int f = 0; f < q->cell.nof_ports; f++) {
        x[f] = q->x[f];
      }
      memset(&x[q->cell.nof_ports], 0, sizeof(cf_t*) * (SRSLTE_MAX_LAYERS - q->cell.nof_ports));

      // extract symbols
      int n = srslte_npdcch_get(q, sf_symbols, q->symbols[0], i);
      if (num_symbols != n) {
        fprintf(stderr, "Expected %d NPDCCH symbols but got %d symbols\n", num_symbols, n);
        return ret;
      }

#if DUMP_SIGNALS
      if (SRSLTE_VERBOSE_ISDEBUG()) {
        DEBUG("SAVED FILE npdcch_rx_mapping_output.bin: NPDCCH after extracting symbols\n");
        srslte_vec_save_file("npdcch_rx_mapping_output.bin", q->symbols[0], n * sizeof(cf_t));
      }
#endif

      // extract channel estimates
      for (int p = 0; p < q->cell.nof_ports; p++) {
        n = srslte_npdcch_get(q, ce[p], q->ce[p], i);
        if (num_symbols != n) {
          fprintf(stderr, "Expected %d NPDCCH symbols but got %d symbols\n", num_symbols, n);
          return ret;
        }
      }

      // in control channels, only diversity is supported
      if (q->cell.nof_ports == 1) {
        // no need for layer demapping
        srslte_predecoding_single(q->symbols[0], q->ce[0], q->d, NULL, num_symbols, 1.0, noise_estimate);
      } else {
        srslte_predecoding_diversity(q->symbols[0], q->ce, x, q->cell.nof_ports, num_symbols, 1.0);
        srslte_layerdemap_diversity(x, q->d, q->cell.nof_ports, num_symbols / q->cell.nof_ports);
      }

#if DUMP_SIGNALS
      if (SRSLTE_VERBOSE_ISDEBUG()) {
        DEBUG("SAVED FILE npdcch_rx_predecode_output.bin: NPDCCH after predecoding symbols\n");
        srslte_vec_save_file("npdcch_rx_predecode_output.bin", q->d, q->num_decoded_symbols * sizeof(cf_t));
      }
#endif

      // demodulate symbols
      srslte_demod_soft_demodulate(SRSLTE_MOD_QPSK, q->d, llr, num_symbols);

      // descramble
      srslte_scrambling_f_offset(&q->seq[sf_idx], llr, 0, e_bits);

#if DUMP_SIGNALS
      if (SRSLTE_VERBOSE_ISDEBUG()) {
        DEBUG("SAVED FILE npdcch_rx_descramble_output.bin: NPDCCH after de-scrambling\n");
        srslte_vec_save_file("npdcch_rx_descramble_output.bin", llr, e_bits);
      }
#endif
    }

    ret = SRSLTE_SUCCESS;
  }
  return ret;
}

static void crc_set_mask_rnti(uint8_t* crc, uint16_t rnti)
{
  uint8_t  mask[16] = {};
  uint8_t* r = mask;

  DEBUG("Mask CRC with RNTI 0x%x\n", rnti);

  srslte_bit_unpack(rnti, &r, 16);
  for (uint32_t i = 0; i < 16; i++) {
    crc[i] = (crc[i] + mask[i]) % 2;
  }
}

void srslte_npdcch_dci_encode_conv(srslte_npdcch_t* q,
                                   uint8_t*         data,
                                   uint32_t         nof_bits,
                                   uint8_t*         coded_data,
                                   uint16_t         rnti)
{
  srslte_convcoder_t encoder;
  int                poly[3] = {0x6D, 0x4F, 0x57};
  encoder.K                  = 7;
  encoder.R                  = 3;
  encoder.tail_biting        = true;
  memcpy(encoder.poly, poly, 3 * sizeof(int));

  srslte_crc_attach(&q->crc, data, nof_bits);
  crc_set_mask_rnti(&data[nof_bits], rnti);

#if DUMP_SIGNALS
  if (SRSLTE_VERBOSE_ISDEBUG()) {
    DEBUG("SAVED FILE npdcch_tx_convcoder_input.bin: NPDCCH before convolution coding\n");
    srslte_vec_save_file("npdcch_tx_convcoder_input.bin", data, nof_bits + 16);
  }
#endif

  srslte_convcoder_encode(&encoder, data, coded_data, nof_bits + 16);
}

/** 36.212 5.3.3.2 to 5.3.3.4
 * TODO: UE transmit antenna selection CRC mask
 */
int srslte_npdcch_dci_encode(srslte_npdcch_t* q,
                             uint8_t*         data,
                             uint8_t*         e,
                             uint32_t         nof_bits,
                             uint32_t         E,
                             uint16_t         rnti)
{
  uint8_t tmp[3 * (SRSLTE_DCI_MAX_BITS + 16)];

  if (q != NULL && data != NULL && e != NULL && nof_bits < SRSLTE_DCI_MAX_BITS && E <= q->max_bits) {
    srslte_npdcch_dci_encode_conv(q, data, nof_bits, tmp, rnti);

    DEBUG("CConv output: ");
    if (SRSLTE_VERBOSE_ISDEBUG()) {
      srslte_vec_fprint_b(stdout, tmp, 3 * (nof_bits + 16));
    }

    srslte_rm_conv_tx(tmp, 3 * (nof_bits + 16), e, E);

    return SRSLTE_SUCCESS;
  } else {
    return SRSLTE_ERROR_INVALID_INPUTS;
  }
}

int srslte_npdcch_encode(srslte_npdcch_t*      q,
                         srslte_dci_msg_t*     msg,
                         srslte_dci_location_t location,
                         uint16_t              rnti,
                         cf_t*                 sf_symbols[SRSLTE_MAX_PORTS],
                         uint32_t              nsubframe)
{
  int ret = SRSLTE_ERROR_INVALID_INPUTS;

  if (q != NULL && sf_symbols != NULL && nsubframe < 10 && srslte_nbiot_dci_location_isvalid(&location)) {
    ret         = SRSLTE_ERROR;
    uint32_t e_bits      = q->nof_cce * q->ncce_bits;
    uint32_t nof_symbols = e_bits / 2;

    if (msg->nof_bits < SRSLTE_DCI_MAX_BITS - 16) {
      DEBUG("Encoding DCI: Nbits: %d, E: %d, nCCE: %d, L: %d, RNTI: 0x%x, sf_idx: %d\n",
            msg->nof_bits,
            e_bits,
            location.ncce,
            location.L,
            rnti,
            nsubframe);

      if (srslte_npdcch_dci_encode(q, msg->payload, q->e, msg->nof_bits, e_bits, rnti) != SRSLTE_SUCCESS) {
        fprintf(stderr, "Error encoding DCI\n");
        return SRSLTE_ERROR;
      }

      // number of layers equals number of ports
      cf_t* x[SRSLTE_MAX_LAYERS] = {NULL};
      for (int i = 0; i < q->cell.nof_ports; i++) {
        x[i] = q->x[i];
      }
      memset(&x[q->cell.nof_ports], 0, sizeof(cf_t*) * (SRSLTE_MAX_LAYERS - q->cell.nof_ports));

#if DUMP_SIGNALS
      if (SRSLTE_VERBOSE_ISDEBUG()) {
        DEBUG("SAVED FILE npdcch_tx_scramble_input.bin: NPDCCH before scrambling\n");
        srslte_vec_save_file("npdcch_tx_scramble_input.bin", q->e, e_bits);
      }
#endif

      srslte_scrambling_b_offset(&q->seq[nsubframe], q->e, 72 * location.ncce, e_bits);

#if DUMP_SIGNALS
      if (SRSLTE_VERBOSE_ISDEBUG()) {
        DEBUG("SAVED FILE npdcch_tx_mod_input.bin: NPDCCH before modulation\n");
        srslte_vec_save_file("npdcch_tx_mod_input.bin", q->e, e_bits);
      }
#endif

      srslte_mod_modulate(&q->mod, q->e, q->d, e_bits);

#if DUMP_SIGNALS
      if (SRSLTE_VERBOSE_ISDEBUG()) {
        DEBUG("SAVED FILE npdcch_tx_precode_input.bin: NPDCCH before precoding symbols\n");
        srslte_vec_save_file("npdcch_tx_precode_input.bin", q->d, nof_symbols * sizeof(cf_t));
      }
#endif

      // layer mapping & precoding
      if (q->cell.nof_ports > 1) {
        srslte_layermap_diversity(q->d, x, q->cell.nof_ports, nof_symbols);
        srslte_precoding_diversity(x, q->symbols, q->cell.nof_ports, nof_symbols / q->cell.nof_ports, 1.0);
      } else {
        memcpy(q->symbols[0], q->d, nof_symbols * sizeof(cf_t));
      }

#if DUMP_SIGNALS
      if (SRSLTE_VERBOSE_ISDEBUG()) {
        DEBUG("SAVED FILE npdcch_tx_mapping_input.bin: NPDCCH before mapping to resource elements\n");
        srslte_vec_save_file("npdcch_tx_mapping_input.bin", q->symbols[0], nof_symbols * sizeof(cf_t));
      }
#endif

      // mapping to resource elements
      for (int i = 0; i < q->cell.nof_ports; i++) {
        srslte_npdcch_put(q, q->symbols[i], sf_symbols[i], SRSLTE_NPDCCH_FORMAT1);
      }

      ret = SRSLTE_SUCCESS;
    } else {
      fprintf(stderr, "Illegal DCI message nCCE: %d, L: %d, nof_cce: %d\n", location.ncce, location.L, q->nof_cce);
    }
  } else {
    fprintf(stderr, "Invalid parameters: L=%d, nCCE=%d\n", location.L, location.ncce);
  }
  return ret;
}

/** 36.213 v9.1.1
 * Computes up to max_candidates UE-specific candidates for DCI messages and saves them
 * in the structure pointed by c.
 * Returns the number of candidates saved in the array c.
 */
uint32_t srslte_npdcch_ue_locations(srslte_dci_location_t* c, uint32_t max_candidates)
{
  // NPDCCH format 1 takes both NCCE
  c[0].L    = 2;
  c[0].ncce = 0;

  // NPDCCH format 0 only takes one NCCE so two of them may be transmitted in one subframe
  c[1].L    = 1;
  c[1].ncce = 0;

  c[2].L    = 1;
  c[2].ncce = 1;

  return max_candidates;
}

uint32_t srslte_npdcch_common_locations(srslte_dci_location_t* c, uint32_t max_candidates)
{
  return srslte_npdcch_ue_locations(c, max_candidates);
}

int srslte_npdcch_cp(srslte_npdcch_t* q, cf_t* input, cf_t* output, bool put, srslte_npdcch_format_t format)
{
  // sanity check
  if (q == NULL || input == NULL || output == NULL) {
    return SRSLTE_ERROR_INVALID_INPUTS;
  }

#if RE_EXT_DEBUG
  int num_extracted = 0;
#endif

  cf_t *in_ptr = input, *out_ptr = output;
  bool  skip_crs = false;

  if (put) {
    out_ptr += (q->i_n_start * q->cell.base.nof_prb * SRSLTE_NRE) + q->cell.nbiot_prb * SRSLTE_NRE;
  } else {
    in_ptr += (q->i_n_start * q->cell.base.nof_prb * SRSLTE_NRE) + q->cell.nbiot_prb * SRSLTE_NRE;
  }

  if (q->cell.mode == SRSLTE_NBIOT_MODE_INBAND_SAME_PCI || q->cell.mode == SRSLTE_NBIOT_MODE_INBAND_DIFFERENT_PCI) {
    skip_crs = true;
  }

  // start mapping at specified OFDM symbol
  for (int l = q->i_n_start; l < SRSLTE_CP_NORM_SF_NSYMB; l++) {
    uint32_t delta  = (q->cell.base.nof_prb - 1) * SRSLTE_NRE; // the number of REs skipped in each OFDM symbol
    uint32_t offset = 0; // the number of REs left out before start of the REF signal RE
    if (l == 5 || l == 6 || l == 12 || l == 13) {
      // always skip NRS
      if (q->nof_nbiot_refs == 2) {
        if (l == 5 || l == 12) {
          offset = q->cell.n_id_ncell % 6;
          delta  = q->cell.n_id_ncell % 6 == 5 ? 1 : 0;
        } else {
          offset = (q->cell.n_id_ncell + 3) % 6;
          delta  = (q->cell.n_id_ncell + 3) % 6 == 5 ? 1 : 0;
        }
      } else if (q->nof_nbiot_refs == 4) {
        offset = q->cell.n_id_ncell % 3;
        delta  = (q->cell.n_id_ncell + ((q->cell.n_id_ncell >= 5) ? 0 : 3)) % 6 == 5 ? 1 : 0;
      } else {
        // TODO: not handled right now
        return SRSLTE_ERROR;
      }

      switch (format) {
        case SRSLTE_NPDCCH_FORMAT0_LOWER_HALF:
          prb_cp_ref(&in_ptr, &out_ptr, offset, q->nof_nbiot_refs, q->nof_nbiot_refs, put);
          // we have copied too much, rewind ptr
          if (put) {
            in_ptr -= (SRSLTE_NRE - q->nof_nbiot_refs) / 2;
          } else {
            out_ptr -= (SRSLTE_NRE - q->nof_nbiot_refs) / 2;
          }
          break;
        case SRSLTE_NPDCCH_FORMAT0_UPPER_HALF:
          // TODO: this causes valgrind to detect an invalid memory access
#if 0
          // skip lower half
          if (put) {
            out_ptr += SRSLTE_NRE / 2;
          } else {
            in_ptr += SRSLTE_NRE / 2;
          }
#endif
          // copy REs
          prb_cp_ref(&in_ptr, &out_ptr, offset, q->nof_nbiot_refs, q->nof_nbiot_refs, put);
          // we have copied too much, rewind ptr
          if (put) {
            in_ptr -= (SRSLTE_NRE - q->nof_nbiot_refs) / 2;
          } else {
            out_ptr -= (SRSLTE_NRE - q->nof_nbiot_refs) / 2;
          }
          break;
        case SRSLTE_NPDCCH_FORMAT1:
          prb_cp_ref(&in_ptr, &out_ptr, offset, q->nof_nbiot_refs, q->nof_nbiot_refs, put);
          break;
        default:
          printf("Wrong NPDCCH format!\n");
          return SRSLTE_ERROR;
      }
    } else if ((l == 0 || l == 4 || l == 7 || l == 11) && skip_crs) {
      // skip LTE's CRS (TODO: use base cell ID?)
      if (q->nof_lte_refs == 2) {
        if (l == 0 || l == 7) {
          offset = q->cell.base.id % 6;
          delta  = (q->cell.base.id + 3) % 6 == 2 ? 1 : 0;
        } else if (l == 4 || l == 11) {
          offset = (q->cell.base.id + 3) % 6;
          delta  = (q->cell.base.id + ((q->cell.base.id <= 5) ? 3 : 0)) % 6 == 5 ? 1 : 0;
        }
      } else {
        offset = q->cell.base.id % 3;
        delta  = q->cell.base.id % 3 == 2 ? 1 : 0;
      }

      switch (format) {
        case SRSLTE_NPDCCH_FORMAT0_LOWER_HALF:
          prb_cp_ref(&in_ptr, &out_ptr, offset, q->nof_lte_refs, q->nof_lte_refs, put);
          // we have copied too much, rewind ptr
          if (put) {
            in_ptr -= (SRSLTE_NRE - q->nof_lte_refs) / 2;
          } else {
            out_ptr -= (SRSLTE_NRE - q->nof_lte_refs) / 2;
          }
          break;
        case SRSLTE_NPDCCH_FORMAT0_UPPER_HALF:
          // skip lower half
          if (put) {
            out_ptr += SRSLTE_NRE / 2;
          } else {
            in_ptr += SRSLTE_NRE / 2;
          }
          // copy REs
          prb_cp_ref(&in_ptr, &out_ptr, offset, q->nof_lte_refs, q->nof_lte_refs, put);
          // we have copied too much, rewind ptr
          if (put) {
            in_ptr -= (SRSLTE_NRE - q->nof_lte_refs) / 2;
          } else {
            out_ptr -= (SRSLTE_NRE - q->nof_lte_refs) / 2;
          }
          break;
        case SRSLTE_NPDCCH_FORMAT1:
          prb_cp_ref(&in_ptr, &out_ptr, offset, q->nof_lte_refs, q->nof_lte_refs, put);
          break;
        default:
          printf("Wrong NPDCCH format!\n");
          return SRSLTE_ERROR;
      }
    } else {
      switch (format) {
        case SRSLTE_NPDCCH_FORMAT0_LOWER_HALF:
          prb_cp_half(&in_ptr, &out_ptr, 1);
          // skip upper half
          if (put) {
            out_ptr += SRSLTE_NRE / 2;
          } else {
            in_ptr += SRSLTE_NRE / 2;
          }
          break;
        case SRSLTE_NPDCCH_FORMAT0_UPPER_HALF:
          // skip lower half
          if (put) {
            out_ptr += SRSLTE_NRE / 2;
          } else {
            in_ptr += SRSLTE_NRE / 2;
          }
          prb_cp_half(&in_ptr, &out_ptr, 1);
          break;
        case SRSLTE_NPDCCH_FORMAT1:
          // occupy entire symbol
          prb_cp(&in_ptr, &out_ptr, 1);
          break;
        default:
          printf("Wrong NPDCCH format!\n");
          return SRSLTE_ERROR;
      }
    }

    if (put) {
      out_ptr += delta;
    } else {
      in_ptr += delta;
    }

#if RE_EXT_DEBUG
    printf("\nl=%d, delta=%d offset=%d\n", l, delta, offset);
    uint32_t num_extracted_this_sym = abs((int)(output - out_ptr)) - num_extracted;
    printf("  - extracted total of %d RE after symbol %d (this symbol=%d)\n",
           abs((int)(output - out_ptr)),
           l,
           num_extracted_this_sym);
    srslte_vec_fprint_c(stdout, &output[num_extracted], num_extracted_this_sym);
    num_extracted = abs((int)(output - out_ptr));
#endif
  }

  int r;
  if (put) {
    r = abs((int)(input - in_ptr));
  } else {
    r = abs((int)(output - out_ptr));
  }

  return r;
}

/**
 * Puts NPDCCH in the subframe
 *
 * Returns the number of symbols written to sf_symbols
 *
 * 36.211 10.3 section 6.3.5
 */
int srslte_npdcch_put(srslte_npdcch_t* q, cf_t* symbols, cf_t* sf_symbols, srslte_npdcch_format_t format)
{
  return srslte_npdcch_cp(q, symbols, sf_symbols, true, format);
}

/**
 * Extracts NPDCCH from the subframe
 *
 * Returns the number of symbols read
 *
 * 36.211 10.3 section 6.3.5
 */
int srslte_npdcch_get(srslte_npdcch_t* q, cf_t* sf_symbols, cf_t* symbols, srslte_npdcch_format_t format)
{
  return srslte_npdcch_cp(q, sf_symbols, symbols, false, format);
}
