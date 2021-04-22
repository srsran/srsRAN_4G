/**
 * Copyright 2013-2021 Software Radio Systems Limited
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

#include "srsue/hdr/stack/rrc/rrc_cell.h"

namespace srsue {

meas_cell::meas_cell(srsran::unique_timer timer_) : timer(std::move(timer_))
{
  timer.set(neighbour_timeout_ms);
  timer.run();
}
meas_cell::meas_cell(const phy_cell_t& phy_cell_, srsran::unique_timer timer) : meas_cell(std::move(timer))
{
  phy_cell = phy_cell_;
}

srsran::plmn_id_t meas_cell_eutra::get_plmn(uint32_t idx) const
{
  if (idx < sib1.cell_access_related_info.plmn_id_list.size() && has_valid_sib1) {
    return srsran::make_plmn_id_t(sib1.cell_access_related_info.plmn_id_list[idx].plmn_id);
  } else {
    return {};
  }
}

void meas_cell_eutra::set_sib1(const asn1::rrc::sib_type1_s& sib1_)
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

void meas_cell_eutra::set_sib2(const asn1::rrc::sib_type2_s& sib2_)
{
  sib2           = sib2_;
  has_valid_sib2 = true;
}
void meas_cell_eutra::set_sib3(const asn1::rrc::sib_type3_s& sib3_)
{
  sib3           = sib3_;
  has_valid_sib3 = true;
}
void meas_cell_eutra::set_sib13(const asn1::rrc::sib_type13_r9_s& sib13_)
{
  sib13           = sib13_;
  has_valid_sib13 = true;
}

bool meas_cell::is_sib_scheduled(uint32_t sib_index) const
{
  return sib_info_map.find(sib_index) != sib_info_map.end();
}

bool meas_cell::has_sibs(srsran::span<const uint32_t> indexes) const
{
  for (uint32_t idx : indexes) {
    if (not has_sib(idx)) {
      return false;
    }
  }
  return true;
}

bool meas_cell::has_sib(uint32_t index) const
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

std::string meas_cell_nr::to_string() const
{
  char buf[256];
  snprintf(buf,
           256,
           "{cell_id: 0x%x, pci: %d, dl_arfcn_nr: %d, rsrp=%+.1f, cfo=%+.1f}",
           get_cell_id(),
           get_pci(),
           get_earfcn(),
           get_rsrp(),
           get_cfo_hz());
  return std::string{buf};
}

std::string meas_cell_eutra::to_string() const
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

bool meas_cell_eutra::has_plmn_id(asn1::rrc::plmn_id_s plmn_id) const
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

uint16_t meas_cell_eutra::get_mcc() const
{
  uint16_t mcc;
  if (has_valid_sib1) {
    if (sib1.cell_access_related_info.plmn_id_list.size() > 0) {
      if (srsran::bytes_to_mcc(&sib1.cell_access_related_info.plmn_id_list[0].plmn_id.mcc[0], &mcc)) {
        return mcc;
      }
    }
  }
  return 0;
}

uint16_t meas_cell_eutra::get_mnc() const
{
  uint16_t mnc;
  if (has_valid_sib1) {
    if (sib1.cell_access_related_info.plmn_id_list.size() > 0) {
      if (srsran::bytes_to_mnc(&sib1.cell_access_related_info.plmn_id_list[0].plmn_id.mnc[0],
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
template <class T>
meas_cell_list<T>::meas_cell_list(srsran::task_sched_handle task_sched_) :
  serv_cell(new T(task_sched_.get_unique_timer())), task_sched(task_sched_)
{}

template <class T>
T* meas_cell_list<T>::get_neighbour_cell_handle(uint32_t earfcn, uint32_t pci)
{
  auto it = find_if(neighbour_cells.begin(), neighbour_cells.end(), [&](const unique_meas_cell& cell) {
    return cell->equals(earfcn, pci);
  });
  return it != neighbour_cells.end() ? it->get() : nullptr;
}
template <class T>
const T* meas_cell_list<T>::get_neighbour_cell_handle(uint32_t earfcn, uint32_t pci) const
{
  auto it = find_if(neighbour_cells.begin(), neighbour_cells.end(), [&](const unique_meas_cell& cell) {
    return cell->equals(earfcn, pci);
  });
  return it != neighbour_cells.end() ? it->get() : nullptr;
}

// If only neighbour PCI is provided, copy full cell from serving cell
template <class T>
bool meas_cell_list<T>::add_meas_cell(const phy_meas_t& meas)
{
  phy_cell_t phy_cell = {};
  phy_cell.earfcn     = meas.earfcn;
  phy_cell.pci        = meas.pci;
  unique_meas_cell c  = unique_meas_cell(new T(phy_cell, task_sched.get_unique_timer()));
  c.get()->set_rsrp(meas.rsrp);
  c.get()->set_rsrq(meas.rsrq);
  c.get()->set_cfo(meas.cfo_hz);
  return add_meas_cell(std::move(c));
}
template <class T>
bool meas_cell_list<T>::add_meas_cell(unique_meas_cell cell)
{
  bool ret = add_neighbour_cell_unsorted(std::move(cell));
  if (ret) {
    sort_neighbour_cells();
  }
  return ret;
}
template <class T>
bool meas_cell_list<T>::add_neighbour_cell_unsorted(unique_meas_cell new_cell)
{
  // Make sure cell is valid
  if (!new_cell->is_valid()) {
    logger.error("Trying to add cell %s but is not valid", new_cell->to_string().c_str());
    return false;
  }

  if (is_same_cell(serving_cell(), *new_cell)) {
    logger.info("Added neighbour cell %s is serving cell", new_cell->to_string().c_str());
    serv_cell = std::move(new_cell);
    return true;
  }

  // If cell exists, update RSRP value
  T* existing_cell = get_neighbour_cell_handle(new_cell->get_earfcn(), new_cell->get_pci());
  if (existing_cell != nullptr) {
    if (std::isnormal(new_cell.get()->get_rsrp())) {
      existing_cell->set_rsrp(new_cell.get()->get_rsrp());
    }
    logger.info("Updated neighbour cell %s rsrp=%f", new_cell->to_string().c_str(), new_cell.get()->get_rsrp());
    return true;
  }

  if (neighbour_cells.size() >= MAX_NEIGHBOUR_CELLS) {
    // If there isn't space, keep the strongest only
    if (not new_cell->greater(neighbour_cells.back().get())) {
      logger.warning("Could not add cell %s: no space in neighbours", new_cell->to_string().c_str());
      return false;
    }

    rem_last_neighbour();
  }

  logger.info(
      "Adding neighbour cell %s, nof_neighbours=%zd", new_cell->to_string().c_str(), neighbour_cells.size() + 1);
  neighbour_cells.push_back(std::move(new_cell));
  return true;
}
template <class T>
void meas_cell_list<T>::rem_last_neighbour()
{
  if (not neighbour_cells.empty()) {
    unique_meas_cell& c = neighbour_cells.back();
    logger.debug("Delete cell %s from neighbor list.", c->to_string().c_str());
    neighbour_cells.pop_back();
  }
}
template <class T>
typename meas_cell_list<T>::unique_meas_cell meas_cell_list<T>::remove_neighbour_cell(uint32_t earfcn, uint32_t pci)
{
  auto it = find_if(neighbour_cells.begin(), neighbour_cells.end(), [&](const unique_meas_cell& cell) {
    return cell->equals(earfcn, pci);
  });
  if (it != neighbour_cells.end()) {
    auto retval = std::move(*it);
    it          = neighbour_cells.erase(it);
    return retval;
  }
  return nullptr;
}

// Sort neighbour cells by decreasing order of RSRP
template <class T>
void meas_cell_list<T>::sort_neighbour_cells()
{
  std::sort(std::begin(neighbour_cells),
            std::end(neighbour_cells),
            [](const unique_meas_cell& a, const unique_meas_cell& b) { return a->greater(b.get()); });

  log_neighbour_cells();
}
template <class T>
void meas_cell_list<T>::log_neighbour_cells() const
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
    logger.debug("Neighbours: %s]", ordered);
  } else {
    logger.debug("Neighbours: Empty");
  }
}

//! Called by main RRC thread to remove neighbours from which measurements have not been received in a while
template <class T>
void meas_cell_list<T>::clean_neighbours()
{
  for (auto it = neighbour_cells.begin(); it != neighbour_cells.end();) {
    if (it->get()->timer.is_expired()) {
      logger.info("Neighbour PCI=%d timed out. Deleting.", (*it)->get_pci());
      it = neighbour_cells.erase(it);
    } else {
      ++it;
    }
  }
}
template <class T>
std::string meas_cell_list<T>::print_neighbour_cells() const
{
  if (neighbour_cells.empty()) {
    return "";
  }
  std::string s;
  s.reserve(256);
  for (auto it = neighbour_cells.begin(); it != neighbour_cells.end() - 1; ++it) {
    s += (*it)->to_string() + ", ";
  }
  s += neighbour_cells.back()->to_string();
  return s;
}
template <class T>
std::set<uint32_t> meas_cell_list<T>::get_neighbour_pcis(uint32_t earfcn) const
{
  std::set<uint32_t> pcis = {};
  for (const unique_meas_cell& cell : neighbour_cells) {
    if (cell->get_earfcn() == earfcn) {
      pcis.insert(cell->get_pci());
    }
  }
  return pcis;
}
template <class T>
bool meas_cell_list<T>::has_neighbour_cell(uint32_t earfcn, uint32_t pci) const
{
  return get_neighbour_cell_handle(earfcn, pci) != nullptr;
}
template <class T>
T* meas_cell_list<T>::find_cell(uint32_t earfcn, uint32_t pci)
{
  if (serving_cell().phy_cell.pci == pci and serving_cell().phy_cell.earfcn == earfcn) {
    return &serving_cell();
  }
  return get_neighbour_cell_handle(earfcn, pci);
}
template <class T>
int meas_cell_list<T>::set_serving_cell(phy_cell_t phy_cell, bool discard_serving)
{
  // don't update neighbor cell list unless serving cell changes
  if (phy_cell.pci == serving_cell().get_pci() && phy_cell.earfcn == serving_cell().get_earfcn()) {
    return SRSRAN_SUCCESS;
  }

  // Remove future serving cell from neighbours to make space for current serving cell
  unique_meas_cell new_serving_cell = remove_neighbour_cell(phy_cell.earfcn, phy_cell.pci);
  if (new_serving_cell == nullptr) {
    logger.error("Setting serving cell: Unknown cell with earfcn=%d, PCI=%d", phy_cell.earfcn, phy_cell.pci);
    return SRSRAN_ERROR;
  }

  // Set new serving cell
  std::swap(serv_cell, new_serving_cell);
  auto& old_serv_cell = new_serving_cell;
  logger.info("Setting serving cell %s, nof_neighbours=%zd", serv_cell->to_string().c_str(), nof_neighbours());

  // Re-add old serving cell to list of neighbours
  if (old_serv_cell->is_valid() and not is_same_cell(phy_cell, *old_serv_cell) and not discard_serving) {
    if (not add_meas_cell(std::move(old_serv_cell))) {
      logger.info("Serving cell not added to list of neighbours. Worse than current neighbours");
    }
  }
  return SRSRAN_SUCCESS;
}

template <class T>
bool meas_cell_list<T>::process_new_cell_meas(const std::vector<phy_meas_t>&                    meas,
                                              const std::function<void(T&, const phy_meas_t&)>& filter_meas)
{
  bool neighbour_added = false;
  for (const auto& m : meas) {
    T* c = nullptr;

    // Get serving_cell handle if it's the serving cell
    bool is_serving_cell = m.earfcn == 0 or is_same_cell(m, serving_cell());
    if (is_serving_cell) {
      c = serv_cell.get();
      if (not serving_cell().is_valid()) {
        logger.error("MEAS:  Received serving cell measurement but undefined or invalid");
        continue;
      }
    } else {
      // Or update/add RRC neighbour cell database
      c = get_neighbour_cell_handle(m.earfcn, m.pci);
    }

    // Filter RSRP/RSRQ measurements if cell exits
    if (c != nullptr) {
      filter_meas(*c, m);
    } else {
      // or just set initial value
      neighbour_added |= add_meas_cell(m);
    }

    logger.info("MEAS:  New measurement %s cell: earfcn=%d, pci=%d, rsrp=%.2f dBm, cfo=%+.1f Hz",
                is_serving_cell ? "serving" : "neighbour",
                m.earfcn,
                m.pci,
                m.rsrp,
                m.cfo_hz);
  }
  return neighbour_added;
}

template class meas_cell_list<meas_cell_eutra>;
template class meas_cell_list<meas_cell_nr>;

} // namespace srsue
