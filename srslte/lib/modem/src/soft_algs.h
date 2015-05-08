/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2015 The srsLTE Developers. See the
 * COPYRIGHT file at the top-level directory of this distribution.
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


/*void llr_approx(const _Complex float *in, 
                float *out, 
                int N, 
                int M, 
                int B,
                _Complex float *symbols, 
                uint32_t (*S)[6][32], 
                float sigma2);
*/
void llr_approx(const _Complex float *in, 
                float *out, 
                int N, 
                int M, 
                int B,
                _Complex float *symbols, 
		uint32_t (*S)[6][32], 
                uint32_t (*idx)[7],	/*64x7 table of integers [0..63], indices to 7 distances to be computed */
		uint32_t (*min)[64][6],	/*2x64x6 table of integers [0..6], indices to 2x6 nearest symbols */
                float sigma2, 
                uint32_t *zone, 
                float *dd);

void llr_exact(const _Complex float *in, 
               float *out, 
               int N, 
               int M, 
               int B,
               _Complex float *symbols, 
               uint32_t (*S)[6][32], 
               float sigma2);

