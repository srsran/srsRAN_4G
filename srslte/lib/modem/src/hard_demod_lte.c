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


#include <stdint.h>
#include <complex.h>
#include <math.h>

#include "srslte/modem/demod_hard.h"
#include "hard_demod_lte.h"


/**
 * @ingroup Hard BPSK demodulator
 *
 *LTE-BPSK constellation:
 *   Q
 *   |  0
 *---------> I
 *1  |
 * \param in input symbols (_Complex float)
 * \param out output symbols (uint8_ts)
 * \param N Number of input symbols
 * \param modulation Modulation type
 */
inline void hard_bpsk_demod(const cf_t* in, uint8_t* out, uint32_t N)
{
  uint32_t s;

  for (s=0; s<N; s++) {    /* received symbols */
    if (__real__ in[s] > 0) {
      if ((__imag__ in[s] > 0) || (__real__ in[s] > -__imag__ in[s])) {
        out[s] = 0x0;
      } else {
        out[s] = 0x1;
      }
    } else {
      if ((__imag__ in[s] < 0) || (__imag__ in[s] < -__real__ in[s])) {
        out[s] = 0x1;
      } else {
        out[s] = 0x0;
      }
    }
  }
}

/**
 * @ingroup Hard QPSK demodulator
 *
 * LTE-QPSK constellation:
 *    Q
 *10  |  00
 *-----------> I
 *11  |  01
 *
 * \param in input symbols (_Complex float)
 * \param out output symbols (uint8_ts)
 * \param N Number of input symbols
 * \param modulation Modulation type
 */
inline void hard_qpsk_demod(const cf_t* in, uint8_t* out, uint32_t N)
{
  uint32_t s;

  for (s=0; s<N; s++) {
    if (__real__ in[s] > 0) {
      out[2*s] = 0x0;
    } else {
      out[2*s] = 0x1;
    }
    if (__imag__ in[s] > 0) {
      out[2*s+1] = 0x0;
    } else {
      out[2*s+1] = 0x1;
    }
  }
}

/**
 * @ingroup Hard 16QAM demodulator
 *
 * LTE-16QAM constellation:
 *               Q
 *  1011    1001  |  0001    0011
 *  1010    1000  |  0000    0010
 *---------------------------------> I
 *  1110    1100  |  0100    0110
 *  1111    1101  |  0101    0111
 *
 * \param in input symbols (_Complex float)
 * \param out output symbols (uint8_ts)
 * \param N Number of input symbols
 * \param modulation Modulation type
 */
inline void hard_qam16_demod(const cf_t* in, uint8_t* out, uint32_t N)
{
  uint32_t s;

  for (s=0; s<N; s++) {
    if (__real__ in[s] > 0) {
      out[4*s] = 0x0;
    } else {
      out[4*s] = 0x1;
    }

    if ((__real__ in[s] > QAM16_THRESHOLD) || (__real__ in[s] < -QAM16_THRESHOLD)) {
      out[4*s+2] = 0x1;
    } else {
         out[4*s+2] = 0x0;
    }

    if (__imag__ in[s] > 0) {
      out[4*s+1] = 0x0;
    } else {
         out[4*s+1] = 0x1;
    }

    if ((__imag__ in[s] > QAM16_THRESHOLD) || (__imag__ in[s] < -QAM16_THRESHOLD)) {
      out[4*s+3] = 0x1;
    } else {
         out[4*s+3] = 0x0;
    }
  }
}

/**
 * @ingroup Hard 64QAM demodulator
 *
 * LTE-64QAM constellation:
 * see [3GPP TS 36.211 version 10.5.0 Release 10, Section 7.1.4]
 *
 * \param in input symbols (_Complex float)
 * \param out output symbols (uint8_ts)
 * \param N Number of input symbols
 * \param modulation Modulation type
 */
inline void hard_qam64_demod(const cf_t* in, uint8_t* out, uint32_t N)
{
  uint32_t s;

  for (s=0; s<N; s++) {
    /* bits associated with/obtained from in-phase component: b0, b2, b4 */
    if (__real__ in[s] > 0){
      out[6*s] = 0x0;
    } else {
      out[6*s] = 0x1;
    }
    if ((__real__ in[s] > QAM64_THRESHOLD_3) || (__real__ in[s] < -QAM64_THRESHOLD_3)) {
      out[6*s+2] = 0x1;
      out[6*s+4] = 0x1;
    } else if ((__real__ in[s] > QAM64_THRESHOLD_2) || (__real__ in[s] < -QAM64_THRESHOLD_2)) {
      out[6*s+2] = 0x1;
      out[6*s+4] = 0x0;
    } else if ((__real__ in[s] > QAM64_THRESHOLD_1) || (__real__ in[s] < -QAM64_THRESHOLD_1)) {
      out[6*s+2] = 0x0;
      out[6*s+4] = 0x0;
    } else {
      out[6*s+2] = 0x0;
      out[6*s+4] = 0x1;
    }

    /* bits associated with/obtained from quadrature component: b1, b3, b5 */
    if (__imag__ in[s] > 0){
      out[6*s+1] = 0x0;
    } else {
      out[6*s+1] = 0x1;
    }
    if ((__imag__ in[s] > QAM64_THRESHOLD_3) || (__imag__ in[s] < -QAM64_THRESHOLD_3)) {
      out[6*s+3] = 0x1;
      out[6*s+5] = 0x1;
    } else if ((__imag__ in[s] > QAM64_THRESHOLD_2) || (__imag__ in[s] < -QAM64_THRESHOLD_2)) {
      out[6*s+3] = 0x1;
      out[6*s+5] = 0x0;
    } else if ((__imag__ in[s] > QAM64_THRESHOLD_1) || (__imag__ in[s] < -QAM64_THRESHOLD_1)) {
      out[6*s+3] = 0x0;
      out[6*s+5] = 0x0;
    } else {
      out[6*s+3] = 0x0;
      out[6*s+5] = 0x1;
    }
  }
}
