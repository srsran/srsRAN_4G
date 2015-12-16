/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2015 Software Radio Systems Limited
 *
 * \section LICENSE
 *
 * This file is part of the srsLTE library.
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <sys/select.h>
#include <pthread.h>
#include <semaphore.h>

#include "srslte/srslte.h"

#include "srslte/rf/rf.h"
srslte_rf_t rf;

char *output_file_name = NULL;

srslte_cell_t cell = {
  6,            // nof_prb
  1,            // nof_ports
  1,            // cell_id
  SRSLTE_CP_NORM,       // cyclic prefix
  SRSLTE_PHICH_R_1,          // PHICH resources      
  SRSLTE_PHICH_NORM    // PHICH length
};
  

char *rf_args = "";
float rf_amp = 0.5, rf_gain = 30.0, rf_freq = 2400000000;

bool null_file_sink=false; 
srslte_filesink_t fsink;
srslte_ofdm_t ifft;
srslte_mod_t modulation; 

uint32_t sf_n_re, sf_n_samples;

cf_t *sf_buffer = NULL, *output_buffer = NULL;

void usage(char *prog) {
  printf("Usage: %s [algfmv]\n", prog);
  printf("\t-a RF args [Default %s]\n", rf_args);
  printf("\t-l RF amplitude [Default %.2f]\n", rf_amp);
  printf("\t-g RF TX gain [Default %.2f dB]\n", rf_gain);
  printf("\t-f RF TX frequency [Default %.1f MHz]\n", rf_freq / 1000000);
  printf("\t-m modulation (1: BPSK, 2: QPSK, 3: QAM16, 4: QAM64) [Default BPSK]\n");
  printf("\t-v [set srslte_verbose to debug, default none]\n");
}

void parse_args(int argc, char **argv) {
  int opt;
  while ((opt = getopt(argc, argv, "algfmv")) != -1) {
    switch (opt) {
    case 'a':
      rf_args = argv[optind];
      break;
    case 'g':
      rf_gain = atof(argv[optind]);
      break;
    case 'l':
      rf_amp = atof(argv[optind]);
      break;
    case 'f':
      rf_freq = atof(argv[optind]);
      break;
    case 'm':
      switch(atoi(argv[optind])) {
      case 1:
        modulation = SRSLTE_MOD_BPSK;
        break;
      case 2:
        modulation = SRSLTE_MOD_QPSK;
        break;
      case 4:
        modulation = SRSLTE_MOD_16QAM;
        break;
      case 6:
        modulation = SRSLTE_MOD_64QAM;
        break;
      default:
        fprintf(stderr, "Invalid modulation %d. Possible values: "
            "(1: BPSK, 2: QPSK, 3: QAM16, 4: QAM64)\n", atoi(argv[optind]));
        break;
      }
      break;
    case 'v':
      srslte_verbose++;
      break;
    default:
      usage(argv[0]);
      exit(-1);
    }
  }
#ifdef DISABLE_RF
  if (!output_file_name) {
    usage(argv[0]);
    exit(-1);
  }
#endif
}

void base_init() {
  
  /* init memory */
  sf_buffer = malloc(sizeof(cf_t) * sf_n_re);
  if (!sf_buffer) {
    perror("malloc");
    exit(-1);
  }
  output_buffer = malloc(sizeof(cf_t) * sf_n_samples);
  if (!output_buffer) {
    perror("malloc");
    exit(-1);
  }
  printf("Opening RF device...\n");
  if (srslte_rf_open(&rf, rf_args)) {
    fprintf(stderr, "Error opening rf\n");
    exit(-1);
  }
  
  /* create ifft object */
  if (srslte_ofdm_tx_init(&ifft, SRSLTE_CP_NORM, cell.nof_prb)) {
    fprintf(stderr, "Error creating iFFT object\n");
    exit(-1);
  }
  srslte_ofdm_set_normalize(&ifft, true);
}

void base_free() {

  srslte_ofdm_tx_free(&ifft);

  if (sf_buffer) {
    free(sf_buffer);
  }
  if (output_buffer) {
    free(output_buffer);
  }
  srslte_rf_close(&rf);
}


int main(int argc, char **argv) {
  int sf_idx=0, N_id_2=0;
  cf_t pss_signal[SRSLTE_PSS_LEN];
  float sss_signal0[SRSLTE_SSS_LEN]; // for subframe 0
  float sss_signal5[SRSLTE_SSS_LEN]; // for subframe 5
  int i;
  
#ifdef DISABLE_RF
  if (argc < 3) {
    usage(argv[0]);
    exit(-1);
  }
#endif

  parse_args(argc, argv);

  N_id_2 = cell.id % 3;
  sf_n_re = 2 * SRSLTE_CP_NORM_NSYMB * cell.nof_prb * SRSLTE_NRE;
  sf_n_samples = 2 * SRSLTE_SLOT_LEN(srslte_symbol_sz(cell.nof_prb));

  cell.phich_length = SRSLTE_PHICH_NORM;
  cell.phich_resources = SRSLTE_PHICH_R_1;

  /* this *must* be called after setting slot_len_* */
  base_init();

  /* Generate PSS/SSS signals */
  srslte_pss_generate(pss_signal, N_id_2);
  srslte_sss_generate(sss_signal0, sss_signal5, cell.id);
  
  printf("Set TX rate: %.2f MHz\n",
      srslte_rf_set_tx_srate(&rf, srslte_sampling_freq_hz(cell.nof_prb)) / 1000000);
  printf("Set TX gain: %.1f dB\n", srslte_rf_set_tx_gain(&rf, rf_gain));
  printf("Set TX freq: %.2f MHz\n",
      srslte_rf_set_tx_freq(&rf, rf_freq) / 1000000);

  uint32_t nbits; 
  
  srslte_modem_table_t modulator; 
  srslte_modem_table_init(&modulator);
  srslte_modem_table_lte(&modulator, modulation);

  srslte_tcod_t turbocoder; 
  srslte_tcod_init(&turbocoder, SRSLTE_TCOD_MAX_LEN_CB);

  srslte_dft_precoding_t dft_precod;
  srslte_dft_precoding_init(&dft_precod, 12);
  
  nbits = srslte_cbsegm_cbindex(sf_n_samples/8/srslte_mod_bits_x_symbol(modulation)/3 - 12);
  uint32_t ncoded_bits = sf_n_samples/8/srslte_mod_bits_x_symbol(modulation); 
  
  uint8_t *data     = malloc(sizeof(uint8_t)*nbits);
  uint8_t *data_enc = malloc(sizeof(uint8_t)*ncoded_bits);
  cf_t    *symbols  = malloc(sizeof(cf_t)*sf_n_samples);
  
  bzero(data_enc, sizeof(uint8_t)*ncoded_bits);
  while (1) {
    for (sf_idx = 0; sf_idx < SRSLTE_NSUBFRAMES_X_FRAME; sf_idx++) {
      bzero(sf_buffer, sizeof(cf_t) * sf_n_re);

#ifdef kk
      if (sf_idx == 0 || sf_idx == 5) {
        srslte_pss_put_slot(pss_signal, sf_buffer, cell.nof_prb, SRSLTE_CP_NORM);
        srslte_sss_put_slot(sf_idx ? sss_signal5 : sss_signal0, sf_buffer, cell.nof_prb,
            SRSLTE_CP_NORM);
        /* Transform to OFDM symbols */
        srslte_ofdm_tx_sf(&ifft, sf_buffer, output_buffer);
        
        float norm_factor = (float) sqrtf(cell.nof_prb)/15;
        srslte_vec_sc_prod_cfc(output_buffer, rf_amp*norm_factor, output_buffer, SRSLTE_SF_LEN_PRB(cell.nof_prb));
      
      } else {
#endif
        /* Generate random data */
        for (i=0;i<nbits;i++) {
          data[i] = rand()%2;
        }
        srslte_tcod_encode(&turbocoder, data, data_enc, nbits);
        srslte_mod_modulate(&modulator, data_enc, symbols, ncoded_bits);        
        srslte_interp_linear_offset_cabs(symbols, output_buffer, 8, sf_n_samples/8, 0, 0);
//    }
      
      /* send to usrp */
      srslte_vec_sc_prod_cfc(output_buffer, rf_amp, output_buffer, sf_n_samples);
      srslte_rf_send(&rf, output_buffer, sf_n_samples, true);
    }
  }

  base_free();

  printf("Done\n");
  exit(0);
}


