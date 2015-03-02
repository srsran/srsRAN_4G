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

#include "liblte/phy/ue/ue_dl.h"

#include <complex.h>
#include <math.h>

#define CURRENT_FFTSIZE   lte_symbol_sz(q->cell.nof_prb)
#define CURRENT_SFLEN     SF_LEN(CURRENT_FFTSIZE, q->cell.cp)

#define CURRENT_SLOTLEN_RE SLOT_LEN_RE(q->cell.nof_prb, q->cell.cp)
#define CURRENT_SFLEN_RE SF_LEN_RE(q->cell.nof_prb, q->cell.cp)

#define MAX_CANDIDATES  64

int ue_dl_init(ue_dl_t *q, 
               lte_cell_t cell) 
{
  int ret = LIBLTE_ERROR_INVALID_INPUTS; 
  
  if (q                 != NULL &&
      lte_cell_isvalid(&cell))   
  {
    ret = LIBLTE_ERROR;
    
    bzero(q, sizeof(ue_dl_t));
    
    q->cell = cell; 
    q->pkt_errors = 0;
    q->pkts_total = 0;
    
    if (lte_fft_init(&q->fft, q->cell.cp, q->cell.nof_prb)) {
      fprintf(stderr, "Error initiating FFT\n");
      goto clean_exit;
    }
    if (chest_dl_init(&q->chest, cell)) {
      fprintf(stderr, "Error initiating channel estimator\n");
      goto clean_exit;
    }
    if (regs_init(&q->regs, q->cell)) {
      fprintf(stderr, "Error initiating REGs\n");
      goto clean_exit;
    }
    if (pbch_init(&q->pbch, q->cell)) {
      fprintf(stderr, "Error creating PBCH object\n");
      goto clean_exit; 
    }
    if (pcfich_init(&q->pcfich, &q->regs, q->cell)) {
      fprintf(stderr, "Error creating PCFICH object\n");
      goto clean_exit;
    }

    if (pdcch_init(&q->pdcch, &q->regs, q->cell)) {
      fprintf(stderr, "Error creating PDCCH object\n");
      goto clean_exit;
    }

    if (pdsch_init(&q->pdsch, q->cell)) {
      fprintf(stderr, "Error creating PDSCH object\n");
      goto clean_exit;
    }
    for (uint32_t i=0;i<NOF_HARQ_PROCESSES; i++) {
      if (harq_init(&q->harq_process[i], q->cell)) {
        fprintf(stderr, "Error initiating HARQ process\n");
        goto clean_exit;
      }
    }
    q->sf_symbols = vec_malloc(CURRENT_SFLEN_RE * sizeof(cf_t));
    if (!q->sf_symbols) {
      perror("malloc");
      goto clean_exit; 
    }
    for (uint32_t i=0;i<q->cell.nof_ports;i++) {
      q->ce[i] = vec_malloc(CURRENT_SFLEN_RE * sizeof(cf_t));
      if (!q->ce[i]) {
        perror("malloc");
        goto clean_exit; 
      }
    }
    
    ret = LIBLTE_SUCCESS;
  } else {
    fprintf(stderr, "Invalid cell properties: Id=%d, Ports=%d, PRBs=%d\n",
            cell.id, cell.nof_ports, cell.nof_prb);      
  }

clean_exit: 
  if (ret == LIBLTE_ERROR) {
    ue_dl_free(q);
  }
  return ret;
}

void ue_dl_free(ue_dl_t *q) {
  if (q) {
    lte_fft_free(&q->fft);
    chest_dl_free(&q->chest);
    regs_free(&q->regs);
    pbch_free(&q->pbch);
    pcfich_free(&q->pcfich);
    pdcch_free(&q->pdcch);
    pdsch_free(&q->pdsch);
    for (uint32_t i=0;i<NOF_HARQ_PROCESSES; i++) {
      harq_free(&q->harq_process[i]);
    }
    if (q->sf_symbols) {
      free(q->sf_symbols);
    }
    for (uint32_t i=0;i<q->cell.nof_ports;i++) {
      if (q->ce[i]) {
        free(q->ce[i]);
      }
    }

    bzero(q, sizeof(ue_dl_t));

  }
}

/* Precalculate the PDSCH scramble sequences for a given RNTI. This function takes a while 
 * to execute, so shall be called once the final C-RNTI has been allocated for the session.
 * For the connection procedure, use pusch_encode_rnti() or pusch_decode_rnti() functions 
 */
void ue_dl_set_rnti(ue_dl_t *q, uint16_t rnti) {
  q->current_rnti = rnti; 
  pdsch_set_rnti(&q->pdsch, rnti);
}

void ue_dl_reset(ue_dl_t *q) {
  harq_reset(&q->harq_process[0]);
}

LIBLTE_API float mean_exec_time=0; 

dci_format_t ue_formats[] = {Format1,Format1A}; // Format1B should go here also
const uint32_t nof_ue_formats = 2; 

dci_format_t common_formats[] = {Format1A,Format1C};
const uint32_t nof_common_formats = 2; 

/** Applies the following operations to a subframe of synchronized samples: 
 *    - OFDM demodulation
 *    - Channel estimation 
 *    - PCFICH decoding
 *    - PDCCH decoding: Find DCI for RNTI given by previous call to ue_dl_set_rnti()
 *    - PDSCH decoding: Decode TB scrambling with RNTI given by ue_dl_set_rnti()
 */
int ue_dl_decode(ue_dl_t *q, cf_t *input, uint8_t *data, uint32_t sf_idx) {
  return ue_dl_decode_sib(q, input, data, sf_idx, q->current_rnti, 0);
}

int ue_dl_decode_rnti(ue_dl_t *q, cf_t *input, uint8_t *data, uint16_t rnti, uint32_t sf_idx) {
  return ue_dl_decode_sib(q, input, data, sf_idx, rnti, 0);
}

int ue_dl_decode_sib(ue_dl_t *q, cf_t *input, uint8_t *data, uint32_t sf_idx, uint16_t rnti, uint32_t rvidx) 
{
  uint32_t cfi, i;
  float cfi_corr; 
  ra_pdsch_t ra_dl;
  dci_location_t locations[MAX_CANDIDATES];
  dci_msg_t dci_msg;
  uint32_t nof_locations;
  uint16_t crc_rem; 
  int ret = LIBLTE_ERROR; 
  struct timeval t[3]; 
  uint32_t nof_formats; 
  dci_format_t *formats = NULL; 

  /* Run FFT for all subframe data */
  lte_fft_run_sf(&q->fft, input, q->sf_symbols);

  gettimeofday(&t[1], NULL);

  /* Get channel estimates for each port */
  chest_dl_estimate(&q->chest, q->sf_symbols, q->ce, sf_idx);
  
  /* First decode PCFICH and obtain CFI */
  if (pcfich_decode(&q->pcfich, q->sf_symbols, q->ce, 
                    chest_dl_get_noise_estimate(&q->chest), sf_idx, &cfi, &cfi_corr)<0) {
    fprintf(stderr, "Error decoding PCFICH\n");
    return LIBLTE_ERROR;
  }

  INFO("Decoded CFI=%d with correlation %.2f\n", cfi, cfi_corr);

  if (regs_set_cfi(&q->regs, cfi)) {
    fprintf(stderr, "Error setting CFI\n");
    return LIBLTE_ERROR;
  }
  
  /* Generate PDCCH candidates */
  if (rnti == SIRNTI) {
    nof_locations = pdcch_common_locations(&q->pdcch, locations, MAX_CANDIDATES, cfi);
    formats = common_formats;
    nof_formats = nof_common_formats;
  } else {
    nof_locations = pdcch_ue_locations(&q->pdcch, locations, MAX_CANDIDATES, sf_idx, cfi, rnti);    
    formats = ue_formats; 
    nof_formats = nof_ue_formats;
  }

  /* Extract all PDCCH symbols and get LLRs */
  if (pdcch_extract_llr(&q->pdcch, q->sf_symbols, q->ce, chest_dl_get_noise_estimate(&q->chest), sf_idx, cfi)) {
    fprintf(stderr, "Error extracting LLRs\n");
    return LIBLTE_ERROR;
  }
  /* For all possible locations, try to decode a DCI message */
  crc_rem = 0;
  uint32_t found_dci = 0; 
  for (int f=0;f<nof_formats && !found_dci;f++) {
    for (i=0;i<nof_locations && !found_dci;i++) {
      if (pdcch_decode_msg(&q->pdcch, &dci_msg, &locations[i], formats[f], &crc_rem)) {
        fprintf(stderr, "Error decoding DCI msg\n");
        return LIBLTE_ERROR;
      }
      INFO("Decoded DCI message RNTI: 0x%x\n", crc_rem);
      
      if (crc_rem == rnti) {

        found_dci++;
        q->nof_pdcch_detected++;
        if (dci_msg_to_ra_dl(&dci_msg, rnti, q->cell, cfi, &ra_dl)) {
          fprintf(stderr, "Error unpacking PDSCH scheduling DCI message\n");
          return LIBLTE_ERROR;
        }

        if (rnti != SIRNTI) {
          rvidx = ra_dl.rv_idx;
        }
        if (harq_setup_dl(&q->harq_process[0], ra_dl.mcs, rvidx, sf_idx, &ra_dl.prb_alloc)) {
          fprintf(stderr, "Error configuring HARQ process\n");
          return LIBLTE_ERROR;
        }
        if (q->harq_process[0].mcs.mod > 0 && q->harq_process[0].mcs.tbs >= 0) {
          ret = pdsch_decode_rnti(&q->pdsch, &q->harq_process[0], q->sf_symbols, 
                                  q->ce, chest_dl_get_noise_estimate(&q->chest),
                                  rnti, data);
          if (ret == LIBLTE_ERROR) {
            q->pkt_errors++;
          } else if (ret == LIBLTE_ERROR_INVALID_INPUTS) {
            fprintf(stderr, "Error calling pdsch_decode()\n");
            return LIBLTE_ERROR; 
          } else if (ret == LIBLTE_SUCCESS) {
            if (VERBOSE_ISINFO()) {
              INFO("Decoded Message: ", 0);
              vec_fprint_hex(stdout, data, ra_dl.mcs.tbs);
            }
          }
          q->pkts_total++;
        }
      }
    }
  }
    

  gettimeofday(&t[2], NULL);
  get_time_interval(t);
  mean_exec_time = (float) VEC_EMA((float) t[0].tv_usec, mean_exec_time, 0.01);
 
  
  if (found_dci > 0 && ret == LIBLTE_SUCCESS) { 
    return ra_dl.mcs.tbs;    
  } else {
    return 0;
  }
}
