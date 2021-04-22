/* Adapted Phil Karn's r=1/3 k=9 viterbi decoder to r=1/3 k=7
 *
 * K=9 r=1/3 Viterbi decoder in portable C
 * Copyright Aug 2006, Phil Karn, KA9Q
 * May be used under the terms of the GNU Lesser General Public License (LGPL)
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "parity.h"
#include "viterbi37.h"
#include <limits.h>
#include <memory.h>

//#define DEBUG

typedef union {
  uint32_t w[64];
} metric_t;
typedef union {
  unsigned long w[2];
} decision_t;

static union {
  uint8_t c[32];
} Branchtab37[3];

/* State info for instance of Viterbi decoder */
struct v37 {
  metric_t    metrics1;                  /* path metric buffer 1 */
  metric_t    metrics2;                  /* path metric buffer 2 */
  decision_t* dp;                        /* Pointer to current decision */
  metric_t *  old_metrics, *new_metrics; /* Pointers to path metrics, swapped on every bit */
  decision_t* decisions;                 /* Beginning of decisions for block */
  uint32_t    len;
};

void clear_v37(struct v37* vp)
{
  bzero(vp->decisions, sizeof(decision_t) * vp->len);
  vp->dp = NULL;
  bzero(&vp->metrics1, sizeof(metric_t));
  bzero(&vp->metrics2, sizeof(metric_t));
  vp->old_metrics = NULL;
  vp->new_metrics = NULL;
}

/* Initialize Viterbi decoder for start of new frame */
int init_viterbi37_port(void* p, int starting_state)
{
  struct v37* vp = p;
  uint32_t    i;

  if (p == NULL)
    return -1;

  clear_v37(vp);

  for (i = 0; i < 64; i++)
    vp->metrics1.w[i] = 63;

  vp->old_metrics = &vp->metrics1;
  vp->new_metrics = &vp->metrics2;
  vp->dp          = vp->decisions;
  if (starting_state != -1) {
    vp->old_metrics->w[starting_state & 255] = 0; /* Bias known start state */
  }
  return 0;
}

void set_viterbi37_polynomial_port(int polys[3])
{
  int state;

  for (state = 0; state < 32; state++) {
    Branchtab37[0].c[state] = (polys[0] < 0) ^ parity((2 * state) & abs(polys[0])) ? 255 : 0;
    Branchtab37[1].c[state] = (polys[1] < 0) ^ parity((2 * state) & abs(polys[1])) ? 255 : 0;
    Branchtab37[2].c[state] = (polys[2] < 0) ^ parity((2 * state) & abs(polys[2])) ? 255 : 0;
  }
}

/* Create a new instance of a Viterbi decoder */
void* create_viterbi37_port(int polys[3], uint32_t len)
{
  struct v37* vp;

  set_viterbi37_polynomial_port(polys);

  if ((vp = (struct v37*)malloc(sizeof(struct v37))) == NULL)
    return NULL;

  if ((vp->decisions = (decision_t*)malloc((len + 6) * sizeof(decision_t))) == NULL) {
    free(vp);
    return NULL;
  }

  vp->len = len + 6;

  return vp;
}

/* Viterbi chainback */
int chainback_viterbi37_port(void*    p,
                             uint8_t* data,  /* Decoded output data */
                             uint32_t nbits, /* Number of data bits */
                             uint32_t endstate)
{ /* Terminal encoder state */
  struct v37* vp = p;
  decision_t* d;

  if (p == NULL)
    return -1;

  d = vp->decisions;

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
  while (nbits-- != 0) {
    int k;

    k           = (d[nbits].w[(endstate >> 2) / 32] >> ((endstate >> 2) % 32)) & 1;
    endstate    = (endstate >> 1) | (k << 7);
    data[nbits] = k;
#ifdef DEBUG
    //   printf("endstate=%3d, k=%d, w[0]=%d, w[1]=%d\n", endstate, k, d[nbits].w[0]&1, d[nbits].w[1]&1);
#endif
  }
  return 0;
}

/* Delete instance of a Viterbi decoder */
void delete_viterbi37_port(void* p)
{
  struct v37* vp = p;

  if (vp != NULL) {
    free(vp->decisions);
    free(vp);
  }
}

/* C-language butterfly */
#define BFLY(i)                                                                                                        \
  {                                                                                                                    \
    uint32_t metric, m0, m1, decision;                                                                                 \
    metric   = (Branchtab37[0].c[i] ^ sym0) + (Branchtab37[1].c[i] ^ sym1) + (Branchtab37[2].c[i] ^ sym2);             \
    m0       = vp->old_metrics->w[i] + metric;                                                                         \
    m1       = vp->old_metrics->w[i + 32] + (765 - metric);                                                            \
    decision = (signed int)(m0 - m1) > 0;                                                                              \
    vp->new_metrics->w[2 * i] = decision ? m1 : m0;                                                                    \
    d->w[i / 16] |= decision << ((2 * i) & 31);                                                                        \
    m0 -= (metric + metric - 765);                                                                                     \
    m1 += (metric + metric - 765);                                                                                     \
    decision                      = (signed int)(m0 - m1) > 0;                                                         \
    vp->new_metrics->w[2 * i + 1] = decision ? m1 : m0;                                                                \
    d->w[i / 16] |= decision << ((2 * i + 1) & 31);                                                                    \
  }

/* Update decoder with a block of demodulated symbols
 * Note that nbits is the number of decoded data bits, not the number
 * of symbols!
 */

int update_viterbi37_blk_port(void* p, uint8_t* syms, uint32_t nbits, uint32_t* best_state)
{
  struct v37* vp = p;
  decision_t* d;

  if (p == NULL)
    return -1;
  uint32_t k = 0;
  d          = (decision_t*)vp->dp;

#ifdef DEBUG
  printf("[");
#endif

  while (nbits--) {
    void*    tmp;
    uint8_t  sym0, sym1, sym2;
    uint32_t i;

    d->w[0] = d->w[1] = 0;

    sym0 = *syms++;
    sym1 = *syms++;
    sym2 = *syms++;

    k++;
    for (i = 0; i < 32; i++)
      BFLY(i);

#ifdef DEBUG
    uint32_t wmin     = UINT_MAX;
    int      minstate = 0;
    for (int j = 0; j < 64; j++) {
      if (vp->new_metrics->w[j] <= wmin) {
        wmin     = vp->new_metrics->w[j];
        minstate = j;
      }
    }

    printf("%3d, ", minstate);
#endif

    d++;
    tmp             = vp->old_metrics;
    vp->old_metrics = vp->new_metrics;
    vp->new_metrics = tmp;
  }
  if (best_state) {
    uint32_t i, bst = 0;
    uint32_t minmetric = UINT_MAX;
    for (i = 0; i < 64; i++) {
      if (vp->old_metrics->w[i] <= minmetric) {
        bst       = i;
        minmetric = vp->old_metrics->w[i];
      }
    }
    *best_state = bst;
  }
  vp->dp = d;

#ifdef DEBUG
  printf("];\n");
#endif

  return 0;
}
