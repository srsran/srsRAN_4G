/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2017 Software Radio Systems Limited
 *
 * \section LICENSE
 *
 * This file is part of srsLTE.
 *
 * srsUE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsUE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#include <string.h>
#include <map>

#include "srslte/common/buffer_pool.h"
#include "srslte/common/log.h"
#include "common_enb.h"
#include "srslte/common/threads.h"
#include "srslte/srslte.h"
#include "srslte/interfaces/enb_interfaces.h"

#ifndef SRSENB_GTPU_H
#define SRSENB_GTPU_H


namespace srsenb {

/****************************************************************************
 * GTPU Header
 * Ref: 3GPP TS 29.281 v10.1.0 Section 5
 *
 *        | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 |
 *
 * 1      |  Version  |PT | * | E | S |PN |
 * 2      |           Message Type        |
 * 3      |         Length (1st Octet)    |
 * 4      |         Length (2nd Octet)    |
 * 5      |          TEID (1st Octet)     |
 * 6      |          TEID (2nd Octet)     |
 * 7      |          TEID (3rd Octet)     |
 * 8      |          TEID (4th Octet)     |
 ***************************************************************************/

#define GTPU_HEADER_LEN 8

// local gtp, mme and m1-u config passed via enb args
struct gtpu_config_t{
 const std::string gtp_bind_addr;
 const std::string mme_addr;
 const std::string m1u_bind_addr;
 const std::string m1u_multi_addr;

 gtpu_config_t(const std::string & gtp_bind_addr_,
               const std::string & mme_addr_,
               const std::string & m1u_bind_addr_,
               const std::string & m1u_multi_addr_) :
  gtp_bind_addr(gtp_bind_addr_),
  mme_addr(mme_addr_),
  m1u_bind_addr(m1u_bind_addr_),
  m1u_multi_addr(m1u_multi_addr_)
 { }
};

class gtpu
    :public gtpu_interface_rrc
    ,public gtpu_interface_pdcp
    ,public thread
{
public:

  gtpu();

  bool init(const gtpu_config_t & config, pdcp_interface_gtpu *pdcp_, srslte::log *gtpu_log_, bool enable_mbsfn = false);
  void stop();

  // gtpu_interface_rrc
  void add_bearer(uint16_t rnti, uint32_t lcid, uint32_t addr, uint32_t teid_out, uint32_t *teid_in);
  void rem_bearer(uint16_t rnti, uint32_t lcid);
  void rem_user(uint16_t rnti);

  // gtpu_interface_pdcp
  void write_pdu(uint16_t rnti, uint32_t lcid, srslte::byte_buffer_t *pdu);

private:
  static const int THREAD_PRIO = 65;
  static const int GTPU_PORT   = 2152;
  srslte::byte_buffer_pool     *pool;
  bool                         running;
  bool                         run_enable;

  bool                         enable_mbsfn;
  std::string                  gtp_bind_addr;
  std::string                  mme_addr;
  std::string                  m1u_bind_addr;
  std::string                  m1u_multi_addr;
  srsenb::pdcp_interface_gtpu *pdcp;
  srslte::log                 *gtpu_log;

  struct mch_config_t{
    const std::string m1u_bind_addr;
    const std::string m1u_multi_addr;

    mch_config_t( const std::string & m1u_bind_addr_,
                  const std::string & m1u_multi_addr_) :
     m1u_bind_addr(m1u_bind_addr_),
     m1u_multi_addr(m1u_multi_addr_)
    { }
};


  // Class to create
  class mch_thread : public thread {
  public:
    mch_thread() : initiated(false),running(false),run_enable(false),pool(NULL) {}
    bool init(const mch_config_t & config, pdcp_interface_gtpu *pdcp_, srslte::log *gtpu_log_);
    void stop();
  private:
    void run_thread();

    bool initiated;
    bool running;
    bool run_enable;

    static const int MCH_THREAD_PRIO = 65;

    pdcp_interface_gtpu *pdcp;
    srslte::log         *gtpu_log;
    int m1u_sd;
    int lcid_counter;
    std::string   m1u_bind_addr;
    std::string   m1u_multi_addr;

    srslte::byte_buffer_pool *pool;
  };

  // MCH thread insteance
  mch_thread  mchthread;

  typedef struct{
    uint32_t teids_in[SRSENB_N_RADIO_BEARERS];
    uint32_t teids_out[SRSENB_N_RADIO_BEARERS];
    uint32_t spgw_addrs[SRSENB_N_RADIO_BEARERS];
  }bearer_map;
  std::map<uint16_t, bearer_map> rnti_bearers;

  // Socket file descriptors
  int snk_fd;
  int src_fd;

  //Threading
  void run_thread();

  pthread_mutex_t mutex;

  /****************************************************************************
   * TEID to RNIT/LCID helper functions
   ***************************************************************************/
  void teidin_to_rntilcid(uint32_t teidin, uint16_t *rnti, uint16_t *lcid);
  void rntilcid_to_teidin(uint16_t rnti, uint16_t lcid, uint32_t *teidin);
};


} // namespace srsenb

#endif // SRSENB_GTPU_H
