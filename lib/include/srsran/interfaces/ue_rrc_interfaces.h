/**
 * Copyright 2013-2022 Software Radio Systems Limited
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

#ifndef SRSRAN_UE_RRC_INTERFACES_H
#define SRSRAN_UE_RRC_INTERFACES_H

#include "phy_interface_types.h"
#include "rrc_interface_types.h"
#include "srsran/asn1/asn1_utils.h"
#include "srsran/asn1/rrc_nr.h"
#include "srsran/common/byte_buffer.h"
#include "srsran/common/tti_point.h"

namespace srsue {

class rrc_interface_mac
{
public:
  virtual void ra_completed()      = 0;
  virtual void ra_problem()        = 0;
  virtual void release_pucch_srs() = 0;
};

class rrc_eutra_interface_rrc_nr
{
public:
  virtual void new_cell_meas_nr(const std::vector<phy_meas_nr_t>& meas)            = 0;
  virtual void nr_rrc_con_reconfig_complete(bool status)                           = 0;
  virtual void nr_notify_reconfiguration_failure()                                 = 0;
  virtual void nr_scg_failure_information(const srsran::scg_failure_cause_t cause) = 0;
};

class rrc_interface_phy_lte
{
public:
  virtual void in_sync()                                          = 0;
  virtual void out_of_sync()                                      = 0;
  virtual void new_cell_meas(const std::vector<phy_meas_t>& meas) = 0;

  typedef struct {
    enum { CELL_FOUND = 0, CELL_NOT_FOUND, ERROR } found;
    enum { MORE_FREQS = 0, NO_MORE_FREQS } last_freq;
  } cell_search_ret_t;

  virtual void cell_search_complete(cell_search_ret_t ret, phy_cell_t found_cell) = 0;
  virtual void cell_select_complete(bool status)                                  = 0;
  virtual void set_config_complete(bool status)                                   = 0;
  virtual void set_scell_complete(bool status)                                    = 0;
};

class rrc_interface_nas
{
public:
  virtual ~rrc_interface_nas()                                                          = default;
  virtual void        write_sdu(srsran::unique_byte_buffer_t sdu)                       = 0;
  virtual uint16_t    get_mcc()                                                         = 0;
  virtual uint16_t    get_mnc()                                                         = 0;
  virtual void        enable_capabilities()                                             = 0;
  virtual bool        plmn_search()                                                     = 0;
  virtual void        plmn_select(srsran::plmn_id_t plmn_id)                            = 0;
  virtual bool        connection_request(srsran::establishment_cause_t cause,
                                         srsran::unique_byte_buffer_t  dedicatedInfoNAS) = 0;
  virtual void        set_ue_identity(srsran::s_tmsi_t s_tmsi)                          = 0;
  virtual bool        is_connected()                                                    = 0;
  virtual void        paging_completed(bool outcome)                                    = 0;
  virtual const char* get_rb_name(uint32_t lcid)                                        = 0;
  virtual bool        has_nr_dc()                                                       = 0;
};

class rrc_interface_pdcp
{
public:
  virtual void        write_pdu(uint32_t lcid, srsran::unique_byte_buffer_t pdu)     = 0;
  virtual void        write_pdu_bcch_bch(srsran::unique_byte_buffer_t pdu)           = 0;
  virtual void        write_pdu_bcch_dlsch(srsran::unique_byte_buffer_t pdu)         = 0;
  virtual void        write_pdu_pcch(srsran::unique_byte_buffer_t pdu)               = 0;
  virtual void        write_pdu_mch(uint32_t lcid, srsran::unique_byte_buffer_t pdu) = 0;
  virtual void        notify_pdcp_integrity_error(uint32_t lcid)                     = 0;
  virtual const char* get_rb_name(uint32_t lcid)                                     = 0;
};

class rrc_interface_rlc
{
public:
  virtual void        max_retx_attempted()                                       = 0;
  virtual void        protocol_failure()                                         = 0;
  virtual const char* get_rb_name(uint32_t lcid)                                 = 0;
  virtual void        write_pdu(uint32_t lcid, srsran::unique_byte_buffer_t pdu) = 0;
};

class rrc_nr_interface_rrc
{
public:
  virtual int  get_eutra_nr_capabilities(srsran::byte_buffer_t* eutra_nr_caps)                                    = 0;
  virtual int  get_nr_capabilities(srsran::byte_buffer_t* nr_cap)                                                 = 0;
  virtual void phy_set_cells_to_meas(uint32_t carrier_freq_r15)                                                   = 0;
  virtual void phy_meas_stop()                                                                                    = 0;
  virtual bool rrc_reconfiguration(bool endc_release_and_add_r15, const asn1::rrc_nr::rrc_recfg_s& rrc_nr_reconf) = 0;
  virtual void rrc_release()                                                                                      = 0;
  virtual bool is_config_pending()                                                                                = 0;
};

class rrc_nr_interface_nas_5g
{
public:
  virtual ~rrc_nr_interface_nas_5g()                           = default;
  virtual int      write_sdu(srsran::unique_byte_buffer_t sdu) = 0;
  virtual bool     is_connected()                              = 0;
  virtual int      connection_request(srsran::nr_establishment_cause_t cause, srsran::unique_byte_buffer_t sdu) = 0;
  virtual uint16_t get_mcc()                                                                                    = 0;
  virtual uint16_t get_mnc()                                                                                    = 0;
};
} // namespace srsue

#endif // SRSRAN_UE_RRC_INTERFACES_H
