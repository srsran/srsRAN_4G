/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSRAN_RRC_NR_SECURITY_CONTEXT_H
#define SRSRAN_RRC_NR_SECURITY_CONTEXT_H

#include "srsgnb/hdr/stack/rrc/rrc_nr_config.h"
#include "srsran/asn1/ngap.h"
#include "srsran/interfaces/gnb_interfaces.h"
#include "srsran/srslog/srslog.h"

namespace srsgnb {

class nr_security_context
{
public:
  explicit nr_security_context(const srsenb::rrc_nr_cfg_t& cfg_) :
    cfg(&cfg_), logger(srslog::fetch_basic_logger("RRC_NR"))
  {}

  explicit nr_security_context(const nr_security_context& other) : logger(srslog::fetch_basic_logger("RRC_NR"))
  {
    cfg                   = other.cfg;
    k_gnb_present         = other.k_gnb_present;
    security_capabilities = other.security_capabilities;
    std::copy(other.k_gnb, other.k_gnb + 32, k_gnb);
    sec_cfg = other.sec_cfg;
    ncc     = other.ncc;
  }

  nr_security_context& operator=(const nr_security_context& other)
  {
    cfg                   = other.cfg;
    k_gnb_present         = other.k_gnb_present;
    security_capabilities = other.security_capabilities;
    std::copy(other.k_gnb, other.k_gnb + 32, k_gnb);
    sec_cfg = other.sec_cfg;
    ncc     = other.ncc;
    return *this;
  }

  bool set_security_capabilities(const asn1::ngap_nr::ue_security_cap_s& caps);
  void set_security_key(const asn1::fixed_bitstring<256, false, true>& key);
  void set_ncc(uint8_t ncc_) { ncc = ncc_; }

  asn1::rrc_nr::security_algorithm_cfg_s get_security_algorithm_cfg() const;
  const srsran::as_security_config_t&    get_as_sec_cfg() const { return sec_cfg; }
  uint8_t                                get_ncc() const { return ncc; }
  bool                                   is_as_sec_cfg_valid() const { return k_gnb_present; }

  void regenerate_keys_handover(uint32_t new_pci, uint32_t new_dl_earfcn);

private:
  void generate_as_keys();

  srslog::basic_logger&            logger;
  const srsenb::rrc_nr_cfg_t*      cfg                   = nullptr;
  bool                             k_gnb_present         = false;
  asn1::ngap_nr::ue_security_cap_s security_capabilities = {};
  uint8_t                          k_gnb[32]             = {}; // Provided by MME
  srsran::as_security_config_t     sec_cfg               = {};
  uint8_t                          ncc                   = 0;
};
} // namespace srsgnb
#endif
