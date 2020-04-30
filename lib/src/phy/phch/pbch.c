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

#include "srslte/srslte.h"
#include <assert.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include "prb_dl.h"
#include "srslte/phy/common/phy_common.h"
#include "srslte/phy/phch/pbch.h"
#include "srslte/phy/utils/bit.h"
#include "srslte/phy/utils/debug.h"
#include "srslte/phy/utils/vector.h"

#define PBCH_RE_CP_NORM 240
#define PBCH_RE_CP_EXT 216

const uint8_t srslte_crc_mask[4][16] = {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                                        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
                                        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                                        {0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1}};

bool srslte_pbch_exists(int nframe, int nslot)
{
  return (!(nframe % 5) && nslot == 1);
}

cf_t* offset_original;

int srslte_pbch_cp(cf_t* input, cf_t* output, srslte_cell_t cell, bool put)
{
  int   i;
  cf_t* ptr;

  offset_original = input;

  if (put) {
    ptr = input;
    output += cell.nof_prb * SRSLTE_NRE / 2 - 36;
  } else {
    ptr = output;
    input += cell.nof_prb * SRSLTE_NRE / 2 - 36;
  }

  /* symbol 0 & 1 */
  for (i = 0; i < 2; i++) {
    prb_cp_ref(&input, &output, cell.id % 3, 4, 4 * 6, put);
    if (put) {
      output += cell.nof_prb * SRSLTE_NRE - 2 * 36 + (cell.id % 3 == 2 ? 1 : 0);
    } else {
      input += cell.nof_prb * SRSLTE_NRE - 2 * 36 + (cell.id % 3 == 2 ? 1 : 0);
    }
  }
  /* symbols 2 & 3 */
  if (SRSLTE_CP_ISNORM(cell.cp)) {
    for (i = 0; i < 2; i++) {
      prb_cp(&input, &output, 6);
      if (put) {
        output += cell.nof_prb * SRSLTE_NRE - 2 * 36;
      } else {
        input += cell.nof_prb * SRSLTE_NRE - 2 * 36;
      }
    }
  } else {
    prb_cp(&input, &output, 6);
    if (put) {
      output += cell.nof_prb * SRSLTE_NRE - 2 * 36;
    } else {
      input += cell.nof_prb * SRSLTE_NRE - 2 * 36;
    }
    prb_cp_ref(&input, &output, cell.id % 3, 4, 4 * 6, put);
  }
  if (put) {
    return input - ptr;
  } else {
    return output - ptr;
  }
}

/**
 * Puts PBCH in slot number 1
 *
 * Returns the number of symbols written to slot1_data
 *
 * 36.211 10.3 section 6.6.4
 *
 * @param[in] pbch PBCH complex symbols to place in slot1_data
 * @param[out] slot1_data Complex symbol buffer for slot1
 * @param[in] cell Cell configuration
 */
int srslte_pbch_put(cf_t* pbch, cf_t* slot1_data, srslte_cell_t cell)
{
  return srslte_pbch_cp(pbch, slot1_data, cell, true);
}

/**
 * Extracts PBCH from slot number 1
 *
 * Returns the number of symbols written to pbch
 *
 * 36.211 10.3 section 6.6.4
 *
 * @param[in] slot1_data Complex symbols for slot1
 * @param[out] pbch Extracted complex PBCH symbols
 * @param[in] cell Cell configuration
 */
int srslte_pbch_get(cf_t* slot1_data, cf_t* pbch, srslte_cell_t cell)
{
  return srslte_pbch_cp(slot1_data, pbch, cell, false);
}

/** Initializes the PBCH transmitter and receiver.
 * At the receiver, the field nof_ports in the cell structure indicates the
 * maximum number of BS transmitter ports to look for.
 */
int srslte_pbch_init(srslte_pbch_t* q)
{
  int ret = SRSLTE_ERROR_INVALID_INPUTS;

  if (q != NULL) {
    ret = SRSLTE_ERROR;

    bzero(q, sizeof(srslte_pbch_t));

    if (srslte_modem_table_lte(&q->mod, SRSLTE_MOD_QPSK)) {
      goto clean;
    }
    int poly[3] = {0x6D, 0x4F, 0x57};
    if (srslte_viterbi_init(&q->decoder, SRSLTE_VITERBI_37, poly, 40, true)) {
      goto clean;
    }
    if (srslte_crc_init(&q->crc, SRSLTE_LTE_CRC16, 16)) {
      goto clean;
    }
    q->encoder.K           = 7;
    q->encoder.R           = 3;
    q->encoder.tail_biting = true;
    memcpy(q->encoder.poly, poly, 3 * sizeof(int));

    q->nof_symbols = PBCH_RE_CP_NORM;

    q->d = srslte_vec_cf_malloc(q->nof_symbols);
    if (!q->d) {
      goto clean;
    }
    int i;
    for (i = 0; i < SRSLTE_MAX_PORTS; i++) {
      q->ce[i] = srslte_vec_cf_malloc(q->nof_symbols);
      if (!q->ce[i]) {
        goto clean;
      }
      q->x[i] = srslte_vec_cf_malloc(q->nof_symbols);
      if (!q->x[i]) {
        goto clean;
      }
      q->symbols[i] = srslte_vec_cf_malloc(q->nof_symbols);
      if (!q->symbols[i]) {
        goto clean;
      }
    }
    q->llr = srslte_vec_f_malloc(q->nof_symbols * 4 * 2);
    if (!q->llr) {
      goto clean;
    }
    q->temp = srslte_vec_f_malloc(q->nof_symbols * 4 * 2);
    if (!q->temp) {
      goto clean;
    }
    q->rm_b = srslte_vec_u8_malloc(q->nof_symbols * 4 * 2);
    if (!q->rm_b) {
      goto clean;
    }

    ret = SRSLTE_SUCCESS;
  }
clean:
  if (ret == SRSLTE_ERROR) {
    srslte_pbch_free(q);
  }
  return ret;
}

void srslte_pbch_free(srslte_pbch_t* q)
{
  srslte_sequence_free(&q->seq);
  srslte_modem_table_free(&q->mod);
  srslte_viterbi_free(&q->decoder);
  int i;
  for (i = 0; i < SRSLTE_MAX_PORTS; i++) {
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
  if (q->llr) {
    free(q->llr);
  }
  if (q->temp) {
    free(q->temp);
  }
  if (q->rm_b) {
    free(q->rm_b);
  }
  if (q->d) {
    free(q->d);
  }
  bzero(q, sizeof(srslte_pbch_t));
}

int srslte_pbch_set_cell(srslte_pbch_t* q, srslte_cell_t cell)
{
  int ret = SRSLTE_ERROR_INVALID_INPUTS;

  if (q != NULL && srslte_cell_isvalid(&cell)) {
    if (cell.nof_ports == 0) {
      q->search_all_ports = true;
      cell.nof_ports      = SRSLTE_MAX_PORTS;
    } else {
      q->search_all_ports = false;
    }

    if (q->cell.id != cell.id || q->cell.nof_prb == 0) {
      q->cell = cell;
      if (srslte_sequence_pbch(&q->seq, q->cell.cp, q->cell.id)) {
        return SRSLTE_ERROR;
      }
    }
    q->nof_symbols = (SRSLTE_CP_ISNORM(q->cell.cp)) ? PBCH_RE_CP_NORM : PBCH_RE_CP_EXT;

    ret = SRSLTE_SUCCESS;
  }
  return ret;
}

/**
 * Unpacks MIB from PBCH message.
 *
 * @param[in] msg PBCH in an unpacked bit array of size 24
 * @param[out] sfn System frame number
 * @param[out] cell MIB information about PHICH and system bandwidth will be saved here
 */
void srslte_pbch_mib_unpack(uint8_t* msg, srslte_cell_t* cell, uint32_t* sfn)
{
  int phich_res;

  uint32_t bw_idx = srslte_bit_pack(&msg, 3);
  switch (bw_idx) {
    case 0:
      cell->nof_prb = 6;
      break;
    case 1:
      cell->nof_prb = 15;
      break;
    default:
      cell->nof_prb = (bw_idx - 1) * 25;
      break;
  }
  if (*msg) {
    cell->phich_length = SRSLTE_PHICH_EXT;
  } else {
    cell->phich_length = SRSLTE_PHICH_NORM;
  }
  msg++;

  phich_res = srslte_bit_pack(&msg, 2);
  switch (phich_res) {
    case 0:
      cell->phich_resources = SRSLTE_PHICH_R_1_6;
      break;
    case 1:
      cell->phich_resources = SRSLTE_PHICH_R_1_2;
      break;
    case 2:
      cell->phich_resources = SRSLTE_PHICH_R_1;
      break;
    case 3:
      cell->phich_resources = SRSLTE_PHICH_R_2;
      break;
  }
  if (sfn) {
    *sfn = srslte_bit_pack(&msg, 8) << 2;
  }
}

/**
 * Packs MIB to PBCH message.
 *
 * @param[out] payload Output unpacked bit array of size 24
 * @param[in] sfn System frame number
 * @param[in] cell Cell configuration to be encoded in MIB
 */
void srslte_pbch_mib_pack(srslte_cell_t* cell, uint32_t sfn, uint8_t* payload)
{
  int bw, phich_res = 0;

  uint8_t* msg = payload;

  bzero(msg, 24);

  if (cell->nof_prb <= 6) {
    bw = 0;
  } else if (cell->nof_prb <= 15) {
    bw = 1;
  } else {
    bw = 1 + cell->nof_prb / 25;
  }
  srslte_bit_unpack(bw, &msg, 3);

  *msg = cell->phich_length == SRSLTE_PHICH_EXT;
  msg++;

  switch (cell->phich_resources) {
    case SRSLTE_PHICH_R_1_6:
      phich_res = 0;
      break;
    case SRSLTE_PHICH_R_1_2:
      phich_res = 1;
      break;
    case SRSLTE_PHICH_R_1:
      phich_res = 2;
      break;
    case SRSLTE_PHICH_R_2:
      phich_res = 3;
      break;
  }
  srslte_bit_unpack(phich_res, &msg, 2);
  srslte_bit_unpack(sfn >> 2, &msg, 8);
}

void srslte_pbch_decode_reset(srslte_pbch_t* q)
{
  q->frame_idx = 0;
}

void srslte_crc_set_mask(uint8_t* data, int nof_ports)
{
  int i;
  for (i = 0; i < 16; i++) {
    data[SRSLTE_BCH_PAYLOAD_LEN + i] = (data[SRSLTE_BCH_PAYLOAD_LEN + i] + srslte_crc_mask[nof_ports - 1][i]) % 2;
  }
}

/* Checks CRC after applying the mask for the given number of ports.
 *
 * The bits buffer size must be at least 40 bytes.
 *
 * Returns 0 if the data is correct, -1 otherwise
 */
uint32_t srslte_pbch_crc_check(srslte_pbch_t* q, uint8_t* bits, uint32_t nof_ports)
{
  uint8_t data[SRSLTE_BCH_PAYLOADCRC_LEN];
  memcpy(data, bits, SRSLTE_BCH_PAYLOADCRC_LEN * sizeof(uint8_t));
  srslte_crc_set_mask(data, nof_ports);
  int ret = srslte_crc_checksum(&q->crc, data, SRSLTE_BCH_PAYLOADCRC_LEN);
  if (ret == 0) {
    uint32_t chkzeros = 0;
    for (int i = 0; i < SRSLTE_BCH_PAYLOAD_LEN; i++) {
      chkzeros += data[i];
    }
    if (chkzeros) {
      return 0;
    } else {
      return SRSLTE_ERROR;
    }
  } else {
    return ret;
  }
}

int decode_frame(srslte_pbch_t* q, uint32_t src, uint32_t dst, uint32_t n, uint32_t nof_bits, uint32_t nof_ports)
{
  int j;

  if (dst + n <= 4 && src + n <= 4) {
    srslte_vec_f_copy(&q->temp[dst * nof_bits], &q->llr[src * nof_bits], n * nof_bits);

    /* descramble */
    srslte_scrambling_f_offset(&q->seq, &q->temp[dst * nof_bits], dst * nof_bits, n * nof_bits);

    for (j = 0; j < dst * nof_bits; j++) {
      q->temp[j] = SRSLTE_RX_NULL;
    }
    for (j = (dst + n) * nof_bits; j < 4 * nof_bits; j++) {
      q->temp[j] = SRSLTE_RX_NULL;
    }

    /* unrate matching */
    srslte_rm_conv_rx(q->temp, 4 * nof_bits, q->rm_f, SRSLTE_BCH_ENCODED_LEN);

    /* Normalize LLR */
    srslte_vec_sc_prod_fff(q->rm_f, 1.0 / ((float)2 * n), q->rm_f, SRSLTE_BCH_ENCODED_LEN);

    /* decode */
    srslte_viterbi_decode_f(&q->decoder, q->rm_f, q->data, SRSLTE_BCH_PAYLOADCRC_LEN);

    if (!srslte_pbch_crc_check(q, q->data, nof_ports)) {
      return 1;
    } else {
      return SRSLTE_SUCCESS;
    }
  } else {
    ERROR("Error in PBCH decoder: Invalid frame pointers dst=%d, src=%d, n=%d\n", src, dst, n);
    return -1;
  }
}

/* Decodes the PBCH channel
 *
 * The PBCH spans in 40 ms. This function is called every 10 ms. It tries to decode the MIB
 * given the symbols of a subframe (1 ms). Successive calls will use more subframes
 * to help the decoding process.
 *
 * Returns 1 if successfully decoded MIB, 0 if not and -1 on error
 */
int srslte_pbch_decode(srslte_pbch_t*         q,
                       srslte_chest_dl_res_t* channel,
                       cf_t*                  sf_symbols[SRSLTE_MAX_PORTS],
                       uint8_t                bch_payload[SRSLTE_BCH_PAYLOAD_LEN],
                       uint32_t*              nof_tx_ports,
                       int*                   sfn_offset)
{
  uint32_t src, dst, nb;
  uint32_t nant;
  int      i;
  int      nof_bits;
  cf_t*    x[SRSLTE_MAX_LAYERS];

  int ret = SRSLTE_ERROR_INVALID_INPUTS;

  if (q != NULL && sf_symbols != NULL) {
    cf_t* slot1_symbols = &sf_symbols[0][SRSLTE_SLOT_LEN_RE(q->cell.nof_prb, q->cell.cp)];

    cf_t* ce_slot1[SRSLTE_MAX_PORTS];
    for (int i = 0; i < SRSLTE_MAX_PORTS; i++) {
      ce_slot1[i] = &channel->ce[i][0][SRSLTE_SLOT_LEN_RE(q->cell.nof_prb, q->cell.cp)];
    }

    /* Set pointers for layermapping & precoding */
    nof_bits = 2 * q->nof_symbols;

    /* number of layers equals number of ports */
    for (i = 0; i < SRSLTE_MAX_PORTS; i++) {
      x[i] = q->x[i];
    }

    /* extract symbols */
    if (q->nof_symbols != srslte_pbch_get(slot1_symbols, q->symbols[0], q->cell)) {
      ERROR("There was an error getting the PBCH symbols\n");
      return SRSLTE_ERROR;
    }

    /* extract channel estimates */
    for (i = 0; i < q->cell.nof_ports; i++) {
      if (q->nof_symbols != srslte_pbch_get(ce_slot1[i], q->ce[i], q->cell)) {
        ERROR("There was an error getting the PBCH symbols\n");
        return SRSLTE_ERROR;
      }
    }

    q->frame_idx++;
    ret = 0;

    uint32_t frame_idx = q->frame_idx;

    /* Try decoding for 1 to cell.nof_ports antennas */
    if (q->search_all_ports) {
      nant = 1;
    } else {
      nant = q->cell.nof_ports;
    }

    do {
      if (nant != 3) {
        DEBUG("Trying %d TX antennas with %d frames\n", nant, frame_idx);

        /* in control channels, only diversity is supported */
        if (nant == 1) {
          /* no need for layer demapping */
          srslte_predecoding_single(q->symbols[0], q->ce[0], q->d, NULL, q->nof_symbols, 1.0f, channel->noise_estimate);
        } else {
          srslte_predecoding_diversity(q->symbols[0], q->ce, x, nant, q->nof_symbols, 1.0f);
          srslte_layerdemap_diversity(x, q->d, nant, q->nof_symbols / nant);
        }

        /* demodulate symbols */
        srslte_demod_soft_demodulate(SRSLTE_MOD_QPSK, q->d, &q->llr[nof_bits * (frame_idx - 1)], q->nof_symbols);

        /* We don't know where the 40 ms begin, so we try all combinations. E.g. if we received
         * 4 frames, try 1,2,3,4 individually, 12, 23, 34 in pairs, 123, 234 and finally 1234.
         * We know they are ordered.
         */
        for (nb = 0; nb < frame_idx; nb++) {
          for (dst = 0; (dst < 4 - nb); dst++) {
            for (src = 0; src < frame_idx - nb; src++) {
              ret = decode_frame(q, src, dst, nb + 1, nof_bits, nant);
              if (ret == 1) {
                if (sfn_offset) {
                  *sfn_offset = (int)dst - src + frame_idx - 1;
                }
                if (nof_tx_ports) {
                  *nof_tx_ports = nant;
                }
                if (bch_payload) {
                  memcpy(bch_payload, q->data, sizeof(uint8_t) * SRSLTE_BCH_PAYLOAD_LEN);
                }
                INFO("Decoded PBCH: src=%d, dst=%d, nb=%d, sfn_offset=%d\n",
                     src,
                     dst,
                     nb + 1,
                     (int)dst - src + frame_idx - 1);
                srslte_pbch_decode_reset(q);
                return 1;
              }
            }
          }
        }
      }
      nant++;
    } while (nant <= q->cell.nof_ports);

    /* If not found, make room for the next packet of radio frame symbols */
    if (q->frame_idx == 4) {
      memmove(q->llr, &q->llr[nof_bits], nof_bits * 3 * sizeof(float));
      q->frame_idx = 3;
    }
  }
  return ret;
}

/** Converts the MIB message to symbols mapped to SLOT #1 ready for transmission
 */
int srslte_pbch_encode(srslte_pbch_t* q,
                       uint8_t        bch_payload[SRSLTE_BCH_PAYLOAD_LEN],
                       cf_t*          sf_symbols[SRSLTE_MAX_PORTS],
                       uint32_t       frame_idx)
{
  int   i;
  int   nof_bits;
  cf_t* x[SRSLTE_MAX_LAYERS];

  if (q != NULL && bch_payload != NULL) {
    /* Set pointers for layermapping & precoding */
    nof_bits = 2 * q->nof_symbols;

    /* number of layers equals number of ports */
    for (i = 0; i < q->cell.nof_ports; i++) {
      x[i] = q->x[i];
    }
    memset(&x[q->cell.nof_ports], 0, sizeof(cf_t*) * (SRSLTE_MAX_LAYERS - q->cell.nof_ports));

    frame_idx = frame_idx % 4;

    memcpy(q->data, bch_payload, sizeof(uint8_t) * SRSLTE_BCH_PAYLOAD_LEN);

    /* encode & modulate */
    srslte_crc_attach(&q->crc, q->data, SRSLTE_BCH_PAYLOAD_LEN);
    srslte_crc_set_mask(q->data, q->cell.nof_ports);

    srslte_convcoder_encode(&q->encoder, q->data, q->data_enc, SRSLTE_BCH_PAYLOADCRC_LEN);

    srslte_rm_conv_tx(q->data_enc, SRSLTE_BCH_ENCODED_LEN, q->rm_b, 4 * nof_bits);

    srslte_scrambling_b_offset(&q->seq, &q->rm_b[frame_idx * nof_bits], frame_idx * nof_bits, nof_bits);
    srslte_mod_modulate(&q->mod, &q->rm_b[frame_idx * nof_bits], q->d, nof_bits);

    /* layer mapping & precoding */
    if (q->cell.nof_ports > 1) {
      srslte_layermap_diversity(q->d, x, q->cell.nof_ports, q->nof_symbols);
      srslte_precoding_diversity(x, q->symbols, q->cell.nof_ports, q->nof_symbols / q->cell.nof_ports, 1.0f);
    } else {
      memcpy(q->symbols[0], q->d, q->nof_symbols * sizeof(cf_t));
    }

    /* mapping to resource elements */
    for (i = 0; i < q->cell.nof_ports; i++) {
      srslte_pbch_put(q->symbols[i], &sf_symbols[i][SRSLTE_SLOT_LEN_RE(q->cell.nof_prb, q->cell.cp)], q->cell);
    }
    return SRSLTE_SUCCESS;
  } else {
    return SRSLTE_ERROR_INVALID_INPUTS;
  }
}
