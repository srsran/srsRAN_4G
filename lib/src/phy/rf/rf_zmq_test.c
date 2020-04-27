/*
 * Copyright 2013-2020 Software Radio Systems Limited
 *
 * This file is part of srsLTE.
 *
 * srsLTE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsLTE is distributed in the hope that it will be useful,
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
#include "srslte/srslte.h"
#include <pthread.h>
#include <srslte/phy/common/phy_common.h>
#include <stdlib.h>
#include <zmq.h>

#define NOF_RX_ANT 1
#define NUM_SF (500)
#define SF_LEN (1920)
#define RF_BUFFER_SIZE (SF_LEN * NUM_SF)
#define TX_OFFSET_MS (4)

static cf_t ue_rx_buffer[RF_BUFFER_SIZE];
static cf_t enb_tx_buffer[RF_BUFFER_SIZE];
static cf_t enb_rx_buffer[RF_BUFFER_SIZE];

static srslte_rf_t ue_radio, enb_radio;
pthread_t          rx_thread;

void* ue_rx_thread_function(void* args)
{
  char rf_args[RF_PARAM_LEN];
  strncpy(rf_args, (char*)args, RF_PARAM_LEN - 1);
  rf_args[RF_PARAM_LEN - 1] = 0;

  // sleep(1);

  printf("opening rx device with args=%s\n", rf_args);
  if (srslte_rf_open_devname(&ue_radio, "zmq", rf_args, NOF_RX_ANT)) {
    fprintf(stderr, "Error opening rf\n");
    exit(-1);
  }

  // receive 5 subframes at once (i.e. mimic initial rx that receives one slot)
  uint32_t num_slots          = NUM_SF / 5;
  uint32_t num_samps_per_slot = SF_LEN * 5;
  uint32_t num_rxed_samps     = 0;
  for (uint32_t i = 0; i < num_slots; ++i) {
    void* data_ptr[SRSLTE_MAX_PORTS] = {NULL};
    data_ptr[0]                      = &ue_rx_buffer[i * num_samps_per_slot];
    num_rxed_samps += srslte_rf_recv_with_time_multi(&ue_radio, data_ptr, num_samps_per_slot, true, NULL, NULL);
  }

  printf("received %d samples.\n", num_rxed_samps);

  printf("closing ue norf device\n");
  srslte_rf_close(&ue_radio);

  return NULL;
}

void enb_tx_function(const char* tx_args, bool timed_tx)
{
  char rf_args[RF_PARAM_LEN];
  strncpy(rf_args, tx_args, RF_PARAM_LEN - 1);
  rf_args[RF_PARAM_LEN - 1] = 0;

  printf("opening tx device with args=%s\n", rf_args);
  if (srslte_rf_open_devname(&enb_radio, "zmq", rf_args, NOF_RX_ANT)) {
    fprintf(stderr, "Error opening rf\n");
    exit(-1);
  }

  // generate random tx data
  for (int i = 0; i < RF_BUFFER_SIZE; i++) {
    enb_tx_buffer[i] = ((float)rand() / (float)RAND_MAX) + _Complex_I * ((float)rand() / (float)RAND_MAX);
  }

  // send data subframe per subframe
  uint32_t num_txed_samples = 0;

  // initial transmission without ts
  void* data_ptr[SRSLTE_MAX_PORTS] = {NULL};
  data_ptr[0]                      = &enb_tx_buffer[num_txed_samples];
  int ret                          = srslte_rf_send_multi(&enb_radio, (void**)data_ptr, SF_LEN, true, true, false);
  num_txed_samples += SF_LEN;

  // from here on, all transmissions are timed relative to the last rx time
  srslte_timestamp_t rx_time, tx_time;

  for (uint32_t i = 0; i < NUM_SF - ((timed_tx) ? TX_OFFSET_MS : 1); ++i) {
    // first recv samples
    data_ptr[0] = enb_rx_buffer;
    srslte_rf_recv_with_time_multi(&enb_radio, data_ptr, SF_LEN, true, &rx_time.full_secs, &rx_time.frac_secs);

    // prepare data buffer
    data_ptr[0] = &enb_tx_buffer[num_txed_samples];

    if (timed_tx) {
      // timed tx relative to receive time (this will cause a cap in the rx'ed samples at the UE resulting in 3 zero
      // subframes)
      srslte_timestamp_copy(&tx_time, &rx_time);
      srslte_timestamp_add(&tx_time, 0, TX_OFFSET_MS * 1e-3);
      ret = srslte_rf_send_timed_multi(
          &enb_radio, (void**)data_ptr, SF_LEN, tx_time.full_secs, tx_time.frac_secs, true, true, false);
    } else {
      // normal tx
      ret = srslte_rf_send_multi(&enb_radio, (void**)data_ptr, SF_LEN, true, true, false);
    }
    if (ret != SRSLTE_SUCCESS) {
      fprintf(stderr, "Error sending data\n");
      exit(-1);
    }

    num_txed_samples += SF_LEN;
  }

  printf("transmitted %d samples in %d subframes\n", num_txed_samples, NUM_SF);

  printf("closing tx device\n");
  srslte_rf_close(&enb_radio);
}

int run_test(const char* rx_args, const char* tx_args, bool timed_tx)
{
  int ret = SRSLTE_ERROR;

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

  // subframe-wise compare tx'ed and rx'ed data (stop 3 subframes earlier for timed tx)
  for (uint32_t i = 0; i < NUM_SF - (timed_tx ? 3 : 0); ++i) {
    uint32_t sf_offet = 0;
    if (timed_tx && i >= 1) {
      // for timed transmission, the enb inserts 3 zero subframes after the first untimed tx
      sf_offet = (TX_OFFSET_MS - 1) * SF_LEN;
    }

#if 0
    // print first 3 samples for each SF
    printf("enb_tx_buffer sf%d:\n", i);
    srslte_vec_fprint_c(stdout, &enb_tx_buffer[i * SF_LEN], 3);
    printf("ue_rx_buffer sf%d:\n", i);
    srslte_vec_fprint_c(stdout, &ue_rx_buffer[sf_offet + i * SF_LEN], 3);
#endif

    if (memcmp(&ue_rx_buffer[sf_offet + i * SF_LEN], &enb_tx_buffer[i * SF_LEN], SF_LEN) != 0) {
      fprintf(stderr, "data mismatch in subframe %d\n", i);
      goto exit;
    }
  }

  ret = SRSLTE_SUCCESS;

exit:
  return ret;
}

int param_test(const char* args_param, const int num_channels)
{
  char rf_args[RF_PARAM_LEN] = {};
  strncpy(rf_args, (char*)args_param, RF_PARAM_LEN - 1);
  rf_args[RF_PARAM_LEN - 1] = 0;

  printf("opening tx device with args=%s\n", rf_args);
  if (srslte_rf_open_devname(&enb_radio, "zmq", rf_args, num_channels)) {
    fprintf(stderr, "Error opening rf\n");
    return SRSLTE_ERROR;
  }

  srslte_rf_close(&enb_radio);

  return SRSLTE_SUCCESS;
}

int main()
{
  // two Rx ports
  if (param_test("rx_port=ipc://dl0,rx_port1=ipc://dl1", 2)) {
    fprintf(stderr, "Param test failed!\n");
    return SRSLTE_ERROR;
  }

  // multiple rx ports, no channel index provided
  if (param_test("rx_port=ipc://dl0,rx_port=ipc://dl1,rx_port=ipc://dl2,rx_port=ipc://dl3,base_srate=1.92e6", 4)) {
    fprintf(stderr, "Param test failed!\n");
    return SRSLTE_ERROR;
  }

  // One Rx, one Tx and all generic options
  if (param_test("rx_port0=tcp://"
                 "localhost:2000,rx_format=sc16,tx_format=sc16,tx_type=pub,rx_type=sub,base_srate=1.92e6,id=test",
                 1)) {
    fprintf(stderr, "Param test failed!\n");
    return SRSLTE_ERROR;
  }

  // 1 port, 2 antennas, MIMO freq config
  if (param_test(
          "tx_port0=tcp://*:2001,tx_port1=tcp://*:2003,rx_port0=tcp://localhost:2000,rx_port1=tcp://"
          "localhost:2002,id=ue,base_srate=23.04e6,tx_freq0=2510e6,tx_freq1=2510e6,rx_freq0=2630e6,,rx_freq1=2630e6",
          2)) {
    fprintf(stderr, "Param test failed!\n");
    return SRSLTE_ERROR;
  }

  // single tx, single rx with continuous transmissions (no timed tx) using IPC transport
  if (run_test("rx_port=ipc://link1,id=ue,base_srate=1.92e6", "tx_port=ipc://link1,id=enb,base_srate=1.92e6", false) !=
      SRSLTE_SUCCESS) {
    fprintf(stderr, "Single tx, single rx test failed!\n");
    return -1;
  }

  // two trx radios with continous tx (no timed tx) using TCP transport for both directions
  if (run_test("tx_port=tcp://*:5554,rx_port=tcp://localhost:5555,id=ue,base_srate=1.92e6",
               "rx_port=tcp://localhost:5554,tx_port=tcp://*:5555,id=enb,base_srate=1.92e6",
               false) != SRSLTE_SUCCESS) {
    fprintf(stderr, "Two TRx radio test failed!\n");
    return -1;
  }

  // two trx radios with continous tx (no timed tx) using TCP for UL (UE tx) and IPC for eNB DL (eNB tx)
  if (run_test("tx_port=tcp://*:5554,rx_port=ipc://dl,id=ue,base_srate=1.92e6",
               "rx_port=tcp://localhost:5554,tx_port=ipc://dl,id=enb,base_srate=1.92e6",
               true) != SRSLTE_SUCCESS) {
    fprintf(stderr, "Two TRx radio test with timed tx failed!\n");
    return -1;
  }

  return SRSLTE_SUCCESS;
}
