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

#include "srsue/hdr/stack/rrc/rrc_cell.h"

namespace srsue {

srslte::plmn_id_t cell_t::get_plmn(uint32_t idx) const
{
  if (idx < sib1.cell_access_related_info.plmn_id_list.size() && has_valid_sib1) {
    return srslte::make_plmn_id_t(sib1.cell_access_related_info.plmn_id_list[idx].plmn_id);
  } else {
    return {};
  }
}

void cell_t::set_sib1(const asn1::rrc::sib_type1_s& sib1_)
{
  sib1           = sib1_;
  has_valid_sib1 = true;

  sib_info_map.clear();
  for (uint32_t i = 0; i < sib1.sched_info_list.size(); ++i) {
    for (uint32_t j = 0; j < sib1.sched_info_list[i].sib_map_info.size(); ++j) {
      sib_info_map.insert(std::make_pair(sib1.sched_info_list[i].sib_map_info[j].to_number() - 1, i));
    }
  }
}

void cell_t::set_sib2(const asn1::rrc::sib_type2_s& sib2_)
{
  sib2           = sib2_;
  has_valid_sib2 = true;
}
void cell_t::set_sib3(const asn1::rrc::sib_type3_s& sib3_)
{
  sib3           = sib3_;
  has_valid_sib3 = true;
}
void cell_t::set_sib13(const asn1::rrc::sib_type13_r9_s& sib13_)
{
  sib13           = sib13_;
  has_valid_sib13 = true;
}

bool cell_t::is_sib_scheduled(uint32_t sib_index) const
{
  return sib_info_map.find(sib_index) != sib_info_map.end();
}

uint32_t cell_t::timeout_secs(struct timeval now) const
{
  struct timeval t[3];
  memcpy(&t[2], &now, sizeof(struct timeval));
  memcpy(&t[1], &last_update, sizeof(struct timeval));
  get_time_interval(t);
  return t[0].tv_sec;
}

bool cell_t::has_sib(uint32_t index) const
{
  switch (index) {
    case 0:
      return has_sib1();
    case 1:
      return has_sib2();
    case 2:
      return has_sib3();
    case 12:
      return has_sib13();
    default:
      break;
  }
  return false;
}

std::string cell_t::to_string() const
{
  char buf[256];
  snprintf(buf,
           256,
           "{cell_id: 0x%x, pci: %d, dl_earfcn: %d, rsrp=%+.1f, cfo=%+.1f}",
           get_cell_id(),
           get_pci(),
           get_earfcn(),
           get_rsrp(),
           get_cfo_hz());
  return std::string{buf};
}

bool cell_t::has_plmn_id(asn1::rrc::plmn_id_s plmn_id) const
{
  if (has_valid_sib1) {
    for (const auto& e : sib1.cell_access_related_info.plmn_id_list) {
      if (plmn_id.mcc == e.plmn_id.mcc && plmn_id.mnc == e.plmn_id.mnc) {
        return true;
      }
    }
  }
  return false;
}

uint16_t cell_t::get_mcc() const
{
  uint16_t mcc;
  if (has_valid_sib1) {
    if (sib1.cell_access_related_info.plmn_id_list.size() > 0) {
      if (srslte::bytes_to_mcc(&sib1.cell_access_related_info.plmn_id_list[0].plmn_id.mcc[0], &mcc)) {
        return mcc;
      }
    }
  }
  return 0;
}

uint16_t cell_t::get_mnc() const
{
  uint16_t mnc;
  if (has_valid_sib1) {
    if (sib1.cell_access_related_info.plmn_id_list.size() > 0) {
      if (srslte::bytes_to_mnc(&sib1.cell_access_related_info.plmn_id_list[0].plmn_id.mnc[0],
                               &mnc,
                               sib1.cell_access_related_info.plmn_id_list[0].plmn_id.mnc.size())) {
        return mnc;
      }
    }
  }
  return 0;
}

} // namespace srsue
