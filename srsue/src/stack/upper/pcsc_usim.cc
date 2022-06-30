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

#include <sstream>

#include "srsran/common/bcd_helpers.h"
#include "srsran/common/standard_streams.h"
#include "srsue/hdr/stack/upper/pcsc_usim.h"
#include "string.h"

#define CHECK_SIM_PIN 0

using namespace srsran;

namespace srsue {

pcsc_usim::pcsc_usim(srslog::basic_logger& logger) : usim_base(logger), sc(logger) {}

pcsc_usim::~pcsc_usim()
{
  if (initiated) {
    sc.deinit();
    initiated = false;
  }
}

int pcsc_usim::init(usim_args_t* args)
{
  int ret = SRSRAN_ERROR;

  if (sc.init(args) != SRSRAN_SUCCESS) {
    return ret;
  }

  // Read IMSI from SIM card
  char   tmp[15];
  size_t tmp_len = 15; // set to max IMSI length
  if (sc.get_imsi(tmp, &tmp_len)) {
    logger.error("Error reading IMSI from SIM.");
    return ret;
  }
  imsi_str.assign(tmp, tmp_len);

  // Check extracted IMSI and convert
  if (15 == imsi_str.length()) {
    const char* imsi_c = imsi_str.c_str();
    imsi               = 0;
    for (int i = 0; i < 15; i++) {
      imsi *= 10;
      imsi += imsi_c[i] - '0';
    }
  } else {
    logger.error("Invalid length for IMSI: %zu should be %d", imsi_str.length(), 15);
    srsran::console("Invalid length for IMSI: %zu should be %d\n", imsi_str.length(), 15);
    return ret;
  }

  // Check IMEI
  if (15 == args->imei.length()) {
    const char* imei_c = args->imei.c_str();
    imei               = 0;
    for (int i = 0; i < 15; i++) {
      imei *= 10;
      imei += imei_c[i] - '0';
    }
  } else {
    logger.error("Invalid length for IMEI: %zu should be %d", args->imei.length(), 15);
    srsran::console("Invalid length for IMEI: %zu should be %d\n", args->imei.length(), 15);
    return ret;
  }

  initiated = true;
  ret       = SRSRAN_SUCCESS;

  return ret;
}

void pcsc_usim::stop() {}

/*******************************************************************************
  NAS interface
*******************************************************************************/

auth_result_t pcsc_usim::generate_authentication_response(uint8_t* rand,
                                                          uint8_t* autn_enb,
                                                          uint16_t mcc,
                                                          uint16_t mnc,
                                                          uint8_t* res,
                                                          int*     res_len,
                                                          uint8_t* k_asme)
{
  auth_result_t ret = AUTH_FAILED;
  if (!initiated) {
    ERROR("USIM not initiated!");
    return ret;
  }

  // Use RAND and AUTN to compute RES, CK, IK using SIM card
  switch (sc.umts_auth(rand, autn_enb, res, res_len, ik, ck, auts)) {
    case 0:
      logger.info("SCARD: USIM authentication successful.");
      break;
    case -1:
      logger.error("SCARD: Failure during USIM UMTS authentication");
      return ret;
    case -2:
      logger.info("SCARD: USIM synchronization failure, AUTS generated");
      logger.debug(auts, AKA_AUTS_LEN, "AUTS");
      memcpy(res, auts, AKA_AUTS_LEN);
      *res_len = AKA_AUTS_LEN;
      return AUTH_SYNCH_FAILURE;
    default:
      logger.warning("SCARD: Unknown USIM failure.");
      return ret;
  }

  // TODO: Extract ak and seq from auts
  memset(ak, 0x00, AK_LEN);

  // Extract sqn from autn
  uint8_t sqn[SQN_LEN];
  for (int i = 0; i < 6; i++) {
    sqn[i] = autn_enb[i] ^ ak[i];
  }

  // Generate K_asme
  logger.debug(ck, CK_LEN, "CK:");
  logger.debug(ik, IK_LEN, "IK:");
  logger.debug(ak, AK_LEN, "AK:");
  logger.debug(sqn, SQN_LEN, "SQN:");
  logger.debug("mcc=%d, mnc=%d", mcc, mnc);
  security_generate_k_asme(ck, ik, sqn, mcc, mnc, k_asme);
  logger.info(k_asme, KEY_LEN, "K_ASME:");

  ret = AUTH_OK;

  return ret;
}

auth_result_t pcsc_usim::generate_authentication_response_5g(uint8_t*    rand,
                                                             uint8_t*    autn_enb,
                                                             const char* serving_network_name,
                                                             uint8_t*    abba,
                                                             uint32_t    abba_len,
                                                             uint8_t*    res_star,
                                                             uint8_t*    k_amf)
{
  uint8_t res[16];
  uint8_t k_ausf[32];
  uint8_t k_seaf[32];
  int     res_len;

  auth_result_t ret = AUTH_FAILED;
  if (!initiated) {
    ERROR("USIM not initiated!");
    return ret;
  }

  // Use RAND and AUTN to compute RES, CK, IK using SIM card
  switch (sc.umts_auth(rand, autn_enb, res, &res_len, ik, ck, auts)) {
    case 0:
      logger.info("SCARD: USIM authentication successful.");
      break;
    case -1:
      logger.error("SCARD: Failure during USIM UMTS authentication");
      return ret;
    case -2:
      logger.info("SCARD: USIM synchronization failure, AUTS generated");
      logger.debug(auts, AKA_AUTS_LEN, "AUTS");
      memcpy(res_star, auts, AKA_AUTS_LEN);
      res_len = AKA_AUTS_LEN;
      return AUTH_SYNCH_FAILURE;
    default:
      logger.warning("SCARD: Unknown USIM failure.");
      return ret;
  }

  // TODO: Extract ak and seq from auts
  memset(ak, 0x00, AK_LEN);

  // Extract sqn from autn
  uint8_t sqn[SQN_LEN];
  for (int i = 0; i < 6; i++) {
    sqn[i] = autn_enb[i] ^ ak[i];
  }

  // Generate K_asme
  logger.debug(ck, CK_LEN, "CK:");
  logger.debug(ik, IK_LEN, "IK:");
  logger.debug(ak, AK_LEN, "AK:");
  logger.debug(sqn, SQN_LEN, "SQN:");
  logger.debug("SSN=%s", serving_network_name);
  // Generate RES STAR
  security_generate_res_star(ck, ik, serving_network_name, rand, res, res_len, res_star);
  logger.debug(res_star, 16, "RES STAR");
  // Generate K_ausf
  security_generate_k_ausf(ck, ik, sqn, serving_network_name, k_ausf);
  logger.debug(k_ausf, 32, "K AUSF");
  // Generate K_seaf
  security_generate_k_seaf(k_ausf, serving_network_name, k_seaf);
  logger.debug(k_seaf, 32, "K SEAF");
  // Generate K_amf
  security_generate_k_amf(k_ausf, imsi_str.c_str(), abba, abba_len, k_amf);
  logger.debug(k_amf, 32, "K AMF");

  ret = AUTH_OK;
  return ret;
}

std::string pcsc_usim::get_mnc_str(const uint8_t* imsi_vec, std::string mcc_str)
{
  uint32_t           mcc_len = 3;
  uint32_t           mnc_len = sc.get_mnc_len();
  std::ostringstream mnc_oss;

  for (uint32_t i = mcc_len; i < mcc_len + mnc_len; i++) {
    mnc_oss << (int)imsi_vec[i];
  }

  return mnc_oss.str();
}

/*********************************
 * PC/SC class
 ********************************/

// return 0 if initialization was successfull, -1 otherwies
int pcsc_usim::scard::init(usim_args_t* args)
{
  int  ret_value    = SRSRAN_ERROR;
  uint pos          = 0; // SC reader
  bool reader_found = false;
  // int transaction = 1;
  size_t blen;

  long ret;
  ret = SCardEstablishContext(SCARD_SCOPE_SYSTEM, NULL, NULL, &scard_context);
  if (ret != SCARD_S_SUCCESS) {
    logger.error("SCardEstablishContext(): %s", pcsc_stringify_error(ret));
    return ret_value;
  }

  unsigned long len = 0;
  ret               = SCardListReaders(scard_context, NULL, NULL, &len);
  if (ret != SCARD_S_SUCCESS) {
    logger.error("SCardListReaders(): %s", pcsc_stringify_error(ret));
    return ret_value;
  }

  char* readers = (char*)malloc(len);
  if (readers == NULL) {
    logger.error("Malloc failed");
    return ret_value;
  }

  ret = SCardListReaders(scard_context, NULL, readers, &len);
  if (ret != SCARD_S_SUCCESS) {
    logger.error("SCardListReaders() 2: %s", pcsc_stringify_error(ret));
    goto clean_exit;
  }
  if (len < 3) {
    logger.info("No smart card readers available.");
    return ret_value;
  }

  /* readers: NULL-separated list of reader names, and terminating NULL */
  pos = 0;
  while (pos < len - 1) {
    logger.info("Available Card Reader: %s", &readers[pos]);
    while (readers[pos] != '\0' && pos < len) {
      pos++;
    }
    pos++; // skip separator
  }

  reader_found = false;
  pos          = 0;

  // If no reader specified, test all available readers for SIM cards. Otherwise consider specified reader only.
  if (args->reader.length() == 0) {
    while (pos < len && !reader_found) {
      logger.info("Trying Card Reader: %s", &readers[pos]);
      // Connect to card
      ret = SCardConnect(scard_context,
                         &readers[pos],
                         SCARD_SHARE_SHARED,
                         SCARD_PROTOCOL_T0 | SCARD_PROTOCOL_T1,
                         &scard_handle,
                         &scard_protocol);
      if (ret == SCARD_S_SUCCESS) {
        reader_found = true;
      } else {
        if (ret == (long)SCARD_E_NO_SMARTCARD) {
          logger.error("No smart card inserted.");
        } else {
          logger.error("%s", pcsc_stringify_error(ret));
        }
        logger.info("Failed to use Card Reader: %s", &readers[pos]);

        // proceed to next reader
        while (pos < len && readers[pos] != '\0') {
          pos++;
        }
        pos++; // skip separator
      }
    }
  } else {
    // card reader specified in config. search for this reader.
    while (pos < len && !reader_found) {
      if (strcmp(&readers[pos], args->reader.c_str()) == 0) {
        reader_found = true;
        logger.info("Card Reader found: %s", args->reader.c_str());
      } else {
        // next reader
        while (pos < len && readers[pos] != '\0') {
          pos++;
        }
        pos++; // skip separator
      }
    }
    if (!reader_found) {
      logger.error("Cannot find reader: %s", args->reader.c_str());
    } else {
      ret = SCardConnect(scard_context,
                         &readers[pos],
                         SCARD_SHARE_SHARED,
                         SCARD_PROTOCOL_T0 | SCARD_PROTOCOL_T1,
                         &scard_handle,
                         &scard_protocol);
      if (ret == SCARD_S_SUCCESS) {
        // successfully connected to card
      } else {
        if (ret == (long)SCARD_E_NO_SMARTCARD) {
          logger.error("No smart card inserted.");
        } else {
          logger.error("%s", pcsc_stringify_error(ret));
        }

        logger.info("Failed to use Card Reader: %s", args->reader.c_str());
      }
    }
  }

  free(readers);
  readers = NULL;

  logger.info("Card=0x%x active_protocol=%lu (%s)",
              (unsigned int)scard_handle,
              (unsigned long)scard_protocol,
              scard_protocol == SCARD_PROTOCOL_T0 ? "T0" : "T1");

  ret = SCardBeginTransaction(scard_handle);
  if (ret != SCARD_S_SUCCESS) {
    logger.error("%s", pcsc_stringify_error(ret));
    goto clean_exit;
  }

  // Verify USIM support
  unsigned char buf[100];
  blen = sizeof(buf);
  if (_select_file(SCARD_FILE_MF, buf, &blen, SCARD_USIM, NULL, 0)) {
    logger.info("USIM is not supported. Trying to use GSM SIM");
    sim_type = SCARD_GSM_SIM;
  } else {
    logger.info("USIM is supported");
    sim_type = SCARD_USIM;
  }

  if (sim_type == SCARD_GSM_SIM) {
    blen = sizeof(buf);
    if (select_file(SCARD_FILE_MF, buf, &blen)) {
      logger.debug("SCARD: Failed to read MF");
      goto clean_exit;
    }

    blen = sizeof(buf);
    if (select_file(SCARD_FILE_GSM_DF, buf, &blen)) {
      logger.debug("SCARD: Failed to read GSM DF");
      goto clean_exit;
    }
  } else {
    unsigned char aid[32];
    int           aid_len;

    aid_len = get_aid(aid, sizeof(aid));
    if (aid_len < 0) {
      logger.debug("SCARD: Failed to find AID for 3G USIM app - try to use standard 3G RID");
      memcpy(aid, "\xa0\x00\x00\x00\x87", 5);
      aid_len = 5;
    }

    logger.debug(aid, aid_len, "SCARD: 3G USIM AID");

    /* Select based on AID = 3G RID from EF_DIR. This is usually
     * starting with A0 00 00 00 87. */
    blen = sizeof(buf);
    if (_select_file(0, buf, &blen, sim_type, aid, aid_len)) {
      logger.error("SCARD: Failed to read 3G USIM app");
      logger.error(aid, aid_len, "SCARD: 3G USIM AID");
      goto clean_exit;
    }
  }

#if CHECK_SIM_PIN
  // Verify whether CHV1 (PIN1) is needed to access the card.
  ret = pin_needed(buf, blen);
  if (ret < 0) {
    logger.debug("SCARD: Failed to determine whether PIN is needed");
    goto clean_exit;
  }
  if (ret) {
    logger.debug("PIN1 needed for SIM access (retry counter=%d)", get_pin_retry_counter());
    pin1_needed = true;
  } else {
    pin1_needed = false;
  }

  // stop before pin retry counter reaches zero
  if (pin1_needed && get_pin_retry_counter() <= 1) {
    logger.error("PIN1 needed for SIM access (retry counter=%d), emergency stop.", get_pin_retry_counter());
    goto clean_exit;
  }

  // Set pin
  if (pin1_needed) {
    // verify PIN
    ret = verify_pin(args->pin.c_str());
    if (ret != SCARD_S_SUCCESS) {
      logger.debug("SCARD: Could not verify PIN");
      goto clean_exit;
    }
  }
#else
  pin1_needed = false;
#endif

  ret = SCardEndTransaction(scard_handle, SCARD_LEAVE_CARD);
  if (ret != SCARD_S_SUCCESS) {
    logger.debug("SCARD: Could not end transaction: 0x%x", (unsigned int)ret);
    goto clean_exit;
  }

  ret_value = SRSRAN_SUCCESS;

clean_exit:
  if (readers) {
    free(readers);
  }

  return ret_value;
}

int pcsc_usim::scard::_select_file(unsigned short file_id,
                                   unsigned char* buf,
                                   size_t*        buf_len,
                                   sim_types_t    sim_type,
                                   unsigned char* aid,
                                   size_t         aidlen)
{
  long          ret;
  unsigned char resp[3];
  unsigned char cmd[50] = {SIM_CMD_SELECT};
  int           cmdlen;
  unsigned char get_resp[5] = {SIM_CMD_GET_RESPONSE};
  size_t        len, rlen;

  if (sim_type == SCARD_USIM) {
    cmd[0]      = USIM_CLA;
    cmd[3]      = 0x04;
    get_resp[0] = USIM_CLA;
  }

  logger.debug("SCARD: select file %04x", file_id);
  if (aid) {
    logger.debug(aid, aidlen, "SCARD: select file by AID");
    if (5 + aidlen > sizeof(cmd))
      return -1;
    cmd[2] = 0x04;   /* Select by AID */
    cmd[4] = aidlen; /* len */
    memcpy(cmd + 5, aid, aidlen);
    cmdlen = 5 + aidlen;
  } else {
    cmd[5] = file_id >> 8;
    cmd[6] = file_id & 0xff;
    cmdlen = 7;
  }
  len = sizeof(resp);
  ret = transmit(cmd, cmdlen, resp, &len);
  if (ret != SCARD_S_SUCCESS) {
    logger.error("SCARD: SCardTransmit failed %s", pcsc_stringify_error(ret));
    return -1;
  }

  if (len != 2) {
    logger.error("SCARD: unexpected resp len %d (expected 2)", (int)len);
    return -1;
  }

  if (resp[0] == 0x98 && resp[1] == 0x04) {
    /* Security status not satisfied (PIN_WLAN) */
    logger.warning("SCARD: Security status not satisfied.");
    return -1;
  }

  if (resp[0] == 0x6e) {
    logger.debug("SCARD: used CLA not supported.");
    return -1;
  }

  if (resp[0] != 0x6c && resp[0] != 0x9f && resp[0] != 0x61) {
    logger.warning("SCARD: unexpected response 0x%02x (expected 0x61, 0x6c, or 0x9f)", resp[0]);
    return -1;
  }

  /* Normal ending of command; resp[1] bytes available */
  get_resp[4] = resp[1];
  logger.debug("SCARD: trying to get response (%d bytes)", resp[1]);

  rlen = *buf_len;
  ret  = transmit(get_resp, sizeof(get_resp), buf, &rlen);
  if (ret == SCARD_S_SUCCESS) {
    *buf_len = resp[1] < rlen ? resp[1] : rlen;
    return 0;
  }

  logger.warning("SCARD: SCardTransmit err=0x%lx", ret);
  return -1;
}

int pcsc_usim::scard::select_file(unsigned short file_id, unsigned char* buf, size_t* buf_len)
{
  return _select_file(file_id, buf, buf_len, sim_type, NULL, 0);
}

long pcsc_usim::scard::transmit(unsigned char* _send, size_t send_len, unsigned char* _recv, size_t* recv_len)
{
  long          ret;
  unsigned long rlen;

  logger.debug(_send, send_len, "SCARD: scard_transmit: send");
  rlen      = *recv_len;
  ret       = SCardTransmit(scard_handle,
                      scard_protocol == SCARD_PROTOCOL_T1 ? SCARD_PCI_T1 : SCARD_PCI_T0,
                      _send,
                      (unsigned long)send_len,
                      NULL,
                      _recv,
                      &rlen);
  *recv_len = rlen;
  if (ret == SCARD_S_SUCCESS) {
    logger.debug(_recv, rlen, "SCARD: SCardTransmit: recv");
  } else {
    logger.error("SCARD: SCardTransmit failed %s", pcsc_stringify_error(ret));
  }
  return ret;
}

int pcsc_usim::scard::pin_needed(unsigned char* hdr, size_t hlen)
{
  if (sim_type == SCARD_GSM_SIM) {
    if (hlen > SCARD_CHV1_OFFSET && !(hdr[SCARD_CHV1_OFFSET] & SCARD_CHV1_FLAG))
      return 1;
    return 0;
  }

  if (sim_type == SCARD_USIM) {
    int ps_do;
    if (parse_fsp_templ(hdr, hlen, &ps_do, NULL))
      return -1;
    /* TODO: there could be more than one PS_DO entry because of
     * multiple PINs in key reference.. */
    if (ps_do > 0 && (ps_do & 0x80))
      return 1;
    return 0;
  }

  return -1;
}

int pcsc_usim::scard::get_pin_retry_counter()
{
  long          ret;
  unsigned char resp[3];
  unsigned char cmd[5] = {SIM_CMD_VERIFY_CHV1};
  size_t        len;
  uint16_t      val;

  logger.info("SCARD: fetching PIN retry counter");

  if (sim_type == SCARD_USIM)
    cmd[0] = USIM_CLA;
  cmd[4] = 0; /* Empty data */

  len = sizeof(resp);
  ret = transmit(cmd, sizeof(cmd), resp, &len);
  if (ret != SCARD_S_SUCCESS)
    return -2;

  if (len != 2) {
    logger.error("SCARD: failed to fetch PIN retry counter");
    return -1;
  }

  val = to_uint16(resp);
  if (val == 0x63c0 || val == 0x6983) {
    logger.debug("SCARD: PIN has been blocked");
    return 0;
  }

  if (val >= 0x63c0 && val <= 0x63cf)
    return val & 0x000f;

  logger.info("SCARD: Unexpected PIN retry counter response value 0x%x", val);
  return 0;
}

int pcsc_usim::scard::get_aid(unsigned char* aid, size_t maxlen)
{
  int rlen, rec;
  struct efdir {
    unsigned char appl_template_tag; /* 0x61 */
    unsigned char appl_template_len;
    unsigned char appl_id_tag; /* 0x4f */
    unsigned char aid_len;
    unsigned char rid[5];
    unsigned char appl_code[2]; /* 0x1002 for 3G USIM */
  } * efdir;
  unsigned char buf[127], *aid_pos;
  size_t        blen;
  unsigned int  aid_len = 0;

  efdir   = (struct efdir*)buf;
  aid_pos = &buf[4];
  blen    = sizeof(buf);
  if (select_file(SCARD_FILE_EF_DIR, buf, &blen)) {
    logger.debug("SCARD: Failed to read EF_DIR");
    return -1;
  }
  logger.debug(buf, blen, "SCARD: EF_DIR select");

  for (rec = 1; rec < 10; rec++) {
    rlen = get_record_len(rec, SIM_RECORD_MODE_ABSOLUTE);
    if (rlen < 0) {
      logger.debug("SCARD: Failed to get EF_DIR record length");
      return -1;
    }
    blen = sizeof(buf);
    if (rlen > (int)blen) {
      logger.debug("SCARD: Too long EF_DIR record");
      return -1;
    }
    if (read_record(buf, rlen, rec, SIM_RECORD_MODE_ABSOLUTE) < 0) {
      logger.debug("SCARD: Failed to read EF_DIR record %d", rec);
      return -1;
    }
    logger.debug(buf, rlen, "SCARD: EF_DIR record");

    if (efdir->appl_template_tag != 0x61) {
      logger.debug("SCARD: Unexpected application template tag 0x%x", efdir->appl_template_tag);
      continue;
    }

    if (efdir->appl_template_len > rlen - 2) {
      logger.debug("SCARD: Too long application template (len=%d rlen=%d)", efdir->appl_template_len, rlen);
      continue;
    }

    if (efdir->appl_id_tag != 0x4f) {
      logger.debug("SCARD: Unexpected application identifier tag 0x%x", efdir->appl_id_tag);
      continue;
    }

    aid_len = efdir->aid_len;
    if (aid_len < 1 || aid_len > 16) {
      logger.debug("SCARD: Invalid AID length %u", aid_len);
      continue;
    }

    logger.debug(aid_pos, aid_len, "SCARD: AID from EF_DIR record");

    if (efdir->appl_code[0] == 0x10 && efdir->appl_code[1] == 0x02) {
      logger.debug("SCARD: 3G USIM app found from EF_DIR record %d", rec);
      break;
    }
  }

  if (rec >= 10) {
    logger.debug("SCARD: 3G USIM app not found from EF_DIR records");
    return -1;
  }

  if (aid_len > maxlen) {
    logger.debug("SCARD: Too long AID");
    return -1;
  }

  memcpy(aid, aid_pos, aid_len);

  return aid_len;
}

int pcsc_usim::scard::get_record_len(unsigned char recnum, unsigned char mode)
{
  unsigned char buf[255];
  unsigned char cmd[5] = {SIM_CMD_READ_RECORD /* , len */};
  size_t        blen;
  long          ret;

  if (sim_type == SCARD_USIM)
    cmd[0] = USIM_CLA;
  cmd[2] = recnum;
  cmd[3] = mode;
  cmd[4] = sizeof(buf);

  blen = sizeof(buf);
  ret  = transmit(cmd, sizeof(cmd), buf, &blen);
  if (ret != SCARD_S_SUCCESS) {
    logger.debug("SCARD: failed to determine file length for record %d", recnum);
    return -1;
  }

  logger.debug(buf, blen, "SCARD: file length determination response");

  if (blen < 2 || (buf[0] != 0x6c && buf[0] != 0x67)) {
    logger.error("SCARD: unexpected response to file length determination");
    return -1;
  }

  return buf[1];
}

int pcsc_usim::scard::read_record(unsigned char* data, size_t len, unsigned char recnum, unsigned char mode)
{
  unsigned char  cmd[5] = {SIM_CMD_READ_RECORD /* , len */};
  size_t         blen   = len + 3;
  unsigned char* buf;
  long           ret;

  if (sim_type == SCARD_USIM)
    cmd[0] = USIM_CLA;
  cmd[2] = recnum;
  cmd[3] = mode;
  cmd[4] = len;

  buf = (unsigned char*)malloc(blen);
  if (buf == NULL)
    return -1;

  ret = transmit(cmd, sizeof(cmd), buf, &blen);
  if (ret != SCARD_S_SUCCESS) {
    free(buf);
    return -2;
  }
  if (blen != len + 2) {
    logger.debug("SCARD: record read returned unexpected length %ld (expected %ld)", (long)blen, (long)len + 2);
    free(buf);
    return -3;
  }

  if (buf[len] != 0x90 || buf[len + 1] != 0x00) {
    logger.debug("SCARD: record read returned unexpected status %02x %02x (expected 90 00)", buf[len], buf[len + 1]);
    free(buf);
    return -4;
  }

  memcpy(data, buf, len);
  free(buf);

  return 0;
}

/**
 * scard_get_imsi - Read IMSI from SIM/USIM card
 * @scard: Pointer to private data from scard_init()
 * @imsi: Buffer for IMSI
 * @len: Length of imsi buffer; set to IMSI length on success
 * Returns: 0 on success, -1 if IMSI file cannot be selected, -2 if IMSI file
 * selection returns invalid result code, -3 if parsing FSP template file fails
 * (USIM only), -4 if IMSI does not fit in the provided imsi buffer (len is set
 * to needed length), -5 if reading IMSI file fails.
 *
 * This function can be used to read IMSI from the SIM/USIM card. If the IMSI
 * file is PIN protected, scard_set_pin() must have been used to set the
 * correct PIN code before calling scard_get_imsi().
 */
int pcsc_usim::scard::get_imsi(char* imsi, size_t* len)
{
  unsigned char buf[100];
  size_t        blen, imsilen, i;
  char*         pos;

  logger.debug("SCARD: reading IMSI from (GSM) EF-IMSI");
  blen = sizeof(buf);
  if (select_file(SCARD_FILE_GSM_EF_IMSI, buf, &blen))
    return -1;
  if (blen < 4) {
    logger.warning("SCARD: too short (GSM) EF-IMSI header (len=%ld)", (long)blen);
    return -2;
  }

  if (sim_type == SCARD_GSM_SIM) {
    blen = to_uint16(&buf[2]);
  } else {
    int file_size;
    if (parse_fsp_templ(buf, blen, NULL, &file_size))
      return -3;
    blen = file_size;
  }
  if (blen < 2 || blen > sizeof(buf)) {
    logger.debug("SCARD: invalid IMSI file length=%ld", (long)blen);
    return -3;
  }

  imsilen = (blen - 2) * 2 + 1;
  logger.debug("SCARD: IMSI file length=%ld imsilen=%ld", (long)blen, (long)imsilen);
  if (imsilen > *len) {
    *len = imsilen;
    return -4;
  }

  if (read_file(buf, blen))
    return -5;

  pos    = imsi;
  *pos++ = '0' + (buf[1] >> 4 & 0x0f);
  for (i = 2; i < blen; i++) {
    unsigned char digit;

    digit = buf[i] & 0x0f;
    if (digit < 10)
      *pos++ = '0' + digit;
    else
      imsilen--;

    digit = buf[i] >> 4 & 0x0f;
    if (digit < 10)
      *pos++ = '0' + digit;
    else
      imsilen--;
  }
  *len = imsilen;

  return 0;
}

int pcsc_usim::scard::read_file(unsigned char* data, size_t len)
{
  unsigned char  cmd[5] = {SIM_CMD_READ_BIN /* , len */};
  size_t         blen   = len + 3;
  unsigned char* buf;
  long           ret;

  cmd[4] = len;

  buf = (unsigned char*)malloc(blen);
  if (buf == NULL)
    return -1;

  if (sim_type == SCARD_USIM)
    cmd[0] = USIM_CLA;
  ret = transmit(cmd, sizeof(cmd), buf, &blen);
  if (ret != SCARD_S_SUCCESS) {
    free(buf);
    return -2;
  }
  if (blen != len + 2) {
    logger.error("SCARD: file read returned unexpected length %ld (expected %ld)", (long)blen, (long)len + 2);
    free(buf);
    return -3;
  }

  if (buf[len] != 0x90 || buf[len + 1] != 0x00) {
    logger.error("SCARD: file read returned unexpected status %02x %02x (expected 90 00)", buf[len], buf[len + 1]);
    free(buf);
    return -4;
  }

  memcpy(data, buf, len);
  free(buf);

  return 0;
}

int pcsc_usim::scard::parse_fsp_templ(unsigned char* buf, size_t buf_len, int* ps_do, int* file_len)
{
  unsigned char *pos, *end;

  if (ps_do)
    *ps_do = -1;
  if (file_len)
    *file_len = -1;

  pos = buf;
  end = pos + buf_len;
  if (*pos != USIM_FSP_TEMPL_TAG) {
    logger.error("SCARD: file header did not start with FSP template tag");
    return -1;
  }
  pos++;
  if (pos >= end)
    return -1;
  if (pos[0] < end - pos)
    end = pos + 1 + pos[0];
  pos++;
  logger.debug(pos, end - pos, "SCARD: file header FSP template");

  while (end - pos >= 2) {
    unsigned char type, len;

    type = pos[0];
    len  = pos[1];
    logger.debug("SCARD: file header TLV 0x%02x len=%d", type, len);
    pos += 2;

    if (len > (unsigned int)(end - pos))
      break;

    switch (type) {
      case USIM_TLV_FILE_DESC:
        logger.debug(pos, len, "SCARD: File Descriptor TLV");
        break;
      case USIM_TLV_FILE_ID:
        logger.debug(pos, len, "SCARD: File Identifier TLV");
        break;
      case USIM_TLV_DF_NAME:
        logger.debug(pos, len, "SCARD: DF name (AID) TLV");
        break;
      case USIM_TLV_PROPR_INFO:
        logger.debug(pos, len, "SCARD: Proprietary information TLV");
        break;
      case USIM_TLV_LIFE_CYCLE_STATUS:
        logger.debug(pos, len, "SCARD: Life Cycle Status Integer TLV");
        break;
      case USIM_TLV_FILE_SIZE:
        logger.debug(pos, len, "SCARD: File size TLV");
        if ((len == 1 || len == 2) && file_len) {
          if (len == 1) {
            *file_len = (int)pos[0];
          } else {
            *file_len = to_uint16(pos);
          }
          logger.debug("SCARD: file_size=%d", *file_len);
        }
        break;
      case USIM_TLV_TOTAL_FILE_SIZE:
        logger.debug(pos, len, "SCARD: Total file size TLV");
        break;
      case USIM_TLV_PIN_STATUS_TEMPLATE:
        logger.debug(pos, len, "SCARD: PIN Status Template DO TLV");
        if (len >= 2 && pos[0] == USIM_PS_DO_TAG && pos[1] >= 1 && ps_do) {
          logger.debug("SCARD: PS_DO=0x%02x", pos[2]);
          *ps_do = (int)pos[2];
        }
        break;
      case USIM_TLV_SHORT_FILE_ID:
        logger.debug(pos, len, "SCARD: Short File Identifier (SFI) TLV");
        break;
      case USIM_TLV_SECURITY_ATTR_8B:
      case USIM_TLV_SECURITY_ATTR_8C:
      case USIM_TLV_SECURITY_ATTR_AB:
        logger.debug(pos, len, "SCARD: Security attribute TLV");
        break;
      default:
        logger.debug(pos, len, "SCARD: Unrecognized TLV");
        break;
    }

    pos += len;

    if (pos == end)
      return 0;
  }
  return -1;
}

/**
 * scard_deinit - Deinitialize SIM/USIM connection
 * @scard: Pointer to private data from scard_init()
 *
 * This function closes the SIM/USIM connect opened with scard_init().
 */
void pcsc_usim::scard::deinit()
{
  long ret;

  logger.debug("SCARD: deinitializing smart card interface");

  ret = SCardDisconnect(scard_handle, SCARD_UNPOWER_CARD);
  if (ret != SCARD_S_SUCCESS) {
    logger.debug("SCARD: Failed to disconnect smart card (err=%ld)", ret);
  }

  ret = SCardReleaseContext(scard_context);
  if (ret != SCARD_S_SUCCESS) {
    logger.debug("Failed to release smart card context (err=%ld)", ret);
  }
}

/**
 * scard_get_mnc_len - Read length of MNC in the IMSI from SIM/USIM card
 * @scard: Pointer to private data from scard_init()
 * Returns: length (>0) on success, -1 if administrative data file cannot be
 * selected, -2 if administrative data file selection returns invalid result
 * code, -3 if parsing FSP template file fails (USIM only), -4 if length of
 * the file is unexpected, -5 if reading file fails, -6 if MNC length is not
 * in range (i.e. 2 or 3), -7 if MNC length is not available.
 *
 */
int pcsc_usim::scard::get_mnc_len()
{
  unsigned char buf[100];
  size_t        blen;
  int           file_size;

  logger.debug("SCARD: reading MNC len from (GSM) EF-AD");
  blen = sizeof(buf);
  if (select_file(SCARD_FILE_GSM_EF_AD, buf, &blen))
    return -1;
  if (blen < 4) {
    logger.debug("SCARD: too short (GSM) EF-AD header (len=%ld)", (long)blen);
    return -2;
  }

  if (sim_type == SCARD_GSM_SIM) {
    file_size = to_uint16(&buf[2]);
  } else {
    if (parse_fsp_templ(buf, blen, NULL, &file_size))
      return -3;
  }
  if (file_size == 3) {
    logger.debug("SCARD: MNC length not available");
    return -7;
  }
  if (file_size < 4 || file_size > (int)sizeof(buf)) {
    logger.debug("SCARD: invalid file length=%ld", (long)file_size);
    return -4;
  }

  if (read_file(buf, file_size))
    return -5;
  buf[3] = buf[3] & 0x0f; /* upper nibble reserved for future use  */
  if (buf[3] < 2 || buf[3] > 3) {
    logger.debug("SCARD: invalid MNC length=%ld", (long)buf[3]);
    return -6;
  }
  logger.debug("SCARD: MNC length=%ld", (long)buf[3]);
  return buf[3];
}

/**
 * scard_umts_auth - Run UMTS authentication command on USIM card
 * @scard: Pointer to private data from scard_init()
 * @_rand: 16-byte RAND value from HLR/AuC
 * @autn: 16-byte AUTN value from HLR/AuC
 * @res: 16-byte buffer for RES
 * @res_len: Variable that will be set to RES length
 * @ik: 16-byte buffer for IK
 * @ck: 16-byte buffer for CK
 * @auts: 14-byte buffer for AUTS
 * Returns: 0 on success, -1 on failure, or -2 if USIM reports synchronization
 * failure
 *
 * This function performs AKA authentication using USIM card and the provided
 * RAND and AUTN values from HLR/AuC. If authentication command can be
 * completed successfully, RES, IK, and CK values will be written into provided
 * buffers and res_len is set to length of received RES value. If USIM reports
 * synchronization failure, the received AUTS value will be written into auts
 * buffer. In this case, RES, IK, and CK are not valid.
 */
int pcsc_usim::scard::umts_auth(const unsigned char* _rand,
                                const unsigned char* autn,
                                unsigned char*       res,
                                int*                 res_len,
                                unsigned char*       ik,
                                unsigned char*       ck,
                                unsigned char*       auts)
{
  unsigned char cmd[5 + 1 + AKA_RAND_LEN + 1 + AKA_AUTN_LEN] = {USIM_CMD_RUN_UMTS_ALG};
  unsigned char get_resp[5]                                  = {USIM_CMD_GET_RESPONSE};
  unsigned char resp[3], buf[64], *pos, *end;
  size_t        len;
  long          ret;

  if (sim_type == SCARD_GSM_SIM) {
    logger.debug("SCARD: Non-USIM card - cannot do UMTS auth");
    return -1;
  }

  logger.debug(_rand, AKA_RAND_LEN, "SCARD: UMTS auth - RAND");
  logger.debug(autn, AKA_AUTN_LEN, "SCARD: UMTS auth - AUTN");
  cmd[5] = AKA_RAND_LEN;
  memcpy(cmd + 6, _rand, AKA_RAND_LEN);
  cmd[6 + AKA_RAND_LEN] = AKA_AUTN_LEN;
  memcpy(cmd + 6 + AKA_RAND_LEN + 1, autn, AKA_AUTN_LEN);

  len = sizeof(resp);
  ret = transmit(cmd, sizeof(cmd), resp, &len);
  if (ret != SCARD_S_SUCCESS)
    return -1;

  if (len <= sizeof(resp))
    logger.debug(resp, len, "SCARD: UMTS alg response");

  if (len == 2 && resp[0] == 0x98 && resp[1] == 0x62) {
    // Authentication error, application specific
    logger.warning("SCARD: UMTS auth failed - MAC != XMAC");
    return -1;
  }

  if (len != 2 || resp[0] != 0x61) {
    logger.warning(
        "SCARD: unexpected response for UMTS auth request (len=%ld resp=%02x %02x)", (long)len, resp[0], resp[1]);
    return -1;
  }
  get_resp[4] = resp[1];

  len = sizeof(buf);
  ret = transmit(get_resp, sizeof(get_resp), buf, &len);
  if (ret != SCARD_S_SUCCESS || len > sizeof(buf))
    return -1;

  logger.debug(buf, len, "SCARD: UMTS get response result");
  if (len >= 2 + AKA_AUTS_LEN && buf[0] == 0xdc && buf[1] == AKA_AUTS_LEN) {
    logger.debug("SCARD: UMTS Synchronization-Failure");
    memcpy(auts, buf + 2, AKA_AUTS_LEN);
    logger.debug(auts, AKA_AUTS_LEN, "SCARD: AUTS");
    *res_len = AKA_AUTS_LEN;
    return -2;
  }

  if (len >= 6 + IK_LEN + CK_LEN && buf[0] == 0xdb) {
    pos = buf + 1;
    end = buf + len;

    /* RES */
    if (pos[0] > RES_MAX_LEN || pos[0] > end - pos) {
      logger.debug("SCARD: Invalid RES");
      return -1;
    }
    *res_len = *pos++;
    memcpy(res, pos, *res_len);
    pos += *res_len;
    logger.debug(res, *res_len, "SCARD: RES");

    /* CK */
    if (pos[0] != CK_LEN || CK_LEN > end - pos) {
      logger.debug("SCARD: Invalid CK");
      return -1;
    }
    pos++;
    memcpy(ck, pos, CK_LEN);
    pos += CK_LEN;
    logger.debug(ck, CK_LEN, "SCARD: CK");

    /* IK */
    if (pos[0] != IK_LEN || IK_LEN > end - pos) {
      logger.debug("SCARD: Invalid IK");
      return -1;
    }
    pos++;
    memcpy(ik, pos, IK_LEN);
    pos += IK_LEN;
    logger.debug(ik, IK_LEN, "SCARD: IK");

    if (end > pos) {
      logger.debug(pos, end - pos, "SCARD: Ignore extra data in end");
    }

    return 0;
  }

  logger.debug("SCARD: Unrecognized response");
  return -1;
}

int pcsc_usim::scard::verify_pin(const char* pin)
{
  long          ret;
  unsigned char resp[3];
  unsigned char cmd[5 + 8] = {SIM_CMD_VERIFY_CHV1};
  size_t        len;

  logger.debug("SCARD: verifying PIN");

  if (pin == NULL || strlen(pin) > 8)
    return -1;

  if (sim_type == SCARD_USIM)
    cmd[0] = USIM_CLA;
  memcpy(cmd + 5, pin, strlen(pin));
  memset(cmd + 5 + strlen(pin), 0xff, 8 - strlen(pin));

  len = sizeof(resp);
  ret = transmit(cmd, sizeof(cmd), resp, &len);
  if (ret != SCARD_S_SUCCESS)
    return -2;

  if (len != 2 || resp[0] != 0x90 || resp[1] != 0x00) {
    logger.debug("SCARD: PIN verification failed");
    return -1;
  }

  logger.debug("SCARD: PIN verified successfully");
  return SCARD_S_SUCCESS;
}

} // namespace srsue
