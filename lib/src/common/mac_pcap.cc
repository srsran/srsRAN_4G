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

#include "srsran/common/mac_pcap.h"
#include "srsran/common/standard_streams.h"
#include "srsran/common/threads.h"

namespace srsran {
mac_pcap::mac_pcap() : mac_pcap_base() {}

mac_pcap::~mac_pcap()
{
  close();
}

uint32_t mac_pcap::open(std::string filename_, uint32_t ue_id_)
{
  std::lock_guard<std::mutex> lock(mutex);
  if (pcap_file != nullptr) {
    logger.error("PCAP writer for %s already running. Close first.", filename_.c_str());
    return SRSRAN_ERROR;
  }

  // set UDP DLT
  dlt       = UDP_DLT;
  pcap_file = DLT_PCAP_Open(dlt, filename_.c_str());
  if (pcap_file == nullptr) {
    logger.error("Couldn't open %s to write PCAP", filename_.c_str());
    return SRSRAN_ERROR;
  }

  filename = filename_;
  ue_id    = ue_id_;
  running  = true;

  // start writer thread
  start();

  return SRSRAN_SUCCESS;
}

uint32_t mac_pcap::close()
{
  {
    std::lock_guard<std::mutex> lock(mutex);
    if (running == false || pcap_file == nullptr) {
      return SRSRAN_ERROR;
    }

    // tell writer thread to stop
    running        = false;
    pcap_pdu_t pdu = {};
    queue.push_blocking(std::move(pdu));
  }

  wait_thread_finish();

  // close file handle
  {
    std::lock_guard<std::mutex> lock(mutex);
    srsran::console("Saving MAC PCAP (DLT=%d) to %s\n", dlt, filename.c_str());
    DLT_PCAP_Close(pcap_file);
    pcap_file = nullptr;
  }

  return SRSRAN_SUCCESS;
}

void mac_pcap::write_pdu(srsran::mac_pcap_base::pcap_pdu_t& pdu)
{
  if (pdu.pdu != nullptr) {
    switch (pdu.rat) {
      case srsran_rat_t::lte:
        LTE_PCAP_MAC_UDP_WritePDU(pcap_file, &pdu.context, pdu.pdu->msg, pdu.pdu->N_bytes);
        break;
      case srsran_rat_t::nr:
        NR_PCAP_MAC_UDP_WritePDU(pcap_file, &pdu.context_nr, pdu.pdu->msg, pdu.pdu->N_bytes);
        break;
      default:
        logger.error("Error writing PDU to PCAP. Unsupported RAT selected.");
    }
  }
}

} // namespace srsran