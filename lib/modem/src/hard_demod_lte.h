/*
 * Copyright (c) 2013, Ismael Gomez-Miguelez <gomezi@tcd.ie>, Vuk Marojevic <maroje@vt.edu>.
 * This file is part of OSLD-lib (http://https://github.com/ismagom/osld-lib)
 *
 * OSLD-lib is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * OSLD-lib is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with OSLD-lib.  If not, see <http://www.gnu.org/licenses/>.
 */

/* Thresholds for Demodulation */
/* Assume perfect amplitude and phase alignment.
 *  Check threshold values for real case
 *  or implement dynamic threshold adjustent as a function of received symbol amplitudes */
#define QAM16_THRESHOLD		2/sqrt(10)
#define QAM64_THRESHOLD_1	2/sqrt(42)
#define QAM64_THRESHOLD_2	4/sqrt(42)
#define QAM64_THRESHOLD_3	6/sqrt(42)

void hard_bpsk_demod(const cf* in, char* out, int N);
void hard_qpsk_demod(const cf* in, char* out, int N);
void hard_qam16_demod(const cf* in, char* out, int N);
void hard_qam64_demod(const cf* in, char* out, int N);
