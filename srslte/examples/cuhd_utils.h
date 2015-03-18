/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2014 The srsLTE Developers. See the
 * COPYRIGHT file at the top-level directory of this distribution.
 *
 * \section LICENSE
 *
 * This file is part of the srsLTE library.
 *
 * srsLTE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsLTE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * A copy of the GNU Lesser General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */


#include "srslte/srslte.h"

typedef struct SRSLTE_API {
  uint32_t max_frames_pbch; // maximum number of 5ms frames to capture for MIB decoding
  uint32_t max_frames_pss; // maximum number of 5ms frames to capture for PSS correlation
  float threshold;   // early-stops cell detection if mean PSR is above this value 
}cell_search_cfg_t;

int cuhd_mib_decoder(void *uhd, 
                     uint32_t max_nof_frames, 
                     srslte_cell_t *cell);

int cuhd_cell_search(void *uhd, 
                     cell_search_cfg_t *config, 
                     int force_N_id_2, 
                     srslte_cell_t *cell);

int cuhd_search_and_decode_mib(void *uhd, 
                               cell_search_cfg_t *config, 
                               int force_N_id_2, 
                               srslte_cell_t *cell);


