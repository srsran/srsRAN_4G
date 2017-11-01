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
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include <boost/thread/mutex.hpp>
#include "hss/hss.h"

namespace srsepc{

hss*          hss::m_instance = NULL;
boost::mutex  hss_instance_mutex;

hss::hss()
 :m_sqn(0)
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
hss::init(hss_args_t *hss_args, srslte::logger *logger)
{
  /*Init loggers*/
  m_logger=logger;
  m_hss_log.init("HSS", m_logger);
  m_hss_log.set_level(srslte::LOG_LEVEL_DEBUG);
  m_hss_log.set_hex_limit(32);

  srand(time(NULL));

  m_hss_log.info("Initialized HSS\n");
  m_hss_log.console("Initialized HSS\n");
  return 0;
}

bool
hss::gen_auth_info_answer_milenage(uint64_t imsi, uint8_t *kasme, uint8_t *autn, uint8_t *rand, uint8_t *xres)
{
  uint8_t k[16];
  uint8_t amf[2];
  uint8_t op[16];
  //uint8_t rand[16];
  uint8_t sqn[6];

  if(!get_k_amf_op(imsi,k,amf,op))
  {
    return false;
  }
  gen_rand(rand);
  get_sqn(sqn);

  return true;
}

bool
hss::get_k_amf_op(uint64_t imsi, uint8_t *k, uint8_t *amf, uint8_t *op )
{

  uint8_t k_tmp[16];
  uint8_t amf_tmp[2]={0x80,0x00};
  uint8_t op_tmp[16]={0x63,0xbf,0xA5,0x0E,0xE6,0x52,0x33,0x65,0xFF,0x14,0xC1,0xF4,0x5F,0x88,0x73,0x7D};
  
  if(imsi != 1010123456789)
  {
    m_hss_log.console("Usernot found. IMSI: %015lu\n",imsi);
    return false;
  }

  for(int i=0;i<8;i++)
  {
    k_tmp[2*i]=i;
    k_tmp[2*i+1]=i;
  }

  m_hss_log.console("Found User %015lu\n",imsi);
  memcpy(k,k_tmp,16);
  memcpy(amf,amf_tmp,2);
  memcpy(op,op_tmp,16);

  return true;
}

void
hss::get_sqn(uint8_t sqn[6])
{
  for (int i=0; i<6; i++)
  {
    sqn[i] = ((uint8_t *)&m_sqn)[i];  
  }
  m_sqn++;
  return; //TODO See TS 33.102, Annex C
}

void
hss::gen_rand(uint8_t rand_[16])
{
  for(int i=0;i<16;i++)
  {
    rand_[i]=rand()%256; //Pulls on byte at a time. It's slow, but does not depend on RAND_MAX.
  }
  return;
}

} //namespace srsepc
