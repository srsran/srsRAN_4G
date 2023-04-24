/**
 * Copyright 2013-2023 Software Radio Systems Limited
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

#ifndef SRSRAN_COMMON_H
#define SRSRAN_COMMON_H

/*******************************************************************************
                              INCLUDES
*******************************************************************************/

#include <chrono>
#include <cstring>
#include <memory>
#include <stdint.h>
#include <string>
#include <sys/time.h>

/*******************************************************************************
                              DEFINES
*******************************************************************************/

#define SRSRAN_UE_CATEGORY 4

#define SRSRAN_N_SRB 3
#define SRSRAN_N_DRB 8
#define SRSRAN_N_RADIO_BEARERS 11

#define SRSRAN_N_MCH_LCIDS 32

#define FDD_HARQ_DELAY_DL_MS 4
#define FDD_HARQ_DELAY_UL_MS 4
#define MSG3_DELAY_MS 2 // Delay added to FDD_HARQ_DELAY_DL_MS

#define TTI_SUB(a, b) ((((a) + 10240) - (b)) % 10240)
#define TTI_ADD(a, b) (((a) + (b)) % 10240)

#define TTI_TX(tti) TTI_ADD(tti, FDD_HARQ_DELAY_DL_MS)

#define TTI_RX(tti) (TTI_SUB(tti, FDD_HARQ_DELAY_UL_MS))
#define TTI_RX_ACK(tti) (TTI_ADD(tti, FDD_HARQ_DELAY_UL_MS + FDD_HARQ_DELAY_DL_MS))

#define TTIMOD_SZ 20
#define TTIMOD(tti) (tti % TTIMOD_SZ)

#define INVALID_TTI 10241
#define TX_ENB_DELAY FDD_HARQ_DELAY_UL_MS

#define PHICH_MAX_SF 6 // Maximum PHICH in a subframe (1 in FDD, > 1 in TDD, see table 9.1.2-1 36.213)

#define ASYNC_DL_SCHED (FDD_HARQ_DELAY_UL_MS <= 4)

// Cat 4 UE - Max number of DL-SCH transport block bits received within a TTI
// 3GPP 36.306 v15.4.0 Table 4.1.1 for Category 11 with 2 layers and 256QAM
#define SRSRAN_MAX_TBSIZE_BITS 97896
#define SRSRAN_BUFFER_HEADER_OFFSET 1020
#define SRSRAN_MAX_BUFFER_SIZE_BITS (SRSRAN_MAX_TBSIZE_BITS + SRSRAN_BUFFER_HEADER_OFFSET)
#define SRSRAN_MAX_BUFFER_SIZE_BYTES (SRSRAN_MAX_TBSIZE_BITS / 8 + SRSRAN_BUFFER_HEADER_OFFSET)

/*******************************************************************************
                              TYPEDEFS
*******************************************************************************/

namespace srsran {

#define ENABLE_TIMESTAMP

// helper functions
inline const char* enum_to_text(const char* const array[], uint32_t nof_types, uint32_t enum_val)
{
  return enum_val >= nof_types ? "" : array[enum_val];
}

template <class ItemType>
constexpr ItemType enum_to_number(ItemType* array, uint32_t nof_types, uint32_t enum_val)
{
  return enum_val >= nof_types ? -1 : array[enum_val];
}

enum class srsran_rat_t { lte, nr, nulltype };
inline std::string to_string(const srsran_rat_t& type)
{
  constexpr static const char* options[] = {"LTE", "NR"};
  return enum_to_text(options, (uint32_t)srsran_rat_t::nulltype, (uint32_t)type);
}

} // namespace srsran

#endif // SRSRAN_COMMON_H
