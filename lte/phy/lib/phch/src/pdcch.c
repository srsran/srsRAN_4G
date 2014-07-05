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
#include <math.h>

#include "liblte/phy/phch/dci.h"
#include "liblte/phy/phch/regs.h"
#include "liblte/phy/phch/pdcch.h"
#include "liblte/phy/common/phy_common.h"
#include "liblte/phy/utils/bit.h"
#include "liblte/phy/utils/vector.h"
#include "liblte/phy/utils/debug.h"

#define PDCCH_NOF_FORMATS               4
#define PDCCH_FORMAT_NOF_CCE(i)         (1<<i)
#define PDCCH_FORMAT_NOF_REGS(i)        ((1<<i)*9)
#define PDCCH_FORMAT_NOF_BITS(i)        ((1<<i)*72)


#define MIN(a,b) ((a>b)?b:a)


static void set_cfi(pdcch_t *q, uint32_t cfi) {
  if (cfi > 0 && cfi < 4) {
    q->nof_regs = (regs_pdcch_nregs(q->regs, cfi) / 9) * 9;
    q->nof_cce = q->nof_regs / 9;
    q->nof_symbols = 4 * q->nof_regs;
    q->nof_bits = 2 * q->nof_symbols;    
  } 
}


/** Initializes the PDCCH transmitter and receiver */
int pdcch_init(pdcch_t *q, regs_t *regs, lte_cell_t cell) {
  int ret = LIBLTE_ERROR_INVALID_INPUTS;
  int i;

  if (q                         != NULL &&
      regs                      != NULL &&
      lte_cell_isvalid(&cell)) 
  {   
    ret = LIBLTE_ERROR;
    bzero(q, sizeof(pdcch_t));
    q->cell = cell;
    q->regs = regs;

    /* Now allocate memory for the maximum number of REGs (CFI=3)
    */
    set_cfi(q, 3);
    q->max_bits = q->nof_bits;

    INFO("Init PDCCH: %d CCEs (%d REGs), %d bits, %d symbols, %d ports\n",
        q->nof_cce, q->nof_regs, q->nof_bits, q->nof_symbols, q->cell.nof_ports);

    if (modem_table_std(&q->mod, LTE_QPSK, true)) {
      goto clean;
    }
    if (crc_init(&q->crc, LTE_CRC16, 16)) {
      goto clean;
    }

    demod_soft_init(&q->demod);
    demod_soft_table_set(&q->demod, &q->mod);
    demod_soft_alg_set(&q->demod, APPROX);

    for (i = 0; i < NSUBFRAMES_X_FRAME; i++) {
      if (sequence_pdcch(&q->seq_pdcch[i], 2 * i, q->cell.id, q->nof_bits)) {
        goto clean;
      }
    }

    uint32_t poly[3] = { 0x6D, 0x4F, 0x57 };
    if (viterbi_init(&q->decoder, viterbi_37, poly, DCI_MAX_BITS + 16, true)) {
      goto clean;
    }

    q->pdcch_e = malloc(sizeof(char) * q->nof_bits);
    if (!q->pdcch_e) {
      goto clean;
    }

    q->pdcch_llr = malloc(sizeof(float) * q->nof_bits);
    if (!q->pdcch_llr) {
      goto clean;
    }

    q->pdcch_d = malloc(sizeof(cf_t) * q->nof_symbols);
    if (!q->pdcch_d) {
      goto clean;
    }

    for (i = 0; i < MAX_PORTS; i++) {
      q->ce[i] = malloc(sizeof(cf_t) * q->nof_symbols);
      if (!q->ce[i]) {
        goto clean;
      }
      q->pdcch_x[i] = malloc(sizeof(cf_t) * q->nof_symbols);
      if (!q->pdcch_x[i]) {
        goto clean;
      }
      q->pdcch_symbols[i] = malloc(sizeof(cf_t) * q->nof_symbols);
      if (!q->pdcch_symbols[i]) {
        goto clean;
      }
    }

    ret = LIBLTE_SUCCESS;
  }
  clean: 
  if (ret == LIBLTE_ERROR) {
    pdcch_free(q);
  }
  return ret;
}

void pdcch_free(pdcch_t *q) {
  int i;

  if (q->pdcch_e) {
    free(q->pdcch_e);
  }
  if (q->pdcch_llr) {
    free(q->pdcch_llr);
  }
  if (q->pdcch_d) {
    free(q->pdcch_d);
  }
  for (i = 0; i < MAX_PORTS; i++) {
    if (q->ce[i]) {
      free(q->ce[i]);
    }
    if (q->pdcch_x[i]) {
      free(q->pdcch_x[i]);
    }
    if (q->pdcch_symbols[i]) {
      free(q->pdcch_symbols[i]);
    }
  }

  for (i = 0; i < NSUBFRAMES_X_FRAME; i++) {
    sequence_free(&q->seq_pdcch[i]);
  }

  modem_table_free(&q->mod);
  viterbi_free(&q->decoder);
}

/** 36.213 v9.1.1 
 * Computes up to max_candidates UE-specific candidates for DCI messages and saves them 
 * in the structure pointed by c.
 * Returns the number of candidates saved in the array c.   
 */
uint32_t pdcch_ue_locations(pdcch_t *q, dci_location_t *c, uint32_t max_candidates,
                        uint32_t nsubframe, uint32_t cfi, uint16_t rnti) {
  
  uint32_t i, k, l, L, m; 
  uint32_t Yk, ncce;
  const int S[4] = { 6, 12, 8, 16 };

  set_cfi(q, cfi);

  // Compute Yk for this subframe
  Yk = rnti;
  for (m = 0; m < nsubframe; m++) {
    Yk = (39827 * Yk) % 65537;
  }

  k = 0;
  // All aggregation levels from 8 to 1
  for (l = 3; l >= 0; l--) {
    L = (1 << l);
    // For all possible ncce offset
    for (i = 0; i < MIN(q->nof_cce / L, 16 / S[l]); i++) {
      ncce = L * ((Yk + i) % (q->nof_cce / L));      
      if (k                              < max_candidates     &&
          ncce + PDCCH_FORMAT_NOF_CCE(L) < q->nof_cce) 
      {            
        c[k].L = l;
        c[k].ncce = ncce;
        
        DEBUG("UE-specific SS Candidate %d: nCCE: %d, L: %d\n",
            k, c[k].ncce, c[k].L);            

        k++;          
      } 
    }
  }

  INFO("Initiated %d candidate(s) in the UE-specific search space for C-RNTI: 0x%x\n", k, rnti);
  
  return k; 
}



/**
 * 36.213 9.1.1
 * Computes up to max_candidates candidates in the common search space 
 * for DCI messages and saves them in the structure pointed by c.  
 * Returns the number of candidates saved in the array c.   
 */
uint32_t pdcch_common_locations(pdcch_t *q, dci_location_t *c, uint32_t max_candidates, 
                                 uint32_t cfi) {
  uint32_t i, l, L, k;

  set_cfi(q, cfi);

  k = 0;
  for (l = 3; l > 1; l--) {
    L = (1 << l);
    for (i = 0; i < MIN(q->nof_cce, 16) / (L); i++) {
      if (k < max_candidates) {
        c[k].L = l;
        c[k].ncce = (L) * (i % (q->nof_cce / (L)));
        DEBUG("Common SS Candidate %d: nCCE: %d, L: %d\n",
            k, c[k].ncce, c[k].L);
        k++;          
      }
    }
  }
  
  INFO("Initiated %d candidate(s) in the Common search space\n", k);
  
  return k;
}






/** 36.212 5.3.3.2 to 5.3.3.4
 *
 * Returns XOR between parity and remainder bits
 *
 * TODO: UE transmit antenna selection CRC mask
 */
static int dci_decode(pdcch_t *q, float *e, char *data, uint32_t E, uint32_t nof_bits, uint16_t *crc) {

  float tmp[3 * (DCI_MAX_BITS + 16)];
  uint16_t p_bits, crc_res;
  char *x;

  if (q         != NULL         &&
      data      != NULL         &&
      E         < q->max_bits   && 
      nof_bits  < DCI_MAX_BITS)
  {

    if (VERBOSE_ISDEBUG()) {
      vec_fprint_f(stdout, e, E);
    }

    /* unrate matching */
    rm_conv_rx(e, E, tmp, 3 * (nof_bits + 16));

    DEBUG("Viterbi input: ", 0);
    if (VERBOSE_ISDEBUG()) {
      vec_fprint_f(stdout, tmp, 3 * (nof_bits + 16));
    }

    /* viterbi decoder */
    viterbi_decode_f(&q->decoder, tmp, data, nof_bits + 16);

    if (VERBOSE_ISDEBUG()) {
      bit_fprint(stdout, data, nof_bits + 16);
    }

    x = &data[nof_bits];
    p_bits = (uint16_t) bit_unpack(&x, 16);
    crc_res = ((uint16_t) crc_checksum(&q->crc, data, nof_bits) & 0xffff);
    DEBUG("p_bits: 0x%x, crc_res: 0x%x, tot: 0x%x\n", p_bits, crc_res,
        p_bits ^ crc_res);
    
    if (crc) {
      *crc = p_bits ^ crc_res; 
    }
    return LIBLTE_SUCCESS;
  } else {
    return LIBLTE_ERROR_INVALID_INPUTS;
  }
}

/** Tries to decode a DCI message from the LLRs stored in the pdcch_t structure by the function 
 * pdcch_extract_llr(). This function can be called multiple times. 
 * The decoded message is stored in msg. Up to nof_locations are tried from the array of dci_locations_t
 * pointed by locations. The CRC is checked agains the RNTI parameter. 
 * 
 * Returns 1 if the message is correctly decoded, 0 if not and -1 on error.  
 */
int pdcch_decode_msg(pdcch_t *q, dci_msg_t *msg, 
                     dci_location_t *locations, uint32_t nof_locations,
                     dci_format_t format, uint16_t rnti) 
{
  if (q                 != NULL && 
      msg               != NULL && 
      locations         != NULL && 
      nof_locations     >  0)
  {
    uint16_t crc_res;
    uint32_t nof_bits = dci_format_sizeof(format, q->cell.nof_prb);
    uint32_t i;
    
    i = 0;
    do {
      INFO("Trying Candidate: Nbits: %d, E: %3d, nCCE: %d, L: %d, RNTI: 0x%x\n",
          nof_bits, PDCCH_FORMAT_NOF_BITS(locations[i].L), locations[i].ncce, locations[i].L, rnti);    
      
      if (dci_decode(q, &q->pdcch_llr[72 * locations[i].ncce], msg->data,
          PDCCH_FORMAT_NOF_BITS(locations[i].L), nof_bits, &crc_res) != LIBLTE_SUCCESS) {
        return LIBLTE_ERROR;
      }
      if (crc_res != rnti) {
        i++;
      }
    } while(i < nof_locations && crc_res != rnti);
    
    if (rnti == crc_res) {
      msg->nof_bits = nof_bits;
      INFO("FOUND Candidate: Nbits: %d, E: %d, nCCE: %d, L: %d, RNTI: 0x%x\n",
          nof_bits, PDCCH_FORMAT_NOF_BITS(locations[i].L), locations[i].ncce, locations[i].L, rnti);
      return 1;
    } else {
      return LIBLTE_SUCCESS;      
    }    
  }
  return LIBLTE_ERROR_INVALID_INPUTS;
}

/** Extracts the LLRs from the subframe symbols (demodulation) and stores them in the pdcch_t structure. 
 * DCI messages can be extracted calling the function pdcch_decode_msg(). 
 * Every time this function is called, the last demodulated symbols are overwritten. 
 */
int pdcch_extract_llr(pdcch_t *q, cf_t *sf_symbols, cf_t *ce[MAX_PORTS], uint32_t nsubframe, uint32_t cfi) {

  /* Set pointers for layermapping & precoding */
  uint32_t i;
  cf_t *x[MAX_LAYERS];

  if (q                 != NULL && 
      nsubframe         <  10   &&
      cfi               >  0    &&
      cfi               <  4)
  {
    set_cfi(q, cfi);
    
    /* number of layers equals number of ports */
    for (i = 0; i < q->cell.nof_ports; i++) {
      x[i] = q->pdcch_x[i];
    }
    memset(&x[q->cell.nof_ports], 0, sizeof(cf_t*) * (MAX_LAYERS - q->cell.nof_ports));

    /* extract symbols */
    int n = regs_pdcch_get(q->regs, sf_symbols, q->pdcch_symbols[0]);
    if (q->nof_symbols != n) {
      fprintf(stderr, "Expected %d PDCCH symbols but got %d symbols\n",
          q->nof_symbols, n);
      return LIBLTE_ERROR;
    }

    /* extract channel estimates */
    for (i = 0; i < q->cell.nof_ports; i++) {
      n = regs_pdcch_get(q->regs, ce[i], q->ce[i]);
      if (q->nof_symbols != n) {
        fprintf(stderr, "Expected %d PDCCH symbols but got %d symbols\n",
            q->nof_symbols, n);
        return LIBLTE_ERROR;
      }
    }

    /* in control channels, only diversity is supported */
    if (q->cell.nof_ports == 1) {
      /* no need for layer demapping */
      predecoding_single_zf(q->pdcch_symbols[0], q->ce[0], q->pdcch_d,
          q->nof_symbols);
    } else {
      predecoding_diversity_zf(q->pdcch_symbols[0], q->ce, x, q->cell.nof_ports,
          q->nof_symbols);
      layerdemap_diversity(x, q->pdcch_d, q->cell.nof_ports,
          q->nof_symbols / q->cell.nof_ports);
    }

    DEBUG("pdcch d symbols: ", 0);
    if (VERBOSE_ISDEBUG()) {
      vec_fprint_c(stdout, q->pdcch_d, q->nof_symbols);
    }

    /* demodulate symbols */
    demod_soft_sigma_set(&q->demod, 1.0);
    demod_soft_demodulate(&q->demod, q->pdcch_d, q->pdcch_llr, q->nof_symbols);

    DEBUG("llr: ", 0);
    if (VERBOSE_ISDEBUG()) {
      vec_fprint_f(stdout, q->pdcch_llr, q->nof_bits);
    }

    /* descramble */
    scrambling_f_offset(&q->seq_pdcch[nsubframe], q->pdcch_llr, 0, q->nof_bits);

    return LIBLTE_SUCCESS;
  } 
  return LIBLTE_ERROR_INVALID_INPUTS;  
}




static void crc_set_mask_rnti(char *crc, uint16_t rnti) {
  uint32_t i;
  char mask[16];
  char *r = mask;

  INFO("Mask CRC with RNTI 0x%x\n", rnti);

  bit_pack(rnti, &r, 16);
  for (i = 0; i < 16; i++) {
    crc[i] = (crc[i] + mask[i]) % 2;
  }
}

/** 36.212 5.3.3.2 to 5.3.3.4
 * TODO: UE transmit antenna selection CRC mask
 */
static int dci_encode(pdcch_t *q, char *data, char *e, uint32_t nof_bits, uint32_t E,
    uint16_t rnti) {
  convcoder_t encoder;
  char tmp[3 * (DCI_MAX_BITS + 16)];
  
  if (q                 != NULL        && 
      data              != NULL        && 
      e                 != NULL        && 
      nof_bits          < DCI_MAX_BITS &&
      E                 < q->max_bits)
  {

    int poly[3] = { 0x6D, 0x4F, 0x57 };
    encoder.K = 7;
    encoder.R = 3;
    encoder.tail_biting = true;
    memcpy(encoder.poly, poly, 3 * sizeof(int));

    crc_attach(&q->crc, data, nof_bits);
    crc_set_mask_rnti(&data[nof_bits], rnti);

    convcoder_encode(&encoder, data, tmp, nof_bits + 16);

    DEBUG("CConv output: ", 0);
    if (VERBOSE_ISDEBUG()) {
      vec_fprint_b(stdout, tmp, 3 * (nof_bits + 16));
    }

    rm_conv_tx(tmp, 3 * (nof_bits + 16), e, E);
    
    return LIBLTE_SUCCESS;
  } else {
    return LIBLTE_ERROR_INVALID_INPUTS;
  }
}

void pdcch_reset(pdcch_t *q) {
    /* should add <NIL> elements? Or maybe random bits to facilitate power estimation */
    bzero(q->pdcch_e, q->nof_bits);
}

/** Encodes ONE DCI message and allocates the encoded bits to the dci_location_t indicated by 
 * the parameter location. The CRC is scrambled with the RNTI parameter. 
 * This function can be called multiple times and encoded DCI messages will be stored in the 
 * pdcch_t structure. A final call to the function pdcch_gen_symbols() will generate and map the 
 * symbols to the subframe for transmission. 
 * If the same location is provided in multiple messages, the encoded bits will be overwritten. 
 * 
 * @TODO: Use a bitmask and CFI to ensure message locations are valid and old messages are not overwritten. 
 */
int pdcch_encode_msg(pdcch_t *q, dci_msg_t *msg, dci_location_t location, uint16_t rnti) {

  int ret = LIBLTE_ERROR_INVALID_INPUTS;
  
  if (q != NULL) {
    ret = LIBLTE_ERROR;
    
    if (location.L < 4       &&
        msg->nof_bits < DCI_MAX_BITS) 
    {      
      INFO("Encoding DCI: Nbits: %d, E: %d, nCCE: %d, L: %d, RNTI: 0x%x\n",
          msg->nof_bits,
          PDCCH_FORMAT_NOF_BITS(location.L),
          location.ncce, location.L, rnti);

      dci_encode(q, msg->data, &q->pdcch_e[72 * location.ncce], msg->nof_bits, 
                 PDCCH_FORMAT_NOF_BITS(location.L), rnti);
    
      ret = LIBLTE_SUCCESS;
      
    } else {
        fprintf(stderr, "Illegal DCI message nCCE: %d, L: %d, nof_cce: %d\n", 
                location.ncce, location.L, q->nof_cce);
    }
  } 
  return ret;
}

/** Converts the set of DCI messages encoded using the function pdcch_encode_msg() 
 * to symbols mapped to the subframe ready for transmission
 */
int pdcch_gen_symbols(pdcch_t *q, cf_t *slot_symbols[MAX_PORTS], uint32_t nsubframe, uint32_t cfi) {
  int i;
  /* Set pointers for layermapping & precoding */
  cf_t *x[MAX_LAYERS];
  
  if (q                 != NULL && 
      slot_symbols      != NULL && 
      nsubframe         <  10   &&
      cfi               >  0    &&
      cfi               <  4)
  {
    set_cfi(q, cfi);

    /* number of layers equals number of ports */
    for (i = 0; i < q->cell.nof_ports; i++) {
      x[i] = q->pdcch_x[i];
    }
    memset(&x[q->cell.nof_ports], 0, sizeof(cf_t*) * (MAX_LAYERS - q->cell.nof_ports));

    scrambling_b_offset(&q->seq_pdcch[nsubframe], q->pdcch_e, 0, q->nof_bits);

    mod_modulate(&q->mod, q->pdcch_e, q->pdcch_d, q->nof_bits);

    /* layer mapping & precoding */
    if (q->cell.nof_ports > 1) {
      layermap_diversity(q->pdcch_d, x, q->cell.nof_ports, q->nof_symbols);
      precoding_diversity(x, q->pdcch_symbols, q->cell.nof_ports,
          q->nof_symbols / q->cell.nof_ports);
    } else {
      memcpy(q->pdcch_symbols[0], q->pdcch_d, q->nof_symbols * sizeof(cf_t));
    }

    /* mapping to resource elements */
    for (i = 0; i < q->cell.nof_ports; i++) {
      regs_pdcch_put(q->regs, q->pdcch_symbols[i], slot_symbols[i]);
    }
    return LIBLTE_SUCCESS;
  } else {
    return LIBLTE_ERROR_INVALID_INPUTS;
  }
}


