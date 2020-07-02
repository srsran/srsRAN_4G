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

/*********************************************
 *           Neighbour Cell List
 ********************************************/

cell_t* cell_list::get_neighbour_cell_handle(uint32_t earfcn, uint32_t pci)
{
  for (unique_cell_t& cell : neighbour_cells) {
    if (cell->equals(earfcn, pci)) {
      return cell.get();
    }
  }
  return nullptr;
}

bool cell_list::add_neighbour(unique_cell_t new_cell)
{
  // Make sure cell is valid
  if (!new_cell->is_valid()) {
    log_h->error("Trying to add cell %s but is not valid", new_cell->to_string().c_str());
    return false;
  }

  // If cell exists, update RSRP value
  cell_t* existing_cell = get_neighbour_cell_handle(new_cell->get_earfcn(), new_cell->get_pci());
  if (existing_cell != nullptr) {
    if (std::isnormal(new_cell.get()->get_rsrp())) {
      existing_cell->set_rsrp(new_cell.get()->get_rsrp());
    }
    log_h->info("Updated neighbour cell %s rsrp=%f\n", new_cell->to_string().c_str(), new_cell.get()->get_rsrp());
    return true;
  }

  if (neighbour_cells.size() >= MAX_NEIGHBOUR_CELLS) {
    // If there isn't space, keep the strongest only
    if (not new_cell->greater(neighbour_cells.back().get())) {
      log_h->warning("Could not add cell %s: no space in neighbours\n", new_cell->to_string().c_str());
      return false;
    }

    rem_last_neighbour();
  }

  log_h->info(
      "Adding neighbour cell %s, nof_neighbours=%zd\n", new_cell->to_string().c_str(), neighbour_cells.size() + 1);
  neighbour_cells.push_back(std::move(new_cell));

  sort_neighbour_cells();
  return true;
}

void cell_list::rem_last_neighbour()
{
  if (not neighbour_cells.empty()) {
    unique_cell_t& c = neighbour_cells.back();
    log_h->debug("Delete cell %s from neighbor list.\n", c->to_string().c_str());
    neighbour_cells.pop_back();
  }
}

// Sort neighbour cells by decreasing order of RSRP
void cell_list::sort_neighbour_cells()
{
  std::sort(std::begin(neighbour_cells), std::end(neighbour_cells), [](const unique_cell_t& a, const unique_cell_t& b) {
    return a->greater(b.get());
  });

  log_neighbour_cells();
}

void cell_list::log_neighbour_cells() const
{
  if (not neighbour_cells.empty()) {
    const int32_t MAX_STR_LEN          = 512;
    char          ordered[MAX_STR_LEN] = {};
    int           n                    = 0;
    n += snprintf(ordered, MAX_STR_LEN, "[%s", neighbour_cells[0]->to_string().c_str());
    for (uint32_t i = 1; i < neighbour_cells.size(); i++) {
      if (n < MAX_STR_LEN) { // make sure there is still room left
        int m = snprintf(&ordered[n], (size_t)MAX_STR_LEN - n, " | %s", neighbour_cells[i]->to_string().c_str());
        if (m > 0) {
          n += m;
        }
      }
    }
    log_h->debug("Neighbours: %s]\n", ordered);
  } else {
    log_h->debug("Neighbours: Empty\n");
  }
}

} // namespace srsue
