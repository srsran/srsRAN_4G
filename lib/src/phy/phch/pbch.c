/**
 * Copyright 2013-2021 Software Radio Systems Limited
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

#include "srsran/srsran.h"
#include <assert.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include "prb_dl.h"
#include "srsran/phy/common/phy_common.h"
#include "srsran/phy/phch/pbch.h"
#include "srsran/phy/utils/bit.h"
#include "srsran/phy/utils/debug.h"
#include "srsran/phy/utils/vector.h"

#define PBCH_RE_CP_NORM 240
#define PBCH_RE_CP_EXT 216

const uint8_t srsran_crc_mask[4][16] = {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                                        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
                                        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                                        {0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1}};

bool srsran_pbch_exists(int nframe, int nslot)
{
  return (!(nframe % 5) && nslot == 1);
}

cf_t* offset_original;

int srsran_pbch_cp(cf_t* input, cf_t* output, srsran_cell_t cell, bool put)
{
  int   i;
  cf_t* ptr;

  offset_original = input;

  if (put) {
    ptr = input;
    output += cell.nof_prb * SRSRAN_NRE / 2 - 36;
  } else {
    ptr = output;
    input += cell.nof_prb * SRSRAN_NRE / 2 - 36;
  }

  /* symbol 0 & 1 */
  for (i = 0; i < 2; i++) {
    prb_cp_ref(&input, &output, cell.id % 3, 4, 4 * 6, put);
    if (put) {
      output += cell.nof_prb * SRSRAN_NRE - 2 * 36 + (cell.id % 3 == 2 ? 1 : 0);
    } else {
      input += cell.nof_prb * SRSRAN_NRE - 2 * 36 + (cell.id % 3 == 2 ? 1 : 0);
    }
  }
  /* symbols 2 & 3 */
  if (SRSRAN_CP_ISNORM(cell.cp)) {
    for (i = 0; i < 2; i++) {
      prb_cp(&input, &output, 6);
      if (put) {
        output += cell.nof_prb * SRSRAN_NRE - 2 * 36;
      } else {
        input += cell.nof_prb * SRSRAN_NRE - 2 * 36;
      }
    }
  } else {
    prb_cp(&input, &output, 6);
    if (put) {
      output += cell.nof_prb * SRSRAN_NRE - 2 * 36;
    } else {
      input += cell.nof_prb * SRSRAN_NRE - 2 * 36;
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
int srsran_pbch_put(cf_t* pbch, cf_t* slot1_data, srsran_cell_t cell)
{
  return srsran_pbch_cp(pbch, slot1_data, cell, true);
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
int srsran_pbch_get(cf_t* slot1_data, cf_t* pbch, srsran_cell_t cell)
{
  return srsran_pbch_cp(slot1_data, pbch, cell, false);
}

/** Initializes the PBCH transmitter and receiver.
 * At the receiver, the field nof_ports in the cell structure indicates the
 * maximum number of BS transmitter ports to look for.
 */
int srsran_pbch_init(srsran_pbch_t* q)
{
  int ret = SRSRAN_ERROR_INVALID_INPUTS;

  if (q != NULL) {
    ret = SRSRAN_ERROR;

    bzero(q, sizeof(srsran_pbch_t));

    if (srsran_modem_table_lte(&q->mod, SRSRAN_MOD_QPSK)) {
      goto clean;
    }
    int poly[3] = {0x6D, 0x4F, 0x57};
    if (srsran_viterbi_init(&q->decoder, SRSRAN_VITERBI_37, poly, 40, true)) {
      goto clean;
    }
    if (srsran_crc_init(&q->crc, SRSRAN_LTE_CRC16, 16)) {
      goto clean;
    }
    q->encoder.K           = 7;
    q->encoder.R           = 3;
    q->encoder.tail_biting = true;
    memcpy(q->encoder.poly, poly, 3 * sizeof(int));

    q->nof_symbols = PBCH_RE_CP_NORM;

    q->d = srsran_vec_cf_malloc(q->nof_symbols);
    if (!q->d) {
      goto clean;
    }
    int i;
    for (i = 0; i < SRSRAN_MAX_PORTS; i++) {
      q->ce[i] = srsran_vec_cf_malloc(q->nof_symbols);
      if (!q->ce[i]) {
        goto clean;
      }
      q->x[i] = srsran_vec_cf_malloc(q->nof_symbols);
      if (!q->x[i]) {
        goto clean;
      }
      q->symbols[i] = srsran_vec_cf_malloc(q->nof_symbols);
      if (!q->symbols[i]) {
        goto clean;
      }
    }
    q->llr = srsran_vec_f_malloc(q->nof_symbols * 4 * 2);
    if (!q->llr) {
      goto clean;
    }
    q->temp = srsran_vec_f_malloc(q->nof_symbols * 4 * 2);
    if (!q->temp) {
      goto clean;
    }
    q->rm_b = srsran_vec_u8_malloc(q->nof_symbols * 4 * 2);
    if (!q->rm_b) {
      goto clean;
    }

    ret = SRSRAN_SUCCESS;
  }
clean:
  if (ret == SRSRAN_ERROR) {
    srsran_pbch_free(q);
  }
  return ret;
}

void srsran_pbch_free(srsran_pbch_t* q)
{
  srsran_sequence_free(&q->seq);
  srsran_modem_table_free(&q->mod);
  srsran_viterbi_free(&q->decoder);
  int i;
  for (i = 0; i < SRSRAN_MAX_PORTS; i++) {
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
  bzero(q, sizeof(srsran_pbch_t));
}

int srsran_pbch_set_cell(srsran_pbch_t* q, srsran_cell_t cell)
{
  int ret = SRSRAN_ERROR_INVALID_INPUTS;

  if (q != NULL && srsran_cell_isvalid(&cell)) {
    if (cell.nof_ports == 0) {
      q->search_all_ports = true;
      cell.nof_ports      = SRSRAN_MAX_PORTS;
    } else {
      q->search_all_ports = false;
    }

    if (q->cell.id != cell.id || q->cell.nof_prb == 0) {
      q->cell = cell;
      if (srsran_sequence_pbch(&q->seq, q->cell.cp, q->cell.id)) {
        return SRSRAN_ERROR;
      }
    }
    q->nof_symbols = (SRSRAN_CP_ISNORM(q->cell.cp)) ? PBCH_RE_CP_NORM : PBCH_RE_CP_EXT;

    ret = SRSRAN_SUCCESS;
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
void srsran_pbch_mib_unpack(uint8_t* msg, srsran_cell_t* cell, uint32_t* sfn)
{
  int phich_res;

  uint32_t bw_idx = srsran_bit_pack(&msg, 3);
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
    cell->phich_length = SRSRAN_PHICH_EXT;
  } else {
    cell->phich_length = SRSRAN_PHICH_NORM;
  }
  msg++;

  phich_res = srsran_bit_pack(&msg, 2);
  switch (phich_res) {
    case 0:
      cell->phich_resources = SRSRAN_PHICH_R_1_6;
      break;
    case 1:
      cell->phich_resources = SRSRAN_PHICH_R_1_2;
      break;
    case 2:
      cell->phich_resources = SRSRAN_PHICH_R_1;
      break;
    case 3:
      cell->phich_resources = SRSRAN_PHICH_R_2;
      break;
  }
  if (sfn) {
    *sfn = srsran_bit_pack(&msg, 8) << 2;
  }
}

/**
 * Packs MIB to PBCH message.
 *
 * @param[out] payload Output unpacked bit array of size 24
 * @param[in] sfn System frame number
 * @param[in] cell Cell configuration to be encoded in MIB
 */
void srsran_pbch_mib_pack(srsran_cell_t* cell, uint32_t sfn, uint8_t* payload)
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
  srsran_bit_unpack(bw, &msg, 3);

  *msg = cell->phich_length == SRSRAN_PHICH_EXT;
  msg++;

  switch (cell->phich_resources) {
    case SRSRAN_PHICH_R_1_6:
      phich_res = 0;
      break;
    case SRSRAN_PHICH_R_1_2:
      phich_res = 1;
      break;
    case SRSRAN_PHICH_R_1:
      phich_res = 2;
      break;
    case SRSRAN_PHICH_R_2:
      phich_res = 3;
      break;
  }
  srsran_bit_unpack(phich_res, &msg, 2);
  srsran_bit_unpack(sfn >> 2, &msg, 8);
}

void srsran_pbch_decode_reset(srsran_pbch_t* q)
{
  q->frame_idx = 0;
}

void srsran_crc_set_mask(uint8_t* data, int nof_ports)
{
  int i;
  for (i = 0; i < 16; i++) {
    data[SRSRAN_BCH_PAYLOAD_LEN + i] = (data[SRSRAN_BCH_PAYLOAD_LEN + i] + srsran_crc_mask[nof_ports - 1][i]) % 2;
  }
}

/* Checks CRC after applying the mask for the given number of ports.
 *
 * The bits buffer size must be at least 40 bytes.
 *
 * Returns 0 if the data is correct, -1 otherwise
 */
uint32_t srsran_pbch_crc_check(srsran_pbch_t* q, uint8_t* bits, uint32_t nof_ports)
{
  uint8_t data[SRSRAN_BCH_PAYLOADCRC_LEN];
  memcpy(data, bits, SRSRAN_BCH_PAYLOADCRC_LEN * sizeof(uint8_t));
  srsran_crc_set_mask(data, nof_ports);
  int ret = srsran_crc_checksum(&q->crc, data, SRSRAN_BCH_PAYLOADCRC_LEN);
  if (ret == 0) {
    uint32_t chkzeros = 0;
    for (int i = 0; i < SRSRAN_BCH_PAYLOAD_LEN; i++) {
      chkzeros += data[i];
    }
    if (chkzeros) {
      return 0;
    } else {
      return SRSRAN_ERROR;
    }
  } else {
    return ret;
  }
}

int decode_frame(srsran_pbch_t* q, uint32_t src, uint32_t dst, uint32_t n, uint32_t nof_bits, uint32_t nof_ports)
{
  int j;

  if (dst + n <= 4 && src + n <= 4) {
    srsran_vec_f_copy(&q->temp[dst * nof_bits], &q->llr[src * nof_bits], n * nof_bits);

    /* descramble */
    srsran_scrambling_f_offset(&q->seq, &q->temp[dst * nof_bits], dst * nof_bits, n * nof_bits);

    for (j = 0; j < dst * nof_bits; j++) {
      q->temp[j] = SRSRAN_RX_NULL;
    }
    for (j = (dst + n) * nof_bits; j < 4 * nof_bits; j++) {
      q->temp[j] = SRSRAN_RX_NULL;
    }

    /* unrate matching */
    srsran_rm_conv_rx(q->temp, 4 * nof_bits, q->rm_f, SRSRAN_BCH_ENCODED_LEN);

    /* Normalize LLR */
    srsran_vec_sc_prod_fff(q->rm_f, 1.0 / ((float)2 * n), q->rm_f, SRSRAN_BCH_ENCODED_LEN);

    /* decode */
    srsran_viterbi_decode_f(&q->decoder, q->rm_f, q->data, SRSRAN_BCH_PAYLOADCRC_LEN);

    if (!srsran_pbch_crc_check(q, q->data, nof_ports)) {
      return 1;
    } else {
      return SRSRAN_SUCCESS;
    }
  } else {
    ERROR("Error in PBCH decoder: Invalid frame pointers dst=%d, src=%d, n=%d", src, dst, n);
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
int srsran_pbch_decode(srsran_pbch_t*         q,
                       srsran_chest_dl_res_t* channel,
                       cf_t*                  sf_symbols[SRSRAN_MAX_PORTS],
                       uint8_t                bch_payload[SRSRAN_BCH_PAYLOAD_LEN],
                       uint32_t*              nof_tx_ports,
                       int*                   sfn_offset)
{
  uint32_t src, dst, nb;
  uint32_t nant;
  int      i;
  int      nof_bits;
  cf_t*    x[SRSRAN_MAX_LAYERS];

  int ret = SRSRAN_ERROR_INVALID_INPUTS;

  if (q != NULL && sf_symbols != NULL) {
    cf_t* slot1_symbols = &sf_symbols[0][SRSRAN_SLOT_LEN_RE(q->cell.nof_prb, q->cell.cp)];

    cf_t* ce_slot1[SRSRAN_MAX_PORTS];
    for (int i = 0; i < SRSRAN_MAX_PORTS; i++) {
      ce_slot1[i] = &channel->ce[i][0][SRSRAN_SLOT_LEN_RE(q->cell.nof_prb, q->cell.cp)];
    }

    /* Set pointers for layermapping & precoding */
    nof_bits = 2 * q->nof_symbols;

    /* number of layers equals number of ports */
    for (i = 0; i < SRSRAN_MAX_PORTS; i++) {
      x[i] = q->x[i];
    }

    /* extract symbols */
    if (q->nof_symbols != srsran_pbch_get(slot1_symbols, q->symbols[0], q->cell)) {
      ERROR("There was an error getting the PBCH symbols");
      return SRSRAN_ERROR;
    }

    /* extract channel estimates */
    for (i = 0; i < q->cell.nof_ports; i++) {
      if (q->nof_symbols != srsran_pbch_get(ce_slot1[i], q->ce[i], q->cell)) {
        ERROR("There was an error getting the PBCH symbols");
        return SRSRAN_ERROR;
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
        DEBUG("Trying %d TX antennas with %d frames", nant, frame_idx);

        /* in control channels, only diversity is supported */
        if (nant == 1) {
          /* no need for layer demapping */
          srsran_predecoding_single(q->symbols[0], q->ce[0], q->d, NULL, q->nof_symbols, 1.0f, channel->noise_estimate);
        } else {
          srsran_predecoding_diversity(q->symbols[0], q->ce, x, nant, q->nof_symbols, 1.0f);
          srsran_layerdemap_diversity(x, q->d, nant, q->nof_symbols / nant);
        }

        /* demodulate symbols */
        srsran_demod_soft_demodulate(SRSRAN_MOD_QPSK, q->d, &q->llr[nof_bits * (frame_idx - 1)], q->nof_symbols);

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
                  memcpy(bch_payload, q->data, sizeof(uint8_t) * SRSRAN_BCH_PAYLOAD_LEN);
                }
                INFO("Decoded PBCH: src=%d, dst=%d, nb=%d, sfn_offset=%d",
                     src,
                     dst,
                     nb + 1,
                     (int)dst - src + frame_idx - 1);
                srsran_pbch_decode_reset(q);
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
int srsran_pbch_encode(srsran_pbch_t* q,
                       uint8_t        bch_payload[SRSRAN_BCH_PAYLOAD_LEN],
                       cf_t*          sf_symbols[SRSRAN_MAX_PORTS],
                       uint32_t       frame_idx)
{
  int   i;
  int   nof_bits;
  cf_t* x[SRSRAN_MAX_LAYERS];

  if (q != NULL && bch_payload != NULL) {
    /* Set pointers for layermapping & precoding */
    nof_bits = 2 * q->nof_symbols;

    /* number of layers equals number of ports */
    for (i = 0; i < q->cell.nof_ports; i++) {
      x[i] = q->x[i];
    }
    memset(&x[q->cell.nof_ports], 0, sizeof(cf_t*) * (SRSRAN_MAX_LAYERS - q->cell.nof_ports));

    frame_idx = frame_idx % 4;

    memcpy(q->data, bch_payload, sizeof(uint8_t) * SRSRAN_BCH_PAYLOAD_LEN);

    /* encode & modulate */
    srsran_crc_attach(&q->crc, q->data, SRSRAN_BCH_PAYLOAD_LEN);
    srsran_crc_set_mask(q->data, q->cell.nof_ports);

    srsran_convcoder_encode(&q->encoder, q->data, q->data_enc, SRSRAN_BCH_PAYLOADCRC_LEN);

    srsran_rm_conv_tx(q->data_enc, SRSRAN_BCH_ENCODED_LEN, q->rm_b, 4 * nof_bits);

    srsran_scrambling_b_offset(&q->seq, &q->rm_b[frame_idx * nof_bits], frame_idx * nof_bits, nof_bits);
    srsran_mod_modulate(&q->mod, &q->rm_b[frame_idx * nof_bits], q->d, nof_bits);

    /* layer mapping & precoding */
    if (q->cell.nof_ports > 1) {
      srsran_layermap_diversity(q->d, x, q->cell.nof_ports, q->nof_symbols);
      srsran_precoding_diversity(x, q->symbols, q->cell.nof_ports, q->nof_symbols / q->cell.nof_ports, 1.0f);
    } else {
      memcpy(q->symbols[0], q->d, q->nof_symbols * sizeof(cf_t));
    }

    /* mapping to resource elements */
    for (i = 0; i < q->cell.nof_ports; i++) {
      srsran_pbch_put(q->symbols[i], &sf_symbols[i][SRSRAN_SLOT_LEN_RE(q->cell.nof_prb, q->cell.cp)], q->cell);
    }
    return SRSRAN_SUCCESS;
  } else {
    return SRSRAN_ERROR_INVALID_INPUTS;
  }
}
