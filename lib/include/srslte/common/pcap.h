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

#ifndef SRSLTE_PCAP_H
#define SRSLTE_PCAP_H

#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/time.h>

#define MAC_LTE_DLT  147
#define NAS_LTE_DLT  148
#define RLC_LTE_DLT  149 // UDP needs to be selected as protocol


/* This structure gets written to the start of the file */
typedef struct pcap_hdr_s {
        unsigned int   magic_number;   /* magic number */
        unsigned short version_major;  /* major version number */
        unsigned short version_minor;  /* minor version number */
        unsigned int   thiszone;       /* GMT to local correction */
        unsigned int   sigfigs;        /* accuracy of timestamps */
        unsigned int   snaplen;        /* max length of captured packets, in octets */
        unsigned int   network;        /* data link type */
} pcap_hdr_t;

/* This structure precedes each packet */
typedef struct pcaprec_hdr_s {
        unsigned int   ts_sec;         /* timestamp seconds */
        unsigned int   ts_usec;        /* timestamp microseconds */
        unsigned int   incl_len;       /* number of octets of packet saved in file */
        unsigned int   orig_len;       /* actual length of packet */
} pcaprec_hdr_t;


/* radioType */
#define FDD_RADIO 1
#define TDD_RADIO 2

/* Direction */
#define DIRECTION_UPLINK   0
#define DIRECTION_DOWNLINK 1

/* rntiType */
#define NO_RNTI  0  /* Used for BCH-BCH */
#define P_RNTI   1
#define RA_RNTI  2
#define C_RNTI   3
#define SI_RNTI  4
#define SPS_RNTI 5
#define M_RNTI   6

#define MAC_LTE_START_STRING        "mac-lte"
#define MAC_LTE_PAYLOAD_TAG         0x01
#define MAC_LTE_RNTI_TAG            0x02
#define MAC_LTE_UEID_TAG            0x03
#define MAC_LTE_FRAME_SUBFRAME_TAG  0x04
#define MAC_LTE_PREDFINED_DATA_TAG  0x05
#define MAC_LTE_RETX_TAG            0x06
#define MAC_LTE_CRC_STATUS_TAG      0x07



/* Context information for every MAC PDU that will be logged */
typedef struct MAC_Context_Info_t {
    unsigned short radioType;
    unsigned char  direction;
    unsigned char  rntiType;
    unsigned short rnti;
    unsigned short ueid;
    unsigned char  isRetx;
    unsigned char  crcStatusOK;

    unsigned short sysFrameNumber;
    unsigned short subFrameNumber;
} MAC_Context_Info_t;

/* Context information for every NAS PDU that will be logged */
typedef struct NAS_Context_Info_s {
  // No Context yet
} NAS_Context_Info_t;


/* RLC-LTE disector */

/* rlcMode */
#define RLC_TM_MODE 1
#define RLC_UM_MODE 2
#define RLC_AM_MODE 4
#define RLC_PREDEF  8

/* priority ? */

/* channelType */
#define CHANNEL_TYPE_CCCH 1
#define CHANNEL_TYPE_BCCH_BCH 2
#define CHANNEL_TYPE_PCCH 3
#define CHANNEL_TYPE_SRB 4
#define CHANNEL_TYPE_DRB 5
#define CHANNEL_TYPE_BCCH_DL_SCH 6
#define CHANNEL_TYPE_MCCH 7
#define CHANNEL_TYPE_MTCH 8

/* sequenceNumberLength */
#define UM_SN_LENGTH_5_BITS 5
#define UM_SN_LENGTH_10_BITS 10
#define AM_SN_LENGTH_10_BITS 10
#define AM_SN_LENGTH_16_BITS 16

/* Narrow band mode */
typedef enum {
  rlc_no_nb_mode = 0,
  rlc_nb_mode = 1
} rlc_lte_nb_mode;

/* Context information for every RLC PDU that will be logged */
typedef struct {
  unsigned char   rlcMode;
  unsigned char   direction;
  unsigned char   priority;
  unsigned char   sequenceNumberLength;
  unsigned short  ueid;
  unsigned short  channelType;
  unsigned short  channelId; /* for SRB: 1=SRB1, 2=SRB2, 3=SRB1bis; for DRB: DRB ID */
  unsigned short  pduLength;
  bool            extendedLiField;
  rlc_lte_nb_mode nbMode;
} RLC_Context_Info_t;


// See Wireshark's packet-rlc-lte.h for details
#define RLC_LTE_START_STRING "rlc-lte"
#define RLC_LTE_SN_LENGTH_TAG    0x02
#define RLC_LTE_DIRECTION_TAG    0x03
#define RLC_LTE_PRIORITY_TAG     0x04
#define RLC_LTE_UEID_TAG         0x05
#define RLC_LTE_CHANNEL_TYPE_TAG 0x06
#define RLC_LTE_CHANNEL_ID_TAG   0x07
#define RLC_LTE_EXT_LI_FIELD_TAG 0x08
#define RLC_LTE_NB_MODE_TAG      0x09
#define RLC_LTE_PAYLOAD_TAG      0x01



/**************************************************************************
 * API functions for opening/closing LTE PCAP files                       *
 **************************************************************************/

/* Open the file and write file header */
inline FILE *LTE_PCAP_Open(uint32_t DLT, const char *fileName)
{
    pcap_hdr_t file_header =
    {
        0xa1b2c3d4,   /* magic number */
        2, 4,         /* version number is 2.4 */
        0,            /* timezone */
        0,            /* sigfigs - apparently all tools do this */
        65535,        /* snaplen - this should be long enough */
        DLT           /* Data Link Type (DLT).  Set as unused value 147 for now */
    };

    FILE *fd = fopen(fileName, "w");
    if (fd == NULL) {
        printf("Failed to open file \"%s\" for writing\n", fileName);
        return NULL;
    }

    /* Write the file header */
    fwrite(&file_header, sizeof(pcap_hdr_t), 1, fd);

    return fd;
}

/* Close the PCAP file */
inline void LTE_PCAP_Close(FILE *fd)
{
  if(fd)
    fclose(fd);
}


/**************************************************************************
 * API functions for writing MAC-LTE PCAP files                           *
 **************************************************************************/

/* Write an individual PDU (PCAP packet header + mac-context + mac-pdu) */
inline int LTE_PCAP_MAC_WritePDU(FILE *fd, MAC_Context_Info_t *context,
                                 const unsigned char *PDU, unsigned int length)
{
    pcaprec_hdr_t packet_header;
    char context_header[256];
    int offset = 0;
    uint16_t tmp16;

    /* Can't write if file wasn't successfully opened */
    if (fd == NULL) {
        printf("Error: Can't write to empty file handle\n");
        return 0;
    }

    /*****************************************************************/
    /* Context information (same as written by UDP heuristic clients */
    context_header[offset++] = context->radioType;
    context_header[offset++] = context->direction;
    context_header[offset++] = context->rntiType;

    /* RNTI */
    context_header[offset++] = MAC_LTE_RNTI_TAG;
    tmp16 = htons(context->rnti);
    memcpy(context_header+offset, &tmp16, 2);
    offset += 2;

    /* UEId */
    context_header[offset++] = MAC_LTE_UEID_TAG;
    tmp16 = htons(context->ueid);
    memcpy(context_header+offset, &tmp16, 2);
    offset += 2;

    /* Subframe Number and System Frame Number */
    /* SFN is stored in 12 MSB and SF in 4 LSB */
    context_header[offset++] = MAC_LTE_FRAME_SUBFRAME_TAG;
    tmp16 = (context->sysFrameNumber << 4) | context->subFrameNumber;
    tmp16 = htons(tmp16);
    memcpy(context_header+offset, &tmp16, 2);
    offset += 2;

    /* CRC Status */
    context_header[offset++] = MAC_LTE_CRC_STATUS_TAG;
    context_header[offset++] = context->crcStatusOK;

    /* Data tag immediately preceding PDU */
    context_header[offset++] = MAC_LTE_PAYLOAD_TAG;


    /****************************************************************/
    /* PCAP Header                                                  */
    struct timeval t;
    gettimeofday(&t, NULL);
    packet_header.ts_sec = t.tv_sec;
    packet_header.ts_usec = t.tv_usec;
    packet_header.incl_len = offset + length;
    packet_header.orig_len = offset + length;

    /***************************************************************/
    /* Now write everything to the file                            */
    fwrite(&packet_header, sizeof(pcaprec_hdr_t), 1, fd);
    fwrite(context_header, 1, offset, fd);
    fwrite(PDU, 1, length, fd);

    return 1;
}



/**************************************************************************
 * API functions for writing NAS-EPS PCAP files                           *
 **************************************************************************/

/* Write an individual PDU (PCAP packet header + nas-context + nas-pdu) */
inline int LTE_PCAP_NAS_WritePDU(FILE *fd, NAS_Context_Info_t *context,
                                 const unsigned char *PDU, unsigned int length)
{
    pcaprec_hdr_t packet_header;

    /* Can't write if file wasn't successfully opened */
    if (fd == NULL) {
        printf("Error: Can't write to empty file handle\n");
        return 0;
    }

    /****************************************************************/
    /* PCAP Header                                                  */
    struct timeval t;
    gettimeofday(&t, NULL);
    packet_header.ts_sec = t.tv_sec;
    packet_header.ts_usec = t.tv_usec;
    packet_header.incl_len = length;
    packet_header.orig_len = length;

    /***************************************************************/
    /* Now write everything to the file                            */
    fwrite(&packet_header, sizeof(pcaprec_hdr_t), 1, fd);
    fwrite(PDU, 1, length, fd);

    return 1;
}


/**************************************************************************
 * API functions for writing RLC-LTE PCAP files                           *
 **************************************************************************/

/* Write an individual RLC PDU (PCAP packet header + UDP header + rlc-context + rlc-pdu) */
inline int LTE_PCAP_RLC_WritePDU(FILE *fd, RLC_Context_Info_t *context,
                                 const unsigned char *PDU, unsigned int length)
{
    pcaprec_hdr_t packet_header;
    char context_header[256];
    int offset = 0;
    uint16_t tmp16;

    /* Can't write if file wasn't successfully opened */
    if (fd == NULL) {
        printf("Error: Can't write to empty file handle\n");
        return 0;
    }

    /*****************************************************************/

    // Add dummy UDP header, start with src and dest port
    context_header[offset++] = 0xde;
    context_header[offset++] = 0xad;
    context_header[offset++] = 0xbe;
    context_header[offset++] = 0xef;
    // length
    tmp16 = length + 12;
    memcpy(context_header+offset, &tmp16, 2);
    offset += 2;
    // dummy CRC
    context_header[offset++] = 0xde;
    context_header[offset++] = 0xad;

    // Start magic string
    memcpy(&context_header[offset], RLC_LTE_START_STRING, strlen(RLC_LTE_START_STRING));
    offset += strlen(RLC_LTE_START_STRING);

    // Fixed field RLC mode
    context_header[offset++] = context->rlcMode;

    // Conditional fields
    if (context->rlcMode == RLC_UM_MODE) {
        context_header[offset++] = RLC_LTE_SN_LENGTH_TAG;
        context_header[offset++] = context->sequenceNumberLength;
    }

    // Optional fields
    context_header[offset++] = RLC_LTE_DIRECTION_TAG;
    context_header[offset++] = context->direction;

    context_header[offset++] = RLC_LTE_PRIORITY_TAG;
    context_header[offset++] = context->priority;

    context_header[offset++] = RLC_LTE_UEID_TAG;
    tmp16 = htons(context->ueid);
    memcpy(context_header+offset, &tmp16, 2);
    offset += 2;

    context_header[offset++] = RLC_LTE_CHANNEL_TYPE_TAG;
    tmp16 = htons(context->channelType);
    memcpy(context_header+offset, &tmp16, 2);
    offset += 2;

    context_header[offset++] = RLC_LTE_CHANNEL_ID_TAG;
    tmp16 = htons(context->channelId);
    memcpy(context_header+offset, &tmp16, 2);
    offset += 2;

    // Now the actual PDU
    context_header[offset++] = RLC_LTE_PAYLOAD_TAG;

    // PCAP header
    struct timeval t;
    gettimeofday(&t, NULL);
    packet_header.ts_sec = t.tv_sec;
    packet_header.ts_usec = t.tv_usec;
    packet_header.incl_len = offset + length;
    packet_header.orig_len = offset + length;

    // Write everything to file
    fwrite(&packet_header, sizeof(pcaprec_hdr_t), 1, fd);
    fwrite(context_header, 1, offset, fd);
    fwrite(PDU, 1, length, fd);

    return 1;
}

#endif // SRSLTE_PCAP_H
