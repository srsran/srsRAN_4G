/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSRAN_RF_SKIQ_IMP_PORT_H
#define SRSRAN_RF_SKIQ_IMP_PORT_H

#include <sidekiq_api.h>
#include <srsran/phy/rf/rf.h>

#include "rf_helper.h"
#include "rf_skiq_imp_cfg.h"
#include "rf_skiq_imp_port.h"
#include "srsran/srsran.h"

typedef struct {
  uint32_t magic;
  uint64_t tstamp;
  uint32_t nsamples;
} skiq_header_t;

typedef enum {
  RF_SKIQ_PORT_STATE_IDLE = 0,
  RF_SKIQ_PORT_STATE_STREAMING,
  RF_SKIQ_PORT_STATE_STOP
} rf_skiq_port_state_t;

typedef struct {
  uint32_t              tx_rb_size;
  uint32_t              rx_rb_size;
  skiq_chan_mode_t      chan_mode;
  char                  stream_mode_str[RF_PARAM_LEN];
  skiq_rx_stream_mode_t stream_mode;
} rf_skiq_port_opts_t;

typedef struct {
  uint8_t              card;
  skiq_tx_hdl_t        hdl;
  skiq_tx_block_t*     p_tx_block;
  uint32_t             p_block_nbytes; // Size in bytes including header
  uint32_t             block_size;     // Size in words (samples)
  uint32_t             next_offset;    // Number of samples remainder
  srsran_ringbuffer_t  rb;
  rf_skiq_port_state_t state;
  pthread_t            thread;
  pthread_mutex_t      mutex; // Protects p_tx_block

  uint64_t current_lo;

  srsran_rf_error_handler_t error_handler;
  void*                     error_handler_arg;

#if SKIQ_TX_LATES_CHECK_PERIOD
  uint64_t last_check_ts;
  uint32_t last_total_late;
  uint32_t last_total_underruns;
#endif // SKIQ_TX_LATES_CHECK_PERIOD
} rf_skiq_tx_port_t;

int  rf_skiq_tx_port_init(rf_skiq_tx_port_t* q, uint8_t card, skiq_tx_hdl_t hdl, const rf_skiq_port_opts_t* opts);
void rf_skiq_tx_port_free(rf_skiq_tx_port_t* q);
void rf_skiq_tx_port_end_of_burst(rf_skiq_tx_port_t* q);
int  rf_skiq_tx_port_send(rf_skiq_tx_port_t* q, const cf_t* buffer, uint32_t nsamples, uint64_t ts);
void rf_skiq_tx_port_set_error_handler(rf_skiq_tx_port_t* q, srsran_rf_error_handler_t error_handler, void* arg);
int  rf_skiq_tx_port_set_lo(rf_skiq_tx_port_t* q, uint64_t lo_freq);

typedef struct {
  uint8_t             card;
  skiq_rx_hdl_t       hdl;
  srsran_ringbuffer_t rb;
  uint32_t            rb_read_rem;
  uint64_t            rb_tstamp_rem;
  bool                rf_overflow; ///< Indicates an RF message was flagged with overflow
  bool                rb_overflow; ///< Indicates that ring-buffer is full and it needs to be flushed

  uint64_t current_lo;

  srsran_rf_error_handler_t error_handler;
  void*                     error_handler_arg;
} rf_skiq_rx_port_t;

int      rf_skiq_rx_port_init(rf_skiq_rx_port_t* q, uint8_t card, skiq_rx_hdl_t hdl, const rf_skiq_port_opts_t* opts);
void     rf_skiq_rx_port_free(rf_skiq_rx_port_t* q);
int      rf_skiq_rx_port_available(rf_skiq_rx_port_t* q);
int      rf_skiq_rx_port_read(rf_skiq_rx_port_t* q, cf_t* dest, uint32_t nsamples, uint64_t* ts_start);
uint64_t rf_skiq_rx_port_get_timestamp(rf_skiq_rx_port_t* q);
int      rf_skiq_rx_port_write(rf_skiq_rx_port_t* q, const skiq_rx_block_t* p_rx_block, uint32_t nbytes);
void     rf_skiq_rx_port_set_error_handler(rf_skiq_rx_port_t* q, srsran_rf_error_handler_t error_handler, void* arg);
void     rf_skiq_rx_port_reset(rf_skiq_rx_port_t* q);
int      rf_skiq_rx_port_set_lo(rf_skiq_rx_port_t* q, uint64_t lo_freq);

#endif // SRSRAN_RF_SKIQ_IMP_PORT_H
