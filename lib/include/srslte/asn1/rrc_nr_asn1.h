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

/*******************************************************************************
 *
 *                    3GPP TS ASN1 RRC NR v15.5.0 (2019-03)
 *
 ******************************************************************************/

#ifndef SRSASN1_RRC_NR_H
#define SRSASN1_RRC_NR_H

#include "asn1_utils.h"
#include <cstdio>
#include <stdarg.h>

namespace asn1 {
namespace rrc_nr {

/*******************************************************************************
 *                        Functions for external logging
 ******************************************************************************/

void log_invalid_access_choice_id(uint32_t val, uint32_t choice_id);

void assert_choice_type(uint32_t val, uint32_t choice_id);

void assert_choice_type(const std::string& access_type,
                        const std::string& current_type,
                        const std::string& choice_type);

const char* convert_enum_idx(const char* array[], uint32_t nof_types, uint32_t enum_val, const char* enum_type);

template <class ItemType>
ItemType map_enum_number(ItemType* array, uint32_t nof_types, uint32_t enum_val, const char* enum_type);

/*******************************************************************************
 *                             Constant Definitions
 ******************************************************************************/

#define ASN1_RRC_NR_MAX_BAND_COMB 65536
#define ASN1_RRC_NR_MAX_CELL_BLACK 16
#define ASN1_RRC_NR_MAX_CELL_INTER 16
#define ASN1_RRC_NR_MAX_CELL_INTRA 16
#define ASN1_RRC_NR_MAX_CELL_MEAS_EUTRA 32
#define ASN1_RRC_NR_MAX_EARFCN 262143
#define ASN1_RRC_NR_MAX_EUTRA_CELL_BLACK 16
#define ASN1_RRC_NR_MAX_EUTRA_NS_PMAX 8
#define ASN1_RRC_NR_MAX_MULTI_BANDS 8
#define ASN1_RRC_NR_MAX_NARFCN 3279165
#define ASN1_RRC_NR_MAX_NR_NS_PMAX 8
#define ASN1_RRC_NR_MAX_NROF_SERVING_CELLS 32
#define ASN1_RRC_NR_MAX_NROF_SERVING_CELLS_MINUS1 31
#define ASN1_RRC_NR_MAX_NROF_AGGREGATED_CELLS_PER_CELL_GROUP 16
#define ASN1_RRC_NR_MAX_NROF_SCELLS 31
#define ASN1_RRC_NR_MAX_NROF_CELL_MEAS 32
#define ASN1_RRC_NR_MAX_NROF_SS_BLOCKS_TO_AVERAGE 16
#define ASN1_RRC_NR_MAX_NROF_CSI_RS_RES_TO_AVERAGE 16
#define ASN1_RRC_NR_MAX_NROF_DL_ALLOCS 16
#define ASN1_RRC_NR_MAX_NROF_SR_CFG_PER_CELL_GROUP 8
#define ASN1_RRC_NR_MAX_LCG_ID 7
#define ASN1_RRC_NR_MAX_LC_ID 32
#define ASN1_RRC_NR_MAX_NROF_TAGS 4
#define ASN1_RRC_NR_MAX_NROF_TAGS_MINUS1 3
#define ASN1_RRC_NR_MAX_NROF_BWPS 4
#define ASN1_RRC_NR_MAX_NROF_COMB_IDC 128
#define ASN1_RRC_NR_MAX_NROF_SYMBOLS_MINUS1 13
#define ASN1_RRC_NR_MAX_NROF_SLOTS 320
#define ASN1_RRC_NR_MAX_NROF_SLOTS_MINUS1 319
#define ASN1_RRC_NR_MAX_NROF_PHYS_RES_BLOCKS 275
#define ASN1_RRC_NR_MAX_NROF_PHYS_RES_BLOCKS_MINUS1 274
#define ASN1_RRC_NR_MAX_NROF_PHYS_RES_BLOCKS_PLUS1 276
#define ASN1_RRC_NR_MAX_NROF_CTRL_RES_SETS_MINUS1 11
#define ASN1_RRC_NR_MAX_CO_RE_SET_DUR 3
#define ASN1_RRC_NR_MAX_NROF_SEARCH_SPACES_MINUS1 39
#define ASN1_RRC_NR_MAX_SFI_DCI_PAYLOAD_SIZE 128
#define ASN1_RRC_NR_MAX_SFI_DCI_PAYLOAD_SIZE_MINUS1 127
#define ASN1_RRC_NR_MAX_INT_DCI_PAYLOAD_SIZE 126
#define ASN1_RRC_NR_MAX_INT_DCI_PAYLOAD_SIZE_MINUS1 125
#define ASN1_RRC_NR_MAX_NROF_RATE_MATCH_PATTERNS 4
#define ASN1_RRC_NR_MAX_NROF_RATE_MATCH_PATTERNS_MINUS1 3
#define ASN1_RRC_NR_MAX_NROF_RATE_MATCH_PATTERNS_PER_GROUP 8
#define ASN1_RRC_NR_MAX_NROF_CSI_REPORT_CFGS 48
#define ASN1_RRC_NR_MAX_NROF_CSI_REPORT_CFGS_MINUS1 47
#define ASN1_RRC_NR_MAX_NROF_CSI_RES_CFGS 112
#define ASN1_RRC_NR_MAX_NROF_CSI_RES_CFGS_MINUS1 111
#define ASN1_RRC_NR_MAX_NROF_AP_CSI_RS_RES_PER_SET 16
#define ASN1_RRC_NR_MAX_NR_OF_CSI_APERIODIC_TRIGGERS 128
#define ASN1_RRC_NR_MAX_NROF_REPORT_CFG_PER_APERIODIC_TRIGGER 16
#define ASN1_RRC_NR_MAX_NROF_NZP_CSI_RS_RES 192
#define ASN1_RRC_NR_MAX_NROF_NZP_CSI_RS_RES_MINUS1 191
#define ASN1_RRC_NR_MAX_NROF_NZP_CSI_RS_RES_PER_SET 64
#define ASN1_RRC_NR_MAX_NROF_NZP_CSI_RS_RES_SETS 64
#define ASN1_RRC_NR_MAX_NROF_NZP_CSI_RS_RES_SETS_MINUS1 63
#define ASN1_RRC_NR_MAX_NROF_NZP_CSI_RS_RES_SETS_PER_CFG 16
#define ASN1_RRC_NR_MAX_NROF_NZP_CSI_RS_RES_PER_CFG 128
#define ASN1_RRC_NR_MAX_NROF_ZP_CSI_RS_RES 32
#define ASN1_RRC_NR_MAX_NROF_ZP_CSI_RS_RES_MINUS1 31
#define ASN1_RRC_NR_MAX_NROF_ZP_CSI_RS_RES_SETS_MINUS1 15
#define ASN1_RRC_NR_MAX_NROF_ZP_CSI_RS_RES_PER_SET 16
#define ASN1_RRC_NR_MAX_NROF_ZP_CSI_RS_RES_SETS 16
#define ASN1_RRC_NR_MAX_NROF_CSI_IM_RES 32
#define ASN1_RRC_NR_MAX_NROF_CSI_IM_RES_MINUS1 31
#define ASN1_RRC_NR_MAX_NROF_CSI_IM_RES_PER_SET 8
#define ASN1_RRC_NR_MAX_NROF_CSI_IM_RES_SETS 64
#define ASN1_RRC_NR_MAX_NROF_CSI_IM_RES_SETS_MINUS1 63
#define ASN1_RRC_NR_MAX_NROF_CSI_IM_RES_SETS_PER_CFG 16
#define ASN1_RRC_NR_MAX_NROF_CSI_SSB_RES_PER_SET 64
#define ASN1_RRC_NR_MAX_NROF_CSI_SSB_RES_SETS 64
#define ASN1_RRC_NR_MAX_NROF_CSI_SSB_RES_SETS_MINUS1 63
#define ASN1_RRC_NR_MAX_NROF_CSI_SSB_RES_SETS_PER_CFG 1
#define ASN1_RRC_NR_MAX_NROF_FAIL_DETECTION_RES 10
#define ASN1_RRC_NR_MAX_NROF_FAIL_DETECTION_RES_MINUS1 9
#define ASN1_RRC_NR_MAX_NROF_OBJ_ID 64
#define ASN1_RRC_NR_MAX_NROF_PAGE_REC 32
#define ASN1_RRC_NR_MAX_NROF_PCI_RANGES 8
#define ASN1_RRC_NR_MAX_PLMN 12
#define ASN1_RRC_NR_MAX_NROF_CSI_RS_RES_RRM 96
#define ASN1_RRC_NR_MAX_NROF_CSI_RS_RES_RRM_MINUS1 95
#define ASN1_RRC_NR_MAX_NROF_MEAS_ID 64
#define ASN1_RRC_NR_MAX_NROF_QUANT_CFG 2
#define ASN1_RRC_NR_MAX_NROF_CSI_RS_CELLS_RRM 96
#define ASN1_RRC_NR_MAX_NROF_SRS_RES_SETS 16
#define ASN1_RRC_NR_MAX_NROF_SRS_RES_SETS_MINUS1 15
#define ASN1_RRC_NR_MAX_NROF_SRS_RES 64
#define ASN1_RRC_NR_MAX_NROF_SRS_RES_MINUS1 63
#define ASN1_RRC_NR_MAX_NROF_SRS_RES_PER_SET 16
#define ASN1_RRC_NR_MAX_NROF_SRS_TRIGGER_STATES_MINUS1 3
#define ASN1_RRC_NR_MAX_NROF_SRS_TRIGGER_STATES_MINUS2 2
#define ASN1_RRC_NR_MAX_RAT_CAP_CONTAINERS 8
#define ASN1_RRC_NR_MAX_SIMUL_BANDS 32
#define ASN1_RRC_NR_MAX_NROF_SLOT_FORMAT_COMBINATIONS_PER_SET 512
#define ASN1_RRC_NR_MAX_NROF_SLOT_FORMAT_COMBINATIONS_PER_SET_MINUS1 511
#define ASN1_RRC_NR_MAX_NROF_PUCCH_RES 128
#define ASN1_RRC_NR_MAX_NROF_PUCCH_RES_MINUS1 127
#define ASN1_RRC_NR_MAX_NROF_PUCCH_RES_SETS 4
#define ASN1_RRC_NR_MAX_NROF_PUCCH_RES_SETS_MINUS1 3
#define ASN1_RRC_NR_MAX_NROF_PUCCH_RES_PER_SET 32
#define ASN1_RRC_NR_MAX_NROF_PUCCH_P0_PER_SET 8
#define ASN1_RRC_NR_MAX_NROF_PUCCH_PATHLOSS_REF_RSS 4
#define ASN1_RRC_NR_MAX_NROF_PUCCH_PATHLOSS_REF_RSS_MINUS1 3
#define ASN1_RRC_NR_MAX_NROF_P0_PUSCH_ALPHA_SETS 30
#define ASN1_RRC_NR_MAX_NROF_P0_PUSCH_ALPHA_SETS_MINUS1 29
#define ASN1_RRC_NR_MAX_NROF_PUSCH_PATHLOSS_REF_RSS 4
#define ASN1_RRC_NR_MAX_NROF_PUSCH_PATHLOSS_REF_RSS_MINUS1 3
#define ASN1_RRC_NR_MAX_NROF_NAICS_ENTRIES 8
#define ASN1_RRC_NR_MAX_BANDS 1024
#define ASN1_RRC_NR_MAX_BANDS_MRDC 1280
#define ASN1_RRC_NR_MAX_BANDS_EUTRA 256
#define ASN1_RRC_NR_MAX_CELL_REPORT 8
#define ASN1_RRC_NR_MAX_DRB 29
#define ASN1_RRC_NR_MAX_FREQ 8
#define ASN1_RRC_NR_MAX_FREQ_IDC_MRDC 32
#define ASN1_RRC_NR_MAX_NROF_CSI_RS 64
#define ASN1_RRC_NR_MAX_NROF_CANDIDATE_BEAMS 16
#define ASN1_RRC_NR_MAX_NROF_PCIS_PER_SMTC 64
#define ASN1_RRC_NR_MAX_NROF_QFIS 64
#define ASN1_RRC_NR_MAX_NR_OF_SEMI_PERSISTENT_PUSCH_TRIGGERS 64
#define ASN1_RRC_NR_MAX_NROF_SR_RES 8
#define ASN1_RRC_NR_MAX_NROF_SLOT_FORMATS_PER_COMBINATION 256
#define ASN1_RRC_NR_MAX_NROF_SPATIAL_RELATION_INFOS 8
#define ASN1_RRC_NR_MAX_NROF_IDXES_TO_REPORT 32
#define ASN1_RRC_NR_MAX_NROF_IDXES_TO_REPORT2 64
#define ASN1_RRC_NR_MAX_NROF_SSBS 64
#define ASN1_RRC_NR_MAX_NROF_SSBS_MINUS1 63
#define ASN1_RRC_NR_MAX_NROF_S_NSSAI 8
#define ASN1_RRC_NR_MAX_NROF_TCI_STATES_PDCCH 64
#define ASN1_RRC_NR_MAX_NROF_TCI_STATES 128
#define ASN1_RRC_NR_MAX_NROF_TCI_STATES_MINUS1 127
#define ASN1_RRC_NR_MAX_NROF_UL_ALLOCS 16
#define ASN1_RRC_NR_MAX_QFI 63
#define ASN1_RRC_NR_MAX_RA_CSIRS_RES 96
#define ASN1_RRC_NR_MAX_RA_OCCASIONS_PER_CSIRS 64
#define ASN1_RRC_NR_MAX_RA_OCCASIONS_MINUS1 511
#define ASN1_RRC_NR_MAX_RA_SSB_RES 64
#define ASN1_RRC_NR_MAX_SCSS 5
#define ASN1_RRC_NR_MAX_SECONDARY_CELL_GROUPS 3
#define ASN1_RRC_NR_MAX_NROF_SERVING_CELLS_EUTRA 32
#define ASN1_RRC_NR_MAX_MBSFN_ALLOCS 8
#define ASN1_RRC_NR_MAX_NROF_MULTI_BANDS 8
#define ASN1_RRC_NR_MAX_CELL_SFTD 3
#define ASN1_RRC_NR_MAX_REPORT_CFG_ID 64
#define ASN1_RRC_NR_MAX_NROF_CODEBOOKS 16
#define ASN1_RRC_NR_MAX_NROF_CSI_RS_RES 7
#define ASN1_RRC_NR_MAX_NROF_SRI_PUSCH_MAPS 16
#define ASN1_RRC_NR_MAX_NROF_SRI_PUSCH_MAPS_MINUS1 15
#define ASN1_RRC_NR_MAX_SIB 32
#define ASN1_RRC_NR_MAX_SI_MSG 32
#define ASN1_RRC_NR_MAX_PO_PER_PF 4
#define ASN1_RRC_NR_MAX_ACCESS_CAT_MINUS1 63
#define ASN1_RRC_NR_MAX_BARR_INFO_SET 8
#define ASN1_RRC_NR_MAX_CELL_EUTRA 8
#define ASN1_RRC_NR_MAX_EUTRA_CARRIER 8
#define ASN1_RRC_NR_MAX_PLMN_IDENTITIES 8
#define ASN1_RRC_NR_MAX_DL_FEATURE_SETS 1024
#define ASN1_RRC_NR_MAX_UL_FEATURE_SETS 1024
#define ASN1_RRC_NR_MAX_EUTRA_DL_FEATURE_SETS 256
#define ASN1_RRC_NR_MAX_EUTRA_UL_FEATURE_SETS 256
#define ASN1_RRC_NR_MAX_FEATURE_SETS_PER_BAND 128
#define ASN1_RRC_NR_MAX_PER_CC_FEATURE_SETS 1024
#define ASN1_RRC_NR_MAX_FEATURE_SET_COMBINATIONS 1024
#define ASN1_RRC_NR_MAX_INTER_RAT_RSTD_FREQ 3
#define ASN1_RRC_NR_MAX_MEAS_FREQS_MN 32
#define ASN1_RRC_NR_MAX_MEAS_FREQS_SN 32
#define ASN1_RRC_NR_MAX_MEAS_IDENTITIES_MN 62
#define ASN1_RRC_NR_MAX_CELL_PREP 32

/*******************************************************************************
 *                              Struct Definitions
 ******************************************************************************/

// PDCCH-ConfigSIB1 ::= SEQUENCE
struct pdcch_cfg_sib1_s {
  uint8_t ctrl_res_set_zero = 0;
  uint8_t search_space_zero = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MIB ::= SEQUENCE
struct mib_s {
  struct sub_carrier_spacing_common_opts {
    enum options { scs15or60, scs30or120, nulltype } value;
    typedef uint8_t number_type;

    std::string to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<sub_carrier_spacing_common_opts> sub_carrier_spacing_common_e_;
  struct dmrs_type_a_position_opts {
    enum options { pos2, pos3, nulltype } value;
    typedef uint8_t number_type;

    std::string to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<dmrs_type_a_position_opts> dmrs_type_a_position_e_;
  struct cell_barred_opts {
    enum options { barred, not_barred, nulltype } value;

    std::string to_string() const;
  };
  typedef enumerated<cell_barred_opts> cell_barred_e_;
  struct intra_freq_resel_opts {
    enum options { allowed, not_allowed, nulltype } value;

    std::string to_string() const;
  };
  typedef enumerated<intra_freq_resel_opts> intra_freq_resel_e_;

  // member variables
  fixed_bitstring<6>            sys_frame_num;
  sub_carrier_spacing_common_e_ sub_carrier_spacing_common;
  uint8_t                       ssb_subcarrier_offset = 0;
  dmrs_type_a_position_e_       dmrs_type_a_position;
  pdcch_cfg_sib1_s              pdcch_cfg_sib1;
  cell_barred_e_                cell_barred;
  intra_freq_resel_e_           intra_freq_resel;
  fixed_bitstring<1>            spare;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BCCH-BCH-MessageType ::= CHOICE
struct bcch_bch_msg_type_c {
  struct types_opts {
    enum options { mib, msg_class_ext, nulltype } value;

    std::string to_string() const;
  };
  typedef enumerated<types_opts> types;

  // choice methods
  bcch_bch_msg_type_c() = default;
  bcch_bch_msg_type_c(const bcch_bch_msg_type_c& other);
  bcch_bch_msg_type_c& operator=(const bcch_bch_msg_type_c& other);
  ~bcch_bch_msg_type_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  mib_s& mib()
  {
    assert_choice_type("mib", type_.to_string(), "BCCH-BCH-MessageType");
    return c.get<mib_s>();
  }
  const mib_s& mib() const
  {
    assert_choice_type("mib", type_.to_string(), "BCCH-BCH-MessageType");
    return c.get<mib_s>();
  }
  mib_s& set_mib()
  {
    set(types::mib);
    return c.get<mib_s>();
  }

private:
  types                  type_;
  choice_buffer_t<mib_s> c;

  void destroy_();
};

// BCCH-BCH-Message ::= SEQUENCE
struct bcch_bch_msg_s {
  bcch_bch_msg_type_c msg;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// EUTRA-NS-PmaxValue ::= SEQUENCE
struct eutra_ns_pmax_value_s {
  bool     add_pmax_present          = false;
  bool     add_spec_emission_present = false;
  int8_t   add_pmax                  = -30;
  uint16_t add_spec_emission         = 1;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// NR-NS-PmaxValue ::= SEQUENCE
struct nr_ns_pmax_value_s {
  bool    add_pmax_present  = false;
  int8_t  add_pmax          = -30;
  uint8_t add_spec_emission = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// EUTRA-NS-PmaxList ::= SEQUENCE (SIZE (1..8)) OF EUTRA-NS-PmaxValue
using eutra_ns_pmax_list_l = dyn_array<eutra_ns_pmax_value_s>;

// EUTRA-Q-OffsetRange ::= ENUMERATED
struct eutra_q_offset_range_opts {
  enum options {
    db_minus24,
    db_minus22,
    db_minus20,
    db_minus18,
    db_minus16,
    db_minus14,
    db_minus12,
    db_minus10,
    db_minus8,
    db_minus6,
    db_minus5,
    db_minus4,
    db_minus3,
    db_minus2,
    db_minus1,
    db0,
    db1,
    db2,
    db3,
    db4,
    db5,
    db6,
    db8,
    db10,
    db12,
    db14,
    db16,
    db18,
    db20,
    db22,
    db24,
    nulltype
  } value;
  typedef int8_t number_type;

  std::string to_string() const;
  int8_t      to_number() const;
};
typedef enumerated<eutra_q_offset_range_opts> eutra_q_offset_range_e;

// NR-NS-PmaxList ::= SEQUENCE (SIZE (1..8)) OF NR-NS-PmaxValue
using nr_ns_pmax_list_l = dyn_array<nr_ns_pmax_value_s>;

// Q-OffsetRange ::= ENUMERATED
struct q_offset_range_opts {
  enum options {
    db_minus24,
    db_minus22,
    db_minus20,
    db_minus18,
    db_minus16,
    db_minus14,
    db_minus12,
    db_minus10,
    db_minus8,
    db_minus6,
    db_minus5,
    db_minus4,
    db_minus3,
    db_minus2,
    db_minus1,
    db0,
    db1,
    db2,
    db3,
    db4,
    db5,
    db6,
    db8,
    db10,
    db12,
    db14,
    db16,
    db18,
    db20,
    db22,
    db24,
    nulltype
  } value;
  typedef int8_t number_type;

  std::string to_string() const;
  int8_t      to_number() const;
};
typedef enumerated<q_offset_range_opts> q_offset_range_e;

// EUTRA-FreqNeighCellInfo ::= SEQUENCE
struct eutra_freq_neigh_cell_info_s {
  bool                   q_rx_lev_min_offset_cell_present = false;
  bool                   q_qual_min_offset_cell_present   = false;
  uint16_t               pci                              = 0;
  eutra_q_offset_range_e q_offset_cell;
  uint8_t                q_rx_lev_min_offset_cell = 1;
  uint8_t                q_qual_min_offset_cell   = 1;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// EUTRA-MultiBandInfo ::= SEQUENCE
struct eutra_multi_band_info_s {
  bool                 eutra_ns_pmax_list_present = false;
  uint16_t             eutra_freq_band_ind        = 1;
  eutra_ns_pmax_list_l eutra_ns_pmax_list;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// EUTRA-PhysCellIdRange ::= SEQUENCE
struct eutra_pci_range_s {
  struct range_opts {
    enum options {
      n4,
      n8,
      n12,
      n16,
      n24,
      n32,
      n48,
      n64,
      n84,
      n96,
      n128,
      n168,
      n252,
      n504,
      spare2,
      spare1,
      nulltype
    } value;
    typedef uint16_t number_type;

    std::string to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<range_opts> range_e_;

  // member variables
  bool     range_present = false;
  uint16_t start         = 0;
  range_e_ range;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// InterFreqNeighCellInfo ::= SEQUENCE
struct inter_freq_neigh_cell_info_s {
  bool             ext                                  = false;
  bool             q_rx_lev_min_offset_cell_present     = false;
  bool             q_rx_lev_min_offset_cell_sul_present = false;
  bool             q_qual_min_offset_cell_present       = false;
  uint16_t         pci                                  = 0;
  q_offset_range_e q_offset_cell;
  uint8_t          q_rx_lev_min_offset_cell     = 1;
  uint8_t          q_rx_lev_min_offset_cell_sul = 1;
  uint8_t          q_qual_min_offset_cell       = 1;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// NR-MultiBandInfo ::= SEQUENCE
struct nr_multi_band_info_s {
  bool              freq_band_ind_nr_present = false;
  bool              nr_ns_pmax_list_present  = false;
  uint16_t          freq_band_ind_nr         = 1;
  nr_ns_pmax_list_l nr_ns_pmax_list;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PCI-Range ::= SEQUENCE
struct pci_range_s {
  struct range_opts {
    enum options {
      n4,
      n8,
      n12,
      n16,
      n24,
      n32,
      n48,
      n64,
      n84,
      n96,
      n128,
      n168,
      n252,
      n504,
      n1008,
      spare1,
      nulltype
    } value;
    typedef uint16_t number_type;

    std::string to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<range_opts> range_e_;

  // member variables
  bool     range_present = false;
  uint16_t start         = 0;
  range_e_ range;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDSCH-TimeDomainResourceAllocation ::= SEQUENCE
struct pdsch_time_domain_res_alloc_s {
  struct map_type_opts {
    enum options { type_a, type_b, nulltype } value;

    std::string to_string() const;
  };
  typedef enumerated<map_type_opts> map_type_e_;

  // member variables
  bool        k0_present = false;
  uint8_t     k0         = 0;
  map_type_e_ map_type;
  uint8_t     start_symbol_and_len = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PUSCH-TimeDomainResourceAllocation ::= SEQUENCE
struct pusch_time_domain_res_alloc_s {
  struct map_type_opts {
    enum options { type_a, type_b, nulltype } value;

    std::string to_string() const;
  };
  typedef enumerated<map_type_opts> map_type_e_;

  // member variables
  bool        k2_present = false;
  uint8_t     k2         = 0;
  map_type_e_ map_type;
  uint8_t     start_symbol_and_len = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CellReselectionSubPriority ::= ENUMERATED
struct cell_resel_sub_prio_opts {
  enum options { odot2, odot4, odot6, odot8, nulltype } value;
  typedef float number_type;

  std::string to_string() const;
  float       to_number() const;
  std::string to_number_string() const;
};
typedef enumerated<cell_resel_sub_prio_opts> cell_resel_sub_prio_e;

// ControlResourceSet ::= SEQUENCE
struct ctrl_res_set_s {
  struct cce_reg_map_type_c_ {
    struct interleaved_s_ {
      struct reg_bundle_size_opts {
        enum options { n2, n3, n6, nulltype } value;
        typedef uint8_t number_type;

        std::string to_string() const;
        uint8_t     to_number() const;
      };
      typedef enumerated<reg_bundle_size_opts> reg_bundle_size_e_;
      struct interleaver_size_opts {
        enum options { n2, n3, n6, nulltype } value;
        typedef uint8_t number_type;

        std::string to_string() const;
        uint8_t     to_number() const;
      };
      typedef enumerated<interleaver_size_opts> interleaver_size_e_;

      // member variables
      bool                shift_idx_present = false;
      reg_bundle_size_e_  reg_bundle_size;
      interleaver_size_e_ interleaver_size;
      uint16_t            shift_idx = 0;
    };
    struct types_opts {
      enum options { interleaved, non_interleaved, nulltype } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    cce_reg_map_type_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    interleaved_s_& interleaved()
    {
      assert_choice_type("interleaved", type_.to_string(), "cce-REG-MappingType");
      return c;
    }
    const interleaved_s_& interleaved() const
    {
      assert_choice_type("interleaved", type_.to_string(), "cce-REG-MappingType");
      return c;
    }
    interleaved_s_& set_interleaved()
    {
      set(types::interleaved);
      return c;
    }

  private:
    types          type_;
    interleaved_s_ c;
  };
  struct precoder_granularity_opts {
    enum options { same_as_reg_bundle, all_contiguous_rbs, nulltype } value;

    std::string to_string() const;
  };
  typedef enumerated<precoder_granularity_opts> precoder_granularity_e_;
  using tci_states_pdcch_to_add_list_l_     = dyn_array<uint8_t>;
  using tci_states_pdcch_to_release_list_l_ = dyn_array<uint8_t>;

  // member variables
  bool                                ext                                      = false;
  bool                                tci_states_pdcch_to_add_list_present     = false;
  bool                                tci_states_pdcch_to_release_list_present = false;
  bool                                tci_present_in_dci_present               = false;
  bool                                pdcch_dmrs_scrambling_id_present         = false;
  uint8_t                             ctrl_res_set_id                          = 0;
  fixed_bitstring<45>                 freq_domain_res;
  uint8_t                             dur = 1;
  cce_reg_map_type_c_                 cce_reg_map_type;
  precoder_granularity_e_             precoder_granularity;
  tci_states_pdcch_to_add_list_l_     tci_states_pdcch_to_add_list;
  tci_states_pdcch_to_release_list_l_ tci_states_pdcch_to_release_list;
  uint32_t                            pdcch_dmrs_scrambling_id = 0;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// EUTRA-AllowedMeasBandwidth ::= ENUMERATED
struct eutra_allowed_meas_bw_opts {
  enum options { mbw6, mbw15, mbw25, mbw50, mbw75, mbw100, nulltype } value;
  typedef uint8_t number_type;

  std::string to_string() const;
  uint8_t     to_number() const;
};
typedef enumerated<eutra_allowed_meas_bw_opts> eutra_allowed_meas_bw_e;

// EUTRA-FreqBlackCellList ::= SEQUENCE (SIZE (1..16)) OF EUTRA-PhysCellIdRange
using eutra_freq_black_cell_list_l = dyn_array<eutra_pci_range_s>;

// EUTRA-FreqNeighCellList ::= SEQUENCE (SIZE (1..8)) OF EUTRA-FreqNeighCellInfo
using eutra_freq_neigh_cell_list_l = dyn_array<eutra_freq_neigh_cell_info_s>;

// EUTRA-MultiBandInfoList ::= SEQUENCE (SIZE (1..8)) OF EUTRA-MultiBandInfo
using eutra_multi_band_info_list_l = dyn_array<eutra_multi_band_info_s>;

// InterFreqBlackCellList ::= SEQUENCE (SIZE (1..16)) OF PCI-Range
using inter_freq_black_cell_list_l = dyn_array<pci_range_s>;

// InterFreqNeighCellList ::= SEQUENCE (SIZE (1..16)) OF InterFreqNeighCellInfo
using inter_freq_neigh_cell_list_l = dyn_array<inter_freq_neigh_cell_info_s>;

// MCC ::= SEQUENCE (SIZE (3)) OF INTEGER (0..9)
using mcc_l = std::array<uint8_t, 3>;

// MNC ::= SEQUENCE (SIZE (2..3)) OF INTEGER (0..9)
using mnc_l = bounded_array<uint8_t, 3>;

// MultiFrequencyBandListNR-SIB ::= SEQUENCE (SIZE (1..8)) OF NR-MultiBandInfo
using multi_freq_band_list_nr_sib_l = dyn_array<nr_multi_band_info_s>;

// PDSCH-TimeDomainResourceAllocationList ::= SEQUENCE (SIZE (1..16)) OF PDSCH-TimeDomainResourceAllocation
using pdsch_time_domain_res_alloc_list_l = dyn_array<pdsch_time_domain_res_alloc_s>;

// PUSCH-TimeDomainResourceAllocationList ::= SEQUENCE (SIZE (1..16)) OF PUSCH-TimeDomainResourceAllocation
using pusch_time_domain_res_alloc_list_l = dyn_array<pusch_time_domain_res_alloc_s>;

// RACH-ConfigGeneric ::= SEQUENCE
struct rach_cfg_generic_s {
  struct msg1_fdm_opts {
    enum options { one, two, four, eight, nulltype } value;
    typedef uint8_t number_type;

    std::string to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<msg1_fdm_opts> msg1_fdm_e_;
  struct preamb_trans_max_opts {
    enum options { n3, n4, n5, n6, n7, n8, n10, n20, n50, n100, n200, nulltype } value;
    typedef uint8_t number_type;

    std::string to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<preamb_trans_max_opts> preamb_trans_max_e_;
  struct pwr_ramp_step_opts {
    enum options { db0, db2, db4, db6, nulltype } value;
    typedef uint8_t number_type;

    std::string to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<pwr_ramp_step_opts> pwr_ramp_step_e_;
  struct ra_resp_win_opts {
    enum options { sl1, sl2, sl4, sl8, sl10, sl20, sl40, sl80, nulltype } value;
    typedef uint8_t number_type;

    std::string to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<ra_resp_win_opts> ra_resp_win_e_;

  // member variables
  bool                ext           = false;
  uint16_t            prach_cfg_idx = 0;
  msg1_fdm_e_         msg1_fdm;
  uint16_t            msg1_freq_start           = 0;
  uint8_t             zero_correlation_zone_cfg = 0;
  int16_t             preamb_rx_target_pwr      = -202;
  preamb_trans_max_e_ preamb_trans_max;
  pwr_ramp_step_e_    pwr_ramp_step;
  ra_resp_win_e_      ra_resp_win;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SS-RSSI-Measurement ::= SEQUENCE
struct ss_rssi_meas_s {
  bounded_bitstring<1, 80> meas_slots;
  uint8_t                  end_symbol = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SSB-MTC ::= SEQUENCE
struct ssb_mtc_s {
  struct periodicity_and_offset_c_ {
    struct types_opts {
      enum options { sf5, sf10, sf20, sf40, sf80, sf160, nulltype } value;
      typedef uint8_t number_type;

      std::string to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    periodicity_and_offset_c_() = default;
    periodicity_and_offset_c_(const periodicity_and_offset_c_& other);
    periodicity_and_offset_c_& operator=(const periodicity_and_offset_c_& other);
    ~periodicity_and_offset_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint8_t& sf5()
    {
      assert_choice_type("sf5", type_.to_string(), "periodicityAndOffset");
      return c.get<uint8_t>();
    }
    uint8_t& sf10()
    {
      assert_choice_type("sf10", type_.to_string(), "periodicityAndOffset");
      return c.get<uint8_t>();
    }
    uint8_t& sf20()
    {
      assert_choice_type("sf20", type_.to_string(), "periodicityAndOffset");
      return c.get<uint8_t>();
    }
    uint8_t& sf40()
    {
      assert_choice_type("sf40", type_.to_string(), "periodicityAndOffset");
      return c.get<uint8_t>();
    }
    uint8_t& sf80()
    {
      assert_choice_type("sf80", type_.to_string(), "periodicityAndOffset");
      return c.get<uint8_t>();
    }
    uint8_t& sf160()
    {
      assert_choice_type("sf160", type_.to_string(), "periodicityAndOffset");
      return c.get<uint8_t>();
    }
    const uint8_t& sf5() const
    {
      assert_choice_type("sf5", type_.to_string(), "periodicityAndOffset");
      return c.get<uint8_t>();
    }
    const uint8_t& sf10() const
    {
      assert_choice_type("sf10", type_.to_string(), "periodicityAndOffset");
      return c.get<uint8_t>();
    }
    const uint8_t& sf20() const
    {
      assert_choice_type("sf20", type_.to_string(), "periodicityAndOffset");
      return c.get<uint8_t>();
    }
    const uint8_t& sf40() const
    {
      assert_choice_type("sf40", type_.to_string(), "periodicityAndOffset");
      return c.get<uint8_t>();
    }
    const uint8_t& sf80() const
    {
      assert_choice_type("sf80", type_.to_string(), "periodicityAndOffset");
      return c.get<uint8_t>();
    }
    const uint8_t& sf160() const
    {
      assert_choice_type("sf160", type_.to_string(), "periodicityAndOffset");
      return c.get<uint8_t>();
    }
    uint8_t& set_sf5()
    {
      set(types::sf5);
      return c.get<uint8_t>();
    }
    uint8_t& set_sf10()
    {
      set(types::sf10);
      return c.get<uint8_t>();
    }
    uint8_t& set_sf20()
    {
      set(types::sf20);
      return c.get<uint8_t>();
    }
    uint8_t& set_sf40()
    {
      set(types::sf40);
      return c.get<uint8_t>();
    }
    uint8_t& set_sf80()
    {
      set(types::sf80);
      return c.get<uint8_t>();
    }
    uint8_t& set_sf160()
    {
      set(types::sf160);
      return c.get<uint8_t>();
    }

  private:
    types               type_;
    pod_choice_buffer_t c;

    void destroy_();
  };
  struct dur_opts {
    enum options { sf1, sf2, sf3, sf4, sf5, nulltype } value;
    typedef uint8_t number_type;

    std::string to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<dur_opts> dur_e_;

  // member variables
  periodicity_and_offset_c_ periodicity_and_offset;
  dur_e_                    dur;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SSB-ToMeasure ::= CHOICE
struct ssb_to_measure_c {
  struct types_opts {
    enum options { short_bitmap, medium_bitmap, long_bitmap, nulltype } value;

    std::string to_string() const;
  };
  typedef enumerated<types_opts> types;

  // choice methods
  ssb_to_measure_c() = default;
  ssb_to_measure_c(const ssb_to_measure_c& other);
  ssb_to_measure_c& operator=(const ssb_to_measure_c& other);
  ~ssb_to_measure_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  fixed_bitstring<4>& short_bitmap()
  {
    assert_choice_type("shortBitmap", type_.to_string(), "SSB-ToMeasure");
    return c.get<fixed_bitstring<4> >();
  }
  fixed_bitstring<8>& medium_bitmap()
  {
    assert_choice_type("mediumBitmap", type_.to_string(), "SSB-ToMeasure");
    return c.get<fixed_bitstring<8> >();
  }
  fixed_bitstring<64>& long_bitmap()
  {
    assert_choice_type("longBitmap", type_.to_string(), "SSB-ToMeasure");
    return c.get<fixed_bitstring<64> >();
  }
  const fixed_bitstring<4>& short_bitmap() const
  {
    assert_choice_type("shortBitmap", type_.to_string(), "SSB-ToMeasure");
    return c.get<fixed_bitstring<4> >();
  }
  const fixed_bitstring<8>& medium_bitmap() const
  {
    assert_choice_type("mediumBitmap", type_.to_string(), "SSB-ToMeasure");
    return c.get<fixed_bitstring<8> >();
  }
  const fixed_bitstring<64>& long_bitmap() const
  {
    assert_choice_type("longBitmap", type_.to_string(), "SSB-ToMeasure");
    return c.get<fixed_bitstring<64> >();
  }
  fixed_bitstring<4>& set_short_bitmap()
  {
    set(types::short_bitmap);
    return c.get<fixed_bitstring<4> >();
  }
  fixed_bitstring<8>& set_medium_bitmap()
  {
    set(types::medium_bitmap);
    return c.get<fixed_bitstring<8> >();
  }
  fixed_bitstring<64>& set_long_bitmap()
  {
    set(types::long_bitmap);
    return c.get<fixed_bitstring<64> >();
  }

private:
  types                                 type_;
  choice_buffer_t<fixed_bitstring<64> > c;

  void destroy_();
};

// SearchSpace ::= SEQUENCE
struct search_space_s {
  struct monitoring_slot_periodicity_and_offset_c_ {
    struct types_opts {
      enum options {
        sl1,
        sl2,
        sl4,
        sl5,
        sl8,
        sl10,
        sl16,
        sl20,
        sl40,
        sl80,
        sl160,
        sl320,
        sl640,
        sl1280,
        sl2560,
        nulltype
      } value;
      typedef uint16_t number_type;

      std::string to_string() const;
      uint16_t    to_number() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    monitoring_slot_periodicity_and_offset_c_() = default;
    monitoring_slot_periodicity_and_offset_c_(const monitoring_slot_periodicity_and_offset_c_& other);
    monitoring_slot_periodicity_and_offset_c_& operator=(const monitoring_slot_periodicity_and_offset_c_& other);
    ~monitoring_slot_periodicity_and_offset_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint8_t& sl2()
    {
      assert_choice_type("sl2", type_.to_string(), "monitoringSlotPeriodicityAndOffset");
      return c.get<uint8_t>();
    }
    uint8_t& sl4()
    {
      assert_choice_type("sl4", type_.to_string(), "monitoringSlotPeriodicityAndOffset");
      return c.get<uint8_t>();
    }
    uint8_t& sl5()
    {
      assert_choice_type("sl5", type_.to_string(), "monitoringSlotPeriodicityAndOffset");
      return c.get<uint8_t>();
    }
    uint8_t& sl8()
    {
      assert_choice_type("sl8", type_.to_string(), "monitoringSlotPeriodicityAndOffset");
      return c.get<uint8_t>();
    }
    uint8_t& sl10()
    {
      assert_choice_type("sl10", type_.to_string(), "monitoringSlotPeriodicityAndOffset");
      return c.get<uint8_t>();
    }
    uint8_t& sl16()
    {
      assert_choice_type("sl16", type_.to_string(), "monitoringSlotPeriodicityAndOffset");
      return c.get<uint8_t>();
    }
    uint8_t& sl20()
    {
      assert_choice_type("sl20", type_.to_string(), "monitoringSlotPeriodicityAndOffset");
      return c.get<uint8_t>();
    }
    uint8_t& sl40()
    {
      assert_choice_type("sl40", type_.to_string(), "monitoringSlotPeriodicityAndOffset");
      return c.get<uint8_t>();
    }
    uint8_t& sl80()
    {
      assert_choice_type("sl80", type_.to_string(), "monitoringSlotPeriodicityAndOffset");
      return c.get<uint8_t>();
    }
    uint8_t& sl160()
    {
      assert_choice_type("sl160", type_.to_string(), "monitoringSlotPeriodicityAndOffset");
      return c.get<uint8_t>();
    }
    uint16_t& sl320()
    {
      assert_choice_type("sl320", type_.to_string(), "monitoringSlotPeriodicityAndOffset");
      return c.get<uint16_t>();
    }
    uint16_t& sl640()
    {
      assert_choice_type("sl640", type_.to_string(), "monitoringSlotPeriodicityAndOffset");
      return c.get<uint16_t>();
    }
    uint16_t& sl1280()
    {
      assert_choice_type("sl1280", type_.to_string(), "monitoringSlotPeriodicityAndOffset");
      return c.get<uint16_t>();
    }
    uint16_t& sl2560()
    {
      assert_choice_type("sl2560", type_.to_string(), "monitoringSlotPeriodicityAndOffset");
      return c.get<uint16_t>();
    }
    const uint8_t& sl2() const
    {
      assert_choice_type("sl2", type_.to_string(), "monitoringSlotPeriodicityAndOffset");
      return c.get<uint8_t>();
    }
    const uint8_t& sl4() const
    {
      assert_choice_type("sl4", type_.to_string(), "monitoringSlotPeriodicityAndOffset");
      return c.get<uint8_t>();
    }
    const uint8_t& sl5() const
    {
      assert_choice_type("sl5", type_.to_string(), "monitoringSlotPeriodicityAndOffset");
      return c.get<uint8_t>();
    }
    const uint8_t& sl8() const
    {
      assert_choice_type("sl8", type_.to_string(), "monitoringSlotPeriodicityAndOffset");
      return c.get<uint8_t>();
    }
    const uint8_t& sl10() const
    {
      assert_choice_type("sl10", type_.to_string(), "monitoringSlotPeriodicityAndOffset");
      return c.get<uint8_t>();
    }
    const uint8_t& sl16() const
    {
      assert_choice_type("sl16", type_.to_string(), "monitoringSlotPeriodicityAndOffset");
      return c.get<uint8_t>();
    }
    const uint8_t& sl20() const
    {
      assert_choice_type("sl20", type_.to_string(), "monitoringSlotPeriodicityAndOffset");
      return c.get<uint8_t>();
    }
    const uint8_t& sl40() const
    {
      assert_choice_type("sl40", type_.to_string(), "monitoringSlotPeriodicityAndOffset");
      return c.get<uint8_t>();
    }
    const uint8_t& sl80() const
    {
      assert_choice_type("sl80", type_.to_string(), "monitoringSlotPeriodicityAndOffset");
      return c.get<uint8_t>();
    }
    const uint8_t& sl160() const
    {
      assert_choice_type("sl160", type_.to_string(), "monitoringSlotPeriodicityAndOffset");
      return c.get<uint8_t>();
    }
    const uint16_t& sl320() const
    {
      assert_choice_type("sl320", type_.to_string(), "monitoringSlotPeriodicityAndOffset");
      return c.get<uint16_t>();
    }
    const uint16_t& sl640() const
    {
      assert_choice_type("sl640", type_.to_string(), "monitoringSlotPeriodicityAndOffset");
      return c.get<uint16_t>();
    }
    const uint16_t& sl1280() const
    {
      assert_choice_type("sl1280", type_.to_string(), "monitoringSlotPeriodicityAndOffset");
      return c.get<uint16_t>();
    }
    const uint16_t& sl2560() const
    {
      assert_choice_type("sl2560", type_.to_string(), "monitoringSlotPeriodicityAndOffset");
      return c.get<uint16_t>();
    }
    uint8_t& set_sl2()
    {
      set(types::sl2);
      return c.get<uint8_t>();
    }
    uint8_t& set_sl4()
    {
      set(types::sl4);
      return c.get<uint8_t>();
    }
    uint8_t& set_sl5()
    {
      set(types::sl5);
      return c.get<uint8_t>();
    }
    uint8_t& set_sl8()
    {
      set(types::sl8);
      return c.get<uint8_t>();
    }
    uint8_t& set_sl10()
    {
      set(types::sl10);
      return c.get<uint8_t>();
    }
    uint8_t& set_sl16()
    {
      set(types::sl16);
      return c.get<uint8_t>();
    }
    uint8_t& set_sl20()
    {
      set(types::sl20);
      return c.get<uint8_t>();
    }
    uint8_t& set_sl40()
    {
      set(types::sl40);
      return c.get<uint8_t>();
    }
    uint8_t& set_sl80()
    {
      set(types::sl80);
      return c.get<uint8_t>();
    }
    uint8_t& set_sl160()
    {
      set(types::sl160);
      return c.get<uint8_t>();
    }
    uint16_t& set_sl320()
    {
      set(types::sl320);
      return c.get<uint16_t>();
    }
    uint16_t& set_sl640()
    {
      set(types::sl640);
      return c.get<uint16_t>();
    }
    uint16_t& set_sl1280()
    {
      set(types::sl1280);
      return c.get<uint16_t>();
    }
    uint16_t& set_sl2560()
    {
      set(types::sl2560);
      return c.get<uint16_t>();
    }

  private:
    types               type_;
    pod_choice_buffer_t c;

    void destroy_();
  };
  struct nrof_candidates_s_ {
    struct aggregation_level1_opts {
      enum options { n0, n1, n2, n3, n4, n5, n6, n8, nulltype } value;
      typedef uint8_t number_type;

      std::string to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<aggregation_level1_opts> aggregation_level1_e_;
    struct aggregation_level2_opts {
      enum options { n0, n1, n2, n3, n4, n5, n6, n8, nulltype } value;
      typedef uint8_t number_type;

      std::string to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<aggregation_level2_opts> aggregation_level2_e_;
    struct aggregation_level4_opts {
      enum options { n0, n1, n2, n3, n4, n5, n6, n8, nulltype } value;
      typedef uint8_t number_type;

      std::string to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<aggregation_level4_opts> aggregation_level4_e_;
    struct aggregation_level8_opts {
      enum options { n0, n1, n2, n3, n4, n5, n6, n8, nulltype } value;
      typedef uint8_t number_type;

      std::string to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<aggregation_level8_opts> aggregation_level8_e_;
    struct aggregation_level16_opts {
      enum options { n0, n1, n2, n3, n4, n5, n6, n8, nulltype } value;
      typedef uint8_t number_type;

      std::string to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<aggregation_level16_opts> aggregation_level16_e_;

    // member variables
    aggregation_level1_e_  aggregation_level1;
    aggregation_level2_e_  aggregation_level2;
    aggregation_level4_e_  aggregation_level4;
    aggregation_level8_e_  aggregation_level8;
    aggregation_level16_e_ aggregation_level16;
  };
  struct search_space_type_c_ {
    struct common_s_ {
      struct dci_format0_minus0_and_format1_minus0_s_ {
        bool ext = false;
        // ...
      };
      struct dci_format2_minus0_s_ {
        struct nrof_candidates_sfi_s_ {
          struct aggregation_level1_opts {
            enum options { n1, n2, nulltype } value;
            typedef uint8_t number_type;

            std::string to_string() const;
            uint8_t     to_number() const;
          };
          typedef enumerated<aggregation_level1_opts> aggregation_level1_e_;
          struct aggregation_level2_opts {
            enum options { n1, n2, nulltype } value;
            typedef uint8_t number_type;

            std::string to_string() const;
            uint8_t     to_number() const;
          };
          typedef enumerated<aggregation_level2_opts> aggregation_level2_e_;
          struct aggregation_level4_opts {
            enum options { n1, n2, nulltype } value;
            typedef uint8_t number_type;

            std::string to_string() const;
            uint8_t     to_number() const;
          };
          typedef enumerated<aggregation_level4_opts> aggregation_level4_e_;
          struct aggregation_level8_opts {
            enum options { n1, n2, nulltype } value;
            typedef uint8_t number_type;

            std::string to_string() const;
            uint8_t     to_number() const;
          };
          typedef enumerated<aggregation_level8_opts> aggregation_level8_e_;
          struct aggregation_level16_opts {
            enum options { n1, n2, nulltype } value;
            typedef uint8_t number_type;

            std::string to_string() const;
            uint8_t     to_number() const;
          };
          typedef enumerated<aggregation_level16_opts> aggregation_level16_e_;

          // member variables
          bool                   aggregation_level1_present  = false;
          bool                   aggregation_level2_present  = false;
          bool                   aggregation_level4_present  = false;
          bool                   aggregation_level8_present  = false;
          bool                   aggregation_level16_present = false;
          aggregation_level1_e_  aggregation_level1;
          aggregation_level2_e_  aggregation_level2;
          aggregation_level4_e_  aggregation_level4;
          aggregation_level8_e_  aggregation_level8;
          aggregation_level16_e_ aggregation_level16;
        };

        // member variables
        bool                   ext = false;
        nrof_candidates_sfi_s_ nrof_candidates_sfi;
        // ...
      };
      struct dci_format2_minus1_s_ {
        bool ext = false;
        // ...
      };
      struct dci_format2_minus2_s_ {
        bool ext = false;
        // ...
      };
      struct dci_format2_minus3_s_ {
        struct dummy1_opts {
          enum options { sl1, sl2, sl4, sl5, sl8, sl10, sl16, sl20, nulltype } value;
          typedef uint8_t number_type;

          std::string to_string() const;
          uint8_t     to_number() const;
        };
        typedef enumerated<dummy1_opts> dummy1_e_;
        struct dummy2_opts {
          enum options { n1, n2, nulltype } value;
          typedef uint8_t number_type;

          std::string to_string() const;
          uint8_t     to_number() const;
        };
        typedef enumerated<dummy2_opts> dummy2_e_;

        // member variables
        bool      ext            = false;
        bool      dummy1_present = false;
        dummy1_e_ dummy1;
        dummy2_e_ dummy2;
        // ...
      };

      // member variables
      bool                                     dci_format0_minus0_and_format1_minus0_present = false;
      bool                                     dci_format2_minus0_present                    = false;
      bool                                     dci_format2_minus1_present                    = false;
      bool                                     dci_format2_minus2_present                    = false;
      bool                                     dci_format2_minus3_present                    = false;
      dci_format0_minus0_and_format1_minus0_s_ dci_format0_minus0_and_format1_minus0;
      dci_format2_minus0_s_                    dci_format2_minus0;
      dci_format2_minus1_s_                    dci_format2_minus1;
      dci_format2_minus2_s_                    dci_format2_minus2;
      dci_format2_minus3_s_                    dci_format2_minus3;
    };
    struct ue_specific_s_ {
      struct dci_formats_opts {
        enum options { formats0_minus0_and_minus1_minus0, formats0_minus1_and_minus1_minus1, nulltype } value;

        std::string to_string() const;
      };
      typedef enumerated<dci_formats_opts> dci_formats_e_;

      // member variables
      bool           ext = false;
      dci_formats_e_ dci_formats;
      // ...
    };
    struct types_opts {
      enum options { common, ue_specific, nulltype } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    search_space_type_c_() = default;
    search_space_type_c_(const search_space_type_c_& other);
    search_space_type_c_& operator=(const search_space_type_c_& other);
    ~search_space_type_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    common_s_& common()
    {
      assert_choice_type("common", type_.to_string(), "searchSpaceType");
      return c.get<common_s_>();
    }
    ue_specific_s_& ue_specific()
    {
      assert_choice_type("ue-Specific", type_.to_string(), "searchSpaceType");
      return c.get<ue_specific_s_>();
    }
    const common_s_& common() const
    {
      assert_choice_type("common", type_.to_string(), "searchSpaceType");
      return c.get<common_s_>();
    }
    const ue_specific_s_& ue_specific() const
    {
      assert_choice_type("ue-Specific", type_.to_string(), "searchSpaceType");
      return c.get<ue_specific_s_>();
    }
    common_s_& set_common()
    {
      set(types::common);
      return c.get<common_s_>();
    }
    ue_specific_s_& set_ue_specific()
    {
      set(types::ue_specific);
      return c.get<ue_specific_s_>();
    }

  private:
    types                                      type_;
    choice_buffer_t<common_s_, ue_specific_s_> c;

    void destroy_();
  };

  // member variables
  bool                                      ctrl_res_set_id_present                        = false;
  bool                                      monitoring_slot_periodicity_and_offset_present = false;
  bool                                      dur_present                                    = false;
  bool                                      monitoring_symbols_within_slot_present         = false;
  bool                                      nrof_candidates_present                        = false;
  bool                                      search_space_type_present                      = false;
  uint8_t                                   search_space_id                                = 0;
  uint8_t                                   ctrl_res_set_id                                = 0;
  monitoring_slot_periodicity_and_offset_c_ monitoring_slot_periodicity_and_offset;
  uint16_t                                  dur = 2;
  fixed_bitstring<14>                       monitoring_symbols_within_slot;
  nrof_candidates_s_                        nrof_candidates;
  search_space_type_c_                      search_space_type;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SpeedStateScaleFactors ::= SEQUENCE
struct speed_state_scale_factors_s {
  struct sf_medium_opts {
    enum options { odot25, odot5, odot75, ldot0, nulltype } value;
    typedef float number_type;

    std::string to_string() const;
    float       to_number() const;
    std::string to_number_string() const;
  };
  typedef enumerated<sf_medium_opts> sf_medium_e_;
  struct sf_high_opts {
    enum options { odot25, odot5, odot75, ldot0, nulltype } value;
    typedef float number_type;

    std::string to_string() const;
    float       to_number() const;
    std::string to_number_string() const;
  };
  typedef enumerated<sf_high_opts> sf_high_e_;

  // member variables
  sf_medium_e_ sf_medium;
  sf_high_e_   sf_high;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SubcarrierSpacing ::= ENUMERATED
struct subcarrier_spacing_opts {
  enum options { khz15, khz30, khz60, khz120, khz240, spare3, spare2, spare1, nulltype } value;
  typedef uint8_t number_type;

  std::string to_string() const;
  uint8_t     to_number() const;
};
typedef enumerated<subcarrier_spacing_opts> subcarrier_spacing_e;

// ThresholdNR ::= SEQUENCE
struct thres_nr_s {
  bool    thres_rsrp_present = false;
  bool    thres_rsrq_present = false;
  bool    thres_sinr_present = false;
  uint8_t thres_rsrp         = 0;
  uint8_t thres_rsrq         = 0;
  uint8_t thres_sinr         = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BWP ::= SEQUENCE
struct bwp_s {
  bool                 cp_present      = false;
  uint16_t             location_and_bw = 0;
  subcarrier_spacing_e subcarrier_spacing;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CarrierFreqEUTRA ::= SEQUENCE
struct carrier_freq_eutra_s {
  struct thresh_x_q_s_ {
    uint8_t thresh_x_high_q = 0;
    uint8_t thresh_x_low_q  = 0;
  };

  // member variables
  bool                         eutra_multi_band_info_list_present = false;
  bool                         eutra_freq_neigh_cell_list_present = false;
  bool                         eutra_black_cell_list_present      = false;
  bool                         cell_resel_prio_present            = false;
  bool                         cell_resel_sub_prio_present        = false;
  bool                         thresh_x_q_present                 = false;
  uint32_t                     carrier_freq                       = 0;
  eutra_multi_band_info_list_l eutra_multi_band_info_list;
  eutra_freq_neigh_cell_list_l eutra_freq_neigh_cell_list;
  eutra_freq_black_cell_list_l eutra_black_cell_list;
  eutra_allowed_meas_bw_e      allowed_meas_bw;
  bool                         presence_ant_port1 = false;
  uint8_t                      cell_resel_prio    = 0;
  cell_resel_sub_prio_e        cell_resel_sub_prio;
  uint8_t                      thresh_x_high = 0;
  uint8_t                      thresh_x_low  = 0;
  int8_t                       q_rx_lev_min  = -70;
  int8_t                       q_qual_min    = -34;
  int8_t                       p_max_eutra   = -30;
  thresh_x_q_s_                thresh_x_q;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// InterFreqCarrierFreqInfo ::= SEQUENCE
struct inter_freq_carrier_freq_info_s {
  struct thresh_x_q_s_ {
    uint8_t thresh_x_high_q = 0;
    uint8_t thresh_x_low_q  = 0;
  };

  // member variables
  bool                          ext                                        = false;
  bool                          freq_band_list_present                     = false;
  bool                          freq_band_list_sul_present                 = false;
  bool                          nrof_ss_blocks_to_average_present          = false;
  bool                          abs_thresh_ss_blocks_consolidation_present = false;
  bool                          smtc_present                               = false;
  bool                          ssb_to_measure_present                     = false;
  bool                          ss_rssi_meas_present                       = false;
  bool                          q_rx_lev_min_sul_present                   = false;
  bool                          q_qual_min_present                         = false;
  bool                          p_max_present                              = false;
  bool                          t_resel_nr_sf_present                      = false;
  bool                          thresh_x_q_present                         = false;
  bool                          cell_resel_prio_present                    = false;
  bool                          cell_resel_sub_prio_present                = false;
  bool                          q_offset_freq_present                      = false;
  bool                          inter_freq_neigh_cell_list_present         = false;
  bool                          inter_freq_black_cell_list_present         = false;
  uint32_t                      dl_carrier_freq                            = 0;
  multi_freq_band_list_nr_sib_l freq_band_list;
  multi_freq_band_list_nr_sib_l freq_band_list_sul;
  uint8_t                       nrof_ss_blocks_to_average = 2;
  thres_nr_s                    abs_thresh_ss_blocks_consolidation;
  ssb_mtc_s                     smtc;
  subcarrier_spacing_e          ssb_subcarrier_spacing;
  ssb_to_measure_c              ssb_to_measure;
  bool                          derive_ssb_idx_from_cell = false;
  ss_rssi_meas_s                ss_rssi_meas;
  int8_t                        q_rx_lev_min     = -70;
  int8_t                        q_rx_lev_min_sul = -70;
  int8_t                        q_qual_min       = -43;
  int8_t                        p_max            = -30;
  uint8_t                       t_resel_nr       = 0;
  speed_state_scale_factors_s   t_resel_nr_sf;
  uint8_t                       thresh_x_high_p = 0;
  uint8_t                       thresh_x_low_p  = 0;
  thresh_x_q_s_                 thresh_x_q;
  uint8_t                       cell_resel_prio = 0;
  cell_resel_sub_prio_e         cell_resel_sub_prio;
  q_offset_range_e              q_offset_freq;
  inter_freq_neigh_cell_list_l  inter_freq_neigh_cell_list;
  inter_freq_black_cell_list_l  inter_freq_black_cell_list;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// IntraFreqNeighCellInfo ::= SEQUENCE
struct intra_freq_neigh_cell_info_s {
  bool             ext                                  = false;
  bool             q_rx_lev_min_offset_cell_present     = false;
  bool             q_rx_lev_min_offset_cell_sul_present = false;
  bool             q_qual_min_offset_cell_present       = false;
  uint16_t         pci                                  = 0;
  q_offset_range_e q_offset_cell;
  uint8_t          q_rx_lev_min_offset_cell     = 1;
  uint8_t          q_rx_lev_min_offset_cell_sul = 1;
  uint8_t          q_qual_min_offset_cell       = 1;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDCCH-ConfigCommon ::= SEQUENCE
struct pdcch_cfg_common_s {
  using common_search_space_list_l_ = dyn_array<search_space_s>;
  struct first_pdcch_monitoring_occasion_of_po_c_ {
    using scs15_kh_zone_t_l_                                                              = bounded_array<uint8_t, 4>;
    using scs30_kh_zone_t_scs15_kh_zhalf_t_l_                                             = bounded_array<uint16_t, 4>;
    using scs60_kh_zone_t_scs30_kh_zhalf_t_scs15_kh_zquarter_t_l_                         = bounded_array<uint16_t, 4>;
    using scs120_kh_zone_t_scs60_kh_zhalf_t_scs30_kh_zquarter_t_scs15_kh_zone_eighth_t_l_ = bounded_array<uint16_t, 4>;
    using scs120_kh_zhalf_t_scs60_kh_zquarter_t_scs30_kh_zone_eighth_t_scs15_kh_zone_sixteenth_t_l_ =
        bounded_array<uint16_t, 4>;
    using scs120_kh_zquarter_t_scs60_kh_zone_eighth_t_scs30_kh_zone_sixteenth_t_l_ = bounded_array<uint16_t, 4>;
    using scs120_kh_zone_eighth_t_scs60_kh_zone_sixteenth_t_l_                     = bounded_array<uint16_t, 4>;
    using scs120_kh_zone_sixteenth_t_l_                                            = bounded_array<uint16_t, 4>;
    struct types_opts {
      enum options {
        scs15_kh_zone_t,
        scs30_kh_zone_t_scs15_kh_zhalf_t,
        scs60_kh_zone_t_scs30_kh_zhalf_t_scs15_kh_zquarter_t,
        scs120_kh_zone_t_scs60_kh_zhalf_t_scs30_kh_zquarter_t_scs15_kh_zone_eighth_t,
        scs120_kh_zhalf_t_scs60_kh_zquarter_t_scs30_kh_zone_eighth_t_scs15_kh_zone_sixteenth_t,
        scs120_kh_zquarter_t_scs60_kh_zone_eighth_t_scs30_kh_zone_sixteenth_t,
        scs120_kh_zone_eighth_t_scs60_kh_zone_sixteenth_t,
        scs120_kh_zone_sixteenth_t,
        nulltype
      } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    first_pdcch_monitoring_occasion_of_po_c_() = default;
    first_pdcch_monitoring_occasion_of_po_c_(const first_pdcch_monitoring_occasion_of_po_c_& other);
    first_pdcch_monitoring_occasion_of_po_c_& operator=(const first_pdcch_monitoring_occasion_of_po_c_& other);
    ~first_pdcch_monitoring_occasion_of_po_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    scs15_kh_zone_t_l_& scs15_kh_zone_t()
    {
      assert_choice_type("sCS15KHZoneT", type_.to_string(), "firstPDCCH-MonitoringOccasionOfPO");
      return c.get<scs15_kh_zone_t_l_>();
    }
    scs30_kh_zone_t_scs15_kh_zhalf_t_l_& scs30_kh_zone_t_scs15_kh_zhalf_t()
    {
      assert_choice_type("sCS30KHZoneT-SCS15KHZhalfT", type_.to_string(), "firstPDCCH-MonitoringOccasionOfPO");
      return c.get<scs30_kh_zone_t_scs15_kh_zhalf_t_l_>();
    }
    scs60_kh_zone_t_scs30_kh_zhalf_t_scs15_kh_zquarter_t_l_& scs60_kh_zone_t_scs30_kh_zhalf_t_scs15_kh_zquarter_t()
    {
      assert_choice_type(
          "sCS60KHZoneT-SCS30KHZhalfT-SCS15KHZquarterT", type_.to_string(), "firstPDCCH-MonitoringOccasionOfPO");
      return c.get<scs60_kh_zone_t_scs30_kh_zhalf_t_scs15_kh_zquarter_t_l_>();
    }
    scs120_kh_zone_t_scs60_kh_zhalf_t_scs30_kh_zquarter_t_scs15_kh_zone_eighth_t_l_&
    scs120_kh_zone_t_scs60_kh_zhalf_t_scs30_kh_zquarter_t_scs15_kh_zone_eighth_t()
    {
      assert_choice_type("sCS120KHZoneT-SCS60KHZhalfT-SCS30KHZquarterT-SCS15KHZoneEighthT",
                         type_.to_string(),
                         "firstPDCCH-MonitoringOccasionOfPO");
      return c.get<scs120_kh_zone_t_scs60_kh_zhalf_t_scs30_kh_zquarter_t_scs15_kh_zone_eighth_t_l_>();
    }
    scs120_kh_zhalf_t_scs60_kh_zquarter_t_scs30_kh_zone_eighth_t_scs15_kh_zone_sixteenth_t_l_&
    scs120_kh_zhalf_t_scs60_kh_zquarter_t_scs30_kh_zone_eighth_t_scs15_kh_zone_sixteenth_t()
    {
      assert_choice_type("sCS120KHZhalfT-SCS60KHZquarterT-SCS30KHZoneEighthT-SCS15KHZoneSixteenthT",
                         type_.to_string(),
                         "firstPDCCH-MonitoringOccasionOfPO");
      return c.get<scs120_kh_zhalf_t_scs60_kh_zquarter_t_scs30_kh_zone_eighth_t_scs15_kh_zone_sixteenth_t_l_>();
    }
    scs120_kh_zquarter_t_scs60_kh_zone_eighth_t_scs30_kh_zone_sixteenth_t_l_&
    scs120_kh_zquarter_t_scs60_kh_zone_eighth_t_scs30_kh_zone_sixteenth_t()
    {
      assert_choice_type("sCS120KHZquarterT-SCS60KHZoneEighthT-SCS30KHZoneSixteenthT",
                         type_.to_string(),
                         "firstPDCCH-MonitoringOccasionOfPO");
      return c.get<scs120_kh_zquarter_t_scs60_kh_zone_eighth_t_scs30_kh_zone_sixteenth_t_l_>();
    }
    scs120_kh_zone_eighth_t_scs60_kh_zone_sixteenth_t_l_& scs120_kh_zone_eighth_t_scs60_kh_zone_sixteenth_t()
    {
      assert_choice_type(
          "sCS120KHZoneEighthT-SCS60KHZoneSixteenthT", type_.to_string(), "firstPDCCH-MonitoringOccasionOfPO");
      return c.get<scs120_kh_zone_eighth_t_scs60_kh_zone_sixteenth_t_l_>();
    }
    scs120_kh_zone_sixteenth_t_l_& scs120_kh_zone_sixteenth_t()
    {
      assert_choice_type("sCS120KHZoneSixteenthT", type_.to_string(), "firstPDCCH-MonitoringOccasionOfPO");
      return c.get<scs120_kh_zone_sixteenth_t_l_>();
    }
    const scs15_kh_zone_t_l_& scs15_kh_zone_t() const
    {
      assert_choice_type("sCS15KHZoneT", type_.to_string(), "firstPDCCH-MonitoringOccasionOfPO");
      return c.get<scs15_kh_zone_t_l_>();
    }
    const scs30_kh_zone_t_scs15_kh_zhalf_t_l_& scs30_kh_zone_t_scs15_kh_zhalf_t() const
    {
      assert_choice_type("sCS30KHZoneT-SCS15KHZhalfT", type_.to_string(), "firstPDCCH-MonitoringOccasionOfPO");
      return c.get<scs30_kh_zone_t_scs15_kh_zhalf_t_l_>();
    }
    const scs60_kh_zone_t_scs30_kh_zhalf_t_scs15_kh_zquarter_t_l_&
    scs60_kh_zone_t_scs30_kh_zhalf_t_scs15_kh_zquarter_t() const
    {
      assert_choice_type(
          "sCS60KHZoneT-SCS30KHZhalfT-SCS15KHZquarterT", type_.to_string(), "firstPDCCH-MonitoringOccasionOfPO");
      return c.get<scs60_kh_zone_t_scs30_kh_zhalf_t_scs15_kh_zquarter_t_l_>();
    }
    const scs120_kh_zone_t_scs60_kh_zhalf_t_scs30_kh_zquarter_t_scs15_kh_zone_eighth_t_l_&
    scs120_kh_zone_t_scs60_kh_zhalf_t_scs30_kh_zquarter_t_scs15_kh_zone_eighth_t() const
    {
      assert_choice_type("sCS120KHZoneT-SCS60KHZhalfT-SCS30KHZquarterT-SCS15KHZoneEighthT",
                         type_.to_string(),
                         "firstPDCCH-MonitoringOccasionOfPO");
      return c.get<scs120_kh_zone_t_scs60_kh_zhalf_t_scs30_kh_zquarter_t_scs15_kh_zone_eighth_t_l_>();
    }
    const scs120_kh_zhalf_t_scs60_kh_zquarter_t_scs30_kh_zone_eighth_t_scs15_kh_zone_sixteenth_t_l_&
    scs120_kh_zhalf_t_scs60_kh_zquarter_t_scs30_kh_zone_eighth_t_scs15_kh_zone_sixteenth_t() const
    {
      assert_choice_type("sCS120KHZhalfT-SCS60KHZquarterT-SCS30KHZoneEighthT-SCS15KHZoneSixteenthT",
                         type_.to_string(),
                         "firstPDCCH-MonitoringOccasionOfPO");
      return c.get<scs120_kh_zhalf_t_scs60_kh_zquarter_t_scs30_kh_zone_eighth_t_scs15_kh_zone_sixteenth_t_l_>();
    }
    const scs120_kh_zquarter_t_scs60_kh_zone_eighth_t_scs30_kh_zone_sixteenth_t_l_&
    scs120_kh_zquarter_t_scs60_kh_zone_eighth_t_scs30_kh_zone_sixteenth_t() const
    {
      assert_choice_type("sCS120KHZquarterT-SCS60KHZoneEighthT-SCS30KHZoneSixteenthT",
                         type_.to_string(),
                         "firstPDCCH-MonitoringOccasionOfPO");
      return c.get<scs120_kh_zquarter_t_scs60_kh_zone_eighth_t_scs30_kh_zone_sixteenth_t_l_>();
    }
    const scs120_kh_zone_eighth_t_scs60_kh_zone_sixteenth_t_l_&
    scs120_kh_zone_eighth_t_scs60_kh_zone_sixteenth_t() const
    {
      assert_choice_type(
          "sCS120KHZoneEighthT-SCS60KHZoneSixteenthT", type_.to_string(), "firstPDCCH-MonitoringOccasionOfPO");
      return c.get<scs120_kh_zone_eighth_t_scs60_kh_zone_sixteenth_t_l_>();
    }
    const scs120_kh_zone_sixteenth_t_l_& scs120_kh_zone_sixteenth_t() const
    {
      assert_choice_type("sCS120KHZoneSixteenthT", type_.to_string(), "firstPDCCH-MonitoringOccasionOfPO");
      return c.get<scs120_kh_zone_sixteenth_t_l_>();
    }
    scs15_kh_zone_t_l_& set_scs15_kh_zone_t()
    {
      set(types::scs15_kh_zone_t);
      return c.get<scs15_kh_zone_t_l_>();
    }
    scs30_kh_zone_t_scs15_kh_zhalf_t_l_& set_scs30_kh_zone_t_scs15_kh_zhalf_t()
    {
      set(types::scs30_kh_zone_t_scs15_kh_zhalf_t);
      return c.get<scs30_kh_zone_t_scs15_kh_zhalf_t_l_>();
    }
    scs60_kh_zone_t_scs30_kh_zhalf_t_scs15_kh_zquarter_t_l_& set_scs60_kh_zone_t_scs30_kh_zhalf_t_scs15_kh_zquarter_t()
    {
      set(types::scs60_kh_zone_t_scs30_kh_zhalf_t_scs15_kh_zquarter_t);
      return c.get<scs60_kh_zone_t_scs30_kh_zhalf_t_scs15_kh_zquarter_t_l_>();
    }
    scs120_kh_zone_t_scs60_kh_zhalf_t_scs30_kh_zquarter_t_scs15_kh_zone_eighth_t_l_&
    set_scs120_kh_zone_t_scs60_kh_zhalf_t_scs30_kh_zquarter_t_scs15_kh_zone_eighth_t()
    {
      set(types::scs120_kh_zone_t_scs60_kh_zhalf_t_scs30_kh_zquarter_t_scs15_kh_zone_eighth_t);
      return c.get<scs120_kh_zone_t_scs60_kh_zhalf_t_scs30_kh_zquarter_t_scs15_kh_zone_eighth_t_l_>();
    }
    scs120_kh_zhalf_t_scs60_kh_zquarter_t_scs30_kh_zone_eighth_t_scs15_kh_zone_sixteenth_t_l_&
    set_scs120_kh_zhalf_t_scs60_kh_zquarter_t_scs30_kh_zone_eighth_t_scs15_kh_zone_sixteenth_t()
    {
      set(types::scs120_kh_zhalf_t_scs60_kh_zquarter_t_scs30_kh_zone_eighth_t_scs15_kh_zone_sixteenth_t);
      return c.get<scs120_kh_zhalf_t_scs60_kh_zquarter_t_scs30_kh_zone_eighth_t_scs15_kh_zone_sixteenth_t_l_>();
    }
    scs120_kh_zquarter_t_scs60_kh_zone_eighth_t_scs30_kh_zone_sixteenth_t_l_&
    set_scs120_kh_zquarter_t_scs60_kh_zone_eighth_t_scs30_kh_zone_sixteenth_t()
    {
      set(types::scs120_kh_zquarter_t_scs60_kh_zone_eighth_t_scs30_kh_zone_sixteenth_t);
      return c.get<scs120_kh_zquarter_t_scs60_kh_zone_eighth_t_scs30_kh_zone_sixteenth_t_l_>();
    }
    scs120_kh_zone_eighth_t_scs60_kh_zone_sixteenth_t_l_& set_scs120_kh_zone_eighth_t_scs60_kh_zone_sixteenth_t()
    {
      set(types::scs120_kh_zone_eighth_t_scs60_kh_zone_sixteenth_t);
      return c.get<scs120_kh_zone_eighth_t_scs60_kh_zone_sixteenth_t_l_>();
    }
    scs120_kh_zone_sixteenth_t_l_& set_scs120_kh_zone_sixteenth_t()
    {
      set(types::scs120_kh_zone_sixteenth_t);
      return c.get<scs120_kh_zone_sixteenth_t_l_>();
    }

  private:
    types type_;
    choice_buffer_t<scs120_kh_zhalf_t_scs60_kh_zquarter_t_scs30_kh_zone_eighth_t_scs15_kh_zone_sixteenth_t_l_,
                    scs120_kh_zone_eighth_t_scs60_kh_zone_sixteenth_t_l_,
                    scs120_kh_zone_sixteenth_t_l_,
                    scs120_kh_zone_t_scs60_kh_zhalf_t_scs30_kh_zquarter_t_scs15_kh_zone_eighth_t_l_,
                    scs120_kh_zquarter_t_scs60_kh_zone_eighth_t_scs30_kh_zone_sixteenth_t_l_,
                    scs15_kh_zone_t_l_,
                    scs30_kh_zone_t_scs15_kh_zhalf_t_l_,
                    scs60_kh_zone_t_scs30_kh_zhalf_t_scs15_kh_zquarter_t_l_>
        c;

    void destroy_();
  };

  // member variables
  bool                        ext                                 = false;
  bool                        ctrl_res_set_zero_present           = false;
  bool                        common_ctrl_res_set_present         = false;
  bool                        search_space_zero_present           = false;
  bool                        common_search_space_list_present    = false;
  bool                        search_space_sib1_present           = false;
  bool                        search_space_other_sys_info_present = false;
  bool                        paging_search_space_present         = false;
  bool                        ra_search_space_present             = false;
  uint8_t                     ctrl_res_set_zero                   = 0;
  ctrl_res_set_s              common_ctrl_res_set;
  uint8_t                     search_space_zero = 0;
  common_search_space_list_l_ common_search_space_list;
  uint8_t                     search_space_sib1           = 0;
  uint8_t                     search_space_other_sys_info = 0;
  uint8_t                     paging_search_space         = 0;
  uint8_t                     ra_search_space             = 0;
  // ...
  // group 0
  copy_ptr<first_pdcch_monitoring_occasion_of_po_c_> first_pdcch_monitoring_occasion_of_po;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDSCH-ConfigCommon ::= SEQUENCE
struct pdsch_cfg_common_s {
  bool                               ext                                  = false;
  bool                               pdsch_time_domain_alloc_list_present = false;
  pdsch_time_domain_res_alloc_list_l pdsch_time_domain_alloc_list;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PLMN-Identity ::= SEQUENCE
struct plmn_id_s {
  bool  mcc_present = false;
  mcc_l mcc;
  mnc_l mnc;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PUCCH-ConfigCommon ::= SEQUENCE
struct pucch_cfg_common_s {
  struct pucch_group_hop_opts {
    enum options { neither, enable, disable, nulltype } value;

    std::string to_string() const;
  };
  typedef enumerated<pucch_group_hop_opts> pucch_group_hop_e_;

  // member variables
  bool               ext                      = false;
  bool               pucch_res_common_present = false;
  bool               hop_id_present           = false;
  bool               p0_nominal_present       = false;
  uint8_t            pucch_res_common         = 0;
  pucch_group_hop_e_ pucch_group_hop;
  uint16_t           hop_id     = 0;
  int16_t            p0_nominal = -202;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PUSCH-ConfigCommon ::= SEQUENCE
struct pusch_cfg_common_s {
  bool                               ext                                           = false;
  bool                               group_hop_enabled_transform_precoding_present = false;
  bool                               pusch_time_domain_alloc_list_present          = false;
  bool                               msg3_delta_preamb_present                     = false;
  bool                               p0_nominal_with_grant_present                 = false;
  pusch_time_domain_res_alloc_list_l pusch_time_domain_alloc_list;
  int8_t                             msg3_delta_preamb     = -1;
  int16_t                            p0_nominal_with_grant = -202;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PagingCycle ::= ENUMERATED
struct paging_cycle_opts {
  enum options { rf32, rf64, rf128, rf256, nulltype } value;
  typedef uint16_t number_type;

  std::string to_string() const;
  uint16_t    to_number() const;
};
typedef enumerated<paging_cycle_opts> paging_cycle_e;

// RACH-ConfigCommon ::= SEQUENCE
struct rach_cfg_common_s {
  struct ssb_per_rach_occasion_and_cb_preambs_per_ssb_c_ {
    struct one_eighth_opts {
      enum options { n4, n8, n12, n16, n20, n24, n28, n32, n36, n40, n44, n48, n52, n56, n60, n64, nulltype } value;
      typedef uint8_t number_type;

      std::string to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<one_eighth_opts> one_eighth_e_;
    struct one_fourth_opts {
      enum options { n4, n8, n12, n16, n20, n24, n28, n32, n36, n40, n44, n48, n52, n56, n60, n64, nulltype } value;
      typedef uint8_t number_type;

      std::string to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<one_fourth_opts> one_fourth_e_;
    struct one_half_opts {
      enum options { n4, n8, n12, n16, n20, n24, n28, n32, n36, n40, n44, n48, n52, n56, n60, n64, nulltype } value;
      typedef uint8_t number_type;

      std::string to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<one_half_opts> one_half_e_;
    struct one_opts {
      enum options { n4, n8, n12, n16, n20, n24, n28, n32, n36, n40, n44, n48, n52, n56, n60, n64, nulltype } value;
      typedef uint8_t number_type;

      std::string to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<one_opts> one_e_;
    struct two_opts {
      enum options { n4, n8, n12, n16, n20, n24, n28, n32, nulltype } value;
      typedef uint8_t number_type;

      std::string to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<two_opts> two_e_;
    struct types_opts {
      enum options { one_eighth, one_fourth, one_half, one, two, four, eight, sixteen, nulltype } value;
      typedef float number_type;

      std::string to_string() const;
      float       to_number() const;
      std::string to_number_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    ssb_per_rach_occasion_and_cb_preambs_per_ssb_c_() = default;
    ssb_per_rach_occasion_and_cb_preambs_per_ssb_c_(const ssb_per_rach_occasion_and_cb_preambs_per_ssb_c_& other);
    ssb_per_rach_occasion_and_cb_preambs_per_ssb_c_&
    operator=(const ssb_per_rach_occasion_and_cb_preambs_per_ssb_c_& other);
    ~ssb_per_rach_occasion_and_cb_preambs_per_ssb_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    one_eighth_e_& one_eighth()
    {
      assert_choice_type("oneEighth", type_.to_string(), "ssb-perRACH-OccasionAndCB-PreamblesPerSSB");
      return c.get<one_eighth_e_>();
    }
    one_fourth_e_& one_fourth()
    {
      assert_choice_type("oneFourth", type_.to_string(), "ssb-perRACH-OccasionAndCB-PreamblesPerSSB");
      return c.get<one_fourth_e_>();
    }
    one_half_e_& one_half()
    {
      assert_choice_type("oneHalf", type_.to_string(), "ssb-perRACH-OccasionAndCB-PreamblesPerSSB");
      return c.get<one_half_e_>();
    }
    one_e_& one()
    {
      assert_choice_type("one", type_.to_string(), "ssb-perRACH-OccasionAndCB-PreamblesPerSSB");
      return c.get<one_e_>();
    }
    two_e_& two()
    {
      assert_choice_type("two", type_.to_string(), "ssb-perRACH-OccasionAndCB-PreamblesPerSSB");
      return c.get<two_e_>();
    }
    uint8_t& four()
    {
      assert_choice_type("four", type_.to_string(), "ssb-perRACH-OccasionAndCB-PreamblesPerSSB");
      return c.get<uint8_t>();
    }
    uint8_t& eight()
    {
      assert_choice_type("eight", type_.to_string(), "ssb-perRACH-OccasionAndCB-PreamblesPerSSB");
      return c.get<uint8_t>();
    }
    uint8_t& sixteen()
    {
      assert_choice_type("sixteen", type_.to_string(), "ssb-perRACH-OccasionAndCB-PreamblesPerSSB");
      return c.get<uint8_t>();
    }
    const one_eighth_e_& one_eighth() const
    {
      assert_choice_type("oneEighth", type_.to_string(), "ssb-perRACH-OccasionAndCB-PreamblesPerSSB");
      return c.get<one_eighth_e_>();
    }
    const one_fourth_e_& one_fourth() const
    {
      assert_choice_type("oneFourth", type_.to_string(), "ssb-perRACH-OccasionAndCB-PreamblesPerSSB");
      return c.get<one_fourth_e_>();
    }
    const one_half_e_& one_half() const
    {
      assert_choice_type("oneHalf", type_.to_string(), "ssb-perRACH-OccasionAndCB-PreamblesPerSSB");
      return c.get<one_half_e_>();
    }
    const one_e_& one() const
    {
      assert_choice_type("one", type_.to_string(), "ssb-perRACH-OccasionAndCB-PreamblesPerSSB");
      return c.get<one_e_>();
    }
    const two_e_& two() const
    {
      assert_choice_type("two", type_.to_string(), "ssb-perRACH-OccasionAndCB-PreamblesPerSSB");
      return c.get<two_e_>();
    }
    const uint8_t& four() const
    {
      assert_choice_type("four", type_.to_string(), "ssb-perRACH-OccasionAndCB-PreamblesPerSSB");
      return c.get<uint8_t>();
    }
    const uint8_t& eight() const
    {
      assert_choice_type("eight", type_.to_string(), "ssb-perRACH-OccasionAndCB-PreamblesPerSSB");
      return c.get<uint8_t>();
    }
    const uint8_t& sixteen() const
    {
      assert_choice_type("sixteen", type_.to_string(), "ssb-perRACH-OccasionAndCB-PreamblesPerSSB");
      return c.get<uint8_t>();
    }
    one_eighth_e_& set_one_eighth()
    {
      set(types::one_eighth);
      return c.get<one_eighth_e_>();
    }
    one_fourth_e_& set_one_fourth()
    {
      set(types::one_fourth);
      return c.get<one_fourth_e_>();
    }
    one_half_e_& set_one_half()
    {
      set(types::one_half);
      return c.get<one_half_e_>();
    }
    one_e_& set_one()
    {
      set(types::one);
      return c.get<one_e_>();
    }
    two_e_& set_two()
    {
      set(types::two);
      return c.get<two_e_>();
    }
    uint8_t& set_four()
    {
      set(types::four);
      return c.get<uint8_t>();
    }
    uint8_t& set_eight()
    {
      set(types::eight);
      return c.get<uint8_t>();
    }
    uint8_t& set_sixteen()
    {
      set(types::sixteen);
      return c.get<uint8_t>();
    }

  private:
    types               type_;
    pod_choice_buffer_t c;

    void destroy_();
  };
  struct group_bcfgured_s_ {
    struct ra_msg3_size_group_a_opts {
      enum options {
        b56,
        b144,
        b208,
        b256,
        b282,
        b480,
        b640,
        b800,
        b1000,
        b72,
        spare6,
        spare5,
        spare4,
        spare3,
        spare2,
        spare1,
        nulltype
      } value;
      typedef uint16_t number_type;

      std::string to_string() const;
      uint16_t    to_number() const;
    };
    typedef enumerated<ra_msg3_size_group_a_opts> ra_msg3_size_group_a_e_;
    struct msg_pwr_offset_group_b_opts {
      enum options { minusinfinity, db0, db5, db8, db10, db12, db15, db18, nulltype } value;
      typedef int8_t number_type;

      std::string to_string() const;
      int8_t      to_number() const;
    };
    typedef enumerated<msg_pwr_offset_group_b_opts> msg_pwr_offset_group_b_e_;

    // member variables
    ra_msg3_size_group_a_e_   ra_msg3_size_group_a;
    msg_pwr_offset_group_b_e_ msg_pwr_offset_group_b;
    uint8_t                   nof_ra_preambs_group_a = 1;
  };
  struct ra_contention_resolution_timer_opts {
    enum options { sf8, sf16, sf24, sf32, sf40, sf48, sf56, sf64, nulltype } value;
    typedef uint8_t number_type;

    std::string to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<ra_contention_resolution_timer_opts> ra_contention_resolution_timer_e_;
  struct prach_root_seq_idx_c_ {
    struct types_opts {
      enum options { l839, l139, nulltype } value;
      typedef uint16_t number_type;

      std::string to_string() const;
      uint16_t    to_number() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    prach_root_seq_idx_c_() = default;
    prach_root_seq_idx_c_(const prach_root_seq_idx_c_& other);
    prach_root_seq_idx_c_& operator=(const prach_root_seq_idx_c_& other);
    ~prach_root_seq_idx_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint16_t& l839()
    {
      assert_choice_type("l839", type_.to_string(), "prach-RootSequenceIndex");
      return c.get<uint16_t>();
    }
    uint8_t& l139()
    {
      assert_choice_type("l139", type_.to_string(), "prach-RootSequenceIndex");
      return c.get<uint8_t>();
    }
    const uint16_t& l839() const
    {
      assert_choice_type("l839", type_.to_string(), "prach-RootSequenceIndex");
      return c.get<uint16_t>();
    }
    const uint8_t& l139() const
    {
      assert_choice_type("l139", type_.to_string(), "prach-RootSequenceIndex");
      return c.get<uint8_t>();
    }
    uint16_t& set_l839()
    {
      set(types::l839);
      return c.get<uint16_t>();
    }
    uint8_t& set_l139()
    {
      set(types::l139);
      return c.get<uint8_t>();
    }

  private:
    types               type_;
    pod_choice_buffer_t c;

    void destroy_();
  };
  struct restricted_set_cfg_opts {
    enum options { unrestricted_set, restricted_set_type_a, restricted_set_type_b, nulltype } value;

    std::string to_string() const;
  };
  typedef enumerated<restricted_set_cfg_opts> restricted_set_cfg_e_;

  // member variables
  bool                                            ext                                                  = false;
  bool                                            total_nof_ra_preambs_present                         = false;
  bool                                            ssb_per_rach_occasion_and_cb_preambs_per_ssb_present = false;
  bool                                            group_bcfgured_present                               = false;
  bool                                            rsrp_thres_ssb_present                               = false;
  bool                                            rsrp_thres_ssb_sul_present                           = false;
  bool                                            msg1_subcarrier_spacing_present                      = false;
  bool                                            msg3_transform_precoder_present                      = false;
  rach_cfg_generic_s                              rach_cfg_generic;
  uint8_t                                         total_nof_ra_preambs = 1;
  ssb_per_rach_occasion_and_cb_preambs_per_ssb_c_ ssb_per_rach_occasion_and_cb_preambs_per_ssb;
  group_bcfgured_s_                               group_bcfgured;
  ra_contention_resolution_timer_e_               ra_contention_resolution_timer;
  uint8_t                                         rsrp_thres_ssb     = 0;
  uint8_t                                         rsrp_thres_ssb_sul = 0;
  prach_root_seq_idx_c_                           prach_root_seq_idx;
  subcarrier_spacing_e                            msg1_subcarrier_spacing;
  restricted_set_cfg_e_                           restricted_set_cfg;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SCS-SpecificCarrier ::= SEQUENCE
struct scs_specific_carrier_s {
  bool                 ext               = false;
  uint16_t             offset_to_carrier = 0;
  subcarrier_spacing_e subcarrier_spacing;
  uint16_t             carrier_bw = 1;
  // ...
  // group 0
  bool     tx_direct_current_location_v1530_present = false;
  uint16_t tx_direct_current_location_v1530         = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SIB-TypeInfo ::= SEQUENCE
struct sib_type_info_s {
  struct type_opts {
    enum options {
      sib_type2,
      sib_type3,
      sib_type4,
      sib_type5,
      sib_type6,
      sib_type7,
      sib_type8,
      sib_type9,
      spare8,
      spare7,
      spare6,
      spare5,
      spare4,
      spare3,
      spare2,
      spare1,
      // ...
      nulltype
    } value;
    typedef uint8_t number_type;

    std::string to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<type_opts, true> type_e_;

  // member variables
  bool    value_tag_present  = false;
  bool    area_scope_present = false;
  type_e_ type;
  uint8_t value_tag = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SetupRelease{ElementTypeParam} ::= CHOICE
template <class elem_type_paramT_>
struct setup_release_c {
  struct types_opts {
    enum options { release, setup, nulltype } value;

    std::string to_string() const;
  };
  typedef enumerated<types_opts> types;

  // choice methods
  setup_release_c() = default;
  void        set(typename types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  elem_type_paramT_& setup()
  {
    assert_choice_type("setup", type_.to_string(), "SetupRelease");
    return c;
  }
  const elem_type_paramT_& setup() const
  {
    assert_choice_type("setup", type_.to_string(), "SetupRelease");
    return c;
  }
  elem_type_paramT_& set_setup()
  {
    set(types::setup);
    return c;
  }

private:
  types             type_;
  elem_type_paramT_ c;
};

// UAC-BarringPerCat ::= SEQUENCE
struct uac_barr_per_cat_s {
  uint8_t access_category       = 1;
  uint8_t uac_barr_info_set_idx = 1;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BCCH-Config ::= SEQUENCE
struct bcch_cfg_s {
  struct mod_period_coeff_opts {
    enum options { n2, n4, n8, n16, nulltype } value;
    typedef uint8_t number_type;

    std::string to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<mod_period_coeff_opts> mod_period_coeff_e_;

  // member variables
  bool                ext = false;
  mod_period_coeff_e_ mod_period_coeff;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BWP-DownlinkCommon ::= SEQUENCE
struct bwp_dl_common_s {
  bool                                ext                      = false;
  bool                                pdcch_cfg_common_present = false;
  bool                                pdsch_cfg_common_present = false;
  bwp_s                               generic_params;
  setup_release_c<pdcch_cfg_common_s> pdcch_cfg_common;
  setup_release_c<pdsch_cfg_common_s> pdsch_cfg_common;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BWP-UplinkCommon ::= SEQUENCE
struct bwp_ul_common_s {
  bool                                ext                      = false;
  bool                                rach_cfg_common_present  = false;
  bool                                pusch_cfg_common_present = false;
  bool                                pucch_cfg_common_present = false;
  bwp_s                               generic_params;
  setup_release_c<rach_cfg_common_s>  rach_cfg_common;
  setup_release_c<pusch_cfg_common_s> pusch_cfg_common;
  setup_release_c<pucch_cfg_common_s> pucch_cfg_common;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CarrierFreqListEUTRA ::= SEQUENCE (SIZE (1..8)) OF CarrierFreqEUTRA
using carrier_freq_list_eutra_l = dyn_array<carrier_freq_eutra_s>;

// FrequencyInfoDL-SIB ::= SEQUENCE
struct freq_info_dl_sib_s {
  using scs_specific_carrier_list_l_ = dyn_array<scs_specific_carrier_s>;

  // member variables
  multi_freq_band_list_nr_sib_l freq_band_list;
  uint16_t                      offset_to_point_a = 0;
  scs_specific_carrier_list_l_  scs_specific_carrier_list;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// FrequencyInfoUL-SIB ::= SEQUENCE
struct freq_info_ul_sib_s {
  using scs_specific_carrier_list_l_ = dyn_array<scs_specific_carrier_s>;

  // member variables
  bool                          ext                           = false;
  bool                          freq_band_list_present        = false;
  bool                          absolute_freq_point_a_present = false;
  bool                          p_max_present                 = false;
  bool                          freq_shift7p5khz_present      = false;
  multi_freq_band_list_nr_sib_l freq_band_list;
  uint32_t                      absolute_freq_point_a = 0;
  scs_specific_carrier_list_l_  scs_specific_carrier_list;
  int8_t                        p_max = -30;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// InterFreqCarrierFreqList ::= SEQUENCE (SIZE (1..8)) OF InterFreqCarrierFreqInfo
using inter_freq_carrier_freq_list_l = dyn_array<inter_freq_carrier_freq_info_s>;

// IntraFreqBlackCellList ::= SEQUENCE (SIZE (1..16)) OF PCI-Range
using intra_freq_black_cell_list_l = dyn_array<pci_range_s>;

// IntraFreqNeighCellList ::= SEQUENCE (SIZE (1..16)) OF IntraFreqNeighCellInfo
using intra_freq_neigh_cell_list_l = dyn_array<intra_freq_neigh_cell_info_s>;

// MobilityStateParameters ::= SEQUENCE
struct mob_state_params_s {
  struct t_eval_opts {
    enum options { s30, s60, s120, s180, s240, spare3, spare2, spare1, nulltype } value;
    typedef uint8_t number_type;

    std::string to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<t_eval_opts> t_eval_e_;
  struct t_hyst_normal_opts {
    enum options { s30, s60, s120, s180, s240, spare3, spare2, spare1, nulltype } value;
    typedef uint8_t number_type;

    std::string to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<t_hyst_normal_opts> t_hyst_normal_e_;

  // member variables
  t_eval_e_        t_eval;
  t_hyst_normal_e_ t_hyst_normal;
  uint8_t          n_cell_change_medium = 1;
  uint8_t          n_cell_change_high   = 1;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PCCH-Config ::= SEQUENCE
struct pcch_cfg_s {
  struct nand_paging_frame_offset_c_ {
    struct types_opts {
      enum options { one_t, half_t, quarter_t, one_eighth_t, one_sixteenth_t, nulltype } value;
      typedef float number_type;

      std::string to_string() const;
      float       to_number() const;
      std::string to_number_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    nand_paging_frame_offset_c_() = default;
    nand_paging_frame_offset_c_(const nand_paging_frame_offset_c_& other);
    nand_paging_frame_offset_c_& operator=(const nand_paging_frame_offset_c_& other);
    ~nand_paging_frame_offset_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint8_t& half_t()
    {
      assert_choice_type("halfT", type_.to_string(), "nAndPagingFrameOffset");
      return c.get<uint8_t>();
    }
    uint8_t& quarter_t()
    {
      assert_choice_type("quarterT", type_.to_string(), "nAndPagingFrameOffset");
      return c.get<uint8_t>();
    }
    uint8_t& one_eighth_t()
    {
      assert_choice_type("oneEighthT", type_.to_string(), "nAndPagingFrameOffset");
      return c.get<uint8_t>();
    }
    uint8_t& one_sixteenth_t()
    {
      assert_choice_type("oneSixteenthT", type_.to_string(), "nAndPagingFrameOffset");
      return c.get<uint8_t>();
    }
    const uint8_t& half_t() const
    {
      assert_choice_type("halfT", type_.to_string(), "nAndPagingFrameOffset");
      return c.get<uint8_t>();
    }
    const uint8_t& quarter_t() const
    {
      assert_choice_type("quarterT", type_.to_string(), "nAndPagingFrameOffset");
      return c.get<uint8_t>();
    }
    const uint8_t& one_eighth_t() const
    {
      assert_choice_type("oneEighthT", type_.to_string(), "nAndPagingFrameOffset");
      return c.get<uint8_t>();
    }
    const uint8_t& one_sixteenth_t() const
    {
      assert_choice_type("oneSixteenthT", type_.to_string(), "nAndPagingFrameOffset");
      return c.get<uint8_t>();
    }
    uint8_t& set_half_t()
    {
      set(types::half_t);
      return c.get<uint8_t>();
    }
    uint8_t& set_quarter_t()
    {
      set(types::quarter_t);
      return c.get<uint8_t>();
    }
    uint8_t& set_one_eighth_t()
    {
      set(types::one_eighth_t);
      return c.get<uint8_t>();
    }
    uint8_t& set_one_sixteenth_t()
    {
      set(types::one_sixteenth_t);
      return c.get<uint8_t>();
    }

  private:
    types               type_;
    pod_choice_buffer_t c;

    void destroy_();
  };
  struct ns_opts {
    enum options { four, two, one, nulltype } value;
    typedef uint8_t number_type;

    std::string to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<ns_opts> ns_e_;
  struct first_pdcch_monitoring_occasion_of_po_c_ {
    using scs15_kh_zone_t_l_                                                              = bounded_array<uint8_t, 4>;
    using scs30_kh_zone_t_scs15_kh_zhalf_t_l_                                             = bounded_array<uint16_t, 4>;
    using scs60_kh_zone_t_scs30_kh_zhalf_t_scs15_kh_zquarter_t_l_                         = bounded_array<uint16_t, 4>;
    using scs120_kh_zone_t_scs60_kh_zhalf_t_scs30_kh_zquarter_t_scs15_kh_zone_eighth_t_l_ = bounded_array<uint16_t, 4>;
    using scs120_kh_zhalf_t_scs60_kh_zquarter_t_scs30_kh_zone_eighth_t_scs15_kh_zone_sixteenth_t_l_ =
        bounded_array<uint16_t, 4>;
    using scs120_kh_zquarter_t_scs60_kh_zone_eighth_t_scs30_kh_zone_sixteenth_t_l_ = bounded_array<uint16_t, 4>;
    using scs120_kh_zone_eighth_t_scs60_kh_zone_sixteenth_t_l_                     = bounded_array<uint16_t, 4>;
    using scs120_kh_zone_sixteenth_t_l_                                            = bounded_array<uint16_t, 4>;
    struct types_opts {
      enum options {
        scs15_kh_zone_t,
        scs30_kh_zone_t_scs15_kh_zhalf_t,
        scs60_kh_zone_t_scs30_kh_zhalf_t_scs15_kh_zquarter_t,
        scs120_kh_zone_t_scs60_kh_zhalf_t_scs30_kh_zquarter_t_scs15_kh_zone_eighth_t,
        scs120_kh_zhalf_t_scs60_kh_zquarter_t_scs30_kh_zone_eighth_t_scs15_kh_zone_sixteenth_t,
        scs120_kh_zquarter_t_scs60_kh_zone_eighth_t_scs30_kh_zone_sixteenth_t,
        scs120_kh_zone_eighth_t_scs60_kh_zone_sixteenth_t,
        scs120_kh_zone_sixteenth_t,
        nulltype
      } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    first_pdcch_monitoring_occasion_of_po_c_() = default;
    first_pdcch_monitoring_occasion_of_po_c_(const first_pdcch_monitoring_occasion_of_po_c_& other);
    first_pdcch_monitoring_occasion_of_po_c_& operator=(const first_pdcch_monitoring_occasion_of_po_c_& other);
    ~first_pdcch_monitoring_occasion_of_po_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    scs15_kh_zone_t_l_& scs15_kh_zone_t()
    {
      assert_choice_type("sCS15KHZoneT", type_.to_string(), "firstPDCCH-MonitoringOccasionOfPO");
      return c.get<scs15_kh_zone_t_l_>();
    }
    scs30_kh_zone_t_scs15_kh_zhalf_t_l_& scs30_kh_zone_t_scs15_kh_zhalf_t()
    {
      assert_choice_type("sCS30KHZoneT-SCS15KHZhalfT", type_.to_string(), "firstPDCCH-MonitoringOccasionOfPO");
      return c.get<scs30_kh_zone_t_scs15_kh_zhalf_t_l_>();
    }
    scs60_kh_zone_t_scs30_kh_zhalf_t_scs15_kh_zquarter_t_l_& scs60_kh_zone_t_scs30_kh_zhalf_t_scs15_kh_zquarter_t()
    {
      assert_choice_type(
          "sCS60KHZoneT-SCS30KHZhalfT-SCS15KHZquarterT", type_.to_string(), "firstPDCCH-MonitoringOccasionOfPO");
      return c.get<scs60_kh_zone_t_scs30_kh_zhalf_t_scs15_kh_zquarter_t_l_>();
    }
    scs120_kh_zone_t_scs60_kh_zhalf_t_scs30_kh_zquarter_t_scs15_kh_zone_eighth_t_l_&
    scs120_kh_zone_t_scs60_kh_zhalf_t_scs30_kh_zquarter_t_scs15_kh_zone_eighth_t()
    {
      assert_choice_type("sCS120KHZoneT-SCS60KHZhalfT-SCS30KHZquarterT-SCS15KHZoneEighthT",
                         type_.to_string(),
                         "firstPDCCH-MonitoringOccasionOfPO");
      return c.get<scs120_kh_zone_t_scs60_kh_zhalf_t_scs30_kh_zquarter_t_scs15_kh_zone_eighth_t_l_>();
    }
    scs120_kh_zhalf_t_scs60_kh_zquarter_t_scs30_kh_zone_eighth_t_scs15_kh_zone_sixteenth_t_l_&
    scs120_kh_zhalf_t_scs60_kh_zquarter_t_scs30_kh_zone_eighth_t_scs15_kh_zone_sixteenth_t()
    {
      assert_choice_type("sCS120KHZhalfT-SCS60KHZquarterT-SCS30KHZoneEighthT-SCS15KHZoneSixteenthT",
                         type_.to_string(),
                         "firstPDCCH-MonitoringOccasionOfPO");
      return c.get<scs120_kh_zhalf_t_scs60_kh_zquarter_t_scs30_kh_zone_eighth_t_scs15_kh_zone_sixteenth_t_l_>();
    }
    scs120_kh_zquarter_t_scs60_kh_zone_eighth_t_scs30_kh_zone_sixteenth_t_l_&
    scs120_kh_zquarter_t_scs60_kh_zone_eighth_t_scs30_kh_zone_sixteenth_t()
    {
      assert_choice_type("sCS120KHZquarterT-SCS60KHZoneEighthT-SCS30KHZoneSixteenthT",
                         type_.to_string(),
                         "firstPDCCH-MonitoringOccasionOfPO");
      return c.get<scs120_kh_zquarter_t_scs60_kh_zone_eighth_t_scs30_kh_zone_sixteenth_t_l_>();
    }
    scs120_kh_zone_eighth_t_scs60_kh_zone_sixteenth_t_l_& scs120_kh_zone_eighth_t_scs60_kh_zone_sixteenth_t()
    {
      assert_choice_type(
          "sCS120KHZoneEighthT-SCS60KHZoneSixteenthT", type_.to_string(), "firstPDCCH-MonitoringOccasionOfPO");
      return c.get<scs120_kh_zone_eighth_t_scs60_kh_zone_sixteenth_t_l_>();
    }
    scs120_kh_zone_sixteenth_t_l_& scs120_kh_zone_sixteenth_t()
    {
      assert_choice_type("sCS120KHZoneSixteenthT", type_.to_string(), "firstPDCCH-MonitoringOccasionOfPO");
      return c.get<scs120_kh_zone_sixteenth_t_l_>();
    }
    const scs15_kh_zone_t_l_& scs15_kh_zone_t() const
    {
      assert_choice_type("sCS15KHZoneT", type_.to_string(), "firstPDCCH-MonitoringOccasionOfPO");
      return c.get<scs15_kh_zone_t_l_>();
    }
    const scs30_kh_zone_t_scs15_kh_zhalf_t_l_& scs30_kh_zone_t_scs15_kh_zhalf_t() const
    {
      assert_choice_type("sCS30KHZoneT-SCS15KHZhalfT", type_.to_string(), "firstPDCCH-MonitoringOccasionOfPO");
      return c.get<scs30_kh_zone_t_scs15_kh_zhalf_t_l_>();
    }
    const scs60_kh_zone_t_scs30_kh_zhalf_t_scs15_kh_zquarter_t_l_&
    scs60_kh_zone_t_scs30_kh_zhalf_t_scs15_kh_zquarter_t() const
    {
      assert_choice_type(
          "sCS60KHZoneT-SCS30KHZhalfT-SCS15KHZquarterT", type_.to_string(), "firstPDCCH-MonitoringOccasionOfPO");
      return c.get<scs60_kh_zone_t_scs30_kh_zhalf_t_scs15_kh_zquarter_t_l_>();
    }
    const scs120_kh_zone_t_scs60_kh_zhalf_t_scs30_kh_zquarter_t_scs15_kh_zone_eighth_t_l_&
    scs120_kh_zone_t_scs60_kh_zhalf_t_scs30_kh_zquarter_t_scs15_kh_zone_eighth_t() const
    {
      assert_choice_type("sCS120KHZoneT-SCS60KHZhalfT-SCS30KHZquarterT-SCS15KHZoneEighthT",
                         type_.to_string(),
                         "firstPDCCH-MonitoringOccasionOfPO");
      return c.get<scs120_kh_zone_t_scs60_kh_zhalf_t_scs30_kh_zquarter_t_scs15_kh_zone_eighth_t_l_>();
    }
    const scs120_kh_zhalf_t_scs60_kh_zquarter_t_scs30_kh_zone_eighth_t_scs15_kh_zone_sixteenth_t_l_&
    scs120_kh_zhalf_t_scs60_kh_zquarter_t_scs30_kh_zone_eighth_t_scs15_kh_zone_sixteenth_t() const
    {
      assert_choice_type("sCS120KHZhalfT-SCS60KHZquarterT-SCS30KHZoneEighthT-SCS15KHZoneSixteenthT",
                         type_.to_string(),
                         "firstPDCCH-MonitoringOccasionOfPO");
      return c.get<scs120_kh_zhalf_t_scs60_kh_zquarter_t_scs30_kh_zone_eighth_t_scs15_kh_zone_sixteenth_t_l_>();
    }
    const scs120_kh_zquarter_t_scs60_kh_zone_eighth_t_scs30_kh_zone_sixteenth_t_l_&
    scs120_kh_zquarter_t_scs60_kh_zone_eighth_t_scs30_kh_zone_sixteenth_t() const
    {
      assert_choice_type("sCS120KHZquarterT-SCS60KHZoneEighthT-SCS30KHZoneSixteenthT",
                         type_.to_string(),
                         "firstPDCCH-MonitoringOccasionOfPO");
      return c.get<scs120_kh_zquarter_t_scs60_kh_zone_eighth_t_scs30_kh_zone_sixteenth_t_l_>();
    }
    const scs120_kh_zone_eighth_t_scs60_kh_zone_sixteenth_t_l_&
    scs120_kh_zone_eighth_t_scs60_kh_zone_sixteenth_t() const
    {
      assert_choice_type(
          "sCS120KHZoneEighthT-SCS60KHZoneSixteenthT", type_.to_string(), "firstPDCCH-MonitoringOccasionOfPO");
      return c.get<scs120_kh_zone_eighth_t_scs60_kh_zone_sixteenth_t_l_>();
    }
    const scs120_kh_zone_sixteenth_t_l_& scs120_kh_zone_sixteenth_t() const
    {
      assert_choice_type("sCS120KHZoneSixteenthT", type_.to_string(), "firstPDCCH-MonitoringOccasionOfPO");
      return c.get<scs120_kh_zone_sixteenth_t_l_>();
    }
    scs15_kh_zone_t_l_& set_scs15_kh_zone_t()
    {
      set(types::scs15_kh_zone_t);
      return c.get<scs15_kh_zone_t_l_>();
    }
    scs30_kh_zone_t_scs15_kh_zhalf_t_l_& set_scs30_kh_zone_t_scs15_kh_zhalf_t()
    {
      set(types::scs30_kh_zone_t_scs15_kh_zhalf_t);
      return c.get<scs30_kh_zone_t_scs15_kh_zhalf_t_l_>();
    }
    scs60_kh_zone_t_scs30_kh_zhalf_t_scs15_kh_zquarter_t_l_& set_scs60_kh_zone_t_scs30_kh_zhalf_t_scs15_kh_zquarter_t()
    {
      set(types::scs60_kh_zone_t_scs30_kh_zhalf_t_scs15_kh_zquarter_t);
      return c.get<scs60_kh_zone_t_scs30_kh_zhalf_t_scs15_kh_zquarter_t_l_>();
    }
    scs120_kh_zone_t_scs60_kh_zhalf_t_scs30_kh_zquarter_t_scs15_kh_zone_eighth_t_l_&
    set_scs120_kh_zone_t_scs60_kh_zhalf_t_scs30_kh_zquarter_t_scs15_kh_zone_eighth_t()
    {
      set(types::scs120_kh_zone_t_scs60_kh_zhalf_t_scs30_kh_zquarter_t_scs15_kh_zone_eighth_t);
      return c.get<scs120_kh_zone_t_scs60_kh_zhalf_t_scs30_kh_zquarter_t_scs15_kh_zone_eighth_t_l_>();
    }
    scs120_kh_zhalf_t_scs60_kh_zquarter_t_scs30_kh_zone_eighth_t_scs15_kh_zone_sixteenth_t_l_&
    set_scs120_kh_zhalf_t_scs60_kh_zquarter_t_scs30_kh_zone_eighth_t_scs15_kh_zone_sixteenth_t()
    {
      set(types::scs120_kh_zhalf_t_scs60_kh_zquarter_t_scs30_kh_zone_eighth_t_scs15_kh_zone_sixteenth_t);
      return c.get<scs120_kh_zhalf_t_scs60_kh_zquarter_t_scs30_kh_zone_eighth_t_scs15_kh_zone_sixteenth_t_l_>();
    }
    scs120_kh_zquarter_t_scs60_kh_zone_eighth_t_scs30_kh_zone_sixteenth_t_l_&
    set_scs120_kh_zquarter_t_scs60_kh_zone_eighth_t_scs30_kh_zone_sixteenth_t()
    {
      set(types::scs120_kh_zquarter_t_scs60_kh_zone_eighth_t_scs30_kh_zone_sixteenth_t);
      return c.get<scs120_kh_zquarter_t_scs60_kh_zone_eighth_t_scs30_kh_zone_sixteenth_t_l_>();
    }
    scs120_kh_zone_eighth_t_scs60_kh_zone_sixteenth_t_l_& set_scs120_kh_zone_eighth_t_scs60_kh_zone_sixteenth_t()
    {
      set(types::scs120_kh_zone_eighth_t_scs60_kh_zone_sixteenth_t);
      return c.get<scs120_kh_zone_eighth_t_scs60_kh_zone_sixteenth_t_l_>();
    }
    scs120_kh_zone_sixteenth_t_l_& set_scs120_kh_zone_sixteenth_t()
    {
      set(types::scs120_kh_zone_sixteenth_t);
      return c.get<scs120_kh_zone_sixteenth_t_l_>();
    }

  private:
    types type_;
    choice_buffer_t<scs120_kh_zhalf_t_scs60_kh_zquarter_t_scs30_kh_zone_eighth_t_scs15_kh_zone_sixteenth_t_l_,
                    scs120_kh_zone_eighth_t_scs60_kh_zone_sixteenth_t_l_,
                    scs120_kh_zone_sixteenth_t_l_,
                    scs120_kh_zone_t_scs60_kh_zhalf_t_scs30_kh_zquarter_t_scs15_kh_zone_eighth_t_l_,
                    scs120_kh_zquarter_t_scs60_kh_zone_eighth_t_scs30_kh_zone_sixteenth_t_l_,
                    scs15_kh_zone_t_l_,
                    scs30_kh_zone_t_scs15_kh_zhalf_t_l_,
                    scs60_kh_zone_t_scs30_kh_zhalf_t_scs15_kh_zquarter_t_l_>
        c;

    void destroy_();
  };

  // member variables
  bool                                     ext                                           = false;
  bool                                     first_pdcch_monitoring_occasion_of_po_present = false;
  paging_cycle_e                           default_paging_cycle;
  nand_paging_frame_offset_c_              nand_paging_frame_offset;
  ns_e_                                    ns;
  first_pdcch_monitoring_occasion_of_po_c_ first_pdcch_monitoring_occasion_of_po;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PLMN-IdentityInfo ::= SEQUENCE
struct plmn_id_info_s {
  using plmn_id_list_l_ = dyn_array<plmn_id_s>;
  struct cell_reserved_for_oper_opts {
    enum options { reserved, not_reserved, nulltype } value;

    std::string to_string() const;
  };
  typedef enumerated<cell_reserved_for_oper_opts> cell_reserved_for_oper_e_;

  // member variables
  bool                      ext           = false;
  bool                      tac_present   = false;
  bool                      ranac_present = false;
  plmn_id_list_l_           plmn_id_list;
  fixed_bitstring<24>       tac;
  uint16_t                  ranac = 0;
  fixed_bitstring<36>       cell_id;
  cell_reserved_for_oper_e_ cell_reserved_for_oper;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RangeToBestCell ::= Q-OffsetRange
typedef q_offset_range_e range_to_best_cell_e;

// SI-RequestResources ::= SEQUENCE
struct si_request_res_s {
  bool    ra_assoc_period_idx_present      = false;
  bool    ra_ssb_occasion_mask_idx_present = false;
  uint8_t ra_preamb_start_idx              = 0;
  uint8_t ra_assoc_period_idx              = 0;
  uint8_t ra_ssb_occasion_mask_idx         = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SIB-Mapping ::= SEQUENCE (SIZE (1..32)) OF SIB-TypeInfo
using sib_map_l = dyn_array<sib_type_info_s>;

// TDD-UL-DL-Pattern ::= SEQUENCE
struct tdd_ul_dl_pattern_s {
  struct dl_ul_tx_periodicity_opts {
    enum options { ms0p5, ms0p625, ms1, ms1p25, ms2, ms2p5, ms5, ms10, nulltype } value;

    std::string to_string() const;
  };
  typedef enumerated<dl_ul_tx_periodicity_opts> dl_ul_tx_periodicity_e_;
  struct dl_ul_tx_periodicity_v1530_opts {
    enum options { ms3, ms4, nulltype } value;
    typedef uint8_t number_type;

    std::string to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<dl_ul_tx_periodicity_v1530_opts> dl_ul_tx_periodicity_v1530_e_;

  // member variables
  bool                    ext = false;
  dl_ul_tx_periodicity_e_ dl_ul_tx_periodicity;
  uint16_t                nrof_dl_slots   = 0;
  uint8_t                 nrof_dl_symbols = 0;
  uint16_t                nrof_ul_slots   = 0;
  uint8_t                 nrof_ul_symbols = 0;
  // ...
  // group 0
  bool                          dl_ul_tx_periodicity_v1530_present = false;
  dl_ul_tx_periodicity_v1530_e_ dl_ul_tx_periodicity_v1530;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// TimeAlignmentTimer ::= ENUMERATED
struct time_align_timer_opts {
  enum options { ms500, ms750, ms1280, ms1920, ms2560, ms5120, ms10240, infinity, nulltype } value;
  typedef int16_t number_type;

  std::string to_string() const;
  int16_t     to_number() const;
};
typedef enumerated<time_align_timer_opts> time_align_timer_e;

// UAC-BarringPerCatList ::= SEQUENCE (SIZE (1..63)) OF UAC-BarringPerCat
using uac_barr_per_cat_list_l = dyn_array<uac_barr_per_cat_s>;

// DownlinkConfigCommonSIB ::= SEQUENCE
struct dl_cfg_common_sib_s {
  bool               ext = false;
  freq_info_dl_sib_s freq_info_dl;
  bwp_dl_common_s    init_dl_bwp;
  bcch_cfg_s         bcch_cfg;
  pcch_cfg_s         pcch_cfg;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PLMN-IdentityInfoList ::= SEQUENCE (SIZE (1..12)) OF PLMN-IdentityInfo
using plmn_id_info_list_l = dyn_array<plmn_id_info_s>;

// SI-RequestConfig ::= SEQUENCE
struct si_request_cfg_s {
  struct rach_occasions_si_s_ {
    struct ssb_per_rach_occasion_opts {
      enum options { one_eighth, one_fourth, one_half, one, two, four, eight, sixteen, nulltype } value;
      typedef float number_type;

      std::string to_string() const;
      float       to_number() const;
      std::string to_number_string() const;
    };
    typedef enumerated<ssb_per_rach_occasion_opts> ssb_per_rach_occasion_e_;

    // member variables
    rach_cfg_generic_s       rach_cfg_si;
    ssb_per_rach_occasion_e_ ssb_per_rach_occasion;
  };
  struct si_request_period_opts {
    enum options { one, two, four, six, eight, ten, twelve, sixteen, nulltype } value;

    std::string to_string() const;
  };
  typedef enumerated<si_request_period_opts> si_request_period_e_;
  using si_request_res_l_ = dyn_array<si_request_res_s>;

  // member variables
  bool                 rach_occasions_si_present = false;
  bool                 si_request_period_present = false;
  rach_occasions_si_s_ rach_occasions_si;
  si_request_period_e_ si_request_period;
  si_request_res_l_    si_request_res;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SIB2 ::= SEQUENCE
struct sib2_s {
  struct cell_resel_info_common_s_ {
    struct q_hyst_opts {
      enum options {
        db0,
        db1,
        db2,
        db3,
        db4,
        db5,
        db6,
        db8,
        db10,
        db12,
        db14,
        db16,
        db18,
        db20,
        db22,
        db24,
        nulltype
      } value;
      typedef uint8_t number_type;

      std::string to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<q_hyst_opts> q_hyst_e_;
    struct speed_state_resel_pars_s_ {
      struct q_hyst_sf_s_ {
        struct sf_medium_opts {
          enum options { db_minus6, db_minus4, db_minus2, db0, nulltype } value;
          typedef int8_t number_type;

          std::string to_string() const;
          int8_t      to_number() const;
        };
        typedef enumerated<sf_medium_opts> sf_medium_e_;
        struct sf_high_opts {
          enum options { db_minus6, db_minus4, db_minus2, db0, nulltype } value;
          typedef int8_t number_type;

          std::string to_string() const;
          int8_t      to_number() const;
        };
        typedef enumerated<sf_high_opts> sf_high_e_;

        // member variables
        sf_medium_e_ sf_medium;
        sf_high_e_   sf_high;
      };

      // member variables
      mob_state_params_s mob_state_params;
      q_hyst_sf_s_       q_hyst_sf;
    };

    // member variables
    bool                      ext                                        = false;
    bool                      nrof_ss_blocks_to_average_present          = false;
    bool                      abs_thresh_ss_blocks_consolidation_present = false;
    bool                      range_to_best_cell_present                 = false;
    bool                      speed_state_resel_pars_present             = false;
    uint8_t                   nrof_ss_blocks_to_average                  = 2;
    thres_nr_s                abs_thresh_ss_blocks_consolidation;
    range_to_best_cell_e      range_to_best_cell;
    q_hyst_e_                 q_hyst;
    speed_state_resel_pars_s_ speed_state_resel_pars;
    // ...
  };
  struct cell_resel_serving_freq_info_s_ {
    bool                  ext                          = false;
    bool                  s_non_intra_search_p_present = false;
    bool                  s_non_intra_search_q_present = false;
    bool                  thresh_serving_low_q_present = false;
    bool                  cell_resel_sub_prio_present  = false;
    uint8_t               s_non_intra_search_p         = 0;
    uint8_t               s_non_intra_search_q         = 0;
    uint8_t               thresh_serving_low_p         = 0;
    uint8_t               thresh_serving_low_q         = 0;
    uint8_t               cell_resel_prio              = 0;
    cell_resel_sub_prio_e cell_resel_sub_prio;
    // ...
  };
  struct intra_freq_cell_resel_info_s_ {
    bool                          ext                        = false;
    bool                          q_rx_lev_min_sul_present   = false;
    bool                          q_qual_min_present         = false;
    bool                          s_intra_search_q_present   = false;
    bool                          freq_band_list_present     = false;
    bool                          freq_band_list_sul_present = false;
    bool                          p_max_present              = false;
    bool                          smtc_present               = false;
    bool                          ss_rssi_meas_present       = false;
    bool                          ssb_to_measure_present     = false;
    int8_t                        q_rx_lev_min               = -70;
    int8_t                        q_rx_lev_min_sul           = -70;
    int8_t                        q_qual_min                 = -43;
    uint8_t                       s_intra_search_p           = 0;
    uint8_t                       s_intra_search_q           = 0;
    uint8_t                       t_resel_nr                 = 0;
    multi_freq_band_list_nr_sib_l freq_band_list;
    multi_freq_band_list_nr_sib_l freq_band_list_sul;
    int8_t                        p_max = -30;
    ssb_mtc_s                     smtc;
    ss_rssi_meas_s                ss_rssi_meas;
    ssb_to_measure_c              ssb_to_measure;
    bool                          derive_ssb_idx_from_cell = false;
    // ...
    // group 0
    copy_ptr<speed_state_scale_factors_s> t_resel_nr_sf;

    // sequence methods
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
  };

  // member variables
  bool                            ext = false;
  cell_resel_info_common_s_       cell_resel_info_common;
  cell_resel_serving_freq_info_s_ cell_resel_serving_freq_info;
  intra_freq_cell_resel_info_s_   intra_freq_cell_resel_info;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SIB3 ::= SEQUENCE
struct sib3_s {
  bool                         ext                                = false;
  bool                         intra_freq_neigh_cell_list_present = false;
  bool                         intra_freq_black_cell_list_present = false;
  bool                         late_non_crit_ext_present          = false;
  intra_freq_neigh_cell_list_l intra_freq_neigh_cell_list;
  intra_freq_black_cell_list_l intra_freq_black_cell_list;
  dyn_octstring                late_non_crit_ext;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SIB4 ::= SEQUENCE
struct sib4_s {
  bool                           ext                       = false;
  bool                           late_non_crit_ext_present = false;
  inter_freq_carrier_freq_list_l inter_freq_carrier_freq_list;
  dyn_octstring                  late_non_crit_ext;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SIB5 ::= SEQUENCE
struct sib5_s {
  bool                        ext                             = false;
  bool                        carrier_freq_list_eutra_present = false;
  bool                        t_resel_eutra_sf_present        = false;
  bool                        late_non_crit_ext_present       = false;
  carrier_freq_list_eutra_l   carrier_freq_list_eutra;
  uint8_t                     t_resel_eutra = 0;
  speed_state_scale_factors_s t_resel_eutra_sf;
  dyn_octstring               late_non_crit_ext;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SIB6 ::= SEQUENCE
struct sib6_s {
  bool                ext                       = false;
  bool                late_non_crit_ext_present = false;
  fixed_bitstring<16> msg_id;
  fixed_bitstring<16> serial_num;
  fixed_octstring<2>  warning_type;
  dyn_octstring       late_non_crit_ext;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SIB7 ::= SEQUENCE
struct sib7_s {
  struct warning_msg_segment_type_opts {
    enum options { not_last_segment, last_segment, nulltype } value;

    std::string to_string() const;
  };
  typedef enumerated<warning_msg_segment_type_opts> warning_msg_segment_type_e_;

  // member variables
  bool                        ext                        = false;
  bool                        data_coding_scheme_present = false;
  bool                        late_non_crit_ext_present  = false;
  fixed_bitstring<16>         msg_id;
  fixed_bitstring<16>         serial_num;
  warning_msg_segment_type_e_ warning_msg_segment_type;
  uint8_t                     warning_msg_segment_num = 0;
  dyn_octstring               warning_msg_segment;
  fixed_octstring<1>          data_coding_scheme;
  dyn_octstring               late_non_crit_ext;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SIB8 ::= SEQUENCE
struct sib8_s {
  struct warning_msg_segment_type_opts {
    enum options { not_last_segment, last_segment, nulltype } value;

    std::string to_string() const;
  };
  typedef enumerated<warning_msg_segment_type_opts> warning_msg_segment_type_e_;

  // member variables
  bool                        ext                                      = false;
  bool                        data_coding_scheme_present               = false;
  bool                        warning_area_coordinates_segment_present = false;
  bool                        late_non_crit_ext_present                = false;
  fixed_bitstring<16>         msg_id;
  fixed_bitstring<16>         serial_num;
  warning_msg_segment_type_e_ warning_msg_segment_type;
  uint8_t                     warning_msg_segment_num = 0;
  dyn_octstring               warning_msg_segment;
  fixed_octstring<1>          data_coding_scheme;
  dyn_octstring               warning_area_coordinates_segment;
  dyn_octstring               late_non_crit_ext;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SIB9 ::= SEQUENCE
struct sib9_s {
  struct time_info_s_ {
    bool               day_light_saving_time_present = false;
    bool               leap_seconds_present          = false;
    bool               local_time_offset_present     = false;
    uint64_t           time_info_utc                 = 0;
    fixed_bitstring<2> day_light_saving_time;
    int16_t            leap_seconds      = -127;
    int8_t             local_time_offset = -63;
  };

  // member variables
  bool          ext                       = false;
  bool          time_info_present         = false;
  bool          late_non_crit_ext_present = false;
  time_info_s_  time_info;
  dyn_octstring late_non_crit_ext;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SchedulingInfo ::= SEQUENCE
struct sched_info_s {
  struct si_broadcast_status_opts {
    enum options { broadcasting, not_broadcasting, nulltype } value;

    std::string to_string() const;
  };
  typedef enumerated<si_broadcast_status_opts> si_broadcast_status_e_;
  struct si_periodicity_opts {
    enum options { rf8, rf16, rf32, rf64, rf128, rf256, rf512, nulltype } value;
    typedef uint16_t number_type;

    std::string to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<si_periodicity_opts> si_periodicity_e_;

  // member variables
  si_broadcast_status_e_ si_broadcast_status;
  si_periodicity_e_      si_periodicity;
  sib_map_l              sib_map_info;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// TDD-UL-DL-ConfigCommon ::= SEQUENCE
struct tdd_ul_dl_cfg_common_s {
  bool                 ext              = false;
  bool                 pattern2_present = false;
  subcarrier_spacing_e ref_subcarrier_spacing;
  tdd_ul_dl_pattern_s  pattern1;
  tdd_ul_dl_pattern_s  pattern2;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UAC-BarringInfoSet ::= SEQUENCE
struct uac_barr_info_set_s {
  struct uac_barr_factor_opts {
    enum options { p00, p05, p10, p15, p20, p25, p30, p40, p50, p60, p70, p75, p80, p85, p90, p95, nulltype } value;
    typedef float number_type;

    std::string to_string() const;
    float       to_number() const;
    std::string to_number_string() const;
  };
  typedef enumerated<uac_barr_factor_opts> uac_barr_factor_e_;
  struct uac_barr_time_opts {
    enum options { s4, s8, s16, s32, s64, s128, s256, s512, nulltype } value;
    typedef uint16_t number_type;

    std::string to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<uac_barr_time_opts> uac_barr_time_e_;

  // member variables
  uac_barr_factor_e_ uac_barr_factor;
  uac_barr_time_e_   uac_barr_time;
  fixed_bitstring<7> uac_barr_for_access_id;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UAC-BarringPerPLMN ::= SEQUENCE
struct uac_barr_per_plmn_s {
  struct uac_ac_barr_list_type_c_ {
    using uac_implicit_ac_barr_list_l_ = std::array<uint8_t, 63>;
    struct types_opts {
      enum options { uac_implicit_ac_barr_list, uac_explicit_ac_barr_list, nulltype } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    uac_ac_barr_list_type_c_() = default;
    uac_ac_barr_list_type_c_(const uac_ac_barr_list_type_c_& other);
    uac_ac_barr_list_type_c_& operator=(const uac_ac_barr_list_type_c_& other);
    ~uac_ac_barr_list_type_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uac_implicit_ac_barr_list_l_& uac_implicit_ac_barr_list()
    {
      assert_choice_type("uac-ImplicitACBarringList", type_.to_string(), "uac-ACBarringListType");
      return c.get<uac_implicit_ac_barr_list_l_>();
    }
    uac_barr_per_cat_list_l& uac_explicit_ac_barr_list()
    {
      assert_choice_type("uac-ExplicitACBarringList", type_.to_string(), "uac-ACBarringListType");
      return c.get<uac_barr_per_cat_list_l>();
    }
    const uac_implicit_ac_barr_list_l_& uac_implicit_ac_barr_list() const
    {
      assert_choice_type("uac-ImplicitACBarringList", type_.to_string(), "uac-ACBarringListType");
      return c.get<uac_implicit_ac_barr_list_l_>();
    }
    const uac_barr_per_cat_list_l& uac_explicit_ac_barr_list() const
    {
      assert_choice_type("uac-ExplicitACBarringList", type_.to_string(), "uac-ACBarringListType");
      return c.get<uac_barr_per_cat_list_l>();
    }
    uac_implicit_ac_barr_list_l_& set_uac_implicit_ac_barr_list()
    {
      set(types::uac_implicit_ac_barr_list);
      return c.get<uac_implicit_ac_barr_list_l_>();
    }
    uac_barr_per_cat_list_l& set_uac_explicit_ac_barr_list()
    {
      set(types::uac_explicit_ac_barr_list);
      return c.get<uac_barr_per_cat_list_l>();
    }

  private:
    types                                                                  type_;
    choice_buffer_t<uac_barr_per_cat_list_l, uac_implicit_ac_barr_list_l_> c;

    void destroy_();
  };

  // member variables
  bool                     uac_ac_barr_list_type_present = false;
  uint8_t                  plmn_id_idx                   = 1;
  uac_ac_barr_list_type_c_ uac_ac_barr_list_type;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UplinkConfigCommonSIB ::= SEQUENCE
struct ul_cfg_common_sib_s {
  freq_info_ul_sib_s freq_info_ul;
  bwp_ul_common_s    init_ul_bwp;
  time_align_timer_e time_align_timer_common;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CellAccessRelatedInfo ::= SEQUENCE
struct cell_access_related_info_s {
  bool                ext                                 = false;
  bool                cell_reserved_for_other_use_present = false;
  plmn_id_info_list_l plmn_id_list;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ConnEstFailureControl ::= SEQUENCE
struct conn_est_fail_ctrl_s {
  struct conn_est_fail_count_opts {
    enum options { n1, n2, n3, n4, nulltype } value;
    typedef uint8_t number_type;

    std::string to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<conn_est_fail_count_opts> conn_est_fail_count_e_;
  struct conn_est_fail_offset_validity_opts {
    enum options { s30, s60, s120, s240, s300, s420, s600, s900, nulltype } value;
    typedef uint16_t number_type;

    std::string to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<conn_est_fail_offset_validity_opts> conn_est_fail_offset_validity_e_;

  // member variables
  bool                             conn_est_fail_offset_present = false;
  conn_est_fail_count_e_           conn_est_fail_count;
  conn_est_fail_offset_validity_e_ conn_est_fail_offset_validity;
  uint8_t                          conn_est_fail_offset = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SI-SchedulingInfo ::= SEQUENCE
struct si_sched_info_s {
  using sched_info_list_l_ = dyn_array<sched_info_s>;
  struct si_win_len_opts {
    enum options { s5, s10, s20, s40, s80, s160, s320, s640, s1280, nulltype } value;
    typedef uint16_t number_type;

    std::string to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<si_win_len_opts> si_win_len_e_;

  // member variables
  bool                ext                        = false;
  bool                si_request_cfg_present     = false;
  bool                si_request_cfg_sul_present = false;
  bool                sys_info_area_id_present   = false;
  sched_info_list_l_  sched_info_list;
  si_win_len_e_       si_win_len;
  si_request_cfg_s    si_request_cfg;
  si_request_cfg_s    si_request_cfg_sul;
  fixed_bitstring<24> sys_info_area_id;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ServingCellConfigCommonSIB ::= SEQUENCE
struct serving_cell_cfg_common_sib_s {
  struct n_timing_advance_offset_opts {
    enum options { n0, n25600, n39936, nulltype } value;
    typedef uint16_t number_type;

    std::string to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<n_timing_advance_offset_opts> n_timing_advance_offset_e_;
  struct ssb_positions_in_burst_s_ {
    bool               group_presence_present = false;
    fixed_bitstring<8> in_one_group;
    fixed_bitstring<8> group_presence;
  };
  struct ssb_periodicity_serving_cell_opts {
    enum options { ms5, ms10, ms20, ms40, ms80, ms160, nulltype } value;
    typedef uint8_t number_type;

    std::string to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<ssb_periodicity_serving_cell_opts> ssb_periodicity_serving_cell_e_;

  // member variables
  bool                            ext                             = false;
  bool                            ul_cfg_common_present           = false;
  bool                            supplementary_ul_present        = false;
  bool                            n_timing_advance_offset_present = false;
  bool                            tdd_ul_dl_cfg_common_present    = false;
  dl_cfg_common_sib_s             dl_cfg_common;
  ul_cfg_common_sib_s             ul_cfg_common;
  ul_cfg_common_sib_s             supplementary_ul;
  n_timing_advance_offset_e_      n_timing_advance_offset;
  ssb_positions_in_burst_s_       ssb_positions_in_burst;
  ssb_periodicity_serving_cell_e_ ssb_periodicity_serving_cell;
  tdd_ul_dl_cfg_common_s          tdd_ul_dl_cfg_common;
  int8_t                          ss_pbch_block_pwr = -60;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SystemInformation-IEs ::= SEQUENCE
struct sys_info_ies_s {
  struct sib_type_and_info_item_c_ {
    struct types_opts {
      enum options { sib2, sib3, sib4, sib5, sib6, sib7, sib8, sib9, /*...*/ nulltype } value;
      typedef uint8_t number_type;

      std::string to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts, true> types;

    // choice methods
    sib_type_and_info_item_c_() = default;
    sib_type_and_info_item_c_(const sib_type_and_info_item_c_& other);
    sib_type_and_info_item_c_& operator=(const sib_type_and_info_item_c_& other);
    ~sib_type_and_info_item_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    sib2_s& sib2()
    {
      assert_choice_type("sib2", type_.to_string(), "sib-TypeAndInfo-item");
      return c.get<sib2_s>();
    }
    sib3_s& sib3()
    {
      assert_choice_type("sib3", type_.to_string(), "sib-TypeAndInfo-item");
      return c.get<sib3_s>();
    }
    sib4_s& sib4()
    {
      assert_choice_type("sib4", type_.to_string(), "sib-TypeAndInfo-item");
      return c.get<sib4_s>();
    }
    sib5_s& sib5()
    {
      assert_choice_type("sib5", type_.to_string(), "sib-TypeAndInfo-item");
      return c.get<sib5_s>();
    }
    sib6_s& sib6()
    {
      assert_choice_type("sib6", type_.to_string(), "sib-TypeAndInfo-item");
      return c.get<sib6_s>();
    }
    sib7_s& sib7()
    {
      assert_choice_type("sib7", type_.to_string(), "sib-TypeAndInfo-item");
      return c.get<sib7_s>();
    }
    sib8_s& sib8()
    {
      assert_choice_type("sib8", type_.to_string(), "sib-TypeAndInfo-item");
      return c.get<sib8_s>();
    }
    sib9_s& sib9()
    {
      assert_choice_type("sib9", type_.to_string(), "sib-TypeAndInfo-item");
      return c.get<sib9_s>();
    }
    const sib2_s& sib2() const
    {
      assert_choice_type("sib2", type_.to_string(), "sib-TypeAndInfo-item");
      return c.get<sib2_s>();
    }
    const sib3_s& sib3() const
    {
      assert_choice_type("sib3", type_.to_string(), "sib-TypeAndInfo-item");
      return c.get<sib3_s>();
    }
    const sib4_s& sib4() const
    {
      assert_choice_type("sib4", type_.to_string(), "sib-TypeAndInfo-item");
      return c.get<sib4_s>();
    }
    const sib5_s& sib5() const
    {
      assert_choice_type("sib5", type_.to_string(), "sib-TypeAndInfo-item");
      return c.get<sib5_s>();
    }
    const sib6_s& sib6() const
    {
      assert_choice_type("sib6", type_.to_string(), "sib-TypeAndInfo-item");
      return c.get<sib6_s>();
    }
    const sib7_s& sib7() const
    {
      assert_choice_type("sib7", type_.to_string(), "sib-TypeAndInfo-item");
      return c.get<sib7_s>();
    }
    const sib8_s& sib8() const
    {
      assert_choice_type("sib8", type_.to_string(), "sib-TypeAndInfo-item");
      return c.get<sib8_s>();
    }
    const sib9_s& sib9() const
    {
      assert_choice_type("sib9", type_.to_string(), "sib-TypeAndInfo-item");
      return c.get<sib9_s>();
    }
    sib2_s& set_sib2()
    {
      set(types::sib2);
      return c.get<sib2_s>();
    }
    sib3_s& set_sib3()
    {
      set(types::sib3);
      return c.get<sib3_s>();
    }
    sib4_s& set_sib4()
    {
      set(types::sib4);
      return c.get<sib4_s>();
    }
    sib5_s& set_sib5()
    {
      set(types::sib5);
      return c.get<sib5_s>();
    }
    sib6_s& set_sib6()
    {
      set(types::sib6);
      return c.get<sib6_s>();
    }
    sib7_s& set_sib7()
    {
      set(types::sib7);
      return c.get<sib7_s>();
    }
    sib8_s& set_sib8()
    {
      set(types::sib8);
      return c.get<sib8_s>();
    }
    sib9_s& set_sib9()
    {
      set(types::sib9);
      return c.get<sib9_s>();
    }

  private:
    types                                                                           type_;
    choice_buffer_t<sib2_s, sib3_s, sib4_s, sib5_s, sib6_s, sib7_s, sib8_s, sib9_s> c;

    void destroy_();
  };
  using sib_type_and_info_l_ = dyn_array<sib_type_and_info_item_c_>;

  // member variables
  bool                 late_non_crit_ext_present = false;
  bool                 non_crit_ext_present      = false;
  sib_type_and_info_l_ sib_type_and_info;
  dyn_octstring        late_non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UAC-AccessCategory1-SelectionAssistanceInfo ::= ENUMERATED
struct uac_access_category1_sel_assist_info_opts {
  enum options { a, b, c, nulltype } value;

  std::string to_string() const;
};
typedef enumerated<uac_access_category1_sel_assist_info_opts> uac_access_category1_sel_assist_info_e;

// UAC-BarringInfoSetList ::= SEQUENCE (SIZE (1..8)) OF UAC-BarringInfoSet
using uac_barr_info_set_list_l = dyn_array<uac_barr_info_set_s>;

// UAC-BarringPerPLMN-List ::= SEQUENCE (SIZE (1..12)) OF UAC-BarringPerPLMN
using uac_barr_per_plmn_list_l = dyn_array<uac_barr_per_plmn_s>;

// UE-TimersAndConstants ::= SEQUENCE
struct ue_timers_and_consts_s {
  struct t300_opts {
    enum options { ms100, ms200, ms300, ms400, ms600, ms1000, ms1500, ms2000, nulltype } value;
    typedef uint16_t number_type;

    std::string to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<t300_opts> t300_e_;
  struct t301_opts {
    enum options { ms100, ms200, ms300, ms400, ms600, ms1000, ms1500, ms2000, nulltype } value;
    typedef uint16_t number_type;

    std::string to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<t301_opts> t301_e_;
  struct t310_opts {
    enum options { ms0, ms50, ms100, ms200, ms500, ms1000, ms2000, nulltype } value;
    typedef uint16_t number_type;

    std::string to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<t310_opts> t310_e_;
  struct n310_opts {
    enum options { n1, n2, n3, n4, n6, n8, n10, n20, nulltype } value;
    typedef uint8_t number_type;

    std::string to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<n310_opts> n310_e_;
  struct t311_opts {
    enum options { ms1000, ms3000, ms5000, ms10000, ms15000, ms20000, ms30000, nulltype } value;
    typedef uint16_t number_type;

    std::string to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<t311_opts> t311_e_;
  struct n311_opts {
    enum options { n1, n2, n3, n4, n5, n6, n8, n10, nulltype } value;
    typedef uint8_t number_type;

    std::string to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<n311_opts> n311_e_;
  struct t319_opts {
    enum options { ms100, ms200, ms300, ms400, ms600, ms1000, ms1500, ms2000, nulltype } value;
    typedef uint16_t number_type;

    std::string to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<t319_opts> t319_e_;

  // member variables
  bool    ext = false;
  t300_e_ t300;
  t301_e_ t301;
  t310_e_ t310;
  n310_e_ n310;
  t311_e_ t311;
  n311_e_ n311;
  t319_e_ t319;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SIB1 ::= SEQUENCE
struct sib1_s {
  struct cell_sel_info_s_ {
    bool    q_rx_lev_min_offset_present = false;
    bool    q_rx_lev_min_sul_present    = false;
    bool    q_qual_min_present          = false;
    bool    q_qual_min_offset_present   = false;
    int8_t  q_rx_lev_min                = -70;
    uint8_t q_rx_lev_min_offset         = 1;
    int8_t  q_rx_lev_min_sul            = -70;
    int8_t  q_qual_min                  = -43;
    uint8_t q_qual_min_offset           = 1;
  };
  struct uac_barr_info_s_ {
    struct uac_access_category1_sel_assist_info_c_ {
      using individual_plmn_list_l_ = bounded_array<uac_access_category1_sel_assist_info_e, 12>;
      struct types_opts {
        enum options { plmn_common, individual_plmn_list, nulltype } value;

        std::string to_string() const;
      };
      typedef enumerated<types_opts> types;

      // choice methods
      uac_access_category1_sel_assist_info_c_() = default;
      uac_access_category1_sel_assist_info_c_(const uac_access_category1_sel_assist_info_c_& other);
      uac_access_category1_sel_assist_info_c_& operator=(const uac_access_category1_sel_assist_info_c_& other);
      ~uac_access_category1_sel_assist_info_c_() { destroy_(); }
      void        set(types::options e = types::nulltype);
      types       type() const { return type_; }
      SRSASN_CODE pack(bit_ref& bref) const;
      SRSASN_CODE unpack(cbit_ref& bref);
      void        to_json(json_writer& j) const;
      // getters
      uac_access_category1_sel_assist_info_e& plmn_common()
      {
        assert_choice_type("plmnCommon", type_.to_string(), "uac-AccessCategory1-SelectionAssistanceInfo");
        return c.get<uac_access_category1_sel_assist_info_e>();
      }
      individual_plmn_list_l_& individual_plmn_list()
      {
        assert_choice_type("individualPLMNList", type_.to_string(), "uac-AccessCategory1-SelectionAssistanceInfo");
        return c.get<individual_plmn_list_l_>();
      }
      const uac_access_category1_sel_assist_info_e& plmn_common() const
      {
        assert_choice_type("plmnCommon", type_.to_string(), "uac-AccessCategory1-SelectionAssistanceInfo");
        return c.get<uac_access_category1_sel_assist_info_e>();
      }
      const individual_plmn_list_l_& individual_plmn_list() const
      {
        assert_choice_type("individualPLMNList", type_.to_string(), "uac-AccessCategory1-SelectionAssistanceInfo");
        return c.get<individual_plmn_list_l_>();
      }
      uac_access_category1_sel_assist_info_e& set_plmn_common()
      {
        set(types::plmn_common);
        return c.get<uac_access_category1_sel_assist_info_e>();
      }
      individual_plmn_list_l_& set_individual_plmn_list()
      {
        set(types::individual_plmn_list);
        return c.get<individual_plmn_list_l_>();
      }

    private:
      types                                    type_;
      choice_buffer_t<individual_plmn_list_l_> c;

      void destroy_();
    };

    // member variables
    bool                                    uac_barr_for_common_present                  = false;
    bool                                    uac_barr_per_plmn_list_present               = false;
    bool                                    uac_access_category1_sel_assist_info_present = false;
    uac_barr_per_cat_list_l                 uac_barr_for_common;
    uac_barr_per_plmn_list_l                uac_barr_per_plmn_list;
    uac_barr_info_set_list_l                uac_barr_info_set_list;
    uac_access_category1_sel_assist_info_c_ uac_access_category1_sel_assist_info;
  };

  // member variables
  bool                          cell_sel_info_present           = false;
  bool                          conn_est_fail_ctrl_present      = false;
  bool                          si_sched_info_present           = false;
  bool                          serving_cell_cfg_common_present = false;
  bool                          ims_emergency_support_present   = false;
  bool                          ecall_over_ims_support_present  = false;
  bool                          ue_timers_and_consts_present    = false;
  bool                          uac_barr_info_present           = false;
  bool                          use_full_resume_id_present      = false;
  bool                          late_non_crit_ext_present       = false;
  bool                          non_crit_ext_present            = false;
  cell_sel_info_s_              cell_sel_info;
  cell_access_related_info_s    cell_access_related_info;
  conn_est_fail_ctrl_s          conn_est_fail_ctrl;
  si_sched_info_s               si_sched_info;
  serving_cell_cfg_common_sib_s serving_cell_cfg_common;
  ue_timers_and_consts_s        ue_timers_and_consts;
  uac_barr_info_s_              uac_barr_info;
  dyn_octstring                 late_non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SystemInformation ::= SEQUENCE
struct sys_info_s {
  struct crit_exts_c_ {
    struct types_opts {
      enum options { sys_info_r15, crit_exts_future, nulltype } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    crit_exts_c_() = default;
    crit_exts_c_(const crit_exts_c_& other);
    crit_exts_c_& operator=(const crit_exts_c_& other);
    ~crit_exts_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    sys_info_ies_s& sys_info_r15()
    {
      assert_choice_type("systemInformation-r15", type_.to_string(), "criticalExtensions");
      return c.get<sys_info_ies_s>();
    }
    const sys_info_ies_s& sys_info_r15() const
    {
      assert_choice_type("systemInformation-r15", type_.to_string(), "criticalExtensions");
      return c.get<sys_info_ies_s>();
    }
    sys_info_ies_s& set_sys_info_r15()
    {
      set(types::sys_info_r15);
      return c.get<sys_info_ies_s>();
    }

  private:
    types                           type_;
    choice_buffer_t<sys_info_ies_s> c;

    void destroy_();
  };

  // member variables
  crit_exts_c_ crit_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BCCH-DL-SCH-MessageType ::= CHOICE
struct bcch_dl_sch_msg_type_c {
  struct c1_c_ {
    struct types_opts {
      enum options { sys_info, sib_type1, nulltype } value;
      typedef uint8_t number_type;

      std::string to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    c1_c_() = default;
    c1_c_(const c1_c_& other);
    c1_c_& operator=(const c1_c_& other);
    ~c1_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    sys_info_s& sys_info()
    {
      assert_choice_type("systemInformation", type_.to_string(), "c1");
      return c.get<sys_info_s>();
    }
    sib1_s& sib_type1()
    {
      assert_choice_type("systemInformationBlockType1", type_.to_string(), "c1");
      return c.get<sib1_s>();
    }
    const sys_info_s& sys_info() const
    {
      assert_choice_type("systemInformation", type_.to_string(), "c1");
      return c.get<sys_info_s>();
    }
    const sib1_s& sib_type1() const
    {
      assert_choice_type("systemInformationBlockType1", type_.to_string(), "c1");
      return c.get<sib1_s>();
    }
    sys_info_s& set_sys_info()
    {
      set(types::sys_info);
      return c.get<sys_info_s>();
    }
    sib1_s& set_sib_type1()
    {
      set(types::sib_type1);
      return c.get<sib1_s>();
    }

  private:
    types                               type_;
    choice_buffer_t<sib1_s, sys_info_s> c;

    void destroy_();
  };
  struct types_opts {
    enum options { c1, msg_class_ext, nulltype } value;
    typedef uint8_t number_type;

    std::string to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<types_opts> types;

  // choice methods
  bcch_dl_sch_msg_type_c() = default;
  bcch_dl_sch_msg_type_c(const bcch_dl_sch_msg_type_c& other);
  bcch_dl_sch_msg_type_c& operator=(const bcch_dl_sch_msg_type_c& other);
  ~bcch_dl_sch_msg_type_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  c1_c_& c1()
  {
    assert_choice_type("c1", type_.to_string(), "BCCH-DL-SCH-MessageType");
    return c.get<c1_c_>();
  }
  const c1_c_& c1() const
  {
    assert_choice_type("c1", type_.to_string(), "BCCH-DL-SCH-MessageType");
    return c.get<c1_c_>();
  }
  c1_c_& set_c1()
  {
    set(types::c1);
    return c.get<c1_c_>();
  }

private:
  types                  type_;
  choice_buffer_t<c1_c_> c;

  void destroy_();
};

// BCCH-DL-SCH-Message ::= SEQUENCE
struct bcch_dl_sch_msg_s {
  bcch_dl_sch_msg_type_c msg;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// Alpha ::= ENUMERATED
struct alpha_opts {
  enum options { alpha0, alpha04, alpha05, alpha06, alpha07, alpha08, alpha09, alpha1, nulltype } value;
  typedef float number_type;

  std::string to_string() const;
  float       to_number() const;
  std::string to_number_string() const;
};
typedef enumerated<alpha_opts> alpha_e;

// BFR-CSIRS-Resource ::= SEQUENCE
struct bfr_csirs_res_s {
  using ra_occasion_list_l_ = dyn_array<uint16_t>;

  // member variables
  bool                ext                      = false;
  bool                ra_occasion_list_present = false;
  bool                ra_preamb_idx_present    = false;
  uint8_t             csi_rs                   = 0;
  ra_occasion_list_l_ ra_occasion_list;
  uint8_t             ra_preamb_idx = 0;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BFR-SSB-Resource ::= SEQUENCE
struct bfr_ssb_res_s {
  bool    ext           = false;
  uint8_t ssb           = 0;
  uint8_t ra_preamb_idx = 0;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BetaOffsets ::= SEQUENCE
struct beta_offsets_s {
  bool    beta_offset_ack_idx1_present       = false;
  bool    beta_offset_ack_idx2_present       = false;
  bool    beta_offset_ack_idx3_present       = false;
  bool    beta_offset_csi_part1_idx1_present = false;
  bool    beta_offset_csi_part1_idx2_present = false;
  bool    beta_offset_csi_part2_idx1_present = false;
  bool    beta_offset_csi_part2_idx2_present = false;
  uint8_t beta_offset_ack_idx1               = 0;
  uint8_t beta_offset_ack_idx2               = 0;
  uint8_t beta_offset_ack_idx3               = 0;
  uint8_t beta_offset_csi_part1_idx1         = 0;
  uint8_t beta_offset_csi_part1_idx2         = 0;
  uint8_t beta_offset_csi_part2_idx1         = 0;
  uint8_t beta_offset_csi_part2_idx2         = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CSI-FrequencyOccupation ::= SEQUENCE
struct csi_freq_occupation_s {
  bool     ext      = false;
  uint16_t start_rb = 0;
  uint16_t nrof_rbs = 24;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// P0-PUCCH ::= SEQUENCE
struct p0_pucch_s {
  uint8_t p0_pucch_id    = 1;
  int8_t  p0_pucch_value = -16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// P0-PUSCH-AlphaSet ::= SEQUENCE
struct p0_pusch_alpha_set_s {
  bool    p0_present            = false;
  bool    alpha_present         = false;
  uint8_t p0_pusch_alpha_set_id = 0;
  int8_t  p0                    = -16;
  alpha_e alpha;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PTRS-UplinkConfig ::= SEQUENCE
struct ptrs_ul_cfg_s {
  struct transform_precoder_disabled_s_ {
    using freq_density_l_ = std::array<uint16_t, 2>;
    using time_density_l_ = std::array<uint8_t, 3>;
    struct max_nrof_ports_opts {
      enum options { n1, n2, nulltype } value;
      typedef uint8_t number_type;

      std::string to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<max_nrof_ports_opts> max_nrof_ports_e_;
    struct res_elem_offset_opts {
      enum options { offset01, offset10, offset11, nulltype } value;
      typedef float number_type;

      std::string to_string() const;
      float       to_number() const;
      std::string to_number_string() const;
    };
    typedef enumerated<res_elem_offset_opts> res_elem_offset_e_;
    struct ptrs_pwr_opts {
      enum options { p00, p01, p10, p11, nulltype } value;
      typedef float number_type;

      std::string to_string() const;
      float       to_number() const;
      std::string to_number_string() const;
    };
    typedef enumerated<ptrs_pwr_opts> ptrs_pwr_e_;

    // member variables
    bool               freq_density_present    = false;
    bool               time_density_present    = false;
    bool               res_elem_offset_present = false;
    freq_density_l_    freq_density;
    time_density_l_    time_density;
    max_nrof_ports_e_  max_nrof_ports;
    res_elem_offset_e_ res_elem_offset;
    ptrs_pwr_e_        ptrs_pwr;
  };
  struct transform_precoder_enabled_s_ {
    using sample_density_l_ = std::array<uint16_t, 5>;

    // member variables
    bool              time_density_transform_precoding_present = false;
    sample_density_l_ sample_density;
  };

  // member variables
  bool                           ext                                 = false;
  bool                           transform_precoder_disabled_present = false;
  bool                           transform_precoder_enabled_present  = false;
  transform_precoder_disabled_s_ transform_precoder_disabled;
  transform_precoder_enabled_s_  transform_precoder_enabled;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PUCCH-MaxCodeRate ::= ENUMERATED
struct pucch_max_code_rate_opts {
  enum options { zero_dot08, zero_dot15, zero_dot25, zero_dot35, zero_dot45, zero_dot60, zero_dot80, nulltype } value;
  typedef float number_type;

  std::string to_string() const;
  float       to_number() const;
  std::string to_number_string() const;
};
typedef enumerated<pucch_max_code_rate_opts> pucch_max_code_rate_e;

// PUCCH-PathlossReferenceRS ::= SEQUENCE
struct pucch_pathloss_ref_rs_s {
  struct ref_sig_c_ {
    struct types_opts {
      enum options { ssb_idx, csi_rs_idx, nulltype } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    ref_sig_c_() = default;
    ref_sig_c_(const ref_sig_c_& other);
    ref_sig_c_& operator=(const ref_sig_c_& other);
    ~ref_sig_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint8_t& ssb_idx()
    {
      assert_choice_type("ssb-Index", type_.to_string(), "referenceSignal");
      return c.get<uint8_t>();
    }
    uint8_t& csi_rs_idx()
    {
      assert_choice_type("csi-RS-Index", type_.to_string(), "referenceSignal");
      return c.get<uint8_t>();
    }
    const uint8_t& ssb_idx() const
    {
      assert_choice_type("ssb-Index", type_.to_string(), "referenceSignal");
      return c.get<uint8_t>();
    }
    const uint8_t& csi_rs_idx() const
    {
      assert_choice_type("csi-RS-Index", type_.to_string(), "referenceSignal");
      return c.get<uint8_t>();
    }
    uint8_t& set_ssb_idx()
    {
      set(types::ssb_idx);
      return c.get<uint8_t>();
    }
    uint8_t& set_csi_rs_idx()
    {
      set(types::csi_rs_idx);
      return c.get<uint8_t>();
    }

  private:
    types               type_;
    pod_choice_buffer_t c;

    void destroy_();
  };

  // member variables
  uint8_t    pucch_pathloss_ref_rs_id = 0;
  ref_sig_c_ ref_sig;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PUCCH-format0 ::= SEQUENCE
struct pucch_format0_s {
  uint8_t init_cyclic_shift = 0;
  uint8_t nrof_symbols      = 1;
  uint8_t start_symbol_idx  = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PUCCH-format1 ::= SEQUENCE
struct pucch_format1_s {
  uint8_t init_cyclic_shift = 0;
  uint8_t nrof_symbols      = 4;
  uint8_t start_symbol_idx  = 0;
  uint8_t time_domain_occ   = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PUCCH-format2 ::= SEQUENCE
struct pucch_format2_s {
  uint8_t nrof_prbs        = 1;
  uint8_t nrof_symbols     = 1;
  uint8_t start_symbol_idx = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PUCCH-format3 ::= SEQUENCE
struct pucch_format3_s {
  uint8_t nrof_prbs        = 1;
  uint8_t nrof_symbols     = 4;
  uint8_t start_symbol_idx = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PUCCH-format4 ::= SEQUENCE
struct pucch_format4_s {
  struct occ_len_opts {
    enum options { n2, n4, nulltype } value;
    typedef uint8_t number_type;

    std::string to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<occ_len_opts> occ_len_e_;
  struct occ_idx_opts {
    enum options { n0, n1, n2, n3, nulltype } value;
    typedef uint8_t number_type;

    std::string to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<occ_idx_opts> occ_idx_e_;

  // member variables
  uint8_t    nrof_symbols = 4;
  occ_len_e_ occ_len;
  occ_idx_e_ occ_idx;
  uint8_t    start_symbol_idx = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PUSCH-PathlossReferenceRS ::= SEQUENCE
struct pusch_pathloss_ref_rs_s {
  struct ref_sig_c_ {
    struct types_opts {
      enum options { ssb_idx, csi_rs_idx, nulltype } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    ref_sig_c_() = default;
    ref_sig_c_(const ref_sig_c_& other);
    ref_sig_c_& operator=(const ref_sig_c_& other);
    ~ref_sig_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint8_t& ssb_idx()
    {
      assert_choice_type("ssb-Index", type_.to_string(), "referenceSignal");
      return c.get<uint8_t>();
    }
    uint8_t& csi_rs_idx()
    {
      assert_choice_type("csi-RS-Index", type_.to_string(), "referenceSignal");
      return c.get<uint8_t>();
    }
    const uint8_t& ssb_idx() const
    {
      assert_choice_type("ssb-Index", type_.to_string(), "referenceSignal");
      return c.get<uint8_t>();
    }
    const uint8_t& csi_rs_idx() const
    {
      assert_choice_type("csi-RS-Index", type_.to_string(), "referenceSignal");
      return c.get<uint8_t>();
    }
    uint8_t& set_ssb_idx()
    {
      set(types::ssb_idx);
      return c.get<uint8_t>();
    }
    uint8_t& set_csi_rs_idx()
    {
      set(types::csi_rs_idx);
      return c.get<uint8_t>();
    }

  private:
    types               type_;
    pod_choice_buffer_t c;

    void destroy_();
  };

  // member variables
  uint8_t    pusch_pathloss_ref_rs_id = 0;
  ref_sig_c_ ref_sig;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SRI-PUSCH-PowerControl ::= SEQUENCE
struct sri_pusch_pwr_ctrl_s {
  struct sri_pusch_closed_loop_idx_opts {
    enum options { i0, i1, nulltype } value;
    typedef uint8_t number_type;

    std::string to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<sri_pusch_closed_loop_idx_opts> sri_pusch_closed_loop_idx_e_;

  // member variables
  uint8_t                      sri_pusch_pwr_ctrl_id        = 0;
  uint8_t                      sri_pusch_pathloss_ref_rs_id = 0;
  uint8_t                      sri_p0_pusch_alpha_set_id    = 0;
  sri_pusch_closed_loop_idx_e_ sri_pusch_closed_loop_idx;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SRS-PeriodicityAndOffset ::= CHOICE
struct srs_periodicity_and_offset_c {
  struct types_opts {
    enum options {
      sl1,
      sl2,
      sl4,
      sl5,
      sl8,
      sl10,
      sl16,
      sl20,
      sl32,
      sl40,
      sl64,
      sl80,
      sl160,
      sl320,
      sl640,
      sl1280,
      sl2560,
      nulltype
    } value;
    typedef uint16_t number_type;

    std::string to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<types_opts> types;

  // choice methods
  srs_periodicity_and_offset_c() = default;
  srs_periodicity_and_offset_c(const srs_periodicity_and_offset_c& other);
  srs_periodicity_and_offset_c& operator=(const srs_periodicity_and_offset_c& other);
  ~srs_periodicity_and_offset_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  uint8_t& sl2()
  {
    assert_choice_type("sl2", type_.to_string(), "SRS-PeriodicityAndOffset");
    return c.get<uint8_t>();
  }
  uint8_t& sl4()
  {
    assert_choice_type("sl4", type_.to_string(), "SRS-PeriodicityAndOffset");
    return c.get<uint8_t>();
  }
  uint8_t& sl5()
  {
    assert_choice_type("sl5", type_.to_string(), "SRS-PeriodicityAndOffset");
    return c.get<uint8_t>();
  }
  uint8_t& sl8()
  {
    assert_choice_type("sl8", type_.to_string(), "SRS-PeriodicityAndOffset");
    return c.get<uint8_t>();
  }
  uint8_t& sl10()
  {
    assert_choice_type("sl10", type_.to_string(), "SRS-PeriodicityAndOffset");
    return c.get<uint8_t>();
  }
  uint8_t& sl16()
  {
    assert_choice_type("sl16", type_.to_string(), "SRS-PeriodicityAndOffset");
    return c.get<uint8_t>();
  }
  uint8_t& sl20()
  {
    assert_choice_type("sl20", type_.to_string(), "SRS-PeriodicityAndOffset");
    return c.get<uint8_t>();
  }
  uint8_t& sl32()
  {
    assert_choice_type("sl32", type_.to_string(), "SRS-PeriodicityAndOffset");
    return c.get<uint8_t>();
  }
  uint8_t& sl40()
  {
    assert_choice_type("sl40", type_.to_string(), "SRS-PeriodicityAndOffset");
    return c.get<uint8_t>();
  }
  uint8_t& sl64()
  {
    assert_choice_type("sl64", type_.to_string(), "SRS-PeriodicityAndOffset");
    return c.get<uint8_t>();
  }
  uint8_t& sl80()
  {
    assert_choice_type("sl80", type_.to_string(), "SRS-PeriodicityAndOffset");
    return c.get<uint8_t>();
  }
  uint8_t& sl160()
  {
    assert_choice_type("sl160", type_.to_string(), "SRS-PeriodicityAndOffset");
    return c.get<uint8_t>();
  }
  uint16_t& sl320()
  {
    assert_choice_type("sl320", type_.to_string(), "SRS-PeriodicityAndOffset");
    return c.get<uint16_t>();
  }
  uint16_t& sl640()
  {
    assert_choice_type("sl640", type_.to_string(), "SRS-PeriodicityAndOffset");
    return c.get<uint16_t>();
  }
  uint16_t& sl1280()
  {
    assert_choice_type("sl1280", type_.to_string(), "SRS-PeriodicityAndOffset");
    return c.get<uint16_t>();
  }
  uint16_t& sl2560()
  {
    assert_choice_type("sl2560", type_.to_string(), "SRS-PeriodicityAndOffset");
    return c.get<uint16_t>();
  }
  const uint8_t& sl2() const
  {
    assert_choice_type("sl2", type_.to_string(), "SRS-PeriodicityAndOffset");
    return c.get<uint8_t>();
  }
  const uint8_t& sl4() const
  {
    assert_choice_type("sl4", type_.to_string(), "SRS-PeriodicityAndOffset");
    return c.get<uint8_t>();
  }
  const uint8_t& sl5() const
  {
    assert_choice_type("sl5", type_.to_string(), "SRS-PeriodicityAndOffset");
    return c.get<uint8_t>();
  }
  const uint8_t& sl8() const
  {
    assert_choice_type("sl8", type_.to_string(), "SRS-PeriodicityAndOffset");
    return c.get<uint8_t>();
  }
  const uint8_t& sl10() const
  {
    assert_choice_type("sl10", type_.to_string(), "SRS-PeriodicityAndOffset");
    return c.get<uint8_t>();
  }
  const uint8_t& sl16() const
  {
    assert_choice_type("sl16", type_.to_string(), "SRS-PeriodicityAndOffset");
    return c.get<uint8_t>();
  }
  const uint8_t& sl20() const
  {
    assert_choice_type("sl20", type_.to_string(), "SRS-PeriodicityAndOffset");
    return c.get<uint8_t>();
  }
  const uint8_t& sl32() const
  {
    assert_choice_type("sl32", type_.to_string(), "SRS-PeriodicityAndOffset");
    return c.get<uint8_t>();
  }
  const uint8_t& sl40() const
  {
    assert_choice_type("sl40", type_.to_string(), "SRS-PeriodicityAndOffset");
    return c.get<uint8_t>();
  }
  const uint8_t& sl64() const
  {
    assert_choice_type("sl64", type_.to_string(), "SRS-PeriodicityAndOffset");
    return c.get<uint8_t>();
  }
  const uint8_t& sl80() const
  {
    assert_choice_type("sl80", type_.to_string(), "SRS-PeriodicityAndOffset");
    return c.get<uint8_t>();
  }
  const uint8_t& sl160() const
  {
    assert_choice_type("sl160", type_.to_string(), "SRS-PeriodicityAndOffset");
    return c.get<uint8_t>();
  }
  const uint16_t& sl320() const
  {
    assert_choice_type("sl320", type_.to_string(), "SRS-PeriodicityAndOffset");
    return c.get<uint16_t>();
  }
  const uint16_t& sl640() const
  {
    assert_choice_type("sl640", type_.to_string(), "SRS-PeriodicityAndOffset");
    return c.get<uint16_t>();
  }
  const uint16_t& sl1280() const
  {
    assert_choice_type("sl1280", type_.to_string(), "SRS-PeriodicityAndOffset");
    return c.get<uint16_t>();
  }
  const uint16_t& sl2560() const
  {
    assert_choice_type("sl2560", type_.to_string(), "SRS-PeriodicityAndOffset");
    return c.get<uint16_t>();
  }
  uint8_t& set_sl2()
  {
    set(types::sl2);
    return c.get<uint8_t>();
  }
  uint8_t& set_sl4()
  {
    set(types::sl4);
    return c.get<uint8_t>();
  }
  uint8_t& set_sl5()
  {
    set(types::sl5);
    return c.get<uint8_t>();
  }
  uint8_t& set_sl8()
  {
    set(types::sl8);
    return c.get<uint8_t>();
  }
  uint8_t& set_sl10()
  {
    set(types::sl10);
    return c.get<uint8_t>();
  }
  uint8_t& set_sl16()
  {
    set(types::sl16);
    return c.get<uint8_t>();
  }
  uint8_t& set_sl20()
  {
    set(types::sl20);
    return c.get<uint8_t>();
  }
  uint8_t& set_sl32()
  {
    set(types::sl32);
    return c.get<uint8_t>();
  }
  uint8_t& set_sl40()
  {
    set(types::sl40);
    return c.get<uint8_t>();
  }
  uint8_t& set_sl64()
  {
    set(types::sl64);
    return c.get<uint8_t>();
  }
  uint8_t& set_sl80()
  {
    set(types::sl80);
    return c.get<uint8_t>();
  }
  uint8_t& set_sl160()
  {
    set(types::sl160);
    return c.get<uint8_t>();
  }
  uint16_t& set_sl320()
  {
    set(types::sl320);
    return c.get<uint16_t>();
  }
  uint16_t& set_sl640()
  {
    set(types::sl640);
    return c.get<uint16_t>();
  }
  uint16_t& set_sl1280()
  {
    set(types::sl1280);
    return c.get<uint16_t>();
  }
  uint16_t& set_sl2560()
  {
    set(types::sl2560);
    return c.get<uint16_t>();
  }

private:
  types               type_;
  pod_choice_buffer_t c;

  void destroy_();
};

// SRS-SpatialRelationInfo ::= SEQUENCE
struct srs_spatial_relation_info_s {
  struct ref_sig_c_ {
    struct srs_s_ {
      uint8_t res_id = 0;
      uint8_t ul_bwp = 0;
    };
    struct types_opts {
      enum options { ssb_idx, csi_rs_idx, srs, nulltype } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    ref_sig_c_() = default;
    ref_sig_c_(const ref_sig_c_& other);
    ref_sig_c_& operator=(const ref_sig_c_& other);
    ~ref_sig_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint8_t& ssb_idx()
    {
      assert_choice_type("ssb-Index", type_.to_string(), "referenceSignal");
      return c.get<uint8_t>();
    }
    uint8_t& csi_rs_idx()
    {
      assert_choice_type("csi-RS-Index", type_.to_string(), "referenceSignal");
      return c.get<uint8_t>();
    }
    srs_s_& srs()
    {
      assert_choice_type("srs", type_.to_string(), "referenceSignal");
      return c.get<srs_s_>();
    }
    const uint8_t& ssb_idx() const
    {
      assert_choice_type("ssb-Index", type_.to_string(), "referenceSignal");
      return c.get<uint8_t>();
    }
    const uint8_t& csi_rs_idx() const
    {
      assert_choice_type("csi-RS-Index", type_.to_string(), "referenceSignal");
      return c.get<uint8_t>();
    }
    const srs_s_& srs() const
    {
      assert_choice_type("srs", type_.to_string(), "referenceSignal");
      return c.get<srs_s_>();
    }
    uint8_t& set_ssb_idx()
    {
      set(types::ssb_idx);
      return c.get<uint8_t>();
    }
    uint8_t& set_csi_rs_idx()
    {
      set(types::csi_rs_idx);
      return c.get<uint8_t>();
    }
    srs_s_& set_srs()
    {
      set(types::srs);
      return c.get<srs_s_>();
    }

  private:
    types                   type_;
    choice_buffer_t<srs_s_> c;

    void destroy_();
  };

  // member variables
  bool       serving_cell_id_present = false;
  uint8_t    serving_cell_id         = 0;
  ref_sig_c_ ref_sig;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CG-UCI-OnPUSCH ::= CHOICE
struct cg_uci_on_pusch_c {
  using dynamic_l_ = dyn_array<beta_offsets_s>;
  struct types_opts {
    enum options { dynamic_type, semi_static, nulltype } value;

    std::string to_string() const;
  };
  typedef enumerated<types_opts> types;

  // choice methods
  cg_uci_on_pusch_c() = default;
  cg_uci_on_pusch_c(const cg_uci_on_pusch_c& other);
  cg_uci_on_pusch_c& operator=(const cg_uci_on_pusch_c& other);
  ~cg_uci_on_pusch_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  dynamic_l_& dynamic_type()
  {
    assert_choice_type("dynamic", type_.to_string(), "CG-UCI-OnPUSCH");
    return c.get<dynamic_l_>();
  }
  beta_offsets_s& semi_static()
  {
    assert_choice_type("semiStatic", type_.to_string(), "CG-UCI-OnPUSCH");
    return c.get<beta_offsets_s>();
  }
  const dynamic_l_& dynamic_type() const
  {
    assert_choice_type("dynamic", type_.to_string(), "CG-UCI-OnPUSCH");
    return c.get<dynamic_l_>();
  }
  const beta_offsets_s& semi_static() const
  {
    assert_choice_type("semiStatic", type_.to_string(), "CG-UCI-OnPUSCH");
    return c.get<beta_offsets_s>();
  }
  dynamic_l_& set_dynamic_type()
  {
    set(types::dynamic_type);
    return c.get<dynamic_l_>();
  }
  beta_offsets_s& set_semi_static()
  {
    set(types::semi_static);
    return c.get<beta_offsets_s>();
  }

private:
  types                                       type_;
  choice_buffer_t<beta_offsets_s, dynamic_l_> c;

  void destroy_();
};

// CSI-RS-ResourceMapping ::= SEQUENCE
struct csi_rs_res_map_s {
  struct freq_domain_alloc_c_ {
    struct types_opts {
      enum options { row1, row2, row4, other, nulltype } value;
      typedef uint8_t number_type;

      std::string to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    freq_domain_alloc_c_() = default;
    freq_domain_alloc_c_(const freq_domain_alloc_c_& other);
    freq_domain_alloc_c_& operator=(const freq_domain_alloc_c_& other);
    ~freq_domain_alloc_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    fixed_bitstring<4>& row1()
    {
      assert_choice_type("row1", type_.to_string(), "frequencyDomainAllocation");
      return c.get<fixed_bitstring<4> >();
    }
    fixed_bitstring<12>& row2()
    {
      assert_choice_type("row2", type_.to_string(), "frequencyDomainAllocation");
      return c.get<fixed_bitstring<12> >();
    }
    fixed_bitstring<3>& row4()
    {
      assert_choice_type("row4", type_.to_string(), "frequencyDomainAllocation");
      return c.get<fixed_bitstring<3> >();
    }
    fixed_bitstring<6>& other()
    {
      assert_choice_type("other", type_.to_string(), "frequencyDomainAllocation");
      return c.get<fixed_bitstring<6> >();
    }
    const fixed_bitstring<4>& row1() const
    {
      assert_choice_type("row1", type_.to_string(), "frequencyDomainAllocation");
      return c.get<fixed_bitstring<4> >();
    }
    const fixed_bitstring<12>& row2() const
    {
      assert_choice_type("row2", type_.to_string(), "frequencyDomainAllocation");
      return c.get<fixed_bitstring<12> >();
    }
    const fixed_bitstring<3>& row4() const
    {
      assert_choice_type("row4", type_.to_string(), "frequencyDomainAllocation");
      return c.get<fixed_bitstring<3> >();
    }
    const fixed_bitstring<6>& other() const
    {
      assert_choice_type("other", type_.to_string(), "frequencyDomainAllocation");
      return c.get<fixed_bitstring<6> >();
    }
    fixed_bitstring<4>& set_row1()
    {
      set(types::row1);
      return c.get<fixed_bitstring<4> >();
    }
    fixed_bitstring<12>& set_row2()
    {
      set(types::row2);
      return c.get<fixed_bitstring<12> >();
    }
    fixed_bitstring<3>& set_row4()
    {
      set(types::row4);
      return c.get<fixed_bitstring<3> >();
    }
    fixed_bitstring<6>& set_other()
    {
      set(types::other);
      return c.get<fixed_bitstring<6> >();
    }

  private:
    types                                 type_;
    choice_buffer_t<fixed_bitstring<12> > c;

    void destroy_();
  };
  struct nrof_ports_opts {
    enum options { p1, p2, p4, p8, p12, p16, p24, p32, nulltype } value;
    typedef uint8_t number_type;

    std::string to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<nrof_ports_opts> nrof_ports_e_;
  struct cdm_type_opts {
    enum options { no_cdm, fd_cdm2, cdm4_fd2_td2, cdm8_fd2_td4, nulltype } value;
    typedef uint8_t number_type;

    std::string to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<cdm_type_opts> cdm_type_e_;
  struct density_c_ {
    struct dot5_opts {
      enum options { even_prbs, odd_prbs, nulltype } value;

      std::string to_string() const;
    };
    typedef enumerated<dot5_opts> dot5_e_;
    struct types_opts {
      enum options { dot5, one, three, spare, nulltype } value;
      typedef uint8_t number_type;

      std::string to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    density_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    dot5_e_& dot5()
    {
      assert_choice_type("dot5", type_.to_string(), "density");
      return c;
    }
    const dot5_e_& dot5() const
    {
      assert_choice_type("dot5", type_.to_string(), "density");
      return c;
    }
    dot5_e_& set_dot5()
    {
      set(types::dot5);
      return c;
    }

  private:
    types   type_;
    dot5_e_ c;
  };

  // member variables
  bool                  ext                                       = false;
  bool                  first_ofdm_symbol_in_time_domain2_present = false;
  freq_domain_alloc_c_  freq_domain_alloc;
  nrof_ports_e_         nrof_ports;
  uint8_t               first_ofdm_symbol_in_time_domain  = 0;
  uint8_t               first_ofdm_symbol_in_time_domain2 = 2;
  cdm_type_e_           cdm_type;
  density_c_            density;
  csi_freq_occupation_s freq_band;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CSI-ResourcePeriodicityAndOffset ::= CHOICE
struct csi_res_periodicity_and_offset_c {
  struct types_opts {
    enum options {
      slots4,
      slots5,
      slots8,
      slots10,
      slots16,
      slots20,
      slots32,
      slots40,
      slots64,
      slots80,
      slots160,
      slots320,
      slots640,
      nulltype
    } value;
    typedef uint16_t number_type;

    std::string to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<types_opts> types;

  // choice methods
  csi_res_periodicity_and_offset_c() = default;
  csi_res_periodicity_and_offset_c(const csi_res_periodicity_and_offset_c& other);
  csi_res_periodicity_and_offset_c& operator=(const csi_res_periodicity_and_offset_c& other);
  ~csi_res_periodicity_and_offset_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  uint8_t& slots4()
  {
    assert_choice_type("slots4", type_.to_string(), "CSI-ResourcePeriodicityAndOffset");
    return c.get<uint8_t>();
  }
  uint8_t& slots5()
  {
    assert_choice_type("slots5", type_.to_string(), "CSI-ResourcePeriodicityAndOffset");
    return c.get<uint8_t>();
  }
  uint8_t& slots8()
  {
    assert_choice_type("slots8", type_.to_string(), "CSI-ResourcePeriodicityAndOffset");
    return c.get<uint8_t>();
  }
  uint8_t& slots10()
  {
    assert_choice_type("slots10", type_.to_string(), "CSI-ResourcePeriodicityAndOffset");
    return c.get<uint8_t>();
  }
  uint8_t& slots16()
  {
    assert_choice_type("slots16", type_.to_string(), "CSI-ResourcePeriodicityAndOffset");
    return c.get<uint8_t>();
  }
  uint8_t& slots20()
  {
    assert_choice_type("slots20", type_.to_string(), "CSI-ResourcePeriodicityAndOffset");
    return c.get<uint8_t>();
  }
  uint8_t& slots32()
  {
    assert_choice_type("slots32", type_.to_string(), "CSI-ResourcePeriodicityAndOffset");
    return c.get<uint8_t>();
  }
  uint8_t& slots40()
  {
    assert_choice_type("slots40", type_.to_string(), "CSI-ResourcePeriodicityAndOffset");
    return c.get<uint8_t>();
  }
  uint8_t& slots64()
  {
    assert_choice_type("slots64", type_.to_string(), "CSI-ResourcePeriodicityAndOffset");
    return c.get<uint8_t>();
  }
  uint8_t& slots80()
  {
    assert_choice_type("slots80", type_.to_string(), "CSI-ResourcePeriodicityAndOffset");
    return c.get<uint8_t>();
  }
  uint8_t& slots160()
  {
    assert_choice_type("slots160", type_.to_string(), "CSI-ResourcePeriodicityAndOffset");
    return c.get<uint8_t>();
  }
  uint16_t& slots320()
  {
    assert_choice_type("slots320", type_.to_string(), "CSI-ResourcePeriodicityAndOffset");
    return c.get<uint16_t>();
  }
  uint16_t& slots640()
  {
    assert_choice_type("slots640", type_.to_string(), "CSI-ResourcePeriodicityAndOffset");
    return c.get<uint16_t>();
  }
  const uint8_t& slots4() const
  {
    assert_choice_type("slots4", type_.to_string(), "CSI-ResourcePeriodicityAndOffset");
    return c.get<uint8_t>();
  }
  const uint8_t& slots5() const
  {
    assert_choice_type("slots5", type_.to_string(), "CSI-ResourcePeriodicityAndOffset");
    return c.get<uint8_t>();
  }
  const uint8_t& slots8() const
  {
    assert_choice_type("slots8", type_.to_string(), "CSI-ResourcePeriodicityAndOffset");
    return c.get<uint8_t>();
  }
  const uint8_t& slots10() const
  {
    assert_choice_type("slots10", type_.to_string(), "CSI-ResourcePeriodicityAndOffset");
    return c.get<uint8_t>();
  }
  const uint8_t& slots16() const
  {
    assert_choice_type("slots16", type_.to_string(), "CSI-ResourcePeriodicityAndOffset");
    return c.get<uint8_t>();
  }
  const uint8_t& slots20() const
  {
    assert_choice_type("slots20", type_.to_string(), "CSI-ResourcePeriodicityAndOffset");
    return c.get<uint8_t>();
  }
  const uint8_t& slots32() const
  {
    assert_choice_type("slots32", type_.to_string(), "CSI-ResourcePeriodicityAndOffset");
    return c.get<uint8_t>();
  }
  const uint8_t& slots40() const
  {
    assert_choice_type("slots40", type_.to_string(), "CSI-ResourcePeriodicityAndOffset");
    return c.get<uint8_t>();
  }
  const uint8_t& slots64() const
  {
    assert_choice_type("slots64", type_.to_string(), "CSI-ResourcePeriodicityAndOffset");
    return c.get<uint8_t>();
  }
  const uint8_t& slots80() const
  {
    assert_choice_type("slots80", type_.to_string(), "CSI-ResourcePeriodicityAndOffset");
    return c.get<uint8_t>();
  }
  const uint8_t& slots160() const
  {
    assert_choice_type("slots160", type_.to_string(), "CSI-ResourcePeriodicityAndOffset");
    return c.get<uint8_t>();
  }
  const uint16_t& slots320() const
  {
    assert_choice_type("slots320", type_.to_string(), "CSI-ResourcePeriodicityAndOffset");
    return c.get<uint16_t>();
  }
  const uint16_t& slots640() const
  {
    assert_choice_type("slots640", type_.to_string(), "CSI-ResourcePeriodicityAndOffset");
    return c.get<uint16_t>();
  }
  uint8_t& set_slots4()
  {
    set(types::slots4);
    return c.get<uint8_t>();
  }
  uint8_t& set_slots5()
  {
    set(types::slots5);
    return c.get<uint8_t>();
  }
  uint8_t& set_slots8()
  {
    set(types::slots8);
    return c.get<uint8_t>();
  }
  uint8_t& set_slots10()
  {
    set(types::slots10);
    return c.get<uint8_t>();
  }
  uint8_t& set_slots16()
  {
    set(types::slots16);
    return c.get<uint8_t>();
  }
  uint8_t& set_slots20()
  {
    set(types::slots20);
    return c.get<uint8_t>();
  }
  uint8_t& set_slots32()
  {
    set(types::slots32);
    return c.get<uint8_t>();
  }
  uint8_t& set_slots40()
  {
    set(types::slots40);
    return c.get<uint8_t>();
  }
  uint8_t& set_slots64()
  {
    set(types::slots64);
    return c.get<uint8_t>();
  }
  uint8_t& set_slots80()
  {
    set(types::slots80);
    return c.get<uint8_t>();
  }
  uint8_t& set_slots160()
  {
    set(types::slots160);
    return c.get<uint8_t>();
  }
  uint16_t& set_slots320()
  {
    set(types::slots320);
    return c.get<uint16_t>();
  }
  uint16_t& set_slots640()
  {
    set(types::slots640);
    return c.get<uint16_t>();
  }

private:
  types               type_;
  pod_choice_buffer_t c;

  void destroy_();
};

// DMRS-UplinkConfig ::= SEQUENCE
struct dmrs_ul_cfg_s {
  struct dmrs_add_position_opts {
    enum options { pos0, pos1, pos3, nulltype } value;
    typedef uint8_t number_type;

    std::string to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<dmrs_add_position_opts> dmrs_add_position_e_;
  struct transform_precoding_disabled_s_ {
    bool     ext                    = false;
    bool     scrambling_id0_present = false;
    bool     scrambling_id1_present = false;
    uint32_t scrambling_id0         = 0;
    uint32_t scrambling_id1         = 0;
    // ...
  };
  struct transform_precoding_enabled_s_ {
    bool     ext                   = false;
    bool     npusch_id_present     = false;
    bool     seq_group_hop_present = false;
    bool     seq_hop_present       = false;
    uint16_t npusch_id             = 0;
    // ...
  };

  // member variables
  bool                            ext                                  = false;
  bool                            dmrs_type_present                    = false;
  bool                            dmrs_add_position_present            = false;
  bool                            phase_tracking_rs_present            = false;
  bool                            max_len_present                      = false;
  bool                            transform_precoding_disabled_present = false;
  bool                            transform_precoding_enabled_present  = false;
  dmrs_add_position_e_            dmrs_add_position;
  setup_release_c<ptrs_ul_cfg_s>  phase_tracking_rs;
  transform_precoding_disabled_s_ transform_precoding_disabled;
  transform_precoding_enabled_s_  transform_precoding_enabled;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// INT-ConfigurationPerServingCell ::= SEQUENCE
struct int_cfg_per_serving_cell_s {
  uint8_t serving_cell_id = 0;
  uint8_t position_in_dci = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PRACH-ResourceDedicatedBFR ::= CHOICE
struct prach_res_ded_bfr_c {
  struct types_opts {
    enum options { ssb, csi_rs, nulltype } value;

    std::string to_string() const;
  };
  typedef enumerated<types_opts> types;

  // choice methods
  prach_res_ded_bfr_c() = default;
  prach_res_ded_bfr_c(const prach_res_ded_bfr_c& other);
  prach_res_ded_bfr_c& operator=(const prach_res_ded_bfr_c& other);
  ~prach_res_ded_bfr_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  bfr_ssb_res_s& ssb()
  {
    assert_choice_type("ssb", type_.to_string(), "PRACH-ResourceDedicatedBFR");
    return c.get<bfr_ssb_res_s>();
  }
  bfr_csirs_res_s& csi_rs()
  {
    assert_choice_type("csi-RS", type_.to_string(), "PRACH-ResourceDedicatedBFR");
    return c.get<bfr_csirs_res_s>();
  }
  const bfr_ssb_res_s& ssb() const
  {
    assert_choice_type("ssb", type_.to_string(), "PRACH-ResourceDedicatedBFR");
    return c.get<bfr_ssb_res_s>();
  }
  const bfr_csirs_res_s& csi_rs() const
  {
    assert_choice_type("csi-RS", type_.to_string(), "PRACH-ResourceDedicatedBFR");
    return c.get<bfr_csirs_res_s>();
  }
  bfr_ssb_res_s& set_ssb()
  {
    set(types::ssb);
    return c.get<bfr_ssb_res_s>();
  }
  bfr_csirs_res_s& set_csi_rs()
  {
    set(types::csi_rs);
    return c.get<bfr_csirs_res_s>();
  }

private:
  types                                           type_;
  choice_buffer_t<bfr_csirs_res_s, bfr_ssb_res_s> c;

  void destroy_();
};

// PTRS-DownlinkConfig ::= SEQUENCE
struct ptrs_dl_cfg_s {
  using freq_density_l_ = std::array<uint16_t, 2>;
  using time_density_l_ = std::array<uint8_t, 3>;
  struct res_elem_offset_opts {
    enum options { offset01, offset10, offset11, nulltype } value;
    typedef float number_type;

    std::string to_string() const;
    float       to_number() const;
    std::string to_number_string() const;
  };
  typedef enumerated<res_elem_offset_opts> res_elem_offset_e_;

  // member variables
  bool               ext                     = false;
  bool               freq_density_present    = false;
  bool               time_density_present    = false;
  bool               epre_ratio_present      = false;
  bool               res_elem_offset_present = false;
  freq_density_l_    freq_density;
  time_density_l_    time_density;
  uint8_t            epre_ratio = 0;
  res_elem_offset_e_ res_elem_offset;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PUCCH-FormatConfig ::= SEQUENCE
struct pucch_format_cfg_s {
  struct nrof_slots_opts {
    enum options { n2, n4, n8, nulltype } value;
    typedef uint8_t number_type;

    std::string to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<nrof_slots_opts> nrof_slots_e_;

  // member variables
  bool                  interslot_freq_hop_present = false;
  bool                  add_dmrs_present           = false;
  bool                  max_code_rate_present      = false;
  bool                  nrof_slots_present         = false;
  bool                  pi2_bpsk_present           = false;
  bool                  simul_harq_ack_csi_present = false;
  pucch_max_code_rate_e max_code_rate;
  nrof_slots_e_         nrof_slots;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PUCCH-PowerControl ::= SEQUENCE
struct pucch_pwr_ctrl_s {
  using p0_set_l_           = dyn_array<p0_pucch_s>;
  using pathloss_ref_rss_l_ = dyn_array<pucch_pathloss_ref_rs_s>;

  // member variables
  bool                ext                                    = false;
  bool                delta_f_pucch_f0_present               = false;
  bool                delta_f_pucch_f1_present               = false;
  bool                delta_f_pucch_f2_present               = false;
  bool                delta_f_pucch_f3_present               = false;
  bool                delta_f_pucch_f4_present               = false;
  bool                p0_set_present                         = false;
  bool                pathloss_ref_rss_present               = false;
  bool                two_pucch_pc_adjustment_states_present = false;
  int8_t              delta_f_pucch_f0                       = -16;
  int8_t              delta_f_pucch_f1                       = -16;
  int8_t              delta_f_pucch_f2                       = -16;
  int8_t              delta_f_pucch_f3                       = -16;
  int8_t              delta_f_pucch_f4                       = -16;
  p0_set_l_           p0_set;
  pathloss_ref_rss_l_ pathloss_ref_rss;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PUCCH-Resource ::= SEQUENCE
struct pucch_res_s {
  struct format_c_ {
    struct types_opts {
      enum options { format0, format1, format2, format3, format4, nulltype } value;
      typedef uint8_t number_type;

      std::string to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    format_c_() = default;
    format_c_(const format_c_& other);
    format_c_& operator=(const format_c_& other);
    ~format_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    pucch_format0_s& format0()
    {
      assert_choice_type("format0", type_.to_string(), "format");
      return c.get<pucch_format0_s>();
    }
    pucch_format1_s& format1()
    {
      assert_choice_type("format1", type_.to_string(), "format");
      return c.get<pucch_format1_s>();
    }
    pucch_format2_s& format2()
    {
      assert_choice_type("format2", type_.to_string(), "format");
      return c.get<pucch_format2_s>();
    }
    pucch_format3_s& format3()
    {
      assert_choice_type("format3", type_.to_string(), "format");
      return c.get<pucch_format3_s>();
    }
    pucch_format4_s& format4()
    {
      assert_choice_type("format4", type_.to_string(), "format");
      return c.get<pucch_format4_s>();
    }
    const pucch_format0_s& format0() const
    {
      assert_choice_type("format0", type_.to_string(), "format");
      return c.get<pucch_format0_s>();
    }
    const pucch_format1_s& format1() const
    {
      assert_choice_type("format1", type_.to_string(), "format");
      return c.get<pucch_format1_s>();
    }
    const pucch_format2_s& format2() const
    {
      assert_choice_type("format2", type_.to_string(), "format");
      return c.get<pucch_format2_s>();
    }
    const pucch_format3_s& format3() const
    {
      assert_choice_type("format3", type_.to_string(), "format");
      return c.get<pucch_format3_s>();
    }
    const pucch_format4_s& format4() const
    {
      assert_choice_type("format4", type_.to_string(), "format");
      return c.get<pucch_format4_s>();
    }
    pucch_format0_s& set_format0()
    {
      set(types::format0);
      return c.get<pucch_format0_s>();
    }
    pucch_format1_s& set_format1()
    {
      set(types::format1);
      return c.get<pucch_format1_s>();
    }
    pucch_format2_s& set_format2()
    {
      set(types::format2);
      return c.get<pucch_format2_s>();
    }
    pucch_format3_s& set_format3()
    {
      set(types::format3);
      return c.get<pucch_format3_s>();
    }
    pucch_format4_s& set_format4()
    {
      set(types::format4);
      return c.get<pucch_format4_s>();
    }

  private:
    types                                                                                                type_;
    choice_buffer_t<pucch_format0_s, pucch_format1_s, pucch_format2_s, pucch_format3_s, pucch_format4_s> c;

    void destroy_();
  };

  // member variables
  bool      intra_slot_freq_hop_present = false;
  bool      second_hop_prb_present      = false;
  uint8_t   pucch_res_id                = 0;
  uint16_t  start_prb                   = 0;
  uint16_t  second_hop_prb              = 0;
  format_c_ format;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PUCCH-ResourceSet ::= SEQUENCE
struct pucch_res_set_s {
  using res_list_l_ = bounded_array<uint8_t, 32>;

  // member variables
  bool        max_payload_minus1_present = false;
  uint8_t     pucch_res_set_id           = 0;
  res_list_l_ res_list;
  uint16_t    max_payload_minus1 = 4;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PUCCH-SpatialRelationInfo ::= SEQUENCE
struct pucch_spatial_relation_info_s {
  struct ref_sig_c_ {
    struct srs_s_ {
      uint8_t res    = 0;
      uint8_t ul_bwp = 0;
    };
    struct types_opts {
      enum options { ssb_idx, csi_rs_idx, srs, nulltype } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    ref_sig_c_() = default;
    ref_sig_c_(const ref_sig_c_& other);
    ref_sig_c_& operator=(const ref_sig_c_& other);
    ~ref_sig_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint8_t& ssb_idx()
    {
      assert_choice_type("ssb-Index", type_.to_string(), "referenceSignal");
      return c.get<uint8_t>();
    }
    uint8_t& csi_rs_idx()
    {
      assert_choice_type("csi-RS-Index", type_.to_string(), "referenceSignal");
      return c.get<uint8_t>();
    }
    srs_s_& srs()
    {
      assert_choice_type("srs", type_.to_string(), "referenceSignal");
      return c.get<srs_s_>();
    }
    const uint8_t& ssb_idx() const
    {
      assert_choice_type("ssb-Index", type_.to_string(), "referenceSignal");
      return c.get<uint8_t>();
    }
    const uint8_t& csi_rs_idx() const
    {
      assert_choice_type("csi-RS-Index", type_.to_string(), "referenceSignal");
      return c.get<uint8_t>();
    }
    const srs_s_& srs() const
    {
      assert_choice_type("srs", type_.to_string(), "referenceSignal");
      return c.get<srs_s_>();
    }
    uint8_t& set_ssb_idx()
    {
      set(types::ssb_idx);
      return c.get<uint8_t>();
    }
    uint8_t& set_csi_rs_idx()
    {
      set(types::csi_rs_idx);
      return c.get<uint8_t>();
    }
    srs_s_& set_srs()
    {
      set(types::srs);
      return c.get<srs_s_>();
    }

  private:
    types                   type_;
    choice_buffer_t<srs_s_> c;

    void destroy_();
  };
  struct closed_loop_idx_opts {
    enum options { i0, i1, nulltype } value;
    typedef uint8_t number_type;

    std::string to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<closed_loop_idx_opts> closed_loop_idx_e_;

  // member variables
  bool               serving_cell_id_present        = false;
  uint8_t            pucch_spatial_relation_info_id = 1;
  uint8_t            serving_cell_id                = 0;
  ref_sig_c_         ref_sig;
  uint8_t            pucch_pathloss_ref_rs_id = 0;
  uint8_t            p0_pucch_id              = 1;
  closed_loop_idx_e_ closed_loop_idx;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PUSCH-PowerControl ::= SEQUENCE
struct pusch_pwr_ctrl_s {
  using p0_alpha_sets_l_                   = dyn_array<p0_pusch_alpha_set_s>;
  using pathloss_ref_rs_to_add_mod_list_l_ = dyn_array<pusch_pathloss_ref_rs_s>;
  using pathloss_ref_rs_to_release_list_l_ = bounded_array<uint8_t, 4>;
  using sri_pusch_map_to_add_mod_list_l_   = dyn_array<sri_pusch_pwr_ctrl_s>;
  using sri_pusch_map_to_release_list_l_   = bounded_array<uint8_t, 16>;

  // member variables
  bool                               tpc_accumulation_present                = false;
  bool                               msg3_alpha_present                      = false;
  bool                               p0_nominal_without_grant_present        = false;
  bool                               p0_alpha_sets_present                   = false;
  bool                               pathloss_ref_rs_to_add_mod_list_present = false;
  bool                               pathloss_ref_rs_to_release_list_present = false;
  bool                               two_pusch_pc_adjustment_states_present  = false;
  bool                               delta_mcs_present                       = false;
  bool                               sri_pusch_map_to_add_mod_list_present   = false;
  bool                               sri_pusch_map_to_release_list_present   = false;
  alpha_e                            msg3_alpha;
  int16_t                            p0_nominal_without_grant = -202;
  p0_alpha_sets_l_                   p0_alpha_sets;
  pathloss_ref_rs_to_add_mod_list_l_ pathloss_ref_rs_to_add_mod_list;
  pathloss_ref_rs_to_release_list_l_ pathloss_ref_rs_to_release_list;
  sri_pusch_map_to_add_mod_list_l_   sri_pusch_map_to_add_mod_list;
  sri_pusch_map_to_release_list_l_   sri_pusch_map_to_release_list;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// QCL-Info ::= SEQUENCE
struct qcl_info_s {
  struct ref_sig_c_ {
    struct types_opts {
      enum options { csi_rs, ssb, nulltype } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    ref_sig_c_() = default;
    ref_sig_c_(const ref_sig_c_& other);
    ref_sig_c_& operator=(const ref_sig_c_& other);
    ~ref_sig_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint8_t& csi_rs()
    {
      assert_choice_type("csi-rs", type_.to_string(), "referenceSignal");
      return c.get<uint8_t>();
    }
    uint8_t& ssb()
    {
      assert_choice_type("ssb", type_.to_string(), "referenceSignal");
      return c.get<uint8_t>();
    }
    const uint8_t& csi_rs() const
    {
      assert_choice_type("csi-rs", type_.to_string(), "referenceSignal");
      return c.get<uint8_t>();
    }
    const uint8_t& ssb() const
    {
      assert_choice_type("ssb", type_.to_string(), "referenceSignal");
      return c.get<uint8_t>();
    }
    uint8_t& set_csi_rs()
    {
      set(types::csi_rs);
      return c.get<uint8_t>();
    }
    uint8_t& set_ssb()
    {
      set(types::ssb);
      return c.get<uint8_t>();
    }

  private:
    types               type_;
    pod_choice_buffer_t c;

    void destroy_();
  };
  struct qcl_type_opts {
    enum options { type_a, type_b, type_c, type_d, nulltype } value;

    std::string to_string() const;
  };
  typedef enumerated<qcl_type_opts> qcl_type_e_;

  // member variables
  bool        ext            = false;
  bool        cell_present   = false;
  bool        bwp_id_present = false;
  uint8_t     cell           = 0;
  uint8_t     bwp_id         = 0;
  ref_sig_c_  ref_sig;
  qcl_type_e_ qcl_type;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RA-Prioritization ::= SEQUENCE
struct ra_prioritization_s {
  struct pwr_ramp_step_high_prio_opts {
    enum options { db0, db2, db4, db6, nulltype } value;
    typedef uint8_t number_type;

    std::string to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<pwr_ramp_step_high_prio_opts> pwr_ramp_step_high_prio_e_;
  struct scaling_factor_bi_opts {
    enum options { zero, dot25, dot5, dot75, nulltype } value;
    typedef uint8_t number_type;

    std::string to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<scaling_factor_bi_opts> scaling_factor_bi_e_;

  // member variables
  bool                       ext                       = false;
  bool                       scaling_factor_bi_present = false;
  pwr_ramp_step_high_prio_e_ pwr_ramp_step_high_prio;
  scaling_factor_bi_e_       scaling_factor_bi;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SRS-Resource ::= SEQUENCE
struct srs_res_s {
  struct nrof_srs_ports_opts {
    enum options { port1, ports2, ports4, nulltype } value;
    typedef uint8_t number_type;

    std::string to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<nrof_srs_ports_opts> nrof_srs_ports_e_;
  struct ptrs_port_idx_opts {
    enum options { n0, n1, nulltype } value;
    typedef uint8_t number_type;

    std::string to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<ptrs_port_idx_opts> ptrs_port_idx_e_;
  struct tx_comb_c_ {
    struct n2_s_ {
      uint8_t comb_offset_n2  = 0;
      uint8_t cyclic_shift_n2 = 0;
    };
    struct n4_s_ {
      uint8_t comb_offset_n4  = 0;
      uint8_t cyclic_shift_n4 = 0;
    };
    struct types_opts {
      enum options { n2, n4, nulltype } value;
      typedef uint8_t number_type;

      std::string to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    tx_comb_c_() = default;
    tx_comb_c_(const tx_comb_c_& other);
    tx_comb_c_& operator=(const tx_comb_c_& other);
    ~tx_comb_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    n2_s_& n2()
    {
      assert_choice_type("n2", type_.to_string(), "transmissionComb");
      return c.get<n2_s_>();
    }
    n4_s_& n4()
    {
      assert_choice_type("n4", type_.to_string(), "transmissionComb");
      return c.get<n4_s_>();
    }
    const n2_s_& n2() const
    {
      assert_choice_type("n2", type_.to_string(), "transmissionComb");
      return c.get<n2_s_>();
    }
    const n4_s_& n4() const
    {
      assert_choice_type("n4", type_.to_string(), "transmissionComb");
      return c.get<n4_s_>();
    }
    n2_s_& set_n2()
    {
      set(types::n2);
      return c.get<n2_s_>();
    }
    n4_s_& set_n4()
    {
      set(types::n4);
      return c.get<n4_s_>();
    }

  private:
    types                         type_;
    choice_buffer_t<n2_s_, n4_s_> c;

    void destroy_();
  };
  struct res_map_s_ {
    struct nrof_symbols_opts {
      enum options { n1, n2, n4, nulltype } value;
      typedef uint8_t number_type;

      std::string to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<nrof_symbols_opts> nrof_symbols_e_;
    struct repeat_factor_opts {
      enum options { n1, n2, n4, nulltype } value;
      typedef uint8_t number_type;

      std::string to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<repeat_factor_opts> repeat_factor_e_;

    // member variables
    uint8_t          start_position = 0;
    nrof_symbols_e_  nrof_symbols;
    repeat_factor_e_ repeat_factor;
  };
  struct freq_hop_s_ {
    uint8_t c_srs = 0;
    uint8_t b_srs = 0;
    uint8_t b_hop = 0;
  };
  struct group_or_seq_hop_opts {
    enum options { neither, group_hop, seq_hop, nulltype } value;

    std::string to_string() const;
  };
  typedef enumerated<group_or_seq_hop_opts> group_or_seq_hop_e_;
  struct res_type_c_ {
    struct aperiodic_s_ {
      bool ext = false;
      // ...
    };
    struct semi_persistent_s_ {
      bool                         ext = false;
      srs_periodicity_and_offset_c periodicity_and_offset_sp;
      // ...
    };
    struct periodic_s_ {
      bool                         ext = false;
      srs_periodicity_and_offset_c periodicity_and_offset_p;
      // ...
    };
    struct types_opts {
      enum options { aperiodic, semi_persistent, periodic, nulltype } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    res_type_c_() = default;
    res_type_c_(const res_type_c_& other);
    res_type_c_& operator=(const res_type_c_& other);
    ~res_type_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    aperiodic_s_& aperiodic()
    {
      assert_choice_type("aperiodic", type_.to_string(), "resourceType");
      return c.get<aperiodic_s_>();
    }
    semi_persistent_s_& semi_persistent()
    {
      assert_choice_type("semi-persistent", type_.to_string(), "resourceType");
      return c.get<semi_persistent_s_>();
    }
    periodic_s_& periodic()
    {
      assert_choice_type("periodic", type_.to_string(), "resourceType");
      return c.get<periodic_s_>();
    }
    const aperiodic_s_& aperiodic() const
    {
      assert_choice_type("aperiodic", type_.to_string(), "resourceType");
      return c.get<aperiodic_s_>();
    }
    const semi_persistent_s_& semi_persistent() const
    {
      assert_choice_type("semi-persistent", type_.to_string(), "resourceType");
      return c.get<semi_persistent_s_>();
    }
    const periodic_s_& periodic() const
    {
      assert_choice_type("periodic", type_.to_string(), "resourceType");
      return c.get<periodic_s_>();
    }
    aperiodic_s_& set_aperiodic()
    {
      set(types::aperiodic);
      return c.get<aperiodic_s_>();
    }
    semi_persistent_s_& set_semi_persistent()
    {
      set(types::semi_persistent);
      return c.get<semi_persistent_s_>();
    }
    periodic_s_& set_periodic()
    {
      set(types::periodic);
      return c.get<periodic_s_>();
    }

  private:
    types                                                          type_;
    choice_buffer_t<aperiodic_s_, periodic_s_, semi_persistent_s_> c;

    void destroy_();
  };

  // member variables
  bool                        ext                           = false;
  bool                        ptrs_port_idx_present         = false;
  bool                        spatial_relation_info_present = false;
  uint8_t                     srs_res_id                    = 0;
  nrof_srs_ports_e_           nrof_srs_ports;
  ptrs_port_idx_e_            ptrs_port_idx;
  tx_comb_c_                  tx_comb;
  res_map_s_                  res_map;
  uint8_t                     freq_domain_position = 0;
  uint16_t                    freq_domain_shift    = 0;
  freq_hop_s_                 freq_hop;
  group_or_seq_hop_e_         group_or_seq_hop;
  res_type_c_                 res_type;
  uint16_t                    seq_id = 0;
  srs_spatial_relation_info_s spatial_relation_info;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SRS-ResourceSet ::= SEQUENCE
struct srs_res_set_s {
  using srs_res_id_list_l_ = bounded_array<uint8_t, 16>;
  struct res_type_c_ {
    struct aperiodic_s_ {
      using aperiodic_srs_res_trigger_list_v1530_l_ = bounded_array<uint8_t, 2>;

      // member variables
      bool    ext                       = false;
      bool    csi_rs_present            = false;
      bool    slot_offset_present       = false;
      uint8_t aperiodic_srs_res_trigger = 1;
      uint8_t csi_rs                    = 0;
      uint8_t slot_offset               = 1;
      // ...
      // group 0
      copy_ptr<aperiodic_srs_res_trigger_list_v1530_l_> aperiodic_srs_res_trigger_list_v1530;

      // sequence methods
      SRSASN_CODE pack(bit_ref& bref) const;
      SRSASN_CODE unpack(cbit_ref& bref);
      void        to_json(json_writer& j) const;
    };
    struct semi_persistent_s_ {
      bool    ext                       = false;
      bool    associated_csi_rs_present = false;
      uint8_t associated_csi_rs         = 0;
      // ...
    };
    struct periodic_s_ {
      bool    ext                       = false;
      bool    associated_csi_rs_present = false;
      uint8_t associated_csi_rs         = 0;
      // ...
    };
    struct types_opts {
      enum options { aperiodic, semi_persistent, periodic, nulltype } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    res_type_c_() = default;
    res_type_c_(const res_type_c_& other);
    res_type_c_& operator=(const res_type_c_& other);
    ~res_type_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    aperiodic_s_& aperiodic()
    {
      assert_choice_type("aperiodic", type_.to_string(), "resourceType");
      return c.get<aperiodic_s_>();
    }
    semi_persistent_s_& semi_persistent()
    {
      assert_choice_type("semi-persistent", type_.to_string(), "resourceType");
      return c.get<semi_persistent_s_>();
    }
    periodic_s_& periodic()
    {
      assert_choice_type("periodic", type_.to_string(), "resourceType");
      return c.get<periodic_s_>();
    }
    const aperiodic_s_& aperiodic() const
    {
      assert_choice_type("aperiodic", type_.to_string(), "resourceType");
      return c.get<aperiodic_s_>();
    }
    const semi_persistent_s_& semi_persistent() const
    {
      assert_choice_type("semi-persistent", type_.to_string(), "resourceType");
      return c.get<semi_persistent_s_>();
    }
    const periodic_s_& periodic() const
    {
      assert_choice_type("periodic", type_.to_string(), "resourceType");
      return c.get<periodic_s_>();
    }
    aperiodic_s_& set_aperiodic()
    {
      set(types::aperiodic);
      return c.get<aperiodic_s_>();
    }
    semi_persistent_s_& set_semi_persistent()
    {
      set(types::semi_persistent);
      return c.get<semi_persistent_s_>();
    }
    periodic_s_& set_periodic()
    {
      set(types::periodic);
      return c.get<periodic_s_>();
    }

  private:
    types                                                          type_;
    choice_buffer_t<aperiodic_s_, periodic_s_, semi_persistent_s_> c;

    void destroy_();
  };
  struct usage_opts {
    enum options { beam_management, codebook, non_codebook, ant_switching, nulltype } value;

    std::string to_string() const;
  };
  typedef enumerated<usage_opts> usage_e_;
  struct pathloss_ref_rs_c_ {
    struct types_opts {
      enum options { ssb_idx, csi_rs_idx, nulltype } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    pathloss_ref_rs_c_() = default;
    pathloss_ref_rs_c_(const pathloss_ref_rs_c_& other);
    pathloss_ref_rs_c_& operator=(const pathloss_ref_rs_c_& other);
    ~pathloss_ref_rs_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint8_t& ssb_idx()
    {
      assert_choice_type("ssb-Index", type_.to_string(), "pathlossReferenceRS");
      return c.get<uint8_t>();
    }
    uint8_t& csi_rs_idx()
    {
      assert_choice_type("csi-RS-Index", type_.to_string(), "pathlossReferenceRS");
      return c.get<uint8_t>();
    }
    const uint8_t& ssb_idx() const
    {
      assert_choice_type("ssb-Index", type_.to_string(), "pathlossReferenceRS");
      return c.get<uint8_t>();
    }
    const uint8_t& csi_rs_idx() const
    {
      assert_choice_type("csi-RS-Index", type_.to_string(), "pathlossReferenceRS");
      return c.get<uint8_t>();
    }
    uint8_t& set_ssb_idx()
    {
      set(types::ssb_idx);
      return c.get<uint8_t>();
    }
    uint8_t& set_csi_rs_idx()
    {
      set(types::csi_rs_idx);
      return c.get<uint8_t>();
    }

  private:
    types               type_;
    pod_choice_buffer_t c;

    void destroy_();
  };
  struct srs_pwr_ctrl_adjustment_states_opts {
    enum options { same_as_fci2, separate_closed_loop, nulltype } value;
    typedef uint8_t number_type;

    std::string to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<srs_pwr_ctrl_adjustment_states_opts> srs_pwr_ctrl_adjustment_states_e_;

  // member variables
  bool                              ext                                    = false;
  bool                              srs_res_id_list_present                = false;
  bool                              alpha_present                          = false;
  bool                              p0_present                             = false;
  bool                              pathloss_ref_rs_present                = false;
  bool                              srs_pwr_ctrl_adjustment_states_present = false;
  uint8_t                           srs_res_set_id                         = 0;
  srs_res_id_list_l_                srs_res_id_list;
  res_type_c_                       res_type;
  usage_e_                          usage;
  alpha_e                           alpha;
  int16_t                           p0 = -202;
  pathloss_ref_rs_c_                pathloss_ref_rs;
  srs_pwr_ctrl_adjustment_states_e_ srs_pwr_ctrl_adjustment_states;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SchedulingRequestResourceConfig ::= SEQUENCE
struct sched_request_res_cfg_s {
  struct periodicity_and_offset_c_ {
    struct types_opts {
      enum options {
        sym2,
        sym6or7,
        sl1,
        sl2,
        sl4,
        sl5,
        sl8,
        sl10,
        sl16,
        sl20,
        sl40,
        sl80,
        sl160,
        sl320,
        sl640,
        nulltype
      } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    periodicity_and_offset_c_() = default;
    periodicity_and_offset_c_(const periodicity_and_offset_c_& other);
    periodicity_and_offset_c_& operator=(const periodicity_and_offset_c_& other);
    ~periodicity_and_offset_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint8_t& sl2()
    {
      assert_choice_type("sl2", type_.to_string(), "periodicityAndOffset");
      return c.get<uint8_t>();
    }
    uint8_t& sl4()
    {
      assert_choice_type("sl4", type_.to_string(), "periodicityAndOffset");
      return c.get<uint8_t>();
    }
    uint8_t& sl5()
    {
      assert_choice_type("sl5", type_.to_string(), "periodicityAndOffset");
      return c.get<uint8_t>();
    }
    uint8_t& sl8()
    {
      assert_choice_type("sl8", type_.to_string(), "periodicityAndOffset");
      return c.get<uint8_t>();
    }
    uint8_t& sl10()
    {
      assert_choice_type("sl10", type_.to_string(), "periodicityAndOffset");
      return c.get<uint8_t>();
    }
    uint8_t& sl16()
    {
      assert_choice_type("sl16", type_.to_string(), "periodicityAndOffset");
      return c.get<uint8_t>();
    }
    uint8_t& sl20()
    {
      assert_choice_type("sl20", type_.to_string(), "periodicityAndOffset");
      return c.get<uint8_t>();
    }
    uint8_t& sl40()
    {
      assert_choice_type("sl40", type_.to_string(), "periodicityAndOffset");
      return c.get<uint8_t>();
    }
    uint8_t& sl80()
    {
      assert_choice_type("sl80", type_.to_string(), "periodicityAndOffset");
      return c.get<uint8_t>();
    }
    uint8_t& sl160()
    {
      assert_choice_type("sl160", type_.to_string(), "periodicityAndOffset");
      return c.get<uint8_t>();
    }
    uint16_t& sl320()
    {
      assert_choice_type("sl320", type_.to_string(), "periodicityAndOffset");
      return c.get<uint16_t>();
    }
    uint16_t& sl640()
    {
      assert_choice_type("sl640", type_.to_string(), "periodicityAndOffset");
      return c.get<uint16_t>();
    }
    const uint8_t& sl2() const
    {
      assert_choice_type("sl2", type_.to_string(), "periodicityAndOffset");
      return c.get<uint8_t>();
    }
    const uint8_t& sl4() const
    {
      assert_choice_type("sl4", type_.to_string(), "periodicityAndOffset");
      return c.get<uint8_t>();
    }
    const uint8_t& sl5() const
    {
      assert_choice_type("sl5", type_.to_string(), "periodicityAndOffset");
      return c.get<uint8_t>();
    }
    const uint8_t& sl8() const
    {
      assert_choice_type("sl8", type_.to_string(), "periodicityAndOffset");
      return c.get<uint8_t>();
    }
    const uint8_t& sl10() const
    {
      assert_choice_type("sl10", type_.to_string(), "periodicityAndOffset");
      return c.get<uint8_t>();
    }
    const uint8_t& sl16() const
    {
      assert_choice_type("sl16", type_.to_string(), "periodicityAndOffset");
      return c.get<uint8_t>();
    }
    const uint8_t& sl20() const
    {
      assert_choice_type("sl20", type_.to_string(), "periodicityAndOffset");
      return c.get<uint8_t>();
    }
    const uint8_t& sl40() const
    {
      assert_choice_type("sl40", type_.to_string(), "periodicityAndOffset");
      return c.get<uint8_t>();
    }
    const uint8_t& sl80() const
    {
      assert_choice_type("sl80", type_.to_string(), "periodicityAndOffset");
      return c.get<uint8_t>();
    }
    const uint8_t& sl160() const
    {
      assert_choice_type("sl160", type_.to_string(), "periodicityAndOffset");
      return c.get<uint8_t>();
    }
    const uint16_t& sl320() const
    {
      assert_choice_type("sl320", type_.to_string(), "periodicityAndOffset");
      return c.get<uint16_t>();
    }
    const uint16_t& sl640() const
    {
      assert_choice_type("sl640", type_.to_string(), "periodicityAndOffset");
      return c.get<uint16_t>();
    }
    uint8_t& set_sl2()
    {
      set(types::sl2);
      return c.get<uint8_t>();
    }
    uint8_t& set_sl4()
    {
      set(types::sl4);
      return c.get<uint8_t>();
    }
    uint8_t& set_sl5()
    {
      set(types::sl5);
      return c.get<uint8_t>();
    }
    uint8_t& set_sl8()
    {
      set(types::sl8);
      return c.get<uint8_t>();
    }
    uint8_t& set_sl10()
    {
      set(types::sl10);
      return c.get<uint8_t>();
    }
    uint8_t& set_sl16()
    {
      set(types::sl16);
      return c.get<uint8_t>();
    }
    uint8_t& set_sl20()
    {
      set(types::sl20);
      return c.get<uint8_t>();
    }
    uint8_t& set_sl40()
    {
      set(types::sl40);
      return c.get<uint8_t>();
    }
    uint8_t& set_sl80()
    {
      set(types::sl80);
      return c.get<uint8_t>();
    }
    uint8_t& set_sl160()
    {
      set(types::sl160);
      return c.get<uint8_t>();
    }
    uint16_t& set_sl320()
    {
      set(types::sl320);
      return c.get<uint16_t>();
    }
    uint16_t& set_sl640()
    {
      set(types::sl640);
      return c.get<uint16_t>();
    }

  private:
    types               type_;
    pod_choice_buffer_t c;

    void destroy_();
  };

  // member variables
  bool                      periodicity_and_offset_present = false;
  bool                      res_present                    = false;
  uint8_t                   sched_request_res_id           = 1;
  uint8_t                   sched_request_id               = 0;
  periodicity_and_offset_c_ periodicity_and_offset;
  uint8_t                   res = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UCI-OnPUSCH ::= SEQUENCE
struct uci_on_pusch_s {
  struct beta_offsets_c_ {
    using dynamic_l_ = std::array<beta_offsets_s, 4>;
    struct types_opts {
      enum options { dynamic_type, semi_static, nulltype } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    beta_offsets_c_() = default;
    beta_offsets_c_(const beta_offsets_c_& other);
    beta_offsets_c_& operator=(const beta_offsets_c_& other);
    ~beta_offsets_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    dynamic_l_& dynamic_type()
    {
      assert_choice_type("dynamic", type_.to_string(), "betaOffsets");
      return c.get<dynamic_l_>();
    }
    beta_offsets_s& semi_static()
    {
      assert_choice_type("semiStatic", type_.to_string(), "betaOffsets");
      return c.get<beta_offsets_s>();
    }
    const dynamic_l_& dynamic_type() const
    {
      assert_choice_type("dynamic", type_.to_string(), "betaOffsets");
      return c.get<dynamic_l_>();
    }
    const beta_offsets_s& semi_static() const
    {
      assert_choice_type("semiStatic", type_.to_string(), "betaOffsets");
      return c.get<beta_offsets_s>();
    }
    dynamic_l_& set_dynamic_type()
    {
      set(types::dynamic_type);
      return c.get<dynamic_l_>();
    }
    beta_offsets_s& set_semi_static()
    {
      set(types::semi_static);
      return c.get<beta_offsets_s>();
    }

  private:
    types                                       type_;
    choice_buffer_t<beta_offsets_s, dynamic_l_> c;

    void destroy_();
  };
  struct scaling_opts {
    enum options { f0p5, f0p65, f0p8, f1, nulltype } value;

    std::string to_string() const;
  };
  typedef enumerated<scaling_opts> scaling_e_;

  // member variables
  bool            beta_offsets_present = false;
  beta_offsets_c_ beta_offsets;
  scaling_e_      scaling;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BeamFailureRecoveryConfig ::= SEQUENCE
struct beam_fail_recovery_cfg_s {
  using candidate_beam_rs_list_l_ = dyn_array<prach_res_ded_bfr_c>;
  struct ssb_per_rach_occasion_opts {
    enum options { one_eighth, one_fourth, one_half, one, two, four, eight, sixteen, nulltype } value;
    typedef float number_type;

    std::string to_string() const;
    float       to_number() const;
    std::string to_number_string() const;
  };
  typedef enumerated<ssb_per_rach_occasion_opts> ssb_per_rach_occasion_e_;
  struct beam_fail_recovery_timer_opts {
    enum options { ms10, ms20, ms40, ms60, ms80, ms100, ms150, ms200, nulltype } value;
    typedef uint8_t number_type;

    std::string to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<beam_fail_recovery_timer_opts> beam_fail_recovery_timer_e_;

  // member variables
  bool                        ext                              = false;
  bool                        root_seq_idx_bfr_present         = false;
  bool                        rach_cfg_bfr_present             = false;
  bool                        rsrp_thres_ssb_present           = false;
  bool                        candidate_beam_rs_list_present   = false;
  bool                        ssb_per_rach_occasion_present    = false;
  bool                        ra_ssb_occasion_mask_idx_present = false;
  bool                        recovery_search_space_id_present = false;
  bool                        ra_prioritization_present        = false;
  bool                        beam_fail_recovery_timer_present = false;
  uint8_t                     root_seq_idx_bfr                 = 0;
  rach_cfg_generic_s          rach_cfg_bfr;
  uint8_t                     rsrp_thres_ssb = 0;
  candidate_beam_rs_list_l_   candidate_beam_rs_list;
  ssb_per_rach_occasion_e_    ssb_per_rach_occasion;
  uint8_t                     ra_ssb_occasion_mask_idx = 0;
  uint8_t                     recovery_search_space_id = 0;
  ra_prioritization_s         ra_prioritization;
  beam_fail_recovery_timer_e_ beam_fail_recovery_timer;
  // ...
  // group 0
  bool                 msg1_subcarrier_spacing_v1530_present = false;
  subcarrier_spacing_e msg1_subcarrier_spacing_v1530;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CSI-AssociatedReportConfigInfo ::= SEQUENCE
struct csi_associated_report_cfg_info_s {
  struct res_for_ch_c_ {
    struct nzp_csi_rs_s_ {
      using qcl_info_l_ = bounded_array<uint8_t, 16>;

      // member variables
      bool        qcl_info_present = false;
      uint8_t     res_set          = 1;
      qcl_info_l_ qcl_info;
    };
    struct types_opts {
      enum options { nzp_csi_rs, csi_ssb_res_set, nulltype } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    res_for_ch_c_() = default;
    res_for_ch_c_(const res_for_ch_c_& other);
    res_for_ch_c_& operator=(const res_for_ch_c_& other);
    ~res_for_ch_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    nzp_csi_rs_s_& nzp_csi_rs()
    {
      assert_choice_type("nzp-CSI-RS", type_.to_string(), "resourcesForChannel");
      return c.get<nzp_csi_rs_s_>();
    }
    uint8_t& csi_ssb_res_set()
    {
      assert_choice_type("csi-SSB-ResourceSet", type_.to_string(), "resourcesForChannel");
      return c.get<uint8_t>();
    }
    const nzp_csi_rs_s_& nzp_csi_rs() const
    {
      assert_choice_type("nzp-CSI-RS", type_.to_string(), "resourcesForChannel");
      return c.get<nzp_csi_rs_s_>();
    }
    const uint8_t& csi_ssb_res_set() const
    {
      assert_choice_type("csi-SSB-ResourceSet", type_.to_string(), "resourcesForChannel");
      return c.get<uint8_t>();
    }
    nzp_csi_rs_s_& set_nzp_csi_rs()
    {
      set(types::nzp_csi_rs);
      return c.get<nzp_csi_rs_s_>();
    }
    uint8_t& set_csi_ssb_res_set()
    {
      set(types::csi_ssb_res_set);
      return c.get<uint8_t>();
    }

  private:
    types                          type_;
    choice_buffer_t<nzp_csi_rs_s_> c;

    void destroy_();
  };

  // member variables
  bool          ext                                     = false;
  bool          csi_im_res_for_interference_present     = false;
  bool          nzp_csi_rs_res_for_interference_present = false;
  uint8_t       report_cfg_id                           = 0;
  res_for_ch_c_ res_for_ch;
  uint8_t       csi_im_res_for_interference     = 1;
  uint8_t       nzp_csi_rs_res_for_interference = 1;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ConfiguredGrantConfig ::= SEQUENCE
struct cfgured_grant_cfg_s {
  struct freq_hop_opts {
    enum options { intra_slot, inter_slot, nulltype } value;

    std::string to_string() const;
  };
  typedef enumerated<freq_hop_opts> freq_hop_e_;
  struct mcs_table_opts {
    enum options { qam256, qam64_low_se, nulltype } value;
    typedef uint16_t number_type;

    std::string to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<mcs_table_opts> mcs_table_e_;
  struct mcs_table_transform_precoder_opts {
    enum options { qam256, qam64_low_se, nulltype } value;
    typedef uint16_t number_type;

    std::string to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<mcs_table_transform_precoder_opts> mcs_table_transform_precoder_e_;
  struct res_alloc_opts {
    enum options { res_alloc_type0, res_alloc_type1, dynamic_switch, nulltype } value;
    typedef uint8_t number_type;

    std::string to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<res_alloc_opts> res_alloc_e_;
  struct pwr_ctrl_loop_to_use_opts {
    enum options { n0, n1, nulltype } value;
    typedef uint8_t number_type;

    std::string to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<pwr_ctrl_loop_to_use_opts> pwr_ctrl_loop_to_use_e_;
  struct transform_precoder_opts {
    enum options { enabled, disabled, nulltype } value;

    std::string to_string() const;
  };
  typedef enumerated<transform_precoder_opts> transform_precoder_e_;
  struct rep_k_opts {
    enum options { n1, n2, n4, n8, nulltype } value;
    typedef uint8_t number_type;

    std::string to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<rep_k_opts> rep_k_e_;
  struct rep_k_rv_opts {
    enum options { s1_minus0231, s2_minus0303, s3_minus0000, nulltype } value;
    typedef uint8_t number_type;

    std::string to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<rep_k_rv_opts> rep_k_rv_e_;
  struct periodicity_opts {
    enum options {
      sym2,
      sym7,
      sym1x14,
      sym2x14,
      sym4x14,
      sym5x14,
      sym8x14,
      sym10x14,
      sym16x14,
      sym20x14,
      sym32x14,
      sym40x14,
      sym64x14,
      sym80x14,
      sym128x14,
      sym160x14,
      sym256x14,
      sym320x14,
      sym512x14,
      sym640x14,
      sym1024x14,
      sym1280x14,
      sym2560x14,
      sym5120x14,
      sym6,
      sym1x12,
      sym2x12,
      sym4x12,
      sym5x12,
      sym8x12,
      sym10x12,
      sym16x12,
      sym20x12,
      sym32x12,
      sym40x12,
      sym64x12,
      sym80x12,
      sym128x12,
      sym160x12,
      sym256x12,
      sym320x12,
      sym512x12,
      sym640x12,
      sym1280x12,
      sym2560x12,
      nulltype
    } value;

    std::string to_string() const;
  };
  typedef enumerated<periodicity_opts> periodicity_e_;
  struct rrc_cfgured_ul_grant_s_ {
    bool                ext                          = false;
    bool                dmrs_seq_initization_present = false;
    bool                srs_res_ind_present          = false;
    bool                freq_hop_offset_present      = false;
    uint16_t            time_domain_offset           = 0;
    uint8_t             time_domain_alloc            = 0;
    fixed_bitstring<18> freq_domain_alloc;
    uint8_t             ant_port                 = 0;
    uint8_t             dmrs_seq_initization     = 0;
    uint8_t             precoding_and_nof_layers = 0;
    uint8_t             srs_res_ind              = 0;
    uint8_t             mcs_and_tbs              = 0;
    uint16_t            freq_hop_offset          = 1;
    uint8_t             pathloss_ref_idx         = 0;
    // ...
  };

  // member variables
  bool                               ext                                  = false;
  bool                               freq_hop_present                     = false;
  bool                               mcs_table_present                    = false;
  bool                               mcs_table_transform_precoder_present = false;
  bool                               uci_on_pusch_present                 = false;
  bool                               rbg_size_present                     = false;
  bool                               transform_precoder_present           = false;
  bool                               rep_k_rv_present                     = false;
  bool                               cfgured_grant_timer_present          = false;
  bool                               rrc_cfgured_ul_grant_present         = false;
  freq_hop_e_                        freq_hop;
  dmrs_ul_cfg_s                      cg_dmrs_cfg;
  mcs_table_e_                       mcs_table;
  mcs_table_transform_precoder_e_    mcs_table_transform_precoder;
  setup_release_c<cg_uci_on_pusch_c> uci_on_pusch;
  res_alloc_e_                       res_alloc;
  pwr_ctrl_loop_to_use_e_            pwr_ctrl_loop_to_use;
  uint8_t                            p0_pusch_alpha = 0;
  transform_precoder_e_              transform_precoder;
  uint8_t                            nrof_harq_processes = 1;
  rep_k_e_                           rep_k;
  rep_k_rv_e_                        rep_k_rv;
  periodicity_e_                     periodicity;
  uint8_t                            cfgured_grant_timer = 1;
  rrc_cfgured_ul_grant_s_            rrc_cfgured_ul_grant;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DMRS-DownlinkConfig ::= SEQUENCE
struct dmrs_dl_cfg_s {
  struct dmrs_add_position_opts {
    enum options { pos0, pos1, pos3, nulltype } value;
    typedef uint8_t number_type;

    std::string to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<dmrs_add_position_opts> dmrs_add_position_e_;

  // member variables
  bool                           ext                       = false;
  bool                           dmrs_type_present         = false;
  bool                           dmrs_add_position_present = false;
  bool                           max_len_present           = false;
  bool                           scrambling_id0_present    = false;
  bool                           scrambling_id1_present    = false;
  bool                           phase_tracking_rs_present = false;
  dmrs_add_position_e_           dmrs_add_position;
  uint32_t                       scrambling_id0 = 0;
  uint32_t                       scrambling_id1 = 0;
  setup_release_c<ptrs_dl_cfg_s> phase_tracking_rs;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DownlinkPreemption ::= SEQUENCE
struct dl_preemption_s {
  struct time_freq_set_opts {
    enum options { set0, set1, nulltype } value;
    typedef uint8_t number_type;

    std::string to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<time_freq_set_opts> time_freq_set_e_;
  using int_cfg_per_serving_cell_l_ = dyn_array<int_cfg_per_serving_cell_s>;

  // member variables
  bool                        ext      = false;
  uint32_t                    int_rnti = 0;
  time_freq_set_e_            time_freq_set;
  uint8_t                     dci_payload_size = 0;
  int_cfg_per_serving_cell_l_ int_cfg_per_serving_cell;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// EUTRA-MBSFN-SubframeConfig ::= SEQUENCE
struct eutra_mbsfn_sf_cfg_s {
  struct radioframe_alloc_period_opts {
    enum options { n1, n2, n4, n8, n16, n32, nulltype } value;
    typedef uint8_t number_type;

    std::string to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<radioframe_alloc_period_opts> radioframe_alloc_period_e_;
  struct sf_alloc1_c_ {
    struct types_opts {
      enum options { one_frame, four_frames, nulltype } value;
      typedef uint8_t number_type;

      std::string to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    sf_alloc1_c_() = default;
    sf_alloc1_c_(const sf_alloc1_c_& other);
    sf_alloc1_c_& operator=(const sf_alloc1_c_& other);
    ~sf_alloc1_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    fixed_bitstring<6>& one_frame()
    {
      assert_choice_type("oneFrame", type_.to_string(), "subframeAllocation1");
      return c.get<fixed_bitstring<6> >();
    }
    fixed_bitstring<24>& four_frames()
    {
      assert_choice_type("fourFrames", type_.to_string(), "subframeAllocation1");
      return c.get<fixed_bitstring<24> >();
    }
    const fixed_bitstring<6>& one_frame() const
    {
      assert_choice_type("oneFrame", type_.to_string(), "subframeAllocation1");
      return c.get<fixed_bitstring<6> >();
    }
    const fixed_bitstring<24>& four_frames() const
    {
      assert_choice_type("fourFrames", type_.to_string(), "subframeAllocation1");
      return c.get<fixed_bitstring<24> >();
    }
    fixed_bitstring<6>& set_one_frame()
    {
      set(types::one_frame);
      return c.get<fixed_bitstring<6> >();
    }
    fixed_bitstring<24>& set_four_frames()
    {
      set(types::four_frames);
      return c.get<fixed_bitstring<24> >();
    }

  private:
    types                                 type_;
    choice_buffer_t<fixed_bitstring<24> > c;

    void destroy_();
  };
  struct sf_alloc2_c_ {
    struct types_opts {
      enum options { one_frame, four_frames, nulltype } value;
      typedef uint8_t number_type;

      std::string to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    sf_alloc2_c_() = default;
    sf_alloc2_c_(const sf_alloc2_c_& other);
    sf_alloc2_c_& operator=(const sf_alloc2_c_& other);
    ~sf_alloc2_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    fixed_bitstring<2>& one_frame()
    {
      assert_choice_type("oneFrame", type_.to_string(), "subframeAllocation2");
      return c.get<fixed_bitstring<2> >();
    }
    fixed_bitstring<8>& four_frames()
    {
      assert_choice_type("fourFrames", type_.to_string(), "subframeAllocation2");
      return c.get<fixed_bitstring<8> >();
    }
    const fixed_bitstring<2>& one_frame() const
    {
      assert_choice_type("oneFrame", type_.to_string(), "subframeAllocation2");
      return c.get<fixed_bitstring<2> >();
    }
    const fixed_bitstring<8>& four_frames() const
    {
      assert_choice_type("fourFrames", type_.to_string(), "subframeAllocation2");
      return c.get<fixed_bitstring<8> >();
    }
    fixed_bitstring<2>& set_one_frame()
    {
      set(types::one_frame);
      return c.get<fixed_bitstring<2> >();
    }
    fixed_bitstring<8>& set_four_frames()
    {
      set(types::four_frames);
      return c.get<fixed_bitstring<8> >();
    }

  private:
    types                                type_;
    choice_buffer_t<fixed_bitstring<8> > c;

    void destroy_();
  };

  // member variables
  bool                       ext               = false;
  bool                       sf_alloc2_present = false;
  radioframe_alloc_period_e_ radioframe_alloc_period;
  uint8_t                    radioframe_alloc_offset = 0;
  sf_alloc1_c_               sf_alloc1;
  sf_alloc2_c_               sf_alloc2;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MultiFrequencyBandListNR ::= SEQUENCE (SIZE (1..8)) OF INTEGER (1..1024)
using multi_freq_band_list_nr_l = bounded_array<uint16_t, 8>;

// PUCCH-Config ::= SEQUENCE
struct pucch_cfg_s {
  using res_set_to_add_mod_list_l_               = dyn_array<pucch_res_set_s>;
  using res_set_to_release_list_l_               = bounded_array<uint8_t, 4>;
  using res_to_add_mod_list_l_                   = dyn_array<pucch_res_s>;
  using res_to_release_list_l_                   = dyn_array<uint8_t>;
  using sched_request_res_to_add_mod_list_l_     = dyn_array<sched_request_res_cfg_s>;
  using sched_request_res_to_release_list_l_     = bounded_array<uint8_t, 8>;
  using multi_csi_pucch_res_list_l_              = bounded_array<uint8_t, 2>;
  using dl_data_to_ul_ack_l_                     = bounded_array<uint8_t, 8>;
  using spatial_relation_info_to_add_mod_list_l_ = dyn_array<pucch_spatial_relation_info_s>;
  using spatial_relation_info_to_release_list_l_ = bounded_array<uint8_t, 8>;

  // member variables
  bool                                     ext                                           = false;
  bool                                     res_set_to_add_mod_list_present               = false;
  bool                                     res_set_to_release_list_present               = false;
  bool                                     res_to_add_mod_list_present                   = false;
  bool                                     res_to_release_list_present                   = false;
  bool                                     format1_present                               = false;
  bool                                     format2_present                               = false;
  bool                                     format3_present                               = false;
  bool                                     format4_present                               = false;
  bool                                     sched_request_res_to_add_mod_list_present     = false;
  bool                                     sched_request_res_to_release_list_present     = false;
  bool                                     multi_csi_pucch_res_list_present              = false;
  bool                                     dl_data_to_ul_ack_present                     = false;
  bool                                     spatial_relation_info_to_add_mod_list_present = false;
  bool                                     spatial_relation_info_to_release_list_present = false;
  bool                                     pucch_pwr_ctrl_present                        = false;
  res_set_to_add_mod_list_l_               res_set_to_add_mod_list;
  res_set_to_release_list_l_               res_set_to_release_list;
  res_to_add_mod_list_l_                   res_to_add_mod_list;
  res_to_release_list_l_                   res_to_release_list;
  setup_release_c<pucch_format_cfg_s>      format1;
  setup_release_c<pucch_format_cfg_s>      format2;
  setup_release_c<pucch_format_cfg_s>      format3;
  setup_release_c<pucch_format_cfg_s>      format4;
  sched_request_res_to_add_mod_list_l_     sched_request_res_to_add_mod_list;
  sched_request_res_to_release_list_l_     sched_request_res_to_release_list;
  multi_csi_pucch_res_list_l_              multi_csi_pucch_res_list;
  dl_data_to_ul_ack_l_                     dl_data_to_ul_ack;
  spatial_relation_info_to_add_mod_list_l_ spatial_relation_info_to_add_mod_list;
  spatial_relation_info_to_release_list_l_ spatial_relation_info_to_release_list;
  pucch_pwr_ctrl_s                         pucch_pwr_ctrl;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PUCCH-TPC-CommandConfig ::= SEQUENCE
struct pucch_tpc_cmd_cfg_s {
  bool    ext                         = false;
  bool    tpc_idx_pcell_present       = false;
  bool    tpc_idx_pucch_scell_present = false;
  uint8_t tpc_idx_pcell               = 1;
  uint8_t tpc_idx_pucch_scell         = 1;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PUSCH-Config ::= SEQUENCE
struct pusch_cfg_s {
  struct tx_cfg_opts {
    enum options { codebook, non_codebook, nulltype } value;

    std::string to_string() const;
  };
  typedef enumerated<tx_cfg_opts> tx_cfg_e_;
  struct freq_hop_opts {
    enum options { intra_slot, inter_slot, nulltype } value;

    std::string to_string() const;
  };
  typedef enumerated<freq_hop_opts> freq_hop_e_;
  using freq_hop_offset_lists_l_ = bounded_array<uint16_t, 4>;
  struct res_alloc_opts {
    enum options { res_alloc_type0, res_alloc_type1, dynamic_switch, nulltype } value;
    typedef uint8_t number_type;

    std::string to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<res_alloc_opts> res_alloc_e_;
  struct pusch_aggregation_factor_opts {
    enum options { n2, n4, n8, nulltype } value;
    typedef uint8_t number_type;

    std::string to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<pusch_aggregation_factor_opts> pusch_aggregation_factor_e_;
  struct mcs_table_opts {
    enum options { qam256, qam64_low_se, nulltype } value;
    typedef uint16_t number_type;

    std::string to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<mcs_table_opts> mcs_table_e_;
  struct mcs_table_transform_precoder_opts {
    enum options { qam256, qam64_low_se, nulltype } value;
    typedef uint16_t number_type;

    std::string to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<mcs_table_transform_precoder_opts> mcs_table_transform_precoder_e_;
  struct transform_precoder_opts {
    enum options { enabled, disabled, nulltype } value;

    std::string to_string() const;
  };
  typedef enumerated<transform_precoder_opts> transform_precoder_e_;
  struct codebook_subset_opts {
    enum options { fully_and_partial_and_non_coherent, partial_and_non_coherent, non_coherent, nulltype } value;

    std::string to_string() const;
  };
  typedef enumerated<codebook_subset_opts> codebook_subset_e_;

  // member variables
  bool                                                               ext                                  = false;
  bool                                                               data_scrambling_id_pusch_present     = false;
  bool                                                               tx_cfg_present                       = false;
  bool                                                               dmrs_ul_for_pusch_map_type_a_present = false;
  bool                                                               dmrs_ul_for_pusch_map_type_b_present = false;
  bool                                                               pusch_pwr_ctrl_present               = false;
  bool                                                               freq_hop_present                     = false;
  bool                                                               freq_hop_offset_lists_present        = false;
  bool                                                               pusch_time_domain_alloc_list_present = false;
  bool                                                               pusch_aggregation_factor_present     = false;
  bool                                                               mcs_table_present                    = false;
  bool                                                               mcs_table_transform_precoder_present = false;
  bool                                                               transform_precoder_present           = false;
  bool                                                               codebook_subset_present              = false;
  bool                                                               max_rank_present                     = false;
  bool                                                               rbg_size_present                     = false;
  bool                                                               uci_on_pusch_present                 = false;
  bool                                                               tp_pi2_bpsk_present                  = false;
  uint16_t                                                           data_scrambling_id_pusch             = 0;
  tx_cfg_e_                                                          tx_cfg;
  setup_release_c<dmrs_ul_cfg_s>                                     dmrs_ul_for_pusch_map_type_a;
  setup_release_c<dmrs_ul_cfg_s>                                     dmrs_ul_for_pusch_map_type_b;
  pusch_pwr_ctrl_s                                                   pusch_pwr_ctrl;
  freq_hop_e_                                                        freq_hop;
  freq_hop_offset_lists_l_                                           freq_hop_offset_lists;
  res_alloc_e_                                                       res_alloc;
  setup_release_c<dyn_seq_of<pusch_time_domain_res_alloc_s, 1, 16> > pusch_time_domain_alloc_list;
  pusch_aggregation_factor_e_                                        pusch_aggregation_factor;
  mcs_table_e_                                                       mcs_table;
  mcs_table_transform_precoder_e_                                    mcs_table_transform_precoder;
  transform_precoder_e_                                              transform_precoder;
  codebook_subset_e_                                                 codebook_subset;
  uint8_t                                                            max_rank = 1;
  setup_release_c<uci_on_pusch_s>                                    uci_on_pusch;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PUSCH-TPC-CommandConfig ::= SEQUENCE
struct pusch_tpc_cmd_cfg_s {
  bool    ext                 = false;
  bool    tpc_idx_present     = false;
  bool    tpc_idx_sul_present = false;
  bool    target_cell_present = false;
  uint8_t tpc_idx             = 1;
  uint8_t tpc_idx_sul         = 1;
  uint8_t target_cell         = 0;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RadioLinkMonitoringRS ::= SEQUENCE
struct radio_link_monitoring_rs_s {
  struct purpose_opts {
    enum options { beam_fail, rlf, both, nulltype } value;

    std::string to_string() const;
  };
  typedef enumerated<purpose_opts> purpose_e_;
  struct detection_res_c_ {
    struct types_opts {
      enum options { ssb_idx, csi_rs_idx, nulltype } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    detection_res_c_() = default;
    detection_res_c_(const detection_res_c_& other);
    detection_res_c_& operator=(const detection_res_c_& other);
    ~detection_res_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint8_t& ssb_idx()
    {
      assert_choice_type("ssb-Index", type_.to_string(), "detectionResource");
      return c.get<uint8_t>();
    }
    uint8_t& csi_rs_idx()
    {
      assert_choice_type("csi-RS-Index", type_.to_string(), "detectionResource");
      return c.get<uint8_t>();
    }
    const uint8_t& ssb_idx() const
    {
      assert_choice_type("ssb-Index", type_.to_string(), "detectionResource");
      return c.get<uint8_t>();
    }
    const uint8_t& csi_rs_idx() const
    {
      assert_choice_type("csi-RS-Index", type_.to_string(), "detectionResource");
      return c.get<uint8_t>();
    }
    uint8_t& set_ssb_idx()
    {
      set(types::ssb_idx);
      return c.get<uint8_t>();
    }
    uint8_t& set_csi_rs_idx()
    {
      set(types::csi_rs_idx);
      return c.get<uint8_t>();
    }

  private:
    types               type_;
    pod_choice_buffer_t c;

    void destroy_();
  };

  // member variables
  bool             ext                         = false;
  uint8_t          radio_link_monitoring_rs_id = 0;
  purpose_e_       purpose;
  detection_res_c_ detection_res;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RateMatchPattern ::= SEQUENCE
struct rate_match_pattern_s {
  struct pattern_type_c_ {
    struct bitmaps_s_ {
      struct symbols_in_res_block_c_ {
        struct types_opts {
          enum options { one_slot, two_slots, nulltype } value;
          typedef uint8_t number_type;

          std::string to_string() const;
          uint8_t     to_number() const;
        };
        typedef enumerated<types_opts> types;

        // choice methods
        symbols_in_res_block_c_() = default;
        symbols_in_res_block_c_(const symbols_in_res_block_c_& other);
        symbols_in_res_block_c_& operator=(const symbols_in_res_block_c_& other);
        ~symbols_in_res_block_c_() { destroy_(); }
        void        set(types::options e = types::nulltype);
        types       type() const { return type_; }
        SRSASN_CODE pack(bit_ref& bref) const;
        SRSASN_CODE unpack(cbit_ref& bref);
        void        to_json(json_writer& j) const;
        // getters
        fixed_bitstring<14>& one_slot()
        {
          assert_choice_type("oneSlot", type_.to_string(), "symbolsInResourceBlock");
          return c.get<fixed_bitstring<14> >();
        }
        fixed_bitstring<28>& two_slots()
        {
          assert_choice_type("twoSlots", type_.to_string(), "symbolsInResourceBlock");
          return c.get<fixed_bitstring<28> >();
        }
        const fixed_bitstring<14>& one_slot() const
        {
          assert_choice_type("oneSlot", type_.to_string(), "symbolsInResourceBlock");
          return c.get<fixed_bitstring<14> >();
        }
        const fixed_bitstring<28>& two_slots() const
        {
          assert_choice_type("twoSlots", type_.to_string(), "symbolsInResourceBlock");
          return c.get<fixed_bitstring<28> >();
        }
        fixed_bitstring<14>& set_one_slot()
        {
          set(types::one_slot);
          return c.get<fixed_bitstring<14> >();
        }
        fixed_bitstring<28>& set_two_slots()
        {
          set(types::two_slots);
          return c.get<fixed_bitstring<28> >();
        }

      private:
        types                                 type_;
        choice_buffer_t<fixed_bitstring<28> > c;

        void destroy_();
      };
      struct periodicity_and_pattern_c_ {
        struct types_opts {
          enum options { n2, n4, n5, n8, n10, n20, n40, nulltype } value;
          typedef uint8_t number_type;

          std::string to_string() const;
          uint8_t     to_number() const;
        };
        typedef enumerated<types_opts> types;

        // choice methods
        periodicity_and_pattern_c_() = default;
        periodicity_and_pattern_c_(const periodicity_and_pattern_c_& other);
        periodicity_and_pattern_c_& operator=(const periodicity_and_pattern_c_& other);
        ~periodicity_and_pattern_c_() { destroy_(); }
        void        set(types::options e = types::nulltype);
        types       type() const { return type_; }
        SRSASN_CODE pack(bit_ref& bref) const;
        SRSASN_CODE unpack(cbit_ref& bref);
        void        to_json(json_writer& j) const;
        // getters
        fixed_bitstring<2>& n2()
        {
          assert_choice_type("n2", type_.to_string(), "periodicityAndPattern");
          return c.get<fixed_bitstring<2> >();
        }
        fixed_bitstring<4>& n4()
        {
          assert_choice_type("n4", type_.to_string(), "periodicityAndPattern");
          return c.get<fixed_bitstring<4> >();
        }
        fixed_bitstring<5>& n5()
        {
          assert_choice_type("n5", type_.to_string(), "periodicityAndPattern");
          return c.get<fixed_bitstring<5> >();
        }
        fixed_bitstring<8>& n8()
        {
          assert_choice_type("n8", type_.to_string(), "periodicityAndPattern");
          return c.get<fixed_bitstring<8> >();
        }
        fixed_bitstring<10>& n10()
        {
          assert_choice_type("n10", type_.to_string(), "periodicityAndPattern");
          return c.get<fixed_bitstring<10> >();
        }
        fixed_bitstring<20>& n20()
        {
          assert_choice_type("n20", type_.to_string(), "periodicityAndPattern");
          return c.get<fixed_bitstring<20> >();
        }
        fixed_bitstring<40>& n40()
        {
          assert_choice_type("n40", type_.to_string(), "periodicityAndPattern");
          return c.get<fixed_bitstring<40> >();
        }
        const fixed_bitstring<2>& n2() const
        {
          assert_choice_type("n2", type_.to_string(), "periodicityAndPattern");
          return c.get<fixed_bitstring<2> >();
        }
        const fixed_bitstring<4>& n4() const
        {
          assert_choice_type("n4", type_.to_string(), "periodicityAndPattern");
          return c.get<fixed_bitstring<4> >();
        }
        const fixed_bitstring<5>& n5() const
        {
          assert_choice_type("n5", type_.to_string(), "periodicityAndPattern");
          return c.get<fixed_bitstring<5> >();
        }
        const fixed_bitstring<8>& n8() const
        {
          assert_choice_type("n8", type_.to_string(), "periodicityAndPattern");
          return c.get<fixed_bitstring<8> >();
        }
        const fixed_bitstring<10>& n10() const
        {
          assert_choice_type("n10", type_.to_string(), "periodicityAndPattern");
          return c.get<fixed_bitstring<10> >();
        }
        const fixed_bitstring<20>& n20() const
        {
          assert_choice_type("n20", type_.to_string(), "periodicityAndPattern");
          return c.get<fixed_bitstring<20> >();
        }
        const fixed_bitstring<40>& n40() const
        {
          assert_choice_type("n40", type_.to_string(), "periodicityAndPattern");
          return c.get<fixed_bitstring<40> >();
        }
        fixed_bitstring<2>& set_n2()
        {
          set(types::n2);
          return c.get<fixed_bitstring<2> >();
        }
        fixed_bitstring<4>& set_n4()
        {
          set(types::n4);
          return c.get<fixed_bitstring<4> >();
        }
        fixed_bitstring<5>& set_n5()
        {
          set(types::n5);
          return c.get<fixed_bitstring<5> >();
        }
        fixed_bitstring<8>& set_n8()
        {
          set(types::n8);
          return c.get<fixed_bitstring<8> >();
        }
        fixed_bitstring<10>& set_n10()
        {
          set(types::n10);
          return c.get<fixed_bitstring<10> >();
        }
        fixed_bitstring<20>& set_n20()
        {
          set(types::n20);
          return c.get<fixed_bitstring<20> >();
        }
        fixed_bitstring<40>& set_n40()
        {
          set(types::n40);
          return c.get<fixed_bitstring<40> >();
        }

      private:
        types                                 type_;
        choice_buffer_t<fixed_bitstring<40> > c;

        void destroy_();
      };

      // member variables
      bool                       ext                             = false;
      bool                       periodicity_and_pattern_present = false;
      fixed_bitstring<275>       res_blocks;
      symbols_in_res_block_c_    symbols_in_res_block;
      periodicity_and_pattern_c_ periodicity_and_pattern;
      // ...
    };
    struct types_opts {
      enum options { bitmaps, ctrl_res_set, nulltype } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    pattern_type_c_() = default;
    pattern_type_c_(const pattern_type_c_& other);
    pattern_type_c_& operator=(const pattern_type_c_& other);
    ~pattern_type_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    bitmaps_s_& bitmaps()
    {
      assert_choice_type("bitmaps", type_.to_string(), "patternType");
      return c.get<bitmaps_s_>();
    }
    uint8_t& ctrl_res_set()
    {
      assert_choice_type("controlResourceSet", type_.to_string(), "patternType");
      return c.get<uint8_t>();
    }
    const bitmaps_s_& bitmaps() const
    {
      assert_choice_type("bitmaps", type_.to_string(), "patternType");
      return c.get<bitmaps_s_>();
    }
    const uint8_t& ctrl_res_set() const
    {
      assert_choice_type("controlResourceSet", type_.to_string(), "patternType");
      return c.get<uint8_t>();
    }
    bitmaps_s_& set_bitmaps()
    {
      set(types::bitmaps);
      return c.get<bitmaps_s_>();
    }
    uint8_t& set_ctrl_res_set()
    {
      set(types::ctrl_res_set);
      return c.get<uint8_t>();
    }

  private:
    types                       type_;
    choice_buffer_t<bitmaps_s_> c;

    void destroy_();
  };
  struct dummy_opts {
    enum options { dynamic_value, semi_static, nulltype } value;

    std::string to_string() const;
  };
  typedef enumerated<dummy_opts> dummy_e_;

  // member variables
  bool                 ext                        = false;
  bool                 subcarrier_spacing_present = false;
  uint8_t              rate_match_pattern_id      = 0;
  pattern_type_c_      pattern_type;
  subcarrier_spacing_e subcarrier_spacing;
  dummy_e_             dummy;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

struct rate_match_pattern_group_item_c_ {
  struct types_opts {
    enum options { cell_level, bwp_level, nulltype } value;

    std::string to_string() const;
  };
  typedef enumerated<types_opts> types;

  // choice methods
  rate_match_pattern_group_item_c_() = default;
  rate_match_pattern_group_item_c_(const rate_match_pattern_group_item_c_& other);
  rate_match_pattern_group_item_c_& operator=(const rate_match_pattern_group_item_c_& other);
  ~rate_match_pattern_group_item_c_() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  uint8_t& cell_level()
  {
    assert_choice_type("cellLevel", type_.to_string(), "RateMatchPatternGroup-item");
    return c.get<uint8_t>();
  }
  uint8_t& bwp_level()
  {
    assert_choice_type("bwpLevel", type_.to_string(), "RateMatchPatternGroup-item");
    return c.get<uint8_t>();
  }
  const uint8_t& cell_level() const
  {
    assert_choice_type("cellLevel", type_.to_string(), "RateMatchPatternGroup-item");
    return c.get<uint8_t>();
  }
  const uint8_t& bwp_level() const
  {
    assert_choice_type("bwpLevel", type_.to_string(), "RateMatchPatternGroup-item");
    return c.get<uint8_t>();
  }
  uint8_t& set_cell_level()
  {
    set(types::cell_level);
    return c.get<uint8_t>();
  }
  uint8_t& set_bwp_level()
  {
    set(types::bwp_level);
    return c.get<uint8_t>();
  }

private:
  types               type_;
  pod_choice_buffer_t c;

  void destroy_();
};

// RateMatchPatternGroup ::= SEQUENCE (SIZE (1..8)) OF RateMatchPatternGroup-item
using rate_match_pattern_group_l = dyn_array<rate_match_pattern_group_item_c_>;

// SRS-CC-SetIndex ::= SEQUENCE
struct srs_cc_set_idx_s {
  bool    cc_set_idx_present           = false;
  bool    cc_idx_in_one_cc_set_present = false;
  uint8_t cc_set_idx                   = 0;
  uint8_t cc_idx_in_one_cc_set         = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SRS-Config ::= SEQUENCE
struct srs_cfg_s {
  using srs_res_set_to_release_list_l_ = bounded_array<uint8_t, 16>;
  using srs_res_set_to_add_mod_list_l_ = dyn_array<srs_res_set_s>;
  using srs_res_to_release_list_l_     = dyn_array<uint8_t>;
  using srs_res_to_add_mod_list_l_     = dyn_array<srs_res_s>;

  // member variables
  bool                           ext                                 = false;
  bool                           srs_res_set_to_release_list_present = false;
  bool                           srs_res_set_to_add_mod_list_present = false;
  bool                           srs_res_to_release_list_present     = false;
  bool                           srs_res_to_add_mod_list_present     = false;
  bool                           tpc_accumulation_present            = false;
  srs_res_set_to_release_list_l_ srs_res_set_to_release_list;
  srs_res_set_to_add_mod_list_l_ srs_res_set_to_add_mod_list;
  srs_res_to_release_list_l_     srs_res_to_release_list;
  srs_res_to_add_mod_list_l_     srs_res_to_add_mod_list;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SRS-TPC-CommandConfig ::= SEQUENCE
struct srs_tpc_cmd_cfg_s {
  bool    ext                                 = false;
  bool    start_bit_of_format2_minus3_present = false;
  bool    field_type_format2_minus3_present   = false;
  uint8_t start_bit_of_format2_minus3         = 1;
  uint8_t field_type_format2_minus3           = 0;
  // ...
  // group 0
  bool    start_bit_of_format2_minus3_sul_v1530_present = false;
  uint8_t start_bit_of_format2_minus3_sul_v1530         = 1;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SlotFormatCombination ::= SEQUENCE
struct slot_format_combination_s {
  using slot_formats_l_ = dyn_array<uint16_t>;

  // member variables
  uint16_t        slot_format_combination_id = 0;
  slot_formats_l_ slot_formats;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// TCI-State ::= SEQUENCE
struct tci_state_s {
  bool       ext               = false;
  bool       qcl_type2_present = false;
  uint8_t    tci_state_id      = 0;
  qcl_info_s qcl_type1;
  qcl_info_s qcl_type2;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ZP-CSI-RS-Resource ::= SEQUENCE
struct zp_csi_rs_res_s {
  bool                             ext                            = false;
  bool                             periodicity_and_offset_present = false;
  uint8_t                          zp_csi_rs_res_id               = 0;
  csi_rs_res_map_s                 res_map;
  csi_res_periodicity_and_offset_c periodicity_and_offset;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ZP-CSI-RS-ResourceSet ::= SEQUENCE
struct zp_csi_rs_res_set_s {
  using zp_csi_rs_res_id_list_l_ = bounded_array<uint8_t, 16>;

  // member variables
  bool                     ext                  = false;
  uint8_t                  zp_csi_rs_res_set_id = 0;
  zp_csi_rs_res_id_list_l_ zp_csi_rs_res_id_list;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BWP-UplinkDedicated ::= SEQUENCE
struct bwp_ul_ded_s {
  bool                                      ext                            = false;
  bool                                      pucch_cfg_present              = false;
  bool                                      pusch_cfg_present              = false;
  bool                                      cfgured_grant_cfg_present      = false;
  bool                                      srs_cfg_present                = false;
  bool                                      beam_fail_recovery_cfg_present = false;
  setup_release_c<pucch_cfg_s>              pucch_cfg;
  setup_release_c<pusch_cfg_s>              pusch_cfg;
  setup_release_c<cfgured_grant_cfg_s>      cfgured_grant_cfg;
  setup_release_c<srs_cfg_s>                srs_cfg;
  setup_release_c<beam_fail_recovery_cfg_s> beam_fail_recovery_cfg;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CFRA-CSIRS-Resource ::= SEQUENCE
struct cfra_csirs_res_s {
  using ra_occasion_list_l_ = dyn_array<uint16_t>;

  // member variables
  bool                ext    = false;
  uint8_t             csi_rs = 0;
  ra_occasion_list_l_ ra_occasion_list;
  uint8_t             ra_preamb_idx = 0;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CFRA-SSB-Resource ::= SEQUENCE
struct cfra_ssb_res_s {
  bool    ext           = false;
  uint8_t ssb           = 0;
  uint8_t ra_preamb_idx = 0;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CSI-AperiodicTriggerState ::= SEQUENCE
struct csi_aperiodic_trigger_state_s {
  using associated_report_cfg_info_list_l_ = dyn_array<csi_associated_report_cfg_info_s>;

  // member variables
  bool                               ext = false;
  associated_report_cfg_info_list_l_ associated_report_cfg_info_list;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CSI-ReportPeriodicityAndOffset ::= CHOICE
struct csi_report_periodicity_and_offset_c {
  struct types_opts {
    enum options {
      slots4,
      slots5,
      slots8,
      slots10,
      slots16,
      slots20,
      slots40,
      slots80,
      slots160,
      slots320,
      nulltype
    } value;
    typedef uint16_t number_type;

    std::string to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<types_opts> types;

  // choice methods
  csi_report_periodicity_and_offset_c() = default;
  csi_report_periodicity_and_offset_c(const csi_report_periodicity_and_offset_c& other);
  csi_report_periodicity_and_offset_c& operator=(const csi_report_periodicity_and_offset_c& other);
  ~csi_report_periodicity_and_offset_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  uint8_t& slots4()
  {
    assert_choice_type("slots4", type_.to_string(), "CSI-ReportPeriodicityAndOffset");
    return c.get<uint8_t>();
  }
  uint8_t& slots5()
  {
    assert_choice_type("slots5", type_.to_string(), "CSI-ReportPeriodicityAndOffset");
    return c.get<uint8_t>();
  }
  uint8_t& slots8()
  {
    assert_choice_type("slots8", type_.to_string(), "CSI-ReportPeriodicityAndOffset");
    return c.get<uint8_t>();
  }
  uint8_t& slots10()
  {
    assert_choice_type("slots10", type_.to_string(), "CSI-ReportPeriodicityAndOffset");
    return c.get<uint8_t>();
  }
  uint8_t& slots16()
  {
    assert_choice_type("slots16", type_.to_string(), "CSI-ReportPeriodicityAndOffset");
    return c.get<uint8_t>();
  }
  uint8_t& slots20()
  {
    assert_choice_type("slots20", type_.to_string(), "CSI-ReportPeriodicityAndOffset");
    return c.get<uint8_t>();
  }
  uint8_t& slots40()
  {
    assert_choice_type("slots40", type_.to_string(), "CSI-ReportPeriodicityAndOffset");
    return c.get<uint8_t>();
  }
  uint8_t& slots80()
  {
    assert_choice_type("slots80", type_.to_string(), "CSI-ReportPeriodicityAndOffset");
    return c.get<uint8_t>();
  }
  uint8_t& slots160()
  {
    assert_choice_type("slots160", type_.to_string(), "CSI-ReportPeriodicityAndOffset");
    return c.get<uint8_t>();
  }
  uint16_t& slots320()
  {
    assert_choice_type("slots320", type_.to_string(), "CSI-ReportPeriodicityAndOffset");
    return c.get<uint16_t>();
  }
  const uint8_t& slots4() const
  {
    assert_choice_type("slots4", type_.to_string(), "CSI-ReportPeriodicityAndOffset");
    return c.get<uint8_t>();
  }
  const uint8_t& slots5() const
  {
    assert_choice_type("slots5", type_.to_string(), "CSI-ReportPeriodicityAndOffset");
    return c.get<uint8_t>();
  }
  const uint8_t& slots8() const
  {
    assert_choice_type("slots8", type_.to_string(), "CSI-ReportPeriodicityAndOffset");
    return c.get<uint8_t>();
  }
  const uint8_t& slots10() const
  {
    assert_choice_type("slots10", type_.to_string(), "CSI-ReportPeriodicityAndOffset");
    return c.get<uint8_t>();
  }
  const uint8_t& slots16() const
  {
    assert_choice_type("slots16", type_.to_string(), "CSI-ReportPeriodicityAndOffset");
    return c.get<uint8_t>();
  }
  const uint8_t& slots20() const
  {
    assert_choice_type("slots20", type_.to_string(), "CSI-ReportPeriodicityAndOffset");
    return c.get<uint8_t>();
  }
  const uint8_t& slots40() const
  {
    assert_choice_type("slots40", type_.to_string(), "CSI-ReportPeriodicityAndOffset");
    return c.get<uint8_t>();
  }
  const uint8_t& slots80() const
  {
    assert_choice_type("slots80", type_.to_string(), "CSI-ReportPeriodicityAndOffset");
    return c.get<uint8_t>();
  }
  const uint8_t& slots160() const
  {
    assert_choice_type("slots160", type_.to_string(), "CSI-ReportPeriodicityAndOffset");
    return c.get<uint8_t>();
  }
  const uint16_t& slots320() const
  {
    assert_choice_type("slots320", type_.to_string(), "CSI-ReportPeriodicityAndOffset");
    return c.get<uint16_t>();
  }
  uint8_t& set_slots4()
  {
    set(types::slots4);
    return c.get<uint8_t>();
  }
  uint8_t& set_slots5()
  {
    set(types::slots5);
    return c.get<uint8_t>();
  }
  uint8_t& set_slots8()
  {
    set(types::slots8);
    return c.get<uint8_t>();
  }
  uint8_t& set_slots10()
  {
    set(types::slots10);
    return c.get<uint8_t>();
  }
  uint8_t& set_slots16()
  {
    set(types::slots16);
    return c.get<uint8_t>();
  }
  uint8_t& set_slots20()
  {
    set(types::slots20);
    return c.get<uint8_t>();
  }
  uint8_t& set_slots40()
  {
    set(types::slots40);
    return c.get<uint8_t>();
  }
  uint8_t& set_slots80()
  {
    set(types::slots80);
    return c.get<uint8_t>();
  }
  uint8_t& set_slots160()
  {
    set(types::slots160);
    return c.get<uint8_t>();
  }
  uint16_t& set_slots320()
  {
    set(types::slots320);
    return c.get<uint16_t>();
  }

private:
  types               type_;
  pod_choice_buffer_t c;

  void destroy_();
};

// CSI-SemiPersistentOnPUSCH-TriggerState ::= SEQUENCE
struct csi_semi_persistent_on_pusch_trigger_state_s {
  bool    ext                        = false;
  uint8_t associated_report_cfg_info = 0;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CodebookConfig ::= SEQUENCE
struct codebook_cfg_s {
  struct codebook_type_c_ {
    struct type1_s_ {
      struct sub_type_c_ {
        struct type_i_single_panel_s_ {
          struct nr_of_ant_ports_c_ {
            struct two_s_ {
              fixed_bitstring<6> two_tx_codebook_subset_restrict;
            };
            struct more_than_two_s_ {
              struct n1_n2_c_ {
                struct types_opts {
                  enum options {
                    two_one_type_i_single_panel_restrict,
                    two_two_type_i_single_panel_restrict,
                    four_one_type_i_single_panel_restrict,
                    three_two_type_i_single_panel_restrict,
                    six_one_type_i_single_panel_restrict,
                    four_two_type_i_single_panel_restrict,
                    eight_one_type_i_single_panel_restrict,
                    four_three_type_i_single_panel_restrict,
                    six_two_type_i_single_panel_restrict,
                    twelve_one_type_i_single_panel_restrict,
                    four_four_type_i_single_panel_restrict,
                    eight_two_type_i_single_panel_restrict,
                    sixteen_one_type_i_single_panel_restrict,
                    nulltype
                  } value;

                  std::string to_string() const;
                };
                typedef enumerated<types_opts> types;

                // choice methods
                n1_n2_c_() = default;
                n1_n2_c_(const n1_n2_c_& other);
                n1_n2_c_& operator=(const n1_n2_c_& other);
                ~n1_n2_c_() { destroy_(); }
                void        set(types::options e = types::nulltype);
                types       type() const { return type_; }
                SRSASN_CODE pack(bit_ref& bref) const;
                SRSASN_CODE unpack(cbit_ref& bref);
                void        to_json(json_writer& j) const;
                // getters
                fixed_bitstring<8>& two_one_type_i_single_panel_restrict()
                {
                  assert_choice_type("two-one-TypeI-SinglePanel-Restriction", type_.to_string(), "n1-n2");
                  return c.get<fixed_bitstring<8> >();
                }
                fixed_bitstring<64>& two_two_type_i_single_panel_restrict()
                {
                  assert_choice_type("two-two-TypeI-SinglePanel-Restriction", type_.to_string(), "n1-n2");
                  return c.get<fixed_bitstring<64> >();
                }
                fixed_bitstring<16>& four_one_type_i_single_panel_restrict()
                {
                  assert_choice_type("four-one-TypeI-SinglePanel-Restriction", type_.to_string(), "n1-n2");
                  return c.get<fixed_bitstring<16> >();
                }
                fixed_bitstring<96>& three_two_type_i_single_panel_restrict()
                {
                  assert_choice_type("three-two-TypeI-SinglePanel-Restriction", type_.to_string(), "n1-n2");
                  return c.get<fixed_bitstring<96> >();
                }
                fixed_bitstring<24>& six_one_type_i_single_panel_restrict()
                {
                  assert_choice_type("six-one-TypeI-SinglePanel-Restriction", type_.to_string(), "n1-n2");
                  return c.get<fixed_bitstring<24> >();
                }
                fixed_bitstring<128>& four_two_type_i_single_panel_restrict()
                {
                  assert_choice_type("four-two-TypeI-SinglePanel-Restriction", type_.to_string(), "n1-n2");
                  return c.get<fixed_bitstring<128> >();
                }
                fixed_bitstring<32>& eight_one_type_i_single_panel_restrict()
                {
                  assert_choice_type("eight-one-TypeI-SinglePanel-Restriction", type_.to_string(), "n1-n2");
                  return c.get<fixed_bitstring<32> >();
                }
                fixed_bitstring<192>& four_three_type_i_single_panel_restrict()
                {
                  assert_choice_type("four-three-TypeI-SinglePanel-Restriction", type_.to_string(), "n1-n2");
                  return c.get<fixed_bitstring<192> >();
                }
                fixed_bitstring<192>& six_two_type_i_single_panel_restrict()
                {
                  assert_choice_type("six-two-TypeI-SinglePanel-Restriction", type_.to_string(), "n1-n2");
                  return c.get<fixed_bitstring<192> >();
                }
                fixed_bitstring<48>& twelve_one_type_i_single_panel_restrict()
                {
                  assert_choice_type("twelve-one-TypeI-SinglePanel-Restriction", type_.to_string(), "n1-n2");
                  return c.get<fixed_bitstring<48> >();
                }
                fixed_bitstring<256>& four_four_type_i_single_panel_restrict()
                {
                  assert_choice_type("four-four-TypeI-SinglePanel-Restriction", type_.to_string(), "n1-n2");
                  return c.get<fixed_bitstring<256> >();
                }
                fixed_bitstring<256>& eight_two_type_i_single_panel_restrict()
                {
                  assert_choice_type("eight-two-TypeI-SinglePanel-Restriction", type_.to_string(), "n1-n2");
                  return c.get<fixed_bitstring<256> >();
                }
                fixed_bitstring<64>& sixteen_one_type_i_single_panel_restrict()
                {
                  assert_choice_type("sixteen-one-TypeI-SinglePanel-Restriction", type_.to_string(), "n1-n2");
                  return c.get<fixed_bitstring<64> >();
                }
                const fixed_bitstring<8>& two_one_type_i_single_panel_restrict() const
                {
                  assert_choice_type("two-one-TypeI-SinglePanel-Restriction", type_.to_string(), "n1-n2");
                  return c.get<fixed_bitstring<8> >();
                }
                const fixed_bitstring<64>& two_two_type_i_single_panel_restrict() const
                {
                  assert_choice_type("two-two-TypeI-SinglePanel-Restriction", type_.to_string(), "n1-n2");
                  return c.get<fixed_bitstring<64> >();
                }
                const fixed_bitstring<16>& four_one_type_i_single_panel_restrict() const
                {
                  assert_choice_type("four-one-TypeI-SinglePanel-Restriction", type_.to_string(), "n1-n2");
                  return c.get<fixed_bitstring<16> >();
                }
                const fixed_bitstring<96>& three_two_type_i_single_panel_restrict() const
                {
                  assert_choice_type("three-two-TypeI-SinglePanel-Restriction", type_.to_string(), "n1-n2");
                  return c.get<fixed_bitstring<96> >();
                }
                const fixed_bitstring<24>& six_one_type_i_single_panel_restrict() const
                {
                  assert_choice_type("six-one-TypeI-SinglePanel-Restriction", type_.to_string(), "n1-n2");
                  return c.get<fixed_bitstring<24> >();
                }
                const fixed_bitstring<128>& four_two_type_i_single_panel_restrict() const
                {
                  assert_choice_type("four-two-TypeI-SinglePanel-Restriction", type_.to_string(), "n1-n2");
                  return c.get<fixed_bitstring<128> >();
                }
                const fixed_bitstring<32>& eight_one_type_i_single_panel_restrict() const
                {
                  assert_choice_type("eight-one-TypeI-SinglePanel-Restriction", type_.to_string(), "n1-n2");
                  return c.get<fixed_bitstring<32> >();
                }
                const fixed_bitstring<192>& four_three_type_i_single_panel_restrict() const
                {
                  assert_choice_type("four-three-TypeI-SinglePanel-Restriction", type_.to_string(), "n1-n2");
                  return c.get<fixed_bitstring<192> >();
                }
                const fixed_bitstring<192>& six_two_type_i_single_panel_restrict() const
                {
                  assert_choice_type("six-two-TypeI-SinglePanel-Restriction", type_.to_string(), "n1-n2");
                  return c.get<fixed_bitstring<192> >();
                }
                const fixed_bitstring<48>& twelve_one_type_i_single_panel_restrict() const
                {
                  assert_choice_type("twelve-one-TypeI-SinglePanel-Restriction", type_.to_string(), "n1-n2");
                  return c.get<fixed_bitstring<48> >();
                }
                const fixed_bitstring<256>& four_four_type_i_single_panel_restrict() const
                {
                  assert_choice_type("four-four-TypeI-SinglePanel-Restriction", type_.to_string(), "n1-n2");
                  return c.get<fixed_bitstring<256> >();
                }
                const fixed_bitstring<256>& eight_two_type_i_single_panel_restrict() const
                {
                  assert_choice_type("eight-two-TypeI-SinglePanel-Restriction", type_.to_string(), "n1-n2");
                  return c.get<fixed_bitstring<256> >();
                }
                const fixed_bitstring<64>& sixteen_one_type_i_single_panel_restrict() const
                {
                  assert_choice_type("sixteen-one-TypeI-SinglePanel-Restriction", type_.to_string(), "n1-n2");
                  return c.get<fixed_bitstring<64> >();
                }
                fixed_bitstring<8>& set_two_one_type_i_single_panel_restrict()
                {
                  set(types::two_one_type_i_single_panel_restrict);
                  return c.get<fixed_bitstring<8> >();
                }
                fixed_bitstring<64>& set_two_two_type_i_single_panel_restrict()
                {
                  set(types::two_two_type_i_single_panel_restrict);
                  return c.get<fixed_bitstring<64> >();
                }
                fixed_bitstring<16>& set_four_one_type_i_single_panel_restrict()
                {
                  set(types::four_one_type_i_single_panel_restrict);
                  return c.get<fixed_bitstring<16> >();
                }
                fixed_bitstring<96>& set_three_two_type_i_single_panel_restrict()
                {
                  set(types::three_two_type_i_single_panel_restrict);
                  return c.get<fixed_bitstring<96> >();
                }
                fixed_bitstring<24>& set_six_one_type_i_single_panel_restrict()
                {
                  set(types::six_one_type_i_single_panel_restrict);
                  return c.get<fixed_bitstring<24> >();
                }
                fixed_bitstring<128>& set_four_two_type_i_single_panel_restrict()
                {
                  set(types::four_two_type_i_single_panel_restrict);
                  return c.get<fixed_bitstring<128> >();
                }
                fixed_bitstring<32>& set_eight_one_type_i_single_panel_restrict()
                {
                  set(types::eight_one_type_i_single_panel_restrict);
                  return c.get<fixed_bitstring<32> >();
                }
                fixed_bitstring<192>& set_four_three_type_i_single_panel_restrict()
                {
                  set(types::four_three_type_i_single_panel_restrict);
                  return c.get<fixed_bitstring<192> >();
                }
                fixed_bitstring<192>& set_six_two_type_i_single_panel_restrict()
                {
                  set(types::six_two_type_i_single_panel_restrict);
                  return c.get<fixed_bitstring<192> >();
                }
                fixed_bitstring<48>& set_twelve_one_type_i_single_panel_restrict()
                {
                  set(types::twelve_one_type_i_single_panel_restrict);
                  return c.get<fixed_bitstring<48> >();
                }
                fixed_bitstring<256>& set_four_four_type_i_single_panel_restrict()
                {
                  set(types::four_four_type_i_single_panel_restrict);
                  return c.get<fixed_bitstring<256> >();
                }
                fixed_bitstring<256>& set_eight_two_type_i_single_panel_restrict()
                {
                  set(types::eight_two_type_i_single_panel_restrict);
                  return c.get<fixed_bitstring<256> >();
                }
                fixed_bitstring<64>& set_sixteen_one_type_i_single_panel_restrict()
                {
                  set(types::sixteen_one_type_i_single_panel_restrict);
                  return c.get<fixed_bitstring<64> >();
                }

              private:
                types                                  type_;
                choice_buffer_t<fixed_bitstring<256> > c;

                void destroy_();
              };

              // member variables
              bool                type_i_single_panel_codebook_subset_restrict_i2_present = false;
              n1_n2_c_            n1_n2;
              fixed_bitstring<16> type_i_single_panel_codebook_subset_restrict_i2;
            };
            struct types_opts {
              enum options { two, more_than_two, nulltype } value;
              typedef uint8_t number_type;

              std::string to_string() const;
              uint8_t     to_number() const;
            };
            typedef enumerated<types_opts> types;

            // choice methods
            nr_of_ant_ports_c_() = default;
            nr_of_ant_ports_c_(const nr_of_ant_ports_c_& other);
            nr_of_ant_ports_c_& operator=(const nr_of_ant_ports_c_& other);
            ~nr_of_ant_ports_c_() { destroy_(); }
            void        set(types::options e = types::nulltype);
            types       type() const { return type_; }
            SRSASN_CODE pack(bit_ref& bref) const;
            SRSASN_CODE unpack(cbit_ref& bref);
            void        to_json(json_writer& j) const;
            // getters
            two_s_& two()
            {
              assert_choice_type("two", type_.to_string(), "nrOfAntennaPorts");
              return c.get<two_s_>();
            }
            more_than_two_s_& more_than_two()
            {
              assert_choice_type("moreThanTwo", type_.to_string(), "nrOfAntennaPorts");
              return c.get<more_than_two_s_>();
            }
            const two_s_& two() const
            {
              assert_choice_type("two", type_.to_string(), "nrOfAntennaPorts");
              return c.get<two_s_>();
            }
            const more_than_two_s_& more_than_two() const
            {
              assert_choice_type("moreThanTwo", type_.to_string(), "nrOfAntennaPorts");
              return c.get<more_than_two_s_>();
            }
            two_s_& set_two()
            {
              set(types::two);
              return c.get<two_s_>();
            }
            more_than_two_s_& set_more_than_two()
            {
              set(types::more_than_two);
              return c.get<more_than_two_s_>();
            }

          private:
            types                                     type_;
            choice_buffer_t<more_than_two_s_, two_s_> c;

            void destroy_();
          };

          // member variables
          nr_of_ant_ports_c_ nr_of_ant_ports;
          fixed_bitstring<8> type_i_single_panel_ri_restrict;
        };
        struct type_i_multi_panel_s_ {
          struct ng_n1_n2_c_ {
            struct types_opts {
              enum options {
                two_two_one_type_i_multi_panel_restrict,
                two_four_one_type_i_multi_panel_restrict,
                four_two_one_type_i_multi_panel_restrict,
                two_two_two_type_i_multi_panel_restrict,
                two_eight_one_type_i_multi_panel_restrict,
                four_four_one_type_i_multi_panel_restrict,
                two_four_two_type_i_multi_panel_restrict,
                four_two_two_type_i_multi_panel_restrict,
                nulltype
              } value;

              std::string to_string() const;
            };
            typedef enumerated<types_opts> types;

            // choice methods
            ng_n1_n2_c_() = default;
            ng_n1_n2_c_(const ng_n1_n2_c_& other);
            ng_n1_n2_c_& operator=(const ng_n1_n2_c_& other);
            ~ng_n1_n2_c_() { destroy_(); }
            void        set(types::options e = types::nulltype);
            types       type() const { return type_; }
            SRSASN_CODE pack(bit_ref& bref) const;
            SRSASN_CODE unpack(cbit_ref& bref);
            void        to_json(json_writer& j) const;
            // getters
            fixed_bitstring<8>& two_two_one_type_i_multi_panel_restrict()
            {
              assert_choice_type("two-two-one-TypeI-MultiPanel-Restriction", type_.to_string(), "ng-n1-n2");
              return c.get<fixed_bitstring<8> >();
            }
            fixed_bitstring<16>& two_four_one_type_i_multi_panel_restrict()
            {
              assert_choice_type("two-four-one-TypeI-MultiPanel-Restriction", type_.to_string(), "ng-n1-n2");
              return c.get<fixed_bitstring<16> >();
            }
            fixed_bitstring<8>& four_two_one_type_i_multi_panel_restrict()
            {
              assert_choice_type("four-two-one-TypeI-MultiPanel-Restriction", type_.to_string(), "ng-n1-n2");
              return c.get<fixed_bitstring<8> >();
            }
            fixed_bitstring<64>& two_two_two_type_i_multi_panel_restrict()
            {
              assert_choice_type("two-two-two-TypeI-MultiPanel-Restriction", type_.to_string(), "ng-n1-n2");
              return c.get<fixed_bitstring<64> >();
            }
            fixed_bitstring<32>& two_eight_one_type_i_multi_panel_restrict()
            {
              assert_choice_type("two-eight-one-TypeI-MultiPanel-Restriction", type_.to_string(), "ng-n1-n2");
              return c.get<fixed_bitstring<32> >();
            }
            fixed_bitstring<16>& four_four_one_type_i_multi_panel_restrict()
            {
              assert_choice_type("four-four-one-TypeI-MultiPanel-Restriction", type_.to_string(), "ng-n1-n2");
              return c.get<fixed_bitstring<16> >();
            }
            fixed_bitstring<128>& two_four_two_type_i_multi_panel_restrict()
            {
              assert_choice_type("two-four-two-TypeI-MultiPanel-Restriction", type_.to_string(), "ng-n1-n2");
              return c.get<fixed_bitstring<128> >();
            }
            fixed_bitstring<64>& four_two_two_type_i_multi_panel_restrict()
            {
              assert_choice_type("four-two-two-TypeI-MultiPanel-Restriction", type_.to_string(), "ng-n1-n2");
              return c.get<fixed_bitstring<64> >();
            }
            const fixed_bitstring<8>& two_two_one_type_i_multi_panel_restrict() const
            {
              assert_choice_type("two-two-one-TypeI-MultiPanel-Restriction", type_.to_string(), "ng-n1-n2");
              return c.get<fixed_bitstring<8> >();
            }
            const fixed_bitstring<16>& two_four_one_type_i_multi_panel_restrict() const
            {
              assert_choice_type("two-four-one-TypeI-MultiPanel-Restriction", type_.to_string(), "ng-n1-n2");
              return c.get<fixed_bitstring<16> >();
            }
            const fixed_bitstring<8>& four_two_one_type_i_multi_panel_restrict() const
            {
              assert_choice_type("four-two-one-TypeI-MultiPanel-Restriction", type_.to_string(), "ng-n1-n2");
              return c.get<fixed_bitstring<8> >();
            }
            const fixed_bitstring<64>& two_two_two_type_i_multi_panel_restrict() const
            {
              assert_choice_type("two-two-two-TypeI-MultiPanel-Restriction", type_.to_string(), "ng-n1-n2");
              return c.get<fixed_bitstring<64> >();
            }
            const fixed_bitstring<32>& two_eight_one_type_i_multi_panel_restrict() const
            {
              assert_choice_type("two-eight-one-TypeI-MultiPanel-Restriction", type_.to_string(), "ng-n1-n2");
              return c.get<fixed_bitstring<32> >();
            }
            const fixed_bitstring<16>& four_four_one_type_i_multi_panel_restrict() const
            {
              assert_choice_type("four-four-one-TypeI-MultiPanel-Restriction", type_.to_string(), "ng-n1-n2");
              return c.get<fixed_bitstring<16> >();
            }
            const fixed_bitstring<128>& two_four_two_type_i_multi_panel_restrict() const
            {
              assert_choice_type("two-four-two-TypeI-MultiPanel-Restriction", type_.to_string(), "ng-n1-n2");
              return c.get<fixed_bitstring<128> >();
            }
            const fixed_bitstring<64>& four_two_two_type_i_multi_panel_restrict() const
            {
              assert_choice_type("four-two-two-TypeI-MultiPanel-Restriction", type_.to_string(), "ng-n1-n2");
              return c.get<fixed_bitstring<64> >();
            }
            fixed_bitstring<8>& set_two_two_one_type_i_multi_panel_restrict()
            {
              set(types::two_two_one_type_i_multi_panel_restrict);
              return c.get<fixed_bitstring<8> >();
            }
            fixed_bitstring<16>& set_two_four_one_type_i_multi_panel_restrict()
            {
              set(types::two_four_one_type_i_multi_panel_restrict);
              return c.get<fixed_bitstring<16> >();
            }
            fixed_bitstring<8>& set_four_two_one_type_i_multi_panel_restrict()
            {
              set(types::four_two_one_type_i_multi_panel_restrict);
              return c.get<fixed_bitstring<8> >();
            }
            fixed_bitstring<64>& set_two_two_two_type_i_multi_panel_restrict()
            {
              set(types::two_two_two_type_i_multi_panel_restrict);
              return c.get<fixed_bitstring<64> >();
            }
            fixed_bitstring<32>& set_two_eight_one_type_i_multi_panel_restrict()
            {
              set(types::two_eight_one_type_i_multi_panel_restrict);
              return c.get<fixed_bitstring<32> >();
            }
            fixed_bitstring<16>& set_four_four_one_type_i_multi_panel_restrict()
            {
              set(types::four_four_one_type_i_multi_panel_restrict);
              return c.get<fixed_bitstring<16> >();
            }
            fixed_bitstring<128>& set_two_four_two_type_i_multi_panel_restrict()
            {
              set(types::two_four_two_type_i_multi_panel_restrict);
              return c.get<fixed_bitstring<128> >();
            }
            fixed_bitstring<64>& set_four_two_two_type_i_multi_panel_restrict()
            {
              set(types::four_two_two_type_i_multi_panel_restrict);
              return c.get<fixed_bitstring<64> >();
            }

          private:
            types                                  type_;
            choice_buffer_t<fixed_bitstring<128> > c;

            void destroy_();
          };

          // member variables
          ng_n1_n2_c_        ng_n1_n2;
          fixed_bitstring<4> ri_restrict;
        };
        struct types_opts {
          enum options { type_i_single_panel, type_i_multi_panel, nulltype } value;

          std::string to_string() const;
        };
        typedef enumerated<types_opts> types;

        // choice methods
        sub_type_c_() = default;
        sub_type_c_(const sub_type_c_& other);
        sub_type_c_& operator=(const sub_type_c_& other);
        ~sub_type_c_() { destroy_(); }
        void        set(types::options e = types::nulltype);
        types       type() const { return type_; }
        SRSASN_CODE pack(bit_ref& bref) const;
        SRSASN_CODE unpack(cbit_ref& bref);
        void        to_json(json_writer& j) const;
        // getters
        type_i_single_panel_s_& type_i_single_panel()
        {
          assert_choice_type("typeI-SinglePanel", type_.to_string(), "subType");
          return c.get<type_i_single_panel_s_>();
        }
        type_i_multi_panel_s_& type_i_multi_panel()
        {
          assert_choice_type("typeI-MultiPanel", type_.to_string(), "subType");
          return c.get<type_i_multi_panel_s_>();
        }
        const type_i_single_panel_s_& type_i_single_panel() const
        {
          assert_choice_type("typeI-SinglePanel", type_.to_string(), "subType");
          return c.get<type_i_single_panel_s_>();
        }
        const type_i_multi_panel_s_& type_i_multi_panel() const
        {
          assert_choice_type("typeI-MultiPanel", type_.to_string(), "subType");
          return c.get<type_i_multi_panel_s_>();
        }
        type_i_single_panel_s_& set_type_i_single_panel()
        {
          set(types::type_i_single_panel);
          return c.get<type_i_single_panel_s_>();
        }
        type_i_multi_panel_s_& set_type_i_multi_panel()
        {
          set(types::type_i_multi_panel);
          return c.get<type_i_multi_panel_s_>();
        }

      private:
        types                                                          type_;
        choice_buffer_t<type_i_multi_panel_s_, type_i_single_panel_s_> c;

        void destroy_();
      };

      // member variables
      sub_type_c_ sub_type;
      uint8_t     codebook_mode = 1;
    };
    struct type2_s_ {
      struct sub_type_c_ {
        struct type_ii_s_ {
          struct n1_n2_codebook_subset_restrict_c_ {
            struct types_opts {
              enum options {
                two_one,
                two_two,
                four_one,
                three_two,
                six_one,
                four_two,
                eight_one,
                four_three,
                six_two,
                twelve_one,
                four_four,
                eight_two,
                sixteen_one,
                nulltype
              } value;

              std::string to_string() const;
            };
            typedef enumerated<types_opts> types;

            // choice methods
            n1_n2_codebook_subset_restrict_c_() = default;
            n1_n2_codebook_subset_restrict_c_(const n1_n2_codebook_subset_restrict_c_& other);
            n1_n2_codebook_subset_restrict_c_& operator=(const n1_n2_codebook_subset_restrict_c_& other);
            ~n1_n2_codebook_subset_restrict_c_() { destroy_(); }
            void        set(types::options e = types::nulltype);
            types       type() const { return type_; }
            SRSASN_CODE pack(bit_ref& bref) const;
            SRSASN_CODE unpack(cbit_ref& bref);
            void        to_json(json_writer& j) const;
            // getters
            fixed_bitstring<16>& two_one()
            {
              assert_choice_type("two-one", type_.to_string(), "n1-n2-codebookSubsetRestriction");
              return c.get<fixed_bitstring<16> >();
            }
            fixed_bitstring<43>& two_two()
            {
              assert_choice_type("two-two", type_.to_string(), "n1-n2-codebookSubsetRestriction");
              return c.get<fixed_bitstring<43> >();
            }
            fixed_bitstring<32>& four_one()
            {
              assert_choice_type("four-one", type_.to_string(), "n1-n2-codebookSubsetRestriction");
              return c.get<fixed_bitstring<32> >();
            }
            fixed_bitstring<59>& three_two()
            {
              assert_choice_type("three-two", type_.to_string(), "n1-n2-codebookSubsetRestriction");
              return c.get<fixed_bitstring<59> >();
            }
            fixed_bitstring<48>& six_one()
            {
              assert_choice_type("six-one", type_.to_string(), "n1-n2-codebookSubsetRestriction");
              return c.get<fixed_bitstring<48> >();
            }
            fixed_bitstring<75>& four_two()
            {
              assert_choice_type("four-two", type_.to_string(), "n1-n2-codebookSubsetRestriction");
              return c.get<fixed_bitstring<75> >();
            }
            fixed_bitstring<64>& eight_one()
            {
              assert_choice_type("eight-one", type_.to_string(), "n1-n2-codebookSubsetRestriction");
              return c.get<fixed_bitstring<64> >();
            }
            fixed_bitstring<107>& four_three()
            {
              assert_choice_type("four-three", type_.to_string(), "n1-n2-codebookSubsetRestriction");
              return c.get<fixed_bitstring<107> >();
            }
            fixed_bitstring<107>& six_two()
            {
              assert_choice_type("six-two", type_.to_string(), "n1-n2-codebookSubsetRestriction");
              return c.get<fixed_bitstring<107> >();
            }
            fixed_bitstring<96>& twelve_one()
            {
              assert_choice_type("twelve-one", type_.to_string(), "n1-n2-codebookSubsetRestriction");
              return c.get<fixed_bitstring<96> >();
            }
            fixed_bitstring<139>& four_four()
            {
              assert_choice_type("four-four", type_.to_string(), "n1-n2-codebookSubsetRestriction");
              return c.get<fixed_bitstring<139> >();
            }
            fixed_bitstring<139>& eight_two()
            {
              assert_choice_type("eight-two", type_.to_string(), "n1-n2-codebookSubsetRestriction");
              return c.get<fixed_bitstring<139> >();
            }
            fixed_bitstring<128>& sixteen_one()
            {
              assert_choice_type("sixteen-one", type_.to_string(), "n1-n2-codebookSubsetRestriction");
              return c.get<fixed_bitstring<128> >();
            }
            const fixed_bitstring<16>& two_one() const
            {
              assert_choice_type("two-one", type_.to_string(), "n1-n2-codebookSubsetRestriction");
              return c.get<fixed_bitstring<16> >();
            }
            const fixed_bitstring<43>& two_two() const
            {
              assert_choice_type("two-two", type_.to_string(), "n1-n2-codebookSubsetRestriction");
              return c.get<fixed_bitstring<43> >();
            }
            const fixed_bitstring<32>& four_one() const
            {
              assert_choice_type("four-one", type_.to_string(), "n1-n2-codebookSubsetRestriction");
              return c.get<fixed_bitstring<32> >();
            }
            const fixed_bitstring<59>& three_two() const
            {
              assert_choice_type("three-two", type_.to_string(), "n1-n2-codebookSubsetRestriction");
              return c.get<fixed_bitstring<59> >();
            }
            const fixed_bitstring<48>& six_one() const
            {
              assert_choice_type("six-one", type_.to_string(), "n1-n2-codebookSubsetRestriction");
              return c.get<fixed_bitstring<48> >();
            }
            const fixed_bitstring<75>& four_two() const
            {
              assert_choice_type("four-two", type_.to_string(), "n1-n2-codebookSubsetRestriction");
              return c.get<fixed_bitstring<75> >();
            }
            const fixed_bitstring<64>& eight_one() const
            {
              assert_choice_type("eight-one", type_.to_string(), "n1-n2-codebookSubsetRestriction");
              return c.get<fixed_bitstring<64> >();
            }
            const fixed_bitstring<107>& four_three() const
            {
              assert_choice_type("four-three", type_.to_string(), "n1-n2-codebookSubsetRestriction");
              return c.get<fixed_bitstring<107> >();
            }
            const fixed_bitstring<107>& six_two() const
            {
              assert_choice_type("six-two", type_.to_string(), "n1-n2-codebookSubsetRestriction");
              return c.get<fixed_bitstring<107> >();
            }
            const fixed_bitstring<96>& twelve_one() const
            {
              assert_choice_type("twelve-one", type_.to_string(), "n1-n2-codebookSubsetRestriction");
              return c.get<fixed_bitstring<96> >();
            }
            const fixed_bitstring<139>& four_four() const
            {
              assert_choice_type("four-four", type_.to_string(), "n1-n2-codebookSubsetRestriction");
              return c.get<fixed_bitstring<139> >();
            }
            const fixed_bitstring<139>& eight_two() const
            {
              assert_choice_type("eight-two", type_.to_string(), "n1-n2-codebookSubsetRestriction");
              return c.get<fixed_bitstring<139> >();
            }
            const fixed_bitstring<128>& sixteen_one() const
            {
              assert_choice_type("sixteen-one", type_.to_string(), "n1-n2-codebookSubsetRestriction");
              return c.get<fixed_bitstring<128> >();
            }
            fixed_bitstring<16>& set_two_one()
            {
              set(types::two_one);
              return c.get<fixed_bitstring<16> >();
            }
            fixed_bitstring<43>& set_two_two()
            {
              set(types::two_two);
              return c.get<fixed_bitstring<43> >();
            }
            fixed_bitstring<32>& set_four_one()
            {
              set(types::four_one);
              return c.get<fixed_bitstring<32> >();
            }
            fixed_bitstring<59>& set_three_two()
            {
              set(types::three_two);
              return c.get<fixed_bitstring<59> >();
            }
            fixed_bitstring<48>& set_six_one()
            {
              set(types::six_one);
              return c.get<fixed_bitstring<48> >();
            }
            fixed_bitstring<75>& set_four_two()
            {
              set(types::four_two);
              return c.get<fixed_bitstring<75> >();
            }
            fixed_bitstring<64>& set_eight_one()
            {
              set(types::eight_one);
              return c.get<fixed_bitstring<64> >();
            }
            fixed_bitstring<107>& set_four_three()
            {
              set(types::four_three);
              return c.get<fixed_bitstring<107> >();
            }
            fixed_bitstring<107>& set_six_two()
            {
              set(types::six_two);
              return c.get<fixed_bitstring<107> >();
            }
            fixed_bitstring<96>& set_twelve_one()
            {
              set(types::twelve_one);
              return c.get<fixed_bitstring<96> >();
            }
            fixed_bitstring<139>& set_four_four()
            {
              set(types::four_four);
              return c.get<fixed_bitstring<139> >();
            }
            fixed_bitstring<139>& set_eight_two()
            {
              set(types::eight_two);
              return c.get<fixed_bitstring<139> >();
            }
            fixed_bitstring<128>& set_sixteen_one()
            {
              set(types::sixteen_one);
              return c.get<fixed_bitstring<128> >();
            }

          private:
            types                                  type_;
            choice_buffer_t<fixed_bitstring<139> > c;

            void destroy_();
          };

          // member variables
          n1_n2_codebook_subset_restrict_c_ n1_n2_codebook_subset_restrict;
          fixed_bitstring<2>                type_ii_ri_restrict;
        };
        struct type_ii_port_sel_s_ {
          struct port_sel_sampling_size_opts {
            enum options { n1, n2, n3, n4, nulltype } value;
            typedef uint8_t number_type;

            std::string to_string() const;
            uint8_t     to_number() const;
          };
          typedef enumerated<port_sel_sampling_size_opts> port_sel_sampling_size_e_;

          // member variables
          bool                      port_sel_sampling_size_present = false;
          port_sel_sampling_size_e_ port_sel_sampling_size;
          fixed_bitstring<2>        type_ii_port_sel_ri_restrict;
        };
        struct types_opts {
          enum options { type_ii, type_ii_port_sel, nulltype } value;

          std::string to_string() const;
        };
        typedef enumerated<types_opts> types;

        // choice methods
        sub_type_c_() = default;
        sub_type_c_(const sub_type_c_& other);
        sub_type_c_& operator=(const sub_type_c_& other);
        ~sub_type_c_() { destroy_(); }
        void        set(types::options e = types::nulltype);
        types       type() const { return type_; }
        SRSASN_CODE pack(bit_ref& bref) const;
        SRSASN_CODE unpack(cbit_ref& bref);
        void        to_json(json_writer& j) const;
        // getters
        type_ii_s_& type_ii()
        {
          assert_choice_type("typeII", type_.to_string(), "subType");
          return c.get<type_ii_s_>();
        }
        type_ii_port_sel_s_& type_ii_port_sel()
        {
          assert_choice_type("typeII-PortSelection", type_.to_string(), "subType");
          return c.get<type_ii_port_sel_s_>();
        }
        const type_ii_s_& type_ii() const
        {
          assert_choice_type("typeII", type_.to_string(), "subType");
          return c.get<type_ii_s_>();
        }
        const type_ii_port_sel_s_& type_ii_port_sel() const
        {
          assert_choice_type("typeII-PortSelection", type_.to_string(), "subType");
          return c.get<type_ii_port_sel_s_>();
        }
        type_ii_s_& set_type_ii()
        {
          set(types::type_ii);
          return c.get<type_ii_s_>();
        }
        type_ii_port_sel_s_& set_type_ii_port_sel()
        {
          set(types::type_ii_port_sel);
          return c.get<type_ii_port_sel_s_>();
        }

      private:
        types                                            type_;
        choice_buffer_t<type_ii_port_sel_s_, type_ii_s_> c;

        void destroy_();
      };
      struct phase_alphabet_size_opts {
        enum options { n4, n8, nulltype } value;
        typedef uint8_t number_type;

        std::string to_string() const;
        uint8_t     to_number() const;
      };
      typedef enumerated<phase_alphabet_size_opts> phase_alphabet_size_e_;
      struct nof_beams_opts {
        enum options { two, three, four, nulltype } value;
        typedef uint8_t number_type;

        std::string to_string() const;
        uint8_t     to_number() const;
      };
      typedef enumerated<nof_beams_opts> nof_beams_e_;

      // member variables
      sub_type_c_            sub_type;
      phase_alphabet_size_e_ phase_alphabet_size;
      bool                   subband_amplitude = false;
      nof_beams_e_           nof_beams;
    };
    struct types_opts {
      enum options { type1, type2, nulltype } value;
      typedef uint8_t number_type;

      std::string to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    codebook_type_c_() = default;
    codebook_type_c_(const codebook_type_c_& other);
    codebook_type_c_& operator=(const codebook_type_c_& other);
    ~codebook_type_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    type1_s_& type1()
    {
      assert_choice_type("type1", type_.to_string(), "codebookType");
      return c.get<type1_s_>();
    }
    type2_s_& type2()
    {
      assert_choice_type("type2", type_.to_string(), "codebookType");
      return c.get<type2_s_>();
    }
    const type1_s_& type1() const
    {
      assert_choice_type("type1", type_.to_string(), "codebookType");
      return c.get<type1_s_>();
    }
    const type2_s_& type2() const
    {
      assert_choice_type("type2", type_.to_string(), "codebookType");
      return c.get<type2_s_>();
    }
    type1_s_& set_type1()
    {
      set(types::type1);
      return c.get<type1_s_>();
    }
    type2_s_& set_type2()
    {
      set(types::type2);
      return c.get<type2_s_>();
    }

  private:
    types                               type_;
    choice_buffer_t<type1_s_, type2_s_> c;

    void destroy_();
  };

  // member variables
  codebook_type_c_ codebook_type;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// EUTRA-MBSFN-SubframeConfigList ::= SEQUENCE (SIZE (1..8)) OF EUTRA-MBSFN-SubframeConfig
using eutra_mbsfn_sf_cfg_list_l = dyn_array<eutra_mbsfn_sf_cfg_s>;

// FrequencyInfoDL ::= SEQUENCE
struct freq_info_dl_s {
  using scs_specific_carrier_list_l_ = dyn_array<scs_specific_carrier_s>;

  // member variables
  bool                         ext                       = false;
  bool                         absolute_freq_ssb_present = false;
  uint32_t                     absolute_freq_ssb         = 0;
  multi_freq_band_list_nr_l    freq_band_list;
  uint32_t                     absolute_freq_point_a = 0;
  scs_specific_carrier_list_l_ scs_specific_carrier_list;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// FrequencyInfoUL ::= SEQUENCE
struct freq_info_ul_s {
  using scs_specific_carrier_list_l_ = dyn_array<scs_specific_carrier_s>;

  // member variables
  bool                         ext                           = false;
  bool                         freq_band_list_present        = false;
  bool                         absolute_freq_point_a_present = false;
  bool                         add_spec_emission_present     = false;
  bool                         p_max_present                 = false;
  bool                         freq_shift7p5khz_present      = false;
  multi_freq_band_list_nr_l    freq_band_list;
  uint32_t                     absolute_freq_point_a = 0;
  scs_specific_carrier_list_l_ scs_specific_carrier_list;
  uint8_t                      add_spec_emission = 0;
  int8_t                       p_max             = -30;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDCCH-Config ::= SEQUENCE
struct pdcch_cfg_s {
  using ctrl_res_set_to_add_mod_list_l_  = dyn_array<ctrl_res_set_s>;
  using ctrl_res_set_to_release_list_l_  = bounded_array<uint8_t, 3>;
  using search_spaces_to_add_mod_list_l_ = dyn_array<search_space_s>;
  using search_spaces_to_release_list_l_ = bounded_array<uint8_t, 10>;

  // member variables
  bool                                 ext                                   = false;
  bool                                 ctrl_res_set_to_add_mod_list_present  = false;
  bool                                 ctrl_res_set_to_release_list_present  = false;
  bool                                 search_spaces_to_add_mod_list_present = false;
  bool                                 search_spaces_to_release_list_present = false;
  bool                                 dl_preemption_present                 = false;
  bool                                 tpc_pusch_present                     = false;
  bool                                 tpc_pucch_present                     = false;
  bool                                 tpc_srs_present                       = false;
  ctrl_res_set_to_add_mod_list_l_      ctrl_res_set_to_add_mod_list;
  ctrl_res_set_to_release_list_l_      ctrl_res_set_to_release_list;
  search_spaces_to_add_mod_list_l_     search_spaces_to_add_mod_list;
  search_spaces_to_release_list_l_     search_spaces_to_release_list;
  setup_release_c<dl_preemption_s>     dl_preemption;
  setup_release_c<pusch_tpc_cmd_cfg_s> tpc_pusch;
  setup_release_c<pucch_tpc_cmd_cfg_s> tpc_pucch;
  setup_release_c<srs_tpc_cmd_cfg_s>   tpc_srs;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDSCH-Config ::= SEQUENCE
struct pdsch_cfg_s {
  using tci_states_to_add_mod_list_l_ = dyn_array<tci_state_s>;
  using tci_states_to_release_list_l_ = dyn_array<uint8_t>;
  struct vrb_to_prb_interleaver_opts {
    enum options { n2, n4, nulltype } value;
    typedef uint8_t number_type;

    std::string to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<vrb_to_prb_interleaver_opts> vrb_to_prb_interleaver_e_;
  struct res_alloc_opts {
    enum options { res_alloc_type0, res_alloc_type1, dynamic_switch, nulltype } value;
    typedef uint8_t number_type;

    std::string to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<res_alloc_opts> res_alloc_e_;
  struct pdsch_aggregation_factor_opts {
    enum options { n2, n4, n8, nulltype } value;
    typedef uint8_t number_type;

    std::string to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<pdsch_aggregation_factor_opts> pdsch_aggregation_factor_e_;
  using rate_match_pattern_to_add_mod_list_l_ = dyn_array<rate_match_pattern_s>;
  using rate_match_pattern_to_release_list_l_ = bounded_array<uint8_t, 4>;
  struct rbg_size_opts {
    enum options { cfg1, cfg2, nulltype } value;
    typedef uint8_t number_type;

    std::string to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<rbg_size_opts> rbg_size_e_;
  struct mcs_table_opts {
    enum options { qam256, qam64_low_se, nulltype } value;
    typedef uint16_t number_type;

    std::string to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<mcs_table_opts> mcs_table_e_;
  struct max_nrof_code_words_sched_by_dci_opts {
    enum options { n1, n2, nulltype } value;
    typedef uint8_t number_type;

    std::string to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<max_nrof_code_words_sched_by_dci_opts> max_nrof_code_words_sched_by_dci_e_;
  struct prb_bundling_type_c_ {
    struct static_bundling_s_ {
      struct bundle_size_opts {
        enum options { n4, wideband, nulltype } value;
        typedef uint8_t number_type;

        std::string to_string() const;
        uint8_t     to_number() const;
      };
      typedef enumerated<bundle_size_opts> bundle_size_e_;

      // member variables
      bool           bundle_size_present = false;
      bundle_size_e_ bundle_size;
    };
    struct dynamic_bundling_s_ {
      struct bundle_size_set1_opts {
        enum options { n4, wideband, n2_wideband, n4_wideband, nulltype } value;

        std::string to_string() const;
      };
      typedef enumerated<bundle_size_set1_opts> bundle_size_set1_e_;
      struct bundle_size_set2_opts {
        enum options { n4, wideband, nulltype } value;
        typedef uint8_t number_type;

        std::string to_string() const;
        uint8_t     to_number() const;
      };
      typedef enumerated<bundle_size_set2_opts> bundle_size_set2_e_;

      // member variables
      bool                bundle_size_set1_present = false;
      bool                bundle_size_set2_present = false;
      bundle_size_set1_e_ bundle_size_set1;
      bundle_size_set2_e_ bundle_size_set2;
    };
    struct types_opts {
      enum options { static_bundling, dynamic_bundling, nulltype } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    prb_bundling_type_c_() = default;
    prb_bundling_type_c_(const prb_bundling_type_c_& other);
    prb_bundling_type_c_& operator=(const prb_bundling_type_c_& other);
    ~prb_bundling_type_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    static_bundling_s_& static_bundling()
    {
      assert_choice_type("staticBundling", type_.to_string(), "prb-BundlingType");
      return c.get<static_bundling_s_>();
    }
    dynamic_bundling_s_& dynamic_bundling()
    {
      assert_choice_type("dynamicBundling", type_.to_string(), "prb-BundlingType");
      return c.get<dynamic_bundling_s_>();
    }
    const static_bundling_s_& static_bundling() const
    {
      assert_choice_type("staticBundling", type_.to_string(), "prb-BundlingType");
      return c.get<static_bundling_s_>();
    }
    const dynamic_bundling_s_& dynamic_bundling() const
    {
      assert_choice_type("dynamicBundling", type_.to_string(), "prb-BundlingType");
      return c.get<dynamic_bundling_s_>();
    }
    static_bundling_s_& set_static_bundling()
    {
      set(types::static_bundling);
      return c.get<static_bundling_s_>();
    }
    dynamic_bundling_s_& set_dynamic_bundling()
    {
      set(types::dynamic_bundling);
      return c.get<dynamic_bundling_s_>();
    }

  private:
    types                                                    type_;
    choice_buffer_t<dynamic_bundling_s_, static_bundling_s_> c;

    void destroy_();
  };
  using zp_csi_rs_res_to_add_mod_list_l_                = dyn_array<zp_csi_rs_res_s>;
  using zp_csi_rs_res_to_release_list_l_                = bounded_array<uint8_t, 32>;
  using aperiodic_zp_csi_rs_res_sets_to_add_mod_list_l_ = dyn_array<zp_csi_rs_res_set_s>;
  using aperiodic_zp_csi_rs_res_sets_to_release_list_l_ = bounded_array<uint8_t, 16>;
  using sp_zp_csi_rs_res_sets_to_add_mod_list_l_        = dyn_array<zp_csi_rs_res_set_s>;
  using sp_zp_csi_rs_res_sets_to_release_list_l_        = bounded_array<uint8_t, 16>;

  // member variables
  bool                           ext                                                  = false;
  bool                           data_scrambling_id_pdsch_present                     = false;
  bool                           dmrs_dl_for_pdsch_map_type_a_present                 = false;
  bool                           dmrs_dl_for_pdsch_map_type_b_present                 = false;
  bool                           tci_states_to_add_mod_list_present                   = false;
  bool                           tci_states_to_release_list_present                   = false;
  bool                           vrb_to_prb_interleaver_present                       = false;
  bool                           pdsch_time_domain_alloc_list_present                 = false;
  bool                           pdsch_aggregation_factor_present                     = false;
  bool                           rate_match_pattern_to_add_mod_list_present           = false;
  bool                           rate_match_pattern_to_release_list_present           = false;
  bool                           rate_match_pattern_group1_present                    = false;
  bool                           rate_match_pattern_group2_present                    = false;
  bool                           mcs_table_present                                    = false;
  bool                           max_nrof_code_words_sched_by_dci_present             = false;
  bool                           zp_csi_rs_res_to_add_mod_list_present                = false;
  bool                           zp_csi_rs_res_to_release_list_present                = false;
  bool                           aperiodic_zp_csi_rs_res_sets_to_add_mod_list_present = false;
  bool                           aperiodic_zp_csi_rs_res_sets_to_release_list_present = false;
  bool                           sp_zp_csi_rs_res_sets_to_add_mod_list_present        = false;
  bool                           sp_zp_csi_rs_res_sets_to_release_list_present        = false;
  bool                           p_zp_csi_rs_res_set_present                          = false;
  uint16_t                       data_scrambling_id_pdsch                             = 0;
  setup_release_c<dmrs_dl_cfg_s> dmrs_dl_for_pdsch_map_type_a;
  setup_release_c<dmrs_dl_cfg_s> dmrs_dl_for_pdsch_map_type_b;
  tci_states_to_add_mod_list_l_  tci_states_to_add_mod_list;
  tci_states_to_release_list_l_  tci_states_to_release_list;
  vrb_to_prb_interleaver_e_      vrb_to_prb_interleaver;
  res_alloc_e_                   res_alloc;
  setup_release_c<dyn_seq_of<pdsch_time_domain_res_alloc_s, 1, 16> > pdsch_time_domain_alloc_list;
  pdsch_aggregation_factor_e_                                        pdsch_aggregation_factor;
  rate_match_pattern_to_add_mod_list_l_                              rate_match_pattern_to_add_mod_list;
  rate_match_pattern_to_release_list_l_                              rate_match_pattern_to_release_list;
  rate_match_pattern_group_l                                         rate_match_pattern_group1;
  rate_match_pattern_group_l                                         rate_match_pattern_group2;
  rbg_size_e_                                                        rbg_size;
  mcs_table_e_                                                       mcs_table;
  max_nrof_code_words_sched_by_dci_e_                                max_nrof_code_words_sched_by_dci;
  prb_bundling_type_c_                                               prb_bundling_type;
  zp_csi_rs_res_to_add_mod_list_l_                                   zp_csi_rs_res_to_add_mod_list;
  zp_csi_rs_res_to_release_list_l_                                   zp_csi_rs_res_to_release_list;
  aperiodic_zp_csi_rs_res_sets_to_add_mod_list_l_                    aperiodic_zp_csi_rs_res_sets_to_add_mod_list;
  aperiodic_zp_csi_rs_res_sets_to_release_list_l_                    aperiodic_zp_csi_rs_res_sets_to_release_list;
  sp_zp_csi_rs_res_sets_to_add_mod_list_l_                           sp_zp_csi_rs_res_sets_to_add_mod_list;
  sp_zp_csi_rs_res_sets_to_release_list_l_                           sp_zp_csi_rs_res_sets_to_release_list;
  setup_release_c<zp_csi_rs_res_set_s>                               p_zp_csi_rs_res_set;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PUCCH-CSI-Resource ::= SEQUENCE
struct pucch_csi_res_s {
  uint8_t ul_bw_part_id = 0;
  uint8_t pucch_res     = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PUSCH-CodeBlockGroupTransmission ::= SEQUENCE
struct pusch_code_block_group_tx_s {
  struct max_code_block_groups_per_transport_block_opts {
    enum options { n2, n4, n6, n8, nulltype } value;
    typedef uint8_t number_type;

    std::string to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<max_code_block_groups_per_transport_block_opts> max_code_block_groups_per_transport_block_e_;

  // member variables
  bool                                         ext = false;
  max_code_block_groups_per_transport_block_e_ max_code_block_groups_per_transport_block;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PortIndexFor8Ranks ::= CHOICE
struct port_idx_for8_ranks_c {
  struct port_idx8_s_ {
    using rank2_minus8_l_ = std::array<uint8_t, 2>;
    using rank3_minus8_l_ = std::array<uint8_t, 3>;
    using rank4_minus8_l_ = std::array<uint8_t, 4>;
    using rank5_minus8_l_ = std::array<uint8_t, 5>;
    using rank6_minus8_l_ = std::array<uint8_t, 6>;
    using rank7_minus8_l_ = std::array<uint8_t, 7>;
    using rank8_minus8_l_ = std::array<uint8_t, 8>;

    // member variables
    bool            rank1_minus8_present = false;
    bool            rank2_minus8_present = false;
    bool            rank3_minus8_present = false;
    bool            rank4_minus8_present = false;
    bool            rank5_minus8_present = false;
    bool            rank6_minus8_present = false;
    bool            rank7_minus8_present = false;
    bool            rank8_minus8_present = false;
    uint8_t         rank1_minus8         = 0;
    rank2_minus8_l_ rank2_minus8;
    rank3_minus8_l_ rank3_minus8;
    rank4_minus8_l_ rank4_minus8;
    rank5_minus8_l_ rank5_minus8;
    rank6_minus8_l_ rank6_minus8;
    rank7_minus8_l_ rank7_minus8;
    rank8_minus8_l_ rank8_minus8;
  };
  struct port_idx4_s_ {
    using rank2_minus4_l_ = std::array<uint8_t, 2>;
    using rank3_minus4_l_ = std::array<uint8_t, 3>;
    using rank4_minus4_l_ = std::array<uint8_t, 4>;

    // member variables
    bool            rank1_minus4_present = false;
    bool            rank2_minus4_present = false;
    bool            rank3_minus4_present = false;
    bool            rank4_minus4_present = false;
    uint8_t         rank1_minus4         = 0;
    rank2_minus4_l_ rank2_minus4;
    rank3_minus4_l_ rank3_minus4;
    rank4_minus4_l_ rank4_minus4;
  };
  struct port_idx2_s_ {
    using rank2_minus2_l_ = std::array<uint8_t, 2>;

    // member variables
    bool            rank1_minus2_present = false;
    bool            rank2_minus2_present = false;
    uint8_t         rank1_minus2         = 0;
    rank2_minus2_l_ rank2_minus2;
  };
  struct types_opts {
    enum options { port_idx8, port_idx4, port_idx2, port_idx1, nulltype } value;
    typedef uint8_t number_type;

    std::string to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<types_opts> types;

  // choice methods
  port_idx_for8_ranks_c() = default;
  port_idx_for8_ranks_c(const port_idx_for8_ranks_c& other);
  port_idx_for8_ranks_c& operator=(const port_idx_for8_ranks_c& other);
  ~port_idx_for8_ranks_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  port_idx8_s_& port_idx8()
  {
    assert_choice_type("portIndex8", type_.to_string(), "PortIndexFor8Ranks");
    return c.get<port_idx8_s_>();
  }
  port_idx4_s_& port_idx4()
  {
    assert_choice_type("portIndex4", type_.to_string(), "PortIndexFor8Ranks");
    return c.get<port_idx4_s_>();
  }
  port_idx2_s_& port_idx2()
  {
    assert_choice_type("portIndex2", type_.to_string(), "PortIndexFor8Ranks");
    return c.get<port_idx2_s_>();
  }
  const port_idx8_s_& port_idx8() const
  {
    assert_choice_type("portIndex8", type_.to_string(), "PortIndexFor8Ranks");
    return c.get<port_idx8_s_>();
  }
  const port_idx4_s_& port_idx4() const
  {
    assert_choice_type("portIndex4", type_.to_string(), "PortIndexFor8Ranks");
    return c.get<port_idx4_s_>();
  }
  const port_idx2_s_& port_idx2() const
  {
    assert_choice_type("portIndex2", type_.to_string(), "PortIndexFor8Ranks");
    return c.get<port_idx2_s_>();
  }
  port_idx8_s_& set_port_idx8()
  {
    set(types::port_idx8);
    return c.get<port_idx8_s_>();
  }
  port_idx4_s_& set_port_idx4()
  {
    set(types::port_idx4);
    return c.get<port_idx4_s_>();
  }
  port_idx2_s_& set_port_idx2()
  {
    set(types::port_idx2);
    return c.get<port_idx2_s_>();
  }

private:
  types                                                     type_;
  choice_buffer_t<port_idx2_s_, port_idx4_s_, port_idx8_s_> c;

  void destroy_();
};

// RadioLinkMonitoringConfig ::= SEQUENCE
struct radio_link_monitoring_cfg_s {
  using fail_detection_res_to_add_mod_list_l_ = dyn_array<radio_link_monitoring_rs_s>;
  using fail_detection_res_to_release_list_l_ = bounded_array<uint8_t, 10>;
  struct beam_fail_instance_max_count_opts {
    enum options { n1, n2, n3, n4, n5, n6, n8, n10, nulltype } value;
    typedef uint8_t number_type;

    std::string to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<beam_fail_instance_max_count_opts> beam_fail_instance_max_count_e_;
  struct beam_fail_detection_timer_opts {
    enum options { pbfd1, pbfd2, pbfd3, pbfd4, pbfd5, pbfd6, pbfd8, pbfd10, nulltype } value;
    typedef uint8_t number_type;

    std::string to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<beam_fail_detection_timer_opts> beam_fail_detection_timer_e_;

  // member variables
  bool                                  ext                                        = false;
  bool                                  fail_detection_res_to_add_mod_list_present = false;
  bool                                  fail_detection_res_to_release_list_present = false;
  bool                                  beam_fail_instance_max_count_present       = false;
  bool                                  beam_fail_detection_timer_present          = false;
  fail_detection_res_to_add_mod_list_l_ fail_detection_res_to_add_mod_list;
  fail_detection_res_to_release_list_l_ fail_detection_res_to_release_list;
  beam_fail_instance_max_count_e_       beam_fail_instance_max_count;
  beam_fail_detection_timer_e_          beam_fail_detection_timer;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SPS-Config ::= SEQUENCE
struct sps_cfg_s {
  struct periodicity_opts {
    enum options {
      ms10,
      ms20,
      ms32,
      ms40,
      ms64,
      ms80,
      ms128,
      ms160,
      ms320,
      ms640,
      spare6,
      spare5,
      spare4,
      spare3,
      spare2,
      spare1,
      nulltype
    } value;
    typedef uint16_t number_type;

    std::string to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<periodicity_opts> periodicity_e_;

  // member variables
  bool           ext                 = false;
  bool           n1_pucch_an_present = false;
  bool           mcs_table_present   = false;
  periodicity_e_ periodicity;
  uint8_t        nrof_harq_processes = 1;
  uint8_t        n1_pucch_an         = 0;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SRS-TPC-PDCCH-Config ::= SEQUENCE
struct srs_tpc_pdcch_cfg_s {
  using srs_cc_set_idxlist_l_ = dyn_array<srs_cc_set_idx_s>;

  // member variables
  bool                  srs_cc_set_idxlist_present = false;
  srs_cc_set_idxlist_l_ srs_cc_set_idxlist;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SlotFormatCombinationsPerCell ::= SEQUENCE
struct slot_format_combinations_per_cell_s {
  using slot_format_combinations_l_ = dyn_array<slot_format_combination_s>;

  // member variables
  bool                        ext                              = false;
  bool                        subcarrier_spacing2_present      = false;
  bool                        slot_format_combinations_present = false;
  bool                        position_in_dci_present          = false;
  uint8_t                     serving_cell_id                  = 0;
  subcarrier_spacing_e        subcarrier_spacing;
  subcarrier_spacing_e        subcarrier_spacing2;
  slot_format_combinations_l_ slot_format_combinations;
  uint8_t                     position_in_dci = 0;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BWP-DownlinkDedicated ::= SEQUENCE
struct bwp_dl_ded_s {
  bool                                         ext                               = false;
  bool                                         pdcch_cfg_present                 = false;
  bool                                         pdsch_cfg_present                 = false;
  bool                                         sps_cfg_present                   = false;
  bool                                         radio_link_monitoring_cfg_present = false;
  setup_release_c<pdcch_cfg_s>                 pdcch_cfg;
  setup_release_c<pdsch_cfg_s>                 pdsch_cfg;
  setup_release_c<sps_cfg_s>                   sps_cfg;
  setup_release_c<radio_link_monitoring_cfg_s> radio_link_monitoring_cfg;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BWP-Uplink ::= SEQUENCE
struct bwp_ul_s {
  bool            ext                = false;
  bool            bwp_common_present = false;
  bool            bwp_ded_present    = false;
  uint8_t         bwp_id             = 0;
  bwp_ul_common_s bwp_common;
  bwp_ul_ded_s    bwp_ded;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CFRA ::= SEQUENCE
struct cfra_s {
  struct occasions_s_ {
    struct ssb_per_rach_occasion_opts {
      enum options { one_eighth, one_fourth, one_half, one, two, four, eight, sixteen, nulltype } value;
      typedef float number_type;

      std::string to_string() const;
      float       to_number() const;
      std::string to_number_string() const;
    };
    typedef enumerated<ssb_per_rach_occasion_opts> ssb_per_rach_occasion_e_;

    // member variables
    bool                     ssb_per_rach_occasion_present = false;
    rach_cfg_generic_s       rach_cfg_generic;
    ssb_per_rach_occasion_e_ ssb_per_rach_occasion;
  };
  struct res_c_ {
    struct ssb_s_ {
      using ssb_res_list_l_ = dyn_array<cfra_ssb_res_s>;

      // member variables
      ssb_res_list_l_ ssb_res_list;
      uint8_t         ra_ssb_occasion_mask_idx = 0;
    };
    struct csirs_s_ {
      using csirs_res_list_l_ = dyn_array<cfra_csirs_res_s>;

      // member variables
      csirs_res_list_l_ csirs_res_list;
      uint8_t           rsrp_thres_csi_rs = 0;
    };
    struct types_opts {
      enum options { ssb, csirs, nulltype } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    res_c_() = default;
    res_c_(const res_c_& other);
    res_c_& operator=(const res_c_& other);
    ~res_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    ssb_s_& ssb()
    {
      assert_choice_type("ssb", type_.to_string(), "resources");
      return c.get<ssb_s_>();
    }
    csirs_s_& csirs()
    {
      assert_choice_type("csirs", type_.to_string(), "resources");
      return c.get<csirs_s_>();
    }
    const ssb_s_& ssb() const
    {
      assert_choice_type("ssb", type_.to_string(), "resources");
      return c.get<ssb_s_>();
    }
    const csirs_s_& csirs() const
    {
      assert_choice_type("csirs", type_.to_string(), "resources");
      return c.get<csirs_s_>();
    }
    ssb_s_& set_ssb()
    {
      set(types::ssb);
      return c.get<ssb_s_>();
    }
    csirs_s_& set_csirs()
    {
      set(types::csirs);
      return c.get<csirs_s_>();
    }

  private:
    types                             type_;
    choice_buffer_t<csirs_s_, ssb_s_> c;

    void destroy_();
  };

  // member variables
  bool         ext               = false;
  bool         occasions_present = false;
  occasions_s_ occasions;
  res_c_       res;
  // ...
  // group 0
  bool    total_nof_ra_preambs_v1530_present = false;
  uint8_t total_nof_ra_preambs_v1530         = 1;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CSI-AperiodicTriggerStateList ::= SEQUENCE (SIZE (1..128)) OF CSI-AperiodicTriggerState
using csi_aperiodic_trigger_state_list_l = dyn_array<csi_aperiodic_trigger_state_s>;

// CSI-IM-Resource ::= SEQUENCE
struct csi_im_res_s {
  struct csi_im_res_elem_pattern_c_ {
    struct pattern0_s_ {
      struct subcarrier_location_p0_opts {
        enum options { s0, s2, s4, s6, s8, s10, nulltype } value;
        typedef uint8_t number_type;

        std::string to_string() const;
        uint8_t     to_number() const;
      };
      typedef enumerated<subcarrier_location_p0_opts> subcarrier_location_p0_e_;

      // member variables
      subcarrier_location_p0_e_ subcarrier_location_p0;
      uint8_t                   symbol_location_p0 = 0;
    };
    struct pattern1_s_ {
      struct subcarrier_location_p1_opts {
        enum options { s0, s4, s8, nulltype } value;
        typedef uint8_t number_type;

        std::string to_string() const;
        uint8_t     to_number() const;
      };
      typedef enumerated<subcarrier_location_p1_opts> subcarrier_location_p1_e_;

      // member variables
      subcarrier_location_p1_e_ subcarrier_location_p1;
      uint8_t                   symbol_location_p1 = 0;
    };
    struct types_opts {
      enum options { pattern0, pattern1, nulltype } value;
      typedef uint8_t number_type;

      std::string to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    csi_im_res_elem_pattern_c_() = default;
    csi_im_res_elem_pattern_c_(const csi_im_res_elem_pattern_c_& other);
    csi_im_res_elem_pattern_c_& operator=(const csi_im_res_elem_pattern_c_& other);
    ~csi_im_res_elem_pattern_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    pattern0_s_& pattern0()
    {
      assert_choice_type("pattern0", type_.to_string(), "csi-IM-ResourceElementPattern");
      return c.get<pattern0_s_>();
    }
    pattern1_s_& pattern1()
    {
      assert_choice_type("pattern1", type_.to_string(), "csi-IM-ResourceElementPattern");
      return c.get<pattern1_s_>();
    }
    const pattern0_s_& pattern0() const
    {
      assert_choice_type("pattern0", type_.to_string(), "csi-IM-ResourceElementPattern");
      return c.get<pattern0_s_>();
    }
    const pattern1_s_& pattern1() const
    {
      assert_choice_type("pattern1", type_.to_string(), "csi-IM-ResourceElementPattern");
      return c.get<pattern1_s_>();
    }
    pattern0_s_& set_pattern0()
    {
      set(types::pattern0);
      return c.get<pattern0_s_>();
    }
    pattern1_s_& set_pattern1()
    {
      set(types::pattern1);
      return c.get<pattern1_s_>();
    }

  private:
    types                                     type_;
    choice_buffer_t<pattern0_s_, pattern1_s_> c;

    void destroy_();
  };

  // member variables
  bool                             ext                             = false;
  bool                             csi_im_res_elem_pattern_present = false;
  bool                             freq_band_present               = false;
  bool                             periodicity_and_offset_present  = false;
  uint8_t                          csi_im_res_id                   = 0;
  csi_im_res_elem_pattern_c_       csi_im_res_elem_pattern;
  csi_freq_occupation_s            freq_band;
  csi_res_periodicity_and_offset_c periodicity_and_offset;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CSI-IM-ResourceSet ::= SEQUENCE
struct csi_im_res_set_s {
  using csi_im_res_l_ = bounded_array<uint8_t, 8>;

  // member variables
  bool          ext               = false;
  uint8_t       csi_im_res_set_id = 0;
  csi_im_res_l_ csi_im_res;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CSI-ReportConfig ::= SEQUENCE
struct csi_report_cfg_s {
  struct report_cfg_type_c_ {
    struct periodic_s_ {
      using pucch_csi_res_list_l_ = dyn_array<pucch_csi_res_s>;

      // member variables
      csi_report_periodicity_and_offset_c report_slot_cfg;
      pucch_csi_res_list_l_               pucch_csi_res_list;
    };
    struct semi_persistent_on_pucch_s_ {
      using pucch_csi_res_list_l_ = dyn_array<pucch_csi_res_s>;

      // member variables
      csi_report_periodicity_and_offset_c report_slot_cfg;
      pucch_csi_res_list_l_               pucch_csi_res_list;
    };
    struct semi_persistent_on_pusch_s_ {
      struct report_slot_cfg_opts {
        enum options { sl5, sl10, sl20, sl40, sl80, sl160, sl320, nulltype } value;
        typedef uint16_t number_type;

        std::string to_string() const;
        uint16_t    to_number() const;
      };
      typedef enumerated<report_slot_cfg_opts> report_slot_cfg_e_;
      using report_slot_offset_list_l_ = bounded_array<uint8_t, 16>;

      // member variables
      report_slot_cfg_e_         report_slot_cfg;
      report_slot_offset_list_l_ report_slot_offset_list;
      uint8_t                    p0alpha = 0;
    };
    struct aperiodic_s_ {
      using report_slot_offset_list_l_ = bounded_array<uint8_t, 16>;

      // member variables
      report_slot_offset_list_l_ report_slot_offset_list;
    };
    struct types_opts {
      enum options { periodic, semi_persistent_on_pucch, semi_persistent_on_pusch, aperiodic, nulltype } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    report_cfg_type_c_() = default;
    report_cfg_type_c_(const report_cfg_type_c_& other);
    report_cfg_type_c_& operator=(const report_cfg_type_c_& other);
    ~report_cfg_type_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    periodic_s_& periodic()
    {
      assert_choice_type("periodic", type_.to_string(), "reportConfigType");
      return c.get<periodic_s_>();
    }
    semi_persistent_on_pucch_s_& semi_persistent_on_pucch()
    {
      assert_choice_type("semiPersistentOnPUCCH", type_.to_string(), "reportConfigType");
      return c.get<semi_persistent_on_pucch_s_>();
    }
    semi_persistent_on_pusch_s_& semi_persistent_on_pusch()
    {
      assert_choice_type("semiPersistentOnPUSCH", type_.to_string(), "reportConfigType");
      return c.get<semi_persistent_on_pusch_s_>();
    }
    aperiodic_s_& aperiodic()
    {
      assert_choice_type("aperiodic", type_.to_string(), "reportConfigType");
      return c.get<aperiodic_s_>();
    }
    const periodic_s_& periodic() const
    {
      assert_choice_type("periodic", type_.to_string(), "reportConfigType");
      return c.get<periodic_s_>();
    }
    const semi_persistent_on_pucch_s_& semi_persistent_on_pucch() const
    {
      assert_choice_type("semiPersistentOnPUCCH", type_.to_string(), "reportConfigType");
      return c.get<semi_persistent_on_pucch_s_>();
    }
    const semi_persistent_on_pusch_s_& semi_persistent_on_pusch() const
    {
      assert_choice_type("semiPersistentOnPUSCH", type_.to_string(), "reportConfigType");
      return c.get<semi_persistent_on_pusch_s_>();
    }
    const aperiodic_s_& aperiodic() const
    {
      assert_choice_type("aperiodic", type_.to_string(), "reportConfigType");
      return c.get<aperiodic_s_>();
    }
    periodic_s_& set_periodic()
    {
      set(types::periodic);
      return c.get<periodic_s_>();
    }
    semi_persistent_on_pucch_s_& set_semi_persistent_on_pucch()
    {
      set(types::semi_persistent_on_pucch);
      return c.get<semi_persistent_on_pucch_s_>();
    }
    semi_persistent_on_pusch_s_& set_semi_persistent_on_pusch()
    {
      set(types::semi_persistent_on_pusch);
      return c.get<semi_persistent_on_pusch_s_>();
    }
    aperiodic_s_& set_aperiodic()
    {
      set(types::aperiodic);
      return c.get<aperiodic_s_>();
    }

  private:
    types                                                                                                type_;
    choice_buffer_t<aperiodic_s_, periodic_s_, semi_persistent_on_pucch_s_, semi_persistent_on_pusch_s_> c;

    void destroy_();
  };
  struct report_quant_c_ {
    struct cri_ri_i1_cqi_s_ {
      struct pdsch_bundle_size_for_csi_opts {
        enum options { n2, n4, nulltype } value;
        typedef uint8_t number_type;

        std::string to_string() const;
        uint8_t     to_number() const;
      };
      typedef enumerated<pdsch_bundle_size_for_csi_opts> pdsch_bundle_size_for_csi_e_;

      // member variables
      bool                         pdsch_bundle_size_for_csi_present = false;
      pdsch_bundle_size_for_csi_e_ pdsch_bundle_size_for_csi;
    };
    struct types_opts {
      enum options {
        none,
        cri_ri_pmi_cqi,
        cri_ri_i1,
        cri_ri_i1_cqi,
        cri_ri_cqi,
        cri_rsrp,
        ssb_idx_rsrp,
        cri_ri_li_pmi_cqi,
        nulltype
      } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    report_quant_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    cri_ri_i1_cqi_s_& cri_ri_i1_cqi()
    {
      assert_choice_type("cri-RI-i1-CQI", type_.to_string(), "reportQuantity");
      return c;
    }
    const cri_ri_i1_cqi_s_& cri_ri_i1_cqi() const
    {
      assert_choice_type("cri-RI-i1-CQI", type_.to_string(), "reportQuantity");
      return c;
    }
    cri_ri_i1_cqi_s_& set_cri_ri_i1_cqi()
    {
      set(types::cri_ri_i1_cqi);
      return c;
    }

  private:
    types            type_;
    cri_ri_i1_cqi_s_ c;
  };
  struct report_freq_cfg_s_ {
    struct cqi_format_ind_opts {
      enum options { wideband_cqi, subband_cqi, nulltype } value;

      std::string to_string() const;
    };
    typedef enumerated<cqi_format_ind_opts> cqi_format_ind_e_;
    struct pmi_format_ind_opts {
      enum options { wideband_pmi, subband_pmi, nulltype } value;

      std::string to_string() const;
    };
    typedef enumerated<pmi_format_ind_opts> pmi_format_ind_e_;
    struct csi_report_band_c_ {
      struct types_opts {
        enum options {
          subbands3,
          subbands4,
          subbands5,
          subbands6,
          subbands7,
          subbands8,
          subbands9,
          subbands10,
          subbands11,
          subbands12,
          subbands13,
          subbands14,
          subbands15,
          subbands16,
          subbands17,
          subbands18,
          // ...
          subbands19_v1530,
          nulltype
        } value;
        typedef uint8_t number_type;

        std::string to_string() const;
        uint8_t     to_number() const;
      };
      typedef enumerated<types_opts, true, 1> types;

      // choice methods
      csi_report_band_c_() = default;
      csi_report_band_c_(const csi_report_band_c_& other);
      csi_report_band_c_& operator=(const csi_report_band_c_& other);
      ~csi_report_band_c_() { destroy_(); }
      void        set(types::options e = types::nulltype);
      types       type() const { return type_; }
      SRSASN_CODE pack(bit_ref& bref) const;
      SRSASN_CODE unpack(cbit_ref& bref);
      void        to_json(json_writer& j) const;
      // getters
      fixed_bitstring<3>& subbands3()
      {
        assert_choice_type("subbands3", type_.to_string(), "csi-ReportingBand");
        return c.get<fixed_bitstring<3> >();
      }
      fixed_bitstring<4>& subbands4()
      {
        assert_choice_type("subbands4", type_.to_string(), "csi-ReportingBand");
        return c.get<fixed_bitstring<4> >();
      }
      fixed_bitstring<5>& subbands5()
      {
        assert_choice_type("subbands5", type_.to_string(), "csi-ReportingBand");
        return c.get<fixed_bitstring<5> >();
      }
      fixed_bitstring<6>& subbands6()
      {
        assert_choice_type("subbands6", type_.to_string(), "csi-ReportingBand");
        return c.get<fixed_bitstring<6> >();
      }
      fixed_bitstring<7>& subbands7()
      {
        assert_choice_type("subbands7", type_.to_string(), "csi-ReportingBand");
        return c.get<fixed_bitstring<7> >();
      }
      fixed_bitstring<8>& subbands8()
      {
        assert_choice_type("subbands8", type_.to_string(), "csi-ReportingBand");
        return c.get<fixed_bitstring<8> >();
      }
      fixed_bitstring<9>& subbands9()
      {
        assert_choice_type("subbands9", type_.to_string(), "csi-ReportingBand");
        return c.get<fixed_bitstring<9> >();
      }
      fixed_bitstring<10>& subbands10()
      {
        assert_choice_type("subbands10", type_.to_string(), "csi-ReportingBand");
        return c.get<fixed_bitstring<10> >();
      }
      fixed_bitstring<11>& subbands11()
      {
        assert_choice_type("subbands11", type_.to_string(), "csi-ReportingBand");
        return c.get<fixed_bitstring<11> >();
      }
      fixed_bitstring<12>& subbands12()
      {
        assert_choice_type("subbands12", type_.to_string(), "csi-ReportingBand");
        return c.get<fixed_bitstring<12> >();
      }
      fixed_bitstring<13>& subbands13()
      {
        assert_choice_type("subbands13", type_.to_string(), "csi-ReportingBand");
        return c.get<fixed_bitstring<13> >();
      }
      fixed_bitstring<14>& subbands14()
      {
        assert_choice_type("subbands14", type_.to_string(), "csi-ReportingBand");
        return c.get<fixed_bitstring<14> >();
      }
      fixed_bitstring<15>& subbands15()
      {
        assert_choice_type("subbands15", type_.to_string(), "csi-ReportingBand");
        return c.get<fixed_bitstring<15> >();
      }
      fixed_bitstring<16>& subbands16()
      {
        assert_choice_type("subbands16", type_.to_string(), "csi-ReportingBand");
        return c.get<fixed_bitstring<16> >();
      }
      fixed_bitstring<17>& subbands17()
      {
        assert_choice_type("subbands17", type_.to_string(), "csi-ReportingBand");
        return c.get<fixed_bitstring<17> >();
      }
      fixed_bitstring<18>& subbands18()
      {
        assert_choice_type("subbands18", type_.to_string(), "csi-ReportingBand");
        return c.get<fixed_bitstring<18> >();
      }
      fixed_bitstring<19>& subbands19_v1530()
      {
        assert_choice_type("subbands19-v1530", type_.to_string(), "csi-ReportingBand");
        return c.get<fixed_bitstring<19> >();
      }
      const fixed_bitstring<3>& subbands3() const
      {
        assert_choice_type("subbands3", type_.to_string(), "csi-ReportingBand");
        return c.get<fixed_bitstring<3> >();
      }
      const fixed_bitstring<4>& subbands4() const
      {
        assert_choice_type("subbands4", type_.to_string(), "csi-ReportingBand");
        return c.get<fixed_bitstring<4> >();
      }
      const fixed_bitstring<5>& subbands5() const
      {
        assert_choice_type("subbands5", type_.to_string(), "csi-ReportingBand");
        return c.get<fixed_bitstring<5> >();
      }
      const fixed_bitstring<6>& subbands6() const
      {
        assert_choice_type("subbands6", type_.to_string(), "csi-ReportingBand");
        return c.get<fixed_bitstring<6> >();
      }
      const fixed_bitstring<7>& subbands7() const
      {
        assert_choice_type("subbands7", type_.to_string(), "csi-ReportingBand");
        return c.get<fixed_bitstring<7> >();
      }
      const fixed_bitstring<8>& subbands8() const
      {
        assert_choice_type("subbands8", type_.to_string(), "csi-ReportingBand");
        return c.get<fixed_bitstring<8> >();
      }
      const fixed_bitstring<9>& subbands9() const
      {
        assert_choice_type("subbands9", type_.to_string(), "csi-ReportingBand");
        return c.get<fixed_bitstring<9> >();
      }
      const fixed_bitstring<10>& subbands10() const
      {
        assert_choice_type("subbands10", type_.to_string(), "csi-ReportingBand");
        return c.get<fixed_bitstring<10> >();
      }
      const fixed_bitstring<11>& subbands11() const
      {
        assert_choice_type("subbands11", type_.to_string(), "csi-ReportingBand");
        return c.get<fixed_bitstring<11> >();
      }
      const fixed_bitstring<12>& subbands12() const
      {
        assert_choice_type("subbands12", type_.to_string(), "csi-ReportingBand");
        return c.get<fixed_bitstring<12> >();
      }
      const fixed_bitstring<13>& subbands13() const
      {
        assert_choice_type("subbands13", type_.to_string(), "csi-ReportingBand");
        return c.get<fixed_bitstring<13> >();
      }
      const fixed_bitstring<14>& subbands14() const
      {
        assert_choice_type("subbands14", type_.to_string(), "csi-ReportingBand");
        return c.get<fixed_bitstring<14> >();
      }
      const fixed_bitstring<15>& subbands15() const
      {
        assert_choice_type("subbands15", type_.to_string(), "csi-ReportingBand");
        return c.get<fixed_bitstring<15> >();
      }
      const fixed_bitstring<16>& subbands16() const
      {
        assert_choice_type("subbands16", type_.to_string(), "csi-ReportingBand");
        return c.get<fixed_bitstring<16> >();
      }
      const fixed_bitstring<17>& subbands17() const
      {
        assert_choice_type("subbands17", type_.to_string(), "csi-ReportingBand");
        return c.get<fixed_bitstring<17> >();
      }
      const fixed_bitstring<18>& subbands18() const
      {
        assert_choice_type("subbands18", type_.to_string(), "csi-ReportingBand");
        return c.get<fixed_bitstring<18> >();
      }
      const fixed_bitstring<19>& subbands19_v1530() const
      {
        assert_choice_type("subbands19-v1530", type_.to_string(), "csi-ReportingBand");
        return c.get<fixed_bitstring<19> >();
      }
      fixed_bitstring<3>& set_subbands3()
      {
        set(types::subbands3);
        return c.get<fixed_bitstring<3> >();
      }
      fixed_bitstring<4>& set_subbands4()
      {
        set(types::subbands4);
        return c.get<fixed_bitstring<4> >();
      }
      fixed_bitstring<5>& set_subbands5()
      {
        set(types::subbands5);
        return c.get<fixed_bitstring<5> >();
      }
      fixed_bitstring<6>& set_subbands6()
      {
        set(types::subbands6);
        return c.get<fixed_bitstring<6> >();
      }
      fixed_bitstring<7>& set_subbands7()
      {
        set(types::subbands7);
        return c.get<fixed_bitstring<7> >();
      }
      fixed_bitstring<8>& set_subbands8()
      {
        set(types::subbands8);
        return c.get<fixed_bitstring<8> >();
      }
      fixed_bitstring<9>& set_subbands9()
      {
        set(types::subbands9);
        return c.get<fixed_bitstring<9> >();
      }
      fixed_bitstring<10>& set_subbands10()
      {
        set(types::subbands10);
        return c.get<fixed_bitstring<10> >();
      }
      fixed_bitstring<11>& set_subbands11()
      {
        set(types::subbands11);
        return c.get<fixed_bitstring<11> >();
      }
      fixed_bitstring<12>& set_subbands12()
      {
        set(types::subbands12);
        return c.get<fixed_bitstring<12> >();
      }
      fixed_bitstring<13>& set_subbands13()
      {
        set(types::subbands13);
        return c.get<fixed_bitstring<13> >();
      }
      fixed_bitstring<14>& set_subbands14()
      {
        set(types::subbands14);
        return c.get<fixed_bitstring<14> >();
      }
      fixed_bitstring<15>& set_subbands15()
      {
        set(types::subbands15);
        return c.get<fixed_bitstring<15> >();
      }
      fixed_bitstring<16>& set_subbands16()
      {
        set(types::subbands16);
        return c.get<fixed_bitstring<16> >();
      }
      fixed_bitstring<17>& set_subbands17()
      {
        set(types::subbands17);
        return c.get<fixed_bitstring<17> >();
      }
      fixed_bitstring<18>& set_subbands18()
      {
        set(types::subbands18);
        return c.get<fixed_bitstring<18> >();
      }
      fixed_bitstring<19>& set_subbands19_v1530()
      {
        set(types::subbands19_v1530);
        return c.get<fixed_bitstring<19> >();
      }

    private:
      types                                 type_;
      choice_buffer_t<fixed_bitstring<19> > c;

      void destroy_();
    };

    // member variables
    bool               cqi_format_ind_present  = false;
    bool               pmi_format_ind_present  = false;
    bool               csi_report_band_present = false;
    cqi_format_ind_e_  cqi_format_ind;
    pmi_format_ind_e_  pmi_format_ind;
    csi_report_band_c_ csi_report_band;
  };
  struct time_restrict_for_ch_meass_opts {
    enum options { cfgured, not_cfgured, nulltype } value;

    std::string to_string() const;
  };
  typedef enumerated<time_restrict_for_ch_meass_opts> time_restrict_for_ch_meass_e_;
  struct time_restrict_for_interference_meass_opts {
    enum options { cfgured, not_cfgured, nulltype } value;

    std::string to_string() const;
  };
  typedef enumerated<time_restrict_for_interference_meass_opts> time_restrict_for_interference_meass_e_;
  struct dummy_opts {
    enum options { n1, n2, nulltype } value;
    typedef uint8_t number_type;

    std::string to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<dummy_opts> dummy_e_;
  struct group_based_beam_report_c_ {
    struct disabled_s_ {
      struct nrof_reported_rs_opts {
        enum options { n1, n2, n3, n4, nulltype } value;
        typedef uint8_t number_type;

        std::string to_string() const;
        uint8_t     to_number() const;
      };
      typedef enumerated<nrof_reported_rs_opts> nrof_reported_rs_e_;

      // member variables
      bool                nrof_reported_rs_present = false;
      nrof_reported_rs_e_ nrof_reported_rs;
    };
    struct types_opts {
      enum options { enabled, disabled, nulltype } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    group_based_beam_report_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    disabled_s_& disabled()
    {
      assert_choice_type("disabled", type_.to_string(), "groupBasedBeamReporting");
      return c;
    }
    const disabled_s_& disabled() const
    {
      assert_choice_type("disabled", type_.to_string(), "groupBasedBeamReporting");
      return c;
    }
    disabled_s_& set_disabled()
    {
      set(types::disabled);
      return c;
    }

  private:
    types       type_;
    disabled_s_ c;
  };
  struct cqi_table_opts {
    enum options { table1, table2, table3, spare1, nulltype } value;
    typedef uint8_t number_type;

    std::string to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<cqi_table_opts> cqi_table_e_;
  struct subband_size_opts {
    enum options { value1, value2, nulltype } value;
    typedef uint8_t number_type;

    std::string to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<subband_size_opts> subband_size_e_;
  using non_pmi_port_ind_l_ = dyn_array<port_idx_for8_ranks_c>;
  struct semi_persistent_on_pusch_v1530_s_ {
    struct report_slot_cfg_v1530_opts {
      enum options { sl4, sl8, sl16, nulltype } value;
      typedef uint8_t number_type;

      std::string to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<report_slot_cfg_v1530_opts> report_slot_cfg_v1530_e_;

    // member variables
    report_slot_cfg_v1530_e_ report_slot_cfg_v1530;
  };

  // member variables
  bool                                    ext                                     = false;
  bool                                    carrier_present                         = false;
  bool                                    csi_im_res_for_interference_present     = false;
  bool                                    nzp_csi_rs_res_for_interference_present = false;
  bool                                    report_freq_cfg_present                 = false;
  bool                                    codebook_cfg_present                    = false;
  bool                                    dummy_present                           = false;
  bool                                    cqi_table_present                       = false;
  bool                                    non_pmi_port_ind_present                = false;
  uint8_t                                 report_cfg_id                           = 0;
  uint8_t                                 carrier                                 = 0;
  uint8_t                                 res_for_ch_meas                         = 0;
  uint8_t                                 csi_im_res_for_interference             = 0;
  uint8_t                                 nzp_csi_rs_res_for_interference         = 0;
  report_cfg_type_c_                      report_cfg_type;
  report_quant_c_                         report_quant;
  report_freq_cfg_s_                      report_freq_cfg;
  time_restrict_for_ch_meass_e_           time_restrict_for_ch_meass;
  time_restrict_for_interference_meass_e_ time_restrict_for_interference_meass;
  codebook_cfg_s                          codebook_cfg;
  dummy_e_                                dummy;
  group_based_beam_report_c_              group_based_beam_report;
  cqi_table_e_                            cqi_table;
  subband_size_e_                         subband_size;
  non_pmi_port_ind_l_                     non_pmi_port_ind;
  // ...
  // group 0
  copy_ptr<semi_persistent_on_pusch_v1530_s_> semi_persistent_on_pusch_v1530;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CSI-ResourceConfig ::= SEQUENCE
struct csi_res_cfg_s {
  struct csi_rs_res_set_list_c_ {
    struct nzp_csi_rs_ssb_s_ {
      using nzp_csi_rs_res_set_list_l_ = bounded_array<uint8_t, 16>;
      using csi_ssb_res_set_list_l_    = std::array<uint8_t, 1>;

      // member variables
      bool                       nzp_csi_rs_res_set_list_present = false;
      bool                       csi_ssb_res_set_list_present    = false;
      nzp_csi_rs_res_set_list_l_ nzp_csi_rs_res_set_list;
      csi_ssb_res_set_list_l_    csi_ssb_res_set_list;
    };
    using csi_im_res_set_list_l_ = bounded_array<uint8_t, 16>;
    struct types_opts {
      enum options { nzp_csi_rs_ssb, csi_im_res_set_list, nulltype } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    csi_rs_res_set_list_c_() = default;
    csi_rs_res_set_list_c_(const csi_rs_res_set_list_c_& other);
    csi_rs_res_set_list_c_& operator=(const csi_rs_res_set_list_c_& other);
    ~csi_rs_res_set_list_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    nzp_csi_rs_ssb_s_& nzp_csi_rs_ssb()
    {
      assert_choice_type("nzp-CSI-RS-SSB", type_.to_string(), "csi-RS-ResourceSetList");
      return c.get<nzp_csi_rs_ssb_s_>();
    }
    csi_im_res_set_list_l_& csi_im_res_set_list()
    {
      assert_choice_type("csi-IM-ResourceSetList", type_.to_string(), "csi-RS-ResourceSetList");
      return c.get<csi_im_res_set_list_l_>();
    }
    const nzp_csi_rs_ssb_s_& nzp_csi_rs_ssb() const
    {
      assert_choice_type("nzp-CSI-RS-SSB", type_.to_string(), "csi-RS-ResourceSetList");
      return c.get<nzp_csi_rs_ssb_s_>();
    }
    const csi_im_res_set_list_l_& csi_im_res_set_list() const
    {
      assert_choice_type("csi-IM-ResourceSetList", type_.to_string(), "csi-RS-ResourceSetList");
      return c.get<csi_im_res_set_list_l_>();
    }
    nzp_csi_rs_ssb_s_& set_nzp_csi_rs_ssb()
    {
      set(types::nzp_csi_rs_ssb);
      return c.get<nzp_csi_rs_ssb_s_>();
    }
    csi_im_res_set_list_l_& set_csi_im_res_set_list()
    {
      set(types::csi_im_res_set_list);
      return c.get<csi_im_res_set_list_l_>();
    }

  private:
    types                                                      type_;
    choice_buffer_t<csi_im_res_set_list_l_, nzp_csi_rs_ssb_s_> c;

    void destroy_();
  };
  struct res_type_opts {
    enum options { aperiodic, semi_persistent, periodic, nulltype } value;

    std::string to_string() const;
  };
  typedef enumerated<res_type_opts> res_type_e_;

  // member variables
  bool                   ext            = false;
  uint8_t                csi_res_cfg_id = 0;
  csi_rs_res_set_list_c_ csi_rs_res_set_list;
  uint8_t                bwp_id = 0;
  res_type_e_            res_type;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CSI-SSB-ResourceSet ::= SEQUENCE
struct csi_ssb_res_set_s {
  using csi_ssb_res_list_l_ = dyn_array<uint8_t>;

  // member variables
  bool                ext                = false;
  uint8_t             csi_ssb_res_set_id = 0;
  csi_ssb_res_list_l_ csi_ssb_res_list;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CSI-SemiPersistentOnPUSCH-TriggerStateList ::= SEQUENCE (SIZE (1..64)) OF CSI-SemiPersistentOnPUSCH-TriggerState
using csi_semi_persistent_on_pusch_trigger_state_list_l = dyn_array<csi_semi_persistent_on_pusch_trigger_state_s>;

// DownlinkConfigCommon ::= SEQUENCE
struct dl_cfg_common_s {
  bool            ext                  = false;
  bool            freq_info_dl_present = false;
  bool            init_dl_bwp_present  = false;
  freq_info_dl_s  freq_info_dl;
  bwp_dl_common_s init_dl_bwp;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// NZP-CSI-RS-Resource ::= SEQUENCE
struct nzp_csi_rs_res_s {
  struct pwr_ctrl_offset_ss_opts {
    enum options { db_minus3, db0, db3, db6, nulltype } value;
    typedef int8_t number_type;

    std::string to_string() const;
    int8_t      to_number() const;
  };
  typedef enumerated<pwr_ctrl_offset_ss_opts> pwr_ctrl_offset_ss_e_;

  // member variables
  bool                             ext                              = false;
  bool                             pwr_ctrl_offset_ss_present       = false;
  bool                             periodicity_and_offset_present   = false;
  bool                             qcl_info_periodic_csi_rs_present = false;
  uint8_t                          nzp_csi_rs_res_id                = 0;
  csi_rs_res_map_s                 res_map;
  int8_t                           pwr_ctrl_offset = -8;
  pwr_ctrl_offset_ss_e_            pwr_ctrl_offset_ss;
  uint16_t                         scrambling_id = 0;
  csi_res_periodicity_and_offset_c periodicity_and_offset;
  uint8_t                          qcl_info_periodic_csi_rs = 0;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// NZP-CSI-RS-ResourceSet ::= SEQUENCE
struct nzp_csi_rs_res_set_s {
  using nzp_csi_rs_res_l_ = dyn_array<uint8_t>;
  struct repeat_opts {
    enum options { on, off, nulltype } value;

    std::string to_string() const;
  };
  typedef enumerated<repeat_opts> repeat_e_;

  // member variables
  bool              ext                              = false;
  bool              repeat_present                   = false;
  bool              aperiodic_trigger_offset_present = false;
  bool              trs_info_present                 = false;
  uint8_t           nzp_csi_res_set_id               = 0;
  nzp_csi_rs_res_l_ nzp_csi_rs_res;
  repeat_e_         repeat;
  uint8_t           aperiodic_trigger_offset = 0;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDSCH-CodeBlockGroupTransmission ::= SEQUENCE
struct pdsch_code_block_group_tx_s {
  struct max_code_block_groups_per_transport_block_opts {
    enum options { n2, n4, n6, n8, nulltype } value;
    typedef uint8_t number_type;

    std::string to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<max_code_block_groups_per_transport_block_opts> max_code_block_groups_per_transport_block_e_;

  // member variables
  bool                                         ext = false;
  max_code_block_groups_per_transport_block_e_ max_code_block_groups_per_transport_block;
  bool                                         code_block_group_flush_ind = false;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PUSCH-ServingCellConfig ::= SEQUENCE
struct pusch_serving_cell_cfg_s {
  struct xoverhead_opts {
    enum options { xoh6, xoh12, xoh18, nulltype } value;
    typedef uint8_t number_type;

    std::string to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<xoverhead_opts> xoverhead_e_;

  // member variables
  bool                                         ext                         = false;
  bool                                         code_block_group_tx_present = false;
  bool                                         rate_matching_present       = false;
  bool                                         xoverhead_present           = false;
  setup_release_c<pusch_code_block_group_tx_s> code_block_group_tx;
  xoverhead_e_                                 xoverhead;
  // ...
  // group 0
  bool    max_mimo_layers_present          = false;
  bool    processing_type2_enabled_present = false;
  uint8_t max_mimo_layers                  = 1;
  bool    processing_type2_enabled         = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PollByte ::= ENUMERATED
struct poll_byte_opts {
  enum options {
    kb1,
    kb2,
    kb5,
    kb8,
    kb10,
    kb15,
    kb25,
    kb50,
    kb75,
    kb100,
    kb125,
    kb250,
    kb375,
    kb500,
    kb750,
    kb1000,
    kb1250,
    kb1500,
    kb2000,
    kb3000,
    kb4000,
    kb4500,
    kb5000,
    kb5500,
    kb6000,
    kb6500,
    kb7000,
    kb7500,
    mb8,
    mb9,
    mb10,
    mb11,
    mb12,
    mb13,
    mb14,
    mb15,
    mb16,
    mb17,
    mb18,
    mb20,
    mb25,
    mb30,
    mb40,
    infinity,
    spare20,
    spare19,
    spare18,
    spare17,
    spare16,
    spare15,
    spare14,
    spare13,
    spare12,
    spare11,
    spare10,
    spare9,
    spare8,
    spare7,
    spare6,
    spare5,
    spare4,
    spare3,
    spare2,
    spare1,
    nulltype
  } value;

  std::string to_string() const;
};
typedef enumerated<poll_byte_opts> poll_byte_e;

// PollPDU ::= ENUMERATED
struct poll_pdu_opts {
  enum options {
    p4,
    p8,
    p16,
    p32,
    p64,
    p128,
    p256,
    p512,
    p1024,
    p2048,
    p4096,
    p6144,
    p8192,
    p12288,
    p16384,
    p20480,
    p24576,
    p28672,
    p32768,
    p40960,
    p49152,
    p57344,
    p65536,
    infinity,
    spare8,
    spare7,
    spare6,
    spare5,
    spare4,
    spare3,
    spare2,
    spare1,
    nulltype
  } value;
  typedef int32_t number_type;

  std::string to_string() const;
  int32_t     to_number() const;
};
typedef enumerated<poll_pdu_opts> poll_pdu_e;

// RateMatchPatternLTE-CRS ::= SEQUENCE
struct rate_match_pattern_lte_crs_s {
  struct carrier_bw_dl_opts {
    enum options { n6, n15, n25, n50, n75, n100, spare2, spare1, nulltype } value;
    typedef uint8_t number_type;

    std::string to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<carrier_bw_dl_opts> carrier_bw_dl_e_;
  struct nrof_crs_ports_opts {
    enum options { n1, n2, n4, nulltype } value;
    typedef uint8_t number_type;

    std::string to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<nrof_crs_ports_opts> nrof_crs_ports_e_;
  struct v_shift_opts {
    enum options { n0, n1, n2, n3, n4, n5, nulltype } value;
    typedef uint8_t number_type;

    std::string to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<v_shift_opts> v_shift_e_;

  // member variables
  bool                      mbsfn_sf_cfg_list_present = false;
  uint16_t                  carrier_freq_dl           = 0;
  carrier_bw_dl_e_          carrier_bw_dl;
  eutra_mbsfn_sf_cfg_list_l mbsfn_sf_cfg_list;
  nrof_crs_ports_e_         nrof_crs_ports;
  v_shift_e_                v_shift;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SN-FieldLengthAM ::= ENUMERATED
struct sn_field_len_am_opts {
  enum options { size12, size18, nulltype } value;
  typedef uint8_t number_type;

  std::string to_string() const;
  uint8_t     to_number() const;
};
typedef enumerated<sn_field_len_am_opts> sn_field_len_am_e;

// SN-FieldLengthUM ::= ENUMERATED
struct sn_field_len_um_opts {
  enum options { size6, size12, nulltype } value;
  typedef uint8_t number_type;

  std::string to_string() const;
  uint8_t     to_number() const;
};
typedef enumerated<sn_field_len_um_opts> sn_field_len_um_e;

// SRS-CarrierSwitching ::= SEQUENCE
struct srs_carrier_switching_s {
  struct srs_switch_from_carrier_opts {
    enum options { sul, nul, nulltype } value;

    std::string to_string() const;
  };
  typedef enumerated<srs_switch_from_carrier_opts> srs_switch_from_carrier_e_;
  struct srs_tpc_pdcch_group_c_ {
    using type_a_l_ = dyn_array<srs_tpc_pdcch_cfg_s>;
    struct types_opts {
      enum options { type_a, type_b, nulltype } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    srs_tpc_pdcch_group_c_() = default;
    srs_tpc_pdcch_group_c_(const srs_tpc_pdcch_group_c_& other);
    srs_tpc_pdcch_group_c_& operator=(const srs_tpc_pdcch_group_c_& other);
    ~srs_tpc_pdcch_group_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    type_a_l_& type_a()
    {
      assert_choice_type("typeA", type_.to_string(), "srs-TPC-PDCCH-Group");
      return c.get<type_a_l_>();
    }
    srs_tpc_pdcch_cfg_s& type_b()
    {
      assert_choice_type("typeB", type_.to_string(), "srs-TPC-PDCCH-Group");
      return c.get<srs_tpc_pdcch_cfg_s>();
    }
    const type_a_l_& type_a() const
    {
      assert_choice_type("typeA", type_.to_string(), "srs-TPC-PDCCH-Group");
      return c.get<type_a_l_>();
    }
    const srs_tpc_pdcch_cfg_s& type_b() const
    {
      assert_choice_type("typeB", type_.to_string(), "srs-TPC-PDCCH-Group");
      return c.get<srs_tpc_pdcch_cfg_s>();
    }
    type_a_l_& set_type_a()
    {
      set(types::type_a);
      return c.get<type_a_l_>();
    }
    srs_tpc_pdcch_cfg_s& set_type_b()
    {
      set(types::type_b);
      return c.get<srs_tpc_pdcch_cfg_s>();
    }

  private:
    types                                           type_;
    choice_buffer_t<srs_tpc_pdcch_cfg_s, type_a_l_> c;

    void destroy_();
  };
  using monitoring_cells_l_ = bounded_array<uint8_t, 32>;

  // member variables
  bool                       ext                                   = false;
  bool                       srs_switch_from_serv_cell_idx_present = false;
  bool                       srs_tpc_pdcch_group_present           = false;
  bool                       monitoring_cells_present              = false;
  uint8_t                    srs_switch_from_serv_cell_idx         = 0;
  srs_switch_from_carrier_e_ srs_switch_from_carrier;
  srs_tpc_pdcch_group_c_     srs_tpc_pdcch_group;
  monitoring_cells_l_        monitoring_cells;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SlotFormatIndicator ::= SEQUENCE
struct slot_format_ind_s {
  using slot_format_comb_to_add_mod_list_l_ = dyn_array<slot_format_combinations_per_cell_s>;
  using slot_format_comb_to_release_list_l_ = bounded_array<uint8_t, 16>;

  // member variables
  bool                                ext                                      = false;
  bool                                slot_format_comb_to_add_mod_list_present = false;
  bool                                slot_format_comb_to_release_list_present = false;
  uint32_t                            sfi_rnti                                 = 0;
  uint8_t                             dci_payload_size                         = 1;
  slot_format_comb_to_add_mod_list_l_ slot_format_comb_to_add_mod_list;
  slot_format_comb_to_release_list_l_ slot_format_comb_to_release_list;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// T-PollRetransmit ::= ENUMERATED
struct t_poll_retx_opts {
  enum options {
    ms5,
    ms10,
    ms15,
    ms20,
    ms25,
    ms30,
    ms35,
    ms40,
    ms45,
    ms50,
    ms55,
    ms60,
    ms65,
    ms70,
    ms75,
    ms80,
    ms85,
    ms90,
    ms95,
    ms100,
    ms105,
    ms110,
    ms115,
    ms120,
    ms125,
    ms130,
    ms135,
    ms140,
    ms145,
    ms150,
    ms155,
    ms160,
    ms165,
    ms170,
    ms175,
    ms180,
    ms185,
    ms190,
    ms195,
    ms200,
    ms205,
    ms210,
    ms215,
    ms220,
    ms225,
    ms230,
    ms235,
    ms240,
    ms245,
    ms250,
    ms300,
    ms350,
    ms400,
    ms450,
    ms500,
    ms800,
    ms1000,
    ms2000,
    ms4000,
    spare5,
    spare4,
    spare3,
    spare2,
    spare1,
    nulltype
  } value;
  typedef uint16_t number_type;

  std::string to_string() const;
  uint16_t    to_number() const;
};
typedef enumerated<t_poll_retx_opts> t_poll_retx_e;

// T-Reassembly ::= ENUMERATED
struct t_reassembly_opts {
  enum options {
    ms0,
    ms5,
    ms10,
    ms15,
    ms20,
    ms25,
    ms30,
    ms35,
    ms40,
    ms45,
    ms50,
    ms55,
    ms60,
    ms65,
    ms70,
    ms75,
    ms80,
    ms85,
    ms90,
    ms95,
    ms100,
    ms110,
    ms120,
    ms130,
    ms140,
    ms150,
    ms160,
    ms170,
    ms180,
    ms190,
    ms200,
    spare1,
    nulltype
  } value;
  typedef uint8_t number_type;

  std::string to_string() const;
  uint8_t     to_number() const;
};
typedef enumerated<t_reassembly_opts> t_reassembly_e;

// T-StatusProhibit ::= ENUMERATED
struct t_status_prohibit_opts {
  enum options {
    ms0,
    ms5,
    ms10,
    ms15,
    ms20,
    ms25,
    ms30,
    ms35,
    ms40,
    ms45,
    ms50,
    ms55,
    ms60,
    ms65,
    ms70,
    ms75,
    ms80,
    ms85,
    ms90,
    ms95,
    ms100,
    ms105,
    ms110,
    ms115,
    ms120,
    ms125,
    ms130,
    ms135,
    ms140,
    ms145,
    ms150,
    ms155,
    ms160,
    ms165,
    ms170,
    ms175,
    ms180,
    ms185,
    ms190,
    ms195,
    ms200,
    ms205,
    ms210,
    ms215,
    ms220,
    ms225,
    ms230,
    ms235,
    ms240,
    ms245,
    ms250,
    ms300,
    ms350,
    ms400,
    ms450,
    ms500,
    ms800,
    ms1000,
    ms1200,
    ms1600,
    ms2000,
    ms2400,
    spare2,
    spare1,
    nulltype
  } value;
  typedef uint16_t number_type;

  std::string to_string() const;
  uint16_t    to_number() const;
};
typedef enumerated<t_status_prohibit_opts> t_status_prohibit_e;

// TDD-UL-DL-SlotConfig ::= SEQUENCE
struct tdd_ul_dl_slot_cfg_s {
  struct symbols_c_ {
    struct explicit_s_ {
      bool    nrof_dl_symbols_present = false;
      bool    nrof_ul_symbols_present = false;
      uint8_t nrof_dl_symbols         = 1;
      uint8_t nrof_ul_symbols         = 1;
    };
    struct types_opts {
      enum options { all_dl, all_ul, explicit_type, nulltype } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    symbols_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    explicit_s_& explicit_type()
    {
      assert_choice_type("explicit", type_.to_string(), "symbols");
      return c;
    }
    const explicit_s_& explicit_type() const
    {
      assert_choice_type("explicit", type_.to_string(), "symbols");
      return c;
    }
    explicit_s_& set_explicit_type()
    {
      set(types::explicit_type);
      return c;
    }

  private:
    types       type_;
    explicit_s_ c;
  };

  // member variables
  uint16_t   slot_idx = 0;
  symbols_c_ symbols;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UL-DataSplitThreshold ::= ENUMERATED
struct ul_data_split_thres_opts {
  enum options {
    b0,
    b100,
    b200,
    b400,
    b800,
    b1600,
    b3200,
    b6400,
    b12800,
    b25600,
    b51200,
    b102400,
    b204800,
    b409600,
    b819200,
    b1228800,
    b1638400,
    b2457600,
    b3276800,
    b4096000,
    b4915200,
    b5734400,
    b6553600,
    infinity,
    spare8,
    spare7,
    spare6,
    spare5,
    spare4,
    spare3,
    spare2,
    spare1,
    nulltype
  } value;
  typedef int32_t number_type;

  std::string to_string() const;
  int32_t     to_number() const;
};
typedef enumerated<ul_data_split_thres_opts> ul_data_split_thres_e;

// UplinkConfigCommon ::= SEQUENCE
struct ul_cfg_common_s {
  bool               freq_info_ul_present = false;
  bool               init_ul_bwp_present  = false;
  freq_info_ul_s     freq_info_ul;
  bwp_ul_common_s    init_ul_bwp;
  time_align_timer_e dummy;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BWP-Downlink ::= SEQUENCE
struct bwp_dl_s {
  bool            ext                = false;
  bool            bwp_common_present = false;
  bool            bwp_ded_present    = false;
  uint8_t         bwp_id             = 0;
  bwp_dl_common_s bwp_common;
  bwp_dl_ded_s    bwp_ded;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CSI-MeasConfig ::= SEQUENCE
struct csi_meas_cfg_s {
  using nzp_csi_rs_res_to_add_mod_list_l_      = dyn_array<nzp_csi_rs_res_s>;
  using nzp_csi_rs_res_to_release_list_l_      = dyn_array<uint8_t>;
  using nzp_csi_rs_res_set_to_add_mod_list_l_  = dyn_array<nzp_csi_rs_res_set_s>;
  using nzp_csi_rs_res_set_to_release_list_l_  = dyn_array<uint8_t>;
  using csi_im_res_to_add_mod_list_l_          = dyn_array<csi_im_res_s>;
  using csi_im_res_to_release_list_l_          = bounded_array<uint8_t, 32>;
  using csi_im_res_set_to_add_mod_list_l_      = dyn_array<csi_im_res_set_s>;
  using csi_im_res_set_to_release_list_l_      = dyn_array<uint8_t>;
  using csi_ssb_res_set_to_add_mod_list_l_     = dyn_array<csi_ssb_res_set_s>;
  using csi_ssb_res_set_to_add_release_list_l_ = dyn_array<uint8_t>;
  using csi_res_cfg_to_add_mod_list_l_         = dyn_array<csi_res_cfg_s>;
  using csi_res_cfg_to_release_list_l_         = dyn_array<uint8_t>;
  using csi_report_cfg_to_add_mod_list_l_      = dyn_array<csi_report_cfg_s>;
  using csi_report_cfg_to_release_list_l_      = dyn_array<uint8_t>;

  // member variables
  bool                                   ext                                                 = false;
  bool                                   nzp_csi_rs_res_to_add_mod_list_present              = false;
  bool                                   nzp_csi_rs_res_to_release_list_present              = false;
  bool                                   nzp_csi_rs_res_set_to_add_mod_list_present          = false;
  bool                                   nzp_csi_rs_res_set_to_release_list_present          = false;
  bool                                   csi_im_res_to_add_mod_list_present                  = false;
  bool                                   csi_im_res_to_release_list_present                  = false;
  bool                                   csi_im_res_set_to_add_mod_list_present              = false;
  bool                                   csi_im_res_set_to_release_list_present              = false;
  bool                                   csi_ssb_res_set_to_add_mod_list_present             = false;
  bool                                   csi_ssb_res_set_to_add_release_list_present         = false;
  bool                                   csi_res_cfg_to_add_mod_list_present                 = false;
  bool                                   csi_res_cfg_to_release_list_present                 = false;
  bool                                   csi_report_cfg_to_add_mod_list_present              = false;
  bool                                   csi_report_cfg_to_release_list_present              = false;
  bool                                   report_trigger_size_present                         = false;
  bool                                   aperiodic_trigger_state_list_present                = false;
  bool                                   semi_persistent_on_pusch_trigger_state_list_present = false;
  nzp_csi_rs_res_to_add_mod_list_l_      nzp_csi_rs_res_to_add_mod_list;
  nzp_csi_rs_res_to_release_list_l_      nzp_csi_rs_res_to_release_list;
  nzp_csi_rs_res_set_to_add_mod_list_l_  nzp_csi_rs_res_set_to_add_mod_list;
  nzp_csi_rs_res_set_to_release_list_l_  nzp_csi_rs_res_set_to_release_list;
  csi_im_res_to_add_mod_list_l_          csi_im_res_to_add_mod_list;
  csi_im_res_to_release_list_l_          csi_im_res_to_release_list;
  csi_im_res_set_to_add_mod_list_l_      csi_im_res_set_to_add_mod_list;
  csi_im_res_set_to_release_list_l_      csi_im_res_set_to_release_list;
  csi_ssb_res_set_to_add_mod_list_l_     csi_ssb_res_set_to_add_mod_list;
  csi_ssb_res_set_to_add_release_list_l_ csi_ssb_res_set_to_add_release_list;
  csi_res_cfg_to_add_mod_list_l_         csi_res_cfg_to_add_mod_list;
  csi_res_cfg_to_release_list_l_         csi_res_cfg_to_release_list;
  csi_report_cfg_to_add_mod_list_l_      csi_report_cfg_to_add_mod_list;
  csi_report_cfg_to_release_list_l_      csi_report_cfg_to_release_list;
  uint8_t                                report_trigger_size = 0;
  setup_release_c<dyn_seq_of<csi_aperiodic_trigger_state_s, 1, 128> > aperiodic_trigger_state_list;
  setup_release_c<dyn_seq_of<csi_semi_persistent_on_pusch_trigger_state_s, 1, 64> >
      semi_persistent_on_pusch_trigger_state_list;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CipheringAlgorithm ::= ENUMERATED
struct ciphering_algorithm_opts {
  enum options { nea0, nea1, nea2, nea3, spare4, spare3, spare2, spare1, /*...*/ nulltype } value;
  typedef uint8_t number_type;

  std::string to_string() const;
  uint8_t     to_number() const;
};
typedef enumerated<ciphering_algorithm_opts, true> ciphering_algorithm_e;

// CrossCarrierSchedulingConfig ::= SEQUENCE
struct cross_carrier_sched_cfg_s {
  struct sched_cell_info_c_ {
    struct own_s_ {
      bool cif_presence = false;
    };
    struct other_s_ {
      uint8_t sched_cell_id     = 0;
      uint8_t cif_in_sched_cell = 1;
    };
    struct types_opts {
      enum options { own, other, nulltype } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    sched_cell_info_c_() = default;
    sched_cell_info_c_(const sched_cell_info_c_& other);
    sched_cell_info_c_& operator=(const sched_cell_info_c_& other);
    ~sched_cell_info_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    own_s_& own()
    {
      assert_choice_type("own", type_.to_string(), "schedulingCellInfo");
      return c.get<own_s_>();
    }
    other_s_& other()
    {
      assert_choice_type("other", type_.to_string(), "schedulingCellInfo");
      return c.get<other_s_>();
    }
    const own_s_& own() const
    {
      assert_choice_type("own", type_.to_string(), "schedulingCellInfo");
      return c.get<own_s_>();
    }
    const other_s_& other() const
    {
      assert_choice_type("other", type_.to_string(), "schedulingCellInfo");
      return c.get<other_s_>();
    }
    own_s_& set_own()
    {
      set(types::own);
      return c.get<own_s_>();
    }
    other_s_& set_other()
    {
      set(types::other);
      return c.get<other_s_>();
    }

  private:
    types                             type_;
    choice_buffer_t<other_s_, own_s_> c;

    void destroy_();
  };

  // member variables
  bool               ext = false;
  sched_cell_info_c_ sched_cell_info;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DL-AM-RLC ::= SEQUENCE
struct dl_am_rlc_s {
  bool                sn_field_len_present = false;
  sn_field_len_am_e   sn_field_len;
  t_reassembly_e      t_reassembly;
  t_status_prohibit_e t_status_prohibit;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DL-UM-RLC ::= SEQUENCE
struct dl_um_rlc_s {
  bool              sn_field_len_present = false;
  sn_field_len_um_e sn_field_len;
  t_reassembly_e    t_reassembly;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// IntegrityProtAlgorithm ::= ENUMERATED
struct integrity_prot_algorithm_opts {
  enum options { nia0, nia1, nia2, nia3, spare4, spare3, spare2, spare1, /*...*/ nulltype } value;
  typedef uint8_t number_type;

  std::string to_string() const;
  uint8_t     to_number() const;
};
typedef enumerated<integrity_prot_algorithm_opts, true> integrity_prot_algorithm_e;

// PDCCH-ServingCellConfig ::= SEQUENCE
struct pdcch_serving_cell_cfg_s {
  bool                               ext                     = false;
  bool                               slot_format_ind_present = false;
  setup_release_c<slot_format_ind_s> slot_format_ind;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDCP-Config ::= SEQUENCE
struct pdcp_cfg_s {
  struct drb_s_ {
    struct discard_timer_opts {
      enum options {
        ms10,
        ms20,
        ms30,
        ms40,
        ms50,
        ms60,
        ms75,
        ms100,
        ms150,
        ms200,
        ms250,
        ms300,
        ms500,
        ms750,
        ms1500,
        infinity,
        nulltype
      } value;
      typedef int16_t number_type;

      std::string to_string() const;
      int16_t     to_number() const;
    };
    typedef enumerated<discard_timer_opts> discard_timer_e_;
    struct pdcp_sn_size_ul_opts {
      enum options { len12bits, len18bits, nulltype } value;
      typedef uint8_t number_type;

      std::string to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<pdcp_sn_size_ul_opts> pdcp_sn_size_ul_e_;
    struct pdcp_sn_size_dl_opts {
      enum options { len12bits, len18bits, nulltype } value;
      typedef uint8_t number_type;

      std::string to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<pdcp_sn_size_dl_opts> pdcp_sn_size_dl_e_;
    struct hdr_compress_c_ {
      struct rohc_s_ {
        struct profiles_s_ {
          bool profile0x0001 = false;
          bool profile0x0002 = false;
          bool profile0x0003 = false;
          bool profile0x0004 = false;
          bool profile0x0006 = false;
          bool profile0x0101 = false;
          bool profile0x0102 = false;
          bool profile0x0103 = false;
          bool profile0x0104 = false;
        };

        // member variables
        bool        max_cid_present           = false;
        bool        drb_continue_rohc_present = false;
        uint16_t    max_cid                   = 1;
        profiles_s_ profiles;
      };
      struct ul_only_rohc_s_ {
        struct profiles_s_ {
          bool profile0x0006 = false;
        };

        // member variables
        bool        max_cid_present           = false;
        bool        drb_continue_rohc_present = false;
        uint16_t    max_cid                   = 1;
        profiles_s_ profiles;
      };
      struct types_opts {
        enum options { not_used, rohc, ul_only_rohc, /*...*/ nulltype } value;

        std::string to_string() const;
      };
      typedef enumerated<types_opts, true> types;

      // choice methods
      hdr_compress_c_() = default;
      hdr_compress_c_(const hdr_compress_c_& other);
      hdr_compress_c_& operator=(const hdr_compress_c_& other);
      ~hdr_compress_c_() { destroy_(); }
      void        set(types::options e = types::nulltype);
      types       type() const { return type_; }
      SRSASN_CODE pack(bit_ref& bref) const;
      SRSASN_CODE unpack(cbit_ref& bref);
      void        to_json(json_writer& j) const;
      // getters
      rohc_s_& rohc()
      {
        assert_choice_type("rohc", type_.to_string(), "headerCompression");
        return c.get<rohc_s_>();
      }
      ul_only_rohc_s_& ul_only_rohc()
      {
        assert_choice_type("uplinkOnlyROHC", type_.to_string(), "headerCompression");
        return c.get<ul_only_rohc_s_>();
      }
      const rohc_s_& rohc() const
      {
        assert_choice_type("rohc", type_.to_string(), "headerCompression");
        return c.get<rohc_s_>();
      }
      const ul_only_rohc_s_& ul_only_rohc() const
      {
        assert_choice_type("uplinkOnlyROHC", type_.to_string(), "headerCompression");
        return c.get<ul_only_rohc_s_>();
      }
      rohc_s_& set_rohc()
      {
        set(types::rohc);
        return c.get<rohc_s_>();
      }
      ul_only_rohc_s_& set_ul_only_rohc()
      {
        set(types::ul_only_rohc);
        return c.get<ul_only_rohc_s_>();
      }

    private:
      types                                     type_;
      choice_buffer_t<rohc_s_, ul_only_rohc_s_> c;

      void destroy_();
    };

    // member variables
    bool               discard_timer_present          = false;
    bool               pdcp_sn_size_ul_present        = false;
    bool               pdcp_sn_size_dl_present        = false;
    bool               integrity_protection_present   = false;
    bool               status_report_required_present = false;
    bool               out_of_order_delivery_present  = false;
    discard_timer_e_   discard_timer;
    pdcp_sn_size_ul_e_ pdcp_sn_size_ul;
    pdcp_sn_size_dl_e_ pdcp_sn_size_dl;
    hdr_compress_c_    hdr_compress;
  };
  struct more_than_one_rlc_s_ {
    struct primary_path_s_ {
      bool    cell_group_present = false;
      bool    lc_ch_present      = false;
      uint8_t cell_group         = 0;
      uint8_t lc_ch              = 1;
    };

    // member variables
    bool                  ul_data_split_thres_present = false;
    bool                  pdcp_dupl_present           = false;
    primary_path_s_       primary_path;
    ul_data_split_thres_e ul_data_split_thres;
    bool                  pdcp_dupl = false;
  };
  struct t_reordering_opts {
    enum options {
      ms0,
      ms1,
      ms2,
      ms4,
      ms5,
      ms8,
      ms10,
      ms15,
      ms20,
      ms30,
      ms40,
      ms50,
      ms60,
      ms80,
      ms100,
      ms120,
      ms140,
      ms160,
      ms180,
      ms200,
      ms220,
      ms240,
      ms260,
      ms280,
      ms300,
      ms500,
      ms750,
      ms1000,
      ms1250,
      ms1500,
      ms1750,
      ms2000,
      ms2250,
      ms2500,
      ms2750,
      ms3000,
      spare28,
      spare27,
      spare26,
      spare25,
      spare24,
      spare23,
      spare22,
      spare21,
      spare20,
      spare19,
      spare18,
      spare17,
      spare16,
      spare15,
      spare14,
      spare13,
      spare12,
      spare11,
      spare10,
      spare09,
      spare08,
      spare07,
      spare06,
      spare05,
      spare04,
      spare03,
      spare02,
      spare01,
      nulltype
    } value;
    typedef uint16_t number_type;

    std::string to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<t_reordering_opts> t_reordering_e_;

  // member variables
  bool                 ext                       = false;
  bool                 drb_present               = false;
  bool                 more_than_one_rlc_present = false;
  bool                 t_reordering_present      = false;
  drb_s_               drb;
  more_than_one_rlc_s_ more_than_one_rlc;
  t_reordering_e_      t_reordering;
  // ...
  // group 0
  bool ciphering_disabled_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDSCH-ServingCellConfig ::= SEQUENCE
struct pdsch_serving_cell_cfg_s {
  struct xoverhead_opts {
    enum options { xoh6, xoh12, xoh18, nulltype } value;
    typedef uint8_t number_type;

    std::string to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<xoverhead_opts> xoverhead_e_;
  struct nrof_harq_processes_for_pdsch_opts {
    enum options { n2, n4, n6, n10, n12, n16, nulltype } value;
    typedef uint8_t number_type;

    std::string to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<nrof_harq_processes_for_pdsch_opts> nrof_harq_processes_for_pdsch_e_;

  // member variables
  bool                                         ext                                   = false;
  bool                                         code_block_group_tx_present           = false;
  bool                                         xoverhead_present                     = false;
  bool                                         nrof_harq_processes_for_pdsch_present = false;
  bool                                         pucch_cell_present                    = false;
  setup_release_c<pdsch_code_block_group_tx_s> code_block_group_tx;
  xoverhead_e_                                 xoverhead;
  nrof_harq_processes_for_pdsch_e_             nrof_harq_processes_for_pdsch;
  uint8_t                                      pucch_cell = 0;
  // ...
  // group 0
  bool    max_mimo_layers_present          = false;
  bool    processing_type2_enabled_present = false;
  uint8_t max_mimo_layers                  = 1;
  bool    processing_type2_enabled         = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RACH-ConfigDedicated ::= SEQUENCE
struct rach_cfg_ded_s {
  bool                ext                       = false;
  bool                cfra_present              = false;
  bool                ra_prioritization_present = false;
  cfra_s              cfra;
  ra_prioritization_s ra_prioritization;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SDAP-Config ::= SEQUENCE
struct sdap_cfg_s {
  struct sdap_hdr_dl_opts {
    enum options { present, absent, nulltype } value;

    std::string to_string() const;
  };
  typedef enumerated<sdap_hdr_dl_opts> sdap_hdr_dl_e_;
  struct sdap_hdr_ul_opts {
    enum options { present, absent, nulltype } value;

    std::string to_string() const;
  };
  typedef enumerated<sdap_hdr_ul_opts> sdap_hdr_ul_e_;
  using mapped_qos_flows_to_add_l_     = dyn_array<uint8_t>;
  using mapped_qos_flows_to_release_l_ = dyn_array<uint8_t>;

  // member variables
  bool                           ext                                 = false;
  bool                           mapped_qos_flows_to_add_present     = false;
  bool                           mapped_qos_flows_to_release_present = false;
  uint16_t                       pdu_session                         = 0;
  sdap_hdr_dl_e_                 sdap_hdr_dl;
  sdap_hdr_ul_e_                 sdap_hdr_ul;
  bool                           default_drb = false;
  mapped_qos_flows_to_add_l_     mapped_qos_flows_to_add;
  mapped_qos_flows_to_release_l_ mapped_qos_flows_to_release;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SchedulingRequestToAddMod ::= SEQUENCE
struct sched_request_to_add_mod_s {
  struct sr_prohibit_timer_opts {
    enum options { ms1, ms2, ms4, ms8, ms16, ms32, ms64, ms128, nulltype } value;
    typedef uint8_t number_type;

    std::string to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<sr_prohibit_timer_opts> sr_prohibit_timer_e_;
  struct sr_trans_max_opts {
    enum options { n4, n8, n16, n32, n64, spare3, spare2, spare1, nulltype } value;
    typedef uint8_t number_type;

    std::string to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<sr_trans_max_opts> sr_trans_max_e_;

  // member variables
  bool                 sr_prohibit_timer_present = false;
  uint8_t              sched_request_id          = 0;
  sr_prohibit_timer_e_ sr_prohibit_timer;
  sr_trans_max_e_      sr_trans_max;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ServingCellConfigCommon ::= SEQUENCE
struct serving_cell_cfg_common_s {
  struct n_timing_advance_offset_opts {
    enum options { n0, n25600, n39936, nulltype } value;
    typedef uint16_t number_type;

    std::string to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<n_timing_advance_offset_opts> n_timing_advance_offset_e_;
  struct ssb_positions_in_burst_c_ {
    struct types_opts {
      enum options { short_bitmap, medium_bitmap, long_bitmap, nulltype } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    ssb_positions_in_burst_c_() = default;
    ssb_positions_in_burst_c_(const ssb_positions_in_burst_c_& other);
    ssb_positions_in_burst_c_& operator=(const ssb_positions_in_burst_c_& other);
    ~ssb_positions_in_burst_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    fixed_bitstring<4>& short_bitmap()
    {
      assert_choice_type("shortBitmap", type_.to_string(), "ssb-PositionsInBurst");
      return c.get<fixed_bitstring<4> >();
    }
    fixed_bitstring<8>& medium_bitmap()
    {
      assert_choice_type("mediumBitmap", type_.to_string(), "ssb-PositionsInBurst");
      return c.get<fixed_bitstring<8> >();
    }
    fixed_bitstring<64>& long_bitmap()
    {
      assert_choice_type("longBitmap", type_.to_string(), "ssb-PositionsInBurst");
      return c.get<fixed_bitstring<64> >();
    }
    const fixed_bitstring<4>& short_bitmap() const
    {
      assert_choice_type("shortBitmap", type_.to_string(), "ssb-PositionsInBurst");
      return c.get<fixed_bitstring<4> >();
    }
    const fixed_bitstring<8>& medium_bitmap() const
    {
      assert_choice_type("mediumBitmap", type_.to_string(), "ssb-PositionsInBurst");
      return c.get<fixed_bitstring<8> >();
    }
    const fixed_bitstring<64>& long_bitmap() const
    {
      assert_choice_type("longBitmap", type_.to_string(), "ssb-PositionsInBurst");
      return c.get<fixed_bitstring<64> >();
    }
    fixed_bitstring<4>& set_short_bitmap()
    {
      set(types::short_bitmap);
      return c.get<fixed_bitstring<4> >();
    }
    fixed_bitstring<8>& set_medium_bitmap()
    {
      set(types::medium_bitmap);
      return c.get<fixed_bitstring<8> >();
    }
    fixed_bitstring<64>& set_long_bitmap()
    {
      set(types::long_bitmap);
      return c.get<fixed_bitstring<64> >();
    }

  private:
    types                                 type_;
    choice_buffer_t<fixed_bitstring<64> > c;

    void destroy_();
  };
  struct ssb_periodicity_serving_cell_opts {
    enum options { ms5, ms10, ms20, ms40, ms80, ms160, spare2, spare1, nulltype } value;
    typedef uint8_t number_type;

    std::string to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<ssb_periodicity_serving_cell_opts> ssb_periodicity_serving_cell_e_;
  struct dmrs_type_a_position_opts {
    enum options { pos2, pos3, nulltype } value;
    typedef uint8_t number_type;

    std::string to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<dmrs_type_a_position_opts> dmrs_type_a_position_e_;
  using rate_match_pattern_to_add_mod_list_l_ = dyn_array<rate_match_pattern_s>;
  using rate_match_pattern_to_release_list_l_ = bounded_array<uint8_t, 4>;

  // member variables
  bool                                          ext                                        = false;
  bool                                          pci_present                                = false;
  bool                                          dl_cfg_common_present                      = false;
  bool                                          ul_cfg_common_present                      = false;
  bool                                          supplementary_ul_cfg_present               = false;
  bool                                          n_timing_advance_offset_present            = false;
  bool                                          ssb_positions_in_burst_present             = false;
  bool                                          ssb_periodicity_serving_cell_present       = false;
  bool                                          lte_crs_to_match_around_present            = false;
  bool                                          rate_match_pattern_to_add_mod_list_present = false;
  bool                                          rate_match_pattern_to_release_list_present = false;
  bool                                          ssb_subcarrier_spacing_present             = false;
  bool                                          tdd_ul_dl_cfg_common_present               = false;
  uint16_t                                      pci                                        = 0;
  dl_cfg_common_s                               dl_cfg_common;
  ul_cfg_common_s                               ul_cfg_common;
  ul_cfg_common_s                               supplementary_ul_cfg;
  n_timing_advance_offset_e_                    n_timing_advance_offset;
  ssb_positions_in_burst_c_                     ssb_positions_in_burst;
  ssb_periodicity_serving_cell_e_               ssb_periodicity_serving_cell;
  dmrs_type_a_position_e_                       dmrs_type_a_position;
  setup_release_c<rate_match_pattern_lte_crs_s> lte_crs_to_match_around;
  rate_match_pattern_to_add_mod_list_l_         rate_match_pattern_to_add_mod_list;
  rate_match_pattern_to_release_list_l_         rate_match_pattern_to_release_list;
  subcarrier_spacing_e                          ssb_subcarrier_spacing;
  tdd_ul_dl_cfg_common_s                        tdd_ul_dl_cfg_common;
  int8_t                                        ss_pbch_block_pwr = -60;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// TAG ::= SEQUENCE
struct tag_s {
  bool               ext    = false;
  uint8_t            tag_id = 0;
  time_align_timer_e time_align_timer;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// TDD-UL-DL-ConfigDedicated ::= SEQUENCE
struct tdd_ul_dl_cfg_ded_s {
  using slot_specific_cfgs_to_add_mod_list_l_ = dyn_array<tdd_ul_dl_slot_cfg_s>;
  using slot_specific_cfgs_torelease_list_l_  = dyn_array<uint16_t>;

  // member variables
  bool                                  ext                                        = false;
  bool                                  slot_specific_cfgs_to_add_mod_list_present = false;
  bool                                  slot_specific_cfgs_torelease_list_present  = false;
  slot_specific_cfgs_to_add_mod_list_l_ slot_specific_cfgs_to_add_mod_list;
  slot_specific_cfgs_torelease_list_l_  slot_specific_cfgs_torelease_list;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UL-AM-RLC ::= SEQUENCE
struct ul_am_rlc_s {
  struct max_retx_thres_opts {
    enum options { t1, t2, t3, t4, t6, t8, t16, t32, nulltype } value;
    typedef uint8_t number_type;

    std::string to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<max_retx_thres_opts> max_retx_thres_e_;

  // member variables
  bool              sn_field_len_present = false;
  sn_field_len_am_e sn_field_len;
  t_poll_retx_e     t_poll_retx;
  poll_pdu_e        poll_pdu;
  poll_byte_e       poll_byte;
  max_retx_thres_e_ max_retx_thres;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UL-UM-RLC ::= SEQUENCE
struct ul_um_rlc_s {
  bool              sn_field_len_present = false;
  sn_field_len_um_e sn_field_len;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UplinkConfig ::= SEQUENCE
struct ul_cfg_s {
  using ul_bwp_to_release_list_l_ = bounded_array<uint8_t, 4>;
  using ul_bwp_to_add_mod_list_l_ = dyn_array<bwp_ul_s>;
  using ul_ch_bw_per_scs_list_l_  = dyn_array<scs_specific_carrier_s>;

  // member variables
  bool                                      ext                            = false;
  bool                                      init_ul_bwp_present            = false;
  bool                                      ul_bwp_to_release_list_present = false;
  bool                                      ul_bwp_to_add_mod_list_present = false;
  bool                                      first_active_ul_bwp_id_present = false;
  bool                                      pusch_serving_cell_cfg_present = false;
  bool                                      carrier_switching_present      = false;
  bwp_ul_ded_s                              init_ul_bwp;
  ul_bwp_to_release_list_l_                 ul_bwp_to_release_list;
  ul_bwp_to_add_mod_list_l_                 ul_bwp_to_add_mod_list;
  uint8_t                                   first_active_ul_bwp_id = 0;
  setup_release_c<pusch_serving_cell_cfg_s> pusch_serving_cell_cfg;
  setup_release_c<srs_carrier_switching_s>  carrier_switching;
  // ...
  // group 0
  bool                               pwr_boost_pi2_bpsk_present = false;
  bool                               pwr_boost_pi2_bpsk         = false;
  copy_ptr<ul_ch_bw_per_scs_list_l_> ul_ch_bw_per_scs_list;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BSR-Config ::= SEQUENCE
struct bsr_cfg_s {
  struct periodic_bsr_timer_opts {
    enum options {
      sf1,
      sf5,
      sf10,
      sf16,
      sf20,
      sf32,
      sf40,
      sf64,
      sf80,
      sf128,
      sf160,
      sf320,
      sf640,
      sf1280,
      sf2560,
      infinity,
      nulltype
    } value;
    typedef int16_t number_type;

    std::string to_string() const;
    int16_t     to_number() const;
  };
  typedef enumerated<periodic_bsr_timer_opts> periodic_bsr_timer_e_;
  struct retx_bsr_timer_opts {
    enum options {
      sf10,
      sf20,
      sf40,
      sf80,
      sf160,
      sf320,
      sf640,
      sf1280,
      sf2560,
      sf5120,
      sf10240,
      spare5,
      spare4,
      spare3,
      spare2,
      spare1,
      nulltype
    } value;
    typedef uint16_t number_type;

    std::string to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<retx_bsr_timer_opts> retx_bsr_timer_e_;
  struct lc_ch_sr_delay_timer_opts {
    enum options { sf20, sf40, sf64, sf128, sf512, sf1024, sf2560, spare1, nulltype } value;
    typedef uint16_t number_type;

    std::string to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<lc_ch_sr_delay_timer_opts> lc_ch_sr_delay_timer_e_;

  // member variables
  bool                    ext                          = false;
  bool                    lc_ch_sr_delay_timer_present = false;
  periodic_bsr_timer_e_   periodic_bsr_timer;
  retx_bsr_timer_e_       retx_bsr_timer;
  lc_ch_sr_delay_timer_e_ lc_ch_sr_delay_timer;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DRB-ToAddMod ::= SEQUENCE
struct drb_to_add_mod_s {
  struct cn_assoc_c_ {
    struct types_opts {
      enum options { eps_bearer_id, sdap_cfg, nulltype } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    cn_assoc_c_() = default;
    cn_assoc_c_(const cn_assoc_c_& other);
    cn_assoc_c_& operator=(const cn_assoc_c_& other);
    ~cn_assoc_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint8_t& eps_bearer_id()
    {
      assert_choice_type("eps-BearerIdentity", type_.to_string(), "cnAssociation");
      return c.get<uint8_t>();
    }
    sdap_cfg_s& sdap_cfg()
    {
      assert_choice_type("sdap-Config", type_.to_string(), "cnAssociation");
      return c.get<sdap_cfg_s>();
    }
    const uint8_t& eps_bearer_id() const
    {
      assert_choice_type("eps-BearerIdentity", type_.to_string(), "cnAssociation");
      return c.get<uint8_t>();
    }
    const sdap_cfg_s& sdap_cfg() const
    {
      assert_choice_type("sdap-Config", type_.to_string(), "cnAssociation");
      return c.get<sdap_cfg_s>();
    }
    uint8_t& set_eps_bearer_id()
    {
      set(types::eps_bearer_id);
      return c.get<uint8_t>();
    }
    sdap_cfg_s& set_sdap_cfg()
    {
      set(types::sdap_cfg);
      return c.get<sdap_cfg_s>();
    }

  private:
    types                       type_;
    choice_buffer_t<sdap_cfg_s> c;

    void destroy_();
  };

  // member variables
  bool        ext                      = false;
  bool        cn_assoc_present         = false;
  bool        reestablish_pdcp_present = false;
  bool        recover_pdcp_present     = false;
  bool        pdcp_cfg_present         = false;
  cn_assoc_c_ cn_assoc;
  uint8_t     drb_id = 1;
  pdcp_cfg_s  pdcp_cfg;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DRX-Config ::= SEQUENCE
struct drx_cfg_s {
  struct drx_on_dur_timer_c_ {
    struct milli_seconds_opts {
      enum options {
        ms1,
        ms2,
        ms3,
        ms4,
        ms5,
        ms6,
        ms8,
        ms10,
        ms20,
        ms30,
        ms40,
        ms50,
        ms60,
        ms80,
        ms100,
        ms200,
        ms300,
        ms400,
        ms500,
        ms600,
        ms800,
        ms1000,
        ms1200,
        ms1600,
        spare8,
        spare7,
        spare6,
        spare5,
        spare4,
        spare3,
        spare2,
        spare1,
        nulltype
      } value;
      typedef uint16_t number_type;

      std::string to_string() const;
      uint16_t    to_number() const;
    };
    typedef enumerated<milli_seconds_opts> milli_seconds_e_;
    struct types_opts {
      enum options { sub_milli_seconds, milli_seconds, nulltype } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    drx_on_dur_timer_c_() = default;
    drx_on_dur_timer_c_(const drx_on_dur_timer_c_& other);
    drx_on_dur_timer_c_& operator=(const drx_on_dur_timer_c_& other);
    ~drx_on_dur_timer_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint8_t& sub_milli_seconds()
    {
      assert_choice_type("subMilliSeconds", type_.to_string(), "drx-onDurationTimer");
      return c.get<uint8_t>();
    }
    milli_seconds_e_& milli_seconds()
    {
      assert_choice_type("milliSeconds", type_.to_string(), "drx-onDurationTimer");
      return c.get<milli_seconds_e_>();
    }
    const uint8_t& sub_milli_seconds() const
    {
      assert_choice_type("subMilliSeconds", type_.to_string(), "drx-onDurationTimer");
      return c.get<uint8_t>();
    }
    const milli_seconds_e_& milli_seconds() const
    {
      assert_choice_type("milliSeconds", type_.to_string(), "drx-onDurationTimer");
      return c.get<milli_seconds_e_>();
    }
    uint8_t& set_sub_milli_seconds()
    {
      set(types::sub_milli_seconds);
      return c.get<uint8_t>();
    }
    milli_seconds_e_& set_milli_seconds()
    {
      set(types::milli_seconds);
      return c.get<milli_seconds_e_>();
    }

  private:
    types               type_;
    pod_choice_buffer_t c;

    void destroy_();
  };
  struct drx_inactivity_timer_opts {
    enum options {
      ms0,
      ms1,
      ms2,
      ms3,
      ms4,
      ms5,
      ms6,
      ms8,
      ms10,
      ms20,
      ms30,
      ms40,
      ms50,
      ms60,
      ms80,
      ms100,
      ms200,
      ms300,
      ms500,
      ms750,
      ms1280,
      ms1920,
      ms2560,
      spare9,
      spare8,
      spare7,
      spare6,
      spare5,
      spare4,
      spare3,
      spare2,
      spare1,
      nulltype
    } value;
    typedef uint16_t number_type;

    std::string to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<drx_inactivity_timer_opts> drx_inactivity_timer_e_;
  struct drx_retx_timer_dl_opts {
    enum options {
      sl0,
      sl1,
      sl2,
      sl4,
      sl6,
      sl8,
      sl16,
      sl24,
      sl33,
      sl40,
      sl64,
      sl80,
      sl96,
      sl112,
      sl128,
      sl160,
      sl320,
      spare15,
      spare14,
      spare13,
      spare12,
      spare11,
      spare10,
      spare9,
      spare8,
      spare7,
      spare6,
      spare5,
      spare4,
      spare3,
      spare2,
      spare1,
      nulltype
    } value;
    typedef uint16_t number_type;

    std::string to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<drx_retx_timer_dl_opts> drx_retx_timer_dl_e_;
  struct drx_retx_timer_ul_opts {
    enum options {
      sl0,
      sl1,
      sl2,
      sl4,
      sl6,
      sl8,
      sl16,
      sl24,
      sl33,
      sl40,
      sl64,
      sl80,
      sl96,
      sl112,
      sl128,
      sl160,
      sl320,
      spare15,
      spare14,
      spare13,
      spare12,
      spare11,
      spare10,
      spare9,
      spare8,
      spare7,
      spare6,
      spare5,
      spare4,
      spare3,
      spare2,
      spare1,
      nulltype
    } value;
    typedef uint16_t number_type;

    std::string to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<drx_retx_timer_ul_opts> drx_retx_timer_ul_e_;
  struct drx_long_cycle_start_offset_c_ {
    struct types_opts {
      enum options {
        ms10,
        ms20,
        ms32,
        ms40,
        ms60,
        ms64,
        ms70,
        ms80,
        ms128,
        ms160,
        ms256,
        ms320,
        ms512,
        ms640,
        ms1024,
        ms1280,
        ms2048,
        ms2560,
        ms5120,
        ms10240,
        nulltype
      } value;
      typedef uint16_t number_type;

      std::string to_string() const;
      uint16_t    to_number() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    drx_long_cycle_start_offset_c_() = default;
    drx_long_cycle_start_offset_c_(const drx_long_cycle_start_offset_c_& other);
    drx_long_cycle_start_offset_c_& operator=(const drx_long_cycle_start_offset_c_& other);
    ~drx_long_cycle_start_offset_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint8_t& ms10()
    {
      assert_choice_type("ms10", type_.to_string(), "drx-LongCycleStartOffset");
      return c.get<uint8_t>();
    }
    uint8_t& ms20()
    {
      assert_choice_type("ms20", type_.to_string(), "drx-LongCycleStartOffset");
      return c.get<uint8_t>();
    }
    uint8_t& ms32()
    {
      assert_choice_type("ms32", type_.to_string(), "drx-LongCycleStartOffset");
      return c.get<uint8_t>();
    }
    uint8_t& ms40()
    {
      assert_choice_type("ms40", type_.to_string(), "drx-LongCycleStartOffset");
      return c.get<uint8_t>();
    }
    uint8_t& ms60()
    {
      assert_choice_type("ms60", type_.to_string(), "drx-LongCycleStartOffset");
      return c.get<uint8_t>();
    }
    uint8_t& ms64()
    {
      assert_choice_type("ms64", type_.to_string(), "drx-LongCycleStartOffset");
      return c.get<uint8_t>();
    }
    uint8_t& ms70()
    {
      assert_choice_type("ms70", type_.to_string(), "drx-LongCycleStartOffset");
      return c.get<uint8_t>();
    }
    uint8_t& ms80()
    {
      assert_choice_type("ms80", type_.to_string(), "drx-LongCycleStartOffset");
      return c.get<uint8_t>();
    }
    uint8_t& ms128()
    {
      assert_choice_type("ms128", type_.to_string(), "drx-LongCycleStartOffset");
      return c.get<uint8_t>();
    }
    uint8_t& ms160()
    {
      assert_choice_type("ms160", type_.to_string(), "drx-LongCycleStartOffset");
      return c.get<uint8_t>();
    }
    uint16_t& ms256()
    {
      assert_choice_type("ms256", type_.to_string(), "drx-LongCycleStartOffset");
      return c.get<uint16_t>();
    }
    uint16_t& ms320()
    {
      assert_choice_type("ms320", type_.to_string(), "drx-LongCycleStartOffset");
      return c.get<uint16_t>();
    }
    uint16_t& ms512()
    {
      assert_choice_type("ms512", type_.to_string(), "drx-LongCycleStartOffset");
      return c.get<uint16_t>();
    }
    uint16_t& ms640()
    {
      assert_choice_type("ms640", type_.to_string(), "drx-LongCycleStartOffset");
      return c.get<uint16_t>();
    }
    uint16_t& ms1024()
    {
      assert_choice_type("ms1024", type_.to_string(), "drx-LongCycleStartOffset");
      return c.get<uint16_t>();
    }
    uint16_t& ms1280()
    {
      assert_choice_type("ms1280", type_.to_string(), "drx-LongCycleStartOffset");
      return c.get<uint16_t>();
    }
    uint16_t& ms2048()
    {
      assert_choice_type("ms2048", type_.to_string(), "drx-LongCycleStartOffset");
      return c.get<uint16_t>();
    }
    uint16_t& ms2560()
    {
      assert_choice_type("ms2560", type_.to_string(), "drx-LongCycleStartOffset");
      return c.get<uint16_t>();
    }
    uint16_t& ms5120()
    {
      assert_choice_type("ms5120", type_.to_string(), "drx-LongCycleStartOffset");
      return c.get<uint16_t>();
    }
    uint16_t& ms10240()
    {
      assert_choice_type("ms10240", type_.to_string(), "drx-LongCycleStartOffset");
      return c.get<uint16_t>();
    }
    const uint8_t& ms10() const
    {
      assert_choice_type("ms10", type_.to_string(), "drx-LongCycleStartOffset");
      return c.get<uint8_t>();
    }
    const uint8_t& ms20() const
    {
      assert_choice_type("ms20", type_.to_string(), "drx-LongCycleStartOffset");
      return c.get<uint8_t>();
    }
    const uint8_t& ms32() const
    {
      assert_choice_type("ms32", type_.to_string(), "drx-LongCycleStartOffset");
      return c.get<uint8_t>();
    }
    const uint8_t& ms40() const
    {
      assert_choice_type("ms40", type_.to_string(), "drx-LongCycleStartOffset");
      return c.get<uint8_t>();
    }
    const uint8_t& ms60() const
    {
      assert_choice_type("ms60", type_.to_string(), "drx-LongCycleStartOffset");
      return c.get<uint8_t>();
    }
    const uint8_t& ms64() const
    {
      assert_choice_type("ms64", type_.to_string(), "drx-LongCycleStartOffset");
      return c.get<uint8_t>();
    }
    const uint8_t& ms70() const
    {
      assert_choice_type("ms70", type_.to_string(), "drx-LongCycleStartOffset");
      return c.get<uint8_t>();
    }
    const uint8_t& ms80() const
    {
      assert_choice_type("ms80", type_.to_string(), "drx-LongCycleStartOffset");
      return c.get<uint8_t>();
    }
    const uint8_t& ms128() const
    {
      assert_choice_type("ms128", type_.to_string(), "drx-LongCycleStartOffset");
      return c.get<uint8_t>();
    }
    const uint8_t& ms160() const
    {
      assert_choice_type("ms160", type_.to_string(), "drx-LongCycleStartOffset");
      return c.get<uint8_t>();
    }
    const uint16_t& ms256() const
    {
      assert_choice_type("ms256", type_.to_string(), "drx-LongCycleStartOffset");
      return c.get<uint16_t>();
    }
    const uint16_t& ms320() const
    {
      assert_choice_type("ms320", type_.to_string(), "drx-LongCycleStartOffset");
      return c.get<uint16_t>();
    }
    const uint16_t& ms512() const
    {
      assert_choice_type("ms512", type_.to_string(), "drx-LongCycleStartOffset");
      return c.get<uint16_t>();
    }
    const uint16_t& ms640() const
    {
      assert_choice_type("ms640", type_.to_string(), "drx-LongCycleStartOffset");
      return c.get<uint16_t>();
    }
    const uint16_t& ms1024() const
    {
      assert_choice_type("ms1024", type_.to_string(), "drx-LongCycleStartOffset");
      return c.get<uint16_t>();
    }
    const uint16_t& ms1280() const
    {
      assert_choice_type("ms1280", type_.to_string(), "drx-LongCycleStartOffset");
      return c.get<uint16_t>();
    }
    const uint16_t& ms2048() const
    {
      assert_choice_type("ms2048", type_.to_string(), "drx-LongCycleStartOffset");
      return c.get<uint16_t>();
    }
    const uint16_t& ms2560() const
    {
      assert_choice_type("ms2560", type_.to_string(), "drx-LongCycleStartOffset");
      return c.get<uint16_t>();
    }
    const uint16_t& ms5120() const
    {
      assert_choice_type("ms5120", type_.to_string(), "drx-LongCycleStartOffset");
      return c.get<uint16_t>();
    }
    const uint16_t& ms10240() const
    {
      assert_choice_type("ms10240", type_.to_string(), "drx-LongCycleStartOffset");
      return c.get<uint16_t>();
    }
    uint8_t& set_ms10()
    {
      set(types::ms10);
      return c.get<uint8_t>();
    }
    uint8_t& set_ms20()
    {
      set(types::ms20);
      return c.get<uint8_t>();
    }
    uint8_t& set_ms32()
    {
      set(types::ms32);
      return c.get<uint8_t>();
    }
    uint8_t& set_ms40()
    {
      set(types::ms40);
      return c.get<uint8_t>();
    }
    uint8_t& set_ms60()
    {
      set(types::ms60);
      return c.get<uint8_t>();
    }
    uint8_t& set_ms64()
    {
      set(types::ms64);
      return c.get<uint8_t>();
    }
    uint8_t& set_ms70()
    {
      set(types::ms70);
      return c.get<uint8_t>();
    }
    uint8_t& set_ms80()
    {
      set(types::ms80);
      return c.get<uint8_t>();
    }
    uint8_t& set_ms128()
    {
      set(types::ms128);
      return c.get<uint8_t>();
    }
    uint8_t& set_ms160()
    {
      set(types::ms160);
      return c.get<uint8_t>();
    }
    uint16_t& set_ms256()
    {
      set(types::ms256);
      return c.get<uint16_t>();
    }
    uint16_t& set_ms320()
    {
      set(types::ms320);
      return c.get<uint16_t>();
    }
    uint16_t& set_ms512()
    {
      set(types::ms512);
      return c.get<uint16_t>();
    }
    uint16_t& set_ms640()
    {
      set(types::ms640);
      return c.get<uint16_t>();
    }
    uint16_t& set_ms1024()
    {
      set(types::ms1024);
      return c.get<uint16_t>();
    }
    uint16_t& set_ms1280()
    {
      set(types::ms1280);
      return c.get<uint16_t>();
    }
    uint16_t& set_ms2048()
    {
      set(types::ms2048);
      return c.get<uint16_t>();
    }
    uint16_t& set_ms2560()
    {
      set(types::ms2560);
      return c.get<uint16_t>();
    }
    uint16_t& set_ms5120()
    {
      set(types::ms5120);
      return c.get<uint16_t>();
    }
    uint16_t& set_ms10240()
    {
      set(types::ms10240);
      return c.get<uint16_t>();
    }

  private:
    types               type_;
    pod_choice_buffer_t c;

    void destroy_();
  };
  struct short_drx_s_ {
    struct drx_short_cycle_opts {
      enum options {
        ms2,
        ms3,
        ms4,
        ms5,
        ms6,
        ms7,
        ms8,
        ms10,
        ms14,
        ms16,
        ms20,
        ms30,
        ms32,
        ms35,
        ms40,
        ms64,
        ms80,
        ms128,
        ms160,
        ms256,
        ms320,
        ms512,
        ms640,
        spare9,
        spare8,
        spare7,
        spare6,
        spare5,
        spare4,
        spare3,
        spare2,
        spare1,
        nulltype
      } value;
      typedef uint16_t number_type;

      std::string to_string() const;
      uint16_t    to_number() const;
    };
    typedef enumerated<drx_short_cycle_opts> drx_short_cycle_e_;

    // member variables
    drx_short_cycle_e_ drx_short_cycle;
    uint8_t            drx_short_cycle_timer = 1;
  };

  // member variables
  bool                           short_drx_present = false;
  drx_on_dur_timer_c_            drx_on_dur_timer;
  drx_inactivity_timer_e_        drx_inactivity_timer;
  uint8_t                        drx_harq_rtt_timer_dl = 0;
  uint8_t                        drx_harq_rtt_timer_ul = 0;
  drx_retx_timer_dl_e_           drx_retx_timer_dl;
  drx_retx_timer_ul_e_           drx_retx_timer_ul;
  drx_long_cycle_start_offset_c_ drx_long_cycle_start_offset;
  short_drx_s_                   short_drx;
  uint8_t                        drx_slot_offset = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DataInactivityTimer ::= ENUMERATED
struct data_inactivity_timer_opts {
  enum options { s1, s2, s3, s5, s7, s10, s15, s20, s40, s50, s60, s80, s100, s120, s150, s180, nulltype } value;
  typedef uint8_t number_type;

  std::string to_string() const;
  uint8_t     to_number() const;
};
typedef enumerated<data_inactivity_timer_opts> data_inactivity_timer_e;

// LogicalChannelConfig ::= SEQUENCE
struct lc_ch_cfg_s {
  struct ul_specific_params_s_ {
    struct prioritised_bit_rate_opts {
      enum options {
        kbps0,
        kbps8,
        kbps16,
        kbps32,
        kbps64,
        kbps128,
        kbps256,
        kbps512,
        kbps1024,
        kbps2048,
        kbps4096,
        kbps8192,
        kbps16384,
        kbps32768,
        kbps65536,
        infinity,
        nulltype
      } value;
      typedef int32_t number_type;

      std::string to_string() const;
      int32_t     to_number() const;
    };
    typedef enumerated<prioritised_bit_rate_opts> prioritised_bit_rate_e_;
    struct bucket_size_dur_opts {
      enum options {
        ms5,
        ms10,
        ms20,
        ms50,
        ms100,
        ms150,
        ms300,
        ms500,
        ms1000,
        spare7,
        spare6,
        spare5,
        spare4,
        spare3,
        spare2,
        spare1,
        nulltype
      } value;
      typedef uint16_t number_type;

      std::string to_string() const;
      uint16_t    to_number() const;
    };
    typedef enumerated<bucket_size_dur_opts> bucket_size_dur_e_;
    using allowed_serving_cells_l_ = bounded_array<uint8_t, 31>;
    using allowed_scs_list_l_      = bounded_array<subcarrier_spacing_e, 5>;
    struct max_pusch_dur_opts {
      enum options { ms0p02, ms0p04, ms0p0625, ms0p125, ms0p25, ms0p5, spare2, spare1, nulltype } value;

      std::string to_string() const;
    };
    typedef enumerated<max_pusch_dur_opts> max_pusch_dur_e_;
    struct bit_rate_query_prohibit_timer_opts {
      enum options { s0, s0dot4, s0dot8, s1dot6, s3, s6, s12, s30, nulltype } value;
      typedef float number_type;

      std::string to_string() const;
      float       to_number() const;
      std::string to_number_string() const;
    };
    typedef enumerated<bit_rate_query_prohibit_timer_opts> bit_rate_query_prohibit_timer_e_;

    // member variables
    bool                     ext                                 = false;
    bool                     allowed_serving_cells_present       = false;
    bool                     allowed_scs_list_present            = false;
    bool                     max_pusch_dur_present               = false;
    bool                     cfgured_grant_type1_allowed_present = false;
    bool                     lc_ch_group_present                 = false;
    bool                     sched_request_id_present            = false;
    uint8_t                  prio                                = 1;
    prioritised_bit_rate_e_  prioritised_bit_rate;
    bucket_size_dur_e_       bucket_size_dur;
    allowed_serving_cells_l_ allowed_serving_cells;
    allowed_scs_list_l_      allowed_scs_list;
    max_pusch_dur_e_         max_pusch_dur;
    uint8_t                  lc_ch_group                  = 0;
    uint8_t                  sched_request_id             = 0;
    bool                     lc_ch_sr_mask                = false;
    bool                     lc_ch_sr_delay_timer_applied = false;
    // ...
    bool                             bit_rate_query_prohibit_timer_present = false;
    bit_rate_query_prohibit_timer_e_ bit_rate_query_prohibit_timer;

    // sequence methods
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
  };

  // member variables
  bool                  ext                        = false;
  bool                  ul_specific_params_present = false;
  ul_specific_params_s_ ul_specific_params;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PHR-Config ::= SEQUENCE
struct phr_cfg_s {
  struct phr_periodic_timer_opts {
    enum options { sf10, sf20, sf50, sf100, sf200, sf500, sf1000, infinity, nulltype } value;
    typedef int16_t number_type;

    std::string to_string() const;
    int16_t     to_number() const;
  };
  typedef enumerated<phr_periodic_timer_opts> phr_periodic_timer_e_;
  struct phr_prohibit_timer_opts {
    enum options { sf0, sf10, sf20, sf50, sf100, sf200, sf500, sf1000, nulltype } value;
    typedef uint16_t number_type;

    std::string to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<phr_prohibit_timer_opts> phr_prohibit_timer_e_;
  struct phr_tx_pwr_factor_change_opts {
    enum options { db1, db3, db6, infinity, nulltype } value;
    typedef int8_t number_type;

    std::string to_string() const;
    int8_t      to_number() const;
  };
  typedef enumerated<phr_tx_pwr_factor_change_opts> phr_tx_pwr_factor_change_e_;
  struct phr_mode_other_cg_opts {
    enum options { real, virtual_value, nulltype } value;

    std::string to_string() const;
  };
  typedef enumerated<phr_mode_other_cg_opts> phr_mode_other_cg_e_;

  // member variables
  bool                        ext = false;
  phr_periodic_timer_e_       phr_periodic_timer;
  phr_prohibit_timer_e_       phr_prohibit_timer;
  phr_tx_pwr_factor_change_e_ phr_tx_pwr_factor_change;
  bool                        multiple_phr         = false;
  bool                        dummy                = false;
  bool                        phr_type2_other_cell = false;
  phr_mode_other_cg_e_        phr_mode_other_cg;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RLC-Config ::= CHOICE
struct rlc_cfg_c {
  struct am_s_ {
    ul_am_rlc_s ul_am_rlc;
    dl_am_rlc_s dl_am_rlc;
  };
  struct um_bi_dir_s_ {
    ul_um_rlc_s ul_um_rlc;
    dl_um_rlc_s dl_um_rlc;
  };
  struct um_uni_dir_ul_s_ {
    ul_um_rlc_s ul_um_rlc;
  };
  struct um_uni_dir_dl_s_ {
    dl_um_rlc_s dl_um_rlc;
  };
  struct types_opts {
    enum options { am, um_bi_dir, um_uni_dir_ul, um_uni_dir_dl, /*...*/ nulltype } value;

    std::string to_string() const;
  };
  typedef enumerated<types_opts, true> types;

  // choice methods
  rlc_cfg_c() = default;
  rlc_cfg_c(const rlc_cfg_c& other);
  rlc_cfg_c& operator=(const rlc_cfg_c& other);
  ~rlc_cfg_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  am_s_& am()
  {
    assert_choice_type("am", type_.to_string(), "RLC-Config");
    return c.get<am_s_>();
  }
  um_bi_dir_s_& um_bi_dir()
  {
    assert_choice_type("um-Bi-Directional", type_.to_string(), "RLC-Config");
    return c.get<um_bi_dir_s_>();
  }
  um_uni_dir_ul_s_& um_uni_dir_ul()
  {
    assert_choice_type("um-Uni-Directional-UL", type_.to_string(), "RLC-Config");
    return c.get<um_uni_dir_ul_s_>();
  }
  um_uni_dir_dl_s_& um_uni_dir_dl()
  {
    assert_choice_type("um-Uni-Directional-DL", type_.to_string(), "RLC-Config");
    return c.get<um_uni_dir_dl_s_>();
  }
  const am_s_& am() const
  {
    assert_choice_type("am", type_.to_string(), "RLC-Config");
    return c.get<am_s_>();
  }
  const um_bi_dir_s_& um_bi_dir() const
  {
    assert_choice_type("um-Bi-Directional", type_.to_string(), "RLC-Config");
    return c.get<um_bi_dir_s_>();
  }
  const um_uni_dir_ul_s_& um_uni_dir_ul() const
  {
    assert_choice_type("um-Uni-Directional-UL", type_.to_string(), "RLC-Config");
    return c.get<um_uni_dir_ul_s_>();
  }
  const um_uni_dir_dl_s_& um_uni_dir_dl() const
  {
    assert_choice_type("um-Uni-Directional-DL", type_.to_string(), "RLC-Config");
    return c.get<um_uni_dir_dl_s_>();
  }
  am_s_& set_am()
  {
    set(types::am);
    return c.get<am_s_>();
  }
  um_bi_dir_s_& set_um_bi_dir()
  {
    set(types::um_bi_dir);
    return c.get<um_bi_dir_s_>();
  }
  um_uni_dir_ul_s_& set_um_uni_dir_ul()
  {
    set(types::um_uni_dir_ul);
    return c.get<um_uni_dir_ul_s_>();
  }
  um_uni_dir_dl_s_& set_um_uni_dir_dl()
  {
    set(types::um_uni_dir_dl);
    return c.get<um_uni_dir_dl_s_>();
  }

private:
  types                                                                    type_;
  choice_buffer_t<am_s_, um_bi_dir_s_, um_uni_dir_dl_s_, um_uni_dir_ul_s_> c;

  void destroy_();
};

// RLF-TimersAndConstants ::= SEQUENCE
struct rlf_timers_and_consts_s {
  struct t310_opts {
    enum options { ms0, ms50, ms100, ms200, ms500, ms1000, ms2000, ms4000, ms6000, nulltype } value;
    typedef uint16_t number_type;

    std::string to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<t310_opts> t310_e_;
  struct n310_opts {
    enum options { n1, n2, n3, n4, n6, n8, n10, n20, nulltype } value;
    typedef uint8_t number_type;

    std::string to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<n310_opts> n310_e_;
  struct n311_opts {
    enum options { n1, n2, n3, n4, n5, n6, n8, n10, nulltype } value;
    typedef uint8_t number_type;

    std::string to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<n311_opts> n311_e_;
  struct t311_v1530_opts {
    enum options { ms1000, ms3000, ms5000, ms10000, ms15000, ms20000, ms30000, nulltype } value;
    typedef uint16_t number_type;

    std::string to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<t311_v1530_opts> t311_v1530_e_;

  // member variables
  bool    ext = false;
  t310_e_ t310;
  n310_e_ n310;
  n311_e_ n311;
  // ...
  // group 0
  t311_v1530_e_ t311_v1530;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ReconfigurationWithSync ::= SEQUENCE
struct recfg_with_sync_s {
  struct t304_opts {
    enum options { ms50, ms100, ms150, ms200, ms500, ms1000, ms2000, ms10000, nulltype } value;
    typedef uint16_t number_type;

    std::string to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<t304_opts> t304_e_;
  struct rach_cfg_ded_c_ {
    struct types_opts {
      enum options { ul, supplementary_ul, nulltype } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    rach_cfg_ded_c_() = default;
    rach_cfg_ded_c_(const rach_cfg_ded_c_& other);
    rach_cfg_ded_c_& operator=(const rach_cfg_ded_c_& other);
    ~rach_cfg_ded_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    rach_cfg_ded_s& ul()
    {
      assert_choice_type("uplink", type_.to_string(), "rach-ConfigDedicated");
      return c.get<rach_cfg_ded_s>();
    }
    rach_cfg_ded_s& supplementary_ul()
    {
      assert_choice_type("supplementaryUplink", type_.to_string(), "rach-ConfigDedicated");
      return c.get<rach_cfg_ded_s>();
    }
    const rach_cfg_ded_s& ul() const
    {
      assert_choice_type("uplink", type_.to_string(), "rach-ConfigDedicated");
      return c.get<rach_cfg_ded_s>();
    }
    const rach_cfg_ded_s& supplementary_ul() const
    {
      assert_choice_type("supplementaryUplink", type_.to_string(), "rach-ConfigDedicated");
      return c.get<rach_cfg_ded_s>();
    }
    rach_cfg_ded_s& set_ul()
    {
      set(types::ul);
      return c.get<rach_cfg_ded_s>();
    }
    rach_cfg_ded_s& set_supplementary_ul()
    {
      set(types::supplementary_ul);
      return c.get<rach_cfg_ded_s>();
    }

  private:
    types                           type_;
    choice_buffer_t<rach_cfg_ded_s> c;

    void destroy_();
  };

  // member variables
  bool                      ext                        = false;
  bool                      sp_cell_cfg_common_present = false;
  bool                      rach_cfg_ded_present       = false;
  serving_cell_cfg_common_s sp_cell_cfg_common;
  uint32_t                  new_ue_id = 0;
  t304_e_                   t304;
  rach_cfg_ded_c_           rach_cfg_ded;
  // ...
  // group 0
  copy_ptr<ssb_mtc_s> smtc;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SRB-ToAddMod ::= SEQUENCE
struct srb_to_add_mod_s {
  bool       ext                      = false;
  bool       reestablish_pdcp_present = false;
  bool       discard_on_pdcp_present  = false;
  bool       pdcp_cfg_present         = false;
  uint8_t    srb_id                   = 1;
  pdcp_cfg_s pdcp_cfg;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SchedulingRequestConfig ::= SEQUENCE
struct sched_request_cfg_s {
  using sched_request_to_add_mod_list_l_ = dyn_array<sched_request_to_add_mod_s>;
  using sched_request_to_release_list_l_ = bounded_array<uint8_t, 8>;

  // member variables
  bool                             sched_request_to_add_mod_list_present = false;
  bool                             sched_request_to_release_list_present = false;
  sched_request_to_add_mod_list_l_ sched_request_to_add_mod_list;
  sched_request_to_release_list_l_ sched_request_to_release_list;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SecurityAlgorithmConfig ::= SEQUENCE
struct security_algorithm_cfg_s {
  bool                       ext                              = false;
  bool                       integrity_prot_algorithm_present = false;
  ciphering_algorithm_e      ciphering_algorithm;
  integrity_prot_algorithm_e integrity_prot_algorithm;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ServingCellConfig ::= SEQUENCE
struct serving_cell_cfg_s {
  using dl_bwp_to_release_list_l_ = bounded_array<uint8_t, 4>;
  using dl_bwp_to_add_mod_list_l_ = dyn_array<bwp_dl_s>;
  struct bwp_inactivity_timer_opts {
    enum options {
      ms2,
      ms3,
      ms4,
      ms5,
      ms6,
      ms8,
      ms10,
      ms20,
      ms30,
      ms40,
      ms50,
      ms60,
      ms80,
      ms100,
      ms200,
      ms300,
      ms500,
      ms750,
      ms1280,
      ms1920,
      ms2560,
      spare10,
      spare9,
      spare8,
      spare7,
      spare6,
      spare5,
      spare4,
      spare3,
      spare2,
      spare1,
      nulltype
    } value;
    typedef uint16_t number_type;

    std::string to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<bwp_inactivity_timer_opts> bwp_inactivity_timer_e_;
  struct scell_deactivation_timer_opts {
    enum options {
      ms20,
      ms40,
      ms80,
      ms160,
      ms200,
      ms240,
      ms320,
      ms400,
      ms480,
      ms520,
      ms640,
      ms720,
      ms840,
      ms1280,
      spare2,
      spare1,
      nulltype
    } value;
    typedef uint16_t number_type;

    std::string to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<scell_deactivation_timer_opts> scell_deactivation_timer_e_;
  struct pathloss_ref_linking_opts {
    enum options { pcell, scell, nulltype } value;

    std::string to_string() const;
  };
  typedef enumerated<pathloss_ref_linking_opts> pathloss_ref_linking_e_;
  using rate_match_pattern_to_add_mod_list_l_ = dyn_array<rate_match_pattern_s>;
  using rate_match_pattern_to_release_list_l_ = bounded_array<uint8_t, 4>;
  using dl_ch_bw_per_scs_list_l_              = dyn_array<scs_specific_carrier_s>;

  // member variables
  bool                                      ext                              = false;
  bool                                      tdd_ul_dl_cfg_ded_present        = false;
  bool                                      init_dl_bwp_present              = false;
  bool                                      dl_bwp_to_release_list_present   = false;
  bool                                      dl_bwp_to_add_mod_list_present   = false;
  bool                                      first_active_dl_bwp_id_present   = false;
  bool                                      bwp_inactivity_timer_present     = false;
  bool                                      default_dl_bwp_id_present        = false;
  bool                                      ul_cfg_present                   = false;
  bool                                      supplementary_ul_present         = false;
  bool                                      pdcch_serving_cell_cfg_present   = false;
  bool                                      pdsch_serving_cell_cfg_present   = false;
  bool                                      csi_meas_cfg_present             = false;
  bool                                      scell_deactivation_timer_present = false;
  bool                                      cross_carrier_sched_cfg_present  = false;
  bool                                      dummy_present                    = false;
  bool                                      pathloss_ref_linking_present     = false;
  bool                                      serving_cell_mo_present          = false;
  tdd_ul_dl_cfg_ded_s                       tdd_ul_dl_cfg_ded;
  bwp_dl_ded_s                              init_dl_bwp;
  dl_bwp_to_release_list_l_                 dl_bwp_to_release_list;
  dl_bwp_to_add_mod_list_l_                 dl_bwp_to_add_mod_list;
  uint8_t                                   first_active_dl_bwp_id = 0;
  bwp_inactivity_timer_e_                   bwp_inactivity_timer;
  uint8_t                                   default_dl_bwp_id = 0;
  ul_cfg_s                                  ul_cfg;
  ul_cfg_s                                  supplementary_ul;
  setup_release_c<pdcch_serving_cell_cfg_s> pdcch_serving_cell_cfg;
  setup_release_c<pdsch_serving_cell_cfg_s> pdsch_serving_cell_cfg;
  setup_release_c<csi_meas_cfg_s>           csi_meas_cfg;
  scell_deactivation_timer_e_               scell_deactivation_timer;
  cross_carrier_sched_cfg_s                 cross_carrier_sched_cfg;
  uint8_t                                   tag_id = 0;
  pathloss_ref_linking_e_                   pathloss_ref_linking;
  uint8_t                                   serving_cell_mo = 1;
  // ...
  // group 0
  copy_ptr<setup_release_c<rate_match_pattern_lte_crs_s> > lte_crs_to_match_around;
  copy_ptr<rate_match_pattern_to_add_mod_list_l_>          rate_match_pattern_to_add_mod_list;
  copy_ptr<rate_match_pattern_to_release_list_l_>          rate_match_pattern_to_release_list;
  copy_ptr<dl_ch_bw_per_scs_list_l_>                       dl_ch_bw_per_scs_list;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// TAG-Config ::= SEQUENCE
struct tag_cfg_s {
  using tag_to_release_list_l_ = bounded_array<uint8_t, 4>;
  using tag_to_add_mod_list_l_ = dyn_array<tag_s>;

  // member variables
  bool                   tag_to_release_list_present = false;
  bool                   tag_to_add_mod_list_present = false;
  tag_to_release_list_l_ tag_to_release_list;
  tag_to_add_mod_list_l_ tag_to_add_mod_list;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DRB-ToAddModList ::= SEQUENCE (SIZE (1..29)) OF DRB-ToAddMod
using drb_to_add_mod_list_l = dyn_array<drb_to_add_mod_s>;

// DRB-ToReleaseList ::= SEQUENCE (SIZE (1..29)) OF INTEGER (1..32)
using drb_to_release_list_l = bounded_array<uint8_t, 29>;

// MAC-CellGroupConfig ::= SEQUENCE
struct mac_cell_group_cfg_s {
  bool                       ext                       = false;
  bool                       drx_cfg_present           = false;
  bool                       sched_request_cfg_present = false;
  bool                       bsr_cfg_present           = false;
  bool                       tag_cfg_present           = false;
  bool                       phr_cfg_present           = false;
  setup_release_c<drx_cfg_s> drx_cfg;
  sched_request_cfg_s        sched_request_cfg;
  bsr_cfg_s                  bsr_cfg;
  tag_cfg_s                  tag_cfg;
  setup_release_c<phr_cfg_s> phr_cfg;
  bool                       skip_ul_tx_dynamic = false;
  // ...
  // group 0
  bool                                                csi_mask_v1530_present = false;
  bool                                                csi_mask_v1530         = false;
  copy_ptr<setup_release_c<data_inactivity_timer_e> > data_inactivity_timer_v1530;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PhysicalCellGroupConfig ::= SEQUENCE
struct phys_cell_group_cfg_s {
  struct pdsch_harq_ack_codebook_opts {
    enum options { semi_static, dynamic_value, nulltype } value;

    std::string to_string() const;
  };
  typedef enumerated<pdsch_harq_ack_codebook_opts> pdsch_harq_ack_codebook_e_;
  struct xscale_opts {
    enum options { db0, db6, spare2, spare1, nulltype } value;
    typedef uint8_t number_type;

    std::string to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<xscale_opts> xscale_e_;

  // member variables
  bool                                          ext                                     = false;
  bool                                          harq_ack_spatial_bundling_pucch_present = false;
  bool                                          harq_ack_spatial_bundling_pusch_present = false;
  bool                                          p_nr_fr1_present                        = false;
  bool                                          tpc_srs_rnti_present                    = false;
  bool                                          tpc_pucch_rnti_present                  = false;
  bool                                          tpc_pusch_rnti_present                  = false;
  bool                                          sp_csi_rnti_present                     = false;
  bool                                          cs_rnti_present                         = false;
  int8_t                                        p_nr_fr1                                = -30;
  pdsch_harq_ack_codebook_e_                    pdsch_harq_ack_codebook;
  uint32_t                                      tpc_srs_rnti   = 0;
  uint32_t                                      tpc_pucch_rnti = 0;
  uint32_t                                      tpc_pusch_rnti = 0;
  uint32_t                                      sp_csi_rnti    = 0;
  setup_release_c<integer<uint32_t, 0, 65535> > cs_rnti;
  // ...
  // group 0
  bool     mcs_c_rnti_present = false;
  bool     p_ue_fr1_present   = false;
  uint32_t mcs_c_rnti         = 0;
  int8_t   p_ue_fr1           = -30;
  // group 1
  bool      xscale_present = false;
  xscale_e_ xscale;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RLC-BearerConfig ::= SEQUENCE
struct rlc_bearer_cfg_s {
  struct served_radio_bearer_c_ {
    struct types_opts {
      enum options { srb_id, drb_id, nulltype } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    served_radio_bearer_c_() = default;
    served_radio_bearer_c_(const served_radio_bearer_c_& other);
    served_radio_bearer_c_& operator=(const served_radio_bearer_c_& other);
    ~served_radio_bearer_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint8_t& srb_id()
    {
      assert_choice_type("srb-Identity", type_.to_string(), "servedRadioBearer");
      return c.get<uint8_t>();
    }
    uint8_t& drb_id()
    {
      assert_choice_type("drb-Identity", type_.to_string(), "servedRadioBearer");
      return c.get<uint8_t>();
    }
    const uint8_t& srb_id() const
    {
      assert_choice_type("srb-Identity", type_.to_string(), "servedRadioBearer");
      return c.get<uint8_t>();
    }
    const uint8_t& drb_id() const
    {
      assert_choice_type("drb-Identity", type_.to_string(), "servedRadioBearer");
      return c.get<uint8_t>();
    }
    uint8_t& set_srb_id()
    {
      set(types::srb_id);
      return c.get<uint8_t>();
    }
    uint8_t& set_drb_id()
    {
      set(types::drb_id);
      return c.get<uint8_t>();
    }

  private:
    types               type_;
    pod_choice_buffer_t c;

    void destroy_();
  };

  // member variables
  bool                   ext                         = false;
  bool                   served_radio_bearer_present = false;
  bool                   reestablish_rlc_present     = false;
  bool                   rlc_cfg_present             = false;
  bool                   mac_lc_ch_cfg_present       = false;
  uint8_t                lc_ch_id                    = 1;
  served_radio_bearer_c_ served_radio_bearer;
  rlc_cfg_c              rlc_cfg;
  lc_ch_cfg_s            mac_lc_ch_cfg;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SCellConfig ::= SEQUENCE
struct scell_cfg_s {
  bool                      ext                      = false;
  bool                      scell_cfg_common_present = false;
  bool                      scell_cfg_ded_present    = false;
  uint8_t                   scell_idx                = 1;
  serving_cell_cfg_common_s scell_cfg_common;
  serving_cell_cfg_s        scell_cfg_ded;
  // ...
  // group 0
  copy_ptr<ssb_mtc_s> smtc;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SRB-ToAddModList ::= SEQUENCE (SIZE (1..2)) OF SRB-ToAddMod
using srb_to_add_mod_list_l = dyn_array<srb_to_add_mod_s>;

// SecurityConfig ::= SEQUENCE
struct security_cfg_s {
  struct key_to_use_opts {
    enum options { master, secondary, nulltype } value;

    std::string to_string() const;
  };
  typedef enumerated<key_to_use_opts> key_to_use_e_;

  // member variables
  bool                     ext                            = false;
  bool                     security_algorithm_cfg_present = false;
  bool                     key_to_use_present             = false;
  security_algorithm_cfg_s security_algorithm_cfg;
  key_to_use_e_            key_to_use;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SpCellConfig ::= SEQUENCE
struct sp_cell_cfg_s {
  bool                                     ext                                   = false;
  bool                                     serv_cell_idx_present                 = false;
  bool                                     recfg_with_sync_present               = false;
  bool                                     rlf_timers_and_consts_present         = false;
  bool                                     rlm_in_sync_out_of_sync_thres_present = false;
  bool                                     sp_cell_cfg_ded_present               = false;
  uint8_t                                  serv_cell_idx                         = 0;
  recfg_with_sync_s                        recfg_with_sync;
  setup_release_c<rlf_timers_and_consts_s> rlf_timers_and_consts;
  serving_cell_cfg_s                       sp_cell_cfg_ded;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CellGroupConfig ::= SEQUENCE
struct cell_group_cfg_s {
  using rlc_bearer_to_add_mod_list_l_ = dyn_array<rlc_bearer_cfg_s>;
  using rlc_bearer_to_release_list_l_ = bounded_array<uint8_t, 32>;
  using scell_to_add_mod_list_l_      = dyn_array<scell_cfg_s>;
  using scell_to_release_list_l_      = bounded_array<uint8_t, 31>;

  // member variables
  bool                          ext                                = false;
  bool                          rlc_bearer_to_add_mod_list_present = false;
  bool                          rlc_bearer_to_release_list_present = false;
  bool                          mac_cell_group_cfg_present         = false;
  bool                          phys_cell_group_cfg_present        = false;
  bool                          sp_cell_cfg_present                = false;
  bool                          scell_to_add_mod_list_present      = false;
  bool                          scell_to_release_list_present      = false;
  uint8_t                       cell_group_id                      = 0;
  rlc_bearer_to_add_mod_list_l_ rlc_bearer_to_add_mod_list;
  rlc_bearer_to_release_list_l_ rlc_bearer_to_release_list;
  mac_cell_group_cfg_s          mac_cell_group_cfg;
  phys_cell_group_cfg_s         phys_cell_group_cfg;
  sp_cell_cfg_s                 sp_cell_cfg;
  scell_to_add_mod_list_l_      scell_to_add_mod_list;
  scell_to_release_list_l_      scell_to_release_list;
  // ...
  // group 0
  bool report_ul_tx_direct_current_v1530_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RadioBearerConfig ::= SEQUENCE
struct radio_bearer_cfg_s {
  bool                  ext                         = false;
  bool                  srb_to_add_mod_list_present = false;
  bool                  srb3_to_release_present     = false;
  bool                  drb_to_add_mod_list_present = false;
  bool                  drb_to_release_list_present = false;
  bool                  security_cfg_present        = false;
  srb_to_add_mod_list_l srb_to_add_mod_list;
  drb_to_add_mod_list_l drb_to_add_mod_list;
  drb_to_release_list_l drb_to_release_list;
  security_cfg_s        security_cfg;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCReject-IEs ::= SEQUENCE
struct rrc_reject_ies_s {
  bool          wait_time_present         = false;
  bool          late_non_crit_ext_present = false;
  bool          non_crit_ext_present      = false;
  uint8_t       wait_time                 = 1;
  dyn_octstring late_non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCSetup-IEs ::= SEQUENCE
struct rrc_setup_ies_s {
  bool               late_non_crit_ext_present = false;
  bool               non_crit_ext_present      = false;
  radio_bearer_cfg_s radio_bearer_cfg;
  dyn_octstring      master_cell_group;
  dyn_octstring      late_non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCReject ::= SEQUENCE
struct rrc_reject_s {
  struct crit_exts_c_ {
    struct types_opts {
      enum options { rrc_reject, crit_exts_future, nulltype } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    crit_exts_c_() = default;
    crit_exts_c_(const crit_exts_c_& other);
    crit_exts_c_& operator=(const crit_exts_c_& other);
    ~crit_exts_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    rrc_reject_ies_s& rrc_reject()
    {
      assert_choice_type("rrcReject", type_.to_string(), "criticalExtensions");
      return c.get<rrc_reject_ies_s>();
    }
    const rrc_reject_ies_s& rrc_reject() const
    {
      assert_choice_type("rrcReject", type_.to_string(), "criticalExtensions");
      return c.get<rrc_reject_ies_s>();
    }
    rrc_reject_ies_s& set_rrc_reject()
    {
      set(types::rrc_reject);
      return c.get<rrc_reject_ies_s>();
    }

  private:
    types                             type_;
    choice_buffer_t<rrc_reject_ies_s> c;

    void destroy_();
  };

  // member variables
  crit_exts_c_ crit_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCSetup ::= SEQUENCE
struct rrc_setup_s {
  struct crit_exts_c_ {
    struct types_opts {
      enum options { rrc_setup, crit_exts_future, nulltype } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    crit_exts_c_() = default;
    crit_exts_c_(const crit_exts_c_& other);
    crit_exts_c_& operator=(const crit_exts_c_& other);
    ~crit_exts_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    rrc_setup_ies_s& rrc_setup()
    {
      assert_choice_type("rrcSetup", type_.to_string(), "criticalExtensions");
      return c.get<rrc_setup_ies_s>();
    }
    const rrc_setup_ies_s& rrc_setup() const
    {
      assert_choice_type("rrcSetup", type_.to_string(), "criticalExtensions");
      return c.get<rrc_setup_ies_s>();
    }
    rrc_setup_ies_s& set_rrc_setup()
    {
      set(types::rrc_setup);
      return c.get<rrc_setup_ies_s>();
    }

  private:
    types                            type_;
    choice_buffer_t<rrc_setup_ies_s> c;

    void destroy_();
  };

  // member variables
  uint8_t      rrc_transaction_id = 0;
  crit_exts_c_ crit_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DL-CCCH-MessageType ::= CHOICE
struct dl_ccch_msg_type_c {
  struct c1_c_ {
    struct types_opts {
      enum options { rrc_reject, rrc_setup, spare2, spare1, nulltype } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    c1_c_() = default;
    c1_c_(const c1_c_& other);
    c1_c_& operator=(const c1_c_& other);
    ~c1_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    rrc_reject_s& rrc_reject()
    {
      assert_choice_type("rrcReject", type_.to_string(), "c1");
      return c.get<rrc_reject_s>();
    }
    rrc_setup_s& rrc_setup()
    {
      assert_choice_type("rrcSetup", type_.to_string(), "c1");
      return c.get<rrc_setup_s>();
    }
    const rrc_reject_s& rrc_reject() const
    {
      assert_choice_type("rrcReject", type_.to_string(), "c1");
      return c.get<rrc_reject_s>();
    }
    const rrc_setup_s& rrc_setup() const
    {
      assert_choice_type("rrcSetup", type_.to_string(), "c1");
      return c.get<rrc_setup_s>();
    }
    rrc_reject_s& set_rrc_reject()
    {
      set(types::rrc_reject);
      return c.get<rrc_reject_s>();
    }
    rrc_setup_s& set_rrc_setup()
    {
      set(types::rrc_setup);
      return c.get<rrc_setup_s>();
    }

  private:
    types                                      type_;
    choice_buffer_t<rrc_reject_s, rrc_setup_s> c;

    void destroy_();
  };
  struct types_opts {
    enum options { c1, msg_class_ext, nulltype } value;
    typedef uint8_t number_type;

    std::string to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<types_opts> types;

  // choice methods
  dl_ccch_msg_type_c() = default;
  dl_ccch_msg_type_c(const dl_ccch_msg_type_c& other);
  dl_ccch_msg_type_c& operator=(const dl_ccch_msg_type_c& other);
  ~dl_ccch_msg_type_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  c1_c_& c1()
  {
    assert_choice_type("c1", type_.to_string(), "DL-CCCH-MessageType");
    return c.get<c1_c_>();
  }
  const c1_c_& c1() const
  {
    assert_choice_type("c1", type_.to_string(), "DL-CCCH-MessageType");
    return c.get<c1_c_>();
  }
  c1_c_& set_c1()
  {
    set(types::c1);
    return c.get<c1_c_>();
  }

private:
  types                  type_;
  choice_buffer_t<c1_c_> c;

  void destroy_();
};

// DL-CCCH-Message ::= SEQUENCE
struct dl_ccch_msg_s {
  dl_ccch_msg_type_c msg;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CSI-RS-Resource-Mobility ::= SEQUENCE
struct csi_rs_res_mob_s {
  struct slot_cfg_c_ {
    struct types_opts {
      enum options { ms4, ms5, ms10, ms20, ms40, nulltype } value;
      typedef uint8_t number_type;

      std::string to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    slot_cfg_c_() = default;
    slot_cfg_c_(const slot_cfg_c_& other);
    slot_cfg_c_& operator=(const slot_cfg_c_& other);
    ~slot_cfg_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint8_t& ms4()
    {
      assert_choice_type("ms4", type_.to_string(), "slotConfig");
      return c.get<uint8_t>();
    }
    uint8_t& ms5()
    {
      assert_choice_type("ms5", type_.to_string(), "slotConfig");
      return c.get<uint8_t>();
    }
    uint8_t& ms10()
    {
      assert_choice_type("ms10", type_.to_string(), "slotConfig");
      return c.get<uint8_t>();
    }
    uint8_t& ms20()
    {
      assert_choice_type("ms20", type_.to_string(), "slotConfig");
      return c.get<uint8_t>();
    }
    uint16_t& ms40()
    {
      assert_choice_type("ms40", type_.to_string(), "slotConfig");
      return c.get<uint16_t>();
    }
    const uint8_t& ms4() const
    {
      assert_choice_type("ms4", type_.to_string(), "slotConfig");
      return c.get<uint8_t>();
    }
    const uint8_t& ms5() const
    {
      assert_choice_type("ms5", type_.to_string(), "slotConfig");
      return c.get<uint8_t>();
    }
    const uint8_t& ms10() const
    {
      assert_choice_type("ms10", type_.to_string(), "slotConfig");
      return c.get<uint8_t>();
    }
    const uint8_t& ms20() const
    {
      assert_choice_type("ms20", type_.to_string(), "slotConfig");
      return c.get<uint8_t>();
    }
    const uint16_t& ms40() const
    {
      assert_choice_type("ms40", type_.to_string(), "slotConfig");
      return c.get<uint16_t>();
    }
    uint8_t& set_ms4()
    {
      set(types::ms4);
      return c.get<uint8_t>();
    }
    uint8_t& set_ms5()
    {
      set(types::ms5);
      return c.get<uint8_t>();
    }
    uint8_t& set_ms10()
    {
      set(types::ms10);
      return c.get<uint8_t>();
    }
    uint8_t& set_ms20()
    {
      set(types::ms20);
      return c.get<uint8_t>();
    }
    uint16_t& set_ms40()
    {
      set(types::ms40);
      return c.get<uint16_t>();
    }

  private:
    types               type_;
    pod_choice_buffer_t c;

    void destroy_();
  };
  struct associated_ssb_s_ {
    uint8_t ssb_idx            = 0;
    bool    is_quasi_colocated = false;
  };
  struct freq_domain_alloc_c_ {
    struct types_opts {
      enum options { row1, row2, nulltype } value;
      typedef uint8_t number_type;

      std::string to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    freq_domain_alloc_c_() = default;
    freq_domain_alloc_c_(const freq_domain_alloc_c_& other);
    freq_domain_alloc_c_& operator=(const freq_domain_alloc_c_& other);
    ~freq_domain_alloc_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    fixed_bitstring<4>& row1()
    {
      assert_choice_type("row1", type_.to_string(), "frequencyDomainAllocation");
      return c.get<fixed_bitstring<4> >();
    }
    fixed_bitstring<12>& row2()
    {
      assert_choice_type("row2", type_.to_string(), "frequencyDomainAllocation");
      return c.get<fixed_bitstring<12> >();
    }
    const fixed_bitstring<4>& row1() const
    {
      assert_choice_type("row1", type_.to_string(), "frequencyDomainAllocation");
      return c.get<fixed_bitstring<4> >();
    }
    const fixed_bitstring<12>& row2() const
    {
      assert_choice_type("row2", type_.to_string(), "frequencyDomainAllocation");
      return c.get<fixed_bitstring<12> >();
    }
    fixed_bitstring<4>& set_row1()
    {
      set(types::row1);
      return c.get<fixed_bitstring<4> >();
    }
    fixed_bitstring<12>& set_row2()
    {
      set(types::row2);
      return c.get<fixed_bitstring<12> >();
    }

  private:
    types                                 type_;
    choice_buffer_t<fixed_bitstring<12> > c;

    void destroy_();
  };

  // member variables
  bool                 ext                    = false;
  bool                 associated_ssb_present = false;
  uint8_t              csi_rs_idx             = 0;
  slot_cfg_c_          slot_cfg;
  associated_ssb_s_    associated_ssb;
  freq_domain_alloc_c_ freq_domain_alloc;
  uint8_t              first_ofdm_symbol_in_time_domain = 0;
  uint16_t             seq_generation_cfg               = 0;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CSI-RS-CellMobility ::= SEQUENCE
struct csi_rs_cell_mob_s {
  struct csi_rs_meas_bw_s_ {
    struct nrof_prbs_opts {
      enum options { size24, size48, size96, size192, size264, nulltype } value;
      typedef uint16_t number_type;

      std::string to_string() const;
      uint16_t    to_number() const;
    };
    typedef enumerated<nrof_prbs_opts> nrof_prbs_e_;

    // member variables
    nrof_prbs_e_ nrof_prbs;
    uint16_t     start_prb = 0;
  };
  struct density_opts {
    enum options { d1, d3, nulltype } value;
    typedef uint8_t number_type;

    std::string to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<density_opts> density_e_;
  using csi_rs_res_list_mob_l_ = dyn_array<csi_rs_res_mob_s>;

  // member variables
  bool                   density_present = false;
  uint16_t               cell_id         = 0;
  csi_rs_meas_bw_s_      csi_rs_meas_bw;
  density_e_             density;
  csi_rs_res_list_mob_l_ csi_rs_res_list_mob;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// Q-OffsetRangeList ::= SEQUENCE
struct q_offset_range_list_s {
  bool             rsrp_offset_ssb_present    = false;
  bool             rsrq_offset_ssb_present    = false;
  bool             sinr_offset_ssb_present    = false;
  bool             rsrp_offset_csi_rs_present = false;
  bool             rsrq_offset_csi_rs_present = false;
  bool             sinr_offset_csi_rs_present = false;
  q_offset_range_e rsrp_offset_ssb;
  q_offset_range_e rsrq_offset_ssb;
  q_offset_range_e sinr_offset_ssb;
  q_offset_range_e rsrp_offset_csi_rs;
  q_offset_range_e rsrq_offset_csi_rs;
  q_offset_range_e sinr_offset_csi_rs;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CSI-RS-ResourceConfigMobility ::= SEQUENCE
struct csi_rs_res_cfg_mob_s {
  using csi_rs_cell_list_mob_l_ = dyn_array<csi_rs_cell_mob_s>;

  // member variables
  bool                    ext = false;
  subcarrier_spacing_e    subcarrier_spacing;
  csi_rs_cell_list_mob_l_ csi_rs_cell_list_mob;
  // ...
  // group 0
  bool    ref_serv_cell_idx_v1530_present = false;
  uint8_t ref_serv_cell_idx_v1530         = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CellsToAddMod ::= SEQUENCE
struct cells_to_add_mod_s {
  uint16_t              pci = 0;
  q_offset_range_list_s cell_individual_offset;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// FilterCoefficient ::= ENUMERATED
struct filt_coef_opts {
  enum options {
    fc0,
    fc1,
    fc2,
    fc3,
    fc4,
    fc5,
    fc6,
    fc7,
    fc8,
    fc9,
    fc11,
    fc13,
    fc15,
    fc17,
    fc19,
    spare1,
    /*...*/ nulltype
  } value;
  typedef uint8_t number_type;

  std::string to_string() const;
  uint8_t     to_number() const;
};
typedef enumerated<filt_coef_opts, true> filt_coef_e;

// MeasReportQuantity ::= SEQUENCE
struct meas_report_quant_s {
  bool rsrp = false;
  bool rsrq = false;
  bool sinr = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasTriggerQuantity ::= CHOICE
struct meas_trigger_quant_c {
  struct types_opts {
    enum options { rsrp, rsrq, sinr, nulltype } value;

    std::string to_string() const;
  };
  typedef enumerated<types_opts> types;

  // choice methods
  meas_trigger_quant_c() = default;
  meas_trigger_quant_c(const meas_trigger_quant_c& other);
  meas_trigger_quant_c& operator=(const meas_trigger_quant_c& other);
  ~meas_trigger_quant_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  uint8_t& rsrp()
  {
    assert_choice_type("rsrp", type_.to_string(), "MeasTriggerQuantity");
    return c.get<uint8_t>();
  }
  uint8_t& rsrq()
  {
    assert_choice_type("rsrq", type_.to_string(), "MeasTriggerQuantity");
    return c.get<uint8_t>();
  }
  uint8_t& sinr()
  {
    assert_choice_type("sinr", type_.to_string(), "MeasTriggerQuantity");
    return c.get<uint8_t>();
  }
  const uint8_t& rsrp() const
  {
    assert_choice_type("rsrp", type_.to_string(), "MeasTriggerQuantity");
    return c.get<uint8_t>();
  }
  const uint8_t& rsrq() const
  {
    assert_choice_type("rsrq", type_.to_string(), "MeasTriggerQuantity");
    return c.get<uint8_t>();
  }
  const uint8_t& sinr() const
  {
    assert_choice_type("sinr", type_.to_string(), "MeasTriggerQuantity");
    return c.get<uint8_t>();
  }
  uint8_t& set_rsrp()
  {
    set(types::rsrp);
    return c.get<uint8_t>();
  }
  uint8_t& set_rsrq()
  {
    set(types::rsrq);
    return c.get<uint8_t>();
  }
  uint8_t& set_sinr()
  {
    set(types::sinr);
    return c.get<uint8_t>();
  }

private:
  types               type_;
  pod_choice_buffer_t c;

  void destroy_();
};

// MeasTriggerQuantityEUTRA ::= CHOICE
struct meas_trigger_quant_eutra_c {
  struct types_opts {
    enum options { rsrp, rsrq, sinr, nulltype } value;

    std::string to_string() const;
  };
  typedef enumerated<types_opts> types;

  // choice methods
  meas_trigger_quant_eutra_c() = default;
  meas_trigger_quant_eutra_c(const meas_trigger_quant_eutra_c& other);
  meas_trigger_quant_eutra_c& operator=(const meas_trigger_quant_eutra_c& other);
  ~meas_trigger_quant_eutra_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  uint8_t& rsrp()
  {
    assert_choice_type("rsrp", type_.to_string(), "MeasTriggerQuantityEUTRA");
    return c.get<uint8_t>();
  }
  uint8_t& rsrq()
  {
    assert_choice_type("rsrq", type_.to_string(), "MeasTriggerQuantityEUTRA");
    return c.get<uint8_t>();
  }
  uint8_t& sinr()
  {
    assert_choice_type("sinr", type_.to_string(), "MeasTriggerQuantityEUTRA");
    return c.get<uint8_t>();
  }
  const uint8_t& rsrp() const
  {
    assert_choice_type("rsrp", type_.to_string(), "MeasTriggerQuantityEUTRA");
    return c.get<uint8_t>();
  }
  const uint8_t& rsrq() const
  {
    assert_choice_type("rsrq", type_.to_string(), "MeasTriggerQuantityEUTRA");
    return c.get<uint8_t>();
  }
  const uint8_t& sinr() const
  {
    assert_choice_type("sinr", type_.to_string(), "MeasTriggerQuantityEUTRA");
    return c.get<uint8_t>();
  }
  uint8_t& set_rsrp()
  {
    set(types::rsrp);
    return c.get<uint8_t>();
  }
  uint8_t& set_rsrq()
  {
    set(types::rsrq);
    return c.get<uint8_t>();
  }
  uint8_t& set_sinr()
  {
    set(types::sinr);
    return c.get<uint8_t>();
  }

private:
  types               type_;
  pod_choice_buffer_t c;

  void destroy_();
};

// MeasTriggerQuantityOffset ::= CHOICE
struct meas_trigger_quant_offset_c {
  struct types_opts {
    enum options { rsrp, rsrq, sinr, nulltype } value;

    std::string to_string() const;
  };
  typedef enumerated<types_opts> types;

  // choice methods
  meas_trigger_quant_offset_c() = default;
  meas_trigger_quant_offset_c(const meas_trigger_quant_offset_c& other);
  meas_trigger_quant_offset_c& operator=(const meas_trigger_quant_offset_c& other);
  ~meas_trigger_quant_offset_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  int8_t& rsrp()
  {
    assert_choice_type("rsrp", type_.to_string(), "MeasTriggerQuantityOffset");
    return c.get<int8_t>();
  }
  int8_t& rsrq()
  {
    assert_choice_type("rsrq", type_.to_string(), "MeasTriggerQuantityOffset");
    return c.get<int8_t>();
  }
  int8_t& sinr()
  {
    assert_choice_type("sinr", type_.to_string(), "MeasTriggerQuantityOffset");
    return c.get<int8_t>();
  }
  const int8_t& rsrp() const
  {
    assert_choice_type("rsrp", type_.to_string(), "MeasTriggerQuantityOffset");
    return c.get<int8_t>();
  }
  const int8_t& rsrq() const
  {
    assert_choice_type("rsrq", type_.to_string(), "MeasTriggerQuantityOffset");
    return c.get<int8_t>();
  }
  const int8_t& sinr() const
  {
    assert_choice_type("sinr", type_.to_string(), "MeasTriggerQuantityOffset");
    return c.get<int8_t>();
  }
  int8_t& set_rsrp()
  {
    set(types::rsrp);
    return c.get<int8_t>();
  }
  int8_t& set_rsrq()
  {
    set(types::rsrq);
    return c.get<int8_t>();
  }
  int8_t& set_sinr()
  {
    set(types::sinr);
    return c.get<int8_t>();
  }

private:
  types               type_;
  pod_choice_buffer_t c;

  void destroy_();
};

// NR-RS-Type ::= ENUMERATED
struct nr_rs_type_opts {
  enum options { ssb, csi_rs, nulltype } value;

  std::string to_string() const;
};
typedef enumerated<nr_rs_type_opts> nr_rs_type_e;

// ReportInterval ::= ENUMERATED
struct report_interv_opts {
  enum options {
    ms120,
    ms240,
    ms480,
    ms640,
    ms1024,
    ms2048,
    ms5120,
    ms10240,
    ms20480,
    ms40960,
    min1,
    min6,
    min12,
    min30,
    nulltype
  } value;
  typedef uint16_t number_type;

  std::string to_string() const;
  uint16_t    to_number() const;
};
typedef enumerated<report_interv_opts> report_interv_e;

// SSB-ConfigMobility ::= SEQUENCE
struct ssb_cfg_mob_s {
  bool                              ext                    = false;
  bool                              ssb_to_measure_present = false;
  bool                              ss_rssi_meas_present   = false;
  setup_release_c<ssb_to_measure_c> ssb_to_measure;
  bool                              derive_ssb_idx_from_cell = false;
  ss_rssi_meas_s                    ss_rssi_meas;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// TimeToTrigger ::= ENUMERATED
struct time_to_trigger_opts {
  enum options {
    ms0,
    ms40,
    ms64,
    ms80,
    ms100,
    ms128,
    ms160,
    ms256,
    ms320,
    ms480,
    ms512,
    ms640,
    ms1024,
    ms1280,
    ms2560,
    ms5120,
    nulltype
  } value;
  typedef uint16_t number_type;

  std::string to_string() const;
  uint16_t    to_number() const;
};
typedef enumerated<time_to_trigger_opts> time_to_trigger_e;

// CellsToAddModList ::= SEQUENCE (SIZE (1..32)) OF CellsToAddMod
using cells_to_add_mod_list_l = dyn_array<cells_to_add_mod_s>;

// EUTRA-BlackCell ::= SEQUENCE
struct eutra_black_cell_s {
  uint8_t           cell_idx_eutra = 1;
  eutra_pci_range_s pci_range;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// EUTRA-Cell ::= SEQUENCE
struct eutra_cell_s {
  uint8_t                cell_idx_eutra = 1;
  uint16_t               pci            = 0;
  eutra_q_offset_range_e cell_individual_offset;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// EUTRA-CellIndexList ::= SEQUENCE (SIZE (1..32)) OF INTEGER (1..32)
using eutra_cell_idx_list_l = bounded_array<uint8_t, 32>;

// EventTriggerConfig ::= SEQUENCE
struct event_trigger_cfg_s {
  struct event_id_c_ {
    struct event_a1_s_ {
      meas_trigger_quant_c a1_thres;
      bool                 report_on_leave = false;
      uint8_t              hysteresis      = 0;
      time_to_trigger_e    time_to_trigger;
    };
    struct event_a2_s_ {
      meas_trigger_quant_c a2_thres;
      bool                 report_on_leave = false;
      uint8_t              hysteresis      = 0;
      time_to_trigger_e    time_to_trigger;
    };
    struct event_a3_s_ {
      meas_trigger_quant_offset_c a3_offset;
      bool                        report_on_leave = false;
      uint8_t                     hysteresis      = 0;
      time_to_trigger_e           time_to_trigger;
      bool                        use_white_cell_list = false;
    };
    struct event_a4_s_ {
      meas_trigger_quant_c a4_thres;
      bool                 report_on_leave = false;
      uint8_t              hysteresis      = 0;
      time_to_trigger_e    time_to_trigger;
      bool                 use_white_cell_list = false;
    };
    struct event_a5_s_ {
      meas_trigger_quant_c a5_thres1;
      meas_trigger_quant_c a5_thres2;
      bool                 report_on_leave = false;
      uint8_t              hysteresis      = 0;
      time_to_trigger_e    time_to_trigger;
      bool                 use_white_cell_list = false;
    };
    struct event_a6_s_ {
      meas_trigger_quant_offset_c a6_offset;
      bool                        report_on_leave = false;
      uint8_t                     hysteresis      = 0;
      time_to_trigger_e           time_to_trigger;
      bool                        use_white_cell_list = false;
    };
    struct types_opts {
      enum options { event_a1, event_a2, event_a3, event_a4, event_a5, event_a6, /*...*/ nulltype } value;
      typedef uint8_t number_type;

      std::string to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts, true> types;

    // choice methods
    event_id_c_() = default;
    event_id_c_(const event_id_c_& other);
    event_id_c_& operator=(const event_id_c_& other);
    ~event_id_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    event_a1_s_& event_a1()
    {
      assert_choice_type("eventA1", type_.to_string(), "eventId");
      return c.get<event_a1_s_>();
    }
    event_a2_s_& event_a2()
    {
      assert_choice_type("eventA2", type_.to_string(), "eventId");
      return c.get<event_a2_s_>();
    }
    event_a3_s_& event_a3()
    {
      assert_choice_type("eventA3", type_.to_string(), "eventId");
      return c.get<event_a3_s_>();
    }
    event_a4_s_& event_a4()
    {
      assert_choice_type("eventA4", type_.to_string(), "eventId");
      return c.get<event_a4_s_>();
    }
    event_a5_s_& event_a5()
    {
      assert_choice_type("eventA5", type_.to_string(), "eventId");
      return c.get<event_a5_s_>();
    }
    event_a6_s_& event_a6()
    {
      assert_choice_type("eventA6", type_.to_string(), "eventId");
      return c.get<event_a6_s_>();
    }
    const event_a1_s_& event_a1() const
    {
      assert_choice_type("eventA1", type_.to_string(), "eventId");
      return c.get<event_a1_s_>();
    }
    const event_a2_s_& event_a2() const
    {
      assert_choice_type("eventA2", type_.to_string(), "eventId");
      return c.get<event_a2_s_>();
    }
    const event_a3_s_& event_a3() const
    {
      assert_choice_type("eventA3", type_.to_string(), "eventId");
      return c.get<event_a3_s_>();
    }
    const event_a4_s_& event_a4() const
    {
      assert_choice_type("eventA4", type_.to_string(), "eventId");
      return c.get<event_a4_s_>();
    }
    const event_a5_s_& event_a5() const
    {
      assert_choice_type("eventA5", type_.to_string(), "eventId");
      return c.get<event_a5_s_>();
    }
    const event_a6_s_& event_a6() const
    {
      assert_choice_type("eventA6", type_.to_string(), "eventId");
      return c.get<event_a6_s_>();
    }
    event_a1_s_& set_event_a1()
    {
      set(types::event_a1);
      return c.get<event_a1_s_>();
    }
    event_a2_s_& set_event_a2()
    {
      set(types::event_a2);
      return c.get<event_a2_s_>();
    }
    event_a3_s_& set_event_a3()
    {
      set(types::event_a3);
      return c.get<event_a3_s_>();
    }
    event_a4_s_& set_event_a4()
    {
      set(types::event_a4);
      return c.get<event_a4_s_>();
    }
    event_a5_s_& set_event_a5()
    {
      set(types::event_a5);
      return c.get<event_a5_s_>();
    }
    event_a6_s_& set_event_a6()
    {
      set(types::event_a6);
      return c.get<event_a6_s_>();
    }

  private:
    types                                                                                         type_;
    choice_buffer_t<event_a1_s_, event_a2_s_, event_a3_s_, event_a4_s_, event_a5_s_, event_a6_s_> c;

    void destroy_();
  };
  struct report_amount_opts {
    enum options { r1, r2, r4, r8, r16, r32, r64, infinity, nulltype } value;
    typedef int8_t number_type;

    std::string to_string() const;
    int8_t      to_number() const;
  };
  typedef enumerated<report_amount_opts> report_amount_e_;

  // member variables
  bool                ext                                 = false;
  bool                report_quant_rs_idxes_present       = false;
  bool                max_nrof_rs_idxes_to_report_present = false;
  bool                report_add_neigh_meas_present       = false;
  event_id_c_         event_id;
  nr_rs_type_e        rs_type;
  report_interv_e     report_interv;
  report_amount_e_    report_amount;
  meas_report_quant_s report_quant_cell;
  uint8_t             max_report_cells = 1;
  meas_report_quant_s report_quant_rs_idxes;
  uint8_t             max_nrof_rs_idxes_to_report = 1;
  bool                include_beam_meass          = false;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// EventTriggerConfigInterRAT ::= SEQUENCE
struct event_trigger_cfg_inter_rat_s {
  struct event_id_c_ {
    struct event_b1_s_ {
      bool                       ext = false;
      meas_trigger_quant_eutra_c b1_thres_eutra;
      bool                       report_on_leave = false;
      uint8_t                    hysteresis      = 0;
      time_to_trigger_e          time_to_trigger;
      // ...
    };
    struct event_b2_s_ {
      bool                       ext = false;
      meas_trigger_quant_c       b2_thres1;
      meas_trigger_quant_eutra_c b2_thres2_eutra;
      bool                       report_on_leave = false;
      uint8_t                    hysteresis      = 0;
      time_to_trigger_e          time_to_trigger;
      // ...
    };
    struct types_opts {
      enum options { event_b1, event_b2, /*...*/ nulltype } value;
      typedef uint8_t number_type;

      std::string to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts, true> types;

    // choice methods
    event_id_c_() = default;
    event_id_c_(const event_id_c_& other);
    event_id_c_& operator=(const event_id_c_& other);
    ~event_id_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    event_b1_s_& event_b1()
    {
      assert_choice_type("eventB1", type_.to_string(), "eventId");
      return c.get<event_b1_s_>();
    }
    event_b2_s_& event_b2()
    {
      assert_choice_type("eventB2", type_.to_string(), "eventId");
      return c.get<event_b2_s_>();
    }
    const event_b1_s_& event_b1() const
    {
      assert_choice_type("eventB1", type_.to_string(), "eventId");
      return c.get<event_b1_s_>();
    }
    const event_b2_s_& event_b2() const
    {
      assert_choice_type("eventB2", type_.to_string(), "eventId");
      return c.get<event_b2_s_>();
    }
    event_b1_s_& set_event_b1()
    {
      set(types::event_b1);
      return c.get<event_b1_s_>();
    }
    event_b2_s_& set_event_b2()
    {
      set(types::event_b2);
      return c.get<event_b2_s_>();
    }

  private:
    types                                     type_;
    choice_buffer_t<event_b1_s_, event_b2_s_> c;

    void destroy_();
  };
  struct report_amount_opts {
    enum options { r1, r2, r4, r8, r16, r32, r64, infinity, nulltype } value;
    typedef int8_t number_type;

    std::string to_string() const;
    int8_t      to_number() const;
  };
  typedef enumerated<report_amount_opts> report_amount_e_;

  // member variables
  bool                ext = false;
  event_id_c_         event_id;
  nr_rs_type_e        rs_type;
  report_interv_e     report_interv;
  report_amount_e_    report_amount;
  meas_report_quant_s report_quant;
  uint8_t             max_report_cells = 1;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// FilterConfig ::= SEQUENCE
struct filt_cfg_s {
  bool        filt_coef_rsrp_present    = false;
  bool        filt_coef_rsrq_present    = false;
  bool        filt_coef_rs_sinr_present = false;
  filt_coef_e filt_coef_rsrp;
  filt_coef_e filt_coef_rsrq;
  filt_coef_e filt_coef_rs_sinr;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PCI-List ::= SEQUENCE (SIZE (1..32)) OF INTEGER (0..1007)
using pci_list_l = bounded_array<uint16_t, 32>;

// PCI-RangeElement ::= SEQUENCE
struct pci_range_elem_s {
  uint8_t     pci_range_idx = 1;
  pci_range_s pci_range;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PCI-RangeIndexList ::= SEQUENCE (SIZE (1..8)) OF INTEGER (1..8)
using pci_range_idx_list_l = bounded_array<uint8_t, 8>;

// PeriodicalReportConfig ::= SEQUENCE
struct periodical_report_cfg_s {
  struct report_amount_opts {
    enum options { r1, r2, r4, r8, r16, r32, r64, infinity, nulltype } value;
    typedef int8_t number_type;

    std::string to_string() const;
    int8_t      to_number() const;
  };
  typedef enumerated<report_amount_opts> report_amount_e_;

  // member variables
  bool                ext                                 = false;
  bool                report_quant_rs_idxes_present       = false;
  bool                max_nrof_rs_idxes_to_report_present = false;
  nr_rs_type_e        rs_type;
  report_interv_e     report_interv;
  report_amount_e_    report_amount;
  meas_report_quant_s report_quant_cell;
  uint8_t             max_report_cells = 1;
  meas_report_quant_s report_quant_rs_idxes;
  uint8_t             max_nrof_rs_idxes_to_report = 1;
  bool                include_beam_meass          = false;
  bool                use_white_cell_list         = false;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PeriodicalReportConfigInterRAT ::= SEQUENCE
struct periodical_report_cfg_inter_rat_s {
  struct report_amount_opts {
    enum options { r1, r2, r4, r8, r16, r32, r64, infinity, nulltype } value;
    typedef int8_t number_type;

    std::string to_string() const;
    int8_t      to_number() const;
  };
  typedef enumerated<report_amount_opts> report_amount_e_;

  // member variables
  bool                ext = false;
  report_interv_e     report_interv;
  report_amount_e_    report_amount;
  meas_report_quant_s report_quant;
  uint8_t             max_report_cells = 1;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RAN-AreaConfig ::= SEQUENCE
struct ran_area_cfg_s {
  using ran_area_code_list_l_ = bounded_array<uint16_t, 32>;

  // member variables
  bool                  ran_area_code_list_present = false;
  fixed_bitstring<24>   tac;
  ran_area_code_list_l_ ran_area_code_list;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ReferenceSignalConfig ::= SEQUENCE
struct ref_sig_cfg_s {
  bool                                  ssb_cfg_mob_present        = false;
  bool                                  csi_rs_res_cfg_mob_present = false;
  ssb_cfg_mob_s                         ssb_cfg_mob;
  setup_release_c<csi_rs_res_cfg_mob_s> csi_rs_res_cfg_mob;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ReportCGI ::= SEQUENCE
struct report_cgi_s {
  bool     ext                          = false;
  uint16_t cell_for_which_to_report_cgi = 0;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ReportCGI-EUTRA ::= SEQUENCE
struct report_cgi_eutra_s {
  bool     ext                          = false;
  uint16_t cell_for_which_to_report_cgi = 0;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SSB-MTC2 ::= SEQUENCE
struct ssb_mtc2_s {
  using pci_list_l_ = dyn_array<uint16_t>;
  struct periodicity_opts {
    enum options { sf5, sf10, sf20, sf40, sf80, spare3, spare2, spare1, nulltype } value;
    typedef uint8_t number_type;

    std::string to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<periodicity_opts> periodicity_e_;

  // member variables
  bool           pci_list_present = false;
  pci_list_l_    pci_list;
  periodicity_e_ periodicity;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasObjectEUTRA ::= SEQUENCE
struct meas_obj_eutra_s {
  using cells_to_add_mod_list_eutran_l_       = dyn_array<eutra_cell_s>;
  using black_cells_to_add_mod_list_eutran_l_ = dyn_array<eutra_black_cell_s>;

  // member variables
  bool                                  ext                                        = false;
  bool                                  cells_to_rem_list_eutran_present           = false;
  bool                                  cells_to_add_mod_list_eutran_present       = false;
  bool                                  black_cells_to_rem_list_eutran_present     = false;
  bool                                  black_cells_to_add_mod_list_eutran_present = false;
  bool                                  eutra_q_offset_range_present               = false;
  uint32_t                              carrier_freq                               = 0;
  eutra_allowed_meas_bw_e               allowed_meas_bw;
  eutra_cell_idx_list_l                 cells_to_rem_list_eutran;
  cells_to_add_mod_list_eutran_l_       cells_to_add_mod_list_eutran;
  eutra_cell_idx_list_l                 black_cells_to_rem_list_eutran;
  black_cells_to_add_mod_list_eutran_l_ black_cells_to_add_mod_list_eutran;
  bool                                  eutra_presence_ant_port1 = false;
  eutra_q_offset_range_e                eutra_q_offset_range;
  bool                                  wideband_rsrq_meas = false;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasObjectNR ::= SEQUENCE
struct meas_obj_nr_s {
  using black_cells_to_add_mod_list_l_ = dyn_array<pci_range_elem_s>;
  using white_cells_to_add_mod_list_l_ = dyn_array<pci_range_elem_s>;
  struct meas_cycle_scell_v1530_opts {
    enum options { sf160, sf256, sf320, sf512, sf640, sf1024, sf1280, nulltype } value;
    typedef uint16_t number_type;

    std::string to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<meas_cycle_scell_v1530_opts> meas_cycle_scell_v1530_e_;

  // member variables
  bool                           ext                                        = false;
  bool                           ssb_freq_present                           = false;
  bool                           ssb_subcarrier_spacing_present             = false;
  bool                           smtc1_present                              = false;
  bool                           smtc2_present                              = false;
  bool                           ref_freq_csi_rs_present                    = false;
  bool                           abs_thresh_ss_blocks_consolidation_present = false;
  bool                           abs_thresh_csi_rs_consolidation_present    = false;
  bool                           nrof_ss_blocks_to_average_present          = false;
  bool                           nrof_csi_rs_res_to_average_present         = false;
  bool                           cells_to_rem_list_present                  = false;
  bool                           cells_to_add_mod_list_present              = false;
  bool                           black_cells_to_rem_list_present            = false;
  bool                           black_cells_to_add_mod_list_present        = false;
  bool                           white_cells_to_rem_list_present            = false;
  bool                           white_cells_to_add_mod_list_present        = false;
  uint32_t                       ssb_freq                                   = 0;
  subcarrier_spacing_e           ssb_subcarrier_spacing;
  ssb_mtc_s                      smtc1;
  ssb_mtc2_s                     smtc2;
  uint32_t                       ref_freq_csi_rs = 0;
  ref_sig_cfg_s                  ref_sig_cfg;
  thres_nr_s                     abs_thresh_ss_blocks_consolidation;
  thres_nr_s                     abs_thresh_csi_rs_consolidation;
  uint8_t                        nrof_ss_blocks_to_average  = 2;
  uint8_t                        nrof_csi_rs_res_to_average = 2;
  uint8_t                        quant_cfg_idx              = 1;
  q_offset_range_list_s          offset_mo;
  pci_list_l                     cells_to_rem_list;
  cells_to_add_mod_list_l        cells_to_add_mod_list;
  pci_range_idx_list_l           black_cells_to_rem_list;
  black_cells_to_add_mod_list_l_ black_cells_to_add_mod_list;
  pci_range_idx_list_l           white_cells_to_rem_list;
  white_cells_to_add_mod_list_l_ white_cells_to_add_mod_list;
  // ...
  // group 0
  bool                      freq_band_ind_nr_v1530_present = false;
  bool                      meas_cycle_scell_v1530_present = false;
  uint16_t                  freq_band_ind_nr_v1530         = 1;
  meas_cycle_scell_v1530_e_ meas_cycle_scell_v1530;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// OverheatingAssistanceConfig ::= SEQUENCE
struct overheat_assist_cfg_s {
  struct overheat_ind_prohibit_timer_opts {
    enum options {
      s0,
      s0dot5,
      s1,
      s2,
      s5,
      s10,
      s20,
      s30,
      s60,
      s90,
      s120,
      s300,
      s600,
      spare3,
      spare2,
      spare1,
      nulltype
    } value;
    typedef float number_type;

    std::string to_string() const;
    float       to_number() const;
    std::string to_number_string() const;
  };
  typedef enumerated<overheat_ind_prohibit_timer_opts> overheat_ind_prohibit_timer_e_;

  // member variables
  overheat_ind_prohibit_timer_e_ overheat_ind_prohibit_timer;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PLMN-RAN-AreaCell ::= SEQUENCE
struct plmn_ran_area_cell_s {
  using ran_area_cells_l_ = bounded_array<fixed_bitstring<36>, 32>;

  // member variables
  bool              plmn_id_present = false;
  plmn_id_s         plmn_id;
  ran_area_cells_l_ ran_area_cells;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PLMN-RAN-AreaConfig ::= SEQUENCE
struct plmn_ran_area_cfg_s {
  using ran_area_l_ = dyn_array<ran_area_cfg_s>;

  // member variables
  bool        plmn_id_present = false;
  plmn_id_s   plmn_id;
  ran_area_l_ ran_area;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// QuantityConfigRS ::= SEQUENCE
struct quant_cfg_rs_s {
  filt_cfg_s ssb_filt_cfg;
  filt_cfg_s csi_rs_filt_cfg;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ReportConfigInterRAT ::= SEQUENCE
struct report_cfg_inter_rat_s {
  struct report_type_c_ {
    struct types_opts {
      enum options { periodical, event_triggered, report_cgi, /*...*/ nulltype } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts, true> types;

    // choice methods
    report_type_c_() = default;
    report_type_c_(const report_type_c_& other);
    report_type_c_& operator=(const report_type_c_& other);
    ~report_type_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    periodical_report_cfg_inter_rat_s& periodical()
    {
      assert_choice_type("periodical", type_.to_string(), "reportType");
      return c.get<periodical_report_cfg_inter_rat_s>();
    }
    event_trigger_cfg_inter_rat_s& event_triggered()
    {
      assert_choice_type("eventTriggered", type_.to_string(), "reportType");
      return c.get<event_trigger_cfg_inter_rat_s>();
    }
    report_cgi_eutra_s& report_cgi()
    {
      assert_choice_type("reportCGI", type_.to_string(), "reportType");
      return c.get<report_cgi_eutra_s>();
    }
    const periodical_report_cfg_inter_rat_s& periodical() const
    {
      assert_choice_type("periodical", type_.to_string(), "reportType");
      return c.get<periodical_report_cfg_inter_rat_s>();
    }
    const event_trigger_cfg_inter_rat_s& event_triggered() const
    {
      assert_choice_type("eventTriggered", type_.to_string(), "reportType");
      return c.get<event_trigger_cfg_inter_rat_s>();
    }
    const report_cgi_eutra_s& report_cgi() const
    {
      assert_choice_type("reportCGI", type_.to_string(), "reportType");
      return c.get<report_cgi_eutra_s>();
    }
    periodical_report_cfg_inter_rat_s& set_periodical()
    {
      set(types::periodical);
      return c.get<periodical_report_cfg_inter_rat_s>();
    }
    event_trigger_cfg_inter_rat_s& set_event_triggered()
    {
      set(types::event_triggered);
      return c.get<event_trigger_cfg_inter_rat_s>();
    }
    report_cgi_eutra_s& set_report_cgi()
    {
      set(types::report_cgi);
      return c.get<report_cgi_eutra_s>();
    }

  private:
    types                                                                                                 type_;
    choice_buffer_t<event_trigger_cfg_inter_rat_s, periodical_report_cfg_inter_rat_s, report_cgi_eutra_s> c;

    void destroy_();
  };

  // member variables
  report_type_c_ report_type;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ReportConfigNR ::= SEQUENCE
struct report_cfg_nr_s {
  struct report_type_c_ {
    struct types_opts {
      enum options { periodical, event_triggered, /*...*/ report_cgi, nulltype } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts, true, 1> types;

    // choice methods
    report_type_c_() = default;
    report_type_c_(const report_type_c_& other);
    report_type_c_& operator=(const report_type_c_& other);
    ~report_type_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    periodical_report_cfg_s& periodical()
    {
      assert_choice_type("periodical", type_.to_string(), "reportType");
      return c.get<periodical_report_cfg_s>();
    }
    event_trigger_cfg_s& event_triggered()
    {
      assert_choice_type("eventTriggered", type_.to_string(), "reportType");
      return c.get<event_trigger_cfg_s>();
    }
    report_cgi_s& report_cgi()
    {
      assert_choice_type("reportCGI", type_.to_string(), "reportType");
      return c.get<report_cgi_s>();
    }
    const periodical_report_cfg_s& periodical() const
    {
      assert_choice_type("periodical", type_.to_string(), "reportType");
      return c.get<periodical_report_cfg_s>();
    }
    const event_trigger_cfg_s& event_triggered() const
    {
      assert_choice_type("eventTriggered", type_.to_string(), "reportType");
      return c.get<event_trigger_cfg_s>();
    }
    const report_cgi_s& report_cgi() const
    {
      assert_choice_type("reportCGI", type_.to_string(), "reportType");
      return c.get<report_cgi_s>();
    }
    periodical_report_cfg_s& set_periodical()
    {
      set(types::periodical);
      return c.get<periodical_report_cfg_s>();
    }
    event_trigger_cfg_s& set_event_triggered()
    {
      set(types::event_triggered);
      return c.get<event_trigger_cfg_s>();
    }
    report_cgi_s& set_report_cgi()
    {
      set(types::report_cgi);
      return c.get<report_cgi_s>();
    }

  private:
    types                                                                       type_;
    choice_buffer_t<event_trigger_cfg_s, periodical_report_cfg_s, report_cgi_s> c;

    void destroy_();
  };

  // member variables
  report_type_c_ report_type;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// FreqPriorityEUTRA ::= SEQUENCE
struct freq_prio_eutra_s {
  bool                  cell_resel_sub_prio_present = false;
  uint32_t              carrier_freq                = 0;
  uint8_t               cell_resel_prio             = 0;
  cell_resel_sub_prio_e cell_resel_sub_prio;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// FreqPriorityNR ::= SEQUENCE
struct freq_prio_nr_s {
  bool                  cell_resel_sub_prio_present = false;
  uint32_t              carrier_freq                = 0;
  uint8_t               cell_resel_prio             = 0;
  cell_resel_sub_prio_e cell_resel_sub_prio;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// GapConfig ::= SEQUENCE
struct gap_cfg_s {
  struct mgl_opts {
    enum options { ms1dot5, ms3, ms3dot5, ms4, ms5dot5, ms6, nulltype } value;
    typedef float number_type;

    std::string to_string() const;
    float       to_number() const;
    std::string to_number_string() const;
  };
  typedef enumerated<mgl_opts> mgl_e_;
  struct mgrp_opts {
    enum options { ms20, ms40, ms80, ms160, nulltype } value;
    typedef uint8_t number_type;

    std::string to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<mgrp_opts> mgrp_e_;
  struct mgta_opts {
    enum options { ms0, ms0dot25, ms0dot5, nulltype } value;
    typedef float number_type;

    std::string to_string() const;
    float       to_number() const;
    std::string to_number_string() const;
  };
  typedef enumerated<mgta_opts> mgta_e_;

  // member variables
  bool    ext        = false;
  uint8_t gap_offset = 0;
  mgl_e_  mgl;
  mgrp_e_ mgrp;
  mgta_e_ mgta;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasGapSharingScheme ::= ENUMERATED
struct meas_gap_sharing_scheme_opts {
  enum options { scheme00, scheme01, scheme10, scheme11, nulltype } value;
  typedef float number_type;

  std::string to_string() const;
  float       to_number() const;
  std::string to_number_string() const;
};
typedef enumerated<meas_gap_sharing_scheme_opts> meas_gap_sharing_scheme_e;

// MeasIdToAddMod ::= SEQUENCE
struct meas_id_to_add_mod_s {
  uint8_t meas_id       = 1;
  uint8_t meas_obj_id   = 1;
  uint8_t report_cfg_id = 1;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasObjectToAddMod ::= SEQUENCE
struct meas_obj_to_add_mod_s {
  struct meas_obj_c_ {
    struct types_opts {
      enum options { meas_obj_nr, /*...*/ meas_obj_eutra, nulltype } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts, true, 1> types;

    // choice methods
    meas_obj_c_() = default;
    meas_obj_c_(const meas_obj_c_& other);
    meas_obj_c_& operator=(const meas_obj_c_& other);
    ~meas_obj_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    meas_obj_nr_s& meas_obj_nr()
    {
      assert_choice_type("measObjectNR", type_.to_string(), "measObject");
      return c.get<meas_obj_nr_s>();
    }
    meas_obj_eutra_s& meas_obj_eutra()
    {
      assert_choice_type("measObjectEUTRA", type_.to_string(), "measObject");
      return c.get<meas_obj_eutra_s>();
    }
    const meas_obj_nr_s& meas_obj_nr() const
    {
      assert_choice_type("measObjectNR", type_.to_string(), "measObject");
      return c.get<meas_obj_nr_s>();
    }
    const meas_obj_eutra_s& meas_obj_eutra() const
    {
      assert_choice_type("measObjectEUTRA", type_.to_string(), "measObject");
      return c.get<meas_obj_eutra_s>();
    }
    meas_obj_nr_s& set_meas_obj_nr()
    {
      set(types::meas_obj_nr);
      return c.get<meas_obj_nr_s>();
    }
    meas_obj_eutra_s& set_meas_obj_eutra()
    {
      set(types::meas_obj_eutra);
      return c.get<meas_obj_eutra_s>();
    }

  private:
    types                                            type_;
    choice_buffer_t<meas_obj_eutra_s, meas_obj_nr_s> c;

    void destroy_();
  };

  // member variables
  uint8_t     meas_obj_id = 1;
  meas_obj_c_ meas_obj;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// OtherConfig-v1540 ::= SEQUENCE
struct other_cfg_v1540_s {
  bool                                   ext                         = false;
  bool                                   overheat_assist_cfg_present = false;
  setup_release_c<overheat_assist_cfg_s> overheat_assist_cfg;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PLMN-RAN-AreaCellList ::= SEQUENCE (SIZE (1..8)) OF PLMN-RAN-AreaCell
using plmn_ran_area_cell_list_l = dyn_array<plmn_ran_area_cell_s>;

// PLMN-RAN-AreaConfigList ::= SEQUENCE (SIZE (1..8)) OF PLMN-RAN-AreaConfig
using plmn_ran_area_cfg_list_l = dyn_array<plmn_ran_area_cfg_s>;

// QuantityConfigNR ::= SEQUENCE
struct quant_cfg_nr_s {
  bool           quant_cfg_rs_idx_present = false;
  quant_cfg_rs_s quant_cfg_cell;
  quant_cfg_rs_s quant_cfg_rs_idx;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RAT-Type ::= ENUMERATED
struct rat_type_opts {
  enum options { nr, eutra_nr, eutra, spare1, /*...*/ nulltype } value;

  std::string to_string() const;
};
typedef enumerated<rat_type_opts, true> rat_type_e;

// ReportConfigToAddMod ::= SEQUENCE
struct report_cfg_to_add_mod_s {
  struct report_cfg_c_ {
    struct types_opts {
      enum options { report_cfg_nr, /*...*/ report_cfg_inter_rat, nulltype } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts, true, 1> types;

    // choice methods
    report_cfg_c_() = default;
    report_cfg_c_(const report_cfg_c_& other);
    report_cfg_c_& operator=(const report_cfg_c_& other);
    ~report_cfg_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    report_cfg_nr_s& report_cfg_nr()
    {
      assert_choice_type("reportConfigNR", type_.to_string(), "reportConfig");
      return c.get<report_cfg_nr_s>();
    }
    report_cfg_inter_rat_s& report_cfg_inter_rat()
    {
      assert_choice_type("reportConfigInterRAT", type_.to_string(), "reportConfig");
      return c.get<report_cfg_inter_rat_s>();
    }
    const report_cfg_nr_s& report_cfg_nr() const
    {
      assert_choice_type("reportConfigNR", type_.to_string(), "reportConfig");
      return c.get<report_cfg_nr_s>();
    }
    const report_cfg_inter_rat_s& report_cfg_inter_rat() const
    {
      assert_choice_type("reportConfigInterRAT", type_.to_string(), "reportConfig");
      return c.get<report_cfg_inter_rat_s>();
    }
    report_cfg_nr_s& set_report_cfg_nr()
    {
      set(types::report_cfg_nr);
      return c.get<report_cfg_nr_s>();
    }
    report_cfg_inter_rat_s& set_report_cfg_inter_rat()
    {
      set(types::report_cfg_inter_rat);
      return c.get<report_cfg_inter_rat_s>();
    }

  private:
    types                                                    type_;
    choice_buffer_t<report_cfg_inter_rat_s, report_cfg_nr_s> c;

    void destroy_();
  };

  // member variables
  uint8_t       report_cfg_id = 1;
  report_cfg_c_ report_cfg;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CarrierInfoNR ::= SEQUENCE
struct carrier_info_nr_s {
  bool                 ext          = false;
  bool                 smtc_present = false;
  uint32_t             carrier_freq = 0;
  subcarrier_spacing_e ssb_subcarrier_spacing;
  ssb_mtc_s            smtc;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DRB-CountMSB-Info ::= SEQUENCE
struct drb_count_msb_info_s {
  uint8_t  drb_id       = 1;
  uint32_t count_msb_ul = 0;
  uint32_t count_msb_dl = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// FreqPriorityListEUTRA ::= SEQUENCE (SIZE (1..8)) OF FreqPriorityEUTRA
using freq_prio_list_eutra_l = dyn_array<freq_prio_eutra_s>;

// FreqPriorityListNR ::= SEQUENCE (SIZE (1..8)) OF FreqPriorityNR
using freq_prio_list_nr_l = dyn_array<freq_prio_nr_s>;

// MasterKeyUpdate ::= SEQUENCE
struct master_key_upd_s {
  bool          ext                     = false;
  bool          nas_container_present   = false;
  bool          key_set_change_ind      = false;
  uint8_t       next_hop_chaining_count = 0;
  dyn_octstring nas_container;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasGapConfig ::= SEQUENCE
struct meas_gap_cfg_s {
  bool                       ext             = false;
  bool                       gap_fr2_present = false;
  setup_release_c<gap_cfg_s> gap_fr2;
  // ...
  // group 0
  copy_ptr<setup_release_c<gap_cfg_s> > gap_fr1;
  copy_ptr<setup_release_c<gap_cfg_s> > gap_ue;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasGapSharingConfig ::= SEQUENCE
struct meas_gap_sharing_cfg_s {
  bool                                       ext                     = false;
  bool                                       gap_sharing_fr2_present = false;
  setup_release_c<meas_gap_sharing_scheme_e> gap_sharing_fr2;
  // ...
  // group 0
  copy_ptr<setup_release_c<meas_gap_sharing_scheme_e> > gap_sharing_fr1;
  copy_ptr<setup_release_c<meas_gap_sharing_scheme_e> > gap_sharing_ue;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasIdToAddModList ::= SEQUENCE (SIZE (1..64)) OF MeasIdToAddMod
using meas_id_to_add_mod_list_l = dyn_array<meas_id_to_add_mod_s>;

// MeasIdToRemoveList ::= SEQUENCE (SIZE (1..64)) OF INTEGER (1..64)
using meas_id_to_rem_list_l = dyn_array<uint8_t>;

// MeasObjectToAddModList ::= SEQUENCE (SIZE (1..64)) OF MeasObjectToAddMod
using meas_obj_to_add_mod_list_l = dyn_array<meas_obj_to_add_mod_s>;

// MeasObjectToRemoveList ::= SEQUENCE (SIZE (1..64)) OF INTEGER (1..64)
using meas_obj_to_rem_list_l = dyn_array<uint8_t>;

// OtherConfig ::= SEQUENCE
struct other_cfg_s {
  struct delay_budget_report_cfg_c_ {
    struct setup_s_ {
      struct delay_budget_report_prohibit_timer_opts {
        enum options { s0, s0dot4, s0dot8, s1dot6, s3, s6, s12, s30, nulltype } value;
        typedef float number_type;

        std::string to_string() const;
        float       to_number() const;
        std::string to_number_string() const;
      };
      typedef enumerated<delay_budget_report_prohibit_timer_opts> delay_budget_report_prohibit_timer_e_;

      // member variables
      delay_budget_report_prohibit_timer_e_ delay_budget_report_prohibit_timer;
    };
    struct types_opts {
      enum options { release, setup, nulltype } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    delay_budget_report_cfg_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    setup_s_& setup()
    {
      assert_choice_type("setup", type_.to_string(), "delayBudgetReportingConfig");
      return c;
    }
    const setup_s_& setup() const
    {
      assert_choice_type("setup", type_.to_string(), "delayBudgetReportingConfig");
      return c;
    }
    setup_s_& set_setup()
    {
      set(types::setup);
      return c;
    }

  private:
    types    type_;
    setup_s_ c;
  };

  // member variables
  bool                       delay_budget_report_cfg_present = false;
  delay_budget_report_cfg_c_ delay_budget_report_cfg;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PeriodicRNAU-TimerValue ::= ENUMERATED
struct periodic_rnau_timer_value_opts {
  enum options { min5, min10, min20, min30, min60, min120, min360, min720, nulltype } value;
  typedef uint16_t number_type;

  std::string to_string() const;
  uint16_t    to_number() const;
};
typedef enumerated<periodic_rnau_timer_value_opts> periodic_rnau_timer_value_e;

// QuantityConfig ::= SEQUENCE
struct quant_cfg_s {
  using quant_cfg_nr_list_l_ = dyn_array<quant_cfg_nr_s>;

  // member variables
  bool                 ext                       = false;
  bool                 quant_cfg_nr_list_present = false;
  quant_cfg_nr_list_l_ quant_cfg_nr_list;
  // ...
  // group 0
  copy_ptr<filt_cfg_s> quant_cfg_eutra;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RAN-NotificationAreaInfo ::= CHOICE
struct ran_notif_area_info_c {
  struct types_opts {
    enum options { cell_list, ran_area_cfg_list, /*...*/ nulltype } value;

    std::string to_string() const;
  };
  typedef enumerated<types_opts, true> types;

  // choice methods
  ran_notif_area_info_c() = default;
  ran_notif_area_info_c(const ran_notif_area_info_c& other);
  ran_notif_area_info_c& operator=(const ran_notif_area_info_c& other);
  ~ran_notif_area_info_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  plmn_ran_area_cell_list_l& cell_list()
  {
    assert_choice_type("cellList", type_.to_string(), "RAN-NotificationAreaInfo");
    return c.get<plmn_ran_area_cell_list_l>();
  }
  plmn_ran_area_cfg_list_l& ran_area_cfg_list()
  {
    assert_choice_type("ran-AreaConfigList", type_.to_string(), "RAN-NotificationAreaInfo");
    return c.get<plmn_ran_area_cfg_list_l>();
  }
  const plmn_ran_area_cell_list_l& cell_list() const
  {
    assert_choice_type("cellList", type_.to_string(), "RAN-NotificationAreaInfo");
    return c.get<plmn_ran_area_cell_list_l>();
  }
  const plmn_ran_area_cfg_list_l& ran_area_cfg_list() const
  {
    assert_choice_type("ran-AreaConfigList", type_.to_string(), "RAN-NotificationAreaInfo");
    return c.get<plmn_ran_area_cfg_list_l>();
  }
  plmn_ran_area_cell_list_l& set_cell_list()
  {
    set(types::cell_list);
    return c.get<plmn_ran_area_cell_list_l>();
  }
  plmn_ran_area_cfg_list_l& set_ran_area_cfg_list()
  {
    set(types::ran_area_cfg_list);
    return c.get<plmn_ran_area_cfg_list_l>();
  }

private:
  types                                                                type_;
  choice_buffer_t<plmn_ran_area_cell_list_l, plmn_ran_area_cfg_list_l> c;

  void destroy_();
};

// RRCReconfiguration-v1540-IEs ::= SEQUENCE
struct rrc_recfg_v1540_ies_s {
  bool              other_cfg_v1540_present = false;
  bool              non_crit_ext_present    = false;
  other_cfg_v1540_s other_cfg_v1540;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RedirectedCarrierInfo-EUTRA ::= SEQUENCE
struct redirected_carrier_info_eutra_s {
  struct cn_type_opts {
    enum options { epc, five_gc, nulltype } value;
    typedef uint8_t number_type;

    std::string to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<cn_type_opts> cn_type_e_;

  // member variables
  bool       cn_type_present = false;
  uint32_t   eutra_freq      = 0;
  cn_type_e_ cn_type;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ReportConfigToAddModList ::= SEQUENCE (SIZE (1..64)) OF ReportConfigToAddMod
using report_cfg_to_add_mod_list_l = dyn_array<report_cfg_to_add_mod_s>;

// ReportConfigToRemoveList ::= SEQUENCE (SIZE (1..64)) OF INTEGER (1..64)
using report_cfg_to_rem_list_l = dyn_array<uint8_t>;

// UE-CapabilityRAT-Request ::= SEQUENCE
struct ue_cap_rat_request_s {
  bool          ext                      = false;
  bool          cap_request_filt_present = false;
  rat_type_e    rat_type;
  dyn_octstring cap_request_filt;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CellReselectionPriorities ::= SEQUENCE
struct cell_resel_priorities_s {
  struct t320_opts {
    enum options { min5, min10, min20, min30, min60, min120, min180, spare1, nulltype } value;
    typedef uint8_t number_type;

    std::string to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<t320_opts> t320_e_;

  // member variables
  bool                   ext                          = false;
  bool                   freq_prio_list_eutra_present = false;
  bool                   freq_prio_list_nr_present    = false;
  bool                   t320_present                 = false;
  freq_prio_list_eutra_l freq_prio_list_eutra;
  freq_prio_list_nr_l    freq_prio_list_nr;
  t320_e_                t320;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DRB-CountMSB-InfoList ::= SEQUENCE (SIZE (1..29)) OF DRB-CountMSB-Info
using drb_count_msb_info_list_l = dyn_array<drb_count_msb_info_s>;

// MeasConfig ::= SEQUENCE
struct meas_cfg_s {
  struct s_measure_cfg_c_ {
    struct types_opts {
      enum options { ssb_rsrp, csi_rsrp, nulltype } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    s_measure_cfg_c_() = default;
    s_measure_cfg_c_(const s_measure_cfg_c_& other);
    s_measure_cfg_c_& operator=(const s_measure_cfg_c_& other);
    ~s_measure_cfg_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint8_t& ssb_rsrp()
    {
      assert_choice_type("ssb-RSRP", type_.to_string(), "s-MeasureConfig");
      return c.get<uint8_t>();
    }
    uint8_t& csi_rsrp()
    {
      assert_choice_type("csi-RSRP", type_.to_string(), "s-MeasureConfig");
      return c.get<uint8_t>();
    }
    const uint8_t& ssb_rsrp() const
    {
      assert_choice_type("ssb-RSRP", type_.to_string(), "s-MeasureConfig");
      return c.get<uint8_t>();
    }
    const uint8_t& csi_rsrp() const
    {
      assert_choice_type("csi-RSRP", type_.to_string(), "s-MeasureConfig");
      return c.get<uint8_t>();
    }
    uint8_t& set_ssb_rsrp()
    {
      set(types::ssb_rsrp);
      return c.get<uint8_t>();
    }
    uint8_t& set_csi_rsrp()
    {
      set(types::csi_rsrp);
      return c.get<uint8_t>();
    }

  private:
    types               type_;
    pod_choice_buffer_t c;

    void destroy_();
  };

  // member variables
  bool                         ext                                = false;
  bool                         meas_obj_to_rem_list_present       = false;
  bool                         meas_obj_to_add_mod_list_present   = false;
  bool                         report_cfg_to_rem_list_present     = false;
  bool                         report_cfg_to_add_mod_list_present = false;
  bool                         meas_id_to_rem_list_present        = false;
  bool                         meas_id_to_add_mod_list_present    = false;
  bool                         s_measure_cfg_present              = false;
  bool                         quant_cfg_present                  = false;
  bool                         meas_gap_cfg_present               = false;
  bool                         meas_gap_sharing_cfg_present       = false;
  meas_obj_to_rem_list_l       meas_obj_to_rem_list;
  meas_obj_to_add_mod_list_l   meas_obj_to_add_mod_list;
  report_cfg_to_rem_list_l     report_cfg_to_rem_list;
  report_cfg_to_add_mod_list_l report_cfg_to_add_mod_list;
  meas_id_to_rem_list_l        meas_id_to_rem_list;
  meas_id_to_add_mod_list_l    meas_id_to_add_mod_list;
  s_measure_cfg_c_             s_measure_cfg;
  quant_cfg_s                  quant_cfg;
  meas_gap_cfg_s               meas_gap_cfg;
  meas_gap_sharing_cfg_s       meas_gap_sharing_cfg;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCReconfiguration-v1530-IEs ::= SEQUENCE
struct rrc_recfg_v1530_ies_s {
  using ded_nas_msg_list_l_ = bounded_array<dyn_octstring, 29>;

  // member variables
  bool                  master_cell_group_present     = false;
  bool                  full_cfg_present              = false;
  bool                  ded_nas_msg_list_present      = false;
  bool                  master_key_upd_present        = false;
  bool                  ded_sib1_delivery_present     = false;
  bool                  ded_sys_info_delivery_present = false;
  bool                  other_cfg_present             = false;
  bool                  non_crit_ext_present          = false;
  dyn_octstring         master_cell_group;
  ded_nas_msg_list_l_   ded_nas_msg_list;
  master_key_upd_s      master_key_upd;
  dyn_octstring         ded_sib1_delivery;
  dyn_octstring         ded_sys_info_delivery;
  other_cfg_s           other_cfg;
  rrc_recfg_v1540_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCRelease-v1540-IEs ::= SEQUENCE
struct rrc_release_v1540_ies_s {
  bool    wait_time_present    = false;
  bool    non_crit_ext_present = false;
  uint8_t wait_time            = 1;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RedirectedCarrierInfo ::= CHOICE
struct redirected_carrier_info_c {
  struct types_opts {
    enum options { nr, eutra, /*...*/ nulltype } value;

    std::string to_string() const;
  };
  typedef enumerated<types_opts, true> types;

  // choice methods
  redirected_carrier_info_c() = default;
  redirected_carrier_info_c(const redirected_carrier_info_c& other);
  redirected_carrier_info_c& operator=(const redirected_carrier_info_c& other);
  ~redirected_carrier_info_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  carrier_info_nr_s& nr()
  {
    assert_choice_type("nr", type_.to_string(), "RedirectedCarrierInfo");
    return c.get<carrier_info_nr_s>();
  }
  redirected_carrier_info_eutra_s& eutra()
  {
    assert_choice_type("eutra", type_.to_string(), "RedirectedCarrierInfo");
    return c.get<redirected_carrier_info_eutra_s>();
  }
  const carrier_info_nr_s& nr() const
  {
    assert_choice_type("nr", type_.to_string(), "RedirectedCarrierInfo");
    return c.get<carrier_info_nr_s>();
  }
  const redirected_carrier_info_eutra_s& eutra() const
  {
    assert_choice_type("eutra", type_.to_string(), "RedirectedCarrierInfo");
    return c.get<redirected_carrier_info_eutra_s>();
  }
  carrier_info_nr_s& set_nr()
  {
    set(types::nr);
    return c.get<carrier_info_nr_s>();
  }
  redirected_carrier_info_eutra_s& set_eutra()
  {
    set(types::eutra);
    return c.get<redirected_carrier_info_eutra_s>();
  }

private:
  types                                                               type_;
  choice_buffer_t<carrier_info_nr_s, redirected_carrier_info_eutra_s> c;

  void destroy_();
};

// SecurityConfigSMC ::= SEQUENCE
struct security_cfg_smc_s {
  bool                     ext = false;
  security_algorithm_cfg_s security_algorithm_cfg;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SuspendConfig ::= SEQUENCE
struct suspend_cfg_s {
  bool                        ext                         = false;
  bool                        ran_notif_area_info_present = false;
  bool                        t380_present                = false;
  fixed_bitstring<40>         full_i_rnti;
  fixed_bitstring<24>         short_i_rnti;
  paging_cycle_e              ran_paging_cycle;
  ran_notif_area_info_c       ran_notif_area_info;
  periodic_rnau_timer_value_e t380;
  uint8_t                     next_hop_chaining_count = 0;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UE-CapabilityRAT-RequestList ::= SEQUENCE (SIZE (1..8)) OF UE-CapabilityRAT-Request
using ue_cap_rat_request_list_l = dyn_array<ue_cap_rat_request_s>;

// CounterCheck-IEs ::= SEQUENCE
struct counter_check_ies_s {
  bool                      late_non_crit_ext_present = false;
  bool                      non_crit_ext_present      = false;
  drb_count_msb_info_list_l drb_count_msb_info_list;
  dyn_octstring             late_non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DLInformationTransfer-IEs ::= SEQUENCE
struct dl_info_transfer_ies_s {
  bool          ded_nas_msg_present       = false;
  bool          late_non_crit_ext_present = false;
  bool          non_crit_ext_present      = false;
  dyn_octstring ded_nas_msg;
  dyn_octstring late_non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MobilityFromNRCommand-IEs ::= SEQUENCE
struct mob_from_nr_cmd_ies_s {
  struct target_rat_type_opts {
    enum options { eutra, spare3, spare2, spare1, /*...*/ nulltype } value;

    std::string to_string() const;
  };
  typedef enumerated<target_rat_type_opts, true> target_rat_type_e_;

  // member variables
  bool               nas_security_param_from_nr_present = false;
  bool               late_non_crit_ext_present          = false;
  bool               non_crit_ext_present               = false;
  target_rat_type_e_ target_rat_type;
  dyn_octstring      target_rat_msg_container;
  dyn_octstring      nas_security_param_from_nr;
  dyn_octstring      late_non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCReconfiguration-IEs ::= SEQUENCE
struct rrc_recfg_ies_s {
  bool                  radio_bearer_cfg_present     = false;
  bool                  secondary_cell_group_present = false;
  bool                  meas_cfg_present             = false;
  bool                  late_non_crit_ext_present    = false;
  bool                  non_crit_ext_present         = false;
  radio_bearer_cfg_s    radio_bearer_cfg;
  dyn_octstring         secondary_cell_group;
  meas_cfg_s            meas_cfg;
  dyn_octstring         late_non_crit_ext;
  rrc_recfg_v1530_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCReestablishment-IEs ::= SEQUENCE
struct rrc_reest_ies_s {
  bool          late_non_crit_ext_present = false;
  bool          non_crit_ext_present      = false;
  uint8_t       next_hop_chaining_count   = 0;
  dyn_octstring late_non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCRelease-IEs ::= SEQUENCE
struct rrc_release_ies_s {
  struct depriorit_req_s_ {
    struct depriorit_type_opts {
      enum options { freq, nr, nulltype } value;

      std::string to_string() const;
    };
    typedef enumerated<depriorit_type_opts> depriorit_type_e_;
    struct depriorit_timer_opts {
      enum options { min5, min10, min15, min30, nulltype } value;
      typedef uint8_t number_type;

      std::string to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<depriorit_timer_opts> depriorit_timer_e_;

    // member variables
    depriorit_type_e_  depriorit_type;
    depriorit_timer_e_ depriorit_timer;
  };

  // member variables
  bool                      redirected_carrier_info_present = false;
  bool                      cell_resel_priorities_present   = false;
  bool                      suspend_cfg_present             = false;
  bool                      depriorit_req_present           = false;
  bool                      late_non_crit_ext_present       = false;
  bool                      non_crit_ext_present            = false;
  redirected_carrier_info_c redirected_carrier_info;
  cell_resel_priorities_s   cell_resel_priorities;
  suspend_cfg_s             suspend_cfg;
  depriorit_req_s_          depriorit_req;
  dyn_octstring             late_non_crit_ext;
  rrc_release_v1540_ies_s   non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCResume-IEs ::= SEQUENCE
struct rrc_resume_ies_s {
  bool               radio_bearer_cfg_present  = false;
  bool               master_cell_group_present = false;
  bool               meas_cfg_present          = false;
  bool               full_cfg_present          = false;
  bool               late_non_crit_ext_present = false;
  bool               non_crit_ext_present      = false;
  radio_bearer_cfg_s radio_bearer_cfg;
  dyn_octstring      master_cell_group;
  meas_cfg_s         meas_cfg;
  dyn_octstring      late_non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SecurityModeCommand-IEs ::= SEQUENCE
struct security_mode_cmd_ies_s {
  bool               late_non_crit_ext_present = false;
  bool               non_crit_ext_present      = false;
  security_cfg_smc_s security_cfg_smc;
  dyn_octstring      late_non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UECapabilityEnquiry-IEs ::= SEQUENCE
struct ue_cap_enquiry_ies_s {
  bool                      late_non_crit_ext_present = false;
  bool                      non_crit_ext_present      = false;
  ue_cap_rat_request_list_l ue_cap_rat_request_list;
  dyn_octstring             late_non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CounterCheck ::= SEQUENCE
struct counter_check_s {
  struct crit_exts_c_ {
    struct types_opts {
      enum options { counter_check, crit_exts_future, nulltype } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    crit_exts_c_() = default;
    crit_exts_c_(const crit_exts_c_& other);
    crit_exts_c_& operator=(const crit_exts_c_& other);
    ~crit_exts_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    counter_check_ies_s& counter_check()
    {
      assert_choice_type("counterCheck", type_.to_string(), "criticalExtensions");
      return c.get<counter_check_ies_s>();
    }
    const counter_check_ies_s& counter_check() const
    {
      assert_choice_type("counterCheck", type_.to_string(), "criticalExtensions");
      return c.get<counter_check_ies_s>();
    }
    counter_check_ies_s& set_counter_check()
    {
      set(types::counter_check);
      return c.get<counter_check_ies_s>();
    }

  private:
    types                                type_;
    choice_buffer_t<counter_check_ies_s> c;

    void destroy_();
  };

  // member variables
  uint8_t      rrc_transaction_id = 0;
  crit_exts_c_ crit_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DLInformationTransfer ::= SEQUENCE
struct dl_info_transfer_s {
  struct crit_exts_c_ {
    struct types_opts {
      enum options { dl_info_transfer, crit_exts_future, nulltype } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    crit_exts_c_() = default;
    crit_exts_c_(const crit_exts_c_& other);
    crit_exts_c_& operator=(const crit_exts_c_& other);
    ~crit_exts_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    dl_info_transfer_ies_s& dl_info_transfer()
    {
      assert_choice_type("dlInformationTransfer", type_.to_string(), "criticalExtensions");
      return c.get<dl_info_transfer_ies_s>();
    }
    const dl_info_transfer_ies_s& dl_info_transfer() const
    {
      assert_choice_type("dlInformationTransfer", type_.to_string(), "criticalExtensions");
      return c.get<dl_info_transfer_ies_s>();
    }
    dl_info_transfer_ies_s& set_dl_info_transfer()
    {
      set(types::dl_info_transfer);
      return c.get<dl_info_transfer_ies_s>();
    }

  private:
    types                                   type_;
    choice_buffer_t<dl_info_transfer_ies_s> c;

    void destroy_();
  };

  // member variables
  uint8_t      rrc_transaction_id = 0;
  crit_exts_c_ crit_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MobilityFromNRCommand ::= SEQUENCE
struct mob_from_nr_cmd_s {
  struct crit_exts_c_ {
    struct types_opts {
      enum options { mob_from_nr_cmd, crit_exts_future, nulltype } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    crit_exts_c_() = default;
    crit_exts_c_(const crit_exts_c_& other);
    crit_exts_c_& operator=(const crit_exts_c_& other);
    ~crit_exts_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    mob_from_nr_cmd_ies_s& mob_from_nr_cmd()
    {
      assert_choice_type("mobilityFromNRCommand", type_.to_string(), "criticalExtensions");
      return c.get<mob_from_nr_cmd_ies_s>();
    }
    const mob_from_nr_cmd_ies_s& mob_from_nr_cmd() const
    {
      assert_choice_type("mobilityFromNRCommand", type_.to_string(), "criticalExtensions");
      return c.get<mob_from_nr_cmd_ies_s>();
    }
    mob_from_nr_cmd_ies_s& set_mob_from_nr_cmd()
    {
      set(types::mob_from_nr_cmd);
      return c.get<mob_from_nr_cmd_ies_s>();
    }

  private:
    types                                  type_;
    choice_buffer_t<mob_from_nr_cmd_ies_s> c;

    void destroy_();
  };

  // member variables
  uint8_t      rrc_transaction_id = 0;
  crit_exts_c_ crit_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCReconfiguration ::= SEQUENCE
struct rrc_recfg_s {
  struct crit_exts_c_ {
    struct types_opts {
      enum options { rrc_recfg, crit_exts_future, nulltype } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    crit_exts_c_() = default;
    crit_exts_c_(const crit_exts_c_& other);
    crit_exts_c_& operator=(const crit_exts_c_& other);
    ~crit_exts_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    rrc_recfg_ies_s& rrc_recfg()
    {
      assert_choice_type("rrcReconfiguration", type_.to_string(), "criticalExtensions");
      return c.get<rrc_recfg_ies_s>();
    }
    const rrc_recfg_ies_s& rrc_recfg() const
    {
      assert_choice_type("rrcReconfiguration", type_.to_string(), "criticalExtensions");
      return c.get<rrc_recfg_ies_s>();
    }
    rrc_recfg_ies_s& set_rrc_recfg()
    {
      set(types::rrc_recfg);
      return c.get<rrc_recfg_ies_s>();
    }

  private:
    types                            type_;
    choice_buffer_t<rrc_recfg_ies_s> c;

    void destroy_();
  };

  // member variables
  uint8_t      rrc_transaction_id = 0;
  crit_exts_c_ crit_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCReestablishment ::= SEQUENCE
struct rrc_reest_s {
  struct crit_exts_c_ {
    struct types_opts {
      enum options { rrc_reest, crit_exts_future, nulltype } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    crit_exts_c_() = default;
    crit_exts_c_(const crit_exts_c_& other);
    crit_exts_c_& operator=(const crit_exts_c_& other);
    ~crit_exts_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    rrc_reest_ies_s& rrc_reest()
    {
      assert_choice_type("rrcReestablishment", type_.to_string(), "criticalExtensions");
      return c.get<rrc_reest_ies_s>();
    }
    const rrc_reest_ies_s& rrc_reest() const
    {
      assert_choice_type("rrcReestablishment", type_.to_string(), "criticalExtensions");
      return c.get<rrc_reest_ies_s>();
    }
    rrc_reest_ies_s& set_rrc_reest()
    {
      set(types::rrc_reest);
      return c.get<rrc_reest_ies_s>();
    }

  private:
    types                            type_;
    choice_buffer_t<rrc_reest_ies_s> c;

    void destroy_();
  };

  // member variables
  uint8_t      rrc_transaction_id = 0;
  crit_exts_c_ crit_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCRelease ::= SEQUENCE
struct rrc_release_s {
  struct crit_exts_c_ {
    struct types_opts {
      enum options { rrc_release, crit_exts_future, nulltype } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    crit_exts_c_() = default;
    crit_exts_c_(const crit_exts_c_& other);
    crit_exts_c_& operator=(const crit_exts_c_& other);
    ~crit_exts_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    rrc_release_ies_s& rrc_release()
    {
      assert_choice_type("rrcRelease", type_.to_string(), "criticalExtensions");
      return c.get<rrc_release_ies_s>();
    }
    const rrc_release_ies_s& rrc_release() const
    {
      assert_choice_type("rrcRelease", type_.to_string(), "criticalExtensions");
      return c.get<rrc_release_ies_s>();
    }
    rrc_release_ies_s& set_rrc_release()
    {
      set(types::rrc_release);
      return c.get<rrc_release_ies_s>();
    }

  private:
    types                              type_;
    choice_buffer_t<rrc_release_ies_s> c;

    void destroy_();
  };

  // member variables
  uint8_t      rrc_transaction_id = 0;
  crit_exts_c_ crit_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCResume ::= SEQUENCE
struct rrc_resume_s {
  struct crit_exts_c_ {
    struct types_opts {
      enum options { rrc_resume, crit_exts_future, nulltype } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    crit_exts_c_() = default;
    crit_exts_c_(const crit_exts_c_& other);
    crit_exts_c_& operator=(const crit_exts_c_& other);
    ~crit_exts_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    rrc_resume_ies_s& rrc_resume()
    {
      assert_choice_type("rrcResume", type_.to_string(), "criticalExtensions");
      return c.get<rrc_resume_ies_s>();
    }
    const rrc_resume_ies_s& rrc_resume() const
    {
      assert_choice_type("rrcResume", type_.to_string(), "criticalExtensions");
      return c.get<rrc_resume_ies_s>();
    }
    rrc_resume_ies_s& set_rrc_resume()
    {
      set(types::rrc_resume);
      return c.get<rrc_resume_ies_s>();
    }

  private:
    types                             type_;
    choice_buffer_t<rrc_resume_ies_s> c;

    void destroy_();
  };

  // member variables
  uint8_t      rrc_transaction_id = 0;
  crit_exts_c_ crit_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SecurityModeCommand ::= SEQUENCE
struct security_mode_cmd_s {
  struct crit_exts_c_ {
    struct types_opts {
      enum options { security_mode_cmd, crit_exts_future, nulltype } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    crit_exts_c_() = default;
    crit_exts_c_(const crit_exts_c_& other);
    crit_exts_c_& operator=(const crit_exts_c_& other);
    ~crit_exts_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    security_mode_cmd_ies_s& security_mode_cmd()
    {
      assert_choice_type("securityModeCommand", type_.to_string(), "criticalExtensions");
      return c.get<security_mode_cmd_ies_s>();
    }
    const security_mode_cmd_ies_s& security_mode_cmd() const
    {
      assert_choice_type("securityModeCommand", type_.to_string(), "criticalExtensions");
      return c.get<security_mode_cmd_ies_s>();
    }
    security_mode_cmd_ies_s& set_security_mode_cmd()
    {
      set(types::security_mode_cmd);
      return c.get<security_mode_cmd_ies_s>();
    }

  private:
    types                                    type_;
    choice_buffer_t<security_mode_cmd_ies_s> c;

    void destroy_();
  };

  // member variables
  uint8_t      rrc_transaction_id = 0;
  crit_exts_c_ crit_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UECapabilityEnquiry ::= SEQUENCE
struct ue_cap_enquiry_s {
  struct crit_exts_c_ {
    struct types_opts {
      enum options { ue_cap_enquiry, crit_exts_future, nulltype } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    crit_exts_c_() = default;
    crit_exts_c_(const crit_exts_c_& other);
    crit_exts_c_& operator=(const crit_exts_c_& other);
    ~crit_exts_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    ue_cap_enquiry_ies_s& ue_cap_enquiry()
    {
      assert_choice_type("ueCapabilityEnquiry", type_.to_string(), "criticalExtensions");
      return c.get<ue_cap_enquiry_ies_s>();
    }
    const ue_cap_enquiry_ies_s& ue_cap_enquiry() const
    {
      assert_choice_type("ueCapabilityEnquiry", type_.to_string(), "criticalExtensions");
      return c.get<ue_cap_enquiry_ies_s>();
    }
    ue_cap_enquiry_ies_s& set_ue_cap_enquiry()
    {
      set(types::ue_cap_enquiry);
      return c.get<ue_cap_enquiry_ies_s>();
    }

  private:
    types                                 type_;
    choice_buffer_t<ue_cap_enquiry_ies_s> c;

    void destroy_();
  };

  // member variables
  uint8_t      rrc_transaction_id = 0;
  crit_exts_c_ crit_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DL-DCCH-MessageType ::= CHOICE
struct dl_dcch_msg_type_c {
  struct c1_c_ {
    struct types_opts {
      enum options {
        rrc_recfg,
        rrc_resume,
        rrc_release,
        rrc_reest,
        security_mode_cmd,
        dl_info_transfer,
        ue_cap_enquiry,
        counter_check,
        mob_from_nr_cmd,
        spare7,
        spare6,
        spare5,
        spare4,
        spare3,
        spare2,
        spare1,
        nulltype
      } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    c1_c_() = default;
    c1_c_(const c1_c_& other);
    c1_c_& operator=(const c1_c_& other);
    ~c1_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    rrc_recfg_s& rrc_recfg()
    {
      assert_choice_type("rrcReconfiguration", type_.to_string(), "c1");
      return c.get<rrc_recfg_s>();
    }
    rrc_resume_s& rrc_resume()
    {
      assert_choice_type("rrcResume", type_.to_string(), "c1");
      return c.get<rrc_resume_s>();
    }
    rrc_release_s& rrc_release()
    {
      assert_choice_type("rrcRelease", type_.to_string(), "c1");
      return c.get<rrc_release_s>();
    }
    rrc_reest_s& rrc_reest()
    {
      assert_choice_type("rrcReestablishment", type_.to_string(), "c1");
      return c.get<rrc_reest_s>();
    }
    security_mode_cmd_s& security_mode_cmd()
    {
      assert_choice_type("securityModeCommand", type_.to_string(), "c1");
      return c.get<security_mode_cmd_s>();
    }
    dl_info_transfer_s& dl_info_transfer()
    {
      assert_choice_type("dlInformationTransfer", type_.to_string(), "c1");
      return c.get<dl_info_transfer_s>();
    }
    ue_cap_enquiry_s& ue_cap_enquiry()
    {
      assert_choice_type("ueCapabilityEnquiry", type_.to_string(), "c1");
      return c.get<ue_cap_enquiry_s>();
    }
    counter_check_s& counter_check()
    {
      assert_choice_type("counterCheck", type_.to_string(), "c1");
      return c.get<counter_check_s>();
    }
    mob_from_nr_cmd_s& mob_from_nr_cmd()
    {
      assert_choice_type("mobilityFromNRCommand", type_.to_string(), "c1");
      return c.get<mob_from_nr_cmd_s>();
    }
    const rrc_recfg_s& rrc_recfg() const
    {
      assert_choice_type("rrcReconfiguration", type_.to_string(), "c1");
      return c.get<rrc_recfg_s>();
    }
    const rrc_resume_s& rrc_resume() const
    {
      assert_choice_type("rrcResume", type_.to_string(), "c1");
      return c.get<rrc_resume_s>();
    }
    const rrc_release_s& rrc_release() const
    {
      assert_choice_type("rrcRelease", type_.to_string(), "c1");
      return c.get<rrc_release_s>();
    }
    const rrc_reest_s& rrc_reest() const
    {
      assert_choice_type("rrcReestablishment", type_.to_string(), "c1");
      return c.get<rrc_reest_s>();
    }
    const security_mode_cmd_s& security_mode_cmd() const
    {
      assert_choice_type("securityModeCommand", type_.to_string(), "c1");
      return c.get<security_mode_cmd_s>();
    }
    const dl_info_transfer_s& dl_info_transfer() const
    {
      assert_choice_type("dlInformationTransfer", type_.to_string(), "c1");
      return c.get<dl_info_transfer_s>();
    }
    const ue_cap_enquiry_s& ue_cap_enquiry() const
    {
      assert_choice_type("ueCapabilityEnquiry", type_.to_string(), "c1");
      return c.get<ue_cap_enquiry_s>();
    }
    const counter_check_s& counter_check() const
    {
      assert_choice_type("counterCheck", type_.to_string(), "c1");
      return c.get<counter_check_s>();
    }
    const mob_from_nr_cmd_s& mob_from_nr_cmd() const
    {
      assert_choice_type("mobilityFromNRCommand", type_.to_string(), "c1");
      return c.get<mob_from_nr_cmd_s>();
    }
    rrc_recfg_s& set_rrc_recfg()
    {
      set(types::rrc_recfg);
      return c.get<rrc_recfg_s>();
    }
    rrc_resume_s& set_rrc_resume()
    {
      set(types::rrc_resume);
      return c.get<rrc_resume_s>();
    }
    rrc_release_s& set_rrc_release()
    {
      set(types::rrc_release);
      return c.get<rrc_release_s>();
    }
    rrc_reest_s& set_rrc_reest()
    {
      set(types::rrc_reest);
      return c.get<rrc_reest_s>();
    }
    security_mode_cmd_s& set_security_mode_cmd()
    {
      set(types::security_mode_cmd);
      return c.get<security_mode_cmd_s>();
    }
    dl_info_transfer_s& set_dl_info_transfer()
    {
      set(types::dl_info_transfer);
      return c.get<dl_info_transfer_s>();
    }
    ue_cap_enquiry_s& set_ue_cap_enquiry()
    {
      set(types::ue_cap_enquiry);
      return c.get<ue_cap_enquiry_s>();
    }
    counter_check_s& set_counter_check()
    {
      set(types::counter_check);
      return c.get<counter_check_s>();
    }
    mob_from_nr_cmd_s& set_mob_from_nr_cmd()
    {
      set(types::mob_from_nr_cmd);
      return c.get<mob_from_nr_cmd_s>();
    }

  private:
    types type_;
    choice_buffer_t<counter_check_s,
                    dl_info_transfer_s,
                    mob_from_nr_cmd_s,
                    rrc_recfg_s,
                    rrc_reest_s,
                    rrc_release_s,
                    rrc_resume_s,
                    security_mode_cmd_s,
                    ue_cap_enquiry_s>
        c;

    void destroy_();
  };
  struct types_opts {
    enum options { c1, msg_class_ext, nulltype } value;
    typedef uint8_t number_type;

    std::string to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<types_opts> types;

  // choice methods
  dl_dcch_msg_type_c() = default;
  dl_dcch_msg_type_c(const dl_dcch_msg_type_c& other);
  dl_dcch_msg_type_c& operator=(const dl_dcch_msg_type_c& other);
  ~dl_dcch_msg_type_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  c1_c_& c1()
  {
    assert_choice_type("c1", type_.to_string(), "DL-DCCH-MessageType");
    return c.get<c1_c_>();
  }
  const c1_c_& c1() const
  {
    assert_choice_type("c1", type_.to_string(), "DL-DCCH-MessageType");
    return c.get<c1_c_>();
  }
  c1_c_& set_c1()
  {
    set(types::c1);
    return c.get<c1_c_>();
  }

private:
  types                  type_;
  choice_buffer_t<c1_c_> c;

  void destroy_();
};

// DL-DCCH-Message ::= SEQUENCE
struct dl_dcch_msg_s {
  dl_dcch_msg_type_c msg;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PagingUE-Identity ::= CHOICE
struct paging_ue_id_c {
  struct types_opts {
    enum options { ng_minus5_g_s_tmsi, full_i_rnti, /*...*/ nulltype } value;
    typedef int8_t number_type;

    std::string to_string() const;
    int8_t      to_number() const;
  };
  typedef enumerated<types_opts, true> types;

  // choice methods
  paging_ue_id_c() = default;
  paging_ue_id_c(const paging_ue_id_c& other);
  paging_ue_id_c& operator=(const paging_ue_id_c& other);
  ~paging_ue_id_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  fixed_bitstring<48>& ng_minus5_g_s_tmsi()
  {
    assert_choice_type("ng-5G-S-TMSI", type_.to_string(), "PagingUE-Identity");
    return c.get<fixed_bitstring<48> >();
  }
  fixed_bitstring<40>& full_i_rnti()
  {
    assert_choice_type("fullI-RNTI", type_.to_string(), "PagingUE-Identity");
    return c.get<fixed_bitstring<40> >();
  }
  const fixed_bitstring<48>& ng_minus5_g_s_tmsi() const
  {
    assert_choice_type("ng-5G-S-TMSI", type_.to_string(), "PagingUE-Identity");
    return c.get<fixed_bitstring<48> >();
  }
  const fixed_bitstring<40>& full_i_rnti() const
  {
    assert_choice_type("fullI-RNTI", type_.to_string(), "PagingUE-Identity");
    return c.get<fixed_bitstring<40> >();
  }
  fixed_bitstring<48>& set_ng_minus5_g_s_tmsi()
  {
    set(types::ng_minus5_g_s_tmsi);
    return c.get<fixed_bitstring<48> >();
  }
  fixed_bitstring<40>& set_full_i_rnti()
  {
    set(types::full_i_rnti);
    return c.get<fixed_bitstring<40> >();
  }

private:
  types                                 type_;
  choice_buffer_t<fixed_bitstring<48> > c;

  void destroy_();
};

// PagingRecord ::= SEQUENCE
struct paging_record_s {
  bool           ext                 = false;
  bool           access_type_present = false;
  paging_ue_id_c ue_id;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PagingRecordList ::= SEQUENCE (SIZE (1..32)) OF PagingRecord
using paging_record_list_l = dyn_array<paging_record_s>;

// Paging ::= SEQUENCE
struct paging_s {
  bool                 paging_record_list_present = false;
  bool                 late_non_crit_ext_present  = false;
  bool                 non_crit_ext_present       = false;
  paging_record_list_l paging_record_list;
  dyn_octstring        late_non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PCCH-MessageType ::= CHOICE
struct pcch_msg_type_c {
  struct c1_c_ {
    struct types_opts {
      enum options { paging, spare1, nulltype } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    c1_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    paging_s& paging()
    {
      assert_choice_type("paging", type_.to_string(), "c1");
      return c;
    }
    const paging_s& paging() const
    {
      assert_choice_type("paging", type_.to_string(), "c1");
      return c;
    }
    paging_s& set_paging()
    {
      set(types::paging);
      return c;
    }

  private:
    types    type_;
    paging_s c;
  };
  struct types_opts {
    enum options { c1, msg_class_ext, nulltype } value;
    typedef uint8_t number_type;

    std::string to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<types_opts> types;

  // choice methods
  pcch_msg_type_c() = default;
  pcch_msg_type_c(const pcch_msg_type_c& other);
  pcch_msg_type_c& operator=(const pcch_msg_type_c& other);
  ~pcch_msg_type_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  c1_c_& c1()
  {
    assert_choice_type("c1", type_.to_string(), "PCCH-MessageType");
    return c.get<c1_c_>();
  }
  const c1_c_& c1() const
  {
    assert_choice_type("c1", type_.to_string(), "PCCH-MessageType");
    return c.get<c1_c_>();
  }
  c1_c_& set_c1()
  {
    set(types::c1);
    return c.get<c1_c_>();
  }

private:
  types                  type_;
  choice_buffer_t<c1_c_> c;

  void destroy_();
};

// PCCH-Message ::= SEQUENCE
struct pcch_msg_s {
  pcch_msg_type_c msg;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// EstablishmentCause ::= ENUMERATED
struct establishment_cause_opts {
  enum options {
    emergency,
    high_prio_access,
    mt_access,
    mo_sig,
    mo_data,
    mo_voice_call,
    mo_video_call,
    mo_sms,
    mps_prio_access,
    mcs_prio_access,
    spare6,
    spare5,
    spare4,
    spare3,
    spare2,
    spare1,
    nulltype
  } value;

  std::string to_string() const;
};
typedef enumerated<establishment_cause_opts> establishment_cause_e;

// InitialUE-Identity ::= CHOICE
struct init_ue_id_c {
  struct types_opts {
    enum options { ng_minus5_g_s_tmsi_part1, random_value, nulltype } value;
    typedef int8_t number_type;

    std::string to_string() const;
    int8_t      to_number() const;
  };
  typedef enumerated<types_opts> types;

  // choice methods
  init_ue_id_c() = default;
  init_ue_id_c(const init_ue_id_c& other);
  init_ue_id_c& operator=(const init_ue_id_c& other);
  ~init_ue_id_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  fixed_bitstring<39>& ng_minus5_g_s_tmsi_part1()
  {
    assert_choice_type("ng-5G-S-TMSI-Part1", type_.to_string(), "InitialUE-Identity");
    return c.get<fixed_bitstring<39> >();
  }
  fixed_bitstring<39>& random_value()
  {
    assert_choice_type("randomValue", type_.to_string(), "InitialUE-Identity");
    return c.get<fixed_bitstring<39> >();
  }
  const fixed_bitstring<39>& ng_minus5_g_s_tmsi_part1() const
  {
    assert_choice_type("ng-5G-S-TMSI-Part1", type_.to_string(), "InitialUE-Identity");
    return c.get<fixed_bitstring<39> >();
  }
  const fixed_bitstring<39>& random_value() const
  {
    assert_choice_type("randomValue", type_.to_string(), "InitialUE-Identity");
    return c.get<fixed_bitstring<39> >();
  }
  fixed_bitstring<39>& set_ng_minus5_g_s_tmsi_part1()
  {
    set(types::ng_minus5_g_s_tmsi_part1);
    return c.get<fixed_bitstring<39> >();
  }
  fixed_bitstring<39>& set_random_value()
  {
    set(types::random_value);
    return c.get<fixed_bitstring<39> >();
  }

private:
  types                                 type_;
  choice_buffer_t<fixed_bitstring<39> > c;

  void destroy_();
};

// ReestabUE-Identity ::= SEQUENCE
struct reestab_ue_id_s {
  uint32_t            c_rnti = 0;
  uint16_t            pci    = 0;
  fixed_bitstring<16> short_mac_i;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ReestablishmentCause ::= ENUMERATED
struct reest_cause_opts {
  enum options { recfg_fail, ho_fail, other_fail, spare1, nulltype } value;

  std::string to_string() const;
};
typedef enumerated<reest_cause_opts> reest_cause_e;

// ResumeCause ::= ENUMERATED
struct resume_cause_opts {
  enum options {
    emergency,
    high_prio_access,
    mt_access,
    mo_sig,
    mo_data,
    mo_voice_call,
    mo_video_call,
    mo_sms,
    rna_upd,
    mps_prio_access,
    mcs_prio_access,
    spare1,
    spare2,
    spare3,
    spare4,
    spare5,
    nulltype
  } value;

  std::string to_string() const;
};
typedef enumerated<resume_cause_opts> resume_cause_e;

// RRCReestablishmentRequest-IEs ::= SEQUENCE
struct rrc_reest_request_ies_s {
  reestab_ue_id_s    ue_id;
  reest_cause_e      reest_cause;
  fixed_bitstring<1> spare;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCResumeRequest-IEs ::= SEQUENCE
struct rrc_resume_request_ies_s {
  fixed_bitstring<24> resume_id;
  fixed_bitstring<16> resume_mac_i;
  resume_cause_e      resume_cause;
  fixed_bitstring<1>  spare;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCSetupRequest-IEs ::= SEQUENCE
struct rrc_setup_request_ies_s {
  init_ue_id_c          ue_id;
  establishment_cause_e establishment_cause;
  fixed_bitstring<1>    spare;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCSystemInfoRequest-r15-IEs ::= SEQUENCE
struct rrc_sys_info_request_r15_ies_s {
  fixed_bitstring<32> requested_si_list;
  fixed_bitstring<12> spare;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCReestablishmentRequest ::= SEQUENCE
struct rrc_reest_request_s {
  rrc_reest_request_ies_s rrc_reest_request;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCResumeRequest ::= SEQUENCE
struct rrc_resume_request_s {
  rrc_resume_request_ies_s rrc_resume_request;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCSetupRequest ::= SEQUENCE
struct rrc_setup_request_s {
  rrc_setup_request_ies_s rrc_setup_request;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCSystemInfoRequest ::= SEQUENCE
struct rrc_sys_info_request_s {
  struct crit_exts_c_ {
    struct types_opts {
      enum options { rrc_sys_info_request_r15, crit_exts_future, nulltype } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    crit_exts_c_() = default;
    crit_exts_c_(const crit_exts_c_& other);
    crit_exts_c_& operator=(const crit_exts_c_& other);
    ~crit_exts_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    rrc_sys_info_request_r15_ies_s& rrc_sys_info_request_r15()
    {
      assert_choice_type("rrcSystemInfoRequest-r15", type_.to_string(), "criticalExtensions");
      return c.get<rrc_sys_info_request_r15_ies_s>();
    }
    const rrc_sys_info_request_r15_ies_s& rrc_sys_info_request_r15() const
    {
      assert_choice_type("rrcSystemInfoRequest-r15", type_.to_string(), "criticalExtensions");
      return c.get<rrc_sys_info_request_r15_ies_s>();
    }
    rrc_sys_info_request_r15_ies_s& set_rrc_sys_info_request_r15()
    {
      set(types::rrc_sys_info_request_r15);
      return c.get<rrc_sys_info_request_r15_ies_s>();
    }

  private:
    types                                           type_;
    choice_buffer_t<rrc_sys_info_request_r15_ies_s> c;

    void destroy_();
  };

  // member variables
  crit_exts_c_ crit_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UL-CCCH-MessageType ::= CHOICE
struct ul_ccch_msg_type_c {
  struct c1_c_ {
    struct types_opts {
      enum options { rrc_setup_request, rrc_resume_request, rrc_reest_request, rrc_sys_info_request, nulltype } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    c1_c_() = default;
    c1_c_(const c1_c_& other);
    c1_c_& operator=(const c1_c_& other);
    ~c1_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    rrc_setup_request_s& rrc_setup_request()
    {
      assert_choice_type("rrcSetupRequest", type_.to_string(), "c1");
      return c.get<rrc_setup_request_s>();
    }
    rrc_resume_request_s& rrc_resume_request()
    {
      assert_choice_type("rrcResumeRequest", type_.to_string(), "c1");
      return c.get<rrc_resume_request_s>();
    }
    rrc_reest_request_s& rrc_reest_request()
    {
      assert_choice_type("rrcReestablishmentRequest", type_.to_string(), "c1");
      return c.get<rrc_reest_request_s>();
    }
    rrc_sys_info_request_s& rrc_sys_info_request()
    {
      assert_choice_type("rrcSystemInfoRequest", type_.to_string(), "c1");
      return c.get<rrc_sys_info_request_s>();
    }
    const rrc_setup_request_s& rrc_setup_request() const
    {
      assert_choice_type("rrcSetupRequest", type_.to_string(), "c1");
      return c.get<rrc_setup_request_s>();
    }
    const rrc_resume_request_s& rrc_resume_request() const
    {
      assert_choice_type("rrcResumeRequest", type_.to_string(), "c1");
      return c.get<rrc_resume_request_s>();
    }
    const rrc_reest_request_s& rrc_reest_request() const
    {
      assert_choice_type("rrcReestablishmentRequest", type_.to_string(), "c1");
      return c.get<rrc_reest_request_s>();
    }
    const rrc_sys_info_request_s& rrc_sys_info_request() const
    {
      assert_choice_type("rrcSystemInfoRequest", type_.to_string(), "c1");
      return c.get<rrc_sys_info_request_s>();
    }
    rrc_setup_request_s& set_rrc_setup_request()
    {
      set(types::rrc_setup_request);
      return c.get<rrc_setup_request_s>();
    }
    rrc_resume_request_s& set_rrc_resume_request()
    {
      set(types::rrc_resume_request);
      return c.get<rrc_resume_request_s>();
    }
    rrc_reest_request_s& set_rrc_reest_request()
    {
      set(types::rrc_reest_request);
      return c.get<rrc_reest_request_s>();
    }
    rrc_sys_info_request_s& set_rrc_sys_info_request()
    {
      set(types::rrc_sys_info_request);
      return c.get<rrc_sys_info_request_s>();
    }

  private:
    types                                                                                                   type_;
    choice_buffer_t<rrc_reest_request_s, rrc_resume_request_s, rrc_setup_request_s, rrc_sys_info_request_s> c;

    void destroy_();
  };
  struct types_opts {
    enum options { c1, msg_class_ext, nulltype } value;
    typedef uint8_t number_type;

    std::string to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<types_opts> types;

  // choice methods
  ul_ccch_msg_type_c() = default;
  ul_ccch_msg_type_c(const ul_ccch_msg_type_c& other);
  ul_ccch_msg_type_c& operator=(const ul_ccch_msg_type_c& other);
  ~ul_ccch_msg_type_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  c1_c_& c1()
  {
    assert_choice_type("c1", type_.to_string(), "UL-CCCH-MessageType");
    return c.get<c1_c_>();
  }
  const c1_c_& c1() const
  {
    assert_choice_type("c1", type_.to_string(), "UL-CCCH-MessageType");
    return c.get<c1_c_>();
  }
  c1_c_& set_c1()
  {
    set(types::c1);
    return c.get<c1_c_>();
  }

private:
  types                  type_;
  choice_buffer_t<c1_c_> c;

  void destroy_();
};

// UL-CCCH-Message ::= SEQUENCE
struct ul_ccch_msg_s {
  ul_ccch_msg_type_c msg;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCResumeRequest1-IEs ::= SEQUENCE
struct rrc_resume_request1_ies_s {
  fixed_bitstring<40> resume_id;
  fixed_bitstring<16> resume_mac_i;
  resume_cause_e      resume_cause;
  fixed_bitstring<1>  spare;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCResumeRequest1 ::= SEQUENCE
struct rrc_resume_request1_s {
  rrc_resume_request1_ies_s rrc_resume_request1;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UL-CCCH1-MessageType ::= CHOICE
struct ul_ccch1_msg_type_c {
  struct c1_c_ {
    struct types_opts {
      enum options { rrc_resume_request1, spare3, spare2, spare1, nulltype } value;
      typedef uint8_t number_type;

      std::string to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    c1_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    rrc_resume_request1_s& rrc_resume_request1()
    {
      assert_choice_type("rrcResumeRequest1", type_.to_string(), "c1");
      return c;
    }
    const rrc_resume_request1_s& rrc_resume_request1() const
    {
      assert_choice_type("rrcResumeRequest1", type_.to_string(), "c1");
      return c;
    }
    rrc_resume_request1_s& set_rrc_resume_request1()
    {
      set(types::rrc_resume_request1);
      return c;
    }

  private:
    types                 type_;
    rrc_resume_request1_s c;
  };
  struct types_opts {
    enum options { c1, msg_class_ext, nulltype } value;
    typedef uint8_t number_type;

    std::string to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<types_opts> types;

  // choice methods
  ul_ccch1_msg_type_c() = default;
  ul_ccch1_msg_type_c(const ul_ccch1_msg_type_c& other);
  ul_ccch1_msg_type_c& operator=(const ul_ccch1_msg_type_c& other);
  ~ul_ccch1_msg_type_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  c1_c_& c1()
  {
    assert_choice_type("c1", type_.to_string(), "UL-CCCH1-MessageType");
    return c.get<c1_c_>();
  }
  const c1_c_& c1() const
  {
    assert_choice_type("c1", type_.to_string(), "UL-CCCH1-MessageType");
    return c.get<c1_c_>();
  }
  c1_c_& set_c1()
  {
    set(types::c1);
    return c.get<c1_c_>();
  }

private:
  types                  type_;
  choice_buffer_t<c1_c_> c;

  void destroy_();
};

// UL-CCCH1-Message ::= SEQUENCE
struct ul_ccch1_msg_s {
  ul_ccch1_msg_type_c msg;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasQuantityResults ::= SEQUENCE
struct meas_quant_results_s {
  bool    rsrp_present = false;
  bool    rsrq_present = false;
  bool    sinr_present = false;
  uint8_t rsrp         = 0;
  uint8_t rsrq         = 0;
  uint8_t sinr         = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PLMN-Identity-EUTRA-5GC ::= CHOICE
struct plmn_id_eutra_minus5_gc_c {
  struct types_opts {
    enum options { plmn_id_eutra_minus5_gc, plmn_idx, nulltype } value;
    typedef int8_t number_type;

    std::string to_string() const;
    int8_t      to_number() const;
  };
  typedef enumerated<types_opts> types;

  // choice methods
  plmn_id_eutra_minus5_gc_c() = default;
  plmn_id_eutra_minus5_gc_c(const plmn_id_eutra_minus5_gc_c& other);
  plmn_id_eutra_minus5_gc_c& operator=(const plmn_id_eutra_minus5_gc_c& other);
  ~plmn_id_eutra_minus5_gc_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  plmn_id_s& plmn_id_eutra_minus5_gc()
  {
    assert_choice_type("plmn-Identity-EUTRA-5GC", type_.to_string(), "PLMN-Identity-EUTRA-5GC");
    return c.get<plmn_id_s>();
  }
  uint8_t& plmn_idx()
  {
    assert_choice_type("plmn-index", type_.to_string(), "PLMN-Identity-EUTRA-5GC");
    return c.get<uint8_t>();
  }
  const plmn_id_s& plmn_id_eutra_minus5_gc() const
  {
    assert_choice_type("plmn-Identity-EUTRA-5GC", type_.to_string(), "PLMN-Identity-EUTRA-5GC");
    return c.get<plmn_id_s>();
  }
  const uint8_t& plmn_idx() const
  {
    assert_choice_type("plmn-index", type_.to_string(), "PLMN-Identity-EUTRA-5GC");
    return c.get<uint8_t>();
  }
  plmn_id_s& set_plmn_id_eutra_minus5_gc()
  {
    set(types::plmn_id_eutra_minus5_gc);
    return c.get<plmn_id_s>();
  }
  uint8_t& set_plmn_idx()
  {
    set(types::plmn_idx);
    return c.get<uint8_t>();
  }

private:
  types                      type_;
  choice_buffer_t<plmn_id_s> c;

  void destroy_();
};

// ResultsPerCSI-RS-Index ::= SEQUENCE
struct results_per_csi_rs_idx_s {
  bool                 csi_rs_results_present = false;
  uint8_t              csi_rs_idx             = 0;
  meas_quant_results_s csi_rs_results;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ResultsPerSSB-Index ::= SEQUENCE
struct results_per_ssb_idx_s {
  bool                 ssb_results_present = false;
  uint8_t              ssb_idx             = 0;
  meas_quant_results_s ssb_results;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CGI-Info ::= SEQUENCE
struct cgi_info_s {
  struct no_sib1_s_ {
    uint8_t          ssb_subcarrier_offset = 0;
    pdcch_cfg_sib1_s pdcch_cfg_sib1;
  };

  // member variables
  bool                      ext                       = false;
  bool                      plmn_id_info_list_present = false;
  bool                      freq_band_list_present    = false;
  bool                      no_sib1_present           = false;
  plmn_id_info_list_l       plmn_id_info_list;
  multi_freq_band_list_nr_l freq_band_list;
  no_sib1_s_                no_sib1;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CellIdentity-EUTRA-5GC ::= CHOICE
struct cell_id_eutra_minus5_gc_c {
  struct types_opts {
    enum options { cell_id_eutra, cell_id_idx, nulltype } value;

    std::string to_string() const;
  };
  typedef enumerated<types_opts> types;

  // choice methods
  cell_id_eutra_minus5_gc_c() = default;
  cell_id_eutra_minus5_gc_c(const cell_id_eutra_minus5_gc_c& other);
  cell_id_eutra_minus5_gc_c& operator=(const cell_id_eutra_minus5_gc_c& other);
  ~cell_id_eutra_minus5_gc_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  fixed_bitstring<28>& cell_id_eutra()
  {
    assert_choice_type("cellIdentity-EUTRA", type_.to_string(), "CellIdentity-EUTRA-5GC");
    return c.get<fixed_bitstring<28> >();
  }
  uint8_t& cell_id_idx()
  {
    assert_choice_type("cellId-index", type_.to_string(), "CellIdentity-EUTRA-5GC");
    return c.get<uint8_t>();
  }
  const fixed_bitstring<28>& cell_id_eutra() const
  {
    assert_choice_type("cellIdentity-EUTRA", type_.to_string(), "CellIdentity-EUTRA-5GC");
    return c.get<fixed_bitstring<28> >();
  }
  const uint8_t& cell_id_idx() const
  {
    assert_choice_type("cellId-index", type_.to_string(), "CellIdentity-EUTRA-5GC");
    return c.get<uint8_t>();
  }
  fixed_bitstring<28>& set_cell_id_eutra()
  {
    set(types::cell_id_eutra);
    return c.get<fixed_bitstring<28> >();
  }
  uint8_t& set_cell_id_idx()
  {
    set(types::cell_id_idx);
    return c.get<uint8_t>();
  }

private:
  types                                 type_;
  choice_buffer_t<fixed_bitstring<28> > c;

  void destroy_();
};

// PLMN-IdentityList-EUTRA-5GC ::= SEQUENCE (SIZE (1..12)) OF PLMN-Identity-EUTRA-5GC
using plmn_id_list_eutra_minus5_gc_l = dyn_array<plmn_id_eutra_minus5_gc_c>;

// PLMN-IdentityList-EUTRA-EPC ::= SEQUENCE (SIZE (1..12)) OF PLMN-Identity
using plmn_id_list_eutra_epc_l = dyn_array<plmn_id_s>;

// ResultsPerCSI-RS-IndexList ::= SEQUENCE (SIZE (1..64)) OF ResultsPerCSI-RS-Index
using results_per_csi_rs_idx_list_l = dyn_array<results_per_csi_rs_idx_s>;

// ResultsPerSSB-IndexList ::= SEQUENCE (SIZE (1..64)) OF ResultsPerSSB-Index
using results_per_ssb_idx_list_l = dyn_array<results_per_ssb_idx_s>;

// CellAccessRelatedInfo-EUTRA-5GC ::= SEQUENCE
struct cell_access_related_info_eutra_minus5_gc_s {
  bool                           ranac_minus5gc_present = false;
  plmn_id_list_eutra_minus5_gc_l plmn_id_list_eutra_minus5gc;
  fixed_bitstring<24>            tac_eutra_minus5gc;
  uint16_t                       ranac_minus5gc = 0;
  cell_id_eutra_minus5_gc_c      cell_id_eutra_minus5gc;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CellAccessRelatedInfo-EUTRA-EPC ::= SEQUENCE
struct cell_access_related_info_eutra_epc_s {
  plmn_id_list_eutra_epc_l plmn_id_list_eutra_epc;
  fixed_bitstring<16>      tac_eutra_epc;
  fixed_bitstring<28>      cell_id_eutra_epc;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasQuantityResultsEUTRA ::= SEQUENCE
struct meas_quant_results_eutra_s {
  bool    rsrp_present = false;
  bool    rsrq_present = false;
  bool    sinr_present = false;
  uint8_t rsrp         = 0;
  uint8_t rsrq         = 0;
  uint8_t sinr         = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasResultNR ::= SEQUENCE
struct meas_result_nr_s {
  struct meas_result_s_ {
    struct cell_results_s_ {
      bool                 results_ssb_cell_present    = false;
      bool                 results_csi_rs_cell_present = false;
      meas_quant_results_s results_ssb_cell;
      meas_quant_results_s results_csi_rs_cell;
    };
    struct rs_idx_results_s_ {
      bool                          results_ssb_idxes_present    = false;
      bool                          results_csi_rs_idxes_present = false;
      results_per_ssb_idx_list_l    results_ssb_idxes;
      results_per_csi_rs_idx_list_l results_csi_rs_idxes;
    };

    // member variables
    bool              rs_idx_results_present = false;
    cell_results_s_   cell_results;
    rs_idx_results_s_ rs_idx_results;
  };

  // member variables
  bool           ext         = false;
  bool           pci_present = false;
  uint16_t       pci         = 0;
  meas_result_s_ meas_result;
  // ...
  // group 0
  copy_ptr<cgi_info_s> cgi_info;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MultiBandInfoListEUTRA ::= SEQUENCE (SIZE (1..8)) OF INTEGER (1..256)
using multi_band_info_list_eutra_l = bounded_array<uint16_t, 8>;

// UplinkTxDirectCurrentBWP ::= SEQUENCE
struct ul_tx_direct_current_bwp_s {
  uint8_t  bwp_id                     = 0;
  bool     shift7dot5k_hz             = false;
  uint16_t tx_direct_current_location = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// EUTRA-RSTD-Info ::= SEQUENCE
struct eutra_rstd_info_s {
  bool     ext             = false;
  uint32_t carrier_freq    = 0;
  uint8_t  meas_prs_offset = 0;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MIMO-LayersDL ::= ENUMERATED
struct mimo_layers_dl_opts {
  enum options { two_layers, four_layers, eight_layers, nulltype } value;
  typedef uint8_t number_type;

  std::string to_string() const;
  uint8_t     to_number() const;
};
typedef enumerated<mimo_layers_dl_opts> mimo_layers_dl_e;

// MIMO-LayersUL ::= ENUMERATED
struct mimo_layers_ul_opts {
  enum options { one_layer, two_layers, four_layers, nulltype } value;
  typedef uint8_t number_type;

  std::string to_string() const;
  uint8_t     to_number() const;
};
typedef enumerated<mimo_layers_ul_opts> mimo_layers_ul_e;

// MeasResultEUTRA ::= SEQUENCE
struct meas_result_eutra_s {
  struct cgi_info_s_ {
    struct cgi_info_epc_s_ {
      using cgi_info_epc_list_l_ = dyn_array<cell_access_related_info_eutra_epc_s>;

      // member variables
      bool                                 cgi_info_epc_list_present = false;
      cell_access_related_info_eutra_epc_s cgi_info_epc_legacy;
      cgi_info_epc_list_l_                 cgi_info_epc_list;
    };
    using cgi_info_minus5_gc_l_ = dyn_array<cell_access_related_info_eutra_minus5_gc_s>;

    // member variables
    bool                         cgi_info_epc_present         = false;
    bool                         cgi_info_minus5_gc_present   = false;
    bool                         multi_band_info_list_present = false;
    bool                         freq_band_ind_prio_present   = false;
    cgi_info_epc_s_              cgi_info_epc;
    cgi_info_minus5_gc_l_        cgi_info_minus5_gc;
    uint16_t                     freq_band_ind = 1;
    multi_band_info_list_eutra_l multi_band_info_list;
  };

  // member variables
  bool                       ext              = false;
  bool                       cgi_info_present = false;
  uint16_t                   eutra_pci        = 0;
  meas_quant_results_eutra_s meas_result;
  cgi_info_s_                cgi_info;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasResultServMO ::= SEQUENCE
struct meas_result_serv_mo_s {
  bool             ext                                 = false;
  bool             meas_result_best_neigh_cell_present = false;
  uint8_t          serv_cell_id                        = 0;
  meas_result_nr_s meas_result_serving_cell;
  meas_result_nr_s meas_result_best_neigh_cell;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ReducedAggregatedBandwidth ::= ENUMERATED
struct reduced_aggregated_bw_opts {
  enum options {
    mhz0,
    mhz10,
    mhz20,
    mhz30,
    mhz40,
    mhz50,
    mhz60,
    mhz80,
    mhz100,
    mhz200,
    mhz300,
    mhz400,
    nulltype
  } value;
  typedef uint16_t number_type;

  std::string to_string() const;
  uint16_t    to_number() const;
};
typedef enumerated<reduced_aggregated_bw_opts> reduced_aggregated_bw_e;

// UplinkTxDirectCurrentCell ::= SEQUENCE
struct ul_tx_direct_current_cell_s {
  using ul_direct_current_bwp_l_ = dyn_array<ul_tx_direct_current_bwp_s>;

  // member variables
  bool                     ext           = false;
  uint8_t                  serv_cell_idx = 0;
  ul_direct_current_bwp_l_ ul_direct_current_bwp;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DRB-CountInfo ::= SEQUENCE
struct drb_count_info_s {
  uint8_t  drb_id   = 1;
  uint64_t count_ul = 0;
  uint64_t count_dl = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// EUTRA-RSTD-InfoList ::= SEQUENCE (SIZE (1..3)) OF EUTRA-RSTD-Info
using eutra_rstd_info_list_l = dyn_array<eutra_rstd_info_s>;

// MeasResultListEUTRA ::= SEQUENCE (SIZE (1..8)) OF MeasResultEUTRA
using meas_result_list_eutra_l = dyn_array<meas_result_eutra_s>;

// MeasResultListNR ::= SEQUENCE (SIZE (1..8)) OF MeasResultNR
using meas_result_list_nr_l = dyn_array<meas_result_nr_s>;

// MeasResultServMOList ::= SEQUENCE (SIZE (1..32)) OF MeasResultServMO
using meas_result_serv_mo_list_l = dyn_array<meas_result_serv_mo_s>;

// OverheatingAssistance ::= SEQUENCE
struct overheat_assist_s {
  struct reduced_max_ccs_s_ {
    uint8_t reduced_ccs_dl = 0;
    uint8_t reduced_ccs_ul = 0;
  };
  struct reduced_max_bw_fr1_s_ {
    reduced_aggregated_bw_e reduced_bw_fr1_dl;
    reduced_aggregated_bw_e reduced_bw_fr1_ul;
  };
  struct reduced_max_bw_fr2_s_ {
    reduced_aggregated_bw_e reduced_bw_fr2_dl;
    reduced_aggregated_bw_e reduced_bw_fr2_ul;
  };
  struct reduced_max_mimo_layers_fr1_s_ {
    mimo_layers_dl_e reduced_mimo_layers_fr1_dl;
    mimo_layers_ul_e reduced_mimo_layers_fr1_ul;
  };
  struct reduced_max_mimo_layers_fr2_s_ {
    mimo_layers_dl_e reduced_mimo_layers_fr2_dl;
    mimo_layers_ul_e reduced_mimo_layers_fr2_ul;
  };

  // member variables
  bool                           reduced_max_ccs_present             = false;
  bool                           reduced_max_bw_fr1_present          = false;
  bool                           reduced_max_bw_fr2_present          = false;
  bool                           reduced_max_mimo_layers_fr1_present = false;
  bool                           reduced_max_mimo_layers_fr2_present = false;
  reduced_max_ccs_s_             reduced_max_ccs;
  reduced_max_bw_fr1_s_          reduced_max_bw_fr1;
  reduced_max_bw_fr2_s_          reduced_max_bw_fr2;
  reduced_max_mimo_layers_fr1_s_ reduced_max_mimo_layers_fr1;
  reduced_max_mimo_layers_fr2_s_ reduced_max_mimo_layers_fr2;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UE-CapabilityRAT-Container ::= SEQUENCE
struct ue_cap_rat_container_s {
  rat_type_e    rat_type;
  dyn_octstring ue_cap_rat_container;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UplinkTxDirectCurrentList ::= SEQUENCE (SIZE (1..32)) OF UplinkTxDirectCurrentCell
using ul_tx_direct_current_list_l = dyn_array<ul_tx_direct_current_cell_s>;

// DRB-CountInfoList ::= SEQUENCE (SIZE (0..29)) OF DRB-CountInfo
using drb_count_info_list_l = dyn_array<drb_count_info_s>;

// DelayBudgetReport ::= CHOICE
struct delay_budget_report_c {
  struct type1_opts {
    enum options {
      ms_minus1280,
      ms_minus640,
      ms_minus320,
      ms_minus160,
      ms_minus80,
      ms_minus60,
      ms_minus40,
      ms_minus20,
      ms0,
      ms20,
      ms40,
      ms60,
      ms80,
      ms160,
      ms320,
      ms640,
      ms1280,
      nulltype
    } value;
    typedef int16_t number_type;

    std::string to_string() const;
    int16_t     to_number() const;
  };
  typedef enumerated<type1_opts> type1_e_;
  struct types_opts {
    enum options { type1, /*...*/ nulltype } value;
    typedef uint8_t number_type;

    std::string to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<types_opts, true> types;

  // choice methods
  types       type() const { return types::type1; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  type1_e_&       type1() { return c; }
  const type1_e_& type1() const { return c; }

private:
  type1_e_ c;
};

// FailureInfoRLC-Bearer ::= SEQUENCE
struct fail_info_rlc_bearer_s {
  struct fail_type_opts {
    enum options { dupl, spare3, spare2, spare1, nulltype } value;

    std::string to_string() const;
  };
  typedef enumerated<fail_type_opts> fail_type_e_;

  // member variables
  uint8_t      cell_group_id = 0;
  uint8_t      lc_ch_id      = 1;
  fail_type_e_ fail_type;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// LocationMeasurementInfo ::= CHOICE
struct location_meas_info_c {
  struct types_opts {
    enum options { eutra_rstd, /*...*/ eutra_fine_timing_detection, nulltype } value;

    std::string to_string() const;
  };
  typedef enumerated<types_opts, true, 1> types;

  // choice methods
  location_meas_info_c() = default;
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  eutra_rstd_info_list_l& eutra_rstd()
  {
    assert_choice_type("eutra-RSTD", type_.to_string(), "LocationMeasurementInfo");
    return c;
  }
  const eutra_rstd_info_list_l& eutra_rstd() const
  {
    assert_choice_type("eutra-RSTD", type_.to_string(), "LocationMeasurementInfo");
    return c;
  }
  eutra_rstd_info_list_l& set_eutra_rstd()
  {
    set(types::eutra_rstd);
    return c;
  }

private:
  types                  type_;
  eutra_rstd_info_list_l c;
};

// MeasResults ::= SEQUENCE
struct meas_results_s {
  struct meas_result_neigh_cells_c_ {
    struct types_opts {
      enum options { meas_result_list_nr, /*...*/ meas_result_list_eutra, nulltype } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts, true, 1> types;

    // choice methods
    meas_result_neigh_cells_c_() = default;
    meas_result_neigh_cells_c_(const meas_result_neigh_cells_c_& other);
    meas_result_neigh_cells_c_& operator=(const meas_result_neigh_cells_c_& other);
    ~meas_result_neigh_cells_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    meas_result_list_nr_l& meas_result_list_nr()
    {
      assert_choice_type("measResultListNR", type_.to_string(), "measResultNeighCells");
      return c.get<meas_result_list_nr_l>();
    }
    meas_result_list_eutra_l& meas_result_list_eutra()
    {
      assert_choice_type("measResultListEUTRA", type_.to_string(), "measResultNeighCells");
      return c.get<meas_result_list_eutra_l>();
    }
    const meas_result_list_nr_l& meas_result_list_nr() const
    {
      assert_choice_type("measResultListNR", type_.to_string(), "measResultNeighCells");
      return c.get<meas_result_list_nr_l>();
    }
    const meas_result_list_eutra_l& meas_result_list_eutra() const
    {
      assert_choice_type("measResultListEUTRA", type_.to_string(), "measResultNeighCells");
      return c.get<meas_result_list_eutra_l>();
    }
    meas_result_list_nr_l& set_meas_result_list_nr()
    {
      set(types::meas_result_list_nr);
      return c.get<meas_result_list_nr_l>();
    }
    meas_result_list_eutra_l& set_meas_result_list_eutra()
    {
      set(types::meas_result_list_eutra);
      return c.get<meas_result_list_eutra_l>();
    }

  private:
    types                                                            type_;
    choice_buffer_t<meas_result_list_eutra_l, meas_result_list_nr_l> c;

    void destroy_();
  };

  // member variables
  bool                       ext                             = false;
  bool                       meas_result_neigh_cells_present = false;
  uint8_t                    meas_id                         = 1;
  meas_result_serv_mo_list_l meas_result_serving_mo_list;
  meas_result_neigh_cells_c_ meas_result_neigh_cells;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCReconfigurationComplete-v1530-IEs ::= SEQUENCE
struct rrc_recfg_complete_v1530_ies_s {
  bool                        ul_tx_direct_current_list_present = false;
  bool                        non_crit_ext_present              = false;
  ul_tx_direct_current_list_l ul_tx_direct_current_list;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RegisteredAMF ::= SEQUENCE
struct registered_amf_s {
  bool                plmn_id_present = false;
  plmn_id_s           plmn_id;
  fixed_bitstring<24> amf_id;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// S-NSSAI ::= CHOICE
struct s_nssai_c {
  struct types_opts {
    enum options { sst, sst_sd, nulltype } value;

    std::string to_string() const;
  };
  typedef enumerated<types_opts> types;

  // choice methods
  s_nssai_c() = default;
  s_nssai_c(const s_nssai_c& other);
  s_nssai_c& operator=(const s_nssai_c& other);
  ~s_nssai_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  fixed_bitstring<8>& sst()
  {
    assert_choice_type("sst", type_.to_string(), "S-NSSAI");
    return c.get<fixed_bitstring<8> >();
  }
  fixed_bitstring<32>& sst_sd()
  {
    assert_choice_type("sst-SD", type_.to_string(), "S-NSSAI");
    return c.get<fixed_bitstring<32> >();
  }
  const fixed_bitstring<8>& sst() const
  {
    assert_choice_type("sst", type_.to_string(), "S-NSSAI");
    return c.get<fixed_bitstring<8> >();
  }
  const fixed_bitstring<32>& sst_sd() const
  {
    assert_choice_type("sst-SD", type_.to_string(), "S-NSSAI");
    return c.get<fixed_bitstring<32> >();
  }
  fixed_bitstring<8>& set_sst()
  {
    set(types::sst);
    return c.get<fixed_bitstring<8> >();
  }
  fixed_bitstring<32>& set_sst_sd()
  {
    set(types::sst_sd);
    return c.get<fixed_bitstring<32> >();
  }

private:
  types                                 type_;
  choice_buffer_t<fixed_bitstring<32> > c;

  void destroy_();
};

// UE-CapabilityRAT-ContainerList ::= SEQUENCE (SIZE (0..8)) OF UE-CapabilityRAT-Container
using ue_cap_rat_container_list_l = dyn_array<ue_cap_rat_container_s>;

// UEAssistanceInformation-v1540-IEs ::= SEQUENCE
struct ueassist_info_v1540_ies_s {
  bool              overheat_assist_present = false;
  bool              non_crit_ext_present    = false;
  overheat_assist_s overheat_assist;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CounterCheckResponse-IEs ::= SEQUENCE
struct counter_check_resp_ies_s {
  bool                  late_non_crit_ext_present = false;
  bool                  non_crit_ext_present      = false;
  drb_count_info_list_l drb_count_info_list;
  dyn_octstring         late_non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// FailureInformation-IEs ::= SEQUENCE
struct fail_info_ies_s {
  bool                   fail_info_rlc_bearer_present = false;
  bool                   late_non_crit_ext_present    = false;
  bool                   non_crit_ext_present         = false;
  fail_info_rlc_bearer_s fail_info_rlc_bearer;
  dyn_octstring          late_non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// LocationMeasurementIndication-IEs ::= SEQUENCE
struct location_meas_ind_ies_s {
  bool                                  late_non_crit_ext_present = false;
  bool                                  non_crit_ext_present      = false;
  setup_release_c<location_meas_info_c> meas_ind;
  dyn_octstring                         late_non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasurementReport-IEs ::= SEQUENCE
struct meas_report_ies_s {
  bool           late_non_crit_ext_present = false;
  bool           non_crit_ext_present      = false;
  meas_results_s meas_results;
  dyn_octstring  late_non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCReconfigurationComplete-IEs ::= SEQUENCE
struct rrc_recfg_complete_ies_s {
  bool                           late_non_crit_ext_present = false;
  bool                           non_crit_ext_present      = false;
  dyn_octstring                  late_non_crit_ext;
  rrc_recfg_complete_v1530_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCReestablishmentComplete-IEs ::= SEQUENCE
struct rrc_reest_complete_ies_s {
  bool          late_non_crit_ext_present = false;
  bool          non_crit_ext_present      = false;
  dyn_octstring late_non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCResumeComplete-IEs ::= SEQUENCE
struct rrc_resume_complete_ies_s {
  bool                        ded_nas_msg_present               = false;
  bool                        sel_plmn_id_present               = false;
  bool                        ul_tx_direct_current_list_present = false;
  bool                        late_non_crit_ext_present         = false;
  bool                        non_crit_ext_present              = false;
  dyn_octstring               ded_nas_msg;
  uint8_t                     sel_plmn_id = 1;
  ul_tx_direct_current_list_l ul_tx_direct_current_list;
  dyn_octstring               late_non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCSetupComplete-IEs ::= SEQUENCE
struct rrc_setup_complete_ies_s {
  struct guami_type_opts {
    enum options { native, mapped, nulltype } value;

    std::string to_string() const;
  };
  typedef enumerated<guami_type_opts> guami_type_e_;
  using s_nssai_list_l_ = dyn_array<s_nssai_c>;
  struct ng_minus5_g_s_tmsi_value_c_ {
    struct types_opts {
      enum options { ng_minus5_g_s_tmsi, ng_minus5_g_s_tmsi_part2, nulltype } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    ng_minus5_g_s_tmsi_value_c_() = default;
    ng_minus5_g_s_tmsi_value_c_(const ng_minus5_g_s_tmsi_value_c_& other);
    ng_minus5_g_s_tmsi_value_c_& operator=(const ng_minus5_g_s_tmsi_value_c_& other);
    ~ng_minus5_g_s_tmsi_value_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    fixed_bitstring<48>& ng_minus5_g_s_tmsi()
    {
      assert_choice_type("ng-5G-S-TMSI", type_.to_string(), "ng-5G-S-TMSI-Value");
      return c.get<fixed_bitstring<48> >();
    }
    fixed_bitstring<9>& ng_minus5_g_s_tmsi_part2()
    {
      assert_choice_type("ng-5G-S-TMSI-Part2", type_.to_string(), "ng-5G-S-TMSI-Value");
      return c.get<fixed_bitstring<9> >();
    }
    const fixed_bitstring<48>& ng_minus5_g_s_tmsi() const
    {
      assert_choice_type("ng-5G-S-TMSI", type_.to_string(), "ng-5G-S-TMSI-Value");
      return c.get<fixed_bitstring<48> >();
    }
    const fixed_bitstring<9>& ng_minus5_g_s_tmsi_part2() const
    {
      assert_choice_type("ng-5G-S-TMSI-Part2", type_.to_string(), "ng-5G-S-TMSI-Value");
      return c.get<fixed_bitstring<9> >();
    }
    fixed_bitstring<48>& set_ng_minus5_g_s_tmsi()
    {
      set(types::ng_minus5_g_s_tmsi);
      return c.get<fixed_bitstring<48> >();
    }
    fixed_bitstring<9>& set_ng_minus5_g_s_tmsi_part2()
    {
      set(types::ng_minus5_g_s_tmsi_part2);
      return c.get<fixed_bitstring<9> >();
    }

  private:
    types                                 type_;
    choice_buffer_t<fixed_bitstring<48> > c;

    void destroy_();
  };

  // member variables
  bool                        registered_amf_present           = false;
  bool                        guami_type_present               = false;
  bool                        s_nssai_list_present             = false;
  bool                        ng_minus5_g_s_tmsi_value_present = false;
  bool                        late_non_crit_ext_present        = false;
  bool                        non_crit_ext_present             = false;
  uint8_t                     sel_plmn_id                      = 1;
  registered_amf_s            registered_amf;
  guami_type_e_               guami_type;
  s_nssai_list_l_             s_nssai_list;
  dyn_octstring               ded_nas_msg;
  ng_minus5_g_s_tmsi_value_c_ ng_minus5_g_s_tmsi_value;
  dyn_octstring               late_non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SecurityModeComplete-IEs ::= SEQUENCE
struct security_mode_complete_ies_s {
  bool          late_non_crit_ext_present = false;
  bool          non_crit_ext_present      = false;
  dyn_octstring late_non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SecurityModeFailure-IEs ::= SEQUENCE
struct security_mode_fail_ies_s {
  bool          late_non_crit_ext_present = false;
  bool          non_crit_ext_present      = false;
  dyn_octstring late_non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UEAssistanceInformation-IEs ::= SEQUENCE
struct ueassist_info_ies_s {
  bool                      delay_budget_report_present = false;
  bool                      late_non_crit_ext_present   = false;
  bool                      non_crit_ext_present        = false;
  delay_budget_report_c     delay_budget_report;
  dyn_octstring             late_non_crit_ext;
  ueassist_info_v1540_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UECapabilityInformation-IEs ::= SEQUENCE
struct ue_cap_info_ies_s {
  bool                        ue_cap_rat_container_list_present = false;
  bool                        late_non_crit_ext_present         = false;
  bool                        non_crit_ext_present              = false;
  ue_cap_rat_container_list_l ue_cap_rat_container_list;
  dyn_octstring               late_non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ULInformationTransfer-IEs ::= SEQUENCE
struct ul_info_transfer_ies_s {
  bool          ded_nas_msg_present       = false;
  bool          late_non_crit_ext_present = false;
  bool          non_crit_ext_present      = false;
  dyn_octstring ded_nas_msg;
  dyn_octstring late_non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CounterCheckResponse ::= SEQUENCE
struct counter_check_resp_s {
  struct crit_exts_c_ {
    struct types_opts {
      enum options { counter_check_resp, crit_exts_future, nulltype } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    crit_exts_c_() = default;
    crit_exts_c_(const crit_exts_c_& other);
    crit_exts_c_& operator=(const crit_exts_c_& other);
    ~crit_exts_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    counter_check_resp_ies_s& counter_check_resp()
    {
      assert_choice_type("counterCheckResponse", type_.to_string(), "criticalExtensions");
      return c.get<counter_check_resp_ies_s>();
    }
    const counter_check_resp_ies_s& counter_check_resp() const
    {
      assert_choice_type("counterCheckResponse", type_.to_string(), "criticalExtensions");
      return c.get<counter_check_resp_ies_s>();
    }
    counter_check_resp_ies_s& set_counter_check_resp()
    {
      set(types::counter_check_resp);
      return c.get<counter_check_resp_ies_s>();
    }

  private:
    types                                     type_;
    choice_buffer_t<counter_check_resp_ies_s> c;

    void destroy_();
  };

  // member variables
  uint8_t      rrc_transaction_id = 0;
  crit_exts_c_ crit_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// FailureInformation ::= SEQUENCE
struct fail_info_s {
  struct crit_exts_c_ {
    struct types_opts {
      enum options { fail_info, crit_exts_future, nulltype } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    crit_exts_c_() = default;
    crit_exts_c_(const crit_exts_c_& other);
    crit_exts_c_& operator=(const crit_exts_c_& other);
    ~crit_exts_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    fail_info_ies_s& fail_info()
    {
      assert_choice_type("failureInformation", type_.to_string(), "criticalExtensions");
      return c.get<fail_info_ies_s>();
    }
    const fail_info_ies_s& fail_info() const
    {
      assert_choice_type("failureInformation", type_.to_string(), "criticalExtensions");
      return c.get<fail_info_ies_s>();
    }
    fail_info_ies_s& set_fail_info()
    {
      set(types::fail_info);
      return c.get<fail_info_ies_s>();
    }

  private:
    types                            type_;
    choice_buffer_t<fail_info_ies_s> c;

    void destroy_();
  };

  // member variables
  crit_exts_c_ crit_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// LocationMeasurementIndication ::= SEQUENCE
struct location_meas_ind_s {
  struct crit_exts_c_ {
    struct types_opts {
      enum options { location_meas_ind, crit_exts_future, nulltype } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    crit_exts_c_() = default;
    crit_exts_c_(const crit_exts_c_& other);
    crit_exts_c_& operator=(const crit_exts_c_& other);
    ~crit_exts_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    location_meas_ind_ies_s& location_meas_ind()
    {
      assert_choice_type("locationMeasurementIndication", type_.to_string(), "criticalExtensions");
      return c.get<location_meas_ind_ies_s>();
    }
    const location_meas_ind_ies_s& location_meas_ind() const
    {
      assert_choice_type("locationMeasurementIndication", type_.to_string(), "criticalExtensions");
      return c.get<location_meas_ind_ies_s>();
    }
    location_meas_ind_ies_s& set_location_meas_ind()
    {
      set(types::location_meas_ind);
      return c.get<location_meas_ind_ies_s>();
    }

  private:
    types                                    type_;
    choice_buffer_t<location_meas_ind_ies_s> c;

    void destroy_();
  };

  // member variables
  crit_exts_c_ crit_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasurementReport ::= SEQUENCE
struct meas_report_s {
  struct crit_exts_c_ {
    struct types_opts {
      enum options { meas_report, crit_exts_future, nulltype } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    crit_exts_c_() = default;
    crit_exts_c_(const crit_exts_c_& other);
    crit_exts_c_& operator=(const crit_exts_c_& other);
    ~crit_exts_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    meas_report_ies_s& meas_report()
    {
      assert_choice_type("measurementReport", type_.to_string(), "criticalExtensions");
      return c.get<meas_report_ies_s>();
    }
    const meas_report_ies_s& meas_report() const
    {
      assert_choice_type("measurementReport", type_.to_string(), "criticalExtensions");
      return c.get<meas_report_ies_s>();
    }
    meas_report_ies_s& set_meas_report()
    {
      set(types::meas_report);
      return c.get<meas_report_ies_s>();
    }

  private:
    types                              type_;
    choice_buffer_t<meas_report_ies_s> c;

    void destroy_();
  };

  // member variables
  crit_exts_c_ crit_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCReconfigurationComplete ::= SEQUENCE
struct rrc_recfg_complete_s {
  struct crit_exts_c_ {
    struct types_opts {
      enum options { rrc_recfg_complete, crit_exts_future, nulltype } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    crit_exts_c_() = default;
    crit_exts_c_(const crit_exts_c_& other);
    crit_exts_c_& operator=(const crit_exts_c_& other);
    ~crit_exts_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    rrc_recfg_complete_ies_s& rrc_recfg_complete()
    {
      assert_choice_type("rrcReconfigurationComplete", type_.to_string(), "criticalExtensions");
      return c.get<rrc_recfg_complete_ies_s>();
    }
    const rrc_recfg_complete_ies_s& rrc_recfg_complete() const
    {
      assert_choice_type("rrcReconfigurationComplete", type_.to_string(), "criticalExtensions");
      return c.get<rrc_recfg_complete_ies_s>();
    }
    rrc_recfg_complete_ies_s& set_rrc_recfg_complete()
    {
      set(types::rrc_recfg_complete);
      return c.get<rrc_recfg_complete_ies_s>();
    }

  private:
    types                                     type_;
    choice_buffer_t<rrc_recfg_complete_ies_s> c;

    void destroy_();
  };

  // member variables
  uint8_t      rrc_transaction_id = 0;
  crit_exts_c_ crit_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCReestablishmentComplete ::= SEQUENCE
struct rrc_reest_complete_s {
  struct crit_exts_c_ {
    struct types_opts {
      enum options { rrc_reest_complete, crit_exts_future, nulltype } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    crit_exts_c_() = default;
    crit_exts_c_(const crit_exts_c_& other);
    crit_exts_c_& operator=(const crit_exts_c_& other);
    ~crit_exts_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    rrc_reest_complete_ies_s& rrc_reest_complete()
    {
      assert_choice_type("rrcReestablishmentComplete", type_.to_string(), "criticalExtensions");
      return c.get<rrc_reest_complete_ies_s>();
    }
    const rrc_reest_complete_ies_s& rrc_reest_complete() const
    {
      assert_choice_type("rrcReestablishmentComplete", type_.to_string(), "criticalExtensions");
      return c.get<rrc_reest_complete_ies_s>();
    }
    rrc_reest_complete_ies_s& set_rrc_reest_complete()
    {
      set(types::rrc_reest_complete);
      return c.get<rrc_reest_complete_ies_s>();
    }

  private:
    types                                     type_;
    choice_buffer_t<rrc_reest_complete_ies_s> c;

    void destroy_();
  };

  // member variables
  uint8_t      rrc_transaction_id = 0;
  crit_exts_c_ crit_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCResumeComplete ::= SEQUENCE
struct rrc_resume_complete_s {
  struct crit_exts_c_ {
    struct types_opts {
      enum options { rrc_resume_complete, crit_exts_future, nulltype } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    crit_exts_c_() = default;
    crit_exts_c_(const crit_exts_c_& other);
    crit_exts_c_& operator=(const crit_exts_c_& other);
    ~crit_exts_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    rrc_resume_complete_ies_s& rrc_resume_complete()
    {
      assert_choice_type("rrcResumeComplete", type_.to_string(), "criticalExtensions");
      return c.get<rrc_resume_complete_ies_s>();
    }
    const rrc_resume_complete_ies_s& rrc_resume_complete() const
    {
      assert_choice_type("rrcResumeComplete", type_.to_string(), "criticalExtensions");
      return c.get<rrc_resume_complete_ies_s>();
    }
    rrc_resume_complete_ies_s& set_rrc_resume_complete()
    {
      set(types::rrc_resume_complete);
      return c.get<rrc_resume_complete_ies_s>();
    }

  private:
    types                                      type_;
    choice_buffer_t<rrc_resume_complete_ies_s> c;

    void destroy_();
  };

  // member variables
  uint8_t      rrc_transaction_id = 0;
  crit_exts_c_ crit_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCSetupComplete ::= SEQUENCE
struct rrc_setup_complete_s {
  struct crit_exts_c_ {
    struct types_opts {
      enum options { rrc_setup_complete, crit_exts_future, nulltype } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    crit_exts_c_() = default;
    crit_exts_c_(const crit_exts_c_& other);
    crit_exts_c_& operator=(const crit_exts_c_& other);
    ~crit_exts_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    rrc_setup_complete_ies_s& rrc_setup_complete()
    {
      assert_choice_type("rrcSetupComplete", type_.to_string(), "criticalExtensions");
      return c.get<rrc_setup_complete_ies_s>();
    }
    const rrc_setup_complete_ies_s& rrc_setup_complete() const
    {
      assert_choice_type("rrcSetupComplete", type_.to_string(), "criticalExtensions");
      return c.get<rrc_setup_complete_ies_s>();
    }
    rrc_setup_complete_ies_s& set_rrc_setup_complete()
    {
      set(types::rrc_setup_complete);
      return c.get<rrc_setup_complete_ies_s>();
    }

  private:
    types                                     type_;
    choice_buffer_t<rrc_setup_complete_ies_s> c;

    void destroy_();
  };

  // member variables
  uint8_t      rrc_transaction_id = 0;
  crit_exts_c_ crit_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SecurityModeComplete ::= SEQUENCE
struct security_mode_complete_s {
  struct crit_exts_c_ {
    struct types_opts {
      enum options { security_mode_complete, crit_exts_future, nulltype } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    crit_exts_c_() = default;
    crit_exts_c_(const crit_exts_c_& other);
    crit_exts_c_& operator=(const crit_exts_c_& other);
    ~crit_exts_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    security_mode_complete_ies_s& security_mode_complete()
    {
      assert_choice_type("securityModeComplete", type_.to_string(), "criticalExtensions");
      return c.get<security_mode_complete_ies_s>();
    }
    const security_mode_complete_ies_s& security_mode_complete() const
    {
      assert_choice_type("securityModeComplete", type_.to_string(), "criticalExtensions");
      return c.get<security_mode_complete_ies_s>();
    }
    security_mode_complete_ies_s& set_security_mode_complete()
    {
      set(types::security_mode_complete);
      return c.get<security_mode_complete_ies_s>();
    }

  private:
    types                                         type_;
    choice_buffer_t<security_mode_complete_ies_s> c;

    void destroy_();
  };

  // member variables
  uint8_t      rrc_transaction_id = 0;
  crit_exts_c_ crit_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SecurityModeFailure ::= SEQUENCE
struct security_mode_fail_s {
  struct crit_exts_c_ {
    struct types_opts {
      enum options { security_mode_fail, crit_exts_future, nulltype } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    crit_exts_c_() = default;
    crit_exts_c_(const crit_exts_c_& other);
    crit_exts_c_& operator=(const crit_exts_c_& other);
    ~crit_exts_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    security_mode_fail_ies_s& security_mode_fail()
    {
      assert_choice_type("securityModeFailure", type_.to_string(), "criticalExtensions");
      return c.get<security_mode_fail_ies_s>();
    }
    const security_mode_fail_ies_s& security_mode_fail() const
    {
      assert_choice_type("securityModeFailure", type_.to_string(), "criticalExtensions");
      return c.get<security_mode_fail_ies_s>();
    }
    security_mode_fail_ies_s& set_security_mode_fail()
    {
      set(types::security_mode_fail);
      return c.get<security_mode_fail_ies_s>();
    }

  private:
    types                                     type_;
    choice_buffer_t<security_mode_fail_ies_s> c;

    void destroy_();
  };

  // member variables
  uint8_t      rrc_transaction_id = 0;
  crit_exts_c_ crit_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UEAssistanceInformation ::= SEQUENCE
struct ueassist_info_s {
  struct crit_exts_c_ {
    struct types_opts {
      enum options { ue_assist_info, crit_exts_future, nulltype } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    crit_exts_c_() = default;
    crit_exts_c_(const crit_exts_c_& other);
    crit_exts_c_& operator=(const crit_exts_c_& other);
    ~crit_exts_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    ueassist_info_ies_s& ue_assist_info()
    {
      assert_choice_type("ueAssistanceInformation", type_.to_string(), "criticalExtensions");
      return c.get<ueassist_info_ies_s>();
    }
    const ueassist_info_ies_s& ue_assist_info() const
    {
      assert_choice_type("ueAssistanceInformation", type_.to_string(), "criticalExtensions");
      return c.get<ueassist_info_ies_s>();
    }
    ueassist_info_ies_s& set_ue_assist_info()
    {
      set(types::ue_assist_info);
      return c.get<ueassist_info_ies_s>();
    }

  private:
    types                                type_;
    choice_buffer_t<ueassist_info_ies_s> c;

    void destroy_();
  };

  // member variables
  crit_exts_c_ crit_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UECapabilityInformation ::= SEQUENCE
struct ue_cap_info_s {
  struct crit_exts_c_ {
    struct types_opts {
      enum options { ue_cap_info, crit_exts_future, nulltype } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    crit_exts_c_() = default;
    crit_exts_c_(const crit_exts_c_& other);
    crit_exts_c_& operator=(const crit_exts_c_& other);
    ~crit_exts_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    ue_cap_info_ies_s& ue_cap_info()
    {
      assert_choice_type("ueCapabilityInformation", type_.to_string(), "criticalExtensions");
      return c.get<ue_cap_info_ies_s>();
    }
    const ue_cap_info_ies_s& ue_cap_info() const
    {
      assert_choice_type("ueCapabilityInformation", type_.to_string(), "criticalExtensions");
      return c.get<ue_cap_info_ies_s>();
    }
    ue_cap_info_ies_s& set_ue_cap_info()
    {
      set(types::ue_cap_info);
      return c.get<ue_cap_info_ies_s>();
    }

  private:
    types                              type_;
    choice_buffer_t<ue_cap_info_ies_s> c;

    void destroy_();
  };

  // member variables
  uint8_t      rrc_transaction_id = 0;
  crit_exts_c_ crit_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ULInformationTransfer ::= SEQUENCE
struct ul_info_transfer_s {
  struct crit_exts_c_ {
    struct types_opts {
      enum options { ul_info_transfer, crit_exts_future, nulltype } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    crit_exts_c_() = default;
    crit_exts_c_(const crit_exts_c_& other);
    crit_exts_c_& operator=(const crit_exts_c_& other);
    ~crit_exts_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    ul_info_transfer_ies_s& ul_info_transfer()
    {
      assert_choice_type("ulInformationTransfer", type_.to_string(), "criticalExtensions");
      return c.get<ul_info_transfer_ies_s>();
    }
    const ul_info_transfer_ies_s& ul_info_transfer() const
    {
      assert_choice_type("ulInformationTransfer", type_.to_string(), "criticalExtensions");
      return c.get<ul_info_transfer_ies_s>();
    }
    ul_info_transfer_ies_s& set_ul_info_transfer()
    {
      set(types::ul_info_transfer);
      return c.get<ul_info_transfer_ies_s>();
    }

  private:
    types                                   type_;
    choice_buffer_t<ul_info_transfer_ies_s> c;

    void destroy_();
  };

  // member variables
  crit_exts_c_ crit_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UL-DCCH-MessageType ::= CHOICE
struct ul_dcch_msg_type_c {
  struct c1_c_ {
    struct types_opts {
      enum options {
        meas_report,
        rrc_recfg_complete,
        rrc_setup_complete,
        rrc_reest_complete,
        rrc_resume_complete,
        security_mode_complete,
        security_mode_fail,
        ul_info_transfer,
        location_meas_ind,
        ue_cap_info,
        counter_check_resp,
        ue_assist_info,
        fail_info,
        spare3,
        spare2,
        spare1,
        nulltype
      } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    c1_c_() = default;
    c1_c_(const c1_c_& other);
    c1_c_& operator=(const c1_c_& other);
    ~c1_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    meas_report_s& meas_report()
    {
      assert_choice_type("measurementReport", type_.to_string(), "c1");
      return c.get<meas_report_s>();
    }
    rrc_recfg_complete_s& rrc_recfg_complete()
    {
      assert_choice_type("rrcReconfigurationComplete", type_.to_string(), "c1");
      return c.get<rrc_recfg_complete_s>();
    }
    rrc_setup_complete_s& rrc_setup_complete()
    {
      assert_choice_type("rrcSetupComplete", type_.to_string(), "c1");
      return c.get<rrc_setup_complete_s>();
    }
    rrc_reest_complete_s& rrc_reest_complete()
    {
      assert_choice_type("rrcReestablishmentComplete", type_.to_string(), "c1");
      return c.get<rrc_reest_complete_s>();
    }
    rrc_resume_complete_s& rrc_resume_complete()
    {
      assert_choice_type("rrcResumeComplete", type_.to_string(), "c1");
      return c.get<rrc_resume_complete_s>();
    }
    security_mode_complete_s& security_mode_complete()
    {
      assert_choice_type("securityModeComplete", type_.to_string(), "c1");
      return c.get<security_mode_complete_s>();
    }
    security_mode_fail_s& security_mode_fail()
    {
      assert_choice_type("securityModeFailure", type_.to_string(), "c1");
      return c.get<security_mode_fail_s>();
    }
    ul_info_transfer_s& ul_info_transfer()
    {
      assert_choice_type("ulInformationTransfer", type_.to_string(), "c1");
      return c.get<ul_info_transfer_s>();
    }
    location_meas_ind_s& location_meas_ind()
    {
      assert_choice_type("locationMeasurementIndication", type_.to_string(), "c1");
      return c.get<location_meas_ind_s>();
    }
    ue_cap_info_s& ue_cap_info()
    {
      assert_choice_type("ueCapabilityInformation", type_.to_string(), "c1");
      return c.get<ue_cap_info_s>();
    }
    counter_check_resp_s& counter_check_resp()
    {
      assert_choice_type("counterCheckResponse", type_.to_string(), "c1");
      return c.get<counter_check_resp_s>();
    }
    ueassist_info_s& ue_assist_info()
    {
      assert_choice_type("ueAssistanceInformation", type_.to_string(), "c1");
      return c.get<ueassist_info_s>();
    }
    fail_info_s& fail_info()
    {
      assert_choice_type("failureInformation", type_.to_string(), "c1");
      return c.get<fail_info_s>();
    }
    const meas_report_s& meas_report() const
    {
      assert_choice_type("measurementReport", type_.to_string(), "c1");
      return c.get<meas_report_s>();
    }
    const rrc_recfg_complete_s& rrc_recfg_complete() const
    {
      assert_choice_type("rrcReconfigurationComplete", type_.to_string(), "c1");
      return c.get<rrc_recfg_complete_s>();
    }
    const rrc_setup_complete_s& rrc_setup_complete() const
    {
      assert_choice_type("rrcSetupComplete", type_.to_string(), "c1");
      return c.get<rrc_setup_complete_s>();
    }
    const rrc_reest_complete_s& rrc_reest_complete() const
    {
      assert_choice_type("rrcReestablishmentComplete", type_.to_string(), "c1");
      return c.get<rrc_reest_complete_s>();
    }
    const rrc_resume_complete_s& rrc_resume_complete() const
    {
      assert_choice_type("rrcResumeComplete", type_.to_string(), "c1");
      return c.get<rrc_resume_complete_s>();
    }
    const security_mode_complete_s& security_mode_complete() const
    {
      assert_choice_type("securityModeComplete", type_.to_string(), "c1");
      return c.get<security_mode_complete_s>();
    }
    const security_mode_fail_s& security_mode_fail() const
    {
      assert_choice_type("securityModeFailure", type_.to_string(), "c1");
      return c.get<security_mode_fail_s>();
    }
    const ul_info_transfer_s& ul_info_transfer() const
    {
      assert_choice_type("ulInformationTransfer", type_.to_string(), "c1");
      return c.get<ul_info_transfer_s>();
    }
    const location_meas_ind_s& location_meas_ind() const
    {
      assert_choice_type("locationMeasurementIndication", type_.to_string(), "c1");
      return c.get<location_meas_ind_s>();
    }
    const ue_cap_info_s& ue_cap_info() const
    {
      assert_choice_type("ueCapabilityInformation", type_.to_string(), "c1");
      return c.get<ue_cap_info_s>();
    }
    const counter_check_resp_s& counter_check_resp() const
    {
      assert_choice_type("counterCheckResponse", type_.to_string(), "c1");
      return c.get<counter_check_resp_s>();
    }
    const ueassist_info_s& ue_assist_info() const
    {
      assert_choice_type("ueAssistanceInformation", type_.to_string(), "c1");
      return c.get<ueassist_info_s>();
    }
    const fail_info_s& fail_info() const
    {
      assert_choice_type("failureInformation", type_.to_string(), "c1");
      return c.get<fail_info_s>();
    }
    meas_report_s& set_meas_report()
    {
      set(types::meas_report);
      return c.get<meas_report_s>();
    }
    rrc_recfg_complete_s& set_rrc_recfg_complete()
    {
      set(types::rrc_recfg_complete);
      return c.get<rrc_recfg_complete_s>();
    }
    rrc_setup_complete_s& set_rrc_setup_complete()
    {
      set(types::rrc_setup_complete);
      return c.get<rrc_setup_complete_s>();
    }
    rrc_reest_complete_s& set_rrc_reest_complete()
    {
      set(types::rrc_reest_complete);
      return c.get<rrc_reest_complete_s>();
    }
    rrc_resume_complete_s& set_rrc_resume_complete()
    {
      set(types::rrc_resume_complete);
      return c.get<rrc_resume_complete_s>();
    }
    security_mode_complete_s& set_security_mode_complete()
    {
      set(types::security_mode_complete);
      return c.get<security_mode_complete_s>();
    }
    security_mode_fail_s& set_security_mode_fail()
    {
      set(types::security_mode_fail);
      return c.get<security_mode_fail_s>();
    }
    ul_info_transfer_s& set_ul_info_transfer()
    {
      set(types::ul_info_transfer);
      return c.get<ul_info_transfer_s>();
    }
    location_meas_ind_s& set_location_meas_ind()
    {
      set(types::location_meas_ind);
      return c.get<location_meas_ind_s>();
    }
    ue_cap_info_s& set_ue_cap_info()
    {
      set(types::ue_cap_info);
      return c.get<ue_cap_info_s>();
    }
    counter_check_resp_s& set_counter_check_resp()
    {
      set(types::counter_check_resp);
      return c.get<counter_check_resp_s>();
    }
    ueassist_info_s& set_ue_assist_info()
    {
      set(types::ue_assist_info);
      return c.get<ueassist_info_s>();
    }
    fail_info_s& set_fail_info()
    {
      set(types::fail_info);
      return c.get<fail_info_s>();
    }

  private:
    types type_;
    choice_buffer_t<counter_check_resp_s,
                    fail_info_s,
                    location_meas_ind_s,
                    meas_report_s,
                    rrc_recfg_complete_s,
                    rrc_reest_complete_s,
                    rrc_resume_complete_s,
                    rrc_setup_complete_s,
                    security_mode_complete_s,
                    security_mode_fail_s,
                    ue_cap_info_s,
                    ueassist_info_s,
                    ul_info_transfer_s>
        c;

    void destroy_();
  };
  struct types_opts {
    enum options { c1, msg_class_ext, nulltype } value;
    typedef uint8_t number_type;

    std::string to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<types_opts> types;

  // choice methods
  ul_dcch_msg_type_c() = default;
  ul_dcch_msg_type_c(const ul_dcch_msg_type_c& other);
  ul_dcch_msg_type_c& operator=(const ul_dcch_msg_type_c& other);
  ~ul_dcch_msg_type_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  c1_c_& c1()
  {
    assert_choice_type("c1", type_.to_string(), "UL-DCCH-MessageType");
    return c.get<c1_c_>();
  }
  const c1_c_& c1() const
  {
    assert_choice_type("c1", type_.to_string(), "UL-DCCH-MessageType");
    return c.get<c1_c_>();
  }
  c1_c_& set_c1()
  {
    set(types::c1);
    return c.get<c1_c_>();
  }

private:
  types                  type_;
  choice_buffer_t<c1_c_> c;

  void destroy_();
};

// UL-DCCH-Message ::= SEQUENCE
struct ul_dcch_msg_s {
  ul_dcch_msg_type_c msg;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CA-BandwidthClassEUTRA ::= ENUMERATED
struct ca_bw_class_eutra_opts {
  enum options { a, b, c, d, e, f, /*...*/ nulltype } value;

  std::string to_string() const;
};
typedef enumerated<ca_bw_class_eutra_opts, true> ca_bw_class_eutra_e;

// CA-BandwidthClassNR ::= ENUMERATED
struct ca_bw_class_nr_opts {
  enum options { a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, /*...*/ nulltype } value;

  std::string to_string() const;
};
typedef enumerated<ca_bw_class_nr_opts, true> ca_bw_class_nr_e;

// BandParameters ::= CHOICE
struct band_params_c {
  struct eutra_s_ {
    bool                ca_bw_class_dl_eutra_present = false;
    bool                ca_bw_class_ul_eutra_present = false;
    uint16_t            band_eutra                   = 1;
    ca_bw_class_eutra_e ca_bw_class_dl_eutra;
    ca_bw_class_eutra_e ca_bw_class_ul_eutra;
  };
  struct nr_s_ {
    bool             ca_bw_class_dl_nr_present = false;
    bool             ca_bw_class_ul_nr_present = false;
    uint16_t         band_nr                   = 1;
    ca_bw_class_nr_e ca_bw_class_dl_nr;
    ca_bw_class_nr_e ca_bw_class_ul_nr;
  };
  struct types_opts {
    enum options { eutra, nr, nulltype } value;

    std::string to_string() const;
  };
  typedef enumerated<types_opts> types;

  // choice methods
  band_params_c() = default;
  band_params_c(const band_params_c& other);
  band_params_c& operator=(const band_params_c& other);
  ~band_params_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  eutra_s_& eutra()
  {
    assert_choice_type("eutra", type_.to_string(), "BandParameters");
    return c.get<eutra_s_>();
  }
  nr_s_& nr()
  {
    assert_choice_type("nr", type_.to_string(), "BandParameters");
    return c.get<nr_s_>();
  }
  const eutra_s_& eutra() const
  {
    assert_choice_type("eutra", type_.to_string(), "BandParameters");
    return c.get<eutra_s_>();
  }
  const nr_s_& nr() const
  {
    assert_choice_type("nr", type_.to_string(), "BandParameters");
    return c.get<nr_s_>();
  }
  eutra_s_& set_eutra()
  {
    set(types::eutra);
    return c.get<eutra_s_>();
  }
  nr_s_& set_nr()
  {
    set(types::nr);
    return c.get<nr_s_>();
  }

private:
  types                            type_;
  choice_buffer_t<eutra_s_, nr_s_> c;

  void destroy_();
};

// CA-ParametersEUTRA ::= SEQUENCE
struct ca_params_eutra_s {
  bool                     ext                                              = false;
  bool                     multiple_timing_advance_present                  = false;
  bool                     simul_rx_tx_present                              = false;
  bool                     supported_naics_minus2_crs_ap_present            = false;
  bool                     add_rx_tx_performance_req_present                = false;
  bool                     ue_ca_pwr_class_n_present                        = false;
  bool                     supported_bw_combination_set_eutra_v1530_present = false;
  bounded_bitstring<1, 8>  supported_naics_minus2_crs_ap;
  bounded_bitstring<1, 32> supported_bw_combination_set_eutra_v1530;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CA-ParametersNR ::= SEQUENCE
struct ca_params_nr_s {
  struct supported_num_tag_opts {
    enum options { n2, n3, n4, nulltype } value;
    typedef uint8_t number_type;

    std::string to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<supported_num_tag_opts> supported_num_tag_e_;

  // member variables
  bool                 ext                                        = false;
  bool                 multiple_timing_advances_present           = false;
  bool                 parallel_tx_srs_pucch_pusch_present        = false;
  bool                 parallel_tx_prach_srs_pucch_pusch_present  = false;
  bool                 simul_rx_tx_inter_band_ca_present          = false;
  bool                 simul_rx_tx_sul_present                    = false;
  bool                 diff_numerology_across_pucch_group_present = false;
  bool                 diff_numerology_within_pucch_group_present = false;
  bool                 supported_num_tag_present                  = false;
  supported_num_tag_e_ supported_num_tag;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MRDC-Parameters ::= SEQUENCE
struct mrdc_params_s {
  struct ul_sharing_eutra_nr_opts {
    enum options { tdm, fdm, both, nulltype } value;

    std::string to_string() const;
  };
  typedef enumerated<ul_sharing_eutra_nr_opts> ul_sharing_eutra_nr_e_;
  struct ul_switching_time_eutra_nr_opts {
    enum options { type1, type2, nulltype } value;
    typedef uint8_t number_type;

    std::string to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<ul_switching_time_eutra_nr_opts> ul_switching_time_eutra_nr_e_;
  struct intra_band_endc_support_v1540_opts {
    enum options { non_contiguous, both, nulltype } value;

    std::string to_string() const;
  };
  typedef enumerated<intra_band_endc_support_v1540_opts> intra_band_endc_support_v1540_e_;

  // member variables
  bool                          ext                                 = false;
  bool                          single_ul_tx_present                = false;
  bool                          dynamic_pwr_sharing_present         = false;
  bool                          tdm_pattern_present                 = false;
  bool                          ul_sharing_eutra_nr_present         = false;
  bool                          ul_switching_time_eutra_nr_present  = false;
  bool                          simul_rx_tx_inter_band_endc_present = false;
  bool                          async_intra_band_endc_present       = false;
  ul_sharing_eutra_nr_e_        ul_sharing_eutra_nr;
  ul_switching_time_eutra_nr_e_ ul_switching_time_eutra_nr;
  // ...
  // group 0
  bool                             dual_pa_architecture_present          = false;
  bool                             intra_band_endc_support_v1540_present = false;
  bool                             ul_timing_align_eutra_nr_present      = false;
  intra_band_endc_support_v1540_e_ intra_band_endc_support_v1540;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BandCombination ::= SEQUENCE
struct band_combination_s {
  using band_list_l_ = dyn_array<band_params_c>;

  // member variables
  bool                     ca_params_eutra_present              = false;
  bool                     ca_params_nr_present                 = false;
  bool                     mrdc_params_present                  = false;
  bool                     supported_bw_combination_set_present = false;
  bool                     pwr_class_v1530_present              = false;
  band_list_l_             band_list;
  uint16_t                 feature_set_combination = 0;
  ca_params_eutra_s        ca_params_eutra;
  ca_params_nr_s           ca_params_nr;
  mrdc_params_s            mrdc_params;
  bounded_bitstring<1, 32> supported_bw_combination_set;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SRS-SwitchingTimeEUTRA ::= SEQUENCE
struct srs_switching_time_eutra_s {
  struct switching_time_dl_opts {
    enum options {
      n0,
      n0dot5,
      n1,
      n1dot5,
      n2,
      n2dot5,
      n3,
      n3dot5,
      n4,
      n4dot5,
      n5,
      n5dot5,
      n6,
      n6dot5,
      n7,
      nulltype
    } value;
    typedef float number_type;

    std::string to_string() const;
    float       to_number() const;
    std::string to_number_string() const;
  };
  typedef enumerated<switching_time_dl_opts> switching_time_dl_e_;
  struct switching_time_ul_opts {
    enum options {
      n0,
      n0dot5,
      n1,
      n1dot5,
      n2,
      n2dot5,
      n3,
      n3dot5,
      n4,
      n4dot5,
      n5,
      n5dot5,
      n6,
      n6dot5,
      n7,
      nulltype
    } value;
    typedef float number_type;

    std::string to_string() const;
    float       to_number() const;
    std::string to_number_string() const;
  };
  typedef enumerated<switching_time_ul_opts> switching_time_ul_e_;

  // member variables
  bool                 switching_time_dl_present = false;
  bool                 switching_time_ul_present = false;
  switching_time_dl_e_ switching_time_dl;
  switching_time_ul_e_ switching_time_ul;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SRS-SwitchingTimeNR ::= SEQUENCE
struct srs_switching_time_nr_s {
  struct switching_time_dl_opts {
    enum options { n0us, n30us, n100us, n140us, n200us, n300us, n500us, n900us, nulltype } value;
    typedef uint16_t number_type;

    std::string to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<switching_time_dl_opts> switching_time_dl_e_;
  struct switching_time_ul_opts {
    enum options { n0us, n30us, n100us, n140us, n200us, n300us, n500us, n900us, nulltype } value;
    typedef uint16_t number_type;

    std::string to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<switching_time_ul_opts> switching_time_ul_e_;

  // member variables
  bool                 switching_time_dl_present = false;
  bool                 switching_time_ul_present = false;
  switching_time_dl_e_ switching_time_dl;
  switching_time_ul_e_ switching_time_ul;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BandParameters-v1540 ::= SEQUENCE
struct band_params_v1540_s {
  struct srs_carrier_switch_c_ {
    struct nr_s_ {
      using srs_switching_times_list_nr_l_ = dyn_array<srs_switching_time_nr_s>;

      // member variables
      srs_switching_times_list_nr_l_ srs_switching_times_list_nr;
    };
    struct eutra_s_ {
      using srs_switching_times_list_eutra_l_ = dyn_array<srs_switching_time_eutra_s>;

      // member variables
      srs_switching_times_list_eutra_l_ srs_switching_times_list_eutra;
    };
    struct types_opts {
      enum options { nr, eutra, nulltype } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    srs_carrier_switch_c_() = default;
    srs_carrier_switch_c_(const srs_carrier_switch_c_& other);
    srs_carrier_switch_c_& operator=(const srs_carrier_switch_c_& other);
    ~srs_carrier_switch_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    nr_s_& nr()
    {
      assert_choice_type("nr", type_.to_string(), "srs-CarrierSwitch");
      return c.get<nr_s_>();
    }
    eutra_s_& eutra()
    {
      assert_choice_type("eutra", type_.to_string(), "srs-CarrierSwitch");
      return c.get<eutra_s_>();
    }
    const nr_s_& nr() const
    {
      assert_choice_type("nr", type_.to_string(), "srs-CarrierSwitch");
      return c.get<nr_s_>();
    }
    const eutra_s_& eutra() const
    {
      assert_choice_type("eutra", type_.to_string(), "srs-CarrierSwitch");
      return c.get<eutra_s_>();
    }
    nr_s_& set_nr()
    {
      set(types::nr);
      return c.get<nr_s_>();
    }
    eutra_s_& set_eutra()
    {
      set(types::eutra);
      return c.get<eutra_s_>();
    }

  private:
    types                            type_;
    choice_buffer_t<eutra_s_, nr_s_> c;

    void destroy_();
  };
  struct srs_tx_switch_v1540_s_ {
    struct supported_srs_tx_port_switch_opts {
      enum options { t1r2, t1r4, t2r4, t1r4_t2r4, t1r1, t2r2, t4r4, not_supported, nulltype } value;

      std::string to_string() const;
    };
    typedef enumerated<supported_srs_tx_port_switch_opts> supported_srs_tx_port_switch_e_;

    // member variables
    bool                            tx_switch_impact_to_rx_present      = false;
    bool                            tx_switch_with_another_band_present = false;
    supported_srs_tx_port_switch_e_ supported_srs_tx_port_switch;
    uint8_t                         tx_switch_impact_to_rx      = 1;
    uint8_t                         tx_switch_with_another_band = 1;
  };

  // member variables
  bool                   srs_carrier_switch_present  = false;
  bool                   srs_tx_switch_v1540_present = false;
  srs_carrier_switch_c_  srs_carrier_switch;
  srs_tx_switch_v1540_s_ srs_tx_switch_v1540;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CA-ParametersNR-v1540 ::= SEQUENCE
struct ca_params_nr_v1540_s {
  struct csi_rs_im_reception_for_feedback_per_band_comb_s_ {
    bool     max_num_simul_nzp_csi_rs_act_bwp_all_cc_present         = false;
    bool     total_num_ports_simul_nzp_csi_rs_act_bwp_all_cc_present = false;
    uint8_t  max_num_simul_nzp_csi_rs_act_bwp_all_cc                 = 1;
    uint16_t total_num_ports_simul_nzp_csi_rs_act_bwp_all_cc         = 2;
  };

  // member variables
  bool                                              simul_srs_assoc_csi_rs_all_cc_present                  = false;
  bool                                              csi_rs_im_reception_for_feedback_per_band_comb_present = false;
  bool                                              simul_csi_reports_all_cc_present                       = false;
  bool                                              dual_pa_architecture_present                           = false;
  uint8_t                                           simul_srs_assoc_csi_rs_all_cc                          = 5;
  csi_rs_im_reception_for_feedback_per_band_comb_s_ csi_rs_im_reception_for_feedback_per_band_comb;
  uint8_t                                           simul_csi_reports_all_cc = 5;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BandCombination-v1540 ::= SEQUENCE
struct band_combination_v1540_s {
  using band_list_v1540_l_ = dyn_array<band_params_v1540_s>;

  // member variables
  bool                 ca_params_nr_v1540_present = false;
  band_list_v1540_l_   band_list_v1540;
  ca_params_nr_v1540_s ca_params_nr_v1540;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CA-ParametersNR-v1550 ::= SEQUENCE
struct ca_params_nr_v1550_s {
  bool aperiodic_csi_diff_scs_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BandCombination-v1550 ::= SEQUENCE
struct band_combination_v1550_s {
  ca_params_nr_v1550_s ca_params_nr_v1550;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BandCombinationList ::= SEQUENCE (SIZE (1..65536)) OF BandCombination
using band_combination_list_l = dyn_array<band_combination_s>;

// BandCombinationList-v1540 ::= SEQUENCE (SIZE (1..65536)) OF BandCombination-v1540
using band_combination_list_v1540_l = dyn_array<band_combination_v1540_s>;

// BandCombinationList-v1550 ::= SEQUENCE (SIZE (1..65536)) OF BandCombination-v1550
using band_combination_list_v1550_l = dyn_array<band_combination_v1550_s>;

// SupportedCSI-RS-Resource ::= SEQUENCE
struct supported_csi_rs_res_s {
  struct max_num_tx_ports_per_res_opts {
    enum options { p2, p4, p8, p12, p16, p24, p32, nulltype } value;
    typedef uint8_t number_type;

    std::string to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<max_num_tx_ports_per_res_opts> max_num_tx_ports_per_res_e_;

  // member variables
  max_num_tx_ports_per_res_e_ max_num_tx_ports_per_res;
  uint8_t                     max_num_res_per_band        = 1;
  uint16_t                    total_num_tx_ports_per_band = 2;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BeamManagementSSB-CSI-RS ::= SEQUENCE
struct beam_management_ssb_csi_rs_s {
  struct max_num_ssb_csi_rs_res_one_tx_opts {
    enum options { n0, n8, n16, n32, n64, nulltype } value;
    typedef uint8_t number_type;

    std::string to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<max_num_ssb_csi_rs_res_one_tx_opts> max_num_ssb_csi_rs_res_one_tx_e_;
  struct max_num_csi_rs_res_opts {
    enum options { n0, n4, n8, n16, n32, n64, nulltype } value;
    typedef uint8_t number_type;

    std::string to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<max_num_csi_rs_res_opts> max_num_csi_rs_res_e_;
  struct max_num_csi_rs_res_two_tx_opts {
    enum options { n0, n4, n8, n16, n32, n64, nulltype } value;
    typedef uint8_t number_type;

    std::string to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<max_num_csi_rs_res_two_tx_opts> max_num_csi_rs_res_two_tx_e_;
  struct supported_csi_rs_density_opts {
    enum options { one, three, one_and_three, nulltype } value;

    std::string to_string() const;
  };
  typedef enumerated<supported_csi_rs_density_opts> supported_csi_rs_density_e_;
  struct max_num_aperiodic_csi_rs_res_opts {
    enum options { n0, n1, n4, n8, n16, n32, n64, nulltype } value;
    typedef uint8_t number_type;

    std::string to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<max_num_aperiodic_csi_rs_res_opts> max_num_aperiodic_csi_rs_res_e_;

  // member variables
  bool                             supported_csi_rs_density_present = false;
  max_num_ssb_csi_rs_res_one_tx_e_ max_num_ssb_csi_rs_res_one_tx;
  max_num_csi_rs_res_e_            max_num_csi_rs_res;
  max_num_csi_rs_res_two_tx_e_     max_num_csi_rs_res_two_tx;
  supported_csi_rs_density_e_      supported_csi_rs_density;
  max_num_aperiodic_csi_rs_res_e_  max_num_aperiodic_csi_rs_res;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CSI-RS-ForTracking ::= SEQUENCE
struct csi_rs_for_tracking_s {
  uint8_t  max_burst_len               = 1;
  uint8_t  max_simul_res_sets_per_cc   = 1;
  uint8_t  max_cfgured_res_sets_per_cc = 1;
  uint16_t max_cfgured_res_sets_all_cc = 1;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CSI-RS-IM-ReceptionForFeedback ::= SEQUENCE
struct csi_rs_im_reception_for_feedback_s {
  struct max_cfg_num_csi_im_per_cc_opts {
    enum options { n1, n2, n4, n8, n16, n32, nulltype } value;
    typedef uint8_t number_type;

    std::string to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<max_cfg_num_csi_im_per_cc_opts> max_cfg_num_csi_im_per_cc_e_;

  // member variables
  uint8_t                      max_cfg_num_nzp_csi_rs_per_cc              = 1;
  uint16_t                     max_cfg_num_ports_across_nzp_csi_rs_per_cc = 2;
  max_cfg_num_csi_im_per_cc_e_ max_cfg_num_csi_im_per_cc;
  uint8_t                      max_num_simul_nzp_csi_rs_per_cc         = 1;
  uint16_t                     total_num_ports_simul_nzp_csi_rs_per_cc = 2;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CSI-RS-ProcFrameworkForSRS ::= SEQUENCE
struct csi_rs_proc_framework_for_srs_s {
  uint8_t max_num_periodic_srs_assoc_csi_rs_per_bwp  = 1;
  uint8_t max_num_aperiodic_srs_assoc_csi_rs_per_bwp = 1;
  uint8_t max_num_sp_srs_assoc_csi_rs_per_bwp        = 0;
  uint8_t simul_srs_assoc_csi_rs_per_cc              = 1;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CSI-ReportFramework ::= SEQUENCE
struct csi_report_framework_s {
  struct max_num_aperiodic_csi_trigger_state_per_cc_opts {
    enum options { n3, n7, n15, n31, n63, n128, nulltype } value;
    typedef uint8_t number_type;

    std::string to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<max_num_aperiodic_csi_trigger_state_per_cc_opts> max_num_aperiodic_csi_trigger_state_per_cc_e_;

  // member variables
  uint8_t                                       max_num_periodic_csi_per_bwp_for_csi_report        = 1;
  uint8_t                                       max_num_aperiodic_csi_per_bwp_for_csi_report       = 1;
  uint8_t                                       max_num_semi_persistent_csi_per_bwp_for_csi_report = 0;
  uint8_t                                       max_num_periodic_csi_per_bwp_for_beam_report       = 1;
  uint8_t                                       max_num_aperiodic_csi_per_bwp_for_beam_report      = 1;
  max_num_aperiodic_csi_trigger_state_per_cc_e_ max_num_aperiodic_csi_trigger_state_per_cc;
  uint8_t                                       max_num_semi_persistent_csi_per_bwp_for_beam_report = 0;
  uint8_t                                       simul_csi_reports_per_cc                            = 1;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CodebookParameters ::= SEQUENCE
struct codebook_params_s {
  struct type1_s_ {
    struct single_panel_s_ {
      using supported_csi_rs_res_list_l_ = dyn_array<supported_csi_rs_res_s>;
      struct modes_opts {
        enum options { mode1, mode1and_mode2, nulltype } value;

        std::string to_string() const;
      };
      typedef enumerated<modes_opts> modes_e_;

      // member variables
      supported_csi_rs_res_list_l_ supported_csi_rs_res_list;
      modes_e_                     modes;
      uint8_t                      max_num_csi_rs_per_res_set = 1;
    };
    struct multi_panel_s_ {
      using supported_csi_rs_res_list_l_ = dyn_array<supported_csi_rs_res_s>;
      struct modes_opts {
        enum options { mode1, mode2, both, nulltype } value;
        typedef uint8_t number_type;

        std::string to_string() const;
        uint8_t     to_number() const;
      };
      typedef enumerated<modes_opts> modes_e_;
      struct nrof_panels_opts {
        enum options { n2, n4, nulltype } value;
        typedef uint8_t number_type;

        std::string to_string() const;
        uint8_t     to_number() const;
      };
      typedef enumerated<nrof_panels_opts> nrof_panels_e_;

      // member variables
      supported_csi_rs_res_list_l_ supported_csi_rs_res_list;
      modes_e_                     modes;
      nrof_panels_e_               nrof_panels;
      uint8_t                      max_num_csi_rs_per_res_set = 1;
    };

    // member variables
    bool            multi_panel_present = false;
    single_panel_s_ single_panel;
    multi_panel_s_  multi_panel;
  };
  struct type2_s_ {
    using supported_csi_rs_res_list_l_ = dyn_array<supported_csi_rs_res_s>;
    struct amplitude_scaling_type_opts {
      enum options { wideband, wideband_and_subband, nulltype } value;

      std::string to_string() const;
    };
    typedef enumerated<amplitude_scaling_type_opts> amplitude_scaling_type_e_;

    // member variables
    bool                         amplitude_subset_restrict_present = false;
    supported_csi_rs_res_list_l_ supported_csi_rs_res_list;
    uint8_t                      param_lx = 2;
    amplitude_scaling_type_e_    amplitude_scaling_type;
  };
  struct type2_port_sel_s_ {
    using supported_csi_rs_res_list_l_ = dyn_array<supported_csi_rs_res_s>;
    struct amplitude_scaling_type_opts {
      enum options { wideband, wideband_and_subband, nulltype } value;

      std::string to_string() const;
    };
    typedef enumerated<amplitude_scaling_type_opts> amplitude_scaling_type_e_;

    // member variables
    supported_csi_rs_res_list_l_ supported_csi_rs_res_list;
    uint8_t                      param_lx = 2;
    amplitude_scaling_type_e_    amplitude_scaling_type;
  };

  // member variables
  bool              type2_present          = false;
  bool              type2_port_sel_present = false;
  type1_s_          type1;
  type2_s_          type2;
  type2_port_sel_s_ type2_port_sel;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DummyG ::= SEQUENCE
struct dummy_g_s {
  struct max_num_ssb_csi_rs_res_one_tx_opts {
    enum options { n8, n16, n32, n64, nulltype } value;
    typedef uint8_t number_type;

    std::string to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<max_num_ssb_csi_rs_res_one_tx_opts> max_num_ssb_csi_rs_res_one_tx_e_;
  struct max_num_ssb_csi_rs_res_two_tx_opts {
    enum options { n0, n4, n8, n16, n32, n64, nulltype } value;
    typedef uint8_t number_type;

    std::string to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<max_num_ssb_csi_rs_res_two_tx_opts> max_num_ssb_csi_rs_res_two_tx_e_;
  struct supported_csi_rs_density_opts {
    enum options { one, three, one_and_three, nulltype } value;

    std::string to_string() const;
  };
  typedef enumerated<supported_csi_rs_density_opts> supported_csi_rs_density_e_;

  // member variables
  max_num_ssb_csi_rs_res_one_tx_e_ max_num_ssb_csi_rs_res_one_tx;
  max_num_ssb_csi_rs_res_two_tx_e_ max_num_ssb_csi_rs_res_two_tx;
  supported_csi_rs_density_e_      supported_csi_rs_density;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DummyH ::= SEQUENCE
struct dummy_h_s {
  uint8_t burst_len                   = 1;
  uint8_t max_simul_res_sets_per_cc   = 1;
  uint8_t max_cfgured_res_sets_per_cc = 1;
  uint8_t max_cfgured_res_sets_all_cc = 1;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PTRS-DensityRecommendationDL ::= SEQUENCE
struct ptrs_density_recommendation_dl_s {
  uint16_t freq_density1 = 1;
  uint16_t freq_density2 = 1;
  uint8_t  time_density1 = 0;
  uint8_t  time_density2 = 0;
  uint8_t  time_density3 = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PTRS-DensityRecommendationUL ::= SEQUENCE
struct ptrs_density_recommendation_ul_s {
  uint16_t freq_density1   = 1;
  uint16_t freq_density2   = 1;
  uint8_t  time_density1   = 0;
  uint8_t  time_density2   = 0;
  uint8_t  time_density3   = 0;
  uint16_t sample_density1 = 1;
  uint16_t sample_density2 = 1;
  uint16_t sample_density3 = 1;
  uint16_t sample_density4 = 1;
  uint16_t sample_density5 = 1;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SpatialRelations ::= SEQUENCE
struct spatial_relations_s {
  struct max_num_cfgured_spatial_relations_opts {
    enum options { n4, n8, n16, n32, n64, n96, nulltype } value;
    typedef uint8_t number_type;

    std::string to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<max_num_cfgured_spatial_relations_opts> max_num_cfgured_spatial_relations_e_;
  struct max_num_active_spatial_relations_opts {
    enum options { n1, n2, n4, n8, n14, nulltype } value;
    typedef uint8_t number_type;

    std::string to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<max_num_active_spatial_relations_opts> max_num_active_spatial_relations_e_;
  struct max_num_dl_rs_qcl_type_d_opts {
    enum options { n1, n2, n4, n8, n14, nulltype } value;
    typedef uint8_t number_type;

    std::string to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<max_num_dl_rs_qcl_type_d_opts> max_num_dl_rs_qcl_type_d_e_;

  // member variables
  bool                                 add_active_spatial_relation_pucch_present = false;
  max_num_cfgured_spatial_relations_e_ max_num_cfgured_spatial_relations;
  max_num_active_spatial_relations_e_  max_num_active_spatial_relations;
  max_num_dl_rs_qcl_type_d_e_          max_num_dl_rs_qcl_type_d;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MIMO-ParametersPerBand ::= SEQUENCE
struct mimo_params_per_band_s {
  struct tci_state_pdsch_s_ {
    struct max_num_cfgured_tcistates_per_cc_opts {
      enum options { n4, n8, n16, n32, n64, n128, nulltype } value;
      typedef uint8_t number_type;

      std::string to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<max_num_cfgured_tcistates_per_cc_opts> max_num_cfgured_tcistates_per_cc_e_;
    struct max_num_active_tci_per_bwp_opts {
      enum options { n1, n2, n4, n8, nulltype } value;
      typedef uint8_t number_type;

      std::string to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<max_num_active_tci_per_bwp_opts> max_num_active_tci_per_bwp_e_;

    // member variables
    bool                                max_num_cfgured_tcistates_per_cc_present = false;
    bool                                max_num_active_tci_per_bwp_present       = false;
    max_num_cfgured_tcistates_per_cc_e_ max_num_cfgured_tcistates_per_cc;
    max_num_active_tci_per_bwp_e_       max_num_active_tci_per_bwp;
  };
  struct pusch_trans_coherence_opts {
    enum options { non_coherent, partial_coherent, full_coherent, nulltype } value;

    std::string to_string() const;
  };
  typedef enumerated<pusch_trans_coherence_opts> pusch_trans_coherence_e_;
  struct max_num_rx_tx_beam_switch_dl_s_ {
    struct scs_minus15k_hz_opts {
      enum options { n4, n7, n14, nulltype } value;
      typedef uint8_t number_type;

      std::string to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<scs_minus15k_hz_opts> scs_minus15k_hz_e_;
    struct scs_minus30k_hz_opts {
      enum options { n4, n7, n14, nulltype } value;
      typedef uint8_t number_type;

      std::string to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<scs_minus30k_hz_opts> scs_minus30k_hz_e_;
    struct scs_minus60k_hz_opts {
      enum options { n4, n7, n14, nulltype } value;
      typedef uint8_t number_type;

      std::string to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<scs_minus60k_hz_opts> scs_minus60k_hz_e_;
    struct scs_minus120k_hz_opts {
      enum options { n4, n7, n14, nulltype } value;
      typedef uint8_t number_type;

      std::string to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<scs_minus120k_hz_opts> scs_minus120k_hz_e_;
    struct scs_minus240k_hz_opts {
      enum options { n4, n7, n14, nulltype } value;
      typedef uint8_t number_type;

      std::string to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<scs_minus240k_hz_opts> scs_minus240k_hz_e_;

    // member variables
    bool                scs_minus15k_hz_present  = false;
    bool                scs_minus30k_hz_present  = false;
    bool                scs_minus60k_hz_present  = false;
    bool                scs_minus120k_hz_present = false;
    bool                scs_minus240k_hz_present = false;
    scs_minus15k_hz_e_  scs_minus15k_hz;
    scs_minus30k_hz_e_  scs_minus30k_hz;
    scs_minus60k_hz_e_  scs_minus60k_hz;
    scs_minus120k_hz_e_ scs_minus120k_hz;
    scs_minus240k_hz_e_ scs_minus240k_hz;
  };
  struct max_num_non_group_beam_report_opts {
    enum options { n1, n2, n4, nulltype } value;
    typedef uint8_t number_type;

    std::string to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<max_num_non_group_beam_report_opts> max_num_non_group_beam_report_e_;
  struct ul_beam_management_s_ {
    struct max_num_srs_res_per_set_bm_opts {
      enum options { n2, n4, n8, n16, nulltype } value;
      typedef uint8_t number_type;

      std::string to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<max_num_srs_res_per_set_bm_opts> max_num_srs_res_per_set_bm_e_;

    // member variables
    max_num_srs_res_per_set_bm_e_ max_num_srs_res_per_set_bm;
    uint8_t                       max_num_srs_res_set = 1;
  };
  struct beam_report_timing_s_ {
    struct scs_minus15k_hz_opts {
      enum options { sym2, sym4, sym8, nulltype } value;
      typedef uint8_t number_type;

      std::string to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<scs_minus15k_hz_opts> scs_minus15k_hz_e_;
    struct scs_minus30k_hz_opts {
      enum options { sym4, sym8, sym14, sym28, nulltype } value;
      typedef uint8_t number_type;

      std::string to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<scs_minus30k_hz_opts> scs_minus30k_hz_e_;
    struct scs_minus60k_hz_opts {
      enum options { sym8, sym14, sym28, nulltype } value;
      typedef uint8_t number_type;

      std::string to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<scs_minus60k_hz_opts> scs_minus60k_hz_e_;
    struct scs_minus120k_hz_opts {
      enum options { sym14, sym28, sym56, nulltype } value;
      typedef uint8_t number_type;

      std::string to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<scs_minus120k_hz_opts> scs_minus120k_hz_e_;

    // member variables
    bool                scs_minus15k_hz_present  = false;
    bool                scs_minus30k_hz_present  = false;
    bool                scs_minus60k_hz_present  = false;
    bool                scs_minus120k_hz_present = false;
    scs_minus15k_hz_e_  scs_minus15k_hz;
    scs_minus30k_hz_e_  scs_minus30k_hz;
    scs_minus60k_hz_e_  scs_minus60k_hz;
    scs_minus120k_hz_e_ scs_minus120k_hz;
  };
  struct ptrs_density_recommendation_set_dl_s_ {
    bool                             scs_minus15k_hz_present  = false;
    bool                             scs_minus30k_hz_present  = false;
    bool                             scs_minus60k_hz_present  = false;
    bool                             scs_minus120k_hz_present = false;
    ptrs_density_recommendation_dl_s scs_minus15k_hz;
    ptrs_density_recommendation_dl_s scs_minus30k_hz;
    ptrs_density_recommendation_dl_s scs_minus60k_hz;
    ptrs_density_recommendation_dl_s scs_minus120k_hz;
  };
  struct ptrs_density_recommendation_set_ul_s_ {
    bool                             scs_minus15k_hz_present  = false;
    bool                             scs_minus30k_hz_present  = false;
    bool                             scs_minus60k_hz_present  = false;
    bool                             scs_minus120k_hz_present = false;
    ptrs_density_recommendation_ul_s scs_minus15k_hz;
    ptrs_density_recommendation_ul_s scs_minus30k_hz;
    ptrs_density_recommendation_ul_s scs_minus60k_hz;
    ptrs_density_recommendation_ul_s scs_minus120k_hz;
  };
  struct beam_switch_timing_s_ {
    struct scs_minus60k_hz_opts {
      enum options { sym14, sym28, sym48, sym224, sym336, nulltype } value;
      typedef uint16_t number_type;

      std::string to_string() const;
      uint16_t    to_number() const;
    };
    typedef enumerated<scs_minus60k_hz_opts> scs_minus60k_hz_e_;
    struct scs_minus120k_hz_opts {
      enum options { sym14, sym28, sym48, sym224, sym336, nulltype } value;
      typedef uint16_t number_type;

      std::string to_string() const;
      uint16_t    to_number() const;
    };
    typedef enumerated<scs_minus120k_hz_opts> scs_minus120k_hz_e_;

    // member variables
    bool                scs_minus60k_hz_present  = false;
    bool                scs_minus120k_hz_present = false;
    scs_minus60k_hz_e_  scs_minus60k_hz;
    scs_minus120k_hz_e_ scs_minus120k_hz;
  };
  using srs_assoc_csi_rs_l_ = dyn_array<supported_csi_rs_res_s>;

  // member variables
  bool                                  ext                                                  = false;
  bool                                  tci_state_pdsch_present                              = false;
  bool                                  add_active_tci_state_pdcch_present                   = false;
  bool                                  pusch_trans_coherence_present                        = false;
  bool                                  beam_correspondence_without_ul_beam_sweeping_present = false;
  bool                                  periodic_beam_report_present                         = false;
  bool                                  aperiodic_beam_report_present                        = false;
  bool                                  sp_beam_report_pucch_present                         = false;
  bool                                  sp_beam_report_pusch_present                         = false;
  bool                                  dummy1_present                                       = false;
  bool                                  max_num_rx_beam_present                              = false;
  bool                                  max_num_rx_tx_beam_switch_dl_present                 = false;
  bool                                  max_num_non_group_beam_report_present                = false;
  bool                                  group_beam_report_present                            = false;
  bool                                  ul_beam_management_present                           = false;
  bool                                  max_num_csi_rs_bfd_present                           = false;
  bool                                  max_num_ssb_bfd_present                              = false;
  bool                                  max_num_csi_rs_ssb_cbd_present                       = false;
  bool                                  dummy2_present                                       = false;
  bool                                  two_ports_ptrs_ul_present                            = false;
  bool                                  dummy5_present                                       = false;
  bool                                  dummy3_present                                       = false;
  bool                                  beam_report_timing_present                           = false;
  bool                                  ptrs_density_recommendation_set_dl_present           = false;
  bool                                  ptrs_density_recommendation_set_ul_present           = false;
  bool                                  dummy4_present                                       = false;
  bool                                  aperiodic_trs_present                                = false;
  tci_state_pdsch_s_                    tci_state_pdsch;
  pusch_trans_coherence_e_              pusch_trans_coherence;
  dummy_g_s                             dummy1;
  uint8_t                               max_num_rx_beam = 2;
  max_num_rx_tx_beam_switch_dl_s_       max_num_rx_tx_beam_switch_dl;
  max_num_non_group_beam_report_e_      max_num_non_group_beam_report;
  ul_beam_management_s_                 ul_beam_management;
  uint8_t                               max_num_csi_rs_bfd     = 1;
  uint8_t                               max_num_ssb_bfd        = 1;
  uint16_t                              max_num_csi_rs_ssb_cbd = 1;
  srs_res_s                             dummy5;
  uint8_t                               dummy3 = 1;
  beam_report_timing_s_                 beam_report_timing;
  ptrs_density_recommendation_set_dl_s_ ptrs_density_recommendation_set_dl;
  ptrs_density_recommendation_set_ul_s_ ptrs_density_recommendation_set_ul;
  dummy_h_s                             dummy4;
  // ...
  // group 0
  bool                                         beam_correspondence_ca_present = false;
  copy_ptr<beam_management_ssb_csi_rs_s>       beam_management_ssb_csi_rs;
  copy_ptr<beam_switch_timing_s_>              beam_switch_timing;
  copy_ptr<codebook_params_s>                  codebook_params;
  copy_ptr<csi_rs_im_reception_for_feedback_s> csi_rs_im_reception_for_feedback;
  copy_ptr<csi_rs_proc_framework_for_srs_s>    csi_rs_proc_framework_for_srs;
  copy_ptr<csi_report_framework_s>             csi_report_framework;
  copy_ptr<csi_rs_for_tracking_s>              csi_rs_for_tracking;
  copy_ptr<srs_assoc_csi_rs_l_>                srs_assoc_csi_rs;
  copy_ptr<spatial_relations_s>                spatial_relations;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BandNR ::= SEQUENCE
struct band_nr_s {
  struct bwp_same_numerology_opts {
    enum options { upto2, upto4, nulltype } value;
    typedef uint8_t number_type;

    std::string to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<bwp_same_numerology_opts> bwp_same_numerology_e_;
  struct ue_pwr_class_opts {
    enum options { pc1, pc2, pc3, pc4, nulltype } value;
    typedef uint8_t number_type;

    std::string to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<ue_pwr_class_opts> ue_pwr_class_e_;
  struct ch_bws_dl_v1530_c_ {
    struct fr1_s_ {
      bool                scs_minus15k_hz_present = false;
      bool                scs_minus30k_hz_present = false;
      bool                scs_minus60k_hz_present = false;
      fixed_bitstring<10> scs_minus15k_hz;
      fixed_bitstring<10> scs_minus30k_hz;
      fixed_bitstring<10> scs_minus60k_hz;
    };
    struct fr2_s_ {
      bool               scs_minus60k_hz_present  = false;
      bool               scs_minus120k_hz_present = false;
      fixed_bitstring<3> scs_minus60k_hz;
      fixed_bitstring<3> scs_minus120k_hz;
    };
    struct types_opts {
      enum options { fr1, fr2, nulltype } value;
      typedef uint8_t number_type;

      std::string to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    ch_bws_dl_v1530_c_() = default;
    ch_bws_dl_v1530_c_(const ch_bws_dl_v1530_c_& other);
    ch_bws_dl_v1530_c_& operator=(const ch_bws_dl_v1530_c_& other);
    ~ch_bws_dl_v1530_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    fr1_s_& fr1()
    {
      assert_choice_type("fr1", type_.to_string(), "channelBWs-DL-v1530");
      return c.get<fr1_s_>();
    }
    fr2_s_& fr2()
    {
      assert_choice_type("fr2", type_.to_string(), "channelBWs-DL-v1530");
      return c.get<fr2_s_>();
    }
    const fr1_s_& fr1() const
    {
      assert_choice_type("fr1", type_.to_string(), "channelBWs-DL-v1530");
      return c.get<fr1_s_>();
    }
    const fr2_s_& fr2() const
    {
      assert_choice_type("fr2", type_.to_string(), "channelBWs-DL-v1530");
      return c.get<fr2_s_>();
    }
    fr1_s_& set_fr1()
    {
      set(types::fr1);
      return c.get<fr1_s_>();
    }
    fr2_s_& set_fr2()
    {
      set(types::fr2);
      return c.get<fr2_s_>();
    }

  private:
    types                           type_;
    choice_buffer_t<fr1_s_, fr2_s_> c;

    void destroy_();
  };
  struct ch_bws_ul_v1530_c_ {
    struct fr1_s_ {
      bool                scs_minus15k_hz_present = false;
      bool                scs_minus30k_hz_present = false;
      bool                scs_minus60k_hz_present = false;
      fixed_bitstring<10> scs_minus15k_hz;
      fixed_bitstring<10> scs_minus30k_hz;
      fixed_bitstring<10> scs_minus60k_hz;
    };
    struct fr2_s_ {
      bool               scs_minus60k_hz_present  = false;
      bool               scs_minus120k_hz_present = false;
      fixed_bitstring<3> scs_minus60k_hz;
      fixed_bitstring<3> scs_minus120k_hz;
    };
    struct types_opts {
      enum options { fr1, fr2, nulltype } value;
      typedef uint8_t number_type;

      std::string to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    ch_bws_ul_v1530_c_() = default;
    ch_bws_ul_v1530_c_(const ch_bws_ul_v1530_c_& other);
    ch_bws_ul_v1530_c_& operator=(const ch_bws_ul_v1530_c_& other);
    ~ch_bws_ul_v1530_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    fr1_s_& fr1()
    {
      assert_choice_type("fr1", type_.to_string(), "channelBWs-UL-v1530");
      return c.get<fr1_s_>();
    }
    fr2_s_& fr2()
    {
      assert_choice_type("fr2", type_.to_string(), "channelBWs-UL-v1530");
      return c.get<fr2_s_>();
    }
    const fr1_s_& fr1() const
    {
      assert_choice_type("fr1", type_.to_string(), "channelBWs-UL-v1530");
      return c.get<fr1_s_>();
    }
    const fr2_s_& fr2() const
    {
      assert_choice_type("fr2", type_.to_string(), "channelBWs-UL-v1530");
      return c.get<fr2_s_>();
    }
    fr1_s_& set_fr1()
    {
      set(types::fr1);
      return c.get<fr1_s_>();
    }
    fr2_s_& set_fr2()
    {
      set(types::fr2);
      return c.get<fr2_s_>();
    }

  private:
    types                           type_;
    choice_buffer_t<fr1_s_, fr2_s_> c;

    void destroy_();
  };
  struct max_ul_duty_cycle_pc2_fr1_opts {
    enum options { n60, n70, n80, n90, n100, nulltype } value;
    typedef uint8_t number_type;

    std::string to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<max_ul_duty_cycle_pc2_fr1_opts> max_ul_duty_cycle_pc2_fr1_e_;

  // member variables
  bool                   ext                                  = false;
  bool                   modified_mpr_behaviour_present       = false;
  bool                   mimo_params_per_band_present         = false;
  bool                   extended_cp_present                  = false;
  bool                   multiple_tci_present                 = false;
  bool                   bwp_without_restrict_present         = false;
  bool                   bwp_same_numerology_present          = false;
  bool                   bwp_diff_numerology_present          = false;
  bool                   cross_carrier_sched_same_scs_present = false;
  bool                   pdsch_minus256_qam_fr2_present       = false;
  bool                   pusch_minus256_qam_present           = false;
  bool                   ue_pwr_class_present                 = false;
  bool                   rate_matching_lte_crs_present        = false;
  bool                   ch_bws_dl_v1530_present              = false;
  bool                   ch_bws_ul_v1530_present              = false;
  uint16_t               band_nr                              = 1;
  fixed_bitstring<8>     modified_mpr_behaviour;
  mimo_params_per_band_s mimo_params_per_band;
  bwp_same_numerology_e_ bwp_same_numerology;
  ue_pwr_class_e_        ue_pwr_class;
  ch_bws_dl_v1530_c_     ch_bws_dl_v1530;
  ch_bws_ul_v1530_c_     ch_bws_ul_v1530;
  // ...
  // group 0
  bool                         max_ul_duty_cycle_pc2_fr1_present = false;
  max_ul_duty_cycle_pc2_fr1_e_ max_ul_duty_cycle_pc2_fr1;
  // group 1
  bool pucch_spatial_rel_info_mac_ce_present = false;
  bool pwr_boosting_pi2_bpsk_present         = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// EUTRA-ParametersCommon ::= SEQUENCE
struct eutra_params_common_s {
  bool                ext                                 = false;
  bool                mfbi_eutra_present                  = false;
  bool                modified_mpr_behavior_eutra_present = false;
  bool                multi_ns_pmax_eutra_present         = false;
  bool                rs_sinr_meas_eutra_present          = false;
  fixed_bitstring<32> modified_mpr_behavior_eutra;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// EUTRA-ParametersXDD-Diff ::= SEQUENCE
struct eutra_params_xdd_diff_s {
  bool ext                              = false;
  bool rsrq_meas_wideband_eutra_present = false;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// EUTRA-Parameters ::= SEQUENCE
struct eutra_params_s {
  using supported_band_list_eutra_l_ = dyn_array<uint16_t>;

  // member variables
  bool                         ext                           = false;
  bool                         eutra_params_common_present   = false;
  bool                         eutra_params_xdd_diff_present = false;
  supported_band_list_eutra_l_ supported_band_list_eutra;
  eutra_params_common_s        eutra_params_common;
  eutra_params_xdd_diff_s      eutra_params_xdd_diff;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// FeatureSet ::= CHOICE
struct feature_set_c {
  struct eutra_s_ {
    uint16_t dl_set_eutra = 0;
    uint16_t ul_set_eutra = 0;
  };
  struct nr_s_ {
    uint16_t dl_set_nr = 0;
    uint16_t ul_set_nr = 0;
  };
  struct types_opts {
    enum options { eutra, nr, nulltype } value;

    std::string to_string() const;
  };
  typedef enumerated<types_opts> types;

  // choice methods
  feature_set_c() = default;
  feature_set_c(const feature_set_c& other);
  feature_set_c& operator=(const feature_set_c& other);
  ~feature_set_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  eutra_s_& eutra()
  {
    assert_choice_type("eutra", type_.to_string(), "FeatureSet");
    return c.get<eutra_s_>();
  }
  nr_s_& nr()
  {
    assert_choice_type("nr", type_.to_string(), "FeatureSet");
    return c.get<nr_s_>();
  }
  const eutra_s_& eutra() const
  {
    assert_choice_type("eutra", type_.to_string(), "FeatureSet");
    return c.get<eutra_s_>();
  }
  const nr_s_& nr() const
  {
    assert_choice_type("nr", type_.to_string(), "FeatureSet");
    return c.get<nr_s_>();
  }
  eutra_s_& set_eutra()
  {
    set(types::eutra);
    return c.get<eutra_s_>();
  }
  nr_s_& set_nr()
  {
    set(types::nr);
    return c.get<nr_s_>();
  }

private:
  types                            type_;
  choice_buffer_t<eutra_s_, nr_s_> c;

  void destroy_();
};

// FeatureSetsPerBand ::= SEQUENCE (SIZE (1..128)) OF FeatureSet
using feature_sets_per_band_l = dyn_array<feature_set_c>;

// FeatureSetCombination ::= SEQUENCE (SIZE (1..32)) OF FeatureSetsPerBand
using feature_set_combination_l = dyn_array<feature_sets_per_band_l>;

// DummyA ::= SEQUENCE
struct dummy_a_s {
  struct max_num_ports_across_nzp_csi_rs_per_cc_opts {
    enum options {
      p2,
      p4,
      p8,
      p12,
      p16,
      p24,
      p32,
      p40,
      p48,
      p56,
      p64,
      p72,
      p80,
      p88,
      p96,
      p104,
      p112,
      p120,
      p128,
      p136,
      p144,
      p152,
      p160,
      p168,
      p176,
      p184,
      p192,
      p200,
      p208,
      p216,
      p224,
      p232,
      p240,
      p248,
      p256,
      nulltype
    } value;
    typedef uint16_t number_type;

    std::string to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<max_num_ports_across_nzp_csi_rs_per_cc_opts> max_num_ports_across_nzp_csi_rs_per_cc_e_;
  struct max_num_cs_im_per_cc_opts {
    enum options { n1, n2, n4, n8, n16, n32, nulltype } value;
    typedef uint8_t number_type;

    std::string to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<max_num_cs_im_per_cc_opts> max_num_cs_im_per_cc_e_;
  struct max_num_simul_csi_rs_act_bwp_all_cc_opts {
    enum options {
      n5,
      n6,
      n7,
      n8,
      n9,
      n10,
      n12,
      n14,
      n16,
      n18,
      n20,
      n22,
      n24,
      n26,
      n28,
      n30,
      n32,
      n34,
      n36,
      n38,
      n40,
      n42,
      n44,
      n46,
      n48,
      n50,
      n52,
      n54,
      n56,
      n58,
      n60,
      n62,
      n64,
      nulltype
    } value;
    typedef uint8_t number_type;

    std::string to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<max_num_simul_csi_rs_act_bwp_all_cc_opts> max_num_simul_csi_rs_act_bwp_all_cc_e_;
  struct total_num_ports_simul_csi_rs_act_bwp_all_cc_opts {
    enum options {
      p8,
      p12,
      p16,
      p24,
      p32,
      p40,
      p48,
      p56,
      p64,
      p72,
      p80,
      p88,
      p96,
      p104,
      p112,
      p120,
      p128,
      p136,
      p144,
      p152,
      p160,
      p168,
      p176,
      p184,
      p192,
      p200,
      p208,
      p216,
      p224,
      p232,
      p240,
      p248,
      p256,
      nulltype
    } value;
    typedef uint16_t number_type;

    std::string to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<total_num_ports_simul_csi_rs_act_bwp_all_cc_opts> total_num_ports_simul_csi_rs_act_bwp_all_cc_e_;

  // member variables
  uint8_t                                        max_num_nzp_csi_rs_per_cc = 1;
  max_num_ports_across_nzp_csi_rs_per_cc_e_      max_num_ports_across_nzp_csi_rs_per_cc;
  max_num_cs_im_per_cc_e_                        max_num_cs_im_per_cc;
  max_num_simul_csi_rs_act_bwp_all_cc_e_         max_num_simul_csi_rs_act_bwp_all_cc;
  total_num_ports_simul_csi_rs_act_bwp_all_cc_e_ total_num_ports_simul_csi_rs_act_bwp_all_cc;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DummyB ::= SEQUENCE
struct dummy_b_s {
  struct max_num_tx_ports_per_res_opts {
    enum options { p2, p4, p8, p12, p16, p24, p32, nulltype } value;
    typedef uint8_t number_type;

    std::string to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<max_num_tx_ports_per_res_opts> max_num_tx_ports_per_res_e_;
  struct supported_codebook_mode_opts {
    enum options { mode1, mode1_and_mode2, nulltype } value;

    std::string to_string() const;
  };
  typedef enumerated<supported_codebook_mode_opts> supported_codebook_mode_e_;

  // member variables
  max_num_tx_ports_per_res_e_ max_num_tx_ports_per_res;
  uint8_t                     max_num_res        = 1;
  uint16_t                    total_num_tx_ports = 2;
  supported_codebook_mode_e_  supported_codebook_mode;
  uint8_t                     max_num_csi_rs_per_res_set = 1;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DummyC ::= SEQUENCE
struct dummy_c_s {
  struct max_num_tx_ports_per_res_opts {
    enum options { p8, p16, p32, nulltype } value;
    typedef uint8_t number_type;

    std::string to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<max_num_tx_ports_per_res_opts> max_num_tx_ports_per_res_e_;
  struct supported_codebook_mode_opts {
    enum options { mode1, mode2, both, nulltype } value;
    typedef uint8_t number_type;

    std::string to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<supported_codebook_mode_opts> supported_codebook_mode_e_;
  struct supported_num_panels_opts {
    enum options { n2, n4, nulltype } value;
    typedef uint8_t number_type;

    std::string to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<supported_num_panels_opts> supported_num_panels_e_;

  // member variables
  max_num_tx_ports_per_res_e_ max_num_tx_ports_per_res;
  uint8_t                     max_num_res        = 1;
  uint16_t                    total_num_tx_ports = 2;
  supported_codebook_mode_e_  supported_codebook_mode;
  supported_num_panels_e_     supported_num_panels;
  uint8_t                     max_num_csi_rs_per_res_set = 1;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DummyD ::= SEQUENCE
struct dummy_d_s {
  struct max_num_tx_ports_per_res_opts {
    enum options { p4, p8, p12, p16, p24, p32, nulltype } value;
    typedef uint8_t number_type;

    std::string to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<max_num_tx_ports_per_res_opts> max_num_tx_ports_per_res_e_;
  struct amplitude_scaling_type_opts {
    enum options { wideband, wideband_and_subband, nulltype } value;

    std::string to_string() const;
  };
  typedef enumerated<amplitude_scaling_type_opts> amplitude_scaling_type_e_;

  // member variables
  bool                        amplitude_subset_restrict_present = false;
  max_num_tx_ports_per_res_e_ max_num_tx_ports_per_res;
  uint8_t                     max_num_res        = 1;
  uint16_t                    total_num_tx_ports = 2;
  uint8_t                     param_lx           = 2;
  amplitude_scaling_type_e_   amplitude_scaling_type;
  uint8_t                     max_num_csi_rs_per_res_set = 1;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DummyE ::= SEQUENCE
struct dummy_e_s {
  struct max_num_tx_ports_per_res_opts {
    enum options { p4, p8, p12, p16, p24, p32, nulltype } value;
    typedef uint8_t number_type;

    std::string to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<max_num_tx_ports_per_res_opts> max_num_tx_ports_per_res_e_;
  struct amplitude_scaling_type_opts {
    enum options { wideband, wideband_and_subband, nulltype } value;

    std::string to_string() const;
  };
  typedef enumerated<amplitude_scaling_type_opts> amplitude_scaling_type_e_;

  // member variables
  max_num_tx_ports_per_res_e_ max_num_tx_ports_per_res;
  uint8_t                     max_num_res        = 1;
  uint16_t                    total_num_tx_ports = 2;
  uint8_t                     param_lx           = 2;
  amplitude_scaling_type_e_   amplitude_scaling_type;
  uint8_t                     max_num_csi_rs_per_res_set = 1;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// FreqSeparationClass ::= ENUMERATED
struct freq_separation_class_opts {
  enum options { c1, c2, c3, /*...*/ nulltype } value;
  typedef uint8_t number_type;

  std::string to_string() const;
  uint8_t     to_number() const;
};
typedef enumerated<freq_separation_class_opts, true> freq_separation_class_e;

// FeatureSetDownlink ::= SEQUENCE
struct feature_set_dl_s {
  using feature_set_list_per_dl_cc_l_ = bounded_array<uint16_t, 32>;
  struct scaling_factor_opts {
    enum options { f0p4, f0p75, f0p8, nulltype } value;

    std::string to_string() const;
  };
  typedef enumerated<scaling_factor_opts> scaling_factor_e_;
  struct pdcch_monitoring_any_occasions_opts {
    enum options { without_dci_gap, with_dci_gap, nulltype } value;

    std::string to_string() const;
  };
  typedef enumerated<pdcch_monitoring_any_occasions_opts> pdcch_monitoring_any_occasions_e_;
  struct time_dur_for_qcl_s_ {
    struct scs_minus60k_hz_opts {
      enum options { s7, s14, s28, nulltype } value;
      typedef uint8_t number_type;

      std::string to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<scs_minus60k_hz_opts> scs_minus60k_hz_e_;
    struct scs_minus120k_hz_opts {
      enum options { s14, s28, nulltype } value;
      typedef uint8_t number_type;

      std::string to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<scs_minus120k_hz_opts> scs_minus120k_hz_e_;

    // member variables
    bool                scs_minus60k_hz_present  = false;
    bool                scs_minus120k_hz_present = false;
    scs_minus60k_hz_e_  scs_minus60k_hz;
    scs_minus120k_hz_e_ scs_minus120k_hz;
  };
  struct pdsch_processing_type1_different_tb_per_slot_s_ {
    struct scs_minus15k_hz_opts {
      enum options { upto2, upto4, upto7, nulltype } value;
      typedef uint8_t number_type;

      std::string to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<scs_minus15k_hz_opts> scs_minus15k_hz_e_;
    struct scs_minus30k_hz_opts {
      enum options { upto2, upto4, upto7, nulltype } value;
      typedef uint8_t number_type;

      std::string to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<scs_minus30k_hz_opts> scs_minus30k_hz_e_;
    struct scs_minus60k_hz_opts {
      enum options { upto2, upto4, upto7, nulltype } value;
      typedef uint8_t number_type;

      std::string to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<scs_minus60k_hz_opts> scs_minus60k_hz_e_;
    struct scs_minus120k_hz_opts {
      enum options { upto2, upto4, upto7, nulltype } value;
      typedef uint8_t number_type;

      std::string to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<scs_minus120k_hz_opts> scs_minus120k_hz_e_;

    // member variables
    bool                scs_minus15k_hz_present  = false;
    bool                scs_minus30k_hz_present  = false;
    bool                scs_minus60k_hz_present  = false;
    bool                scs_minus120k_hz_present = false;
    scs_minus15k_hz_e_  scs_minus15k_hz;
    scs_minus30k_hz_e_  scs_minus30k_hz;
    scs_minus60k_hz_e_  scs_minus60k_hz;
    scs_minus120k_hz_e_ scs_minus120k_hz;
  };
  using dummy4_l_ = dyn_array<dummy_b_s>;
  using dummy5_l_ = dyn_array<dummy_c_s>;
  using dummy6_l_ = dyn_array<dummy_d_s>;
  using dummy7_l_ = dyn_array<dummy_e_s>;

  // member variables
  bool                                            intra_band_freq_separation_dl_present                = false;
  bool                                            scaling_factor_present                               = false;
  bool                                            cross_carrier_sched_other_scs_present                = false;
  bool                                            scell_without_ssb_present                            = false;
  bool                                            csi_rs_meas_scell_without_ssb_present                = false;
  bool                                            dummy1_present                                       = false;
  bool                                            type1_minus3_css_present                             = false;
  bool                                            pdcch_monitoring_any_occasions_present               = false;
  bool                                            dummy2_present                                       = false;
  bool                                            ue_specific_ul_dl_assign_present                     = false;
  bool                                            search_space_sharing_ca_dl_present                   = false;
  bool                                            time_dur_for_qcl_present                             = false;
  bool                                            pdsch_processing_type1_different_tb_per_slot_present = false;
  bool                                            dummy3_present                                       = false;
  bool                                            dummy4_present                                       = false;
  bool                                            dummy5_present                                       = false;
  bool                                            dummy6_present                                       = false;
  bool                                            dummy7_present                                       = false;
  feature_set_list_per_dl_cc_l_                   feature_set_list_per_dl_cc;
  freq_separation_class_e                         intra_band_freq_separation_dl;
  scaling_factor_e_                               scaling_factor;
  pdcch_monitoring_any_occasions_e_               pdcch_monitoring_any_occasions;
  time_dur_for_qcl_s_                             time_dur_for_qcl;
  pdsch_processing_type1_different_tb_per_slot_s_ pdsch_processing_type1_different_tb_per_slot;
  dummy_a_s                                       dummy3;
  dummy4_l_                                       dummy4;
  dummy5_l_                                       dummy5;
  dummy6_l_                                       dummy6;
  dummy7_l_                                       dummy7;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ProcessingParameters ::= SEQUENCE
struct processing_params_s {
  struct fallback_opts {
    enum options { sc, cap1_only, nulltype } value;
    typedef uint8_t number_type;

    std::string to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<fallback_opts> fallback_e_;
  struct different_tb_per_slot_s_ {
    bool    upto1_present = false;
    bool    upto2_present = false;
    bool    upto4_present = false;
    bool    upto7_present = false;
    uint8_t upto1         = 1;
    uint8_t upto2         = 1;
    uint8_t upto4         = 1;
    uint8_t upto7         = 1;
  };

  // member variables
  bool                     different_tb_per_slot_present = false;
  fallback_e_              fallback;
  different_tb_per_slot_s_ different_tb_per_slot;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// FeatureSetDownlink-v1540 ::= SEQUENCE
struct feature_set_dl_v1540_s {
  struct pdcch_monitoring_any_occasions_with_span_gap_s_ {
    struct scs_minus15k_hz_opts {
      enum options { set1, set2, set3, nulltype } value;
      typedef uint8_t number_type;

      std::string to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<scs_minus15k_hz_opts> scs_minus15k_hz_e_;
    struct scs_minus30k_hz_opts {
      enum options { set1, set2, set3, nulltype } value;
      typedef uint8_t number_type;

      std::string to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<scs_minus30k_hz_opts> scs_minus30k_hz_e_;
    struct scs_minus60k_hz_opts {
      enum options { set1, set2, set3, nulltype } value;
      typedef uint8_t number_type;

      std::string to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<scs_minus60k_hz_opts> scs_minus60k_hz_e_;
    struct scs_minus120k_hz_opts {
      enum options { set1, set2, set3, nulltype } value;
      typedef uint8_t number_type;

      std::string to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<scs_minus120k_hz_opts> scs_minus120k_hz_e_;

    // member variables
    bool                scs_minus15k_hz_present  = false;
    bool                scs_minus30k_hz_present  = false;
    bool                scs_minus60k_hz_present  = false;
    bool                scs_minus120k_hz_present = false;
    scs_minus15k_hz_e_  scs_minus15k_hz;
    scs_minus30k_hz_e_  scs_minus30k_hz;
    scs_minus60k_hz_e_  scs_minus60k_hz;
    scs_minus120k_hz_e_ scs_minus120k_hz;
  };
  struct pdsch_processing_type2_s_ {
    bool                scs_minus15k_hz_present = false;
    bool                scs_minus30k_hz_present = false;
    bool                scs_minus60k_hz_present = false;
    processing_params_s scs_minus15k_hz;
    processing_params_s scs_minus30k_hz;
    processing_params_s scs_minus60k_hz;
  };
  struct pdsch_processing_type2_limited_s_ {
    struct different_tb_per_slot_scs_minus30k_hz_opts {
      enum options { upto1, upto2, upto4, upto7, nulltype } value;
      typedef uint8_t number_type;

      std::string to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<different_tb_per_slot_scs_minus30k_hz_opts> different_tb_per_slot_scs_minus30k_hz_e_;

    // member variables
    different_tb_per_slot_scs_minus30k_hz_e_ different_tb_per_slot_scs_minus30k_hz;
  };

  // member variables
  bool                                            one_fl_dmrs_two_add_dmrs_dl_present                  = false;
  bool                                            add_dmrs_dl_alt_present                              = false;
  bool                                            two_fl_dmrs_two_add_dmrs_dl_present                  = false;
  bool                                            one_fl_dmrs_three_add_dmrs_dl_present                = false;
  bool                                            pdcch_monitoring_any_occasions_with_span_gap_present = false;
  bool                                            pdsch_separation_with_gap_present                    = false;
  bool                                            pdsch_processing_type2_present                       = false;
  bool                                            pdsch_processing_type2_limited_present               = false;
  bool                                            dl_mcs_table_alt_dynamic_ind_present                 = false;
  pdcch_monitoring_any_occasions_with_span_gap_s_ pdcch_monitoring_any_occasions_with_span_gap;
  pdsch_processing_type2_s_                       pdsch_processing_type2;
  pdsch_processing_type2_limited_s_               pdsch_processing_type2_limited;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ModulationOrder ::= ENUMERATED
struct mod_order_opts {
  enum options { bpsk_halfpi, bpsk, qpsk, qam16, qam64, qam256, nulltype } value;
  typedef float number_type;

  std::string to_string() const;
  float       to_number() const;
  std::string to_number_string() const;
};
typedef enumerated<mod_order_opts> mod_order_e;

// SupportedBandwidth ::= CHOICE
struct supported_bw_c {
  struct fr1_opts {
    enum options { mhz5, mhz10, mhz15, mhz20, mhz25, mhz30, mhz40, mhz50, mhz60, mhz80, mhz100, nulltype } value;
    typedef uint8_t number_type;

    std::string to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<fr1_opts> fr1_e_;
  struct fr2_opts {
    enum options { mhz50, mhz100, mhz200, mhz400, nulltype } value;
    typedef uint16_t number_type;

    std::string to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<fr2_opts> fr2_e_;
  struct types_opts {
    enum options { fr1, fr2, nulltype } value;
    typedef uint8_t number_type;

    std::string to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<types_opts> types;

  // choice methods
  supported_bw_c() = default;
  supported_bw_c(const supported_bw_c& other);
  supported_bw_c& operator=(const supported_bw_c& other);
  ~supported_bw_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  fr1_e_& fr1()
  {
    assert_choice_type("fr1", type_.to_string(), "SupportedBandwidth");
    return c.get<fr1_e_>();
  }
  fr2_e_& fr2()
  {
    assert_choice_type("fr2", type_.to_string(), "SupportedBandwidth");
    return c.get<fr2_e_>();
  }
  const fr1_e_& fr1() const
  {
    assert_choice_type("fr1", type_.to_string(), "SupportedBandwidth");
    return c.get<fr1_e_>();
  }
  const fr2_e_& fr2() const
  {
    assert_choice_type("fr2", type_.to_string(), "SupportedBandwidth");
    return c.get<fr2_e_>();
  }
  fr1_e_& set_fr1()
  {
    set(types::fr1);
    return c.get<fr1_e_>();
  }
  fr2_e_& set_fr2()
  {
    set(types::fr2);
    return c.get<fr2_e_>();
  }

private:
  types               type_;
  pod_choice_buffer_t c;

  void destroy_();
};

// FeatureSetDownlinkPerCC ::= SEQUENCE
struct feature_set_dl_per_cc_s {
  bool                 ch_bw_minus90mhz_present          = false;
  bool                 max_num_mimo_layers_pdsch_present = false;
  bool                 supported_mod_order_dl_present    = false;
  subcarrier_spacing_e supported_subcarrier_spacing_dl;
  supported_bw_c       supported_bw_dl;
  mimo_layers_dl_e     max_num_mimo_layers_pdsch;
  mod_order_e          supported_mod_order_dl;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DummyF ::= SEQUENCE
struct dummy_f_s {
  uint8_t max_num_periodic_csi_report_per_bwp        = 1;
  uint8_t max_num_aperiodic_csi_report_per_bwp       = 1;
  uint8_t max_num_semi_persistent_csi_report_per_bwp = 0;
  uint8_t simul_csi_reports_all_cc                   = 5;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DummyI ::= SEQUENCE
struct dummy_i_s {
  struct supported_srs_tx_port_switch_opts {
    enum options { t1r2, t1r4, t2r4, t1r4_t2r4, tr_equal, nulltype } value;

    std::string to_string() const;
  };
  typedef enumerated<supported_srs_tx_port_switch_opts> supported_srs_tx_port_switch_e_;

  // member variables
  bool                            tx_switch_impact_to_rx_present = false;
  supported_srs_tx_port_switch_e_ supported_srs_tx_port_switch;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// FeatureSetUplink ::= SEQUENCE
struct feature_set_ul_s {
  using feature_set_list_per_ul_cc_l_ = bounded_array<uint16_t, 32>;
  struct scaling_factor_opts {
    enum options { f0p4, f0p75, f0p8, nulltype } value;

    std::string to_string() const;
  };
  typedef enumerated<scaling_factor_opts> scaling_factor_e_;
  struct pusch_processing_type1_different_tb_per_slot_s_ {
    struct scs_minus15k_hz_opts {
      enum options { upto2, upto4, upto7, nulltype } value;
      typedef uint8_t number_type;

      std::string to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<scs_minus15k_hz_opts> scs_minus15k_hz_e_;
    struct scs_minus30k_hz_opts {
      enum options { upto2, upto4, upto7, nulltype } value;
      typedef uint8_t number_type;

      std::string to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<scs_minus30k_hz_opts> scs_minus30k_hz_e_;
    struct scs_minus60k_hz_opts {
      enum options { upto2, upto4, upto7, nulltype } value;
      typedef uint8_t number_type;

      std::string to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<scs_minus60k_hz_opts> scs_minus60k_hz_e_;
    struct scs_minus120k_hz_opts {
      enum options { upto2, upto4, upto7, nulltype } value;
      typedef uint8_t number_type;

      std::string to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<scs_minus120k_hz_opts> scs_minus120k_hz_e_;

    // member variables
    bool                scs_minus15k_hz_present  = false;
    bool                scs_minus30k_hz_present  = false;
    bool                scs_minus60k_hz_present  = false;
    bool                scs_minus120k_hz_present = false;
    scs_minus15k_hz_e_  scs_minus15k_hz;
    scs_minus30k_hz_e_  scs_minus30k_hz;
    scs_minus60k_hz_e_  scs_minus60k_hz;
    scs_minus120k_hz_e_ scs_minus120k_hz;
  };

  // member variables
  bool                                            scaling_factor_present                               = false;
  bool                                            cross_carrier_sched_other_scs_present                = false;
  bool                                            intra_band_freq_separation_ul_present                = false;
  bool                                            search_space_sharing_ca_ul_present                   = false;
  bool                                            dummy1_present                                       = false;
  bool                                            supported_srs_res_present                            = false;
  bool                                            two_pucch_group_present                              = false;
  bool                                            dynamic_switch_sul_present                           = false;
  bool                                            simul_tx_sul_non_sul_present                         = false;
  bool                                            pusch_processing_type1_different_tb_per_slot_present = false;
  bool                                            dummy2_present                                       = false;
  feature_set_list_per_ul_cc_l_                   feature_set_list_per_ul_cc;
  scaling_factor_e_                               scaling_factor;
  freq_separation_class_e                         intra_band_freq_separation_ul;
  dummy_i_s                                       dummy1;
  srs_res_s                                       supported_srs_res;
  pusch_processing_type1_different_tb_per_slot_s_ pusch_processing_type1_different_tb_per_slot;
  dummy_f_s                                       dummy2;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// FeatureSetUplink-v1540 ::= SEQUENCE
struct feature_set_ul_v1540_s {
  struct pusch_processing_type2_s_ {
    bool                scs_minus15k_hz_present = false;
    bool                scs_minus30k_hz_present = false;
    bool                scs_minus60k_hz_present = false;
    processing_params_s scs_minus15k_hz;
    processing_params_s scs_minus30k_hz;
    processing_params_s scs_minus60k_hz;
  };

  // member variables
  bool                      zero_slot_offset_aperiodic_srs_present = false;
  bool                      pa_phase_discontinuity_impacts_present = false;
  bool                      pusch_separation_with_gap_present      = false;
  bool                      pusch_processing_type2_present         = false;
  bool                      ul_mcs_table_alt_dynamic_ind_present   = false;
  pusch_processing_type2_s_ pusch_processing_type2;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// FeatureSetUplinkPerCC ::= SEQUENCE
struct feature_set_ul_per_cc_s {
  struct mimo_cb_pusch_s_ {
    bool             max_num_mimo_layers_cb_pusch_present = false;
    mimo_layers_ul_e max_num_mimo_layers_cb_pusch;
    uint8_t          max_num_srs_res_per_set = 1;
  };

  // member variables
  bool                 ch_bw_minus90mhz_present                 = false;
  bool                 mimo_cb_pusch_present                    = false;
  bool                 max_num_mimo_layers_non_cb_pusch_present = false;
  bool                 supported_mod_order_ul_present           = false;
  subcarrier_spacing_e supported_subcarrier_spacing_ul;
  supported_bw_c       supported_bw_ul;
  mimo_cb_pusch_s_     mimo_cb_pusch;
  mimo_layers_ul_e     max_num_mimo_layers_non_cb_pusch;
  mod_order_e          supported_mod_order_ul;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// FeatureSetUplinkPerCC-v1540 ::= SEQUENCE
struct feature_set_ul_per_cc_v1540_s {
  struct mimo_non_cb_pusch_s_ {
    uint8_t max_num_srs_res_per_set  = 1;
    uint8_t max_num_simul_srs_res_tx = 1;
  };

  // member variables
  bool                 mimo_non_cb_pusch_present = false;
  mimo_non_cb_pusch_s_ mimo_non_cb_pusch;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// FeatureSets ::= SEQUENCE
struct feature_sets_s {
  using feature_sets_dl_l_              = dyn_array<feature_set_dl_s>;
  using feature_sets_dl_per_cc_l_       = dyn_array<feature_set_dl_per_cc_s>;
  using feature_sets_ul_l_              = dyn_array<feature_set_ul_s>;
  using feature_sets_ul_per_cc_l_       = dyn_array<feature_set_ul_per_cc_s>;
  using feature_sets_dl_v1540_l_        = dyn_array<feature_set_dl_v1540_s>;
  using feature_sets_ul_v1540_l_        = dyn_array<feature_set_ul_v1540_s>;
  using feature_sets_ul_per_cc_v1540_l_ = dyn_array<feature_set_ul_per_cc_v1540_s>;

  // member variables
  bool                      ext                            = false;
  bool                      feature_sets_dl_present        = false;
  bool                      feature_sets_dl_per_cc_present = false;
  bool                      feature_sets_ul_present        = false;
  bool                      feature_sets_ul_per_cc_present = false;
  feature_sets_dl_l_        feature_sets_dl;
  feature_sets_dl_per_cc_l_ feature_sets_dl_per_cc;
  feature_sets_ul_l_        feature_sets_ul;
  feature_sets_ul_per_cc_l_ feature_sets_ul_per_cc;
  // ...
  // group 0
  copy_ptr<feature_sets_dl_v1540_l_>        feature_sets_dl_v1540;
  copy_ptr<feature_sets_ul_v1540_l_>        feature_sets_ul_v1540;
  copy_ptr<feature_sets_ul_per_cc_v1540_l_> feature_sets_ul_per_cc_v1540;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// AggregatedBandwidth ::= ENUMERATED
struct aggregated_bw_opts {
  enum options {
    mhz50,
    mhz100,
    mhz150,
    mhz200,
    mhz250,
    mhz300,
    mhz350,
    mhz400,
    mhz450,
    mhz500,
    mhz550,
    mhz600,
    mhz650,
    mhz700,
    mhz750,
    mhz800,
    nulltype
  } value;
  typedef uint16_t number_type;

  std::string to_string() const;
  uint16_t    to_number() const;
};
typedef enumerated<aggregated_bw_opts> aggregated_bw_e;

// FreqBandInformationEUTRA ::= SEQUENCE
struct freq_band_info_eutra_s {
  bool                ca_bw_class_dl_eutra_present = false;
  bool                ca_bw_class_ul_eutra_present = false;
  uint16_t            band_eutra                   = 1;
  ca_bw_class_eutra_e ca_bw_class_dl_eutra;
  ca_bw_class_eutra_e ca_bw_class_ul_eutra;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// FreqBandInformationNR ::= SEQUENCE
struct freq_band_info_nr_s {
  bool            max_bw_requested_dl_present       = false;
  bool            max_bw_requested_ul_present       = false;
  bool            max_carriers_requested_dl_present = false;
  bool            max_carriers_requested_ul_present = false;
  uint16_t        band_nr                           = 1;
  aggregated_bw_e max_bw_requested_dl;
  aggregated_bw_e max_bw_requested_ul;
  uint8_t         max_carriers_requested_dl = 1;
  uint8_t         max_carriers_requested_ul = 1;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// FreqBandInformation ::= CHOICE
struct freq_band_info_c {
  struct types_opts {
    enum options { band_info_eutra, band_info_nr, nulltype } value;

    std::string to_string() const;
  };
  typedef enumerated<types_opts> types;

  // choice methods
  freq_band_info_c() = default;
  freq_band_info_c(const freq_band_info_c& other);
  freq_band_info_c& operator=(const freq_band_info_c& other);
  ~freq_band_info_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  freq_band_info_eutra_s& band_info_eutra()
  {
    assert_choice_type("bandInformationEUTRA", type_.to_string(), "FreqBandInformation");
    return c.get<freq_band_info_eutra_s>();
  }
  freq_band_info_nr_s& band_info_nr()
  {
    assert_choice_type("bandInformationNR", type_.to_string(), "FreqBandInformation");
    return c.get<freq_band_info_nr_s>();
  }
  const freq_band_info_eutra_s& band_info_eutra() const
  {
    assert_choice_type("bandInformationEUTRA", type_.to_string(), "FreqBandInformation");
    return c.get<freq_band_info_eutra_s>();
  }
  const freq_band_info_nr_s& band_info_nr() const
  {
    assert_choice_type("bandInformationNR", type_.to_string(), "FreqBandInformation");
    return c.get<freq_band_info_nr_s>();
  }
  freq_band_info_eutra_s& set_band_info_eutra()
  {
    set(types::band_info_eutra);
    return c.get<freq_band_info_eutra_s>();
  }
  freq_band_info_nr_s& set_band_info_nr()
  {
    set(types::band_info_nr);
    return c.get<freq_band_info_nr_s>();
  }

private:
  types                                                        type_;
  choice_buffer_t<freq_band_info_eutra_s, freq_band_info_nr_s> c;

  void destroy_();
};

// FreqBandList ::= SEQUENCE (SIZE (1..1280)) OF FreqBandInformation
using freq_band_list_l = dyn_array<freq_band_info_c>;

// IMS-ParametersCommon ::= SEQUENCE
struct ims_params_common_s {
  bool ext                                = false;
  bool voice_over_eutra_minus5_gc_present = false;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// IMS-ParametersFRX-Diff ::= SEQUENCE
struct ims_params_frx_diff_s {
  bool ext                   = false;
  bool voice_over_nr_present = false;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// IMS-Parameters ::= SEQUENCE
struct ims_params_s {
  bool                  ext                         = false;
  bool                  ims_params_common_present   = false;
  bool                  ims_params_frx_diff_present = false;
  ims_params_common_s   ims_params_common;
  ims_params_frx_diff_s ims_params_frx_diff;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// InterRAT-Parameters ::= SEQUENCE
struct inter_rat_params_s {
  bool           ext           = false;
  bool           eutra_present = false;
  eutra_params_s eutra;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MAC-ParametersCommon ::= SEQUENCE
struct mac_params_common_s {
  bool ext                           = false;
  bool lcp_restrict_present          = false;
  bool dummy_present                 = false;
  bool lch_to_scell_restrict_present = false;
  // ...
  // group 0
  bool recommended_bit_rate_present       = false;
  bool recommended_bit_rate_query_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MAC-ParametersXDD-Diff ::= SEQUENCE
struct mac_params_xdd_diff_s {
  bool ext                             = false;
  bool skip_ul_tx_dynamic_present      = false;
  bool lc_ch_sr_delay_timer_present    = false;
  bool long_drx_cycle_present          = false;
  bool short_drx_cycle_present         = false;
  bool multiple_sr_cfgs_present        = false;
  bool multiple_cfgured_grants_present = false;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MAC-Parameters ::= SEQUENCE
struct mac_params_s {
  bool                  mac_params_common_present   = false;
  bool                  mac_params_xdd_diff_present = false;
  mac_params_common_s   mac_params_common;
  mac_params_xdd_diff_s mac_params_xdd_diff;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasAndMobParametersCommon ::= SEQUENCE
struct meas_and_mob_params_common_s {
  struct max_num_csi_rs_rrm_rs_sinr_opts {
    enum options { n4, n8, n16, n32, n64, n96, nulltype } value;
    typedef uint8_t number_type;

    std::string to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<max_num_csi_rs_rrm_rs_sinr_opts> max_num_csi_rs_rrm_rs_sinr_e_;

  // member variables
  bool                ext                           = false;
  bool                supported_gap_pattern_present = false;
  bool                ssb_rlm_present               = false;
  bool                ssb_and_csi_rs_rlm_present    = false;
  fixed_bitstring<22> supported_gap_pattern;
  // ...
  // group 0
  bool event_b_meas_and_report_present = false;
  bool ho_fdd_tdd_present              = false;
  bool eutra_cgi_report_present        = false;
  bool nr_cgi_report_present           = false;
  // group 1
  bool                          independent_gap_cfg_present            = false;
  bool                          periodic_eutra_meas_and_report_present = false;
  bool                          ho_fr1_fr2_present                     = false;
  bool                          max_num_csi_rs_rrm_rs_sinr_present     = false;
  max_num_csi_rs_rrm_rs_sinr_e_ max_num_csi_rs_rrm_rs_sinr;
  // group 2
  bool nr_cgi_report_endc_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasAndMobParametersFRX-Diff ::= SEQUENCE
struct meas_and_mob_params_frx_diff_s {
  struct max_num_res_csi_rs_rlm_opts {
    enum options { n2, n4, n6, n8, nulltype } value;
    typedef uint8_t number_type;

    std::string to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<max_num_res_csi_rs_rlm_opts> max_num_res_csi_rs_rlm_e_;

  // member variables
  bool ext                                        = false;
  bool ss_sinr_meas_present                       = false;
  bool csi_rsrp_and_rsrq_meas_with_ssb_present    = false;
  bool csi_rsrp_and_rsrq_meas_without_ssb_present = false;
  bool csi_sinr_meas_present                      = false;
  bool csi_rs_rlm_present                         = false;
  // ...
  // group 0
  bool ho_inter_f_present = false;
  bool ho_lte_present     = false;
  bool ho_e_lte_present   = false;
  // group 1
  bool                      max_num_res_csi_rs_rlm_present = false;
  max_num_res_csi_rs_rlm_e_ max_num_res_csi_rs_rlm;
  // group 2
  bool simul_rx_data_ssb_diff_numerology_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasAndMobParametersXDD-Diff ::= SEQUENCE
struct meas_and_mob_params_xdd_diff_s {
  bool ext                                       = false;
  bool intra_and_inter_f_meas_and_report_present = false;
  bool event_a_meas_and_report_present           = false;
  // ...
  // group 0
  bool ho_inter_f_present = false;
  bool ho_lte_present     = false;
  bool ho_e_lte_present   = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasAndMobParameters ::= SEQUENCE
struct meas_and_mob_params_s {
  bool                           meas_and_mob_params_common_present   = false;
  bool                           meas_and_mob_params_xdd_diff_present = false;
  bool                           meas_and_mob_params_frx_diff_present = false;
  meas_and_mob_params_common_s   meas_and_mob_params_common;
  meas_and_mob_params_xdd_diff_s meas_and_mob_params_xdd_diff;
  meas_and_mob_params_frx_diff_s meas_and_mob_params_frx_diff;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasAndMobParametersMRDC-Common ::= SEQUENCE
struct meas_and_mob_params_mrdc_common_s {
  bool independent_gap_cfg_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasAndMobParametersMRDC-FRX-Diff ::= SEQUENCE
struct meas_and_mob_params_mrdc_frx_diff_s {
  bool simul_rx_data_ssb_diff_numerology_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasAndMobParametersMRDC-XDD-Diff ::= SEQUENCE
struct meas_and_mob_params_mrdc_xdd_diff_s {
  bool sftd_meas_ps_cell_present = false;
  bool sftd_meas_nr_cell_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasAndMobParametersMRDC ::= SEQUENCE
struct meas_and_mob_params_mrdc_s {
  bool                                meas_and_mob_params_mrdc_common_present   = false;
  bool                                meas_and_mob_params_mrdc_xdd_diff_present = false;
  bool                                meas_and_mob_params_mrdc_frx_diff_present = false;
  meas_and_mob_params_mrdc_common_s   meas_and_mob_params_mrdc_common;
  meas_and_mob_params_mrdc_xdd_diff_s meas_and_mob_params_mrdc_xdd_diff;
  meas_and_mob_params_mrdc_frx_diff_s meas_and_mob_params_mrdc_frx_diff;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasResult2NR ::= SEQUENCE
struct meas_result2_nr_s {
  bool                  ext                                    = false;
  bool                  ssb_freq_present                       = false;
  bool                  ref_freq_csi_rs_present                = false;
  bool                  meas_result_serving_cell_present       = false;
  bool                  meas_result_neigh_cell_list_nr_present = false;
  uint32_t              ssb_freq                               = 0;
  uint32_t              ref_freq_csi_rs                        = 0;
  meas_result_nr_s      meas_result_serving_cell;
  meas_result_list_nr_l meas_result_neigh_cell_list_nr;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasResultCellSFTD ::= SEQUENCE
struct meas_result_cell_sftd_s {
  bool     rsrp_result_present          = false;
  uint16_t pci                          = 0;
  uint16_t sfn_offset_result            = 0;
  int16_t  frame_boundary_offset_result = -30720;
  uint8_t  rsrp_result                  = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasResultCellListSFTD ::= SEQUENCE (SIZE (1..3)) OF MeasResultCellSFTD
using meas_result_cell_list_sftd_l = dyn_array<meas_result_cell_sftd_s>;

// MeasResultList2NR ::= SEQUENCE (SIZE (1..8)) OF MeasResult2NR
using meas_result_list2_nr_l = dyn_array<meas_result2_nr_s>;

// MeasResultSCG-Failure ::= SEQUENCE
struct meas_result_scg_fail_s {
  bool                   ext = false;
  meas_result_list2_nr_l meas_result_per_mo_list;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// Phy-ParametersCommon ::= SEQUENCE
struct phy_params_common_s {
  struct bwp_switching_delay_opts {
    enum options { type1, type2, nulltype } value;
    typedef uint8_t number_type;

    std::string to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<bwp_switching_delay_opts> bwp_switching_delay_e_;

  // member variables
  bool                   ext                                          = false;
  bool                   csi_rs_cfra_for_ho_present                   = false;
  bool                   dynamic_prb_bundling_dl_present              = false;
  bool                   sp_csi_report_pucch_present                  = false;
  bool                   sp_csi_report_pusch_present                  = false;
  bool                   nzp_csi_rs_intef_mgmt_present                = false;
  bool                   type2_sp_csi_feedback_long_pucch_present     = false;
  bool                   precoder_granularity_coreset_present         = false;
  bool                   dynamic_harq_ack_codebook_present            = false;
  bool                   semi_static_harq_ack_codebook_present        = false;
  bool                   spatial_bundling_harq_ack_present            = false;
  bool                   dynamic_beta_offset_ind_harq_ack_csi_present = false;
  bool                   pucch_repeat_f1_minus3_minus4_present        = false;
  bool                   ra_type0_pusch_present                       = false;
  bool                   dynamic_switch_ra_type0_minus1_pdsch_present = false;
  bool                   dynamic_switch_ra_type0_minus1_pusch_present = false;
  bool                   pdsch_map_type_a_present                     = false;
  bool                   pdsch_map_type_b_present                     = false;
  bool                   interleaving_vrb_to_prb_pdsch_present        = false;
  bool                   inter_slot_freq_hop_pusch_present            = false;
  bool                   type1_pusch_repeat_multi_slots_present       = false;
  bool                   type2_pusch_repeat_multi_slots_present       = false;
  bool                   pusch_repeat_multi_slots_present             = false;
  bool                   pdsch_repeat_multi_slots_present             = false;
  bool                   dl_sps_present                               = false;
  bool                   cfgured_ul_grant_type1_present               = false;
  bool                   cfgured_ul_grant_type2_present               = false;
  bool                   pre_empt_ind_dl_present                      = false;
  bool                   cbg_trans_ind_dl_present                     = false;
  bool                   cbg_trans_ind_ul_present                     = false;
  bool                   cbg_flush_ind_dl_present                     = false;
  bool                   dynamic_harq_ack_code_b_cbg_retx_dl_present  = false;
  bool                   rate_matching_resrc_set_semi_static_present  = false;
  bool                   rate_matching_resrc_set_dynamic_present      = false;
  bool                   bwp_switching_delay_present                  = false;
  bwp_switching_delay_e_ bwp_switching_delay;
  // ...
  // group 0
  bool dummy_present = false;
  // group 1
  bool max_num_search_spaces_present                = false;
  bool rate_matching_ctrl_resrc_set_dynamic_present = false;
  bool max_layers_mimo_ind_present                  = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// Phy-ParametersFR1 ::= SEQUENCE
struct phy_params_fr1_s {
  struct pdsch_re_map_fr1_per_symbol_opts {
    enum options { n10, n20, nulltype } value;
    typedef uint8_t number_type;

    std::string to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<pdsch_re_map_fr1_per_symbol_opts> pdsch_re_map_fr1_per_symbol_e_;
  struct pdsch_re_map_fr1_per_slot_opts {
    enum options {
      n16,
      n32,
      n48,
      n64,
      n80,
      n96,
      n112,
      n128,
      n144,
      n160,
      n176,
      n192,
      n208,
      n224,
      n240,
      n256,
      nulltype
    } value;
    typedef uint16_t number_type;

    std::string to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<pdsch_re_map_fr1_per_slot_opts> pdsch_re_map_fr1_per_slot_e_;

  // member variables
  bool                           ext                                      = false;
  bool                           pdcch_monitoring_single_occasion_present = false;
  bool                           scs_minus60k_hz_present                  = false;
  bool                           pdsch_minus256_qam_fr1_present           = false;
  bool                           pdsch_re_map_fr1_per_symbol_present      = false;
  pdsch_re_map_fr1_per_symbol_e_ pdsch_re_map_fr1_per_symbol;
  // ...
  // group 0
  bool                         pdsch_re_map_fr1_per_slot_present = false;
  pdsch_re_map_fr1_per_slot_e_ pdsch_re_map_fr1_per_slot;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// Phy-ParametersFR2 ::= SEQUENCE
struct phy_params_fr2_s {
  struct pdsch_re_map_fr2_per_symbol_opts {
    enum options { n6, n20, nulltype } value;
    typedef uint8_t number_type;

    std::string to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<pdsch_re_map_fr2_per_symbol_opts> pdsch_re_map_fr2_per_symbol_e_;
  struct pdsch_re_map_fr2_per_slot_opts {
    enum options {
      n16,
      n32,
      n48,
      n64,
      n80,
      n96,
      n112,
      n128,
      n144,
      n160,
      n176,
      n192,
      n208,
      n224,
      n240,
      n256,
      nulltype
    } value;
    typedef uint16_t number_type;

    std::string to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<pdsch_re_map_fr2_per_slot_opts> pdsch_re_map_fr2_per_slot_e_;

  // member variables
  bool                           ext                                 = false;
  bool                           dummy_present                       = false;
  bool                           pdsch_re_map_fr2_per_symbol_present = false;
  pdsch_re_map_fr2_per_symbol_e_ pdsch_re_map_fr2_per_symbol;
  // ...
  // group 0
  bool                         pcell_fr2_present                 = false;
  bool                         pdsch_re_map_fr2_per_slot_present = false;
  pdsch_re_map_fr2_per_slot_e_ pdsch_re_map_fr2_per_slot;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// Phy-ParametersFRX-Diff ::= SEQUENCE
struct phy_params_frx_diff_s {
  struct supported_dmrs_type_dl_opts {
    enum options { type1, type1_and2, nulltype } value;

    std::string to_string() const;
  };
  typedef enumerated<supported_dmrs_type_dl_opts> supported_dmrs_type_dl_e_;
  struct supported_dmrs_type_ul_opts {
    enum options { type1, type1_and2, nulltype } value;

    std::string to_string() const;
  };
  typedef enumerated<supported_dmrs_type_ul_opts> supported_dmrs_type_ul_e_;
  struct mux_sr_harq_ack_csi_pucch_once_per_slot_s_ {
    bool same_symbol_present = false;
    bool diff_symbol_present = false;
  };

  // member variables
  bool                      ext                                              = false;
  bool                      dynamic_sfi_present                              = false;
  bool                      dummy1_present                                   = false;
  bool                      two_fl_dmrs_present                              = false;
  bool                      dummy2_present                                   = false;
  bool                      dummy3_present                                   = false;
  bool                      supported_dmrs_type_dl_present                   = false;
  bool                      supported_dmrs_type_ul_present                   = false;
  bool                      semi_open_loop_csi_present                       = false;
  bool                      csi_report_without_pmi_present                   = false;
  bool                      csi_report_without_cqi_present                   = false;
  bool                      one_ports_ptrs_present                           = false;
  bool                      two_pucch_f0_minus2_consec_symbols_present       = false;
  bool                      pucch_f2_with_fh_present                         = false;
  bool                      pucch_f3_with_fh_present                         = false;
  bool                      pucch_f4_with_fh_present                         = false;
  bool                      freq_hop_pucch_f0_minus2_present                 = false;
  bool                      freq_hop_pucch_f1_minus3_minus4_present          = false;
  bool                      mux_sr_harq_ack_csi_pucch_multi_per_slot_present = false;
  bool                      uci_code_block_segmentation_present              = false;
  bool                      one_pucch_long_and_short_format_present          = false;
  bool                      two_pucch_any_others_in_slot_present             = false;
  bool                      intra_slot_freq_hop_pusch_present                = false;
  bool                      pusch_lbrm_present                               = false;
  bool                      pdcch_blind_detection_ca_present                 = false;
  bool                      tpc_pusch_rnti_present                           = false;
  bool                      tpc_pucch_rnti_present                           = false;
  bool                      tpc_srs_rnti_present                             = false;
  bool                      absolute_tpc_cmd_present                         = false;
  bool                      two_different_tpc_loop_pusch_present             = false;
  bool                      two_different_tpc_loop_pucch_present             = false;
  bool                      pusch_half_pi_bpsk_present                       = false;
  bool                      pucch_f3_minus4_half_pi_bpsk_present             = false;
  bool                      almost_contiguous_cp_ofdm_ul_present             = false;
  bool                      sp_csi_rs_present                                = false;
  bool                      sp_csi_im_present                                = false;
  bool                      tdd_multi_dl_ul_switch_per_slot_present          = false;
  bool                      multiple_coreset_present                         = false;
  fixed_bitstring<2>        dummy1;
  fixed_bitstring<2>        two_fl_dmrs;
  fixed_bitstring<2>        dummy2;
  fixed_bitstring<2>        dummy3;
  supported_dmrs_type_dl_e_ supported_dmrs_type_dl;
  supported_dmrs_type_ul_e_ supported_dmrs_type_ul;
  fixed_bitstring<2>        one_ports_ptrs;
  uint8_t                   pdcch_blind_detection_ca = 4;
  // ...
  // group 0
  bool                                                 mux_sr_harq_ack_pucch_present              = false;
  bool                                                 mux_multiple_group_ctrl_ch_overlap_present = false;
  bool                                                 dl_sched_offset_pdsch_type_a_present       = false;
  bool                                                 dl_sched_offset_pdsch_type_b_present       = false;
  bool                                                 ul_sched_offset_present                    = false;
  bool                                                 dl_minus64_qam_mcs_table_alt_present       = false;
  bool                                                 ul_minus64_qam_mcs_table_alt_present       = false;
  bool                                                 cqi_table_alt_present                      = false;
  bool                                                 one_fl_dmrs_two_add_dmrs_ul_present        = false;
  bool                                                 two_fl_dmrs_two_add_dmrs_ul_present        = false;
  bool                                                 one_fl_dmrs_three_add_dmrs_ul_present      = false;
  copy_ptr<csi_rs_im_reception_for_feedback_s>         csi_rs_im_reception_for_feedback;
  copy_ptr<csi_rs_proc_framework_for_srs_s>            csi_rs_proc_framework_for_srs;
  copy_ptr<csi_report_framework_s>                     csi_report_framework;
  copy_ptr<mux_sr_harq_ack_csi_pucch_once_per_slot_s_> mux_sr_harq_ack_csi_pucch_once_per_slot;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// Phy-ParametersXDD-Diff ::= SEQUENCE
struct phy_params_xdd_diff_s {
  bool ext                                        = false;
  bool dynamic_sfi_present                        = false;
  bool two_pucch_f0_minus2_consec_symbols_present = false;
  bool two_different_tpc_loop_pusch_present       = false;
  bool two_different_tpc_loop_pucch_present       = false;
  // ...
  // group 0
  bool dl_sched_offset_pdsch_type_a_present = false;
  bool dl_sched_offset_pdsch_type_b_present = false;
  bool ul_sched_offset_present              = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// Phy-Parameters ::= SEQUENCE
struct phy_params_s {
  bool                  phy_params_common_present   = false;
  bool                  phy_params_xdd_diff_present = false;
  bool                  phy_params_frx_diff_present = false;
  bool                  phy_params_fr1_present      = false;
  bool                  phy_params_fr2_present      = false;
  phy_params_common_s   phy_params_common;
  phy_params_xdd_diff_s phy_params_xdd_diff;
  phy_params_frx_diff_s phy_params_frx_diff;
  phy_params_fr1_s      phy_params_fr1;
  phy_params_fr2_s      phy_params_fr2;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// NAICS-Capability-Entry ::= SEQUENCE
struct naics_cap_entry_s {
  struct nof_aggregated_prb_opts {
    enum options {
      n50,
      n75,
      n100,
      n125,
      n150,
      n175,
      n200,
      n225,
      n250,
      n275,
      n300,
      n350,
      n400,
      n450,
      n500,
      spare,
      nulltype
    } value;
    typedef uint16_t number_type;

    std::string to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<nof_aggregated_prb_opts> nof_aggregated_prb_e_;

  // member variables
  bool                  ext                  = false;
  uint8_t               nof_naics_capable_cc = 1;
  nof_aggregated_prb_e_ nof_aggregated_prb;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// Phy-ParametersMRDC ::= SEQUENCE
struct phy_params_mrdc_s {
  using naics_cap_list_l_ = dyn_array<naics_cap_entry_s>;

  // member variables
  bool              ext                    = false;
  bool              naics_cap_list_present = false;
  naics_cap_list_l_ naics_cap_list;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RF-Parameters ::= SEQUENCE
struct rf_params_s {
  using supported_band_list_nr_l_ = dyn_array<band_nr_s>;

  // member variables
  bool                      ext                                     = false;
  bool                      supported_band_combination_list_present = false;
  bool                      applied_freq_band_list_filt_present     = false;
  supported_band_list_nr_l_ supported_band_list_nr;
  band_combination_list_l   supported_band_combination_list;
  freq_band_list_l          applied_freq_band_list_filt;
  // ...
  // group 0
  bool                                    srs_switching_time_requested_present = false;
  copy_ptr<band_combination_list_v1540_l> supported_band_combination_list_v1540;
  // group 1
  copy_ptr<band_combination_list_v1550_l> supported_band_combination_list_v1550;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RF-ParametersMRDC ::= SEQUENCE
struct rf_params_mrdc_s {
  bool                    ext                                     = false;
  bool                    supported_band_combination_list_present = false;
  bool                    applied_freq_band_list_filt_present     = false;
  band_combination_list_l supported_band_combination_list;
  freq_band_list_l        applied_freq_band_list_filt;
  // ...
  // group 0
  bool                                    srs_switching_time_requested_present = false;
  copy_ptr<band_combination_list_v1540_l> supported_band_combination_list_v1540;
  // group 1
  copy_ptr<band_combination_list_v1550_l> supported_band_combination_list_v1550;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UE-CapabilityRequestFilterNR-v1540 ::= SEQUENCE
struct ue_cap_request_filt_nr_v1540_s {
  bool srs_switching_time_request_present = false;
  bool non_crit_ext_present               = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UE-CapabilityRequestFilterNR ::= SEQUENCE
struct ue_cap_request_filt_nr_s {
  bool                           freq_band_list_filt_present = false;
  bool                           non_crit_ext_present        = false;
  freq_band_list_l               freq_band_list_filt;
  ue_cap_request_filt_nr_v1540_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// GeneralParametersMRDC-XDD-Diff ::= SEQUENCE
struct general_params_mrdc_xdd_diff_s {
  bool ext                                    = false;
  bool split_srb_with_one_ul_path_present     = false;
  bool split_drb_with_ul_both_mcg_scg_present = false;
  bool srb3_present                           = false;
  bool v2x_eutra_v1530_present                = false;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDCP-ParametersMRDC ::= SEQUENCE
struct pdcp_params_mrdc_s {
  bool pdcp_dupl_split_srb_present = false;
  bool pdcp_dupl_split_drb_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UE-MRDC-CapabilityAddFRX-Mode ::= SEQUENCE
struct ue_mrdc_cap_add_frx_mode_s {
  meas_and_mob_params_mrdc_frx_diff_s meas_and_mob_params_mrdc_frx_diff;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UE-MRDC-CapabilityAddXDD-Mode ::= SEQUENCE
struct ue_mrdc_cap_add_xdd_mode_s {
  bool                                meas_and_mob_params_mrdc_xdd_diff_present = false;
  bool                                general_params_mrdc_xdd_diff_present      = false;
  meas_and_mob_params_mrdc_xdd_diff_s meas_and_mob_params_mrdc_xdd_diff;
  general_params_mrdc_xdd_diff_s      general_params_mrdc_xdd_diff;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UE-MRDC-Capability ::= SEQUENCE
struct ue_mrdc_cap_s {
  using feature_set_combinations_l_ = dyn_array<feature_set_combination_l>;

  // member variables
  bool                           meas_and_mob_params_mrdc_present = false;
  bool                           phy_params_mrdc_v1530_present    = false;
  bool                           general_params_mrdc_present      = false;
  bool                           fdd_add_ue_mrdc_cap_present      = false;
  bool                           tdd_add_ue_mrdc_cap_present      = false;
  bool                           fr1_add_ue_mrdc_cap_present      = false;
  bool                           fr2_add_ue_mrdc_cap_present      = false;
  bool                           feature_set_combinations_present = false;
  bool                           pdcp_params_mrdc_v1530_present   = false;
  bool                           late_non_crit_ext_present        = false;
  bool                           non_crit_ext_present             = false;
  meas_and_mob_params_mrdc_s     meas_and_mob_params_mrdc;
  phy_params_mrdc_s              phy_params_mrdc_v1530;
  rf_params_mrdc_s               rf_params_mrdc;
  general_params_mrdc_xdd_diff_s general_params_mrdc;
  ue_mrdc_cap_add_xdd_mode_s     fdd_add_ue_mrdc_cap;
  ue_mrdc_cap_add_xdd_mode_s     tdd_add_ue_mrdc_cap;
  ue_mrdc_cap_add_frx_mode_s     fr1_add_ue_mrdc_cap;
  ue_mrdc_cap_add_frx_mode_s     fr2_add_ue_mrdc_cap;
  feature_set_combinations_l_    feature_set_combinations;
  pdcp_params_mrdc_s             pdcp_params_mrdc_v1530;
  dyn_octstring                  late_non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SDAP-Parameters ::= SEQUENCE
struct sdap_params_s {
  bool ext                       = false;
  bool as_reflective_qos_present = false;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UE-NR-Capability-v1550 ::= SEQUENCE
struct ue_nr_cap_v1550_s {
  bool reduced_cp_latency_present = false;
  bool non_crit_ext_present       = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UE-NR-CapabilityAddFRX-Mode ::= SEQUENCE
struct ue_nr_cap_add_frx_mode_s {
  bool                           phy_params_frx_diff_present          = false;
  bool                           meas_and_mob_params_frx_diff_present = false;
  phy_params_frx_diff_s          phy_params_frx_diff;
  meas_and_mob_params_frx_diff_s meas_and_mob_params_frx_diff;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UE-NR-CapabilityAddFRX-Mode-v1540 ::= SEQUENCE
struct ue_nr_cap_add_frx_mode_v1540_s {
  bool                  ims_params_frx_diff_present = false;
  ims_params_frx_diff_s ims_params_frx_diff;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UE-NR-Capability-v1540 ::= SEQUENCE
struct ue_nr_cap_v1540_s {
  bool                           sdap_params_present             = false;
  bool                           overheat_ind_present            = false;
  bool                           ims_params_present              = false;
  bool                           fr1_add_ue_nr_cap_v1540_present = false;
  bool                           fr2_add_ue_nr_cap_v1540_present = false;
  bool                           fr1_fr2_add_ue_nr_cap_present   = false;
  bool                           non_crit_ext_present            = false;
  sdap_params_s                  sdap_params;
  ims_params_s                   ims_params;
  ue_nr_cap_add_frx_mode_v1540_s fr1_add_ue_nr_cap_v1540;
  ue_nr_cap_add_frx_mode_v1540_s fr2_add_ue_nr_cap_v1540;
  ue_nr_cap_add_frx_mode_s       fr1_fr2_add_ue_nr_cap;
  ue_nr_cap_v1550_s              non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UE-NR-CapabilityAddXDD-Mode-v1530 ::= SEQUENCE
struct ue_nr_cap_add_xdd_mode_v1530_s {
  eutra_params_xdd_diff_s eutra_params_xdd_diff;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// AccessStratumRelease ::= ENUMERATED
struct access_stratum_release_opts {
  enum options { rel15, spare7, spare6, spare5, spare4, spare3, spare2, spare1, /*...*/ nulltype } value;
  typedef uint8_t number_type;

  std::string to_string() const;
  uint8_t     to_number() const;
};
typedef enumerated<access_stratum_release_opts, true> access_stratum_release_e;

// PDCP-Parameters ::= SEQUENCE
struct pdcp_params_s {
  struct supported_rohc_profiles_s_ {
    bool profile0x0000 = false;
    bool profile0x0001 = false;
    bool profile0x0002 = false;
    bool profile0x0003 = false;
    bool profile0x0004 = false;
    bool profile0x0006 = false;
    bool profile0x0101 = false;
    bool profile0x0102 = false;
    bool profile0x0103 = false;
    bool profile0x0104 = false;
  };
  struct max_num_rohc_context_sessions_opts {
    enum options {
      cs2,
      cs4,
      cs8,
      cs12,
      cs16,
      cs24,
      cs32,
      cs48,
      cs64,
      cs128,
      cs256,
      cs512,
      cs1024,
      cs16384,
      spare2,
      spare1,
      nulltype
    } value;
    typedef uint16_t number_type;

    std::string to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<max_num_rohc_context_sessions_opts> max_num_rohc_context_sessions_e_;

  // member variables
  bool                             ext                              = false;
  bool                             ul_only_rohc_profiles_present    = false;
  bool                             continue_rohc_context_present    = false;
  bool                             out_of_order_delivery_present    = false;
  bool                             short_sn_present                 = false;
  bool                             pdcp_dupl_srb_present            = false;
  bool                             pdcp_dupl_mcg_or_scg_drb_present = false;
  supported_rohc_profiles_s_       supported_rohc_profiles;
  max_num_rohc_context_sessions_e_ max_num_rohc_context_sessions;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RLC-Parameters ::= SEQUENCE
struct rlc_params_s {
  bool ext                      = false;
  bool am_with_short_sn_present = false;
  bool um_with_short_sn_present = false;
  bool um_with_long_sn_present  = false;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UE-NR-Capability-v1530 ::= SEQUENCE
struct ue_nr_cap_v1530_s {
  bool                           fdd_add_ue_nr_cap_v1530_present = false;
  bool                           tdd_add_ue_nr_cap_v1530_present = false;
  bool                           dummy_present                   = false;
  bool                           inter_rat_params_present        = false;
  bool                           inactive_state_present          = false;
  bool                           delay_budget_report_present     = false;
  bool                           non_crit_ext_present            = false;
  ue_nr_cap_add_xdd_mode_v1530_s fdd_add_ue_nr_cap_v1530;
  ue_nr_cap_add_xdd_mode_v1530_s tdd_add_ue_nr_cap_v1530;
  inter_rat_params_s             inter_rat_params;
  ue_nr_cap_v1540_s              non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UE-NR-CapabilityAddXDD-Mode ::= SEQUENCE
struct ue_nr_cap_add_xdd_mode_s {
  bool                           phy_params_xdd_diff_present          = false;
  bool                           mac_params_xdd_diff_present          = false;
  bool                           meas_and_mob_params_xdd_diff_present = false;
  phy_params_xdd_diff_s          phy_params_xdd_diff;
  mac_params_xdd_diff_s          mac_params_xdd_diff;
  meas_and_mob_params_xdd_diff_s meas_and_mob_params_xdd_diff;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UE-NR-Capability ::= SEQUENCE
struct ue_nr_cap_s {
  using feature_set_combinations_l_ = dyn_array<feature_set_combination_l>;

  // member variables
  bool                        rlc_params_present               = false;
  bool                        mac_params_present               = false;
  bool                        meas_and_mob_params_present      = false;
  bool                        fdd_add_ue_nr_cap_present        = false;
  bool                        tdd_add_ue_nr_cap_present        = false;
  bool                        fr1_add_ue_nr_cap_present        = false;
  bool                        fr2_add_ue_nr_cap_present        = false;
  bool                        feature_sets_present             = false;
  bool                        feature_set_combinations_present = false;
  bool                        late_non_crit_ext_present        = false;
  bool                        non_crit_ext_present             = false;
  access_stratum_release_e    access_stratum_release;
  pdcp_params_s               pdcp_params;
  rlc_params_s                rlc_params;
  mac_params_s                mac_params;
  phy_params_s                phy_params;
  rf_params_s                 rf_params;
  meas_and_mob_params_s       meas_and_mob_params;
  ue_nr_cap_add_xdd_mode_s    fdd_add_ue_nr_cap;
  ue_nr_cap_add_xdd_mode_s    tdd_add_ue_nr_cap;
  ue_nr_cap_add_frx_mode_s    fr1_add_ue_nr_cap;
  ue_nr_cap_add_frx_mode_s    fr2_add_ue_nr_cap;
  feature_sets_s              feature_sets;
  feature_set_combinations_l_ feature_set_combinations;
  dyn_octstring               late_non_crit_ext;
  ue_nr_cap_v1530_s           non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// AS-Config ::= SEQUENCE
struct as_cfg_s {
  bool          ext = false;
  dyn_octstring rrc_recfg;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BandCombinationInfo ::= SEQUENCE
struct band_combination_info_s {
  using allowed_feature_sets_list_l_ = dyn_array<uint8_t>;

  // member variables
  uint32_t                     band_combination_idx = 1;
  allowed_feature_sets_list_l_ allowed_feature_sets_list;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ReestabNCellInfo ::= SEQUENCE
struct reestab_ncell_info_s {
  fixed_bitstring<36>  cell_id;
  fixed_bitstring<256> key_g_node_b_star;
  fixed_bitstring<16>  short_mac_i;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BandCombinationInfoList ::= SEQUENCE (SIZE (1..65536)) OF BandCombinationInfo
using band_combination_info_list_l = dyn_array<band_combination_info_s>;

// ReestabNCellInfoList ::= SEQUENCE (SIZE (1..32)) OF ReestabNCellInfo
using reestab_ncell_info_list_l = dyn_array<reestab_ncell_info_s>;

// ConfigRestrictInfoSCG ::= SEQUENCE
struct cfg_restrict_info_scg_s {
  struct pwr_coordination_fr1_s_ {
    bool   p_max_nr_fr1_present = false;
    bool   p_max_eutra_present  = false;
    bool   p_max_ue_fr1_present = false;
    int8_t p_max_nr_fr1         = -30;
    int8_t p_max_eutra          = -30;
    int8_t p_max_ue_fr1         = -30;
  };
  struct serv_cell_idx_range_scg_s_ {
    uint8_t low_bound = 0;
    uint8_t up_bound  = 0;
  };

  // member variables
  bool                         ext                                = false;
  bool                         allowed_bc_list_mrdc_present       = false;
  bool                         pwr_coordination_fr1_present       = false;
  bool                         serv_cell_idx_range_scg_present    = false;
  bool                         max_meas_freqs_scg_nr_present      = false;
  bool                         max_meas_identities_scg_nr_present = false;
  band_combination_info_list_l allowed_bc_list_mrdc;
  pwr_coordination_fr1_s_      pwr_coordination_fr1;
  serv_cell_idx_range_scg_s_   serv_cell_idx_range_scg;
  uint8_t                      max_meas_freqs_scg_nr      = 1;
  uint8_t                      max_meas_identities_scg_nr = 1;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ReestablishmentInfo ::= SEQUENCE
struct reest_info_s {
  bool                      add_reestab_info_list_present = false;
  uint16_t                  source_pci                    = 0;
  fixed_bitstring<16>       target_cell_short_mac_i;
  reestab_ncell_info_list_l add_reestab_info_list;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// AS-Context ::= SEQUENCE
struct as_context_s {
  bool                    ext                       = false;
  bool                    reest_info_present        = false;
  bool                    cfg_restrict_info_present = false;
  reest_info_s            reest_info;
  cfg_restrict_info_scg_s cfg_restrict_info;
  // ...
  // group 0
  copy_ptr<ran_notif_area_info_c> ran_notif_area_info;
  // group 1
  bool          ue_assist_info_present = false;
  dyn_octstring ue_assist_info;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// AffectedCarrierFreqCombEUTRA ::= SEQUENCE (SIZE (1..32)) OF INTEGER (0..262143)
using affected_carrier_freq_comb_eutra_l = bounded_array<uint32_t, 32>;

// AffectedCarrierFreqCombNR ::= SEQUENCE (SIZE (1..32)) OF INTEGER (0..3279165)
using affected_carrier_freq_comb_nr_l = bounded_array<uint32_t, 32>;

// VictimSystemType ::= SEQUENCE
struct victim_sys_type_s {
  bool gps_present       = false;
  bool glonass_present   = false;
  bool bds_present       = false;
  bool galileo_present   = false;
  bool wlan_present      = false;
  bool bluetooth_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// AffectedCarrierFreqCombInfoMRDC ::= SEQUENCE
struct affected_carrier_freq_comb_info_mrdc_s {
  struct interference_direction_mrdc_opts {
    enum options { eutra_nr, nr, other, utra_nr_other, nr_other, spare3, spare2, spare1, nulltype } value;

    std::string to_string() const;
  };
  typedef enumerated<interference_direction_mrdc_opts> interference_direction_mrdc_e_;
  struct affected_carrier_freq_comb_mrdc_s_ {
    bool                               affected_carrier_freq_comb_eutra_present = false;
    affected_carrier_freq_comb_eutra_l affected_carrier_freq_comb_eutra;
    affected_carrier_freq_comb_nr_l    affected_carrier_freq_comb_nr;
  };

  // member variables
  bool                               affected_carrier_freq_comb_mrdc_present = false;
  victim_sys_type_s                  victim_sys_type;
  interference_direction_mrdc_e_     interference_direction_mrdc;
  affected_carrier_freq_comb_mrdc_s_ affected_carrier_freq_comb_mrdc;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BandCombinationInfoSN ::= SEQUENCE
struct band_combination_info_sn_s {
  uint32_t band_combination_idx   = 1;
  uint8_t  requested_feature_sets = 1;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PH-UplinkCarrierSCG ::= SEQUENCE
struct ph_ul_carrier_scg_s {
  struct ph_type1or3_opts {
    enum options { type1, type3, nulltype } value;
    typedef uint8_t number_type;

    std::string to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<ph_type1or3_opts> ph_type1or3_e_;

  // member variables
  bool           ext = false;
  ph_type1or3_e_ ph_type1or3;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PH-InfoSCG ::= SEQUENCE
struct ph_info_scg_s {
  bool                ext                         = false;
  bool                ph_supplementary_ul_present = false;
  uint8_t             serv_cell_idx               = 0;
  ph_ul_carrier_scg_s ph_ul;
  ph_ul_carrier_scg_s ph_supplementary_ul;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// FR-Info ::= SEQUENCE
struct fr_info_s {
  struct fr_type_opts {
    enum options { fr1, fr2, nulltype } value;
    typedef uint8_t number_type;

    std::string to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<fr_type_opts> fr_type_e_;

  // member variables
  uint8_t    serv_cell_idx = 0;
  fr_type_e_ fr_type;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// NR-FreqInfo ::= SEQUENCE
struct nr_freq_info_s {
  bool     ext                   = false;
  bool     measured_freq_present = false;
  uint32_t measured_freq         = 0;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PH-TypeListSCG ::= SEQUENCE (SIZE (1..32)) OF PH-InfoSCG
using ph_type_list_scg_l = dyn_array<ph_info_scg_s>;

// CG-Config-v1540-IEs ::= SEQUENCE
struct cg_cfg_v1540_ies_s {
  struct report_cgi_request_s_ {
    struct requested_cell_info_s_ {
      uint32_t ssb_freq                     = 0;
      uint16_t cell_for_which_to_report_cgi = 0;
    };

    // member variables
    bool                   requested_cell_info_present = false;
    requested_cell_info_s_ requested_cell_info;
  };

  // member variables
  bool                  pscell_freq_present        = false;
  bool                  report_cgi_request_present = false;
  bool                  ph_info_scg_present        = false;
  bool                  non_crit_ext_present       = false;
  uint32_t              pscell_freq                = 0;
  report_cgi_request_s_ report_cgi_request;
  ph_type_list_scg_l    ph_info_scg;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CandidateServingFreqListNR ::= SEQUENCE (SIZE (1..32)) OF INTEGER (0..3279165)
using candidate_serving_freq_list_nr_l = bounded_array<uint32_t, 32>;

// ConfigRestrictModReqSCG ::= SEQUENCE
struct cfg_restrict_mod_req_scg_s {
  bool                       ext                         = false;
  bool                       requested_bc_mrdc_present   = false;
  bool                       requested_p_max_fr1_present = false;
  band_combination_info_sn_s requested_bc_mrdc;
  int8_t                     requested_p_max_fr1 = -30;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DRX-Info ::= SEQUENCE
struct drx_info_s {
  struct drx_long_cycle_start_offset_c_ {
    struct types_opts {
      enum options {
        ms10,
        ms20,
        ms32,
        ms40,
        ms60,
        ms64,
        ms70,
        ms80,
        ms128,
        ms160,
        ms256,
        ms320,
        ms512,
        ms640,
        ms1024,
        ms1280,
        ms2048,
        ms2560,
        ms5120,
        ms10240,
        nulltype
      } value;
      typedef uint16_t number_type;

      std::string to_string() const;
      uint16_t    to_number() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    drx_long_cycle_start_offset_c_() = default;
    drx_long_cycle_start_offset_c_(const drx_long_cycle_start_offset_c_& other);
    drx_long_cycle_start_offset_c_& operator=(const drx_long_cycle_start_offset_c_& other);
    ~drx_long_cycle_start_offset_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint8_t& ms10()
    {
      assert_choice_type("ms10", type_.to_string(), "drx-LongCycleStartOffset");
      return c.get<uint8_t>();
    }
    uint8_t& ms20()
    {
      assert_choice_type("ms20", type_.to_string(), "drx-LongCycleStartOffset");
      return c.get<uint8_t>();
    }
    uint8_t& ms32()
    {
      assert_choice_type("ms32", type_.to_string(), "drx-LongCycleStartOffset");
      return c.get<uint8_t>();
    }
    uint8_t& ms40()
    {
      assert_choice_type("ms40", type_.to_string(), "drx-LongCycleStartOffset");
      return c.get<uint8_t>();
    }
    uint8_t& ms60()
    {
      assert_choice_type("ms60", type_.to_string(), "drx-LongCycleStartOffset");
      return c.get<uint8_t>();
    }
    uint8_t& ms64()
    {
      assert_choice_type("ms64", type_.to_string(), "drx-LongCycleStartOffset");
      return c.get<uint8_t>();
    }
    uint8_t& ms70()
    {
      assert_choice_type("ms70", type_.to_string(), "drx-LongCycleStartOffset");
      return c.get<uint8_t>();
    }
    uint8_t& ms80()
    {
      assert_choice_type("ms80", type_.to_string(), "drx-LongCycleStartOffset");
      return c.get<uint8_t>();
    }
    uint8_t& ms128()
    {
      assert_choice_type("ms128", type_.to_string(), "drx-LongCycleStartOffset");
      return c.get<uint8_t>();
    }
    uint8_t& ms160()
    {
      assert_choice_type("ms160", type_.to_string(), "drx-LongCycleStartOffset");
      return c.get<uint8_t>();
    }
    uint16_t& ms256()
    {
      assert_choice_type("ms256", type_.to_string(), "drx-LongCycleStartOffset");
      return c.get<uint16_t>();
    }
    uint16_t& ms320()
    {
      assert_choice_type("ms320", type_.to_string(), "drx-LongCycleStartOffset");
      return c.get<uint16_t>();
    }
    uint16_t& ms512()
    {
      assert_choice_type("ms512", type_.to_string(), "drx-LongCycleStartOffset");
      return c.get<uint16_t>();
    }
    uint16_t& ms640()
    {
      assert_choice_type("ms640", type_.to_string(), "drx-LongCycleStartOffset");
      return c.get<uint16_t>();
    }
    uint16_t& ms1024()
    {
      assert_choice_type("ms1024", type_.to_string(), "drx-LongCycleStartOffset");
      return c.get<uint16_t>();
    }
    uint16_t& ms1280()
    {
      assert_choice_type("ms1280", type_.to_string(), "drx-LongCycleStartOffset");
      return c.get<uint16_t>();
    }
    uint16_t& ms2048()
    {
      assert_choice_type("ms2048", type_.to_string(), "drx-LongCycleStartOffset");
      return c.get<uint16_t>();
    }
    uint16_t& ms2560()
    {
      assert_choice_type("ms2560", type_.to_string(), "drx-LongCycleStartOffset");
      return c.get<uint16_t>();
    }
    uint16_t& ms5120()
    {
      assert_choice_type("ms5120", type_.to_string(), "drx-LongCycleStartOffset");
      return c.get<uint16_t>();
    }
    uint16_t& ms10240()
    {
      assert_choice_type("ms10240", type_.to_string(), "drx-LongCycleStartOffset");
      return c.get<uint16_t>();
    }
    const uint8_t& ms10() const
    {
      assert_choice_type("ms10", type_.to_string(), "drx-LongCycleStartOffset");
      return c.get<uint8_t>();
    }
    const uint8_t& ms20() const
    {
      assert_choice_type("ms20", type_.to_string(), "drx-LongCycleStartOffset");
      return c.get<uint8_t>();
    }
    const uint8_t& ms32() const
    {
      assert_choice_type("ms32", type_.to_string(), "drx-LongCycleStartOffset");
      return c.get<uint8_t>();
    }
    const uint8_t& ms40() const
    {
      assert_choice_type("ms40", type_.to_string(), "drx-LongCycleStartOffset");
      return c.get<uint8_t>();
    }
    const uint8_t& ms60() const
    {
      assert_choice_type("ms60", type_.to_string(), "drx-LongCycleStartOffset");
      return c.get<uint8_t>();
    }
    const uint8_t& ms64() const
    {
      assert_choice_type("ms64", type_.to_string(), "drx-LongCycleStartOffset");
      return c.get<uint8_t>();
    }
    const uint8_t& ms70() const
    {
      assert_choice_type("ms70", type_.to_string(), "drx-LongCycleStartOffset");
      return c.get<uint8_t>();
    }
    const uint8_t& ms80() const
    {
      assert_choice_type("ms80", type_.to_string(), "drx-LongCycleStartOffset");
      return c.get<uint8_t>();
    }
    const uint8_t& ms128() const
    {
      assert_choice_type("ms128", type_.to_string(), "drx-LongCycleStartOffset");
      return c.get<uint8_t>();
    }
    const uint8_t& ms160() const
    {
      assert_choice_type("ms160", type_.to_string(), "drx-LongCycleStartOffset");
      return c.get<uint8_t>();
    }
    const uint16_t& ms256() const
    {
      assert_choice_type("ms256", type_.to_string(), "drx-LongCycleStartOffset");
      return c.get<uint16_t>();
    }
    const uint16_t& ms320() const
    {
      assert_choice_type("ms320", type_.to_string(), "drx-LongCycleStartOffset");
      return c.get<uint16_t>();
    }
    const uint16_t& ms512() const
    {
      assert_choice_type("ms512", type_.to_string(), "drx-LongCycleStartOffset");
      return c.get<uint16_t>();
    }
    const uint16_t& ms640() const
    {
      assert_choice_type("ms640", type_.to_string(), "drx-LongCycleStartOffset");
      return c.get<uint16_t>();
    }
    const uint16_t& ms1024() const
    {
      assert_choice_type("ms1024", type_.to_string(), "drx-LongCycleStartOffset");
      return c.get<uint16_t>();
    }
    const uint16_t& ms1280() const
    {
      assert_choice_type("ms1280", type_.to_string(), "drx-LongCycleStartOffset");
      return c.get<uint16_t>();
    }
    const uint16_t& ms2048() const
    {
      assert_choice_type("ms2048", type_.to_string(), "drx-LongCycleStartOffset");
      return c.get<uint16_t>();
    }
    const uint16_t& ms2560() const
    {
      assert_choice_type("ms2560", type_.to_string(), "drx-LongCycleStartOffset");
      return c.get<uint16_t>();
    }
    const uint16_t& ms5120() const
    {
      assert_choice_type("ms5120", type_.to_string(), "drx-LongCycleStartOffset");
      return c.get<uint16_t>();
    }
    const uint16_t& ms10240() const
    {
      assert_choice_type("ms10240", type_.to_string(), "drx-LongCycleStartOffset");
      return c.get<uint16_t>();
    }
    uint8_t& set_ms10()
    {
      set(types::ms10);
      return c.get<uint8_t>();
    }
    uint8_t& set_ms20()
    {
      set(types::ms20);
      return c.get<uint8_t>();
    }
    uint8_t& set_ms32()
    {
      set(types::ms32);
      return c.get<uint8_t>();
    }
    uint8_t& set_ms40()
    {
      set(types::ms40);
      return c.get<uint8_t>();
    }
    uint8_t& set_ms60()
    {
      set(types::ms60);
      return c.get<uint8_t>();
    }
    uint8_t& set_ms64()
    {
      set(types::ms64);
      return c.get<uint8_t>();
    }
    uint8_t& set_ms70()
    {
      set(types::ms70);
      return c.get<uint8_t>();
    }
    uint8_t& set_ms80()
    {
      set(types::ms80);
      return c.get<uint8_t>();
    }
    uint8_t& set_ms128()
    {
      set(types::ms128);
      return c.get<uint8_t>();
    }
    uint8_t& set_ms160()
    {
      set(types::ms160);
      return c.get<uint8_t>();
    }
    uint16_t& set_ms256()
    {
      set(types::ms256);
      return c.get<uint16_t>();
    }
    uint16_t& set_ms320()
    {
      set(types::ms320);
      return c.get<uint16_t>();
    }
    uint16_t& set_ms512()
    {
      set(types::ms512);
      return c.get<uint16_t>();
    }
    uint16_t& set_ms640()
    {
      set(types::ms640);
      return c.get<uint16_t>();
    }
    uint16_t& set_ms1024()
    {
      set(types::ms1024);
      return c.get<uint16_t>();
    }
    uint16_t& set_ms1280()
    {
      set(types::ms1280);
      return c.get<uint16_t>();
    }
    uint16_t& set_ms2048()
    {
      set(types::ms2048);
      return c.get<uint16_t>();
    }
    uint16_t& set_ms2560()
    {
      set(types::ms2560);
      return c.get<uint16_t>();
    }
    uint16_t& set_ms5120()
    {
      set(types::ms5120);
      return c.get<uint16_t>();
    }
    uint16_t& set_ms10240()
    {
      set(types::ms10240);
      return c.get<uint16_t>();
    }

  private:
    types               type_;
    pod_choice_buffer_t c;

    void destroy_();
  };
  struct short_drx_s_ {
    struct drx_short_cycle_opts {
      enum options {
        ms2,
        ms3,
        ms4,
        ms5,
        ms6,
        ms7,
        ms8,
        ms10,
        ms14,
        ms16,
        ms20,
        ms30,
        ms32,
        ms35,
        ms40,
        ms64,
        ms80,
        ms128,
        ms160,
        ms256,
        ms320,
        ms512,
        ms640,
        spare9,
        spare8,
        spare7,
        spare6,
        spare5,
        spare4,
        spare3,
        spare2,
        spare1,
        nulltype
      } value;
      typedef uint16_t number_type;

      std::string to_string() const;
      uint16_t    to_number() const;
    };
    typedef enumerated<drx_short_cycle_opts> drx_short_cycle_e_;

    // member variables
    drx_short_cycle_e_ drx_short_cycle;
    uint8_t            drx_short_cycle_timer = 1;
  };

  // member variables
  bool                           short_drx_present = false;
  drx_long_cycle_start_offset_c_ drx_long_cycle_start_offset;
  short_drx_s_                   short_drx;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// FR-InfoList ::= SEQUENCE (SIZE (1..31)) OF FR-Info
using fr_info_list_l = dyn_array<fr_info_s>;

// MeasConfigSN ::= SEQUENCE
struct meas_cfg_sn_s {
  using measured_frequencies_sn_l_ = dyn_array<nr_freq_info_s>;

  // member variables
  bool                       ext                             = false;
  bool                       measured_frequencies_sn_present = false;
  measured_frequencies_sn_l_ measured_frequencies_sn;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CG-Config-IEs ::= SEQUENCE
struct cg_cfg_ies_s {
  bool                             scg_cell_group_cfg_present             = false;
  bool                             scg_rb_cfg_present                     = false;
  bool                             cfg_restrict_mod_req_present           = false;
  bool                             drx_info_scg_present                   = false;
  bool                             candidate_cell_info_list_sn_present    = false;
  bool                             meas_cfg_sn_present                    = false;
  bool                             sel_band_combination_nr_present        = false;
  bool                             fr_info_list_scg_present               = false;
  bool                             candidate_serving_freq_list_nr_present = false;
  bool                             non_crit_ext_present                   = false;
  dyn_octstring                    scg_cell_group_cfg;
  dyn_octstring                    scg_rb_cfg;
  cfg_restrict_mod_req_scg_s       cfg_restrict_mod_req;
  drx_info_s                       drx_info_scg;
  dyn_octstring                    candidate_cell_info_list_sn;
  meas_cfg_sn_s                    meas_cfg_sn;
  band_combination_info_sn_s       sel_band_combination_nr;
  fr_info_list_l                   fr_info_list_scg;
  candidate_serving_freq_list_nr_l candidate_serving_freq_list_nr;
  cg_cfg_v1540_ies_s               non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CG-Config ::= SEQUENCE
struct cg_cfg_s {
  struct crit_exts_c_ {
    struct c1_c_ {
      struct types_opts {
        enum options { cg_cfg, spare3, spare2, spare1, nulltype } value;

        std::string to_string() const;
      };
      typedef enumerated<types_opts> types;

      // choice methods
      c1_c_() = default;
      void        set(types::options e = types::nulltype);
      types       type() const { return type_; }
      SRSASN_CODE pack(bit_ref& bref) const;
      SRSASN_CODE unpack(cbit_ref& bref);
      void        to_json(json_writer& j) const;
      // getters
      cg_cfg_ies_s& cg_cfg()
      {
        assert_choice_type("cg-Config", type_.to_string(), "c1");
        return c;
      }
      const cg_cfg_ies_s& cg_cfg() const
      {
        assert_choice_type("cg-Config", type_.to_string(), "c1");
        return c;
      }
      cg_cfg_ies_s& set_cg_cfg()
      {
        set(types::cg_cfg);
        return c;
      }

    private:
      types        type_;
      cg_cfg_ies_s c;
    };
    struct types_opts {
      enum options { c1, crit_exts_future, nulltype } value;
      typedef uint8_t number_type;

      std::string to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    crit_exts_c_() = default;
    crit_exts_c_(const crit_exts_c_& other);
    crit_exts_c_& operator=(const crit_exts_c_& other);
    ~crit_exts_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    c1_c_& c1()
    {
      assert_choice_type("c1", type_.to_string(), "criticalExtensions");
      return c.get<c1_c_>();
    }
    const c1_c_& c1() const
    {
      assert_choice_type("c1", type_.to_string(), "criticalExtensions");
      return c.get<c1_c_>();
    }
    c1_c_& set_c1()
    {
      set(types::c1);
      return c.get<c1_c_>();
    }

  private:
    types                  type_;
    choice_buffer_t<c1_c_> c;

    void destroy_();
  };

  // member variables
  crit_exts_c_ crit_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PH-UplinkCarrierMCG ::= SEQUENCE
struct ph_ul_carrier_mcg_s {
  struct ph_type1or3_opts {
    enum options { type1, type3, nulltype } value;
    typedef uint8_t number_type;

    std::string to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<ph_type1or3_opts> ph_type1or3_e_;

  // member variables
  bool           ext = false;
  ph_type1or3_e_ ph_type1or3;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PH-InfoMCG ::= SEQUENCE
struct ph_info_mcg_s {
  bool                ext                         = false;
  bool                ph_supplementary_ul_present = false;
  uint8_t             serv_cell_idx               = 0;
  ph_ul_carrier_mcg_s ph_ul;
  ph_ul_carrier_mcg_s ph_supplementary_ul;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PH-TypeListMCG ::= SEQUENCE (SIZE (1..32)) OF PH-InfoMCG
using ph_type_list_mcg_l = dyn_array<ph_info_mcg_s>;

// CG-ConfigInfo-v1540-IEs ::= SEQUENCE
struct cg_cfg_info_v1540_ies_s {
  struct meas_result_report_cgi_s_ {
    uint32_t   ssb_freq                     = 0;
    uint16_t   cell_for_which_to_report_cgi = 0;
    cgi_info_s cgi_info;
  };

  // member variables
  bool                      ph_info_mcg_present            = false;
  bool                      meas_result_report_cgi_present = false;
  bool                      non_crit_ext_present           = false;
  ph_type_list_mcg_l        ph_info_mcg;
  meas_result_report_cgi_s_ meas_result_report_cgi;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MRDC-AssistanceInfo ::= SEQUENCE
struct mrdc_assist_info_s {
  using affected_carrier_freq_comb_info_list_mrdc_l_ = dyn_array<affected_carrier_freq_comb_info_mrdc_s>;

  // member variables
  bool                                         ext = false;
  affected_carrier_freq_comb_info_list_mrdc_l_ affected_carrier_freq_comb_info_list_mrdc;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasConfigMN ::= SEQUENCE
struct meas_cfg_mn_s {
  using measured_frequencies_mn_l_ = dyn_array<nr_freq_info_s>;
  struct gap_purpose_opts {
    enum options { per_ue, per_fr1, nulltype } value;
    typedef uint8_t number_type;

    std::string to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<gap_purpose_opts> gap_purpose_e_;

  // member variables
  bool                       ext                             = false;
  bool                       measured_frequencies_mn_present = false;
  bool                       meas_gap_cfg_present            = false;
  bool                       gap_purpose_present             = false;
  measured_frequencies_mn_l_ measured_frequencies_mn;
  setup_release_c<gap_cfg_s> meas_gap_cfg;
  gap_purpose_e_             gap_purpose;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CG-ConfigInfo-IEs ::= SEQUENCE
struct cg_cfg_info_ies_s {
  struct scg_fail_info_s_ {
    struct fail_type_opts {
      enum options {
        t310_expiry,
        random_access_problem,
        rlc_max_num_retx,
        synch_recfg_fail_scg,
        scg_recfg_fail,
        srb3_integrity_fail,
        nulltype
      } value;
      typedef uint16_t number_type;

      std::string to_string() const;
      uint16_t    to_number() const;
    };
    typedef enumerated<fail_type_opts> fail_type_e_;

    // member variables
    fail_type_e_  fail_type;
    dyn_octstring meas_result_scg;
  };

  // member variables
  bool                         ue_cap_info_present                 = false;
  bool                         candidate_cell_info_list_mn_present = false;
  bool                         candidate_cell_info_list_sn_present = false;
  bool                         meas_result_cell_list_sftd_present  = false;
  bool                         scg_fail_info_present               = false;
  bool                         cfg_restrict_info_present           = false;
  bool                         drx_info_mcg_present                = false;
  bool                         meas_cfg_mn_present                 = false;
  bool                         source_cfg_scg_present              = false;
  bool                         scg_rb_cfg_present                  = false;
  bool                         mcg_rb_cfg_present                  = false;
  bool                         mrdc_assist_info_present            = false;
  bool                         non_crit_ext_present                = false;
  dyn_octstring                ue_cap_info;
  meas_result_list2_nr_l       candidate_cell_info_list_mn;
  dyn_octstring                candidate_cell_info_list_sn;
  meas_result_cell_list_sftd_l meas_result_cell_list_sftd;
  scg_fail_info_s_             scg_fail_info;
  cfg_restrict_info_scg_s      cfg_restrict_info;
  drx_info_s                   drx_info_mcg;
  meas_cfg_mn_s                meas_cfg_mn;
  dyn_octstring                source_cfg_scg;
  dyn_octstring                scg_rb_cfg;
  dyn_octstring                mcg_rb_cfg;
  mrdc_assist_info_s           mrdc_assist_info;
  cg_cfg_info_v1540_ies_s      non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CG-ConfigInfo ::= SEQUENCE
struct cg_cfg_info_s {
  struct crit_exts_c_ {
    struct c1_c_ {
      struct types_opts {
        enum options { cg_cfg_info, spare3, spare2, spare1, nulltype } value;

        std::string to_string() const;
      };
      typedef enumerated<types_opts> types;

      // choice methods
      c1_c_() = default;
      void        set(types::options e = types::nulltype);
      types       type() const { return type_; }
      SRSASN_CODE pack(bit_ref& bref) const;
      SRSASN_CODE unpack(cbit_ref& bref);
      void        to_json(json_writer& j) const;
      // getters
      cg_cfg_info_ies_s& cg_cfg_info()
      {
        assert_choice_type("cg-ConfigInfo", type_.to_string(), "c1");
        return c;
      }
      const cg_cfg_info_ies_s& cg_cfg_info() const
      {
        assert_choice_type("cg-ConfigInfo", type_.to_string(), "c1");
        return c;
      }
      cg_cfg_info_ies_s& set_cg_cfg_info()
      {
        set(types::cg_cfg_info);
        return c;
      }

    private:
      types             type_;
      cg_cfg_info_ies_s c;
    };
    struct types_opts {
      enum options { c1, crit_exts_future, nulltype } value;
      typedef uint8_t number_type;

      std::string to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    crit_exts_c_() = default;
    crit_exts_c_(const crit_exts_c_& other);
    crit_exts_c_& operator=(const crit_exts_c_& other);
    ~crit_exts_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    c1_c_& c1()
    {
      assert_choice_type("c1", type_.to_string(), "criticalExtensions");
      return c.get<c1_c_>();
    }
    const c1_c_& c1() const
    {
      assert_choice_type("c1", type_.to_string(), "criticalExtensions");
      return c.get<c1_c_>();
    }
    c1_c_& set_c1()
    {
      set(types::c1);
      return c.get<c1_c_>();
    }

  private:
    types                  type_;
    choice_buffer_t<c1_c_> c;

    void destroy_();
  };

  // member variables
  crit_exts_c_ crit_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

struct cells_triggered_list_item_c_ {
  struct types_opts {
    enum options { pci, pci_eutra, nulltype } value;

    std::string to_string() const;
  };
  typedef enumerated<types_opts> types;

  // choice methods
  cells_triggered_list_item_c_() = default;
  cells_triggered_list_item_c_(const cells_triggered_list_item_c_& other);
  cells_triggered_list_item_c_& operator=(const cells_triggered_list_item_c_& other);
  ~cells_triggered_list_item_c_() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  uint16_t& pci()
  {
    assert_choice_type("physCellId", type_.to_string(), "CellsTriggeredList-item");
    return c.get<uint16_t>();
  }
  uint16_t& pci_eutra()
  {
    assert_choice_type("physCellIdEUTRA", type_.to_string(), "CellsTriggeredList-item");
    return c.get<uint16_t>();
  }
  const uint16_t& pci() const
  {
    assert_choice_type("physCellId", type_.to_string(), "CellsTriggeredList-item");
    return c.get<uint16_t>();
  }
  const uint16_t& pci_eutra() const
  {
    assert_choice_type("physCellIdEUTRA", type_.to_string(), "CellsTriggeredList-item");
    return c.get<uint16_t>();
  }
  uint16_t& set_pci()
  {
    set(types::pci);
    return c.get<uint16_t>();
  }
  uint16_t& set_pci_eutra()
  {
    set(types::pci_eutra);
    return c.get<uint16_t>();
  }

private:
  types               type_;
  pod_choice_buffer_t c;

  void destroy_();
};

// CellsTriggeredList ::= SEQUENCE (SIZE (1..32)) OF CellsTriggeredList-item
using cells_triggered_list_l = dyn_array<cells_triggered_list_item_c_>;

// HandoverCommand-IEs ::= SEQUENCE
struct ho_cmd_ies_s {
  bool          non_crit_ext_present = false;
  dyn_octstring ho_cmd_msg;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// HandoverCommand ::= SEQUENCE
struct ho_cmd_s {
  struct crit_exts_c_ {
    struct c1_c_ {
      struct types_opts {
        enum options { ho_cmd, spare3, spare2, spare1, nulltype } value;

        std::string to_string() const;
      };
      typedef enumerated<types_opts> types;

      // choice methods
      c1_c_() = default;
      void        set(types::options e = types::nulltype);
      types       type() const { return type_; }
      SRSASN_CODE pack(bit_ref& bref) const;
      SRSASN_CODE unpack(cbit_ref& bref);
      void        to_json(json_writer& j) const;
      // getters
      ho_cmd_ies_s& ho_cmd()
      {
        assert_choice_type("handoverCommand", type_.to_string(), "c1");
        return c;
      }
      const ho_cmd_ies_s& ho_cmd() const
      {
        assert_choice_type("handoverCommand", type_.to_string(), "c1");
        return c;
      }
      ho_cmd_ies_s& set_ho_cmd()
      {
        set(types::ho_cmd);
        return c;
      }

    private:
      types        type_;
      ho_cmd_ies_s c;
    };
    struct types_opts {
      enum options { c1, crit_exts_future, nulltype } value;
      typedef uint8_t number_type;

      std::string to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    crit_exts_c_() = default;
    crit_exts_c_(const crit_exts_c_& other);
    crit_exts_c_& operator=(const crit_exts_c_& other);
    ~crit_exts_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    c1_c_& c1()
    {
      assert_choice_type("c1", type_.to_string(), "criticalExtensions");
      return c.get<c1_c_>();
    }
    const c1_c_& c1() const
    {
      assert_choice_type("c1", type_.to_string(), "criticalExtensions");
      return c.get<c1_c_>();
    }
    c1_c_& set_c1()
    {
      set(types::c1);
      return c.get<c1_c_>();
    }

  private:
    types                  type_;
    choice_buffer_t<c1_c_> c;

    void destroy_();
  };

  // member variables
  crit_exts_c_ crit_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRM-Config ::= SEQUENCE
struct rrm_cfg_s {
  struct ue_inactive_time_opts {
    enum options {
      s1,
      s2,
      s3,
      s5,
      s7,
      s10,
      s15,
      s20,
      s25,
      s30,
      s40,
      s50,
      min1,
      min1s20,
      min1s40,
      min2,
      min2s30,
      min3,
      min3s30,
      min4,
      min5,
      min6,
      min7,
      min8,
      min9,
      min10,
      min12,
      min14,
      min17,
      min20,
      min24,
      min28,
      min33,
      min38,
      min44,
      min50,
      hr1,
      hr1min30,
      hr2,
      hr2min30,
      hr3,
      hr3min30,
      hr4,
      hr5,
      hr6,
      hr8,
      hr10,
      hr13,
      hr16,
      hr20,
      day1,
      day1hr12,
      day2,
      day2hr12,
      day3,
      day4,
      day5,
      day7,
      day10,
      day14,
      day19,
      day24,
      day30,
      day_more_than30,
      nulltype
    } value;

    std::string to_string() const;
  };
  typedef enumerated<ue_inactive_time_opts> ue_inactive_time_e_;

  // member variables
  bool                   ext                              = false;
  bool                   ue_inactive_time_present         = false;
  bool                   candidate_cell_info_list_present = false;
  ue_inactive_time_e_    ue_inactive_time;
  meas_result_list2_nr_l candidate_cell_info_list;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// HandoverPreparationInformation-IEs ::= SEQUENCE
struct ho_prep_info_ies_s {
  bool                        source_cfg_present   = false;
  bool                        rrm_cfg_present      = false;
  bool                        as_context_present   = false;
  bool                        non_crit_ext_present = false;
  ue_cap_rat_container_list_l ue_cap_rat_list;
  as_cfg_s                    source_cfg;
  rrm_cfg_s                   rrm_cfg;
  as_context_s                as_context;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// HandoverPreparationInformation ::= SEQUENCE
struct ho_prep_info_s {
  struct crit_exts_c_ {
    struct c1_c_ {
      struct types_opts {
        enum options { ho_prep_info, spare3, spare2, spare1, nulltype } value;

        std::string to_string() const;
      };
      typedef enumerated<types_opts> types;

      // choice methods
      c1_c_() = default;
      void        set(types::options e = types::nulltype);
      types       type() const { return type_; }
      SRSASN_CODE pack(bit_ref& bref) const;
      SRSASN_CODE unpack(cbit_ref& bref);
      void        to_json(json_writer& j) const;
      // getters
      ho_prep_info_ies_s& ho_prep_info()
      {
        assert_choice_type("handoverPreparationInformation", type_.to_string(), "c1");
        return c;
      }
      const ho_prep_info_ies_s& ho_prep_info() const
      {
        assert_choice_type("handoverPreparationInformation", type_.to_string(), "c1");
        return c;
      }
      ho_prep_info_ies_s& set_ho_prep_info()
      {
        set(types::ho_prep_info);
        return c;
      }

    private:
      types              type_;
      ho_prep_info_ies_s c;
    };
    struct types_opts {
      enum options { c1, crit_exts_future, nulltype } value;
      typedef uint8_t number_type;

      std::string to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    crit_exts_c_() = default;
    crit_exts_c_(const crit_exts_c_& other);
    crit_exts_c_& operator=(const crit_exts_c_& other);
    ~crit_exts_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    c1_c_& c1()
    {
      assert_choice_type("c1", type_.to_string(), "criticalExtensions");
      return c.get<c1_c_>();
    }
    const c1_c_& c1() const
    {
      assert_choice_type("c1", type_.to_string(), "criticalExtensions");
      return c.get<c1_c_>();
    }
    c1_c_& set_c1()
    {
      set(types::c1);
      return c.get<c1_c_>();
    }

  private:
    types                  type_;
    choice_buffer_t<c1_c_> c;

    void destroy_();
  };

  // member variables
  crit_exts_c_ crit_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasTiming ::= SEQUENCE
struct meas_timing_s {
  struct freq_and_timing_s_ {
    bool                 ss_rssi_meas_present = false;
    uint32_t             carrier_freq         = 0;
    subcarrier_spacing_e ssb_subcarrier_spacing;
    ssb_mtc_s            ssb_meas_timing_cfg;
    ss_rssi_meas_s       ss_rssi_meas;
  };

  // member variables
  bool               ext                     = false;
  bool               freq_and_timing_present = false;
  freq_and_timing_s_ freq_and_timing;
  // ...
  // group 0
  bool                       pci_present = false;
  copy_ptr<ssb_to_measure_c> ssb_to_measure_v1540;
  uint16_t                   pci = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasTimingList ::= SEQUENCE (SIZE (1..32)) OF MeasTiming
using meas_timing_list_l = dyn_array<meas_timing_s>;

// MeasurementTimingConfiguration-v1550-IEs ::= SEQUENCE
struct meas_timing_cfg_v1550_ies_s {
  bool non_crit_ext_present      = false;
  bool camp_on_first_ssb         = false;
  bool ps_cell_only_on_first_ssb = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasurementTimingConfiguration-IEs ::= SEQUENCE
struct meas_timing_cfg_ies_s {
  bool                        meas_timing_present  = false;
  bool                        non_crit_ext_present = false;
  meas_timing_list_l          meas_timing;
  meas_timing_cfg_v1550_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasurementTimingConfiguration ::= SEQUENCE
struct meas_timing_cfg_s {
  struct crit_exts_c_ {
    struct c1_c_ {
      struct types_opts {
        enum options { meas_timing_conf, spare3, spare2, spare1, nulltype } value;

        std::string to_string() const;
      };
      typedef enumerated<types_opts> types;

      // choice methods
      c1_c_() = default;
      void        set(types::options e = types::nulltype);
      types       type() const { return type_; }
      SRSASN_CODE pack(bit_ref& bref) const;
      SRSASN_CODE unpack(cbit_ref& bref);
      void        to_json(json_writer& j) const;
      // getters
      meas_timing_cfg_ies_s& meas_timing_conf()
      {
        assert_choice_type("measTimingConf", type_.to_string(), "c1");
        return c;
      }
      const meas_timing_cfg_ies_s& meas_timing_conf() const
      {
        assert_choice_type("measTimingConf", type_.to_string(), "c1");
        return c;
      }
      meas_timing_cfg_ies_s& set_meas_timing_conf()
      {
        set(types::meas_timing_conf);
        return c;
      }

    private:
      types                 type_;
      meas_timing_cfg_ies_s c;
    };
    struct types_opts {
      enum options { c1, crit_exts_future, nulltype } value;
      typedef uint8_t number_type;

      std::string to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    crit_exts_c_() = default;
    crit_exts_c_(const crit_exts_c_& other);
    crit_exts_c_& operator=(const crit_exts_c_& other);
    ~crit_exts_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    c1_c_& c1()
    {
      assert_choice_type("c1", type_.to_string(), "criticalExtensions");
      return c.get<c1_c_>();
    }
    const c1_c_& c1() const
    {
      assert_choice_type("c1", type_.to_string(), "criticalExtensions");
      return c.get<c1_c_>();
    }
    c1_c_& set_c1()
    {
      set(types::c1);
      return c.get<c1_c_>();
    }

  private:
    types                  type_;
    choice_buffer_t<c1_c_> c;

    void destroy_();
  };

  // member variables
  crit_exts_c_ crit_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UERadioAccessCapabilityInformation-IEs ::= SEQUENCE
struct ue_radio_access_cap_info_ies_s {
  bool          non_crit_ext_present = false;
  dyn_octstring ue_radio_access_cap_info;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UERadioAccessCapabilityInformation ::= SEQUENCE
struct ue_radio_access_cap_info_s {
  struct crit_exts_c_ {
    struct c1_c_ {
      struct types_opts {
        enum options {
          ue_radio_access_cap_info,
          spare7,
          spare6,
          spare5,
          spare4,
          spare3,
          spare2,
          spare1,
          nulltype
        } value;

        std::string to_string() const;
      };
      typedef enumerated<types_opts> types;

      // choice methods
      c1_c_() = default;
      void        set(types::options e = types::nulltype);
      types       type() const { return type_; }
      SRSASN_CODE pack(bit_ref& bref) const;
      SRSASN_CODE unpack(cbit_ref& bref);
      void        to_json(json_writer& j) const;
      // getters
      ue_radio_access_cap_info_ies_s& ue_radio_access_cap_info()
      {
        assert_choice_type("ueRadioAccessCapabilityInformation", type_.to_string(), "c1");
        return c;
      }
      const ue_radio_access_cap_info_ies_s& ue_radio_access_cap_info() const
      {
        assert_choice_type("ueRadioAccessCapabilityInformation", type_.to_string(), "c1");
        return c;
      }
      ue_radio_access_cap_info_ies_s& set_ue_radio_access_cap_info()
      {
        set(types::ue_radio_access_cap_info);
        return c;
      }

    private:
      types                          type_;
      ue_radio_access_cap_info_ies_s c;
    };
    struct types_opts {
      enum options { c1, crit_exts_future, nulltype } value;
      typedef uint8_t number_type;

      std::string to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    crit_exts_c_() = default;
    crit_exts_c_(const crit_exts_c_& other);
    crit_exts_c_& operator=(const crit_exts_c_& other);
    ~crit_exts_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    c1_c_& c1()
    {
      assert_choice_type("c1", type_.to_string(), "criticalExtensions");
      return c.get<c1_c_>();
    }
    const c1_c_& c1() const
    {
      assert_choice_type("c1", type_.to_string(), "criticalExtensions");
      return c.get<c1_c_>();
    }
    c1_c_& set_c1()
    {
      set(types::c1);
      return c.get<c1_c_>();
    }

  private:
    types                  type_;
    choice_buffer_t<c1_c_> c;

    void destroy_();
  };

  // member variables
  crit_exts_c_ crit_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UERadioPagingInformation-IEs ::= SEQUENCE
struct ue_radio_paging_info_ies_s {
  using supported_band_list_nr_for_paging_l_ = dyn_array<uint16_t>;

  // member variables
  bool                                 supported_band_list_nr_for_paging_present = false;
  bool                                 non_crit_ext_present                      = false;
  supported_band_list_nr_for_paging_l_ supported_band_list_nr_for_paging;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UERadioPagingInformation ::= SEQUENCE
struct ue_radio_paging_info_s {
  struct crit_exts_c_ {
    struct c1_c_ {
      struct types_opts {
        enum options { ue_radio_paging_info, spare7, spare6, spare5, spare4, spare3, spare2, spare1, nulltype } value;

        std::string to_string() const;
      };
      typedef enumerated<types_opts> types;

      // choice methods
      c1_c_() = default;
      void        set(types::options e = types::nulltype);
      types       type() const { return type_; }
      SRSASN_CODE pack(bit_ref& bref) const;
      SRSASN_CODE unpack(cbit_ref& bref);
      void        to_json(json_writer& j) const;
      // getters
      ue_radio_paging_info_ies_s& ue_radio_paging_info()
      {
        assert_choice_type("ueRadioPagingInformation", type_.to_string(), "c1");
        return c;
      }
      const ue_radio_paging_info_ies_s& ue_radio_paging_info() const
      {
        assert_choice_type("ueRadioPagingInformation", type_.to_string(), "c1");
        return c;
      }
      ue_radio_paging_info_ies_s& set_ue_radio_paging_info()
      {
        set(types::ue_radio_paging_info);
        return c;
      }

    private:
      types                      type_;
      ue_radio_paging_info_ies_s c;
    };
    struct types_opts {
      enum options { c1, crit_exts_future, nulltype } value;
      typedef uint8_t number_type;

      std::string to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    crit_exts_c_() = default;
    crit_exts_c_(const crit_exts_c_& other);
    crit_exts_c_& operator=(const crit_exts_c_& other);
    ~crit_exts_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    c1_c_& c1()
    {
      assert_choice_type("c1", type_.to_string(), "criticalExtensions");
      return c.get<c1_c_>();
    }
    const c1_c_& c1() const
    {
      assert_choice_type("c1", type_.to_string(), "criticalExtensions");
      return c.get<c1_c_>();
    }
    c1_c_& set_c1()
    {
      set(types::c1);
      return c.get<c1_c_>();
    }

  private:
    types                  type_;
    choice_buffer_t<c1_c_> c;

    void destroy_();
  };

  // member variables
  crit_exts_c_ crit_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// VarMeasConfig ::= SEQUENCE
struct var_meas_cfg_s {
  struct s_measure_cfg_c_ {
    struct types_opts {
      enum options { ssb_rsrp, csi_rsrp, nulltype } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    s_measure_cfg_c_() = default;
    s_measure_cfg_c_(const s_measure_cfg_c_& other);
    s_measure_cfg_c_& operator=(const s_measure_cfg_c_& other);
    ~s_measure_cfg_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint8_t& ssb_rsrp()
    {
      assert_choice_type("ssb-RSRP", type_.to_string(), "s-MeasureConfig");
      return c.get<uint8_t>();
    }
    uint8_t& csi_rsrp()
    {
      assert_choice_type("csi-RSRP", type_.to_string(), "s-MeasureConfig");
      return c.get<uint8_t>();
    }
    const uint8_t& ssb_rsrp() const
    {
      assert_choice_type("ssb-RSRP", type_.to_string(), "s-MeasureConfig");
      return c.get<uint8_t>();
    }
    const uint8_t& csi_rsrp() const
    {
      assert_choice_type("csi-RSRP", type_.to_string(), "s-MeasureConfig");
      return c.get<uint8_t>();
    }
    uint8_t& set_ssb_rsrp()
    {
      set(types::ssb_rsrp);
      return c.get<uint8_t>();
    }
    uint8_t& set_csi_rsrp()
    {
      set(types::csi_rsrp);
      return c.get<uint8_t>();
    }

  private:
    types               type_;
    pod_choice_buffer_t c;

    void destroy_();
  };

  // member variables
  bool                         meas_id_list_present    = false;
  bool                         meas_obj_list_present   = false;
  bool                         report_cfg_list_present = false;
  bool                         quant_cfg_present       = false;
  bool                         s_measure_cfg_present   = false;
  meas_id_to_add_mod_list_l    meas_id_list;
  meas_obj_to_add_mod_list_l   meas_obj_list;
  report_cfg_to_add_mod_list_l report_cfg_list;
  quant_cfg_s                  quant_cfg;
  s_measure_cfg_c_             s_measure_cfg;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// VarMeasReport ::= SEQUENCE
struct var_meas_report_s {
  bool                   cells_triggered_list_present = false;
  uint8_t                meas_id                      = 1;
  cells_triggered_list_l cells_triggered_list;
  int64_t                nof_reports_sent = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// VarMeasReportList ::= SEQUENCE (SIZE (1..64)) OF VarMeasReport
using var_meas_report_list_l = dyn_array<var_meas_report_s>;

// VarResumeMAC-Input ::= SEQUENCE
struct var_resume_mac_input_s {
  uint16_t            source_pci = 0;
  fixed_bitstring<36> target_cell_id;
  uint32_t            source_c_rnti = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// VarShortMAC-Input ::= SEQUENCE
struct var_short_mac_input_s {
  uint16_t            source_pci = 0;
  fixed_bitstring<36> target_cell_id;
  uint32_t            source_c_rnti = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

} // namespace rrc_nr
} // namespace asn1

#endif // SRSASN1_RRC_NR_H
