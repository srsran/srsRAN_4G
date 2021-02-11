/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2020 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srslte/srslte.h"

#include "pdcp_interface_types.h"
#include "rlc_interface_types.h"
#include "rrc_interface_types.h"
#include "srslte/asn1/rrc_utils.h"
#include "srslte/common/common.h"
#include "srslte/common/interfaces_common.h"
#include "srslte/common/security.h"
#include "srslte/interfaces/sched_interface.h"
#include <map>
#include <vector>

#ifndef SRSLTE_ENB_INTERFACES_H
#define SRSLTE_ENB_INTERFACES_H

namespace srsenb {

/* Interface MAC -> PHY */
class phy_interface_mac_lte
{
public:
  /**
   * Removes an RNTI context from all the physical layer components, including secondary cells
   * @param rnti identifier of the user
   */
  virtual void rem_rnti(uint16_t rnti) = 0;

  /**
   * Pregenerates the scrambling sequences for a given RNTI.
   * WARNING: This function make take several ms to complete.
   *
   * @param rnti identifier of the user
   */
  virtual int pregen_sequences(uint16_t rnti) = 0;

  /**
   *
   * @param stop
   */
  virtual void set_mch_period_stop(uint32_t stop) = 0;

  /**
   * Activates and/or deactivates Secondary Cells in the PHY for a given RNTI. Requires the RNTI of the given UE and a
   * vector with the activation/deactivation values. Use true for activation and false for deactivation. The index 0 is
   * reserved for PCell and will not be used.
   *
   * @param rnti identifier of the user
   * @param activation vector with the activate/deactivate.
   */
  virtual void set_activation_deactivation_scell(uint16_t                                     rnti,
                                                 const std::array<bool, SRSLTE_MAX_CARRIERS>& activation) = 0;
};

/* Interface RRC -> PHY */
class phy_interface_rrc_lte
{
public:
  srslte::phy_cfg_mbsfn_t mbsfn_cfg;

  virtual void configure_mbsfn(srslte::sib2_mbms_t* sib2, srslte::sib13_t* sib13, const srslte::mcch_msg_t& mcch) = 0;

  struct phy_rrc_cfg_t {
    bool              configured = false; ///< Indicates whether PHY shall consider configuring this cell/carrier
    uint32_t          enb_cc_idx = 0;     ///< eNb Cell index
    srslte::phy_cfg_t phy_cfg    = {};    ///< Dedicated physical layer configuration
  };

  typedef std::vector<phy_rrc_cfg_t> phy_rrc_cfg_list_t;

  /**
   * Sets the physical layer dedicated configuration for a given RNTI. The dedicated configuration list shall provide
   * all the required information configuration for the following cases:
   * - Add an RNTI straight from RRC
   * - Moving primary to another serving cell
   * - Add/Remove secondary serving cells
   *
   * Remind this call will partially reconfigure the primary serving cell, `complete_config``shall be called
   * in order to complete the configuration.
   *
   * @param rnti the given RNTI
   * @param phy_cfg_list Physical layer configuration for the indicated eNb cell
   */
  virtual void set_config(uint16_t rnti, const phy_rrc_cfg_list_t& phy_cfg_list) = 0;

  /**
   * Instructs the physical layer the configuration has been complete from upper layers for a given RNTI
   *
   * @param rnti the given UE identifier (RNTI)
   */
  virtual void complete_config(uint16_t rnti) = 0;
};

// RLC interface for MAC
class rlc_interface_mac
{
public:
  /* MAC calls RLC to get RLC segment of nof_bytes length.
   * Segmentation happens in this function. RLC PDU is stored in payload. */
  virtual int read_pdu(uint16_t rnti, uint32_t lcid, uint8_t* payload, uint32_t nof_bytes) = 0;

  virtual void read_pdu_pcch(uint8_t* payload, uint32_t buffer_size) = 0;

  /* MAC calls RLC to push an RLC PDU. This function is called from an independent MAC thread.
   * PDU gets placed into the buffer and higher layer thread gets notified. */
  virtual void write_pdu(uint16_t rnti, uint32_t lcid, uint8_t* payload, uint32_t nof_bytes) = 0;
};

// RLC interface for PDCP
class rlc_interface_pdcp
{
public:
  /* PDCP calls RLC to push an RLC SDU. SDU gets placed into the RLC buffer and MAC pulls
   * RLC PDUs according to TB size. */
  virtual void write_sdu(uint16_t rnti, uint32_t lcid, srslte::unique_byte_buffer_t sdu) = 0;
  virtual void discard_sdu(uint16_t rnti, uint32_t lcid, uint32_t sn)                    = 0;
  virtual bool rb_is_um(uint16_t rnti, uint32_t lcid)                                    = 0;
  virtual bool sdu_queue_is_full(uint16_t rnti, uint32_t lcid)                           = 0;
};

// RLC interface for RRC
class rlc_interface_rrc
{
public:
  virtual void clear_buffer(uint16_t rnti)                                               = 0;
  virtual void add_user(uint16_t rnti)                                                   = 0;
  virtual void rem_user(uint16_t rnti)                                                   = 0;
  virtual void add_bearer(uint16_t rnti, uint32_t lcid, srslte::rlc_config_t cnfg)       = 0;
  virtual void add_bearer_mrb(uint16_t rnti, uint32_t lcid)                              = 0;
  virtual void del_bearer(uint16_t rnti, uint32_t lcid)                                  = 0;
  virtual void write_sdu(uint16_t rnti, uint32_t lcid, srslte::unique_byte_buffer_t sdu) = 0;
  virtual bool has_bearer(uint16_t rnti, uint32_t lcid)                                  = 0;
  virtual bool suspend_bearer(uint16_t rnti, uint32_t lcid)                              = 0;
  virtual bool resume_bearer(uint16_t rnti, uint32_t lcid)                               = 0;
  virtual void reestablish(uint16_t rnti)                                                = 0;
};

// PDCP interface for GTPU
class pdcp_interface_gtpu
{
public:
  virtual void write_sdu(uint16_t rnti, uint32_t lcid, srslte::unique_byte_buffer_t sdu, int pdcp_sn = -1) = 0;
  virtual std::map<uint32_t, srslte::unique_byte_buffer_t> get_buffered_pdus(uint16_t rnti, uint32_t lcid) = 0;
};

// PDCP interface for RRC
class pdcp_interface_rrc
{
public:
  virtual void reset(uint16_t rnti)                                                                        = 0;
  virtual void add_user(uint16_t rnti)                                                                     = 0;
  virtual void rem_user(uint16_t rnti)                                                                     = 0;
  virtual void write_sdu(uint16_t rnti, uint32_t lcid, srslte::unique_byte_buffer_t sdu, int pdcp_sn = -1) = 0;
  virtual void add_bearer(uint16_t rnti, uint32_t lcid, srslte::pdcp_config_t cnfg)                        = 0;
  virtual void del_bearer(uint16_t rnti, uint32_t lcid)                                                    = 0;
  virtual void config_security(uint16_t rnti, uint32_t lcid, srslte::as_security_config_t sec_cfg)         = 0;
  virtual void enable_integrity(uint16_t rnti, uint32_t lcid)                                              = 0;
  virtual void enable_encryption(uint16_t rnti, uint32_t lcid)                                             = 0;
  virtual bool get_bearer_state(uint16_t rnti, uint32_t lcid, srslte::pdcp_lte_state_t* state)             = 0;
  virtual bool set_bearer_state(uint16_t rnti, uint32_t lcid, const srslte::pdcp_lte_state_t& state)       = 0;
  virtual void reestablish(uint16_t rnti)                                                                  = 0;
};

// PDCP interface for RLC
class pdcp_interface_rlc
{
public:
  /* RLC calls PDCP to push a PDCP PDU. */
  virtual void write_pdu(uint16_t rnti, uint32_t lcid, srslte::unique_byte_buffer_t pdu)            = 0;
  virtual void notify_delivery(uint16_t rnti, uint32_t lcid, const std::vector<uint32_t>& pdcp_sns) = 0;
};

// GTPU interface for PDCP
class gtpu_interface_pdcp
{
public:
  virtual void write_pdu(uint16_t rnti, uint32_t lcid, srslte::unique_byte_buffer_t pdu) = 0;
};

// GTPU interface for RRC
class gtpu_interface_rrc
{
public:
  struct bearer_props {
    bool     forward_from_teidin_present = false;
    bool     flush_before_teidin_present = false;
    uint32_t forward_from_teidin         = 0;
    uint32_t flush_before_teidin         = 0;
  };

  virtual uint32_t
               add_bearer(uint16_t rnti, uint32_t lcid, uint32_t addr, uint32_t teid_out, const bearer_props* props = nullptr) = 0;
  virtual void set_tunnel_status(uint32_t teidin, bool dl_active)    = 0;
  virtual void rem_bearer(uint16_t rnti, uint32_t lcid)              = 0;
  virtual void mod_bearer_rnti(uint16_t old_rnti, uint16_t new_rnti) = 0;
  virtual void rem_user(uint16_t rnti)                               = 0;
};

// Combined interface for stack (MAC and RRC) to access PHY
class phy_interface_stack_lte : public phy_interface_mac_lte, public phy_interface_rrc_lte
{};

class stack_interface_phy_lte;

class stack_interface_s1ap_lte
{
public:
  virtual void add_mme_socket(int fd)    = 0;
  virtual void remove_mme_socket(int fd) = 0;
};

class stack_interface_gtpu_lte
{
public:
  virtual void add_gtpu_s1u_socket_handler(int fd) = 0;
  virtual void add_gtpu_m1u_socket_handler(int fd) = 0;
};

} // namespace srsenb

#endif // SRSLTE_ENB_INTERFACES_H
