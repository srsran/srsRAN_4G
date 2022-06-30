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

#ifndef SRSRAN_RRC_CONFIG_H
#define SRSRAN_RRC_CONFIG_H

namespace srsue {

#define SRSRAN_RRC_N_BANDS 43

struct rrc_args_t {
  std::string                             ue_category_str;
  uint32_t                                ue_category;
  int                                     ue_category_ul;
  int                                     ue_category_dl;
  uint32_t                                release;
  uint32_t                                feature_group;
  std::array<uint8_t, SRSRAN_RRC_N_BANDS> supported_bands;
  std::vector<uint32_t>                   supported_bands_nr;
  uint32_t                                nof_supported_bands;
  bool                                    support_ca;
  int                                     mbms_service_id;
  uint32_t                                mbms_service_port;
};

#define SRSRAN_UE_CATEGORY_DEFAULT "4"
#define SRSRAN_UE_CATEGORY_MIN 1
#define SRSRAN_UE_CATEGORY_MAX 21
#define SRSRAN_RELEASE_MIN 8
#define SRSRAN_RELEASE_MAX 15
#define SRSRAN_RELEASE_DEFAULT (SRSRAN_RELEASE_MIN)

} // namespace srsue

#endif // SRSRAN_RRC_CONFIG_H
