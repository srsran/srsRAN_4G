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


#ifndef DCI_
#define DCI_

#include "lte/common/base.h"

typedef _Complex float cf_t;

/**
 * DCI message generation according to the formats, as specified in
 * 36.212 Section 5.3.3.1
 *
 * Call the function dci_init(&q) to generate a collection of DCI messages
 * to be transmitted in a subframe. Each subsequent call to
 * dci_add_formatXX(&q, ...) generates the DCI message and appends the data
 * to the collection "q".
 *
 */

#define DCI_MAX_BITS	45

typedef enum {
	FORMAT0,
	FORMAT1,
	FORMAT1A,
	/* ... */
}dci_format_t;

typedef enum {
	DCI_COMMON=0, DCI_UE=1
}dci_spec_t;

/** TODO: this is Release 8 */
typedef struct {
	/* 36.213 Table 8.4-2: hop_half is 0 for < 10 Mhz and 10 for > 10 Mh.
	 * hop_quart is 00 for > 10 Mhz and hop_quart_neg is 01 for > 10 Mhz.
	 */
	enum {hop_disabled, hop_half, hop_quart, hop_quart_neg, hop_type_2} freq_hop_fl;
	int n_rb_ul; // number of resource blocks
	int riv; // Resource Indication Value (36.213 8.1)
	int mcs_and_rv; // MCS and RV value
	enum {ndi_true=1, ndi_false=0} ndi; // New Data Indicator
	int tpc; // Transmit Power Control
	int dm_rs; // DM RS
	enum {cqi_true=0, cqi_false=1} cqi_request;
}dci_format0_t;

typedef struct {

}dci_format1_t;

typedef struct {
	unsigned char nof_bits;
	unsigned char L; // Aggregation level
	unsigned char ncce; // Position of first CCE of the dci
	unsigned short rnti;
}dci_candidate_t;

typedef struct {
	char data[DCI_MAX_BITS];
	dci_candidate_t location;
}dci_msg_t;

typedef struct {
	dci_msg_t *msg;
	int nof_dcis;
}dci_t;


int dci_init(dci_t *q, int nof_dci);
void dci_free(dci_t *q);
void dci_candidate_fprint(FILE *f, dci_candidate_t *q);

int dci_format0_add(dci_t *q, dci_format0_t *msg, int L, int nCCE, unsigned short rnti);
int dci_format0_sizeof(int nof_prb);

int dci_format1_add(dci_t *q, dci_format1_t *msg, int L, int nCCE, unsigned short rnti);
int dci_format1_sizeof(int nof_prb, int P);

int dci_format1A_add(dci_t *q, dci_format1_t *msg);
int dci_format1A_sizeof(int nof_prb, bool random_access_initiated);

int dci_format1C_add(dci_t *q, dci_format1_t *msg);
int dci_format1C_sizeof();

#endif
