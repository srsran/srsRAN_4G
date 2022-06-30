/**
 * Copyright 2013-2022 Software Radio Systems Limited
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

#include "rf_zmq_imp.h"
#include "srsran/common/tsan_options.h"
#include "srsran/phy/common/timestamp.h"
#include "srsran/phy/utils/debug.h"
#include <complex.h>
#include <pthread.h>
#include <srsran/phy/common/phy_common.h>
#include <srsran/phy/utils/vector.h>
#include <stdlib.h>
#include <zmq.h>

#define PRINT_SAMPLES 1
#define COMPARE_BITS 0
#define COMPARE_EPSILON (1e-6f)
#define NOF_RX_ANT 4
#define NUM_SF (500)
#define SF_LEN (1920)
#define RF_BUFFER_SIZE (SF_LEN * NUM_SF)
#define TX_OFFSET_MS (4)

static cf_t ue_rx_buffer[NOF_RX_ANT][RF_BUFFER_SIZE];
static cf_t enb_tx_buffer[NOF_RX_ANT][RF_BUFFER_SIZE];
static cf_t enb_rx_buffer[NOF_RX_ANT][RF_BUFFER_SIZE];

static srsran_rf_t ue_radio, enb_radio;
pthread_t          rx_thread;

void* ue_rx_thread_function(void* args)
{
  char rf_args[RF_PARAM_LEN];
  strncpy(rf_args, (char*)args, RF_PARAM_LEN - 1);
  rf_args[RF_PARAM_LEN - 1] = 0;

  // sleep(1);

  printf("opening rx device with args=%s\n", rf_args);
  if (srsran_rf_open_devname(&ue_radio, "zmq", rf_args, NOF_RX_ANT)) {
    fprintf(stderr, "Error opening rf\n");
    exit(-1);
  }

  // receive 5 subframes at once (i.e. mimic initial rx that receives one slot)
  uint32_t num_slots          = NUM_SF / 5;
  uint32_t num_samps_per_slot = SF_LEN * 5;
  uint32_t num_rxed_samps     = 0;
  for (uint32_t i = 0; i < num_slots; ++i) {
    void* data_ptr[SRSRAN_MAX_PORTS] = {NULL};
    for (uint32_t c = 0; c < NOF_RX_ANT; c++) {
      data_ptr[c] = &ue_rx_buffer[c][i * num_samps_per_slot];
    }
    num_rxed_samps += srsran_rf_recv_with_time_multi(&ue_radio, data_ptr, num_samps_per_slot, true, NULL, NULL);
  }

  printf("received %d samples.\n", num_rxed_samps);

  printf("closing ue zmq device\n");
  srsran_rf_close(&ue_radio);

  return NULL;
}

void enb_tx_function(const char* tx_args, bool timed_tx)
{
  char rf_args[RF_PARAM_LEN];
  strncpy(rf_args, tx_args, RF_PARAM_LEN - 1);
  rf_args[RF_PARAM_LEN - 1] = 0;

  printf("opening tx device with args=%s\n", rf_args);
  if (srsran_rf_open_devname(&enb_radio, "zmq", rf_args, NOF_RX_ANT)) {
    fprintf(stderr, "Error opening rf\n");
    exit(-1);
  }

  // generate random tx data
  for (int c = 0; c < NOF_RX_ANT; c++) {
    for (int i = 0; i < RF_BUFFER_SIZE; i++) {
      enb_tx_buffer[c][i] = ((float)rand() / (float)RAND_MAX) + _Complex_I * ((float)rand() / (float)RAND_MAX);
    }
  }

  // send data subframe per subframe
  uint32_t num_txed_samples = 0;

  // initial transmission without ts
  void* data_ptr[SRSRAN_MAX_PORTS] = {NULL};
  cf_t  tx_buffer[NOF_RX_ANT][SF_LEN];
  for (int c = 0; c < NOF_RX_ANT; c++) {
    memcpy(&tx_buffer[c], &enb_tx_buffer[c][num_txed_samples], SF_LEN * sizeof(cf_t));
    data_ptr[c] = &tx_buffer[c][0];
  }
  int ret = srsran_rf_send_multi(&enb_radio, (void**)data_ptr, SF_LEN, true, true, false);
  num_txed_samples += SF_LEN;

  // from here on, all transmissions are timed relative to the last rx time
  srsran_timestamp_t rx_time, tx_time;

  for (uint32_t i = 0; i < NUM_SF - ((timed_tx) ? TX_OFFSET_MS : 1); ++i) {
    // first recv samples
    for (int c = 0; c < NOF_RX_ANT; c++) {
      data_ptr[c] = enb_rx_buffer[c];
    }
    srsran_rf_recv_with_time_multi(&enb_radio, data_ptr, SF_LEN, true, &rx_time.full_secs, &rx_time.frac_secs);

    // prepare data buffer
    for (int c = 0; c < NOF_RX_ANT; c++) {
      memcpy(&tx_buffer[c], &enb_tx_buffer[c][num_txed_samples], SF_LEN * sizeof(cf_t));
      data_ptr[c] = &tx_buffer[c][0];
    }

    if (timed_tx) {
      // timed tx relative to receive time (this will cause a cap in the rx'ed samples at the UE resulting in 3 zero
      // subframes)
      srsran_timestamp_copy(&tx_time, &rx_time);
      srsran_timestamp_add(&tx_time, 0, TX_OFFSET_MS * 1e-3);
      ret = srsran_rf_send_timed_multi(
          &enb_radio, (void**)data_ptr, SF_LEN, tx_time.full_secs, tx_time.frac_secs, true, true, false);
    } else {
      // normal tx
      ret = srsran_rf_send_multi(&enb_radio, (void**)data_ptr, SF_LEN, true, true, false);
    }
    if (ret != SRSRAN_SUCCESS) {
      fprintf(stderr, "Error sending data\n");
      exit(-1);
    }

    num_txed_samples += SF_LEN;
  }

  printf("transmitted %d samples in %d subframes\n", num_txed_samples, NUM_SF);

  printf("closing tx device\n");
  srsran_rf_close(&enb_radio);
}

int run_test(const char* rx_args, const char* tx_args, bool timed_tx)
{
  int ret = SRSRAN_ERROR;

  // make sure we can receive in slots
  if (NUM_SF % 5 != 0) {
    fprintf(stderr, "number of subframes must be multiple of 5\n");
    goto exit;
  }

  // start Rx thread
  if (pthread_create(&rx_thread, NULL, ue_rx_thread_function, (void*)rx_args)) {
    perror("pthread_create");
    exit(-1);
  }

  enb_tx_function(tx_args, timed_tx);

  // wait for rx thread
  pthread_join(rx_thread, NULL);

  // channel-wise comparison
  for (int c = 0; c < NOF_RX_ANT; c++) {
    // subframe-wise compare tx'ed and rx'ed data (stop 3 subframes earlier for timed tx)
    for (uint32_t i = 0; i < NUM_SF - (timed_tx ? 3 : 0); ++i) {
      uint32_t sf_offet = 0;
      if (timed_tx && i >= 1) {
        // for timed transmission, the enb inserts 3 zero subframes after the first untimed tx
        sf_offet = (TX_OFFSET_MS - 1) * SF_LEN;
      }

#if PRINT_SAMPLES
      // print first 10 samples for each SF
      printf("enb_tx_buffer sf%d:\n", i);
      srsran_vec_fprint_c(stdout, &enb_tx_buffer[c][i * SF_LEN], 10);
      printf("ue_rx_buffer sf%d:\n", i);
      srsran_vec_fprint_c(stdout, &ue_rx_buffer[c][sf_offet + i * SF_LEN], 10);
#endif

#if COMPARE_BITS
      int d = memcmp(&ue_rx_buffer[sf_offet + i * SF_LEN], &enb_tx_buffer[i * SF_LEN], SF_LEN);
      if (d) {
        d = d > 0 ? d : -d;
        fprintf(stderr, "data mismatch in subframe %d, sample %d\n", i, d);
        printf("enb_tx_buffer sf%d:\n", i);
        srsran_vec_fprint_c(stdout, &enb_tx_buffer[i * SF_LEN + d], 10);
        printf("ue_rx_buffer sf%d:\n", i);
        srsran_vec_fprint_c(stdout, &ue_rx_buffer[sf_offet + i * SF_LEN + d], 10);
        goto exit;
      }
#else
      srsran_vec_sub_ccc(&ue_rx_buffer[c][sf_offet + i * SF_LEN],
                         &enb_tx_buffer[c][i * SF_LEN],
                         &ue_rx_buffer[c][sf_offet + i * SF_LEN],
                         SF_LEN);
      uint32_t max_ix = srsran_vec_max_abs_ci(&ue_rx_buffer[c][sf_offet + i * SF_LEN], SF_LEN);
      if (cabsf(ue_rx_buffer[c][sf_offet + i * SF_LEN + max_ix]) > COMPARE_EPSILON) {
        fprintf(stderr, "data mismatch in subframe %d\n", i);
        goto exit;
      }
#endif
    }
  }

  ret = SRSRAN_SUCCESS;

exit:
  return ret;
}

int param_test(const char* args_param, const int num_channels)
{
  char rf_args[RF_PARAM_LEN] = {};
  strncpy(rf_args, (char*)args_param, RF_PARAM_LEN - 1);
  rf_args[RF_PARAM_LEN - 1] = 0;

  printf("opening tx device with args=%s\n", rf_args);
  if (srsran_rf_open_devname(&enb_radio, "zmq", rf_args, num_channels)) {
    fprintf(stderr, "Error opening rf\n");
    return SRSRAN_ERROR;
  }

  srsran_rf_close(&enb_radio);

  return SRSRAN_SUCCESS;
}

int main()
{
  //  // two Rx ports
  //  if (param_test("rx_port=ipc://dl0,rx_port1=ipc://dl1", 2)) {
  //    fprintf(stderr, "Param test failed!\n");
  //    return SRSRAN_ERROR;
  //  }

  //  // multiple rx ports, no channel index provided
  //  if (param_test("rx_port=ipc://dl0,rx_port=ipc://dl1,rx_port=ipc://dl2,rx_port=ipc://dl3,base_srate=1.92e6", 4)) {
  //    fprintf(stderr, "Param test failed!\n");
  //    return SRSRAN_ERROR;
  //  }

  //  // One Rx, one Tx and all generic options
  //  if (param_test("rx_port0=tcp://"
  //                 "localhost:2000,rx_format=sc16,tx_format=sc16,tx_type=pub,rx_type=sub,base_srate=1.92e6,id=test",
  //                 1)) {
  //    fprintf(stderr, "Param test failed!\n");
  //    return SRSRAN_ERROR;
  //  }

  //  // 1 port, 2 antennas, MIMO freq config
  //  if (param_test(
  //          "tx_port0=tcp://*:2001,tx_port1=tcp://*:2003,rx_port0=tcp://localhost:2000,rx_port1=tcp://"
  //          "localhost:2002,id=ue,base_srate=23.04e6,tx_freq0=2510e6,tx_freq1=2510e6,rx_freq0=2630e6,,rx_freq1=2630e6",
  //          2)) {
  //    fprintf(stderr, "Param test failed!\n");
  //    return SRSRAN_ERROR;
  //  }

#if NOF_RX_ANT == 1
  // single tx, single rx with continuous transmissions (no timed tx) using IPC transport
  if (run_test("rx_port=ipc://link1,id=ue,base_srate=1.92e6", "tx_port=ipc://link1,id=enb,base_srate=1.92e6", false) !=
      SRSRAN_SUCCESS) {
    fprintf(stderr, "Single tx, single rx test failed!\n");
    return -1;
  }
#endif

  // up to 4 trx radios with continous tx (no decimation, no timed tx)
  if (run_test("tx_port=tcp://*:5554,tx_port=tcp://*:5556,tx_port=tcp://*:5558,tx_port=tcp://*:5560,rx_port=tcp://"
               "localhost:5555,rx_port=tcp://localhost:5557,rx_port=tcp://localhost:5559,rx_port=tcp://"
               "localhost:5561,id=ue,base_srate=1.92e6,log_trx_timeout=true,trx_timeout_ms=1000",
               "rx_port=tcp://localhost:5554,rx_port=tcp://localhost:5556,rx_port=tcp://localhost:5558,rx_port=tcp://"
               "localhost:5560,tx_port=tcp://*:5555,tx_port=tcp://*:5557,tx_port=tcp://*:5559,tx_port=tcp://"
               "*:5561,id=enb,base_srate=1.92e6",
               false) != SRSRAN_SUCCESS) {
    fprintf(stderr, "Multi TRx radio test failed!\n");
    return -1;
  }

  // up to 4 trx radios with continous tx (timed tx) using TCP for UL (UE tx) and IPC for eNB DL (eNB tx)
  if (run_test("tx_port=tcp://*:5554,tx_port=tcp://*:5556,tx_port=tcp://*:5558,tx_port=tcp://*:5560,rx_port=ipc://"
               "dl0,rx_port=ipc://dl1,rx_port=ipc://dl2,rx_port=ipc://dl3,id=ue,base_srate=1.92e6",
               "rx_port=tcp://localhost:5554,rx_port=tcp://localhost:5556,rx_port=tcp://localhost:5558,rx_port=tcp://"
               "localhost:5560,tx_port=ipc://dl0,tx_port=ipc://dl1,tx_port=ipc://dl2,tx_port=ipc://"
               "dl3,id=enb,base_srate=1.92e6",
               true) != SRSRAN_SUCCESS) {
    fprintf(stderr, "Multi TRx radio test with timed tx failed!\n");
    return -1;
  }

  // up to 4 trx radios with continous tx (timed tx) using TCP for UL (UE tx) and IPC for eNB DL (eNB tx)
  // with decimation 23.04e6 <-> 1.92e6
  if (run_test("tx_port=tcp://*:5554,tx_port=tcp://*:5556,tx_port=tcp://*:5558,tx_port=tcp://*:5560,rx_port=ipc://"
               "dl0,rx_port=ipc://dl1,rx_port=ipc://dl2,rx_port=ipc://dl3,id=ue,base_srate=23.04e6",
               "rx_port=tcp://localhost:5554,rx_port=tcp://localhost:5556,rx_port=tcp://localhost:5558,rx_port=tcp://"
               "localhost:5560,tx_port=ipc://dl0,tx_port=ipc://dl1,tx_port=ipc://dl2,tx_port=ipc://"
               "dl3,id=enb,base_srate=23.04e6",
               true) != SRSRAN_SUCCESS) {
    fprintf(stderr, "Multi TRx radio test with timed tx and decimation failed!\n");
    return -1;
  }

  return SRSRAN_SUCCESS;
}
