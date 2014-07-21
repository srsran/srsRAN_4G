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

#include "iodev.h"

#include "liblte/phy/io/filesource.h"
#include "liblte/phy/phch/ue_sync.h"
#include "liblte/phy/utils/debug.h"
#include "liblte/phy/utils/vector.h"

#ifndef DISABLE_UHD
#include "liblte/cuhd/cuhd.h"
#endif


int cuhd_recv_wrapper(void *h, void *data, uint32_t nsamples) {
  DEBUG(" ----  Receive %d samples  ---- \n", nsamples);
  return cuhd_recv(h, data, nsamples, 1);
}

/* Setup USRP or input file */
int iodev_init(iodev_t *q, iodev_cfg_t *config) {
  
  if (config->input_file_name) {
    if (filesource_init(&q->fsrc, config->input_file_name, COMPLEX_FLOAT_BIN)) {
      return LIBLTE_ERROR;
    }
    q->input_buffer_file = vec_malloc(SF_LEN_MAX * sizeof(cf_t));
    if (!q->input_buffer_file) {
      perror("malloc");
      return LIBLTE_ERROR; 
    }

    q->mode = FILESOURCE;
    q->sf_len = 1920;
    
  } else {
#ifndef DISABLE_UHD
    printf("Opening UHD device...\n");
    if (cuhd_open(config->uhd_args, &q->uhd)) {
      fprintf(stderr, "Error opening uhd\n");
      return LIBLTE_ERROR;
    }
    
    /* set uhd_freq */
    cuhd_set_rx_gain(q->uhd, config->uhd_gain);
    cuhd_set_rx_freq(q->uhd, (double) config->uhd_freq);

    cuhd_rx_wait_lo_locked(q->uhd);
    DEBUG("Set uhd_freq to %.3f MHz\n", (double ) config->uhd_freq);

    DEBUG("Starting receiver...\n", 0);
    cuhd_start_rx_stream(q->uhd);
    
    if (config->find_threshold > 0.0) {
      ue_sync_set_threshold(&q->sframe, config->find_threshold);
    }
        
    ue_sync_init(&q->sframe, cuhd_set_rx_srate, cuhd_recv_wrapper, q->uhd);

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
  int n; 
  if (q->mode == FILESOURCE) {
    DEBUG(" -----   READING %d SAMPLES ---- \n", q->sf_len);
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
    return q->uhd; 
  } else {
    return NULL; 
  }
}

bool iodev_isfile(iodev_t *q) {
  return q->mode == FILESOURCE;
}

bool iodev_isUSRP(iodev_t *q) {
  return q->mode == UHD;
}


