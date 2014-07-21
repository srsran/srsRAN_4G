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
#include <math.h>
#include <complex.h>
#include <stdint.h>

#include "soft_algs.h"

/**
 * @ingroup Soft Modulation Demapping based on the approximate
 * log-likelihood algorithm
 * Common algorithm that approximates the log-likelihood ratio. It takes
 * only the two closest constellation symbols into account, one with a '0'
 * and the other with a '1' at the given bit position.
 *
 * \param in input symbols (_Complex float)
 * \param out output symbols (float)
 * \param N Number of input symbols
 * \param M Number of constellation points
 * \param B Number of bits per symbol
 * \param symbols constellation symbols
 * \param S Soft demapping auxiliary matrix
 * \param sigma2 Noise vatiance
 */
/* Note: Optimized implementation of approximate LLR algorithm, July 2014*/
void llr_approx(const _Complex float *in, float *out, int N, int M, int B,
    _Complex float *symbols, uint32_t (*S)[6][32], float sigma2) {
  int i, s, b;
  float num, den;
  int change_sign = -1;
  float x, y, d[64];

  for (s=0; s<N; s++) {  	/* recevied symbols */
	  /* Compute exp{·} of the distances between received symbol and all constallation symbols */
	  for (i=0; i<M; i++) {
		x = __real__ in[s] - __real__ symbols[i];
		y = __imag__ in[s] - __imag__ symbols[i];
		d[i] = x*x + y*y;
	  }

	  for (b=0; b<B; b++) {/* bits per symbol*/
	  	/* initiate num[b] and den[b] */
		num = d[S[0][b][0]];
		den = d[S[1][b][0]];

		/* half the constellation symbols have '1'|'0' at any bit pos. */
	        for (i=1; i<M/2; i++) {
	        	if (d[S[0][b][i]] < num) {
	        		num = d[S[0][b][i]];
	        	}
	        	if (d[S[1][b][i]] < den) {
	        		den = d[S[1][b][i]];
	        	}
	        }
	        /* Theoretical LLR and approximate LLR values are positive if
	         * symbol(s) with '0' is/are closer and negative if symbol(s)
	         * with '1' are closer.
	         * Change sign if mapping negative to '0' and positive to '1' */
	        out[s*B+b] = change_sign*(den-num)/sigma2;
	  }
  }

}

/**
 * @ingroup Soft Modulation Demapping based on the approximate
 * log-likelihood ratio algorithm
 * Common algorithm that approximates the log-likelihood ratio. It takes
 * only the two closest constellation symbols into account, one with a '0'
 * and the other with a '1' at the given bit position.
 *
 * \param in input symbols (_Complex float)
 * \param out output symbols (float)
 * \param N Number of input symbols
 * \param M Number of constellation points
 * \param B Number of bits per symbol
 * \param symbols constellation symbols
 * \param S Soft demapping auxiliary matrix
 * \param sigma2 Noise vatiance
 */
/* Note: Optimized implementation of exact LLR algorithm, July 2014*/
void llr_exact(const _Complex float *in, float *out, int N, int M, int B,
    _Complex float *symbols, uint32_t (*S)[6][32], float sigma2) {
  int i, s, b;
  float num, den;
  int change_sign = -1;
  float x, y, d[64];

  for (s=0; s<N; s++) {  	/* recevied symbols */
    	/* Compute exp{·} of the distances between received symbol and all constallation symbols */
    	for (i=0; i<M; i++) {
    		x = __real__ in[s] - __real__ symbols[i];
    		y = __imag__ in[s] - __imag__ symbols[i];
    		d[i] = exp(-1*(x*x + y*y)/sigma2);
    	}

    	for (b=0; b<B; b++) {/* bits per symbol*/
    		/* initiate num[b] and den[b] */
    		num = 0;
    		den = 0;
    	        /* half the constellation symbols have '1'|'0' at any bit pos. */
    	        for (i=0; i<M/2; i++) {
    	        	num += d[S[0][b][i]];
    	        	den += d[S[1][b][i]];
    	        }
    	        /* Theoretical LLR and approximate LLR values are positive if
    	         * symbol(s) with '0' is/are closer and negative if symbol(s)
    	         * with '1' are closer.
    	         * Change sign if mapping negative to '0' and positive to '1' */
    	        out[s*B+b] = change_sign*log(num/den);
	}
  }
}
