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

#include "srsue/hdr/stack/upper/usim_base.h"
#include "srsran/common/bcd_helpers.h"
#include "srsue/hdr/stack/upper/usim.h"

#ifdef HAVE_PCSC
#include "srsue/hdr/stack/upper/pcsc_usim.h"
#endif

namespace srsue {

std::unique_ptr<usim_base> usim_base::get_instance(usim_args_t* args, srslog::basic_logger& logger)
{
#if HAVE_PCSC
  if (args->mode == "pcsc") {
    return std::unique_ptr<usim_base>(new pcsc_usim(logger));
  }
#endif

  // default to soft USIM
  return std::unique_ptr<usim_base>(new usim(logger));
}

usim_base::usim_base(srslog::basic_logger& logger) : logger(logger) {}

usim_base::~usim_base() {}

/*
 * NAS Interface
 */
std::string usim_base::get_imsi_str()
{
  return imsi_str;
}
std::string usim_base::get_imei_str()
{
  return imei_str;
}

bool usim_base::get_imsi_vec(uint8_t* imsi_, uint32_t n)
{
  if (!initiated) {
    logger.error("USIM not initiated!");
    return false;
  }

  if (NULL == imsi_ || n < 15) {
    logger.error("Invalid parameters to get_imsi_vec");
    return false;
  }

  uint64_t temp = imsi;
  for (int i = 14; i >= 0; i--) {
    imsi_[i] = temp % 10;
    temp /= 10;
  }
  return true;
}

bool usim_base::get_imei_vec(uint8_t* imei_, uint32_t n)
{
  if (!initiated) {
    logger.error("USIM not initiated!");
    return false;
  }

  if (NULL == imei_ || n < 15) {
    logger.error("Invalid parameters to get_imei_vec");
    return false;
  }

  uint64_t temp = imei;
  for (int i = 14; i >= 0; i--) {
    imei_[i] = temp % 10;
    temp /= 10;
  }
  return true;
}

std::string usim_base::get_mcc_str(const uint8_t* imsi_vec)
{
  std::ostringstream mcc_oss;
  uint32_t           mcc_len = 3;
  for (uint32_t i = 0; i < mcc_len; i++) {
    mcc_oss << (int)imsi_vec[i];
  }
  return mcc_oss.str();
}

bool usim_base::get_home_plmn_id(srsran::plmn_id_t* home_plmn_id)
{
  if (!initiated) {
    logger.error("USIM not initiated!");
    return false;
  }

  uint8_t imsi_vec[15];
  get_imsi_vec(imsi_vec, 15);

  std::string mcc_str = get_mcc_str(imsi_vec);
  std::string mnc_str = get_mnc_str(imsi_vec, mcc_str);

  std::string plmn_str = mcc_str + mnc_str;
  if (home_plmn_id->from_string(plmn_str)) {
    logger.error("Error reading home PLMN from SIM.");
    return false;
  }

  logger.info("Read Home PLMN Id=%s", home_plmn_id->to_string().c_str());

  return true;
}

bool usim_base::get_home_mcc_bytes(uint8_t* mcc_, uint32_t n)
{
  if (!initiated) {
    logger.error("USIM not initiated!");
    return false;
  }

  if (NULL == mcc_ || n < 3) {
    logger.error("Invalid parameters to get_home_mcc_bytes");
    return false;
  }

  uint8_t imsi_vec[15];
  get_imsi_vec(imsi_vec, 15);

  std::string mcc_str = get_mcc_str(imsi_vec);
  uint16_t    mcc_bcd = 0;

  srsran::string_to_mcc(mcc_str, &mcc_bcd);
  srsran::mcc_to_bytes(mcc_bcd, mcc_);

  return true;
}

bool usim_base::get_home_mnc_bytes(uint8_t* mnc_, uint32_t n)
{
  if (!initiated) {
    logger.error("USIM not initiated!");
    return false;
  }

  if (NULL == mnc_ || n < 3) {
    logger.error("Invalid parameters to get_home_mcc_bytes");
    return false;
  }

  uint8_t imsi_vec[15];
  get_imsi_vec(imsi_vec, 15);

  std::string mcc_str = get_mcc_str(imsi_vec);
  std::string mnc_str = get_mnc_str(imsi_vec, mcc_str);

  uint16_t mnc_bcd = 0;
  uint8_t  len     = 0;

  srsran::string_to_mnc(mnc_str, &mnc_bcd);
  srsran::mnc_to_bytes(mnc_bcd, mnc_, &len);

  if (len == 2) {
    mnc_[2] = 0xf;
  }

  return true;
}

bool usim_base::get_home_msin_bcd(uint8_t* msin_, uint32_t n)
{
  if (!initiated) {
    logger.error("USIM not initiated!");
    return false;
  }

  if (NULL == msin_ || n < 5) {
    logger.error("Invalid parameters to get_home_mcc_bytes");
    return false;
  }

  srsran::plmn_id_t tmp_plmn;
  get_home_plmn_id(&tmp_plmn);

  uint8_t imsi_vec[15];
  get_imsi_vec(imsi_vec, 15);

  int total_msin_len = (tmp_plmn.nof_mnc_digits + 3) / 2;
  int j              = 0;
  for (int i = tmp_plmn.nof_mnc_digits + 3; i < 15; i += 2) {
    msin_[j] = (imsi_vec[i]) | (imsi_vec[i + 1] << 4);
    j++;
  }
  return true;
}

void usim_base::generate_nas_keys(uint8_t*                            k_asme,
                                  uint8_t*                            k_nas_enc,
                                  uint8_t*                            k_nas_int,
                                  srsran::CIPHERING_ALGORITHM_ID_ENUM cipher_algo,
                                  srsran::INTEGRITY_ALGORITHM_ID_ENUM integ_algo)
{
  if (!initiated) {
    logger.error("USIM not initiated!");
    return;
  }

  // Generate K_nas_enc and K_nas_int
  security_generate_k_nas(k_asme, cipher_algo, integ_algo, k_nas_enc, k_nas_int);
}

/*
 *  RRC Interface
 */

void usim_base::generate_as_keys(uint8_t* k_asme_, uint32_t count_ul, srsran::as_security_config_t* sec_cfg)
{
  if (!initiated) {
    logger.error("USIM not initiated!");
    return;
  }

  logger.info("Generating AS Keys. NAS UL COUNT %d", count_ul);
  logger.debug(k_asme_, 32, "K_asme");

  // Generate K_enb
  srsran::security_generate_k_enb(k_asme_, count_ul, k_enb_ctx.k_enb.data());

  memcpy(k_asme, k_asme_, 32);

  // Save initial k_enb
  memcpy(k_enb_initial, k_enb_ctx.k_enb.data(), 32);

  // Generate K_rrc_enc and K_rrc_int
  security_generate_k_rrc(k_enb_ctx.k_enb.data(),
                          sec_cfg->cipher_algo,
                          sec_cfg->integ_algo,
                          sec_cfg->k_rrc_enc.data(),
                          sec_cfg->k_rrc_int.data());

  // Generate K_up_enc and K_up_int
  security_generate_k_up(k_enb_ctx.k_enb.data(),
                         sec_cfg->cipher_algo,
                         sec_cfg->integ_algo,
                         sec_cfg->k_up_enc.data(),
                         sec_cfg->k_up_int.data());

  k_enb_ctx.ncc          = 0;
  k_enb_ctx.is_first_ncc = true;

  logger.debug(k_enb_ctx.k_enb.data(), 32, "Initial K_eNB");
  logger.debug(sec_cfg->k_rrc_int.data(), sec_cfg->k_rrc_int.size(), "K_RRC_int");
  logger.debug(sec_cfg->k_rrc_enc.data(), sec_cfg->k_rrc_enc.size(), "K_RRC_enc");
  logger.debug(sec_cfg->k_up_int.data(), sec_cfg->k_up_int.size(), "K_UP_int");
  logger.debug(sec_cfg->k_up_enc.data(), sec_cfg->k_up_enc.size(), "K_UP_enc");
}

void usim_base::generate_as_keys_ho(uint32_t pci, uint32_t earfcn, int ncc, srsran::as_security_config_t* sec_cfg)
{
  if (!initiated) {
    logger.error("USIM not initiated!");
    return;
  }

  logger.info("Re-generating AS Keys. PCI 0x%02x, DL-EARFCN %d, NCC %d", pci, earfcn, ncc);
  logger.info(k_enb_ctx.k_enb.data(), 32, "Old K_eNB");

  uint8_t* enb_star_key = k_enb_ctx.k_enb.data();

  if (ncc < 0) {
    ncc = k_enb_ctx.ncc;
  }

  while (k_enb_ctx.ncc != (uint32_t)ncc) {
    uint8_t* sync = nullptr;
    if (k_enb_ctx.is_first_ncc) {
      logger.debug("Using K_enb_initial for sync. 0x%02x, DL-EARFCN %d, NCC used: %d", pci, earfcn, k_enb_ctx.ncc);
      sync                   = k_enb_initial;
      k_enb_ctx.is_first_ncc = false;
    } else {
      logger.debug("Using NH for sync. 0x%02x, DL-EARFCN %d, NCC %d", pci, earfcn, k_enb_ctx.ncc);
      sync = k_enb_ctx.nh.data();
    }
    logger.debug(k_enb_ctx.nh.data(), 32, "NH:");

    // Generate NH
    srsran::security_generate_nh(k_asme, sync, k_enb_ctx.nh.data());

    k_enb_ctx.ncc++;
    if (k_enb_ctx.ncc == 8) {
      k_enb_ctx.ncc = 0;
    }
    enb_star_key = k_enb_ctx.nh.data();
  }

  // Generate K_enb
  srsran::security_generate_k_enb_star(enb_star_key, pci, earfcn, k_enb_star);

  // K_enb becomes K_enb*
  memcpy(k_enb_ctx.k_enb.data(), k_enb_star, 32);

  // Generate K_rrc_enc and K_rrc_int
  security_generate_k_rrc(k_enb_ctx.k_enb.data(),
                          sec_cfg->cipher_algo,
                          sec_cfg->integ_algo,
                          sec_cfg->k_rrc_enc.data(),
                          sec_cfg->k_rrc_int.data());

  // Generate K_up_enc and K_up_int
  security_generate_k_up(k_enb_ctx.k_enb.data(),
                         sec_cfg->cipher_algo,
                         sec_cfg->integ_algo,
                         sec_cfg->k_up_enc.data(),
                         sec_cfg->k_up_int.data());

  logger.info(k_enb_ctx.k_enb.data(), 32, "HO K_eNB");
  logger.info(sec_cfg->k_rrc_enc.data(), sec_cfg->k_rrc_enc.size(), "HO K_RRC_enc");
  logger.info(sec_cfg->k_rrc_int.data(), sec_cfg->k_rrc_int.size(), "HO K_RRC_int");
}

void usim_base::store_keys_before_ho(const srsran::as_security_config_t& as_ctx)
{
  logger.info("Storing AS Keys pre-handover. NCC=%d", k_enb_ctx.ncc);
  logger.info(k_enb_ctx.k_enb.data(), 32, "Old K_eNB");
  logger.info(as_ctx.k_rrc_enc.data(), as_ctx.k_rrc_enc.size(), "Old K_RRC_enc");
  logger.info(as_ctx.k_rrc_enc.data(), as_ctx.k_rrc_enc.size(), "Old K_RRC_enc");
  logger.info(as_ctx.k_rrc_int.data(), as_ctx.k_rrc_int.size(), "Old K_RRC_int");
  logger.info(as_ctx.k_rrc_int.data(), as_ctx.k_rrc_int.size(), "Old K_RRC_int");
  old_k_enb_ctx = k_enb_ctx;
  old_as_ctx    = as_ctx;
  return;
}

void usim_base::restore_keys_from_failed_ho(srsran::as_security_config_t* as_ctx)
{
  logger.info("Restoring Keys from failed handover. NCC=%d", old_k_enb_ctx.ncc);
  *as_ctx   = old_as_ctx;
  k_enb_ctx = old_k_enb_ctx;
  return;
}

bool usim_base::generate_nas_keys_5g(uint8_t*                            k_amf,
                                     uint8_t*                            k_nas_enc,
                                     uint8_t*                            k_nas_int,
                                     srsran::CIPHERING_ALGORITHM_ID_ENUM cipher_algo,
                                     srsran::INTEGRITY_ALGORITHM_ID_ENUM integ_algo)
{
  if (!initiated) {
    logger.error("USIM not initiated!");
    return false;
  }
  // Generate K_nas_enc and K_nas_int
  security_generate_k_nas_5g(k_amf, cipher_algo, integ_algo, k_nas_enc, k_nas_int);
  return true;
}

/*
 *  NR RRC Interface
 */

void usim_base::generate_nr_as_keys(srsran::as_key_t& k_amf, uint32_t count_ul, srsran::as_security_config_t* sec_cfg)
{
  if (!initiated) {
    logger.error("USIM not initiated!");
    return;
  }

  logger.info("Generating NR AS Keys. NAS UL COUNT %d", count_ul);
  logger.debug(k_amf.data(), 32, "K_amf");

  // Generate K_gnb
  srsran::security_generate_k_gnb(k_amf, count_ul, k_gnb_ctx.k_gnb);
  logger.info(k_gnb_ctx.k_gnb.data(), 32, "K_gnb");

  // Save initial k_gnb
  k_gnb_initial = k_gnb_ctx.k_gnb;

  security_generate_k_nr_rrc(k_gnb_ctx.k_gnb.data(),
                             sec_cfg->cipher_algo,
                             sec_cfg->integ_algo,
                             sec_cfg->k_rrc_enc.data(),
                             sec_cfg->k_rrc_int.data());

  security_generate_k_nr_up(k_gnb_ctx.k_gnb.data(),
                            sec_cfg->cipher_algo,
                            sec_cfg->integ_algo,
                            sec_cfg->k_up_enc.data(),
                            sec_cfg->k_up_int.data());

  logger.info(k_gnb_ctx.k_gnb.data(), 32, "Initial K_gnb");
  logger.info(sec_cfg->k_rrc_int.data(), sec_cfg->k_rrc_int.size(), "NR K_RRC_int");
  logger.info(sec_cfg->k_rrc_enc.data(), sec_cfg->k_rrc_enc.size(), "NR K_RRC_enc");
  logger.info(sec_cfg->k_up_int.data(), sec_cfg->k_up_int.size(), "NR K_UP_int");
  logger.info(sec_cfg->k_up_enc.data(), sec_cfg->k_up_enc.size(), "NR K_UP_enc");
}

bool usim_base::generate_nr_context(uint16_t sk_counter, srsran::as_security_config_t* sec_cfg)
{
  if (!initiated) {
    logger.error("USIM not initiated!");
    return false;
  }
  logger.info("Generating Keys. SCG Counter %d", sk_counter);

  srsran::security_generate_sk_gnb(k_enb_ctx.k_enb.data(), sk_counter, k_gnb_ctx.sk_gnb.data());
  logger.info(k_gnb_ctx.sk_gnb.data(), 32, "k_sk_gnb");
  if (update_nr_context(sec_cfg) == false) {
    return false;
  }
  return true;
}

bool usim_base::update_nr_context(srsran::as_security_config_t* sec_cfg)
{
  if (!initiated) {
    logger.error("USIM not initiated!");
    return false;
  }
  logger.info(k_gnb_ctx.sk_gnb.data(), 32, "k_sk_gnb");
  // Generate K_rrc_enc and K_rrc_int
  security_generate_k_nr_rrc(k_gnb_ctx.sk_gnb.data(),
                             sec_cfg->cipher_algo,
                             sec_cfg->integ_algo,
                             sec_cfg->k_rrc_enc.data(),
                             sec_cfg->k_rrc_int.data());

  // Generate K_up_enc and K_up_int
  security_generate_k_nr_up(k_gnb_ctx.sk_gnb.data(),
                            sec_cfg->cipher_algo,
                            sec_cfg->integ_algo,
                            sec_cfg->k_up_enc.data(),
                            sec_cfg->k_up_int.data());

  logger.debug(sec_cfg->k_rrc_int.data(), sec_cfg->k_rrc_int.size(), "NR K_RRC_int");
  logger.debug(sec_cfg->k_rrc_enc.data(), sec_cfg->k_rrc_enc.size(), "NR K_RRC_enc");
  logger.debug(sec_cfg->k_up_int.data(), sec_cfg->k_up_int.size(), "NR K_UP_int");
  logger.debug(sec_cfg->k_up_enc.data(), sec_cfg->k_up_enc.size(), "NR K_UP_enc");
  return true;
}

} // namespace srsue
