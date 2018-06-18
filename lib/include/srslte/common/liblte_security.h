/*******************************************************************************

    Copyright 2014 Ben Wojtowicz

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

*******************************************************************************

    File: liblte_security.h

    Description: Contains all the definitions for the LTE security algorithm
                 library.

    Revision History
    ----------    -------------    --------------------------------------------
    08/03/2014    Ben Wojtowicz    Created file.
    09/03/2014    Ben Wojtowicz    Added key generation and EIA2.

*******************************************************************************/

#ifndef SRSLTE_LIBLTE_SECURITY_H
#define SRSLTE_LIBLTE_SECURITY_H

/*******************************************************************************
                              INCLUDES
*******************************************************************************/

#include "srslte/asn1/liblte_common.h"

/*******************************************************************************
                              DEFINES
*******************************************************************************/


/*******************************************************************************
                              TYPEDEFS
*******************************************************************************/


/*******************************************************************************
                              DECLARATIONS
*******************************************************************************/

/*********************************************************************
    Name: liblte_security_generate_k_asme

    Description: Generate the security key Kasme.

    Document Reference: 33.401 v10.0.0 Annex A.2
*********************************************************************/
// Defines
// Enums
// Structs
// Functions
LIBLTE_ERROR_ENUM liblte_security_generate_k_asme(uint8  *ck,
                                                  uint8  *ik,
                                                  uint8  *ak,
                                                  uint8  *sqn,
                                                  uint16  mcc,
                                                  uint16  mnc,
                                                  uint8  *k_asme);

/*********************************************************************
    Name: liblte_security_generate_k_enb

    Description: Generate the security key Kenb.

    Document Reference: 33.401 v10.0.0 Annex A.2
*********************************************************************/
// Defines
// Enums
// Structs
// Functions
LIBLTE_ERROR_ENUM liblte_security_generate_k_enb(uint8  *k_asme,
                                                 uint32  nas_count,
                                                 uint8  *k_enb);

LIBLTE_ERROR_ENUM liblte_security_generate_k_enb_star(uint8  *k_enb,
                                                      uint32  pci,
                                                      uint32_t earfcn,
                                                      uint8  *k_enb_star);

LIBLTE_ERROR_ENUM liblte_security_generate_nh( uint8_t *k_asme,
                                               uint8_t *sync,
                                               uint8_t *nh);

/*********************************************************************
    Name: liblte_security_generate_k_nas

    Description: Generate the NAS security keys KNASenc and KNASint.

    Document Reference: 33.401 v10.0.0 Annex A.2
*********************************************************************/
// Defines
// Enums
typedef enum{
    LIBLTE_SECURITY_CIPHERING_ALGORITHM_ID_EEA0 = 0,
    LIBLTE_SECURITY_CIPHERING_ALGORITHM_ID_128_EEA1,
    LIBLTE_SECURITY_CIPHERING_ALGORITHM_ID_128_EEA2,
    LIBLTE_SECURITY_CIPHERING_ALGORITHM_ID_N_ITEMS,
}LIBLTE_SECURITY_CIPHERING_ALGORITHM_ID_ENUM;
static const char liblte_security_ciphering_algorithm_id_text[LIBLTE_SECURITY_CIPHERING_ALGORITHM_ID_N_ITEMS][20] = {"EEA0",
                                                                                                                     "128-EEA1",
                                                                                                                     "128-EEA2"};
typedef enum{
    LIBLTE_SECURITY_INTEGRITY_ALGORITHM_ID_EIA0 = 0,
    LIBLTE_SECURITY_INTEGRITY_ALGORITHM_ID_128_EIA1,
    LIBLTE_SECURITY_INTEGRITY_ALGORITHM_ID_128_EIA2,
    LIBLTE_SECURITY_INTEGRITY_ALGORITHM_ID_N_ITEMS,
}LIBLTE_SECURITY_INTEGRITY_ALGORITHM_ID_ENUM;
static const char liblte_security_integrity_algorithm_id_text[LIBLTE_SECURITY_INTEGRITY_ALGORITHM_ID_N_ITEMS][20] = {"EIA0",
                                                                                                                     "128-EIA1",
                                                                                                                     "128-EIA2"};
// Structs
// Functions
LIBLTE_ERROR_ENUM liblte_security_generate_k_nas(uint8                                       *k_asme,
                                                 LIBLTE_SECURITY_CIPHERING_ALGORITHM_ID_ENUM  enc_alg_id,
                                                 LIBLTE_SECURITY_INTEGRITY_ALGORITHM_ID_ENUM  int_alg_id,
                                                 uint8                                       *k_nas_enc,
                                                 uint8                                       *k_nas_int);



/*********************************************************************
    Name: liblte_security_generate_k_rrc

    Description: Generate the RRC security keys KRRCenc and KRRCint.

    Document Reference: 33.401 v10.0.0 Annex A.2
*********************************************************************/
// Defines
// Enums
// Structs
// Functions
LIBLTE_ERROR_ENUM liblte_security_generate_k_rrc(uint8                                       *k_enb,
                                                 LIBLTE_SECURITY_CIPHERING_ALGORITHM_ID_ENUM  enc_alg_id,
                                                 LIBLTE_SECURITY_INTEGRITY_ALGORITHM_ID_ENUM  int_alg_id,
                                                 uint8                                       *k_rrc_enc,
                                                 uint8                                       *k_rrc_int);

/*********************************************************************
    Name: liblte_security_generate_k_up

    Description: Generate the user plane security keys KUPenc and
                 KUPint.

    Document Reference: 33.401 v10.0.0 Annex A.2
*********************************************************************/
// Defines
// Enums
// Structs
// Functions
LIBLTE_ERROR_ENUM liblte_security_generate_k_up(uint8                                       *k_enb,
                                                LIBLTE_SECURITY_CIPHERING_ALGORITHM_ID_ENUM  enc_alg_id,
                                                LIBLTE_SECURITY_INTEGRITY_ALGORITHM_ID_ENUM  int_alg_id,
                                                uint8                                       *k_up_enc,
                                                uint8                                       *k_up_int);

/*********************************************************************
    Name: liblte_security_128_eia2

    Description: 128-bit integrity algorithm EIA2.

    Document Reference: 33.401 v10.0.0 Annex B.2.3
                        33.102 v10.0.0 Section 6.5.4
                        RFC4493
*********************************************************************/
// Defines
#define LIBLTE_SECURITY_DIRECTION_UPLINK   0
#define LIBLTE_SECURITY_DIRECTION_DOWNLINK 1
// Enums
// Structs
// Functions
LIBLTE_ERROR_ENUM liblte_security_128_eia2(uint8  *key,
                                           uint32  count,
                                           uint8   bearer,
                                           uint8   direction,
                                           uint8  *msg,
                                           uint32  msg_len,
                                           uint8  *mac);
LIBLTE_ERROR_ENUM liblte_security_128_eia2(uint8                 *key,
                                           uint32                 count,
                                           uint8                  bearer,
                                           uint8                  direction,
                                           LIBLTE_BIT_MSG_STRUCT *msg,
                                           uint8                 *mac);

/*********************************************************************
    Name: liblte_security_encryption_eea1

    Description: 128-bit encryption algorithm EEA1.

    Document Reference: 33.401 v13.1.0 Annex B.1.2
                        35.215 v13.0.0 References
                        Specification of the 3GPP Confidentiality and
                            Integrity Algorithms UEA2 & UIA2 D1 v2.1
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_security_encryption_eea1(uint8  *key,
                                                  uint32  count,
                                                  uint8   bearer,
                                                  uint8   direction,
                                                  uint8  *msg,
                                                  uint32  msg_len,
                                                  uint8  *out);

/*********************************************************************
    Name: liblte_security_decryption_eea1

    Description: 128-bit decryption algorithm EEA1.

    Document Reference: 33.401 v13.1.0 Annex B.1.2
                        35.215 v13.0.0 References
                        Specification of the 3GPP Confidentiality and
                            Integrity Algorithms UEA2 & UIA2 D1 v2.1
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_security_decryption_eea1(uint8  *key,
                                                  uint32  count,
                                                  uint8   bearer,
                                                  uint8   direction,
                                                  uint8  *ct,
                                                  uint32  ct_len,
                                                  uint8  *out);

/*********************************************************************
    Name: liblte_security_encryption_eea2

    Description: 128-bit encryption algorithm EEA2.

    Document Reference: 33.401 v13.1.0 Annex B.1.3
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_security_encryption_eea2(uint8  *key,
                                                  uint32  count,
                                                  uint8   bearer,
                                                  uint8   direction,
                                                  uint8  *msg,
                                                  uint32  msg_len,
                                                  uint8  *out);

/*********************************************************************
    Name: liblte_security_decryption_eea2

    Description: 128-bit decryption algorithm EEA2.

    Document Reference: 33.401 v13.1.0 Annex B.1.3
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_security_decryption_eea2(uint8  *key,
                                                  uint32  count,
                                                  uint8   bearer,
                                                  uint8   direction,
                                                  uint8  *ct,
                                                  uint32  ct_len,
                                                  uint8  *out);


/*********************************************************************
    Name: liblte_security_milenage_f1

    Description: Milenage security function F1.  Computes network
                 authentication code MAC-A from key K, random
                 challenge RAND, sequence number SQN, and
                 authentication management field AMF.

    Document Reference: 35.206 v10.0.0 Annex 3
*********************************************************************/
// Defines
// Enums
// Structs
// Functions
LIBLTE_ERROR_ENUM liblte_security_milenage_f1(uint8 *k,
                                              uint8 *op,
                                              uint8 *rand,
                                              uint8 *sqn,
                                              uint8 *amf,
                                              uint8 *mac_a);

/*********************************************************************
    Name: liblte_security_milenage_f1_star

    Description: Milenage security function F1*.  Computes resynch
                 authentication code MAC-S from key K, random
                 challenge RAND, sequence number SQN, and
                 authentication management field AMF.

    Document Reference: 35.206 v10.0.0 Annex 3
*********************************************************************/
// Defines
// Enums
// Structs
// Functions
LIBLTE_ERROR_ENUM liblte_security_milenage_f1_star(uint8 *k,
                                                   uint8 *op,
                                                   uint8 *rand,
                                                   uint8 *sqn,
                                                   uint8 *amf,
                                                   uint8 *mac_s);

/*********************************************************************
    Name: liblte_security_milenage_f2345

    Description: Milenage security functions F2, F3, F4, and F5.
                 Computes response RES, confidentiality key CK,
                 integrity key IK, and anonymity key AK from random
                 challenge RAND.

    Document Reference: 35.206 v10.0.0 Annex 3
*********************************************************************/
// Defines
// Enums
// Structs
// Functions
LIBLTE_ERROR_ENUM liblte_security_milenage_f2345(uint8 *k,
                                                 uint8 *op,
                                                 uint8 *rand,
                                                 uint8 *res,
                                                 uint8 *ck,
                                                 uint8 *ik,
                                                 uint8 *ak);

/*********************************************************************
    Name: liblte_security_milenage_f5_star

    Description: Milenage security function F5*.  Computes resynch
                 anonymity key AK from key K and random challenge
                 RAND.

    Document Reference: 35.206 v10.0.0 Annex 3
*********************************************************************/
// Defines
// Enums
// Structs
// Functions
LIBLTE_ERROR_ENUM liblte_security_milenage_f5_star(uint8 *k,
                                                   uint8 *op,
                                                   uint8 *rand,
                                                   uint8 *ak);

#endif // SRSLTE_LIBLTE_SECURITY_H
