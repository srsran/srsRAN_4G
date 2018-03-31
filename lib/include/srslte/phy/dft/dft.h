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


#ifndef SRSLTE_DFT_H
#define SRSLTE_DFT_H
 
#include <stdbool.h>
#include "srslte/config.h"

/**********************************************************************************************
 *  File:         dft.h
 *
 *  Description:  Generic DFT module.
 *                Supports one-dimensional complex and real transforms. Options are set
 *                using the dft_plan_set_x functions.
 *
 *                Options (default is false):
 *
 *                mirror - Rearranges negative and positive frequency bins. Swaps after
 *                         transform for FORWARD, swaps before transform for BACKWARD.
 *                db     - Provides output in dB (10*log10(x)).
 *                norm   - Normalizes output (by sqrt(len) for complex, len for real).
 *                dc     - Handles insertion and removal of null DC carrier internally.
 *
 *  Reference:
 *********************************************************************************************/

typedef enum {
  SRSLTE_DFT_COMPLEX, SRSLTE_REAL
}srslte_dft_mode_t;

typedef enum {
  SRSLTE_DFT_FORWARD, SRSLTE_DFT_BACKWARD
}srslte_dft_dir_t;

typedef struct SRSLTE_API {
  int init_size;      // DFT length used in the first initialization
  int size;           // DFT length
  void *in;           // Input buffer
  void *out;          // Output buffer
  void *p;            // DFT plan
  bool is_guru;
  bool forward;       // Forward transform?
  bool mirror;        // Shift negative and positive frequencies?
  bool db;            // Provide output in dB?
  bool norm;          // Normalize output?
  bool dc;            // Handle insertion/removal of null DC carrier internally?
  srslte_dft_dir_t dir;     // Forward/Backward
  srslte_dft_mode_t mode;   // Complex/Real
}srslte_dft_plan_t;

SRSLTE_API void srslte_dft_load();

SRSLTE_API void srslte_dft_exit();

SRSLTE_API int srslte_dft_plan(srslte_dft_plan_t *plan,
                               int dft_points, 
                               srslte_dft_dir_t dir,                         
                               srslte_dft_mode_t type);

SRSLTE_API int srslte_dft_plan_c(srslte_dft_plan_t *plan,
                                 int dft_points, 
                                 srslte_dft_dir_t dir);

SRSLTE_API int srslte_dft_plan_guru_c(srslte_dft_plan_t *plan,
                                      int dft_points,
                                      srslte_dft_dir_t dir,
                                      cf_t *in_buffer,
                                      cf_t *out_buffer,
                                      int istride,
                                      int ostride,
                                      int how_many,
                                      int idist,
                                      int odist);

SRSLTE_API int srslte_dft_plan_r(srslte_dft_plan_t *plan, 
                                 int dft_points, 
                                 srslte_dft_dir_t dir);

SRSLTE_API int srslte_dft_replan(srslte_dft_plan_t *plan,
                                 const int new_dft_points);

SRSLTE_API int srslte_dft_replan_guru_c(srslte_dft_plan_t *plan,
                                        const int new_dft_points,
                                        cf_t *in_buffer,
                                        cf_t *out_buffer,
                                        int istride,
                                        int ostride,
                                        int how_many,
                                        int idist,
                                        int odist);

SRSLTE_API int srslte_dft_replan_c(srslte_dft_plan_t *plan,
                                   int new_dft_points);

SRSLTE_API int srslte_dft_replan_r(srslte_dft_plan_t *plan,
                                   int new_dft_points);


SRSLTE_API void srslte_dft_plan_free(srslte_dft_plan_t *plan);

/* Set options */

SRSLTE_API void srslte_dft_plan_set_mirror(srslte_dft_plan_t *plan, 
                                           bool val);

SRSLTE_API void srslte_dft_plan_set_db(srslte_dft_plan_t *plan, 
                                       bool val);

SRSLTE_API void srslte_dft_plan_set_norm(srslte_dft_plan_t *plan, 
                                         bool val);

SRSLTE_API void srslte_dft_plan_set_dc(srslte_dft_plan_t *plan, 
                                       bool val);

/* Compute DFT */

SRSLTE_API void srslte_dft_run(srslte_dft_plan_t *plan,
                               const void *in,
                               void *out);

SRSLTE_API void srslte_dft_run_c_zerocopy(srslte_dft_plan_t *plan,
                                          const cf_t *in,
                                          cf_t *out); 

SRSLTE_API void srslte_dft_run_c(srslte_dft_plan_t *plan,
                                 const cf_t *in,
                                 cf_t *out);

SRSLTE_API void srslte_dft_run_guru_c(srslte_dft_plan_t *plan);

SRSLTE_API void srslte_dft_run_r(srslte_dft_plan_t *plan, 
                                 const float *in,
                                 float *out);

#endif // SRSLTE_DFT_H

