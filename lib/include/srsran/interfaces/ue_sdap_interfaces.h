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

/******************************************************************************
 * File:        ue_sdap_interfaces.h
 * Description: Abstract base class interfaces for SDAP layer
 *****************************************************************************/

#ifndef SRSRAN_UE_SDAP_INTERFACES_H
#define SRSRAN_UE_SDAP_INTERFACES_H

/*****************************
 *      SDAP INTERFACES
 ****************************/
class sdap_interface_pdcp_nr
{
public:
  virtual void write_pdu(uint32_t lcid, srsran::unique_byte_buffer_t pdu) = 0;
};
class sdap_interface_gw_nr
{
public:
  virtual void write_sdu(uint32_t lcid, srsran::unique_byte_buffer_t pdu) = 0;
};

class sdap_interface_rrc
{
public:
  struct bearer_cfg_t {
    bool     is_data;
    bool     add_downlink_header;
    bool     add_uplink_header;
    uint32_t qfi;
  };
  virtual bool set_bearer_cfg(uint32_t lcid, const bearer_cfg_t& cfg) = 0;
};

#endif // SRSRAN_UE_SDAP_INTERFACES_H
