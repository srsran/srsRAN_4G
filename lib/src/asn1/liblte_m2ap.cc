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
#include "srslte/asn1/liblte_m2ap.h"
#include <stdarg.h>
#include <stdio.h>

/*******************************************************************************
                              LOGGING
*******************************************************************************/

static log_handler_t log_handler;
static void*         callback_ctx = NULL;

void liblte_log_register_handler(void* ctx, log_handler_t handler)
{
  log_handler  = handler;
  callback_ctx = ctx;
}

static void liblte_log_print(const char* format, ...)
{
  va_list args;
  va_start(args, format);
  if (log_handler) {
    char* args_msg = NULL;
    if (vasprintf(&args_msg, format, args) > 0) {
      log_handler(callback_ctx, args_msg);
    }
    if (args_msg) {
      free(args_msg);
    }
  } else {
    vprintf(format, args);
  }
  va_end(args);
}

/*******************************************************************************
 * Procedure code criticality lookups
 *******************************************************************************/
LIBLTE_M2AP_CRITICALITY_ENUM liblte_m2ap_procedure_criticality(uint8_t procedureCode)
{
  switch (procedureCode) {
    case LIBLTE_M2AP_PROC_ID_SESSIONSTART:
      return LIBLTE_M2AP_CRITICALITY_REJECT;
    case LIBLTE_M2AP_PROC_ID_SESSIONSTOP:
      return LIBLTE_M2AP_CRITICALITY_REJECT;
    case LIBLTE_M2AP_PROC_ID_MBMSSCHEDULINGINFORMATION:
      return LIBLTE_M2AP_CRITICALITY_REJECT;
    case LIBLTE_M2AP_PROC_ID_ERRORINDICATION:
      return LIBLTE_M2AP_CRITICALITY_REJECT;
    case LIBLTE_M2AP_PROC_ID_RESET:
      return LIBLTE_M2AP_CRITICALITY_REJECT;
    case LIBLTE_M2AP_PROC_ID_M2SETUP:
      return LIBLTE_M2AP_CRITICALITY_REJECT;
    case LIBLTE_M2AP_PROC_ID_ENBCONFIGURATIONUPDATE:
      return LIBLTE_M2AP_CRITICALITY_REJECT;
    case LIBLTE_M2AP_PROC_ID_MCECONFIGURATIONUPDATE:
      return LIBLTE_M2AP_CRITICALITY_REJECT;
    case LIBLTE_M2AP_PROC_ID_PRIVATEMESSAGE:
      return LIBLTE_M2AP_CRITICALITY_REJECT;
    case LIBLTE_M2AP_PROC_ID_SESSIONUPDATE:
      return LIBLTE_M2AP_CRITICALITY_REJECT;
    case LIBLTE_M2AP_PROC_ID_MBMSSERVICECOUNTING:
      return LIBLTE_M2AP_CRITICALITY_REJECT;
    case LIBLTE_M2AP_PROC_ID_MBMSSERVICECOUNTINGRESULTSREPORT:
      return LIBLTE_M2AP_CRITICALITY_REJECT;
  }
  return LIBLTE_M2AP_CRITICALITY_REJECT;
}

/*******************************************************************************
 * ProtocolIE-Field
 *******************************************************************************/
LIBLTE_ERROR_ENUM
liblte_m2ap_pack_protocolie_header(uint32_t len, uint32_t ie_id, LIBLTE_M2AP_CRITICALITY_ENUM crit, uint8_t** ptr)
{
  liblte_value_2_bits(ie_id, ptr, 16); // ProtocolIE-ID
  liblte_value_2_bits(crit, ptr, 2);   // Criticality
  liblte_align_up_zero(ptr, 8);
  if (len < 128) { // Length
    liblte_value_2_bits(0, ptr, 1);
    liblte_value_2_bits(len, ptr, 7);
  } else if (len < 16383) {
    liblte_value_2_bits(1, ptr, 1);
    liblte_value_2_bits(0, ptr, 1);
    liblte_value_2_bits(len, ptr, 14);
  } else {
    // TODO: Unlikely to have more than 16K of octets
  }

  return LIBLTE_SUCCESS;
}

LIBLTE_ERROR_ENUM
liblte_m2ap_unpack_protocolie_header(uint8_t** ptr, uint32_t* ie_id, LIBLTE_M2AP_CRITICALITY_ENUM* crit, uint32_t* len)
{
  *ie_id = liblte_bits_2_value(ptr, 16);                              // ProtocolIE-ID
  *crit  = (LIBLTE_M2AP_CRITICALITY_ENUM)liblte_bits_2_value(ptr, 2); // Criticality
  liblte_align_up(ptr, 8);
  if (0 == liblte_bits_2_value(ptr, 1)) { // Length
    *len = liblte_bits_2_value(ptr, 7);
  } else {
    if (0 == liblte_bits_2_value(ptr, 1)) {
      *len = liblte_bits_2_value(ptr, 14);
    } else {
      // TODO: Unlikely to have more than 16K of octets
    }
  }
  return LIBLTE_SUCCESS;
}

/*******************************************************************************
 * ProtocolIE ProtocolIE_ID INTEGER
 *******************************************************************************/
LIBLTE_ERROR_ENUM liblte_m2ap_pack_protocolie_id(LIBLTE_M2AP_PROTOCOLIE_ID_STRUCT* ie, uint8_t** ptr)
{
  LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

  if (ie != NULL && ptr != NULL) {
    // Integer - ie->ProtocolIE_ID
    // lb:0, ub:65535
    liblte_align_up_zero(ptr, 8);
    liblte_value_2_bits(0, ptr, (2 * 8) - 16);
    liblte_value_2_bits(ie->ProtocolIE_ID, ptr, 16);
    liblte_align_up_zero(ptr, 8);
    err = LIBLTE_SUCCESS;
  }
  return err;
}

LIBLTE_ERROR_ENUM liblte_m2ap_unpack_protocolie_id(uint8_t** ptr, LIBLTE_M2AP_PROTOCOLIE_ID_STRUCT* ie)
{
  LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

  if (ie != NULL && ptr != NULL) {
    // Integer - ie->ProtocolIE_ID
    // lb:0, ub:65535
    liblte_align_up(ptr, 8);
    ie->ProtocolIE_ID = (uint16_t)liblte_bits_2_value(ptr, 16);
    err               = LIBLTE_SUCCESS;
  }
  return err;
}

/*******************************************************************************
 * ProtocolIE ProtocolIE_SingleContainer SEQUENCE
 *******************************************************************************/
LIBLTE_ERROR_ENUM liblte_m2ap_pack_protocolie_singlecontainer(LIBLTE_M2AP_PROTOCOLIE_SINGLECONTAINER_STRUCT* ie,
                                                              uint8_t**                                      ptr)
{
  LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

  if (ie != NULL && ptr != NULL) {

    liblte_value_2_bits(ie->id.ProtocolIE_ID, ptr, 16); // ProtocolIE-ID
    liblte_value_2_bits(ie->criticality, ptr, 2);       // Criticality
    liblte_align_up_zero(ptr, 8);
    // if(liblte_m2ap_pack_protocolie_id(&ie->id, ptr) != LIBLTE_SUCCESS) {
    //  return LIBLTE_ERROR_ENCODE_FAIL;
    //}

    // Enum - ie->criticality
    // liblte_value_2_bits(ie->criticality, ptr, 8);
    err = LIBLTE_SUCCESS;
  }
  return err;
}

LIBLTE_ERROR_ENUM liblte_m2ap_unpack_protocolie_singlecontainer(uint8_t**                                      ptr,
                                                                LIBLTE_M2AP_PROTOCOLIE_SINGLECONTAINER_STRUCT* ie)
{
  LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

  if (ie != NULL && ptr != NULL) {

    if (liblte_m2ap_unpack_protocolie_id(ptr, &ie->id) != LIBLTE_SUCCESS) {
      return LIBLTE_ERROR_DECODE_FAIL;
    }

    // Enum - ie->criticality
    ie->criticality = (LIBLTE_M2AP_CRITICALITY_ENUM)liblte_bits_2_value(ptr, 2);
    liblte_align_up(ptr, 8);

    err = LIBLTE_SUCCESS;
  }
  return err;
}

/*******************************************************************************
 * ProtocolIE ProtocolExtensionID INTEGER
 *******************************************************************************/
LIBLTE_ERROR_ENUM liblte_m2ap_pack_protocolextensionid(LIBLTE_M2AP_PROTOCOLEXTENSIONID_STRUCT* ie, uint8_t** ptr)
{
  LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

  if (ie != NULL && ptr != NULL) {
    // Integer - ie->ProtocolExtensionID
    // lb:0, ub:65535
    liblte_align_up_zero(ptr, 8);
    liblte_value_2_bits(0, ptr, (2 * 8) - 16);
    liblte_value_2_bits(ie->ProtocolExtensionID, ptr, 16);
    liblte_align_up_zero(ptr, 8);
    err = LIBLTE_SUCCESS;
  }
  return err;
}

LIBLTE_ERROR_ENUM liblte_m2ap_unpack_protocolextensionid(uint8_t** ptr, LIBLTE_M2AP_PROTOCOLEXTENSIONID_STRUCT* ie)
{
  LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

  if (ie != NULL && ptr != NULL) {
    // Integer - ie->ProtocolExtensionID
    // lb:0, ub:65535
    liblte_align_up(ptr, 8);
    ie->ProtocolExtensionID = (uint16_t)liblte_bits_2_value(ptr, 2.0 * 8);
    liblte_align_up(ptr, 8);
    err = LIBLTE_SUCCESS;
  }
  return err;
}

/*******************************************************************************
 * ProtocolIE ProtocolExtensionField SEQUENCE
 *******************************************************************************/
LIBLTE_ERROR_ENUM liblte_m2ap_pack_protocolextensionfield(LIBLTE_M2AP_PROTOCOLEXTENSIONFIELD_STRUCT* ie, uint8_t** ptr)
{
  LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

  if (ie != NULL && ptr != NULL) {

    if (liblte_m2ap_pack_protocolextensionid(&ie->id, ptr) != LIBLTE_SUCCESS) {
      return LIBLTE_ERROR_ENCODE_FAIL;
    }

    // Enum - ie->criticality
    liblte_value_2_bits(ie->criticality, ptr, 2);

    err = LIBLTE_SUCCESS;
  }
  return err;
}

LIBLTE_ERROR_ENUM liblte_m2ap_unpack_protocolextensionfield(uint8_t**                                  ptr,
                                                            LIBLTE_M2AP_PROTOCOLEXTENSIONFIELD_STRUCT* ie)
{
  LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

  if (ie != NULL && ptr != NULL) {

    if (liblte_m2ap_unpack_protocolextensionid(ptr, &ie->id) != LIBLTE_SUCCESS) {
      return LIBLTE_ERROR_DECODE_FAIL;
    }

    // Enum - ie->criticality
    ie->criticality = (LIBLTE_M2AP_CRITICALITY_ENUM)liblte_bits_2_value(ptr, 2);

    err = LIBLTE_SUCCESS;
  }
  return err;
}

/*******************************************************************************
 * ProtocolIE ProtocolExtensionContainer DYNAMIC SEQUENCE OF
 *******************************************************************************/
// lb:1, ub:65535
LIBLTE_ERROR_ENUM liblte_m2ap_pack_protocolextensioncontainer(LIBLTE_M2AP_PROTOCOLEXTENSIONCONTAINER_STRUCT* ie,
                                                              uint8_t**                                      ptr)
{
  LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

  if (ie != NULL && ptr != NULL) {
    if (ie->len > 32) {
      liblte_log_print(
          "ProtocolExtensionContainer pack error - max supported dynamic sequence length = 32, ie->len = %d\n",
          ie->len);
      return LIBLTE_ERROR_ENCODE_FAIL;
    }
    // Length
    liblte_value_2_bits(ie->len - 1, ptr, 16);
    liblte_align_up_zero(ptr, 8);
    for (uint32_t i = 0; i < ie->len; i++) {
      if (liblte_m2ap_pack_protocolextensionfield(&ie->buffer[i], ptr) != LIBLTE_SUCCESS) {
        return LIBLTE_ERROR_ENCODE_FAIL;
      }
    }
    err = LIBLTE_SUCCESS;
  }
  return err;
}

LIBLTE_ERROR_ENUM liblte_m2ap_unpack_protocolextensioncontainer(uint8_t**                                      ptr,
                                                                LIBLTE_M2AP_PROTOCOLEXTENSIONCONTAINER_STRUCT* ie)
{
  LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

  if (ie != NULL && ptr != NULL) {
    // Length
    ie->len = liblte_bits_2_value(ptr, 16) + 1;
    liblte_align_up(ptr, 8);
    if (ie->len > 32) {
      liblte_log_print(
          "ProtocolExtensionContainer unpack error - max supported dynamic sequence length = 32, ie->len = %d\n",
          ie->len);
      return LIBLTE_ERROR_DECODE_FAIL;
    }
    for (uint32_t i = 0; i < ie->len; i++) {
      if (liblte_m2ap_unpack_protocolextensionfield(ptr, &ie->buffer[i]) != LIBLTE_SUCCESS) {
        return LIBLTE_ERROR_DECODE_FAIL;
      }
    }
    err = LIBLTE_SUCCESS;
  }
  return err;
}

/*******************************************************************************
 * ProtocolIE Criticality Diagnostics SEQUENCE
 *******************************************************************************/
LIBLTE_ERROR_ENUM liblte_m2ap_pack_criticalitydiagnostics(LIBLTE_M2AP_CRITICALITYDIAGNOSTICS_STRUCT* ie, uint8_t** ptr)
{
  LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

  if (ie != NULL && ptr != NULL) {
    liblte_log_print("CriticalityDiagnostics pack error - Criticality Diagnostics not supported yet\n");
    return LIBLTE_ERROR_ENCODE_FAIL;
  }
  return err;
}

LIBLTE_ERROR_ENUM liblte_m2ap_unpack_criticalitydiagnostics(uint8_t**                                  ptr,
                                                            LIBLTE_M2AP_CRITICALITYDIAGNOSTICS_STRUCT* ie)
{
  LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

  if (ie != NULL && ptr != NULL) {
    liblte_log_print("CriticalityDiagnostics unpack error - criticality diagnostics not supported yet\n");
    return LIBLTE_ERROR_DECODE_FAIL;
  }
  return err;
}
/*******************************************************************************
 * ProtocolIE PLMNidentity STATIC OCTET STRING
 *******************************************************************************/
LIBLTE_ERROR_ENUM liblte_m2ap_pack_plmnidentity(LIBLTE_M2AP_PLMN_IDENTITY_STRUCT* ie, uint8_t** ptr)
{
  LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

  if (ie != NULL && ptr != NULL) {
    // Static octet string
    if (LIBLTE_M2AP_PLMN_IDENTITY_OCTET_STRING_LEN > 2) { // X.691 Sec.16
      liblte_align_up_zero(ptr, 8);
    }
    // Octets
    uint32_t i;
    for (i = 0; i < LIBLTE_M2AP_PLMN_IDENTITY_OCTET_STRING_LEN; i++) {
      liblte_value_2_bits(ie->buffer[i], ptr, 8);
    }
    err = LIBLTE_SUCCESS;
  }
  return err;
}

LIBLTE_ERROR_ENUM liblte_m2ap_unpack_plmnidentity(uint8_t** ptr, LIBLTE_M2AP_PLMN_IDENTITY_STRUCT* ie)
{
  LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

  if (ie != NULL && ptr != NULL) {

    // Static octet string
    if (LIBLTE_M2AP_PLMN_IDENTITY_OCTET_STRING_LEN > 2) { // X.691 Sec.16
      liblte_align_up(ptr, 8);
    }
    // Octets
    uint32_t i;
    for (i = 0; i < LIBLTE_M2AP_PLMN_IDENTITY_OCTET_STRING_LEN; i++) {
      ie->buffer[i] = liblte_bits_2_value(ptr, 8);
    }
    err = LIBLTE_SUCCESS;
  }
  return err;
}

/*******************************************************************************
 * ProtocolIE IP address OCTET STRING
 *******************************************************************************/
LIBLTE_ERROR_ENUM liblte_m2ap_pack_ipaddress(LIBLTE_M2AP_IP_ADDRESS_STRUCT* ie, uint8_t** ptr)
{
  LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

  if (ie != NULL && ptr != NULL) {
    liblte_value_2_bits(ie->len - 4, ptr, 4);
    liblte_align_up_zero(ptr, 8);
    // Octets
    for (uint8_t i = 0; i < ie->len; i++) {
      liblte_value_2_bits(ie->buffer[i], ptr, 8);
    }
    err = LIBLTE_SUCCESS;
  }
  return err;
}

LIBLTE_ERROR_ENUM liblte_m2ap_unpack_ipaddress(uint8_t** ptr, LIBLTE_M2AP_IP_ADDRESS_STRUCT* ie)
{
  LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

  if (ie != NULL && ptr != NULL) {

    ie->len = liblte_bits_2_value(ptr, 4) + 4;
    liblte_align_up(ptr, 8);
    if (ie->len > 16) {
      return LIBLTE_ERROR_DECODE_FAIL;
    }
    for (uint8_t i = 0; i < ie->len; i++) {
      ie->buffer[i] = liblte_bits_2_value(ptr, 8);
    }
    err = LIBLTE_SUCCESS;
  }
  return err;
}

/*******************************************************************************
 * ProtocolIE GTP TEID OCTET STRING
 *******************************************************************************/
LIBLTE_ERROR_ENUM liblte_m2ap_pack_gtpteid(LIBLTE_M2AP_GTP_TEID_STRUCT* ie, uint8_t** ptr)
{
  LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

  if (ie != NULL && ptr != NULL) {

    // Octets
    uint32_t i;
    for (i = 0; i < 4; i++) {
      liblte_value_2_bits(ie->buffer[i], ptr, 8);
    }
    err = LIBLTE_SUCCESS;
  }
  return err;
}

LIBLTE_ERROR_ENUM liblte_m2ap_unpack_gtpteid(uint8_t** ptr, LIBLTE_M2AP_GTP_TEID_STRUCT* ie)
{
  LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

  if (ie != NULL && ptr != NULL) {
    for (int i = 0; i < 4; i++) {
      ie->buffer[i] = liblte_bits_2_value(ptr, 8);
    }
    err = LIBLTE_SUCCESS;
  }
  return err;
}
/*******************************************************************************
 * ProtocolIE Service Id STATIC OCTET STRING
 *******************************************************************************/
LIBLTE_ERROR_ENUM liblte_m2ap_pack_serviceid(LIBLTE_M2AP_SERVICE_ID_STRUCT* ie, uint8_t** ptr)
{
  LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

  if (ie != NULL && ptr != NULL) {
    // Static octet string
    if (LIBLTE_M2AP_SERVICE_ID_OCTET_STRING_LEN > 2) { // X.691 Sec.16
      liblte_align_up_zero(ptr, 8);
    }
    // Octets
    uint32_t i;
    for (i = 0; i < LIBLTE_M2AP_SERVICE_ID_OCTET_STRING_LEN; i++) {
      liblte_value_2_bits(ie->buffer[i], ptr, 8);
    }
    err = LIBLTE_SUCCESS;
  }
  return err;
}

LIBLTE_ERROR_ENUM liblte_m2ap_unpack_serviceid(uint8_t** ptr, LIBLTE_M2AP_SERVICE_ID_STRUCT* ie)
{
  LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

  if (ie != NULL && ptr != NULL) {
    // Static octet string
    if (LIBLTE_M2AP_SERVICE_ID_OCTET_STRING_LEN > 2) { // X.691 Sec.16
      liblte_align_up(ptr, 8);
    }
    // Octets
    uint32_t i;
    for (i = 0; i < LIBLTE_M2AP_SERVICE_ID_OCTET_STRING_LEN; i++) {
      ie->buffer[i] = liblte_bits_2_value(ptr, 8);
    }
    err = LIBLTE_SUCCESS;
  }
  return err;
}
/*******************************************************************************
 * ProtocolIE ENBname PrintableString
 *******************************************************************************/
LIBLTE_ERROR_ENUM liblte_m2ap_pack_enbname(LIBLTE_M2AP_ENBNAME_STRUCT* ie, uint8_t** ptr)
{
  LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

  if (ie != NULL && ptr != NULL) {
    // Printable string - ENBname
    // Extension
    liblte_value_2_bits(ie->ext ? 1 : 0, ptr, 1);
    if (ie->ext) {
      liblte_log_print("ENBname error: M2AP ASN extensions not currently supported\n");
      return LIBLTE_ERROR_ENCODE_FAIL;
    }

    // Length
    liblte_value_2_bits(ie->n_octets - 1, ptr, 8);
    liblte_align_up_zero(ptr, 8);

    uint32_t i;
    for (i = 0; i < ie->n_octets; i++) {
      liblte_value_2_bits(ie->buffer[i], ptr, 8);
    }
    err = LIBLTE_SUCCESS;
  }
  return err;
}

LIBLTE_ERROR_ENUM liblte_m2ap_unpack_enbname(uint8_t** ptr, LIBLTE_M2AP_ENBNAME_STRUCT* ie)
{
  LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

  if (ie != NULL && ptr != NULL) {
    // Printable string - ENBname
    // Extension
    ie->ext = liblte_bits_2_value(ptr, 1);
    if (ie->ext) {
      liblte_log_print("ENBname error: M2AP ASN extensions not currently supported\n");
      return LIBLTE_ERROR_DECODE_FAIL;
    }

    // Length
    ie->n_octets = liblte_bits_2_value(ptr, 8) + 1;
    liblte_align_up(ptr, 8);

    uint32_t i;
    for (i = 0; i < ie->n_octets; i++) {
      ie->buffer[i] = liblte_bits_2_value(ptr, 8);
    }
    err = LIBLTE_SUCCESS;
  }
  return err;
}

/*******************************************************************************
 * ProtocolIE MCEname PrintableString
 *******************************************************************************/
LIBLTE_ERROR_ENUM liblte_m2ap_pack_mcename(LIBLTE_M2AP_MCE_NAME_STRUCT* ie, uint8_t** ptr)
{
  LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

  if (ie != NULL && ptr != NULL) {
    // Printable string - ENBname
    // Extension
    liblte_value_2_bits(ie->ext ? 1 : 0, ptr, 1);
    if (ie->ext) {
      liblte_log_print("MCEname error: M2AP ASN extensions not currently supported\n");
      return LIBLTE_ERROR_ENCODE_FAIL;
    }

    // Length
    liblte_value_2_bits(ie->n_octets - 1, ptr, 8);
    liblte_align_up_zero(ptr, 8);

    uint32_t i;
    for (i = 0; i < ie->n_octets; i++) {
      liblte_value_2_bits(ie->buffer[i], ptr, 8);
    }
    err = LIBLTE_SUCCESS;
  }
  return err;
}

LIBLTE_ERROR_ENUM liblte_m2ap_unpack_mcename(uint8_t** ptr, LIBLTE_M2AP_MCE_NAME_STRUCT* ie)
{
  LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

  if (ie != NULL && ptr != NULL) {
    // Printable string - ENBname
    // Extension
    ie->ext = liblte_bits_2_value(ptr, 1);
    if (ie->ext) {
      liblte_log_print("MCEname error: M2AP ASN extensions not currently supported\n");
      return LIBLTE_ERROR_DECODE_FAIL;
    }

    // Length
    ie->n_octets = liblte_bits_2_value(ptr, 8) + 1;
    liblte_align_up(ptr, 8);

    uint32_t i;
    for (i = 0; i < ie->n_octets; i++) {
      ie->buffer[i] = liblte_bits_2_value(ptr, 8);
    }
    err = LIBLTE_SUCCESS;
  }
  return err;
}

/*******************************************************************************
 * ProtocolIE ENB-ID STATIC BIT STRING
 *******************************************************************************/
LIBLTE_ERROR_ENUM liblte_m2ap_pack_enbid(LIBLTE_M2AP_ENB_ID_STRUCT* ie, uint8_t** ptr)
{
  LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

  if (ie != NULL && ptr != NULL) {
    // Extension
    liblte_value_2_bits(ie->ext ? 1 : 0, ptr, 1);
    if (ie->ext) {
      liblte_log_print("ENBname error: M2AP ASN extensions not currently supported\n");
      return LIBLTE_ERROR_ENCODE_FAIL;
    }
    liblte_align_up(ptr, 8);

    // Static bit string - eNB-Id
    uint32_t i;
    for (i = 0; i < LIBLTE_M2AP_ENBID_BIT_STRING_LEN; i++) {
      liblte_value_2_bits(ie->buffer[i], ptr, 1);
    }
    err = LIBLTE_SUCCESS;
  }
  return err;
}

LIBLTE_ERROR_ENUM liblte_m2ap_unpack_enbid(uint8_t** ptr, LIBLTE_M2AP_ENB_ID_STRUCT* ie)
{
  LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

  if (ie != NULL && ptr != NULL) {
    // Extension
    ie->ext = liblte_bits_2_value(ptr, 1);
    if (ie->ext) {
      liblte_log_print("ENBname error: M2AP ASN extensions not currently supported\n");
      return LIBLTE_ERROR_DECODE_FAIL;
    }

    // Static bit string - eNB-Id
    liblte_align_up_zero(ptr, 8);
    uint32_t i;
    for (i = 0; i < LIBLTE_M2AP_ENBID_BIT_STRING_LEN; i++) {
      ie->buffer[i] = liblte_bits_2_value(ptr, 1);
    }
    liblte_align_up_zero(ptr, 8);
    err = LIBLTE_SUCCESS;
  }
  return err;
}

/*******************************************************************************
 * ProtocolIE Global_ENB_ID SEQUENCE
 *******************************************************************************/
LIBLTE_ERROR_ENUM liblte_m2ap_pack_globalenbid(LIBLTE_M2AP_GLOBAL_ENB_ID_STRUCT* ie, uint8_t** ptr)
{
  LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

  if (ie != NULL && ptr != NULL) {
    // Extension
    liblte_value_2_bits(ie->ext ? 1 : 0, ptr, 1);
    if (ie->ext) {
      liblte_log_print("Global_ENB_ID error: M2AP ASN extensions not currently supported\n");
      return LIBLTE_ERROR_ENCODE_FAIL;
    }

    liblte_value_2_bits(ie->iE_Extensions_present ? 1 : 0, ptr, 1);
    liblte_align_up_zero(ptr, 8);

    if (liblte_m2ap_pack_plmnidentity(&ie->pLMNidentity, ptr) != LIBLTE_SUCCESS) {
      return LIBLTE_ERROR_ENCODE_FAIL;
    }

    if (liblte_m2ap_pack_enbid(&ie->eNB_ID, ptr) != LIBLTE_SUCCESS) {
      return LIBLTE_ERROR_ENCODE_FAIL;
    }

    if (ie->iE_Extensions_present) {
      if (liblte_m2ap_pack_protocolextensioncontainer(&ie->iE_Extensions, ptr) != LIBLTE_SUCCESS) {
        return LIBLTE_ERROR_ENCODE_FAIL;
      }
    }
    err = LIBLTE_SUCCESS;
  }
  return err;
}

LIBLTE_ERROR_ENUM liblte_m2ap_unpack_globalenbid(uint8_t** ptr, LIBLTE_M2AP_GLOBAL_ENB_ID_STRUCT* ie)
{
  LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

  if (ie != NULL && ptr != NULL) {
    // Extension
    ie->ext = liblte_bits_2_value(ptr, 1);
    if (ie->ext) {
      liblte_log_print("Global_ENB_ID error: M2AP ASN extensions not currently supported\n");
      return LIBLTE_ERROR_DECODE_FAIL;
    }

    ie->iE_Extensions_present = liblte_bits_2_value(ptr, 1);

    if (liblte_m2ap_unpack_plmnidentity(ptr, &ie->pLMNidentity) != LIBLTE_SUCCESS) {
      return LIBLTE_ERROR_DECODE_FAIL;
    }

    if (liblte_m2ap_unpack_enbid(ptr, &ie->eNB_ID) != LIBLTE_SUCCESS) {
      return LIBLTE_ERROR_DECODE_FAIL;
    }

    if (ie->iE_Extensions_present) {
      if (liblte_m2ap_unpack_protocolextensioncontainer(ptr, &ie->iE_Extensions) != LIBLTE_SUCCESS) {
        return LIBLTE_ERROR_DECODE_FAIL;
      }
    }
    err = LIBLTE_SUCCESS;
  }
  return err;
}

/*******************************************************************************
 * ProtocolIE ECGI SEQUENCE
 *******************************************************************************/
LIBLTE_ERROR_ENUM liblte_m2ap_pack_ecgi(LIBLTE_M2AP_ECGI_STRUCT* ie, uint8_t** ptr)
{
  LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;
  if (ie != NULL && ptr != NULL) {
    // Extension present
    liblte_value_2_bits(ie->ext ? 1 : 0, ptr, 1);
    if (ie->ext) {
      liblte_log_print("eNB-MBMS-Configuration-Data Item error: M2AP ASN extensions not currently supported\n");
      return LIBLTE_ERROR_ENCODE_FAIL;
    }
    // IE Extension present
    liblte_value_2_bits(ie->iE_Extensions_present ? 1 : 0, ptr, 1);
    if (ie->iE_Extensions_present) {
      liblte_log_print("eNB-MBMS-Configuration-Data Item error: M2AP ASN extensions not currently supported\n");
      return LIBLTE_ERROR_ENCODE_FAIL;
    }
    // PLMN Identity
    if (liblte_m2ap_pack_plmnidentity(&ie->pLMN_Identity, ptr) != LIBLTE_SUCCESS) {
      return LIBLTE_ERROR_ENCODE_FAIL;
    }
    // E-UTRAN Cell Identifier
    if (liblte_m2ap_pack_eutrancellidentifier(&ie->EUTRANCellIdentifier, ptr) != LIBLTE_SUCCESS) {
      return LIBLTE_ERROR_DECODE_FAIL;
    }

    if (ie->iE_Extensions_present) {
      if (liblte_m2ap_pack_protocolextensioncontainer(&ie->iE_Extensions, ptr) != LIBLTE_SUCCESS) {
        return LIBLTE_ERROR_DECODE_FAIL;
      }
    }
    err = LIBLTE_SUCCESS;
  }
  return err;
}

LIBLTE_ERROR_ENUM liblte_m2ap_unpack_ecgi(uint8_t** ptr, LIBLTE_M2AP_ECGI_STRUCT* ie)
{
  LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

  if (ie != NULL && ptr != NULL) {
    // Extension
    ie->ext = liblte_bits_2_value(ptr, 1);
    if (ie->ext) {
      liblte_log_print("ECGI error: M2AP ASN extensions not currently supported\n");
      return LIBLTE_ERROR_DECODE_FAIL;
    }
    ie->iE_Extensions_present = liblte_bits_2_value(ptr, 1);

    if (liblte_m2ap_unpack_plmnidentity(ptr, &ie->pLMN_Identity) != LIBLTE_SUCCESS) {
      return LIBLTE_ERROR_DECODE_FAIL;
    }

    if (liblte_m2ap_unpack_eutrancellidentifier(ptr, &ie->EUTRANCellIdentifier) != LIBLTE_SUCCESS) {
      return LIBLTE_ERROR_DECODE_FAIL;
    }

    if (ie->iE_Extensions_present) {
      if (liblte_m2ap_unpack_protocolextensioncontainer(ptr, &ie->iE_Extensions) != LIBLTE_SUCCESS) {
        return LIBLTE_ERROR_DECODE_FAIL;
      }
    }
    err = LIBLTE_SUCCESS;
  }
  return err;
}

/*******************************************************************************
 * ProtocolIE MCE-ID STATIC OCTET STRING
 *******************************************************************************/
LIBLTE_ERROR_ENUM liblte_m2ap_pack_mceid(LIBLTE_M2AP_MCE_ID_STRUCT* ie, uint8_t** ptr)
{
  LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

  if (ie != NULL && ptr != NULL) {
    // Static octet string
    if (LIBLTE_M2AP_MCEID_OCTET_STRING_LEN > 2) { // X.691 Sec.16
      liblte_align_up_zero(ptr, 8);
    }
    // Octets
    uint32_t i;
    for (i = 0; i < LIBLTE_M2AP_MCEID_OCTET_STRING_LEN; i++) {
      liblte_value_2_bits(ie->buffer[i], ptr, 8);
    }
    err = LIBLTE_SUCCESS;
  }
  return err;
}

LIBLTE_ERROR_ENUM liblte_m2ap_unpack_mceid(uint8_t** ptr, LIBLTE_M2AP_MCE_ID_STRUCT* ie)
{
  LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

  if (ie != NULL && ptr != NULL) {
    // Static octet string
    if (LIBLTE_M2AP_MCEID_OCTET_STRING_LEN > 2) { // X.691 Sec.16
      liblte_align_up(ptr, 8);
    }
    // Octets
    uint32_t i;
    for (i = 0; i < LIBLTE_M2AP_MCEID_OCTET_STRING_LEN; i++) {
      ie->buffer[i] = liblte_bits_2_value(ptr, 8);
    }
    err = LIBLTE_SUCCESS;
  }
  return err;
}
/*******************************************************************************
 * ProtocolIE Global_MCE_ID SEQUENCE
 *******************************************************************************/
LIBLTE_ERROR_ENUM liblte_m2ap_pack_globalmceid(LIBLTE_M2AP_GLOBAL_MCE_ID_STRUCT* ie, uint8_t** ptr)
{
  LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

  if (ie != NULL && ptr != NULL) {

    // Extension
    liblte_value_2_bits(ie->ext ? 1 : 0, ptr, 1);
    if (ie->ext) {
      liblte_log_print("Global_ENB_ID error: M2AP ASN extensions not currently supported\n");
      return LIBLTE_ERROR_ENCODE_FAIL;
    }
    liblte_align_up_zero(ptr, 8);

    if (liblte_m2ap_pack_plmnidentity(&ie->pLMN_Identity, ptr) != LIBLTE_SUCCESS) {
      return LIBLTE_ERROR_ENCODE_FAIL;
    }

    if (liblte_m2ap_pack_mceid(&ie->mCE_ID, ptr) != LIBLTE_SUCCESS) {
      return LIBLTE_ERROR_ENCODE_FAIL;
    }

    if (ie->iE_Extensions_present) {
      if (liblte_m2ap_pack_protocolextensioncontainer(&ie->iE_extensions, ptr) != LIBLTE_SUCCESS) {
        return LIBLTE_ERROR_ENCODE_FAIL;
      }
    }
    err = LIBLTE_SUCCESS;
  }
  return err;
}

LIBLTE_ERROR_ENUM liblte_m2ap_unpack_globalmceid(uint8_t** ptr, LIBLTE_M2AP_GLOBAL_MCE_ID_STRUCT* ie)
{
  LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

  if (ie != NULL && ptr != NULL) {
    // Extension
    ie->ext = liblte_bits_2_value(ptr, 1);
    if (ie->ext) {
      liblte_log_print("Global_ENB_ID error: M2AP ASN extensions not currently supported\n");
      return LIBLTE_ERROR_DECODE_FAIL;
    }

    ie->iE_Extensions_present = liblte_bits_2_value(ptr, 1);

    if (liblte_m2ap_unpack_plmnidentity(ptr, &ie->pLMN_Identity) != LIBLTE_SUCCESS) {
      return LIBLTE_ERROR_DECODE_FAIL;
    }

    if (liblte_m2ap_unpack_mceid(ptr, &ie->mCE_ID) != LIBLTE_SUCCESS) {
      return LIBLTE_ERROR_DECODE_FAIL;
    }

    if (ie->iE_Extensions_present) {
      if (liblte_m2ap_unpack_protocolextensioncontainer(ptr, &ie->iE_extensions) != LIBLTE_SUCCESS) {
        return LIBLTE_ERROR_DECODE_FAIL;
      }
    }
    err = LIBLTE_SUCCESS;
  }
  return err;
}
/*******************************************************************************
 * ProtocolIE EUTRANCellIdentifier STATIC BIT STRING
 *******************************************************************************/
LIBLTE_ERROR_ENUM liblte_m2ap_pack_eutrancellidentifier(LIBLTE_M2AP_EUTRAN_CELL_IDENTIFIER_STRUCT* ie, uint8_t** ptr)
{
  LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

  if (ie != NULL && ptr != NULL) {
    // Static bit string - E-UTRAN Cell Identifier
    liblte_value_2_bits(ie->eUTRANCellIdentifier, ptr, LIBLTE_M2AP_EUTRAN_CELL_IDENTIFIER_BIT_STRING_LEN);
    err = LIBLTE_SUCCESS;
  }
  return err;
}

LIBLTE_ERROR_ENUM liblte_m2ap_unpack_eutrancellidentifier(uint8_t** ptr, LIBLTE_M2AP_EUTRAN_CELL_IDENTIFIER_STRUCT* ie)
{
  LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

  if (ie != NULL && ptr != NULL) {
    // Static bit string - E-UTRAN Cell Identifier
    ie->eUTRANCellIdentifier = liblte_bits_2_value(ptr, LIBLTE_M2AP_EUTRAN_CELL_IDENTIFIER_BIT_STRING_LEN);
    err                      = LIBLTE_SUCCESS;
  }
  return err;
}

/*******************************************************************************
 * ProtocolIE MBSFN-Synchronization-Area-Id INTEGER
 *******************************************************************************/
LIBLTE_ERROR_ENUM liblte_m2ap_pack_mbsfnsynchronisationareaid(LIBLTE_M2AP_MBSFN_SYNCHRONISATION_AREA_ID_STRUCT* ie,
                                                              uint8_t**                                         ptr)
{
  LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;
  if (ie != NULL && ptr != NULL) {
    // Integer - ie->local
    // lb:0, ub:65535
    liblte_align_up_zero(ptr, 8);
    liblte_value_2_bits(ie->mbsfn_synchronisation_area_id, ptr, 16);
    err = LIBLTE_SUCCESS;
  }
  return err;
}
LIBLTE_ERROR_ENUM liblte_m2ap_unpack_mbsfnsynchronisationareaid(uint8_t**                                         ptr,
                                                                LIBLTE_M2AP_MBSFN_SYNCHRONISATION_AREA_ID_STRUCT* ie)
{
  LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;
  if (ie != NULL && ptr != NULL) {
    // Integer - ie->local
    // lb:0, ub:65535
    liblte_align_up(ptr, 8);
    ie->mbsfn_synchronisation_area_id = liblte_bits_2_value(ptr, 16);
    err                               = LIBLTE_SUCCESS;
  }
  return err;
}

/*******************************************************************************
 * ProtocolIE MBMS-Service-Area-List DYNAMIC OCTET STRING
 *******************************************************************************/
// lb:0, ub:512
LIBLTE_ERROR_ENUM liblte_m2ap_pack_mbmsserviceareaidlist(LIBLTE_M2AP_MBMS_SERVICE_AREA_ID_LIST_STRUCT* ie,
                                                         uint8_t**                                     ptr)
{
  LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;
  liblte_value_2_bits(ie->len - 1, ptr, 8);
  for (uint8_t i = 0; i < ie->len; i++) {
    liblte_m2ap_pack_mbmsservicearea(&ie->buffer[i], ptr);
  }
  err = LIBLTE_SUCCESS;
  return err;
}

LIBLTE_ERROR_ENUM liblte_m2ap_unpack_mbmsserviceareaidlist(uint8_t**                                     ptr,
                                                           LIBLTE_M2AP_MBMS_SERVICE_AREA_ID_LIST_STRUCT* ie)
{
  LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;
  if (ie != NULL && ptr != NULL) {
    ie->len = liblte_bits_2_value(ptr, 8) + 1;
    for (uint8_t i = 0; i < ie->len; i++) {
      liblte_m2ap_unpack_mbmsservicearea(ptr, &ie->buffer[i]);
    }
    err = LIBLTE_SUCCESS;
  }
  return err;
}

/*******************************************************************************
 * ProtocolIE MBMS-Service-Area DYNAMIC OCTET STRING
 *******************************************************************************/
// lb:0, ub:512
LIBLTE_ERROR_ENUM liblte_m2ap_pack_mbmsservicearea(LIBLTE_M2AP_MBMS_SERVICE_AREA_STRUCT* ie, uint8_t** ptr)
{
  LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;
  if (ie != NULL && ptr != NULL) {
    if (ie->n_octets < 127) { // Length
      liblte_value_2_bits(0, ptr, 1);
      liblte_value_2_bits(ie->n_octets, ptr, 7);
    } else {
      if (ie->n_octets < 16384) {
        liblte_value_2_bits(0, ptr, 1);
        liblte_value_2_bits(ie->n_octets, ptr, 14);
      } else {
        // too large
        return LIBLTE_ERROR_ENCODE_FAIL;
      }
    }
    for (int i = 0; i < ie->n_octets; i++) {
      liblte_value_2_bits(ie->buffer[i], ptr, 8);
    }
    err = LIBLTE_SUCCESS;
  }
  return err;
}
LIBLTE_ERROR_ENUM liblte_m2ap_unpack_mbmsservicearea(uint8_t** ptr, LIBLTE_M2AP_MBMS_SERVICE_AREA_STRUCT* ie)
{
  LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;
  if (ie != NULL && ptr != NULL) {
    if (0 == liblte_bits_2_value(ptr, 1)) { // Length
      ie->n_octets = liblte_bits_2_value(ptr, 7);
    } else {
      if (0 == liblte_bits_2_value(ptr, 1)) {
        ie->n_octets = liblte_bits_2_value(ptr, 14);
      } else {
        // TODO: Unlikely to have more than 16K of octets
      }
    }
    // ie->n_octets = liblte_bits_2_value(ptr,8);
    for (int i = 0; i < ie->n_octets; i++) {
      ie->buffer[i] = liblte_bits_2_value(ptr, 8);
    }
    err = LIBLTE_SUCCESS;
  }
  return err;
}

/*******************************************************************************
 * ProtocolIE ENB-MBMS-ConfigurationDataItem SEQUENCE
 *******************************************************************************/
LIBLTE_ERROR_ENUM liblte_m2ap_pack_enbmbmsconfigurationdataitem(LIBLTE_M2AP_ENB_MBMS_CONFIGURATION_DATA_ITEM_STRUCT* ie,
                                                                uint8_t** ptr)
{
  LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

  if (ie != NULL && ptr != NULL) {
    // Extension present
    liblte_value_2_bits(ie->ext ? 1 : 0, ptr, 1);
    if (ie->ext) {
      liblte_log_print("eNB-MBMS-Configuration-Data Item error: M2AP ASN extensions not currently supported\n");
      return LIBLTE_ERROR_ENCODE_FAIL;
    }
    // IE Extension present
    liblte_value_2_bits(ie->iE_Extensions_present ? 1 : 0, ptr, 1);
    if (ie->iE_Extensions_present) {
      liblte_log_print("eNB-MBMS-Configuration-Data Item error: M2AP ASN extensions not currently supported\n");
      return LIBLTE_ERROR_ENCODE_FAIL;
    }

    // eCGI
    err = liblte_m2ap_pack_ecgi(&ie->eCGI, ptr);
    if (err != LIBLTE_SUCCESS)
      return err;

    // MBSFN Synchronization Area Id
    if (liblte_m2ap_pack_mbsfnsynchronisationareaid(&ie->mbsfnSynchronisationArea, ptr) != LIBLTE_SUCCESS) {
      return LIBLTE_ERROR_ENCODE_FAIL;
    }

    // MBMS Service area id
    if (liblte_m2ap_pack_mbmsserviceareaidlist(&ie->mbmsServiceAreaList, ptr) != LIBLTE_SUCCESS) {
      return LIBLTE_ERROR_ENCODE_FAIL;
    }
    err = LIBLTE_SUCCESS;
  }
  return err;
}

LIBLTE_ERROR_ENUM
liblte_m2ap_unpack_enbmbmsconfigurationdataitem(uint8_t** ptr, LIBLTE_M2AP_ENB_MBMS_CONFIGURATION_DATA_ITEM_STRUCT* ie)
{
  LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

  if (ie != NULL && ptr != NULL) {
    // Extension
    ie->ext = liblte_bits_2_value(ptr, 1);
    if (ie->ext) {
      liblte_log_print("eNB-MBMS-Configuration-Data Item error: M2AP ASN extensions not currently supported\n");
      return LIBLTE_ERROR_DECODE_FAIL;
    }
    ie->iE_Extensions_present = liblte_bits_2_value(ptr, 1);
    if (liblte_m2ap_unpack_ecgi(ptr, &ie->eCGI) != LIBLTE_SUCCESS) {
      return LIBLTE_ERROR_DECODE_FAIL;
    }
    if (liblte_m2ap_unpack_mbsfnsynchronisationareaid(ptr, &ie->mbsfnSynchronisationArea) != LIBLTE_SUCCESS) {
      return LIBLTE_ERROR_DECODE_FAIL;
    }
    if (liblte_m2ap_unpack_mbmsserviceareaidlist(ptr, &ie->mbmsServiceAreaList) != LIBLTE_SUCCESS) {
      return LIBLTE_ERROR_DECODE_FAIL;
    }
    if (ie->iE_Extensions_present) {
      if (liblte_m2ap_unpack_protocolextensioncontainer(ptr, &ie->iE_Extensions) != LIBLTE_SUCCESS) {
        return LIBLTE_ERROR_DECODE_FAIL;
      }
    }
    err = LIBLTE_SUCCESS;
  }
  return err;
}

/*******************************************************************************
 * ProtocolIE MCCH-Update-Time INTEGER (0..255)
 *******************************************************************************/
// lb:0, ub:255
LIBLTE_ERROR_ENUM liblte_m2ap_pack_mcchupdatetime(LIBLTE_M2AP_MCCH_UPDATE_TIME_STRUCT* ie, uint8_t** ptr)
{
  LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;
  if (ie != NULL && ptr != NULL) {
    liblte_value_2_bits(ie->mcchUpdateTime, ptr, 8);
    err = LIBLTE_SUCCESS;
  }
  return err;
}
LIBLTE_ERROR_ENUM liblte_m2ap_unpack_mcchupdatetime(uint8_t** ptr, LIBLTE_M2AP_MCCH_UPDATE_TIME_STRUCT* ie)
{
  LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;
  if (ie != NULL && ptr != NULL) {
    ie->mcchUpdateTime = liblte_bits_2_value(ptr, 8);
    err                = LIBLTE_SUCCESS;
  }
  return err;
}

/*******************************************************************************
 * ProtocolIE MBSFN-Area-Id INTEGER (0..255)
 *******************************************************************************/
// lb:0, ub:512
LIBLTE_ERROR_ENUM liblte_m2ap_pack_mbsfnareaid(LIBLTE_M2AP_MBSFN_AREA_ID_STRUCT* ie, uint8_t** ptr)
{
  LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;
  if (ie != NULL && ptr != NULL) {
    liblte_value_2_bits(ie->mbsfn_area_id, ptr, 8);
    err = LIBLTE_SUCCESS;
  }
  return err;
}
LIBLTE_ERROR_ENUM liblte_m2ap_unpack_mbsfnareaid(uint8_t** ptr, LIBLTE_M2AP_MBSFN_AREA_ID_STRUCT* ie)
{
  LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;
  if (ie != NULL && ptr != NULL) {
    ie->mbsfn_area_id = liblte_bits_2_value(ptr, 8);
    err               = LIBLTE_SUCCESS;
  }
  return err;
}

/*******************************************************************************
 * ProtocolIE LCID INTEGER (0..28)
 *******************************************************************************/
// lb:0, ub:28
LIBLTE_ERROR_ENUM liblte_m2ap_pack_lcid(LIBLTE_M2AP_LCID_STRUCT* ie, uint8_t** ptr)
{
  LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;
  if (ie != NULL && ptr != NULL) {
    liblte_value_2_bits(ie->lcid, ptr, 5);
    err = LIBLTE_SUCCESS;
  }
  return err;
}
LIBLTE_ERROR_ENUM liblte_m2ap_unpack_lcid(uint8_t** ptr, LIBLTE_M2AP_LCID_STRUCT* ie)
{
  LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;
  if (ie != NULL && ptr != NULL) {
    ie->lcid = liblte_bits_2_value(ptr, 5);
    err      = LIBLTE_SUCCESS;
  }
  return err;
}

/*******************************************************************************
 * ProtocolIE Data MCS INTEGER (0..28)
 *******************************************************************************/
// lb:0, ub:28
LIBLTE_ERROR_ENUM liblte_m2ap_pack_datamcs(LIBLTE_M2AP_DATA_MCS_STRUCT* ie, uint8_t** ptr)
{
  LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;
  if (ie != NULL && ptr != NULL) {
    liblte_value_2_bits(ie->dataMCS, ptr, 5);
    err = LIBLTE_SUCCESS;
  }
  return err;
}
LIBLTE_ERROR_ENUM liblte_m2ap_unpack_datamcs(uint8_t** ptr, LIBLTE_M2AP_DATA_MCS_STRUCT* ie)
{
  LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;
  if (ie != NULL && ptr != NULL) {
    ie->dataMCS = liblte_bits_2_value(ptr, 5);
    err         = LIBLTE_SUCCESS;
  }
  return err;
}

/*******************************************************************************
 * ProtocolIE Radio Frame Allocation Offset INTEGER (0..7)
 *******************************************************************************/
// lb:0, ub:7
LIBLTE_ERROR_ENUM liblte_m2ap_pack_radioframeallocationoffset(LIBLTE_M2AP_RADIOFRAME_ALLOCATION_OFFSET_STRUCT* ie,
                                                              uint8_t**                                        ptr)
{
  LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;
  if (ie != NULL && ptr != NULL) {
    liblte_value_2_bits(ie->radioframeAllocationOffset, ptr, 3);
    err = LIBLTE_SUCCESS;
  }
  return err;
}
LIBLTE_ERROR_ENUM liblte_m2ap_unpack_radioframeallocationoffset(uint8_t**                                        ptr,
                                                                LIBLTE_M2AP_RADIOFRAME_ALLOCATION_OFFSET_STRUCT* ie)
{
  LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;
  if (ie != NULL && ptr != NULL) {
    ie->radioframeAllocationOffset = liblte_bits_2_value(ptr, 3);
    err                            = LIBLTE_SUCCESS;
  }
  return err;
}

/*******************************************************************************
 * ProtocolIE AllocatedSubframesEnd INTEGER (0..1535)
 *******************************************************************************/
// lb:0, ub:1535
LIBLTE_ERROR_ENUM liblte_m2ap_pack_allocatedsubframesend(LIBLTE_M2AP_ALLOCATED_SUBFRAMES_END_STRUCT* ie, uint8_t** ptr)
{
  LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;
  if (ie != NULL && ptr != NULL) {
    liblte_value_2_bits(ie->allocated_subframes_end, ptr, 16);
    err = LIBLTE_SUCCESS;
  }
  return err;
}
LIBLTE_ERROR_ENUM liblte_m2ap_unpack_allocatedsubframesend(uint8_t**                                   ptr,
                                                           LIBLTE_M2AP_ALLOCATED_SUBFRAMES_END_STRUCT* ie)
{
  LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;
  if (ie != NULL && ptr != NULL) {
    ie->allocated_subframes_end = liblte_bits_2_value(ptr, 16);
    err                         = LIBLTE_SUCCESS;
  }
  return err;
}
/*******************************************************************************
 * ProtocolIE MCE-MBMS-M2AP-ID INTEGER (0..16777215)
 *******************************************************************************/
LIBLTE_ERROR_ENUM liblte_m2ap_pack_mcembmsm2apid(LIBLTE_M2AP_MCE_MBMS_M2AP_ID_STRUCT* ie, uint8_t** ptr)
{
  LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;
  if (ie != NULL && ptr != NULL) {
    if (ie->mce_mbms_m2ap_id < 256) {
      liblte_value_2_bits(0, ptr, 2);
      liblte_align_up_zero(ptr, 8);
      liblte_value_2_bits(ie->mce_mbms_m2ap_id, ptr, 8);
    } else if (ie->mce_mbms_m2ap_id < 65536) {
      liblte_value_2_bits(1, ptr, 2);
      liblte_align_up_zero(ptr, 8);
      liblte_value_2_bits(ie->mce_mbms_m2ap_id, ptr, 16);
    } else if (ie->mce_mbms_m2ap_id < 16777215) {
      liblte_value_2_bits(2, ptr, 2);
      liblte_align_up_zero(ptr, 8);
      liblte_value_2_bits(ie->mce_mbms_m2ap_id, ptr, 24);
    } else {
      return LIBLTE_ERROR_ENCODE_FAIL;
    }
    err = LIBLTE_SUCCESS;
  }
  return err;
}
LIBLTE_ERROR_ENUM liblte_m2ap_unpack_mcembmsm2apid(uint8_t** ptr, LIBLTE_M2AP_MCE_MBMS_M2AP_ID_STRUCT* ie)
{
  LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;
  if (ie != NULL && ptr != NULL) {
    uint8_t value = liblte_bits_2_value(ptr, 2);
    liblte_align_up(ptr, 8);
    if (value == 0) {
      ie->mce_mbms_m2ap_id = liblte_bits_2_value(ptr, 8);
    } else if (value == 1) {
      ie->mce_mbms_m2ap_id = liblte_bits_2_value(ptr, 16);
    } else if (value == 2) {
      ie->mce_mbms_m2ap_id = liblte_bits_2_value(ptr, 24);
    } else {
      return LIBLTE_ERROR_DECODE_FAIL;
    }
    err = LIBLTE_SUCCESS;
  }
  return err;
}

/*******************************************************************************
 * ProtocolIE ENB-MBMS-M2AP-ID INTEGER (0..65535)
 *******************************************************************************/
LIBLTE_ERROR_ENUM liblte_m2ap_pack_enbmbmsm2apid(LIBLTE_M2AP_ENB_MBMS_M2AP_ID_STRUCT* ie, uint8_t** ptr)
{
  LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;
  if (ie != NULL && ptr != NULL) {
    if (ie->enb_mbms_m2ap_id < 256) {
      liblte_value_2_bits(0, ptr, 2);
      liblte_align_up_zero(ptr, 8);
      liblte_value_2_bits(ie->enb_mbms_m2ap_id, ptr, 8);
    } else if (ie->enb_mbms_m2ap_id < 65535) {
      liblte_value_2_bits(1, ptr, 2);
      liblte_align_up_zero(ptr, 8);
      liblte_value_2_bits(ie->enb_mbms_m2ap_id, ptr, 16);
    } else {
      return LIBLTE_ERROR_ENCODE_FAIL;
    }
    err = LIBLTE_SUCCESS;
  }
  return err;
}
LIBLTE_ERROR_ENUM liblte_m2ap_unpack_enbmbmsm2apid(uint8_t** ptr, LIBLTE_M2AP_ENB_MBMS_M2AP_ID_STRUCT* ie)
{
  LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;
  if (ie != NULL && ptr != NULL) {
    uint8_t value = liblte_bits_2_value(ptr, 2);
    liblte_align_up(ptr, 8);
    if (value == 0) {
      ie->enb_mbms_m2ap_id = liblte_bits_2_value(ptr, 8);
    } else if (value == 1) {
      ie->enb_mbms_m2ap_id = liblte_bits_2_value(ptr, 16);
    } else {
      return LIBLTE_ERROR_DECODE_FAIL;
    }
    err = LIBLTE_SUCCESS;
  }
  return err;
}
/*******************************************************************************
 * ProtocolIE PDCCH-Length ENUM
 *******************************************************************************/
// lb:0, ub:512
LIBLTE_ERROR_ENUM liblte_m2ap_pack_pdcchlength(LIBLTE_M2AP_PDCCH_LENGTH_STRUCT* ie, uint8_t** ptr)
{
  LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;
  if (ie != NULL && ptr != NULL) {
    liblte_value_2_bits(ie->ext, ptr, 1);
    liblte_value_2_bits(ie->pdcchLength, ptr, 1);
    err = LIBLTE_SUCCESS;
  }
  return err;
}
LIBLTE_ERROR_ENUM liblte_m2ap_unpack_pdcchlength(uint8_t** ptr, LIBLTE_M2AP_PDCCH_LENGTH_STRUCT* ie)
{
  LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;
  if (ie != NULL && ptr != NULL) {
    ie->ext = liblte_bits_2_value(ptr, 1);
    if (ie->ext) {
      liblte_log_print("PDCCH-Length error: M2AP ASN extensions not currently supported\n");
      return LIBLTE_ERROR_DECODE_FAIL;
    }

    uint32_t tmp = liblte_bits_2_value(ptr, 1);
    if (tmp == LIBLTE_M2AP_PDCCH_LENGTH_S1) {
      ie->pdcchLength = LIBLTE_M2AP_PDCCH_LENGTH_S1;
    } else if (tmp == LIBLTE_M2AP_PDCCH_LENGTH_S2) {
      ie->pdcchLength = LIBLTE_M2AP_PDCCH_LENGTH_S2;
    } else {
      return LIBLTE_ERROR_ENCODE_FAIL;
    }
    err = LIBLTE_SUCCESS;
  }
  return err;
}

/*******************************************************************************
 * ProtocolIE Repetition-Period ENUM
 *******************************************************************************/
// lb:0, ub:512
LIBLTE_ERROR_ENUM liblte_m2ap_pack_repetitionperiod(LIBLTE_M2AP_REPETITION_PERIOD_STRUCT* ie, uint8_t** ptr)
{
  LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;
  if (ie != NULL && ptr != NULL) {
    liblte_value_2_bits(ie->repetitionPeriod, ptr, 2);
    err = LIBLTE_SUCCESS;
  }
  return err;
}
LIBLTE_ERROR_ENUM liblte_m2ap_unpack_repetitionperiod(uint8_t** ptr, LIBLTE_M2AP_REPETITION_PERIOD_STRUCT* ie)
{
  LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;
  if (ie != NULL && ptr != NULL) {
    uint32_t tmp = liblte_bits_2_value(ptr, 2);
    if (tmp == LIBLTE_M2AP_REPETITION_PERIOD_RF32) {
      ie->repetitionPeriod = LIBLTE_M2AP_REPETITION_PERIOD_RF32;
    } else if (tmp == LIBLTE_M2AP_REPETITION_PERIOD_RF64) {
      ie->repetitionPeriod = LIBLTE_M2AP_REPETITION_PERIOD_RF64;
    } else if (tmp == LIBLTE_M2AP_REPETITION_PERIOD_RF128) {
      ie->repetitionPeriod = LIBLTE_M2AP_REPETITION_PERIOD_RF128;
    } else if (tmp == LIBLTE_M2AP_REPETITION_PERIOD_RF256) {
      ie->repetitionPeriod = LIBLTE_M2AP_REPETITION_PERIOD_RF256;
    } else {
      return LIBLTE_ERROR_ENCODE_FAIL;
    }
    err = LIBLTE_SUCCESS;
  }
  return err;
}

/*******************************************************************************
 * ProtocolIE Offset ENUM
 *******************************************************************************/
// lb:0, ub:512
LIBLTE_ERROR_ENUM liblte_m2ap_pack_offset(LIBLTE_M2AP_OFFSET_STRUCT* ie, uint8_t** ptr)
{
  LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;
  if (ie != NULL && ptr != NULL) {
    liblte_value_2_bits(ie->offset, ptr, 4);
    err = LIBLTE_SUCCESS;
  }
  return err;
}
LIBLTE_ERROR_ENUM liblte_m2ap_unpack_offset(uint8_t** ptr, LIBLTE_M2AP_OFFSET_STRUCT* ie)
{
  LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;
  if (ie != NULL && ptr != NULL) {
    ie->offset = liblte_bits_2_value(ptr, 4);
    err        = LIBLTE_SUCCESS;
  }
  return err;
}
/*******************************************************************************
 * ProtocolIE Modification Period ENUM
 *******************************************************************************/
// lb:0, ub:512
LIBLTE_ERROR_ENUM liblte_m2ap_pack_modificationperiod(LIBLTE_M2AP_MODIFICATION_PERIOD_STRUCT* ie, uint8_t** ptr)
{
  LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;
  if (ie != NULL && ptr != NULL) {
    liblte_value_2_bits(ie->modificationPeriod, ptr, 1);
    err = LIBLTE_SUCCESS;
  }
  return err;
}
LIBLTE_ERROR_ENUM liblte_m2ap_unpack_modificationperiod(uint8_t** ptr, LIBLTE_M2AP_MODIFICATION_PERIOD_STRUCT* ie)
{
  LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;
  if (ie != NULL && ptr != NULL) {
    uint32_t tmp = liblte_bits_2_value(ptr, 1);
    if (tmp == LIBLTE_M2AP_MODIFICATION_PERIOD_RF512) {
      ie->modificationPeriod = LIBLTE_M2AP_MODIFICATION_PERIOD_RF512;
    } else if (tmp == LIBLTE_M2AP_MODIFICATION_PERIOD_RF1024) {
      ie->modificationPeriod = LIBLTE_M2AP_MODIFICATION_PERIOD_RF1024;
    } else {
      return LIBLTE_ERROR_ENCODE_FAIL;
    }
    err = LIBLTE_SUCCESS;
  }
  return err;
}
/*******************************************************************************
 * ProtocolIE Subframe Allocation Info STATIC BIT STRING
 *******************************************************************************/
// lb:0, ub:512
LIBLTE_ERROR_ENUM liblte_m2ap_pack_subframeallocationinfo(LIBLTE_M2AP_SUBFRAME_ALLOCATION_INFO_STRUCT* ie,
                                                          uint8_t**                                    ptr)
{
  LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;
  if (ie != NULL && ptr != NULL) {
    for (int i = 0; i < LIBLTE_M2AP_SUBFRAME_ALLOCATION_INFO_BIT_STRING_LEN; i++) {
      liblte_value_2_bits(ie->buffer[i], ptr, 1);
    }
    err = LIBLTE_SUCCESS;
  }
  return err;
}
LIBLTE_ERROR_ENUM liblte_m2ap_unpack_subframeallocationinfo(uint8_t**                                    ptr,
                                                            LIBLTE_M2AP_SUBFRAME_ALLOCATION_INFO_STRUCT* ie)
{
  LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;
  if (ie != NULL && ptr != NULL) {
    for (int i = 0; i < LIBLTE_M2AP_SUBFRAME_ALLOCATION_INFO_BIT_STRING_LEN; i++) {
      ie->buffer[i] = liblte_bits_2_value(ptr, 1);
    }
    err = LIBLTE_SUCCESS;
  }
  return err;
}

/*******************************************************************************
 * ProtocolIE Subframe Allocation CHOICE
 *******************************************************************************/
// lb:0, ub:512
LIBLTE_ERROR_ENUM liblte_m2ap_pack_subframeallocation(LIBLTE_M2AP_SUBFRAME_ALLOCATION_STRUCT* ie, uint8_t** ptr)
{
  LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;
  if (ie != NULL && ptr != NULL) {
    liblte_value_2_bits(ie->choice_type, ptr, 1);
    if (ie->choice_type == LIBLTE_M2AP_SUBFRAME_ALLOCATION_ONE_FRAME) {
      for (int i = 0; i < 6; i++) {
        liblte_value_2_bits(ie->choice.oneFrame[i], ptr, 1);
      }
    } else if (ie->choice_type == LIBLTE_M2AP_SUBFRAME_ALLOCATION_FOUR_FRAMES) {
      for (int i = 0; i < 24; i++) {
        liblte_value_2_bits(ie->choice.fourFrames[i], ptr, 1);
      }
    }
    err = LIBLTE_SUCCESS;
  }
  return err;
}
LIBLTE_ERROR_ENUM liblte_m2ap_unpack_subframeallocation(uint8_t** ptr, LIBLTE_M2AP_SUBFRAME_ALLOCATION_STRUCT* ie)
{
  LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;
  if (ie != NULL && ptr != NULL) {
    uint8_t tmp = liblte_bits_2_value(ptr, 1);
    if (tmp == LIBLTE_M2AP_SUBFRAME_ALLOCATION_ONE_FRAME) {
      ie->choice_type = LIBLTE_M2AP_SUBFRAME_ALLOCATION_ONE_FRAME;
      for (int i = 0; i < 6; i++) {
        ie->choice.oneFrame[i] = liblte_bits_2_value(ptr, 1);
      }
    } else {
      ie->choice_type = LIBLTE_M2AP_SUBFRAME_ALLOCATION_FOUR_FRAMES;
      for (int i = 0; i < 24; i++) {
        ie->choice.fourFrames[i] = liblte_bits_2_value(ptr, 1);
      }
    }
    err = LIBLTE_SUCCESS;
  }
  return err;
}
/*******************************************************************************
 * ProtocolIE Modulation and Coding Scheme ENUM
 *******************************************************************************/
// lb:0, ub:512
LIBLTE_ERROR_ENUM liblte_m2ap_pack_modulationandcodingscheme(LIBLTE_M2AP_MODULATION_AND_CODING_SCHEME_STRUCT* ie,
                                                             uint8_t**                                        ptr)
{
  LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;
  if (ie != NULL && ptr != NULL) {
    liblte_value_2_bits(ie->mcs, ptr, 2);
    err = LIBLTE_SUCCESS;
  }
  return err;
}
LIBLTE_ERROR_ENUM liblte_m2ap_unpack_modulationandcodingscheme(uint8_t**                                        ptr,
                                                               LIBLTE_M2AP_MODULATION_AND_CODING_SCHEME_STRUCT* ie)
{
  LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;
  if (ie != NULL && ptr != NULL) {
    uint32_t tmp = liblte_bits_2_value(ptr, 2);
    if (tmp == LIBLTE_M2AP_MODULATION_AND_CODING_SCHEME_N2) {
      ie->mcs = LIBLTE_M2AP_MODULATION_AND_CODING_SCHEME_N2;
    } else if (tmp == LIBLTE_M2AP_MODULATION_AND_CODING_SCHEME_N7) {
      ie->mcs = LIBLTE_M2AP_MODULATION_AND_CODING_SCHEME_N7;
    } else if (tmp == LIBLTE_M2AP_MODULATION_AND_CODING_SCHEME_N13) {
      ie->mcs = LIBLTE_M2AP_MODULATION_AND_CODING_SCHEME_N13;
    } else if (tmp == LIBLTE_M2AP_MODULATION_AND_CODING_SCHEME_N19) {
      ie->mcs = LIBLTE_M2AP_MODULATION_AND_CODING_SCHEME_N19;
    } else {
      return LIBLTE_ERROR_ENCODE_FAIL;
    }
    err = LIBLTE_SUCCESS;
  }
  return err;
}
/*******************************************************************************
 * ProtocolIE RadioframeAllocationPeriod ENUM
 *******************************************************************************/
// lb:0, ub:512
LIBLTE_ERROR_ENUM liblte_m2ap_pack_radioframeallocationperiod(LIBLTE_M2AP_RADIOFRAME_ALLOCATION_PERIOD_STRUCT* ie,
                                                              uint8_t**                                        ptr)
{
  LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;
  if (ie != NULL && ptr != NULL) {
    liblte_value_2_bits(ie->e, ptr, 3);
    err = LIBLTE_SUCCESS;
  }
  return err;
}
LIBLTE_ERROR_ENUM liblte_m2ap_unpack_radioframeallocationperiod(uint8_t**                                        ptr,
                                                                LIBLTE_M2AP_RADIOFRAME_ALLOCATION_PERIOD_STRUCT* ie)
{
  LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;
  if (ie != NULL && ptr != NULL) {
    uint32_t tmp = liblte_bits_2_value(ptr, 3);
    if (tmp == LIBLTE_M2AP_RADIOFRAME_ALLOCATION_PERIOD_N1) {
      ie->e = LIBLTE_M2AP_RADIOFRAME_ALLOCATION_PERIOD_N1;
    } else if (tmp == LIBLTE_M2AP_RADIOFRAME_ALLOCATION_PERIOD_N2) {
      ie->e = LIBLTE_M2AP_RADIOFRAME_ALLOCATION_PERIOD_N2;
    } else if (tmp == LIBLTE_M2AP_RADIOFRAME_ALLOCATION_PERIOD_N4) {
      ie->e = LIBLTE_M2AP_RADIOFRAME_ALLOCATION_PERIOD_N4;
    } else if (tmp == LIBLTE_M2AP_RADIOFRAME_ALLOCATION_PERIOD_N8) {
      ie->e = LIBLTE_M2AP_RADIOFRAME_ALLOCATION_PERIOD_N8;
    } else if (tmp == LIBLTE_M2AP_RADIOFRAME_ALLOCATION_PERIOD_N16) {
      ie->e = LIBLTE_M2AP_RADIOFRAME_ALLOCATION_PERIOD_N16;
    } else if (tmp == LIBLTE_M2AP_RADIOFRAME_ALLOCATION_PERIOD_N32) {
      ie->e = LIBLTE_M2AP_RADIOFRAME_ALLOCATION_PERIOD_N32;
    } else {
      return LIBLTE_ERROR_DECODE_FAIL;
    }
    err = LIBLTE_SUCCESS;
  }
  return err;
}

/*******************************************************************************
 * ProtocolIE Common-Subframe-Scheduling-Period ENUM
 *******************************************************************************/
// lb:0, ub:512
LIBLTE_ERROR_ENUM
liblte_m2ap_pack_commonsubframeallocationperiod(LIBLTE_M2AP_COMMON_SUBFRAME_ALLOCATION_PERIOD_STRUCT* ie, uint8_t** ptr)
{
  LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;
  if (ie != NULL && ptr != NULL) {
    liblte_value_2_bits(ie->e, ptr, 3);
    err = LIBLTE_SUCCESS;
  }
  return err;
}
LIBLTE_ERROR_ENUM
liblte_m2ap_unpack_commonsubframeallocationperiod(uint8_t**                                             ptr,
                                                  LIBLTE_M2AP_COMMON_SUBFRAME_ALLOCATION_PERIOD_STRUCT* ie)
{
  LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;
  if (ie != NULL && ptr != NULL) {
    uint32_t tmp = liblte_bits_2_value(ptr, 3);
    if (tmp == LIBLTE_M2AP_COMMON_SUBFRAME_ALLOCATION_PERIOD_RF8) {
      ie->e = LIBLTE_M2AP_COMMON_SUBFRAME_ALLOCATION_PERIOD_RF8;
    } else if (tmp == LIBLTE_M2AP_COMMON_SUBFRAME_ALLOCATION_PERIOD_RF16) {
      ie->e = LIBLTE_M2AP_COMMON_SUBFRAME_ALLOCATION_PERIOD_RF16;
    } else if (tmp == LIBLTE_M2AP_COMMON_SUBFRAME_ALLOCATION_PERIOD_RF32) {
      ie->e = LIBLTE_M2AP_COMMON_SUBFRAME_ALLOCATION_PERIOD_RF32;
    } else if (tmp == LIBLTE_M2AP_COMMON_SUBFRAME_ALLOCATION_PERIOD_RF64) {
      ie->e = LIBLTE_M2AP_COMMON_SUBFRAME_ALLOCATION_PERIOD_RF64;
    } else if (tmp == LIBLTE_M2AP_COMMON_SUBFRAME_ALLOCATION_PERIOD_RF128) {
      ie->e = LIBLTE_M2AP_COMMON_SUBFRAME_ALLOCATION_PERIOD_RF128;
    } else if (tmp == LIBLTE_M2AP_COMMON_SUBFRAME_ALLOCATION_PERIOD_RF256) {
      ie->e = LIBLTE_M2AP_COMMON_SUBFRAME_ALLOCATION_PERIOD_RF256;
    } else {
      return LIBLTE_ERROR_DECODE_FAIL;
    }
    err = LIBLTE_SUCCESS;
  }
  return err;
}
/*******************************************************************************
 * ProtocolIE MCH Scheduling Period ENUM
 *******************************************************************************/
// lb:0, ub:512
LIBLTE_ERROR_ENUM liblte_m2ap_pack_mchschedulingperiod(LIBLTE_M2AP_MCH_SCHEDULING_PERIOD_STRUCT* ie, uint8_t** ptr)
{
  LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;
  if (ie != NULL && ptr != NULL) {
    liblte_value_2_bits(ie->e, ptr, 3);
    err = LIBLTE_SUCCESS;
  }
  return err;
}
LIBLTE_ERROR_ENUM liblte_m2ap_unpack_mchschedulingperiod(uint8_t** ptr, LIBLTE_M2AP_MCH_SCHEDULING_PERIOD_STRUCT* ie)
{
  LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;
  if (ie != NULL && ptr != NULL) {
    uint32_t tmp = liblte_bits_2_value(ptr, 3);
    if (tmp == LIBLTE_M2AP_MCH_SCHEDULING_PERIOD_RF8) {
      ie->e = LIBLTE_M2AP_MCH_SCHEDULING_PERIOD_RF8;
    } else if (tmp == LIBLTE_M2AP_MCH_SCHEDULING_PERIOD_RF16) {
      ie->e = LIBLTE_M2AP_MCH_SCHEDULING_PERIOD_RF16;
    } else if (tmp == LIBLTE_M2AP_MCH_SCHEDULING_PERIOD_RF32) {
      ie->e = LIBLTE_M2AP_MCH_SCHEDULING_PERIOD_RF32;
    } else if (tmp == LIBLTE_M2AP_MCH_SCHEDULING_PERIOD_RF64) {
      ie->e = LIBLTE_M2AP_MCH_SCHEDULING_PERIOD_RF64;
    } else if (tmp == LIBLTE_M2AP_MCH_SCHEDULING_PERIOD_RF128) {
      ie->e = LIBLTE_M2AP_MCH_SCHEDULING_PERIOD_RF128;
    } else if (tmp == LIBLTE_M2AP_MCH_SCHEDULING_PERIOD_RF256) {
      ie->e = LIBLTE_M2AP_MCH_SCHEDULING_PERIOD_RF256;
    } else if (tmp == LIBLTE_M2AP_MCH_SCHEDULING_PERIOD_RF512) {
      ie->e = LIBLTE_M2AP_MCH_SCHEDULING_PERIOD_RF512;
    } else if (tmp == LIBLTE_M2AP_MCH_SCHEDULING_PERIOD_RF1024) {
      ie->e = LIBLTE_M2AP_MCH_SCHEDULING_PERIOD_RF1024;
    } else {
      return LIBLTE_ERROR_DECODE_FAIL;
    }
    err = LIBLTE_SUCCESS;
  }
  return err;
}
/*******************************************************************************
 * ProtocolIE Cell-Reservation-Info ENUM
 *******************************************************************************/
// lb:0, ub:512
LIBLTE_ERROR_ENUM liblte_m2ap_pack_cellreservationinfo(LIBLTE_M2AP_CELL_RESERVATION_INFO_STRUCT* ie, uint8_t** ptr)
{
  LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;
  if (ie != NULL && ptr != NULL) {
    liblte_value_2_bits(ie->ext, ptr, 1);
    liblte_value_2_bits(ie->e, ptr, 1);
    err = LIBLTE_SUCCESS;
  }
  return err;
}
LIBLTE_ERROR_ENUM liblte_m2ap_unpack_cellreservationinfo(uint8_t** ptr, LIBLTE_M2AP_CELL_RESERVATION_INFO_STRUCT* ie)
{
  LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;
  if (ie != NULL && ptr != NULL) {
    ie->ext      = liblte_bits_2_value(ptr, 1);
    uint32_t tmp = liblte_bits_2_value(ptr, 1);
    if (tmp == LIBLTE_M2AP_CELL_RESERVATION_INFO_RESERVED_CELL) {
      ie->e = LIBLTE_M2AP_CELL_RESERVATION_INFO_RESERVED_CELL;
    } else if (tmp == LIBLTE_M2AP_CELL_RESERVATION_INFO_NON_RESERVED_CELL) {
      ie->e = LIBLTE_M2AP_CELL_RESERVATION_INFO_NON_RESERVED_CELL;
    } else {
      return LIBLTE_ERROR_ENCODE_FAIL;
    }
    err = LIBLTE_SUCCESS;
  }
  return err;
}
/***************************************************************************************
 * ProtocolIE TMGI SEQUENCE
 ***************************************************************************************/
LIBLTE_ERROR_ENUM liblte_m2ap_pack_tmgi(LIBLTE_M2AP_TMGI_STRUCT* ie, uint8_t** ptr)
{
  LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;
  if (ie != NULL && ptr != NULL) {
    // Extension
    liblte_value_2_bits(ie->ext, ptr, 1);
    if (ie->ext) {
      liblte_log_print("Cell-Information Item error: M2AP ASN extensions not currently supported\n");
      return LIBLTE_ERROR_ENCODE_FAIL;
    }

    // Bools
    liblte_value_2_bits(ie->iE_Extensions_present, ptr, 1);
    if (liblte_m2ap_pack_plmnidentity(&ie->pLMN_Identity, ptr) != LIBLTE_SUCCESS) {
      return LIBLTE_ERROR_ENCODE_FAIL;
    }
    if (liblte_m2ap_pack_serviceid(&ie->serviceID, ptr) != LIBLTE_SUCCESS) {
      return LIBLTE_ERROR_ENCODE_FAIL;
    }
  }
  err = LIBLTE_SUCCESS;
  return err;
}

LIBLTE_ERROR_ENUM liblte_m2ap_unpack_tmgi(uint8_t** ptr, LIBLTE_M2AP_TMGI_STRUCT* ie)
{
  LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

  if (ie != NULL && ptr != NULL) {
    // Extension
    ie->ext = liblte_bits_2_value(ptr, 1);
    if (ie->ext) {
      liblte_log_print("TMGI error: M2AP ASN extensions not currently supported\n");
      return LIBLTE_ERROR_DECODE_FAIL;
    }
    // Bools
    ie->iE_Extensions_present = liblte_bits_2_value(ptr, 1);
    liblte_align_up(ptr, 8);

    // PLMN Identity
    if (liblte_m2ap_unpack_plmnidentity(ptr, &ie->pLMN_Identity) != LIBLTE_SUCCESS) {
      return LIBLTE_ERROR_DECODE_FAIL;
    }
    if (liblte_m2ap_unpack_serviceid(ptr, &ie->serviceID) != LIBLTE_SUCCESS) {
      return LIBLTE_ERROR_DECODE_FAIL;
    }
    err = LIBLTE_SUCCESS;
  }
  return err;
}
/***************************************************************************************
 * ProtocolIE TNL Information SEQUENCE
 ***************************************************************************************/
LIBLTE_ERROR_ENUM liblte_m2ap_pack_tnlinformation(LIBLTE_M2AP_TNL_INFORMATION_STRUCT* ie, uint8_t** ptr)
{
  LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;
  if (ie != NULL && ptr != NULL) {
    // Extension
    liblte_value_2_bits(ie->ext, ptr, 1);
    if (ie->ext) {
      liblte_log_print("TNL-Information Item error: M2AP ASN extensions not currently supported\n");
      return LIBLTE_ERROR_ENCODE_FAIL;
    }

    // Bools
    liblte_value_2_bits(ie->iE_Extensions_present, ptr, 1);

    if (liblte_m2ap_pack_ipaddress(&ie->iPMCAddress, ptr) != LIBLTE_SUCCESS) {
      return LIBLTE_ERROR_DECODE_FAIL;
    }
    if (liblte_m2ap_pack_ipaddress(&ie->iPSourceAddress, ptr) != LIBLTE_SUCCESS) {
      return LIBLTE_ERROR_DECODE_FAIL;
    }
    if (liblte_m2ap_pack_gtpteid(&ie->gtpTeid, ptr) != LIBLTE_SUCCESS) {
      return LIBLTE_ERROR_DECODE_FAIL;
    }
  }
  err = LIBLTE_SUCCESS;
  return err;
}

LIBLTE_ERROR_ENUM liblte_m2ap_unpack_tnlinformation(uint8_t** ptr, LIBLTE_M2AP_TNL_INFORMATION_STRUCT* ie)
{
  LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

  if (ie != NULL && ptr != NULL) {
    // Extension
    ie->ext = liblte_bits_2_value(ptr, 1);
    if (ie->ext) {
      liblte_log_print("Cell-Information Item error: M2AP ASN extensions not currently supported\n");
      return LIBLTE_ERROR_DECODE_FAIL;
    }

    // Bools
    ie->iE_Extensions_present = liblte_bits_2_value(ptr, 1);

    if (liblte_m2ap_unpack_ipaddress(ptr, &ie->iPMCAddress) != LIBLTE_SUCCESS) {
      return LIBLTE_ERROR_DECODE_FAIL;
    }
    if (liblte_m2ap_unpack_ipaddress(ptr, &ie->iPSourceAddress) != LIBLTE_SUCCESS) {
      return LIBLTE_ERROR_DECODE_FAIL;
    }
    if (liblte_m2ap_unpack_gtpteid(ptr, &ie->gtpTeid) != LIBLTE_SUCCESS) {
      return LIBLTE_ERROR_DECODE_FAIL;
    }
    err = LIBLTE_SUCCESS;
  }
  return err;
}
/***************************************************************************************
 * ProtocolIE CellInformation SEQUENCE
 ***************************************************************************************/
LIBLTE_ERROR_ENUM liblte_m2ap_pack_cellinformation(LIBLTE_M2AP_CELL_INFORMATION_STRUCT* ie, uint8_t** ptr)
{
  LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;
  if (ie != NULL && ptr != NULL) {
    // Extension
    liblte_value_2_bits(ie->ext, ptr, 1);
    if (ie->ext) {
      liblte_log_print("Cell-Information Item error: M2AP ASN extensions not currently supported\n");
      return LIBLTE_ERROR_ENCODE_FAIL;
    }

    // Bools
    liblte_value_2_bits(ie->ie_extensions_present, ptr, 1);
    if (liblte_m2ap_pack_ecgi(&ie->eCGI, ptr) != LIBLTE_SUCCESS) {
      return LIBLTE_ERROR_ENCODE_FAIL;
    }
    if (liblte_m2ap_pack_cellreservationinfo(&ie->cellReservationInfo, ptr) != LIBLTE_SUCCESS) {
      return LIBLTE_ERROR_ENCODE_FAIL;
    }
  }
  err = LIBLTE_SUCCESS;
  return err;
}

LIBLTE_ERROR_ENUM liblte_m2ap_unpack_cellinformation(uint8_t** ptr, LIBLTE_M2AP_CELL_INFORMATION_STRUCT* ie)
{
  LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

  if (ie != NULL && ptr != NULL) {
    // Extension
    ie->ext = liblte_bits_2_value(ptr, 1);
    if (ie->ext) {
      liblte_log_print("Cell-Information Item error: M2AP ASN extensions not currently supported\n");
      return LIBLTE_ERROR_DECODE_FAIL;
    }

    // Bools
    ie->ie_extensions_present = liblte_bits_2_value(ptr, 1);
    if (liblte_m2ap_unpack_ecgi(ptr, &ie->eCGI) != LIBLTE_SUCCESS) {
      return LIBLTE_ERROR_DECODE_FAIL;
    }
    if (liblte_m2ap_unpack_cellreservationinfo(ptr, &ie->cellReservationInfo) != LIBLTE_SUCCESS) {
      return LIBLTE_ERROR_DECODE_FAIL;
    }
    err = LIBLTE_SUCCESS;
  }
  return err;
}

/***************************************************************************************
 * ProtocolIE CellInformation-List SEQUENCE
 ***************************************************************************************/
LIBLTE_ERROR_ENUM liblte_m2ap_pack_cellinformationlist(LIBLTE_M2AP_CELL_INFORMATION_LIST_STRUCT* ie, uint8_t** ptr)
{
  LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;
  if (ie != NULL && ptr != NULL) {
    // Length
    liblte_align_up_zero(ptr, 8);
    liblte_value_2_bits(ie->len - 1, ptr, 8);
    if (ie->len > 32) {
      liblte_log_print("Cell-Information-List pack error - max supported dynamic sequence length = 32, ie->len = %d\n",
                       ie->len);
      return LIBLTE_ERROR_ENCODE_FAIL;
    }

    for (uint16_t i = 0; i < ie->len; i++) {
      err = liblte_m2ap_pack_cellinformation(&ie->buffer[i], ptr);
      if (err != LIBLTE_SUCCESS)
        return err;
    }
    err = LIBLTE_SUCCESS;
  }
  return err;
}

LIBLTE_ERROR_ENUM liblte_m2ap_unpack_cellinformationlist(uint8_t** ptr, LIBLTE_M2AP_CELL_INFORMATION_LIST_STRUCT* ie)
{
  LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

  if (ie != NULL && ptr != NULL) {
    // Length
    liblte_align_up(ptr, 8);
    ie->len = liblte_bits_2_value(ptr, 8) + 1;
    if (ie->len > 32) {
      liblte_log_print(
          "Cell-Information-List unpack error - max supported dynamic sequence length = 32, ie->len = %d\n", ie->len);
      return LIBLTE_ERROR_DECODE_FAIL;
    }

    for (uint16_t i = 0; i < ie->len; i++) {
      err = liblte_m2ap_unpack_cellinformation(ptr, &ie->buffer[i]);
      if (err != LIBLTE_SUCCESS)
        return err;
    }
    err = LIBLTE_SUCCESS;
  }
  return err;
}
/*******************************************************************************
 * ProtocolIE MCCHRelatedBCCH-ConfigPerMBSFNArea-Item SEQUENCE
 *******************************************************************************/
LIBLTE_ERROR_ENUM liblte_m2ap_pack_mcchrelatedbcchconfigpermbsfnareaitem(
    LIBLTE_M2AP_MCCH_RELATED_BCCH_CONFIG_PER_MBSFN_AREA_ITEM_STRUCT* ie,
    uint8_t**                                                        ptr)
{
  LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

  if (ie != NULL && ptr != NULL) {
    // Extension present
    liblte_value_2_bits(ie->ext ? 1 : 0, ptr, 1);
    if (ie->ext) {
      liblte_log_print("MCCHRelatedBCCH-ConfigPerMBSFNArea Item error: M2AP ASN extensions not currently supported\n");
      return LIBLTE_ERROR_ENCODE_FAIL;
    }
    // Bools
    liblte_value_2_bits(ie->cellInformationList_present ? 1 : 0, ptr, 1);
    // IE Extension present
    liblte_value_2_bits(ie->iE_Extensions_present ? 1 : 0, ptr, 1);
    if (ie->iE_Extensions_present) {
      liblte_log_print("MCCHRelatedBCCH-ConfigPerMBSFNArea Item error: M2AP ASN extensions not currently supported\n");
      return LIBLTE_ERROR_ENCODE_FAIL;
    }
    liblte_align_up_zero(ptr, 8);

    // MBSFN Area Id
    err = liblte_m2ap_pack_mbsfnareaid(&ie->mbsfnArea, ptr);
    if (err != LIBLTE_SUCCESS)
      return err;

    // PDCCH Length
    if (liblte_m2ap_pack_pdcchlength(&ie->pdcchLength, ptr) != LIBLTE_SUCCESS) {
      return LIBLTE_ERROR_ENCODE_FAIL;
    }

    // Repetition Period
    if (liblte_m2ap_pack_repetitionperiod(&ie->repetitionPeriod, ptr) != LIBLTE_SUCCESS) {
      return LIBLTE_ERROR_ENCODE_FAIL;
    }

    // Offset
    if (liblte_m2ap_pack_offset(&ie->offset, ptr) != LIBLTE_SUCCESS) {
      return LIBLTE_ERROR_ENCODE_FAIL;
    }

    // Modification Period
    if (liblte_m2ap_pack_modificationperiod(&ie->modificationPeriod, ptr) != LIBLTE_SUCCESS) {
      return LIBLTE_ERROR_ENCODE_FAIL;
    }

    // Subframe Allocation Info
    if (liblte_m2ap_pack_subframeallocationinfo(&ie->subframeAllocationInfo, ptr) != LIBLTE_SUCCESS) {
      return LIBLTE_ERROR_ENCODE_FAIL;
    }

    // Modulation and Conding Scheme
    if (liblte_m2ap_pack_modulationandcodingscheme(&ie->modulationAndCodingScheme, ptr) != LIBLTE_SUCCESS) {
      return LIBLTE_ERROR_ENCODE_FAIL;
    }

    // Cell Information List
    if (liblte_m2ap_pack_cellinformationlist(&ie->cellInformationList, ptr) != LIBLTE_SUCCESS) {
      return LIBLTE_ERROR_ENCODE_FAIL;
    }

    if (ie->iE_Extensions_present) {
      if (liblte_m2ap_unpack_protocolextensioncontainer(ptr, &ie->iE_Extensions) != LIBLTE_SUCCESS) {
        return LIBLTE_ERROR_DECODE_FAIL;
      }
    }
    err = LIBLTE_SUCCESS;
  }
  return err;
}

LIBLTE_ERROR_ENUM liblte_m2ap_unpack_mcchrelatedbcchconfigpermbsfnareaitem(
    uint8_t**                                                        ptr,
    LIBLTE_M2AP_MCCH_RELATED_BCCH_CONFIG_PER_MBSFN_AREA_ITEM_STRUCT* ie)
{
  LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

  if (ie != NULL && ptr != NULL) {
    // Extension
    ie->ext = liblte_bits_2_value(ptr, 1);
    if (ie->ext) {
      liblte_log_print("MCCHRelatedBCCH-ConfigPerMBSFNArea Item error: M2AP ASN extensions not currently supported\n");
      return LIBLTE_ERROR_DECODE_FAIL;
    }

    // Bools
    ie->cellInformationList_present = liblte_bits_2_value(ptr, 1);
    ie->iE_Extensions_present       = liblte_bits_2_value(ptr, 1);
    liblte_align_up_zero(ptr, 8);

    // MBSFN Area Id
    if (liblte_m2ap_unpack_mbsfnareaid(ptr, &ie->mbsfnArea) != LIBLTE_SUCCESS) {
      return LIBLTE_ERROR_DECODE_FAIL;
    }
    // PDCCH Length
    if (liblte_m2ap_unpack_pdcchlength(ptr, &ie->pdcchLength) != LIBLTE_SUCCESS) {
      return LIBLTE_ERROR_DECODE_FAIL;
    }
    // Repetition Period
    if (liblte_m2ap_unpack_repetitionperiod(ptr, &ie->repetitionPeriod) != LIBLTE_SUCCESS) {
      return LIBLTE_ERROR_DECODE_FAIL;
    }
    // Offset
    if (liblte_m2ap_unpack_offset(ptr, &ie->offset) != LIBLTE_SUCCESS) {
      return LIBLTE_ERROR_DECODE_FAIL;
    }
    ////Modification Period
    if (liblte_m2ap_unpack_modificationperiod(ptr, &ie->modificationPeriod) != LIBLTE_SUCCESS) {
      return LIBLTE_ERROR_DECODE_FAIL;
    }
    // Subframe Allocation Info
    if (liblte_m2ap_unpack_subframeallocationinfo(ptr, &ie->subframeAllocationInfo) != LIBLTE_SUCCESS) {
      return LIBLTE_ERROR_DECODE_FAIL;
    }

    // Modulation Coding Scheme
    if (liblte_m2ap_unpack_modulationandcodingscheme(ptr, &ie->modulationAndCodingScheme) != LIBLTE_SUCCESS) {
      return LIBLTE_ERROR_DECODE_FAIL;
    }

    // Cell Information List
    if (ie->cellInformationList_present) {
      if (liblte_m2ap_unpack_cellinformationlist(ptr, &ie->cellInformationList) != LIBLTE_SUCCESS) {
        return LIBLTE_ERROR_DECODE_FAIL;
      }
    }
    if (ie->iE_Extensions_present) {
      if (liblte_m2ap_unpack_protocolextensioncontainer(ptr, &ie->iE_Extensions) != LIBLTE_SUCCESS) {
        return LIBLTE_ERROR_DECODE_FAIL;
      }
    }
    err = LIBLTE_SUCCESS;
  }
  return err;
}

/***************************************************************************************
 * ProtocolIE ENB-MBMS-Configuration-data-List SEQUENCE OF ProtocolIE-Single-Container
 ***************************************************************************************/
LIBLTE_ERROR_ENUM liblte_m2ap_pack_enbmbmsconfigurationdatalist(LIBLTE_M2AP_ENB_MBMS_CONFIGURATION_DATA_LIST_STRUCT* ie,
                                                                uint8_t** ptr)
{
  LIBLTE_ERROR_ENUM                             err = LIBLTE_ERROR_INVALID_INPUTS;
  LIBLTE_M2AP_PROTOCOLIE_SINGLECONTAINER_STRUCT proto_container;
  proto_container.id.ProtocolIE_ID = LIBLTE_M2AP_IE_ID_ENB_MBMS_CONFIGURATION_DATA_ITEM;
  proto_container.criticality      = LIBLTE_M2AP_CRITICALITY_REJECT;

  if (ie != NULL && ptr != NULL) {
    if (ie->len > 32) {
      liblte_log_print(
          "ProtocolIE_ENB-MBMS-Configuration-data-List pack error - max supported dynamic sequence length = 32, "
          "ie->len = %d\n",
          ie->len);
      return LIBLTE_ERROR_ENCODE_FAIL;
    }

    // Length
    liblte_value_2_bits(ie->len - 1, ptr, 8);

    for (uint8_t i = 0; i < ie->len; i++) {
      LIBLTE_BIT_MSG_STRUCT tmp_msg;
      uint8_t*              tmp_ptr;

      // ProtocolIE - ENB-MBMS-Configuration-Data-Item
      tmp_ptr = tmp_msg.msg;
      err     = liblte_m2ap_pack_enbmbmsconfigurationdataitem(&ie->buffer[i], &tmp_ptr);
      if (err != LIBLTE_SUCCESS) {
        return err;
      }
      liblte_align_up_zero(&tmp_ptr, 8);
      tmp_msg.N_bits = tmp_ptr - tmp_msg.msg;
      if (liblte_m2ap_pack_protocolie_header((tmp_msg.N_bits) / 8,
                                             LIBLTE_M2AP_IE_ID_ENB_MBMS_CONFIGURATION_DATA_ITEM,
                                             LIBLTE_M2AP_CRITICALITY_REJECT,
                                             ptr) != LIBLTE_SUCCESS) {
        return LIBLTE_ERROR_ENCODE_FAIL;
      }
      memcpy(*ptr, tmp_msg.msg, tmp_msg.N_bits);
      *ptr += tmp_msg.N_bits;
    }
    err = LIBLTE_SUCCESS;
  }
  return err;
}

LIBLTE_ERROR_ENUM
liblte_m2ap_unpack_enbmbmsconfigurationdatalist(uint8_t** ptr, LIBLTE_M2AP_ENB_MBMS_CONFIGURATION_DATA_LIST_STRUCT* ie)
{
  LIBLTE_ERROR_ENUM                             err = LIBLTE_ERROR_INVALID_INPUTS;
  LIBLTE_M2AP_CRITICALITY_ENUM                  crit;
  LIBLTE_M2AP_PROTOCOLIE_SINGLECONTAINER_STRUCT proto_container;
  uint32_t                                      ie_id;
  uint32_t                                      len;

  if (ie != NULL && ptr != NULL) {
    // Length TODO!!!
    ie->len = liblte_bits_2_value(ptr, 16) + 1;
    liblte_align_up(ptr, 8);
    if (ie->len > 32) {
      liblte_log_print(
          "ENB-MBMS-Configuration-data-List unpack error - max supported dynamic sequence length = 32, ie->len = %d\n",
          ie->len);
      return LIBLTE_ERROR_DECODE_FAIL;
    }

    for (uint8_t i = 0; i < ie->len; i++) {
      if (liblte_m2ap_unpack_protocolie_singlecontainer(ptr, &proto_container) != LIBLTE_SUCCESS) {
        return LIBLTE_ERROR_DECODE_FAIL;
      }
      err = liblte_m2ap_unpack_enbmbmsconfigurationdataitem(ptr, &ie->buffer[i]);
      if (err != LIBLTE_SUCCESS)
        return err;
    }
    err = LIBLTE_SUCCESS;
  }
  return err;
}

/***************************************************************************************
 * ProtocolIE MCCHrelatedBCCH-ConfigPerMBSFNArea SEQUENCE OF ProtocolIE-Single-Container
 ***************************************************************************************/
LIBLTE_ERROR_ENUM
liblte_m2ap_pack_mcchrelatedbcchconfigpermbsfnarea(LIBLTE_M2AP_MCCH_RELATED_BCCH_CONFIG_PER_MBSFN_AREA_STRUCT* ie,
                                                   uint8_t**                                                   ptr)
{
  LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

  if (ie != NULL && ptr != NULL) {
    if (ie->len > 32) {
      liblte_log_print(
          "ProtocolIE_MCCHrelatedBCCH-ConfigPerMBSFNArea pack error - max supported dynamic sequence length = 32, "
          "ie->len = %d\n",
          ie->len);
      return LIBLTE_ERROR_ENCODE_FAIL;
    }

    // Length
    liblte_value_2_bits(ie->len - 1, ptr, 8);

    for (uint16_t i = 0; i < ie->len; i++) {
      LIBLTE_BIT_MSG_STRUCT tmp_msg;
      uint8_t*              tmp_ptr;

      // ProtocolIE - MCCHrelatedBCCH-ConfigPerMBSFNArea
      tmp_ptr = tmp_msg.msg;
      err     = liblte_m2ap_pack_mcchrelatedbcchconfigpermbsfnareaitem(&ie->buffer[i], &tmp_ptr);
      if (err != LIBLTE_SUCCESS) {
        return err;
      }
      liblte_align_up_zero(&tmp_ptr, 8);
      tmp_msg.N_bits = tmp_ptr - tmp_msg.msg;
      if (liblte_m2ap_pack_protocolie_header((tmp_msg.N_bits) / 8,
                                             LIBLTE_M2AP_IE_ID_MCCHRELATEDBCCH_CONFIGPERMBSFNAREA_ITEM,
                                             LIBLTE_M2AP_CRITICALITY_REJECT,
                                             ptr) != LIBLTE_SUCCESS) {
        return LIBLTE_ERROR_ENCODE_FAIL;
      }
      memcpy(*ptr, tmp_msg.msg, tmp_msg.N_bits);
      *ptr += tmp_msg.N_bits;
    }
    err = LIBLTE_SUCCESS;
  }
  return err;
}

LIBLTE_ERROR_ENUM
liblte_m2ap_unpack_mcchrelatedbcchconfigpermbsfnarea(uint8_t**                                                   ptr,
                                                     LIBLTE_M2AP_MCCH_RELATED_BCCH_CONFIG_PER_MBSFN_AREA_STRUCT* ie)
{
  LIBLTE_ERROR_ENUM                             err = LIBLTE_ERROR_INVALID_INPUTS;
  LIBLTE_M2AP_CRITICALITY_ENUM                  crit;
  LIBLTE_M2AP_PROTOCOLIE_SINGLECONTAINER_STRUCT proto_container;
  uint32_t                                      ie_id;
  uint32_t                                      len;

  if (ie != NULL && ptr != NULL) {
    // Length TODO!!!
    ie->len = liblte_bits_2_value(ptr, 16) + 1;
    liblte_align_up(ptr, 8);
    if (ie->len > 32) {
      liblte_log_print(
          "MCCHrelatedBCCH-ConfigPerMBSFNAreaunpack error - max supported dynamic sequence length=32, ie->len = %d\n",
          ie->len);
      return LIBLTE_ERROR_DECODE_FAIL;
    }

    for (uint16_t i = 0; i < ie->len; i++) {
      if (liblte_m2ap_unpack_protocolie_singlecontainer(ptr, &proto_container) != LIBLTE_SUCCESS) {
        return LIBLTE_ERROR_DECODE_FAIL;
      }
      err = liblte_m2ap_unpack_mcchrelatedbcchconfigpermbsfnareaitem(ptr, &ie->buffer[i]);
      if (err != LIBLTE_SUCCESS)
        return err;
    }
    err = LIBLTE_SUCCESS;
  }
  return err;
}

/*******************************************************************************
 * ProtocolIE MBMSSessionListPerPMCH-Item SEQUENCE (SIZE(1..maxnoofSessionsPerPMCH)) OF SEQUENCE
 *******************************************************************************/
LIBLTE_ERROR_ENUM liblte_m2ap_pack_mbmssessionlistperpmchitem(LIBLTE_M2AP_MBMS_SESSION_LIST_PER_PMCH_ITEM_STRUCT* ie,
                                                              uint8_t**                                           ptr)
{
  LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

  if (ie != NULL && ptr != NULL) {
    // Length
    liblte_value_2_bits(ie->len - 1, ptr, 5);

    for (uint16_t i = 0; i < ie->len; i++) {
      liblte_value_2_bits(ie->buffer[i].ext, ptr, 1);
      liblte_value_2_bits(ie->buffer[i].iE_Extensions_present, ptr, 1);

      // TMGI
      if (liblte_m2ap_pack_tmgi(&ie->buffer[i].Tmgi, ptr) != LIBLTE_SUCCESS) {
        return LIBLTE_ERROR_ENCODE_FAIL;
      }
      // LCID
      if (liblte_m2ap_pack_lcid(&ie->buffer[i].Lcid, ptr) != LIBLTE_SUCCESS) {
        return LIBLTE_ERROR_ENCODE_FAIL;
      }
    }
    err = LIBLTE_SUCCESS;
  }
  return err;
}

LIBLTE_ERROR_ENUM liblte_m2ap_unpack_mbmssessionlistperpmchitem(uint8_t**                                           ptr,
                                                                LIBLTE_M2AP_MBMS_SESSION_LIST_PER_PMCH_ITEM_STRUCT* ie)
{
  LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

  if (ie != NULL && ptr != NULL) {
    // Length
    ie->len = liblte_bits_2_value(ptr, 5) + 1;

    for (uint16_t i = 0; i < ie->len; i++) {
      ie->buffer[i].ext                   = liblte_bits_2_value(ptr, 1);
      ie->buffer[i].iE_Extensions_present = liblte_bits_2_value(ptr, 1);
      // TMGI
      if (liblte_m2ap_unpack_tmgi(ptr, &ie->buffer[i].Tmgi) != LIBLTE_SUCCESS) {
        return LIBLTE_ERROR_DECODE_FAIL;
      }
      // LCID
      if (liblte_m2ap_unpack_lcid(ptr, &ie->buffer[i].Lcid) != LIBLTE_SUCCESS) {
        return LIBLTE_ERROR_DECODE_FAIL;
      }
    }

    err = LIBLTE_SUCCESS;
  }
  return err;
}

/*******************************************************************************
 * ProtocolIE MBSFN-Subframe-Configuration SEQUENCE
 *******************************************************************************/
LIBLTE_ERROR_ENUM liblte_m2ap_pack_mbsfnsubframeconfiguration(LIBLTE_M2AP_MBSFN_SUBFRAME_CONFIGURATION_STRUCT* ie,
                                                              uint8_t**                                        ptr)
{
  LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

  if (ie != NULL && ptr != NULL) {
    // Extension present
    liblte_value_2_bits(ie->ext ? 1 : 0, ptr, 1);
    if (ie->ext) {
      liblte_log_print("MBSFN-Subframe-Configuration Item error: M2AP ASN extensions not currently supported\n");
      return LIBLTE_ERROR_ENCODE_FAIL;
    }
    // IE Extension present
    liblte_value_2_bits(ie->iE_Extensions_present ? 1 : 0, ptr, 1);
    if (ie->iE_Extensions_present) {
      liblte_log_print("MBSFN Subframe Configuration Item error: M2AP ASN extensions not currently supported\n");
      return LIBLTE_ERROR_ENCODE_FAIL;
    }

    // Radio Frame Allocation Period
    if (liblte_m2ap_pack_radioframeallocationperiod(&ie->radioFrameAllocationPeriod, ptr) != LIBLTE_SUCCESS) {
      return LIBLTE_ERROR_ENCODE_FAIL;
    }

    // Radio Frame Allocation Offset
    if (liblte_m2ap_pack_radioframeallocationoffset(&ie->radioFrameAllocationOffset, ptr) != LIBLTE_SUCCESS) {
      return LIBLTE_ERROR_ENCODE_FAIL;
    }

    // Subframe Allocation
    if (liblte_m2ap_pack_subframeallocation(&ie->subframeAllocation, ptr) != LIBLTE_SUCCESS) {
      return LIBLTE_ERROR_ENCODE_FAIL;
    }

    if (ie->iE_Extensions_present) {
      if (liblte_m2ap_pack_protocolextensioncontainer(&ie->iE_Extensions, ptr) != LIBLTE_SUCCESS) {
        return LIBLTE_ERROR_DECODE_FAIL;
      }
    }
    err = LIBLTE_SUCCESS;
  }
  return err;
}

LIBLTE_ERROR_ENUM liblte_m2ap_unpack_mbsfnsubframeconfiguration(uint8_t**                                        ptr,
                                                                LIBLTE_M2AP_MBSFN_SUBFRAME_CONFIGURATION_STRUCT* ie)
{
  LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

  if (ie != NULL && ptr != NULL) {
    // Extension
    ie->ext = liblte_bits_2_value(ptr, 1);
    if (ie->ext) {
      liblte_log_print("PMCH Configuration error: M2AP ASN extensions not currently supported\n");
      return LIBLTE_ERROR_DECODE_FAIL;
    }

    // Bools
    ie->iE_Extensions_present = liblte_bits_2_value(ptr, 1);

    // Radio Frame Allocation Period
    if (liblte_m2ap_unpack_radioframeallocationperiod(ptr, &ie->radioFrameAllocationPeriod) != LIBLTE_SUCCESS) {
      return LIBLTE_ERROR_DECODE_FAIL;
    }

    // Radio Frame Allocation Offset
    if (liblte_m2ap_unpack_radioframeallocationoffset(ptr, &ie->radioFrameAllocationOffset) != LIBLTE_SUCCESS) {
      return LIBLTE_ERROR_DECODE_FAIL;
    }

    // Subframe Allocation
    if (liblte_m2ap_unpack_subframeallocation(ptr, &ie->subframeAllocation) != LIBLTE_SUCCESS) {
      return LIBLTE_ERROR_DECODE_FAIL;
    }

    if (ie->iE_Extensions_present) {
      if (liblte_m2ap_unpack_protocolextensioncontainer(ptr, &ie->iE_Extensions) != LIBLTE_SUCCESS) {
        return LIBLTE_ERROR_DECODE_FAIL;
      }
    }
    err = LIBLTE_SUCCESS;
  }
  return err;
}
/***************************************************************************************
 * ProtocolIE MBSFN-Subframe-Configuration-List SEQUENCE OF ProtocolIE-Single-Container
 ***************************************************************************************/
LIBLTE_ERROR_ENUM
liblte_m2ap_pack_mbsfnsubframeconfigurationlist(LIBLTE_M2AP_MBSFN_SUBFRAME_CONFIGURATION_LIST_STRUCT* ie, uint8_t** ptr)
{
  LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

  if (ie != NULL && ptr != NULL) {
    if (ie->len > 32) {
      liblte_log_print("ProtocolIE_PMCH-Configuration-List pack error - max supported dynamic sequence length = 32, "
                       "ie->len = %d\n",
                       ie->len);
      return LIBLTE_ERROR_ENCODE_FAIL;
    }

    // Length
    liblte_value_2_bits(ie->len - 1, ptr, 3);
    liblte_align_up_zero(ptr, 8);

    for (uint32_t i = 0; i < ie->len; i++) {
      LIBLTE_BIT_MSG_STRUCT tmp_msg;
      uint8_t*              tmp_ptr;

      // ProtocolIE - MBSFN-Subframe-Configuration
      tmp_ptr = tmp_msg.msg;
      err     = liblte_m2ap_pack_mbsfnsubframeconfiguration(&ie->buffer[i], &tmp_ptr);
      if (err != LIBLTE_SUCCESS) {
        return err;
      }
      liblte_align_up_zero(&tmp_ptr, 8);
      tmp_msg.N_bits = tmp_ptr - tmp_msg.msg;
      if (liblte_m2ap_pack_protocolie_header((tmp_msg.N_bits) / 8,
                                             LIBLTE_M2AP_IE_ID_MBSFN_SUBFRAME_CONFIGURATION_ITEM,
                                             LIBLTE_M2AP_CRITICALITY_REJECT,
                                             ptr) != LIBLTE_SUCCESS) {
        return LIBLTE_ERROR_ENCODE_FAIL;
      }
      memcpy(*ptr, tmp_msg.msg, tmp_msg.N_bits);
      *ptr += tmp_msg.N_bits;
    }

    err = LIBLTE_SUCCESS;
  }
  return err;
}

LIBLTE_ERROR_ENUM
liblte_m2ap_unpack_mbsfnsubframeconfigurationlist(uint8_t**                                             ptr,
                                                  LIBLTE_M2AP_MBSFN_SUBFRAME_CONFIGURATION_LIST_STRUCT* ie)
{
  LIBLTE_ERROR_ENUM                             err = LIBLTE_ERROR_INVALID_INPUTS;
  LIBLTE_M2AP_CRITICALITY_ENUM                  crit;
  LIBLTE_M2AP_PROTOCOLIE_SINGLECONTAINER_STRUCT proto_container;
  uint32_t                                      ie_id;
  uint32_t                                      len;

  if (ie != NULL && ptr != NULL) {
    // Length TODO!!!
    ie->len = liblte_bits_2_value(ptr, 3) + 1;
    liblte_align_up(ptr, 8);
    if (ie->len > 32) {
      liblte_log_print(
          "MBSFN-Area-Configuration-List unpack error - max supported dynamic sequence length = 32, ie->len = %d\n",
          ie->len);
      return LIBLTE_ERROR_DECODE_FAIL;
    }

    for (uint32_t i = 0; i < ie->len; i++) {
      if (liblte_m2ap_unpack_protocolie_header(ptr, &ie_id, &crit, &len) != LIBLTE_SUCCESS) {
        return LIBLTE_ERROR_DECODE_FAIL;
      }
      err = liblte_m2ap_unpack_mbsfnsubframeconfiguration(ptr, &ie->buffer[i]);
      if (err != LIBLTE_SUCCESS)
        return err;
    }
    err = LIBLTE_SUCCESS;
  }
  return err;
}
/*******************************************************************************
 * ProtocolIE PMCH-Configiguration SEQUENCE
 *******************************************************************************/
LIBLTE_ERROR_ENUM liblte_m2ap_pack_pmchconfiguration(LIBLTE_M2AP_PMCH_CONFIGURATION_STRUCT* ie, uint8_t** ptr)
{
  LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

  if (ie != NULL && ptr != NULL) {
    // Extension present
    liblte_value_2_bits(ie->ext ? 1 : 0, ptr, 1);
    if (ie->ext) {
      liblte_log_print("PMCH-Configuration Item error: M2AP ASN extensions not currently supported\n");
      return LIBLTE_ERROR_ENCODE_FAIL;
    }

    // IE Extension present
    liblte_value_2_bits(ie->iE_Extensions_present ? 1 : 0, ptr, 1);
    if (ie->iE_Extensions_present) {
      liblte_log_print("PMCH Configuration Item error: M2AP ASN extensions not currently supported\n");
      return LIBLTE_ERROR_ENCODE_FAIL;
    }
    liblte_align_up_zero(ptr, 8);

    // Allocated Subframes End
    if (liblte_m2ap_pack_allocatedsubframesend(&ie->allocatedSubframesEnd, ptr) != LIBLTE_SUCCESS) {
      return LIBLTE_ERROR_ENCODE_FAIL;
    }

    // Data MCS
    if (liblte_m2ap_pack_datamcs(&ie->dataMCS, ptr) != LIBLTE_SUCCESS) {
      return LIBLTE_ERROR_DECODE_FAIL;
    }

    // Scheduling Period
    if (liblte_m2ap_pack_mchschedulingperiod(&ie->mchSchedulingPeriod, ptr) != LIBLTE_SUCCESS) {
      return LIBLTE_ERROR_DECODE_FAIL;
    }

    if (ie->iE_Extensions_present) {
      if (liblte_m2ap_unpack_protocolextensioncontainer(ptr, &ie->iE_Extensions) != LIBLTE_SUCCESS) {
        return LIBLTE_ERROR_DECODE_FAIL;
      }
    }
    err = LIBLTE_SUCCESS;
  }
  return err;
}

LIBLTE_ERROR_ENUM liblte_m2ap_unpack_pmchconfiguration(uint8_t** ptr, LIBLTE_M2AP_PMCH_CONFIGURATION_STRUCT* ie)
{
  LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

  if (ie != NULL && ptr != NULL) {
    // Extension
    ie->ext = liblte_bits_2_value(ptr, 1);
    if (ie->ext) {
      liblte_log_print("PMCH Configuration error: M2AP ASN extensions not currently supported\n");
      return LIBLTE_ERROR_DECODE_FAIL;
    }

    // Bools
    // ie->cellInformationList_present = liblte_bits_2_value(ptr, 1);
    ie->iE_Extensions_present = liblte_bits_2_value(ptr, 1);
    liblte_align_up_zero(ptr, 8);

    // Allocated Subframes End
    if (liblte_m2ap_unpack_allocatedsubframesend(ptr, &ie->allocatedSubframesEnd) != LIBLTE_SUCCESS) {
      return LIBLTE_ERROR_DECODE_FAIL;
    }

    // Data MCS
    if (liblte_m2ap_unpack_datamcs(ptr, &ie->dataMCS) != LIBLTE_SUCCESS) {
      return LIBLTE_ERROR_DECODE_FAIL;
    }

    // Scheduling Period
    if (liblte_m2ap_unpack_mchschedulingperiod(ptr, &ie->mchSchedulingPeriod) != LIBLTE_SUCCESS) {
      return LIBLTE_ERROR_DECODE_FAIL;
    }

    if (ie->iE_Extensions_present) {
      if (liblte_m2ap_unpack_protocolextensioncontainer(ptr, &ie->iE_Extensions) != LIBLTE_SUCCESS) {
        return LIBLTE_ERROR_DECODE_FAIL;
      }
    }
    err = LIBLTE_SUCCESS;
  }
  return err;
}
/*******************************************************************************
 * ProtocolIE PMCH-Configiguration-Item SEQUENCE
 *******************************************************************************/
LIBLTE_ERROR_ENUM liblte_m2ap_pack_pmchconfigurationitem(LIBLTE_M2AP_PMCH_CONFIGURATION_ITEM_STRUCT* ie, uint8_t** ptr)
{
  LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

  if (ie != NULL && ptr != NULL) {
    // Extension present
    liblte_value_2_bits(ie->ext ? 1 : 0, ptr, 1);
    if (ie->ext) {
      liblte_log_print("PMCH-Configuration Item error: M2AP ASN extensions not currently supported\n");
      return LIBLTE_ERROR_ENCODE_FAIL;
    }

    // IE Extension present
    liblte_value_2_bits(ie->iE_Extensions_present ? 1 : 0, ptr, 1);
    if (ie->iE_Extensions_present) {
      liblte_log_print("PMCH Configuration Item error: M2AP ASN extensions not currently supported\n");
      return LIBLTE_ERROR_ENCODE_FAIL;
    }

    // PMCH-Configuration
    err = liblte_m2ap_pack_pmchconfiguration(&ie->PMCHConfiguration, ptr);
    if (err != LIBLTE_SUCCESS) {
      return err;
    }

    // MBMS Session List per PMCH Item
    if (liblte_m2ap_pack_mbmssessionlistperpmchitem(&ie->MBMSSessionListPerPMCHItem, ptr) != LIBLTE_SUCCESS) {
      return LIBLTE_ERROR_DECODE_FAIL;
    }

    if (ie->iE_Extensions_present) {
      if (liblte_m2ap_unpack_protocolextensioncontainer(ptr, &ie->iE_Extensions) != LIBLTE_SUCCESS) {
        return LIBLTE_ERROR_DECODE_FAIL;
      }
    }
    err = LIBLTE_SUCCESS;
  }
  return err;
}

LIBLTE_ERROR_ENUM liblte_m2ap_unpack_pmchconfigurationitem(uint8_t**                                   ptr,
                                                           LIBLTE_M2AP_PMCH_CONFIGURATION_ITEM_STRUCT* ie)
{
  LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

  if (ie != NULL && ptr != NULL) {
    // Extension
    ie->ext = liblte_bits_2_value(ptr, 1);
    if (ie->ext) {
      liblte_log_print("PMCH Configuration Item error: M2AP ASN extensions not currently supported\n");
      return LIBLTE_ERROR_DECODE_FAIL;
    }

    // Bools
    ie->iE_Extensions_present = liblte_bits_2_value(ptr, 1);

    // PMCH Configuration
    if (liblte_m2ap_unpack_pmchconfiguration(ptr, &ie->PMCHConfiguration) != LIBLTE_SUCCESS) {
      return LIBLTE_ERROR_DECODE_FAIL;
    }

    // MBMS Session List per PMCH Item
    if (liblte_m2ap_unpack_mbmssessionlistperpmchitem(ptr, &ie->MBMSSessionListPerPMCHItem) != LIBLTE_SUCCESS) {
      return LIBLTE_ERROR_DECODE_FAIL;
    }

    if (ie->iE_Extensions_present) {
      if (liblte_m2ap_unpack_protocolextensioncontainer(ptr, &ie->iE_Extensions) != LIBLTE_SUCCESS) {
        return LIBLTE_ERROR_DECODE_FAIL;
      }
    }
    err = LIBLTE_SUCCESS;
  }
  return err;
}
/***************************************************************************************
 * ProtocolIE PMCH-Configuration-List SEQUENCE OF ProtocolIE-Single-Container
 ***************************************************************************************/
LIBLTE_ERROR_ENUM liblte_m2ap_pack_pmchconfigurationlist(LIBLTE_M2AP_PMCH_CONFIGURATION_LIST_STRUCT* ie, uint8_t** ptr)
{
  LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

  if (ie != NULL && ptr != NULL) {
    if (ie->len > 32) {
      liblte_log_print("ProtocolIE_PMCH-Configuration-List pack error - max supported dynamic sequence length = 32, "
                       "ie->len = %d\n",
                       ie->len);
      return LIBLTE_ERROR_ENCODE_FAIL;
    }

    // Length
    liblte_value_2_bits(ie->len, ptr, 4);
    liblte_align_up_zero(ptr, 8);

    for (uint16_t i = 0; i < ie->len; i++) {
      LIBLTE_BIT_MSG_STRUCT tmp_msg;
      uint8_t*              tmp_ptr;

      // ProtocolIE - PMCH-Configuration-Item
      tmp_ptr = tmp_msg.msg;
      err     = liblte_m2ap_pack_pmchconfigurationitem(&ie->buffer[i], &tmp_ptr);
      if (err != LIBLTE_SUCCESS) {
        return err;
      }
      liblte_align_up_zero(&tmp_ptr, 8);
      tmp_msg.N_bits = tmp_ptr - tmp_msg.msg;
      if (liblte_m2ap_pack_protocolie_header(
              (tmp_msg.N_bits) / 8, LIBLTE_M2AP_IE_ID_PMCH_CONFIGURATION_ITEM, LIBLTE_M2AP_CRITICALITY_REJECT, ptr) !=
          LIBLTE_SUCCESS) {
        return LIBLTE_ERROR_ENCODE_FAIL;
      }
      memcpy(*ptr, tmp_msg.msg, tmp_msg.N_bits);
      *ptr += tmp_msg.N_bits;
    }
    err = LIBLTE_SUCCESS;
  }
  return err;
}

LIBLTE_ERROR_ENUM liblte_m2ap_unpack_pmchconfigurationlist(uint8_t**                                   ptr,
                                                           LIBLTE_M2AP_PMCH_CONFIGURATION_LIST_STRUCT* ie)
{
  LIBLTE_ERROR_ENUM                             err = LIBLTE_ERROR_INVALID_INPUTS;
  LIBLTE_M2AP_CRITICALITY_ENUM                  crit;
  LIBLTE_M2AP_PROTOCOLIE_SINGLECONTAINER_STRUCT proto_container;
  uint32_t                                      ie_id;
  uint32_t                                      len;

  if (ie != NULL && ptr != NULL) {
    // Length TODO!!!
    ie->len = liblte_bits_2_value(ptr, 4);
    liblte_align_up(ptr, 8);
    if (ie->len > 32) {
      liblte_log_print(
          "MBSFN-Area-Configuration-List unpack error - max supported dynamic sequence length = 32, ie->len = %d\n",
          ie->len);
      return LIBLTE_ERROR_DECODE_FAIL;
    }

    for (uint16_t i = 0; i < ie->len; i++) {
      if (liblte_m2ap_unpack_protocolie_header(ptr, &ie_id, &crit, &len) != LIBLTE_SUCCESS) {
        return LIBLTE_ERROR_DECODE_FAIL;
      }
      err = liblte_m2ap_unpack_pmchconfigurationitem(ptr, &ie->buffer[i]);
      if (err != LIBLTE_SUCCESS)
        return err;
    }
    err = LIBLTE_SUCCESS;
  }
  return err;
}

/*******************************************************************************
 * ProtocolIE MBSFN-Area-Configuration-Item SEQUENCE
 *******************************************************************************/
LIBLTE_ERROR_ENUM liblte_m2ap_pack_mbsfnareaconfigurationitem(LIBLTE_M2AP_MBSFN_AREA_CONFIGURATION_ITEM_STRUCT* ie,
                                                              uint8_t**                                         ptr)
{
  LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

  if (ie != NULL && ptr != NULL) {
    int                   n_ies = 4;
    LIBLTE_BIT_MSG_STRUCT tmp_msg;
    uint8_t*              tmp_ptr;

    liblte_value_2_bits(n_ies, ptr, 16);

    // PMCH Configuration List
    tmp_ptr = tmp_msg.msg;
    err     = liblte_m2ap_pack_pmchconfigurationlist(&ie->PMCHConfigurationList, &tmp_ptr);
    if (err != LIBLTE_SUCCESS)
      return err;
    liblte_align_up_zero(&tmp_ptr, 8);
    tmp_msg.N_bits = tmp_ptr - tmp_msg.msg;
    if (liblte_m2ap_pack_protocolie_header(
            (tmp_msg.N_bits) / 8, LIBLTE_M2AP_IE_ID_PMCH_CONFIGURATION_LIST, LIBLTE_M2AP_CRITICALITY_REJECT, ptr) !=
        LIBLTE_SUCCESS) {
      return LIBLTE_ERROR_ENCODE_FAIL;
    }
    memcpy(*ptr, tmp_msg.msg, tmp_msg.N_bits);
    *ptr += tmp_msg.N_bits;

    // MBSFN Subframe Configuration List
    tmp_ptr = tmp_msg.msg;
    err     = liblte_m2ap_pack_mbsfnsubframeconfigurationlist(&ie->MBSFNSubframeConfigurationList, &tmp_ptr);
    if (err != LIBLTE_SUCCESS)
      return err;
    liblte_align_up_zero(&tmp_ptr, 8);
    tmp_msg.N_bits = tmp_ptr - tmp_msg.msg;
    if (liblte_m2ap_pack_protocolie_header((tmp_msg.N_bits) / 8,
                                           LIBLTE_M2AP_IE_ID_MBSFN_SUBFRAME_CONFIGURATION_LIST,
                                           LIBLTE_M2AP_CRITICALITY_REJECT,
                                           ptr) != LIBLTE_SUCCESS) {
      return LIBLTE_ERROR_ENCODE_FAIL;
    }
    memcpy(*ptr, tmp_msg.msg, tmp_msg.N_bits);
    *ptr += tmp_msg.N_bits;

    // Common Subframe Allocation Period
    tmp_ptr = tmp_msg.msg;
    err     = liblte_m2ap_pack_commonsubframeallocationperiod(&ie->CommonSubframeAllocationPeriod, &tmp_ptr);
    if (err != LIBLTE_SUCCESS)
      return err;
    liblte_align_up_zero(&tmp_ptr, 8);
    tmp_msg.N_bits = tmp_ptr - tmp_msg.msg;
    if (liblte_m2ap_pack_protocolie_header((tmp_msg.N_bits) / 8,
                                           LIBLTE_M2AP_IE_ID_COMMON_SUBFRAME_ALLOCATION_PERIOD,
                                           LIBLTE_M2AP_CRITICALITY_REJECT,
                                           ptr) != LIBLTE_SUCCESS) {
      return LIBLTE_ERROR_ENCODE_FAIL;
    }
    memcpy(*ptr, tmp_msg.msg, tmp_msg.N_bits);
    *ptr += tmp_msg.N_bits;

    // MBSFN Area Id
    tmp_ptr = tmp_msg.msg;
    err     = liblte_m2ap_pack_mbsfnareaid(&ie->MBSFNAreaId, &tmp_ptr);
    if (err != LIBLTE_SUCCESS)
      return err;
    liblte_align_up_zero(&tmp_ptr, 8);
    tmp_msg.N_bits = tmp_ptr - tmp_msg.msg;
    if (liblte_m2ap_pack_protocolie_header(
            (tmp_msg.N_bits) / 8, LIBLTE_M2AP_IE_ID_MBSFN_AREA_ID, LIBLTE_M2AP_CRITICALITY_REJECT, ptr) !=
        LIBLTE_SUCCESS) {
      return LIBLTE_ERROR_ENCODE_FAIL;
    }
    memcpy(*ptr, tmp_msg.msg, tmp_msg.N_bits);
    *ptr += tmp_msg.N_bits;

    err = LIBLTE_SUCCESS;
  }
  return err;
}

LIBLTE_ERROR_ENUM liblte_m2ap_unpack_mbsfnareaconfigurationitem(uint8_t**                                         ptr,
                                                                LIBLTE_M2AP_MBSFN_AREA_CONFIGURATION_ITEM_STRUCT* ie)
{
  LIBLTE_ERROR_ENUM            err = LIBLTE_ERROR_INVALID_INPUTS;
  LIBLTE_M2AP_CRITICALITY_ENUM crit;
  uint32_t                     ie_id;
  uint32_t                     len;
  uint32_t                     n_ie;
  uint32_t                     i;

  if (ie != NULL && ptr != NULL) {
    // No. of ProtocolIE-Container
    n_ie = liblte_bits_2_value(ptr, 16);

    // Unpack ProtocolIE Fields
    for (i = 0; i < n_ie; i++) {
      liblte_align_up_zero(ptr, 8);
      if (liblte_m2ap_unpack_protocolie_header(ptr, &ie_id, &crit, &len) != LIBLTE_SUCCESS) {
        return LIBLTE_ERROR_DECODE_FAIL;
      }
      if (LIBLTE_M2AP_IE_ID_PMCH_CONFIGURATION_LIST == ie_id) {
        // PMCH Configuration List
        if (liblte_m2ap_unpack_pmchconfigurationlist(ptr, &ie->PMCHConfigurationList) != LIBLTE_SUCCESS) {
          return LIBLTE_ERROR_DECODE_FAIL;
        }
        liblte_align_up(ptr, 8);
      } else if (LIBLTE_M2AP_IE_ID_MBSFN_SUBFRAME_CONFIGURATION_LIST == ie_id) {
        if (liblte_m2ap_unpack_mbsfnsubframeconfigurationlist(ptr, &ie->MBSFNSubframeConfigurationList) !=
            LIBLTE_SUCCESS) {
          return LIBLTE_ERROR_DECODE_FAIL;
        }
        liblte_align_up(ptr, 8);
      } else if (LIBLTE_M2AP_IE_ID_COMMON_SUBFRAME_ALLOCATION_PERIOD == ie_id) {
        if (liblte_m2ap_unpack_commonsubframeallocationperiod(ptr, &ie->CommonSubframeAllocationPeriod) !=
            LIBLTE_SUCCESS) {
          return LIBLTE_ERROR_DECODE_FAIL;
        }
      } else if (LIBLTE_M2AP_IE_ID_MBSFN_AREA_ID == ie_id) {
        if (liblte_m2ap_unpack_mbsfnareaid(ptr, &ie->MBSFNAreaId) != LIBLTE_SUCCESS) {
          return LIBLTE_ERROR_DECODE_FAIL;
        }
      }
    }
    err = LIBLTE_SUCCESS;
  }
  return err;
}

/***************************************************************************************
 * ProtocolIE MBSFN-Area-Configuration-List SEQUENCE OF ProtocolIE-Single-Container
 ***************************************************************************************/
LIBLTE_ERROR_ENUM liblte_m2ap_pack_mbsfnareaconfigurationlist(LIBLTE_M2AP_MBSFN_AREA_CONFIGURATION_LIST_STRUCT* ie,
                                                              uint8_t**                                         ptr)
{
  LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

  if (ie != NULL && ptr != NULL) {
    if (ie->len > 32) {
      liblte_log_print(
          "ProtocolIE_MBSFN-Area-Configuration-List pack error - max supported dynamic sequence length = 32, "
          "ie->len = %d\n",
          ie->len);
      return LIBLTE_ERROR_ENCODE_FAIL;
    }

    // Length
    liblte_value_2_bits(ie->len - 1, ptr, 8);

    for (uint8_t i = 0; i < ie->len; i++) {
      // ProtocolIE - MBSFN-Area-Configuration-Item
      err = liblte_m2ap_pack_mbsfnareaconfigurationitem(&ie->buffer[i], ptr);
      if (err != LIBLTE_SUCCESS) {
        return err;
      }
      liblte_align_up_zero(ptr, 8);
    }
    err = LIBLTE_SUCCESS;
  }
  return err;
}

LIBLTE_ERROR_ENUM liblte_m2ap_unpack_mbsfnareaconfigurationlist(uint8_t**                                         ptr,
                                                                LIBLTE_M2AP_MBSFN_AREA_CONFIGURATION_LIST_STRUCT* ie)
{
  LIBLTE_ERROR_ENUM                             err = LIBLTE_ERROR_INVALID_INPUTS;
  LIBLTE_M2AP_CRITICALITY_ENUM                  crit;
  LIBLTE_M2AP_PROTOCOLIE_SINGLECONTAINER_STRUCT proto_container;
  uint32_t                                      ie_id;
  uint32_t                                      len;

  if (ie != NULL && ptr != NULL) {
    // Length TODO!!!
    ie->len = liblte_bits_2_value(ptr, 8) + 1;
    liblte_align_up(ptr, 8);
    if (ie->len > 32) {
      liblte_log_print(
          "MBSFN-Area-Configuration-List unpack error - max supported dynamic sequence length = 32, ie->len = %d\n",
          ie->len);
      return LIBLTE_ERROR_DECODE_FAIL;
    }

    for (uint8_t i = 0; i < ie->len; i++) {
      err = liblte_m2ap_unpack_mbsfnareaconfigurationitem(ptr, &ie->buffer[i]);
      if (err != LIBLTE_SUCCESS)
        return err;
    }
    err = LIBLTE_SUCCESS;
  }
  return err;
}
/*******************************************************************************
 * Protocol Message M2SetupRequest STRUCT
 *******************************************************************************/
LIBLTE_ERROR_ENUM liblte_m2ap_pack_m2setuprequest(LIBLTE_M2AP_MESSAGE_M2SETUPREQUEST_STRUCT* msg, uint8_t** ptr)
{
  LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

  if (msg != NULL && ptr != NULL) {

    // Extension
    liblte_value_2_bits(msg->ext ? 1 : 0, ptr, 1);
    liblte_align_up_zero(ptr, 8);
    if (msg->ext) {
      liblte_log_print("M2SetupRequestIEs error: M2AP ASN extensions not currently supported\n");
      return LIBLTE_ERROR_ENCODE_FAIL;
    }

    // No. of ProtocolIE
    uint32_t n_ie = 3;
    if (!msg->eNBname_present)
      n_ie--;
    liblte_value_2_bits(n_ie, ptr, 16);

    // Temp container for IEs
    LIBLTE_BIT_MSG_STRUCT tmp_msg = {};
    uint8_t*              tmp_ptr;

    // ProtocolIE - Global_ENB_ID
    tmp_ptr = tmp_msg.msg;
    if (liblte_m2ap_pack_globalenbid(&msg->Global_ENB_ID, &tmp_ptr) != LIBLTE_SUCCESS) {
      return LIBLTE_ERROR_ENCODE_FAIL;
    }
    liblte_align_up_zero(&tmp_ptr, 8);
    tmp_msg.N_bits = tmp_ptr - tmp_msg.msg;
    if (liblte_m2ap_pack_protocolie_header(
            (tmp_msg.N_bits) / 8, LIBLTE_M2AP_IE_ID_GLOBALENB_ID, LIBLTE_M2AP_CRITICALITY_REJECT, ptr) !=
        LIBLTE_SUCCESS) {
      return LIBLTE_ERROR_ENCODE_FAIL;
    }
    memcpy(*ptr, tmp_msg.msg, tmp_msg.N_bits);
    *ptr += tmp_msg.N_bits;

    // ProtocolIE - eNBname
    if (msg->eNBname_present) {
      tmp_ptr = tmp_msg.msg;
      if (liblte_m2ap_pack_enbname(&msg->eNBname, &tmp_ptr) != LIBLTE_SUCCESS) {
        return LIBLTE_ERROR_ENCODE_FAIL;
      }
      liblte_align_up_zero(&tmp_ptr, 8);
      tmp_msg.N_bits = tmp_ptr - tmp_msg.msg;
      if (liblte_m2ap_pack_protocolie_header(
              tmp_msg.N_bits / 8, LIBLTE_M2AP_IE_ID_ENBNAME, LIBLTE_M2AP_CRITICALITY_IGNORE, ptr) != LIBLTE_SUCCESS) {
        return LIBLTE_ERROR_ENCODE_FAIL;
      }
      memcpy(*ptr, tmp_msg.msg, tmp_msg.N_bits);
      *ptr += tmp_msg.N_bits;
    }

    // Protocol IE - eNB-MBMS-Configuration-Data-List
    tmp_ptr = tmp_msg.msg;
    if (liblte_m2ap_pack_enbmbmsconfigurationdatalist(&msg->configurationDataList, &tmp_ptr) != LIBLTE_SUCCESS) {
      return LIBLTE_ERROR_ENCODE_FAIL;
    }
    liblte_align_up_zero(&tmp_ptr, 8);
    tmp_msg.N_bits = tmp_ptr - tmp_msg.msg;
    if (liblte_m2ap_pack_protocolie_header(tmp_msg.N_bits / 8,
                                           LIBLTE_M2AP_IE_ID_ENB_MBMS_CONFIGURATION_DATA_LIST,
                                           LIBLTE_M2AP_CRITICALITY_REJECT,
                                           ptr) != LIBLTE_SUCCESS) {
      return LIBLTE_ERROR_ENCODE_FAIL;
    }
    memcpy(*ptr, tmp_msg.msg, tmp_msg.N_bits);
    *ptr += tmp_msg.N_bits;

    err = LIBLTE_SUCCESS;
  }
  return err;
}

LIBLTE_ERROR_ENUM liblte_m2ap_unpack_m2setuprequest(uint8_t** ptr, LIBLTE_M2AP_MESSAGE_M2SETUPREQUEST_STRUCT* msg)
{
  LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

  if (msg != NULL && ptr != NULL) {
    LIBLTE_M2AP_CRITICALITY_ENUM crit;
    uint32_t                     ie_id;
    uint32_t                     len;
    uint32_t                     n_ie;
    uint32_t                     i;

    // Set booleans
    msg->eNBname_present = false;

    // Extension
    msg->ext = liblte_bits_2_value(ptr, 1);
    liblte_align_up(ptr, 8);
    if (msg->ext) {
      liblte_log_print("M2SetupRequestIEs error: M2AP ASN extensions not currently supported\n");
      return LIBLTE_ERROR_DECODE_FAIL;
    }

    // No. of ProtocolIE-Container
    n_ie = liblte_bits_2_value(ptr, 16);

    // Unpack ProtocolIE Fields
    for (i = 0; i < n_ie; i++) {
      if (liblte_m2ap_unpack_protocolie_header(ptr, &ie_id, &crit, &len) != LIBLTE_SUCCESS) {
        return LIBLTE_ERROR_DECODE_FAIL;
      }
      if (LIBLTE_M2AP_IE_ID_GLOBALENB_ID == ie_id) {
        if (liblte_m2ap_unpack_globalenbid(ptr, &msg->Global_ENB_ID) != LIBLTE_SUCCESS) {
          return LIBLTE_ERROR_DECODE_FAIL;
        }
        liblte_align_up(ptr, 8);
      } else if (LIBLTE_M2AP_IE_ID_ENBNAME == ie_id) {
        if (liblte_m2ap_unpack_enbname(ptr, &msg->eNBname) != LIBLTE_SUCCESS) {
          return LIBLTE_ERROR_DECODE_FAIL;
        }
        liblte_align_up(ptr, 8);
        msg->eNBname_present = true;
      } else if (LIBLTE_M2AP_IE_ID_ENB_MBMS_CONFIGURATION_DATA_LIST == ie_id) {
        if (liblte_m2ap_unpack_enbmbmsconfigurationdatalist(ptr, &msg->configurationDataList) != LIBLTE_SUCCESS) {
          return LIBLTE_ERROR_DECODE_FAIL;
        }
      }
    }

    err = LIBLTE_SUCCESS;
  }
  return err;
}

/*******************************************************************************
 * Protocol Message M2SetupResponse STRUCT
 *******************************************************************************/
LIBLTE_ERROR_ENUM liblte_m2ap_pack_m2setupresponse(LIBLTE_M2AP_MESSAGE_M2SETUPRESPONSE_STRUCT* msg, uint8_t** ptr)
{
  LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

  if (msg != NULL && ptr != NULL) {
    // Extension
    liblte_value_2_bits(msg->ext ? 1 : 0, ptr, 1);
    liblte_align_up_zero(ptr, 8);
    if (msg->ext) {
      liblte_log_print("M2SetupResponseIEs error: M2AP ASN extensions not currently supported\n");
      return LIBLTE_ERROR_ENCODE_FAIL;
    }

    // No. of ProtocolIE
    uint32_t n_ie = 2;
    liblte_value_2_bits(n_ie, ptr, 16);

    // Temp container for IEs
    LIBLTE_BIT_MSG_STRUCT tmp_msg;
    uint8_t*              tmp_ptr;

    // ProtocolIE - Global_MCE_ID
    tmp_ptr = tmp_msg.msg;
    if (liblte_m2ap_pack_globalmceid(&msg->Global_MCE_ID, &tmp_ptr) != LIBLTE_SUCCESS) {
      return LIBLTE_ERROR_ENCODE_FAIL;
    }
    liblte_align_up_zero(&tmp_ptr, 8);
    tmp_msg.N_bits = tmp_ptr - tmp_msg.msg;
    if (liblte_m2ap_pack_protocolie_header(
            (tmp_msg.N_bits) / 8, LIBLTE_M2AP_IE_ID_GLOBALMCE_ID, LIBLTE_M2AP_CRITICALITY_REJECT, ptr) !=
        LIBLTE_SUCCESS) {
      return LIBLTE_ERROR_ENCODE_FAIL;
    }
    memcpy(*ptr, tmp_msg.msg, tmp_msg.N_bits);
    *ptr += tmp_msg.N_bits;

    // ProtocolIE - MCEname TODO

    // ProtocolIE - MCCHrelatedBCCH-ConfigPerMBSFNArea
    tmp_ptr = tmp_msg.msg;
    if (liblte_m2ap_pack_mcchrelatedbcchconfigpermbsfnarea(&msg->MCCHrelatedBCCHConfigPerMBSFNArea, &tmp_ptr) !=
        LIBLTE_SUCCESS) {
      return LIBLTE_ERROR_ENCODE_FAIL;
    }
    liblte_align_up_zero(&tmp_ptr, 8);
    tmp_msg.N_bits = tmp_ptr - tmp_msg.msg;
    if (liblte_m2ap_pack_protocolie_header((tmp_msg.N_bits) / 8,
                                           LIBLTE_M2AP_IE_ID_MCCHRELATEDBCCH_CONFIGPERMBSFNAREA,
                                           LIBLTE_M2AP_CRITICALITY_REJECT,
                                           ptr) != LIBLTE_SUCCESS) {
      return LIBLTE_ERROR_ENCODE_FAIL;
    }
    memcpy(*ptr, tmp_msg.msg, tmp_msg.N_bits);
    *ptr += tmp_msg.N_bits;
    err = LIBLTE_SUCCESS;
  }
  return err;
}

LIBLTE_ERROR_ENUM liblte_m2ap_unpack_m2setupresponse(uint8_t** ptr, LIBLTE_M2AP_MESSAGE_M2SETUPRESPONSE_STRUCT* msg)
{
  LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

  if (msg != NULL && ptr != NULL) {
    LIBLTE_M2AP_CRITICALITY_ENUM crit;
    uint32_t                     ie_id;
    uint32_t                     len;
    uint32_t                     n_ie;
    uint32_t                     i;

    // Set bools
    msg->MCEname_present = false;

    // Extension
    msg->ext = liblte_bits_2_value(ptr, 1);
    liblte_align_up(ptr, 8);
    if (msg->ext) {
      liblte_log_print("M2SetupResponseIEs error: M2AP ASN extensions not currently supported\n");
      return LIBLTE_ERROR_DECODE_FAIL;
    }

    // No. of ProtocolIE-Container
    n_ie = liblte_bits_2_value(ptr, 16);
    // Unpack ProtocolIE Fields
    for (i = 0; i < n_ie; i++) {
      if (liblte_m2ap_unpack_protocolie_header(ptr, &ie_id, &crit, &len) != LIBLTE_SUCCESS) {
        return LIBLTE_ERROR_DECODE_FAIL;
      }
      if (LIBLTE_M2AP_IE_ID_GLOBALMCE_ID == ie_id) {
        if (liblte_m2ap_unpack_globalmceid(ptr, &msg->Global_MCE_ID) != LIBLTE_SUCCESS) {
          return LIBLTE_ERROR_DECODE_FAIL;
        }
        liblte_align_up(ptr, 8);
      } else if (LIBLTE_M2AP_IE_ID_MCENAME == ie_id) {
        if (liblte_m2ap_unpack_mcename(ptr, &msg->MCEname) != LIBLTE_SUCCESS) {
          return LIBLTE_ERROR_DECODE_FAIL;
        }
        liblte_align_up(ptr, 8);
        msg->MCEname_present = true;
      } else if (LIBLTE_M2AP_IE_ID_MCCHRELATEDBCCH_CONFIGPERMBSFNAREA == ie_id) {
        if (liblte_m2ap_unpack_mcchrelatedbcchconfigpermbsfnarea(ptr, &msg->MCCHrelatedBCCHConfigPerMBSFNArea) !=
            LIBLTE_SUCCESS) {
          return LIBLTE_ERROR_DECODE_FAIL;
        }
        liblte_align_up(ptr, 8);
      }
    }
    err = LIBLTE_SUCCESS;
  }
  return err;
}

/*******************************************************************************
 * Protocol Message MBMSSessionStartRequest STRUCT
 *******************************************************************************/
LIBLTE_ERROR_ENUM liblte_m2ap_pack_mbmssessionstartrequest(LIBLTE_M2AP_MESSAGE_MBMSSESSIONSTARTREQUEST_STRUCT* msg,
                                                           uint8_t**                                           ptr)
{
  LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

  if (msg != NULL && ptr != NULL) {
    // Extension
    liblte_value_2_bits(msg->ext ? 1 : 0, ptr, 1);
    liblte_align_up_zero(ptr, 8);
    if (msg->ext) {
      liblte_log_print("M2SetupResponseIEs error: M2AP ASN extensions not currently supported\n");
      return LIBLTE_ERROR_ENCODE_FAIL;
    }

    // No. of ProtocolIE
    uint32_t n_ie = 4;
    liblte_value_2_bits(n_ie, ptr, 16);

    // Temp container for IEs
    LIBLTE_BIT_MSG_STRUCT tmp_msg;
    uint8_t*              tmp_ptr;

    // ProtocolIE - MCE MBMS M2AP Id
    tmp_ptr = tmp_msg.msg;
    if (liblte_m2ap_pack_mcembmsm2apid(&msg->MceMbmsM2apId, &tmp_ptr) != LIBLTE_SUCCESS) {
      return LIBLTE_ERROR_ENCODE_FAIL;
    }
    liblte_align_up_zero(&tmp_ptr, 8);
    tmp_msg.N_bits = tmp_ptr - tmp_msg.msg;
    if (liblte_m2ap_pack_protocolie_header(
            (tmp_msg.N_bits) / 8, LIBLTE_M2AP_IE_ID_MCE_MBMS_M2AP_ID, LIBLTE_M2AP_CRITICALITY_REJECT, ptr) !=
        LIBLTE_SUCCESS) {
      return LIBLTE_ERROR_ENCODE_FAIL;
    }
    memcpy(*ptr, tmp_msg.msg, tmp_msg.N_bits);
    *ptr += tmp_msg.N_bits;

    // ProtocolIE - TMGI
    tmp_ptr = tmp_msg.msg;
    if (liblte_m2ap_pack_tmgi(&msg->Tmgi, &tmp_ptr) != LIBLTE_SUCCESS) {
      return LIBLTE_ERROR_ENCODE_FAIL;
    }
    liblte_align_up_zero(&tmp_ptr, 8);
    tmp_msg.N_bits = tmp_ptr - tmp_msg.msg;
    if (liblte_m2ap_pack_protocolie_header(
            (tmp_msg.N_bits) / 8, LIBLTE_M2AP_IE_ID_TMGI, LIBLTE_M2AP_CRITICALITY_REJECT, ptr) != LIBLTE_SUCCESS) {
      return LIBLTE_ERROR_ENCODE_FAIL;
    }
    memcpy(*ptr, tmp_msg.msg, tmp_msg.N_bits);
    *ptr += tmp_msg.N_bits;

    // ProtocolIE - MBMS Session Id
    /*
    if (ie->MBMSSessionId_present) {
      tmp_ptr = tmp_msg.msg;
      if(liblte_m2ap_pack_mbmssessionid(&msg->MbmsSessionId, &tmp_ptr) != LIBLTE_SUCCESS) {
         return LIBLTE_ERROR_ENCODE_FAIL;
      }
      liblte_align_up_zero(&tmp_ptr, 8);
      tmp_msg.N_bits = tmp_ptr - tmp_msg.msg;
      if(liblte_m2ap_pack_protocolie_header((tmp_msg.N_bits) / 8,
                                          LIBLTE_M2AP_IE_ID_TMGI,
                                          LIBLTE_M2AP_CRITICALITY_REJECT,
                                          ptr) != LIBLTE_SUCCESS) {
        return LIBLTE_ERROR_ENCODE_FAIL;
      }
    }
    memcpy(*ptr, tmp_msg.msg, tmp_msg.N_bits);
    *ptr += tmp_msg.N_bits;
    */

    // ProtocolIE - MBMS Service Area
    tmp_ptr = tmp_msg.msg;
    if (liblte_m2ap_pack_mbmsservicearea(&msg->MbmsServiceArea, &tmp_ptr) != LIBLTE_SUCCESS) {
      return LIBLTE_ERROR_ENCODE_FAIL;
    }
    liblte_align_up_zero(&tmp_ptr, 8);
    tmp_msg.N_bits = tmp_ptr - tmp_msg.msg;
    if (liblte_m2ap_pack_protocolie_header(
            (tmp_msg.N_bits) / 8, LIBLTE_M2AP_IE_ID_MBMS_SERVICE_AREA, LIBLTE_M2AP_CRITICALITY_REJECT, ptr) !=
        LIBLTE_SUCCESS) {
      return LIBLTE_ERROR_ENCODE_FAIL;
    }
    memcpy(*ptr, tmp_msg.msg, tmp_msg.N_bits);
    *ptr += tmp_msg.N_bits;

    // ProtocolIE - TNL Information
    tmp_ptr = tmp_msg.msg;
    if (liblte_m2ap_pack_tnlinformation(&msg->TnlInformation, &tmp_ptr) != LIBLTE_SUCCESS) {
      return LIBLTE_ERROR_ENCODE_FAIL;
    }
    liblte_align_up_zero(&tmp_ptr, 8);
    tmp_msg.N_bits = tmp_ptr - tmp_msg.msg;
    if (liblte_m2ap_pack_protocolie_header(
            (tmp_msg.N_bits) / 8, LIBLTE_M2AP_IE_ID_TNL_INFORMATION, LIBLTE_M2AP_CRITICALITY_REJECT, ptr) !=
        LIBLTE_SUCCESS) {
      return LIBLTE_ERROR_ENCODE_FAIL;
    }
    memcpy(*ptr, tmp_msg.msg, tmp_msg.N_bits);
    *ptr += tmp_msg.N_bits;

    err = LIBLTE_SUCCESS;
  }
  return err;
}

LIBLTE_ERROR_ENUM liblte_m2ap_unpack_mbmssessionstartrequest(uint8_t**                                           ptr,
                                                             LIBLTE_M2AP_MESSAGE_MBMSSESSIONSTARTREQUEST_STRUCT* msg)
{
  LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

  if (msg != NULL && ptr != NULL) {
    LIBLTE_M2AP_CRITICALITY_ENUM crit;
    uint32_t                     ie_id;
    uint32_t                     len;
    uint32_t                     n_ie;
    uint32_t                     i;

    // Set bools
    msg->MbmsSessionId_present = false;

    // Extension
    msg->ext = liblte_bits_2_value(ptr, 1);
    liblte_align_up(ptr, 8);
    if (msg->ext) {
      liblte_log_print("M2SetupResponseIEs error: M2AP ASN extensions not currently supported\n");
      return LIBLTE_ERROR_DECODE_FAIL;
    }

    // No. of ProtocolIE-Container
    n_ie = liblte_bits_2_value(ptr, 16);
    // Unpack ProtocolIE Fields
    for (i = 0; i < n_ie; i++) {

      if (liblte_m2ap_unpack_protocolie_header(ptr, &ie_id, &crit, &len) != LIBLTE_SUCCESS) {
        return LIBLTE_ERROR_DECODE_FAIL;
      }
      if (LIBLTE_M2AP_IE_ID_MCE_MBMS_M2AP_ID == ie_id) {
        if (liblte_m2ap_unpack_mcembmsm2apid(ptr, &msg->MceMbmsM2apId) != LIBLTE_SUCCESS) {
          return LIBLTE_ERROR_DECODE_FAIL;
        }
        liblte_align_up(ptr, 8);
      } else if (LIBLTE_M2AP_IE_ID_TMGI == ie_id) {
        if (liblte_m2ap_unpack_tmgi(ptr, &msg->Tmgi) != LIBLTE_SUCCESS) {
          return LIBLTE_ERROR_DECODE_FAIL;
        }
        liblte_align_up(ptr, 8);
      } else if (LIBLTE_M2AP_IE_ID_MBMS_SESSION_ID == ie_id) {
        msg->MbmsSessionId_present = true;
        // if(liblte_m2ap_unpack_mbmssessionid(ptr, &msg->MbmsSessionId) != LIBLTE_SUCCESS) {
        //  return LIBLTE_ERROR_DECODE_FAIL;
        //}
        liblte_align_up(ptr, 8);
      } else if (LIBLTE_M2AP_IE_ID_MBMS_SERVICE_AREA == ie_id) {
        if (liblte_m2ap_unpack_mbmsservicearea(ptr, &msg->MbmsServiceArea) != LIBLTE_SUCCESS) {
          return LIBLTE_ERROR_DECODE_FAIL;
        }
        liblte_align_up(ptr, 8);
      } else if (LIBLTE_M2AP_IE_ID_TNL_INFORMATION == ie_id) {
        if (liblte_m2ap_unpack_tnlinformation(ptr, &msg->TnlInformation) != LIBLTE_SUCCESS) {
          return LIBLTE_ERROR_DECODE_FAIL;
        }
        liblte_align_up(ptr, 8);
      }
    }
    err = LIBLTE_SUCCESS;
  }
  return err;
}

/*******************************************************************************
 * Protocol Message MBMSSessionStartResponse STRUCT
 *******************************************************************************/
LIBLTE_ERROR_ENUM liblte_m2ap_pack_mbmssessionstartresponse(LIBLTE_M2AP_MESSAGE_MBMSSESSIONSTARTRESPONSE_STRUCT* msg,
                                                            uint8_t**                                            ptr)
{
  LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

  if (msg != NULL && ptr != NULL) {
    // Extension
    liblte_value_2_bits(msg->ext ? 1 : 0, ptr, 1);
    liblte_align_up_zero(ptr, 8);
    if (msg->ext) {
      liblte_log_print("M2SetupResponseIEs error: M2AP ASN extensions not currently supported\n");
      return LIBLTE_ERROR_ENCODE_FAIL;
    }

    // No. of ProtocolIE
    uint32_t n_ie = 2;
    liblte_value_2_bits(n_ie, ptr, 16);

    // Temp container for IEs
    LIBLTE_BIT_MSG_STRUCT tmp_msg;
    uint8_t*              tmp_ptr;

    // ProtocolIE - MCE MBMS M2AP Id
    tmp_ptr = tmp_msg.msg;
    if (liblte_m2ap_pack_mcembmsm2apid(&msg->MceMbmsM2apId, &tmp_ptr) != LIBLTE_SUCCESS) {
      return LIBLTE_ERROR_ENCODE_FAIL;
    }
    liblte_align_up_zero(&tmp_ptr, 8);
    tmp_msg.N_bits = tmp_ptr - tmp_msg.msg;
    if (liblte_m2ap_pack_protocolie_header(
            (tmp_msg.N_bits) / 8, LIBLTE_M2AP_IE_ID_MCE_MBMS_M2AP_ID, LIBLTE_M2AP_CRITICALITY_REJECT, ptr) !=
        LIBLTE_SUCCESS) {
      return LIBLTE_ERROR_ENCODE_FAIL;
    }
    memcpy(*ptr, tmp_msg.msg, tmp_msg.N_bits);
    *ptr += tmp_msg.N_bits;

    // ProtocolIE - ENB MBMS M2AP Id
    tmp_ptr = tmp_msg.msg;
    if (liblte_m2ap_pack_enbmbmsm2apid(&msg->EnbMbmsM2apId, &tmp_ptr) != LIBLTE_SUCCESS) {
      return LIBLTE_ERROR_ENCODE_FAIL;
    }
    liblte_align_up_zero(&tmp_ptr, 8);
    tmp_msg.N_bits = tmp_ptr - tmp_msg.msg;
    if (liblte_m2ap_pack_protocolie_header(
            (tmp_msg.N_bits) / 8, LIBLTE_M2AP_IE_ID_ENB_MBMS_M2AP_ID, LIBLTE_M2AP_CRITICALITY_REJECT, ptr) !=
        LIBLTE_SUCCESS) {
      return LIBLTE_ERROR_ENCODE_FAIL;
    }
    memcpy(*ptr, tmp_msg.msg, tmp_msg.N_bits);
    *ptr += tmp_msg.N_bits;

    // ProtocolIE - Criticality Diagnostics
    /*
    tmp_ptr = tmp_msg.msg;
    if(liblte_m2ap_pack_mbmsservicearea(&msg->MbmsServiceArea, &tmp_ptr) != LIBLTE_SUCCESS) {
      return LIBLTE_ERROR_ENCODE_FAIL;
    }
    liblte_align_up_zero(&tmp_ptr, 8);
    tmp_msg.N_bits = tmp_ptr - tmp_msg.msg;
    if(liblte_m2ap_pack_protocolie_header((tmp_msg.N_bits) / 8,
                                          LIBLTE_M2AP_IE_ID_MBMS_SERVICE_AREA,
                                          LIBLTE_M2AP_CRITICALITY_REJECT,
                                          ptr) != LIBLTE_SUCCESS) {
      return LIBLTE_ERROR_ENCODE_FAIL;
    }
    memcpy(*ptr, tmp_msg.msg, tmp_msg.N_bits);
    *ptr += tmp_msg.N_bits;
    */

    err = LIBLTE_SUCCESS;
  }
  return err;
}

LIBLTE_ERROR_ENUM liblte_m2ap_unpack_mbmssessionstartresponse(uint8_t**                                            ptr,
                                                              LIBLTE_M2AP_MESSAGE_MBMSSESSIONSTARTRESPONSE_STRUCT* msg)
{
  LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

  if (msg != NULL && ptr != NULL) {
    LIBLTE_M2AP_CRITICALITY_ENUM crit;
    uint32_t                     ie_id;
    uint32_t                     len;
    uint32_t                     n_ie;
    uint32_t                     i;

    // Set bools
    msg->CriticalityDiagnostics_present = false;

    // Extension
    msg->ext = liblte_bits_2_value(ptr, 1);
    liblte_align_up(ptr, 8);
    if (msg->ext) {
      liblte_log_print("M2SetupResponseIEs error: M2AP ASN extensions not currently supported\n");
      return LIBLTE_ERROR_DECODE_FAIL;
    }

    // No. of ProtocolIE-Container
    n_ie = liblte_bits_2_value(ptr, 16);
    // Unpack ProtocolIE Fields
    for (i = 0; i < n_ie; i++) {

      if (liblte_m2ap_unpack_protocolie_header(ptr, &ie_id, &crit, &len) != LIBLTE_SUCCESS) {
        return LIBLTE_ERROR_DECODE_FAIL;
      }
      if (LIBLTE_M2AP_IE_ID_MCE_MBMS_M2AP_ID == ie_id) {
        if (liblte_m2ap_unpack_mcembmsm2apid(ptr, &msg->MceMbmsM2apId) != LIBLTE_SUCCESS) {
          return LIBLTE_ERROR_DECODE_FAIL;
        }
        liblte_align_up(ptr, 8);
      } else if (LIBLTE_M2AP_IE_ID_ENB_MBMS_M2AP_ID == ie_id) {
        if (liblte_m2ap_unpack_enbmbmsm2apid(ptr, &msg->EnbMbmsM2apId) != LIBLTE_SUCCESS) {
          return LIBLTE_ERROR_DECODE_FAIL;
        }
        liblte_align_up(ptr, 8);
      } else if (LIBLTE_M2AP_IE_ID_CRITICALITYDIAGNOSTICS == ie_id) {
        /*msg->CriticalityDiagnostics_present = true;
        if(liblte_m2ap_unpack_criticalitydiagnostics(ptr, &msg->CriticalityDiagnostics) != LIBLTE_SUCCESS) {
          return LIBLTE_ERROR_DECODE_FAIL;
        }
        liblte_align_up(ptr, 8);*/
      }
    }
    err = LIBLTE_SUCCESS;
  }
  return err;
}

/*******************************************************************************
 * Protocol Message MBMS Scheduling Information STRUCT
 *******************************************************************************/
LIBLTE_ERROR_ENUM liblte_m2ap_pack_mbmsschedulinginformation(LIBLTE_M2AP_MESSAGE_MBMSSCHEDULINGINFORMATION_STRUCT* msg,
                                                             uint8_t**                                             ptr)
{
  LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

  if (msg != NULL && ptr != NULL) {

    // Extension
    liblte_value_2_bits(msg->ext ? 1 : 0, ptr, 1);
    liblte_align_up_zero(ptr, 8);
    if (msg->ext) {
      liblte_log_print("MBMSSchedulingInformation IEs error: M2AP ASN extensions not currently supported\n");
      return LIBLTE_ERROR_ENCODE_FAIL;
    }

    // No. of ProtocolIE
    uint32_t n_ie = 2;
    liblte_value_2_bits(n_ie, ptr, 16);

    // Temp container for IEs
    LIBLTE_BIT_MSG_STRUCT tmp_msg;
    uint8_t*              tmp_ptr;

    // ProtocolIE - MCCH-Update-Time
    tmp_ptr = tmp_msg.msg;
    if (liblte_m2ap_pack_mcchupdatetime(&msg->MCCHUpdateTime, &tmp_ptr) != LIBLTE_SUCCESS) {
      return LIBLTE_ERROR_ENCODE_FAIL;
    }
    liblte_align_up_zero(&tmp_ptr, 8);
    tmp_msg.N_bits = tmp_ptr - tmp_msg.msg;
    if (liblte_m2ap_pack_protocolie_header(
            (tmp_msg.N_bits) / 8, LIBLTE_M2AP_IE_ID_MCCH_UPDATE_TIME, LIBLTE_M2AP_CRITICALITY_REJECT, ptr) !=
        LIBLTE_SUCCESS) {
      return LIBLTE_ERROR_ENCODE_FAIL;
    }
    memcpy(*ptr, tmp_msg.msg, tmp_msg.N_bits);
    *ptr += tmp_msg.N_bits;

    // ProtocolIE - MBSFN-Area-Configuration-List
    tmp_ptr = tmp_msg.msg;
    if (liblte_m2ap_pack_mbsfnareaconfigurationlist(&msg->MbsfnAreaConfigurationList, &tmp_ptr) != LIBLTE_SUCCESS) {
      return LIBLTE_ERROR_ENCODE_FAIL;
    }
    liblte_align_up_zero(&tmp_ptr, 8);
    tmp_msg.N_bits = tmp_ptr - tmp_msg.msg;
    if (liblte_m2ap_pack_protocolie_header(
            tmp_msg.N_bits / 8, LIBLTE_M2AP_IE_ID_MBSFN_AREA_CONFIGURATION_LIST, LIBLTE_M2AP_CRITICALITY_REJECT, ptr) !=
        LIBLTE_SUCCESS) {
      return LIBLTE_ERROR_ENCODE_FAIL;
    }
    memcpy(*ptr, tmp_msg.msg, tmp_msg.N_bits);
    *ptr += tmp_msg.N_bits;

    err = LIBLTE_SUCCESS;
  }
  return err;
}

LIBLTE_ERROR_ENUM
liblte_m2ap_unpack_mbmsschedulinginformation(uint8_t** ptr, LIBLTE_M2AP_MESSAGE_MBMSSCHEDULINGINFORMATION_STRUCT* msg)
{
  LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

  if (msg != NULL && ptr != NULL) {
    LIBLTE_M2AP_CRITICALITY_ENUM crit;
    uint32_t                     ie_id;
    uint32_t                     len;
    uint32_t                     n_ie;
    uint32_t                     i;

    // Extension
    msg->ext = liblte_bits_2_value(ptr, 1);
    liblte_align_up(ptr, 8);
    if (msg->ext) {
      liblte_log_print("M2SetupRequestIEs error: M2AP ASN extensions not currently supported\n");
      return LIBLTE_ERROR_DECODE_FAIL;
    }

    // No. of ProtocolIE-Container
    n_ie = liblte_bits_2_value(ptr, 16);

    // Unpack ProtocolIE Fields
    for (i = 0; i < n_ie; i++) {
      if (liblte_m2ap_unpack_protocolie_header(ptr, &ie_id, &crit, &len) != LIBLTE_SUCCESS) {
        return LIBLTE_ERROR_DECODE_FAIL;
      }

      if (LIBLTE_M2AP_IE_ID_MCCH_UPDATE_TIME == ie_id) {
        if (liblte_m2ap_unpack_mcchupdatetime(ptr, &msg->MCCHUpdateTime) != LIBLTE_SUCCESS) {
          return LIBLTE_ERROR_DECODE_FAIL;
        }
        liblte_align_up(ptr, 8);
      } else if (LIBLTE_M2AP_IE_ID_MBSFN_AREA_CONFIGURATION_LIST == ie_id) {
        if (liblte_m2ap_unpack_mbsfnareaconfigurationlist(ptr, &msg->MbsfnAreaConfigurationList) != LIBLTE_SUCCESS) {
          return LIBLTE_ERROR_DECODE_FAIL;
        }
        liblte_align_up(ptr, 8);
      }
    }

    err = LIBLTE_SUCCESS;
  }
  return err;
}

/*******************************************************************************
 * Protocol Message MBMS Scheduling Information Response STRUCT
 *******************************************************************************/
LIBLTE_ERROR_ENUM
liblte_m2ap_pack_mbmsschedulinginformationresponse(LIBLTE_M2AP_MESSAGE_MBMSSCHEDULINGINFORMATIONRESPONSE_STRUCT* msg,
                                                   uint8_t**                                                     ptr)
{
  LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

  if (msg != NULL && ptr != NULL) {

    // Extension
    liblte_value_2_bits(msg->ext ? 1 : 0, ptr, 1);
    liblte_align_up_zero(ptr, 8);
    if (msg->ext) {
      liblte_log_print("MBMSSchedulingInformation IEs error: M2AP ASN extensions not currently supported\n");
      return LIBLTE_ERROR_ENCODE_FAIL;
    }

    // No. of ProtocolIE
    uint32_t n_ie = 1;
    if (msg->CriticalityDiagnostics_present == false) {
      --n_ie;
    }
    liblte_value_2_bits(n_ie, ptr, 16);

    if (msg->CriticalityDiagnostics_present) {
      // Temp container for IEs
      LIBLTE_BIT_MSG_STRUCT tmp_msg;
      uint8_t*              tmp_ptr;

      // ProtocolIE - Criticality Diagnostics
      tmp_ptr = tmp_msg.msg;
      if (liblte_m2ap_pack_criticalitydiagnostics(&msg->CriticalityDiagnostics, &tmp_ptr) != LIBLTE_SUCCESS) {
        return LIBLTE_ERROR_ENCODE_FAIL;
      }
      liblte_align_up_zero(&tmp_ptr, 8);
      tmp_msg.N_bits = tmp_ptr - tmp_msg.msg;
      if (liblte_m2ap_pack_protocolie_header(
              (tmp_msg.N_bits) / 8, LIBLTE_M2AP_IE_ID_CRITICALITYDIAGNOSTICS, LIBLTE_M2AP_CRITICALITY_REJECT, ptr) !=
          LIBLTE_SUCCESS) {
        return LIBLTE_ERROR_ENCODE_FAIL;
      }
      memcpy(*ptr, tmp_msg.msg, tmp_msg.N_bits);
      *ptr += tmp_msg.N_bits;
    }
    err = LIBLTE_SUCCESS;
  }
  return err;
}

LIBLTE_ERROR_ENUM
liblte_m2ap_unpack_mbmsschedulinginformationresponse(uint8_t**                                                     ptr,
                                                     LIBLTE_M2AP_MESSAGE_MBMSSCHEDULINGINFORMATIONRESPONSE_STRUCT* msg)
{
  LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

  if (msg != NULL && ptr != NULL) {
    LIBLTE_M2AP_CRITICALITY_ENUM crit;
    uint32_t                     ie_id;
    uint32_t                     len;
    uint32_t                     n_ie;
    uint32_t                     i;

    // Extension
    msg->ext = liblte_bits_2_value(ptr, 1);
    liblte_align_up(ptr, 8);
    if (msg->ext) {
      liblte_log_print("M2SetupRequestIEs error: M2AP ASN extensions not currently supported\n");
      return LIBLTE_ERROR_DECODE_FAIL;
    }

    // No. of ProtocolIE-Container
    n_ie = liblte_bits_2_value(ptr, 16);
    if (n_ie == 0) {
      msg->CriticalityDiagnostics_present = false;
    } else {
      msg->CriticalityDiagnostics_present = true;
    }
    // Unpack ProtocolIE Fields
    for (i = 0; i < n_ie; i++) {
      if (liblte_m2ap_unpack_protocolie_header(ptr, &ie_id, &crit, &len) != LIBLTE_SUCCESS) {
        return LIBLTE_ERROR_DECODE_FAIL;
      }

      if (LIBLTE_M2AP_IE_ID_CRITICALITYDIAGNOSTICS == ie_id) {
        if (liblte_m2ap_unpack_criticalitydiagnostics(ptr, &msg->CriticalityDiagnostics) != LIBLTE_SUCCESS) {
          return LIBLTE_ERROR_DECODE_FAIL;
        }
        liblte_align_up(ptr, 8);
      } else {
        return LIBLTE_ERROR_DECODE_FAIL;
      }
    }

    err = LIBLTE_SUCCESS;
  }
  return err;
}
/*******************************************************************************
 * InitiatingMessage CHOICE
 *******************************************************************************/
LIBLTE_ERROR_ENUM liblte_m2ap_pack_initiatingmessage(LIBLTE_M2AP_INITIATINGMESSAGE_STRUCT* msg, uint8_t** ptr)
{
  LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

  if (msg != NULL && ptr != NULL) {
    LIBLTE_BIT_MSG_STRUCT tmp_msg;
    uint8_t*              tmp_ptr = tmp_msg.msg;

    // Message
    if (msg->choice_type == LIBLTE_M2AP_INITIATINGMESSAGE_CHOICE_M2SETUPREQUEST) {
      if (liblte_m2ap_pack_m2setuprequest(&msg->choice.M2SetupRequest, &tmp_ptr) != LIBLTE_SUCCESS) {
        return LIBLTE_ERROR_ENCODE_FAIL;
      }
    } else if (msg->choice_type == LIBLTE_M2AP_INITIATINGMESSAGE_CHOICE_MBMSSESSIONSTARTREQUEST) {
      if (liblte_m2ap_pack_mbmssessionstartrequest(&msg->choice.MbmsSessionStartRequest, &tmp_ptr) != LIBLTE_SUCCESS) {
        return LIBLTE_ERROR_ENCODE_FAIL;
      }
    } else if (msg->choice_type == LIBLTE_M2AP_INITIATINGMESSAGE_CHOICE_MBMSSCHEDULINGINFORMATION) {
      if (liblte_m2ap_pack_mbmsschedulinginformation(&msg->choice.MbmsSchedulingInformation, &tmp_ptr) !=
          LIBLTE_SUCCESS) {
        return LIBLTE_ERROR_ENCODE_FAIL;
      }
    }
    tmp_msg.N_bits = tmp_ptr - tmp_msg.msg;

    // Procedure code
    liblte_value_2_bits(msg->procedureCode, ptr, 8);

    // Criticality
    LIBLTE_M2AP_CRITICALITY_ENUM crit = liblte_m2ap_procedure_criticality(msg->procedureCode);
    liblte_value_2_bits(crit, ptr, 2);
    liblte_align_up_zero(ptr, 8);

    // Length
    uint32_t len = (tmp_msg.N_bits + 7) / 8;
    if (len < 128) {
      liblte_value_2_bits(0, ptr, 1);
      liblte_value_2_bits(len, ptr, 7);
    } else if (len < 16383) {
      liblte_value_2_bits(1, ptr, 1);
      liblte_value_2_bits(0, ptr, 1);
      liblte_value_2_bits(len, ptr, 14);
    } else {
      // TODO: Unlikely to have more than 16K of octets
    }

    memcpy(*ptr, tmp_msg.msg, tmp_msg.N_bits);
    *ptr += tmp_msg.N_bits;

    err = LIBLTE_SUCCESS;
  }
  return err;
}

LIBLTE_ERROR_ENUM liblte_m2ap_unpack_initiatingmessage(uint8_t** ptr, LIBLTE_M2AP_INITIATINGMESSAGE_STRUCT* msg)
{
  LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

  if (msg != NULL && ptr != NULL) {
    // Procedure code
    msg->procedureCode = liblte_bits_2_value(ptr, 8);

    // Criticality
    msg->criticality = (LIBLTE_M2AP_CRITICALITY_ENUM)liblte_bits_2_value(ptr, 2);
    liblte_align_up(ptr, 8);

    // Length
    uint32_t len = 0;
    if (0 == liblte_bits_2_value(ptr, 1)) {
      len = liblte_bits_2_value(ptr, 7);
    } else {
      if (0 == liblte_bits_2_value(ptr, 1)) {
        len = liblte_bits_2_value(ptr, 14);
      } else {
        // TODO: Unlikely to have more than 16K of octets
      }
    }

    // Message
    if (msg->procedureCode == LIBLTE_M2AP_PROC_ID_M2SETUP) {
      msg->choice_type = LIBLTE_M2AP_INITIATINGMESSAGE_CHOICE_M2SETUPREQUEST;
      err              = liblte_m2ap_unpack_m2setuprequest(ptr, &msg->choice.M2SetupRequest);
    } else if (msg->procedureCode == LIBLTE_M2AP_PROC_ID_SESSIONSTART) {
      msg->choice_type = LIBLTE_M2AP_INITIATINGMESSAGE_CHOICE_MBMSSESSIONSTARTREQUEST;
      err              = liblte_m2ap_unpack_mbmssessionstartrequest(ptr, &msg->choice.MbmsSessionStartRequest);
    } else if (msg->procedureCode == LIBLTE_M2AP_PROC_ID_MBMSSCHEDULINGINFORMATION) {
      msg->choice_type = LIBLTE_M2AP_INITIATINGMESSAGE_CHOICE_MBMSSCHEDULINGINFORMATION;
      err              = liblte_m2ap_unpack_mbmsschedulinginformation(ptr, &msg->choice.MbmsSchedulingInformation);
    } else {
      err = LIBLTE_ERROR_DECODE_FAIL;
    }
  }
  return err;
}

/*******************************************************************************
 * SuccessfulOutcome CHOICE
 *******************************************************************************/
LIBLTE_ERROR_ENUM liblte_m2ap_pack_successfuloutcome(LIBLTE_M2AP_SUCCESSFULOUTCOME_STRUCT* msg, uint8_t** ptr)
{
  LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

  if (msg != NULL && ptr != NULL) {
    LIBLTE_BIT_MSG_STRUCT tmp_msg;
    uint8_t*              tmp_ptr = tmp_msg.msg;

    // Message
    if (msg->choice_type == LIBLTE_M2AP_SUCCESSFULOUTCOME_CHOICE_M2SETUPRESPONSE) {
      if (liblte_m2ap_pack_m2setupresponse(&msg->choice.M2SetupResponse, &tmp_ptr) != LIBLTE_SUCCESS) {
        return LIBLTE_ERROR_ENCODE_FAIL;
      }
    } else if (msg->choice_type == LIBLTE_M2AP_SUCCESSFULOUTCOME_CHOICE_MBMSSESSIONSTARTRESPONSE) {
      if (liblte_m2ap_pack_mbmssessionstartresponse(&msg->choice.MbmsSessionStartResponse, &tmp_ptr) !=
          LIBLTE_SUCCESS) {
        return LIBLTE_ERROR_ENCODE_FAIL;
      }
    } else if (msg->choice_type == LIBLTE_M2AP_SUCCESSFULOUTCOME_CHOICE_MBMSSCHEDULINGINFORMATIONRESPONSE) {
      if (liblte_m2ap_pack_mbmsschedulinginformationresponse(&msg->choice.MbmsSchedulingInformationResponse,
                                                             &tmp_ptr) != LIBLTE_SUCCESS) {
        return LIBLTE_ERROR_ENCODE_FAIL;
      }
    } else {
      return LIBLTE_ERROR_ENCODE_FAIL;
    }
    tmp_msg.N_bits = tmp_ptr - tmp_msg.msg;

    // Procedure code
    liblte_value_2_bits(msg->procedureCode, ptr, 8);

    // Criticality
    LIBLTE_M2AP_CRITICALITY_ENUM crit = liblte_m2ap_procedure_criticality(msg->procedureCode);
    liblte_value_2_bits(crit, ptr, 2);
    liblte_align_up_zero(ptr, 8);

    // Length
    uint32_t len = (tmp_msg.N_bits + 7) / 8;
    if (len < 128) {
      liblte_value_2_bits(0, ptr, 1);
      liblte_value_2_bits(len, ptr, 7);
    } else if (len < 16383) {
      liblte_value_2_bits(1, ptr, 1);
      liblte_value_2_bits(0, ptr, 1);
      liblte_value_2_bits(len, ptr, 14);
    } else {
      // TODO: Unlikely to have more than 16K of octets
    }

    memcpy(*ptr, tmp_msg.msg, tmp_msg.N_bits);
    *ptr += tmp_msg.N_bits;

    err = LIBLTE_SUCCESS;
  }
  return err;
}

LIBLTE_ERROR_ENUM liblte_m2ap_unpack_successfuloutcome(uint8_t** ptr, LIBLTE_M2AP_SUCCESSFULOUTCOME_STRUCT* msg)
{
  LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

  if (msg != NULL && ptr != NULL) {
    // Procedure code
    msg->procedureCode = liblte_bits_2_value(ptr, 8);

    // Criticality
    msg->criticality = (LIBLTE_M2AP_CRITICALITY_ENUM)liblte_bits_2_value(ptr, 2);
    liblte_align_up(ptr, 8);

    // Length
    uint32_t len = 0;
    if (0 == liblte_bits_2_value(ptr, 1)) {
      len = liblte_bits_2_value(ptr, 7);
    } else {
      if (0 == liblte_bits_2_value(ptr, 1)) {
        len = liblte_bits_2_value(ptr, 14);
      } else {
        // TODO: Unlikely to have more than 16K of octets
      }
    }

    // Message
    if (msg->procedureCode == LIBLTE_M2AP_PROC_ID_M2SETUP) {
      msg->choice_type = LIBLTE_M2AP_SUCCESSFULOUTCOME_CHOICE_M2SETUPRESPONSE;
      if (liblte_m2ap_unpack_m2setupresponse(ptr, &msg->choice.M2SetupResponse) != LIBLTE_SUCCESS) {
        return LIBLTE_ERROR_DECODE_FAIL;
      }
      err = LIBLTE_SUCCESS;
    } else if (msg->procedureCode == LIBLTE_M2AP_PROC_ID_SESSIONSTART) {
      msg->choice_type = LIBLTE_M2AP_SUCCESSFULOUTCOME_CHOICE_MBMSSESSIONSTARTRESPONSE;
      if (liblte_m2ap_unpack_mbmssessionstartresponse(ptr, &msg->choice.MbmsSessionStartResponse) != LIBLTE_SUCCESS) {
        return LIBLTE_ERROR_DECODE_FAIL;
      }
      err = LIBLTE_SUCCESS;
    } else if (msg->procedureCode == LIBLTE_M2AP_PROC_ID_MBMSSCHEDULINGINFORMATION) {
      msg->choice_type = LIBLTE_M2AP_SUCCESSFULOUTCOME_CHOICE_MBMSSCHEDULINGINFORMATIONRESPONSE;
      if (liblte_m2ap_unpack_mbmsschedulinginformationresponse(ptr, &msg->choice.MbmsSchedulingInformationResponse) !=
          LIBLTE_SUCCESS) {
        return LIBLTE_ERROR_DECODE_FAIL;
      }
      err = LIBLTE_SUCCESS;
    } else {
      return LIBLTE_ERROR_DECODE_FAIL;
    }
  }
  return err;
}

/*******************************************************************************
 * M2AP_PDU CHOICE
 *******************************************************************************/
LIBLTE_ERROR_ENUM liblte_m2ap_pack_m2ap_pdu(LIBLTE_M2AP_M2AP_PDU_STRUCT* m2ap_pdu, LIBLTE_BYTE_MSG_STRUCT* msg)
{
  LIBLTE_ERROR_ENUM     err = LIBLTE_ERROR_INVALID_INPUTS;
  LIBLTE_BIT_MSG_STRUCT bit_msg;

  if (m2ap_pdu != NULL && msg != NULL) {
    uint8_t*  p   = bit_msg.msg;
    uint8_t** ptr = &p;

    // Extension
    liblte_value_2_bits(m2ap_pdu->ext ? 1 : 0, ptr, 1);

    // Message choice
    liblte_value_2_bits(m2ap_pdu->choice_type, ptr, 2);
    liblte_align_up_zero(ptr, 8);

    // Message
    if (LIBLTE_M2AP_M2AP_PDU_CHOICE_INITIATINGMESSAGE == m2ap_pdu->choice_type) {
      if (liblte_m2ap_pack_initiatingmessage(&m2ap_pdu->choice.initiatingMessage, ptr) != LIBLTE_SUCCESS) {
        return LIBLTE_ERROR_ENCODE_FAIL;
      }
    } else if (LIBLTE_M2AP_M2AP_PDU_CHOICE_SUCCESSFULOUTCOME == m2ap_pdu->choice_type) {
      if (liblte_m2ap_pack_successfuloutcome(&m2ap_pdu->choice.successfulOutcome, ptr) != LIBLTE_SUCCESS) {
        return LIBLTE_ERROR_ENCODE_FAIL;
      }
    } /*else if(LIBLTE_M2AP_M2AP_PDU_CHOICE_UNSUCCESSFULOUTCOME == m2ap_pdu->choice_type) {
      if(liblte_m2ap_pack_unsuccessfuloutcome(&m2ap_pdu->choice.unsuccessfulOutcome, ptr) != LIBLTE_SUCCESS) {
        return LIBLTE_ERROR_ENCODE_FAIL;
      }
    }*/

    liblte_align_up_zero(ptr, 8);
    bit_msg.N_bits = (*ptr - bit_msg.msg);

    liblte_pack(&bit_msg, msg);
    err = LIBLTE_SUCCESS;
  }
  return err;
}

LIBLTE_ERROR_ENUM liblte_m2ap_unpack_m2ap_pdu(LIBLTE_BYTE_MSG_STRUCT* msg, LIBLTE_M2AP_M2AP_PDU_STRUCT* m2ap_pdu)
{
  LIBLTE_ERROR_ENUM     err = LIBLTE_ERROR_INVALID_INPUTS;
  LIBLTE_BIT_MSG_STRUCT bit_msg;

  if (m2ap_pdu != NULL && msg != NULL) {
    liblte_unpack(msg, &bit_msg);

    uint8_t*  p   = bit_msg.msg;
    uint8_t** ptr = &p;

    // Extension
    m2ap_pdu->ext = liblte_bits_2_value(ptr, 1);

    // Message choice
    m2ap_pdu->choice_type = (LIBLTE_M2AP_M2AP_PDU_CHOICE_ENUM)liblte_bits_2_value(ptr, 2);
    liblte_align_up(ptr, 8);

    // Message
    if (LIBLTE_M2AP_M2AP_PDU_CHOICE_INITIATINGMESSAGE == m2ap_pdu->choice_type) {
      if (liblte_m2ap_unpack_initiatingmessage(ptr, &m2ap_pdu->choice.initiatingMessage) != LIBLTE_SUCCESS) {
        return LIBLTE_ERROR_DECODE_FAIL;
      }
    } else if (LIBLTE_M2AP_M2AP_PDU_CHOICE_SUCCESSFULOUTCOME == m2ap_pdu->choice_type) {
      if (liblte_m2ap_unpack_successfuloutcome(ptr, &m2ap_pdu->choice.successfulOutcome) != LIBLTE_SUCCESS) {
        return LIBLTE_ERROR_DECODE_FAIL;
      }
    } /*else if(LIBLTE_M2AP_M2AP_PDU_CHOICE_UNSUCCESSFULOUTCOME == m2ap_pdu->choice_type) {
      if(liblte_m2ap_unpack_unsuccessfuloutcome(ptr, &m2ap_pdu->choice.unsuccessfulOutcome) != LIBLTE_SUCCESS) {
        return LIBLTE_ERROR_DECODE_FAIL;
      }
    }*/
    err = LIBLTE_SUCCESS;
  }
  return err;
}
