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
#include <math.h>
#include <sys/time.h>

#include <unistd.h>

#include "srslte/srslte.h"

#include "srslte/phy/rf/rf.h"
#include "srslte/phy/rf/rf_utils.h"

int rf_rssi_scan(srslte_rf_t *rf, float *freqs, float *rssi, int nof_bands, double fs, int nsamp) {
  int i, j;
  int ret = -1;
  cf_t *buffer;
  double f;

  buffer = calloc(nsamp, sizeof(cf_t));
  if (!buffer) {
    goto free_and_exit;
  }

  srslte_rf_set_rx_gain(rf, 20.0);
  srslte_rf_set_rx_srate(rf, fs);

  for (i=0;i<nof_bands;i++) {
    srslte_rf_stop_rx_stream(rf);

    f = (double) freqs[i];
    srslte_rf_set_rx_freq(rf, f);
    srslte_rf_rx_wait_lo_locked(rf);
    usleep(10000);
    srslte_rf_start_rx_stream(rf, false);

    /* discard first samples */
    for (j=0;j<2;j++) {
      if (srslte_rf_recv(rf, buffer, nsamp, 1) != nsamp) {
        goto free_and_exit;
      }
    }
    rssi[i] = srslte_vec_avg_power_cf(buffer, nsamp);
    printf("[%3d]: Freq %4.1f Mhz - RSSI: %3.2f dBm\r", i, f/1000000, 10*log10f(rssi[i]) + 30); fflush(stdout);
    if (SRSLTE_VERBOSE_ISINFO()) {
      printf("\n");
    }
  }
  srslte_rf_stop_rx_stream(rf);

  ret = 0;
free_and_exit:
  free(buffer);
  return ret;
}

int srslte_rf_recv_wrapper_cs(void *h, cf_t *data[SRSLTE_MAX_PORTS], uint32_t nsamples, srslte_timestamp_t *t) {
  DEBUG(" ----  Receive %d samples  ---- \n", nsamples);
  void *ptr[SRSLTE_MAX_PORTS];
  for (int i=0;i<SRSLTE_MAX_PORTS;i++) {
    ptr[i] = data[i];
  }
  return srslte_rf_recv_with_time_multi(h, ptr, nsamples, 1, NULL, NULL);
}

double srslte_rf_set_rx_gain_th_wrapper(void *h, double f) {
  return srslte_rf_set_rx_gain_th((srslte_rf_t*) h, f);
}

/** This function is simply a wrapper to the ue_cell_search module for rf devices 
 * Return 1 if the MIB is decoded, 0 if not or -1 on error. 
 */
int rf_mib_decoder(srslte_rf_t *rf, uint32_t nof_rx_antennas,cell_search_cfg_t *config, srslte_cell_t *cell, float *cfo) {
  int ret = SRSLTE_ERROR; 
  srslte_ue_mib_sync_t ue_mib; 
  uint8_t bch_payload[SRSLTE_BCH_PAYLOAD_LEN];

  if (srslte_ue_mib_sync_init_multi(&ue_mib, srslte_rf_recv_wrapper_cs, nof_rx_antennas, (void*) rf)) {
    fprintf(stderr, "Error initiating srslte_ue_mib_sync\n");
    goto clean_exit; 
  }

  if (srslte_ue_mib_sync_set_cell(&ue_mib, cell->id, cell->cp)) {
    fprintf(stderr, "Error initiating srslte_ue_mib_sync\n");
    goto clean_exit;
  }

  int srate = srslte_sampling_freq_hz(SRSLTE_UE_MIB_NOF_PRB);
  INFO("Setting sampling frequency %.2f MHz for PSS search\n", (float) srate/1000000);
  srslte_rf_set_rx_srate(rf, (float) srate);
  
  INFO("Starting receiver...\n");
  srslte_rf_start_rx_stream(rf, false);

  // Copy CFO estimate if provided and disable CP estimation during find
  if (cfo) {
    ue_mib.ue_sync.cfo_current_value = *cfo/15000;
    ue_mib.ue_sync.cfo_is_copied = true;
    ue_mib.ue_sync.cfo_correct_enable_find = true;
    srslte_sync_set_cfo_cp_enable(&ue_mib.ue_sync.sfind, false, 0);
  }

  /* Find and decode MIB */
  ret = srslte_ue_mib_sync_decode(&ue_mib, config->max_frames_pbch, bch_payload, &cell->nof_ports, NULL); 
  if (ret < 0) {
    fprintf(stderr, "Error decoding MIB\n");
    goto clean_exit; 
  }
  if (ret == 1) {
    srslte_pbch_mib_unpack(bch_payload, cell, NULL);
  }
  
  // Save CFO 
  if (cfo) {
    *cfo = srslte_ue_sync_get_cfo(&ue_mib.ue_sync);
  }
  
clean_exit: 

  srslte_rf_stop_rx_stream(rf);
  srslte_ue_mib_sync_free(&ue_mib);

  return ret; 
}

/** This function is simply a wrapper to the ue_cell_search module for rf devices 
 */
int rf_cell_search(srslte_rf_t *rf, uint32_t nof_rx_antennas,
                   cell_search_cfg_t *config, 
                   int force_N_id_2, srslte_cell_t *cell, float *cfo) 
{
  int ret = SRSLTE_ERROR; 
  srslte_ue_cellsearch_t cs; 
  srslte_ue_cellsearch_result_t found_cells[3];

  bzero(found_cells, 3*sizeof(srslte_ue_cellsearch_result_t));
    
  if (srslte_ue_cellsearch_init_multi(&cs, config->max_frames_pss, srslte_rf_recv_wrapper_cs, nof_rx_antennas, (void*) rf)) {
    fprintf(stderr, "Error initiating UE cell detect\n");
    return SRSLTE_ERROR; 
  }
  if (config->nof_valid_pss_frames) {
    srslte_ue_cellsearch_set_nof_valid_frames(&cs, config->nof_valid_pss_frames);
  }

  INFO("Setting sampling frequency %.2f MHz for PSS search\n", SRSLTE_CS_SAMP_FREQ/1000000);
  srslte_rf_set_rx_srate(rf, SRSLTE_CS_SAMP_FREQ);
  
  INFO("Starting receiver...\n");
  srslte_rf_start_rx_stream(rf, false);

  /* Find a cell in the given N_id_2 or go through the 3 of them to find the strongest */
  uint32_t max_peak_cell = 0;
  if (force_N_id_2 >= 0) {
    ret = srslte_ue_cellsearch_scan_N_id_2(&cs, force_N_id_2, &found_cells[force_N_id_2]);
    max_peak_cell = force_N_id_2;
  } else {
    ret = srslte_ue_cellsearch_scan(&cs, found_cells, &max_peak_cell); 
  }
  if (ret < 0) {
    srslte_rf_stop_rx_stream(rf);
    fprintf(stderr, "Error searching cell\n");
    return SRSLTE_ERROR;
  } else if (ret == 0) {
    srslte_rf_stop_rx_stream(rf);
    fprintf(stderr, "Could not find any cell in this frequency\n");
    return SRSLTE_SUCCESS;
  }
  
  for (int i=0;i<3;i++) {
    if (i == max_peak_cell) {
      printf("*");
    } else {
      printf(" ");
    }
    printf("Found Cell_id: %3d CP: %s, DetectRatio=%2.0f%% PSR=%.2f, Power=%.1f dBm\n", 
           found_cells[i].cell_id, srslte_cp_string(found_cells[i].cp), 
           found_cells[i].mode*100,
           found_cells[i].psr, 20*log10(found_cells[i].peak*1000));
  }
  
  // Save result 
  if (cell) {
    cell->id = found_cells[max_peak_cell].cell_id;
    cell->cp = found_cells[max_peak_cell].cp; 
  }

  // Save CFO
  if (cfo) {
    *cfo = found_cells[max_peak_cell].cfo;
  }
  
  srslte_rf_stop_rx_stream(rf);
  srslte_ue_cellsearch_free(&cs);

  return ret; 
}


/* Finds a cell and decodes MIB from the PBCH. 
 * Returns 1 if the cell is found and MIB is decoded successfully. 
 * 0 if no cell was found or MIB could not be decoded, 
 * -1 on error
 */
int rf_search_and_decode_mib(srslte_rf_t *rf, uint32_t nof_rx_antennas, cell_search_cfg_t *config, int force_N_id_2, srslte_cell_t *cell, float *cfo) 
{
  int ret = SRSLTE_ERROR; 
  
  printf("Searching for cell...\n");
  ret = rf_cell_search(rf, nof_rx_antennas, config, force_N_id_2, cell, cfo);
  if (ret > 0) {
    printf("Decoding PBCH for cell %d (N_id_2=%d)\n", cell->id, cell->id%3);        
    ret = rf_mib_decoder(rf, nof_rx_antennas, config, cell, cfo);
    if (ret < 0) {
      fprintf(stderr, "Could not decode PBCH from CELL ID %d\n", cell->id);
      return SRSLTE_ERROR;
    }    
  }
  return ret;
}

