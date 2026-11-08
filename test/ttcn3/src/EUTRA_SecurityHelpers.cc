// SPDX-FileCopyrightText: Copyright (C) 2013-2026 Software Radio Systems Limited
// SPDX-License-Identifier: AGPL-3.0-or-later

#include "EUTRA_ExternalSecurityFunctions.hh"
#include "mbedtls/md.h"
#include "srsran/common/liblte_security.h"
#include <stdarg.h>
#include <string.h>
#include <vector>

// liblte doesn't take all inputs as const, but only reads them
static uint8_t* to_ptr(const OCTETSTRING& o)
{
  return const_cast<uint8_t*>(static_cast<const unsigned char*>(o));
}

// Calculates the MAC with EIA0 (null), EIA1 (SNOW 3G), EIA2 (AES) or EIA3 (ZUC)
static bool calc_mac(int                alg,
                     const OCTETSTRING& key,
                     uint32_t           count,
                     uint8_t            bearer,
                     uint8_t            direction,
                     const OCTETSTRING& msg,
                     uint8_t*           mac)
{
  switch (alg) {
    case 0:
      memset(mac, 0, 4);
      return true;
    case 1:
      return liblte_security_128_eia1(key, count, bearer, direction, to_ptr(msg), msg.lengthof(), mac) ==
             LIBLTE_SUCCESS;
    case 2:
      return liblte_security_128_eia2(key, count, bearer, direction, msg, msg.lengthof(), mac) == LIBLTE_SUCCESS;
    case 3:
      return liblte_security_128_eia3(key, count, bearer, direction, to_ptr(msg), msg.lengthof() * 8, mac) ==
             LIBLTE_SUCCESS;
    default:
      TTCN_Logger::log(TTCN_ERROR, "Unsupported integrity algorithm %d\n", alg);
      return false;
  }
}

// Ciphers or deciphers msg with EEA0 (null), EEA1 (SNOW 3G), EEA2 (AES) or EEA3 (ZUC), out needs to fit msg
static bool cipher(int                alg,
                   const OCTETSTRING& key,
                   uint32_t           count,
                   uint8_t            bearer,
                   uint8_t            direction,
                   const OCTETSTRING& msg,
                   uint8_t*           out)
{
  uint32_t len_bits = msg.lengthof() * 8;
  switch (alg) {
    case 0:
      memcpy(out, msg, msg.lengthof());
      return true;
    case 1:
      return liblte_security_encryption_eea1(to_ptr(key), count, bearer, direction, to_ptr(msg), len_bits, out) ==
             LIBLTE_SUCCESS;
    case 2:
      return liblte_security_encryption_eea2(key, count, bearer, direction, msg, len_bits, out) == LIBLTE_SUCCESS;
    case 3:
      return liblte_security_encryption_eea3(to_ptr(key), count, bearer, direction, to_ptr(msg), len_bits, out) ==
             LIBLTE_SUCCESS;
    default:
      TTCN_Logger::log(TTCN_ERROR, "Unsupported ciphering algorithm %d\n", alg);
      return false;
  }
}

BITSTRING EUTRA__ExternalSecurityFunctions::fx__KeyDerivationFunction(const INTEGER&     p__KDF,
                                                                      const BITSTRING&   p__Key,
                                                                      const OCTETSTRING& p__String)
{
  boolean Debugging;
  if (TTCN_Logger::log_this_event(TTCN_DEBUG))
    Debugging = TRUE;
  else
    Debugging = FALSE;

  BITSTRING ret;

  // only KDF 1 (tsc_KDF_HMAC_SHA_256) is defined

  // print parameters
  if (Debugging) {
    TTCN_Logger::begin_event(TTCN_DEBUG);
    TTCN_Logger::log_event("fx__KeyDerivationFunction()\n");

    TTCN_Logger::log_event("p__KDF: ");
    p__KDF.log();
    TTCN_Logger::log_event("\n");

    TTCN_Logger::log_event("p__Key: ");
    OCTETSTRING key_oct = bit2oct(p__Key);
    key_oct.log();
    TTCN_Logger::log_event("\n");

    TTCN_Logger::log_event("p__String: ");
    p__String.log();
    TTCN_Logger::log_event("\n");

    TTCN_Logger::end_event();
  }

  // The FC bytes to distinguish the generated keys (TS 33.401 Annex A)
  const uint8_t FC_k_asme     = 0x10;
  const uint8_t FC_k_enb      = 0x11;
  const uint8_t FC_nh         = 0x12;
  const uint8_t FC_k_enb_star = 0x13;
  const uint8_t FC_k_nas      = 0x15;

  uint8_t key_buffer[32] = {};

  switch (p__String[0].get_octet()) {
    case FC_k_asme:
    case FC_k_enb:
    case FC_nh:
    case FC_k_enb_star:
    case FC_k_nas: {
      // all keys are derived with the generic KDF, S is constructed by the test suite
      OCTETSTRING key_oct = bit2oct(p__Key);
      if (mbedtls_md_hmac(mbedtls_md_info_from_type(MBEDTLS_MD_SHA256),
                          key_oct,
                          key_oct.lengthof(),
                          p__String,
                          p__String.lengthof(),
                          key_buffer) != 0) {
        TTCN_Logger::log(TTCN_ERROR, "Key derivation failed\n");
      }
    } break;
    default:
      TTCN_Logger::log(TTCN_ERROR, "Unknown FC byte\n");
  }

  // convert into Octetstring and log
  OCTETSTRING key(32, key_buffer);

  if (Debugging) {
    TTCN_Logger::begin_event(TTCN_DEBUG);

    TTCN_Logger::log_event("Generated key: ");
    key.log();
    TTCN_Logger::log_event("\n");

    TTCN_Logger::end_event();
  }

  // set as return
  ret = oct2bit(key);

  return ret;
}

OCTETSTRING EUTRA__ExternalSecurityFunctions::fx__NasIntegrityAlgorithm(const OCTETSTRING& p__EncodedNasPdu,
                                                                        const BITSTRING&   p__IntegrityAlgorithm,
                                                                        const BITSTRING&   p__KNASint,
                                                                        const OCTETSTRING& p__NasCount,
                                                                        const BITSTRING&   p__BearerId,
                                                                        const INTEGER&     p__Direction)
{
  boolean Debugging;
  if (TTCN_Logger::log_this_event(TTCN_DEBUG)) {
    Debugging = TRUE;
  } else {
    Debugging = FALSE;
  }

  OCTETSTRING ret     = int2oct(0, 4);
  OCTETSTRING key_oct = bit2oct(p__KNASint);

  // print parameters
  if (Debugging) {
    TTCN_Logger::begin_event(TTCN_DEBUG);
    TTCN_Logger::log_event("p__EncodedNasPdu: ");
    p__EncodedNasPdu.log();
    TTCN_Logger::log_event("\n");

    TTCN_Logger::log_event("p__IntegrityAlgorithm: ");
    p__IntegrityAlgorithm.log();
    TTCN_Logger::log_event("\n");

    TTCN_Logger::log_event("p__KNASint: ");
    key_oct.log();
    TTCN_Logger::log_event("\n");

    TTCN_Logger::log_event("p__NasCount: ");
    p__NasCount.log();
    TTCN_Logger::log_event("\n");

    TTCN_Logger::log_event("p__BearerId: ");
    p__BearerId.log();
    TTCN_Logger::log_event("\n");

    TTCN_Logger::log_event("p__Direction: ");
    p__Direction.log();
    TTCN_Logger::log_event("\n");
    TTCN_Logger::end_event();
  }

  // calculate MAC
  uint8_t mac[4] = {};
  if (!calc_mac(bit2int(p__IntegrityAlgorithm),
                key_oct,
                oct2int(p__NasCount).get_long_long_val(),
                bit2int(p__BearerId),
                p__Direction,
                p__EncodedNasPdu,
                mac)) {
    TTCN_Logger::log(TTCN_ERROR, "Calculating NAS MAC failed\n");
    return ret;
  }

  // convert to result type
  ret = OCTETSTRING(4, mac);

  if (Debugging) {
    TTCN_Logger::begin_event(TTCN_DEBUG);
    TTCN_Logger::log_event("Calculated MAC: ");
    ret.log();
    TTCN_Logger::end_event();
  }

  return ret;
}

OCTETSTRING EUTRA__ExternalSecurityFunctions::fx__NasCiphering(const OCTETSTRING& p__EncodedNasPdu,
                                                               const BITSTRING&   p__CipheringAlgorithm,
                                                               const BITSTRING&   p__KNASenc,
                                                               const OCTETSTRING& p__NasCount,
                                                               const BITSTRING&   p__BearerId)
{
  boolean Debugging;
  if (TTCN_Logger::log_this_event(TTCN_DEBUG)) {
    Debugging = TRUE;
  } else {
    Debugging = FALSE;
  }

  // print parameters
  if (Debugging) {
    TTCN_Logger::begin_event(TTCN_DEBUG);
    TTCN_Logger::log_event("p__EncodedNasPdu: ");
    p__EncodedNasPdu.log();
    TTCN_Logger::log_event("\n");

    TTCN_Logger::log_event("p__CipheringAlgorithm: ");
    p__CipheringAlgorithm.log();
    TTCN_Logger::log_event("\n");

    TTCN_Logger::log_event("p__KNASenc: ");
    p__KNASenc.log();
    TTCN_Logger::log_event("\n");

    TTCN_Logger::log_event("p__NasCount: ");
    p__NasCount.log();
    TTCN_Logger::log_event("\n");

    TTCN_Logger::log_event("p__BearerId: ");
    p__BearerId.log();
    TTCN_Logger::log_event("\n");

    TTCN_Logger::end_event();
  }
  OCTETSTRING          ret = int2oct(0, 4);
  std::vector<uint8_t> ciphered_msg(p__EncodedNasPdu.lengthof());

  if (!cipher(bit2int(p__CipheringAlgorithm),
              bit2oct(p__KNASenc),
              oct2int(p__NasCount).get_long_long_val(),
              bit2int(p__BearerId),
              LIBLTE_SECURITY_DIRECTION_DOWNLINK,
              p__EncodedNasPdu,
              ciphered_msg.data())) {
    TTCN_Logger::log(TTCN_ERROR, "Ciphering NAS PDU failed\n");
    return ret;
  }

  // convert to result type
  ret = OCTETSTRING(p__EncodedNasPdu.lengthof(), ciphered_msg.data());

  if (Debugging) {
    TTCN_Logger::begin_event(TTCN_DEBUG);
    TTCN_Logger::log_event("Ciphered message: ");
    ret.log();
    TTCN_Logger::end_event();
  }

  return ret;
}

OCTETSTRING EUTRA__ExternalSecurityFunctions::fx__NasDeciphering(const OCTETSTRING& p__CipheredNasMsg,
                                                                 const BITSTRING&   p__CipheringAlgorithm,
                                                                 const BITSTRING&   p__KNASenc,
                                                                 const OCTETSTRING& p__NasCount,
                                                                 const BITSTRING&   p__BearerId)
{
  boolean Debugging;
  if (TTCN_Logger::log_this_event(TTCN_DEBUG)) {
    Debugging = TRUE;
  } else {
    Debugging = FALSE;
  }

  OCTETSTRING ret = int2oct(0, 4);

  // print parameters
  if (Debugging) {
    TTCN_Logger::begin_event(TTCN_DEBUG);
    TTCN_Logger::log_event("p__CipheredNasMsg: ");
    p__CipheredNasMsg.log();
    TTCN_Logger::log_event("\n");

    TTCN_Logger::log_event("p__CipheringAlgorithm: ");
    p__CipheringAlgorithm.log();
    TTCN_Logger::log_event("\n");

    TTCN_Logger::log_event("p__KNASenc: ");
    p__KNASenc.log();
    TTCN_Logger::log_event("\n");

    TTCN_Logger::log_event("p__NasCount: ");
    p__NasCount.log();
    TTCN_Logger::log_event("\n");

    TTCN_Logger::log_event("p__BearerId: ");
    p__BearerId.log();
    TTCN_Logger::log_event("\n");

    TTCN_Logger::end_event();
  }

  std::vector<uint8_t> deciphered_msg(p__CipheredNasMsg.lengthof());

  if (!cipher(bit2int(p__CipheringAlgorithm),
              bit2oct(p__KNASenc),
              oct2int(p__NasCount).get_long_long_val(),
              bit2int(p__BearerId),
              LIBLTE_SECURITY_DIRECTION_UPLINK,
              p__CipheredNasMsg,
              deciphered_msg.data())) {
    TTCN_Logger::log(TTCN_ERROR, "Deciphering NAS PDU failed\n");
    return ret;
  }

  // convert to result type
  ret = OCTETSTRING(p__CipheredNasMsg.lengthof(), deciphered_msg.data());

  if (Debugging) {
    TTCN_Logger::begin_event(TTCN_DEBUG);
    TTCN_Logger::log_event("Deciphered message: ");
    ret.log();
    TTCN_Logger::end_event();
  }

  return ret;
}

OCTETSTRING EUTRA__ExternalSecurityFunctions::fx__AsIntegrityAlgorithm(
    const OCTETSTRING&                                                                   p__EncodedPdcpPdu,
    const EUTRA__RRC__ASN1__Definitions::SecurityAlgorithmConfig_integrityProtAlgorithm& p__IntegrityAlgorithm,
    const BITSTRING&                                                                     p__KRRCint,
    const OCTETSTRING&                                                                   p__PdcpCount,
    const BITSTRING&                                                                     p__BearerId,
    const INTEGER&                                                                       p__Direction)
{
  boolean Debugging;
  if (TTCN_Logger::log_this_event(TTCN_DEBUG))
    Debugging = TRUE;
  else
    Debugging = FALSE;

  OCTETSTRING ret     = int2oct(0, 4);
  OCTETSTRING key_oct = bit2oct(p__KRRCint);

  // print parameters
  if (Debugging) {
    TTCN_Logger::begin_event(TTCN_DEBUG);
    TTCN_Logger::log_event("p__EncodedPdcpPdu: ");
    p__EncodedPdcpPdu.log();
    TTCN_Logger::log_event("\n");

    TTCN_Logger::log_event("p__IntegrityAlgorithm: ");
    p__IntegrityAlgorithm.log();
    TTCN_Logger::log_event("\n");

    TTCN_Logger::log_event("p__KRRCint: ");
    key_oct.log();
    TTCN_Logger::log_event("\n");

    TTCN_Logger::log_event("p__PdcpCount: ");
    p__PdcpCount.log();
    TTCN_Logger::log_event("\n");

    TTCN_Logger::log_event("p__BearerId: ");
    p__BearerId.log();
    TTCN_Logger::log_event("\n");

    TTCN_Logger::log_event("p__Direction: ");
    p__Direction.log();
    TTCN_Logger::log_event("\n");

    TTCN_Logger::end_event();
  }

  uint8_t mac[4] = {};

  // calculate MAC
  if (!calc_mac(p__IntegrityAlgorithm,
                key_oct,
                oct2int(p__PdcpCount).get_long_long_val(),
                bit2int(p__BearerId),
                p__Direction,
                p__EncodedPdcpPdu,
                mac)) {
    TTCN_Logger::log(TTCN_ERROR, "Calculating RRC MAC failed\n");
    return ret;
  }

  // convert to result type
  ret = OCTETSTRING(4, mac);

  if (Debugging) {
    TTCN_Logger::begin_event(TTCN_DEBUG);
    TTCN_Logger::log_event("Calculated MAC: ");
    ret.log();
    TTCN_Logger::log_event("\n");

    TTCN_Logger::end_event();
  }

  return ret;
}

OCTETSTRING EUTRA__ExternalSecurityFunctions::fx__AsCiphering(
    const OCTETSTRING&                                                               p__EncodedSdu,
    const EUTRA__RRC__ASN1__Definitions::SecurityAlgorithmConfig_cipheringAlgorithm& p__CipheringAlgorithm,
    const BITSTRING&                                                                 p__Key,
    const OCTETSTRING&                                                               p__PdcpCount,
    const BITSTRING&                                                                 p__BearerId)
{
  boolean Debugging;
  if (TTCN_Logger::log_this_event(TTCN_DEBUG)) {
    Debugging = TRUE;
  } else {
    Debugging = FALSE;
  }

  // print parameters
  if (Debugging) {
    TTCN_Logger::begin_event(TTCN_DEBUG);
    TTCN_Logger::log_event("p__EncodedSdu: ");
    p__EncodedSdu.log();

    TTCN_Logger::log_event("p__CipheringAlgorithm: ");
    p__CipheringAlgorithm.log();

    TTCN_Logger::log_event("p__Key: ");
    p__Key.log();

    TTCN_Logger::log_event("p__PdcpCount: ");
    p__PdcpCount.log();

    TTCN_Logger::log_event("p__BearerId: ");
    p__BearerId.log();

    TTCN_Logger::end_event();
  }

  OCTETSTRING          ret = int2oct(0, 4);
  std::vector<uint8_t> ciphered_msg(p__EncodedSdu.lengthof());

  if (!cipher(p__CipheringAlgorithm,
              bit2oct(p__Key),
              oct2int(p__PdcpCount).get_long_long_val(),
              bit2int(p__BearerId),
              LIBLTE_SECURITY_DIRECTION_DOWNLINK,
              p__EncodedSdu,
              ciphered_msg.data())) {
    TTCN_Logger::log(TTCN_ERROR, "Ciphering RRC PDU failed\n");
    return ret;
  }

  // convert to result type
  ret = OCTETSTRING(p__EncodedSdu.lengthof(), ciphered_msg.data());

  if (Debugging) {
    TTCN_Logger::begin_event(TTCN_DEBUG);
    TTCN_Logger::log_event("Ciphered message: ");
    ret.log();
    TTCN_Logger::end_event();
  }

  return ret;
}

OCTETSTRING EUTRA__ExternalSecurityFunctions::fx__AsDeciphering(
    const OCTETSTRING&                                                               p__CipheredSdu,
    const EUTRA__RRC__ASN1__Definitions::SecurityAlgorithmConfig_cipheringAlgorithm& p__CipheringAlgorithm,
    const BITSTRING&                                                                 p__Key,
    const OCTETSTRING&                                                               p__PdcpCount,
    const BITSTRING&                                                                 p__BearerId)
{
  boolean Debugging;
  if (TTCN_Logger::log_this_event(TTCN_DEBUG)) {
    Debugging = TRUE;
  } else {
    Debugging = FALSE;
  }

  // print parameters
  if (Debugging) {
    TTCN_Logger::begin_event(TTCN_DEBUG);
    TTCN_Logger::log_event("p__CipheredSdu: ");
    p__CipheredSdu.log();
    TTCN_Logger::log_event("\n");

    TTCN_Logger::log_event("p__CipheringAlgorithm: ");
    p__CipheringAlgorithm.log();
    TTCN_Logger::log_event("\n");

    TTCN_Logger::log_event("p__Key: ");
    p__Key.log();
    TTCN_Logger::log_event("\n");

    TTCN_Logger::log_event("p__PdcpCount: ");
    p__PdcpCount.log();
    TTCN_Logger::log_event("\n");

    TTCN_Logger::log_event("p__BearerId: ");
    p__BearerId.log();
    TTCN_Logger::log_event("\n");

    TTCN_Logger::end_event();
  }

  OCTETSTRING          ret = int2oct(0, 4);
  std::vector<uint8_t> deciphered_msg(p__CipheredSdu.lengthof());

  if (!cipher(p__CipheringAlgorithm,
              bit2oct(p__Key),
              oct2int(p__PdcpCount).get_long_long_val(),
              bit2int(p__BearerId),
              LIBLTE_SECURITY_DIRECTION_UPLINK,
              p__CipheredSdu,
              deciphered_msg.data())) {
    TTCN_Logger::log(TTCN_ERROR, "Deciphering RRC PDU failed\n");
    return ret;
  }

  // convert to result type
  ret = OCTETSTRING(p__CipheredSdu.lengthof(), deciphered_msg.data());

  if (Debugging) {
    TTCN_Logger::begin_event(TTCN_DEBUG);
    TTCN_Logger::log_event("Deciphered message: ");
    ret.log();
    TTCN_Logger::end_event();
  }

  return ret;
}
