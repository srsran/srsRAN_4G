/*
 * Copyright 2013-2020 Software Radio Systems Limited
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
#include "srsepc/hdr/hss/hss.h"
#include "srslte/common/security.h"
#include <inttypes.h> // for printing uint64_t
#include <iomanip>
#include <sstream>
#include <stdlib.h> /* srand, rand */
#include <string>
#include <time.h>

namespace srsepc {

hss*            hss::m_instance    = NULL;
pthread_mutex_t hss_instance_mutex = PTHREAD_MUTEX_INITIALIZER;

hss::hss()
{
  return;
}

hss::~hss()
{
  return;
}

hss* hss::get_instance()
{
  pthread_mutex_lock(&hss_instance_mutex);
  if (NULL == m_instance) {
    m_instance = new hss();
  }
  pthread_mutex_unlock(&hss_instance_mutex);
  return (m_instance);
}

void hss::cleanup()
{
  pthread_mutex_lock(&hss_instance_mutex);
  if (NULL != m_instance) {
    delete m_instance;
    m_instance = NULL;
  }
  pthread_mutex_unlock(&hss_instance_mutex);
}

int hss::init(hss_args_t* hss_args, srslte::log_filter* hss_log)
{
  srand(time(NULL));
  /*Init loggers*/
  m_hss_log = hss_log;

  /*Read user information from DB*/
  if (read_db_file(hss_args->db_file) == false) {
    srslte::console("Error reading user database file %s\n", hss_args->db_file.c_str());
    return -1;
  }

  mcc = hss_args->mcc;
  mnc = hss_args->mnc;

  db_file = hss_args->db_file;

  m_hss_log->info("HSS Initialized. DB file %s, MCC: %d, MNC: %d\n", hss_args->db_file.c_str(), mcc, mnc);
  srslte::console("HSS Initialized.\n");
  return 0;
}

void hss::stop()
{
  write_db_file(db_file);
  return;
}

bool hss::read_db_file(std::string db_filename)
{
  std::ifstream m_db_file;

  m_db_file.open(db_filename.c_str(), std::ifstream::in);
  if (!m_db_file.is_open()) {
    return false;
  }
  m_hss_log->info("Opened DB file: %s\n", db_filename.c_str());

  std::string line;
  while (std::getline(m_db_file, line)) {
    if (line[0] != '#' && line.length() > 0) {
      uint                     column_size = 10;
      std::vector<std::string> split       = split_string(line, ',');
      if (split.size() != column_size) {
        m_hss_log->error("Error parsing UE database. Wrong number of columns in .csv\n");
        m_hss_log->error("Columns: %zd, Expected %d.\n", split.size(), column_size);

        srslte::console("\nError parsing UE database. Wrong number of columns in user database CSV.\n");
        srslte::console("Perhaps you are using an old user_db.csv?\n");
        srslte::console("See 'srsepc/user_db.csv.example' for an example.\n\n");
        return false;
      }
      std::unique_ptr<hss_ue_ctx_t> ue_ctx = std::unique_ptr<hss_ue_ctx_t>(new hss_ue_ctx_t);
      ue_ctx->name                         = split[0];
      if (split[1] == std::string("xor")) {
        ue_ctx->algo = HSS_ALGO_XOR;
      } else if (split[1] == std::string("mil")) {
        ue_ctx->algo = HSS_ALGO_MILENAGE;
      } else {
        m_hss_log->error("Neither XOR nor MILENAGE configured.\n");
        return false;
      }
      ue_ctx->imsi = strtoull(split[2].c_str(), nullptr, 10);
      get_uint_vec_from_hex_str(split[3], ue_ctx->key, 16);
      if (split[4] == std::string("op")) {
        ue_ctx->op_configured = true;
        get_uint_vec_from_hex_str(split[5], ue_ctx->op, 16);
        srslte::compute_opc(ue_ctx->key, ue_ctx->op, ue_ctx->opc);
      } else if (split[4] == std::string("opc")) {
        ue_ctx->op_configured = false;
        get_uint_vec_from_hex_str(split[5], ue_ctx->opc, 16);
      } else {
        m_hss_log->error("Neither OP nor OPc configured.\n");
        return false;
      }
      get_uint_vec_from_hex_str(split[6], ue_ctx->amf, 2);
      get_uint_vec_from_hex_str(split[7], ue_ctx->sqn, 6);

      m_hss_log->debug("Added user from DB, IMSI: %015" PRIu64 "\n", ue_ctx->imsi);
      m_hss_log->debug_hex(ue_ctx->key, 16, "User Key : ");
      if (ue_ctx->op_configured) {
        m_hss_log->debug_hex(ue_ctx->op, 16, "User OP : ");
      }
      m_hss_log->debug_hex(ue_ctx->opc, 16, "User OPc : ");
      m_hss_log->debug_hex(ue_ctx->amf, 2, "AMF : ");
      m_hss_log->debug_hex(ue_ctx->sqn, 6, "SQN : ");
      ue_ctx->qci = (uint16_t)strtol(split[8].c_str(), nullptr, 10);
      m_hss_log->debug("Default Bearer QCI: %d\n", ue_ctx->qci);

      if (split[9] == std::string("dynamic")) {
        ue_ctx->static_ip_addr = "0.0.0.0";
      } else {
        char buf[128] = {0};
        if (inet_pton(AF_INET, split[9].c_str(), buf)) {
          if (m_ip_to_imsi.insert(std::make_pair(split[9], ue_ctx->imsi)).second) {
            ue_ctx->static_ip_addr = split[9];
            m_hss_log->info("static ip addr %s\n", ue_ctx->static_ip_addr.c_str());
          } else {
            m_hss_log->info("duplicate static ip addr %s\n", split[9].c_str());
            return false;
          }
        } else {
          m_hss_log->info("invalid static ip addr %s, %s\n", split[9].c_str(), strerror(errno));
          return false;
        }
      }
      m_imsi_to_ue_ctx.insert(std::make_pair(ue_ctx->imsi, std::move(ue_ctx)));
    }
  }

  if (m_db_file.is_open()) {
    m_db_file.close();
  }

  return true;
}

bool hss::write_db_file(std::string db_filename)
{
  std::string line;
  uint8_t     k[16];
  uint8_t     amf[2];
  uint8_t     op[16];
  uint8_t     sqn[6];

  std::ofstream m_db_file;

  m_db_file.open(db_filename.c_str(), std::ofstream::out);
  if (!m_db_file.is_open()) {
    return false;
  }
  m_hss_log->info("Opened DB file: %s\n", db_filename.c_str());

  // Write comment info
  m_db_file << "#                                                                                           \n"
            << "# .csv to store UE's information in HSS                                                     \n"
            << "# Kept in the following format: \"Name,Auth,IMSI,Key,OP_Type,OP/OPc,AMF,SQN,QCI,IP_alloc\"  \n"
            << "#                                                                                           \n"
            << "# Name:     Human readable name to help distinguish UE's. Ignored by the HSS                \n"
            << "# Auth:     Authentication algorithm used by the UE. Valid algorithms are XOR               \n"
            << "#           (xor) and MILENAGE (mil)                                                        \n"
            << "# IMSI:     UE's IMSI value                                                                 \n"
            << "# Key:      UE's key, where other keys are derived from. Stored in hexadecimal              \n"
            << "# OP_Type:  Operator's code type, either OP or OPc                                          \n"
            << "# OP/OPc:   Operator Code/Cyphered Operator Code, stored in hexadecimal                     \n"
            << "# AMF:      Authentication management field, stored in hexadecimal                          \n"
            << "# SQN:      UE's Sequence number for freshness of the authentication                        \n"
            << "# QCI:      QoS Class Identifier for the UE's default bearer.                               \n"
            << "# IP_alloc: IP allocation stratagy for the SPGW.                                            \n"
            << "#           With 'dynamic' the SPGW will automatically allocate IPs                         \n"
            << "#           With a valid IPv4 (e.g. '172.16.0.2') the UE will have a statically assigned IP.\n"
            << "#                                                                                           \n"
            << "# Note: Lines starting by '#' are ignored and will be overwritten                           \n";

  std::map<uint64_t, std::unique_ptr<hss_ue_ctx_t> >::iterator it = m_imsi_to_ue_ctx.begin();
  while (it != m_imsi_to_ue_ctx.end()) {
    m_db_file << it->second->name;
    m_db_file << ",";
    m_db_file << (it->second->algo == HSS_ALGO_XOR ? "xor" : "mil");
    m_db_file << ",";
    m_db_file << std::setfill('0') << std::setw(15) << it->second->imsi;
    m_db_file << ",";
    m_db_file << hex_string(it->second->key, 16);
    m_db_file << ",";
    if (it->second->op_configured) {
      m_db_file << "op,";
      m_db_file << hex_string(it->second->op, 16);
    } else {
      m_db_file << "opc,";
      m_db_file << hex_string(it->second->opc, 16);
    }
    m_db_file << ",";
    m_db_file << hex_string(it->second->amf, 2);
    m_db_file << ",";
    m_db_file << hex_string(it->second->sqn, 6);
    m_db_file << ",";
    m_db_file << it->second->qci;
    if (it->second->static_ip_addr != "0.0.0.0") {
      m_db_file << ",";
      m_db_file << it->second->static_ip_addr;
    } else {
      m_db_file << ",dynamic";
    }
    m_db_file << std::endl;
    it++;
  }
  if (m_db_file.is_open()) {
    m_db_file.close();
  }
  return true;
}

bool hss::gen_auth_info_answer(uint64_t imsi, uint8_t* k_asme, uint8_t* autn, uint8_t* rand, uint8_t* xres)
{

  m_hss_log->debug("Generating AUTH info answer\n");
  hss_ue_ctx_t* ue_ctx = get_ue_ctx(imsi);
  if (ue_ctx == nullptr) {
    srslte::console("User not found at HSS. IMSI: %015" PRIu64 "\n", imsi);
    m_hss_log->error("User not found at HSS. IMSI: %015" PRIu64 "\n", imsi);
    return false;
  }

  switch (ue_ctx->algo) {
    case HSS_ALGO_XOR:
      gen_auth_info_answer_xor(ue_ctx, k_asme, autn, rand, xres);
      break;
    case HSS_ALGO_MILENAGE:
      gen_auth_info_answer_milenage(ue_ctx, k_asme, autn, rand, xres);
      break;
  }
  increment_ue_sqn(ue_ctx);
  return true;
}

void hss::gen_auth_info_answer_milenage(hss_ue_ctx_t* ue_ctx,
                                        uint8_t*      k_asme,
                                        uint8_t*      autn,
                                        uint8_t*      rand,
                                        uint8_t*      xres)
{
  // Get K, AMF, OPC and SQN
  uint8_t* k   = ue_ctx->key;
  uint8_t* amf = ue_ctx->amf;
  uint8_t* opc = ue_ctx->opc;
  uint8_t* sqn = ue_ctx->sqn;

  // Temp variables
  uint8_t ck[16];
  uint8_t ik[16];
  uint8_t ak[6];
  uint8_t mac[8];

  gen_rand(rand);

  srslte::security_milenage_f2345(k, opc, rand, xres, ck, ik, ak);

  m_hss_log->debug_hex(k, 16, "User Key : ");
  m_hss_log->debug_hex(opc, 16, "User OPc : ");
  m_hss_log->debug_hex(rand, 16, "User Rand : ");
  m_hss_log->debug_hex(xres, 8, "User XRES: ");
  m_hss_log->debug_hex(ck, 16, "User CK: ");
  m_hss_log->debug_hex(ik, 16, "User IK: ");
  m_hss_log->debug_hex(ak, 6, "User AK: ");

  srslte::security_milenage_f1(k, opc, rand, sqn, amf, mac);

  m_hss_log->debug_hex(sqn, 6, "User SQN : ");
  m_hss_log->debug_hex(mac, 8, "User MAC : ");

  // Generate K_asme
  srslte::security_generate_k_asme(ck, ik, ak, sqn, mcc, mnc, k_asme);

  m_hss_log->debug("User MCC : %x  MNC : %x \n", mcc, mnc);
  m_hss_log->debug_hex(k_asme, 32, "User k_asme : ");

  // Generate AUTN (autn = sqn ^ ak |+| amf |+| mac)
  for (int i = 0; i < 6; i++) {
    autn[i] = sqn[i] ^ ak[i];
  }
  for (int i = 0; i < 2; i++) {
    autn[6 + i] = amf[i];
  }
  for (int i = 0; i < 8; i++) {
    autn[8 + i] = mac[i];
  }
  m_hss_log->debug_hex(autn, 16, "User AUTN: ");

  // Set last RAND
  ue_ctx->set_last_rand(rand);
  return;
}

void hss::gen_auth_info_answer_xor(hss_ue_ctx_t* ue_ctx, uint8_t* k_asme, uint8_t* autn, uint8_t* rand, uint8_t* xres)
{
  // Get K, AMF, OPC and SQN
  uint8_t* k   = ue_ctx->key;
  uint8_t* amf = ue_ctx->amf;
  uint8_t* opc = ue_ctx->opc;
  uint8_t* sqn = ue_ctx->sqn;

  // Temp variables
  uint8_t xdout[16];
  uint8_t cdout[8];

  uint8_t ck[16];
  uint8_t ik[16];
  uint8_t ak[6];
  uint8_t mac[8];

  int i = 0;

  // Gen RAND
  gen_rand(rand);

  // Use RAND and K to compute RES, CK, IK and AK
  for (i = 0; i < 16; i++) {
    xdout[i] = k[i] ^ rand[i];
  }

  for (i = 0; i < 16; i++) {
    xres[i] = xdout[i];
    ck[i]   = xdout[(i + 1) % 16];
    ik[i]   = xdout[(i + 2) % 16];
  }
  for (i = 0; i < 6; i++) {
    ak[i] = xdout[i + 3];
  }

  m_hss_log->debug_hex(k, 16, "User Key : ");
  m_hss_log->debug_hex(opc, 16, "User OPc : ");
  m_hss_log->debug_hex(rand, 16, "User Rand : ");
  m_hss_log->debug_hex(xres, 8, "User XRES: ");
  m_hss_log->debug_hex(ck, 16, "User CK: ");
  m_hss_log->debug_hex(ik, 16, "User IK: ");
  m_hss_log->debug_hex(ak, 6, "User AK: ");

  // Generate cdout
  for (i = 0; i < 6; i++) {
    cdout[i] = sqn[i];
  }
  for (i = 0; i < 2; i++) {
    cdout[6 + i] = amf[i];
  }

  // Generate MAC
  for (i = 0; i < 8; i++) {
    mac[i] = xdout[i] ^ cdout[i];
  }

  m_hss_log->debug_hex(sqn, 6, "User SQN : ");
  m_hss_log->debug_hex(mac, 8, "User MAC : ");

  // Generate AUTN (autn = sqn ^ ak |+| amf |+| mac)
  for (int i = 0; i < 6; i++) {
    autn[i] = sqn[i] ^ ak[i];
  }
  for (int i = 0; i < 2; i++) {
    autn[6 + i] = amf[i];
  }
  for (int i = 0; i < 8; i++) {
    autn[8 + i] = mac[i];
  }

  // Generate K_asme
  srslte::security_generate_k_asme(ck, ik, ak, sqn, mcc, mnc, k_asme);

  m_hss_log->debug("User MCC : %x  MNC : %x \n", mcc, mnc);
  m_hss_log->debug_hex(k_asme, 32, "User k_asme : ");

  // Generate AUTN (autn = sqn ^ ak |+| amf |+| mac)
  for (int i = 0; i < 6; i++) {
    autn[i] = sqn[i] ^ ak[i];
  }
  for (int i = 0; i < 2; i++) {
    autn[6 + i] = amf[i];
  }
  for (int i = 0; i < 8; i++) {
    autn[8 + i] = mac[i];
  }

  m_hss_log->debug_hex(autn, 8, "User AUTN: ");

  // Set last RAND
  ue_ctx->set_last_rand(rand);
  return;
}

bool hss::gen_update_loc_answer(uint64_t imsi, uint8_t* qci)
{
  std::map<uint64_t, std::unique_ptr<hss_ue_ctx_t> >::iterator ue_ctx_it = m_imsi_to_ue_ctx.find(imsi);
  if (ue_ctx_it == m_imsi_to_ue_ctx.end()) {
    m_hss_log->info("User not found. IMSI: %015" PRIu64 "\n", imsi);
    srslte::console("User not found at HSS. IMSI: %015" PRIu64 "\n", imsi);
    return false;
  }
  const std::unique_ptr<hss_ue_ctx_t>& ue_ctx = ue_ctx_it->second;
  m_hss_log->info("Found User %015" PRIu64 "\n", imsi);
  *qci = ue_ctx->qci;
  return true;
}

bool hss::resync_sqn(uint64_t imsi, uint8_t* auts)
{
  m_hss_log->debug("Re-syncing SQN\n");
  hss_ue_ctx_t* ue_ctx = get_ue_ctx(imsi);
  if (ue_ctx == nullptr) {
    srslte::console("User not found at HSS. IMSI: %015" PRIu64 "\n", imsi);
    m_hss_log->error("User not found at HSS. IMSI: %015" PRIu64 "\n", imsi);
    return false;
  }

  switch (ue_ctx->algo) {
    case HSS_ALGO_XOR:
      resync_sqn_xor(ue_ctx, auts);
      break;
    case HSS_ALGO_MILENAGE:
      resync_sqn_milenage(ue_ctx, auts);
      break;
  }

  increment_seq_after_resync(ue_ctx);
  return true;
}

void hss::resync_sqn_xor(hss_ue_ctx_t* ue_ctx, uint8_t* auts)
{
  m_hss_log->error("XOR SQN synchronization not supported yet\n");
  srslte::console("XOR SQNs synchronization not supported yet\n");
  return;
}

void hss::resync_sqn_milenage(hss_ue_ctx_t* ue_ctx, uint8_t* auts)
{
  // Get K, AMF, OPC and SQN
  uint8_t* k   = ue_ctx->key;
  uint8_t* amf = ue_ctx->amf;
  uint8_t* opc = ue_ctx->opc;
  uint8_t* sqn = ue_ctx->sqn;

  // Temp variables
  uint8_t last_rand[16];
  uint8_t ak[6];
  uint8_t mac_s[8];
  uint8_t sqn_ms_xor_ak[6];

  ue_ctx->get_last_rand(last_rand);

  for (int i = 0; i < 6; i++) {
    sqn_ms_xor_ak[i] = auts[i];
  }

  for (int i = 0; i < 8; i++) {
    mac_s[i] = auts[i + 6];
  }

  m_hss_log->debug_hex(k, 16, "User Key : ");
  m_hss_log->debug_hex(opc, 16, "User OPc : ");
  m_hss_log->debug_hex(amf, 2, "User AMF : ");
  m_hss_log->debug_hex(last_rand, 16, "User Last Rand : ");
  m_hss_log->debug_hex(auts, 16, "AUTS : ");
  m_hss_log->debug_hex(sqn_ms_xor_ak, 6, "SQN xor AK : ");
  m_hss_log->debug_hex(mac_s, 8, "MAC : ");

  srslte::security_milenage_f5_star(k, opc, last_rand, ak);
  m_hss_log->debug_hex(ak, 6, "Resynch AK : ");

  uint8_t sqn_ms[6];
  for (int i = 0; i < 6; i++) {
    sqn_ms[i] = sqn_ms_xor_ak[i] ^ ak[i];
  }
  m_hss_log->debug_hex(sqn_ms, 6, "SQN MS : ");
  m_hss_log->debug_hex(sqn, 6, "SQN HE : ");

  uint8_t mac_s_tmp[8];

  uint8_t dummy_amf[2] = {};

  srslte::security_milenage_f1_star(k, opc, last_rand, sqn_ms, dummy_amf, mac_s_tmp);
  m_hss_log->debug_hex(mac_s_tmp, 8, "MAC calc : ");

  ue_ctx->set_sqn(sqn_ms);
  return;
}

void hss::increment_ue_sqn(hss_ue_ctx_t* ue_ctx)
{
  increment_sqn(ue_ctx->sqn, ue_ctx->sqn);
  m_hss_log->debug("Incremented SQN  -- IMSI: %015" PRIu64 "\n", ue_ctx->imsi);
  m_hss_log->debug_hex(ue_ctx->sqn, 6, "SQN: ");
}

void hss::increment_sqn(uint8_t* sqn, uint8_t* next_sqn)
{
  // The following SQN incrementation function is implemented according to 3GPP TS 33.102 version 11.5.1 Annex C
  uint64_t seq;
  uint64_t ind;
  uint64_t sqn64;

  sqn64 = 0;

  for (int i = 0; i < 6; i++) {
    sqn64 |= (uint64_t)sqn[i] << (5 - i) * 8;
  }

  seq = sqn64 >> LTE_FDD_ENB_IND_HE_N_BITS;
  ind = sqn64 & LTE_FDD_ENB_IND_HE_MASK;

  uint64_t nextseq;
  uint64_t nextind;
  uint64_t nextsqn;

  nextseq = (seq + 1) % LTE_FDD_ENB_SEQ_HE_MAX_VALUE;
  nextind = (ind + 1) % LTE_FDD_ENB_IND_HE_MAX_VALUE;
  nextsqn = (nextseq << LTE_FDD_ENB_IND_HE_N_BITS) | nextind;

  for (int i = 0; i < 6; i++) {
    next_sqn[i] = (nextsqn >> (5 - i) * 8) & 0xFF;
  }
  return;
}

void hss::increment_seq_after_resync(hss_ue_ctx_t* ue_ctx)
{
  // This function only increment the SEQ part of the SQN for resynchronization purpose
  uint8_t* sqn = ue_ctx->sqn;

  uint64_t seq;
  uint64_t ind;
  uint64_t sqn64;

  sqn64 = 0;

  for (int i = 0; i < 6; i++) {
    sqn64 |= (uint64_t)sqn[i] << (5 - i) * 8;
  }

  seq = sqn64 >> LTE_FDD_ENB_IND_HE_N_BITS;
  ind = sqn64 & LTE_FDD_ENB_IND_HE_MASK;

  uint64_t nextseq;
  uint64_t nextsqn;

  nextseq = (seq + 1) % LTE_FDD_ENB_SEQ_HE_MAX_VALUE;
  nextsqn = (nextseq << LTE_FDD_ENB_IND_HE_N_BITS) | ind;

  for (int i = 0; i < 6; i++) {
    sqn[i] = (nextsqn >> (5 - i) * 8) & 0xFF;
  }
  return;
}

void hss::gen_rand(uint8_t rand_[16])
{
  for (int i = 0; i < 16; i++) {
    rand_[i] = rand() % 256; // Pulls on byte at a time. It's slow, but does not depend on RAND_MAX.
  }
  return;
}

hss_ue_ctx_t* hss::get_ue_ctx(uint64_t imsi)
{
  std::map<uint64_t, std::unique_ptr<hss_ue_ctx_t> >::iterator ue_ctx_it = m_imsi_to_ue_ctx.find(imsi);
  if (ue_ctx_it == m_imsi_to_ue_ctx.end()) {
    m_hss_log->info("User not found. IMSI: %015" PRIu64 "\n", imsi);
    return nullptr;
  }

  return ue_ctx_it->second.get();
}

/* Helper functions*/
std::vector<std::string> hss::split_string(const std::string& str, char delimiter)
{
  std::vector<std::string> tokens;
  std::string              token;
  std::istringstream       tokenStream(str);

  while (std::getline(tokenStream, token, delimiter)) {
    tokens.push_back(token);
  }
  return tokens;
}

void hss::get_uint_vec_from_hex_str(const std::string& key_str, uint8_t* key, uint len)
{
  const char* pos = key_str.c_str();

  for (uint count = 0; count < len; count++) {
    sscanf(pos, "%2hhx", &key[count]);
    pos += 2;
  }
  return;
}

std::string hss::hex_string(uint8_t* hex, int size)
{
  std::stringstream ss;

  ss << std::hex << std::setfill('0');
  for (int i = 0; i < size; i++) {
    ss << std::setw(2) << static_cast<unsigned>(hex[i]);
  }
  return ss.str();
}

std::map<std::string, uint64_t> hss::get_ip_to_imsi(void) const
{
  return m_ip_to_imsi;
}

} // namespace srsepc
