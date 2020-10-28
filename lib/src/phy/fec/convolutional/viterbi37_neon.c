/* Adapted Phil Karn's r=1/3 k=9 viterbi decoder to r=1/3 k=7
 *
 * K=15 r=1/6 Viterbi decoder for ARM NEON
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
#ifdef HAVE_NEON

#include <arm_neon.h>

typedef union {
  unsigned char c[64];
  uint8x16_t    v[4];
} metric_t;
typedef union {
  unsigned long  w[2];
  unsigned char  c[8];
  unsigned short s[4];
  uint8x8_t      v[1];
} decision_t;

union branchtab27 {
  unsigned char c[32];
  uint8x16_t    v[2];
} Branchtab37_neon[3];

int8_t __attribute__((aligned(16))) xr[8];
uint8x8_t mask_and;
int8x8_t  mask_shift;

/* State info for instance of Viterbi decoder */
struct v37 {
  metric_t    metrics1;                  /* path metric buffer 1 */
  metric_t    metrics2;                  /* path metric buffer 2 */
  decision_t* dp;                        /* Pointer to current decision */
  metric_t *  old_metrics, *new_metrics; /* Pointers to path metrics, swapped on every bit */
  decision_t* decisions;                 /* Beginning of decisions for block */
  uint32_t    len;
};

void set_viterbi37_polynomial_neon(int polys[3])
{
  int state;

  for (state = 0; state < 32; state++) {
    Branchtab37_neon[0].c[state] = (polys[0] < 0) ^ parity((2 * state) & polys[0]) ? 255 : 0;
    Branchtab37_neon[1].c[state] = (polys[1] < 0) ^ parity((2 * state) & polys[1]) ? 255 : 0;
    Branchtab37_neon[2].c[state] = (polys[2] < 0) ^ parity((2 * state) & polys[2]) ? 255 : 0;
  }
}

void clear_v37_neon(struct v37* vp)
{
  bzero(vp->decisions, sizeof(decision_t) * vp->len);
  vp->dp = NULL;
  bzero(&vp->metrics1, sizeof(metric_t));
  bzero(&vp->metrics2, sizeof(metric_t));
  vp->old_metrics = NULL;
  vp->new_metrics = NULL;
}

/* Initialize Viterbi decoder for start of new frame */
int init_viterbi37_neon(void* p, int starting_state)
{
  struct v37* vp = p;
  uint32_t    i;
  for (i = 0; i < 64; i++)
    vp->metrics1.c[i] = 63;

  clear_v37_neon(vp);
  for (int i = 0; i < 8; i++)
    xr[i] = i - 7;

  mask_and   = vdup_n_u8(0x80);
  mask_shift = vld1_s8(xr);

  vp->old_metrics = &vp->metrics1;
  vp->new_metrics = &vp->metrics2;
  vp->dp          = vp->decisions;
  if (starting_state != -1) {
    vp->old_metrics->c[starting_state & 63] = 0; /* Bias known start state */
  }
  return 0;
}

/* Create a new instance of a Viterbi decoder */
void* create_viterbi37_neon(int polys[3], uint32_t len)
{
  void*       p;
  struct v37* vp;

  set_viterbi37_polynomial_neon(polys);

  /* Ordinary malloc() only returns 8-byte alignment, we need 16 */
  if (posix_memalign(&p, sizeof(uint8x16_t), sizeof(struct v37)))
    return NULL;

  vp = (struct v37*)p;
  if (posix_memalign(&p, sizeof(uint8x16_t), (len + 6) * sizeof(decision_t))) {
    free(vp);
    return NULL;
  }
  vp->decisions = (decision_t*)p;
  vp->len       = len + 6;
  return vp;
}

/* Viterbi chainback */
int chainback_viterbi37_neon(void*    p,
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
void delete_viterbi37_neon(void* p)
{
  struct v37* vp = p;

  if (vp != NULL) {
    free(vp->decisions);
    free(vp);
  }
}

void print_uint8x16_t(char* s, uint8x16_t val)
{

  printf("%s: ", s);

  uint8_t* x = (uint8_t*)&val;
  for (int i = 0; i < 16; i++) {
    printf("%3d, ", x[i]);
  }
  printf("\n");
}

static inline int movemask_neon(uint8x16_t movemask_low_in)
{
  uint8x8_t lo = vget_low_u8(movemask_low_in);
  uint8x8_t hi = vget_high_u8(movemask_low_in);
  lo           = vand_u8(lo, mask_and);
  lo           = vshl_u8(lo, mask_shift);
  hi           = vand_u8(hi, mask_and);
  hi           = vshl_u8(hi, mask_shift);

  lo = vpadd_u8(lo, lo);
  lo = vpadd_u8(lo, lo);
  lo = vpadd_u8(lo, lo);

  hi = vpadd_u8(hi, hi);
  hi = vpadd_u8(hi, hi);
  hi = vpadd_u8(hi, hi);

  return ((hi[0] << 8) | (lo[0] & 0xFF));
}

void update_viterbi37_blk_neon(void* p, unsigned char* syms, int nbits, uint32_t* best_state)
{
  struct v37* vp = p;
  decision_t* d;

  uint8_t thirtyone;
  thirtyone = 31;
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
    uint8x16_t sym0v, sym1v, sym2v;
    void*      tmp;
    int        i;

    // printf("nbits=%d, syms=%d,%d,%d\n", nbits, syms[0], syms[1], syms[2]);fflush(stdout);

    /* Splat the 0th symbol across sym0v, the 1st symbol across sym1v, etc */
    sym0v = vld1q_dup_u8(syms);
    sym1v = vld1q_dup_u8(syms + 1);
    sym2v = vld1q_dup_u8(syms + 2);
    syms += 3;

    for (i = 0; i < 2; i++) {
      uint8x16_t decision0, decision1, metric, m_metric, m0, m1, m2, m3, survivor0, survivor1;

      /* Form branch metrics */
      m0     = vrhaddq_u8(veorq_u8(Branchtab37_neon[0].v[i], sym0v), veorq_u8(Branchtab37_neon[1].v[i], sym1v));
      metric = vrhaddq_u8(veorq_u8(Branchtab37_neon[2].v[i], sym2v), m0);

      metric   = vshrq_n_u8(metric, 3);
      m_metric = vsubq_u8(vld1q_dup_u8(&thirtyone), metric);

      /* Add branch metrics to path metrics */
      m0 = vaddq_u8(vp->old_metrics->v[i], metric);
      m3 = vaddq_u8(vp->old_metrics->v[2 + i], metric);
      m1 = vaddq_u8(vp->old_metrics->v[2 + i], m_metric);
      m2 = vaddq_u8(vp->old_metrics->v[i], m_metric);

      /* Compare and select, using modulo arithmetic */
      decision0 = (uint8x16_t)vcgtq_s8(vsubq_s8((int8x16_t)m0, (int8x16_t)m1), vdupq_n_s8(0));
      decision1 = (uint8x16_t)vcgtq_s8(vsubq_s8((int8x16_t)m2, (int8x16_t)m3), vdupq_n_s8(0));
      survivor0 = vorrq_u8(vandq_u8(decision0, m1), vandq_u8(vmvnq_u8(decision0), m0));
      survivor1 = vorrq_u8(vandq_u8(decision1, m3), vandq_u8(vmvnq_u8(decision1), m2));

      /* Pack each set of decisions into 16 bits */
      uint8x8_t   a1              = vget_low_u8(decision0);
      uint8x8_t   b1              = vget_low_u8(decision1);
      uint8x8x2_t result          = vzip_u8(a1, b1);
      uint8x16_t  movemask_low_in = vcombine_u8(result.val[0], result.val[1]);

      d->s[2 * i] = movemask_neon(movemask_low_in);

      a1                        = vget_high_u8(decision0);
      b1                        = vget_high_u8(decision1);
      result                    = vzip_u8(a1, b1);
      uint8x16_t movemask_hi_in = vcombine_u8(result.val[0], result.val[1]);

      d->s[2 * i + 1] = movemask_neon(movemask_hi_in);

      a1                        = vget_low_u8(survivor0);
      b1                        = vget_low_u8(survivor1);
      result                    = vzip_u8(a1, b1);
      vp->new_metrics->v[2 * i] = vcombine_u8(result.val[0], result.val[1]);

      a1                            = vget_high_u8(survivor0);
      b1                            = vget_high_u8(survivor1);
      result                        = vzip_u8(a1, b1);
      vp->new_metrics->v[2 * i + 1] = vcombine_u8(result.val[0], result.val[1]);
    }

    // See if we need to normalize
    if (vp->new_metrics->c[0] > 100) {
      int        i;
      uint8_t    adjust;
      uint8x16_t adjustv;

      union {
        uint8x16_t   v;
        signed short w[8];
      } t;

      adjustv = vp->new_metrics->v[0];
      for (i = 1; i < 4; i++) {
        adjustv = vminq_u8(vp->new_metrics->v[i], adjustv);
      }

      adjustv = vminq_u8(adjustv, vextq_u8(adjustv, vdupq_n_u8(0), (8)));
      adjustv = vminq_u8(adjustv, vextq_u8(adjustv, vdupq_n_u8(0), (4)));
      adjustv = vminq_u8(adjustv, vextq_u8(adjustv, vdupq_n_u8(0), (2)));
      t.v     = adjustv;
      adjust  = t.w[0];
      adjustv = vld1q_dup_u8(&adjust);

      /* We cannot use a saturated subtract, because we often have to adjust by more than SHRT_MAX
       * This is okay since it can't overflow anyway
       */
      for (i = 0; i < 4; i++) {
        vp->new_metrics->v[i] = vsubq_u8(vp->new_metrics->v[i], adjustv);
      }
    }
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
