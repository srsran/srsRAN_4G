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

#include <iostream> 
#include <boost/thread/mutex.hpp>
#include "sgw/sgw.h"

namespace srsepc{

sgw*          sgw::m_instance = NULL;
boost::mutex  sgw_instance_mutex;

sgw::sgw():
  m_running(false)
{
  m_pool = srslte::byte_buffer_pool::get_instance();     
  return;
}

sgw::~sgw()
{
  return;
}

sgw*
sgw::get_instance(void)
{
  boost::mutex::scoped_lock lock(sgw_instance_mutex);
  if(NULL == m_instance) {
    m_instance = new sgw();
  }
  return(m_instance);
}

void
sgw::cleanup(void)
{
  boost::mutex::scoped_lock lock(sgw_instance_mutex);
  if(NULL != m_instance) {
    delete m_instance;
    m_instance = NULL;
  }
}

int
sgw::init(sgw_args_t* args, srslte::log_filter *sgw_log)
{
  
  m_sgw_log = sgw_log;
  m_sgw_log->info("S-GW Initialized.\n");
  m_sgw_log->console("S-GW Initialized.\n");
  return 0;
}

void
sgw::stop()
{
  if(m_running)
  {
    m_running = false;
    thread_cancel();
    wait_thread_finish();
  }
  return;
}

void
sgw::run_thread()
{
  //Mark the thread as running
  m_running=true;
  while (m_running)
  {
    sleep(1);
  }
  return;
}

} //namespace srsepc
