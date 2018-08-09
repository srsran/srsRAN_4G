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
#include <iomanip>
#include <inttypes.h> // for printing uint64_t
#include "srsepc/hdr/hss/hss.h"
#include "srslte/common/security.h"

using namespace srslte;

namespace srsepc{

hss*          hss::m_instance = NULL;
pthread_mutex_t hss_instance_mutex = PTHREAD_MUTEX_INITIALIZER;

hss::hss()
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
  pthread_mutex_lock(&hss_instance_mutex);
  if(NULL == m_instance) {
    m_instance = new hss();
  }
  pthread_mutex_unlock(&hss_instance_mutex);
  return(m_instance);
}

void
hss::cleanup(void)
{
  pthread_mutex_lock(&hss_instance_mutex);
  if(NULL != m_instance) {
    delete m_instance;
    m_instance = NULL;
  }
  pthread_mutex_unlock(&hss_instance_mutex);
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

  db_file = hss_args->db_file;

  m_hss_log->info("HSS Initialized. DB file %s, authentication algorithm %s, MCC: %d, MNC: %d\n", hss_args->db_file.c_str(),hss_args->auth_algo.c_str(), mcc, mnc);
  m_hss_log->console("HSS Initialized.\n");
  return 0;
}

void
hss::stop(void)
{
  write_db_file(db_file);
  std::map<uint64_t,hss_ue_ctx_t*>::iterator it = m_imsi_to_ue_ctx.begin();
  while(it!=m_imsi_to_ue_ctx.end())
    {
      m_hss_log->info("Deleting UE context in HSS. IMSI: %015lu\n", it->second->imsi);
      m_hss_log->console("Deleting UE context in HSS. IMSI: %015lu\n", it->second->imsi);
      delete it->second;
      m_imsi_to_ue_ctx.erase(it++);
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
  std::ifstream m_db_file;

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
      uint column_size = 8;
      std::vector<std::string> split = split_string(line,',');
      if(split.size() != column_size)
      {
        m_hss_log->error("Error parsing UE database. Wrong number of columns in .csv\n");
        m_hss_log->error("Columns: %lu, Expected %d.\n",split.size(),column_size);
        return false;
      }
      hss_ue_ctx_t *ue_ctx = new hss_ue_ctx_t;
      ue_ctx->name = split[0];
      ue_ctx->imsi = atoll(split[1].c_str());
      get_uint_vec_from_hex_str(split[2],ue_ctx->key,16);
      if(split[3] == std::string("op"))
      {
        ue_ctx->op_configured = true;
        get_uint_vec_from_hex_str(split[4],ue_ctx->op,16);
        compute_opc(ue_ctx->key,ue_ctx->op,ue_ctx->opc);
      }
      else if (split[3] == std::string("opc"))
      {
        ue_ctx->op_configured =false;
        get_uint_vec_from_hex_str(split[4],ue_ctx->opc,16);
      }
      else
      {
        m_hss_log->error("Neither OP nor OPc configured.\n");
        return false;
      }
      get_uint_vec_from_hex_str(split[5],ue_ctx->amf,2);
      get_uint_vec_from_hex_str(split[6],ue_ctx->sqn,6);

      m_hss_log->debug("Added user from DB, IMSI: %015lu\n", ue_ctx->imsi);
      m_hss_log->debug_hex(ue_ctx->key, 16, "User Key : ");
      if(ue_ctx->op_configured){
        m_hss_log->debug_hex(ue_ctx->op, 16, "User OP : ");
      }
      m_hss_log->debug_hex(ue_ctx->opc, 16, "User OPc : ");
      m_hss_log->debug_hex(ue_ctx->amf, 2, "AMF : ");
      m_hss_log->debug_hex(ue_ctx->sqn, 6, "SQN : ");
      ue_ctx->qci = atoi(split[7].c_str());
      m_hss_log->debug("Default Bearer QCI: %d\n",ue_ctx->qci);
      m_imsi_to_ue_ctx.insert(std::pair<uint64_t,hss_ue_ctx_t*>(ue_ctx->imsi,ue_ctx));
    }
  }

  if(m_db_file.is_open())
  {
    m_db_file.close();
  }

  return true;
}

bool hss::write_db_file(std::string db_filename)
{
  std::string line;
  uint8_t k[16];
  uint8_t amf[2];
  uint8_t op[16];
  uint8_t sqn[6];

  std::ofstream m_db_file;

  m_db_file.open(db_filename.c_str(), std::ofstream::out);
  if(!m_db_file.is_open())
  {
    return false;
  }
  m_hss_log->info("Opened DB file: %s\n", db_filename.c_str() );

  //Write comment info
  m_db_file << "#                                                                            " << std::endl
            << "# .csv to store UE's information in HSS                                      " << std::endl
            << "# Kept in the following format: \"Name,IMSI,Key,OP_Type,OP,AMF,SQN,QCI\"     " << std::endl
            << "#                                                                            " << std::endl
            << "# Name:    Human readable name to help distinguish UE's. Ignored by the HSS  " << std::endl
            << "# IMSI:    UE's IMSI value                                                   " << std::endl
            << "# Key:     UE's key, where other keys are derived from. Stored in hexadecimal" << std::endl
            << "# OP_Type: Operator's code type, either OP or OPc                            " << std::endl
            << "# OP/OPc:  Operator Code/Cyphered Operator Code, stored in hexadecimal       " << std::endl
            << "# AMF:     Authentication management field, stored in hexadecimal            " << std::endl
            << "# SQN:     UE's Sequence number for freshness of the authentication          " << std::endl
            << "# QCI:     QoS Class Identifier for the UE's default bearer.                 " << std::endl
            << "#                                                                            " << std::endl
            << "# Note: Lines starting by '#' are ignored and will be overwritten            " << std::endl;

  std::map<uint64_t,hss_ue_ctx_t*>::iterator it = m_imsi_to_ue_ctx.begin();
  while(it!=m_imsi_to_ue_ctx.end())
  {
      m_db_file << it->second->name;
      m_db_file << ",";
      m_db_file << std::setfill('0') << std::setw(15) << it->second->imsi;
      m_db_file << ",";
      m_db_file << hex_string(it->second->key, 16);
      m_db_file << ",";
      if(it->second->op_configured){
        m_db_file << "op,";
        m_db_file << hex_string(it->second->op, 16);
      }
      else{
        m_db_file << "opc,";
        m_db_file << hex_string(it->second->opc, 16);
      }
      m_db_file << ",";
      m_db_file << hex_string(it->second->amf, 2);
      m_db_file << ",";
      m_db_file << hex_string(it->second->sqn, 6);
      m_db_file << ",";
      m_db_file << it->second->qci;
      m_db_file << std::endl;
      it++;
  }
  if(m_db_file.is_open())
  {
    m_db_file.close();
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
  increment_ue_sqn(imsi);
  return ret;

}



bool
hss::gen_auth_info_answer_milenage(uint64_t imsi, uint8_t *k_asme, uint8_t *autn, uint8_t *rand, uint8_t *xres)
{
  uint8_t k[16];
  uint8_t amf[2];
  uint8_t opc[16];
  uint8_t sqn[6];

  uint8_t     ck[16];
  uint8_t     ik[16];
  uint8_t     ak[6];
  uint8_t     mac[8];


  if(!get_k_amf_opc_sqn(imsi, k, amf, opc, sqn))
  {
    return false;
  }
  gen_rand(rand);
  
  security_milenage_f2345( k,
                           opc,
                           rand,
                           xres,
                           ck,
                           ik,
                           ak);

  m_hss_log->debug_hex(k, 16, "User Key : ");
  m_hss_log->debug_hex(opc, 16, "User OPc : ");
  m_hss_log->debug_hex(rand, 16, "User Rand : ");
  m_hss_log->debug_hex(xres, 8, "User XRES: ");
  m_hss_log->debug_hex(ck, 16, "User CK: ");
  m_hss_log->debug_hex(ik, 16, "User IK: ");
  m_hss_log->debug_hex(ak, 6, "User AK: ");

  security_milenage_f1( k,
                        opc,
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

  set_last_rand(imsi, rand);

  return true;
}

bool
hss::gen_auth_info_answer_xor(uint64_t imsi, uint8_t *k_asme, uint8_t *autn, uint8_t *rand, uint8_t *xres)
{
  uint8_t k[16];
  uint8_t amf[2];
  uint8_t opc[16];
  uint8_t sqn[6];

  uint8_t  xdout[16];
  uint8_t  cdout[8];

  uint8_t     ck[16];
  uint8_t     ik[16];
  uint8_t     ak[6];
  uint8_t     mac[8];

  int i = 0;

  if(!get_k_amf_opc_sqn(imsi, k, amf, opc, sqn))
  {
    return false;
  }
  gen_rand(rand);

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
  m_hss_log->debug_hex(opc, 16, "User OPc : ");
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

  set_last_rand(imsi, rand);

  return true;
}

bool
hss::gen_update_loc_answer(uint64_t imsi, uint8_t* qci)
{
  std::map<uint64_t,hss_ue_ctx_t*>::iterator ue_ctx_it = m_imsi_to_ue_ctx.find(imsi);
  if(ue_ctx_it == m_imsi_to_ue_ctx.end())
  {
    m_hss_log->info("User not found. IMSI: %015lu\n",imsi);
    m_hss_log->console("User not found. IMSI: %015lu\n",imsi);
    return false;
  }
  hss_ue_ctx_t *ue_ctx = ue_ctx_it->second;
  m_hss_log->info("Found User %015lu\n",imsi);
  *qci = ue_ctx->qci;
  return true;
}



bool
hss::get_k_amf_opc_sqn(uint64_t imsi, uint8_t *k, uint8_t *amf, uint8_t *opc, uint8_t *sqn)
{

  std::map<uint64_t,hss_ue_ctx_t*>::iterator ue_ctx_it = m_imsi_to_ue_ctx.find(imsi);
  if(ue_ctx_it == m_imsi_to_ue_ctx.end())
  {
    m_hss_log->info("User not found. IMSI: %015lu\n",imsi);
    m_hss_log->console("User not found. IMSI: %015lu\n",imsi);
    return false;
  }
  hss_ue_ctx_t *ue_ctx = ue_ctx_it->second;
  m_hss_log->info("Found User %015lu\n",imsi);
  memcpy(k, ue_ctx->key, 16);
  memcpy(amf, ue_ctx->amf, 2);
  memcpy(opc, ue_ctx->opc, 16);
  memcpy(sqn, ue_ctx->sqn, 6);

  return true;
}

bool
hss::resync_sqn(uint64_t imsi, uint8_t *auts)
{
  bool ret = false;
  switch (m_auth_algo)
  {
  case HSS_ALGO_XOR:
    ret = resync_sqn_xor(imsi, auts);
    break;
  case HSS_ALGO_MILENAGE:
    ret = resync_sqn_milenage(imsi, auts);
    break;
  }
  increment_ue_sqn(imsi);
  return ret;
}

bool 
hss::resync_sqn_xor(uint64_t imsi, uint8_t *auts)
{
  m_hss_log->error("XOR SQN synchronization not supported yet\n");
  m_hss_log->console("XOR SQNs synchronization not supported yet\n");
  return false;
}


bool 
hss::resync_sqn_milenage(uint64_t imsi, uint8_t *auts)
{
  uint8_t last_rand[16];
  uint8_t ak[6];
  uint8_t mac_s[8];
  uint8_t sqn_ms_xor_ak[6];

  uint8_t k[16];
  uint8_t amf[2];
  uint8_t opc[16];
  uint8_t sqn[6];

  if(!get_k_amf_opc_sqn(imsi, k, amf, opc, sqn))
  {
    return false;
  }

  get_last_rand(imsi, last_rand);

  for(int i=0; i<6; i++){
    sqn_ms_xor_ak[i] = auts[i];
  }

  for(int i=0; i<8; i++){
    mac_s[i] = auts[i+6];
  }

  m_hss_log->debug_hex(k, 16, "User Key : ");
  m_hss_log->debug_hex(opc, 16, "User OPc : ");
  m_hss_log->debug_hex(last_rand, 16, "User Last Rand : ");
  m_hss_log->debug_hex(auts, 16, "AUTS : ");
  m_hss_log->debug_hex(sqn_ms_xor_ak, 6, "SQN xor AK : ");
  m_hss_log->debug_hex(mac_s, 8, "MAC : ");

  security_milenage_f5_star(k, opc, last_rand, ak);
  m_hss_log->debug_hex(ak, 6, "Resynch AK : ");

  uint8_t sqn_ms[6];
  for(int i=0; i<6; i++){
    sqn_ms[i] = sqn_ms_xor_ak[i] ^ ak[i];
  }
  m_hss_log->debug_hex(sqn_ms, 6, "SQN MS : ");
  m_hss_log->debug_hex(sqn   , 6, "SQN HE : ");

  m_hss_log->debug_hex(amf, 2, "AMF : ");

  uint8_t mac_s_tmp[8];

  security_milenage_f1_star(k, opc, last_rand, sqn_ms, amf, mac_s_tmp);

  m_hss_log->debug_hex(mac_s_tmp, 8, "MAC calc : ");

  set_sqn(imsi, sqn_ms);

  return true;
}

void
hss::increment_ue_sqn(uint64_t imsi)
{
  hss_ue_ctx_t *ue_ctx = NULL;
  bool ret = get_ue_ctx(imsi, &ue_ctx);
  if(ret == false)
  {
    return;
  }

  increment_sqn(ue_ctx->sqn,ue_ctx->sqn);
  m_hss_log->debug("Incremented SQN (IMSI: %" PRIu64 ")" PRIu64 "\n", imsi);
  m_hss_log->debug_hex(ue_ctx->sqn, 6, "SQN: ");
}

void
hss::increment_sqn(uint8_t *sqn, uint8_t *next_sqn)
{
  // Awkward 48 bit sqn and doing arithmetic 
  uint64_t tmp_sqn = 0;
  uint8_t *p = (uint8_t *)&tmp_sqn;

  for(int i = 0; i < 6; i++) {
    p[5-i] = sqn[i];
  }

  tmp_sqn++;
  for(int i = 0; i < 6; i++){
    next_sqn[i] = p[5-i];
  }
  return;
}

void
hss::set_sqn(uint64_t imsi, uint8_t *sqn)
{
  hss_ue_ctx_t *ue_ctx = NULL;
  bool ret = get_ue_ctx(imsi, &ue_ctx);
  if(ret == false)
  {
    return;
  }
  memcpy(ue_ctx->sqn, sqn, 6);
}

void
hss::set_last_rand(uint64_t imsi, uint8_t *rand)
{
  hss_ue_ctx_t *ue_ctx = NULL;
  bool ret = get_ue_ctx(imsi, &ue_ctx);
  if(ret == false)
  {
    return;
  }
  memcpy(ue_ctx->last_rand, rand, 16);

}

void
hss::get_last_rand(uint64_t imsi, uint8_t *rand)
{
  hss_ue_ctx_t *ue_ctx = NULL;
  bool ret = get_ue_ctx(imsi, &ue_ctx);
  if(ret == false)
  {
    return;
  }
  memcpy(rand, ue_ctx->last_rand, 16);
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

bool hss::get_ue_ctx(uint64_t imsi, hss_ue_ctx_t **ue_ctx)
{
  std::map<uint64_t,hss_ue_ctx_t*>::iterator ue_ctx_it = m_imsi_to_ue_ctx.find(imsi);
  if(ue_ctx_it == m_imsi_to_ue_ctx.end())
  {
    m_hss_log->info("User not found. IMSI: %015lu\n",imsi);
    return false;
  }

  *ue_ctx = ue_ctx_it->second;
  return true;
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


std::string 
hss::hex_string(uint8_t *hex, int size)
{
  std::stringstream ss;

  ss << std::hex << std::setfill('0');
  for(int i=0;i<size;i++) {
    ss << std::setw(2) << static_cast<unsigned>(hex[i]);
  }
  return ss.str();
}
} //namespace srsepc
