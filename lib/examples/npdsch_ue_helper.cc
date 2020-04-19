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

#include "npdsch_ue_helper.h"
#include "srslte/asn1/rrc_asn1_nbiot.h"
#include "srslte/phy/utils/vector.h" // for SRSLTE_MIN

int get_sib2_params(const uint8_t* sib1_payload, const uint32_t len, srslte_nbiot_si_params_t* sib2_params)
{
  memset(sib2_params, 0, sizeof(srslte_nbiot_si_params_t));

  // unpack SIB
  asn1::rrc::bcch_dl_sch_msg_nb_s dlsch_msg;
  asn1::cbit_ref                  dlsch_bref(sib1_payload, len);
  asn1::SRSASN_CODE               err = dlsch_msg.unpack(dlsch_bref);
  if (err != asn1::SRSASN_SUCCESS) {
    fprintf(stderr, "Error unpacking DL-SCH message\n");
    return SRSLTE_ERROR;
  }

  // set SIB2-NB parameters
  sib2_params->n              = 1;
  auto sched_info             = dlsch_msg.msg.c1().sib_type1_r13().sched_info_list_r13.begin();
  sib2_params->si_periodicity = sched_info->si_periodicity_r13.to_number();
  if (dlsch_msg.msg.c1().sib_type1_r13().si_radio_frame_offset_r13_present) {
    sib2_params->si_radio_frame_offset = dlsch_msg.msg.c1().sib_type1_r13().si_radio_frame_offset_r13;
  }
  sib2_params->si_repetition_pattern = sched_info->si_repeat_pattern_r13.to_number();
  sib2_params->si_tb                 = sched_info->si_tb_r13.to_number();
  sib2_params->si_window_length      = dlsch_msg.msg.c1().sib_type1_r13().si_win_len_r13.to_number();

  return SRSLTE_SUCCESS;
}

/*
 * Unpacks BCCH BCH message and writes JSON formatted message to output
 */
int bcch_bch_to_pretty_string(const uint8_t* bcch_bch_payload,
                              const uint32_t input_len,
                              char*          output,
                              const uint32_t max_output_len)
{
  asn1::rrc::bcch_bch_msg_nb_s bch_msg;
  asn1::cbit_ref               bch_bref(bcch_bch_payload, input_len);
  asn1::SRSASN_CODE            err = bch_msg.unpack(bch_bref);
  if (err != asn1::SRSASN_SUCCESS) {
    fprintf(stderr, "Error unpacking BCCH message\n");
    return SRSLTE_ERROR;
  }

  asn1::json_writer json_writer;
  bch_msg.to_json(json_writer);

  uint32_t output_len = SRSLTE_MIN(max_output_len, json_writer.to_string().length());
  memcpy(output, json_writer.to_string().c_str(), output_len);

  return SRSLTE_SUCCESS;
}

/*
 * Unpacks BCCH DL-SCH message and writes JSON formatted message to output
 */
int bcch_dl_sch_to_pretty_string(const uint8_t* bcch_dl_sch_payload,
                                 const uint32_t input_len,
                                 char*          output,
                                 const uint32_t max_output_len)
{
  asn1::rrc::bcch_dl_sch_msg_nb_s dlsch_msg;
  asn1::cbit_ref                  dlsch_bref(bcch_dl_sch_payload, input_len);
  asn1::SRSASN_CODE               err = dlsch_msg.unpack(dlsch_bref);
  if (err != asn1::SRSASN_SUCCESS) {
    fprintf(stderr, "Error unpacking DL-SCH message\n");
    return SRSLTE_ERROR;
  }

  asn1::json_writer json_writer;
  dlsch_msg.to_json(json_writer);

  uint32_t output_len = SRSLTE_MIN(max_output_len, json_writer.to_string().length());
  memcpy(output, json_writer.to_string().c_str(), output_len);

  return SRSLTE_SUCCESS;
}