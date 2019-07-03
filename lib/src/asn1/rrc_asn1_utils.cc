/*
 * Copyright 2013-2019 Software Radio Systems Limited
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

#include "srslte/asn1/rrc_asn1_utils.h"
#include "srslte/asn1/rrc_asn1.h"
#include "srslte/common/bcd_helpers.h"
#include "srslte/config.h"
#include <algorithm>

namespace srslte {

int plmn_id_t::from_asn1(const asn1::rrc::plmn_id_s* asn1_type)
{
  if (not asn1_type->mcc_present) {
    return SRSLTE_ERROR;
  }
  std::copy(&asn1_type->mcc[0], &asn1_type->mcc[3], &mcc[0]);
  nof_mnc_digits = asn1_type->mnc.size();
  std::copy(&asn1_type->mnc[0], &asn1_type->mnc[nof_mnc_digits], &mnc[0]);

  return SRSLTE_SUCCESS;
}

void plmn_id_t::to_asn1(asn1::rrc::plmn_id_s* asn1_type) const
{
  asn1_type->mcc_present = true;
  std::copy(&mcc[0], &mcc[3], &asn1_type->mcc[0]);
  asn1_type->mnc.resize(nof_mnc_digits);
  std::copy(&mnc[0], &mnc[nof_mnc_digits], &asn1_type->mnc[0]);
}

void plmn_id_t::from_number(uint16_t mcc_num, uint16_t mnc_num)
{
  srslte::mcc_to_bytes(mcc_num, &mcc[0]);
  srslte::mnc_to_bytes(mnc_num, &mnc[0], &nof_mnc_digits);
}

int plmn_id_t::from_string(const std::string& plmn_str)
{
  if (plmn_str.size() < 5 or plmn_str.size() > 6) {
    return SRSLTE_ERROR;
  }
  uint16_t mnc_num, mcc_num;
  if (not string_to_mcc(std::string(plmn_str.begin(), plmn_str.begin() + 3), &mcc_num)) {
    return SRSLTE_ERROR;
  }
  if (not string_to_mnc(std::string(plmn_str.begin() + 3, plmn_str.end()), &mnc_num)) {
    return SRSLTE_ERROR;
  }
  if (not mcc_to_bytes(mcc_num, &mcc[0])) {
    return SRSLTE_ERROR;
  }
  return mnc_to_bytes(mnc_num, &mnc[0], &nof_mnc_digits) ? SRSLTE_SUCCESS : SRSLTE_ERROR;
}

std::string plmn_id_t::to_string() const
{
  std::string mcc_str, mnc_str;
  uint16_t    mnc_num, mcc_num;
  bytes_to_mnc(&mnc[0], &mnc_num, nof_mnc_digits);
  bytes_to_mcc(&mcc[0], &mcc_num);
  mnc_to_string(mnc_num, &mnc_str);
  mcc_to_string(mcc_num, &mcc_str);
  return mcc_str + mnc_str;
}

void s_tmsi_t::from_asn1(const asn1::rrc::s_tmsi_s* asn1_type)
{
  mmec   = asn1_type->mmec.to_number();
  m_tmsi = asn1_type->m_tmsi.to_number();
}

void s_tmsi_t::to_asn1(asn1::rrc::s_tmsi_s* asn1_type) const
{
  asn1_type->mmec.from_number(mmec);
  asn1_type->m_tmsi.from_number(m_tmsi);
}

static_assert((int)establishment_cause_t::nulltype == (int)asn1::rrc::establishment_cause_opts::nulltype,
              "Failed conversion of ASN1 type");
std::string to_string(const establishment_cause_t& cause)
{
  asn1::rrc::establishment_cause_e asn1_cause((asn1::rrc::establishment_cause_opts::options)cause);
  return asn1_cause.to_string();
}

} // namespace