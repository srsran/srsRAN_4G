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
#include <string.h>

#include "soft_algs.h"
#include "liblte/phy/utils/vector.h"

#define QAM16_THRESHOLD         2/sqrt(10)
#define QAM64_THRESHOLD_1       2/sqrt(42)
#define QAM64_THRESHOLD_2       4/sqrt(42)
#define QAM64_THRESHOLD_3       6/sqrt(42)


typedef _Complex float cf_t;

// There are 3 implemenations: 1 - based on zones; 2 - using volk, 3 - straightforward C
#define LLR_APPROX_IMPLEMENTATION 1

#if LLR_APPROX_IMPLEMENTATION == 1


/**
 * @ingroup Received modulation symbol zone
 * Determine location of received modulation symbol
 *
 * \param in input symbol (_Complex float)
 * \param z associated zone in constellation diagram (int)
 * \param N number of symbols
 */
static void zone_QPSK(const cf_t * in, uint32_t * z, int N)
{

  int s;
  float re, im;

  for (s = 0; s < N; s++) {

    re = __real__ in[s];
    im = __imag__ in[s];

    if (re > 0) {
      if (im > 0) {             /* 1st Quadrand (upper-right) */
        z[s] = 0;
      } else {                  /* 4th Quadrand (lower-right) */
        z[s] = 1;
      }
    } else {
      if (im > 0) {             /* 2nd Quadrand (upper-left) */
        z[s] = 2;
      } else {                  /* 3rd Quadrand (lower-left) */
        z[s] = 3;
      }
    }
  }
}

/**
 * @ingroup Received modulation symbol zone
 * Determine location of received modulation symbol
 *
 * \param in input symbol (_Complex float)
 * \param z associated zone in constellation diagram (int)
 * \param N number of symbols
 */
static void zone_QAM16(const cf_t * in, uint32_t * z, int N)
{

  int s;
  float re, im;

  for (s = 0; s < N; s++) {

    re = __real__ in[s];
    im = __imag__ in[s];

    if (re > 0) {
      if (im > 0) {             /* 1st Quadrand (upper-right) */
        if (re > QAM16_THRESHOLD) {
          if (im > QAM16_THRESHOLD) {
            z[s] = 3;
          } else {
            z[s] = 2;
          }
        } else {
          if (im > QAM16_THRESHOLD) {
            z[s] = 1;
          } else {
            z[s] = 0;
          }
        }
      } else {                  /* 4th Quadrand (lower-right) */
        if (re > QAM16_THRESHOLD) {
          if (im < -QAM16_THRESHOLD) {
            z[s] = 7;
          } else {
            z[s] = 6;
          }
        } else {
          if (im < -QAM16_THRESHOLD) {
            z[s] = 5;
          } else {
            z[s] = 4;
          }
        }
      }
    } else {
      if (im > 0) {             /* 2nd Quadrand (upper-left) */
        if (re < -QAM16_THRESHOLD) {
          if (im > QAM16_THRESHOLD) {
            z[s] = 11;
          } else {
            z[s] = 10;
          }
        } else {
          if (im > QAM16_THRESHOLD) {
            z[s] = 9;
          } else {
            z[s] = 8;
          }
        }
      } else {                  /* 3rd Quadrand (lower-left) */
        if (re < -QAM16_THRESHOLD) {
          if (im < -QAM16_THRESHOLD) {
            z[s] = 15;
          } else {
            z[s] = 14;
          }
        } else {
          if (im < -QAM16_THRESHOLD) {
            z[s] = 13;
          } else {
            z[s] = 12;
          }
        }
      }
    }
  }
}

/**
 * @ingroup Received modulation symbol zone
 * Determine location of received modulation symbol
 *
 * \param in input symbol (_Complex float)
 * \param z associated zone in constellation diagram (int)
 * \param N number of symbols
 */

static void zone_QAM64(const cf_t * in, uint32_t * z, int N)
{

  int s;
  float re, im;

  for (s = 0; s < N; s++) {

    re = __real__ in[s];
    im = __imag__ in[s];

    if (re > 0) {

      if (im > 0) {

        if (re > QAM64_THRESHOLD_2) {

          if (re > QAM64_THRESHOLD_3) {

            if (im > QAM64_THRESHOLD_2) {
              if (im > QAM64_THRESHOLD_3) {
                z[s] = 15;
              } else {
                z[s] = 14;
              }
            } else if (im > QAM64_THRESHOLD_1) {
              z[s] = 10;
            } else {
              z[s] = 11;
            }

          } else {

            if (im > QAM64_THRESHOLD_2) {
              if (im > QAM64_THRESHOLD_3) {
                z[s] = 13;
              } else {
                z[s] = 12;
              }
            } else if (im > QAM64_THRESHOLD_1) {
              z[s] = 8;
            } else {
              z[s] = 9;
            }
          }

        } else if (re > QAM64_THRESHOLD_1) {

          if (im > QAM64_THRESHOLD_2) {
            if (im > QAM64_THRESHOLD_3) {
              z[s] = 5;
            } else {
              z[s] = 4;
            }
          } else if (im > QAM64_THRESHOLD_1) {
            z[s] = 0;
          } else {
            z[s] = 1;
          }

        } else {
          if (im > QAM64_THRESHOLD_2) {
            if (im > QAM64_THRESHOLD_3) {
              z[s] = 7;
            } else {
              z[s] = 6;
            }
          } else if (im > QAM64_THRESHOLD_1) {
            z[s] = 2;
          } else {
            z[s] = 3;
          }
        }

      } else {                  /* forth quadrant (lower-right) */

        if (re > QAM64_THRESHOLD_2) {

          if (re > QAM64_THRESHOLD_3) {

            if (im < -QAM64_THRESHOLD_2) {
              if (im < -QAM64_THRESHOLD_3) {
                z[s] = 31;
              } else {
                z[s] = 30;
              }
            } else if (im < -QAM64_THRESHOLD_1) {
              z[s] = 26;
            } else {
              z[s] = 27;
            }

          } else {

            if (im < -QAM64_THRESHOLD_2) {
              if (im < -QAM64_THRESHOLD_3) {
                z[s] = 29;
              } else {
                z[s] = 28;
              }
            } else if (im < -QAM64_THRESHOLD_1) {
              z[s] = 24;
            } else {
              z[s] = 25;
            }
          }

        } else if (re > QAM64_THRESHOLD_1) {

          if (im < -QAM64_THRESHOLD_2) {
            if (im < -QAM64_THRESHOLD_3) {
              z[s] = 21;
            } else {
              z[s] = 20;
            }
          } else if (im < -QAM64_THRESHOLD_1) {
            z[s] = 16;
          } else {
            z[s] = 17;
          }

        } else {
          if (im < -QAM64_THRESHOLD_2) {
            if (im < -QAM64_THRESHOLD_3) {
              z[s] = 23;
            } else {
              z[s] = 22;
            }
          } else if (im < -QAM64_THRESHOLD_1) {
            z[s] = 18;
          } else {
            z[s] = 19;
          }
        }
      }

    } else {                    /* re < 0 */

      if (im > 0) {             /* second quadrant (upper-left) */

        if (re < -QAM64_THRESHOLD_2) {

          if (re < -QAM64_THRESHOLD_3) {

            if (im > QAM64_THRESHOLD_2) {
              if (im > QAM64_THRESHOLD_3) {
                z[s] = 47;
              } else {
                z[s] = 46;
              }
            } else if (im > QAM64_THRESHOLD_1) {
              z[s] = 42;
            } else {
              z[s] = 43;
            }

          } else {

            if (im > QAM64_THRESHOLD_2) {
              if (im > QAM64_THRESHOLD_3) {
                z[s] = 45;
              } else {
                z[s] = 44;
              }
            } else if (im > QAM64_THRESHOLD_1) {
              z[s] = 40;
            } else {
              z[s] = 41;
            }
          }

        } else if (re < -QAM64_THRESHOLD_1) {

          if (im > QAM64_THRESHOLD_2) {
            if (im > QAM64_THRESHOLD_3) {
              z[s] = 37;
            } else {
              z[s] = 36;
            }
          } else if (im > QAM64_THRESHOLD_1) {
            z[s] = 32;
          } else {
            z[s] = 33;
          }

        } else {
          if (im > QAM64_THRESHOLD_2) {
            if (im > QAM64_THRESHOLD_3) {
              z[s] = 39;
            } else {
              z[s] = 38;
            }
          } else if (im > QAM64_THRESHOLD_1) {
            z[s] = 34;
          } else {
            z[s] = 35;
          }
        }
      } else {                  /* third quadrant (lower-left) */
        if (re < -QAM64_THRESHOLD_2) {

          if (re < -QAM64_THRESHOLD_3) {

            if (im < -QAM64_THRESHOLD_2) {
              if (im < -QAM64_THRESHOLD_3) {
                z[s] = 63;
              } else {
                z[s] = 62;
              }
            } else if (im < -QAM64_THRESHOLD_1) {
              z[s] = 58;
            } else {
              z[s] = 59;
            }

          } else {

            if (im < -QAM64_THRESHOLD_2) {
              if (im < -QAM64_THRESHOLD_3) {
                z[s] = 61;
              } else {
                z[s] = 60;
              }
            } else if (im < -QAM64_THRESHOLD_1) {
              z[s] = 56;
            } else {
              z[s] = 57;
            }
          }

        } else if (re < -QAM64_THRESHOLD_1) {

          if (im < -QAM64_THRESHOLD_2) {
            if (im < -QAM64_THRESHOLD_3) {
              z[s] = 53;
            } else {
              z[s] = 52;
            }
          } else if (im < -QAM64_THRESHOLD_1) {
            z[s] = 48;
          } else {
            z[s] = 49;
          }

        } else {
          if (im < -QAM64_THRESHOLD_2) {
            if (im < -QAM64_THRESHOLD_3) {
              z[s] = 55;
            } else {
              z[s] = 54;
            }
          } else if (im < -QAM64_THRESHOLD_1) {
            z[s] = 50;
          } else {
            z[s] = 51;
          }
        }

      }
    }
  }
}

static void compute_zone(const cf_t * in, uint32_t * z, int N, int B)
{
  switch (B) {
    case 1:{
        memset(z, 0, N * sizeof(uint32_t));
        break;
      }                         /* BPSK */
    case 2:{
        zone_QPSK(in, z, N);
        break;
      }                         /* QPSK */
    case 4:{
        zone_QAM16(in, z, N);
        break;
      }                         /* 16QAM */
    case 6:{
        zone_QAM64(in, z, N);
        break;
      }                         /* 64QAM */
  }
}

static void compute_square_dist(uint32_t *zone, float *dd, const cf_t * in, cf_t * symbols,
                                uint32_t(*idx)[7], int N, int B)
{
  int s, b;
  float *d_ptr;
  cf_t symbols_extract[7];

  for (s = 0; s < N; s++) {     /* N: number of received symbols */
    d_ptr = &dd[7*s];
    for (b = 0; b < B + 1; b++) {
      symbols_extract[b] = symbols[idx[zone[s]][b]];    
      /* only subset of distances to constellation points needed for LLR approx */
    }
    vec_square_dist(in[s], symbols_extract, d_ptr, B + 1);      /* B+1 distances to be computed */
  }
}

static void compute_llr(uint32_t *zone, float *dd, int N, int B, uint32_t(*min)[64][6], float sigma2,
                        float *out)
{
  int s, b;
  for (s = 0; s < N; s++) {
    for (b = 0; b < B; b++) {   /* bits per symbol */
      out[s * B + b] =
        (dd[7*s+min[0][zone[s]][b]] - dd[7*s+min[1][zone[s]][b]]) / sigma2;
    }
  }
}

void llr_approx(const _Complex float *in, float *out, int N, int M, int B,
                _Complex float *symbols, uint32_t(*S)[6][32], uint32_t(*idx)[7],
                uint32_t(*min)[64][6], float sigma2, uint32_t *zone, float *dd)
{
  if ((M == 1) || (M == 2) || (M == 4) || (M == 16) || (M == 64)) {
    compute_zone(in, zone, N, B);
    compute_square_dist(zone, dd, in, symbols, idx, N, B);
    compute_llr(zone, dd, N, B, min, sigma2, out);
  }
}

#elif LLR_APPROX_IMPLEMENTATION == 2

float d[10000][64];
float num[10000], den[10000];

static void compute_square_dist(const cf_t * in, cf_t * symbols, int N, int M)
{
  int s;
  float *d_ptr;
  for (s = 0; s < N; s++) {
    d_ptr = d[s];
    vec_square_dist(in[s], symbols, d_ptr, M);
  }
}

static void compute_min_dist(uint32_t(*S)[6][32], int N, int B, int M)
{
  int s, b, i;
  for (s = 0; s < N; s++) {
    for (b = 0; b < B; b++) {   /* bits per symbol */
      /* initiate num[b] and den[b] */
      num[s * B + b] = 1e10;
      den[s * B + b] = 1e10;

      for (i = 0; i < M / 2; i++) {
        if (d[s][S[0][b][i]] < num[s * B + b]) {
          num[s * B + b] = d[s][S[0][b][i]];
        }
        if (d[s][S[1][b][i]] < den[s * B + b]) {
          den[s * B + b] = d[s][S[1][b][i]];
        }
      }
    }
  }
}

static void compute_llr(int N, int B, float sigma2, float *out)
{
  int s, b;
  for (s = 0; s < N; s++) {
    for (b = 0; b < B; b++) {       /* bits per symbol */
      out[s * B + b] = (num[s * B + b] - den[s * B + b]) / sigma2;
    }
  }
}

void llr_approx(const _Complex float *in, float *out, int N, int M, int B,
                _Complex float *symbols, uint32_t(*S)[6][32], float sigma2)
{

  if (M <= 64) {
    compute_square_dist(in, symbols, N, M);

    compute_min_dist(S, N, B, M);

    compute_llr(N, B, sigma2, out);
  }
  for (b = 0; b < B; b++) {  /* bits per symbol */
    out[s * B + b] = (num[s * B + b] - den[s * B + b]) / sigma2;
  }
}

void llr_approx(const _Complex float *in, float *out, int N, int M, int B,
                _Complex float *symbols, uint32_t(*S)[6][32], uint32_t(*idx)[7],
                uint32_t(*min)[64][6], float sigma2)
{

  if (M <= 64) {
    compute_square_dist(in, symbols, N, M);

    compute_min_dist(S, N, B, M);

    compute_llr(N, B, sigma2, out);
  }
}

#else

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
void llr_approx(const _Complex float *in, float *out, int N, int M, int B,
                _Complex float *symbols, uint32_t(*S)[6][32], uint32_t(*idx)[7],
                uint32_t(*min)[64][6], float sigma2)
{
  int i, s, b;
  float num, den;
  int change_sign = -1;
  float x, y, d[64];

  for (s = 0; s < N; s++) {     /* recevied symbols */
    /* Compute the distances squared d[i] between the received symbol and all constellation points */
    for (i = 0; i < M; i++) {
      x = __real__ in[s] - __real__ symbols[i];
      y = __imag__ in[s] - __imag__ symbols[i];
      d[i] = x * x + y * y;
    }

    for (b = 0; b < B; b++) {   /* bits per symbol */
      /* initiate num[b] and den[b] */
      num = d[S[0][b][0]];
      den = d[S[1][b][0]];

      /* Minimum distance squared search between recevied symbol and a constellation point with a
         '1' and a '0' for each bit position */
      for (i = 1; i < M / 2; i++) {     /* half the constellation points have '1'|'0' at any given bit position */
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
      out[s * B + b] = change_sign * (den - num) / sigma2;
      if (s < 10)
        printf("out[%d]=%f=%f/%f\n", s * B + b, out[s * B + b], num, den);
    }
/*      if (s<10)
        printf("out[%d]=%f=%f/%f\n",s*B+b,out[s*B+b], num,den);
  */ }

}

#endif

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
void llr_exact(const _Complex float *in, float *out, int N, int M, int B,
               _Complex float *symbols, uint32_t(*S)[6][32], float sigma2)
{
  int i, s, b;
  float num, den;
  int change_sign = -1;
  float x, y, d[64];

  for (s = 0; s < N; s++) {     /* recevied symbols */
    /* Compute exp{·} of the distances squared d[i] between the received symbol and all constellation points */
    for (i = 0; i < M; i++) {
      x = __real__ in[s] - __real__ symbols[i];
      y = __imag__ in[s] - __imag__ symbols[i];
      d[i] = exp(-1 * (x * x + y * y) / sigma2);
    }

    /* Sum up the corresponding d[i]'s for each bit position */
    for (b = 0; b < B; b++) {   /* bits per symbol */
      /* initiate num[b] and den[b] */
      num = 0;
      den = 0;

      for (i = 0; i < M / 2; i++) {     /* half the constellation points have '1'|'0' at any given bit position */
        num += d[S[0][b][i]];
        den += d[S[1][b][i]];
      }
      /* Theoretical LLR and approximate LLR values are positive if
       * symbol(s) with '0' is/are closer and negative if symbol(s)
       * with '1' are closer.
       * Change sign if mapping negative to '0' and positive to '1' */
      out[s * B + b] = change_sign * log(num / den);
    }
  }
}
