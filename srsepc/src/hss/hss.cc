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

#include <iostream> //TODO Remove
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/sctp.h>
#include <boost/thread/mutex.hpp>
#include "mme/mme.h"

namespace srsepc{

hss*          hss::m_instance = NULL;
boost::mutex  hss_instance_mutex;

hss::hss():
  m_running(false)
{
  m_pool = srslte::byte_buffer_pool::get_instance();     
  return;
}

hss::~hss()
{
  return;
}

hss*
hss::get_instance(void)
{
  boost::mutex::scoped_lock lock(hss_instance_mutex);
  if(NULL == m_instance) {
    m_instance = new hss();
  }
  return(m_instance);
}

void
hss::cleanup(void)
{
  boost::mutex::scoped_lock lock(hss_instance_mutex);
  if(NULL != m_instance) {
    delete m_instance;
    m_instance = NULL;
  }
}

int
hss::init(all_args_t* args)
{
  /*Init loggers*/
  if (!args->log_args.filename.compare("stdout")) {
    m_logger = &m_logger_stdout;
  } else {
    m_logger_file.init(args->log_args.filename);
    m_logger_file.log("\n---  Software Radio Systems HSS log ---\n\n");
    m_logger = &m_logger_file;
  }

  m_hss_log.init("HSS", m_logger);
  m_hss_log.set_level(srslte::LOG_LEVEL_DEBUG);
  m_hss_log.set_hex_limit(32);
  if(m_hss.init(args->s1ap_args, &m_s1ap_log)){
    m_hss_log.error("Error initializing MME S1APP\n");
    exit(-1);
  }
  m_hss_log.info("Initialized HSS\n");
  m_hss_log.console("Initialized HSS\n");
  return 0;
}



} //namespace srsepc
