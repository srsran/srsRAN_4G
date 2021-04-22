/**
 * Copyright 2013-2021 Software Radio Systems Limited
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

#ifndef SRSRAN_PDCP_BASE_TEST_H
#define SRSRAN_PDCP_BASE_TEST_H

#include "srsran/common/buffer_pool.h"
#include "srsran/common/security.h"
#include "srsran/common/test_common.h"
#include "srsran/interfaces/pdcp_interface_types.h"
#include "srsran/interfaces/ue_gw_interfaces.h"
#include "srsran/interfaces/ue_interfaces.h"
#include "srsran/interfaces/ue_rlc_interfaces.h"
#include <iostream>

int compare_two_packets(const srsran::unique_byte_buffer_t& msg1, const srsran::unique_byte_buffer_t& msg2)
{
  TESTASSERT(msg1->N_bytes == msg2->N_bytes);
  TESTASSERT(memcmp(msg1->msg, msg2->msg, msg1->N_bytes) == 0);
  return 0;
}

/*
 * Dummy classes
 */
class rlc_dummy : public srsue::rlc_interface_pdcp
{
public:
  explicit rlc_dummy(srslog::basic_logger& logger) : logger(logger) {}

  void get_last_sdu(const srsran::unique_byte_buffer_t& pdu)
  {
    memcpy(pdu->msg, last_pdcp_pdu->msg, last_pdcp_pdu->N_bytes);
    pdu->N_bytes = last_pdcp_pdu->N_bytes;
  }
  void write_sdu(uint32_t lcid, srsran::unique_byte_buffer_t sdu)
  {
    logger.info(sdu->msg, sdu->N_bytes, "RLC SDU");
    last_pdcp_pdu.swap(sdu);
    rx_count++;
  }
  void discard_sdu(uint32_t lcid, uint32_t discard_sn)
  {
    logger.info("Notifing RLC to discard SDU (SN=%u)", discard_sn);
    discard_count++;
    logger.info("Discard_count=%" PRIu64 "", discard_count);
  }

  uint64_t rx_count      = 0;
  uint64_t discard_count = 0;

private:
  srslog::basic_logger&        logger;
  srsran::unique_byte_buffer_t last_pdcp_pdu;

  bool rb_is_um(uint32_t lcid) { return false; }
  bool sdu_queue_is_full(uint32_t lcid) { return false; };
};

class rrc_dummy : public srsue::rrc_interface_pdcp
{
public:
  explicit rrc_dummy(srslog::basic_logger& logger) : logger(logger) {}

  void write_pdu_bcch_bch(srsran::unique_byte_buffer_t pdu) {}
  void write_pdu_bcch_dlsch(srsran::unique_byte_buffer_t pdu) {}
  void write_pdu_pcch(srsran::unique_byte_buffer_t pdu) {}
  void write_pdu_mch(uint32_t lcid, srsran::unique_byte_buffer_t pdu) {}

  const char* get_rb_name(uint32_t lcid) { return "None"; }

  srslog::basic_logger& logger;

  // Members for testing
  uint32_t                     rx_count = 0;
  srsran::unique_byte_buffer_t last_pdu;

  // Methods for testing
  void get_last_pdu(const srsran::unique_byte_buffer_t& pdu)
  {
    memcpy(pdu->msg, last_pdu->msg, last_pdu->N_bytes);
    pdu->N_bytes = last_pdu->N_bytes;
  }

  void write_pdu(uint32_t lcid, srsran::unique_byte_buffer_t pdu)
  {
    logger.info(pdu->msg, pdu->N_bytes, "RRC PDU");
    rx_count++;
    last_pdu.swap(pdu);
  }
};

class gw_dummy : public srsue::gw_interface_pdcp
{
public:
  explicit gw_dummy(srslog::basic_logger& logger) : logger(logger) {}

  void     write_pdu_mch(uint32_t lcid, srsran::unique_byte_buffer_t pdu) {}
  uint32_t rx_count = 0;

  void get_last_pdu(const srsran::unique_byte_buffer_t& pdu)
  {
    memcpy(pdu->msg, last_pdu->msg, last_pdu->N_bytes);
    pdu->N_bytes = last_pdu->N_bytes;
  }
  void write_pdu(uint32_t lcid, srsran::unique_byte_buffer_t pdu)
  {
    logger.info(pdu->msg, pdu->N_bytes, "GW PDU");
    rx_count++;
    last_pdu.swap(pdu);
  }

private:
  srslog::basic_logger&        logger;
  srsran::unique_byte_buffer_t last_pdu;
};

// Helper to print packets
void print_packet_array(const srsran::unique_byte_buffer_t& msg)
{
  printf("uint8_t msg[] = {\n");
  for (uint64_t i = 0; i < msg->N_bytes; ++i) {
    printf("0x%02x, ", msg->msg[i]);
  }
  printf("\n};\n");
}
#endif // SRSRAN_PDCP_BASE_TEST_H
