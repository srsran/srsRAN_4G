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

/* Thresholds for Demodulation */
/* Assume perfect amplitude and phase alignment.
 *  Check threshold values for real case
 *  or implement dynamic threshold adjustent as a function of received symbol amplitudes */
#define QAM16_THRESHOLD    2/sqrt(10)
#define QAM64_THRESHOLD_1  2/sqrt(42)
#define QAM64_THRESHOLD_2  4/sqrt(42)
#define QAM64_THRESHOLD_3  6/sqrt(42)

void hard_bpsk_demod(const cf_t* in, 
                     uint8_t* out, 
                     uint32_t N);

void hard_qpsk_demod(const cf_t* in, 
                     uint8_t* out, 
                     uint32_t N);

void hard_qam16_demod(const cf_t* in, 
                                 uint8_t* out, 
                                 uint32_t N);

void hard_qam64_demod(const cf_t* in, 
                                 uint8_t* out, 
                                 uint32_t N);
