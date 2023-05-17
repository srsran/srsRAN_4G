/**
 * Copyright 2013-2023 Software Radio Systems Limited
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

#ifndef SRSRAN_SECURITY_H
#define SRSRAN_SECURITY_H

/******************************************************************************
 * Common security header - wraps ciphering/integrity check algorithms.
 *****************************************************************************/

#include "srsran/common/common.h"
#include "srsran/srslog/srslog.h"

#include <vector>

#define AKA_RAND_LEN 16
#define AKA_AUTN_LEN 16
#define AKA_AUTS_LEN 14
#define RES_MAX_LEN 16
#define MAC_LEN 8
#define IK_LEN 16
#define CK_LEN 16
#define AK_LEN 6
#define SQN_LEN 6

#define KEY_LEN 32
namespace srsran {

typedef enum {
  CIPHERING_ALGORITHM_ID_EEA0 = 0,
  CIPHERING_ALGORITHM_ID_128_EEA1,
  CIPHERING_ALGORITHM_ID_128_EEA2,
  CIPHERING_ALGORITHM_ID_128_EEA3,
  CIPHERING_ALGORITHM_ID_N_ITEMS,
} CIPHERING_ALGORITHM_ID_ENUM;
static const char ciphering_algorithm_id_text[CIPHERING_ALGORITHM_ID_N_ITEMS][20] = {"EEA0",
                                                                                     "128-EEA1",
                                                                                     "128-EEA2",
                                                                                     "128-EEA3"};

typedef enum {
  INTEGRITY_ALGORITHM_ID_EIA0 = 0,
  INTEGRITY_ALGORITHM_ID_128_EIA1,
  INTEGRITY_ALGORITHM_ID_128_EIA2,
  INTEGRITY_ALGORITHM_ID_128_EIA3,
  INTEGRITY_ALGORITHM_ID_N_ITEMS,
} INTEGRITY_ALGORITHM_ID_ENUM;
static const char integrity_algorithm_id_text[INTEGRITY_ALGORITHM_ID_N_ITEMS][20] = {"EIA0",
                                                                                     "128-EIA1",
                                                                                     "128-EIA2",
                                                                                     "128-EIA3"};

typedef enum {
  CIPHERING_ALGORITHM_ID_NR_NEA0 = 0,
  CIPHERING_ALGORITHM_ID_NR_128_NEA1,
  CIPHERING_ALGORITHM_ID_NR_128_NEA2,
  CIPHERING_ALGORITHM_ID_NR_128_NEA3,
  CIPHERING_ALGORITHM_ID_NR_N_ITEMS,
} CIPHERING_ALGORITHM_ID_NR_ENUM;
static const char ciphering_algorithm_id_nr_text[CIPHERING_ALGORITHM_ID_N_ITEMS][20] = {"NEA0",
                                                                                        "128-NEA1",
                                                                                        "128-NEA2",
                                                                                        "128-NEA3"};
typedef enum {
  INTEGRITY_ALGORITHM_ID_NR_NIA0 = 0,
  INTEGRITY_ALGORITHM_ID_NR_128_NIA1,
  INTEGRITY_ALGORITHM_ID_NR_128_NIA2,
  INTEGRITY_ALGORITHM_ID_NR_128_NIA3,
  INTEGRITY_ALGORITHM_ID_NR_N_ITEMS,
} INTEGRITY_ALGORITHM_ID_NR_ENUM;
static const char integrity_algorithm_id_nr_text[INTEGRITY_ALGORITHM_ID_N_ITEMS][20] = {"NIA0",
                                                                                        "128-NIA1",
                                                                                        "128-NIA2",
                                                                                        "128-NIA3"};
typedef enum {
  SECURITY_DIRECTION_UPLINK   = 0,
  SECURITY_DIRECTION_DOWNLINK = 1,
  SECURITY_DIRECTION_N_ITEMS,
} security_direction_t;
static const char security_direction_text[INTEGRITY_ALGORITHM_ID_N_ITEMS][20] = {"Uplink", "Downlink"};

using as_key_t = std::array<uint8_t, 32>;
struct k_enb_context_t {
  as_key_t k_enb;
  as_key_t nh;
  bool     is_first_ncc;
  uint32_t ncc;
};

struct k_gnb_context_t {
  as_key_t k_gnb;
  as_key_t sk_gnb;
};

struct as_security_config_t {
  as_key_t                    k_rrc_int;
  as_key_t                    k_rrc_enc;
  as_key_t                    k_up_int;
  as_key_t                    k_up_enc;
  INTEGRITY_ALGORITHM_ID_ENUM integ_algo;
  CIPHERING_ALGORITHM_ID_ENUM cipher_algo;
};

struct nr_as_security_config_t {
  as_key_t                       k_nr_rrc_int;
  as_key_t                       k_nr_rrc_enc;
  as_key_t                       k_nr_up_int;
  as_key_t                       k_nr_up_enc;
  INTEGRITY_ALGORITHM_ID_NR_ENUM integ_algo;
  CIPHERING_ALGORITHM_ID_NR_ENUM cipher_algo;
};

template <typename... Args>
void log_error(const char* format, Args&&... args)
{
  srslog::fetch_basic_logger("SEC").error(format, std::forward<Args>(args)...);
}

template <typename... Args>
void log_warning(const char* format, Args&&... args)
{
  srslog::fetch_basic_logger("SEC").warning(format, std::forward<Args>(args)...);
}

template <typename... Args>
void log_info(const char* format, Args&&... args)
{
  srslog::fetch_basic_logger("SEC").info(format, std::forward<Args>(args)...);
}

template <typename... Args>
void log_debug(const char* format, Args&&... args)
{
  srslog::fetch_basic_logger("SEC").debug(format, std::forward<Args>(args)...);
}

/******************************************************************************
 * Key Generation
 *****************************************************************************/

int kdf_common(const uint8_t fc, const std::array<uint8_t, 32>& key, const std::vector<uint8_t>& P, uint8_t* output);
int kdf_common(const uint8_t                  fc,
               const std::array<uint8_t, 32>& key,
               const std::vector<uint8_t>&    P0,
               const std::vector<uint8_t>&    P1,
               uint8_t*                       output);
int kdf_common(const uint8_t                  fc,
               const std::array<uint8_t, 32>& key,
               const std::vector<uint8_t>&    P0,
               const std::vector<uint8_t>&    P1,
               const std::vector<uint8_t>&    P3,
               uint8_t*                       output);

uint8_t security_generate_k_asme(const uint8_t* ck,
                                 const uint8_t* ik,
                                 const uint8_t* ak_xor_sqn,
                                 const uint16_t mcc,
                                 const uint16_t mnc,
                                 uint8_t*       k_asme);

uint8_t security_generate_k_ausf(const uint8_t* ck,
                                 const uint8_t* ik,
                                 const uint8_t* ak_xor_sqn,
                                 const char*    serving_network_name,
                                 uint8_t*       k_ausf);

uint8_t security_generate_k_amf(const uint8_t* k_seaf,
                                const char*    supi_,
                                const uint8_t* abba_,
                                const uint32_t abba_len,
                                uint8_t*       k_amf);

uint8_t security_generate_k_seaf(const uint8_t* k_ausf, const char* serving_network_name, uint8_t* k_seaf);

uint8_t security_generate_k_gnb(const as_key_t& k_amf, const uint32_t nas_count, as_key_t& k_gnb);

uint8_t security_generate_k_enb(const uint8_t* k_asme, const uint32_t nas_count, uint8_t* k_enb);

uint8_t security_generate_k_nb_star_common(uint8_t        fc,
                                           const uint8_t* k_enb,
                                           const uint32_t pci_,
                                           const uint32_t earfcn_,
                                           uint8_t*       k_enb_star);

uint8_t
security_generate_k_enb_star(const uint8_t* k_enb, const uint32_t pci, const uint32_t earfcn, uint8_t* k_enb_star);

uint8_t
security_generate_k_gnb_star(const uint8_t* k_gnb, const uint32_t pci_, const uint32_t dl_arfcn_, uint8_t* k_gnb_star);

uint8_t security_generate_nh(const uint8_t* k_asme, const uint8_t* sync, uint8_t* nh);

uint8_t security_generate_k_nas(const uint8_t*                    k_asme,
                                const CIPHERING_ALGORITHM_ID_ENUM enc_alg_id,
                                const INTEGRITY_ALGORITHM_ID_ENUM int_alg_id,
                                uint8_t*                          k_nas_enc,
                                uint8_t*                          k_nas_int);

uint8_t security_generate_k_nas_5g(const uint8_t*                    k_amf,
                                   const CIPHERING_ALGORITHM_ID_ENUM enc_alg_id,
                                   const INTEGRITY_ALGORITHM_ID_ENUM int_alg_id,
                                   uint8_t*                          k_nas_enc,
                                   uint8_t*                          k_nas_int);

uint8_t security_generate_k_rrc(const uint8_t*                    k_enb,
                                const CIPHERING_ALGORITHM_ID_ENUM enc_alg_id,
                                const INTEGRITY_ALGORITHM_ID_ENUM int_alg_id,
                                uint8_t*                          k_rrc_enc,
                                uint8_t*                          k_rrc_int);

uint8_t security_generate_k_up(const uint8_t*                    k_enb,
                               const CIPHERING_ALGORITHM_ID_ENUM enc_alg_id,
                               const INTEGRITY_ALGORITHM_ID_ENUM int_alg_id,
                               uint8_t*                          k_up_enc,
                               uint8_t*                          k_up_int);

uint8_t security_generate_k_nr_rrc(const uint8_t*                    k_gnb,
                                   const CIPHERING_ALGORITHM_ID_ENUM enc_alg_id,
                                   const INTEGRITY_ALGORITHM_ID_ENUM int_alg_id,
                                   uint8_t*                          k_rrc_enc,
                                   uint8_t*                          k_rrc_int);

uint8_t security_generate_k_nr_up(const uint8_t*                    k_gnb,
                                  const CIPHERING_ALGORITHM_ID_ENUM enc_alg_id,
                                  const INTEGRITY_ALGORITHM_ID_ENUM int_alg_id,
                                  uint8_t*                          k_up_enc,
                                  uint8_t*                          k_up_int);

uint8_t security_generate_sk_gnb(const uint8_t* k_enb, const uint16_t scg_count, uint8_t* sk_gnb);

uint8_t security_generate_res_star(const uint8_t* ck,
                                   const uint8_t* ik,
                                   const char*    serving_network_name,
                                   const uint8_t* rand,
                                   const uint8_t* res,
                                   const size_t   res_len,
                                   uint8_t*       res_star);
/******************************************************************************
 * Integrity Protection
 *****************************************************************************/
uint8_t security_128_eia1(const uint8_t* key,
                          uint32_t       count,
                          uint32_t       bearer,
                          uint8_t        direction,
                          uint8_t*       msg,
                          uint32_t       msg_len,
                          uint8_t*       mac);

uint8_t security_128_eia2(const uint8_t* key,
                          uint32_t       count,
                          uint32_t       bearer,
                          uint8_t        direction,
                          uint8_t*       msg,
                          uint32_t       msg_len,
                          uint8_t*       mac);

uint8_t security_128_eia3(const uint8_t* key,
                          uint32_t       count,
                          uint32_t       bearer,
                          uint8_t        direction,
                          uint8_t*       msg,
                          uint32_t       msg_len,
                          uint8_t*       mac);

uint8_t security_md5(const uint8_t* input, size_t len, uint8_t* output);

/******************************************************************************
 * Encryption / Decryption
 *****************************************************************************/
uint8_t security_128_eea1(uint8_t* key,
                          uint32_t count,
                          uint8_t  bearer,
                          uint8_t  direction,
                          uint8_t* msg,
                          uint32_t msg_len,
                          uint8_t* msg_out);

uint8_t security_128_eea2(uint8_t* key,
                          uint32_t count,
                          uint8_t  bearer,
                          uint8_t  direction,
                          uint8_t* msg,
                          uint32_t msg_len,
                          uint8_t* msg_out);

uint8_t security_128_eea3(uint8_t* key,
                          uint32_t count,
                          uint8_t  bearer,
                          uint8_t  direction,
                          uint8_t* msg,
                          uint32_t msg_len,
                          uint8_t* msg_out);

/******************************************************************************
 * Authentication
 *****************************************************************************/
uint8_t compute_opc(uint8_t* k, uint8_t* op, uint8_t* opc);

uint8_t security_milenage_f1(uint8_t* k, uint8_t* op, uint8_t* rand, uint8_t* sqn, uint8_t* amf, uint8_t* mac_a);

uint8_t security_milenage_f1_star(uint8_t* k, uint8_t* op, uint8_t* rand, uint8_t* sqn, uint8_t* amf, uint8_t* mac_s);

uint8_t
security_milenage_f2345(uint8_t* k, uint8_t* op, uint8_t* rand, uint8_t* res, uint8_t* ck, uint8_t* ik, uint8_t* ak);

uint8_t security_milenage_f5_star(uint8_t* k, uint8_t* op, uint8_t* rand, uint8_t* ak);

int security_xor_f2345(uint8_t* k, uint8_t* rand, uint8_t* res, uint8_t* ck, uint8_t* ik, uint8_t* ak);
int security_xor_f1(uint8_t* k, uint8_t* rand, uint8_t* sqn, uint8_t* amf, uint8_t* mac_a);

} // namespace srsran
#endif // SRSRAN_SECURITY_H
