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


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "iodev.h"

#include "liblte/phy/io/filesource.h"
#include "liblte/phy/ue/ue_sync.h"
#include "liblte/phy/utils/debug.h"
#include "liblte/phy/utils/vector.h"

#ifndef DISABLE_UHD
#include "liblte/cuhd/cuhd.h"
#endif

#include "cell_search_utils.h"

#ifndef DISABLE_UHD
int cuhd_recv_wrapper(void *h, void *data, uint32_t nsamples) {
  DEBUG(" ----  Receive %d samples  ---- \n", nsamples);
  return cuhd_recv(h, data, nsamples, 1);
}
#endif

/* Setup USRP or input file */
int iodev_init(iodev_t *q, iodev_cfg_t *config, lte_cell_t *cell, pbch_mib_t *mib) {
  
  if (config->input_file_name) {
    
    mib->phich_resources = R_1; 
    mib->phich_length = PHICH_NORM;
    
    cell->id = config->cell_id_file;
    cell->cp = CPNORM; 
    cell->nof_ports = config->nof_ports_file; 
    cell->nof_prb = config->nof_prb_file; 

    if (filesource_init(&q->fsrc, config->input_file_name, COMPLEX_FLOAT_BIN)) {
      return LIBLTE_ERROR;
    }
    q->mode = FILESOURCE;
    int symbol_sz = lte_symbol_sz(cell->nof_prb);
    if (symbol_sz > 0) {
      q->sf_len = SF_LEN(symbol_sz);      
    } else {
      fprintf(stderr, "Invalid number of PRB %d\n", cell->nof_prb);
      return LIBLTE_ERROR; 
    }

    q->input_buffer_file = vec_malloc(q->sf_len * sizeof(cf_t));
    if (!q->input_buffer_file) {
      perror("malloc");
      return LIBLTE_ERROR; 
    }
    q->sf_idx = 9; 
    
  } else {
#ifndef DISABLE_UHD
    printf("Opening UHD device...\n");
    if (cuhd_open(config->uhd_args, &q->uhd)) {
      fprintf(stderr, "Error opening uhd\n");
      return LIBLTE_ERROR;
    }
    
    cuhd_set_rx_gain(q->uhd, config->uhd_gain);

    /* set receiver frequency */
    cuhd_set_rx_freq(q->uhd, (double) config->uhd_freq);

    cuhd_rx_wait_lo_locked(q->uhd);
    DEBUG("Set uhd_freq to %.3f MHz\n", (double ) config->uhd_freq);

    int n; 
    ue_celldetect_t cd;
    ue_celldetect_result_t found_cells[3];

    cf_t *buffer = vec_malloc(sizeof(cf_t) * 96000);
    if (!buffer) {
      perror("malloc");
      return LIBLTE_ERROR;
    }
    if (ue_celldetect_init(&cd)) {
      fprintf(stderr, "Error initiating UE cell detect\n");
      exit(-1);
    }
    n = find_cell(q->uhd, &cd, buffer, found_cells);
    if (n < 0) {
      fprintf(stderr, "Error searching cell\n");
      exit(-1);
    }

    int max_peak_cell = 0;
    float max_peak_value = -1.0;
    if (n > 0) {
      for (int i=0;i<3;i++) {
        if (found_cells[i].peak > max_peak_value) {
          max_peak_value = found_cells[i].peak;
          max_peak_cell = i;
        }
      }
      if (decode_pbch(q->uhd, buffer, &found_cells[max_peak_cell], 400, mib)) {
        fprintf(stderr, "Could not decode PBCH from CELL ID %d\n", found_cells[max_peak_cell].cell_id);
        return LIBLTE_ERROR;
      }
    } else {
      fprintf(stderr, "Could not find any cell in this frequency\n");
      return LIBLTE_ERROR;
    }
    
    free(buffer);
    cell->cp = found_cells[max_peak_cell].cp;
    cell->id = found_cells[max_peak_cell].cell_id;
    cell->nof_prb = mib->nof_prb;
    cell->nof_ports = mib->nof_ports; 
    
    /* set sampling frequency */
    int srate = lte_sampling_freq_hz(cell->nof_prb);
    if (srate != -1) {  
      cuhd_set_rx_srate(q->uhd, (double) srate);      
    } else {
      fprintf(stderr, "Invalid number of PRB %d\n", cell->nof_prb);
      return LIBLTE_ERROR;
    }

    DEBUG("Starting receiver...\n", 0);
    cuhd_start_rx_stream(q->uhd);
    
    if (ue_sync_init(&q->sframe, *cell, cuhd_recv_wrapper, q->uhd)) {
      fprintf(stderr, "Error initiating ue_sync\n");
      return LIBLTE_ERROR; 
    }

    /* Decodes the SSS signal during the tracking phase. Extra overhead, but makes sure we are in the correct subframe */  
    ue_sync_decode_sss_on_track(&q->sframe, true);

    // Here, the subframe length and input buffer is managed by ue_sync
    q->mode = UHD; 
  
#else
    printf("Error UHD not available. Select an input file\n");
    return LIBLTE_ERROR;
#endif
  }
  
  memcpy(&q->config, config, sizeof(iodev_cfg_t));
  
  return LIBLTE_SUCCESS;
}



void iodev_free(iodev_t *q) {
  if (q->mode == FILESOURCE) {
    filesource_free(&q->fsrc);
  } else {
#ifndef DISABLE_UHD
    cuhd_close(q->uhd);
#endif
  }
}

/* Receive samples from the USRP or read from file */
int iodev_receive(iodev_t *q, cf_t **buffer) {
  int n=0;
  if (q->mode == FILESOURCE) {
    INFO(" -----   READING %d SAMPLES ---- \n", q->sf_len);
    n = filesource_read(&q->fsrc, q->input_buffer_file, q->sf_len);
    *buffer = q->input_buffer_file; 
    if (n == -1) {
      fprintf(stderr, "Error reading file\n");
      /* wrap file if arrive to end */
    } else if (n < q->sf_len) {
      DEBUG("Read %d from file. Seeking to 0\n",n);
      filesource_seek(&q->fsrc, 0);
      n = filesource_read(&q->fsrc, q->input_buffer_file, q->sf_len);
      if (n == -1) {
        fprintf(stderr, "Error reading file\n");
        /* wrap file if arrive to end */
      } else {
        n = 1; 
      }
    } else {
      n = 1; 
    }
    q->sf_idx++;
    if (q->sf_idx == 10) {
      q->sf_idx = 0;
    }
    usleep(5000);
  } else {
    /* Use ue_sync_work which returns a synchronized buffer of subframe samples */
#ifndef DISABLE_UHD
    n = ue_sync_get_buffer(&q->sframe, buffer);
    if (n < 0) {
      fprintf(stderr, "Error calling ue_sync_work()\n");
    }
#endif
  }
  return n; 
}

void* iodev_get_cuhd(iodev_t *q) {
  if (q->mode == UHD) {
#ifndef DISABLE_UHD
    return q->uhd; 
#endif
  }
  return NULL;

}

bool iodev_isfile(iodev_t *q) {
  return q->mode == FILESOURCE;
}

bool iodev_isUSRP(iodev_t *q) {
  return q->mode == UHD;
}

uint32_t iodev_get_sfidx(iodev_t *q) {
  if (iodev_isUSRP(q)) {
#ifndef DISABLE_UHD
    return ue_sync_get_sfidx(&q->sframe);
#endif
  }
  return q->sf_idx;
}


