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

/**
 * \brief Structures and utility functions for DL/UL resource allocation for NB-IoT.
 *
 * Reference:    3GPP TS 36.213 version 13.2.0 Release 13
 */

#ifndef SRSRAN_RA_NBIOT_H
#define SRSRAN_RA_NBIOT_H

#include <stdbool.h>
#include <stdint.h>

#include "srsran/config.h"
#include "srsran/phy/common/phy_common.h"
#include "srsran/phy/phch/npbch.h"
#include "srsran/phy/phch/ra.h"

#define SRSRAN_NPUSCH_MAX_SC 12
#define SRSRAN_NPUSCH_MAX_SLOTS (16 * SRSRAN_NPUSCH_N_RU_MAX)
#define SRSRAN_NPUSCH_MAX_SF (SRSRAN_NPUSCH_MAX_SLOTS / 2)
#define SRSRAN_NPUSCH_N_REP_MAX 128
#define SRSRAN_NPUSCH_N_RU_MAX 10
#define SRSRAN_NPUSCH_MAX_NOF_RU_SLOTS_PROD 24
#define SRSRAN_NPUSCH_FORMAT2_MAX_SLOTS 4

#define DUMMY_R_MAX 64
#define SIB1_NB_TTI 256
#define SIB1_NB_MAX_REP 16

/// All System Information types defined for NB-IoT
typedef enum SRSRAN_API {
  SRSRAN_NBIOT_SI_TYPE_MIB = 0, ///< Essential information required to receive further sys information
  SRSRAN_NBIOT_SI_TYPE_SIB1,    ///< Cell access and selection, other SIB scheduling
  SRSRAN_NBIOT_SI_TYPE_SIB2,    ///< Radio resource configuration information
  SRSRAN_NBIOT_SI_TYPE_SIB3,    ///< Cell re-selection information for intra-frequency, inter-frequency
  SRSRAN_NBIOT_SI_TYPE_SIB4,    ///< Neighboring cell related information relevant for intra-frequency cell re-selection
  SRSRAN_NBIOT_SI_TYPE_SIB5,    ///< Neighboring cell related information relevant for inter-frequency cell re-selection
  SRSRAN_NBIOT_SI_TYPE_SIB14,   ///< Access Barring parameters
  SRSRAN_NBIOT_SI_TYPE_SIB16,   ///< Information related to GPS time and Coordinated Universal Time (UTC)
  SRSRAN_NBIOT_SI_TYPE_NITEMS
} srsran_nbiot_si_type_t;

typedef struct SRSRAN_API {
  bool     has_sib1;        ///< Whether this NPDSCH carries SystemInformationBlockType1-NB
  bool     is_ra;           ///< Order is set to 1 for random access procedure
  uint32_t nprach_start;    ///< Starting number for NPRACH repetitions
  uint32_t nprach_sc;       ///< Subcarrier indication of NPRACH
  uint32_t i_delay;         ///< Scheduling delay
  uint32_t i_sf;            ///< Resource assignment, i.e. number of subframes
  uint32_t i_rep;           ///< Repetition number
  uint32_t i_n_start;       ///< The starting OFDM symbol signalled through RRC
  uint32_t harq_ack;        ///< HARQ-ACK resource
  uint32_t dci_sf_rep_num;  ///< DCI subframe repetition number
  uint32_t sched_info_sib1; ///< broadcasted through MIB-NB for TBS calculation
} srsran_ra_nbiot_t;

/// This shall be replaced/moved by proper structures and packing/unpacking based on ASN1
typedef struct SRSRAN_API {
  uint32_t n; ///< Index of entry in schedulingInfoList
  uint32_t si_periodicity;
  uint32_t si_radio_frame_offset;
  uint32_t si_repetition_pattern;
  uint32_t si_tb;
  uint32_t si_window_length;
} srsran_nbiot_si_params_t;

typedef enum SRSRAN_API {
  SRSRAN_NPUSCH_FORMAT1 = 0,
  SRSRAN_NPUSCH_FORMAT2,
  SRSRAN_NPUSCH_FORMAT_NITEMS
} srsran_npusch_format_t;
static const char srsran_npusch_format_text[SRSRAN_NPUSCH_FORMAT_NITEMS][20] = {"Format 1 (UL-SCH)", "Format 2 (UCI)"};

typedef enum SRSRAN_API {
  SRSRAN_NPUSCH_SC_SPACING_15000 = 0,
  SRSRAN_NPUSCH_SC_SPACING_3750  = 1,
  SRSRAN_NPUSCH_SC_SPACING_NITEMS
} srsran_npusch_sc_spacing_t;
static const char srsran_npusch_sc_spacing_text[SRSRAN_NPUSCH_SC_SPACING_NITEMS][20] = {"15kHz", "3.75kHz"};

/**************************************************
 * Structures used for Downlink Resource Allocation
 **************************************************/

typedef struct SRSRAN_API {
  uint32_t       Qm;
  srsran_ra_tb_t mcs[SRSRAN_MAX_TB];
  uint32_t       start_hfn;
  uint32_t       start_sfn;
  uint32_t       start_sfidx;
  uint32_t       k0;
  uint32_t       nof_sf;
  uint32_t       nof_rep;
  uint32_t       l_start;
  uint32_t       ack_nack_resource;

  srsran_nbiot_mode_t mode; ///< needed to compute starting symbol for NPDSCH
  bool                has_sib1;
} srsran_ra_nbiot_dl_grant_t;

/// Unpacked DCI message for DL grant
typedef struct SRSRAN_API {
  uint32_t format; ///< Flag for format N0/format N1 differentiation – 1 bit, 0 for format N0 and value 1 for format N1
  srsran_ra_nbiot_t alloc;

  uint32_t mcs_idx;
  int      rv_idx;
  bool     ndi;

  // Format N2 specifics
  bool    dci_is_n2;
  uint8_t dir_indication_info;
} srsran_ra_nbiot_dl_dci_t;

/// Structures used for Uplink Resource Allocation

/// Unpacked DCI message
typedef struct SRSRAN_API {
  srsran_npusch_sc_spacing_t sc_spacing;
  uint32_t format; ///< Flag for format N0/format N1 differentiation – 1 bit, 0 for format N0 and value 1 for format N1
  uint32_t i_sc;   ///< Subcarrier indication field
  uint32_t i_delay;        ///< Scheduling delay
  uint32_t i_ru;           ///< Resource assignment, i.e. number of resource units
  uint32_t i_mcs;          ///< MCS field
  uint32_t i_rv;           ///< Redundency version
  uint32_t i_rep;          ///< Repetition number
  bool     ndi;            ///< New data indicator
  uint32_t dci_sf_rep_num; ///< DCI subframe repetition number
} srsran_ra_nbiot_ul_dci_t;

typedef struct SRSRAN_API {
  uint32_t                   Qm;
  srsran_ra_tb_t             mcs;
  uint32_t                   k0;      ///< TODO: consider removing k0 and compute tx_tti directly
  uint32_t                   nof_rep; ///< number of repetitions
  uint32_t                   nof_ru;  ///< Number of resource units
  uint32_t                   nof_slots;
  uint32_t                   nof_sc;                             ///< The total number of Subcarriers (N_sc_RU)
  uint32_t                   sc_alloc_set[SRSRAN_NPUSCH_MAX_SC]; ///< The set of subcarriers
  uint32_t                   rv_idx;
  uint32_t                   tx_tti;
  srsran_npusch_sc_spacing_t sc_spacing;
  srsran_npusch_format_t     format;
} srsran_ra_nbiot_ul_grant_t;

typedef union {
  srsran_ra_nbiot_ul_grant_t ul;
  srsran_ra_nbiot_dl_grant_t dl;
} srsran_nbiot_phy_grant_t;

#define SRSRAN_NBIOT_PHY_GRANT_LEN sizeof(srsran_nbiot_phy_grant_t)

/// Structure that gives the number of encoded bits and RE for a UL/DL grant
typedef struct {
  uint32_t lstart;
  uint32_t nof_symb;
  uint32_t nof_bits;
  uint32_t nof_re;
} srsran_ra_nbits_t;

/// According to Section 16.3.3 in TS 36.213 v13.3
typedef struct SRSRAN_API {
  uint32_t sc_spacing; ///< SC spacing (1bit)
  uint32_t i_sc;       ///< Subcarrier indication field (6bits)
  uint32_t i_delay;    ///< Scheduling delay (2bits)
  uint32_t n_rep;      ///< Repetition number (3bits) (Is this i_rep or N_rep?)
  uint32_t i_mcs;      ///< MCS field (3bits), according to 16.3.3-1
} srsran_nbiot_dci_rar_grant_t;

/// Functions
SRSRAN_API int srsran_ra_nbiot_dl_dci_to_grant(srsran_ra_nbiot_dl_dci_t*   dci,
                                               srsran_ra_nbiot_dl_grant_t* grant,
                                               uint32_t                    sfn,
                                               uint32_t                    sf_idx,
                                               uint32_t                    r_max,
                                               bool                        is_prescheduled,
                                               srsran_nbiot_mode_t         mode);

SRSRAN_API void srsran_ra_nbiot_dl_grant_to_nbits(srsran_ra_nbiot_dl_grant_t* grant,
                                                  srsran_nbiot_cell_t         cell,
                                                  uint32_t                    sf_idx,
                                                  srsran_ra_nbits_t*          nbits);

SRSRAN_API void
srsran_ra_nbiot_ul_get_uci_grant(srsran_ra_nbiot_ul_grant_t* grant, const uint8_t resource_field, const uint32_t tti);

SRSRAN_API int srsran_ra_nbiot_ul_dci_to_grant(srsran_ra_nbiot_ul_dci_t*   dci,
                                               srsran_ra_nbiot_ul_grant_t* grant,
                                               uint32_t                    rx_tti,
                                               srsran_npusch_sc_spacing_t  spacing);

SRSRAN_API void srsran_ra_nbiot_ul_grant_to_nbits(srsran_ra_nbiot_ul_grant_t* grant, srsran_ra_nbits_t* nbits);

SRSRAN_API int
srsran_ra_nbiot_ul_rar_dci_to_grant(srsran_ra_nbiot_ul_dci_t* dci, srsran_ra_nbiot_ul_grant_t* grant, uint32_t rx_tti);

SRSRAN_API void srsran_ra_npdsch_fprint(FILE* f, srsran_ra_nbiot_dl_dci_t* ra, uint32_t nof_prb);

SRSRAN_API void srsran_ra_npusch_fprint(FILE* f, srsran_ra_nbiot_ul_dci_t* dci);

SRSRAN_API void srsran_nbiot_dl_dci_fprint(FILE* f, srsran_ra_nbiot_dl_dci_t* dci);

/*!
 * Checks if TTI contains reference signal
 * In safe mode only those subframes that are guaranteed to contain
 * NRS are considered.
 *
 * \param tti the TTI in question
 * \return true if TTI contains DL ref signal
 */
SRSRAN_API bool srsran_ra_nbiot_dl_has_ref_signal(uint32_t tti);

/*!
 * Checks if TTI contains reference signal for standalone deployment
 *
 * \param tti the TTI in question
 * \return true if TTI contains DL ref signal
 */
SRSRAN_API bool srsran_ra_nbiot_dl_has_ref_signal_standalone(uint32_t tti);

/*!
 * Checks if TTI contains reference signal for inband deployments
 *
 * \param tti the TTI in question
 * \return true if TTI contains DL ref signal
 */
SRSRAN_API bool srsran_ra_nbiot_dl_has_ref_signal_inband(uint32_t tti);

/*!
 * Checks if TTI is a valid downlink TTI, i.e., can carry either a NPDCCH or NPDSCH
 *
 * \param tti the TTI in question
 * \return true if TTI is valid DL TTI
 */
SRSRAN_API bool srsran_ra_nbiot_is_valid_dl_sf(uint32_t tti);

/*!
 * Calculate the number of resource elements per subframe that carry data
 *
 * \param cell the cell structure
 * \param l_start the starting OFDM symbols that carries data
 * \return the number of resource elements
 */
SRSRAN_API uint32_t srsran_ra_nbiot_dl_grant_nof_re(srsran_nbiot_cell_t cell, uint32_t l_start);

SRSRAN_API void srsran_ra_nbiot_dl_grant_fprint(FILE* f, srsran_ra_nbiot_dl_grant_t* grant);

SRSRAN_API void srsran_ra_nbiot_ul_grant_fprint(FILE* f, srsran_ra_nbiot_ul_grant_t* grant);

SRSRAN_API int srsran_ra_n_rep_sib1_nb(srsran_mib_nb_t* mib);

SRSRAN_API int srsran_ra_nbiot_get_sib1_tbs(srsran_mib_nb_t* mib);

SRSRAN_API int srsran_ra_nbiot_get_npdsch_tbs(uint32_t i_tbs, uint32_t i_sf);

SRSRAN_API int srsran_ra_nbiot_get_npusch_tbs(uint32_t i_tbs, uint32_t i_ru);

SRSRAN_API int srsran_ra_nbiot_get_starting_sib1_frame(uint32_t cell_id, srsran_mib_nb_t* mib);

SRSRAN_API int srsran_ra_nbiot_sib1_start(uint32_t n_id_ncell, srsran_mib_nb_t* mib);

SRSRAN_API float srsran_ra_nbiot_get_delta_f(srsran_npusch_sc_spacing_t spacing);

#endif // SRSRAN_RA_NBIOT_H
