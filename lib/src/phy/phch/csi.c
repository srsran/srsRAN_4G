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
#include "srsran/phy/phch/csi.h"
#include "srsran/phy/utils/bit.h"
#include "srsran/phy/utils/debug.h"
#include "srsran/phy/utils/vector.h"
#include <math.h>

#define CSI_WIDEBAND_CSI_NOF_BITS 4

/// Implements SNRI to CQI conversion
uint32_t csi_snri_db_to_cqi(srsran_csi_cqi_table_t table, float snri_db)
{
  return 15;
}

// Implements CSI report triggers
static bool csi_report_trigger(const srsran_csi_hl_report_cfg_t* cfg, uint32_t slot_idx)
{
  switch (cfg->type) {
    case SRSRAN_CSI_REPORT_TYPE_PERIODIC:
      return (slot_idx + cfg->periodic.period - cfg->periodic.offset) % cfg->periodic.period == 0;
    default:; // Do nothing
  }
  return false;
}

static void csi_wideband_cri_ri_pmi_cqi_quantify(const srsran_csi_hl_report_cfg_t* cfg,
                                                 const srsran_csi_measurements_t*  channel_meas,
                                                 const srsran_csi_measurements_t*  interf_meas,
                                                 srsran_csi_report_cfg_t*          report_cfg,
                                                 srsran_csi_report_value_t*        report_value)
{
  // Take SNR by default
  float wideband_sinr_db = channel_meas->wideband_snr_db;

  // If interference is provided, use the channel RSRP and interference EPRE to calculate the SINR
  if (interf_meas != NULL) {
    wideband_sinr_db = channel_meas->wideband_rsrp_dBm - interf_meas->wideband_epre_dBm;
  }

  // Fill report configuration
  report_cfg->type      = cfg->type;
  report_cfg->quantity  = SRSRAN_CSI_REPORT_QUANTITY_CRI_RI_PMI_CQI;
  report_cfg->freq_cfg  = SRSRAN_CSI_REPORT_FREQ_WIDEBAND;
  report_cfg->nof_ports = channel_meas->nof_ports;
  report_cfg->K_csi_rs  = channel_meas->K_csi_rs;

  // Save PUCCH resource only if periodic type
  if (cfg->type == SRSRAN_CSI_REPORT_TYPE_PERIODIC) {
    report_cfg->pucch_resource = cfg->periodic.resource;
  }

  // Fill quantified values
  report_value->wideband_cri_ri_pmi_cqi.cqi = csi_snri_db_to_cqi(cfg->cqi_table, wideband_sinr_db);
  report_value->wideband_cri_ri_pmi_cqi.ri  = 0;
  report_value->wideband_cri_ri_pmi_cqi.pmi = 0;
}

static uint32_t csi_wideband_cri_ri_pmi_cqi_nof_bits(const srsran_csi_report_cfg_t* cfg)
{
  // Compute number of bits for CRI
  uint32_t nof_bits_cri = 0;
  if (cfg->K_csi_rs > 0) {
    nof_bits_cri = (uint32_t)ceilf(log2f((float)cfg->K_csi_rs));
  }

  switch (cfg->nof_ports) {
    case 1:
      return CSI_WIDEBAND_CSI_NOF_BITS + nof_bits_cri;
    default:
      ERROR("Invalid or not implemented number of ports (%d)", cfg->nof_ports);
  }
  return 0;
}

static uint32_t csi_wideband_cri_ri_pmi_cqi_pack(const srsran_csi_report_cfg_t*   cfg,
                                                 const srsran_csi_report_value_t* value,
                                                 uint8_t*                         o_csi1)
{
  // Compute number of bits for CRI
  uint32_t nof_bits_cri = 0;
  if (cfg->K_csi_rs > 0) {
    nof_bits_cri = (uint32_t)ceilf(log2f((float)cfg->K_csi_rs));
  }

  // Write wideband CQI
  srsran_bit_unpack(value->wideband_cri_ri_pmi_cqi.cqi, &o_csi1, CSI_WIDEBAND_CSI_NOF_BITS);

  // Compute number of bits for CRI and write
  srsran_bit_unpack(value->cri, &o_csi1, nof_bits_cri);

  return nof_bits_cri + CSI_WIDEBAND_CSI_NOF_BITS;
}

static uint32_t csi_wideband_cri_ri_pmi_cqi_unpack(const srsran_csi_report_cfg_t* cfg,
                                                   uint8_t*                       o_csi1,
                                                   srsran_csi_report_value_t*     value)
{
  // Compute number of bits for CRI
  uint32_t nof_bits_cri = 0;
  if (cfg->K_csi_rs > 0) {
    nof_bits_cri = (uint32_t)ceilf(log2f((float)cfg->K_csi_rs));
  }

  // Write wideband CQI
  value->wideband_cri_ri_pmi_cqi.cqi = srsran_bit_pack(&o_csi1, CSI_WIDEBAND_CSI_NOF_BITS);

  // Compute number of bits for CRI and write
  value->cri = srsran_bit_pack(&o_csi1, nof_bits_cri);

  return nof_bits_cri + CSI_WIDEBAND_CSI_NOF_BITS;
}

static uint32_t csi_none_nof_bits(const srsran_csi_report_cfg_t* cfg)
{
  return cfg->K_csi_rs;
}

static uint32_t
csi_none_pack(const srsran_csi_report_cfg_t* cfg, const srsran_csi_report_value_t* value, uint8_t* o_csi1)
{
  srsran_vec_u8_copy(o_csi1, (uint8_t*)value->none, cfg->K_csi_rs);

  return cfg->K_csi_rs;
}

static uint32_t
csi_none_unpack(const srsran_csi_report_cfg_t* cfg, const uint8_t* o_csi1, srsran_csi_report_value_t* value)
{
  srsran_vec_u8_copy((uint8_t*)value->none, o_csi1, cfg->K_csi_rs);

  return cfg->K_csi_rs;
}

int srsran_csi_generate_reports(const srsran_csi_hl_cfg_t*      cfg,
                                uint32_t                        slot_idx,
                                const srsran_csi_measurements_t measurements[SRSRAN_CSI_MAX_NOF_RESOURCES],
                                srsran_csi_report_cfg_t         report_cfg[SRSRAN_CSI_MAX_NOF_REPORT],
                                srsran_csi_report_value_t       report_value[SRSRAN_CSI_MAX_NOF_REPORT])
{
  uint32_t count = 0;

  // Check inputs
  if (cfg == NULL || measurements == NULL || report_cfg == NULL || report_value == NULL) {
    return SRSRAN_ERROR_INVALID_INPUTS;
  }

  // Iterate every possible configured CSI report
  for (uint32_t i = 0; i < SRSRAN_CSI_MAX_NOF_REPORT; i++) {
    // Skip if report is not configured or triggered
    if (!csi_report_trigger(&cfg->reports[i], slot_idx)) {
      continue;
    }

    // Select channel measurement
    if (cfg->reports->channel_meas_id >= SRSRAN_CSI_MAX_NOF_RESOURCES) {
      ERROR("Channel measurement ID (%d) is out of range", cfg->reports->channel_meas_id);
      return SRSRAN_ERROR;
    }
    const srsran_csi_measurements_t* channel_meas = &measurements[cfg->reports->channel_meas_id];

    // Select interference measurement
    const srsran_csi_measurements_t* interf_meas = NULL;
    if (cfg->reports->interf_meas_present) {
      if (cfg->reports->interf_meas_id >= SRSRAN_CSI_MAX_NOF_RESOURCES) {
        ERROR("Interference measurement ID (%d) is out of range", cfg->reports->interf_meas_id);
        return SRSRAN_ERROR;
      }
      interf_meas = &measurements[cfg->reports->interf_meas_id];
    }

    // Quantify measurements according to frequency and quantity configuration
    if (cfg->reports->freq_cfg == SRSRAN_CSI_REPORT_FREQ_WIDEBAND &&
        cfg->reports->quantity == SRSRAN_CSI_REPORT_QUANTITY_CRI_RI_PMI_CQI) {
      csi_wideband_cri_ri_pmi_cqi_quantify(
          &cfg->reports[i], channel_meas, interf_meas, &report_cfg[count], &report_value[count]);
      count++;
    } else {
      ; // Ignore other types
    }
  }

  return (int)count;
}

int srsran_csi_part1_nof_bits(const srsran_csi_report_cfg_t* report_list, uint32_t nof_reports)
{
  uint32_t count = 0;

  // Check input pointer
  if (report_list == NULL) {
    return SRSRAN_ERROR_INVALID_INPUTS;
  }

  // Iterate all report configurations
  for (uint32_t i = 0; i < nof_reports; i++) {
    const srsran_csi_report_cfg_t* report = &report_list[i];
    if (report->quantity && report->quantity == SRSRAN_CSI_REPORT_QUANTITY_CRI_RI_PMI_CQI) {
      count += csi_wideband_cri_ri_pmi_cqi_nof_bits(report);
    } else if (report->quantity == SRSRAN_CSI_REPORT_QUANTITY_NONE) {
      count += csi_none_nof_bits(report);
    }
  }

  return (int)count;
}

int srsran_csi_part2_nof_bits(const srsran_csi_report_cfg_t* report_list, uint32_t nof_reports)
{
  uint32_t count = 0;

  // Check input pointer
  if (report_list == NULL) {
    return SRSRAN_ERROR_INVALID_INPUTS;
  }

  return (int)count;
}

bool srsran_csi_has_part2(const srsran_csi_report_cfg_t* report_list, uint32_t nof_reports)
{
  if (report_list == NULL || nof_reports == 0) {
    return false;
  }

  for (uint32_t i = 0; i < nof_reports; i++) {
    if (report_list[i].has_part2) {
      return true;
    }
  }
  return false;
}

int srsran_csi_part1_pack(const srsran_csi_report_cfg_t*   report_cfg,
                          const srsran_csi_report_value_t* report_value,
                          uint32_t                         nof_reports,
                          uint8_t*                         o_csi1,
                          uint32_t                         max_o_csi1)
{
  uint32_t count = 0;

  if (report_cfg == NULL || report_value == NULL || o_csi1 == NULL) {
    return SRSRAN_ERROR_INVALID_INPUTS;
  }

  int n = srsran_csi_part1_nof_bits(report_cfg, nof_reports);
  if (n > (int)max_o_csi1) {
    ERROR("The maximum number of CSI bits (%d) is not enough to accommodate %d bits", max_o_csi1, n);
    return SRSRAN_ERROR;
  }

  for (uint32_t i = 0; i < nof_reports && count < max_o_csi1; i++) {
    if (report_cfg[i].freq_cfg == SRSRAN_CSI_REPORT_FREQ_WIDEBAND &&
        report_cfg[i].quantity == SRSRAN_CSI_REPORT_QUANTITY_CRI_RI_PMI_CQI) {
      count += csi_wideband_cri_ri_pmi_cqi_pack(&report_cfg[i], &report_value[i], &o_csi1[count]);
    } else if (report_cfg[i].quantity == SRSRAN_CSI_REPORT_QUANTITY_NONE) {
      count += csi_none_pack(&report_cfg[i], &report_value[i], &o_csi1[count]);
    } else {
      ERROR("CSI frequency (%d) and quantity (%d) combination is not implemented",
            report_cfg[i].freq_cfg,
            report_cfg[i].quantity);
    }
  }

  return (int)count;
}

int srsran_csi_part1_unpack(const srsran_csi_report_cfg_t* report_cfg,
                            uint32_t                       nof_reports,
                            uint8_t*                       o_csi1,
                            uint32_t                       max_o_csi1,
                            srsran_csi_report_value_t*     report_value)
{
  uint32_t count = 0;

  if (report_cfg == NULL || report_value == NULL || o_csi1 == NULL) {
    return SRSRAN_ERROR_INVALID_INPUTS;
  }

  int n = srsran_csi_part1_nof_bits(report_cfg, nof_reports);
  if (n > (int)max_o_csi1) {
    ERROR("The maximum number of CSI bits (%d) is not enough to accommodate %d bits", max_o_csi1, n);
    return SRSRAN_ERROR;
  }

  for (uint32_t i = 0; i < nof_reports && count < max_o_csi1; i++) {
    if (report_cfg[i].freq_cfg == SRSRAN_CSI_REPORT_FREQ_WIDEBAND &&
        report_cfg[i].quantity == SRSRAN_CSI_REPORT_QUANTITY_CRI_RI_PMI_CQI) {
      count += csi_wideband_cri_ri_pmi_cqi_unpack(&report_cfg[i], &o_csi1[count], &report_value[i]);
    } else if (report_cfg[i].quantity == SRSRAN_CSI_REPORT_QUANTITY_NONE) {
      count += csi_none_unpack(&report_cfg[i], &o_csi1[count], &report_value[i]);
    } else {
      ERROR("CSI frequency (%d) and quantity (%d) combination is not implemented",
            report_cfg[i].freq_cfg,
            report_cfg[i].quantity);
    }
  }

  return (int)count;
}

uint32_t srsran_csi_str(const srsran_csi_report_cfg_t*   report_cfg,
                        const srsran_csi_report_value_t* report_value,
                        uint32_t                         nof_reports,
                        char*                            str,
                        uint32_t                         str_len)
{
  uint32_t len = 0;
  for (uint32_t i = 0; i < nof_reports; i++) {
    if (report_cfg[i].freq_cfg == SRSRAN_CSI_REPORT_FREQ_WIDEBAND &&
        report_cfg[i].quantity == SRSRAN_CSI_REPORT_QUANTITY_CRI_RI_PMI_CQI) {
      len = srsran_print_check(str, str_len, len, "cqi=%d ", report_value[i].wideband_cri_ri_pmi_cqi.cqi);
    } else if (report_cfg[i].quantity == SRSRAN_CSI_REPORT_QUANTITY_NONE) {
      char tmp[20] = {};
      srsran_vec_sprint_bin(tmp, sizeof(tmp), report_value[i].none, report_cfg->K_csi_rs);
      len = srsran_print_check(str, str_len, len, "csi=%s ", tmp);
    }
  }
  return len;
}
