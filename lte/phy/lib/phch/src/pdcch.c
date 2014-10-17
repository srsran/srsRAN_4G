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


#define NOF_COMMON_FORMATS      2
const dci_format_t common_formats[NOF_COMMON_FORMATS] = { Format1A, Format1C };

#define NOF_UE_FORMATS          2
const dci_format_t ue_formats[NOF_UE_FORMATS] = { Format0, Format1 }; // 1A has the same payload as 0


static void set_cfi(pdcch_t *q, uint32_t cfi) {
  if (cfi > 0 && cfi < 4) {
    q->nof_regs = (regs_pdcch_nregs(q->regs, cfi) / 9) * 9;
    q->nof_cce = q->nof_regs / 9;
  } 
}


/** Initializes the PDCCH transmitter and receiver */
int pdcch_init(pdcch_t *q, regs_t *regs, lte_cell_t cell) {
  int ret = LIBLTE_ERROR_INVALID_INPUTS;
  uint32_t i;

  if (q                         != NULL &&
      regs                      != NULL &&
      lte_cell_isvalid(&cell)) 
  {   
    ret = LIBLTE_ERROR;
    bzero(q, sizeof(pdcch_t));
    q->cell = cell;
    q->regs = regs;

    /* Allocate memory for the largest aggregation level L=3 */
    q->max_bits = PDCCH_FORMAT_NOF_BITS(3);

    INFO("Init PDCCH: %d bits, %d symbols, %d ports\n", q->max_bits, q->max_bits/2, q->cell.nof_ports);

    if (modem_table_lte(&q->mod, LTE_QPSK, true)) {
      goto clean;
    }
    if (crc_init(&q->crc, LTE_CRC16, 16)) {
      goto clean;
    }

    demod_soft_init(&q->demod);
    demod_soft_table_set(&q->demod, &q->mod);
    demod_soft_alg_set(&q->demod, APPROX);

    for (i = 0; i < NSUBFRAMES_X_FRAME; i++) {
      // we need to pregenerate the sequence for the maximum number of bits, which is 8 times 
      // the maximum number of REGs (for CFI=3)
      if (sequence_pdcch(&q->seq_pdcch[i], 2 * i, q->cell.id, 8*regs_pdcch_nregs(q->regs, 3))) {
        goto clean;
      }
    }

    uint32_t poly[3] = { 0x6D, 0x4F, 0x57 };
    if (viterbi_init(&q->decoder, viterbi_37, poly, DCI_MAX_BITS + 16, true)) {
      goto clean;
    }

    q->pdcch_e = malloc(sizeof(uint8_t) * q->max_bits);
    if (!q->pdcch_e) {
      goto clean;
    }

    q->pdcch_llr = malloc(sizeof(float) * q->max_bits);
    if (!q->pdcch_llr) {
      goto clean;
    }

    q->pdcch_d = malloc(sizeof(cf_t) * q->max_bits / 2);
    if (!q->pdcch_d) {
      goto clean;
    }

    for (i = 0; i < MAX_PORTS; i++) {
      q->ce[i] = malloc(sizeof(cf_t) * q->max_bits / 2);
      if (!q->ce[i]) {
        goto clean;
      }
      q->pdcch_x[i] = malloc(sizeof(cf_t) * q->max_bits / 2);
      if (!q->pdcch_x[i]) {
        goto clean;
      }
      q->pdcch_symbols[i] = malloc(sizeof(cf_t) * q->max_bits / 2);
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
  
  int l; // this must be int because of the for(;;--) loop
  uint32_t i, k, L, m; 
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
          ncce + PDCCH_FORMAT_NOF_CCE(l) <= q->nof_cce) 
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
                                uint32_t cfi) 
{
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
static int dci_decode(pdcch_t *q, float *e, uint8_t *data, uint32_t E, uint32_t nof_bits, uint16_t *crc) {

  float tmp[3 * (DCI_MAX_BITS + 16)];
  uint16_t p_bits, crc_res;
  uint8_t *x;

  if (q         != NULL         &&
      data      != NULL         &&
      E         <= q->max_bits   && 
      nof_bits  <= DCI_MAX_BITS)
  {

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
    DEBUG("p_bits: 0x%x, crc_checksum: 0x%x, crc_rem: 0x%x\n", p_bits, crc_res,
        p_bits ^ crc_res);
    
    if (crc) {
      *crc = p_bits ^ crc_res; 
    }
    return LIBLTE_SUCCESS;
  } else {
    fprintf(stderr, "Invalid parameters: E: %d, max_bits: %d, nof_bits: %d\n", E, q->max_bits, nof_bits);
    return LIBLTE_ERROR_INVALID_INPUTS;
  }
}

/** Tries to decode a DCI message from the LLRs stored in the pdcch_t structure by the function 
 * pdcch_extract_llr(). This function can be called multiple times. 
 * The decoded message is stored in msg and the CRC remainder in crc_rem pointer
 * 
 */
int pdcch_decode_msg(pdcch_t *q, dci_msg_t *msg, dci_format_t format, uint16_t *crc_rem) 
{
  int ret = LIBLTE_ERROR_INVALID_INPUTS;
  if (q                 != NULL && 
      msg               != NULL && 
      crc_rem           != NULL)
  {
    uint32_t nof_bits = dci_format_sizeof(format, q->cell.nof_prb);
    
    ret = dci_decode(q, q->pdcch_llr, msg->data, q->e_bits, nof_bits, crc_rem);
    if (ret == LIBLTE_SUCCESS) {
      msg->nof_bits = nof_bits;
    }
  }
  return ret;
}

/** Extracts the LLRs from dci_location_t location of the subframe and stores them in the pdcch_t structure. 
 * DCI messages can be extracted from this location calling the function pdcch_decode_msg(). 
 * Every time this function is called (with a different location), the last demodulated symbols are overwritten and
 * new messages from other locations can be decoded 
 */
int pdcch_extract_llr(pdcch_t *q, cf_t *sf_symbols, cf_t *ce[MAX_PORTS], 
                      dci_location_t location, uint32_t nsubframe, uint32_t cfi) {

  int ret = LIBLTE_ERROR_INVALID_INPUTS;
  
  /* Set pointers for layermapping & precoding */
  uint32_t i, nof_symbols;
  cf_t *x[MAX_LAYERS];

  if (q                 != NULL && 
      nsubframe         <  10   &&
      cfi               >  0    &&
      cfi               <  4    &&
     dci_location_isvalid(&location))
  {
    set_cfi(q, cfi);
    
    q->e_bits = PDCCH_FORMAT_NOF_BITS(location.L);
    nof_symbols = q->e_bits/2;
    ret = LIBLTE_ERROR;
    
    if (location.ncce + PDCCH_FORMAT_NOF_CCE(location.L) <= q->nof_cce) {  
      
      INFO("Extracting LLRs: E: %d, nCCE: %d, L: %d, SF: %d, CFI: %d\n",
          q->e_bits, location.ncce, location.L, nsubframe, cfi);

      /* number of layers equals number of ports */
      for (i = 0; i < q->cell.nof_ports; i++) {
        x[i] = q->pdcch_x[i];
      }
      memset(&x[q->cell.nof_ports], 0, sizeof(cf_t*) * (MAX_LAYERS - q->cell.nof_ports));

      /* extract symbols */
      int n = regs_pdcch_get_offset(q->regs, sf_symbols, q->pdcch_symbols[0], 
                                    location.ncce * 9, PDCCH_FORMAT_NOF_REGS(location.L));
      if (nof_symbols != n) {
        fprintf(stderr, "Expected %d PDCCH symbols but got %d symbols\n", nof_symbols, n);
        return ret;
      }

      /* extract channel estimates */
      for (i = 0; i < q->cell.nof_ports; i++) {
        n = regs_pdcch_get_offset(q->regs, ce[i], q->ce[i], 
                                  location.ncce * 9, PDCCH_FORMAT_NOF_REGS(location.L));
        if (nof_symbols != n) {
          fprintf(stderr, "Expected %d PDCCH symbols but got %d symbols\n", nof_symbols, n);
          return ret;
        }
      }

      /* in control channels, only diversity is supported */
      if (q->cell.nof_ports == 1) {
        /* no need for layer demapping */
        predecoding_single_zf(q->pdcch_symbols[0], q->ce[0], q->pdcch_d, nof_symbols);
      } else {
        predecoding_diversity_zf(q->pdcch_symbols[0], q->ce, x, q->cell.nof_ports, nof_symbols);
        layerdemap_diversity(x, q->pdcch_d, q->cell.nof_ports, nof_symbols / q->cell.nof_ports);
      }

      DEBUG("pdcch d symbols: ", 0);
      if (VERBOSE_ISDEBUG()) {
        vec_fprint_c(stdout, q->pdcch_d, nof_symbols);
      }

      /* demodulate symbols */
      demod_soft_sigma_set(&q->demod, 1.0);
      demod_soft_demodulate(&q->demod, q->pdcch_d, q->pdcch_llr, nof_symbols);

      DEBUG("llr: ", 0);
      if (VERBOSE_ISDEBUG()) {
        vec_fprint_f(stdout, q->pdcch_llr, q->e_bits);
      }

      /* descramble */
      scrambling_f_offset(&q->seq_pdcch[nsubframe], q->pdcch_llr, 72 * location.ncce, q->e_bits);

      ret = LIBLTE_SUCCESS;
    } else {
        fprintf(stderr, "Illegal DCI message nCCE: %d, L: %d, nof_cce: %d\n",  location.ncce, location.L, q->nof_cce);
    }
  } 
  return ret;  
}




static void crc_set_mask_rnti(uint8_t *crc, uint16_t rnti) {
  uint32_t i;
  uint8_t mask[16];
  uint8_t *r = mask;

  INFO("Mask CRC with RNTI 0x%x\n", rnti);

  bit_pack(rnti, &r, 16);
  for (i = 0; i < 16; i++) {
    crc[i] = (crc[i] + mask[i]) % 2;
  }
}

/** 36.212 5.3.3.2 to 5.3.3.4
 * TODO: UE transmit antenna selection CRC mask
 */
static int dci_encode(pdcch_t *q, uint8_t *data, uint8_t *e, uint32_t nof_bits, uint32_t E,
    uint16_t rnti) {
  convcoder_t encoder;
  uint8_t tmp[3 * (DCI_MAX_BITS + 16)];
  
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

/** Encodes ONE DCI message and allocates the encoded bits to the dci_location_t indicated by 
 * the parameter location. The CRC is scrambled with the RNTI parameter. 
 * This function can be called multiple times and encoded DCI messages will be allocated to the 
 * sf_symbols buffer ready for transmission. 
 * If the same location is provided in multiple messages, the encoded bits will be overwritten. 
 * 
 * @TODO: Use a bitmask and CFI to ensure message locations are valid and old messages are not overwritten. 
 */
int pdcch_encode(pdcch_t *q, dci_msg_t *msg, dci_location_t location, uint16_t rnti, 
                 cf_t *sf_symbols[MAX_PORTS], uint32_t nsubframe, uint32_t cfi) {

  int ret = LIBLTE_ERROR_INVALID_INPUTS;
  uint32_t i;
  cf_t *x[MAX_LAYERS];
  uint32_t nof_symbols;
  
  if (q                 != NULL &&
      sf_symbols        != NULL && 
      nsubframe         <  10   &&
      cfi               >  0    &&
      cfi               <  4    && 
      dci_location_isvalid(&location))
  {

    set_cfi(q, cfi);

    q->e_bits = PDCCH_FORMAT_NOF_BITS(location.L);
    nof_symbols = q->e_bits/2;
    ret = LIBLTE_ERROR;
    
    if (location.ncce + PDCCH_FORMAT_NOF_CCE(location.L) <= q->nof_cce && 
        msg->nof_bits < DCI_MAX_BITS) 
    {      
      INFO("Encoding DCI: Nbits: %d, E: %d, nCCE: %d, L: %d, RNTI: 0x%x\n",
          msg->nof_bits, q->e_bits, location.ncce, location.L, rnti);

      dci_encode(q, msg->data, q->pdcch_e, msg->nof_bits, q->e_bits, rnti);
    
      /* number of layers equals number of ports */
      for (i = 0; i < q->cell.nof_ports; i++) {
        x[i] = q->pdcch_x[i];
      }
      memset(&x[q->cell.nof_ports], 0, sizeof(cf_t*) * (MAX_LAYERS - q->cell.nof_ports));

      scrambling_b_offset(&q->seq_pdcch[nsubframe], q->pdcch_e, 72 * location.ncce, q->e_bits);
      
      DEBUG("Scrambling output: ", 0);
      if (VERBOSE_ISDEBUG()) {        
        vec_fprint_b(stdout, q->pdcch_e, q->e_bits);
      }
      
      mod_modulate(&q->mod, q->pdcch_e, q->pdcch_d, q->e_bits);

      /* layer mapping & precoding */
      if (q->cell.nof_ports > 1) {
        layermap_diversity(q->pdcch_d, x, q->cell.nof_ports, nof_symbols);
        precoding_diversity(x, q->pdcch_symbols, q->cell.nof_ports, nof_symbols / q->cell.nof_ports);
      } else {
        memcpy(q->pdcch_symbols[0], q->pdcch_d, nof_symbols * sizeof(cf_t));
      }
      
      /* mapping to resource elements */
      for (i = 0; i < q->cell.nof_ports; i++) {
        regs_pdcch_put_offset(q->regs, q->pdcch_symbols[i], sf_symbols[i], 
                              location.ncce * 9, PDCCH_FORMAT_NOF_REGS(location.L));
      }
      
      ret = LIBLTE_SUCCESS;
      
    } else {
        fprintf(stderr, "Illegal DCI message nCCE: %d, L: %d, nof_cce: %d\n", location.ncce, location.L, q->nof_cce);
    }
  } 
  return ret;
}

