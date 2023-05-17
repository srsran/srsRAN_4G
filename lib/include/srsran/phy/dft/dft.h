/**
 * Copyright 2013-2023 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#ifndef SRSRAN_DFT_H
#define SRSRAN_DFT_H

#include "srsran/config.h"
#include <stdbool.h>

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

#ifdef __cplusplus
extern "C" {
#endif

typedef enum { SRSRAN_DFT_COMPLEX, SRSRAN_REAL } srsran_dft_mode_t;

typedef enum { SRSRAN_DFT_FORWARD, SRSRAN_DFT_BACKWARD } srsran_dft_dir_t;

typedef struct SRSRAN_API {
  int               init_size; // DFT length used in the first initialization
  int               size;      // DFT length
  void*             in;        // Input buffer
  void*             out;       // Output buffer
  void*             p;         // DFT plan
  bool              is_guru;
  bool              forward; // Forward transform?
  bool              mirror;  // Shift negative and positive frequencies?
  bool              db;      // Provide output in dB?
  bool              norm;    // Normalize output?
  bool              dc;      // Handle insertion/removal of null DC carrier internally?
  srsran_dft_dir_t  dir;     // Forward/Backward
  srsran_dft_mode_t mode;    // Complex/Real
} srsran_dft_plan_t;

SRSRAN_API int srsran_dft_plan(srsran_dft_plan_t* plan, int dft_points, srsran_dft_dir_t dir, srsran_dft_mode_t type);

SRSRAN_API int srsran_dft_plan_c(srsran_dft_plan_t* plan, int dft_points, srsran_dft_dir_t dir);

SRSRAN_API int srsran_dft_plan_guru_c(srsran_dft_plan_t* plan,
                                      int                dft_points,
                                      srsran_dft_dir_t   dir,
                                      cf_t*              in_buffer,
                                      cf_t*              out_buffer,
                                      int                istride,
                                      int                ostride,
                                      int                how_many,
                                      int                idist,
                                      int                odist);

SRSRAN_API int srsran_dft_plan_r(srsran_dft_plan_t* plan, int dft_points, srsran_dft_dir_t dir);

SRSRAN_API int srsran_dft_replan(srsran_dft_plan_t* plan, const int new_dft_points);

SRSRAN_API int srsran_dft_replan_guru_c(srsran_dft_plan_t* plan,
                                        const int          new_dft_points,
                                        cf_t*              in_buffer,
                                        cf_t*              out_buffer,
                                        int                istride,
                                        int                ostride,
                                        int                how_many,
                                        int                idist,
                                        int                odist);

SRSRAN_API int srsran_dft_replan_c(srsran_dft_plan_t* plan, int new_dft_points);

SRSRAN_API int srsran_dft_replan_r(srsran_dft_plan_t* plan, int new_dft_points);

SRSRAN_API void srsran_dft_plan_free(srsran_dft_plan_t* plan);

/* Set options */

SRSRAN_API void srsran_dft_plan_set_mirror(srsran_dft_plan_t* plan, bool val);

SRSRAN_API void srsran_dft_plan_set_db(srsran_dft_plan_t* plan, bool val);

SRSRAN_API void srsran_dft_plan_set_norm(srsran_dft_plan_t* plan, bool val);

SRSRAN_API void srsran_dft_plan_set_dc(srsran_dft_plan_t* plan, bool val);

/* Compute DFT */

SRSRAN_API void srsran_dft_run(srsran_dft_plan_t* plan, const void* in, void* out);

SRSRAN_API void srsran_dft_run_c_zerocopy(srsran_dft_plan_t* plan, const cf_t* in, cf_t* out);

SRSRAN_API void srsran_dft_run_c(srsran_dft_plan_t* plan, const cf_t* in, cf_t* out);

SRSRAN_API void srsran_dft_run_guru_c(srsran_dft_plan_t* plan);

SRSRAN_API void srsran_dft_run_r(srsran_dft_plan_t* plan, const float* in, float* out);

#ifdef __cplusplus
}
#endif

#endif // SRSRAN_DFT_H
