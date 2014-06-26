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


#include <strings.h>
#include "liblte/phy/common/phy_common.h"
#include "liblte/phy/common/sequence.h"

/**
 * 36.211 6.6.1
 */
int sequence_pbch(sequence_t *seq, lte_cp_t cp, int cell_id) {
  bzero(seq, sizeof(sequence_t));
  return sequence_LTEPRS(seq, CP_ISNORM(cp)?1920:1728, cell_id);
}

/**
 * 36.211 6.7.1
 */
int sequence_pcfich(sequence_t *seq, int nslot, int cell_id) {
  bzero(seq, sizeof(sequence_t));
  return sequence_LTEPRS(seq, 32, (nslot/2+1) * (2*cell_id + 1) * 512 + cell_id);
}


/**
 * 36.211 6.9.1
 */
int sequence_phich(sequence_t *seq, int nslot, int cell_id) {
  bzero(seq, sizeof(sequence_t));
  return sequence_LTEPRS(seq, 12, (nslot/2+1) * (2*cell_id + 1) * 512 + cell_id);
}

/**
 * 36.211 6.8.2
 */
int sequence_pdcch(sequence_t *seq, int nslot, int cell_id, int len) {
  bzero(seq, sizeof(sequence_t));
  return sequence_LTEPRS(seq, len, (nslot/2) * 512 + cell_id);
}

/**
 * 36.211 6.3.1
 */
int sequence_pdsch(sequence_t *seq, unsigned short rnti, int q, int nslot, int cell_id, int len) {
	bzero(seq, sizeof(sequence_t));
	return sequence_LTEPRS(seq, len, (rnti<<14) + (q<<13) + ((nslot/2)<<9) + cell_id);
}
