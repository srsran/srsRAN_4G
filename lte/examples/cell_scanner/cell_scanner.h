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

#include "liblte/phy/phy.h"

typedef struct {
  int cell_detect_max_frames;
  float cell_detect_early_stop_threshold;
  int measure_avg_nof_frames; 
  char *uhd_args; 
} cell_scanner_config_t;

typedef struct {
  lte_cell_t phy_cell; 
  uint32_t cell_id; 
  float rsrp; 
  float rsrq; 
  float rssi; 
  float snr; 
} cell_scanner_result_t;

typedef struct {
  cell_scanner_config_t config; 
  void *uhd; 
} cell_scanner_t; 

int cell_scanner_init(cell_scanner_t *q, 
                      cell_scanner_config_t *config);

void cell_scanner_close(cell_scanner_t *q);

int cell_scanner_all_cells(cell_scanner_t *q, 
                            float frequency, 
                            cell_scanner_result_t *result);

int cell_scanner_cell(cell_scanner_t *q, 
                       float frequency, 
                       int N_id_2, 
                       cell_scanner_result_t *result);
