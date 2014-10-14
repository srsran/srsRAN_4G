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


#include <complex.h>
#include <math.h>

int decode_pbch(void *uhd, 
                ue_celldetect_result_t *found_cell, 
                uint32_t nof_frames_total, 
                pbch_mib_t *mib);

int find_all_cells(void *uhd, 
                   ue_celldetect_result_t found_cell[3]);

int find_cell(void *uhd, 
               ue_celldetect_result_t *found_cell, 
               uint32_t N_id_2);

int cell_search(void *uhd, 
                int force_N_id_2, 
                lte_cell_t *cell, 
                pbch_mib_t *mib);