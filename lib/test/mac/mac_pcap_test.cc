/**
 * Copyright 2013-2021 Software Radio Systems Limited
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

#include "srslte/common/common.h"
#include "srslte/common/mac_pcap.h"
#include "srslte/common/test_common.h"
#include <iostream>
#include <thread>

// Write #num_pdus UL MAC PDUs using PCAP handle
void write_pcap_eutra_thread_function(srslte::mac_pcap*               pcap_handle,
                                      const std::array<uint8_t, 150>& pdu,
                                      uint32_t                        num_pdus)
{
  for (uint32_t i = 0; i < num_pdus; i++) {
    pcap_handle->write_ul_crnti(const_cast<uint8_t*>(pdu.data()), pdu.size(), 0x1001, true, 1, 0);
  }

  std::cout << "Finished thread " << std::this_thread::get_id() << "\n";
}

// Write #num_pdus DL MAC NR PDUs using PCAP handle
void write_pcap_nr_thread_function(srslte::mac_pcap* pcap_handle, const std::array<uint8_t, 11>& pdu, uint32_t num_pdus)
{
  for (uint32_t i = 0; i < num_pdus; i++) {
    pcap_handle->write_dl_crnti_nr(const_cast<uint8_t*>(pdu.data()), pdu.size(), 0x1001, 0, 1);
  }

  std::cout << "Finished thread " << std::this_thread::get_id() << "\n";
}

int mac_pcap_eutra_test()
{
  std::array<uint8_t, 150> tv = {
      0x21, 0x08, 0x22, 0x80, 0x82, 0x1f, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x02, 0x02, 0x02, 0x02, 0x02,
      0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02,
      0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02,
      0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02,
      0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02,
      0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02,
      0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02,
      0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

  uint32_t num_threads         = 10;
  uint32_t num_pdus_per_thread = 100;

  std::unique_ptr<srslte::mac_pcap> pcap_handle =
      std::unique_ptr<srslte::mac_pcap>(new srslte::mac_pcap(srslte::srslte_rat_t::lte));
  TESTASSERT(pcap_handle->open("mac_pcap_test.pcap") == SRSLTE_SUCCESS);
  TESTASSERT(pcap_handle->open("mac_pcap_test.pcap") != SRSLTE_SUCCESS); // open again will fail

  std::vector<std::thread> writer_threads;

  for (uint32_t i = 0; i < num_threads; i++) {
    writer_threads.push_back(std::thread(write_pcap_eutra_thread_function, pcap_handle.get(), tv, num_pdus_per_thread));
  }

  // wait for threads to finish
  for (std::thread& thread : writer_threads) {
    thread.join();
  }

  TESTASSERT(pcap_handle->close() == SRSLTE_SUCCESS);
  TESTASSERT(pcap_handle->close() != 0); // closing twice will fail

  return SRSLTE_SUCCESS;
}

int mac_pcap_nr_test()
{
  std::array<uint8_t, 11> tv = {0x42, 0x00, 0x08, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};

  uint32_t num_threads         = 10;
  uint32_t num_pdus_per_thread = 100;

  std::unique_ptr<srslte::mac_pcap> pcap_handle =
      std::unique_ptr<srslte::mac_pcap>(new srslte::mac_pcap(srslte::srslte_rat_t::nr));
  TESTASSERT(pcap_handle->open("mac_pcap_nr_test.pcap") == SRSLTE_SUCCESS);
  TESTASSERT(pcap_handle->open("mac_pcap_nr_test.pcap") != SRSLTE_SUCCESS); // open again will fail

  std::vector<std::thread> writer_threads;

  for (uint32_t i = 0; i < num_threads; i++) {
    writer_threads.push_back(std::thread(write_pcap_nr_thread_function, pcap_handle.get(), tv, num_pdus_per_thread));
  }

  // wait for threads to finish
  for (std::thread& thread : writer_threads) {
    thread.join();
  }

  TESTASSERT(pcap_handle->close() == SRSLTE_SUCCESS);
  TESTASSERT(pcap_handle->close() != 0); // closing twice will fail

  return SRSLTE_SUCCESS;
}

int main()
{
  TESTASSERT(mac_pcap_eutra_test() == SRSLTE_SUCCESS);
  TESTASSERT(mac_pcap_nr_test() == SRSLTE_SUCCESS);
}
