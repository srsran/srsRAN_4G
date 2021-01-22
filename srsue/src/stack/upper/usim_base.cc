/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2020 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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
    log->error("USIM not initiated!\n");
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
    log->error("USIM not initiated!\n");
    return false;
  }

  if (NULL == imei_ || n < 15) {
    log->error("Invalid parameters to get_imei_vec\n");
    return false;
  }

  uint64 temp = imei;
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

bool usim_base::get_home_plmn_id(srslte::plmn_id_t* home_plmn_id)
{
  if (!initiated) {
    log->error("USIM not initiated!\n");
    return false;
  }

  uint8_t imsi_vec[15];
  get_imsi_vec(imsi_vec, 15);

  std::string mcc_str = get_mcc_str(imsi_vec);
  std::string mnc_str = get_mnc_str(imsi_vec, mcc_str);

  std::string plmn_str = mcc_str + mnc_str;
  if (home_plmn_id->from_string(plmn_str)) {
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
    log->error("USIM not initiated!\n");
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
    log->error("USIM not initiated!\n");
    return;
  }

  log->info("Generating AS Keys. NAS UL COUNT %d\n", count_ul);
  log->debug_hex(k_asme_, 32, "K_asme");

  // Generate K_enb
  srslte::security_generate_k_enb(k_asme_, count_ul, k_enb_ctx.k_enb.data());

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

  log->debug_hex(k_enb_ctx.k_enb.data(), 32, "Initial K_eNB");
  log->debug_hex(sec_cfg->k_rrc_int.data(), sec_cfg->k_rrc_int.size(), "K_RRC_int");
  log->debug_hex(sec_cfg->k_rrc_enc.data(), sec_cfg->k_rrc_enc.size(), "K_RRC_enc");
  log->debug_hex(sec_cfg->k_up_int.data(), sec_cfg->k_up_int.size(), "K_UP_int");
  log->debug_hex(sec_cfg->k_up_enc.data(), sec_cfg->k_up_enc.size(), "K_UP_enc");
}

void usim_base::generate_as_keys_ho(uint32_t pci, uint32_t earfcn, int ncc, srslte::as_security_config_t* sec_cfg)
{
  if (!initiated) {
    log->error("USIM not initiated!\n");
    return;
  }

  log->info("Re-generating AS Keys. PCI 0x%02x, DL-EARFCN %d, NCC %d\n", pci, earfcn, ncc);
  log->info_hex(k_enb_ctx.k_enb.data(), 32, "Old K_eNB");

  uint8_t* enb_star_key = k_enb_ctx.k_enb.data();

  if (ncc < 0) {
    ncc = k_enb_ctx.ncc;
  }

  while (k_enb_ctx.ncc != (uint32_t)ncc) {
    uint8_t* sync = nullptr;
    if (k_enb_ctx.is_first_ncc) {
      log->debug("Using K_enb_initial for sync. 0x%02x, DL-EARFCN %d, NCC used: %d\n", pci, earfcn, k_enb_ctx.ncc);
      sync                   = k_enb_initial;
      k_enb_ctx.is_first_ncc = false;
    } else {
      log->debug("Using NH for sync. 0x%02x, DL-EARFCN %d, NCC %d\n", pci, earfcn, k_enb_ctx.ncc);
      sync = k_enb_ctx.nh.data();
    }
    log->debug_hex(k_enb_ctx.nh.data(), 32, "NH:");

    // Generate NH
    srslte::security_generate_nh(k_asme, sync, k_enb_ctx.nh.data());

    k_enb_ctx.ncc++;
    if (k_enb_ctx.ncc == 8) {
      k_enb_ctx.ncc = 0;
    }
    enb_star_key = k_enb_ctx.nh.data();
  }

  // Generate K_enb
  srslte::security_generate_k_enb_star(enb_star_key, pci, earfcn, k_enb_star);

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

  log->info_hex(k_enb_ctx.k_enb.data(), 32, "HO K_eNB");
  log->info_hex(sec_cfg->k_rrc_enc.data(), sec_cfg->k_rrc_enc.size(), "HO K_RRC_enc");
  log->info_hex(sec_cfg->k_rrc_int.data(), sec_cfg->k_rrc_int.size(), "HO K_RRC_int");
}

void usim_base::store_keys_before_ho(const srslte::as_security_config_t& as_ctx)
{
  log->info("Storing AS Keys pre-handover. NCC=%d\n", k_enb_ctx.ncc);
  log->info_hex(k_enb_ctx.k_enb.data(), 32, "Old K_eNB");
  log->info_hex(as_ctx.k_rrc_enc.data(), as_ctx.k_rrc_enc.size(), "Old K_RRC_enc");
  log->info_hex(as_ctx.k_rrc_enc.data(), as_ctx.k_rrc_enc.size(), "Old K_RRC_enc");
  log->info_hex(as_ctx.k_rrc_int.data(), as_ctx.k_rrc_int.size(), "Old K_RRC_int");
  log->info_hex(as_ctx.k_rrc_int.data(), as_ctx.k_rrc_int.size(), "Old K_RRC_int");
  old_k_enb_ctx = k_enb_ctx;
  old_as_ctx    = as_ctx;
  return;
}

void usim_base::restore_keys_from_failed_ho(srslte::as_security_config_t* as_ctx)
{
  log->info("Restoring Keys from failed handover. NCC=%d\n", old_k_enb_ctx.ncc);
  *as_ctx   = old_as_ctx;
  k_enb_ctx = old_k_enb_ctx;
  return;
}

/*
 *  NR RRC Interface
 */

void usim_base::generate_nr_context(uint16_t sk_counter, srslte::as_security_config_t* sec_cfg)
{
  if (!initiated) {
    log->error("USIM not initiated!\n");
    return;
  }
  log->info("Generating Keys. SCG Counter %d\n", sk_counter);

  srslte::security_generate_sk_gnb(k_enb_ctx.k_enb.data(), k_gnb_ctx.sk_gnb.data(), sk_counter);
  log->info_hex(k_gnb_ctx.sk_gnb.data(), 32, "k_sk_gnb");
  update_nr_context(sec_cfg);
}

void usim_base::update_nr_context(srslte::as_security_config_t* sec_cfg)
{
  if (!initiated) {
    log->error("USIM not initiated!\n");
    return;
  }
  log->info_hex(k_gnb_ctx.sk_gnb.data(), 32, "k_sk_gnb");
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

  log->debug_hex(sec_cfg->k_rrc_int.data(), sec_cfg->k_rrc_int.size(), "NR K_RRC_int");
  log->debug_hex(sec_cfg->k_rrc_enc.data(), sec_cfg->k_rrc_enc.size(), "NR K_RRC_enc");
  log->debug_hex(sec_cfg->k_up_int.data(), sec_cfg->k_up_int.size(), "NR K_UP_int");
  log->debug_hex(sec_cfg->k_up_enc.data(), sec_cfg->k_up_enc.size(), "NR K_UP_enc");
}

} // namespace srsue
