/**
 * Copyright 2013-2023 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#include "srsran/srsran.h"
#include <complex.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include "srsran/phy/ch_estimation/refsignal_dl.h"
#include "srsran/phy/common/phy_common.h"
#include "srsran/phy/common/sequence.h"
#include "srsran/phy/utils/debug.h"
#include "srsran/phy/utils/vector.h"

/** Allocates memory for the 20 slots in a subframe
 */
int srsran_refsignal_cs_init(srsran_refsignal_t* q, uint32_t max_prb)
{
  int ret = SRSRAN_ERROR_INVALID_INPUTS;

  if (q != NULL) {
    ret = SRSRAN_ERROR;
    bzero(q, sizeof(srsran_refsignal_t));
    for (int p = 0; p < 2; p++) {
      for (int i = 0; i < SRSRAN_NOF_SF_X_FRAME; i++) {
        q->pilots[p][i] = srsran_vec_cf_malloc(SRSRAN_REFSIGNAL_MAX_NUM_SF(max_prb));
        if (!q->pilots[p][i]) {
          perror("malloc");
          goto free_and_exit;
        }
      }
    }
    ret = SRSRAN_SUCCESS;
  }
free_and_exit:
  if (ret == SRSRAN_ERROR) {
    srsran_refsignal_free(q);
  }
  return ret;
}

/** Allocates and precomputes the Cell-Specific Reference (CSR) signal for
 * the 20 slots in a subframe
 */
int srsran_refsignal_cs_set_cell(srsran_refsignal_t* q, srsran_cell_t cell)
{
  uint32_t          c_init;
  uint32_t          N_cp, mp;
  srsran_sequence_t seq;
  int               ret = SRSRAN_ERROR_INVALID_INPUTS;

  if (q != NULL && srsran_cell_isvalid(&cell)) {
    if (cell.id != q->cell.id || q->cell.nof_prb == 0) {
      q->cell = cell;

      bzero(&seq, sizeof(srsran_sequence_t));
      if (srsran_sequence_init(&seq, 2 * 2 * SRSRAN_MAX_PRB)) {
        return SRSRAN_ERROR;
      }

      if (SRSRAN_CP_ISNORM(cell.cp)) {
        N_cp = 1;
      } else {
        N_cp = 0;
      }

      srsran_dl_sf_cfg_t sf_cfg;
      ZERO_OBJECT(sf_cfg);

      for (uint32_t ns = 0; ns < SRSRAN_NSLOTS_X_FRAME; ns++) {
        for (uint32_t p = 0; p < 2; p++) {
          sf_cfg.tti        = ns / 2;
          uint32_t nsymbols = srsran_refsignal_cs_nof_symbols(q, &sf_cfg, 2 * p) / 2;
          for (uint32_t l = 0; l < nsymbols; l++) {
            /* Compute sequence init value */
            uint32_t lp = srsran_refsignal_cs_nsymbol(l, cell.cp, 2 * p);
            c_init      = 1024 * (7 * (ns + 1) + lp + 1) * (2 * cell.id + 1) + 2 * cell.id + N_cp;

            /* generate sequence for this symbol and slot */
            srsran_sequence_set_LTE_pr(&seq, 2 * 2 * SRSRAN_MAX_PRB, c_init);

            /* Compute signal */
            for (uint32_t i = 0; i < 2 * q->cell.nof_prb; i++) {
              uint32_t idx = SRSRAN_REFSIGNAL_PILOT_IDX(i, (ns % 2) * nsymbols + l, q->cell);
              mp           = i + SRSRAN_MAX_PRB - cell.nof_prb;
              /* save signal */
              __real__ q->pilots[p][ns / 2][idx] = (1 - 2 * (float)seq.c[2 * mp + 0]) * M_SQRT1_2;
              __imag__ q->pilots[p][ns / 2][idx] = (1 - 2 * (float)seq.c[2 * mp + 1]) * M_SQRT1_2;
            }
          }
        }
      }
      srsran_sequence_free(&seq);
    }
    ret = SRSRAN_SUCCESS;
  }
  return ret;
}

/** Deallocates a srsran_refsignal_cs_t object allocated with srsran_refsignal_cs_init */
void srsran_refsignal_free(srsran_refsignal_t* q)
{
  for (int p = 0; p < 2; p++) {
    for (int i = 0; i < SRSRAN_NOF_SF_X_FRAME; i++) {
      if (q->pilots[p][i]) {
        free(q->pilots[p][i]);
      }
    }
  }
  bzero(q, sizeof(srsran_refsignal_t));
}

uint32_t srsran_refsignal_cs_v(uint32_t port_id, uint32_t ref_symbol_idx)
{
  uint32_t v = 0;
  switch (port_id) {
    case 0:
      if (!(ref_symbol_idx % 2)) {
        v = 0;
      } else {
        v = 3;
      }
      break;
    case 1:
      if (!(ref_symbol_idx % 2)) {
        v = 3;
      } else {
        v = 0;
      }
      break;
    case 2:
      if (ref_symbol_idx == 0) {
        v = 0;
      } else {
        v = 3;
      }
      break;
    case 3:
      if (ref_symbol_idx == 0) {
        v = 3;
      } else {
        v = 0;
      }
      break;
  }
  return v;
}

inline uint32_t srsran_refsignal_cs_nof_symbols(srsran_refsignal_t* q, srsran_dl_sf_cfg_t* sf, uint32_t port_id)
{
  if (q == NULL || sf == NULL || q->cell.frame_type == SRSRAN_FDD || !sf->tdd_config.configured ||
      srsran_sfidx_tdd_type(sf->tdd_config, sf->tti % 10) == SRSRAN_TDD_SF_D) {
    if (port_id < 2) {
      return 4;
    } else {
      return 2;
    }
  } else {
    uint32_t nof_dw_symbols = srsran_sfidx_tdd_nof_dw(sf->tdd_config);
    if (q->cell.cp == SRSRAN_CP_NORM) {
      if (nof_dw_symbols >= 12) {
        if (port_id < 2) {
          return 4;
        } else {
          return 2;
        }
      } else if (nof_dw_symbols >= 9) {
        if (port_id < 2) {
          return 3;
        } else {
          return 2;
        }
      } else if (nof_dw_symbols >= 5) {
        if (port_id < 2) {
          return 2;
        } else {
          return 1;
        }
      } else {
        return 1;
      }
    } else {
      if (nof_dw_symbols >= 10) {
        if (port_id < 2) {
          return 4;
        } else {
          return 2;
        }
      } else if (nof_dw_symbols >= 8) {
        if (port_id < 2) {
          return 3;
        } else {
          return 2;
        }
      } else if (nof_dw_symbols >= 4) {
        if (port_id < 2) {
          return 2;
        } else {
          return 1;
        }
      } else {
        return 1;
      }
    }
  }
}

inline uint32_t srsran_refsignal_cs_nof_pilots_x_slot(uint32_t nof_ports)
{
  switch (nof_ports) {
    case 2:
      return 8;
    case 4:
      return 12;
    default:
      return 4;
  }
}

inline uint32_t srsran_refsignal_cs_nof_re(srsran_refsignal_t* q, srsran_dl_sf_cfg_t* sf, uint32_t port_id)
{
  uint32_t nof_re = srsran_refsignal_cs_nof_symbols(q, sf, port_id);
  if (q != NULL) {
    nof_re *= q->cell.nof_prb * 2; // 2 RE per PRB
  }
  return nof_re;
}

inline uint32_t srsran_refsignal_cs_fidx(srsran_cell_t cell, uint32_t l, uint32_t port_id, uint32_t m)
{
  return 6 * m + ((srsran_refsignal_cs_v(port_id, l) + (cell.id % 6)) % 6);
}

inline uint32_t srsran_refsignal_cs_nsymbol(uint32_t l, srsran_cp_t cp, uint32_t port_id)
{
  if (port_id < 2) {
    if (l % 2) {
      return (l / 2 + 1) * SRSRAN_CP_NSYMB(cp) - 3;
    } else {
      return (l / 2) * SRSRAN_CP_NSYMB(cp);
    }
  } else {
    return 1 + l * SRSRAN_CP_NSYMB(cp);
  }
}

/* Maps a reference signal initialized with srsran_refsignal_cs_init() into an array of subframe symbols */
int srsran_refsignal_cs_put_sf(srsran_refsignal_t* q, srsran_dl_sf_cfg_t* sf, uint32_t port_id, cf_t* sf_symbols)
{
  uint32_t i, l;
  uint32_t fidx;

  if (q != NULL && port_id < SRSRAN_MAX_PORTS && sf_symbols != NULL) {
    cf_t* pilots = q->pilots[port_id / 2][sf->tti % 10];
    for (l = 0; l < srsran_refsignal_cs_nof_symbols(q, sf, port_id); l++) {
      uint32_t nsymbol = srsran_refsignal_cs_nsymbol(l, q->cell.cp, port_id);
      /* Compute offset frequency index */
      fidx = ((srsran_refsignal_cs_v(port_id, l) + (q->cell.id % 6)) % 6);
      for (i = 0; i < 2 * q->cell.nof_prb; i++) {
        sf_symbols[SRSRAN_RE_IDX(q->cell.nof_prb, nsymbol, fidx)] = pilots[SRSRAN_REFSIGNAL_PILOT_IDX(i, l, q->cell)];
        fidx += SRSRAN_NRE / 2; // 1 reference every 6 RE
      }
    }
    return SRSRAN_SUCCESS;
  } else {
    return SRSRAN_ERROR_INVALID_INPUTS;
  }
}

/** Copies the RE containing references from an array of subframe symbols to the pilots array. */
int srsran_refsignal_cs_get_sf(srsran_refsignal_t* q,
                               srsran_dl_sf_cfg_t* sf,
                               uint32_t            port_id,
                               cf_t*               sf_symbols,
                               cf_t*               pilots)
{
  uint32_t i, l;
  uint32_t fidx;

  if (q != NULL && pilots != NULL && sf_symbols != NULL) {
    for (l = 0; l < srsran_refsignal_cs_nof_symbols(q, sf, port_id); l++) {
      uint32_t nsymbol = srsran_refsignal_cs_nsymbol(l, q->cell.cp, port_id);
      /* Compute offset frequency index */
      fidx = srsran_refsignal_cs_fidx(q->cell, l, port_id, 0);
      for (i = 0; i < 2 * q->cell.nof_prb; i++) {
        pilots[SRSRAN_REFSIGNAL_PILOT_IDX(i, l, q->cell)] = sf_symbols[SRSRAN_RE_IDX(q->cell.nof_prb, nsymbol, fidx)];
        fidx += SRSRAN_NRE / 2; // 2 references per PRB
      }
    }
    return SRSRAN_SUCCESS;
  } else {
    return SRSRAN_ERROR_INVALID_INPUTS;
  }
}

SRSRAN_API int srsran_refsignal_mbsfn_put_sf(srsran_cell_t cell,
                                             uint32_t      port_id,
                                             cf_t*         cs_pilots,
                                             cf_t*         mbsfn_pilots,
                                             cf_t*         sf_symbols)
{
  uint32_t i, l;
  uint32_t fidx;

  if (srsran_cell_isvalid(&cell) && srsran_portid_isvalid(port_id) && cs_pilots != NULL && mbsfn_pilots != NULL &&
      sf_symbols != NULL) {
    // adding CS refs for the non-mbsfn section of the sub-frame
    fidx = ((srsran_refsignal_cs_v(port_id, 0) + (cell.id % 6)) % 6);
    for (i = 0; i < 2 * cell.nof_prb; i++) {
      sf_symbols[SRSRAN_RE_IDX(cell.nof_prb, 0, fidx)] = cs_pilots[SRSRAN_REFSIGNAL_PILOT_IDX(i, 0, cell)];
      fidx += SRSRAN_NRE / 2; // 1 reference every 6 RE
    }

    for (l = 0; l < srsran_refsignal_mbsfn_nof_symbols(); l++) {
      uint32_t nsymbol = srsran_refsignal_mbsfn_nsymbol(l);
      fidx             = srsran_refsignal_mbsfn_fidx(l);
      for (i = 0; i < 6 * cell.nof_prb; i++) {
        sf_symbols[SRSRAN_RE_IDX(cell.nof_prb, nsymbol, fidx)] =
            mbsfn_pilots[SRSRAN_REFSIGNAL_PILOT_IDX_MBSFN(i, l, cell)];
        fidx += SRSRAN_NRE / 6;
      }
    }

    return SRSRAN_SUCCESS;
  } else {
    return SRSRAN_ERROR_INVALID_INPUTS;
  }
}

uint32_t srsran_refsignal_mbsfn_nof_symbols()
{
  return 3;
}

inline uint32_t srsran_refsignal_mbsfn_fidx(uint32_t l)
{
  uint32_t ret = 0;
  if (l == 0) {
    ret = 0;
  } else if (l == 1) {
    ret = 1;
  } else if (l == 2) {
    ret = 0;
  }

  return ret;
}
inline uint32_t srsran_refsignal_mbsfn_nsymbol(uint32_t l)
{
  uint32_t ret = 0;
  if (l == 0) {
    ret = 2;
  } else if (l == 1) {
    ret = 6;
  } else if (l == 2) {
    ret = 10;
  }

  return ret;
}

int srsran_refsignal_mbsfn_gen_seq(srsran_refsignal_t* q, srsran_cell_t cell, uint32_t N_mbsfn_id)
{
  uint32_t c_init;
  uint32_t i, ns, l, p;
  uint32_t mp;
  int      ret = SRSRAN_ERROR;

  srsran_sequence_t seq_mbsfn;
  bzero(&seq_mbsfn, sizeof(srsran_sequence_t));
  if (srsran_sequence_init(&seq_mbsfn, 20 * SRSRAN_MAX_PRB)) {
    goto free_and_exit;
  }

  for (ns = 0; ns < SRSRAN_NOF_SF_X_FRAME; ns++) {
    for (p = 0; p < 2; p++) {
      uint32_t nsymbols = 3; // replace with function
      for (l = 0; l < nsymbols; l++) {
        uint32_t lp   = (srsran_refsignal_mbsfn_nsymbol(l)) % 6;
        uint32_t slot = (l) ? (ns * 2 + 1) : (ns * 2);
        c_init        = 512 * (7 * (slot + 1) + lp + 1) * (2 * N_mbsfn_id + 1) + N_mbsfn_id;
        srsran_sequence_set_LTE_pr(&seq_mbsfn, SRSRAN_MAX_PRB * 20, c_init);
        for (i = 0; i < 6 * q->cell.nof_prb; i++) {
          uint32_t idx                   = SRSRAN_REFSIGNAL_PILOT_IDX_MBSFN(i, l, q->cell);
          mp                             = i + 3 * (SRSRAN_MAX_PRB - cell.nof_prb);
          __real__ q->pilots[p][ns][idx] = (1 - 2 * (float)seq_mbsfn.c[2 * mp + 0]) * M_SQRT1_2;
          __imag__ q->pilots[p][ns][idx] = (1 - 2 * (float)seq_mbsfn.c[2 * mp + 1]) * M_SQRT1_2;
        }
      }
    }
  }

  srsran_sequence_free(&seq_mbsfn);
  ret = SRSRAN_SUCCESS;

free_and_exit:
  if (ret == SRSRAN_ERROR) {
    srsran_sequence_free(&seq_mbsfn);
    srsran_refsignal_free(q);
  }
  return ret;
}

int srsran_refsignal_mbsfn_init(srsran_refsignal_t* q, uint32_t max_prb)
{
  int      ret = SRSRAN_ERROR_INVALID_INPUTS;
  uint32_t i, p;
  if (q != NULL) {
    ret = SRSRAN_ERROR;
    bzero(q, sizeof(srsran_refsignal_t));

    q->type = SRSRAN_SF_MBSFN;

    for (p = 0; p < 2; p++) {
      for (i = 0; i < SRSRAN_NOF_SF_X_FRAME; i++) {
        q->pilots[p][i] = srsran_vec_cf_malloc(max_prb * 18);
        if (!q->pilots[p][i]) {
          perror("malloc");
          goto free_and_exit;
        }
      }
    }

    ret = SRSRAN_SUCCESS;
  }

free_and_exit:
  if (ret == SRSRAN_ERROR) {
    srsran_refsignal_free(q);
  }
  return ret;
}

int srsran_refsignal_mbsfn_set_cell(srsran_refsignal_t* q, srsran_cell_t cell, uint16_t mbsfn_area_id)
{
  int ret = SRSRAN_SUCCESS;

  if (q == NULL) {
    ret = SRSRAN_ERROR_INVALID_INPUTS;
    goto exit;
  }

  q->cell          = cell;
  q->mbsfn_area_id = mbsfn_area_id;
  if (srsran_refsignal_mbsfn_gen_seq(q, q->cell, q->mbsfn_area_id) < SRSRAN_SUCCESS) {
    ret = SRSRAN_ERROR;
    goto exit;
  }

exit:
  return ret;
}

int srsran_refsignal_mbsfn_get_sf(srsran_cell_t cell, uint32_t port_id, cf_t* sf_symbols, cf_t* pilots)
{
  uint32_t i, l;
  uint32_t fidx;
  uint32_t nsymbol;
  if (srsran_cell_isvalid(&cell) && srsran_portid_isvalid(port_id) && pilots != NULL && sf_symbols != NULL) {
    // getting refs from non mbsfn section of subframe
    nsymbol = srsran_refsignal_cs_nsymbol(0, cell.cp, port_id);
    fidx    = ((srsran_refsignal_cs_v(port_id, 0) + (cell.id % 6)) % 6);
    for (i = 0; i < 2 * cell.nof_prb; i++) {
      pilots[SRSRAN_REFSIGNAL_PILOT_IDX(i, 0, cell)] = sf_symbols[SRSRAN_RE_IDX(cell.nof_prb, nsymbol, fidx)];
      fidx += SRSRAN_NRE / 2; // 2 references per PRB
    }

    for (l = 0; l < srsran_refsignal_mbsfn_nof_symbols(); l++) {
      nsymbol = srsran_refsignal_mbsfn_nsymbol(l);
      fidx    = srsran_refsignal_mbsfn_fidx(l);
      for (i = 0; i < 6 * cell.nof_prb; i++) {
        pilots[SRSRAN_REFSIGNAL_PILOT_IDX_MBSFN(i, l, cell) + (2 * cell.nof_prb)] =
            sf_symbols[SRSRAN_RE_IDX(cell.nof_prb, nsymbol, fidx)];
        fidx += SRSRAN_NRE / 6;
      }
    }

    return SRSRAN_SUCCESS;
  } else {
    return SRSRAN_ERROR_INVALID_INPUTS;
  }
}
