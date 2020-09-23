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

#include "srsue/hdr/stack/upper/usim_base.h"
#include "srsue/hdr/stack/upper/usim.h"

#ifdef HAVE_PCSC
#include "srsue/hdr/stack/upper/pcsc_usim.h"
#endif

namespace srsue {

std::unique_ptr<usim_base> usim_base::get_instance(usim_args_t* args, srslte::log* log_)
{
#if HAVE_PCSC
  if (args->mode == "pcsc") {
    return std::unique_ptr<usim_base>(new pcsc_usim(log_));
  }
#endif

  // default to soft USIM
  return std::unique_ptr<usim_base>(new usim(log_));
}

usim_base::usim_base(srslte::log* _log) : log(_log) {}

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
    ERROR("USIM not initiated!\n");
    return false;
  }

  if (NULL == imsi_ || n < 15) {
    log->error("Invalid parameters to get_imsi_vec\n");
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
    ERROR("USIM not initiated!\n");
    return false;
  }

  if (NULL == imei_ || n < 15) {
    ERROR("Invalid parameters to get_imei_vec\n");
    return false;
  }

  uint64 temp = imei;
  for (int i = 14; i >= 0; i--) {
    imei_[i] = temp % 10;
    temp /= 10;
  }
  return true;
}

bool usim_base::get_home_plmn_id(srslte::plmn_id_t* home_plmn_id)
{
  if (!initiated) {
    ERROR("USIM not initiated!\n");
    return false;
  }

  uint8_t imsi_vec[15];
  get_imsi_vec(imsi_vec, 15);

  std::ostringstream plmn_str;

  uint mcc_len = 3;
  for (uint i = 0; i < mcc_len; i++) {
    plmn_str << (int)imsi_vec[i];
  }

  for (uint i = mcc_len; i < mcc_len + mnc_length; i++) {
    plmn_str << (int)imsi_vec[i];
  }

  if (home_plmn_id->from_string(plmn_str.str())) {
    log->error("Error reading home PLMN from SIM.\n");
    return false;
  }

  log->info("Read Home PLMN Id=%s\n", home_plmn_id->to_string().c_str());

  return true;
}

void usim_base::generate_nas_keys(uint8_t*                            k_asme,
                                  uint8_t*                            k_nas_enc,
                                  uint8_t*                            k_nas_int,
                                  srslte::CIPHERING_ALGORITHM_ID_ENUM cipher_algo,
                                  srslte::INTEGRITY_ALGORITHM_ID_ENUM integ_algo)
{
  if (!initiated) {
    ERROR("USIM not initiated!\n");
    return;
  }

  // Generate K_nas_enc and K_nas_int
  security_generate_k_nas(k_asme, cipher_algo, integ_algo, k_nas_enc, k_nas_int);
}

/*
 *  RRC Interface
 */
void usim_base::generate_as_keys(uint8_t* k_asme_, uint32_t count_ul, srslte::as_security_config_t* sec_cfg)
{
  if (!initiated) {
    ERROR("USIM not initiated!\n");
    return;
  }

  // Generate K_enb
  srslte::security_generate_k_enb(k_asme_, count_ul, k_enb);

  memcpy(k_asme, k_asme_, 32);

  // Save initial k_enb
  memcpy(k_enb_initial, k_enb, 32);

  // Generate K_rrc_enc and K_rrc_int
  security_generate_k_rrc(
      k_enb, sec_cfg->cipher_algo, sec_cfg->integ_algo, sec_cfg->k_rrc_enc.data(), sec_cfg->k_rrc_int.data());

  // Generate K_up_enc and K_up_int
  security_generate_k_up(
      k_enb, sec_cfg->cipher_algo, sec_cfg->integ_algo, sec_cfg->k_up_enc.data(), sec_cfg->k_up_int.data());

  current_ncc  = 0;
  is_first_ncc = true;
}

void usim_base::generate_as_keys_ho(uint32_t pci, uint32_t earfcn, int ncc, srslte::as_security_config_t* sec_cfg)
{
  if (!initiated) {
    ERROR("USIM not initiated!\n");
    return;
  }

  INFO("Generating AS Keys HO. PCI 0x%02x, DL-EARFCN %d, NCC %d\n", pci, earfcn, ncc);
  log->info_hex(k_enb, 32, "Old K_eNB");

  uint8_t* enb_star_key = k_enb;

  if (ncc < 0) {
    ncc = current_ncc;
  }

  // Generate successive NH
  while (current_ncc != (uint32_t)ncc) {
    uint8_t* sync = NULL;
    if (is_first_ncc) {
      DEBUG("Using K_enb_initial for sync. 0x%02x, DL-EARFCN %d, NCC %d\n", pci, earfcn, current_ncc);
      sync         = k_enb_initial;
      is_first_ncc = false;
    } else {
      DEBUG("Using NH for sync. 0x%02x, DL-EARFCN %d, NCC %d\n", pci, earfcn, current_ncc);
      sync = nh;
    }
    // Generate NH
    srslte::security_generate_nh(k_asme, sync, nh);

    current_ncc++;
    if (current_ncc == 8) {
      current_ncc = 0;
    }
    enb_star_key = nh;
  }

  // Generate K_enb
  srslte::security_generate_k_enb_star(enb_star_key, pci, earfcn, k_enb_star);

  // K_enb becomes K_enb*
  memcpy(k_enb, k_enb_star, 32);

  // Generate K_rrc_enc and K_rrc_int
  security_generate_k_rrc(
      k_enb, sec_cfg->cipher_algo, sec_cfg->integ_algo, sec_cfg->k_rrc_enc.data(), sec_cfg->k_rrc_int.data());

  // Generate K_up_enc and K_up_int
  security_generate_k_up(
      k_enb, sec_cfg->cipher_algo, sec_cfg->integ_algo, sec_cfg->k_up_enc.data(), sec_cfg->k_up_int.data());

  log->info_hex(k_enb, 32, "HO K_eNB");
  log->info_hex(sec_cfg->k_rrc_enc.data(), sec_cfg->k_rrc_enc.size(), "HO K_RRC_enc");
  log->info_hex(sec_cfg->k_rrc_int.data(), sec_cfg->k_rrc_int.size(), "HO K_RRC_int");
}

void usim_base::store_keys_before_ho(const srslte::as_security_config_t& as_ctx)
{
  INFO("Storing AS Keys pre-handover. NCC=%d\n", current_ncc);
  log->info_hex(k_enb, 32, "Old K_eNB");
  log->info_hex(as_ctx.k_rrc_enc.data(), as_ctx.k_rrc_enc.size(), "Old K_RRC_enc");
  log->info_hex(as_ctx.k_rrc_enc.data(), as_ctx.k_rrc_enc.size(), "Old K_RRC_enc");
  log->info_hex(as_ctx.k_rrc_int.data(), as_ctx.k_rrc_int.size(), "Old K_RRC_int");
  log->info_hex(as_ctx.k_rrc_int.data(), as_ctx.k_rrc_int.size(), "Old K_RRC_int");
  old_is_first_ncc = is_first_ncc;
  old_as_ctx       = as_ctx;
  old_ncc          = current_ncc;
  memcpy(old_k_enb, k_enb, 32);
  return;
}

void usim_base::restore_keys_from_failed_ho(srslte::as_security_config_t* as_ctx)
{
  INFO("Restoring Keys from failed handover. NCC=%d\n", old_ncc);
  *as_ctx      = old_as_ctx;
  current_ncc  = old_ncc;
  is_first_ncc = old_is_first_ncc;
  memcpy(k_enb, old_k_enb, 32);
  return;
}

} // namespace srsue
