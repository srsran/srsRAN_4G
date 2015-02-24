/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2014 The libLTE Developers. See the
 * COPYRIGHT file at the top-level directory of this distribution.
 *
 * \section LICENSE
 *
 * This file is part of the libLTE library.
 *
 * libLTE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * libLTE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * A copy of the GNU Lesser General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <math.h>

#include "prb_dl.h"
#include "liblte/phy/phch/pbch.h"
#include "liblte/phy/common/phy_common.h"
#include "liblte/phy/utils/bit.h"
#include "liblte/phy/utils/vector.h"
#include "liblte/phy/utils/debug.h"

const uint8_t crc_mask[4][16] = {
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0 }, { 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1 } };

bool pbch_exists(int nframe, int nslot) {
  return (!(nframe % 5) && nslot == 1);
}

cf_t *offset_original;

int pbch_cp(cf_t *input, cf_t *output, lte_cell_t cell, bool put) {
  int i;
  cf_t *ptr;
  
  offset_original = input; 
  
  if (put) {
    ptr = input;
    output += cell.nof_prb * RE_X_RB / 2 - 36;
  } else {
    ptr = output;
    input += cell.nof_prb * RE_X_RB / 2 - 36;
  }
  
  /* symbol 0 & 1 */
  for (i = 0; i < 2; i++) {
    prb_cp_ref(&input, &output, cell.id % 3, 4, 4*6, put);
    if (put) {
      output += cell.nof_prb * RE_X_RB - 2*36 + (cell.id%3==2?1:0);      
    } else {
      input += cell.nof_prb * RE_X_RB - 2*36 + (cell.id%3==2?1:0);
    }
  }
  /* symbols 2 & 3 */
  if (CP_ISNORM(cell.cp)) {
    for (i = 0; i < 2; i++) {
      prb_cp(&input, &output, 6);
      if (put) {
        output += cell.nof_prb * RE_X_RB - 2*36;
      } else {
        input += cell.nof_prb * RE_X_RB - 2*36;
      }
    }
  } else {
    prb_cp(&input, &output, 6);
    if (put) {
      output += cell.nof_prb * RE_X_RB - 2*36;
    } else {
      input += cell.nof_prb * RE_X_RB - 2*36;
    }
    prb_cp_ref(&input, &output, cell.id % 3, 4, 4*6, put);
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
 */
int pbch_put(cf_t *pbch, cf_t *slot1_data, lte_cell_t cell) {
  return pbch_cp(pbch, slot1_data, cell, true);
}

/**
 * Extracts PBCH from slot number 1
 *
 * Returns the number of symbols written to pbch
 *
 * 36.211 10.3 section 6.6.4
 */
int pbch_get(cf_t *slot1_data, cf_t *pbch, lte_cell_t cell) {
  return pbch_cp(slot1_data, pbch, cell, false);
}

/** Initializes the PBCH transmitter and receiver. 
 * At the receiver, the field nof_ports in the cell structure indicates the 
 * maximum number of BS transmitter ports to look for.  
 */
int pbch_init(pbch_t *q, lte_cell_t cell) {
  int ret = LIBLTE_ERROR_INVALID_INPUTS;

  if (q                       != NULL &&
      lte_cell_isvalid(&cell))
  {
    ret = LIBLTE_ERROR;

    bzero(q, sizeof(pbch_t));
    q->cell = cell;
    q->nof_symbols = (CP_ISNORM(q->cell.cp)) ? PBCH_RE_CPNORM : PBCH_RE_CPEXT;
    
    if (precoding_init(&q->precoding, SF_LEN_RE(cell.nof_prb, cell.cp))) {
      fprintf(stderr, "Error initializing precoding\n");
    }

    if (modem_table_lte(&q->mod, LTE_QPSK, true)) {
      goto clean;
    }
    demod_soft_init(&q->demod, q->nof_symbols);
    demod_soft_table_set(&q->demod, &q->mod);
    demod_soft_alg_set(&q->demod, APPROX);
    
    if (sequence_pbch(&q->seq_pbch, q->cell.cp, q->cell.id)) {
      goto clean;
    }

    uint32_t poly[3] = { 0x6D, 0x4F, 0x57 };
    if (viterbi_init(&q->decoder, viterbi_37, poly, 40, true)) {
      goto clean;
    }
    if (crc_init(&q->crc, LTE_CRC16, 16)) {
      goto clean;
    }
    q->encoder.K = 7;
    q->encoder.R = 3;
    q->encoder.tail_biting = true;
    memcpy(q->encoder.poly, poly, 3 * sizeof(int));

    q->pbch_d = vec_malloc(sizeof(cf_t) * q->nof_symbols);
    if (!q->pbch_d) {
      goto clean;
    }
    int i;
    for (i = 0; i < q->cell.nof_ports; i++) {
      q->ce[i] = vec_malloc(sizeof(cf_t) * q->nof_symbols);
      if (!q->ce[i]) {
        goto clean;
      }
      q->pbch_x[i] = vec_malloc(sizeof(cf_t) * q->nof_symbols);
      if (!q->pbch_x[i]) {
        goto clean;
      }
      q->pbch_symbols[i] = vec_malloc(sizeof(cf_t) * q->nof_symbols);
      if (!q->pbch_symbols[i]) {
        goto clean;
      }
    }
    q->pbch_llr = vec_malloc(sizeof(float) * q->nof_symbols * 4 * 2);
    if (!q->pbch_llr) {
      goto clean;
    }
    q->temp = vec_malloc(sizeof(float) * q->nof_symbols * 4 * 2);
    if (!q->temp) {
      goto clean;
    }
    q->pbch_rm_b = vec_malloc(sizeof(float) * q->nof_symbols * 4 * 2);
    if (!q->pbch_rm_b) {
      goto clean;
    }
    ret = LIBLTE_SUCCESS;
  }
clean: 
  if (ret == LIBLTE_ERROR) {
    pbch_free(q);
  }
  return ret;
}

void pbch_free(pbch_t *q) {
  if (q->pbch_d) {
    free(q->pbch_d);
  }
  int i;
  for (i = 0; i < q->cell.nof_ports; i++) {
    if (q->ce[i]) {
      free(q->ce[i]);
    }
    if (q->pbch_x[i]) {
      free(q->pbch_x[i]);
    }
    if (q->pbch_symbols[i]) {
      free(q->pbch_symbols[i]);
    }
  }
  if (q->pbch_llr) {
    free(q->pbch_llr);
  }
  if (q->temp) {
    free(q->temp);
  }
  if (q->pbch_rm_b) {
    free(q->pbch_rm_b);
  }
  precoding_free(&q->precoding);
  sequence_free(&q->seq_pbch);
  modem_table_free(&q->mod);
  viterbi_free(&q->decoder);
  demod_soft_free(&q->demod);

  bzero(q, sizeof(pbch_t));

}


/** Unpacks MIB from PBCH message.
 * msg buffer must be 24 byte length at least
 */
void pbch_mib_unpack(uint8_t *msg, lte_cell_t *cell, uint32_t *sfn) {
  int bw, phich_res;

  bw = bit_unpack(&msg, 3);
  switch (bw) {
  case 0:
    cell->nof_prb = 6;
    break;
  case 1:
    cell->nof_prb = 15;
    break;
  default:
    cell->nof_prb = (bw - 1) * 25;
    break;
  }
  if (*msg) {
    cell->phich_length = PHICH_EXT;
  } else {
    cell->phich_length = PHICH_NORM;
  }
  msg++;

  phich_res = bit_unpack(&msg, 2);
  switch (phich_res) {
  case 0:
      cell->phich_resources = R_1_6;
    break;
  case 1:
      cell->phich_resources = R_1_2;
    break;
  case 2:
      cell->phich_resources = R_1;
    break;
  case 3:
      cell->phich_resources = R_2;
    break;
  }
  if (sfn) {
    *sfn = bit_unpack(&msg, 8) << 2;    
  }
}

/** Unpacks MIB from PBCH message.
 * msg buffer must be 24 byte length at least
 */
void pbch_mib_pack(lte_cell_t *cell, uint32_t sfn, uint8_t *msg) {
  int bw, phich_res = 0;

  bzero(msg, 24);

  if (cell->nof_prb <= 6) {
    bw = 0;
  } else if (cell->nof_prb <= 15) {
    bw = 1;
  } else {
    bw = 1 + cell->nof_prb / 25;
  }
  bit_pack(bw, &msg, 3);

  *msg = cell->phich_length == PHICH_EXT;
  msg++;

  switch (cell->phich_resources) {
  case R_1_6:
    phich_res = 0;
    break;
  case R_1_2:
    phich_res = 1;
    break;
  case R_1:
    phich_res = 2;
    break;
  case R_2:
    phich_res = 3;
    break;
  }
  bit_pack(phich_res, &msg, 2);
  bit_pack(sfn >> 2, &msg, 8);
}

void pbch_mib_fprint(FILE *stream, lte_cell_t *cell, uint32_t sfn, uint32_t cell_id) {
  printf(" - Cell ID:         %d\n", cell_id);
  printf(" - Nof ports:       %d\n", cell->nof_ports);
  printf(" - PRB:             %d\n", cell->nof_prb);
  printf(" - PHICH Length:    %s\n",
         cell->phich_length == PHICH_EXT ? "Extended" : "Normal");
  printf(" - PHICH Resources: ");
  switch (cell->phich_resources) {
  case R_1_6:
    printf("1/6");
    break;
  case R_1_2:
    printf("1/2");
    break;
  case R_1:
    printf("1");
    break;
  case R_2:
    printf("2");
    break;
  }
  printf("\n");
  printf(" - SFN:             %d\n", sfn);
}


void pbch_decode_reset(pbch_t *q) {
  q->frame_idx = 0;
}

void crc_set_mask(uint8_t *data, int nof_ports) {
  int i;
  for (i = 0; i < 16; i++) {
    data[BCH_PAYLOAD_LEN + i] = (data[BCH_PAYLOAD_LEN + i] + crc_mask[nof_ports - 1][i]) % 2;
  }

}

/* Checks CRC after applying the mask for the given number of ports.
 *
 * The bits buffer size must be at least 40 bytes.
 *
 * Returns 0 if the data is correct, -1 otherwise
 */
uint32_t pbch_crc_check(pbch_t *q, uint8_t *bits, uint32_t nof_ports) {
  uint8_t data[BCH_PAYLOADCRC_LEN];
  memcpy(data, bits, BCH_PAYLOADCRC_LEN * sizeof(uint8_t));
  crc_set_mask(data, nof_ports);
  int ret = crc_checksum(&q->crc, data, BCH_PAYLOADCRC_LEN);
  if (ret == 0) {
    uint32_t chkzeros=0;
    for (int i=0;i<BCH_PAYLOAD_LEN;i++) {
      chkzeros += data[i];
    }
    if (chkzeros) {
      return 0;
    } else {
      return LIBLTE_ERROR;
    }
  } else {
    return ret; 
  }
}

int pbch_decode_frame(pbch_t *q, uint32_t src, uint32_t dst, uint32_t n,
    uint32_t nof_bits, uint32_t nof_ports) {
  int j;
  
  DEBUG("Trying to decode PBCH %d bits, %d ports, src: %d, dst: %d, n=%d\n", nof_bits, nof_ports, src, dst, n);

  memcpy(&q->temp[dst * nof_bits], &q->pbch_llr[src * nof_bits],
      n * nof_bits * sizeof(float));

  /* descramble */
  scrambling_f_offset(&q->seq_pbch, &q->temp[dst * nof_bits], dst * nof_bits,
      n * nof_bits);

  for (j = 0; j < dst * nof_bits; j++) {
    q->temp[j] = RX_NULL;
  }
  for (j = (dst + n) * nof_bits; j < 4 * nof_bits; j++) {
    q->temp[j] = RX_NULL;
  }

  /* unrate matching */
  rm_conv_rx(q->temp, 4 * nof_bits, q->pbch_rm_f, BCH_ENCODED_LEN);
  
  /* decode */
  viterbi_decode_f(&q->decoder, q->pbch_rm_f, q->data, BCH_PAYLOADCRC_LEN);

 if (!pbch_crc_check(q, q->data, nof_ports)) {
    return 1;
  } else {
    return LIBLTE_SUCCESS;
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
int pbch_decode(pbch_t *q, cf_t *slot1_symbols, cf_t *ce_slot1[MAX_PORTS], float noise_estimate, 
                 uint8_t bch_payload[BCH_PAYLOAD_LEN], uint32_t *nof_tx_ports, uint32_t *sfn_offset) 
{
  uint32_t src, dst, nb;
  uint32_t nant;
  int i;
  int nof_bits;
  cf_t *x[MAX_LAYERS];
  
  int ret = LIBLTE_ERROR_INVALID_INPUTS;
  
  if (q                 != NULL &&
      slot1_symbols     != NULL)
  {
    for (i=0;i<q->cell.nof_ports;i++) {
      if (ce_slot1[i] == NULL) {
        return LIBLTE_ERROR_INVALID_INPUTS;
      } 
    }

    /* Set pointers for layermapping & precoding */
    nof_bits = 2 * q->nof_symbols;

    /* number of layers equals number of ports */
    for (i = 0; i < MAX_PORTS; i++) {
      x[i] = q->pbch_x[i];
    }
    memset(&x[MAX_PORTS], 0, sizeof(cf_t*) * (MAX_LAYERS - MAX_PORTS));
    
    /* extract symbols */
    if (q->nof_symbols != pbch_get(slot1_symbols, q->pbch_symbols[0], q->cell)) {
      fprintf(stderr, "There was an error getting the PBCH symbols\n");
      return LIBLTE_ERROR;
    }

    /* extract channel estimates */
    for (i = 0; i < q->cell.nof_ports; i++) {
      if (q->nof_symbols != pbch_get(ce_slot1[i], q->ce[i], q->cell)) {
        fprintf(stderr, "There was an error getting the PBCH symbols\n");
        return LIBLTE_ERROR;
      }
    }

    q->frame_idx++;
    ret = 0;

    /* Try decoding for 1 to cell.nof_ports antennas */
    for (nant = 1; nant <= q->cell.nof_ports && !ret; nant++) {
      if (nant != 3) {
        DEBUG("Trying %d TX antennas with %d frames\n", nant, q->frame_idx);

        /* in conctrol channels, only diversity is supported */
        if (nant == 1) {
          /* no need for layer demapping */
          predecoding_single(&q->precoding, q->pbch_symbols[0], q->ce[0], q->pbch_d,
              q->nof_symbols, noise_estimate);
        } else {
          predecoding_diversity(&q->precoding, q->pbch_symbols[0], q->ce, x, nant,
              q->nof_symbols, noise_estimate);
          layerdemap_diversity(x, q->pbch_d, nant, q->nof_symbols / nant);
        }

        /* demodulate symbols */
        demod_soft_sigma_set(&q->demod, 1.0);
        demod_soft_demodulate(&q->demod, q->pbch_d,
            &q->pbch_llr[nof_bits * (q->frame_idx - 1)], q->nof_symbols);
        
        /* We don't know where the 40 ms begin, so we try all combinations. E.g. if we received
        * 4 frames, try 1,2,3,4 individually, 12, 23, 34 in pairs, 123, 234 and finally 1234.
        * We know they are ordered.
        *
        * FIXME: There are unnecessary checks because 2,3,4 have already been processed in the previous
        * calls.
        */
        for (nb = 0; nb < q->frame_idx && !ret; nb++) {
          for (dst = 0; (dst < 4 - nb) && !ret; dst++) {
            for (src = 0; src < q->frame_idx - nb && !ret; src++) {
              ret = pbch_decode_frame(q, src, dst, nb + 1, nof_bits, nant);     
              if (ret == 1) {
                if (sfn_offset) {
                  *sfn_offset = dst - src;
                }
                if (nof_tx_ports) {
                  *nof_tx_ports = nant; 
                }
                if (bch_payload) {
                  memcpy(bch_payload, q->data, sizeof(uint8_t) * BCH_PAYLOAD_LEN);      
                }
              }
            }
          }
        }
      }
    }

    /* If not found, make room for the next packet of radio frame symbols */
    if (q->frame_idx == 4) {
      memmove(q->pbch_llr, &q->pbch_llr[nof_bits], nof_bits * 3 * sizeof(float));
      q->frame_idx = 3;
    }
  }
  return ret;
}

/** Converts the MIB message to symbols mapped to SLOT #1 ready for transmission
 */
int pbch_encode(pbch_t *q, uint8_t bch_payload[BCH_PAYLOAD_LEN], cf_t *slot1_symbols[MAX_PORTS]) {
  int i;
  int nof_bits;
  cf_t *x[MAX_LAYERS];
  
  if (q                 != NULL &&
      bch_payload               != NULL)
  {
    for (i=0;i<q->cell.nof_ports;i++) {
      if (slot1_symbols[i] == NULL) {
        return LIBLTE_ERROR_INVALID_INPUTS;
      } 
    }
    /* Set pointers for layermapping & precoding */
    nof_bits = 2 * q->nof_symbols;

    /* number of layers equals number of ports */
    for (i = 0; i < q->cell.nof_ports; i++) {
      x[i] = q->pbch_x[i];
    }
    memset(&x[q->cell.nof_ports], 0, sizeof(cf_t*) * (MAX_LAYERS - q->cell.nof_ports));
    
    if (q->frame_idx == 0) {
      memcpy(q->data, bch_payload, sizeof(uint8_t) * BCH_PAYLOAD_LEN);

      /* encode & modulate */
      crc_attach(&q->crc, q->data, BCH_PAYLOAD_LEN);
      crc_set_mask(q->data, q->cell.nof_ports);
      
      convcoder_encode(&q->encoder, q->data, q->data_enc, BCH_PAYLOADCRC_LEN);

      rm_conv_tx(q->data_enc, BCH_ENCODED_LEN, q->pbch_rm_b, 4 * nof_bits);

    }

    scrambling_b_offset(&q->seq_pbch, &q->pbch_rm_b[q->frame_idx * nof_bits],
        q->frame_idx * nof_bits, nof_bits);
    mod_modulate(&q->mod, &q->pbch_rm_b[q->frame_idx * nof_bits], q->pbch_d,
        nof_bits);

    /* layer mapping & precoding */
    if (q->cell.nof_ports > 1) {
      layermap_diversity(q->pbch_d, x, q->cell.nof_ports, q->nof_symbols);
      precoding_diversity(&q->precoding, x, q->pbch_symbols, q->cell.nof_ports,
          q->nof_symbols / q->cell.nof_ports);
    } else {
      memcpy(q->pbch_symbols[0], q->pbch_d, q->nof_symbols * sizeof(cf_t));
    }

    /* mapping to resource elements */
    for (i = 0; i < q->cell.nof_ports; i++) {
      pbch_put(q->pbch_symbols[i], slot1_symbols[i], q->cell);
    }
    q->frame_idx++;
    if (q->frame_idx == 4) {
      q->frame_idx = 0;
    }
    return LIBLTE_SUCCESS;
  } else {
    return LIBLTE_ERROR_INVALID_INPUTS;
  }
}

