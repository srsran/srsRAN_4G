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
  uint32_t                                nof_lte_carriers;
  uint32_t                                nof_nr_carriers;
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
