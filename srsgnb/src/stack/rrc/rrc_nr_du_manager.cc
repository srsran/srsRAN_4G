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

#include "srsgnb/hdr/stack/rrc/rrc_nr_du_manager.h"
#include "srsgnb/hdr/stack/rrc/cell_asn1_config.h"
#include "srsran/common/string_helpers.h"

using namespace asn1::rrc_nr;

namespace srsenb {

du_config_manager::du_config_manager(const rrc_nr_cfg_t& cfg_) : cfg(cfg_), logger(srslog::fetch_basic_logger("RRC-NR"))
{}

du_config_manager::~du_config_manager() {}

int du_config_manager::add_cell()
{
  // add cell
  std::unique_ptr<du_cell_config> obj  = std::make_unique<du_cell_config>();
  du_cell_config&                 cell = *obj;
  cell.cc                              = cells.size();

  // Fill general cell params
  cell.pci = cfg.cell_list[cell.cc].phy_cell.carrier.pci;

  // fill MIB ASN.1
  if (fill_mib_from_enb_cfg(cfg.cell_list[cell.cc], cell.mib) != SRSRAN_SUCCESS) {
    return SRSRAN_ERROR;
  }

  // Pack MIB
  cell.packed_mib = srsran::make_byte_buffer();
  if (cell.packed_mib == nullptr) {
    logger.error("Couldn't allocate PDU in %s().", __FUNCTION__);
    return SRSRAN_ERROR;
  }
  {
    asn1::bit_ref  bref(cell.packed_mib->msg, cell.packed_mib->get_tailroom());
    bcch_bch_msg_s bch_msg;
    bch_msg.msg.set_mib() = cell.mib;
    if (bch_msg.pack(bref) != asn1::SRSASN_SUCCESS) {
      logger.error("Couldn't pack mib msg");
      return SRSRAN_ERROR;
    }
    cell.packed_mib->N_bytes = bref.distance_bytes();
  }
  logger.info(
      cell.packed_mib->data(), cell.packed_mib->size(), "BCCH-BCH Message (with MIB) (%d B)", cell.packed_mib->size());
  asn1::json_writer js;
  cell.mib.to_json(js);
  logger.info("MIB content: %s", js.to_string().c_str());

  // fill SIB1 ASN.1
  if (fill_sib1_from_enb_cfg(cfg, cell.cc, cell.sib1) != SRSRAN_SUCCESS) {
    logger.error("Couldn't generate SIB1");
    return SRSRAN_ERROR;
  }

  // Pack SIB1
  cell.packed_sib1 = srsran::make_byte_buffer();
  if (cell.packed_sib1 == nullptr) {
    logger.error("Couldn't allocate PDU in %s().", __FUNCTION__);
    return SRSRAN_ERROR;
  }
  {
    asn1::bit_ref     bref(cell.packed_sib1->msg, cell.packed_sib1->get_tailroom());
    bcch_dl_sch_msg_s bcch_msg;
    bcch_msg.msg.set_c1().set_sib_type1() = cell.sib1;
    if (bcch_msg.pack(bref) != asn1::SRSASN_SUCCESS) {
      logger.error("Couldn't pack SIB1 msg");
      return SRSRAN_ERROR;
    }
    cell.packed_sib1->N_bytes = bref.distance_bytes();
  }
  logger.info(cell.packed_sib1->data(),
              cell.packed_sib1->size(),
              "BCCH-DL-SCH-Message (with SIB1) (%d B)",
              cell.packed_sib1->size());
  cell.sib1.to_json(js);
  logger.info("SIB1 content: %s", js.to_string().c_str());

  cells.push_back(std::move(obj));
  return SRSRAN_SUCCESS;
}

} // namespace srsenb