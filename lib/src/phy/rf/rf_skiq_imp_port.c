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

#include "rf_skiq_imp_port.h"

static void rf_skiq_rx_port_handle_overflow(rf_skiq_rx_port_t* q)
{
  srsran_rf_error_t error = {};

  error.type = SRSRAN_RF_ERROR_OVERFLOW;
  if (q->error_handler) {
    q->error_handler(q->error_handler_arg, error);
  } else {
    SKIQ_RF_INFO("Rx overflow detected in %d:%d\n", q->card, (int)q->hdl);
  }
}

#if SKIQ_TX_LATES_CHECK_PERIOD
static bool rf_skiq_tx_port_handle_late(rf_skiq_tx_port_t* q)
{
  // Get number of lattes from FPGA
  uint32_t total_late = 0;
  if (skiq_read_tx_num_late_timestamps(q->card, q->hdl, &total_late)) {
    ERROR("Error reading lates from port %d:%d\n", q->card, (int)q->hdl);
  }

  // Calculate number of late timestamps
  uint32_t new_late = total_late;

  // Remove previous read value
  if (new_late >= q->last_total_late) {
    new_late = new_late - q->last_total_late;
  }

  // Update latest value
  q->last_total_late = total_late;

  // No late, do not report them
  if (new_late == 0) {
    return false;
  }

  if (q->error_handler) {
    srsran_rf_error_t error = {};
    error.type              = SRSRAN_RF_ERROR_LATE;
    error.opt               = new_late;
    q->error_handler(q->error_handler_arg, error);
  } else {
    SKIQ_RF_INFO("Port %d late events detected in %d:%d\n", new_late, q->card, (int)q->hdl);
  }

  return true;
}
#endif // SKIQ_TX_LATES_CHECK_PERIOD

static void* writer_thread(void* arg)
{
  uint64_t           last_tx_ts = 0;
  rf_skiq_tx_port_t* q          = (rf_skiq_tx_port_t*)arg;
  skiq_tx_block_t*   p_tx_block = NULL;

  if (skiq_start_tx_streaming(q->card, q->hdl)) {
    ERROR("Error starting Tx stream %d:%d\n", q->card, (int)q->hdl);
    return NULL;
  }

  q->state = RF_SKIQ_PORT_STATE_STREAMING;

  while (q->state != RF_SKIQ_PORT_STATE_STOP) {
    // Read block from ring-buffer
    int n = srsran_ringbuffer_read_block(&q->rb, (void**)&p_tx_block, q->p_block_nbytes, 1000);

    // Stop state is detected
    if (q->state == RF_SKIQ_PORT_STATE_STOP) {
      break;
    }

    // If the ring buffer read resulted in timeout
    if (n == SRSRAN_ERROR_TIMEOUT) {
      continue;
    }

    // Ignore blocks with TS=0
    if (p_tx_block->timestamp == 0) {
      continue;
    }

    // Check if the timestamp is the past (this can be caused by sample rate change)
    if (last_tx_ts > p_tx_block->timestamp) {

      // Get current RF timestamp
      uint64_t curr_tx_ts = 0UL;
      skiq_read_curr_tx_timestamp(q->card, q->hdl, &curr_tx_ts);

      // Avoids giving a block to the FPGA that has already passed, otherwise it could hang forever
      if (curr_tx_ts > p_tx_block->timestamp) {
        SKIQ_RF_ERROR("[Tx %d:%d block] Tx block (ts=%ld) is in the past (last_tx_ts=%ld, curr_tx_ts=%ld), ignoring\n",
                      q->card,
                      (int)q->hdl,
                      q->p_tx_block->timestamp,
                      last_tx_ts,
                      curr_tx_ts);
        continue;
      }
    }
    last_tx_ts = p_tx_block->timestamp + q->block_size;

    // If the ring-buffer did not return with error code...
    if (n > SRSRAN_SUCCESS) {
      SKIQ_RF_DEBUG("[Tx %d:%d block] ts=%ld; nsamples=%d; last_tx_ts=%ld;\n",
                    q->card,
                    (int)q->hdl,
                    p_tx_block->timestamp,
                    q->block_size,
                    last_tx_ts);

      if (skiq_transmit(q->card, q->hdl, p_tx_block, NULL) < 0) {
        ERROR("Error transmitting card %d\n", q->card);
        q->state = RF_SKIQ_PORT_STATE_STOP;
      }

#if SKIQ_TX_LATES_CHECK_PERIOD
      if (q->last_check_ts + SKIQ_TX_LATES_CHECK_PERIOD < p_tx_block->timestamp) {
        // Handle late timestamps events
        rf_skiq_tx_port_handle_late(q);

        // Update last check TS
        q->last_check_ts = p_tx_block->timestamp;
      }
#endif // SKIQ_TX_LATES_CHECK_PERIOD
    }
  }

  if (skiq_stop_tx_streaming(q->card, q->hdl)) {
    ERROR("Error stopping Tx stream %d:%d\n", q->card, (int)q->hdl);
  }

  SKIQ_RF_INFO("Exiting writer thread!\n");

  return NULL;
}

int rf_skiq_tx_port_init(rf_skiq_tx_port_t* q, uint8_t card, skiq_tx_hdl_t hdl, const rf_skiq_port_opts_t* opts)
{
  // Defines the block size in multiples of 256 words
  uint32_t nof_blocks_per_packet = 4;
  switch (opts->stream_mode) {
    case skiq_rx_stream_mode_high_tput:
      nof_blocks_per_packet = 8;
      break;
    case skiq_rx_stream_mode_low_latency:
      nof_blocks_per_packet = 2;
      break;
    case skiq_rx_stream_mode_balanced:
    default:
      // Keep default value
      break;
  }

  q->card           = card;
  q->hdl            = hdl;
  q->block_size     = SKIQ_TX_PACKET_SIZE(nof_blocks_per_packet, opts->chan_mode);
  q->p_block_nbytes = q->block_size * 4 + SKIQ_TX_HEADER_SIZE_IN_BYTES;

  // configure the data flow mode to use timestamps
  if (skiq_write_tx_data_flow_mode(card, hdl, skiq_tx_with_timestamps_data_flow_mode) != 0) {
    ERROR("Setting Tx data flow mode");
    return SRSRAN_ERROR;
  }

  // configure the transfer mode to synchronous
  if (skiq_write_tx_transfer_mode(card, hdl, skiq_tx_transfer_mode_sync) != 0) {
    ERROR("setting tx transfer mode");
    return SRSRAN_ERROR;
  }

  // configure Tx block size
  if (skiq_write_tx_block_size(card, hdl, q->block_size) != 0) {
    ERROR("configuring Tx block size");
    return SRSRAN_ERROR;
  }

  q->p_tx_block = skiq_tx_block_allocate(q->block_size);
  if (q->p_tx_block == NULL) {
    ERROR("Allocating Tx block");
    return SRSRAN_ERROR;
  }

  // initialise ring buffer
  if (srsran_ringbuffer_init(&q->rb, (int)(opts->tx_rb_size * q->p_block_nbytes))) {
    ERROR("Initialising ringbuffer");
    return SRSRAN_ERROR;
  }

  // Initialise thread parameters
  pthread_attr_t     attr;
  struct sched_param param;

  param.sched_priority = sched_get_priority_max(SCHED_FIFO);
  pthread_attr_init(&attr);
  if (pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED)) {
    ERROR("Error not enough privileges to set Scheduling priority\n");
  }

  if (pthread_attr_setschedpolicy(&attr, SCHED_FIFO)) {
    ERROR("Error not enough privileges to set Scheduling priority\n");
  }

  if (pthread_attr_setschedparam(&attr, &param)) {
    ERROR("Error not enough privileges to set Scheduling priority\n");
  }

  // Launch thread
  if (pthread_create(&q->thread, &attr, writer_thread, q)) {
    ERROR("Error creating writer thread with attributes (Did you miss sudo?). Trying without attributes.\n");

    // try to create thread without attributes
    pthread_attr_destroy(&attr);
    if (pthread_create(&q->thread, NULL, writer_thread, q)) {
      ERROR("Error creating writer thread, even without thread attributes. Exiting.\n");
      return SRSRAN_ERROR;
    }
  }

  // Rename thread
  char thread_name[32] = {};
  if (snprintf(thread_name, sizeof(thread_name), "SKIQ Tx %d:%d", q->card, (int)q->hdl) > 0) {
    pthread_setname_np(q->thread, thread_name);
  }

  return SRSRAN_SUCCESS;
}

void rf_skiq_tx_port_free(rf_skiq_tx_port_t* q)
{
  // Stop thread
  q->state = RF_SKIQ_PORT_STATE_STOP;

  // Unlock ringbuffer
  srsran_ringbuffer_write(&q->rb, (void*)q->p_tx_block, q->p_block_nbytes);

  // Wait thread to return
  pthread_join(q->thread, NULL);

  if (q->p_tx_block) {
    skiq_tx_block_free(q->p_tx_block);
  }
  srsran_ringbuffer_free(&q->rb);
}

void rf_skiq_tx_port_end_of_burst(rf_skiq_tx_port_t* q)
{
  pthread_mutex_lock(&q->mutex);

  // Fill pending block if any, otherwise push a block with zeros
  if (q->next_offset > 0) {
    // Calculate pending samples to fill the block
    uint32_t pending = q->block_size - q->next_offset;

    // Zero pending samples in the block
    srsran_vec_i16_zero(&q->p_tx_block->data[q->next_offset * 2], 2 * pending);

    // Write block into the ring-buffer
    srsran_ringbuffer_write_block(&q->rb, q->p_tx_block, q->p_block_nbytes);

    SKIQ_RF_DEBUG("[Tx EOB %d:%d] Padding offset=%d; n=%d; ts=%ld\n",
                  q->card,
                  (int)q->hdl,
                  q->next_offset,
                  pending,
                  q->p_tx_block->timestamp);

    // Reset next offset, so next transmission uses a new block
    q->next_offset = 0;
  }

  pthread_mutex_unlock(&q->mutex);
}

int rf_skiq_tx_port_send(rf_skiq_tx_port_t* q, const cf_t* buffer, uint32_t nsamples, uint64_t ts)
{
  // Ignore transmission if the stream is not enabled
  if (q->state != RF_SKIQ_PORT_STATE_STREAMING) {
    return nsamples;
  }

  // If no data and no block has started, early return
  if (buffer == NULL && q->next_offset == 0) {
    return nsamples;
  }

  pthread_mutex_lock(&q->mutex);

  // Calculate destination where IQ shall be stored
  int16_t* p_tx_iq = &q->p_tx_block->data[q->next_offset * 2];

  // Calculate number of samples to take from buffer
  nsamples = SRSRAN_MIN(nsamples, q->block_size - q->next_offset);

  //  Set time stamp only if no offset
  if (q->next_offset == 0) {
    skiq_tx_set_block_timestamp(q->p_tx_block, ts);
  }

  SKIQ_RF_DEBUG(
      "[Tx %d:%d] Write offset=%d; nsamples=%d; ts=%ld\n", q->card, (int)q->hdl, q->next_offset, nsamples, ts);

  // Fill data ...
  if (buffer == NULL) {
    // ... with zeros
    srsran_vec_i16_zero(p_tx_iq, 2 * nsamples);
  } else {
    // ... with samples, after conversion
    srsran_vec_convert_conj_cs(buffer, SKIQ_NORM, p_tx_iq, nsamples);
  }
  q->next_offset += nsamples;

  // If the number of samples does not fill the block, return early
  if (q->next_offset < q->block_size) {
    pthread_mutex_unlock(&q->mutex);
    return nsamples;
  }

  if (srsran_ringbuffer_space(&q->rb) < q->p_block_nbytes * 2) {
    ERROR("Tx buffer overflow\n");
    pthread_mutex_unlock(&q->mutex);
    return nsamples;
  }

  // Actual write in ring buffer
  int n = srsran_ringbuffer_write_timed_block(&q->rb, q->p_tx_block, q->p_block_nbytes, 5);

  pthread_mutex_unlock(&q->mutex);

  // In case of error (e.g. timeout) return code
  if (n < SRSRAN_SUCCESS) {
    return n;
  }

  // In case of number of bytes mismatch return error
  if (n != q->p_block_nbytes) {
    ERROR("Error writing in Tx buffer %d:%d\n", q->card, (int)q->hdl);
    return SRSRAN_ERROR;
  }

  // Reset offset only if the block was successfully written into the ring-buffer
  q->next_offset = 0;

  // Return the number of samples writen in the buffer
  return nsamples;
}

void rf_skiq_tx_port_set_error_handler(rf_skiq_tx_port_t* q, srsran_rf_error_handler_t error_handler, void* arg)
{
  q->error_handler     = error_handler;
  q->error_handler_arg = arg;
}

int rf_skiq_tx_port_set_lo(rf_skiq_tx_port_t* q, uint64_t lo_freq)
{
  // Skip setting LO frequency if it is not required
  if (q->current_lo == lo_freq) {
    return SRSRAN_SUCCESS;
  }

  skiq_filt_t filt = (lo_freq < 3000000000UL) ? skiq_filt_0_to_3000_MHz : skiq_filt_3000_to_6000_MHz;

  if (skiq_write_tx_LO_freq(q->card, q->hdl, lo_freq)) {
    ERROR("Setting card %d:%d Tx Lo frequency", q->card, (int)q->hdl);
    return SRSRAN_ERROR;
  }

  if (skiq_write_tx_filter_path(q->card, q->hdl, filt)) {
    ERROR("Setting card %d:%d Tx filter", q->card, q->hdl);
    return SRSRAN_ERROR;
  }

  q->current_lo = lo_freq;

  return SRSRAN_SUCCESS;
}

int rf_skiq_rx_port_init(rf_skiq_rx_port_t* q, uint8_t card, skiq_rx_hdl_t hdl, const rf_skiq_port_opts_t* opts)
{
  q->card = card;
  q->hdl  = hdl;

  // enabling DC offset correction can cause an IQ impairment
  if (skiq_write_rx_dc_offset_corr(card, hdl, false)) {
    ERROR("Setting RX DC offset correction");
    return SRSRAN_ERROR;
  }

  // set rx gain mode
  if (skiq_write_rx_gain_mode(card, hdl, skiq_rx_gain_manual)) {
    ERROR("Setting RX gain mode");
    return SRSRAN_ERROR;
  }

  // Rx block size in bytes
  int32_t rx_block_size = skiq_read_rx_block_size(q->card, opts->stream_mode) - SKIQ_RX_HEADER_SIZE_IN_BYTES;

  // initialise ring buffer
  if (srsran_ringbuffer_init(&q->rb, (int)(opts->rx_rb_size * rx_block_size))) {
    ERROR("Initialising ringbuffer");
    return SRSRAN_ERROR;
  }

  return SRSRAN_SUCCESS;
}

void rf_skiq_rx_port_free(rf_skiq_rx_port_t* q)
{

  srsran_ringbuffer_free(&q->rb);
}

int rf_skiq_rx_port_available(rf_skiq_rx_port_t* q)
{
  return srsran_ringbuffer_status(&q->rb);
}

int rf_skiq_rx_port_read(rf_skiq_rx_port_t* q, cf_t* dest, uint32_t nsamples, uint64_t* ts_start)
{
  // Detect start of new block
  if (q->rb_read_rem == 0) {
    skiq_header_t header = {};

    // If ring-buffer overflow was detected...
    if (q->rb_overflow) {
      // Reset ring buffer
      srsran_ringbuffer_reset(&q->rb);

      // Clear overflow flag
      q->rb_overflow = false;

      // Set samples to zero
      srsran_vec_cf_zero(dest, nsamples);

      // Set default timestamp
      *ts_start = 0;

      // Since the buffer is empty, return the full amount of samples so it does not delay reception of other channels
      return nsamples;
    }

    // Read a packet. First the header
    if (srsran_ringbuffer_read(&q->rb, &header, sizeof(skiq_header_t)) != sizeof(skiq_header_t)) {
      ERROR("Error reading header from ring-buffer  %d:%d corrupted\n", q->card, (int)q->hdl);
      return SRSRAN_ERROR;
    }

    // Check header magic word
    if (header.magic != SKIQ_RX_BUFFFER_MAGIC_WORD) {
      ERROR("Error ring-buffer %d:%d corrupted\n", q->card, (int)q->hdl);
      return SRSRAN_ERROR;
    }

    // Successful read
    q->rb_read_rem   = header.nsamples;
    q->rb_tstamp_rem = header.tstamp;
  }

  // Limit number of samples to the remainder of the stored packet
  nsamples = SRSRAN_MIN(q->rb_read_rem, nsamples);

  // Read any remainder of a packet from the ring buffer
  int n = srsran_ringbuffer_read_convert_conj(&q->rb, dest, SKIQ_NORM, nsamples);

  // Detect error in read
  if (n < SRSRAN_SUCCESS) {
    ERROR("Error reading packet remainder from %d:%d\n", q->card, (int)q->hdl);
    return SRSRAN_ERROR;
  }

  SKIQ_RF_DEBUG("[Rx %d:%d] Read nsamples=%d/%d; ts=%ld\n", q->card, (int)q->hdl, n, nsamples, q->rb_tstamp_rem);

  // Update timestamp
  *ts_start = q->rb_tstamp_rem;

  // Update reminder
  q->rb_read_rem -= n;
  q->rb_tstamp_rem += n;

  // Return number of read samples
  return n;
}

uint64_t rf_skiq_rx_port_get_timestamp(rf_skiq_rx_port_t* q)
{
  return q->rb_tstamp_rem;
}

int rf_skiq_rx_port_write(rf_skiq_rx_port_t* q, const skiq_rx_block_t* p_rx_block, uint32_t nsamples)
{
  // Prepare header
  skiq_header_t header = {};
  header.magic         = SKIQ_RX_BUFFFER_MAGIC_WORD;
  header.tstamp        = p_rx_block->rf_timestamp;
  header.nsamples      = nsamples;

  // Ignore block if the overflow flag has risen
  if (q->rb_overflow) {
    return nsamples;
  }

  SKIQ_RF_DEBUG("[Rx %d:%d block] ts=%ld; nsamples=%d;\n", q->card, (int)q->hdl, header.tstamp, header.nsamples);

  // Check space in the ring-buffer prior to writing
  if (srsran_ringbuffer_space(&q->rb) >= sizeof(skiq_header_t) + nsamples * 4) {
    // Write header
    if (srsran_ringbuffer_write_block(&q->rb, &header, sizeof(skiq_header_t)) != sizeof(skiq_header_t)) {
      ERROR("Writing header in Rx buffer %d:%d!\n", q->card, (int)q->hdl);
      return SRSRAN_ERROR;
    }

    // Write IQ samples
    if (srsran_ringbuffer_write_block(&q->rb, (uint8_t*)p_rx_block->data, (int)nsamples * 4) != nsamples * 4) {
      ERROR("Writing base-band in Rx buffer %d:%d!\n", q->card, (int)q->hdl);
      return SRSRAN_ERROR;
    }
  } else {
    SKIQ_RF_INFO("Rx %d:%d ring-buffer overflow!\n", q->card, (int)q->hdl);
    q->rb_overflow = true;
    rf_skiq_rx_port_handle_overflow(q);
  }

  // Process overload, call handle only for rising-edges
  if (!q->rf_overflow && p_rx_block->overload) {
    rf_skiq_rx_port_handle_overflow(q);
  }
  q->rf_overflow = p_rx_block->overload;

  return nsamples;
}

void rf_skiq_rx_port_set_error_handler(rf_skiq_rx_port_t* q, srsran_rf_error_handler_t error_handler, void* arg)
{
  q->error_handler     = error_handler;
  q->error_handler_arg = arg;
}

void rf_skiq_rx_port_reset(rf_skiq_rx_port_t* q)
{
  SKIQ_RF_INFO("Rx port %d:%d reset\n", q->card, (int)q->hdl);
  q->rb_read_rem   = 0;
  q->rb_tstamp_rem = 0;
  srsran_ringbuffer_reset(&q->rb);
}

int rf_skiq_rx_port_set_lo(rf_skiq_rx_port_t* q, uint64_t lo_freq)
{
  // Skip setting LO frequency if it is not required
  if (q->current_lo == lo_freq) {
    return SRSRAN_SUCCESS;
  }

  skiq_filt_t filt = (lo_freq < 3000000000UL) ? skiq_filt_0_to_3000_MHz : skiq_filt_3000_to_6000_MHz;

  if (skiq_write_rx_LO_freq(q->card, q->hdl, lo_freq)) {
    ERROR("Setting card %d:%d Rx Lo frequency", q->card, (int)q->hdl);
    return SRSRAN_ERROR;
  }

  if (skiq_write_rx_preselect_filter_path(q->card, q->hdl, filt)) {
    ERROR("Setting card %d:%d Rx filter", q->card, q->hdl);
    return SRSRAN_ERROR;
  }

  q->current_lo = lo_freq;

  return SRSRAN_SUCCESS;
}
