/*
 * Copyright (c) 2013, Vuk Marojevic <maroje@vt.edu>.
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

void llr_approx(const _Complex float *in, float *out, int N, int M, int B,
	_Complex float *symbols, int (*S)[6][32], float sigma2);

void llr_exact(const _Complex float *in, float *out, int N, int M, int B,
	_Complex float *symbols, int (*S)[6][32], float sigma2);
