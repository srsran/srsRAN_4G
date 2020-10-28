/* Adapted Phil Karn's r=1/3 k=9 viterbi decoder to r=1/3 k=7
 *
 * K=15 r=1/6 Viterbi decoder for x86 SSE2
 * Copyright Mar 2004, Phil Karn, KA9Q
 * May be used under the terms of the GNU Lesser General Public License (LGPL)
 */

#include "parity.h"
#include <limits.h>
#include <memory.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

//#define DEBUG

#ifdef LV_HAVE_AVX2

#include <emmintrin.h>
#include <immintrin.h>
#include <tmmintrin.h>
#define _mm256_set_m128i(v0, v1) _mm256_insertf128_si256(_mm256_castsi128_si256(v1), (v0), 1)

#define _mm256_setr_m128i(v0, v1) _mm256_set_m128i((v1), (v0))

typedef union {
  unsigned char c[64];
  __m128i       v[4];
} metric_t;
typedef union {
  unsigned int   w[2];
  unsigned char  c[8];
  unsigned short s[4];
  __m64          v;
} decision_t;

static union branchtab27 {
  unsigned char c[32];
  __m256i       v;
} Branchtab37_sse2[3];

int firstGo;
/* State info for instance of Viterbi decoder */
struct v37 {
  metric_t    metrics1;                  /* path metric buffer 1 */
  metric_t    metrics2;                  /* path metric buffer 2 */
  decision_t* dp;                        /* Pointer to current decision */
  metric_t *  old_metrics, *new_metrics; /* Pointers to path metrics, swapped on every bit */
  decision_t* decisions;                 /* Beginning of decisions for block */
  uint32_t    len;
};

void set_viterbi37_polynomial_avx2(int polys[3])
{
  int state;

  for (state = 0; state < 32; state++) {
    Branchtab37_sse2[0].c[state] = (polys[0] < 0) ^ parity((2 * state) & polys[0]) ? 255 : 0;
    Branchtab37_sse2[1].c[state] = (polys[1] < 0) ^ parity((2 * state) & polys[1]) ? 255 : 0;
    Branchtab37_sse2[2].c[state] = (polys[2] < 0) ^ parity((2 * state) & polys[2]) ? 255 : 0;
  }
}

void clear_v37_avx2(struct v37* vp)
{
  bzero(vp->decisions, sizeof(decision_t) * vp->len);
  vp->dp = NULL;
  bzero(&vp->metrics1, sizeof(metric_t));
  bzero(&vp->metrics2, sizeof(metric_t));
  vp->old_metrics = NULL;
  vp->new_metrics = NULL;
}

/* Initialize Viterbi decoder for start of new frame */
int init_viterbi37_avx2(void* p, int starting_state)
{
  struct v37* vp = p;
  uint32_t    i;
  firstGo = 1;

  for (i = 0; i < 64; i++)
    vp->metrics1.c[i] = 63;

  clear_v37_avx2(vp);

  vp->old_metrics = &vp->metrics1;
  vp->new_metrics = &vp->metrics2;
  vp->dp          = vp->decisions;

  if (starting_state != -1) {
    vp->old_metrics->c[starting_state & 63] = 0; /* Bias known start state */
  }
  return 0;
}

/* Create a new instance of a Viterbi decoder */
void* create_viterbi37_avx2(int polys[3], uint32_t len)
{
  void*       p;
  struct v37* vp;

  set_viterbi37_polynomial_avx2(polys);

  /* Ordinary malloc() only returns 8-byte alignment, we need 16 */
  if (posix_memalign(&p, sizeof(__m128i), sizeof(struct v37)))
    return NULL;

  vp = (struct v37*)p;
  if (posix_memalign(&p, sizeof(__m128i), (len + 6) * sizeof(decision_t))) {
    free(vp);
    return NULL;
  }
  vp->decisions = (decision_t*)p;
  vp->len       = len + 6;
  return vp;
}

/* Viterbi chainback */
int chainback_viterbi37_avx2(void*    p,
                             uint8_t* data,  /* Decoded output data */
                             uint32_t nbits, /* Number of data bits */
                             uint32_t endstate)
{ /* Terminal encoder state */
  struct v37* vp = p;

  if (p == NULL)
    return -1;

  decision_t* d = (decision_t*)vp->decisions;

  /* Make room beyond the end of the encoder register so we can
   * accumulate a full byte of decoded data
   */
  endstate %= 64;
  endstate <<= 2;

  /* The store into data[] only needs to be done every 8 bits.
   * But this avoids a conditional branch, and the writes will
   * combine in the cache anyway
   */
  d += 6; /* Look past tail */
  while (nbits--) {
    int k;

    k           = (d[nbits].c[(endstate >> 2) / 8] >> ((endstate >> 2) % 8)) & 1;
    endstate    = (endstate >> 1) | (k << 7);
    data[nbits] = k;
    // printf("nbits=%d, endstate=%3d, k=%d, w[0]=%d, w[1]=%d, c=%d\n", nbits, endstate, k, d[nbits].s[1]&1,
    // d[nbits].s[2]&1, d[nbits].c[(endstate>>2)/8]&1);
  }
  return 0;
}

/* Delete instance of a Viterbi decoder */
void delete_viterbi37_avx2(void* p)
{
  struct v37* vp = p;

  if (vp != NULL) {
    free(vp->decisions);
    free(vp);
  }
}
void printer_256i(char* s, __m256i val)
{

  printf("%s: ", s);

  uint8_t* x = (uint8_t*)&val;
  for (int i = 0; i < 32; i++) {
    printf("%3d, ", x[i]);
  }
  printf("\n");
}

void printer_128i(char* s, __m128i val)
{

  printf("%s: ", s);

  uint8_t* x = (uint8_t*)&val;
  for (int i = 0; i < 16; i++) {
    printf("%3d, ", x[i]);
  }
  printf("\n");
}

void printer_m64(char* s, __m64 val)
{

  printf("%s: ", s);

  uint8_t* x = (uint8_t*)&val;
  for (int i = 0; i < 8; i++) {
    printf("%3d, ", x[i]);
  }
  printf("\n");
}

void update_viterbi37_blk_avx2(void* p, unsigned char* syms, int nbits, uint32_t* best_state)
{
  struct v37* vp = p;
  decision_t* d;

  if (p == NULL)
    return;

#ifdef DEBUG
  printf("[");
#endif

  d = (decision_t*)vp->dp;

  for (int s = 0; s < nbits; s++) {
    memset(d + s, 0, sizeof(decision_t));
  }

  while (nbits--) {
    __m256i sym0v, sym1v, sym2v;

    void* tmp;

    sym0v = _mm256_set1_epi8(syms[0]);
    sym1v = _mm256_set1_epi8(syms[1]);
    sym2v = _mm256_set1_epi8(syms[2]);

    syms += 3;

    __m256i decision0, decision1, survivor0, survivor1, metric, m_metric, m0, m1, m2, m3;

    /* Form branch metrics */
    m0 =
        _mm256_avg_epu8(_mm256_xor_si256(Branchtab37_sse2[0].v, sym0v), _mm256_xor_si256(Branchtab37_sse2[1].v, sym1v));
    metric = _mm256_avg_epu8(_mm256_xor_si256(Branchtab37_sse2[2].v, sym2v), m0);

#ifdef DEBUG
    print_128i("metric_initial", metric);
#endif
    /* There's no packed bytes right shift in SSE2, so we use the word version and mask
     */
    metric   = _mm256_srli_epi16(metric, 3);
    metric   = _mm256_and_si256(metric, _mm256_set1_epi8(31));
    m_metric = _mm256_sub_epi8(_mm256_set1_epi8(31), metric);

#ifdef DEBUG
    print_128i("metric        ", metric);
    print_128i("m_metric      ", m_metric);
#endif

    __m256i temp = _mm256_set_m128i(vp->old_metrics->v[1], vp->old_metrics->v[0]);
    m0           = _mm256_add_epi8(temp, metric);
    m2           = _mm256_add_epi8(temp, m_metric);

    temp = _mm256_set_m128i(vp->old_metrics->v[3], vp->old_metrics->v[2]);
    m3   = _mm256_add_epi8(temp, metric);
    m1   = _mm256_add_epi8(temp, m_metric);

    /* Compare and select, using modulo arithmetic */
    decision0 = _mm256_cmpgt_epi8(_mm256_sub_epi8(m0, m1), _mm256_setzero_si256());
    decision1 = _mm256_cmpgt_epi8(_mm256_sub_epi8(m2, m3), _mm256_setzero_si256());
    survivor0 = _mm256_or_si256(_mm256_and_si256(decision0, m1), _mm256_andnot_si256(decision0, m0));
    survivor1 = _mm256_or_si256(_mm256_and_si256(decision1, m3), _mm256_andnot_si256(decision1, m2));

    unsigned int x = _mm256_movemask_epi8(_mm256_unpackhi_epi8(decision0, decision1));
    unsigned int y = _mm256_movemask_epi8(_mm256_unpacklo_epi8(decision0, decision1));

    d->s[0] = (short)y;
    d->s[1] = (short)x;
    d->s[2] = (short)(y >> 16);
    d->s[3] = (short)(x >> 16);

    __m256i unpack;
    unpack                = _mm256_unpacklo_epi8(survivor0, survivor1);
    vp->new_metrics->v[0] = _mm256_castsi256_si128(unpack);

    vp->new_metrics->v[1] = _mm256_extractf128_si256(unpack, 1);

    unpack = _mm256_unpackhi_epi8(survivor0, survivor1);

    vp->new_metrics->v[2] = _mm256_castsi256_si128(unpack);
    vp->new_metrics->v[3] = _mm256_extractf128_si256(unpack, 1);

    __m128i temp1 = vp->new_metrics->v[1];

    vp->new_metrics->v[1] = vp->new_metrics->v[2];
    vp->new_metrics->v[2] = temp1;

    // See if we need to normalize
    if (vp->new_metrics->c[0] > 100) {
      int     i;
      uint8_t adjust;
      __m128i adjustv;
      union {
        __m128i      v;
        signed short w[8];
      } t;

      adjustv = vp->new_metrics->v[0];
      for (i = 1; i < 4; i++) {
        adjustv = _mm_min_epu8(adjustv, vp->new_metrics->v[i]);
      }

      adjustv = _mm_min_epu8(adjustv, _mm_srli_si128(adjustv, 8));
      adjustv = _mm_min_epu8(adjustv, _mm_srli_si128(adjustv, 4));
      adjustv = _mm_min_epu8(adjustv, _mm_srli_si128(adjustv, 2));

      t.v     = adjustv;
      adjust  = t.w[0];
      adjustv = _mm_set1_epi8(adjust);

      /* We cannot use a saturated subtract, because we often have to adjust by more than SHRT_MAX
       * This is okay since it can't overflow anyway
       */
      for (i = 0; i < 4; i++)
        vp->new_metrics->v[i] = _mm_sub_epi8(vp->new_metrics->v[i], adjustv);
    }

    firstGo = 0;
    d++;
    /* Swap pointers to old and new metrics */
    tmp             = vp->old_metrics;
    vp->old_metrics = vp->new_metrics;
    vp->new_metrics = tmp;
  }

  if (best_state) {
    uint32_t i, bst = 0;
    uint8_t  minmetric = UINT8_MAX;
    for (i = 0; i < 64; i++) {
      if (vp->old_metrics->c[i] <= minmetric) {
        bst       = i;
        minmetric = vp->old_metrics->c[i];
      }
    }
    *best_state = bst;
  }

#ifdef DEBUG
  printf("];\n===========================================\n");
#endif

  vp->dp = d;
}

#endif
