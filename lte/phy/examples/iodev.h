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

#ifndef IODEF_H
#define IODEF_H

#include "liblte/config.h"

#include "liblte/phy/phch/ue_sync.h"
#include "liblte/phy/io/filesource.h"

#ifndef DISABLE_UHD
#include "liblte/cuhd/cuhd.h"
#endif

/*********
 * 
 * This component is a wrapper to the cuhd or filesource modules. It uses 
 * sync_frame_t to read aligned subframes from the USRP or filesource to read 
 * subframes from a file. 
 * 
 * When created, it starts receiving/reading at 1.92 MHz. The sampling frequency 
 * can then be changed using iodev_set_srate()
 */


typedef enum LIBLTE_API {FILESOURCE, UHD} iodev_mode_t; 

typedef _Complex float cf_t; 

typedef struct LIBLTE_API {
  char *input_file_name;
  float uhd_freq;
  float uhd_gain;
  char *uhd_args;
  float find_threshold;
} iodev_cfg_t;

typedef struct LIBLTE_API {
  #ifndef DISABLE_UHD
  void *uhd;
  ue_sync_t sframe;
  #endif
  uint32_t sf_len; 
  cf_t *input_buffer_file; // for UHD mode, the input buffer is managed by sync_frame_t
  filesource_t fsrc;  
  iodev_cfg_t config; 
  iodev_mode_t mode; 
} iodev_t; 


LIBLTE_API int iodev_init(iodev_t *q, 
                          iodev_cfg_t *config, 
                          uint32_t file_sf_len);

LIBLTE_API void iodev_free(iodev_t *q);

LIBLTE_API int iodev_receive(iodev_t *q, 
                             cf_t **buffer);

LIBLTE_API void* iodev_get_cuhd(iodev_t *q);

LIBLTE_API bool iodev_isfile(iodev_t *q); 

LIBLTE_API bool iodev_isUSRP(iodev_t *q); 

#endif