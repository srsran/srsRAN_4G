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

#include "srslte/asn1/rrc_nr_asn1_utils.h"
#include "srslte/asn1/rrc_nr_asn1.h"
#include "srslte/config.h"
#include <algorithm>

namespace srslte {

/***************************
 *        PLMN ID
 **************************/

bool plmn_is_valid(const asn1::rrc_nr::plmn_id_s& asn1_type)
{
  return asn1_type.mcc_present and (asn1_type.mnc.size() == 3 or asn1_type.mnc.size() == 2);
}

plmn_id_t make_plmn_id_t(const asn1::rrc_nr::plmn_id_s& asn1_type)
{
  if (not plmn_is_valid(asn1_type)) {
    return {};
  }
  plmn_id_t plmn;
  std::copy(&asn1_type.mcc[0], &asn1_type.mcc[3], &plmn.mcc[0]);
  plmn.nof_mnc_digits = asn1_type.mnc.size();
  std::copy(&asn1_type.mnc[0], &asn1_type.mnc[plmn.nof_mnc_digits], &plmn.mnc[0]);

  return plmn;
}

void to_asn1(asn1::rrc_nr::plmn_id_s* asn1_type, const plmn_id_t& cfg)
{
  asn1_type->mcc_present = true;
  std::copy(&cfg.mcc[0], &cfg.mcc[3], &asn1_type->mcc[0]);
  asn1_type->mnc.resize(cfg.nof_mnc_digits);
  std::copy(&cfg.mnc[0], &cfg.mnc[cfg.nof_mnc_digits], &asn1_type->mnc[0]);
}

} // namespace srslte

namespace srsenb {

int set_sched_cell_cfg_sib1(srsenb::sched_interface::cell_cfg_t* sched_cfg, const asn1::rrc_nr::sib1_s& sib1)
{
  bzero(sched_cfg, sizeof(srsenb::sched_interface::cell_cfg_t));

  // set SIB1 and SIB2+ period
  sched_cfg->sibs[0].period_rf = 16; // SIB1 is always 16 rf
  for (uint32_t i = 0; i < sib1.si_sched_info.sched_info_list.size(); i++) {
    sched_cfg->sibs[i + 1].period_rf = sib1.si_sched_info.sched_info_list[i].si_periodicity.to_number();
  }

  // si-WindowLength
  sched_cfg->si_window_ms = sib1.si_sched_info.si_win_len.to_number();

  // setup PRACH
  if (not sib1.si_sched_info.si_request_cfg.rach_occasions_si_present) {
    asn1::log_error("Expected RA Resp Win present\n");
    return SRSLTE_ERROR;
  }
  sched_cfg->prach_rar_window = sib1.si_sched_info.si_request_cfg.rach_occasions_si.rach_cfg_si.ra_resp_win.to_number();
  sched_cfg->prach_freq_offset = sib1.si_sched_info.si_request_cfg.rach_occasions_si.rach_cfg_si.msg1_freq_start;
  sched_cfg->maxharq_msg3tx    = sib1.si_sched_info.si_request_cfg.rach_occasions_si.rach_cfg_si.preamb_trans_max;

  return SRSLTE_SUCCESS;
}

} // namespace srsenb