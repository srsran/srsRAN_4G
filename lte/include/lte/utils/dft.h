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


#ifndef DFT_H_
#define DFT_H_
 
#include <fftw3.h>
#include "lte/config.h"


/* dft is a frontend to the fftw3 library. It facilitates the computation of
 * complex or real DFT, power spectral density, normalization, etc.
 * It also supports the creation of multiple FFT plans for different FFT sizes
 * or options, selecting a different one at runtime.
 */


typedef enum {
  COMPLEX_2_COMPLEX, REAL_2_REAL, COMPLEX_2_REAL
}dft_mode_t;

typedef enum {
  FORWARD, BACKWARD
}dft_dir_t;


#define DFT_MIRROR_PRE  1
#define DFT_PSD      2
#define DFT_OUT_DB    4
#define DFT_MIRROR_POS  8
#define DFT_NORMALIZE   16
#define DFT_DC_OFFSET   32

typedef struct LIBLTE_API {
  int size;
  int sign;
  void *in;
  void *out;
  void *p;
  int options;
  dft_dir_t dir;
  dft_mode_t mode;
}dft_plan_t;

typedef _Complex float dft_c_t;
typedef float dft_r_t;

/* Create DFT plans */

LIBLTE_API int dft_plan(dft_plan_t *plan, const int dft_points,
                        dft_mode_t mode, dft_dir_t dir);
LIBLTE_API int dft_plan_c2c(dft_plan_t *plan, const int dft_points, dft_dir_t dir);
LIBLTE_API int dft_plan_r2r(dft_plan_t *plan, const int dft_points, dft_dir_t dir);
LIBLTE_API int dft_plan_c2r(dft_plan_t *plan, const int dft_points, dft_dir_t dir);
LIBLTE_API void dft_plan_free(dft_plan_t *plan);


/* Create a vector of DFT plans */

LIBLTE_API int dft_plan_vector(dft_plan_t *plans, const int *dft_points,
                               dft_mode_t *modes, dft_dir_t *dirs, int nof_plans);
LIBLTE_API int dft_plan_multi_c2c(dft_plan_t *plans, const int *dft_points,
                                  dft_dir_t dir, int nof_plans);
LIBLTE_API int dft_plan_multi_c2r(dft_plan_t *plans, const int *dft_points,
                                  dft_dir_t dir, int nof_plans);
LIBLTE_API int dft_plan_multi_r2r(dft_plan_t *plans, const int *dft_points,
                                  dft_dir_t dir, int nof_plans);
LIBLTE_API void dft_plan_free_vector(dft_plan_t *plans, int nof_plans);

/* Compute DFT */

LIBLTE_API void dft_run(dft_plan_t *plan, void *in, void *out);
LIBLTE_API void dft_run_c2c(dft_plan_t *plan, dft_c_t *in, dft_c_t *out);
LIBLTE_API void dft_run_r2r(dft_plan_t *plan, dft_r_t *in, dft_r_t *out);
LIBLTE_API void dft_run_c2r(dft_plan_t *plan, dft_c_t *in, dft_r_t *out);

#endif // DFT_H_

