/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2020 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSLTE_UE_MIB_NBIOT_H
#define SRSLTE_UE_MIB_NBIOT_H

#include "srslte/config.h"
#include "srslte/phy/ch_estimation/chest_dl_nbiot.h"
#include "srslte/phy/dft/ofdm.h"
#include "srslte/phy/phch/npbch.h"
#include "srslte/phy/sync/cfo.h"
#include "srslte/phy/ue/ue_sync_nbiot.h"

#define SRSLTE_UE_MIB_NBIOT_NOF_PRB 6
#define SRSLTE_UE_MIB_NBIOT_FOUND 1
#define SRSLTE_UE_MIB_NBIOT_NOTFOUND 0

/**
 * \brief This object decodes the MIB-NB from the NPBCH of an NB-IoT LTE signal.
 *
 * The function srslte_ue_mib_nbiot_decode() shall be called multiple
 * times, each passing a number of samples multiple of 19200,
 * sampled at 1.92 MHz (that is, 10 ms of samples).
 *
 * The function uses the sync_t object to find the NPSS sequence and
 * decode the NPBCH to obtain the MIB.
 *
 * The function returns 0 until the MIB is decoded.
 */

typedef struct SRSLTE_API {
  srslte_sync_nbiot_t sfind;

  cf_t* sf_symbols;
  cf_t* ce[SRSLTE_MAX_PORTS];

  srslte_ofdm_t           fft;
  srslte_chest_dl_nbiot_t chest;
  srslte_npbch_t          npbch;

  uint8_t  last_bch_payload[SRSLTE_MIB_NB_LEN];
  uint32_t nof_tx_ports;
  uint32_t nof_rx_antennas;
  uint32_t sfn_offset;

  uint32_t frame_cnt;
} srslte_ue_mib_nbiot_t;

SRSLTE_API int srslte_ue_mib_nbiot_init(srslte_ue_mib_nbiot_t* q, cf_t** in_buffer, uint32_t max_prb);

SRSLTE_API int srslte_ue_mib_nbiot_set_cell(srslte_ue_mib_nbiot_t* q, srslte_nbiot_cell_t cell);

SRSLTE_API void srslte_ue_mib_nbiot_free(srslte_ue_mib_nbiot_t* q);

SRSLTE_API void srslte_ue_mib_nbiot_reset(srslte_ue_mib_nbiot_t* q);

SRSLTE_API int srslte_ue_mib_nbiot_decode(srslte_ue_mib_nbiot_t* q,
                                          cf_t*                  input,
                                          uint8_t*               bch_payload,
                                          uint32_t*              nof_tx_ports,
                                          int*                   sfn_offset);

/* This interface uses ue_mib and ue_sync to first get synchronized subframes
 * and then decode MIB
 *
 * This object calls the pbch object with nof_ports=0 for blind nof_ports determination
 */
typedef struct {
  srslte_ue_mib_nbiot_t  ue_mib;
  srslte_nbiot_ue_sync_t ue_sync;
  cf_t*                  sf_buffer[SRSLTE_MAX_PORTS];
  uint32_t               nof_rx_antennas;
} srslte_ue_mib_sync_nbiot_t;

SRSLTE_API int
srslte_ue_mib_sync_nbiot_init_multi(srslte_ue_mib_sync_nbiot_t* q,
                                    int(recv_callback)(void*, cf_t* [SRSLTE_MAX_PORTS], uint32_t, srslte_timestamp_t*),
                                    uint32_t nof_rx_antennas,
                                    void*    stream_handler);

SRSLTE_API void srslte_ue_mib_sync_nbiot_free(srslte_ue_mib_sync_nbiot_t* q);

SRSLTE_API int srslte_ue_mib_sync_nbiot_set_cell(srslte_ue_mib_sync_nbiot_t* q, srslte_nbiot_cell_t cell);

SRSLTE_API void srslte_ue_mib_sync_nbiot_reset(srslte_ue_mib_sync_nbiot_t* q);

SRSLTE_API int srslte_ue_mib_sync_nbiot_decode(srslte_ue_mib_sync_nbiot_t* q,
                                               uint32_t                    max_frames_timeout,
                                               uint8_t*                    bch_payload,
                                               uint32_t*                   nof_tx_ports,
                                               int*                        sfn_offset);

#endif // SRSLTE_UE_MIB_NBIOT_H
