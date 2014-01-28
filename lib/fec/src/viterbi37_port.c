/* K=9 r=1/3 Viterbi decoder in portable C
 * Copyright Aug 2006, Phil Karn, KA9Q
 * May be used under the terms of the GNU Lesser General Public License (LGPL)
 */
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

#include "parity.h"
#include "viterbi37.h"
#include "utils/debug.h"

typedef union {
	unsigned int w[64];
} metric_t;
typedef union {
	unsigned long w[2];
} decision_t;

static union {
	unsigned char c[32];
} Branchtab37[3];

#define DEB 0

/* State info for instance of Viterbi decoder */
struct v37 {
	metric_t metrics1; /* path metric buffer 1 */
	metric_t metrics2; /* path metric buffer 2 */
	decision_t *dp; /* Pointer to current decision */
	metric_t *old_metrics, *new_metrics; /* Pointers to path metrics, swapped on every bit */
	decision_t *decisions; /* Beginning of decisions for block */
};

/* Initialize Viterbi decoder for start of new frame */
int init_viterbi37_port(void *p, int starting_state) {
	struct v37 *vp = p;
	int i;

	if (p == NULL)
		return -1;

	for (i = 0; i < 64; i++)
		vp->metrics1.w[i] = 63;

	vp->old_metrics = &vp->metrics1;
	vp->new_metrics = &vp->metrics2;
	vp->dp = vp->decisions;
	if (starting_state != -1) {
		vp->old_metrics->w[starting_state & 63] = 0; /* Bias known start state */
	}
	return 0;
}

void set_viterbi37_polynomial_port(int polys[3]) {
	int state;

	for (state = 0; state < 32; state++) {
		Branchtab37[0].c[state] =
				(polys[0] < 0) ^ parity((2 * state) & abs(polys[0])) ? 255 : 0;
		Branchtab37[1].c[state] =
				(polys[1] < 0) ^ parity((2 * state) & abs(polys[1])) ? 255 : 0;
		Branchtab37[2].c[state] =
				(polys[2] < 0) ^ parity((2 * state) & abs(polys[2])) ? 255 : 0;
	}
}

/* Create a new instance of a Viterbi decoder */
void *create_viterbi37_port(int polys[3], int len, bool tail_biting) {
	struct v37 *vp;

	set_viterbi37_polynomial_port(polys);

	if ((vp = (struct v37 *) malloc(sizeof(struct v37))) == NULL)
		return NULL ;

	if ((vp->decisions = (decision_t *) malloc((len + 6) * sizeof(decision_t)))
			== NULL) {
		free(vp);
		return NULL ;
	}
	init_viterbi37_port(vp, tail_biting?-1:0);

	return vp;
}

/* Viterbi chainback */
int chainback_viterbi37_port(void *p, char *data, /* Decoded output data */
	unsigned int nbits, /* Number of data bits */
	unsigned int endstate) { /* Terminal encoder state */

	struct v37 *vp = p;
	decision_t *d;

	if (p == NULL)
		return -1;

	d = vp->decisions;

	/* Make room beyond the end of the encoder register so we can
	 * accumulate a full byte of decoded data
	 */

	endstate=0;
	/* The store into data[] only needs to be done every 8 bits.
	 * But this avoids a conditional branch, and the writes will
	 * combine in the cache anyway
	 */
	d += 6; /* Look past tail */
	while (nbits-- != 0) {
		int k;

		k = (d[nbits].w[(endstate >> 2) / 32] >> ((endstate >> 2) % 32)) & 1;
		endstate = (endstate >> 1) | (k << 7);
		data[nbits] = k;
	}
	return 0;
}

/* Delete instance of a Viterbi decoder */
void delete_viterbi37_port(void *p) {
	struct v37 *vp = p;

	if (vp != NULL) {
		free(vp->decisions);
		free(vp);
	}
}

/* C-language butterfly */
#define BFLY(i) {\
unsigned int metric,m0,m1,decision;\
    metric = (Branchtab37[0].c[i] ^ sym0) + (Branchtab37[1].c[i] ^ sym1) + \
     (Branchtab37[2].c[i] ^ sym2);\
    m0 = vp->old_metrics->w[i] + metric;\
    m1 = vp->old_metrics->w[i+32] + (765 - metric);\
    decision = (signed int)(m0-m1) > 0;\
    vp->new_metrics->w[2*i] = decision ? m1 : m0;\
    d->w[i/16] |= decision << ((2*i)&31);\
    m0 -= (metric+metric-765);\
    m1 += (metric+metric-765);\
    decision = (signed int)(m0-m1) > 0;\
    vp->new_metrics->w[2*i+1] = decision ? m1 : m0;\
    d->w[i/16] |= decision << ((2*i+1)&31);\
}

unsigned char tochar_clip(float sym, float amp) {
	float ret = sym * (127.5 / amp) + 127.5;
	if (ret > 255) {
		ret = 255;
	}
	if (ret < 0) {
		ret = 0;
	}
	return (unsigned char) ret;
}

/* Update decoder with a block of demodulated symbols
 * Note that nbits is the number of decoded data bits, not the number
 * of symbols!
 */

int update_viterbi37_blk_port(void *p, float *syms, int nbits, float amp, int framebits) {
	struct v37 *vp = p;
	decision_t *d;

	if (p == NULL)
		return -1;
	int k=0;
	d = (decision_t *) vp->dp;

	while (nbits--) {
		void *tmp;
		unsigned char sym0, sym1, sym2;

		d->w[0] = d->w[1] = 0;

		k++;

		if (k < framebits) {
			sym0 = tochar_clip(*syms++, amp);
			sym1 = tochar_clip(*syms++, amp);
			sym2 = tochar_clip(*syms++, amp);
		} else {
			sym0=255;
			sym1=255;
			sym2=255;
		}

		BFLY(0);
		BFLY(1);
		BFLY(2);
		BFLY(3);
		BFLY(4);
		BFLY(5);
		BFLY(6);
		BFLY(7);
		BFLY(8);
		BFLY(9);
		BFLY(10);
		BFLY(11);
		BFLY(12);
		BFLY(13);
		BFLY(14);
		BFLY(15);
		BFLY(16);
		BFLY(17);
		BFLY(18);
		BFLY(19);
		BFLY(20);
		BFLY(21);
		BFLY(22);
		BFLY(23);
		BFLY(24);
		BFLY(25);
		BFLY(26);
		BFLY(27);
		BFLY(28);
		BFLY(29);
		BFLY(30);
		BFLY(31);

		d++;
		tmp = vp->old_metrics;
		vp->old_metrics = vp->new_metrics;
		vp->new_metrics = tmp;
	}
	vp->dp = d;
	return 0;
}
