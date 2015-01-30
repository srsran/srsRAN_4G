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

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <math.h>

#include "liblte/phy/phch/uci.h"
#include "liblte/phy/phch/harq.h"
#include "liblte/phy/common/phy_common.h"
#include "liblte/phy/utils/vector.h"


/* Encode UCI-CQI */
int uci_encode_cqi(uint8_t *data, uint8_t *e_bits, uint32_t tbs, uint32_t nb_e)
{
  fprintf(stderr, "Not implemented\n");
  return -1; 
}

static uint32_t Q_prime(uint32_t O, float beta, harq_t *harq_process) {
  uint32_t M_sc = harq_process->prb_alloc.slot[0].nof_prb * RE_X_RB;
  
  uint32_t K = harq_process->cb_segm.C1*harq_process->cb_segm.K1 + 
    harq_process->cb_segm.C2*harq_process->cb_segm.K2;
  uint32_t M_sc_init = harq_process->nof_prb_pusch_init * RE_X_RB;
    
  uint32_t x = (uint32_t) ceilf((float) O*M_sc_init*harq_process->N_symb_ul*beta/K);

  printf("%d=%d*%d*%d*%f/%d\n",x,O,M_sc_init,harq_process->N_symb_ul,beta,K);

  uint32_t Q_prime = MIN(x, 4*M_sc);

  return Q_prime; 
}

/* Encode UCI RI and HARQ bits 
 *  Currently only supporting 1-bit RI or 1-bit HARQ
 */
uint32_t uci_encode_ri_ack(uint8_t data, float beta, uint8_t *q_bits, harq_t *harq_process)
{
  uint8_t Q_m = lte_mod_bits_x_symbol(harq_process->mcs.mod);
  
  q_bits[0] = data;
  q_bits[1] = 2; 
  for (int i=2;i<Q_m;i++) {
    q_bits[i] = 3;
  }
  
  uint32_t Qprime = Q_prime(1, beta, harq_process);

  for (int i=1;i<Qprime;i++) {
    memcpy(&q_bits[i*Q_m], q_bits, Q_m*sizeof(uint8_t));  
  }
  
  printf("Q_m: %d, Qprime: %d, beta: %f\n", Q_m, Qprime, beta);
  
  return Qprime * Q_m;
}

