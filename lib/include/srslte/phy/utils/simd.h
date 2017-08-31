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

#ifndef SRSLTE_SIMD_H_H
#define SRSLTE_SIMD_H_H

/*
 * SSE Macros
 */
#ifdef LV_HAVE_SSE
#define _MM_SWAP(X) ((__m128)_mm_shuffle_ps(X, X, _MM_SHUFFLE(2,3,0,1)))
#define _MM_PERM(X) ((__m128)_mm_shuffle_ps(X, X, _MM_SHUFFLE(2,1,3,0)))
#define _MM_MULJ_PS(X) _MM_SWAP(_MM_CONJ_PS(X))
#define _MM_CONJ_PS(X) (_mm_xor_ps(X, _mm_set_ps(-0.0f, 0.0f, -0.0f, 0.0f)))
#define _MM_SQMOD_PS(X) _MM_PERM(_mm_hadd_ps(_mm_mul_ps(X,X), _mm_set_ps(0.0f, 0.0f, 0.0f, 0.0f)))
#define _MM_PROD_PS(a, b) _mm_addsub_ps(_mm_mul_ps(a,_mm_moveldup_ps(b)),_mm_mul_ps(\
                            _mm_shuffle_ps(a,a,0xB1),_mm_movehdup_ps(b)))

#endif /* LV_HAVE_SSE */

/*
 * AVX Macros
 */
#ifdef LV_HAVE_AVX

#define _MM256_MULJ_PS(X) _mm256_permute_ps(_MM256_CONJ_PS(X), 0b10110001)
#define _MM256_CONJ_PS(X) (_mm256_xor_ps(X, _mm256_set_ps(-0.0f, 0.0f, -0.0f, 0.0f, -0.0f, 0.0f, -0.0f, 0.0f)))

#ifdef LV_HAVE_FMA
#define _MM256_SQMOD_PS(A, B) _mm256_permute_ps(_mm256_hadd_ps(_mm256_fmadd_ps(A, A, _mm256_mul_ps(B,B)), \
                                           _mm256_set_ps(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f)), 0b11011100)
#define _MM256_PROD_PS(a, b) _mm256_fmaddsub_ps(a,_mm256_moveldup_ps(b),\
                              _mm256_mul_ps(_mm256_shuffle_ps(a,a,0xB1),_mm256_movehdup_ps(b)))
#else
#define _MM256_SQMOD_PS(A, B) _mm256_permute_ps(_mm256_hadd_ps(_mm256_add_ps(_mm256_mul_ps(A,A), _mm256_mul_ps(B,B)), \
                                           _mm256_set_ps(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f)), 0b11011100)
#define _MM256_PROD_PS(a, b) _mm256_addsub_ps(_mm256_mul_ps(a,_mm256_moveldup_ps(b)),\
                              _mm256_mul_ps(_mm256_shuffle_ps(a,a,0xB1),_mm256_movehdup_ps(b)))
#endif /* LV_HAVE_FMA */
#endif /* LV_HAVE_AVX */


/*
 * AVX extension with FMA Macros
 */
#ifdef LV_HAVE_FMA

#define _MM256_SQMOD_ADD_PS(A, B, C) _mm256_permute_ps(_mm256_hadd_ps(_mm256_fmadd_ps(A, A, _mm256_fmadd_ps(B, B, C)),\
                                           _mm256_set_ps(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f)), 0b11011100)

#define _MM256_PROD_ADD_PS(A, B, C) _mm256_fmaddsub_ps(A,_mm256_moveldup_ps(B),\
                              _mm256_fmaddsub_ps(_mm256_shuffle_ps(A,A,0xB1),_mm256_movehdup_ps(B), C))

#define _MM256_PROD_SUB_PS(A, B, C) _mm256_fmaddsub_ps(A,_mm256_moveldup_ps(B),\
                              _mm256_fmsubadd_ps(_mm256_shuffle_ps(A,A,0xB1),_mm256_movehdup_ps(B), C))
#endif /* LV_HAVE_FMA */

#endif //SRSLTE_SIMD_H_H
