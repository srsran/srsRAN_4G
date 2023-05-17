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
#include "srsran/phy/phch/pbch_msg_nr.h"
#include "srsran/phy/utils/bit.h"
#include "srsran/phy/utils/vector.h"

static bool pbch_msg_nr_is_mib(const srsran_pbch_msg_nr_t* msg)
{
  return msg->payload[0] == 0;
}

bool srsran_pbch_msg_nr_is_mib(const srsran_pbch_msg_nr_t* msg)
{
  if (msg == NULL) {
    return false;
  }

  return pbch_msg_nr_is_mib(msg);
}

int srsran_pbch_msg_nr_mib_pack(const srsran_mib_nr_t* mib, srsran_pbch_msg_nr_t* pbch_msg)
{
  if (mib == NULL || pbch_msg == NULL) {
    return SRSRAN_ERROR_INVALID_INPUTS;
  }

  // Copy PBCH message context
  pbch_msg->sfn_4lsb  = mib->sfn & 0b1111;
  pbch_msg->ssb_idx   = mib->ssb_idx;
  pbch_msg->k_ssb_msb = mib->ssb_offset >> 4U;
  pbch_msg->hrf       = mib->hrf;

  // Pack MIB payload
  uint8_t* y = pbch_msg->payload;

  // MIB - 1 bit
  *(y++) = 0;

  // systemFrameNumber - 6 bits MSB
  srsran_bit_unpack(mib->sfn >> 4U, &y, 6);

  // subCarrierSpacingCommon - 1 bit
  *(y++) = (mib->scs_common == srsran_subcarrier_spacing_15kHz || mib->scs_common == srsran_subcarrier_spacing_60kHz)
               ? 0
               : 1;

  // ssb-SubcarrierOffset - 4 bits
  srsran_bit_unpack(mib->ssb_offset, &y, 4);

  // dmrs-TypeA-Position - 1 bit
  *(y++) = (mib->dmrs_typeA_pos == srsran_dmrs_sch_typeA_pos_2) ? 0 : 1;

  // pdcch-ConfigSIB1
  // controlResourceSetZero - 4 bits
  srsran_bit_unpack(mib->coreset0_idx, &y, 4);

  // searchSpaceZero - 4 bits
  srsran_bit_unpack(mib->ss0_idx, &y, 4);

  // Barred - 1 bit
  *(y++) = (mib->cell_barred) ? 0 : 1;

  // intraFreqReselection - 1 bit
  *(y++) = (mib->intra_freq_reselection) ? 0 : 1;

  // Spare - 1 bit
  srsran_bit_unpack(mib->spare, &y, 1);

  return SRSRAN_SUCCESS;
}

int srsran_pbch_msg_nr_mib_unpack(const srsran_pbch_msg_nr_t* pbch_msg, srsran_mib_nr_t* mib)
{
  if (mib == NULL || pbch_msg == NULL) {
    return SRSRAN_ERROR_INVALID_INPUTS;
  }

  // Copy PBCH message context
  mib->sfn        = pbch_msg->sfn_4lsb;
  mib->ssb_idx    = pbch_msg->ssb_idx;
  mib->hrf        = pbch_msg->hrf;
  mib->ssb_offset = pbch_msg->k_ssb_msb << 4U;

  // Pack MIB payload
  uint8_t* y = (uint8_t*)pbch_msg->payload;

  // MIB - 1 bit
  if (!pbch_msg_nr_is_mib(pbch_msg)) {
    return SRSRAN_ERROR;
  }
  y++;

  // systemFrameNumber - 6 bits MSB
  mib->sfn |= srsran_bit_pack(&y, 6) << 4U;

  // subCarrierSpacingCommon - 1 bit
  mib->scs_common = *(y++) == 0 ? srsran_subcarrier_spacing_15kHz : srsran_subcarrier_spacing_30kHz;

  // ssb-SubcarrierOffset - 4 bits
  mib->ssb_offset |= srsran_bit_pack(&y, 4);

  // dmrs-TypeA-Position - 1 bit
  mib->dmrs_typeA_pos = *(y++) == 0 ? srsran_dmrs_sch_typeA_pos_2 : srsran_dmrs_sch_typeA_pos_3;

  // pdcch-ConfigSIB1
  // controlResourceSetZero - 4 bits
  mib->coreset0_idx = srsran_bit_pack(&y, 4);

  // searchSpaceZero - 4 bits
  mib->ss0_idx = srsran_bit_pack(&y, 4);

  // Barred - 1 bit
  mib->cell_barred = (*(y++) == 0);

  // intraFreqReselection - 1 bit
  mib->intra_freq_reselection = (*(y++) == 0);

  // Spare - 1 bit
  mib->spare = srsran_bit_pack(&y, 1);

  return SRSRAN_SUCCESS;
}

uint32_t srsran_pbch_msg_info(const srsran_pbch_msg_nr_t* msg, char* str, uint32_t str_len)
{
  if (msg == NULL || str == NULL || str_len == 0) {
    return 0;
  }

  uint32_t len = 0;

  len = srsran_print_check(str, str_len, len, "payload=");

  len += srsran_vec_sprint_hex(&str[len], str_len - len, (uint8_t*)msg->payload, SRSRAN_PBCH_MSG_NR_SZ);

  len = srsran_print_check(str,
                           str_len,
                           len,
                           " sfn_lsb=%d ssb_idx=%d k_ssb_msb=%d hrf=%d ",
                           msg->sfn_4lsb,
                           msg->ssb_idx,
                           msg->k_ssb_msb,
                           msg->hrf);

  return len;
}

uint32_t srsran_pbch_msg_nr_mib_info(const srsran_mib_nr_t* mib, char* str, uint32_t str_len)
{
  uint32_t len = 0;

  len = srsran_print_check(str,
                           str_len,
                           len,
                           "sfn=%d ssb_idx=%d hrf=%c scs=%d ssb_offset=%d dmrs_typeA_pos=%s coreset0=%d ss0=%d "
                           "barred=%c intra_freq_reselection=%c spare=%d",
                           mib->sfn,
                           mib->ssb_idx,
                           mib->hrf ? 'y' : 'n',
                           SRSRAN_SUBC_SPACING_NR(mib->scs_common) / 1000,
                           mib->ssb_offset,
                           mib->dmrs_typeA_pos == srsran_dmrs_sch_typeA_pos_2 ? "pos2" : "pos3",
                           mib->coreset0_idx,
                           mib->ss0_idx,
                           mib->cell_barred ? 'y' : 'n',
                           mib->intra_freq_reselection ? 'y' : 'n',
                           mib->spare);

  return len;
}
