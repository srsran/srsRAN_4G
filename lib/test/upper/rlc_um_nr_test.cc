/*
 * Copyright 2013-2019 Software Radio Systems Limited
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

#include "rlc_test_common.h"
#include "srslte/common/log_filter.h"
#include "srslte/config.h"
#include "srslte/upper/rlc.h"
#include "srslte/upper/rlc_um.h"

#include <array>
#include <iostream>
#include <memory>
#include <vector>

#define TESTASSERT(cond)                                                                                               \
  {                                                                                                                    \
    if (!(cond)) {                                                                                                     \
      std::cout << "[" << __FUNCTION__ << "][Line " << __LINE__ << "]: FAIL at " << (#cond) << std::endl;              \
      return -1;                                                                                                       \
    }                                                                                                                  \
  }

#define PCAP 0
#define PCAP_CRNTI (0x1001)
#define PCAP_TTI (666)

using namespace srslte;

#if PCAP
#include "srslte/common/mac_nr_pcap.h"
#include "srslte/common/mac_nr_pdu.h"
static std::unique_ptr<srslte::mac_nr_pcap> pcap_handle = nullptr;
#endif

int write_pdu_to_pcap(const uint32_t lcid, const uint8_t* payload, const uint32_t len)
{
#if PCAP
  if (pcap_handle) {
    byte_buffer_t      tx_buffer;
    srslte::nr_mac_pdu tx_pdu;
    tx_pdu.init_tx(&tx_buffer, len + 10);
    tx_pdu.add_sdu(lcid, payload, len);
    tx_pdu.pack();
    pcap_handle->write_dl_crnti(tx_buffer.msg, tx_buffer.N_bytes, PCAP_CRNTI, true, PCAP_TTI);
    return SRSLTE_SUCCESS;
  }
#endif
  return SRSLTE_ERROR;
}

template <std::size_t N>
srslte::byte_buffer_t make_pdu_and_log(const std::array<uint8_t, N>& tv)
{
  srslte::byte_buffer_t pdu;
  memcpy(pdu.msg, tv.data(), tv.size());
  pdu.N_bytes = tv.size();
  write_pdu_to_pcap(4, tv.data(), tv.size());
  return pdu;
}

// Basic test to write UM PDU with 6 bit SN
int rlc_um_nr_test1()
{
  srslte::log_filter log1("RLC_UM_1");
  srslte::log_filter log2("RLC_UM_2");
  log1.set_level(srslte::LOG_LEVEL_DEBUG);
  log2.set_level(srslte::LOG_LEVEL_DEBUG);
  log1.set_hex_limit(-1);
  log2.set_hex_limit(-1);
  rlc_um_tester  tester;
  srslte::timers timers(16);
  const uint32_t num_sdus = 5;
  int            len      = 0;

  rlc_um rlc1(&log1, 3, &tester, &tester, &timers);
  rlc_um rlc2(&log2, 3, &tester, &tester, &timers);

  rlc_config_t cnfg = rlc_config_t::default_rlc_um_nr_config(6);

  TESTASSERT(rlc1.configure(cnfg) == true);
  TESTASSERT(rlc2.configure(cnfg) == true);

  tester.set_expected_sdu_len(1);

  // Push 5 SDUs into RLC1
  byte_buffer_pool*    pool = byte_buffer_pool::get_instance();
  unique_byte_buffer_t sdu_bufs[num_sdus];
  for (uint32_t i = 0; i < num_sdus; i++) {
    sdu_bufs[i]          = srslte::allocate_unique_buffer(*pool, true);
    *sdu_bufs[i]->msg    = i; // Write the index into the buffer
    sdu_bufs[i]->N_bytes = 1; // Give each buffer a size of 1 byte
    rlc1.write_sdu(std::move(sdu_bufs[i]));
  }

  TESTASSERT(14 == rlc1.get_buffer_state());

  // Read 5 PDUs from RLC1 (1 byte each)
  unique_byte_buffer_t pdu_bufs[num_sdus];
  for (uint32_t i = 0; i < num_sdus; i++) {
    pdu_bufs[i]          = srslte::allocate_unique_buffer(*pool, true);
    len                  = rlc1.read_pdu(pdu_bufs[i]->msg, 4); // 3 bytes for header + payload
    pdu_bufs[i]->N_bytes = len;

    // write PCAP
    write_pdu_to_pcap(4, pdu_bufs[i]->msg, pdu_bufs[i]->N_bytes);
  }

  TESTASSERT(0 == rlc1.get_buffer_state());

  // TODO: add receive test

  return SRSLTE_SUCCESS;
}

// Basic test for SDU segmentation
int rlc_um_nr_test2()
{
  srslte::log_filter log1("RLC_UM_1");
  srslte::log_filter log2("RLC_UM_2");
  log1.set_level(srslte::LOG_LEVEL_DEBUG);
  log2.set_level(srslte::LOG_LEVEL_DEBUG);
  log1.set_hex_limit(-1);
  log2.set_hex_limit(-1);
  rlc_um_tester  tester;
  srslte::timers timers(16);
  const uint32_t num_sdus = 1;
  const uint32_t sdu_size = 100;
  int            len      = 0;

  rlc_um rlc1(&log1, 3, &tester, &tester, &timers);
  rlc_um rlc2(&log2, 3, &tester, &tester, &timers);

  rlc_config_t cnfg = rlc_config_t::default_rlc_um_nr_config(6);

  TESTASSERT(rlc1.configure(cnfg) == true);
  TESTASSERT(rlc2.configure(cnfg) == true);

  tester.set_expected_sdu_len(1);

  // Push SDUs into RLC1
  byte_buffer_pool*    pool = byte_buffer_pool::get_instance();
  unique_byte_buffer_t sdu_bufs[num_sdus];
  for (uint32_t i = 0; i < num_sdus; i++) {
    sdu_bufs[i] = srslte::allocate_unique_buffer(*pool, true);
    // Write the index into the buffer
    for (uint32_t k = 0; k < sdu_size; ++k) {
      sdu_bufs[i]->msg[k] = i;
    }
    sdu_bufs[i]->N_bytes = sdu_size;
    rlc1.write_sdu(std::move(sdu_bufs[i]));
  }

  // FIXME: check buffer state calculation
  TESTASSERT(103 == rlc1.get_buffer_state());

  // Read PDUs from RLC1 with grant of 25 Bytes each
  const uint32_t       max_num_pdus = 10;
  unique_byte_buffer_t pdu_bufs[max_num_pdus];
  for (uint32_t i = 0; i < max_num_pdus; i++) {
    pdu_bufs[i]          = srslte::allocate_unique_buffer(*pool, true);
    len                  = rlc1.read_pdu(pdu_bufs[i]->msg, 25); // 3 bytes for header + payload
    pdu_bufs[i]->N_bytes = len;

    // write PCAP
    write_pdu_to_pcap(4, pdu_bufs[i]->msg, pdu_bufs[i]->N_bytes);
  }

  TESTASSERT(0 == rlc1.get_buffer_state());

  // TODO: add receive test

  return SRSLTE_SUCCESS;
}

int main(int argc, char** argv)
{
#if PCAP
  pcap_handle = std::unique_ptr<srslte::mac_nr_pcap>(new srslte::mac_nr_pcap());
  pcap_handle->open("rlc_um_nr_test.pcap");
#endif

  if (rlc_um_nr_test1()) {
    fprintf(stderr, "rlc_um_nr_test1() failed.\n");
    return SRSLTE_ERROR;
  }

  if (rlc_um_nr_test2()) {
    fprintf(stderr, "rlc_um_nr_test2() failed.\n");
    return SRSLTE_ERROR;
  }
  return SRSLTE_SUCCESS;
}
