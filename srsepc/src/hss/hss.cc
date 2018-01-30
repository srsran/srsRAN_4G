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
#include <string>
#include <sstream>
#include <boost/thread/mutex.hpp>
#include "hss/hss.h"
#include "srslte/common/security.h"

using namespace srslte;

namespace srsepc{

hss*          hss::m_instance = NULL;
boost::mutex  hss_instance_mutex;

hss::hss()
// :m_sqn(0x112233445566)
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
hss::init(hss_args_t *hss_args, srslte::log_filter *hss_log)
{
  srand(time(NULL));
  /*Init loggers*/
  m_hss_log = hss_log;

  /*Set authentication algorithm*/
  if(set_auth_algo(hss_args->auth_algo) == false)
  {
    return -1;
  }
  /*Read user information from DB*/
  if(read_db_file(hss_args->db_file) == false)
  {
    m_hss_log->console("Error reading user database file %s\n", hss_args->db_file.c_str());
    return -1;
  }

  mcc = hss_args->mcc;
  mnc = hss_args->mnc;

  m_hss_log->info("HSS Initialized. DB file %s, authentication algorithm %s, MCC: %d, MNC: %d\n", hss_args->db_file.c_str(),hss_args->auth_algo.c_str(), mcc, mnc);
  m_hss_log->console("HSS Initialized\n");
  return 0;
}

void
hss::stop(void)
{
  std::map<uint64_t,hss_ue_ctx_t*>::iterator it = m_imsi_to_ue_ctx.begin();
  while(it!=m_imsi_to_ue_ctx.end())
    {
      m_hss_log->info("Deleting UE context in HSS. IMSI: %lu\n", it->second->imsi);
      m_hss_log->console("Deleting UE context in HSS. IMSI: %lu\n", it->second->imsi);
      delete it->second;
      m_imsi_to_ue_ctx.erase(it++);
    }
  if(m_db_file.is_open())
    {
      m_db_file.close();
    }
  return;
}


bool
hss::set_auth_algo(std::string auth_algo)
{
  if(auth_algo != "xor" && auth_algo != "milenage" )
  {
    m_hss_log->error("Unrecognized authentication algorithm. auth_algo = %s\n", auth_algo.c_str());
    return false;
  }
  if(auth_algo == "xor")
  {
    m_auth_algo = HSS_ALGO_XOR;
  }
  else
  {
    m_auth_algo = HSS_ALGO_MILENAGE;
  }
  return true;
}

bool
hss::read_db_file(std::string db_filename)
{
  m_db_file.open(db_filename.c_str(), std::ifstream::in);
  if(!m_db_file.is_open())
  {
    return false;
  }
  m_hss_log->info("Opened DB file: %s\n", db_filename.c_str() );

  std::string line;
  while (std::getline(m_db_file, line))
  {
    if(line[0] != '#')
    {
      std::vector<std::string> split = split_string(line,','); 
      if(split.size()!=5)
      {
        m_hss_log->error("Error parsing UE database\n");
        return false;
      }
      hss_ue_ctx_t *ue_ctx = new hss_ue_ctx_t;
      ue_ctx->name = split[0];
      ue_ctx->imsi = atoll(split[1].c_str());
      get_uint_vec_from_hex_str(split[2],ue_ctx->key,16);
      get_uint_vec_from_hex_str(split[3],ue_ctx->op,16);
      get_uint_vec_from_hex_str(split[4],ue_ctx->amf,2);

      m_hss_log->debug("Added user from DB, IMSI: %015lu\n", ue_ctx->imsi);
      m_hss_log->debug_hex(ue_ctx->key, 16, "User Key : ");
      m_hss_log->debug_hex(ue_ctx->op, 16, "User OP : ");
      m_hss_log->debug_hex(ue_ctx->amf, 2, "AMF : ");

      m_imsi_to_ue_ctx.insert(std::pair<uint64_t,hss_ue_ctx_t*>(ue_ctx->imsi,ue_ctx));
    }
  }

  return true;
}

bool
hss::gen_auth_info_answer(uint64_t imsi, uint8_t *k_asme, uint8_t *autn, uint8_t *rand, uint8_t *xres)
{
  bool ret = false;
  switch (m_auth_algo)
  {
  case HSS_ALGO_XOR:
    ret = gen_auth_info_answer_xor(imsi, k_asme, autn, rand, xres);
    break;
  case HSS_ALGO_MILENAGE:
    ret = gen_auth_info_answer_milenage(imsi, k_asme, autn, rand, xres);
    break;
  }
  return ret;

}

bool
hss::gen_auth_info_answer_milenage(uint64_t imsi, uint8_t *k_asme, uint8_t *autn, uint8_t *rand, uint8_t *xres)
{
  uint8_t k[16];
  uint8_t amf[2];
  uint8_t op[16];
  uint8_t sqn[6];

  uint8_t     ck[16];
  uint8_t     ik[16];
  uint8_t     ak[6];
  uint8_t     mac[8];


  if(!get_k_amf_op(imsi,k,amf,op))
  {
    return false;
  }
  gen_rand(rand);
  get_sqn(sqn);
 
  security_milenage_f2345( k,
                           op,
                           rand,
                           xres,
                           ck,
                           ik,
                           ak);

  m_hss_log->debug_hex(k, 16, "User Key : ");
  m_hss_log->debug_hex(op, 16, "User OP : ");
  m_hss_log->debug_hex(rand, 16, "User Rand : ");
  m_hss_log->debug_hex(xres, 8, "User XRES: ");
  m_hss_log->debug_hex(ck, 16, "User CK: ");
  m_hss_log->debug_hex(ik, 16, "User IK: ");
  m_hss_log->debug_hex(ak, 6, "User AK: ");

  security_milenage_f1( k,
                        op,
                        rand,
                        sqn,
                        amf,
                        mac);

  m_hss_log->debug_hex(sqn, 6, "User SQN : ");
  m_hss_log->debug_hex(mac, 8, "User MAC : ");

  // Generate K_asme
  security_generate_k_asme( ck,
                            ik,
                            ak,
                            sqn,
                            mcc,
                            mnc,
                            k_asme);

  m_hss_log->debug("User MCC : %x  MNC : %x \n", mcc, mnc);
  m_hss_log->debug_hex(k_asme, 32, "User k_asme : ");

  //Generate AUTN (autn = sqn ^ ak |+| amf |+| mac)
  for(int i=0;i<6;i++ )
  {
    autn[i] = sqn[i]^ak[i];
  }
  for(int i=0;i<2;i++)
  {
    autn[6+i]=amf[i];
  }
  for(int i=0;i<8;i++)
  {
    autn[8+i]=mac[i];
  }
  
  m_hss_log->debug_hex(autn, 16, "User AUTN: ");

  return true;
}

bool
hss::gen_auth_info_answer_xor(uint64_t imsi, uint8_t *k_asme, uint8_t *autn, uint8_t *rand, uint8_t *xres)
{
  uint8_t k[16];
  uint8_t amf[2];
  uint8_t op[16];
  uint8_t sqn[6];

  uint8_t  xdout[16];
  uint8_t  cdout[8];

  uint8_t     ck[16];
  uint8_t     ik[16];
  uint8_t     ak[6];
  uint8_t     mac[8];

  int i = 0;

  if(!get_k_amf_op(imsi,k,amf,op))
  {
    return false;
  }
  gen_rand(rand);
  get_sqn(sqn);

  // Use RAND and K to compute RES, CK, IK and AK
  for(i=0; i<16; i++) {
    xdout[i] = k[i]^rand[i];
  }

  for(i=0; i<16; i++) {
    xres[i]  = xdout[i];
    ck[i]   = xdout[(i+1)%16];
    ik[i]   = xdout[(i+2)%16];
  }
  for(i=0; i<6; i++) {
    ak[i] = xdout[i+3];
  }

  m_hss_log->debug_hex(k, 16, "User Key : ");
  m_hss_log->debug_hex(op, 16, "User OP : ");
  m_hss_log->debug_hex(rand, 16, "User Rand : ");
  m_hss_log->debug_hex(xres, 8, "User XRES: ");
  m_hss_log->debug_hex(ck, 16, "User CK: ");
  m_hss_log->debug_hex(ik, 16, "User IK: ");
  m_hss_log->debug_hex(ak, 6, "User AK: ");

  // Generate cdout
  for(i=0; i<6; i++) {
    cdout[i] = sqn[i];
  }
  for(i=0; i<2; i++) {
    cdout[6+i] = amf[i];
  }

  // Generate MAC
  for(i=0;i<8;i++) {
    mac[i] = xdout[i] ^ cdout[i];
  }

  m_hss_log->debug_hex(sqn, 6, "User SQN : ");
  m_hss_log->debug_hex(mac, 8, "User MAC : ");

  //Generate AUTN (autn = sqn ^ ak |+| amf |+| mac)
  for(int i=0;i<6;i++ )
  {
      autn[i] = sqn[i]^ak[i];
  }
  for(int i=0;i<2;i++)
  {
      autn[6+i]=amf[i];
  }
  for(int i=0;i<8;i++)
  {
      autn[8+i]=mac[i];
  }

  // Generate K_asme
  security_generate_k_asme( ck,
                            ik,
                            ak,
                            sqn,
                            mcc,
                            mnc,
                            k_asme);
  
  m_hss_log->debug("User MCC : %x  MNC : %x \n", mcc, mnc);
  m_hss_log->debug_hex(k_asme, 32, "User k_asme : ");

  //Generate AUTN (autn = sqn ^ ak |+| amf |+| mac)
  for(int i=0;i<6;i++ )
  {
    autn[i] = sqn[i]^ak[i];
  }
  for(int i=0;i<2;i++)
  {
    autn[6+i]=amf[i];
  }
  for(int i=0;i<8;i++)
  {
    autn[8+i]=mac[i];
  }

  m_hss_log->debug_hex(autn, 8, "User AUTN: ");

  return true;
}


bool
hss::get_k_amf_op(uint64_t imsi, uint8_t *k, uint8_t *amf, uint8_t *op )
{

  /*
  uint8_t k_tmp[16] ={0x00,0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x99,0xaa,0xbb,0xcc,0xdd,0xee,0xff};
  uint8_t amf_tmp[2]={0x80,0x00};
  uint8_t op_tmp[16]={0x63,0xbf,0xA5,0x0E,0xE6,0x52,0x33,0x65,0xFF,0x14,0xC1,0xF4,0x5F,0x88,0x73,0x7D};
  */
  std::map<uint64_t,hss_ue_ctx_t*>::iterator ue_ctx_it = m_imsi_to_ue_ctx.find(imsi);
  if(ue_ctx_it == m_imsi_to_ue_ctx.end())
  {
    m_hss_log->info("User not found. IMSI: %015lu\n",imsi);
    m_hss_log->console("User not found. IMSI: %015lu\n",imsi);
    return false;
  }
  hss_ue_ctx_t *ue_ctx = ue_ctx_it->second;
  m_hss_log->info("Found User %015lu\n",imsi);
  memcpy(k,ue_ctx->key,16);
  memcpy(amf,ue_ctx->amf,2);
  memcpy(op,ue_ctx->op,16);

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

/* Helper functions*/
std::vector<std::string>
hss::split_string(const std::string &str, char delimiter)
{
  std::vector<std::string> tokens;
  std::string token;
  std::istringstream tokenStream(str);

  while (std::getline(tokenStream, token, delimiter))
  {
    tokens.push_back(token); 
  }
  return tokens;
}

void
hss::get_uint_vec_from_hex_str(const std::string &key_str, uint8_t *key, uint len)
{
  const char *pos =  key_str.c_str();

  for (uint count = 0; count < len; count++) {
    sscanf(pos, "%2hhx", &key[count]);
    pos += 2;
  }

  return;
}

  /*
uint64_t
string_to_imsi()
{
  uint64_t imsi = 0;
  for(int i=0;i<=14;i++){
    imsi  += attach_req.eps_mobile_id.imsi[i]*std::pow(10,14-i);
  }
  return imsi;
}
  */
} //namespace srsepc
