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
#include <boost/thread/mutex.hpp>
#include "mme/mme.h"

namespace srsepc{

mme*          mme::m_instance = NULL;
boost::mutex  mme_instance_mutex;

mme::mme()
{
    return;
}

mme::~mme()
{
  return;
}

mme*
mme::get_instance(void)
{
  boost::mutex::scoped_lock lock(mme_instance_mutex);
  if(NULL == m_instance) {
    m_instance = new mme();
  }
  return(m_instance);
}

void
mme::cleanup(void)
{
  boost::mutex::scoped_lock lock(mme_instance_mutex);
  if(NULL != m_instance) {
    delete m_instance;
    m_instance = NULL;
  }
}

int
mme::init(all_args_t* args)
{
  if(m_s1ap.init(args->s1ap_args)){
    std::cout << "Error initializing MME S1APP" << std::endl;
    exit(-1);
  }
  return 0;
}

int
mme::get_s1_mme()
{
  return m_s1ap.get_s1_mme();
}

} //namespace srsepc
