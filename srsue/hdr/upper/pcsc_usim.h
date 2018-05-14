/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2015 Software Radio Systems Limited
 *
 * \section LICENSE
 *
 * This file is part of the srsUE library.
 *
 * srsUE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsUE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#ifndef SRSUE_PCSC_USIM_H
#define SRSUE_PCSC_USIM_H

#include <string>
#include "srslte/common/log.h"
#include "srslte/common/common.h"
#include "srslte/interfaces/ue_interfaces.h"
#include "srslte/common/security.h"
#include "srsue/hdr/upper/usim.h"
#include <winscard.h>

namespace srsue {

#define AKA_RAND_LEN 16
#define AKA_AUTN_LEN 16
#define AKA_AUTS_LEN 14
#define RES_MAX_LEN  16
#define MAC_LEN      8
#define IK_LEN       16
#define CK_LEN       16
#define AK_LEN       6
#define SQN_LEN      6

#define KEY_LEN      32

typedef enum {
  SCARD_GSM_SIM,
  SCARD_USIM
} sim_types_t;

static inline uint16_t to_uint16(const uint8_t *a)
{
  return (a[0] << 8) | a[1];
}

class pcsc_usim
    :public usim_base
{
public:
  pcsc_usim();
  ~pcsc_usim();
  int init(usim_args_t *args, srslte::log *usim_log_);
  void stop();

  // NAS interface
  std::string get_imsi_str();
  std::string get_imei_str();

  bool get_imsi_vec(uint8_t* imsi_, uint32_t n);
  bool get_imei_vec(uint8_t* imei_, uint32_t n);
  bool get_home_plmn_id(LIBLTE_RRC_PLMN_IDENTITY_STRUCT *home_plmn_id);

  auth_result_t generate_authentication_response(uint8_t  *rand,
                                                 uint8_t  *autn_enb,
                                                 uint16_t  mcc,
                                                 uint16_t  mnc,
                                                 uint8_t  *res,
                                                 int      *res_len,
                                                 uint8_t  *k_asme);

  void generate_nas_keys(uint8_t *k_asme,
                         uint8_t *k_nas_enc,
                         uint8_t *k_nas_int,
                         srslte::CIPHERING_ALGORITHM_ID_ENUM cipher_algo,
                         srslte::INTEGRITY_ALGORITHM_ID_ENUM integ_algo);

  // RRC interface
  void generate_as_keys(uint8_t *k_asme,
                        uint32_t count_ul,
                        uint8_t *k_rrc_enc,
                        uint8_t *k_rrc_int,
                        uint8_t *k_up_enc,
                        uint8_t *k_up_int,
                        srslte::CIPHERING_ALGORITHM_ID_ENUM cipher_algo,
                        srslte::INTEGRITY_ALGORITHM_ID_ENUM integ_algo);

  void generate_as_keys_ho(uint32_t pci,
                           uint32_t earfcn,
                           int ncc,
                           uint8_t *k_rrc_enc,
                           uint8_t *k_rrc_int,
                           uint8_t *k_up_enc,
                           uint8_t *k_up_int,
                           srslte::CIPHERING_ALGORITHM_ID_ENUM cipher_algo,
                           srslte::INTEGRITY_ALGORITHM_ID_ENUM integ_algo);


private:
  srslte::log *log;

  // User data
  uint8_t     amf[2];  // 3GPP 33.102 v10.0.0 Annex H
  uint8_t     op[16];
  uint64_t    imsi;
  uint64_t    imei;
  uint8_t     k[16];

  std::string imsi_str;
  std::string imei_str;

  uint32_t    mnc_length;

  // Security variables
  uint8_t     rand[AKA_RAND_LEN];
  uint8_t     ck[CK_LEN];
  uint8_t     ik[IK_LEN];
  uint8_t     ak[AK_LEN];
  uint8_t     mac[MAC_LEN];
  uint8_t     autn[AKA_AUTN_LEN];
  uint8_t     k_asme[KEY_LEN];
  uint8_t     nh[KEY_LEN];
  uint8_t     k_enb[KEY_LEN];
  uint8_t     k_enb_star[KEY_LEN];
  uint8_t     auts[AKA_AUTS_LEN];

  uint32_t    current_ncc;

  bool initiated;

  // Smartcard sub-class which is a port of the PC/SC smartcard implementation
  // of WPA Supplicant written by Jouni Malinen <j@w1.fi> and licensed under BSD
  // Source: https://w1.fi/cvs.html
  class scard {
  public:
    scard() : log(NULL) {};
    ~scard() {};

    int init(usim_args_t *args, srslte::log *log_);
    void deinit();

    int select_file(unsigned short file_id,unsigned char *buf, size_t *buf_len);
    int _select_file(unsigned short file_id, unsigned char *buf, size_t *buf_len, sim_types_t sim_type, unsigned char *aid, size_t aidlen);

    long transmit(unsigned char *_send, size_t send_len, unsigned char *_recv, size_t *recv_len);

    int get_aid(unsigned char *aid, size_t maxlen);
    int get_record_len(unsigned char recnum, unsigned char mode);
    int read_record(unsigned char *data, size_t len, unsigned char recnum, unsigned char mode);
    int get_imsi(char *imsi, size_t *len);
    int parse_fsp_templ(unsigned char *buf, size_t buf_len, int *ps_do, int *file_len);
    int read_file(unsigned char *data, size_t len);
    int get_mnc_len();
    int umts_auth(const unsigned char *_rand,
                  const unsigned char *autn,
                  unsigned char *res, int *res_len,
                  unsigned char *ik, unsigned char *ck, unsigned char *auts);
    int pin_needed(unsigned char *hdr, size_t hlen);
    int verify_pin(const char *pin);
    int get_pin_retry_counter();

  private:
    /* See ETSI GSM 11.11 and ETSI TS 102 221 for details.
     * SIM commands:
     * Command APDU: CLA INS P1 P2 P3 Data
     *   CLA (class of instruction): A0 for GSM, 00 for USIM
     *   INS (instruction)
     *   P1 P2 P3 (parameters, P3 = length of Data)
     * Response APDU: Data SW1 SW2
     *   SW1 SW2 (Status words)
     * Commands (INS P1 P2 P3):
     *   SELECT: A4 00 00 02 <file_id, 2 bytes>
     *   GET RESPONSE: C0 00 00 <len>
     *   RUN GSM ALG: 88 00 00 00 <RAND len = 10>
     *   RUN UMTS ALG: 88 00 81 <len=0x22> data: 0x10 | RAND | 0x10 | AUTN
     *	P1 = ID of alg in card
     *	P2 = ID of secret key
     *   READ BINARY: B0 <offset high> <offset low> <len>
     *   READ RECORD: B2 <record number> <mode> <len>
     *	P2 (mode) = '02' (next record), '03' (previous record),
     *		    '04' (absolute mode)
     *   VERIFY CHV: 20 00 <CHV number> 08
     *   CHANGE CHV: 24 00 <CHV number> 10
     *   DISABLE CHV: 26 00 01 08
     *   ENABLE CHV: 28 00 01 08
     *   UNBLOCK CHV: 2C 00 <00=CHV1, 02=CHV2> 10
     *   SLEEP: FA 00 00 00
     */

    /* GSM SIM commands */
    #define SIM_CMD_SELECT                0xa0, 0xa4, 0x00, 0x00, 0x02
    #define SIM_CMD_RUN_GSM_ALG           0xa0, 0x88, 0x00, 0x00, 0x10
    #define SIM_CMD_GET_RESPONSE          0xa0, 0xc0, 0x00, 0x00
    #define SIM_CMD_READ_BIN              0xa0, 0xb0, 0x00, 0x00
    #define SIM_CMD_READ_RECORD           0xa0, 0xb2, 0x00, 0x00
    #define SIM_CMD_VERIFY_CHV1           0xa0, 0x20, 0x00, 0x01, 0x08

    /* USIM commands */
    #define USIM_CLA                      0x00
    #define USIM_CMD_RUN_UMTS_ALG         0x00, 0x88, 0x00, 0x81, 0x22
    #define USIM_CMD_GET_RESPONSE         0x00, 0xc0, 0x00, 0x00

    #define SIM_RECORD_MODE_ABSOLUTE      0x04

    #define USIM_FSP_TEMPL_TAG            0x62

    #define USIM_TLV_FILE_DESC            0x82
    #define USIM_TLV_FILE_ID              0x83
    #define USIM_TLV_DF_NAME              0x84
    #define USIM_TLV_PROPR_INFO           0xA5
    #define USIM_TLV_LIFE_CYCLE_STATUS    0x8A
    #define USIM_TLV_FILE_SIZE            0x80
    #define USIM_TLV_TOTAL_FILE_SIZE      0x81
    #define USIM_TLV_PIN_STATUS_TEMPLATE  0xC6
    #define USIM_TLV_SHORT_FILE_ID        0x88
    #define USIM_TLV_SECURITY_ATTR_8B     0x8B
    #define USIM_TLV_SECURITY_ATTR_8C     0x8C
    #define USIM_TLV_SECURITY_ATTR_AB     0xAB

    #define USIM_PS_DO_TAG                0x90

    /* GSM files
     * File type in first octet:
     * 3F = Master File
     * 7F = Dedicated File
     * 2F = Elementary File under the Master File
     * 6F = Elementary File under a Dedicated File
     */
    #define SCARD_FILE_MF                 0x3F00
    #define SCARD_FILE_GSM_DF             0x7F20
    #define SCARD_FILE_UMTS_DF            0x7F50
    #define SCARD_FILE_GSM_EF_IMSI        0x6F07
    #define SCARD_FILE_GSM_EF_AD          0x6FAD
    #define SCARD_FILE_EF_DIR             0x2F00
    #define SCARD_FILE_EF_ICCID           0x2FE2
    #define SCARD_FILE_EF_CK              0x6FE1
    #define SCARD_FILE_EF_IK              0x6FE2

    #define SCARD_CHV1_OFFSET             13
    #define SCARD_CHV1_FLAG               0x80

    SCARDCONTEXT scard_context;
    SCARDHANDLE scard_handle;
    long unsigned scard_protocol;
    sim_types_t sim_type;
    bool pin1_needed;
    srslte::log *log;
  };

  scard       sc;
};

} // namespace srsue

#endif // SRSUE_PCSC_USIM_H
