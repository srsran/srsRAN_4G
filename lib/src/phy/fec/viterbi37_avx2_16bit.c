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

typedef union {
  // unsigned char c[64];
  //__m128i       v[4];
  unsigned short c[64];
  __m256i        v[4];
} metric_t;

typedef union {
  unsigned int   w[2];
  unsigned char  c[8];
  unsigned short s[4];
  __m64          v[1];
} decision_t;

union branchtab27 {

  // unsigned char c[32];
  //__m128i       v[2];
  unsigned short c[32];
  __m256i        v[2];

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

void set_viterbi37_polynomial_avx2_16bit(int polys[3])
{
  int state;
  for (state = 0; state < 32; state++) {
    Branchtab37_sse2[0].c[state] = (polys[0] < 0) ^ parity((2 * state) & polys[0]) ? 65535 : 0;
    Branchtab37_sse2[1].c[state] = (polys[1] < 0) ^ parity((2 * state) & polys[1]) ? 65535 : 0;
    Branchtab37_sse2[2].c[state] = (polys[2] < 0) ^ parity((2 * state) & polys[2]) ? 65535 : 0;
  }
}

void clear_v37_avx2_16bit(struct v37* vp)
{
  bzero(vp->decisions, sizeof(decision_t) * vp->len);
  vp->dp = NULL;
  bzero(&vp->metrics1, sizeof(metric_t));
  bzero(&vp->metrics2, sizeof(metric_t));
  vp->old_metrics = NULL;
  vp->new_metrics = NULL;
}

/* Initialize Viterbi decoder for start of new frame */
int init_viterbi37_avx2_16bit(void* p, int starting_state)
{

  struct v37* vp = p;
  uint32_t    i;

  for (i = 0; i < 64; i++)
    vp->metrics1.c[i] = 63;

  clear_v37_avx2_16bit(vp);
  firstGo         = 1;
  vp->old_metrics = &vp->metrics1;
  vp->new_metrics = &vp->metrics2;
  vp->dp          = vp->decisions;
  if (starting_state != -1) {
    vp->old_metrics->c[starting_state & 63] = 0; /* Bias known start state */
  }
  return 0;
}

/* Create a new instance of a Viterbi decoder */
void* create_viterbi37_avx2_16bit(int polys[3], uint32_t len)
{
  void*       p;
  struct v37* vp;

  set_viterbi37_polynomial_avx2_16bit(polys);

  /* Ordinary malloc() only returns 8-byte alignment, we need 16 */
  if (posix_memalign(&p, sizeof(__m256i), sizeof(struct v37)))
    return NULL;

  vp = (struct v37*)p;
  if (posix_memalign(&p, sizeof(__m256i), (len + 6) * sizeof(decision_t))) {
    free(vp);
    return NULL;
  }
  vp->decisions = (decision_t*)p;
  vp->len       = len + 6;
  return vp;
}

/* Viterbi chainback */
int chainback_viterbi37_avx2_16bit(void*    p,
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
void delete_viterbi37_avx2_16bit(void* p)
{
  struct v37* vp = p;

  if (vp != NULL) {
    free(vp->decisions);
    free(vp);
  }
}

void print_256i(char* s, __m256i val)
{

  printf("%s: ", s);

  uint16_t* x = (uint16_t*)&val;
  for (int i = 0; i < 16; i++) {
    printf("%.5f, ", (float)x[i] / 65535);
  }
  printf("\n");
}
void print_256i_char(char* s, __m256i val)
{

  printf("%s: ", s);

  uint8_t* x = (uint8_t*)&val;
  for (int i = 0; i < 32; i++) {
    printf("%d, ", x[31 - i]);
  }
  printf("\n");
}

inline unsigned short my_mm256_movemask_epi16(__m256i x)
{
  uint32_t x1  = _mm256_movemask_epi8(x);
  uint16_t tmp = 0;
  for (int i = 0; i < 16; i++) {
    tmp |= ((x1 >> ((i * 2) + 1)) & 0x01) << i;
  }

  return (tmp);
}

void update_viterbi37_blk_avx2_16bit(void* p, unsigned short* syms, int nbits, uint32_t* best_state)
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
    void*   tmp;
    int     i;

    // printf("nbits=%d, syms=%d,%d,%d\n", nbits, syms[0], syms[1], syms[2]);fflush(stdout);

    /* Splat the 0th symbol across sym0v, the 1st symbol across sym1v, etc */

    sym0v = _mm256_set1_epi16(syms[0]);
    sym1v = _mm256_set1_epi16(syms[1]);
    sym2v = _mm256_set1_epi16(syms[2]);

    syms += 3;

    for (i = 0; i < 2; i++) {

      __m256i decision0, decision1, metric, m_metric, m0, m1, m2, m3, survivor0, survivor1;

      /* Form branch metrics */
      m0     = _mm256_avg_epu16(_mm256_xor_si256(Branchtab37_sse2[0].v[i], sym0v),
                            _mm256_xor_si256(Branchtab37_sse2[1].v[i], sym1v));
      metric = _mm256_avg_epu16(_mm256_xor_si256(Branchtab37_sse2[2].v[i], sym2v), m0);

#ifdef DEBUG
      print_128i("metric_initial", metric);
#endif
      /* There's no packed bytes right shift in SSE2, so we use the word version and mask
       */

      metric   = _mm256_srli_epi16(metric, 3);
      m_metric = _mm256_sub_epi16(_mm256_set1_epi16(8191), metric);

#ifdef DEBUG
      print_128i("metric        ", metric);
      print_128i("m_metric      ", m_metric);
#endif

      /* Add branch metrics to path metrics */

      m0 = _mm256_add_epi16(vp->old_metrics->v[i], metric);
      m3 = _mm256_add_epi16(vp->old_metrics->v[2 + i], metric);
      m1 = _mm256_add_epi16(vp->old_metrics->v[2 + i], m_metric);
      m2 = _mm256_add_epi16(vp->old_metrics->v[i], m_metric);

      /* Compare and select, using modulo arithmetic */

      decision0 = _mm256_cmpgt_epi16(_mm256_sub_epi16(m0, m1), _mm256_setzero_si256());
      decision1 = _mm256_cmpgt_epi16(_mm256_sub_epi16(m2, m3), _mm256_setzero_si256());
      survivor0 = _mm256_or_si256(_mm256_and_si256(decision0, m1), _mm256_andnot_si256(decision0, m0));
      survivor1 = _mm256_or_si256(_mm256_and_si256(decision1, m3), _mm256_andnot_si256(decision1, m2));

      /* Pack each set of decisions into 16 bits */

      decision0 = _mm256_permute4x64_epi64(decision0, 216);
      decision1 = _mm256_permute4x64_epi64(decision1, 216);

      __m256i packed = _mm256_packus_epi16(_mm256_srli_epi16(_mm256_unpacklo_epi16(decision0, decision1), 8),
                                           _mm256_srli_epi16(_mm256_unpackhi_epi16(decision0, decision1), 8));

      d->w[i] = _mm256_movemask_epi8(packed);

      unsigned char temp_char1 = d->c[4 * i + 1];
      unsigned char temp_char2 = d->c[4 * i + 2];

      d->c[4 * i + 1] = temp_char2;
      d->c[4 * i + 2] = temp_char1;

      /* Store surviving metrics */
      survivor0 = _mm256_permute4x64_epi64(survivor0, 216);
      survivor1 = _mm256_permute4x64_epi64(survivor1, 216);

      vp->new_metrics->v[2 * i]     = _mm256_unpacklo_epi16(survivor0, survivor1);
      vp->new_metrics->v[2 * i + 1] = _mm256_unpackhi_epi16(survivor0, survivor1);
    }

    // See if we need to normalize
    if (vp->new_metrics->c[0] > 12288) {
      int i;

      uint16_t adjust;
      __m256i  adjustv;
      union {
        __m256i      v;
        signed short w[8];
      } t;

      adjustv = vp->new_metrics->v[0];
      for (i = 1; i < 4; i++) {
        adjustv = _mm256_min_epu16(adjustv, vp->new_metrics->v[i]);
      }

      adjustv = _mm256_min_epu16(adjustv, _mm256_srli_si256(adjustv, 16));
      adjustv = _mm256_min_epu16(adjustv, _mm256_srli_si256(adjustv, 8));
      adjustv = _mm256_min_epu16(adjustv, _mm256_srli_si256(adjustv, 4));

      t.v     = adjustv;
      adjust  = t.w[0];
      adjustv = _mm256_set1_epi16(adjust);

      /* We cannot use a saturated subtract, because we often have to adjust by more than SHRT_MAX
       * This is okay since it can't overflow anyway
       */
      for (i = 0; i < 4; i++)
        vp->new_metrics->v[i] = _mm256_sub_epi16(vp->new_metrics->v[i], adjustv);
    }

    d++;
    /* Swap pointers to old and new metrics */
    tmp             = vp->old_metrics;
    vp->old_metrics = vp->new_metrics;
    vp->new_metrics = tmp;
  }

  if (best_state) {
    uint32_t i, bst = 0;

    uint16_t minmetric = UINT16_MAX;
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
