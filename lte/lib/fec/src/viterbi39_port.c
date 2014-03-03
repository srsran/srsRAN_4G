/* K=9 r=1/3 Viterbi decoder in portable C
 * Copyright Aug 2006, Phil Karn, KA9Q
 * May be used under the terms of the GNU Lesser General Public License (LGPL)
 */
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include "viterbi39.h"
#include "parity.h"

typedef union {
	unsigned int w[256];
} metric_t;
typedef union {
	unsigned long w[8];
} decision_t;

static union {
	unsigned char c[128];
} Branchtab39[3];

/* State info for instance of Viterbi decoder */
struct v39 {
	metric_t metrics1; /* path metric buffer 1 */
	metric_t metrics2; /* path metric buffer 2 */
	decision_t *dp; /* Pointer to current decision */
	metric_t *old_metrics, *new_metrics; /* Pointers to path metrics, swapped on every bit */
	decision_t *decisions; /* Beginning of decisions for block */
};

/* Initialize Viterbi decoder for start of new frame */
int init_viterbi39_port(void *p, int starting_state) {
	struct v39 *vp = p;
	int i;

	if (p == NULL)
		return -1;
	for (i = 0; i < 256; i++)
		vp->metrics1.w[i] = 63;

	vp->old_metrics = &vp->metrics1;
	vp->new_metrics = &vp->metrics2;
	vp->dp = vp->decisions;
	vp->old_metrics->w[starting_state & 255] = 0; /* Bias known start state */
	return 0;
}

void set_viterbi39_polynomial_port(int polys[3]) {
	int state;

	for (state = 0; state < 128; state++) {
		Branchtab39[0].c[state] =
				(polys[0] < 0) ^ parity((2 * state) & abs(polys[0])) ? 255 : 0;
		Branchtab39[1].c[state] =
				(polys[1] < 0) ^ parity((2 * state) & abs(polys[1])) ? 255 : 0;
		Branchtab39[2].c[state] =
				(polys[2] < 0) ^ parity((2 * state) & abs(polys[2])) ? 255 : 0;
	}
}

/* Create a new instance of a Viterbi decoder */
void *create_viterbi39_port(int polys[3], int len) {
	struct v39 *vp;

	set_viterbi39_polynomial_port(polys);

	if ((vp = (struct v39 *) malloc(sizeof(struct v39))) == NULL)
		return NULL ;

	if ((vp->decisions = (decision_t *) malloc((len + 8) * sizeof(decision_t)))
			== NULL) {
		free(vp);
		return NULL ;
	}
	init_viterbi39_port(vp, 0);

	return vp;
}

/* Viterbi chainback */
int chainback_viterbi39_port(void *p, char *data, /* Decoded output data */
		unsigned int nbits, /* Number of data bits */
		unsigned int endstate) { /* Terminal encoder state */
	struct v39 *vp = p;
	decision_t *d;

	if (p == NULL)
		return -1;

	d = vp->decisions;
	/* Make room beyond the end of the encoder register so we can
	 * accumulate a full byte of decoded data
	 */
	endstate %= 256;

	/* The store into data[] only needs to be done every 8 bits.
	 * But this avoids a conditional branch, and the writes will
	 * combine in the cache anyway
	 */
	d += 8; /* Look past tail */
	while (nbits-- != 0) {
		int k;

		k = (d[nbits].w[(endstate) / 32] >> (endstate % 32)) & 1;
		endstate = (endstate >> 1) | (k << 7);
		data[nbits] = k;
	}
	return 0;
}

/* Delete instance of a Viterbi decoder */
void delete_viterbi39_port(void *p) {
	struct v39 *vp = p;

	if (vp != NULL) {
		free(vp->decisions);
		free(vp);
	}
}

/* C-language butterfly */
#define BFLY(i) {\
unsigned int metric,m0,m1,decision;\
    metric = (Branchtab39[0].c[i] ^ sym0) + (Branchtab39[1].c[i] ^ sym1) + \
     (Branchtab39[2].c[i] ^ sym2);\
    m0 = vp->old_metrics->w[i] + metric;\
    m1 = vp->old_metrics->w[i+128] + (765 - metric);\
    decision = (signed int)(m0-m1) > 0;\
    vp->new_metrics->w[2*i] = decision ? m1 : m0;\
    d->w[i/16] |= decision << ((2*i)&31);\
    m0 -= (metric+metric-765);\
    m1 += (metric+metric-765);\
    decision = (signed int)(m0-m1) > 0;\
    vp->new_metrics->w[2*i+1] = decision ? m1 : m0;\
    d->w[i/16] |= decision << ((2*i+1)&31);\
}

/* Update decoder with a block of demodulated symbols
 * Note that nbits is the number of decoded data bits, not the number
 * of symbols!
 */

int update_viterbi39_blk_port(void *p, unsigned char *syms, int nbits) {
	struct v39 *vp = p;
	decision_t *d;

	if (p == NULL)
		return -1;

	d = (decision_t *) vp->dp;
	while (nbits--) {
		void *tmp;
		unsigned char sym0, sym1, sym2;
		int i;

		for (i = 0; i < 8; i++)
			d->w[i] = 0;
		sym0 = *syms++;
		sym1 = *syms++;
		sym2 = *syms++;

		for (i = 0; i < 128; i++)
			BFLY(i);

		d++;
		tmp = vp->old_metrics;
		vp->old_metrics = vp->new_metrics;
		vp->new_metrics = tmp;
	}
	vp->dp = d;
	return 0;
}
