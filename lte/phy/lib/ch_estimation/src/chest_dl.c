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
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <complex.h>

#include "liblte/config.h"

#include "liblte/phy/ch_estimation/chest_dl.h"
#include "liblte/phy/utils/vector.h"

//#define VOLK_INTERP

/** 3GPP LTE Downlink channel estimator and equalizer. 
 * Estimates the channel in the resource elements transmitting references and interpolates for the rest
 * of the resource grid. 
 * 
 * The equalizer uses the channel estimates to produce an estimation of the transmitted symbol. 
 * 
 * This object depends on the refsignal_t object for creating the LTE CSR signal.  
*/

int chest_dl_init(chest_dl_t *q, lte_cell_t cell) 
{
  int ret = LIBLTE_ERROR_INVALID_INPUTS;
  if (q                != NULL &&
      lte_cell_isvalid(&cell)) 
  {
    bzero(q, sizeof(chest_dl_t));
    
    ret = refsignal_cs_generate(&q->csr_signal, cell); 
    if (ret != LIBLTE_SUCCESS) {
      fprintf(stderr, "Error initializing CSR signal (%d)\n",ret);
      goto clean_exit;
    }
    
    q->pilot_symbol_avg = vec_malloc(sizeof(cf_t) * 2*cell.nof_prb);
    if (!q->pilot_symbol_avg) {
      perror("malloc");
      goto clean_exit;
    }
    
    for (int i=0;i<cell.nof_ports;i++) {
      q->pilot_estimates[i] = vec_malloc(sizeof(cf_t) * REFSIGNAL_MAX_NUM_SF(cell.nof_prb));
      if (!q->pilot_estimates[i]) {
        perror("malloc");
        goto clean_exit;
      }      
      q->pilot_recv_signal[i] = vec_malloc(sizeof(cf_t) * REFSIGNAL_MAX_NUM_SF(cell.nof_prb));
      if (!q->pilot_recv_signal[i]) {
        perror("malloc");
        goto clean_exit;
      }
      #ifdef VOLK_INTERP
      ret = interp_init(&q->interp_freq[i], LINEAR, 2*cell.nof_prb, RE_X_RB/2);
      if (ret == LIBLTE_SUCCESS) {
        ret = interp_init(&q->interp_time[i], LINEAR, 2, CP_NSYMB(cell.cp) - 3);
      }     
      #endif

    }
    
    /* Init buffer for holding CE estimates averages */
    
    q->cell = cell; 
  }
  
  ret = LIBLTE_SUCCESS;
  
clean_exit:
  if (ret != LIBLTE_SUCCESS) {
      chest_dl_free(q);
  }
  return ret; 
}

void chest_dl_free(chest_dl_t *q) 
{
  refsignal_cs_free(&q->csr_signal);

  if (q->pilot_symbol_avg) {
    free(q->pilot_symbol_avg);
  }
  
  for (int i=0;i<MAX_PORTS;i++) {
    if (q->pilot_estimates[i]) {
      free(q->pilot_estimates[i]);
    }      
    if (q->pilot_recv_signal[i]) {
      free(q->pilot_recv_signal[i]);
    }
    #ifdef VOLK_INTERP
    interp_free(&q->interp_freq[i]);
    interp_free(&q->interp_time[i]);      
    #endif
  }
}

#define pilot_est(idx) q->pilot_estimates[port_id][REFSIGNAL_PILOT_IDX(idx,l,ns,q->cell)]

#if CHEST_RS_AVERAGE_TIME > 1
cf_t timeavg[CHEST_RS_AVERAGE_TIME-1][12];
int nof_timeavg=0;
#endif

static void average_pilots(chest_dl_t *q, uint32_t sf_idx, uint32_t port_id) 
{
  uint32_t ns, l;
  int i;  
  /* For each symbol with pilots in a slot */
  for (ns=2*sf_idx;ns<2*(sf_idx+1);ns++) {
    for (l=0;l<refsignal_cs_nof_symbols(port_id);l++) {
      bzero(q->pilot_symbol_avg, 2*q->cell.nof_prb);

      /** Frequency average */
#if CHEST_RS_AVERAGE_FREQ > 1
      const uint32_t M = CHEST_RS_AVERAGE_FREQ;
      cf_t xint[CHEST_RS_AVERAGE_FREQ];
      int j, k;
      /* Extrapolate first M/2 samples  */
      for (i=M/2-1;i>=0;i--) {
        k=0;
        for (j=i+M/2;j>=0;j--) {
          xint[k]=pilot_est(j);         
          k++;
        }
        for (;j>=i-M/2;j--) {
          if (k>=2) {
            xint[k] = interp_linear_onesample(&xint[k-2]);
            k++;            
          }
        }
        q->pilot_symbol_avg[i] = vec_acc_cc(xint,M)/M;        
        //q->pilot_symbol_avg[i] = (pilot_est(0)+pilot_est(1))/2;
      }
      
      for (i=M/2;i<2*q->cell.nof_prb-M/2;i++) {
        q->pilot_symbol_avg[i] = vec_acc_cc(&pilot_est(i-M/2),M)/M;
      }

      /* Extrapolate last M/2 samples  */
      for (;i<2*q->cell.nof_prb;i++) {
        k=0;
        for (j=i-M/2;j<2*q->cell.nof_prb;j++) {
          xint[k]=pilot_est(j);         
          k++;
        }
        for (;k<M;k++) {
          if (k>=2) {
            xint[k] = interp_linear_onesample(&xint[k-2]);
          }
        }
        q->pilot_symbol_avg[i] = vec_acc_cc(xint,M)/M;        
        //q->pilot_symbol_avg[i] = (pilot_est(i)+pilot_est(i+1))/2;

      }
#else 
      memcpy(q->pilot_symbol_avg, &pilot_est(0), 2*q->cell.nof_prb*sizeof(cf_t));
#endif
      
      /* Time average last symbols */
#if CHEST_RS_AVERAGE_TIME > 1
      if (nof_timeavg<CHEST_RS_AVERAGE_TIME-1) {
        memcpy(timeavg[nof_timeavg],q->pilot_symbol_avg, 2*q->cell.nof_prb * sizeof(cf_t));
        nof_timeavg++;
      } else {
        bzero(&pilot_est(0),2*q->cell.nof_prb*sizeof(cf_t));
        for (i=0;i<nof_timeavg;i++) {
          vec_sum_ccc(timeavg[i],&pilot_est(0),&pilot_est(0),2*q->cell.nof_prb);          
        }                  
        vec_sum_ccc(q->pilot_symbol_avg,&pilot_est(0),&pilot_est(0),2*q->cell.nof_prb);          
        vec_sc_prod_cfc(&pilot_est(0), 1.0/CHEST_RS_AVERAGE_TIME, &pilot_est(0), 2*q->cell.nof_prb);
        for (i=0;i<nof_timeavg-1;i++) {
          memcpy(timeavg[i],timeavg[i+1],2*q->cell.nof_prb*sizeof(cf_t));
        }        
        memcpy(timeavg[i],q->pilot_symbol_avg,2*q->cell.nof_prb*sizeof(cf_t));
      }
#else
      memcpy(&pilot_est(0), q->pilot_symbol_avg, 2*q->cell.nof_prb * sizeof(cf_t));
#endif
      
    }
  }
  
}

static void interpolate_pilots(chest_dl_t *q, cf_t *ce, uint32_t sf_idx, uint32_t port_id) 
{
  /* interpolate the symbols with references in the freq domain */
  uint32_t ns, l, i,j; 
  cf_t x[2], y[MAX_NSYMB];

  for (ns=2*sf_idx;ns<2*(sf_idx+1);ns++) {
    for (l=0;l<refsignal_cs_nof_symbols(port_id);l++) {
      uint32_t fidx_offset = refsignal_fidx(q->cell, ns, l, port_id, 0);
#ifdef VOLK_INTERP
    interp_run_offset(&q->interp_freq[port_id], 
                      &q->pilot_estimates[port_id][((ns%2)*2+l)*2*q->cell.nof_prb], 
                      &ce[refsignal_nsymbol(q->cell,ns,l) * q->cell.nof_prb * RE_X_RB], 
                      fidx_offset, RE_X_RB/2-fidx_offset);
#else
    interp_linear_offset(&q->pilot_estimates[port_id][((ns%2)*2+l)*2*q->cell.nof_prb],
        &ce[refsignal_nsymbol(q->cell,ns,l) * q->cell.nof_prb * RE_X_RB], RE_X_RB/2,
        2*q->cell.nof_prb, fidx_offset, RE_X_RB/2-fidx_offset);
#endif
    }
  }
  /* now interpolate in the time domain */
  for (i=0;i<RE_X_RB*q->cell.nof_prb; i++) {
    if (refsignal_cs_nof_symbols(port_id) > 1) {
      for (ns=2*sf_idx;ns<2*(sf_idx+1);ns++) {
        j=0;
        for (l=0;l<refsignal_cs_nof_symbols(port_id);l++) {
          x[j] = ce[refsignal_nsymbol(q->cell,ns,l) * q->cell.nof_prb * RE_X_RB + i];
          j++;
        }
  #ifdef VOLK_INTERP
        interp_run_offset(&q->interp_time[port_id], x, y, 
                          0, CP_NSYMB(q->cell.cp) - 4);
  #else
        interp_linear_offset(x, y, CP_NSYMB(q->cell.cp) - 3,
            2, 0, CP_NSYMB(q->cell.cp) - 4);
  #endif
        for (j=0;j<CP_NSYMB(q->cell.cp);j++) {
          ce[(j+((ns%2)*CP_NSYMB(q->cell.cp))) * q->cell.nof_prb*RE_X_RB + i] = y[j];
        }
      }
    } else {
      fprintf(stderr, "3/4 Ports interpolator not implemented\n");
      exit(-1);
    }       
  }
}

int chest_dl_estimate_port(chest_dl_t *q, cf_t *input, cf_t *ce, uint32_t sf_idx, uint32_t port_id) 
{
  //filter2d_reset(&q->filter);
  
  /* Get references from the input signal */
  refsignal_cs_get_sf(q->cell, port_id, sf_idx, input, q->pilot_recv_signal[port_id]);
  
  /* Use the known CSR signal to compute Least-squares estimates */
  vec_div_ccc_mod1(q->pilot_recv_signal[port_id], q->csr_signal.pilots[sf_idx], 
              q->pilot_estimates[port_id], REFSIGNAL_NUM_SF(q->cell.nof_prb, port_id)); 
  
  /* Average pilot estimates */
  average_pilots(q, sf_idx, port_id);
  
  /* Interpolate to create channel estimates for all resource grid */
  interpolate_pilots(q, ce, sf_idx, port_id);
  
  return 0;
}

int chest_dl_estimate(chest_dl_t *q, cf_t *input, cf_t *ce[MAX_PORTS], uint32_t sf_idx) 
{
  uint32_t port_id; 
  
  for (port_id=0;port_id<q->cell.nof_ports;port_id++) {
    chest_dl_estimate_port(q, input, ce[port_id], sf_idx, port_id);
  }
  return LIBLTE_SUCCESS;
}

int chest_dl_equalize_zf(chest_dl_t *q, cf_t *input, cf_t *ce[MAX_PORTS], cf_t *output) 
{
  fprintf(stderr, "Not implemented\n");
  return -1;
}

int chest_dl_equalize_mmse(chest_dl_t *q, cf_t *input, cf_t *ce[MAX_PORTS], float *noise_estimate, cf_t *output)
{
  fprintf(stderr, "Not implemented\n");
  return -1;  
}

float chest_dl_get_rssi(chest_dl_t *q) {
  return q->rssi; 
}

float chest_dl_get_rsrq(chest_dl_t *q) {
  return q->rsrq; 
}

float chest_dl_get_rsrp(chest_dl_t *q) {
  return q->rsrp; 
}

