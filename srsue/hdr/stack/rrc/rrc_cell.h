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

#ifndef SRSLTE_RRC_CELL_H
#define SRSLTE_RRC_CELL_H

#include "srslte/asn1/rrc_asn1.h"
#include "srslte/asn1/rrc_asn1_utils.h"
#include "srslte/interfaces/ue_interfaces.h"

namespace srsue {

inline std::string to_string(const srsue::phy_cell_t& c)
{
  char buffer[64];
  snprintf(buffer, 64, "{earfcn=%d, pci=%d}\n", c.earfcn, c.pci);
  return {buffer};
}

class meas_cell
{
public:
  const static int neighbour_timeout_ms = 5000;

  explicit meas_cell(srslte::unique_timer timer);
  meas_cell(const phy_cell_t& phy_cell_, srslte::unique_timer timer);

  // comparison based on pci and earfcn
  bool is_valid() const { return phy_cell.earfcn != 0 && srslte_cellid_isvalid(phy_cell.pci); }
  bool equals(const meas_cell& x) { return equals(x.phy_cell.earfcn, x.phy_cell.pci); }
  bool equals(uint32_t earfcn, uint32_t pci) { return earfcn == phy_cell.earfcn && pci == phy_cell.pci; }

  // NaN means an RSRP value has not yet been obtained. Keep then in the list and clean them if never updated
  bool greater(const meas_cell* x) const { return rsrp > x->rsrp || std::isnan(rsrp); }
  bool greater(const meas_cell& x) const { return rsrp > x.rsrp || std::isnan(rsrp); }

  bool              has_plmn_id(asn1::rrc::plmn_id_s plmn_id) const;
  uint32_t          nof_plmns() const { return has_sib1() ? sib1.cell_access_related_info.plmn_id_list.size() : 0; }
  srslte::plmn_id_t get_plmn(uint32_t idx) const;

  uint16_t get_tac() const { return has_sib1() ? (uint16_t)sib1.cell_access_related_info.tac.to_number() : 0; }
  uint32_t get_earfcn() const { return phy_cell.earfcn; }
  uint32_t get_pci() const { return phy_cell.pci; }

  void set_rsrp(float rsrp_)
  {
    if (!std::isnan(rsrp_)) {
      rsrp = rsrp_;
    }
    timer.run();
  }
  void set_rsrq(float rsrq_)
  {
    if (!std::isnan(rsrq_)) {
      rsrq = rsrq_;
    }
  }
  void set_cfo(float cfo_Hz_)
  {
    if (not std::isnan(cfo_Hz_) && not std::isinf(cfo_Hz_)) {
      phy_cell.cfo_hz = cfo_Hz_;
    }
  }

  float get_rsrp() const { return rsrp; }
  float get_rsrq() const { return rsrq; }
  float get_cfo_hz() const { return phy_cell.cfo_hz; }

  void set_sib1(const asn1::rrc::sib_type1_s& sib1_);
  void set_sib2(const asn1::rrc::sib_type2_s& sib2_);
  void set_sib3(const asn1::rrc::sib_type3_s& sib3_);
  void set_sib13(const asn1::rrc::sib_type13_r9_s& sib13_);

  const asn1::rrc::sib_type1_s*     sib1ptr() const { return has_sib1() ? &sib1 : nullptr; }
  const asn1::rrc::sib_type2_s*     sib2ptr() const { return has_sib2() ? &sib2 : nullptr; }
  const asn1::rrc::sib_type3_s*     sib3ptr() const { return has_sib3() ? &sib3 : nullptr; }
  const asn1::rrc::sib_type13_r9_s* sib13ptr() const { return has_sib13() ? &sib13 : nullptr; }

  uint32_t get_cell_id() const { return (uint32_t)sib1.cell_access_related_info.cell_id.to_number(); }

  bool has_sibs(srslte::span<const uint32_t> indexes) const;
  bool has_sib(uint32_t index) const;
  bool has_sib1() const { return has_valid_sib1; }
  bool has_sib2() const { return has_valid_sib2; }
  bool has_sib3() const { return has_valid_sib3; }
  bool has_sib13() const { return has_valid_sib13; }

  void reset_sibs()
  {
    has_valid_sib1  = false;
    has_valid_sib2  = false;
    has_valid_sib3  = false;
    has_valid_sib13 = false;
  }

  uint16_t get_mcc() const;
  uint16_t get_mnc() const;

  std::string to_string() const;

  bool is_sib_scheduled(uint32_t sib_index) const;

  phy_cell_t                 phy_cell = {0, 0, 0};
  bool                       has_mcch = false;
  asn1::rrc::sib_type1_s     sib1     = {};
  asn1::rrc::sib_type2_s     sib2     = {};
  asn1::rrc::sib_type3_s     sib3     = {};
  asn1::rrc::sib_type13_r9_s sib13    = {};
  asn1::rrc::mcch_msg_s      mcch     = {};
  srslte::unique_timer       timer;

private:
  float rsrp = NAN;
  float rsrq = NAN;

  bool                         has_valid_sib1  = false;
  bool                         has_valid_sib2  = false;
  bool                         has_valid_sib3  = false;
  bool                         has_valid_sib13 = false;
  std::map<uint32_t, uint32_t> sib_info_map; ///< map of sib_index to index of schedInfoList in SIB1
};

//! Universal methods to extract pci/earfcn and compare the two values
template <typename T>
uint32_t get_pci(const T& t)
{
  return t.pci;
}
template <>
inline uint32_t get_pci(const meas_cell& t)
{
  return t.get_pci();
}
template <typename T>
uint32_t get_earfcn(const T& t)
{
  return t.earfcn;
}
template <>
inline uint32_t get_earfcn(const meas_cell& t)
{
  return t.get_earfcn();
}
template <typename T, typename U>
bool is_same_cell(const T& lhs, const U& rhs)
{
  return get_pci(lhs) == get_pci(rhs) and get_earfcn(lhs) == get_earfcn(rhs);
}

class meas_cell_list
{
  using phy_meas_t = rrc_interface_phy_lte::phy_meas_t;

public:
  const static int                   NEIGHBOUR_TIMEOUT   = 5;
  const static int                   MAX_NEIGHBOUR_CELLS = 8;
  typedef std::unique_ptr<meas_cell> unique_meas_cell;

  explicit meas_cell_list(srslte::task_sched_handle task_sched_);

  bool             add_meas_cell(const phy_meas_t& meas);
  bool             add_meas_cell(unique_meas_cell cell);
  void             rem_last_neighbour();
  unique_meas_cell remove_neighbour_cell(uint32_t earfcn, uint32_t pci);
  void             clean_neighbours();
  void             sort_neighbour_cells();

  bool process_new_cell_meas(const std::vector<phy_meas_t>&                            meas,
                             const std::function<void(meas_cell&, const phy_meas_t&)>& filter_meas);

  meas_cell*         get_neighbour_cell_handle(uint32_t earfcn, uint32_t pci);
  const meas_cell*   get_neighbour_cell_handle(uint32_t earfcn, uint32_t pci) const;
  void               log_neighbour_cells() const;
  std::string        print_neighbour_cells() const;
  std::set<uint32_t> get_neighbour_pcis(uint32_t earfcn) const;
  bool               has_neighbour_cell(uint32_t earfcn, uint32_t pci) const;
  size_t             nof_neighbours() const { return neighbour_cells.size(); }
  meas_cell&         operator[](size_t idx) { return *neighbour_cells[idx]; }
  const meas_cell&   operator[](size_t idx) const { return *neighbour_cells[idx]; }
  meas_cell&         at(size_t idx) { return *neighbour_cells.at(idx); }
  meas_cell*         find_cell(uint32_t earfcn, uint32_t pci);

  // serving cell handling
  int set_serving_cell(phy_cell_t phy_cell, bool discard_serving);

  meas_cell&       serving_cell() { return *serv_cell; }
  const meas_cell& serving_cell() const { return *serv_cell; }

  using iterator = std::vector<unique_meas_cell>::iterator;
  iterator begin() { return neighbour_cells.begin(); }
  iterator end() { return neighbour_cells.end(); }

private:
  bool add_neighbour_cell_unsorted(unique_meas_cell cell);

  // args
  srslte::log_ref           log_h{"RRC"};
  srslte::task_sched_handle task_sched;

  unique_meas_cell              serv_cell;
  std::vector<unique_meas_cell> neighbour_cells;
};

} // namespace srsue

#endif // SRSLTE_RRC_CELL_H
