/*******************************************************************************

    Copyright 2012-2014 Ben Wojtowicz
    Copyright 2014 Andrew Murphy (SIB13 unpack)

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

    File: liblte_rrc.cc

    Description: Contains all the implementations for the LTE Radio Resource
                 Control Layer library.

    Revision History
    ----------    -------------    --------------------------------------------
    03/24/2012    Ben Wojtowicz    Created file.
    04/25/2012    Ben Wojtowicz    Added SIB1 parameters, IEs, and messages
    06/02/2012    Ben Wojtowicz    Added message and IE packing
    06/09/2012    Ben Wojtowicz    Added the number of bits used to SIB1 pack
    08/19/2012    Ben Wojtowicz    Added functionality to support SIB2, SIB3,
                                   SIB4, and SIB8 packing and unpacking
    10/06/2012    Ben Wojtowicz    Added more decoding/encoding.
    11/10/2012    Ben Wojtowicz    Filled in the N_bits for SI messages
    03/03/2013    Ben Wojtowicz    Added carrier_freqs_geran, SIB5, SIB6, SIB7
                                   and paging packing and unpacking.
    07/21/2013    Ben Wojtowicz    Fixed several bugs and moved to the common
                                   message struct.
    09/16/2013    Ben Wojtowicz    Added error checking on sizes passed to
                                   memcpy.
    03/26/2014    Ben Wojtowicz    Added support for RRC Connection Request,
                                   RRC Connection Reestablishment Request,
                                   and UL CCCH Messages.
    05/04/2014    Ben Wojtowicz    Added support for DL CCCH Messages.
    06/15/2014    Ben Wojtowicz    Added support for UL DCCH Messages.
    08/03/2014    Ben Wojtowicz    Added more decoding/encoding and using the
                                   common value_2_bits and bits_2_value
                                   functions.
    09/19/2014    Andrew Murphy    Added SIB13 unpack.
    11/01/2014    Ben Wojtowicz    Added more decoding/encoding.
    11/09/2014    Ben Wojtowicz    Added SIB13 pack.
    11/29/2014    Ben Wojtowicz    Fixed a bug in RRC connection reestablishment
                                   UE identity.

*******************************************************************************/

/*******************************************************************************
                              INCLUDES
*******************************************************************************/

#include "srslte/asn1/liblte_rrc.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

/*******************************************************************************
                              DEFINES
*******************************************************************************/


/*******************************************************************************
                              TYPEDEFS
*******************************************************************************/


/*******************************************************************************
                              GLOBAL VARIABLES
*******************************************************************************/

LIBLTE_BIT_MSG_STRUCT global_msg;

/*******************************************************************************
                              HELPERS
*******************************************************************************/

/*********************************************************************
    Functions for external logging 
*********************************************************************/
static log_handler_t log_handler; 
static void *callback_ctx = NULL; 

void liblte_rrc_log_register_handler(void *ctx, log_handler_t handler) {
  log_handler  = handler; 
  callback_ctx = ctx; 
}

static void liblte_rrc_log_print(const char *format, ...) {
  va_list   args;
  va_start(args, format);
  if (log_handler) {
    char *args_msg = NULL;
    if(vasprintf(&args_msg, format, args) > 0) {
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

/*********************************************************************
    Description: Simply consume non-critical extensions for rel > r8
*********************************************************************/
void liblte_rrc_consume_noncrit_extension(bool ext, const char *func_name, uint8 **ie_ptr)
{
  uint32 i=0;
  uint32 elem_flags=0;
  uint32 elem_len=0;

  if (ext) {
    uint8 n_elems = liblte_bits_2_value(ie_ptr, 7) + 1;
    for(i=0; i<n_elems; i++)
    {
      elem_flags |= (liblte_bits_2_value(ie_ptr, 1) << i);
    }
    for(i=0; i<n_elems; i++)
    {
      if(elem_flags & 0x1)
      {
        if(0 == liblte_bits_2_value(ie_ptr, 1))
        {
            elem_len = liblte_bits_2_value(ie_ptr, 7);
        }else{
            if(0 == liblte_bits_2_value(ie_ptr, 1))
            {
                elem_len = liblte_bits_2_value(ie_ptr, 14);
            }else{
                // FIXME: Unlikely to have more than 16K of octets
                elem_len = 0;
            }
        }
        liblte_bits_2_value(ie_ptr, 8*elem_len);
      }
      elem_flags = elem_flags >> 1;
    }
    if (func_name) {
      liblte_rrc_log_print("Detected an extension in RRC function: %s\n", func_name);
    }
  }
}

void liblte_rrc_warning_not_handled(bool opt, const char *func_name) 
{
  if (opt) {
    liblte_rrc_log_print("Unhandled feature in RRC function: %s\n\n", func_name?func_name:"Unknown");
  }
}

/*******************************************************************************
                              INFORMATION ELEMENT FUNCTIONS
*******************************************************************************/

/*********************************************************************
    IE Name: MBSFN Notification Config

    Description: Specifies the MBMS notification related configuration
                 parameters, that are applicable for all MBSFN areas

    Document Reference: 36.331 v10.0.0 Section 6.3.7
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_rrc_pack_mbsfn_notification_config_ie(LIBLTE_RRC_MBSFN_NOTIFICATION_CONFIG_STRUCT  *mbsfn_notification_cnfg,
                                                               uint8                                       **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(mbsfn_notification_cnfg != NULL &&
       ie_ptr                  != NULL)
    {
        liblte_value_2_bits(mbsfn_notification_cnfg->repetition_coeff, ie_ptr, 1);
        liblte_value_2_bits(mbsfn_notification_cnfg->offset,           ie_ptr, 4);
        liblte_value_2_bits(mbsfn_notification_cnfg->sf_index - 1,     ie_ptr, 3);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_unpack_mbsfn_notification_config_ie(uint8                                       **ie_ptr,
                                                                 LIBLTE_RRC_MBSFN_NOTIFICATION_CONFIG_STRUCT  *mbsfn_notification_cnfg)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr                  != NULL &&
       mbsfn_notification_cnfg != NULL)
    {
        mbsfn_notification_cnfg->repetition_coeff = (LIBLTE_RRC_NOTIFICATION_REPETITION_COEFF_R9_ENUM)liblte_bits_2_value(ie_ptr, 1);
        mbsfn_notification_cnfg->offset           = liblte_bits_2_value(ie_ptr, 4);
        mbsfn_notification_cnfg->sf_index         = liblte_bits_2_value(ie_ptr, 3) + 1;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: MBSFN Area Info List

    Description: Contains the information required to acquire the MBMS
                 control information associated with one or more MBSFN
                 areas

    Document Reference: 36.331 v10.0.0 Section 6.3.7
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_rrc_pack_mbsfn_area_info_ie(LIBLTE_RRC_MBSFN_AREA_INFO_STRUCT  *mbsfn_area_info,
                                                     uint8                             **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(mbsfn_area_info != NULL &&
       ie_ptr          != NULL)
    {
        // Extension indicator
        liblte_value_2_bits(0, ie_ptr, 1);

        liblte_value_2_bits(mbsfn_area_info->mbsfn_area_id_r9,            ie_ptr, 8);
        liblte_value_2_bits(mbsfn_area_info->non_mbsfn_region_length,     ie_ptr, 1);
        liblte_value_2_bits(mbsfn_area_info->notification_indicator_r9,   ie_ptr, 3);
        liblte_value_2_bits(mbsfn_area_info->mcch_repetition_period_r9,   ie_ptr, 2);
        liblte_value_2_bits(mbsfn_area_info->mcch_offset_r9,              ie_ptr, 4);
        liblte_value_2_bits(mbsfn_area_info->mcch_modification_period_r9, ie_ptr, 1);
        liblte_value_2_bits(mbsfn_area_info->sf_alloc_info_r9,            ie_ptr, 6);
        liblte_value_2_bits(mbsfn_area_info->signalling_mcs_r9,           ie_ptr, 2);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_unpack_mbsfn_area_info_ie(uint8                             **ie_ptr,
                                                       LIBLTE_RRC_MBSFN_AREA_INFO_STRUCT  *mbsfn_area_info)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;
    bool              ext;

    if(ie_ptr          != NULL &&
       mbsfn_area_info != NULL)
    {
        // Extension indicator
        ext = liblte_bits_2_value(ie_ptr, 1);

        mbsfn_area_info->mbsfn_area_id_r9            = liblte_bits_2_value(ie_ptr, 8);
        mbsfn_area_info->non_mbsfn_region_length     = (LIBLTE_RRC_NON_MBSFN_REGION_LENGTH_ENUM)liblte_bits_2_value(ie_ptr, 1);
        mbsfn_area_info->notification_indicator_r9   = liblte_bits_2_value(ie_ptr, 3);
        mbsfn_area_info->mcch_repetition_period_r9   = (LIBLTE_RRC_MCCH_REPETITION_PERIOD_ENUM)liblte_bits_2_value(ie_ptr, 2);
        mbsfn_area_info->mcch_offset_r9              = liblte_bits_2_value(ie_ptr, 4);
        mbsfn_area_info->mcch_modification_period_r9 = (LIBLTE_RRC_MCCH_MODIFICATION_PERIOD_ENUM)liblte_bits_2_value(ie_ptr, 1);
        mbsfn_area_info->sf_alloc_info_r9            = liblte_bits_2_value(ie_ptr, 6);
        mbsfn_area_info->signalling_mcs_r9           = (LIBLTE_RRC_MCCH_SIGNALLING_MCS_ENUM)liblte_bits_2_value(ie_ptr, 2);

        liblte_rrc_consume_noncrit_extension(ext, __func__, ie_ptr);
        
        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: MBSFN Subframe Config

    Description: Defines subframes that are reserved for MBSFN in
                 downlink

    Document Reference: 36.331 v10.0.0 Section 6.3.7
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_rrc_pack_mbsfn_subframe_config_ie(LIBLTE_RRC_MBSFN_SUBFRAME_CONFIG_STRUCT  *mbsfn_subfr_cnfg,
                                                           uint8                                   **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(mbsfn_subfr_cnfg != NULL &&
       ie_ptr           != NULL)
    {
        liblte_value_2_bits(mbsfn_subfr_cnfg->radio_fr_alloc_period,  ie_ptr, 3);
        liblte_value_2_bits(mbsfn_subfr_cnfg->radio_fr_alloc_offset,  ie_ptr, 3);
        liblte_value_2_bits(mbsfn_subfr_cnfg->subfr_alloc_num_frames, ie_ptr, 1);
        if(LIBLTE_RRC_SUBFRAME_ALLOCATION_NUM_FRAMES_ONE == mbsfn_subfr_cnfg->subfr_alloc_num_frames)
        {
            liblte_value_2_bits(mbsfn_subfr_cnfg->subfr_alloc, ie_ptr, 6);
        }else{
            liblte_value_2_bits(mbsfn_subfr_cnfg->subfr_alloc, ie_ptr, 24);
        }

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_unpack_mbsfn_subframe_config_ie(uint8                                   **ie_ptr,
                                                             LIBLTE_RRC_MBSFN_SUBFRAME_CONFIG_STRUCT  *mbsfn_subfr_cnfg)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr           != NULL &&
       mbsfn_subfr_cnfg != NULL)
    {
        mbsfn_subfr_cnfg->radio_fr_alloc_period  = (LIBLTE_RRC_RADIO_FRAME_ALLOCATION_PERIOD_ENUM)liblte_bits_2_value(ie_ptr, 3);
        mbsfn_subfr_cnfg->radio_fr_alloc_offset  = liblte_bits_2_value(ie_ptr, 3);
        mbsfn_subfr_cnfg->subfr_alloc_num_frames = (LIBLTE_RRC_SUBFRAME_ALLOCATION_NUM_FRAMES_ENUM)liblte_bits_2_value(ie_ptr, 1);
        if(LIBLTE_RRC_SUBFRAME_ALLOCATION_NUM_FRAMES_ONE == mbsfn_subfr_cnfg->subfr_alloc_num_frames)
        {
            mbsfn_subfr_cnfg->subfr_alloc = liblte_bits_2_value(ie_ptr, 6);
        }else{
            mbsfn_subfr_cnfg->subfr_alloc = liblte_bits_2_value(ie_ptr, 24);
        }

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: TMGI

    Description: Temporary Mobile Group Identity (PLMN + MBMS service ID)

    Document Reference: 36.331 v10.0.0 Section 6.3.7
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_rrc_pack_tmgi_r9_ie(LIBLTE_RRC_TMGI_R9_STRUCT *tmgi,
                                             uint8                    **ie_ptr)
{
  LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

  if(tmgi   != NULL &&
     ie_ptr != NULL)
  {
      liblte_value_2_bits(tmgi->plmn_id_explicit?1:0,  ie_ptr, 1);
      if(tmgi->plmn_id_explicit){
          liblte_rrc_pack_plmn_identity_ie(&tmgi->plmn_id_r9, ie_ptr);
      }else{
          liblte_value_2_bits(tmgi->plmn_index_r9-1,  ie_ptr, 3);
      }
      liblte_value_2_bits(tmgi->serviceid_r9,  ie_ptr, 24);

      err = LIBLTE_SUCCESS;
  }

  return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_unpack_tmgi_r9_ie(uint8                    **ie_ptr,
                                               LIBLTE_RRC_TMGI_R9_STRUCT *tmgi)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr != NULL &&
       tmgi   != NULL)
    {
        tmgi->plmn_id_explicit = liblte_bits_2_value(ie_ptr, 1);
        if(tmgi->plmn_id_explicit){
            liblte_rrc_unpack_plmn_identity_ie(ie_ptr, &tmgi->plmn_id_r9);
        }else{
            tmgi->plmn_index_r9 = liblte_bits_2_value(ie_ptr, 3) + 1;
        }
        tmgi->serviceid_r9 = liblte_bits_2_value(ie_ptr, 24);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: MBMS Session Info

    Description: Information about an individual MBMS session

    Document Reference: 36.331 v10.0.0 Section 6.3.7
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_rrc_pack_mbms_session_info_r9_ie(LIBLTE_RRC_MBMS_SESSION_INFO_R9_STRUCT  *mbms_session_info,
                                                          uint8                                  **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(mbms_session_info != NULL &&
       ie_ptr            != NULL)
    {
        // ext
        liblte_value_2_bits(0, ie_ptr, 1);

        liblte_value_2_bits(mbms_session_info->sessionid_r9_present?1:0, ie_ptr, 1);
        liblte_rrc_pack_tmgi_r9_ie(&mbms_session_info->tmgi_r9, ie_ptr);
        if(mbms_session_info->sessionid_r9_present){
            liblte_value_2_bits(mbms_session_info->sessionid_r9, ie_ptr, 8);
        }
        liblte_value_2_bits(mbms_session_info->logicalchannelid_r9, ie_ptr, 5);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_unpack_mbms_session_info_r9_ie(uint8                                  **ie_ptr,
                                                            LIBLTE_RRC_MBMS_SESSION_INFO_R9_STRUCT  *mbms_session_info)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr            != NULL &&
       mbms_session_info != NULL)
    {
        // ext
        bool ext = liblte_bits_2_value(ie_ptr, 1);

        mbms_session_info->sessionid_r9_present = liblte_bits_2_value(ie_ptr, 1);
        liblte_rrc_unpack_tmgi_r9_ie(ie_ptr, &mbms_session_info->tmgi_r9);
        if(mbms_session_info->sessionid_r9_present){
            mbms_session_info->sessionid_r9 = liblte_bits_2_value(ie_ptr, 8);
        }
        mbms_session_info->logicalchannelid_r9 = liblte_bits_2_value(ie_ptr, 5);

        liblte_rrc_consume_noncrit_extension(ext, __func__, ie_ptr);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: PMCH Config

    Description: Contains configuration parameters of the sessions
                 carried by a PMCH

    Document Reference: 36.331 v10.0.0 Section 6.3.7
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_rrc_pack_pmch_config_r9_ie(LIBLTE_RRC_PMCH_CONFIG_R9_STRUCT *pmch_cnfg,
                                                    uint8                           **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(pmch_cnfg != NULL &&
       ie_ptr    != NULL)
    {
        // ext
        liblte_value_2_bits(0, ie_ptr, 1);

        liblte_value_2_bits(pmch_cnfg->sf_alloc_end_r9, ie_ptr, 11);
        liblte_value_2_bits(pmch_cnfg->datamcs_r9, ie_ptr, 5);
        liblte_value_2_bits(pmch_cnfg->mch_schedulingperiod_r9, ie_ptr, 3);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_unpack_pmch_config_r9_ie(uint8                           **ie_ptr,
                                                      LIBLTE_RRC_PMCH_CONFIG_R9_STRUCT *pmch_cnfg)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr    != NULL &&
       pmch_cnfg != NULL)
    {
        // ext
        bool ext = liblte_bits_2_value(ie_ptr, 1);

        pmch_cnfg->sf_alloc_end_r9          = liblte_bits_2_value(ie_ptr, 11);
        pmch_cnfg->datamcs_r9               = liblte_bits_2_value(ie_ptr, 5);
        pmch_cnfg->mch_schedulingperiod_r9  = (LIBLTE_RRC_MCH_SCHEDULING_PERIOD_R9_ENUM)liblte_bits_2_value(ie_ptr, 3);

        liblte_rrc_consume_noncrit_extension(ext, __func__, ie_ptr);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}


/*********************************************************************
    IE Name: PMCH Info

    Description: Specifies configuration of PMCH of an MBSFN area

    Document Reference: 36.331 v10.0.0 Section 6.3.7
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_rrc_pack_pmch_info_r9_ie(LIBLTE_RRC_PMCH_INFO_R9_STRUCT  *pmch_info,
                                                  uint8                          **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;
    uint32 i;

    if(pmch_info != NULL &&
       ie_ptr    != NULL)
    {
        // ext
        liblte_value_2_bits(0, ie_ptr, 1);

        liblte_rrc_pack_pmch_config_r9_ie(&pmch_info->pmch_config_r9, ie_ptr);
        liblte_value_2_bits(pmch_info->mbms_sessioninfolist_r9_size, ie_ptr, 5);
        for(i=0; i<pmch_info->mbms_sessioninfolist_r9_size; i++){
          liblte_rrc_pack_mbms_session_info_r9_ie(&pmch_info->mbms_sessioninfolist_r9[i], ie_ptr);
        }

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_unpack_pmch_info_r9_ie(uint8                          **ie_ptr,
                                                    LIBLTE_RRC_PMCH_INFO_R9_STRUCT  *pmch_info)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;
    uint32 i;

    if(ie_ptr    != NULL &&
       pmch_info != NULL)
    {
        // ext
        bool ext = liblte_bits_2_value(ie_ptr, 1);

        liblte_rrc_unpack_pmch_config_r9_ie(ie_ptr, &pmch_info->pmch_config_r9);
        pmch_info->mbms_sessioninfolist_r9_size = liblte_bits_2_value(ie_ptr, 5);
        for(i=0; i<pmch_info->mbms_sessioninfolist_r9_size; i++){
          liblte_rrc_unpack_mbms_session_info_r9_ie(ie_ptr, &pmch_info->mbms_sessioninfolist_r9[i]);
        }

        liblte_rrc_consume_noncrit_extension(ext, __func__, ie_ptr);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: C-RNTI

    Description: Identifies a UE having a RRC connection within a cell

    Document Reference: 36.331 v10.0.0 Section 6.3.6
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_rrc_pack_c_rnti_ie(uint16   rnti,
                                            uint8  **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr != NULL)
    {
        liblte_value_2_bits(rnti, ie_ptr, 16);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_unpack_c_rnti_ie(uint8  **ie_ptr,
                                              uint16  *rnti)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr != NULL &&
       rnti   != NULL)
    {
        *rnti = liblte_bits_2_value(ie_ptr, 16);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: Dedicated Info CDMA2000

    Description: Transfers UE specific CDMA2000 information between
                 the network and the UE

    Document Reference: 36.331 v10.0.0 Section 6.3.6
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_rrc_pack_dedicated_info_cdma2000_ie(LIBLTE_BYTE_MSG_STRUCT  *ded_info_cdma2000,
                                                             uint8                         **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;
    uint32            i;

    if(ded_info_cdma2000 != NULL &&
       ie_ptr            != NULL)
    {
        if(ded_info_cdma2000->N_bytes < 128)
        {
            liblte_value_2_bits(0,                          ie_ptr, 1);
            liblte_value_2_bits(ded_info_cdma2000->N_bytes, ie_ptr, 7);
        }else if(ded_info_cdma2000->N_bytes < 16383){
            liblte_value_2_bits(1,                          ie_ptr, 1);
            liblte_value_2_bits(0,                          ie_ptr, 1);
            liblte_value_2_bits(ded_info_cdma2000->N_bytes, ie_ptr, 14);
        }else{
            // FIXME: Unlikely to have more than 16K of octets
        }

        for(i=0; i<ded_info_cdma2000->N_bytes; i++)
        {
            liblte_value_2_bits(ded_info_cdma2000->msg[i], ie_ptr, 8);
        }

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_unpack_dedicated_info_cdma2000_ie(uint8                         **ie_ptr,
                                                               LIBLTE_BYTE_MSG_STRUCT  *ded_info_cdma2000)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;
    uint32            i;

    if(ie_ptr            != NULL &&
       ded_info_cdma2000 != NULL)
    {
        if(0 == liblte_bits_2_value(ie_ptr, 1))
        {
            ded_info_cdma2000->N_bytes = liblte_bits_2_value(ie_ptr, 7);
        }else{
            if(0 == liblte_bits_2_value(ie_ptr, 1))
            {
                ded_info_cdma2000->N_bytes = liblte_bits_2_value(ie_ptr, 14);
            }else{
                // FIXME: Unlikely to have more than 16K of octets
                ded_info_cdma2000->N_bytes = 0;
            }
        }

        for(i=0; i<ded_info_cdma2000->N_bytes; i++)
        {
            ded_info_cdma2000->msg[i] = liblte_bits_2_value(ie_ptr, 8);
        }

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: Dedicated Info NAS

    Description: Transfers UE specific NAS layer information between
                 the network and the UE

    Document Reference: 36.331 v10.0.0 Section 6.3.6
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_rrc_pack_dedicated_info_nas_ie(LIBLTE_BYTE_MSG_STRUCT  *ded_info_nas,
                                                        uint8                         **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;
    uint32            i;

    if(ded_info_nas != NULL &&
       ie_ptr       != NULL)
    {
        if(ded_info_nas->N_bytes < 128)
        {
            liblte_value_2_bits(0,                     ie_ptr, 1);
            liblte_value_2_bits(ded_info_nas->N_bytes, ie_ptr, 7);
        }else if(ded_info_nas->N_bytes < 16383){
            liblte_value_2_bits(1,                     ie_ptr, 1);
            liblte_value_2_bits(0,                     ie_ptr, 1);
            liblte_value_2_bits(ded_info_nas->N_bytes, ie_ptr, 14);
        }else{
            // FIXME: Unlikely to have more than 16K of octets
        }

        for(i=0; i<ded_info_nas->N_bytes; i++)
        {
            liblte_value_2_bits(ded_info_nas->msg[i], ie_ptr, 8);
        }

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_unpack_dedicated_info_nas_ie(uint8                         **ie_ptr,
                                                          LIBLTE_BYTE_MSG_STRUCT  *ded_info_nas)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;
    uint32            i;

    if(ie_ptr       != NULL &&
       ded_info_nas != NULL)
    {
        if(0 == liblte_bits_2_value(ie_ptr, 1))
        {
            ded_info_nas->N_bytes = liblte_bits_2_value(ie_ptr, 7);
        }else{
            if(0 == liblte_bits_2_value(ie_ptr, 1))
            {
                ded_info_nas->N_bytes = liblte_bits_2_value(ie_ptr, 14);
            }else{
                // FIXME: Unlikely to have more than 16K of octets
                ded_info_nas->N_bytes = 0;
            }
        }

        for(i=0; i<ded_info_nas->N_bytes; i++)
        {
            ded_info_nas->msg[i] = liblte_bits_2_value(ie_ptr, 8);
        }

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: Filter Coefficient

    Description: Specifies the measurement filtering coefficient

    Document Reference: 36.331 v10.0.0 Section 6.3.6
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_rrc_pack_filter_coefficient_ie(LIBLTE_RRC_FILTER_COEFFICIENT_ENUM   filter_coeff,
                                                        uint8                              **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr != NULL)
    {
        // Extension indicator
        liblte_value_2_bits(0, ie_ptr, 1);

        liblte_value_2_bits(filter_coeff, ie_ptr, 4);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_unpack_filter_coefficient_ie(uint8                              **ie_ptr,
                                                          LIBLTE_RRC_FILTER_COEFFICIENT_ENUM  *filter_coeff)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr       != NULL &&
       filter_coeff != NULL)
    {
        // Extension indicator
        bool ext = liblte_bits_2_value(ie_ptr, 1);

        *filter_coeff = (LIBLTE_RRC_FILTER_COEFFICIENT_ENUM)liblte_bits_2_value(ie_ptr, 4);

        liblte_rrc_consume_noncrit_extension(ext, __func__, ie_ptr);
        
        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: MMEC

    Description: Identifies an MME within the scope of an MME group
                 within a PLMN

    Document Reference: 36.331 v10.0.0 Section 6.3.6
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_rrc_pack_mmec_ie(uint8   mmec,
                                          uint8 **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr != NULL)
    {
        liblte_value_2_bits(mmec, ie_ptr, 8);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_unpack_mmec_ie(uint8 **ie_ptr,
                                            uint8  *mmec)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr != NULL &&
       mmec   != NULL)
    {
        *mmec = liblte_bits_2_value(ie_ptr, 8);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: Neigh Cell Config

    Description: Provides the information related to MBSFN and TDD
                 UL/DL configuration of neighbor cells

    Document Reference: 36.331 v10.0.0 Section 6.3.6
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_rrc_pack_neigh_cell_config_ie(uint8   neigh_cell_config,
                                                       uint8 **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr != NULL)
    {
        liblte_value_2_bits(neigh_cell_config, ie_ptr, 2);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_unpack_neigh_cell_config_ie(uint8 **ie_ptr,
                                                         uint8  *neigh_cell_config)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr            != NULL &&
       neigh_cell_config != NULL)
    {
        *neigh_cell_config = liblte_bits_2_value(ie_ptr, 2);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: Other Config

    Description: Contains configuration related to other configuration

    Document Reference: 36.331 v10.0.0 Section 6.3.6
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_rrc_pack_other_config_ie(LIBLTE_RRC_OTHER_CONFIG_R9_STRUCT  *other_cnfg,
                                                  uint8                             **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(other_cnfg != NULL &&
       ie_ptr     != NULL)
    {
        // Extension indicator
        liblte_value_2_bits(0, ie_ptr, 1);

        // Optional indicator
        liblte_value_2_bits(other_cnfg->report_proximity_cnfg_present, ie_ptr, 1);

        if(true == other_cnfg->report_proximity_cnfg_present)
        {
            // Optional indicators
            liblte_value_2_bits(other_cnfg->report_proximity_cnfg.report_proximity_ind_eutra_present, ie_ptr, 1);
            liblte_value_2_bits(other_cnfg->report_proximity_cnfg.report_proximity_ind_utra_present,  ie_ptr, 1);

            if(true == other_cnfg->report_proximity_cnfg.report_proximity_ind_eutra_present)
            {
                liblte_value_2_bits(other_cnfg->report_proximity_cnfg.report_proximity_ind_eutra, ie_ptr, 1);
            }

            if(true == other_cnfg->report_proximity_cnfg.report_proximity_ind_utra_present)
            {
                liblte_value_2_bits(other_cnfg->report_proximity_cnfg.report_proximity_ind_utra, ie_ptr, 1);
            }
        }

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_unpack_other_config_ie(uint8                             **ie_ptr,
                                                    LIBLTE_RRC_OTHER_CONFIG_R9_STRUCT  *other_cnfg)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr     != NULL &&
       other_cnfg != NULL)
    {
        // Extension indicator
        bool ext = liblte_bits_2_value(ie_ptr, 1);

        // Optional indicator
        other_cnfg->report_proximity_cnfg_present = liblte_bits_2_value(ie_ptr, 1);

        if(true == other_cnfg->report_proximity_cnfg_present)
        {
            // Optional indicators
            other_cnfg->report_proximity_cnfg.report_proximity_ind_eutra_present = liblte_bits_2_value(ie_ptr, 1);
            other_cnfg->report_proximity_cnfg.report_proximity_ind_utra_present  = liblte_bits_2_value(ie_ptr, 1);

            if(true == other_cnfg->report_proximity_cnfg.report_proximity_ind_eutra_present)
            {
                other_cnfg->report_proximity_cnfg.report_proximity_ind_eutra = (LIBLTE_RRC_REPORT_PROXIMITY_INDICATION_EUTRA_R9_ENUM)liblte_bits_2_value(ie_ptr, 1);
            }

            if(true == other_cnfg->report_proximity_cnfg.report_proximity_ind_utra_present)
            {
                other_cnfg->report_proximity_cnfg.report_proximity_ind_utra = (LIBLTE_RRC_REPORT_PROXIMITY_INDICATION_UTRA_R9_ENUM)liblte_bits_2_value(ie_ptr, 1);
            }
        }
        
        liblte_rrc_consume_noncrit_extension(ext, __func__, ie_ptr);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: RAND CDMA2000 (1xRTT)

    Description: Contains a random value, generated by the eNB, to be
                 passed to the CDMA2000 upper layers

    Document Reference: 36.331 v10.0.0 Section 6.3.6
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_rrc_pack_rand_cdma2000_1xrtt_ie(uint32   rand,
                                                         uint8  **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr != NULL)
    {
        liblte_value_2_bits(rand, ie_ptr, 32);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_unpack_rand_cdma2000_1xrtt_ie(uint8  **ie_ptr,
                                                           uint32  *rand)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr != NULL &&
       rand   != NULL)
    {
        *rand = liblte_bits_2_value(ie_ptr, 32);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: RAT Type

    Description: Indicates the radio access technology (RAT),
                 including E-UTRA, of the requested/transferred UE
                 capabilities

    Document Reference: 36.331 v10.0.0 Section 6.3.6
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_rrc_pack_rat_type_ie(LIBLTE_RRC_RAT_TYPE_ENUM   rat_type,
                                              uint8                    **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr != NULL)
    {
        // Extension indicator
        liblte_value_2_bits(0, ie_ptr, 1);

        liblte_value_2_bits(rat_type, ie_ptr, 3);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_unpack_rat_type_ie(uint8                    **ie_ptr,
                                                LIBLTE_RRC_RAT_TYPE_ENUM  *rat_type)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr   != NULL &&
       rat_type != NULL)
    {
        // Extension indicator
        bool ext = liblte_bits_2_value(ie_ptr, 1);

        *rat_type = (LIBLTE_RRC_RAT_TYPE_ENUM)liblte_bits_2_value(ie_ptr, 3);

        liblte_rrc_consume_noncrit_extension(ext, __func__, ie_ptr);
        
        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: RRC Transaction Identifier

    Description: Identifies an RRC procedure along with the message
                 type

    Document Reference: 36.331 v10.0.0 Section 6.3.6
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_rrc_pack_rrc_transaction_identifier_ie(uint8   rrc_transaction_id,
                                                                uint8 **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr != NULL)
    {
        liblte_value_2_bits(rrc_transaction_id, ie_ptr, 2);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_unpack_rrc_transaction_identifier_ie(uint8 **ie_ptr,
                                                                  uint8  *rrc_transaction_id)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr             != NULL &&
       rrc_transaction_id != NULL)
    {
        *rrc_transaction_id = liblte_bits_2_value(ie_ptr, 2);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: S-TMSI

    Description: Contains an S-Temporary Mobile Subscriber Identity,
                 a temporary UE identity provided by the EPC which
                 uniquely identifies the UE within the tracking area

    Document Reference: 36.331 v10.0.0 Section 6.3.6
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_rrc_pack_s_tmsi_ie(LIBLTE_RRC_S_TMSI_STRUCT  *s_tmsi,
                                            uint8                    **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(s_tmsi != NULL &&
       ie_ptr != NULL)
    {
        liblte_rrc_pack_mmec_ie(s_tmsi->mmec, ie_ptr);
        liblte_value_2_bits(s_tmsi->m_tmsi, ie_ptr, 32);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_unpack_s_tmsi_ie(uint8                    **ie_ptr,
                                              LIBLTE_RRC_S_TMSI_STRUCT  *s_tmsi)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr != NULL &&
       s_tmsi != NULL)
    {
        liblte_rrc_unpack_mmec_ie(ie_ptr, &s_tmsi->mmec);
        s_tmsi->m_tmsi = liblte_bits_2_value(ie_ptr, 32);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: UE Capability RAT Container List

    Description: Contains list of containers, one for each RAT for
                 which UE capabilities are transferred

    Document Reference: 36.331 v10.0.0 Section 6.3.6
*********************************************************************/
// FIXME

/*********************************************************************
    IE Name: UE EUTRA Capability

    Description: Conveys the E-UTRA UE Radio Access Capability
                 Parameters

    Document Reference: 36.331 v10.0.0 Section 6.3.6
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_rrc_pack_pdcp_params_ie(LIBLTE_RRC_PDCP_PARAMS_STRUCT  *pdcp_params,
                                                 uint8                         **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;
    uint32            i;

    if(pdcp_params != NULL &&
       ie_ptr          != NULL)
    {
        // Extension indicator
        liblte_value_2_bits(0, ie_ptr, 1);

        // Max ROHC CTXTS default?
        liblte_value_2_bits(pdcp_params->max_rohc_ctxts_present, ie_ptr, 1);

        // Supported ROHC Profiles
        for(i=0; i<9; i++)
        {
            liblte_value_2_bits(pdcp_params->supported_rohc_profiles[i], ie_ptr, 1);
        }

        if(pdcp_params->max_rohc_ctxts_present)
        {
            liblte_value_2_bits(pdcp_params->max_rohc_ctxts, ie_ptr, 4);
        }
        
        err = LIBLTE_SUCCESS;
    }

    return(err);
}

LIBLTE_ERROR_ENUM liblte_rrc_unpack_pdcp_params_ie(uint8                         **ie_ptr,
                                                   LIBLTE_RRC_PDCP_PARAMS_STRUCT  *pdcp_params)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;
    uint32            i;

    if(ie_ptr          != NULL &&
       pdcp_params != NULL)
    {
        // Extension indicator
        bool ext = liblte_bits_2_value(ie_ptr, 1);

        // Max ROHC CTXTS default?
        pdcp_params->max_rohc_ctxts_present = liblte_bits_2_value(ie_ptr, 1);

        // Supported ROHC Profiles
        for(i=0; i<9; i++)
        {
            pdcp_params->supported_rohc_profiles[i] = liblte_bits_2_value(ie_ptr, 1);
        }

        if(pdcp_params->max_rohc_ctxts_present)
        {
            pdcp_params->max_rohc_ctxts = (LIBLTE_RRC_MAX_ROHC_CTXTS_ENUM)liblte_bits_2_value(ie_ptr, 4);
        }
        
        liblte_rrc_consume_noncrit_extension(ext, __func__, ie_ptr);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

LIBLTE_ERROR_ENUM liblte_rrc_pack_phy_layer_params_ie(LIBLTE_RRC_PHY_LAYER_PARAMS_STRUCT  *params,
                                                      uint8                              **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(params != NULL &&
       ie_ptr          != NULL)
    {
        liblte_value_2_bits(params->tx_antenna_selection_supported, ie_ptr, 1);
        liblte_value_2_bits(params->specific_ref_sigs_supported,    ie_ptr, 1);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

LIBLTE_ERROR_ENUM liblte_rrc_unpack_phy_layer_params_ie(uint8                              **ie_ptr,
                                                        LIBLTE_RRC_PHY_LAYER_PARAMS_STRUCT  *params)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr          != NULL &&
       params != NULL)
    {
        params->tx_antenna_selection_supported = liblte_bits_2_value(ie_ptr, 1);
        params->specific_ref_sigs_supported    = liblte_bits_2_value(ie_ptr, 1);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

LIBLTE_ERROR_ENUM liblte_rrc_pack_rf_params_ie(LIBLTE_RRC_RF_PARAMS_STRUCT  *params,
                                               uint8                       **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;
    uint32            i;

    if(params != NULL &&
       ie_ptr          != NULL)
    {
        liblte_value_2_bits(params->N_supported_band_eutras-1, ie_ptr, 6);
        for(i=0; i<params->N_supported_band_eutras; i++)
        {
            liblte_value_2_bits(params->supported_band_eutra[i].band_eutra-1, ie_ptr, 6);
            liblte_value_2_bits(params->supported_band_eutra[i].half_duplex, ie_ptr, 1);
        }

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

LIBLTE_ERROR_ENUM liblte_rrc_unpack_rf_params_ie(uint8                       **ie_ptr,
                                                 LIBLTE_RRC_RF_PARAMS_STRUCT  *params)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;
    uint32            i;

    if(ie_ptr          != NULL &&
       params != NULL)
    {
        params->N_supported_band_eutras = liblte_bits_2_value(ie_ptr, 6) + 1;
        for(i=0; i<params->N_supported_band_eutras; i++)
        {
            params->supported_band_eutra[i].band_eutra   = liblte_bits_2_value(ie_ptr, 6) + 1;
            params->supported_band_eutra[i].half_duplex  = liblte_bits_2_value(ie_ptr, 1);
        }

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

LIBLTE_ERROR_ENUM liblte_rrc_pack_band_info_eutra_ie(LIBLTE_RRC_BAND_INFO_EUTRA_STRUCT  *info,
                                                     uint8                             **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;
    uint32            i;

    if(info   != NULL &&
       ie_ptr != NULL)
    {
        // Option indicator
        liblte_value_2_bits(0, ie_ptr, 1);

        liblte_value_2_bits(info->N_inter_freq_need_for_gaps-1, ie_ptr, 6);
        for(i=0; i<info->N_inter_freq_need_for_gaps; i++)
        {
            liblte_value_2_bits(info->inter_freq_need_for_gaps[i], ie_ptr, 1);
        }

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

LIBLTE_ERROR_ENUM liblte_rrc_unpack_band_info_eutra_ie(uint8                             **ie_ptr,
                                                       LIBLTE_RRC_BAND_INFO_EUTRA_STRUCT  *info)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;
    uint32            i;

    if(ie_ptr != NULL &&
       info   != NULL)
    {
        // Option indicator
        liblte_bits_2_value(ie_ptr, 1);

        info->N_inter_freq_need_for_gaps = liblte_bits_2_value(ie_ptr, 6) + 1;
        for(i=0; i<info->N_inter_freq_need_for_gaps; i++)
        {
            info->inter_freq_need_for_gaps[i] = liblte_bits_2_value(ie_ptr, 1);
        }

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

LIBLTE_ERROR_ENUM liblte_rrc_pack_meas_params_ie(LIBLTE_RRC_MEAS_PARAMS_STRUCT  *params,
                                                 uint8                         **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;
    uint32            i;

    if(params   != NULL &&
       ie_ptr   != NULL)
    {
        liblte_value_2_bits(params->N_band_list_eutra-1, ie_ptr, 6);
        for(i=0; i<params->N_band_list_eutra; i++)
        {
            liblte_rrc_pack_band_info_eutra_ie(&params->band_list_eutra[i], ie_ptr);
        }

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

LIBLTE_ERROR_ENUM liblte_rrc_unpack_meas_params_ie(uint8                         **ie_ptr,
                                                   LIBLTE_RRC_MEAS_PARAMS_STRUCT  *params)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;
    uint32            i;

    if(ie_ptr   != NULL &&
       params   != NULL)
    {
        params->N_band_list_eutra = liblte_bits_2_value(ie_ptr, 6) + 1;
        for(i=0; i<params->N_band_list_eutra; i++)
        {
            liblte_rrc_unpack_band_info_eutra_ie(ie_ptr, &params->band_list_eutra[i]);
        }

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

LIBLTE_ERROR_ENUM liblte_rrc_pack_inter_rat_params_ie(LIBLTE_RRC_INTER_RAT_PARAMS_STRUCT  *params,
                                                      uint8                              **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(params   != NULL &&
       ie_ptr   != NULL)
    {
        // WARNING: Hardcoding all options to not present
        liblte_value_2_bits(0, ie_ptr, 7);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_unpack_inter_rat_params_ie(uint8                              **ie_ptr,
                                                        LIBLTE_RRC_INTER_RAT_PARAMS_STRUCT  *params)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;
    uint32            i;

    if(ie_ptr   != NULL &&
       params   != NULL)
    {
        // WARNING: Hardcoding all options to not present
        params->utra_fdd_present       = false;
        params->utra_tdd128_present    = false;
        params->utra_tdd384_present    = false;
        params->utra_tdd768_present    = false;
        params->geran_present          = false;
        params->cdma2000_hrpd_present  = false;
        params->cdma2000_1xrtt_present = false;
        err = LIBLTE_SUCCESS;
    }

    return(err);
}

LIBLTE_ERROR_ENUM liblte_rrc_pack_ue_eutra_capability_ie(LIBLTE_RRC_UE_EUTRA_CAPABILITY_STRUCT *ue_eutra_capability,
                                                         LIBLTE_BIT_MSG_STRUCT                 *msg)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr;

    if(ue_eutra_capability   != NULL &&
       msg                   != NULL)
    {
        msg_ptr = msg->msg;

        // Option indicator - featureGroupIndicators
        liblte_value_2_bits(ue_eutra_capability->feature_group_indicator_present, &msg_ptr, 1);

        // Option indicator - nonCriticalExtension
        liblte_value_2_bits(0, &msg_ptr, 1);

        // Option indicator - access stratum release enum
        liblte_value_2_bits(0, &msg_ptr, 1);
        liblte_value_2_bits(ue_eutra_capability->access_stratum_release,      &msg_ptr, 3);

        liblte_value_2_bits(ue_eutra_capability->ue_category - 1,             &msg_ptr, 3);
        liblte_rrc_pack_pdcp_params_ie(&ue_eutra_capability->pdcp_params,     &msg_ptr);
        liblte_rrc_pack_phy_layer_params_ie(&ue_eutra_capability->phy_params, &msg_ptr);
        liblte_rrc_pack_rf_params_ie(&ue_eutra_capability->rf_params,         &msg_ptr);
        liblte_rrc_pack_meas_params_ie(&ue_eutra_capability->meas_params,     &msg_ptr);
        if(ue_eutra_capability->feature_group_indicator_present)
          liblte_value_2_bits(ue_eutra_capability->feature_group_indicator, &msg_ptr, 32);
        liblte_rrc_pack_inter_rat_params_ie(&ue_eutra_capability->inter_rat_params, &msg_ptr);

        // Fill in the number of bits used
        msg->N_bits = msg_ptr - msg->msg;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_unpack_ue_eutra_capability_ie(uint8                                **ie_ptr,
                                                           LIBLTE_RRC_UE_EUTRA_CAPABILITY_STRUCT *ue_eutra_capability)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;

    if(ue_eutra_capability   != NULL &&
       ie_ptr                != NULL)
    {
        // Option indicator - featureGroupIndicators
        ue_eutra_capability->feature_group_indicator_present = liblte_bits_2_value(ie_ptr, 1);

        // Option indicator - nonCriticalExtension
        bool ext = liblte_bits_2_value(ie_ptr, 1);

        // Option indicator - access stratum release enum
        liblte_bits_2_value(ie_ptr, 1);
        ue_eutra_capability->access_stratum_release = liblte_bits_2_value(ie_ptr, 3);

        ue_eutra_capability->ue_category            = liblte_bits_2_value(ie_ptr, 3) + 1;
        liblte_rrc_unpack_pdcp_params_ie(ie_ptr, &ue_eutra_capability->pdcp_params);
        liblte_rrc_unpack_phy_layer_params_ie(ie_ptr, &ue_eutra_capability->phy_params);
        liblte_rrc_unpack_rf_params_ie(ie_ptr, &ue_eutra_capability->rf_params);
        liblte_rrc_unpack_meas_params_ie(ie_ptr, &ue_eutra_capability->meas_params);
        if(ue_eutra_capability->feature_group_indicator_present)
          ue_eutra_capability->feature_group_indicator = liblte_bits_2_value(ie_ptr, 32);
        liblte_rrc_unpack_inter_rat_params_ie(ie_ptr, &ue_eutra_capability->inter_rat_params);

        liblte_rrc_consume_noncrit_extension(ext, __func__, ie_ptr);
        
        err = LIBLTE_SUCCESS;
    }

    return(err);
}



/*********************************************************************
    IE Name: UE Timers and Constants

    Description: Contains timers and constants used by the UE in
                 either RRC_CONNECTED or RRC_IDLE

    Document Reference: 36.331 v10.0.0 Section 6.3.6
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_rrc_pack_ue_timers_and_constants_ie(LIBLTE_RRC_UE_TIMERS_AND_CONSTANTS_STRUCT  *ue_timers_and_constants,
                                                             uint8                                     **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ue_timers_and_constants != NULL &&
       ie_ptr                  != NULL)
    {
        // Extension indicator
        liblte_value_2_bits(0, ie_ptr, 1);

        liblte_value_2_bits(ue_timers_and_constants->t300, ie_ptr, 3);
        liblte_value_2_bits(ue_timers_and_constants->t301, ie_ptr, 3);
        liblte_value_2_bits(ue_timers_and_constants->t310, ie_ptr, 3);
        liblte_value_2_bits(ue_timers_and_constants->n310, ie_ptr, 3);
        liblte_value_2_bits(ue_timers_and_constants->t311, ie_ptr, 3);
        liblte_value_2_bits(ue_timers_and_constants->n311, ie_ptr, 3);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_unpack_ue_timers_and_constants_ie(uint8                                     **ie_ptr,
                                                               LIBLTE_RRC_UE_TIMERS_AND_CONSTANTS_STRUCT  *ue_timers_and_constants)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr                  != NULL &&
       ue_timers_and_constants != NULL)
    {
        // Extension indicator
        bool ext = liblte_bits_2_value(ie_ptr, 1);

        ue_timers_and_constants->t300 = (LIBLTE_RRC_T300_ENUM)liblte_bits_2_value(ie_ptr, 3);
        ue_timers_and_constants->t301 = (LIBLTE_RRC_T301_ENUM)liblte_bits_2_value(ie_ptr, 3);
        ue_timers_and_constants->t310 = (LIBLTE_RRC_T310_ENUM)liblte_bits_2_value(ie_ptr, 3);
        ue_timers_and_constants->n310 = (LIBLTE_RRC_N310_ENUM)liblte_bits_2_value(ie_ptr, 3);
        ue_timers_and_constants->t311 = (LIBLTE_RRC_T311_ENUM)liblte_bits_2_value(ie_ptr, 3);
        ue_timers_and_constants->n311 = (LIBLTE_RRC_N311_ENUM)liblte_bits_2_value(ie_ptr, 3);

        liblte_rrc_consume_noncrit_extension(ext, __func__, ie_ptr);
        
        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: Allowed Meas Bandwidth

    Description: Indicates the maximum allowed measurement bandwidth
                 on a carrier frequency as defined by the parameter
                 Transmission Bandwidth Configuration

    Document Reference: 36.331 v10.0.0 Section 6.3.5
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_rrc_pack_allowed_meas_bandwidth_ie(LIBLTE_RRC_ALLOWED_MEAS_BANDWIDTH_ENUM   allowed_meas_bw,
                                                            uint8                                  **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr != NULL)
    {
        liblte_value_2_bits(allowed_meas_bw, ie_ptr, 3);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_unpack_allowed_meas_bandwidth_ie(uint8                                  **ie_ptr,
                                                              LIBLTE_RRC_ALLOWED_MEAS_BANDWIDTH_ENUM  *allowed_meas_bw)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr          != NULL &&
       allowed_meas_bw != NULL)
    {
        *allowed_meas_bw = (LIBLTE_RRC_ALLOWED_MEAS_BANDWIDTH_ENUM)liblte_bits_2_value(ie_ptr, 3);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: Hysteresis

    Description: Used within the entry and leave condition of an
                 event triggered reporting condition

    Document Reference: 36.331 v10.0.0 Section 6.3.5
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_rrc_pack_hysteresis_ie(uint8   hysteresis,
                                                uint8 **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr != NULL)
    {
        // FIXME: Convert from actual value
        liblte_value_2_bits(hysteresis, ie_ptr, 5);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_unpack_hysteresis_ie(uint8 **ie_ptr,
                                                  uint8  *hysteresis)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr     != NULL &&
       hysteresis != NULL)
    {
        // FIXME: Convert to actual value
        *hysteresis = liblte_bits_2_value(ie_ptr, 5);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: Location Info

    Description: Transfers location information available at the UE to
                 correlate measurements and UE position information

    Document Reference: 36.331 v10.0.0 Section 6.3.5
*********************************************************************/
// FIXME

/*********************************************************************
    IE Name: Meas Config

    Description: Specifies measurements to be performed by the UE,
                 and covers intra-frequency, inter-frequency and
                 inter-RAT mobility as well as configuration of
                 measurement gaps

    Document Reference: 36.331 v10.0.0 Section 6.3.5
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_rrc_pack_meas_config_ie(LIBLTE_RRC_MEAS_CONFIG_STRUCT  *meas_cnfg,
                                                 uint8                         **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;
    uint32            i;

    if(meas_cnfg != NULL &&
       ie_ptr    != NULL)
    {
        // Extension indicator
        liblte_value_2_bits(0, ie_ptr, 1);

        // Optional indicators
        if(meas_cnfg->N_meas_obj_to_remove)
        {
            liblte_value_2_bits(1, ie_ptr, 1);
        }else{
            liblte_value_2_bits(0, ie_ptr, 1);
        }
        liblte_value_2_bits(meas_cnfg->meas_obj_to_add_mod_list_present, ie_ptr, 1);
        if(meas_cnfg->N_rep_cnfg_to_remove)
        {
            liblte_value_2_bits(1, ie_ptr, 1);
        }else{
            liblte_value_2_bits(0, ie_ptr, 1);
        }
        liblte_value_2_bits(meas_cnfg->rep_cnfg_to_add_mod_list_present, ie_ptr, 1);
        if(meas_cnfg->N_meas_id_to_remove)
        {
            liblte_value_2_bits(1, ie_ptr, 1);
        }else{
            liblte_value_2_bits(0, ie_ptr, 1);
        }
        liblte_value_2_bits(meas_cnfg->meas_id_to_add_mod_list_present, ie_ptr, 1);
        liblte_value_2_bits(meas_cnfg->quantity_cnfg_present,           ie_ptr, 1);
        liblte_value_2_bits(meas_cnfg->meas_gap_cnfg_present,           ie_ptr, 1);
        liblte_value_2_bits(meas_cnfg->s_meas_present,                  ie_ptr, 1);
        liblte_value_2_bits(meas_cnfg->pre_reg_info_hrpd_present,       ie_ptr, 1);
        liblte_value_2_bits(meas_cnfg->speed_state_params_present,      ie_ptr, 1);

        // Meas Object To Remove List
        if(0 != meas_cnfg->N_meas_obj_to_remove)
        {
            liblte_value_2_bits(meas_cnfg->N_meas_obj_to_remove - 1, ie_ptr, 5);
        }
        for(i=0; i<meas_cnfg->N_meas_obj_to_remove; i++)
        {
            liblte_rrc_pack_meas_object_id_ie(meas_cnfg->meas_obj_to_remove_list[i], ie_ptr);
        }

        // Meas Object To Add Mod List
        if(meas_cnfg->meas_obj_to_add_mod_list_present)
        {
            liblte_rrc_pack_meas_object_to_add_mod_list_ie(&meas_cnfg->meas_obj_to_add_mod_list, ie_ptr);
        }

        // Report Config To Remove List
        if(0 != meas_cnfg->N_rep_cnfg_to_remove)
        {
            liblte_value_2_bits(meas_cnfg->N_rep_cnfg_to_remove - 1, ie_ptr, 5);
        }
        for(i=0; i<meas_cnfg->N_rep_cnfg_to_remove; i++)
        {
            liblte_rrc_pack_report_config_id_ie(meas_cnfg->rep_cnfg_to_remove_list[i], ie_ptr);
        }

        // Report Config To Add Mod List
        if(meas_cnfg->rep_cnfg_to_add_mod_list_present)
        {
            liblte_rrc_pack_report_config_to_add_mod_list_ie(&meas_cnfg->rep_cnfg_to_add_mod_list, ie_ptr);
        }

        // Meas ID To Remove List
        if(0 != meas_cnfg->N_meas_id_to_remove)
        {
            liblte_value_2_bits(meas_cnfg->N_meas_id_to_remove - 1, ie_ptr, 5);
        }
        for(i=0; i<meas_cnfg->N_meas_id_to_remove; i++)
        {
            liblte_rrc_pack_meas_id_ie(meas_cnfg->meas_id_to_remove_list[i], ie_ptr);
        }

        // Meas ID To Add Mod List
        if(meas_cnfg->meas_id_to_add_mod_list_present)
        {
            liblte_rrc_pack_meas_id_to_add_mod_list_ie(&meas_cnfg->meas_id_to_add_mod_list, ie_ptr);
        }

        // Quantity Config
        if(meas_cnfg->quantity_cnfg_present)
        {
            liblte_rrc_pack_quantity_config_ie(&meas_cnfg->quantity_cnfg, ie_ptr);
        }

        // Meas Gap Config
        if(meas_cnfg->meas_gap_cnfg_present)
        {
            liblte_rrc_pack_meas_gap_config_ie(&meas_cnfg->meas_gap_cnfg, ie_ptr);
        }

        // S Measure
        if(meas_cnfg->s_meas_present)
        {
            liblte_rrc_pack_rsrp_range_ie(meas_cnfg->s_meas, ie_ptr);
        }

        // Pre Registration Info HRPD
        if(meas_cnfg->pre_reg_info_hrpd_present)
        {
            liblte_rrc_pack_pre_registration_info_hrpd_ie(&meas_cnfg->pre_reg_info_hrpd, ie_ptr);
        }

        // Speed State Pars
        if(meas_cnfg->speed_state_params_present)
        {
            // Release choice
            liblte_value_2_bits(1, ie_ptr, 1);

            // Mobility State Parameters
            liblte_rrc_pack_mobility_state_parameters_ie(&meas_cnfg->speed_state_params.mob_state_params, ie_ptr);

            // Time To Trigger SF
            liblte_rrc_pack_speed_state_scale_factors_ie(&meas_cnfg->speed_state_params.time_to_trig_sf, ie_ptr);
        }
        
        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_unpack_meas_config_ie(uint8                         **ie_ptr,
                                                   LIBLTE_RRC_MEAS_CONFIG_STRUCT  *meas_cnfg)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;
    uint32            i;
    bool              meas_obj_to_remove_present;
    bool              rep_cnfg_to_remove_present;
    bool              meas_id_to_remove_present;

    if(ie_ptr    != NULL &&
       meas_cnfg != NULL)
    {
        // Extension indicator
        bool ext = liblte_bits_2_value(ie_ptr, 1);

        // Optional indicators
        meas_obj_to_remove_present                  = liblte_bits_2_value(ie_ptr, 1);
        meas_cnfg->meas_obj_to_add_mod_list_present = liblte_bits_2_value(ie_ptr, 1);
        rep_cnfg_to_remove_present                  = liblte_bits_2_value(ie_ptr, 1);
        meas_cnfg->rep_cnfg_to_add_mod_list_present = liblte_bits_2_value(ie_ptr, 1);
        meas_id_to_remove_present                   = liblte_bits_2_value(ie_ptr, 1);
        meas_cnfg->meas_id_to_add_mod_list_present  = liblte_bits_2_value(ie_ptr, 1);
        meas_cnfg->quantity_cnfg_present            = liblte_bits_2_value(ie_ptr, 1);
        meas_cnfg->meas_gap_cnfg_present            = liblte_bits_2_value(ie_ptr, 1);
        meas_cnfg->s_meas_present                   = liblte_bits_2_value(ie_ptr, 1);
        meas_cnfg->pre_reg_info_hrpd_present        = liblte_bits_2_value(ie_ptr, 1);
        meas_cnfg->speed_state_params_present       = liblte_bits_2_value(ie_ptr, 1);

        // Meas Object To Remove List
        if(meas_obj_to_remove_present)
        {
            meas_cnfg->N_meas_obj_to_remove = liblte_bits_2_value(ie_ptr, 5) + 1;
            for(i=0; i<meas_cnfg->N_meas_obj_to_remove; i++)
            {
                liblte_rrc_unpack_meas_object_id_ie(ie_ptr, &meas_cnfg->meas_obj_to_remove_list[i]);
            }
        }else{
            meas_cnfg->N_meas_obj_to_remove = 0;
        }

        // Meas Object To Add Mod List
        if(meas_cnfg->meas_obj_to_add_mod_list_present)
        {
            liblte_rrc_unpack_meas_object_to_add_mod_list_ie(ie_ptr, &meas_cnfg->meas_obj_to_add_mod_list);
        }

        // Report Config To Remove List
        if(rep_cnfg_to_remove_present)
        {
            meas_cnfg->N_rep_cnfg_to_remove = liblte_bits_2_value(ie_ptr, 5) + 1;
            for(i=0; i<meas_cnfg->N_rep_cnfg_to_remove; i++)
            {
                liblte_rrc_unpack_report_config_id_ie(ie_ptr, &meas_cnfg->rep_cnfg_to_remove_list[i]);
            }
        }else{
            meas_cnfg->N_rep_cnfg_to_remove = 0;
        }

        // Report Config To Add Mod List
        if(meas_cnfg->rep_cnfg_to_add_mod_list_present)
        {
            liblte_rrc_unpack_report_config_to_add_mod_list_ie(ie_ptr, &meas_cnfg->rep_cnfg_to_add_mod_list);
        }

        // Meas ID To Remove List
        if(meas_id_to_remove_present)
        {
            meas_cnfg->N_meas_id_to_remove = liblte_bits_2_value(ie_ptr, 5) + 1;
            for(i=0; i<meas_cnfg->N_meas_id_to_remove; i++)
            {
                liblte_rrc_unpack_meas_id_ie(ie_ptr, &meas_cnfg->meas_id_to_remove_list[i]);
            }
        }else{
            meas_cnfg->N_meas_id_to_remove = 0;
        }

        // Meas ID To Add Mod List
        if(meas_cnfg->meas_id_to_add_mod_list_present)
        {
            liblte_rrc_unpack_meas_id_to_add_mod_list_ie(ie_ptr, &meas_cnfg->meas_id_to_add_mod_list);
        }

        // Quantity Config
        if(meas_cnfg->quantity_cnfg_present)
        {
            liblte_rrc_unpack_quantity_config_ie(ie_ptr, &meas_cnfg->quantity_cnfg);
        }

        // Meas Gap Config
        if(meas_cnfg->meas_gap_cnfg_present)
        {
            liblte_rrc_unpack_meas_gap_config_ie(ie_ptr, &meas_cnfg->meas_gap_cnfg);
        }

        // S Measure
        if(meas_cnfg->s_meas_present)
        {
            liblte_rrc_unpack_rsrp_range_ie(ie_ptr, &meas_cnfg->s_meas);
        }

        // Pre Registration Info HRPD
        if(meas_cnfg->pre_reg_info_hrpd_present)
        {
            liblte_rrc_unpack_pre_registration_info_hrpd_ie(ie_ptr, &meas_cnfg->pre_reg_info_hrpd);
        }

        // Speed State Pars
        if(meas_cnfg->speed_state_params_present)
        {
            // Release choice
            meas_cnfg->speed_state_params_present = liblte_bits_2_value(ie_ptr, 1);

            if(meas_cnfg->speed_state_params_present)
            {
                // Mobility State Parameters
                liblte_rrc_unpack_mobility_state_parameters_ie(ie_ptr, &meas_cnfg->speed_state_params.mob_state_params);

                // Time To Trigger SF
                liblte_rrc_unpack_speed_state_scale_factors_ie(ie_ptr, &meas_cnfg->speed_state_params.time_to_trig_sf);
            }
        }

        liblte_rrc_consume_noncrit_extension(ext, __func__, ie_ptr);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: Meas Gap Config

    Description: Specifies the measurement gap configuration and
                 controls setup/release of measurement gaps

    Document Reference: 36.331 v10.0.0 Section 6.3.5
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_rrc_pack_meas_gap_config_ie(LIBLTE_RRC_MEAS_GAP_CONFIG_STRUCT  *meas_gap_cnfg,
                                                     uint8                             **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(meas_gap_cnfg != NULL &&
       ie_ptr        != NULL)
    {
        // Release choice
        liblte_value_2_bits(meas_gap_cnfg->setup_present, ie_ptr, 1);

        if(meas_gap_cnfg->setup_present)
        {
            // Gap Offset Type
            liblte_value_2_bits(meas_gap_cnfg->gap_offset_type, ie_ptr, 1);

            // Gap Offset
            if(LIBLTE_RRC_GAP_OFFSET_TYPE_GP0 == meas_gap_cnfg->gap_offset_type)
            {
                liblte_value_2_bits(meas_gap_cnfg->gap_offset, ie_ptr, 6);
            }else{
                liblte_value_2_bits(meas_gap_cnfg->gap_offset, ie_ptr, 7);
            }
        }

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_unpack_meas_gap_config_ie(uint8                             **ie_ptr,
                                                       LIBLTE_RRC_MEAS_GAP_CONFIG_STRUCT  *meas_gap_cnfg)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr        != NULL &&
       meas_gap_cnfg != NULL)
    {
        // Release choice
        meas_gap_cnfg->setup_present = liblte_bits_2_value(ie_ptr, 1);

        if(meas_gap_cnfg->setup_present)
        {
          // Gap Offset Type
          meas_gap_cnfg->gap_offset_type = (LIBLTE_RRC_GAP_OFFSET_TYPE_ENUM)liblte_bits_2_value(ie_ptr, 1);

          // Gap Offset
          if(LIBLTE_RRC_GAP_OFFSET_TYPE_GP0 == meas_gap_cnfg->gap_offset_type)
          {
              meas_gap_cnfg->gap_offset = liblte_bits_2_value(ie_ptr, 6);
          }else{
              meas_gap_cnfg->gap_offset = liblte_bits_2_value(ie_ptr, 7);
          }
        }

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: Meas ID

    Description: Identifies a measurement configuration, i.e. linking
                 of a measurement object and a reporting configuration

    Document Reference: 36.331 v10.0.0 Section 6.3.5
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_rrc_pack_meas_id_ie(uint8   meas_id,
                                             uint8 **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr != NULL)
    {
        liblte_value_2_bits(meas_id - 1, ie_ptr, 5);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_unpack_meas_id_ie(uint8 **ie_ptr,
                                               uint8  *meas_id)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr  != NULL &&
       meas_id != NULL)
    {
        *meas_id = liblte_bits_2_value(ie_ptr, 5) + 1;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: Meas Id To Add Mod List

    Description: Concerns a list of measurement identities to add or
                 modify, with for each entry the meas ID, the
                 associated meas object ID and the associated report
                 config ID

    Document Reference: 36.331 v10.0.0 Section 6.3.5
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_rrc_pack_meas_id_to_add_mod_list_ie(LIBLTE_RRC_MEAS_ID_TO_ADD_MOD_LIST_STRUCT  *list,
                                                             uint8                                     **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;
    uint32            i;

    if(list   != NULL &&
       ie_ptr != NULL)
    {
        // List Size
        liblte_value_2_bits(list->N_meas_id - 1, ie_ptr, 5);

        for(i=0; i<list->N_meas_id; i++)
        {
            // Meas ID
            liblte_rrc_pack_meas_id_ie(list->meas_id_list[i].meas_id, ie_ptr);

            // Meas Object ID
            liblte_rrc_pack_meas_object_id_ie(list->meas_id_list[i].meas_obj_id, ie_ptr);

            // Report Config ID
            liblte_rrc_pack_report_config_id_ie(list->meas_id_list[i].rep_cnfg_id, ie_ptr);
        }

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_unpack_meas_id_to_add_mod_list_ie(uint8                                     **ie_ptr,
                                                               LIBLTE_RRC_MEAS_ID_TO_ADD_MOD_LIST_STRUCT  *list)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;
    uint32            i;

    if(ie_ptr != NULL &&
       list   != NULL)
    {
        // List Size
        list->N_meas_id = liblte_bits_2_value(ie_ptr, 5) + 1;

        for(i=0; i<list->N_meas_id; i++)
        {
            // Meas ID
            liblte_rrc_unpack_meas_id_ie(ie_ptr, &list->meas_id_list[i].meas_id);

            // Meas Object ID
            liblte_rrc_unpack_meas_object_id_ie(ie_ptr, &list->meas_id_list[i].meas_obj_id);

            // Report Config ID
            liblte_rrc_unpack_report_config_id_ie(ie_ptr, &list->meas_id_list[i].rep_cnfg_id);
        }

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: Meas Object CDMA2000

    Description: Specifies information applicable for inter-RAT
                 CDMA2000 neighboring cells

    Document Reference: 36.331 v10.0.0 Section 6.3.5
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_rrc_pack_meas_object_cdma2000_ie(LIBLTE_RRC_MEAS_OBJECT_CDMA2000_STRUCT  *meas_obj_cdma2000,
                                                          uint8                                  **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;
    uint32            i;

    if(meas_obj_cdma2000 != NULL &&
       ie_ptr            != NULL)
    {
        // Extension indicator
        liblte_value_2_bits(0, ie_ptr, 1);

        // Optional indicators
        liblte_value_2_bits(meas_obj_cdma2000->search_win_size_present,      ie_ptr, 1);
        liblte_value_2_bits(meas_obj_cdma2000->cells_to_remove_list_present, ie_ptr, 1);
        if(0 != meas_obj_cdma2000->N_cells_to_add_mod)
        {
            liblte_value_2_bits(1, ie_ptr, 1);
        }else{
            liblte_value_2_bits(0, ie_ptr, 1);
        }
        liblte_value_2_bits(meas_obj_cdma2000->cell_for_which_to_rep_cgi_present, ie_ptr, 1);

        // CDMA2000 Type
        liblte_rrc_pack_cdma2000_type_ie(meas_obj_cdma2000->cdma2000_type, ie_ptr);

        // Carrier Freq
        liblte_rrc_pack_carrier_freq_cdma2000_ie(&meas_obj_cdma2000->carrier_freq, ie_ptr);

        // Search Window Size
        if(meas_obj_cdma2000->search_win_size_present)
        {
            liblte_value_2_bits(meas_obj_cdma2000->search_win_size, ie_ptr, 4);
        }

        // Offset Freq
        liblte_rrc_pack_q_offset_range_inter_rat_ie(meas_obj_cdma2000->offset_freq, ie_ptr);

        // Cells To Remove List
        if(meas_obj_cdma2000->cells_to_remove_list_present)
        {
            liblte_rrc_pack_cell_index_list_ie(&meas_obj_cdma2000->cells_to_remove_list, ie_ptr);
        }

        // Cells To Add Mod List
        if(0 != meas_obj_cdma2000->N_cells_to_add_mod)
        {
            liblte_value_2_bits(meas_obj_cdma2000->N_cells_to_add_mod - 1, ie_ptr, 5);
        }
        for(i=0; i<meas_obj_cdma2000->N_cells_to_add_mod; i++)
        {
            // Cell Index
            liblte_value_2_bits(meas_obj_cdma2000->cells_to_add_mod_list[i].cell_idx - 1, ie_ptr, 5);

            // Phys Cell ID
            liblte_rrc_pack_phys_cell_id_cdma2000_ie(meas_obj_cdma2000->cells_to_add_mod_list[i].pci, ie_ptr);
        }

        // Cell For Which To Report CGI
        if(meas_obj_cdma2000->cell_for_which_to_rep_cgi_present)
        {
            liblte_rrc_pack_phys_cell_id_cdma2000_ie(meas_obj_cdma2000->cell_for_which_to_rep_cgi_present, ie_ptr);
        }

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_unpack_meas_object_cdma2000_ie(uint8                                  **ie_ptr,
                                                            LIBLTE_RRC_MEAS_OBJECT_CDMA2000_STRUCT  *meas_obj_cdma2000)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;
    uint32            i;
    bool              cells_to_add_mod_present;

    if(ie_ptr            != NULL &&
       meas_obj_cdma2000 != NULL)
    {
        // Extension indicator
        liblte_bits_2_value(ie_ptr, 1);

        // Optional indicators
        meas_obj_cdma2000->search_win_size_present           = liblte_bits_2_value(ie_ptr, 1);
        meas_obj_cdma2000->cells_to_remove_list_present      = liblte_bits_2_value(ie_ptr, 1);
        cells_to_add_mod_present                             = liblte_bits_2_value(ie_ptr, 1);
        meas_obj_cdma2000->cell_for_which_to_rep_cgi_present = liblte_bits_2_value(ie_ptr, 1);

        // CDMA2000 Type
        liblte_rrc_unpack_cdma2000_type_ie(ie_ptr, &meas_obj_cdma2000->cdma2000_type);

        // Carrier Freq
        liblte_rrc_unpack_carrier_freq_cdma2000_ie(ie_ptr, &meas_obj_cdma2000->carrier_freq);

        // Search Window Size
        if(meas_obj_cdma2000->search_win_size_present)
        {
            meas_obj_cdma2000->search_win_size = liblte_bits_2_value(ie_ptr, 4);
        }

        // Offset Freq
        liblte_rrc_unpack_q_offset_range_inter_rat_ie(ie_ptr, &meas_obj_cdma2000->offset_freq);

        // Cells To Remove List
        if(meas_obj_cdma2000->cells_to_remove_list_present)
        {
            liblte_rrc_unpack_cell_index_list_ie(ie_ptr, &meas_obj_cdma2000->cells_to_remove_list);
        }

        // Cells To Add Mod List
        if(cells_to_add_mod_present)
        {
            meas_obj_cdma2000->N_cells_to_add_mod = liblte_bits_2_value(ie_ptr, 5) + 1;
            for(i=0; i<meas_obj_cdma2000->N_cells_to_add_mod; i++)
            {
                // Cell Index
                meas_obj_cdma2000->cells_to_add_mod_list[i].cell_idx = liblte_bits_2_value(ie_ptr, 5) + 1;

                // Phys Cell ID
                liblte_rrc_unpack_phys_cell_id_cdma2000_ie(ie_ptr, &meas_obj_cdma2000->cells_to_add_mod_list[i].pci);
            }
        }else{
            meas_obj_cdma2000->N_cells_to_add_mod = 0;
        }

        // Cell For Which To Report CGI
        if(meas_obj_cdma2000->cell_for_which_to_rep_cgi_present)
        {
            liblte_rrc_unpack_phys_cell_id_cdma2000_ie(ie_ptr, &meas_obj_cdma2000->cell_for_which_to_rep_cgi);
        }

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: Meas Object EUTRA

    Description: Specifies information applicable for intra-frequency
                 or inter-frequency E-UTRA cells

    Document Reference: 36.331 v10.0.0 Section 6.3.5
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_rrc_pack_meas_object_eutra_ie(LIBLTE_RRC_MEAS_OBJECT_EUTRA_STRUCT  *meas_obj_eutra,
                                                       uint8                               **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;
    uint32            i;

    if(meas_obj_eutra != NULL &&
       ie_ptr         != NULL)
    {
        // Extension indicator
        liblte_value_2_bits(0, ie_ptr, 1);

        // Optional indicators
        liblte_value_2_bits(meas_obj_eutra->offset_freq_not_default, ie_ptr, 1);
        liblte_value_2_bits(meas_obj_eutra->cells_to_remove_list_present, ie_ptr, 1);
        if(0 != meas_obj_eutra->N_cells_to_add_mod)
        {
            liblte_value_2_bits(1, ie_ptr, 1);
        }else{
            liblte_value_2_bits(0, ie_ptr, 1);
        }
        liblte_value_2_bits(meas_obj_eutra->black_cells_to_remove_list_present, ie_ptr, 1);
        if(0 != meas_obj_eutra->N_black_cells_to_add_mod)
        {
            liblte_value_2_bits(1, ie_ptr, 1);
        }else{
            liblte_value_2_bits(0, ie_ptr, 1);
        }
        liblte_value_2_bits(meas_obj_eutra->cell_for_which_to_rep_cgi_present, ie_ptr, 1);

        // Carrier Freq
        liblte_rrc_pack_arfcn_value_eutra_ie(meas_obj_eutra->carrier_freq, ie_ptr);

        // Allowed Meas Bandwidth
        liblte_rrc_pack_allowed_meas_bandwidth_ie(meas_obj_eutra->allowed_meas_bw, ie_ptr);

        // Presence Antenna Port 1
        liblte_rrc_pack_presence_antenna_port_1_ie(meas_obj_eutra->presence_ant_port_1, ie_ptr);

        // Neigh Cell Config
        liblte_rrc_pack_neigh_cell_config_ie(meas_obj_eutra->neigh_cell_cnfg, ie_ptr);

        // Offset Freq
        if(meas_obj_eutra->offset_freq_not_default)
        {
          liblte_rrc_pack_q_offset_range_ie(meas_obj_eutra->offset_freq, ie_ptr);
        }

        // Cells To Remove List
        if(meas_obj_eutra->cells_to_remove_list_present)
        {
            liblte_rrc_pack_cell_index_list_ie(&meas_obj_eutra->cells_to_remove_list, ie_ptr);
        }

        // Cells To Add Mod List
        if(0 != meas_obj_eutra->N_cells_to_add_mod)
        {
            liblte_value_2_bits(meas_obj_eutra->N_cells_to_add_mod - 1, ie_ptr, 5);
        }
        for(i=0; i<meas_obj_eutra->N_cells_to_add_mod; i++)
        {
            // Cell Index
            liblte_value_2_bits(meas_obj_eutra->cells_to_add_mod_list[i].cell_idx - 1, ie_ptr, 5);

            // Phys Cell ID
            liblte_rrc_pack_phys_cell_id_ie(meas_obj_eutra->cells_to_add_mod_list[i].pci, ie_ptr);

            // Cell Individual Offset
            liblte_rrc_pack_q_offset_range_ie(meas_obj_eutra->cells_to_add_mod_list[i].cell_offset, ie_ptr);
        }

        // Black Cells To Remove List
        if(meas_obj_eutra->black_cells_to_remove_list_present)
        {
            liblte_rrc_pack_cell_index_list_ie(&meas_obj_eutra->black_cells_to_remove_list, ie_ptr);
        }

        // Black Cells To Add Mod List
        if(0 != meas_obj_eutra->N_black_cells_to_add_mod)
        {
            liblte_value_2_bits(meas_obj_eutra->N_black_cells_to_add_mod - 1, ie_ptr, 5);
        }
        for(i=0; i<meas_obj_eutra->N_black_cells_to_add_mod; i++)
        {
            // Cell Index
            liblte_value_2_bits(meas_obj_eutra->black_cells_to_add_mod_list[i].cell_idx - 1, ie_ptr, 5);

            // Phys Cell ID Range
            liblte_rrc_pack_phys_cell_id_range_ie(&meas_obj_eutra->black_cells_to_add_mod_list[i].pci_range, ie_ptr);
        }

        // Cell For Which To Report CGI
        if(meas_obj_eutra->cell_for_which_to_rep_cgi_present)
        {
            liblte_rrc_pack_phys_cell_id_ie(meas_obj_eutra->cell_for_which_to_rep_cgi, ie_ptr);
        }

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_unpack_meas_object_eutra_ie(uint8                               **ie_ptr,
                                                         LIBLTE_RRC_MEAS_OBJECT_EUTRA_STRUCT  *meas_obj_eutra)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;
    uint32            i;
    bool              cells_to_add_mod_list_present;
    bool              black_cells_to_add_mod_list_present;

    if(ie_ptr         != NULL &&
       meas_obj_eutra != NULL)
    {
        // Extension indicator
        liblte_bits_2_value(ie_ptr, 1);

        // Optional indicators
        meas_obj_eutra->offset_freq_not_default            = liblte_bits_2_value(ie_ptr, 1);
        meas_obj_eutra->cells_to_remove_list_present       = liblte_bits_2_value(ie_ptr, 1);
        cells_to_add_mod_list_present                      = liblte_bits_2_value(ie_ptr, 1);
        meas_obj_eutra->black_cells_to_remove_list_present = liblte_bits_2_value(ie_ptr, 1);
        black_cells_to_add_mod_list_present                = liblte_bits_2_value(ie_ptr, 1);
        meas_obj_eutra->cell_for_which_to_rep_cgi_present  = liblte_bits_2_value(ie_ptr, 1);

        // Carrier Freq
        liblte_rrc_unpack_arfcn_value_eutra_ie(ie_ptr, &meas_obj_eutra->carrier_freq);

        // Allowed Meas Bandwidth
        liblte_rrc_unpack_allowed_meas_bandwidth_ie(ie_ptr, &meas_obj_eutra->allowed_meas_bw);

        // Presence Antenna Port 1
        liblte_rrc_unpack_presence_antenna_port_1_ie(ie_ptr, &meas_obj_eutra->presence_ant_port_1);

        // Neigh Cell Config
        liblte_rrc_unpack_neigh_cell_config_ie(ie_ptr, &meas_obj_eutra->neigh_cell_cnfg);

        // Offset Freq
        if(meas_obj_eutra->offset_freq_not_default)
        {
          liblte_rrc_unpack_q_offset_range_ie(ie_ptr, &meas_obj_eutra->offset_freq);
        }

        // Cells To Remove List
        if(meas_obj_eutra->cells_to_remove_list_present)
        {
            liblte_rrc_unpack_cell_index_list_ie(ie_ptr, &meas_obj_eutra->cells_to_remove_list);
        }

        // Cells To Add Mod List
        if(cells_to_add_mod_list_present)
        {
            meas_obj_eutra->N_cells_to_add_mod = liblte_bits_2_value(ie_ptr, 5) + 1;
            for(i=0; i<meas_obj_eutra->N_cells_to_add_mod; i++)
            {
                // Cell Index
                meas_obj_eutra->cells_to_add_mod_list[i].cell_idx = liblte_bits_2_value(ie_ptr, 5) + 1;

                // Phys Cell ID
                liblte_rrc_unpack_phys_cell_id_ie(ie_ptr, &meas_obj_eutra->cells_to_add_mod_list[i].pci);

                // Cell Individual Offset
                liblte_rrc_unpack_q_offset_range_ie(ie_ptr, &meas_obj_eutra->cells_to_add_mod_list[i].cell_offset);
            }
        }else{
            meas_obj_eutra->N_cells_to_add_mod = 0;
        }

        // Black Cells To Remove List
        if(meas_obj_eutra->black_cells_to_remove_list_present)
        {
            liblte_rrc_unpack_cell_index_list_ie(ie_ptr, &meas_obj_eutra->black_cells_to_remove_list);
        }

        // Black Cells To Add Mod List
        if(black_cells_to_add_mod_list_present)
        {
            meas_obj_eutra->N_black_cells_to_add_mod = liblte_bits_2_value(ie_ptr, 5) + 1;
            for(i=0; i<meas_obj_eutra->N_black_cells_to_add_mod; i++)
            {
                // Cell Index
                meas_obj_eutra->black_cells_to_add_mod_list[i].cell_idx = liblte_bits_2_value(ie_ptr, 5) + 1;

                // Phys Cell ID Range
                liblte_rrc_unpack_phys_cell_id_range_ie(ie_ptr, &meas_obj_eutra->black_cells_to_add_mod_list[i].pci_range);
            }
        }else{
            meas_obj_eutra->N_black_cells_to_add_mod = 0;
        }

        // Cell For Which To Report CGI
        if(meas_obj_eutra->cell_for_which_to_rep_cgi_present)
        {
            liblte_rrc_unpack_phys_cell_id_ie(ie_ptr, &meas_obj_eutra->cell_for_which_to_rep_cgi);
        }

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: Meas Object GERAN

    Description: Specifies information applicable for inter-RAT
                 GERAN neighboring frequencies

    Document Reference: 36.331 v10.0.0 Section 6.3.5
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_rrc_pack_meas_object_geran_ie(LIBLTE_RRC_MEAS_OBJECT_GERAN_STRUCT  *meas_obj_geran,
                                                       uint8                               **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(meas_obj_geran != NULL &&
       ie_ptr         != NULL)
    {
        // Extension indicator
        liblte_value_2_bits(0, ie_ptr, 1);

        // Optional indicator
        liblte_value_2_bits(meas_obj_geran->cell_for_which_to_rep_cgi_present, ie_ptr, 1);

        // Carrier Freqs
        liblte_rrc_pack_carrier_freqs_geran_ie(&meas_obj_geran->carrier_freqs, ie_ptr);

        // Offset Freq
        liblte_rrc_pack_q_offset_range_inter_rat_ie(meas_obj_geran->offset_freq, ie_ptr);

        // NCC Permitted
        liblte_value_2_bits(meas_obj_geran->ncc_permitted, ie_ptr, 8);

        // Cell For Which To Report CGI
        if(meas_obj_geran->cell_for_which_to_rep_cgi_present)
        {
            liblte_rrc_pack_phys_cell_id_geran_ie(&meas_obj_geran->cell_for_which_to_rep_cgi, ie_ptr);
        }

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_unpack_meas_object_geran_ie(uint8                               **ie_ptr,
                                                         LIBLTE_RRC_MEAS_OBJECT_GERAN_STRUCT  *meas_obj_geran)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr         != NULL &&
       meas_obj_geran != NULL)
    {
        // Extension indicator
        liblte_bits_2_value(ie_ptr, 1);

        // Optional indicator
        meas_obj_geran->cell_for_which_to_rep_cgi_present = liblte_bits_2_value(ie_ptr, 1);

        // Carrier Freqs
        liblte_rrc_unpack_carrier_freqs_geran_ie(ie_ptr, &meas_obj_geran->carrier_freqs);

        // Offset Freq
        liblte_rrc_unpack_q_offset_range_inter_rat_ie(ie_ptr, &meas_obj_geran->offset_freq);

        // NCC Permitted
        meas_obj_geran->ncc_permitted = liblte_bits_2_value(ie_ptr, 8);

        // Cell For Which To Report CGI
        if(meas_obj_geran->cell_for_which_to_rep_cgi_present)
        {
            liblte_rrc_unpack_phys_cell_id_geran_ie(ie_ptr, &meas_obj_geran->cell_for_which_to_rep_cgi);
        }

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: Meas Object ID

    Description: Identifies a measurement object configuration

    Document Reference: 36.331 v10.0.0 Section 6.3.5
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_rrc_pack_meas_object_id_ie(uint8   meas_object_id,
                                                    uint8 **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr != NULL)
    {
        liblte_value_2_bits(meas_object_id - 1, ie_ptr, 5);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_unpack_meas_object_id_ie(uint8 **ie_ptr,
                                                      uint8  *meas_object_id)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr         != NULL &&
       meas_object_id != NULL)
    {
        *meas_object_id = liblte_bits_2_value(ie_ptr, 5) + 1;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: Meas Object To Add Mod List

    Description: Concerns a list of measurement objects to add or
                 modify

    Document Reference: 36.331 v10.0.0 Section 6.3.5
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_rrc_pack_meas_object_to_add_mod_list_ie(LIBLTE_RRC_MEAS_OBJECT_TO_ADD_MOD_LIST_STRUCT  *list,
                                                                 uint8                                         **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;
    uint32            i;

    if(list   != NULL &&
       ie_ptr != NULL)
    {
        // List Size
        liblte_value_2_bits(list->N_meas_obj - 1, ie_ptr, 5);

        for(i=0; i<list->N_meas_obj; i++)
        {
            // Meas Object ID
            liblte_rrc_pack_meas_object_id_ie(list->meas_obj_list[i].meas_obj_id, ie_ptr);

            // Meas Object Choice Extension
            liblte_value_2_bits(0, ie_ptr, 1); // Choice from before extension marker

            // Meas Object Choice
            liblte_value_2_bits(list->meas_obj_list[i].meas_obj_type, ie_ptr, 2);

            // Meas Object
            if(LIBLTE_RRC_MEAS_OBJECT_TYPE_EUTRA == list->meas_obj_list[i].meas_obj_type)
            {
                liblte_rrc_pack_meas_object_eutra_ie(&list->meas_obj_list[i].meas_obj_eutra, ie_ptr);
            }else if(LIBLTE_RRC_MEAS_OBJECT_TYPE_UTRA == list->meas_obj_list[i].meas_obj_type){
                liblte_rrc_pack_meas_object_utra_ie(&list->meas_obj_list[i].meas_obj_utra, ie_ptr);
            }else if(LIBLTE_RRC_MEAS_OBJECT_TYPE_GERAN == list->meas_obj_list[i].meas_obj_type){
                liblte_rrc_pack_meas_object_geran_ie(&list->meas_obj_list[i].meas_obj_geran, ie_ptr);
            }else{
                liblte_rrc_pack_meas_object_cdma2000_ie(&list->meas_obj_list[i].meas_obj_cdma2000, ie_ptr);
            }
        }

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_unpack_meas_object_to_add_mod_list_ie(uint8                                         **ie_ptr,
                                                                   LIBLTE_RRC_MEAS_OBJECT_TO_ADD_MOD_LIST_STRUCT  *list)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;
    uint32            i;

    if(ie_ptr != NULL &&
       list   != NULL)
    {
        // List Size
        list->N_meas_obj = liblte_bits_2_value(ie_ptr, 5) + 1;

        for(i=0; i<list->N_meas_obj; i++)
        {
            // Meas Object ID
            liblte_rrc_unpack_meas_object_id_ie(ie_ptr, &list->meas_obj_list[i].meas_obj_id);

            // Meas Object Choice Extension
            liblte_bits_2_value(ie_ptr, 1);

            // Meas Object Choice
            list->meas_obj_list[i].meas_obj_type = (LIBLTE_RRC_MEAS_OBJECT_TYPE_ENUM)liblte_bits_2_value(ie_ptr, 2);

            // Meas Object
            if(LIBLTE_RRC_MEAS_OBJECT_TYPE_EUTRA == list->meas_obj_list[i].meas_obj_type)
            {
                liblte_rrc_unpack_meas_object_eutra_ie(ie_ptr, &list->meas_obj_list[i].meas_obj_eutra);
            }else if(LIBLTE_RRC_MEAS_OBJECT_TYPE_UTRA == list->meas_obj_list[i].meas_obj_type){
                liblte_rrc_unpack_meas_object_utra_ie(ie_ptr, &list->meas_obj_list[i].meas_obj_utra);
            }else if(LIBLTE_RRC_MEAS_OBJECT_TYPE_GERAN == list->meas_obj_list[i].meas_obj_type){
                liblte_rrc_unpack_meas_object_geran_ie(ie_ptr, &list->meas_obj_list[i].meas_obj_geran);
            }else{
                liblte_rrc_unpack_meas_object_cdma2000_ie(ie_ptr, &list->meas_obj_list[i].meas_obj_cdma2000);
            }
        }

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: Meas Object UTRA

    Description: Specifies information applicable for inter-RAT UTRA
                 neighboring cells

    Document Reference: 36.331 v10.0.0 Section 6.3.5
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_rrc_pack_meas_object_utra_ie(LIBLTE_RRC_MEAS_OBJECT_UTRA_STRUCT  *meas_obj_utra,
                                                      uint8                              **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;
    uint32            i;

    if(meas_obj_utra != NULL &&
       ie_ptr        != NULL)
    {
        // Extension indicator
        liblte_value_2_bits(0, ie_ptr, 1);

        // Optional indicators
        liblte_value_2_bits(meas_obj_utra->cells_to_remove_list_present,       ie_ptr, 1);
        liblte_value_2_bits(meas_obj_utra->cells_to_add_mod_list_present,      ie_ptr, 1);
        liblte_value_2_bits(meas_obj_utra->cells_for_which_to_rep_cgi_present, ie_ptr, 1);

        // Carrier Freq
        liblte_rrc_pack_arfcn_value_utra_ie(meas_obj_utra->carrier_freq, ie_ptr);

        // Offset Freq
        liblte_rrc_pack_q_offset_range_inter_rat_ie(meas_obj_utra->offset_freq, ie_ptr);

        // Cells To Remove List
        if(meas_obj_utra->cells_to_remove_list_present)
        {
            liblte_rrc_pack_cell_index_list_ie(&meas_obj_utra->cells_to_remove_list, ie_ptr);
        }

        // Cells To Add Mod List
        if(meas_obj_utra->cells_to_add_mod_list_present)
        {
            // UTRA System Type
            liblte_value_2_bits(meas_obj_utra->cells_to_add_mod_list.type, ie_ptr, 1);

            liblte_value_2_bits(meas_obj_utra->cells_to_add_mod_list.N_cells - 1, ie_ptr, 5);
            for(i=0; i<meas_obj_utra->cells_to_add_mod_list.N_cells; i++)
            {
                if(LIBLTE_RRC_UTRA_SYSTEM_TYPE_FDD == meas_obj_utra->cells_to_add_mod_list.type)
                {
                    // Cell Index
                    liblte_value_2_bits(meas_obj_utra->cells_to_add_mod_list.cells_fdd[i].cell_idx - 1, ie_ptr, 5);

                    // Phys Cell ID
                    liblte_rrc_pack_phys_cell_id_utra_fdd_ie(meas_obj_utra->cells_to_add_mod_list.cells_fdd[i].pci, ie_ptr);
                }else{
                    // Cell Index
                    liblte_value_2_bits(meas_obj_utra->cells_to_add_mod_list.cells_tdd[i].cell_idx - 1, ie_ptr, 5);

                    // Phys Cell ID
                    liblte_rrc_pack_phys_cell_id_utra_tdd_ie(meas_obj_utra->cells_to_add_mod_list.cells_tdd[i].pci, ie_ptr);
                }
            }
        }

        // Cells For Which To Report CGI
        if(meas_obj_utra->cells_for_which_to_rep_cgi_present)
        {
            // UTRA System Type
            liblte_value_2_bits(meas_obj_utra->cells_for_which_to_rep_cgi.type, ie_ptr, 1);

            if(LIBLTE_RRC_UTRA_SYSTEM_TYPE_FDD == meas_obj_utra->cells_for_which_to_rep_cgi.type)
            {
                // Phys Cell ID
                liblte_rrc_pack_phys_cell_id_utra_fdd_ie(meas_obj_utra->cells_for_which_to_rep_cgi.pci_fdd, ie_ptr);
            }else{
                // Phys Cell ID
                liblte_rrc_pack_phys_cell_id_utra_tdd_ie(meas_obj_utra->cells_for_which_to_rep_cgi.pci_tdd, ie_ptr);
            }
        }

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_unpack_meas_object_utra_ie(uint8                              **ie_ptr,
                                                        LIBLTE_RRC_MEAS_OBJECT_UTRA_STRUCT  *meas_obj_utra)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;
    uint32            i;

    if(ie_ptr        != NULL &&
       meas_obj_utra != NULL)
    {
        // Extension indicator
        liblte_bits_2_value(ie_ptr, 1);

        // Optional indicators
        meas_obj_utra->cells_to_remove_list_present       = liblte_bits_2_value(ie_ptr, 1);
        meas_obj_utra->cells_to_add_mod_list_present      = liblte_bits_2_value(ie_ptr, 1);
        meas_obj_utra->cells_for_which_to_rep_cgi_present = liblte_bits_2_value(ie_ptr, 1);

        // Carrier Freq
        liblte_rrc_unpack_arfcn_value_utra_ie(ie_ptr, &meas_obj_utra->carrier_freq);

        // Offset Freq
        liblte_rrc_unpack_q_offset_range_inter_rat_ie(ie_ptr, &meas_obj_utra->offset_freq);

        // Cells To Remove List
        if(meas_obj_utra->cells_to_remove_list_present)
        {
            liblte_rrc_unpack_cell_index_list_ie(ie_ptr, &meas_obj_utra->cells_to_remove_list);
        }

        // Cells To Add Mod List
        if(meas_obj_utra->cells_to_add_mod_list_present)
        {
            // UTRA System Type
            meas_obj_utra->cells_to_add_mod_list.type = (LIBLTE_RRC_UTRA_SYSTEM_TYPE_ENUM)liblte_bits_2_value(ie_ptr, 1);

            meas_obj_utra->cells_to_add_mod_list.N_cells = liblte_bits_2_value(ie_ptr, 5) + 1;
            for(i=0; i<meas_obj_utra->cells_to_add_mod_list.N_cells; i++)
            {
                if(LIBLTE_RRC_UTRA_SYSTEM_TYPE_FDD == meas_obj_utra->cells_to_add_mod_list.type)
                {
                    // Cell Index
                    meas_obj_utra->cells_to_add_mod_list.cells_fdd[i].cell_idx = liblte_bits_2_value(ie_ptr, 5) + 1;

                    // Phys Cell ID
                    liblte_rrc_unpack_phys_cell_id_utra_fdd_ie(ie_ptr, &meas_obj_utra->cells_to_add_mod_list.cells_fdd[i].pci);
                }else{
                    // Cell Index
                    meas_obj_utra->cells_to_add_mod_list.cells_tdd[i].cell_idx = liblte_bits_2_value(ie_ptr, 5) + 1;

                    // Phys Cell ID
                    liblte_rrc_unpack_phys_cell_id_utra_tdd_ie(ie_ptr, &meas_obj_utra->cells_to_add_mod_list.cells_tdd[i].pci);
                }
            }
        }

        // Cells For Which To Report CGI
        if(meas_obj_utra->cells_for_which_to_rep_cgi_present)
        {
            // UTRA System Type
            meas_obj_utra->cells_for_which_to_rep_cgi.type = (LIBLTE_RRC_UTRA_SYSTEM_TYPE_ENUM)liblte_bits_2_value(ie_ptr, 1);

            if(LIBLTE_RRC_UTRA_SYSTEM_TYPE_FDD == meas_obj_utra->cells_for_which_to_rep_cgi.type)
            {
                // Phys Cell ID
                liblte_rrc_unpack_phys_cell_id_utra_fdd_ie(ie_ptr, &meas_obj_utra->cells_for_which_to_rep_cgi.pci_fdd);
            }else{
                // Phys Cell ID
                liblte_rrc_unpack_phys_cell_id_utra_tdd_ie(ie_ptr, &meas_obj_utra->cells_for_which_to_rep_cgi.pci_tdd);
            }
        }

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: Meas Results

    Description: Covers measured results for intra-frequency,
                 inter-frequency and inter-RAT mobility

    Document Reference: 36.331 v10.0.0 Section 6.3.5
*********************************************************************/
// FIXME

/*********************************************************************
    IE Name: Quantity Config

    Description: Specifies the measurement quantities and layer 3
                 filtering coefficients for E-UTRA and inter-RAT
                 measurements

    Document Reference: 36.331 v10.0.0 Section 6.3.5
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_rrc_pack_quantity_config_ie(LIBLTE_RRC_QUANTITY_CONFIG_STRUCT  *qc,
                                                     uint8                             **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(qc     != NULL &&
       ie_ptr != NULL)
    {
        // Extension indicator
        liblte_value_2_bits(0, ie_ptr, 1);

        // Optional indicators
        liblte_value_2_bits(qc->qc_eutra_present,    ie_ptr, 1);
        liblte_value_2_bits(qc->qc_utra_present,     ie_ptr, 1);
        liblte_value_2_bits(qc->qc_geran_present,    ie_ptr, 1);
        liblte_value_2_bits(qc->qc_cdma2000_present, ie_ptr, 1);

        // Quality Config EUTRA
        if(qc->qc_eutra_present)
        {
            liblte_rrc_pack_filter_coefficient_ie(qc->qc_eutra.fc_rsrp, ie_ptr);
            liblte_rrc_pack_filter_coefficient_ie(qc->qc_eutra.fc_rsrq, ie_ptr);
        }

        // Quality Config UTRA
        if(qc->qc_utra_present)
        {
            liblte_value_2_bits(qc->qc_utra.mq_fdd, ie_ptr, 1);
            liblte_value_2_bits(qc->qc_utra.mq_tdd, ie_ptr, 1);
            liblte_rrc_pack_filter_coefficient_ie(qc->qc_utra.fc, ie_ptr);
        }

        // Quality Config GERAN
        if(qc->qc_geran_present)
        {
            liblte_value_2_bits(qc->qc_geran.mq, ie_ptr, 1);
            liblte_rrc_pack_filter_coefficient_ie(qc->qc_geran.fc, ie_ptr);
        }

        // Quality Config CDMA2000
        if(qc->qc_cdma2000_present)
        {
            liblte_value_2_bits(qc->qc_cdma2000.mq, ie_ptr, 1);
        }

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_unpack_quantity_config_ie(uint8                             **ie_ptr,
                                                       LIBLTE_RRC_QUANTITY_CONFIG_STRUCT  *qc)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr != NULL &&
       qc     != NULL)
    {
        // Extension indicator
        liblte_bits_2_value(ie_ptr, 1);

        // Optional indicators
        qc->qc_eutra_present    = liblte_bits_2_value(ie_ptr, 1);
        qc->qc_utra_present     = liblte_bits_2_value(ie_ptr, 1);
        qc->qc_geran_present    = liblte_bits_2_value(ie_ptr, 1);
        qc->qc_cdma2000_present = liblte_bits_2_value(ie_ptr, 1);

        // Quantity Config EUTRA
        if(qc->qc_eutra_present)
        {
            qc->qc_eutra.fc_rsrp_not_default = liblte_bits_2_value(ie_ptr, 1);
            qc->qc_eutra.fc_rsrq_not_default = liblte_bits_2_value(ie_ptr, 1);
            if(qc->qc_eutra.fc_rsrp_not_default) {
              liblte_rrc_unpack_filter_coefficient_ie(ie_ptr, &qc->qc_eutra.fc_rsrp);
            }
            if(qc->qc_eutra.fc_rsrq_not_default) {
              liblte_rrc_unpack_filter_coefficient_ie(ie_ptr, &qc->qc_eutra.fc_rsrq);
            }
        }

        // Quantity Config UTRA
        if(qc->qc_utra_present)
        {
            qc->qc_utra.fc_not_default = liblte_bits_2_value(ie_ptr, 1);
            qc->qc_utra.mq_fdd = (LIBLTE_RRC_MEAS_QUANTITY_UTRA_FDD_ENUM)liblte_bits_2_value(ie_ptr, 1);
            qc->qc_utra.mq_tdd = LIBLTE_RRC_MEAS_QUANTITY_UTRA_TDD_PCCPCH_RSCP;
            if(qc->qc_utra.fc_not_default) {
              liblte_rrc_unpack_filter_coefficient_ie(ie_ptr, &qc->qc_utra.fc);
            }
        }

        // Quantity Config GERAN
        if(qc->qc_geran_present)
        {
            qc->qc_geran.fc_not_default = liblte_bits_2_value(ie_ptr, 1);
            qc->qc_geran.mq = LIBLTE_RRC_MEAS_QUANTITY_GERAN_RSSI;
            if(qc->qc_geran.fc_not_default) {
              liblte_rrc_unpack_filter_coefficient_ie(ie_ptr, &qc->qc_geran.fc);
            }
        }

        // Quality Config CDMA2000
        if(qc->qc_cdma2000_present)
        {
            qc->qc_cdma2000.mq = (LIBLTE_RRC_MEAS_QUANTITY_CDMA2000_ENUM)liblte_bits_2_value(ie_ptr, 1);
        }

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: Report Config EUTRA

    Description: Specifies criteria for triggering of an E-UTRA
                 measurement reporting event

    Document Reference: 36.331 v10.0.0 Section 6.3.5
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_rrc_pack_report_config_eutra_ie(LIBLTE_RRC_REPORT_CONFIG_EUTRA_STRUCT  *rep_cnfg_eutra,
                                                         uint8                                 **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(rep_cnfg_eutra != NULL &&
       ie_ptr         != NULL)
    {
        // Extension indicator
        liblte_value_2_bits(0, ie_ptr, 1);

        // Trigger Type
        liblte_value_2_bits(rep_cnfg_eutra->trigger_type, ie_ptr, 1);
        if(LIBLTE_RRC_TRIGGER_TYPE_EUTRA_EVENT == rep_cnfg_eutra->trigger_type)
        {
            // Event ID choice extension indicator
            liblte_value_2_bits(0, ie_ptr, 1); // Choice with extension - unlikely to be >63 choices

            // Event ID
            liblte_value_2_bits(rep_cnfg_eutra->event.event_id, ie_ptr, 3);
            if(LIBLTE_RRC_EVENT_ID_EUTRA_A1 == rep_cnfg_eutra->event.event_id)
            {
                // Threshold Type
                liblte_value_2_bits(rep_cnfg_eutra->event.event_a1.eutra.type, ie_ptr, 1);
                if(LIBLTE_RRC_THRESHOLD_EUTRA_TYPE_RSRP == rep_cnfg_eutra->event.event_a1.eutra.type)
                {
                    liblte_rrc_pack_rsrp_range_ie(rep_cnfg_eutra->event.event_a1.eutra.range, ie_ptr);
                }else{
                    liblte_rrc_pack_rsrq_range_ie(rep_cnfg_eutra->event.event_a1.eutra.range, ie_ptr);
                }
            }else if(LIBLTE_RRC_EVENT_ID_EUTRA_A2 == rep_cnfg_eutra->event.event_id){
                // Threshold Type
                liblte_value_2_bits(rep_cnfg_eutra->event.event_a2.eutra.type, ie_ptr, 1);
                if(LIBLTE_RRC_THRESHOLD_EUTRA_TYPE_RSRP == rep_cnfg_eutra->event.event_a2.eutra.type)
                {
                    liblte_rrc_pack_rsrp_range_ie(rep_cnfg_eutra->event.event_a2.eutra.range, ie_ptr);
                }else{
                    liblte_rrc_pack_rsrq_range_ie(rep_cnfg_eutra->event.event_a2.eutra.range, ie_ptr);
                }
            }else if(LIBLTE_RRC_EVENT_ID_EUTRA_A3 == rep_cnfg_eutra->event.event_id){
                // Offset
                liblte_value_2_bits(rep_cnfg_eutra->event.event_a3.offset + 30, ie_ptr, 6);

                // Report On Leave
                liblte_value_2_bits(rep_cnfg_eutra->event.event_a3.report_on_leave, ie_ptr, 1);
            }else if(LIBLTE_RRC_EVENT_ID_EUTRA_A4 == rep_cnfg_eutra->event.event_id){
                // Threshold Type
                liblte_value_2_bits(rep_cnfg_eutra->event.event_a4.eutra.type, ie_ptr, 1);
                if(LIBLTE_RRC_THRESHOLD_EUTRA_TYPE_RSRP == rep_cnfg_eutra->event.event_a4.eutra.type)
                {
                    liblte_rrc_pack_rsrp_range_ie(rep_cnfg_eutra->event.event_a4.eutra.range, ie_ptr);
                }else{
                    liblte_rrc_pack_rsrq_range_ie(rep_cnfg_eutra->event.event_a4.eutra.range, ie_ptr);
                }
            }else if(LIBLTE_RRC_EVENT_ID_EUTRA_A5 == rep_cnfg_eutra->event.event_id){
                // Threshold1 Type
                liblte_value_2_bits(rep_cnfg_eutra->event.event_a5.eutra1.type, ie_ptr, 1);
                if(LIBLTE_RRC_THRESHOLD_EUTRA_TYPE_RSRP == rep_cnfg_eutra->event.event_a5.eutra1.type)
                {
                    liblte_rrc_pack_rsrp_range_ie(rep_cnfg_eutra->event.event_a5.eutra1.range, ie_ptr);
                }else{
                    liblte_rrc_pack_rsrq_range_ie(rep_cnfg_eutra->event.event_a5.eutra1.range, ie_ptr);
                }

                // Threshold2 Type
                liblte_value_2_bits(rep_cnfg_eutra->event.event_a5.eutra2.type, ie_ptr, 1);
                if(LIBLTE_RRC_THRESHOLD_EUTRA_TYPE_RSRP == rep_cnfg_eutra->event.event_a5.eutra2.type)
                {
                    liblte_rrc_pack_rsrp_range_ie(rep_cnfg_eutra->event.event_a5.eutra2.range, ie_ptr);
                }else{
                    liblte_rrc_pack_rsrq_range_ie(rep_cnfg_eutra->event.event_a5.eutra2.range, ie_ptr);
                }
            }else{
                // Offset
                liblte_value_2_bits(rep_cnfg_eutra->event.event_a6.offset + 30, ie_ptr, 6);

                // Report On Leave
                liblte_value_2_bits(rep_cnfg_eutra->event.event_a6.report_on_leave, ie_ptr, 1);
            }

            // Hysteresis
            liblte_rrc_pack_hysteresis_ie(rep_cnfg_eutra->event.hysteresis, ie_ptr);

            // Time To Trigger
            liblte_rrc_pack_time_to_trigger_ie(rep_cnfg_eutra->event.time_to_trigger, ie_ptr);
        }else{
            // Purpose
            liblte_value_2_bits(rep_cnfg_eutra->periodical.purpose, ie_ptr, 1);
        }

        // Trigger Quantity
        liblte_value_2_bits(rep_cnfg_eutra->trigger_quantity, ie_ptr, 1);

        // Report Quantity
        liblte_value_2_bits(rep_cnfg_eutra->report_quantity, ie_ptr, 1);

        // Max Report Cells
        liblte_value_2_bits(rep_cnfg_eutra->max_report_cells - 1, ie_ptr, 3);

        // Report Interval
        liblte_rrc_pack_report_interval_ie(rep_cnfg_eutra->report_interval, ie_ptr);

        // Report Amount
        liblte_value_2_bits(rep_cnfg_eutra->report_amount, ie_ptr, 3);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_unpack_report_config_eutra_ie(uint8                                 **ie_ptr,
                                                           LIBLTE_RRC_REPORT_CONFIG_EUTRA_STRUCT  *rep_cnfg_eutra)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr         != NULL &&
       rep_cnfg_eutra != NULL)
    {
        // Extension indicator
        liblte_bits_2_value(ie_ptr, 1);

        // Trigger Type
        rep_cnfg_eutra->trigger_type = (LIBLTE_RRC_TRIGGER_TYPE_EUTRA_ENUM)liblte_bits_2_value(ie_ptr, 1);
        if(LIBLTE_RRC_TRIGGER_TYPE_EUTRA_EVENT == rep_cnfg_eutra->trigger_type)
        {
            // Event ID choice extension indicator
            liblte_bits_2_value(ie_ptr, 1);

            // Event ID
            rep_cnfg_eutra->event.event_id = (LIBLTE_RRC_EVENT_ID_EUTRA_ENUM)liblte_bits_2_value(ie_ptr, 3);

            if(LIBLTE_RRC_EVENT_ID_EUTRA_A1 == rep_cnfg_eutra->event.event_id)
            {
                // Threshold Type
                rep_cnfg_eutra->event.event_a1.eutra.type = (LIBLTE_RRC_THRESHOLD_EUTRA_TYPE_ENUM)liblte_bits_2_value(ie_ptr, 1);
                if(LIBLTE_RRC_THRESHOLD_EUTRA_TYPE_RSRP == rep_cnfg_eutra->event.event_a1.eutra.type)
                {
                    liblte_rrc_unpack_rsrp_range_ie(ie_ptr, &rep_cnfg_eutra->event.event_a1.eutra.range);
                }else{
                    liblte_rrc_unpack_rsrq_range_ie(ie_ptr, &rep_cnfg_eutra->event.event_a1.eutra.range);
                }
            }else if(LIBLTE_RRC_EVENT_ID_EUTRA_A2 == rep_cnfg_eutra->event.event_id){
                // Threshold Type
                rep_cnfg_eutra->event.event_a2.eutra.type = (LIBLTE_RRC_THRESHOLD_EUTRA_TYPE_ENUM)liblte_bits_2_value(ie_ptr, 1);
                if(LIBLTE_RRC_THRESHOLD_EUTRA_TYPE_RSRP == rep_cnfg_eutra->event.event_a2.eutra.type)
                {
                    liblte_rrc_unpack_rsrp_range_ie(ie_ptr, &rep_cnfg_eutra->event.event_a2.eutra.range);
                }else{
                    liblte_rrc_unpack_rsrq_range_ie(ie_ptr, &rep_cnfg_eutra->event.event_a2.eutra.range);
                }
            }else if(LIBLTE_RRC_EVENT_ID_EUTRA_A3 == rep_cnfg_eutra->event.event_id){
                // Offset
                rep_cnfg_eutra->event.event_a3.offset = liblte_bits_2_value(ie_ptr, 6) - 30;

                // Report On Leave
                rep_cnfg_eutra->event.event_a3.report_on_leave = liblte_bits_2_value(ie_ptr, 1);
            }else if(LIBLTE_RRC_EVENT_ID_EUTRA_A4 == rep_cnfg_eutra->event.event_id){
                // Threshold Type
                rep_cnfg_eutra->event.event_a4.eutra.type = (LIBLTE_RRC_THRESHOLD_EUTRA_TYPE_ENUM)liblte_bits_2_value(ie_ptr, 1);
                if(LIBLTE_RRC_THRESHOLD_EUTRA_TYPE_RSRP == rep_cnfg_eutra->event.event_a4.eutra.type)
                {
                    liblte_rrc_unpack_rsrp_range_ie(ie_ptr, &rep_cnfg_eutra->event.event_a4.eutra.range);
                }else{
                    liblte_rrc_unpack_rsrq_range_ie(ie_ptr, &rep_cnfg_eutra->event.event_a4.eutra.range);
                }
            }else if(LIBLTE_RRC_EVENT_ID_EUTRA_A5 == rep_cnfg_eutra->event.event_id){
                // Threshold1 Type
                rep_cnfg_eutra->event.event_a5.eutra1.type = (LIBLTE_RRC_THRESHOLD_EUTRA_TYPE_ENUM)liblte_bits_2_value(ie_ptr, 1);
                if(LIBLTE_RRC_THRESHOLD_EUTRA_TYPE_RSRP == rep_cnfg_eutra->event.event_a5.eutra1.type)
                {
                    liblte_rrc_unpack_rsrp_range_ie(ie_ptr, &rep_cnfg_eutra->event.event_a5.eutra1.range);
                }else{
                    liblte_rrc_unpack_rsrq_range_ie(ie_ptr, &rep_cnfg_eutra->event.event_a5.eutra1.range);
                }

                // Threshold2 Type
                rep_cnfg_eutra->event.event_a5.eutra2.type = (LIBLTE_RRC_THRESHOLD_EUTRA_TYPE_ENUM)liblte_bits_2_value(ie_ptr, 1);
                if(LIBLTE_RRC_THRESHOLD_EUTRA_TYPE_RSRP == rep_cnfg_eutra->event.event_a5.eutra2.type)
                {
                    liblte_rrc_unpack_rsrp_range_ie(ie_ptr, &rep_cnfg_eutra->event.event_a5.eutra2.range);
                }else{
                    liblte_rrc_unpack_rsrq_range_ie(ie_ptr, &rep_cnfg_eutra->event.event_a5.eutra2.range);
                }
            }else{
                // Offset
                rep_cnfg_eutra->event.event_a6.offset = liblte_bits_2_value(ie_ptr, 6) - 30;

                // Report On Leave
                rep_cnfg_eutra->event.event_a6.report_on_leave = liblte_bits_2_value(ie_ptr, 1);
            }

            // Hysteresis
            liblte_rrc_unpack_hysteresis_ie(ie_ptr, &rep_cnfg_eutra->event.hysteresis);

            // Time To Trigger
            liblte_rrc_unpack_time_to_trigger_ie(ie_ptr, &rep_cnfg_eutra->event.time_to_trigger);
        }else{
            // Purpose
            rep_cnfg_eutra->periodical.purpose = (LIBLTE_RRC_PURPOSE_EUTRA_ENUM)liblte_bits_2_value(ie_ptr, 1);
        }

        // Trigger Quantity
        rep_cnfg_eutra->trigger_quantity = (LIBLTE_RRC_TRIGGER_QUANTITY_ENUM)liblte_bits_2_value(ie_ptr, 1);

        // Report Quantity
        rep_cnfg_eutra->report_quantity = (LIBLTE_RRC_REPORT_QUANTITY_ENUM)liblte_bits_2_value(ie_ptr, 1);

        // Max Report Cells
        rep_cnfg_eutra->max_report_cells = liblte_bits_2_value(ie_ptr, 3) + 1;

        // Report Interval
        liblte_rrc_unpack_report_interval_ie(ie_ptr, &rep_cnfg_eutra->report_interval);

        // Report Amount
        rep_cnfg_eutra->report_amount = (LIBLTE_RRC_REPORT_AMOUNT_ENUM)liblte_bits_2_value(ie_ptr, 3);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: Report Config ID

    Description: Identifies a measurement reporting configuration

    Document Reference: 36.331 v10.0.0 Section 6.3.5
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_rrc_pack_report_config_id_ie(uint8   report_cnfg_id,
                                                      uint8 **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr != NULL)
    {
        liblte_value_2_bits(report_cnfg_id - 1, ie_ptr, 5);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_unpack_report_config_id_ie(uint8 **ie_ptr,
                                                        uint8  *report_cnfg_id)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr         != NULL &&
       report_cnfg_id != NULL)
    {
        *report_cnfg_id = liblte_bits_2_value(ie_ptr, 5) + 1;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: Report Config Inter RAT

    Description: Specifies criteria for triggering of an inter-RAT
                 measurement reporting event

    Document Reference: 36.331 v10.0.0 Section 6.3.5
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_rrc_pack_report_config_inter_rat_ie(LIBLTE_RRC_REPORT_CONFIG_INTER_RAT_STRUCT  *rep_cnfg_inter_rat,
                                                             uint8                                     **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(rep_cnfg_inter_rat != NULL &&
       ie_ptr             != NULL)
    {
        // Extension indicator
        liblte_value_2_bits(0, ie_ptr, 1);

        // Trigger Type
        liblte_value_2_bits(rep_cnfg_inter_rat->trigger_type, ie_ptr, 1);
        if(LIBLTE_RRC_TRIGGER_TYPE_INTER_RAT_EVENT == rep_cnfg_inter_rat->trigger_type)
        {
            // Event ID
            liblte_value_2_bits(rep_cnfg_inter_rat->event.event_id, ie_ptr, 1);
            if(LIBLTE_RRC_EVENT_ID_INTER_RAT_B1 == rep_cnfg_inter_rat->event.event_id)
            {
                // Threshold Type
                liblte_value_2_bits(rep_cnfg_inter_rat->event.event_b1.type, ie_ptr, 2);
                if(LIBLTE_RRC_THRESHOLD_INTER_RAT_TYPE_UTRA == rep_cnfg_inter_rat->event.event_b1.type)
                {
                    // Type
                    liblte_value_2_bits(rep_cnfg_inter_rat->event.event_b1.utra.type, ie_ptr, 1);
                    if(LIBLTE_RRC_THRESHOLD_UTRA_TYPE_RSCP == rep_cnfg_inter_rat->event.event_b1.utra.type)
                    {
                        liblte_value_2_bits(rep_cnfg_inter_rat->event.event_b1.utra.value + 5, ie_ptr, 7);
                    }else{
                        liblte_value_2_bits(rep_cnfg_inter_rat->event.event_b1.utra.value, ie_ptr, 6);
                    }
                }else if(LIBLTE_RRC_THRESHOLD_INTER_RAT_TYPE_GERAN == rep_cnfg_inter_rat->event.event_b1.type){
                    liblte_value_2_bits(rep_cnfg_inter_rat->event.event_b1.geran, ie_ptr, 6);
                }else{
                    liblte_value_2_bits(rep_cnfg_inter_rat->event.event_b1.cdma2000, ie_ptr, 6);
                }
            }else{
                // Threshold1 Type
                liblte_value_2_bits(rep_cnfg_inter_rat->event.event_b2.eutra.type, ie_ptr, 1);
                if(LIBLTE_RRC_THRESHOLD_EUTRA_TYPE_RSRP == rep_cnfg_inter_rat->event.event_b2.eutra.type)
                {
                    liblte_rrc_pack_rsrp_range_ie(rep_cnfg_inter_rat->event.event_b2.eutra.range, ie_ptr);
                }else{
                    liblte_rrc_pack_rsrq_range_ie(rep_cnfg_inter_rat->event.event_b2.eutra.range, ie_ptr);
                }

                // Threshold2 Type
                liblte_value_2_bits(rep_cnfg_inter_rat->event.event_b2.type2, ie_ptr, 2);
                if(LIBLTE_RRC_THRESHOLD_INTER_RAT_TYPE_UTRA == rep_cnfg_inter_rat->event.event_b2.type2)
                {
                    // Type
                    liblte_value_2_bits(rep_cnfg_inter_rat->event.event_b2.utra.type, ie_ptr, 1);
                    if(LIBLTE_RRC_THRESHOLD_UTRA_TYPE_RSCP == rep_cnfg_inter_rat->event.event_b2.utra.type)
                    {
                        liblte_value_2_bits(rep_cnfg_inter_rat->event.event_b2.utra.value + 5, ie_ptr, 7);
                    }else{
                        liblte_value_2_bits(rep_cnfg_inter_rat->event.event_b2.utra.value, ie_ptr, 6);
                    }
                }else if(LIBLTE_RRC_THRESHOLD_INTER_RAT_TYPE_GERAN == rep_cnfg_inter_rat->event.event_b2.type2){
                    liblte_value_2_bits(rep_cnfg_inter_rat->event.event_b2.geran, ie_ptr, 6);
                }else{
                    liblte_value_2_bits(rep_cnfg_inter_rat->event.event_b2.cdma2000, ie_ptr, 6);
                }
            }

            // Hysteresis
            liblte_rrc_pack_hysteresis_ie(rep_cnfg_inter_rat->event.hysteresis, ie_ptr);

            // Time To Trigger
            liblte_rrc_pack_time_to_trigger_ie(rep_cnfg_inter_rat->event.time_to_trigger, ie_ptr);
        }else{
            // Purpose
            liblte_value_2_bits(rep_cnfg_inter_rat->periodical.purpose, ie_ptr, 2);
        }

        // Max Report Cells
        liblte_value_2_bits(rep_cnfg_inter_rat->max_report_cells - 1, ie_ptr, 3);

        // Report Interval
        liblte_rrc_pack_report_interval_ie(rep_cnfg_inter_rat->report_interval, ie_ptr);

        // Report Amount
        liblte_value_2_bits(rep_cnfg_inter_rat->report_amount, ie_ptr, 3);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_unpack_report_config_inter_rat_ie(uint8                                     **ie_ptr,
                                                               LIBLTE_RRC_REPORT_CONFIG_INTER_RAT_STRUCT  *rep_cnfg_inter_rat)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr             != NULL &&
       rep_cnfg_inter_rat != NULL)
    {
        // Extension indicator
        liblte_bits_2_value(ie_ptr, 1);

        // Trigger Type
        rep_cnfg_inter_rat->trigger_type = (LIBLTE_RRC_TRIGGER_TYPE_INTER_RAT_ENUM)liblte_bits_2_value(ie_ptr, 1);
        if(LIBLTE_RRC_TRIGGER_TYPE_INTER_RAT_EVENT == rep_cnfg_inter_rat->trigger_type)
        {
            // Event ID
            rep_cnfg_inter_rat->event.event_id = (LIBLTE_RRC_EVENT_ID_INTER_RAT_ENUM)liblte_bits_2_value(ie_ptr, 1);
            if(LIBLTE_RRC_EVENT_ID_INTER_RAT_B1 == rep_cnfg_inter_rat->event.event_id)
            {
                // Threshold Type
                rep_cnfg_inter_rat->event.event_b1.type = (LIBLTE_RRC_THRESHOLD_INTER_RAT_TYPE_ENUM)liblte_bits_2_value(ie_ptr, 2);
                if(LIBLTE_RRC_THRESHOLD_INTER_RAT_TYPE_UTRA == rep_cnfg_inter_rat->event.event_b1.type)
                {
                    // Type
                    rep_cnfg_inter_rat->event.event_b1.utra.type = (LIBLTE_RRC_THRESHOLD_UTRA_TYPE_ENUM)liblte_bits_2_value(ie_ptr, 1);
                    if(LIBLTE_RRC_THRESHOLD_UTRA_TYPE_RSCP == rep_cnfg_inter_rat->event.event_b1.utra.type)
                    {
                        rep_cnfg_inter_rat->event.event_b1.utra.value = liblte_bits_2_value(ie_ptr, 7) - 5;
                    }else{
                        rep_cnfg_inter_rat->event.event_b1.utra.value = liblte_bits_2_value(ie_ptr, 6);
                    }
                }else if(LIBLTE_RRC_THRESHOLD_INTER_RAT_TYPE_GERAN == rep_cnfg_inter_rat->event.event_b1.type){
                    rep_cnfg_inter_rat->event.event_b1.geran = liblte_bits_2_value(ie_ptr, 6);
                }else{
                    rep_cnfg_inter_rat->event.event_b1.cdma2000 = liblte_bits_2_value(ie_ptr, 6);
                }
            }else{
                // Threshold1 Type
                rep_cnfg_inter_rat->event.event_b2.eutra.type = (LIBLTE_RRC_THRESHOLD_EUTRA_TYPE_ENUM)liblte_bits_2_value(ie_ptr, 1);
                if(LIBLTE_RRC_THRESHOLD_EUTRA_TYPE_RSRP == rep_cnfg_inter_rat->event.event_b2.eutra.type)
                {
                    liblte_rrc_unpack_rsrp_range_ie(ie_ptr, &rep_cnfg_inter_rat->event.event_b2.eutra.range);
                }else{
                    liblte_rrc_unpack_rsrq_range_ie(ie_ptr, &rep_cnfg_inter_rat->event.event_b2.eutra.range);
                }

                // Threshold2 Type
                rep_cnfg_inter_rat->event.event_b2.type2 = (LIBLTE_RRC_THRESHOLD_INTER_RAT_TYPE_ENUM)liblte_bits_2_value(ie_ptr, 2);
                if(LIBLTE_RRC_THRESHOLD_INTER_RAT_TYPE_UTRA == rep_cnfg_inter_rat->event.event_b2.type2)
                {
                    // Type
                    rep_cnfg_inter_rat->event.event_b2.utra.type = (LIBLTE_RRC_THRESHOLD_UTRA_TYPE_ENUM)liblte_bits_2_value(ie_ptr, 1);
                    if(LIBLTE_RRC_THRESHOLD_UTRA_TYPE_RSCP == rep_cnfg_inter_rat->event.event_b2.utra.type)
                    {
                        rep_cnfg_inter_rat->event.event_b2.utra.value = liblte_bits_2_value(ie_ptr, 7) - 5;
                    }else{
                        rep_cnfg_inter_rat->event.event_b2.utra.value = liblte_bits_2_value(ie_ptr, 6);
                    }
                }else if(LIBLTE_RRC_THRESHOLD_INTER_RAT_TYPE_GERAN == rep_cnfg_inter_rat->event.event_b2.type2){
                    rep_cnfg_inter_rat->event.event_b2.geran = liblte_bits_2_value(ie_ptr, 6);
                }else{
                    rep_cnfg_inter_rat->event.event_b2.cdma2000 = liblte_bits_2_value(ie_ptr, 6);
                }
            }

            // Hysteresis
            liblte_rrc_unpack_hysteresis_ie(ie_ptr, &rep_cnfg_inter_rat->event.hysteresis);

            // Time To Trigger
            liblte_rrc_unpack_time_to_trigger_ie(ie_ptr, &rep_cnfg_inter_rat->event.time_to_trigger);
        }else{
            // Purpose
            rep_cnfg_inter_rat->periodical.purpose = (LIBLTE_RRC_PURPOSE_INTER_RAT_ENUM)liblte_bits_2_value(ie_ptr, 2);
        }

        // Max Report Cells
        rep_cnfg_inter_rat->max_report_cells = liblte_bits_2_value(ie_ptr, 3) + 1;

        // Report Interval
        liblte_rrc_unpack_report_interval_ie(ie_ptr, &rep_cnfg_inter_rat->report_interval);

        // Report Amount
        rep_cnfg_inter_rat->report_amount = (LIBLTE_RRC_REPORT_AMOUNT_ENUM)liblte_bits_2_value(ie_ptr, 3);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: Report Config To Add Mod List

    Description: Concerns a list of reporting configurations to add
                 or modify

    Document Reference: 36.331 v10.0.0 Section 6.3.5
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_rrc_pack_report_config_to_add_mod_list_ie(LIBLTE_RRC_REPORT_CONFIG_TO_ADD_MOD_LIST_STRUCT  *list,
                                                                   uint8                                           **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;
    uint32            i;

    if(list   != NULL &&
       ie_ptr != NULL)
    {
        // List Size
        liblte_value_2_bits(list->N_rep_cnfg - 1, ie_ptr, 5);

        for(i=0; i<list->N_rep_cnfg; i++)
        {
            // Report Config ID
            liblte_rrc_pack_report_config_id_ie(list->rep_cnfg_list[i].rep_cnfg_id, ie_ptr);

            // Report Config Choice
            liblte_value_2_bits(list->rep_cnfg_list[i].rep_cnfg_type, ie_ptr, 1);

            if(LIBLTE_RRC_REPORT_CONFIG_TYPE_EUTRA == list->rep_cnfg_list[i].rep_cnfg_type)
            {
                liblte_rrc_pack_report_config_eutra_ie(&list->rep_cnfg_list[i].rep_cnfg_eutra, ie_ptr);
            }else{
                liblte_rrc_pack_report_config_inter_rat_ie(&list->rep_cnfg_list[i].rep_cnfg_inter_rat, ie_ptr);
            }
        }

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_unpack_report_config_to_add_mod_list_ie(uint8                                           **ie_ptr,
                                                                     LIBLTE_RRC_REPORT_CONFIG_TO_ADD_MOD_LIST_STRUCT  *list)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;
    uint32            i;

    if(ie_ptr != NULL &&
       list   != NULL)
    {
        // List Size
        list->N_rep_cnfg = liblte_bits_2_value(ie_ptr, 5) + 1;

        for(i=0; i<list->N_rep_cnfg; i++)
        {
            // Report Config ID
            liblte_rrc_unpack_report_config_id_ie(ie_ptr, &list->rep_cnfg_list[i].rep_cnfg_id);

            // Report Config Choice
            list->rep_cnfg_list[i].rep_cnfg_type = (LIBLTE_RRC_REPORT_CONFIG_TYPE_ENUM)liblte_bits_2_value(ie_ptr, 1);

            if(LIBLTE_RRC_REPORT_CONFIG_TYPE_EUTRA == list->rep_cnfg_list[i].rep_cnfg_type)
            {
                liblte_rrc_unpack_report_config_eutra_ie(ie_ptr, &list->rep_cnfg_list[i].rep_cnfg_eutra);
            }else{
                liblte_rrc_unpack_report_config_inter_rat_ie(ie_ptr, &list->rep_cnfg_list[i].rep_cnfg_inter_rat);
            }
        }

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: Report Interval

    Description: Indicates the interval between periodic reports

    Document Reference: 36.331 v10.0.0 Section 6.3.5
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_rrc_pack_report_interval_ie(LIBLTE_RRC_REPORT_INTERVAL_ENUM   report_int,
                                                     uint8                           **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr != NULL)
    {
        liblte_value_2_bits(report_int, ie_ptr, 4);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_unpack_report_interval_ie(uint8                           **ie_ptr,
                                                       LIBLTE_RRC_REPORT_INTERVAL_ENUM  *report_int)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr     != NULL &&
       report_int != NULL)
    {
        *report_int = (LIBLTE_RRC_REPORT_INTERVAL_ENUM)liblte_bits_2_value(ie_ptr, 4);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: RSRP Range

    Description: Specifies the value range used in RSRP measurements
                 and thresholds

    Document Reference: 36.331 v10.0.0 Section 6.3.5
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_rrc_pack_rsrp_range_ie(uint8   rsrp_range,
                                                uint8 **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr != NULL)
    {
        liblte_value_2_bits(rsrp_range, ie_ptr, 7);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_unpack_rsrp_range_ie(uint8 **ie_ptr,
                                                  uint8  *rsrp_range)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr     != NULL &&
       rsrp_range != NULL)
    {
        *rsrp_range = liblte_bits_2_value(ie_ptr, 7);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: RSRQ Range

    Description: Specifies the value range used in RSRQ measurements
                 and thresholds

    Document Reference: 36.331 v10.0.0 Section 6.3.5
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_rrc_pack_rsrq_range_ie(uint8   rsrq_range,
                                                uint8 **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr != NULL)
    {
        liblte_value_2_bits(rsrq_range, ie_ptr, 6);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_unpack_rsrq_range_ie(uint8 **ie_ptr,
                                                  uint8  *rsrq_range)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr     != NULL &&
       rsrq_range != NULL)
    {
        *rsrq_range = liblte_bits_2_value(ie_ptr, 6);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: Time To Trigger

    Description: Specifies the value range used for the time to
                 trigger parameter, which concerns the time during
                 which specific criteria for the event needs to be
                 met in order to trigger a measurement report

    Document Reference: 36.331 v10.0.0 Section 6.3.5
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_rrc_pack_time_to_trigger_ie(LIBLTE_RRC_TIME_TO_TRIGGER_ENUM   time_to_trigger,
                                                     uint8                           **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr != NULL)
    {
        liblte_value_2_bits(time_to_trigger, ie_ptr, 4);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_unpack_time_to_trigger_ie(uint8                           **ie_ptr,
                                                       LIBLTE_RRC_TIME_TO_TRIGGER_ENUM  *time_to_trigger)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr          != NULL &&
       time_to_trigger != NULL)
    {
        *time_to_trigger = (LIBLTE_RRC_TIME_TO_TRIGGER_ENUM)liblte_bits_2_value(ie_ptr, 4);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: Additional Spectrum Emission

    Description: FIXME

    Document Reference: 36.331 v10.0.0 Section 6.3.4
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_rrc_pack_additional_spectrum_emission_ie(uint8   add_spect_em,
                                                                  uint8 **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr != NULL)
    {
        liblte_value_2_bits(add_spect_em - 1, ie_ptr, 5);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_unpack_additional_spectrum_emission_ie(uint8 **ie_ptr,
                                                                    uint8  *add_spect_em)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr       != NULL &&
       add_spect_em != NULL)
    {
        *add_spect_em = liblte_bits_2_value(ie_ptr, 5) + 1;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: ARFCN value CDMA2000

    Description: Indicates the CDMA2000 carrier frequency within
                 a CDMA2000 band

    Document Reference: 36.331 v10.0.0 Section 6.3.4
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_rrc_pack_arfcn_value_cdma2000_ie(uint16   arfcn,
                                                          uint8  **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr != NULL)
    {
        liblte_value_2_bits(arfcn, ie_ptr, 11);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_unpack_arfcn_value_cdma2000_ie(uint8  **ie_ptr,
                                                            uint16  *arfcn)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr != NULL &&
       arfcn  != NULL)
    {
        *arfcn = liblte_bits_2_value(ie_ptr, 11);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: ARFCN value EUTRA

    Description: Indicates the ARFCN applicable for a downlink,
                 uplink, or bi-directional (TDD) E-UTRA carrier
                 frequency

    Document Reference: 36.331 v10.0.0 Section 6.3.4
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_rrc_pack_arfcn_value_eutra_ie(uint16   arfcn,
                                                       uint8  **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr != NULL)
    {
        liblte_value_2_bits(arfcn, ie_ptr, 16);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_unpack_arfcn_value_eutra_ie(uint8  **ie_ptr,
                                                         uint16  *arfcn)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr != NULL &&
       arfcn  != NULL)
    {
        *arfcn = liblte_bits_2_value(ie_ptr, 16);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: ARFCN value GERAN

    Description: Specifies the ARFCN value applicable for a GERAN
                 BCCH carrier frequency

    Document Reference: 36.331 v10.0.0 Section 6.3.4
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_rrc_pack_arfcn_value_geran_ie(uint16   arfcn,
                                                       uint8  **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr != NULL)
    {
        liblte_value_2_bits(arfcn, ie_ptr, 10);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_unpack_arfcn_value_geran_ie(uint8  **ie_ptr,
                                                         uint16  *arfcn)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr != NULL &&
       arfcn  != NULL)
    {
        *arfcn = liblte_bits_2_value(ie_ptr, 10);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: ARFCN value UTRA

    Description: Indicates the ARFCN applicable for a downlink (Nd,
                 FDD) or bi-directional (Nt, TDD) UTRA carrier
                 frequency

    Document Reference: 36.331 v10.0.0 Section 6.3.4
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_rrc_pack_arfcn_value_utra_ie(uint16   arfcn,
                                                      uint8  **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr != NULL)
    {
        liblte_value_2_bits(arfcn, ie_ptr, 14);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_unpack_arfcn_value_utra_ie(uint8  **ie_ptr,
                                                        uint16  *arfcn)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr != NULL &&
       arfcn  != NULL)
    {
        *arfcn = liblte_bits_2_value(ie_ptr, 14);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: Band Class CDMA2000

    Description: Defines the CDMA2000 band in which the CDMA2000
                 carrier frequency can be found

    Document Reference: 36.331 v10.0.0 Section 6.3.4
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_rrc_pack_band_class_cdma2000_ie(LIBLTE_RRC_BAND_CLASS_CDMA2000_ENUM   bc_cdma2000,
                                                         uint8                               **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr != NULL)
    {
        // Extension indicator
        liblte_value_2_bits(0, ie_ptr, 1);

        liblte_value_2_bits(bc_cdma2000, ie_ptr, 5);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_unpack_band_class_cdma2000_ie(uint8                               **ie_ptr,
                                                           LIBLTE_RRC_BAND_CLASS_CDMA2000_ENUM  *bc_cdma2000)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr      != NULL &&
       bc_cdma2000 != NULL)
    {
        // Extension indicator
        liblte_bits_2_value(ie_ptr, 1);

        *bc_cdma2000 = (LIBLTE_RRC_BAND_CLASS_CDMA2000_ENUM)liblte_bits_2_value(ie_ptr, 5);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: Band Indicator GERAN

    Description: Indicates how to interpret an associated GERAN
                 carrier ARFCN

    Document Reference: 36.331 v10.0.0 Section 6.3.4
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_rrc_pack_band_indicator_geran_ie(LIBLTE_RRC_BAND_INDICATOR_GERAN_ENUM   bi_geran,
                                                          uint8                                **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr != NULL)
    {
        liblte_value_2_bits(bi_geran, ie_ptr, 1);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_unpack_band_indicator_geran_ie(uint8                                **ie_ptr,
                                                            LIBLTE_RRC_BAND_INDICATOR_GERAN_ENUM  *bi_geran)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr   != NULL &&
       bi_geran != NULL)
    {
        *bi_geran = (LIBLTE_RRC_BAND_INDICATOR_GERAN_ENUM)liblte_bits_2_value(ie_ptr, 1);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: Carrier Freq CDMA2000

    Description: Provides the CDMA2000 carrier information

    Document Reference: 36.331 v10.0.0 Section 6.3.4
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_rrc_pack_carrier_freq_cdma2000_ie(LIBLTE_RRC_CARRIER_FREQ_CDMA2000_STRUCT  *carrier_freq,
                                                           uint8                                   **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(carrier_freq != NULL &&
       ie_ptr       != NULL)
    {
        liblte_rrc_pack_band_class_cdma2000_ie(carrier_freq->bandclass, ie_ptr);
        liblte_rrc_pack_arfcn_value_cdma2000_ie(carrier_freq->arfcn, ie_ptr);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_unpack_carrier_freq_cdma2000_ie(uint8                                   **ie_ptr,
                                                             LIBLTE_RRC_CARRIER_FREQ_CDMA2000_STRUCT  *carrier_freq)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr       != NULL &&
       carrier_freq != NULL)
    {
        liblte_rrc_unpack_band_class_cdma2000_ie(ie_ptr, &carrier_freq->bandclass);
        liblte_rrc_unpack_arfcn_value_cdma2000_ie(ie_ptr, &carrier_freq->arfcn);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: Carrier Freq GERAN

    Description: Provides an unambiguous carrier frequency description
                 of a GERAN cell

    Document Reference: 36.331 v10.0.0 Section 6.3.4
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_rrc_pack_carrier_freq_geran_ie(LIBLTE_RRC_CARRIER_FREQ_GERAN_STRUCT  *carrier_freq,
                                                        uint8                                **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(carrier_freq != NULL &&
       ie_ptr       != NULL)
    {
        liblte_rrc_pack_arfcn_value_geran_ie(carrier_freq->arfcn, ie_ptr);
        liblte_rrc_pack_band_indicator_geran_ie(carrier_freq->band_indicator, ie_ptr);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_unpack_carrier_freq_geran_ie(uint8                                **ie_ptr,
                                                          LIBLTE_RRC_CARRIER_FREQ_GERAN_STRUCT  *carrier_freq)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr       != NULL &&
       carrier_freq != NULL)
    {
        liblte_rrc_unpack_arfcn_value_geran_ie(ie_ptr, &carrier_freq->arfcn);
        liblte_rrc_unpack_band_indicator_geran_ie(ie_ptr, &carrier_freq->band_indicator);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: Carrier Freqs GERAN

    Description: Provides one or more GERAN ARFCN values, which
                 represent a list of GERAN BCCH carrier frequencies

    Document Reference: 36.331 v10.0.0 Section 6.3.4
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_rrc_pack_carrier_freqs_geran_ie(LIBLTE_RRC_CARRIER_FREQS_GERAN_STRUCT  *carrier_freqs,
                                                         uint8                                 **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;
    uint32            i;

    if(carrier_freqs != NULL &&
       ie_ptr        != NULL)
    {
        liblte_rrc_pack_arfcn_value_geran_ie(carrier_freqs->starting_arfcn, ie_ptr);
        liblte_rrc_pack_band_indicator_geran_ie(carrier_freqs->band_indicator, ie_ptr);
        liblte_value_2_bits(carrier_freqs->following_arfcns, ie_ptr, 2);
        if(LIBLTE_RRC_FOLLOWING_ARFCNS_EXPLICIT_LIST == carrier_freqs->following_arfcns)
        {
            liblte_value_2_bits(carrier_freqs->explicit_list_of_arfcns_size, ie_ptr, 5);
            for(i=0; i<carrier_freqs->explicit_list_of_arfcns_size; i++)
            {
                liblte_rrc_pack_arfcn_value_geran_ie(carrier_freqs->explicit_list_of_arfcns[i], ie_ptr);
            }
        }else if(LIBLTE_RRC_FOLLOWING_ARFCNS_EQUALLY_SPACED == carrier_freqs->following_arfcns){
            liblte_value_2_bits(carrier_freqs->equally_spaced_arfcns.arfcn_spacing - 1, ie_ptr, 3);
            liblte_value_2_bits(carrier_freqs->equally_spaced_arfcns.number_of_arfcns,  ie_ptr, 5);
        }else{ // LIBLTE_RRC_FOLLOWING_ARFCNS_VARIABLE_BIT_MAP == carrier_freqs->following_arfcns
            liblte_value_2_bits(carrier_freqs->variable_bit_map_of_arfcns, ie_ptr, 16);
        }

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_unpack_carrier_freqs_geran_ie(uint8                                 **ie_ptr,
                                                           LIBLTE_RRC_CARRIER_FREQS_GERAN_STRUCT  *carrier_freqs)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;
    uint32            i;

    if(ie_ptr        != NULL &&
       carrier_freqs != NULL)
    {
        liblte_rrc_unpack_arfcn_value_geran_ie(ie_ptr, &carrier_freqs->starting_arfcn);
        liblte_rrc_unpack_band_indicator_geran_ie(ie_ptr, &carrier_freqs->band_indicator);
        carrier_freqs->following_arfcns = (LIBLTE_RRC_FOLLOWING_ARFCNS_ENUM)liblte_bits_2_value(ie_ptr, 2);
        if(LIBLTE_RRC_FOLLOWING_ARFCNS_EXPLICIT_LIST == carrier_freqs->following_arfcns)
        {
            carrier_freqs->explicit_list_of_arfcns_size = liblte_bits_2_value(ie_ptr, 5);
            for(i=0; i<carrier_freqs->explicit_list_of_arfcns_size; i++)
            {
                liblte_rrc_unpack_arfcn_value_geran_ie(ie_ptr, &carrier_freqs->explicit_list_of_arfcns[i]);
            }
        }else if(LIBLTE_RRC_FOLLOWING_ARFCNS_EQUALLY_SPACED == carrier_freqs->following_arfcns){
            carrier_freqs->equally_spaced_arfcns.arfcn_spacing    = liblte_bits_2_value(ie_ptr, 3) + 1;
            carrier_freqs->equally_spaced_arfcns.number_of_arfcns = liblte_bits_2_value(ie_ptr, 5);
        }else{ // LIBLTE_RRC_FOLLOWING_ARFCNS_VARIABLE_BIT_MAP == carrier_freqs->following_arfcns
            carrier_freqs->variable_bit_map_of_arfcns = liblte_bits_2_value(ie_ptr, 16);
        }

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: CDMA2000 Type

    Description: Describes the type of CDMA2000 network

    Document Reference: 36.331 v10.0.0 Section 6.3.4
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_rrc_pack_cdma2000_type_ie(LIBLTE_RRC_CDMA2000_TYPE_ENUM   cdma2000_type,
                                                   uint8                         **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr != NULL)
    {
        liblte_value_2_bits(cdma2000_type, ie_ptr, 1);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_unpack_cdma2000_type_ie(uint8                         **ie_ptr,
                                                     LIBLTE_RRC_CDMA2000_TYPE_ENUM  *cdma2000_type)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr        != NULL &&
       cdma2000_type != NULL)
    {
        *cdma2000_type = (LIBLTE_RRC_CDMA2000_TYPE_ENUM)liblte_bits_2_value(ie_ptr, 1);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: Cell Identity

    Description: Unambiguously identifies a cell within a PLMN

    Document Reference: 36.331 v10.0.0 Section 6.3.4
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_rrc_pack_cell_identity_ie(uint32   cell_id,
                                                   uint8  **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr != NULL)
    {
        liblte_value_2_bits(cell_id, ie_ptr, 28);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_unpack_cell_identity_ie(uint8  **ie_ptr,
                                                     uint32  *cell_id)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr  != NULL &&
       cell_id != NULL)
    {
        *cell_id = liblte_bits_2_value(ie_ptr, 28);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: Cell Index List

    Description: Concerns a list of cell indecies, which may be used
                 for different purposes

    Document Reference: 36.331 v10.0.0 Section 6.3.4
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_rrc_pack_cell_index_list_ie(LIBLTE_RRC_CELL_INDEX_LIST_STRUCT  *cell_idx_list,
                                                     uint8                             **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;
    uint32            i;

    if(cell_idx_list != NULL &&
       ie_ptr        != NULL)
    {
        // List Size
        liblte_value_2_bits(cell_idx_list->N_cell_idx - 1, ie_ptr, 5);

        for(i=0; i<cell_idx_list->N_cell_idx; i++)
        {
            // Cell Index
            liblte_value_2_bits(cell_idx_list->cell_idx[i] - 1, ie_ptr, 5);
        }

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_unpack_cell_index_list_ie(uint8                             **ie_ptr,
                                                       LIBLTE_RRC_CELL_INDEX_LIST_STRUCT  *cell_idx_list)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;
    uint32            i;

    if(ie_ptr        != NULL &&
       cell_idx_list != NULL)
    {
        // List Size
        cell_idx_list->N_cell_idx = liblte_bits_2_value(ie_ptr, 5) + 1;

        for(i=0; i<cell_idx_list->N_cell_idx; i++)
        {
            // Cell Index
            cell_idx_list->cell_idx[i] = liblte_bits_2_value(ie_ptr, 5) + 1;
        }

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: Cell Reselection Priority

    Description: Contains the absolute priority of the concerned
                 carrier frequency/set of frequencies (GERAN)/
                 bandclass (CDMA2000), as used by the cell
                 reselection procedure

    Document Reference: 36.331 v10.0.0 Section 6.3.4
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_rrc_pack_cell_reselection_priority_ie(uint8   cell_resel_prio,
                                                               uint8 **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr != NULL)
    {
        liblte_value_2_bits(cell_resel_prio, ie_ptr, 3);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_unpack_cell_reselection_priority_ie(uint8 **ie_ptr,
                                                                 uint8  *cell_resel_prio)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr          != NULL &&
       cell_resel_prio != NULL)
    {
        *cell_resel_prio = liblte_bits_2_value(ie_ptr, 3);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: CSFB Registration Param 1xRTT

    Description: Indicates whether or not the UE shall perform a
                 CDMA2000 1xRTT pre-registration if the UE does not
                 have a valid/current pre-registration

    Document Reference: 36.331 v10.0.0 Section 6.3.4
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_rrc_pack_csfb_registration_param_1xrtt_ie(LIBLTE_RRC_CSFB_REGISTRATION_PARAM_1XRTT_STRUCT  *csfb_reg_param,
                                                                   uint8                                           **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(csfb_reg_param != NULL &&
       ie_ptr         != NULL)
    {
        liblte_value_2_bits(csfb_reg_param->sid,             ie_ptr, 15);
        liblte_value_2_bits(csfb_reg_param->nid,             ie_ptr, 16);
        liblte_value_2_bits(csfb_reg_param->multiple_sid,    ie_ptr,  1);
        liblte_value_2_bits(csfb_reg_param->multiple_nid,    ie_ptr,  1);
        liblte_value_2_bits(csfb_reg_param->home_reg,        ie_ptr,  1);
        liblte_value_2_bits(csfb_reg_param->foreign_sid_reg, ie_ptr,  1);
        liblte_value_2_bits(csfb_reg_param->foreign_nid_reg, ie_ptr,  1);
        liblte_value_2_bits(csfb_reg_param->param_reg,       ie_ptr,  1);
        liblte_value_2_bits(csfb_reg_param->power_up_reg,    ie_ptr,  1);
        liblte_value_2_bits(csfb_reg_param->reg_period,      ie_ptr,  7);
        liblte_value_2_bits(csfb_reg_param->reg_zone,        ie_ptr, 12);
        liblte_value_2_bits(csfb_reg_param->total_zone,      ie_ptr,  3);
        liblte_value_2_bits(csfb_reg_param->zone_timer,      ie_ptr,  3);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_unpack_csfb_registration_param_1xrtt_ie(uint8                                           **ie_ptr,
                                                                     LIBLTE_RRC_CSFB_REGISTRATION_PARAM_1XRTT_STRUCT  *csfb_reg_param)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr         != NULL &&
       csfb_reg_param != NULL)
    {
        csfb_reg_param->sid             = liblte_bits_2_value(ie_ptr, 15);
        csfb_reg_param->nid             = liblte_bits_2_value(ie_ptr, 16);
        csfb_reg_param->multiple_sid    = liblte_bits_2_value(ie_ptr,  1);
        csfb_reg_param->multiple_nid    = liblte_bits_2_value(ie_ptr,  1);
        csfb_reg_param->home_reg        = liblte_bits_2_value(ie_ptr,  1);
        csfb_reg_param->foreign_sid_reg = liblte_bits_2_value(ie_ptr,  1);
        csfb_reg_param->foreign_nid_reg = liblte_bits_2_value(ie_ptr,  1);
        csfb_reg_param->param_reg       = liblte_bits_2_value(ie_ptr,  1);
        csfb_reg_param->power_up_reg    = liblte_bits_2_value(ie_ptr,  1);
        csfb_reg_param->reg_period      = liblte_bits_2_value(ie_ptr,  7);
        csfb_reg_param->reg_zone        = liblte_bits_2_value(ie_ptr, 12);
        csfb_reg_param->total_zone      = liblte_bits_2_value(ie_ptr,  3);
        csfb_reg_param->zone_timer      = liblte_bits_2_value(ie_ptr,  3);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_pack_csfb_registration_param_1xrtt_v920_ie(LIBLTE_RRC_CSFB_REGISTRATION_PARAM_1XRTT_V920_STRUCT  *csfb_reg_param,
                                                                        uint8                                                **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(csfb_reg_param != NULL &&
       ie_ptr         != NULL)
    {
        liblte_value_2_bits(csfb_reg_param->power_down_reg, ie_ptr, 1);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_unpack_csfb_registration_param_1xrtt_v920_ie(uint8                                                **ie_ptr,
                                                                          LIBLTE_RRC_CSFB_REGISTRATION_PARAM_1XRTT_V920_STRUCT  *csfb_reg_param)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr         != NULL &&
       csfb_reg_param != NULL)
    {
        csfb_reg_param->power_down_reg = (LIBLTE_RRC_POWER_DOWN_REG_R9_ENUM)liblte_bits_2_value(ie_ptr, 1);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: Cell Global ID EUTRA

    Description: Specifies the Evolved Cell Global Identifier (ECGI),
                 the globally unique identity of a cell in E-UTRA

    Document Reference: 36.331 v10.0.0 Section 6.3.4
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_rrc_pack_cell_global_id_eutra_ie(LIBLTE_RRC_CELL_GLOBAL_ID_EUTRA_STRUCT  *cell_global_id,
                                                          uint8                                  **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(cell_global_id != NULL &&
       ie_ptr         != NULL)
    {
        liblte_rrc_pack_plmn_identity_ie(&cell_global_id->plmn_id, ie_ptr);
        liblte_rrc_pack_cell_identity_ie(cell_global_id->cell_id, ie_ptr);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_unpack_cell_global_id_eutra_ie(uint8                                  **ie_ptr,
                                                            LIBLTE_RRC_CELL_GLOBAL_ID_EUTRA_STRUCT  *cell_global_id)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr         != NULL &&
       cell_global_id != NULL)
    {
        liblte_rrc_unpack_plmn_identity_ie(ie_ptr, &cell_global_id->plmn_id);
        liblte_rrc_unpack_cell_identity_ie(ie_ptr, &cell_global_id->cell_id);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: Cell Global ID UTRA

    Description: Specifies the global UTRAN Cell Identifier, the
                 globally unique identity of a cell in UTRA

    Document Reference: 36.331 v10.0.0 Section 6.3.4
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_rrc_pack_cell_global_id_utra_ie(LIBLTE_RRC_CELL_GLOBAL_ID_UTRA_STRUCT  *cell_global_id,
                                                         uint8                                 **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(cell_global_id != NULL &&
       ie_ptr         != NULL)
    {
        liblte_rrc_pack_plmn_identity_ie(&cell_global_id->plmn_id, ie_ptr);
        liblte_value_2_bits(cell_global_id->cell_id, ie_ptr, 28);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_unpack_cell_global_id_utra_ie(uint8                                 **ie_ptr,
                                                           LIBLTE_RRC_CELL_GLOBAL_ID_UTRA_STRUCT  *cell_global_id)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr         != NULL &&
       cell_global_id != NULL)
    {
        liblte_rrc_unpack_plmn_identity_ie(ie_ptr, &cell_global_id->plmn_id);
        cell_global_id->cell_id = liblte_bits_2_value(ie_ptr, 28);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: Cell Global ID GERAN

    Description: Specifies the Cell Global Identity (CGI), the
                 globally unique identity of a cell in GERAN

    Document Reference: 36.331 v10.0.0 Section 6.3.4
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_rrc_pack_cell_global_id_geran_ie(LIBLTE_RRC_CELL_GLOBAL_ID_GERAN_STRUCT  *cell_global_id,
                                                          uint8                                  **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(cell_global_id != NULL &&
       ie_ptr         != NULL)
    {
        liblte_rrc_pack_plmn_identity_ie(&cell_global_id->plmn_id, ie_ptr);
        liblte_value_2_bits(cell_global_id->lac,     ie_ptr, 16);
        liblte_value_2_bits(cell_global_id->cell_id, ie_ptr, 16);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_unpack_cell_global_id_geran_ie(uint8                                  **ie_ptr,
                                                            LIBLTE_RRC_CELL_GLOBAL_ID_GERAN_STRUCT  *cell_global_id)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr         != NULL &&
       cell_global_id != NULL)
    {
        liblte_rrc_unpack_plmn_identity_ie(ie_ptr, &cell_global_id->plmn_id);
        cell_global_id->lac     = liblte_bits_2_value(ie_ptr, 16);
        cell_global_id->cell_id = liblte_bits_2_value(ie_ptr, 16);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: Cell Global ID CDMA2000

    Description: Specifies the Cell Global Identity (CGI), the
                 globally unique identity of a cell in CDMA2000

    Document Reference: 36.331 v10.0.0 Section 6.3.4
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_rrc_pack_cell_global_id_cdma2000_ie(LIBLTE_RRC_CELL_GLOBAL_ID_CDMA2000_STRUCT  *cell_global_id,
                                                             uint8                                     **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(cell_global_id != NULL &&
       ie_ptr         != NULL)
    {
        liblte_value_2_bits((uint32)(cell_global_id->onexrtt >> 15),       ie_ptr, 32);
        liblte_value_2_bits((uint32)(cell_global_id->onexrtt & 0x7FFFULL), ie_ptr, 15);
        liblte_value_2_bits(cell_global_id->hrpd[0],                       ie_ptr, 32);
        liblte_value_2_bits(cell_global_id->hrpd[1],                       ie_ptr, 32);
        liblte_value_2_bits(cell_global_id->hrpd[2],                       ie_ptr, 32);
        liblte_value_2_bits(cell_global_id->hrpd[3],                       ie_ptr, 32);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_unpack_cell_global_id_cdma2000_ie(uint8                                     **ie_ptr,
                                                               LIBLTE_RRC_CELL_GLOBAL_ID_CDMA2000_STRUCT  *cell_global_id)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr         != NULL &&
       cell_global_id != NULL)
    {
        cell_global_id->onexrtt  = (uint64)liblte_bits_2_value(ie_ptr, 32) << 15;
        cell_global_id->onexrtt |= (uint64)liblte_bits_2_value(ie_ptr, 15);
        cell_global_id->hrpd[0]  = liblte_bits_2_value(ie_ptr, 32);
        cell_global_id->hrpd[1]  = liblte_bits_2_value(ie_ptr, 32);
        cell_global_id->hrpd[2]  = liblte_bits_2_value(ie_ptr, 32);
        cell_global_id->hrpd[3]  = liblte_bits_2_value(ie_ptr, 32);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: CSG Identity

    Description: Identifies a Closed Subscriber Group

    Document Reference: 36.331 v10.0.0 Section 6.3.4
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_rrc_pack_csg_identity_ie(uint32   csg_id,
                                                  uint8  **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr != NULL)
    {
        liblte_value_2_bits(csg_id, ie_ptr, 27);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_unpack_csg_identity_ie(uint8  **ie_ptr,
                                                    uint32  *csg_id)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr != NULL &&
       csg_id != NULL)
    {
        *csg_id = liblte_bits_2_value(ie_ptr, 27);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: Mobility Control Info

    Description: Includes parameters relevant for network controlled
                 mobility to/within E-UTRA

    Document Reference: 36.331 v10.0.0 Section 6.3.4
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_rrc_pack_mobility_control_info_ie(LIBLTE_RRC_MOBILITY_CONTROL_INFO_STRUCT  *mob_ctrl_info,
                                                           uint8                                   **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(mob_ctrl_info != NULL &&
       ie_ptr        != NULL)
    {
        // Extension indicator
        liblte_value_2_bits(0, ie_ptr, 1);

        // Optional indicators
        liblte_value_2_bits(mob_ctrl_info->carrier_freq_eutra_present, ie_ptr, 1);
        liblte_value_2_bits(mob_ctrl_info->carrier_bw_eutra_present,   ie_ptr, 1);
        liblte_value_2_bits(mob_ctrl_info->add_spect_em_present,       ie_ptr, 1);
        liblte_value_2_bits(mob_ctrl_info->rach_cnfg_ded_present,      ie_ptr, 1);

        // Target Phys Cell ID
        liblte_rrc_pack_phys_cell_id_ie(mob_ctrl_info->target_pci, ie_ptr);

        // Carrier Freq
        if(mob_ctrl_info->carrier_freq_eutra_present)
        {
            // Optional indicator
            liblte_value_2_bits(mob_ctrl_info->carrier_freq_eutra.ul_carrier_freq_present, ie_ptr, 1);

            // DL Carrier Freq
            liblte_rrc_pack_arfcn_value_eutra_ie(mob_ctrl_info->carrier_freq_eutra.dl_carrier_freq, ie_ptr);

            // UL Carrier Freq
            if(mob_ctrl_info->carrier_freq_eutra.ul_carrier_freq_present)
            {
                liblte_rrc_pack_arfcn_value_eutra_ie(mob_ctrl_info->carrier_freq_eutra.ul_carrier_freq, ie_ptr);
            }
        }

        // Carrier Bandwidth
        if(mob_ctrl_info->carrier_bw_eutra_present)
        {
            // Optional indicator
            liblte_value_2_bits(mob_ctrl_info->carrier_bw_eutra.ul_bw_present, ie_ptr, 1);

            // DL Bandwidth
            liblte_value_2_bits(mob_ctrl_info->carrier_bw_eutra.dl_bw, ie_ptr, 4);

            // UL Bandwidth
            if(mob_ctrl_info->carrier_bw_eutra.ul_bw_present)
            {
                liblte_value_2_bits(mob_ctrl_info->carrier_bw_eutra.ul_bw, ie_ptr, 4);
            }
        }

        // Additional Spectrum Emission
        if(mob_ctrl_info->add_spect_em_present)
        {
            liblte_rrc_pack_additional_spectrum_emission_ie(mob_ctrl_info->add_spect_em, ie_ptr);
        }

        // T304
        liblte_value_2_bits(mob_ctrl_info->t304, ie_ptr, 3);

        // New UE Identity
        liblte_rrc_pack_c_rnti_ie(mob_ctrl_info->new_ue_id, ie_ptr);

        // Radio Resource Config Common
        liblte_rrc_pack_rr_config_common_ie(&mob_ctrl_info->rr_cnfg_common, ie_ptr);

        // RACH Config Dedicated
        if(mob_ctrl_info->rach_cnfg_ded_present)
        {
            liblte_rrc_pack_rach_config_dedicated_ie(&mob_ctrl_info->rach_cnfg_ded, ie_ptr);
        }

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_unpack_mobility_control_info_ie(uint8                                   **ie_ptr,
                                                             LIBLTE_RRC_MOBILITY_CONTROL_INFO_STRUCT  *mob_ctrl_info)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr        != NULL &&
       mob_ctrl_info != NULL)
    {
        // Extension indicator
        liblte_bits_2_value(ie_ptr, 1);

        // Optional indicators
        mob_ctrl_info->carrier_freq_eutra_present = liblte_bits_2_value(ie_ptr, 1);
        mob_ctrl_info->carrier_bw_eutra_present   = liblte_bits_2_value(ie_ptr, 1);
        mob_ctrl_info->add_spect_em_present       = liblte_bits_2_value(ie_ptr, 1);
        mob_ctrl_info->rach_cnfg_ded_present      = liblte_bits_2_value(ie_ptr, 1);

        // Target Phys Cell ID
        liblte_rrc_unpack_phys_cell_id_ie(ie_ptr, &mob_ctrl_info->target_pci);

        // Carrier Freq
        if(mob_ctrl_info->carrier_freq_eutra_present)
        {
            // Optional indicator
            mob_ctrl_info->carrier_freq_eutra.ul_carrier_freq_present = liblte_bits_2_value(ie_ptr, 1);

            // DL Carrier Freq
            liblte_rrc_unpack_arfcn_value_eutra_ie(ie_ptr, &mob_ctrl_info->carrier_freq_eutra.dl_carrier_freq);

            // UL Carrier Freq
            if(mob_ctrl_info->carrier_freq_eutra.ul_carrier_freq_present)
            {
                liblte_rrc_unpack_arfcn_value_eutra_ie(ie_ptr, &mob_ctrl_info->carrier_freq_eutra.ul_carrier_freq);
            }
        }

        // Carrier Bandwidth
        if(mob_ctrl_info->carrier_bw_eutra_present)
        {
            // Optional indicator
            mob_ctrl_info->carrier_bw_eutra.ul_bw_present = liblte_bits_2_value(ie_ptr, 1);

            // DL Bandwidth
            mob_ctrl_info->carrier_bw_eutra.dl_bw = (LIBLTE_RRC_BANDWIDTH_ENUM)liblte_bits_2_value(ie_ptr, 4);

            // UL Bandwidth
            if(mob_ctrl_info->carrier_bw_eutra.ul_bw_present)
            {
                mob_ctrl_info->carrier_bw_eutra.ul_bw = (LIBLTE_RRC_BANDWIDTH_ENUM)liblte_bits_2_value(ie_ptr, 4);
            }
        }

        // Additional Spectrum Emission
        if(mob_ctrl_info->add_spect_em_present)
        {
            liblte_rrc_unpack_additional_spectrum_emission_ie(ie_ptr, &mob_ctrl_info->add_spect_em);
        }

        // T304
        mob_ctrl_info->t304 = (LIBLTE_RRC_T304_ENUM)liblte_bits_2_value(ie_ptr, 3);

        // New UE Identity
        liblte_rrc_unpack_c_rnti_ie(ie_ptr, &mob_ctrl_info->new_ue_id);

        // Radio Resource Config Common
        liblte_rrc_unpack_rr_config_common_ie(ie_ptr, &mob_ctrl_info->rr_cnfg_common);

        // RACH Config Dedicated
        if(mob_ctrl_info->rach_cnfg_ded_present)
        {
            liblte_rrc_unpack_rach_config_dedicated_ie(ie_ptr, &mob_ctrl_info->rach_cnfg_ded);
        }

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: Mobility Parameters CDMA2000 (1xRTT)

    Description: Contains the parameters provided to the UE for
                 handover and (enhanced) CSFB to 1xRTT support

    Document Reference: 36.331 v10.0.0 Section 6.3.4
*********************************************************************/
// FIXME

/*********************************************************************
    IE Name: Mobility State Parameters

    Description: Contains parameters to determine UE mobility state

    Document Reference: 36.331 v10.0.0 Section 6.3.4
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_rrc_pack_mobility_state_parameters_ie(LIBLTE_RRC_MOBILITY_STATE_PARAMETERS_STRUCT  *mobility_state_params,
                                                               uint8                                       **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(mobility_state_params != NULL &&
       ie_ptr                != NULL)
    {
        liblte_value_2_bits(mobility_state_params->t_eval,                   ie_ptr, 3);
        liblte_value_2_bits(mobility_state_params->t_hyst_normal,            ie_ptr, 3);
        liblte_value_2_bits(mobility_state_params->n_cell_change_medium - 1, ie_ptr, 4);
        liblte_value_2_bits(mobility_state_params->n_cell_change_high - 1,   ie_ptr, 4);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_unpack_mobility_state_parameters_ie(uint8                                       **ie_ptr,
                                                                 LIBLTE_RRC_MOBILITY_STATE_PARAMETERS_STRUCT  *mobility_state_params)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr                != NULL &&
       mobility_state_params != NULL)
    {
        mobility_state_params->t_eval               = (LIBLTE_RRC_T_EVALUATION_ENUM)liblte_bits_2_value(ie_ptr, 3);
        mobility_state_params->t_hyst_normal        = (LIBLTE_RRC_T_HYST_NORMAL_ENUM)liblte_bits_2_value(ie_ptr, 3);
        mobility_state_params->n_cell_change_medium = liblte_bits_2_value(ie_ptr, 4) + 1;
        mobility_state_params->n_cell_change_high   = liblte_bits_2_value(ie_ptr, 4) + 1;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: Phys Cell ID

    Description: Indicates the physical layer identity of the cell

    Document Reference: 36.331 v10.0.0 Section 6.3.4
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_rrc_pack_phys_cell_id_ie(uint16   phys_cell_id,
                                                  uint8  **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr != NULL)
    {
        liblte_value_2_bits(phys_cell_id, ie_ptr, 9);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_unpack_phys_cell_id_ie(uint8  **ie_ptr,
                                                    uint16  *phys_cell_id)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr       != NULL &&
       phys_cell_id != NULL)
    {
        *phys_cell_id = liblte_bits_2_value(ie_ptr, 9);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: Phys Cell ID Range

    Description: Encodes either a single or a range of physical cell
                 identities

    Document Reference: 36.331 v10.0.0 Section 6.3.4
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_rrc_pack_phys_cell_id_range_ie(LIBLTE_RRC_PHYS_CELL_ID_RANGE_STRUCT  *phys_cell_id_range,
                                                        uint8                                **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;
    bool              opt;

    if(phys_cell_id_range != NULL &&
       ie_ptr             != NULL)
    {
        opt = (LIBLTE_RRC_PHYS_CELL_ID_RANGE_N1 != phys_cell_id_range->range);

        liblte_value_2_bits(opt, ie_ptr, 1);

        liblte_rrc_pack_phys_cell_id_ie(phys_cell_id_range->start, ie_ptr);

        if(opt)
        {
            liblte_value_2_bits(phys_cell_id_range->range, ie_ptr, 4);
        }

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_unpack_phys_cell_id_range_ie(uint8                                **ie_ptr,
                                                          LIBLTE_RRC_PHYS_CELL_ID_RANGE_STRUCT  *phys_cell_id_range)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;
    bool              opt;

    if(ie_ptr             != NULL &&
       phys_cell_id_range != NULL)
    {
        opt = liblte_bits_2_value(ie_ptr, 1);

        liblte_rrc_unpack_phys_cell_id_ie(ie_ptr, &phys_cell_id_range->start);

        if(true == opt)
        {
            phys_cell_id_range->range = (LIBLTE_RRC_PHYS_CELL_ID_RANGE_ENUM)liblte_bits_2_value(ie_ptr, 4);
        }else{
            phys_cell_id_range->range = LIBLTE_RRC_PHYS_CELL_ID_RANGE_N1;
        }

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: Phys Cell ID Range UTRA FDD List

    Description: Encodes one or more of Phys Cell ID Range UTRA FDD

    Document Reference: 36.331 v10.0.0 Section 6.3.4
*********************************************************************/
// FIXME

/*********************************************************************
    IE Name: Phys Cell ID CDMA2000

    Description: Identifies the PN offset that represents the
                 "Physical cell identity" in CDMA2000

    Document Reference: 36.331 v10.0.0 Section 6.3.4
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_rrc_pack_phys_cell_id_cdma2000_ie(uint16   phys_cell_id,
                                                           uint8  **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr != NULL)
    {
        liblte_value_2_bits(phys_cell_id, ie_ptr, 9);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_unpack_phys_cell_id_cdma2000_ie(uint8  **ie_ptr,
                                                             uint16  *phys_cell_id)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr       != NULL &&
       phys_cell_id != NULL)
    {
        *phys_cell_id = liblte_bits_2_value(ie_ptr, 9);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: Phys Cell ID GERAN

    Description: Contains the Base Station Identity Code (BSIC)

    Document Reference: 36.331 v10.0.0 Section 6.3.4
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_rrc_pack_phys_cell_id_geran_ie(LIBLTE_RRC_PHYS_CELL_ID_GERAN_STRUCT  *phys_cell_id,
                                                        uint8                                **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(phys_cell_id != NULL &&
       ie_ptr       != NULL)
    {
        liblte_value_2_bits(phys_cell_id->ncc, ie_ptr, 3);
        liblte_value_2_bits(phys_cell_id->bcc, ie_ptr, 3);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_unpack_phys_cell_id_geran_ie(uint8                                **ie_ptr,
                                                          LIBLTE_RRC_PHYS_CELL_ID_GERAN_STRUCT  *phys_cell_id)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr       != NULL &&
       phys_cell_id != NULL)
    {
        phys_cell_id->ncc = liblte_bits_2_value(ie_ptr, 3);
        phys_cell_id->bcc = liblte_bits_2_value(ie_ptr, 3);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: Phys Cell ID UTRA FDD

    Description: Indicates the physical layer identity of the cell

    Document Reference: 36.331 v10.0.0 Section 6.3.4
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_rrc_pack_phys_cell_id_utra_fdd_ie(uint16   phys_cell_id,
                                                           uint8  **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr != NULL)
    {
        liblte_value_2_bits(phys_cell_id, ie_ptr, 9);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_unpack_phys_cell_id_utra_fdd_ie(uint8  **ie_ptr,
                                                             uint16  *phys_cell_id)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr       != NULL &&
       phys_cell_id != NULL)
    {
        *phys_cell_id = liblte_bits_2_value(ie_ptr, 9);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: Phys Cell ID UTRA TDD

    Description: Indicates the physical layer identity of the cell

    Document Reference: 36.331 v10.0.0 Section 6.3.4
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_rrc_pack_phys_cell_id_utra_tdd_ie(uint8   phys_cell_id,
                                                           uint8 **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr != NULL)
    {
        liblte_value_2_bits(phys_cell_id, ie_ptr, 7);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_unpack_phys_cell_id_utra_tdd_ie(uint8 **ie_ptr,
                                                             uint8  *phys_cell_id)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr       != NULL &&
       phys_cell_id != NULL)
    {
        *phys_cell_id = liblte_bits_2_value(ie_ptr, 7);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: PLMN Identity

    Description: Identifies a Public Land Mobile Network

    Document Reference: 36.331 v10.0.0 Section 6.3.4
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_rrc_pack_plmn_identity_ie(LIBLTE_RRC_PLMN_IDENTITY_STRUCT  *plmn_id,
                                                   uint8                           **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             mcc_opt = true;
    uint8             mnc_size;

    if(plmn_id != NULL &&
       ie_ptr  != NULL)
    {
        if(0xFFFF == plmn_id->mcc) {
          mcc_opt = false;
        }
        liblte_value_2_bits(mcc_opt, ie_ptr, 1);

        if(true == mcc_opt)
        {
            liblte_value_2_bits((plmn_id->mcc>>8)&0x0F, ie_ptr, 4);
            liblte_value_2_bits((plmn_id->mcc>>4)&0x0F, ie_ptr, 4);
            liblte_value_2_bits((plmn_id->mcc)&0x0F, ie_ptr, 4);
        }

        if((plmn_id->mnc & 0xFF00) == 0xFF00)
        {
            mnc_size = 2;
            liblte_value_2_bits((mnc_size)-2, ie_ptr, 1);
            liblte_value_2_bits((plmn_id->mnc>>4)&0x0F, ie_ptr, 4);
            liblte_value_2_bits((plmn_id->mnc)&0x0F, ie_ptr, 4);
        }else{
            mnc_size = 3;
            liblte_value_2_bits((mnc_size)-2, ie_ptr, 1);
            liblte_value_2_bits((plmn_id->mnc>>8)&0x0F, ie_ptr, 4);
            liblte_value_2_bits((plmn_id->mnc>>4)&0x0F, ie_ptr, 4);
            liblte_value_2_bits((plmn_id->mnc)&0x0F, ie_ptr, 4);
        }

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_unpack_plmn_identity_ie(uint8                           **ie_ptr,
                                                     LIBLTE_RRC_PLMN_IDENTITY_STRUCT  *plmn_id)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             mcc_opt;
    uint8             mnc_size;

    if(ie_ptr  != NULL &&
       plmn_id != NULL)
    {
        mcc_opt = liblte_bits_2_value(ie_ptr, 1);

        if(true == mcc_opt)
        {
            plmn_id->mcc  = 0xF000;
            plmn_id->mcc |= (liblte_bits_2_value(ie_ptr, 4) << 8);
            plmn_id->mcc |= (liblte_bits_2_value(ie_ptr, 4) << 4);
            plmn_id->mcc |= liblte_bits_2_value(ie_ptr, 4);

        }else{
            plmn_id->mcc = LIBLTE_RRC_MCC_NOT_PRESENT;
        }

        mnc_size     = (liblte_bits_2_value(ie_ptr, 1) + 2);
        if(2 == mnc_size)
        {
            plmn_id->mnc  = 0xFF00;
            plmn_id->mnc |= (liblte_bits_2_value(ie_ptr, 4) << 4);
            plmn_id->mnc |= liblte_bits_2_value(ie_ptr, 4);
        }else{
            plmn_id->mnc  = 0xF000;
            plmn_id->mnc |= (liblte_bits_2_value(ie_ptr, 4) << 8);
            plmn_id->mnc |= (liblte_bits_2_value(ie_ptr, 4) << 4);
            plmn_id->mnc |= liblte_bits_2_value(ie_ptr, 4);
        }

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: Pre Registration Info HRPD

    Description: FIXME

    Document Reference: 36.331 v10.0.0 Section 6.3.4
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_rrc_pack_pre_registration_info_hrpd_ie(LIBLTE_RRC_PRE_REGISTRATION_INFO_HRPD_STRUCT  *pre_reg_info_hrpd,
                                                                uint8                                        **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;
    uint32            i;

    if(pre_reg_info_hrpd != NULL &&
       ie_ptr            != NULL)
    {
        // Optional indicators
        liblte_value_2_bits(pre_reg_info_hrpd->pre_reg_zone_id_present, ie_ptr, 1);
        if(0 != pre_reg_info_hrpd->secondary_pre_reg_zone_id_list_size)
        {
            liblte_value_2_bits(1, ie_ptr, 1);
        }else{
            liblte_value_2_bits(0, ie_ptr, 1);
        }

        liblte_value_2_bits(pre_reg_info_hrpd->pre_reg_allowed, ie_ptr, 1);

        if(true == pre_reg_info_hrpd->pre_reg_zone_id_present)
        {
            liblte_value_2_bits(pre_reg_info_hrpd->pre_reg_zone_id, ie_ptr, 8);
        }

        if(0 != pre_reg_info_hrpd->secondary_pre_reg_zone_id_list_size)
        {
            liblte_value_2_bits(pre_reg_info_hrpd->secondary_pre_reg_zone_id_list_size - 1, ie_ptr, 1);
            for(i=0; i<pre_reg_info_hrpd->secondary_pre_reg_zone_id_list_size; i++)
            {
                liblte_value_2_bits(pre_reg_info_hrpd->secondary_pre_reg_zone_id_list[i], ie_ptr, 8);
            }
        }

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_unpack_pre_registration_info_hrpd_ie(uint8                                        **ie_ptr,
                                                                  LIBLTE_RRC_PRE_REGISTRATION_INFO_HRPD_STRUCT  *pre_reg_info_hrpd)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;
    uint32            i;
    bool              secondary_pre_reg_zone_id_opt;

    if(ie_ptr            != NULL &&
       pre_reg_info_hrpd != NULL)
    {
        // Optional indicators
        pre_reg_info_hrpd->pre_reg_zone_id_present = liblte_bits_2_value(ie_ptr, 1);
        secondary_pre_reg_zone_id_opt              = liblte_bits_2_value(ie_ptr, 1);

        pre_reg_info_hrpd->pre_reg_allowed = liblte_bits_2_value(ie_ptr, 1);

        if(true == pre_reg_info_hrpd->pre_reg_zone_id_present)
        {
            pre_reg_info_hrpd->pre_reg_zone_id = liblte_bits_2_value(ie_ptr, 8);
        }

        if(true == secondary_pre_reg_zone_id_opt)
        {
            pre_reg_info_hrpd->secondary_pre_reg_zone_id_list_size = liblte_bits_2_value(ie_ptr, 1) + 1;
            for(i=0; i<pre_reg_info_hrpd->secondary_pre_reg_zone_id_list_size; i++)
            {
                pre_reg_info_hrpd->secondary_pre_reg_zone_id_list[i] = liblte_bits_2_value(ie_ptr, 8);
            }
        }else{
            pre_reg_info_hrpd->secondary_pre_reg_zone_id_list_size = 0;
        }

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: Q Qual Min

    Description: Indicates for cell selection/re-selection the
                 required minimum received RSRQ level in the (E-UTRA)
                 cell

    Document Reference: 36.331 v10.0.0 Section 6.3.4
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_rrc_pack_q_qual_min_ie(int8    q_qual_min,
                                                uint8 **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr != NULL)
    {
        liblte_value_2_bits(q_qual_min + 34, ie_ptr, 5);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_unpack_q_qual_min_ie(uint8 **ie_ptr,
                                                  int8   *q_qual_min)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr     != NULL &&
       q_qual_min != NULL)
    {
        *q_qual_min = (int8)liblte_bits_2_value(ie_ptr, 5) - 34;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: Q Rx Lev Min

    Description: Indicates the required minimum received RSRP level in
                 the (E-UTRA) cell for cell selection/re-selection

    Document Reference: 36.331 v10.0.0 Section 6.3.4
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_rrc_pack_q_rx_lev_min_ie(int16   q_rx_lev_min,
                                                  uint8 **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr != NULL)
    {
        liblte_value_2_bits((q_rx_lev_min / 2) + 70, ie_ptr, 6);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_unpack_q_rx_lev_min_ie(uint8 **ie_ptr,
                                                    int16  *q_rx_lev_min)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr       != NULL &&
       q_rx_lev_min != NULL)
    {
        *q_rx_lev_min = ((int16)liblte_bits_2_value(ie_ptr, 6) - 70) * 2;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: Q Offset Range

    Description: Indicates a cell or frequency specific offset to be
                 applied when evaluating candidates for cell
                 reselection or when evaluating triggering conditions
                 for measurement reporting

    Document Reference: 36.331 v10.0.0 Section 6.3.4
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_rrc_pack_q_offset_range_ie(LIBLTE_RRC_Q_OFFSET_RANGE_ENUM   q_offset_range,
                                                    uint8                          **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr != NULL)
    {
        liblte_value_2_bits(q_offset_range, ie_ptr, 5);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_unpack_q_offset_range_ie(uint8                          **ie_ptr,
                                                      LIBLTE_RRC_Q_OFFSET_RANGE_ENUM  *q_offset_range)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr         != NULL &&
       q_offset_range != NULL)
    {
        *q_offset_range = (LIBLTE_RRC_Q_OFFSET_RANGE_ENUM)liblte_bits_2_value(ie_ptr, 5);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: Q Offset Range Inter RAT

    Description: Indicates a frequency specific offset to be applied
                 when evaluating triggering conditions for
                 measurement reporting

    Document Reference: 36.331 v10.0.0 Section 6.3.4
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_rrc_pack_q_offset_range_inter_rat_ie(int8    q_offset_range_inter_rat,
                                                              uint8 **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr != NULL)
    {
        liblte_value_2_bits(q_offset_range_inter_rat + 15, ie_ptr, 5);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_unpack_q_offset_range_inter_rat_ie(uint8 **ie_ptr,
                                                                int8   *q_offset_range_inter_rat)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr                   != NULL &&
       q_offset_range_inter_rat != NULL)
    {
        *q_offset_range_inter_rat = (int8)(liblte_bits_2_value(ie_ptr, 5)) - 15;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: Reselection Threshold

    Description: Indicates an RX level threshold for cell reselection

    Document Reference: 36.331 v10.0.0 Section 6.3.4
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_rrc_pack_reselection_threshold_ie(uint8   resel_thresh,
                                                           uint8 **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr != NULL)
    {
        liblte_value_2_bits(resel_thresh / 2, ie_ptr, 5);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_unpack_reselection_threshold_ie(uint8 **ie_ptr,
                                                             uint8  *resel_thresh)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr       != NULL &&
       resel_thresh != NULL)
    {
        *resel_thresh = liblte_bits_2_value(ie_ptr, 5) * 2;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: Reselection Threshold Q

    Description: Indicates a quality level threshold for cell
                 reselection

    Document Reference: 36.331 v10.0.0 Section 6.3.4
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_rrc_pack_reselection_threshold_q_ie(uint8   resel_thresh_q,
                                                             uint8 **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr != NULL)
    {
        liblte_value_2_bits(resel_thresh_q, ie_ptr, 5);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_unpack_reselection_threshold_q_ie(uint8 **ie_ptr,
                                                               uint8  *resel_thresh_q)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr         != NULL &&
       resel_thresh_q != NULL)
    {
        *resel_thresh_q = liblte_bits_2_value(ie_ptr, 5);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: S Cell Index

    Description: Contains a short identity, used to identify an
                 SCell

    Document Reference: 36.331 v10.0.0 Section 6.3.4
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_rrc_pack_s_cell_index_ie(uint8   s_cell_idx,
                                                  uint8 **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr != NULL)
    {
        liblte_value_2_bits(s_cell_idx - 1, ie_ptr, 3);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_unpack_s_cell_index_ie(uint8 **ie_ptr,
                                                    uint8  *s_cell_idx)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr     != NULL &&
       s_cell_idx != NULL)
    {
        *s_cell_idx = liblte_bits_2_value(ie_ptr, 3) + 1;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: Serv Cell Index

    Description: Contains a short identity, used to identify a
                 serving cell

    Document Reference: 36.331 v10.0.0 Section 6.3.4
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_rrc_pack_serv_cell_index_ie(uint8   serv_cell_idx,
                                                     uint8 **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr != NULL)
    {
        liblte_value_2_bits(serv_cell_idx, ie_ptr, 3);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_unpack_serv_cell_index_ie(uint8 **ie_ptr,
                                                       uint8  *serv_cell_idx)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr        != NULL &&
       serv_cell_idx != NULL)
    {
        *serv_cell_idx = liblte_bits_2_value(ie_ptr, 3);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: Speed State Scale Factors

    Description: Contains factors, to be applied when the UE is in
                 medium or high speed state, used for scaling a
                 mobility control related parameter

    Document Reference: 36.331 v10.0.0 Section 6.3.4
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_rrc_pack_speed_state_scale_factors_ie(LIBLTE_RRC_SPEED_STATE_SCALE_FACTORS_STRUCT  *speed_state_scale_factors,
                                                               uint8                                       **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(speed_state_scale_factors != NULL &&
       ie_ptr                    != NULL)
    {
        liblte_value_2_bits(speed_state_scale_factors->sf_medium, ie_ptr, 2);
        liblte_value_2_bits(speed_state_scale_factors->sf_high,   ie_ptr, 2);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_unpack_speed_state_scale_factors_ie(uint8                                       **ie_ptr,
                                                                 LIBLTE_RRC_SPEED_STATE_SCALE_FACTORS_STRUCT  *speed_state_scale_factors)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr                    != NULL &&
       speed_state_scale_factors != NULL)
    {
        speed_state_scale_factors->sf_medium = (LIBLTE_RRC_SSSF_MEDIUM_ENUM)liblte_bits_2_value(ie_ptr, 2);
        speed_state_scale_factors->sf_high   = (LIBLTE_RRC_SSSF_HIGH_ENUM)liblte_bits_2_value(ie_ptr, 2);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: System Info List GERAN

    Description: Contains system information of a GERAN cell

    Document Reference: 36.331 v10.0.0 Section 6.3.4
*********************************************************************/
// FIXME

/*********************************************************************
    IE Name: System Time Info CDMA2000

    Description: Informs the UE about the absolute time in the current
                 cell

    Document Reference: 36.331 v10.0.0 Section 6.3.4
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_rrc_pack_system_time_info_cdma2000_ie(LIBLTE_RRC_SYSTEM_TIME_INFO_CDMA2000_STRUCT  *sys_time_info_cdma2000,
                                                               uint8                                       **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(sys_time_info_cdma2000 != NULL &&
       ie_ptr                 != NULL)
    {
        liblte_value_2_bits(sys_time_info_cdma2000->cdma_eutra_sync,   ie_ptr, 1);
        liblte_value_2_bits(sys_time_info_cdma2000->system_time_async, ie_ptr, 1);
        if(true == sys_time_info_cdma2000->system_time_async)
        {
            liblte_value_2_bits((uint32)(sys_time_info_cdma2000->system_time >> 17),     ie_ptr, 32);
            liblte_value_2_bits((uint32)(sys_time_info_cdma2000->system_time & 0x1FFFF), ie_ptr, 17);
        }else{
            liblte_value_2_bits((uint32)(sys_time_info_cdma2000->system_time >> 7),   ie_ptr, 32);
            liblte_value_2_bits((uint32)(sys_time_info_cdma2000->system_time & 0x7F), ie_ptr,  7);
        }

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_unpack_system_time_info_cdma2000_ie(uint8                                       **ie_ptr,
                                                                 LIBLTE_RRC_SYSTEM_TIME_INFO_CDMA2000_STRUCT  *sys_time_info_cdma2000)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr                 != NULL &&
       sys_time_info_cdma2000 != NULL)
    {
        sys_time_info_cdma2000->cdma_eutra_sync   = liblte_bits_2_value(ie_ptr, 1);
        sys_time_info_cdma2000->system_time_async = liblte_bits_2_value(ie_ptr, 1);
        if(true == sys_time_info_cdma2000->system_time_async)
        {
            sys_time_info_cdma2000->system_time  = (uint64)liblte_bits_2_value(ie_ptr, 32) << 17;
            sys_time_info_cdma2000->system_time |= (uint64)liblte_bits_2_value(ie_ptr, 17);
        }else{
            sys_time_info_cdma2000->system_time  = (uint64)liblte_bits_2_value(ie_ptr, 32) << 7;
            sys_time_info_cdma2000->system_time |= (uint64)liblte_bits_2_value(ie_ptr,  7);
        }

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: Tracking Area Code

    Description: Identifies a tracking area within the scope of a
                 PLMN

    Document Reference: 36.331 v10.0.0 Section 6.3.4
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_rrc_pack_tracking_area_code_ie(uint16   tac,
                                                        uint8  **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr != NULL)
    {
        liblte_value_2_bits(tac, ie_ptr, 16);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_unpack_tracking_area_code_ie(uint8  **ie_ptr,
                                                          uint16  *tac)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr != NULL &&
       tac    != NULL)
    {
        *tac = liblte_bits_2_value(ie_ptr, 16);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: T Reselection

    Description: Contains the timer T_reselection_rat for E-UTRA,
                 UTRA, GERAN, or CDMA2000

    Document Reference: 36.331 v10.0.0 Section 6.3.4
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_rrc_pack_t_reselection_ie(uint8   t_resel,
                                                   uint8 **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr != NULL)
    {
        liblte_value_2_bits(t_resel, ie_ptr, 3);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_unpack_t_reselection_ie(uint8 **ie_ptr,
                                                     uint8  *t_resel)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr  != NULL &&
       t_resel != NULL)
    {
        *t_resel = liblte_bits_2_value(ie_ptr, 3);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: Next Hop Chaining Count

    Description: Updates the Kenb key and corresponds to parameter
                 NCC

    Document Reference: 36.331 v10.0.0 Section 6.3.3
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_rrc_pack_next_hop_chaining_count_ie(uint8   next_hop_chaining_count,
                                                             uint8 **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr != NULL)
    {
        liblte_value_2_bits(next_hop_chaining_count, ie_ptr, 3);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_unpack_next_hop_chaining_count_ie(uint8 **ie_ptr,
                                                               uint8  *next_hop_chaining_count)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr                  != NULL &&
       next_hop_chaining_count != NULL)
    {
        *next_hop_chaining_count = liblte_bits_2_value(ie_ptr, 3);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: Security Algorithm Config

    Description: Configures AS integrity protection algorithm (SRBs)
                 and AS ciphering algorithm (SRBs and DRBs)

    Document Reference: 36.331 v10.0.0 Section 6.3.3
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_rrc_pack_security_algorithm_config_ie(LIBLTE_RRC_SECURITY_ALGORITHM_CONFIG_STRUCT  *sec_alg_cnfg,
                                                               uint8                                       **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(sec_alg_cnfg != NULL &&
       ie_ptr       != NULL)
    {
        // Extension indicator
        liblte_value_2_bits(0, ie_ptr, 1);

        liblte_value_2_bits(sec_alg_cnfg->cipher_alg, ie_ptr, 3);

        // Extension indicator
        liblte_value_2_bits(0, ie_ptr, 1);

        liblte_value_2_bits(sec_alg_cnfg->int_alg, ie_ptr, 3);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_unpack_security_algorithm_config_ie(uint8                                       **ie_ptr,
                                                                 LIBLTE_RRC_SECURITY_ALGORITHM_CONFIG_STRUCT  *sec_alg_cnfg)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr       != NULL &&
       sec_alg_cnfg != NULL)
    {
        // Extension indicator
        liblte_bits_2_value(ie_ptr, 1);

        sec_alg_cnfg->cipher_alg = (LIBLTE_RRC_CIPHERING_ALGORITHM_ENUM)liblte_bits_2_value(ie_ptr, 3);

        // Extension indicator
        liblte_bits_2_value(ie_ptr, 1);

        sec_alg_cnfg->int_alg = (LIBLTE_RRC_INTEGRITY_PROT_ALGORITHM_ENUM)liblte_bits_2_value(ie_ptr, 3);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: Short MAC I

    Description: Identifies and verifies the UE at RRC connection
                 re-establishment

    Document Reference: 36.331 v10.0.0 Section 6.3.3
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_rrc_pack_short_mac_i_ie(uint16   short_mac_i,
                                                 uint8  **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr != NULL)
    {
        liblte_value_2_bits(short_mac_i, ie_ptr, 16);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_unpack_short_mac_i_ie(uint8  **ie_ptr,
                                                   uint16  *short_mac_i)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr      != NULL &&
       short_mac_i != NULL)
    {
        *short_mac_i = liblte_bits_2_value(ie_ptr, 16);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: Antenna Info

    Description: Specifies the common and the UE specific antenna
                 configuration

    Document Reference: 36.331 v10.0.0 Section 6.3.2
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_rrc_pack_antenna_info_common_ie(LIBLTE_RRC_ANTENNA_PORTS_COUNT_ENUM   antenna_ports_cnt,
                                                         uint8                               **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr != NULL)
    {
        liblte_value_2_bits(antenna_ports_cnt, ie_ptr, 2);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_unpack_antenna_info_common_ie(uint8                               **ie_ptr,
                                                           LIBLTE_RRC_ANTENNA_PORTS_COUNT_ENUM  *antenna_ports_cnt)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr            != NULL &&
       antenna_ports_cnt != NULL)
    {
        *antenna_ports_cnt = (LIBLTE_RRC_ANTENNA_PORTS_COUNT_ENUM)liblte_bits_2_value(ie_ptr, 2);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_pack_antenna_info_dedicated_ie(LIBLTE_RRC_ANTENNA_INFO_DEDICATED_STRUCT  *antenna_info,
                                                            uint8                                    **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(antenna_info != NULL &&
       ie_ptr       != NULL)
    {
        // Optional indicator
        liblte_value_2_bits(antenna_info->codebook_subset_restriction_present, ie_ptr, 1);

        // Transmission Mode
        liblte_value_2_bits(antenna_info->tx_mode, ie_ptr, 3);

        // Codebook Subset Restriction
        if(antenna_info->codebook_subset_restriction_present)
        {
            liblte_value_2_bits(antenna_info->codebook_subset_restriction_choice, ie_ptr, 3);
            switch(antenna_info->codebook_subset_restriction_choice)
            {
            case LIBLTE_RRC_CODEBOOK_SUBSET_RESTRICTION_N2_TM3:
                liblte_value_2_bits(antenna_info->codebook_subset_restriction, ie_ptr, 2);
                break;
            case LIBLTE_RRC_CODEBOOK_SUBSET_RESTRICTION_N4_TM3:
            case LIBLTE_RRC_CODEBOOK_SUBSET_RESTRICTION_N2_TM5:
            case LIBLTE_RRC_CODEBOOK_SUBSET_RESTRICTION_N2_TM6:
                liblte_value_2_bits(antenna_info->codebook_subset_restriction, ie_ptr, 4);
                break;
            case LIBLTE_RRC_CODEBOOK_SUBSET_RESTRICTION_N2_TM4:
                liblte_value_2_bits(antenna_info->codebook_subset_restriction, ie_ptr, 6);
                break;
            case LIBLTE_RRC_CODEBOOK_SUBSET_RESTRICTION_N4_TM4:
                liblte_value_2_bits(antenna_info->codebook_subset_restriction >> 32, ie_ptr, 32);
                liblte_value_2_bits(antenna_info->codebook_subset_restriction,       ie_ptr, 32);
                break;
            case LIBLTE_RRC_CODEBOOK_SUBSET_RESTRICTION_N4_TM5:
            case LIBLTE_RRC_CODEBOOK_SUBSET_RESTRICTION_N4_TM6:
                liblte_value_2_bits(antenna_info->codebook_subset_restriction, ie_ptr, 16);
                break;
            }
        }

        // UE Transmit Antenna Selection
        liblte_value_2_bits(antenna_info->ue_tx_antenna_selection_setup_present, ie_ptr, 1);
        if(antenna_info->ue_tx_antenna_selection_setup_present)
        {
            liblte_value_2_bits(antenna_info->ue_tx_antenna_selection_setup, ie_ptr, 1);
        }

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_unpack_antenna_info_dedicated_ie(uint8                                    **ie_ptr,
                                                              LIBLTE_RRC_ANTENNA_INFO_DEDICATED_STRUCT  *antenna_info)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr       != NULL &&
       antenna_info != NULL)
    {
        // Optional indicator
        antenna_info->codebook_subset_restriction_present = liblte_bits_2_value(ie_ptr, 1);

        // Transmission Mode
        antenna_info->tx_mode = (LIBLTE_RRC_TRANSMISSION_MODE_ENUM)liblte_bits_2_value(ie_ptr, 3);

        // Codebook Subset Restriction
        if(antenna_info->codebook_subset_restriction_present)
        {
            antenna_info->codebook_subset_restriction_choice = (LIBLTE_RRC_CODEBOOK_SUBSET_RESTRICTION_CHOICE_ENUM)liblte_bits_2_value(ie_ptr, 3);
            switch(antenna_info->codebook_subset_restriction_choice)
            {
            case LIBLTE_RRC_CODEBOOK_SUBSET_RESTRICTION_N2_TM3:
                antenna_info->codebook_subset_restriction = liblte_bits_2_value(ie_ptr, 2);
                break;
            case LIBLTE_RRC_CODEBOOK_SUBSET_RESTRICTION_N4_TM3:
            case LIBLTE_RRC_CODEBOOK_SUBSET_RESTRICTION_N2_TM5:
            case LIBLTE_RRC_CODEBOOK_SUBSET_RESTRICTION_N2_TM6:
                antenna_info->codebook_subset_restriction = liblte_bits_2_value(ie_ptr, 4);
                break;
            case LIBLTE_RRC_CODEBOOK_SUBSET_RESTRICTION_N2_TM4:
                antenna_info->codebook_subset_restriction = liblte_bits_2_value(ie_ptr, 6);
                break;
            case LIBLTE_RRC_CODEBOOK_SUBSET_RESTRICTION_N4_TM4:
                antenna_info->codebook_subset_restriction  = (uint64)(liblte_bits_2_value(ie_ptr, 32)) << 32;
                antenna_info->codebook_subset_restriction |= liblte_bits_2_value(ie_ptr, 32);
                break;
            case LIBLTE_RRC_CODEBOOK_SUBSET_RESTRICTION_N4_TM5:
            case LIBLTE_RRC_CODEBOOK_SUBSET_RESTRICTION_N4_TM6:
                antenna_info->codebook_subset_restriction = liblte_bits_2_value(ie_ptr, 16);
                break;
            }
        }

        // UE Transmit Antenna Selection
        antenna_info->ue_tx_antenna_selection_setup_present = liblte_bits_2_value(ie_ptr, 1);
        if(antenna_info->ue_tx_antenna_selection_setup_present)
        {
            antenna_info->ue_tx_antenna_selection_setup = (LIBLTE_RRC_UE_TX_ANTENNA_SELECTION_ENUM)liblte_bits_2_value(ie_ptr, 1);
        }

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: CQI Report Config

    Description: Specifies the CQI reporting configuration

    Document Reference: 36.331 v10.0.0 Section 6.3.2
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_rrc_pack_cqi_report_config_ie(LIBLTE_RRC_CQI_REPORT_CONFIG_STRUCT  *cqi_report_cnfg,
                                                       uint8                               **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(cqi_report_cnfg != NULL &&
       ie_ptr          != NULL)
    {
        // Optional indicators
        liblte_value_2_bits(cqi_report_cnfg->report_mode_aperiodic_present, ie_ptr, 1);
        liblte_value_2_bits(cqi_report_cnfg->report_periodic_present,       ie_ptr, 1);

        // CQI Report Mode Aperiodic
        if(cqi_report_cnfg->report_mode_aperiodic_present)
        {
            liblte_value_2_bits(cqi_report_cnfg->report_mode_aperiodic, ie_ptr, 3);
        }

        // Nom PDSCH RS EPRE Offset
        liblte_value_2_bits(cqi_report_cnfg->nom_pdsch_rs_epre_offset + 1, ie_ptr, 3);

        // CQI Report Periodic
        if(cqi_report_cnfg->report_periodic_present)
        {
            liblte_value_2_bits(cqi_report_cnfg->report_periodic_setup_present, ie_ptr, 1);
            if(cqi_report_cnfg->report_periodic_setup_present)
            {
                // Optional indicator
                liblte_value_2_bits(cqi_report_cnfg->report_periodic.ri_cnfg_idx_present, ie_ptr, 1);

                // CQI PUCCH Resource Index
                liblte_value_2_bits(cqi_report_cnfg->report_periodic.pucch_resource_idx, ie_ptr, 11);

                // CQI PMI Config Index
                liblte_value_2_bits(cqi_report_cnfg->report_periodic.pmi_cnfg_idx, ie_ptr, 10);

                // CQI Format Indicator Periodic
                liblte_value_2_bits(cqi_report_cnfg->report_periodic.format_ind_periodic, ie_ptr, 1);
                if(LIBLTE_RRC_CQI_FORMAT_INDICATOR_PERIODIC_SUBBAND_CQI == cqi_report_cnfg->report_periodic.format_ind_periodic)
                {
                    liblte_value_2_bits(cqi_report_cnfg->report_periodic.format_ind_periodic_subband_k - 1, ie_ptr, 2);
                }

                // RI Config Index
                if(cqi_report_cnfg->report_periodic.ri_cnfg_idx_present)
                {
                    liblte_value_2_bits(cqi_report_cnfg->report_periodic.ri_cnfg_idx, ie_ptr, 10);
                }

                // Simultaneous Ack/Nack and CQI
                liblte_value_2_bits(cqi_report_cnfg->report_periodic.simult_ack_nack_and_cqi, ie_ptr, 1);
            }
        }

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_unpack_cqi_report_config_ie(uint8                               **ie_ptr,
                                                         LIBLTE_RRC_CQI_REPORT_CONFIG_STRUCT  *cqi_report_cnfg)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr          != NULL &&
       cqi_report_cnfg != NULL)
    {
        // Optional indicators
        cqi_report_cnfg->report_mode_aperiodic_present = liblte_bits_2_value(ie_ptr, 1);
        cqi_report_cnfg->report_periodic_present       = liblte_bits_2_value(ie_ptr, 1);

        // CQI Report Mode Aperiodic
        if(cqi_report_cnfg->report_mode_aperiodic_present)
        {
            cqi_report_cnfg->report_mode_aperiodic = (LIBLTE_RRC_CQI_REPORT_MODE_APERIODIC_ENUM)liblte_bits_2_value(ie_ptr, 3);
        }

        // Nom PDSCH RS EPRE Offset
        cqi_report_cnfg->nom_pdsch_rs_epre_offset = liblte_bits_2_value(ie_ptr, 3) - 1;

        // CQI Report Periodic
        if(cqi_report_cnfg->report_periodic_present)
        {
            cqi_report_cnfg->report_periodic_setup_present = liblte_bits_2_value(ie_ptr, 1);
            if(cqi_report_cnfg->report_periodic_setup_present)
            {
                // Optional indicator
                cqi_report_cnfg->report_periodic.ri_cnfg_idx_present = liblte_bits_2_value(ie_ptr, 1);

                // CQI PUCCH Resource Index
                cqi_report_cnfg->report_periodic.pucch_resource_idx = liblte_bits_2_value(ie_ptr, 11);

                // CQI PMI Config Index
                cqi_report_cnfg->report_periodic.pmi_cnfg_idx = liblte_bits_2_value(ie_ptr, 10);

                // CQI Format Indicator Periodic
                cqi_report_cnfg->report_periodic.format_ind_periodic = (LIBLTE_RRC_CQI_FORMAT_INDICATOR_PERIODIC_ENUM)liblte_bits_2_value(ie_ptr, 1);
                if(LIBLTE_RRC_CQI_FORMAT_INDICATOR_PERIODIC_SUBBAND_CQI == cqi_report_cnfg->report_periodic.format_ind_periodic)
                {
                    cqi_report_cnfg->report_periodic.format_ind_periodic_subband_k = liblte_bits_2_value(ie_ptr, 2) + 1;
                }

                // RI Config Index
                if(cqi_report_cnfg->report_periodic.ri_cnfg_idx_present)
                {
                    cqi_report_cnfg->report_periodic.ri_cnfg_idx = liblte_bits_2_value(ie_ptr, 10);
                }

                // Simultaneous Ack/Nack and CQI
                cqi_report_cnfg->report_periodic.simult_ack_nack_and_cqi = liblte_bits_2_value(ie_ptr, 1);
            }
        }

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: Cross Carrier Scheduling Config

    Description: Specifies the configuration when the cross carrier
                 scheduling is used in a cell

    Document Reference: 36.331 v10.0.0 Section 6.3.2
*********************************************************************/
// FIXME

/*********************************************************************
    IE Name: CSI RS Config

    Description: Specifies the CSI (Channel State Information)
                 reference signal configuration

    Document Reference: 36.331 v10.0.0 Section 6.3.2
*********************************************************************/
// FIXME

/*********************************************************************
    IE Name: DRB Identity

    Description: Identifies a DRB used by a UE

    Document Reference: 36.331 v10.0.0 Section 6.3.2
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_rrc_pack_drb_identity_ie(uint8   drb_id,
                                                  uint8 **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr != NULL)
    {
        liblte_value_2_bits(drb_id - 1, ie_ptr, 5);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_unpack_drb_identity_ie(uint8 **ie_ptr,
                                                    uint8  *drb_id)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr != NULL &&
       drb_id != NULL)
    {
        *drb_id = liblte_bits_2_value(ie_ptr, 5) + 1;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: Logical Channel Config

    Description: Configures the logical channel parameters

    Document Reference: 36.331 v10.0.0 Section 6.3.2
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_rrc_pack_logical_channel_config_ie(LIBLTE_RRC_LOGICAL_CHANNEL_CONFIG_STRUCT  *log_chan_cnfg,
                                                            uint8                                    **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(log_chan_cnfg != NULL &&
       ie_ptr        != NULL)
    {
        // Extension indicator
        liblte_value_2_bits(0, ie_ptr, 1); // FIXME: Handle extension

        // Optional indicator
        liblte_value_2_bits(log_chan_cnfg->ul_specific_params_present, ie_ptr, 1);

        if(true == log_chan_cnfg->ul_specific_params_present)
        {
            // Optional indicator
            liblte_value_2_bits(log_chan_cnfg->ul_specific_params.log_chan_group_present, ie_ptr, 1);

            liblte_value_2_bits(log_chan_cnfg->ul_specific_params.priority - 1,         ie_ptr, 4);
            liblte_value_2_bits(log_chan_cnfg->ul_specific_params.prioritized_bit_rate, ie_ptr, 4);
            liblte_value_2_bits(log_chan_cnfg->ul_specific_params.bucket_size_duration, ie_ptr, 3);

            if(true == log_chan_cnfg->ul_specific_params.log_chan_group_present)
            {
                liblte_value_2_bits(log_chan_cnfg->ul_specific_params.log_chan_group, ie_ptr, 2);
            }
        }

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_unpack_logical_channel_config_ie(uint8                                    **ie_ptr,
                                                              LIBLTE_RRC_LOGICAL_CHANNEL_CONFIG_STRUCT  *log_chan_cnfg)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;
    bool              ext;

    if(ie_ptr        != NULL &&
       log_chan_cnfg != NULL)
    {
        // Extension indicator
        ext = liblte_bits_2_value(ie_ptr, 1); // FIXME: Handle extension

        // Optional indicator
        log_chan_cnfg->ul_specific_params_present = liblte_bits_2_value(ie_ptr, 1);

        if(true == log_chan_cnfg->ul_specific_params_present)
        {
            // Optional indicator
            log_chan_cnfg->ul_specific_params.log_chan_group_present = liblte_bits_2_value(ie_ptr, 1);

            log_chan_cnfg->ul_specific_params.priority             = liblte_bits_2_value(ie_ptr, 4) + 1;
            log_chan_cnfg->ul_specific_params.prioritized_bit_rate = (LIBLTE_RRC_PRIORITIZED_BIT_RATE_ENUM)liblte_bits_2_value(ie_ptr, 4);
            log_chan_cnfg->ul_specific_params.bucket_size_duration = (LIBLTE_RRC_BUCKET_SIZE_DURATION_ENUM)liblte_bits_2_value(ie_ptr, 3);

            if(true == log_chan_cnfg->ul_specific_params.log_chan_group_present)
            {
                log_chan_cnfg->ul_specific_params.log_chan_group = liblte_bits_2_value(ie_ptr, 2);
            }
        }

        // Consume non-critical extensions
        liblte_rrc_consume_noncrit_extension(ext, __func__, ie_ptr);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: MAC Main Config

    Description: Specifies the MAC main configuration for signalling
                 and data radio bearers

    Document Reference: 36.331 v10.0.0 Section 6.3.2
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_rrc_pack_mac_main_config_ie(LIBLTE_RRC_MAC_MAIN_CONFIG_STRUCT  *mac_main_cnfg,
                                                     uint8                             **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;
    bool              ext = false;

    if(mac_main_cnfg != NULL &&
       ie_ptr        != NULL)
    {
        // Extension indicator
        liblte_value_2_bits(ext, ie_ptr, 1);

        // Optional indicators
        liblte_value_2_bits(mac_main_cnfg->ulsch_cnfg_present, ie_ptr, 1);
        liblte_value_2_bits(mac_main_cnfg->drx_cnfg_present,   ie_ptr, 1);
        liblte_value_2_bits(mac_main_cnfg->phr_cnfg_present,   ie_ptr, 1);

        // ULSCH Config
        if(mac_main_cnfg->ulsch_cnfg_present)
        {
            // Optional indicators
            liblte_value_2_bits(mac_main_cnfg->ulsch_cnfg.max_harq_tx_present,        ie_ptr, 1);
            liblte_value_2_bits(mac_main_cnfg->ulsch_cnfg.periodic_bsr_timer_present, ie_ptr, 1);

            // Max HARQ TX
            if(mac_main_cnfg->ulsch_cnfg.max_harq_tx_present)
            {
                liblte_value_2_bits(mac_main_cnfg->ulsch_cnfg.max_harq_tx, ie_ptr, 4);
            }

            // Periodic BSR Timer
            if(mac_main_cnfg->ulsch_cnfg.periodic_bsr_timer_present)
            {
                liblte_value_2_bits(mac_main_cnfg->ulsch_cnfg.periodic_bsr_timer, ie_ptr, 4);
            }

            // Re-TX BSR Timer
            liblte_value_2_bits(mac_main_cnfg->ulsch_cnfg.retx_bsr_timer, ie_ptr, 3);

            // TTI Bundling
            liblte_value_2_bits(mac_main_cnfg->ulsch_cnfg.tti_bundling, ie_ptr, 1);
        }

        // DRX Config
        if(mac_main_cnfg->drx_cnfg_present)
        {
            liblte_value_2_bits(mac_main_cnfg->drx_cnfg.setup_present, ie_ptr, 1);
            if(mac_main_cnfg->drx_cnfg.setup_present)
            {
                // Optional indicators
                liblte_value_2_bits(mac_main_cnfg->drx_cnfg.short_drx_present, ie_ptr, 1);

                // On Duration Timer
                liblte_value_2_bits(mac_main_cnfg->drx_cnfg.on_duration_timer, ie_ptr, 4);

                // DRX Inactivity Timer
                liblte_value_2_bits(mac_main_cnfg->drx_cnfg.drx_inactivity_timer, ie_ptr, 5);

                // DRX Retransmission Timer
                liblte_value_2_bits(mac_main_cnfg->drx_cnfg.drx_retx_timer, ie_ptr, 3);

                // Long DRX Cycle Start Offset
                liblte_value_2_bits(mac_main_cnfg->drx_cnfg.long_drx_cycle_start_offset_choice, ie_ptr, 4);
                switch(mac_main_cnfg->drx_cnfg.long_drx_cycle_start_offset_choice)
                {
                case LIBLTE_RRC_LONG_DRX_CYCLE_START_OFFSET_SF10:
                    liblte_value_2_bits(mac_main_cnfg->drx_cnfg.long_drx_cycle_start_offset, ie_ptr, 4);
                    break;
                case LIBLTE_RRC_LONG_DRX_CYCLE_START_OFFSET_SF20:
                case LIBLTE_RRC_LONG_DRX_CYCLE_START_OFFSET_SF32:
                    liblte_value_2_bits(mac_main_cnfg->drx_cnfg.long_drx_cycle_start_offset, ie_ptr, 5);
                    break;
                case LIBLTE_RRC_LONG_DRX_CYCLE_START_OFFSET_SF40:
                case LIBLTE_RRC_LONG_DRX_CYCLE_START_OFFSET_SF64:
                    liblte_value_2_bits(mac_main_cnfg->drx_cnfg.long_drx_cycle_start_offset, ie_ptr, 6);
                    break;
                case LIBLTE_RRC_LONG_DRX_CYCLE_START_OFFSET_SF80:
                case LIBLTE_RRC_LONG_DRX_CYCLE_START_OFFSET_SF128:
                    liblte_value_2_bits(mac_main_cnfg->drx_cnfg.long_drx_cycle_start_offset, ie_ptr, 7);
                    break;
                case LIBLTE_RRC_LONG_DRX_CYCLE_START_OFFSET_SF160:
                case LIBLTE_RRC_LONG_DRX_CYCLE_START_OFFSET_SF256:
                    liblte_value_2_bits(mac_main_cnfg->drx_cnfg.long_drx_cycle_start_offset, ie_ptr, 8);
                    break;
                case LIBLTE_RRC_LONG_DRX_CYCLE_START_OFFSET_SF320:
                case LIBLTE_RRC_LONG_DRX_CYCLE_START_OFFSET_SF512:
                    liblte_value_2_bits(mac_main_cnfg->drx_cnfg.long_drx_cycle_start_offset, ie_ptr, 9);
                    break;
                case LIBLTE_RRC_LONG_DRX_CYCLE_START_OFFSET_SF640:
                case LIBLTE_RRC_LONG_DRX_CYCLE_START_OFFSET_SF1024:
                    liblte_value_2_bits(mac_main_cnfg->drx_cnfg.long_drx_cycle_start_offset, ie_ptr, 10);
                    break;
                case LIBLTE_RRC_LONG_DRX_CYCLE_START_OFFSET_SF1280:
                case LIBLTE_RRC_LONG_DRX_CYCLE_START_OFFSET_SF2048:
                    liblte_value_2_bits(mac_main_cnfg->drx_cnfg.long_drx_cycle_start_offset, ie_ptr, 11);
                    break;
                case LIBLTE_RRC_LONG_DRX_CYCLE_START_OFFSET_SF2560:
                    liblte_value_2_bits(mac_main_cnfg->drx_cnfg.long_drx_cycle_start_offset, ie_ptr, 12);
                    break;
                }

                // Short DRX
                if(mac_main_cnfg->drx_cnfg.short_drx_present)
                {
                    // Short DRX Cycle
                    liblte_value_2_bits(mac_main_cnfg->drx_cnfg.short_drx_cycle, ie_ptr, 4);

                    // DRX Short Cycle Timer
                    liblte_value_2_bits(mac_main_cnfg->drx_cnfg.short_drx_cycle_timer - 1, ie_ptr, 4);
                }
            }
        }

        // Time Alignment Timer Dedicated
        liblte_rrc_pack_time_alignment_timer_ie(mac_main_cnfg->time_alignment_timer, ie_ptr);

        // PHR Config
        if(mac_main_cnfg->phr_cnfg_present)
        {
            liblte_value_2_bits(mac_main_cnfg->phr_cnfg.setup_present, ie_ptr, 1);
            if(mac_main_cnfg->phr_cnfg.setup_present)
            {
                // Periodic PHR Timer
                liblte_value_2_bits(mac_main_cnfg->phr_cnfg.periodic_phr_timer, ie_ptr, 3);

                // Prohibit PHR Timer
                liblte_value_2_bits(mac_main_cnfg->phr_cnfg.prohibit_phr_timer, ie_ptr, 3);

                // DL Pathloss Change
                liblte_value_2_bits(mac_main_cnfg->phr_cnfg.dl_pathloss_change, ie_ptr, 2);
            }
        }

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_unpack_mac_main_config_ie(uint8                             **ie_ptr,
                                                       LIBLTE_RRC_MAC_MAIN_CONFIG_STRUCT  *mac_main_cnfg)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;
    bool              ext;

    if(ie_ptr        != NULL &&
       mac_main_cnfg != NULL)
    {
        // Extension indicator
        ext = liblte_bits_2_value(ie_ptr, 1);

        // Optional indicators
        mac_main_cnfg->ulsch_cnfg_present = liblte_bits_2_value(ie_ptr, 1);
        mac_main_cnfg->drx_cnfg_present   = liblte_bits_2_value(ie_ptr, 1);
        mac_main_cnfg->phr_cnfg_present   = liblte_bits_2_value(ie_ptr, 1);

        // ULSCH Config
        if(mac_main_cnfg->ulsch_cnfg_present)
        {
            // Optional indicators
            mac_main_cnfg->ulsch_cnfg.max_harq_tx_present        = liblte_bits_2_value(ie_ptr, 1);
            mac_main_cnfg->ulsch_cnfg.periodic_bsr_timer_present = liblte_bits_2_value(ie_ptr, 1);

            // Max HARQ TX
            if(mac_main_cnfg->ulsch_cnfg.max_harq_tx_present)
            {
                mac_main_cnfg->ulsch_cnfg.max_harq_tx = (LIBLTE_RRC_MAX_HARQ_TX_ENUM)liblte_bits_2_value(ie_ptr, 4);
            }

            // Periodic BSR Timer
            if(mac_main_cnfg->ulsch_cnfg.periodic_bsr_timer_present)
            {
                mac_main_cnfg->ulsch_cnfg.periodic_bsr_timer = (LIBLTE_RRC_PERIODIC_BSR_TIMER_ENUM)liblte_bits_2_value(ie_ptr, 4);
            }

            // Re-TX BSR Timer
            mac_main_cnfg->ulsch_cnfg.retx_bsr_timer = (LIBLTE_RRC_RETRANSMISSION_BSR_TIMER_ENUM)liblte_bits_2_value(ie_ptr, 3);

            // TTI Bundling
            mac_main_cnfg->ulsch_cnfg.tti_bundling = liblte_bits_2_value(ie_ptr, 1);
        }

        // DRX Config
        if(mac_main_cnfg->drx_cnfg_present)
        {
            mac_main_cnfg->drx_cnfg.setup_present = liblte_bits_2_value(ie_ptr, 1);
            if(mac_main_cnfg->drx_cnfg.setup_present)
            {
                // Optional indicators
                mac_main_cnfg->drx_cnfg.short_drx_present = liblte_bits_2_value(ie_ptr, 1);

                // On Duration Timer
                mac_main_cnfg->drx_cnfg.on_duration_timer = (LIBLTE_RRC_ON_DURATION_TIMER_ENUM)liblte_bits_2_value(ie_ptr, 4);

                // DRX Inactivity Timer
                mac_main_cnfg->drx_cnfg.drx_inactivity_timer = (LIBLTE_RRC_DRX_INACTIVITY_TIMER_ENUM)liblte_bits_2_value(ie_ptr, 5);

                // DRX Retransmission Timer
                mac_main_cnfg->drx_cnfg.drx_retx_timer = (LIBLTE_RRC_DRX_RETRANSMISSION_TIMER_ENUM)liblte_bits_2_value(ie_ptr, 3);

                // Long DRX Cycle Short Offset
                mac_main_cnfg->drx_cnfg.long_drx_cycle_start_offset_choice = (LIBLTE_RRC_LONG_DRX_CYCLE_START_OFFSET_CHOICE_ENUM)liblte_bits_2_value(ie_ptr, 4);
                switch(mac_main_cnfg->drx_cnfg.long_drx_cycle_start_offset_choice)
                {
                case LIBLTE_RRC_LONG_DRX_CYCLE_START_OFFSET_SF10:
                    mac_main_cnfg->drx_cnfg.long_drx_cycle_start_offset = liblte_bits_2_value(ie_ptr, 4);
                    break;
                case LIBLTE_RRC_LONG_DRX_CYCLE_START_OFFSET_SF20:
                case LIBLTE_RRC_LONG_DRX_CYCLE_START_OFFSET_SF32:
                    mac_main_cnfg->drx_cnfg.long_drx_cycle_start_offset = liblte_bits_2_value(ie_ptr, 5);
                    break;
                case LIBLTE_RRC_LONG_DRX_CYCLE_START_OFFSET_SF40:
                case LIBLTE_RRC_LONG_DRX_CYCLE_START_OFFSET_SF64:
                    mac_main_cnfg->drx_cnfg.long_drx_cycle_start_offset = liblte_bits_2_value(ie_ptr, 6);
                    break;
                case LIBLTE_RRC_LONG_DRX_CYCLE_START_OFFSET_SF80:
                case LIBLTE_RRC_LONG_DRX_CYCLE_START_OFFSET_SF128:
                    mac_main_cnfg->drx_cnfg.long_drx_cycle_start_offset = liblte_bits_2_value(ie_ptr, 7);
                    break;
                case LIBLTE_RRC_LONG_DRX_CYCLE_START_OFFSET_SF160:
                case LIBLTE_RRC_LONG_DRX_CYCLE_START_OFFSET_SF256:
                    mac_main_cnfg->drx_cnfg.long_drx_cycle_start_offset = liblte_bits_2_value(ie_ptr, 8);
                    break;
                case LIBLTE_RRC_LONG_DRX_CYCLE_START_OFFSET_SF320:
                case LIBLTE_RRC_LONG_DRX_CYCLE_START_OFFSET_SF512:
                    mac_main_cnfg->drx_cnfg.long_drx_cycle_start_offset = liblte_bits_2_value(ie_ptr, 9);
                    break;
                case LIBLTE_RRC_LONG_DRX_CYCLE_START_OFFSET_SF640:
                case LIBLTE_RRC_LONG_DRX_CYCLE_START_OFFSET_SF1024:
                    mac_main_cnfg->drx_cnfg.long_drx_cycle_start_offset = liblte_bits_2_value(ie_ptr, 10);
                    break;
                case LIBLTE_RRC_LONG_DRX_CYCLE_START_OFFSET_SF1280:
                case LIBLTE_RRC_LONG_DRX_CYCLE_START_OFFSET_SF2048:
                    mac_main_cnfg->drx_cnfg.long_drx_cycle_start_offset = liblte_bits_2_value(ie_ptr, 11);
                    break;
                case LIBLTE_RRC_LONG_DRX_CYCLE_START_OFFSET_SF2560:
                    mac_main_cnfg->drx_cnfg.long_drx_cycle_start_offset = liblte_bits_2_value(ie_ptr, 12);
                    break;
                }

                // Short DRX
                if(mac_main_cnfg->drx_cnfg.short_drx_present)
                {
                    // Short DRX Cycle
                    mac_main_cnfg->drx_cnfg.short_drx_cycle = (LIBLTE_RRC_SHORT_DRX_CYCLE_ENUM)liblte_bits_2_value(ie_ptr, 4);

                    // DRX Short Cycle Timer
                    mac_main_cnfg->drx_cnfg.short_drx_cycle_timer = liblte_bits_2_value(ie_ptr, 4) + 1;
                }
            }
        }

        // Time Alignment Timer Dedicated
        liblte_rrc_unpack_time_alignment_timer_ie(ie_ptr, &mac_main_cnfg->time_alignment_timer);

        // PHR Config
        if(mac_main_cnfg->phr_cnfg_present)
        {
            mac_main_cnfg->phr_cnfg.setup_present = liblte_bits_2_value(ie_ptr, 1);
            if(mac_main_cnfg->phr_cnfg.setup_present)
            {
                // Periodic PHR Timer
                mac_main_cnfg->phr_cnfg.periodic_phr_timer = (LIBLTE_RRC_PERIODIC_PHR_TIMER_ENUM)liblte_bits_2_value(ie_ptr, 3);

                // Prohibit PHR Timer
                mac_main_cnfg->phr_cnfg.prohibit_phr_timer = (LIBLTE_RRC_PROHIBIT_PHR_TIMER_ENUM)liblte_bits_2_value(ie_ptr, 3);

                // DL Pathloss Change
                mac_main_cnfg->phr_cnfg.dl_pathloss_change = (LIBLTE_RRC_DL_PATHLOSS_CHANGE_ENUM)liblte_bits_2_value(ie_ptr, 2);
            }
        }

        // Consume non-critical extensions
        liblte_rrc_consume_noncrit_extension(ext, __func__, ie_ptr);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: PDCP Config

    Description: Sets the configurable PDCP parameters for data
                 radio bearers

    Document Reference: 36.331 v10.0.0 Section 6.3.2
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_rrc_pack_pdcp_config_ie(LIBLTE_RRC_PDCP_CONFIG_STRUCT  *pdcp_cnfg,
                                                 uint8                         **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(pdcp_cnfg != NULL &&
       ie_ptr    != NULL)
    {
        // Extension indicator
        liblte_value_2_bits(0, ie_ptr, 1);

        // Optional indicators
        liblte_value_2_bits(pdcp_cnfg->discard_timer_present,                 ie_ptr, 1);
        liblte_value_2_bits(pdcp_cnfg->rlc_am_status_report_required_present, ie_ptr, 1);
        liblte_value_2_bits(pdcp_cnfg->rlc_um_pdcp_sn_size_present,           ie_ptr, 1);

        // Discard Timer
        if(pdcp_cnfg->discard_timer_present)
        {
            liblte_value_2_bits(pdcp_cnfg->discard_timer, ie_ptr, 3);
        }

        // RLC AM
        if(pdcp_cnfg->rlc_am_status_report_required_present)
        {
            liblte_value_2_bits(pdcp_cnfg->rlc_am_status_report_required, ie_ptr, 1);
        }

        // RLC UM
        if(pdcp_cnfg->rlc_um_pdcp_sn_size_present)
        {
            liblte_value_2_bits(pdcp_cnfg->rlc_um_pdcp_sn_size, ie_ptr, 1);
        }

        // Header Compression
        liblte_value_2_bits(pdcp_cnfg->hdr_compression_rohc, ie_ptr, 1);
        if(pdcp_cnfg->hdr_compression_rohc)
        {
            // Extension indicator
            liblte_value_2_bits(0, ie_ptr, 1);

            // Max CID
            liblte_value_2_bits(pdcp_cnfg->hdr_compression_max_cid - 1, ie_ptr, 14);

            // Profiles
            liblte_value_2_bits(pdcp_cnfg->hdr_compression_profile_0001, ie_ptr, 1);
            liblte_value_2_bits(pdcp_cnfg->hdr_compression_profile_0002, ie_ptr, 1);
            liblte_value_2_bits(pdcp_cnfg->hdr_compression_profile_0003, ie_ptr, 1);
            liblte_value_2_bits(pdcp_cnfg->hdr_compression_profile_0004, ie_ptr, 1);
            liblte_value_2_bits(pdcp_cnfg->hdr_compression_profile_0006, ie_ptr, 1);
            liblte_value_2_bits(pdcp_cnfg->hdr_compression_profile_0101, ie_ptr, 1);
            liblte_value_2_bits(pdcp_cnfg->hdr_compression_profile_0102, ie_ptr, 1);
            liblte_value_2_bits(pdcp_cnfg->hdr_compression_profile_0103, ie_ptr, 1);
            liblte_value_2_bits(pdcp_cnfg->hdr_compression_profile_0104, ie_ptr, 1);
        }

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_unpack_pdcp_config_ie(uint8                         **ie_ptr,
                                                   LIBLTE_RRC_PDCP_CONFIG_STRUCT  *pdcp_cnfg)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr    != NULL &&
       pdcp_cnfg != NULL)
    {
        // Extension indicator
        bool ext = liblte_bits_2_value(ie_ptr, 1);

        // Optional indicators
        pdcp_cnfg->discard_timer_present                 = liblte_bits_2_value(ie_ptr, 1);
        pdcp_cnfg->rlc_am_status_report_required_present = liblte_bits_2_value(ie_ptr, 1);
        pdcp_cnfg->rlc_um_pdcp_sn_size_present           = liblte_bits_2_value(ie_ptr, 1);

        // Discard Timer
        if(pdcp_cnfg->discard_timer_present)
        {
            pdcp_cnfg->discard_timer = (LIBLTE_RRC_DISCARD_TIMER_ENUM)liblte_bits_2_value(ie_ptr, 3);
        }

        // RLC AM
        if(pdcp_cnfg->rlc_am_status_report_required_present)
        {
            pdcp_cnfg->rlc_am_status_report_required = liblte_bits_2_value(ie_ptr, 1);
        }

        // RLC UM
        if(pdcp_cnfg->rlc_um_pdcp_sn_size_present)
        {
            pdcp_cnfg->rlc_um_pdcp_sn_size = (LIBLTE_RRC_PDCP_SN_SIZE_ENUM)liblte_bits_2_value(ie_ptr, 1);
        }

        // Header Compression
        pdcp_cnfg->hdr_compression_rohc = liblte_bits_2_value(ie_ptr, 1);
        if(pdcp_cnfg->hdr_compression_rohc)
        {
            // Extension indicator
            bool ext2 = liblte_bits_2_value(ie_ptr, 1);

            // Max CID
            pdcp_cnfg->hdr_compression_max_cid = liblte_bits_2_value(ie_ptr, 14) + 1;

            // Profiles
            pdcp_cnfg->hdr_compression_profile_0001 = liblte_bits_2_value(ie_ptr, 1);
            pdcp_cnfg->hdr_compression_profile_0002 = liblte_bits_2_value(ie_ptr, 1);
            pdcp_cnfg->hdr_compression_profile_0003 = liblte_bits_2_value(ie_ptr, 1);
            pdcp_cnfg->hdr_compression_profile_0004 = liblte_bits_2_value(ie_ptr, 1);
            pdcp_cnfg->hdr_compression_profile_0006 = liblte_bits_2_value(ie_ptr, 1);
            pdcp_cnfg->hdr_compression_profile_0101 = liblte_bits_2_value(ie_ptr, 1);
            pdcp_cnfg->hdr_compression_profile_0102 = liblte_bits_2_value(ie_ptr, 1);
            pdcp_cnfg->hdr_compression_profile_0103 = liblte_bits_2_value(ie_ptr, 1);
            pdcp_cnfg->hdr_compression_profile_0104 = liblte_bits_2_value(ie_ptr, 1);
            
            liblte_rrc_consume_noncrit_extension(ext2, __func__, ie_ptr);
        }
        
        liblte_rrc_consume_noncrit_extension(ext, __func__, ie_ptr);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: PDSCH Config

    Description: Specifies the common and the UE specific PDSCH
                 configuration

    Document Reference: 36.331 v10.0.0 Section 6.3.2
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_rrc_pack_pdsch_config_common_ie(LIBLTE_RRC_PDSCH_CONFIG_COMMON_STRUCT  *pdsch_config,
                                                         uint8                                 **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(pdsch_config != NULL &&
       ie_ptr       != NULL)
    {
        liblte_value_2_bits(pdsch_config->rs_power + 60, ie_ptr, 7);
        liblte_value_2_bits(pdsch_config->p_b,           ie_ptr, 2);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_unpack_pdsch_config_common_ie(uint8                                 **ie_ptr,
                                                           LIBLTE_RRC_PDSCH_CONFIG_COMMON_STRUCT  *pdsch_config)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr       != NULL &&
       pdsch_config != NULL)
    {
        pdsch_config->rs_power = liblte_bits_2_value(ie_ptr, 7) - 60;
        pdsch_config->p_b      = liblte_bits_2_value(ie_ptr, 2);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_pack_pdsch_config_dedicated_ie(LIBLTE_RRC_PDSCH_CONFIG_P_A_ENUM   p_a,
                                                            uint8                            **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr != NULL)
    {
        liblte_value_2_bits(p_a, ie_ptr, 3);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_unpack_pdsch_config_dedicated_ie(uint8                            **ie_ptr,
                                                              LIBLTE_RRC_PDSCH_CONFIG_P_A_ENUM  *p_a)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(p_a    != NULL &&
       ie_ptr != NULL)
    {
        *p_a = (LIBLTE_RRC_PDSCH_CONFIG_P_A_ENUM)liblte_bits_2_value(ie_ptr, 3);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: PHICH Config

    Description: Specifies the PHICH configuration

    Document Reference: 36.331 v10.0.0 Section 6.3.2
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_rrc_pack_phich_config_ie(LIBLTE_RRC_PHICH_CONFIG_STRUCT  *phich_config,
                                                  uint8                          **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(phich_config != NULL &&
       ie_ptr       != NULL)
    {
        liblte_value_2_bits(phich_config->dur, ie_ptr, 1);
        liblte_value_2_bits(phich_config->res, ie_ptr, 2);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_unpack_phich_config_ie(uint8                          **ie_ptr,
                                                    LIBLTE_RRC_PHICH_CONFIG_STRUCT  *phich_config)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr       != NULL &&
       phich_config != NULL)
    {
        phich_config->dur = (LIBLTE_RRC_PHICH_DURATION_ENUM)liblte_bits_2_value(ie_ptr, 1);
        phich_config->res = (LIBLTE_RRC_PHICH_RESOURCE_ENUM)liblte_bits_2_value(ie_ptr, 2);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: Physical Config Dedicated

    Description: Specifies the UE specific physical channel
                 configuration

    Document Reference: 36.331 v10.0.0 Section 6.3.2
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_rrc_pack_physical_config_dedicated_ie(LIBLTE_RRC_PHYSICAL_CONFIG_DEDICATED_STRUCT  *phy_cnfg_ded,
                                                               uint8                                       **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;
    bool              ext = false;

    if(phy_cnfg_ded != NULL &&
       ie_ptr       != NULL)
    {
        // Extension indicator
        liblte_value_2_bits(ext, ie_ptr, 1);

        // Optional indicators
        liblte_value_2_bits(phy_cnfg_ded->pdsch_cnfg_ded_present,       ie_ptr, 1);
        liblte_value_2_bits(phy_cnfg_ded->pucch_cnfg_ded_present,       ie_ptr, 1);
        liblte_value_2_bits(phy_cnfg_ded->pusch_cnfg_ded_present,       ie_ptr, 1);
        liblte_value_2_bits(phy_cnfg_ded->ul_pwr_ctrl_ded_present,      ie_ptr, 1);
        liblte_value_2_bits(phy_cnfg_ded->tpc_pdcch_cnfg_pucch_present, ie_ptr, 1);
        liblte_value_2_bits(phy_cnfg_ded->tpc_pdcch_cnfg_pusch_present, ie_ptr, 1);
        liblte_value_2_bits(phy_cnfg_ded->cqi_report_cnfg_present,      ie_ptr, 1);
        liblte_value_2_bits(phy_cnfg_ded->srs_ul_cnfg_ded_present,      ie_ptr, 1);
        liblte_value_2_bits(phy_cnfg_ded->antenna_info_present,         ie_ptr, 1);
        liblte_value_2_bits(phy_cnfg_ded->sched_request_cnfg_present,   ie_ptr, 1);

        // PDSCH Config
        if(phy_cnfg_ded->pdsch_cnfg_ded_present)
        {
            liblte_rrc_pack_pdsch_config_dedicated_ie(phy_cnfg_ded->pdsch_cnfg_ded, ie_ptr);
        }

        // PUCCH Config
        if(phy_cnfg_ded->pucch_cnfg_ded_present)
        {
            liblte_rrc_pack_pucch_config_dedicated_ie(&phy_cnfg_ded->pucch_cnfg_ded, ie_ptr);
        }

        // PUSCH Config
        if(phy_cnfg_ded->pusch_cnfg_ded_present)
        {
            liblte_rrc_pack_pusch_config_dedicated_ie(&phy_cnfg_ded->pusch_cnfg_ded, ie_ptr);
        }

        // Uplink Power Control
        if(phy_cnfg_ded->ul_pwr_ctrl_ded_present)
        {
            liblte_rrc_pack_ul_power_control_dedicated_ie(&phy_cnfg_ded->ul_pwr_ctrl_ded, ie_ptr);
        }

        // TPC PDCCH Config PUCCH
        if(phy_cnfg_ded->tpc_pdcch_cnfg_pucch_present)
        {
            liblte_rrc_pack_tpc_pdcch_config_ie(&phy_cnfg_ded->tpc_pdcch_cnfg_pucch, ie_ptr);
        }

        // TPC PDCCH Config PUSCH
        if(phy_cnfg_ded->tpc_pdcch_cnfg_pusch_present)
        {
            liblte_rrc_pack_tpc_pdcch_config_ie(&phy_cnfg_ded->tpc_pdcch_cnfg_pusch, ie_ptr);
        }

        // CQI Report Config
        if(phy_cnfg_ded->cqi_report_cnfg_present)
        {
            liblte_rrc_pack_cqi_report_config_ie(&phy_cnfg_ded->cqi_report_cnfg, ie_ptr);
        }

        // SRS UL Config
        if(phy_cnfg_ded->srs_ul_cnfg_ded_present)
        {
            liblte_rrc_pack_srs_ul_config_dedicated_ie(&phy_cnfg_ded->srs_ul_cnfg_ded, ie_ptr);
        }

        // Antenna Info
        if(phy_cnfg_ded->antenna_info_present)
        {
            liblte_value_2_bits(phy_cnfg_ded->antenna_info_default_value, ie_ptr, 1);
            if(!phy_cnfg_ded->antenna_info_default_value)
            {
                liblte_rrc_pack_antenna_info_dedicated_ie(&phy_cnfg_ded->antenna_info_explicit_value, ie_ptr);
            }
        }

        // Scheduling Request Config
        if(phy_cnfg_ded->sched_request_cnfg_present)
        {
            liblte_rrc_pack_scheduling_request_config_ie(&phy_cnfg_ded->sched_request_cnfg, ie_ptr);
        }

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_unpack_physical_config_dedicated_ie(uint8                                       **ie_ptr,
                                                                 LIBLTE_RRC_PHYSICAL_CONFIG_DEDICATED_STRUCT  *phy_cnfg_ded)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;
    bool              ext;

    if(ie_ptr       != NULL &&
       phy_cnfg_ded != NULL)
    {
        // Extension indicator
        ext = liblte_bits_2_value(ie_ptr, 1);

        // Optional indicators
        phy_cnfg_ded->pdsch_cnfg_ded_present       = liblte_bits_2_value(ie_ptr, 1);
        phy_cnfg_ded->pucch_cnfg_ded_present       = liblte_bits_2_value(ie_ptr, 1);
        phy_cnfg_ded->pusch_cnfg_ded_present       = liblte_bits_2_value(ie_ptr, 1);
        phy_cnfg_ded->ul_pwr_ctrl_ded_present      = liblte_bits_2_value(ie_ptr, 1);
        phy_cnfg_ded->tpc_pdcch_cnfg_pucch_present = liblte_bits_2_value(ie_ptr, 1);
        phy_cnfg_ded->tpc_pdcch_cnfg_pusch_present = liblte_bits_2_value(ie_ptr, 1);
        phy_cnfg_ded->cqi_report_cnfg_present      = liblte_bits_2_value(ie_ptr, 1);
        phy_cnfg_ded->srs_ul_cnfg_ded_present      = liblte_bits_2_value(ie_ptr, 1);
        phy_cnfg_ded->antenna_info_present         = liblte_bits_2_value(ie_ptr, 1);
        phy_cnfg_ded->sched_request_cnfg_present   = liblte_bits_2_value(ie_ptr, 1);

        // PDSCH Config
        if(phy_cnfg_ded->pdsch_cnfg_ded_present)
        {
            liblte_rrc_unpack_pdsch_config_dedicated_ie(ie_ptr, &phy_cnfg_ded->pdsch_cnfg_ded);
        }

        // PUCCH Config
        if(phy_cnfg_ded->pucch_cnfg_ded_present)
        {
            liblte_rrc_unpack_pucch_config_dedicated_ie(ie_ptr, &phy_cnfg_ded->pucch_cnfg_ded);
        }

        // PUSCH Config
        if(phy_cnfg_ded->pusch_cnfg_ded_present)
        {
            liblte_rrc_unpack_pusch_config_dedicated_ie(ie_ptr, &phy_cnfg_ded->pusch_cnfg_ded);
        }

        // Uplink Power Control
        if(phy_cnfg_ded->ul_pwr_ctrl_ded_present)
        {
            liblte_rrc_unpack_ul_power_control_dedicated_ie(ie_ptr, &phy_cnfg_ded->ul_pwr_ctrl_ded);
        }

        // TPC PDCCH Config PUCCH
        if(phy_cnfg_ded->tpc_pdcch_cnfg_pucch_present)
        {
            liblte_rrc_unpack_tpc_pdcch_config_ie(ie_ptr, &phy_cnfg_ded->tpc_pdcch_cnfg_pucch);
        }

        // TPC PDCCH Config PUSCH
        if(phy_cnfg_ded->tpc_pdcch_cnfg_pusch_present)
        {
            liblte_rrc_unpack_tpc_pdcch_config_ie(ie_ptr, &phy_cnfg_ded->tpc_pdcch_cnfg_pusch);
        }

        // CQI Report Config
        if(phy_cnfg_ded->cqi_report_cnfg_present)
        {
            liblte_rrc_unpack_cqi_report_config_ie(ie_ptr, &phy_cnfg_ded->cqi_report_cnfg);
        }

        // SRS UL Config
        if(phy_cnfg_ded->srs_ul_cnfg_ded_present)
        {
            liblte_rrc_unpack_srs_ul_config_dedicated_ie(ie_ptr, &phy_cnfg_ded->srs_ul_cnfg_ded);
        }

        // Antenna Info
        if(phy_cnfg_ded->antenna_info_present)
        {
            phy_cnfg_ded->antenna_info_default_value = liblte_bits_2_value(ie_ptr, 1);
            if(!phy_cnfg_ded->antenna_info_default_value)
            {
                liblte_rrc_unpack_antenna_info_dedicated_ie(ie_ptr, &phy_cnfg_ded->antenna_info_explicit_value);
            }
        }

        // Scheduling Request Config
        if(phy_cnfg_ded->sched_request_cnfg_present)
        {
            liblte_rrc_unpack_scheduling_request_config_ie(ie_ptr, &phy_cnfg_ded->sched_request_cnfg);
        }

        // Consume non-critical extensions
        liblte_rrc_consume_noncrit_extension(ext, __func__, ie_ptr);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: P Max

    Description: Limits the UE's uplink transmission power on a
                 carrier frequency and is used to calculate the
                 parameter P Compensation

    Document Reference: 36.331 v10.0.0 Section 6.3.2
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_rrc_pack_p_max_ie(int8    p_max,
                                           uint8 **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr != NULL)
    {
        liblte_value_2_bits(p_max + 30, ie_ptr, 6);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_unpack_p_max_ie(uint8 **ie_ptr,
                                             int8   *p_max)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr != NULL &&
       p_max  != NULL)
    {
        *p_max = (int8)liblte_bits_2_value(ie_ptr, 6) - 30;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: PRACH Config

    Description: Specifies the PRACH configuration in the system
                 information and in the mobility control information

    Document Reference: 36.331 v10.0.0 Section 6.3.2
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_rrc_pack_prach_config_sib_ie(LIBLTE_RRC_PRACH_CONFIG_SIB_STRUCT  *prach_cnfg,
                                                      uint8                              **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(prach_cnfg != NULL &&
       ie_ptr     != NULL)
    {
        liblte_value_2_bits(prach_cnfg->root_sequence_index,                          ie_ptr, 10);
        liblte_value_2_bits(prach_cnfg->prach_cnfg_info.prach_config_index,           ie_ptr,  6);
        liblte_value_2_bits(prach_cnfg->prach_cnfg_info.high_speed_flag,              ie_ptr,  1);
        liblte_value_2_bits(prach_cnfg->prach_cnfg_info.zero_correlation_zone_config, ie_ptr,  4);
        liblte_value_2_bits(prach_cnfg->prach_cnfg_info.prach_freq_offset,            ie_ptr,  7);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_unpack_prach_config_sib_ie(uint8                              **ie_ptr,
                                                        LIBLTE_RRC_PRACH_CONFIG_SIB_STRUCT  *prach_cnfg)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr     != NULL &&
       prach_cnfg != NULL)
    {
        prach_cnfg->root_sequence_index                          = liblte_bits_2_value(ie_ptr, 10);
        prach_cnfg->prach_cnfg_info.prach_config_index           = liblte_bits_2_value(ie_ptr,  6);
        prach_cnfg->prach_cnfg_info.high_speed_flag              = liblte_bits_2_value(ie_ptr,  1);
        prach_cnfg->prach_cnfg_info.zero_correlation_zone_config = liblte_bits_2_value(ie_ptr,  4);
        prach_cnfg->prach_cnfg_info.prach_freq_offset            = liblte_bits_2_value(ie_ptr,  7);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_pack_prach_config_ie(LIBLTE_RRC_PRACH_CONFIG_STRUCT  *prach_cnfg,
                                                  uint8                          **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(prach_cnfg != NULL &&
       ie_ptr     != NULL)
    {
        // Optional indicator
        liblte_value_2_bits(prach_cnfg->prach_cnfg_info_present, ie_ptr, 1);

        liblte_value_2_bits(prach_cnfg->root_sequence_index, ie_ptr, 10);

        if(true == prach_cnfg->prach_cnfg_info_present)
        {
            liblte_value_2_bits(prach_cnfg->prach_cnfg_info.prach_config_index,           ie_ptr, 6);
            liblte_value_2_bits(prach_cnfg->prach_cnfg_info.high_speed_flag,              ie_ptr, 1);
            liblte_value_2_bits(prach_cnfg->prach_cnfg_info.zero_correlation_zone_config, ie_ptr, 4);
            liblte_value_2_bits(prach_cnfg->prach_cnfg_info.prach_freq_offset,            ie_ptr, 7);
        }

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_unpack_prach_config_ie(uint8                          **ie_ptr,
                                                    LIBLTE_RRC_PRACH_CONFIG_STRUCT  *prach_cnfg)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr     != NULL &&
       prach_cnfg != NULL)
    {
        // Optional indicator
        prach_cnfg->prach_cnfg_info_present = liblte_bits_2_value(ie_ptr, 1);

        prach_cnfg->root_sequence_index = liblte_bits_2_value(ie_ptr, 10);

        if(true == prach_cnfg->prach_cnfg_info_present)
        {
            prach_cnfg->prach_cnfg_info.prach_config_index           = liblte_bits_2_value(ie_ptr, 6);
            prach_cnfg->prach_cnfg_info.high_speed_flag              = liblte_bits_2_value(ie_ptr, 1);
            prach_cnfg->prach_cnfg_info.zero_correlation_zone_config = liblte_bits_2_value(ie_ptr, 4);
            prach_cnfg->prach_cnfg_info.prach_freq_offset            = liblte_bits_2_value(ie_ptr, 7);
        }

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_pack_prach_config_scell_r10_ie(uint8   prach_cnfg_idx,
                                                            uint8 **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr != NULL)
    {
        liblte_value_2_bits(prach_cnfg_idx, ie_ptr, 6);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_unpack_prach_config_scell_r10_ie(uint8 **ie_ptr,
                                                              uint8  *prach_cnfg_idx)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr         != NULL &&
       prach_cnfg_idx != NULL)
    {
        *prach_cnfg_idx = liblte_bits_2_value(ie_ptr, 6);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: Presence Antenna Port 1

    Description: Indicates whether all the neighboring cells use
                 antenna port 1

    Document Reference: 36.331 v10.0.0 Section 6.3.2
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_rrc_pack_presence_antenna_port_1_ie(bool    presence_ant_port_1,
                                                             uint8 **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr != NULL)
    {
        liblte_value_2_bits(presence_ant_port_1, ie_ptr, 1);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_unpack_presence_antenna_port_1_ie(uint8 **ie_ptr,
                                                               bool   *presence_ant_port_1)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr              != NULL &&
       presence_ant_port_1 != NULL)
    {
        *presence_ant_port_1 = liblte_bits_2_value(ie_ptr, 1);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: PUCCH Config

    Description: Specifies the common and the UE specific PUCCH
                 configuration

    Document Reference: 36.331 v10.0.0 Section 6.3.2
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_rrc_pack_pucch_config_common_ie(LIBLTE_RRC_PUCCH_CONFIG_COMMON_STRUCT  *pucch_cnfg,
                                                         uint8                                 **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(pucch_cnfg != NULL &&
       ie_ptr     != NULL)
    {
        liblte_value_2_bits(pucch_cnfg->delta_pucch_shift, ie_ptr,  2);
        liblte_value_2_bits(pucch_cnfg->n_rb_cqi,          ie_ptr,  7);
        liblte_value_2_bits(pucch_cnfg->n_cs_an,           ie_ptr,  3);
        liblte_value_2_bits(pucch_cnfg->n1_pucch_an,       ie_ptr, 11);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_unpack_pucch_config_common_ie(uint8                                 **ie_ptr,
                                                           LIBLTE_RRC_PUCCH_CONFIG_COMMON_STRUCT  *pucch_cnfg)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr     != NULL &&
       pucch_cnfg != NULL)
    {
        pucch_cnfg->delta_pucch_shift = (LIBLTE_RRC_DELTA_PUCCH_SHIFT_ENUM)liblte_bits_2_value(ie_ptr, 2);
        pucch_cnfg->n_rb_cqi          = liblte_bits_2_value(ie_ptr,  7);
        pucch_cnfg->n_cs_an           = liblte_bits_2_value(ie_ptr,  3);
        pucch_cnfg->n1_pucch_an       = liblte_bits_2_value(ie_ptr, 11);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_pack_pucch_config_dedicated_ie(LIBLTE_RRC_PUCCH_CONFIG_DEDICATED_STRUCT  *pucch_cnfg,
                                                            uint8                                    **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(pucch_cnfg != NULL &&
       ie_ptr     != NULL)
    {
        // Optional indicator
        liblte_value_2_bits(pucch_cnfg->tdd_ack_nack_feedback_mode_present, ie_ptr, 1);

        // Ack/Nack Repetition
        liblte_value_2_bits(pucch_cnfg->ack_nack_repetition_setup_present, ie_ptr, 1);
        if(pucch_cnfg->ack_nack_repetition_setup_present)
        {
            // Repetition Factor
            liblte_value_2_bits(pucch_cnfg->ack_nack_repetition_factor, ie_ptr, 2);

            // N1 PUCCH AN Repetition
            liblte_value_2_bits(pucch_cnfg->ack_nack_repetition_n1_pucch_an, ie_ptr, 11);
        }

        // TDD Ack/Nack Feedback Mode
        if(pucch_cnfg->tdd_ack_nack_feedback_mode_present)
        {
            liblte_value_2_bits(pucch_cnfg->tdd_ack_nack_feedback_mode, ie_ptr, 1);
        }

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_unpack_pucch_config_dedicated_ie(uint8                                    **ie_ptr,
                                                              LIBLTE_RRC_PUCCH_CONFIG_DEDICATED_STRUCT  *pucch_cnfg)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr     != NULL &&
       pucch_cnfg != NULL)
    {
        // Optional indicator
        pucch_cnfg->tdd_ack_nack_feedback_mode_present = liblte_bits_2_value(ie_ptr, 1);

        // Ack/Nack Repetition
        pucch_cnfg->ack_nack_repetition_setup_present = liblte_bits_2_value(ie_ptr, 1);
        if(pucch_cnfg->ack_nack_repetition_setup_present)
        {
            // Repetition Factor
            pucch_cnfg->ack_nack_repetition_factor = (LIBLTE_RRC_ACK_NACK_REPETITION_FACTOR_ENUM)liblte_bits_2_value(ie_ptr, 2);

            // N1 PUCCH AN Repetition
            pucch_cnfg->ack_nack_repetition_n1_pucch_an = liblte_bits_2_value(ie_ptr, 11);
        }

        // TDD Ack/Nack Feedback Mode
        if(pucch_cnfg->tdd_ack_nack_feedback_mode_present)
        {
            pucch_cnfg->tdd_ack_nack_feedback_mode = (LIBLTE_RRC_TDD_ACK_NACK_FEEDBACK_MODE_ENUM)liblte_bits_2_value(ie_ptr, 1);
        }

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: PUSCH Config

    Description: Specifies the common and the UE specific PUSCH
                 configuration and the reference signal configuration
                 for PUSCH and PUCCH

    Document Reference: 36.331 v10.0.0 Section 6.3.2
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_rrc_pack_pusch_config_common_ie(LIBLTE_RRC_PUSCH_CONFIG_COMMON_STRUCT  *pusch_cnfg,
                                                         uint8                                 **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(pusch_cnfg != NULL &&
       ie_ptr     != NULL)
    {
        // PUSCH Config Basic
        liblte_value_2_bits(pusch_cnfg->n_sb - 1,             ie_ptr, 2);
        liblte_value_2_bits(pusch_cnfg->hopping_mode,         ie_ptr, 1);
        liblte_value_2_bits(pusch_cnfg->pusch_hopping_offset, ie_ptr, 7);
        liblte_value_2_bits(pusch_cnfg->enable_64_qam,        ie_ptr, 1);

        // UL Reference Signals PUSCH
        liblte_value_2_bits(pusch_cnfg->ul_rs.group_hopping_enabled,    ie_ptr, 1);
        liblte_value_2_bits(pusch_cnfg->ul_rs.group_assignment_pusch,   ie_ptr, 5);
        liblte_value_2_bits(pusch_cnfg->ul_rs.sequence_hopping_enabled, ie_ptr, 1);
        liblte_value_2_bits(pusch_cnfg->ul_rs.cyclic_shift,             ie_ptr, 3);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_unpack_pusch_config_common_ie(uint8                                 **ie_ptr,
                                                           LIBLTE_RRC_PUSCH_CONFIG_COMMON_STRUCT  *pusch_cnfg)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr     != NULL &&
       pusch_cnfg != NULL)
    {
        // PUSCH Config Basic
        pusch_cnfg->n_sb                 = liblte_bits_2_value(ie_ptr, 2) + 1;
        pusch_cnfg->hopping_mode         = (LIBLTE_RRC_HOPPING_MODE_ENUM)liblte_bits_2_value(ie_ptr, 1);
        pusch_cnfg->pusch_hopping_offset = liblte_bits_2_value(ie_ptr, 7);
        pusch_cnfg->enable_64_qam        = liblte_bits_2_value(ie_ptr, 1);

        // UL Reference Signals PUSCH
        pusch_cnfg->ul_rs.group_hopping_enabled    = liblte_bits_2_value(ie_ptr, 1);
        pusch_cnfg->ul_rs.group_assignment_pusch   = liblte_bits_2_value(ie_ptr, 5);
        pusch_cnfg->ul_rs.sequence_hopping_enabled = liblte_bits_2_value(ie_ptr, 1);
        pusch_cnfg->ul_rs.cyclic_shift             = liblte_bits_2_value(ie_ptr, 3);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_pack_pusch_config_dedicated_ie(LIBLTE_RRC_PUSCH_CONFIG_DEDICATED_STRUCT  *pusch_cnfg,
                                                            uint8                                    **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(pusch_cnfg != NULL &&
       ie_ptr     != NULL)
    {
        // Beta Offset ACK Index
        liblte_value_2_bits(pusch_cnfg->beta_offset_ack_idx, ie_ptr, 4);

        // Beta Offset RI Index
        liblte_value_2_bits(pusch_cnfg->beta_offset_ri_idx, ie_ptr, 4);

        // Beta Offset CQI Index
        liblte_value_2_bits(pusch_cnfg->beta_offset_cqi_idx, ie_ptr, 4);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_unpack_pusch_config_dedicated_ie(uint8                                    **ie_ptr,
                                                              LIBLTE_RRC_PUSCH_CONFIG_DEDICATED_STRUCT  *pusch_cnfg)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr     != NULL &&
       pusch_cnfg != NULL)
    {
        // Beta Offset ACK Index
        pusch_cnfg->beta_offset_ack_idx = liblte_bits_2_value(ie_ptr, 4);

        // Beta Offset RI Index
        pusch_cnfg->beta_offset_ri_idx = liblte_bits_2_value(ie_ptr, 4);

        // Beta Offset CQI Index
        pusch_cnfg->beta_offset_cqi_idx = liblte_bits_2_value(ie_ptr, 4);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: RACH Config Common

    Description: Specifies the generic random access parameters

    Document Reference: 36.331 v10.0.0 Section 6.3.2
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_rrc_pack_rach_config_common_ie(LIBLTE_RRC_RACH_CONFIG_COMMON_STRUCT  *rach_cnfg,
                                                        uint8                                **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(rach_cnfg != NULL &&
       ie_ptr    != NULL)
    {
        // Extension indicator
        liblte_value_2_bits(0, ie_ptr, 1);

        // Preamble Info
        liblte_value_2_bits(rach_cnfg->preambles_group_a_cnfg.present, ie_ptr, 1);
        liblte_value_2_bits(rach_cnfg->num_ra_preambles,               ie_ptr, 4);
        if(true == rach_cnfg->preambles_group_a_cnfg.present)
        {
            // Extension indicator
            liblte_value_2_bits(0, ie_ptr, 1);

            liblte_value_2_bits(rach_cnfg->preambles_group_a_cnfg.size_of_ra,             ie_ptr, 4);
            liblte_value_2_bits(rach_cnfg->preambles_group_a_cnfg.msg_size,               ie_ptr, 2);
            liblte_value_2_bits(rach_cnfg->preambles_group_a_cnfg.msg_pwr_offset_group_b, ie_ptr, 3);
        }

        // Power Ramping Parameters
        liblte_value_2_bits(rach_cnfg->pwr_ramping_step,            ie_ptr, 2);
        liblte_value_2_bits(rach_cnfg->preamble_init_rx_target_pwr, ie_ptr, 4);

        // RA Supervision Info
        liblte_value_2_bits(rach_cnfg->preamble_trans_max, ie_ptr, 4);
        liblte_value_2_bits(rach_cnfg->ra_resp_win_size,   ie_ptr, 3);
        liblte_value_2_bits(rach_cnfg->mac_con_res_timer,  ie_ptr, 3);

        liblte_value_2_bits(rach_cnfg->max_harq_msg3_tx - 1, ie_ptr, 3);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_unpack_rach_config_common_ie(uint8                                **ie_ptr,
                                                          LIBLTE_RRC_RACH_CONFIG_COMMON_STRUCT  *rach_cnfg)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr    != NULL &&
       rach_cnfg != NULL)
    {
        // Extension indicator
        bool ext = liblte_bits_2_value(ie_ptr, 1);

        // Preamble Info
        rach_cnfg->preambles_group_a_cnfg.present = liblte_bits_2_value(ie_ptr, 1);
        rach_cnfg->num_ra_preambles               = (LIBLTE_RRC_NUMBER_OF_RA_PREAMBLES_ENUM)liblte_bits_2_value(ie_ptr, 4);
        if(true == rach_cnfg->preambles_group_a_cnfg.present)
        {
            // Extension indicator
            bool ext2 = liblte_bits_2_value(ie_ptr, 1);

            rach_cnfg->preambles_group_a_cnfg.size_of_ra             = (LIBLTE_RRC_SIZE_OF_RA_PREAMBLES_GROUP_A_ENUM)liblte_bits_2_value(ie_ptr, 4);
            rach_cnfg->preambles_group_a_cnfg.msg_size               = (LIBLTE_RRC_MESSAGE_SIZE_GROUP_A_ENUM)liblte_bits_2_value(ie_ptr, 2);
            rach_cnfg->preambles_group_a_cnfg.msg_pwr_offset_group_b = (LIBLTE_RRC_MESSAGE_POWER_OFFSET_GROUP_B_ENUM)liblte_bits_2_value(ie_ptr, 3);
            
            liblte_rrc_consume_noncrit_extension(ext2, __func__, ie_ptr);
            
        }else{
            rach_cnfg->preambles_group_a_cnfg.size_of_ra = (LIBLTE_RRC_SIZE_OF_RA_PREAMBLES_GROUP_A_ENUM)rach_cnfg->num_ra_preambles;
        }

        // Power Ramping Parameters
        rach_cnfg->pwr_ramping_step            = (LIBLTE_RRC_POWER_RAMPING_STEP_ENUM)liblte_bits_2_value(ie_ptr, 2);
        rach_cnfg->preamble_init_rx_target_pwr = (LIBLTE_RRC_PREAMBLE_INITIAL_RECEIVED_TARGET_POWER_ENUM)liblte_bits_2_value(ie_ptr, 4);

        // RA Supervision Info
        rach_cnfg->preamble_trans_max = (LIBLTE_RRC_PREAMBLE_TRANS_MAX_ENUM)liblte_bits_2_value(ie_ptr, 4);
        rach_cnfg->ra_resp_win_size   = (LIBLTE_RRC_RA_RESPONSE_WINDOW_SIZE_ENUM)liblte_bits_2_value(ie_ptr, 3);
        rach_cnfg->mac_con_res_timer  = (LIBLTE_RRC_MAC_CONTENTION_RESOLUTION_TIMER_ENUM)liblte_bits_2_value(ie_ptr, 3);

        rach_cnfg->max_harq_msg3_tx = liblte_bits_2_value(ie_ptr, 3) + 1;

        liblte_rrc_consume_noncrit_extension(ext, __func__, ie_ptr);
        
        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: RACH Config Dedicated

    Description: Specifies the dedicated random access parameters

    Document Reference: 36.331 v10.0.0 Section 6.3.2
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_rrc_pack_rach_config_dedicated_ie(LIBLTE_RRC_RACH_CONFIG_DEDICATED_STRUCT  *rach_cnfg,
                                                           uint8                                   **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(rach_cnfg != NULL &&
       ie_ptr    != NULL)
    {
        liblte_value_2_bits(rach_cnfg->preamble_index,   ie_ptr, 6);
        liblte_value_2_bits(rach_cnfg->prach_mask_index, ie_ptr, 4);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_unpack_rach_config_dedicated_ie(uint8                                   **ie_ptr,
                                                             LIBLTE_RRC_RACH_CONFIG_DEDICATED_STRUCT  *rach_cnfg)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr    != NULL &&
       rach_cnfg != NULL)
    {
        rach_cnfg->preamble_index   = liblte_bits_2_value(ie_ptr, 6);
        rach_cnfg->prach_mask_index = liblte_bits_2_value(ie_ptr, 4);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: Radio Resource Config Common

    Description: Specifies the common radio resource configurations
                 in the system information and in the mobility control
                 information, including random access parameters
                 and static physical layer parameters

    Document Reference: 36.331 v10.0.0 Section 6.3.2
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_rrc_pack_rr_config_common_sib_ie(LIBLTE_RRC_RR_CONFIG_COMMON_SIB_STRUCT  *rr_cnfg,
                                                          uint8                                  **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(rr_cnfg != NULL &&
       ie_ptr  != NULL)
    {
        // Extension indicator
        liblte_value_2_bits(0, ie_ptr, 1);

        liblte_rrc_pack_rach_config_common_ie(&rr_cnfg->rach_cnfg, ie_ptr);

        // BCCH Config
        liblte_value_2_bits(rr_cnfg->bcch_cnfg.modification_period_coeff, ie_ptr, 2);

        // PCCH Config
        liblte_value_2_bits(rr_cnfg->pcch_cnfg.default_paging_cycle, ie_ptr, 2);
        liblte_value_2_bits(rr_cnfg->pcch_cnfg.nB,                   ie_ptr, 3);

        liblte_rrc_pack_prach_config_sib_ie(&rr_cnfg->prach_cnfg,         ie_ptr);
        liblte_rrc_pack_pdsch_config_common_ie(&rr_cnfg->pdsch_cnfg,      ie_ptr);
        liblte_rrc_pack_pusch_config_common_ie(&rr_cnfg->pusch_cnfg,      ie_ptr);
        liblte_rrc_pack_pucch_config_common_ie(&rr_cnfg->pucch_cnfg,      ie_ptr);
        liblte_rrc_pack_srs_ul_config_common_ie(&rr_cnfg->srs_ul_cnfg,    ie_ptr);
        liblte_rrc_pack_ul_power_control_common_ie(&rr_cnfg->ul_pwr_ctrl, ie_ptr);

        // UL CP Length
        liblte_value_2_bits(rr_cnfg->ul_cp_length, ie_ptr, 1);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_unpack_rr_config_common_sib_ie(uint8                                  **ie_ptr,
                                                            LIBLTE_RRC_RR_CONFIG_COMMON_SIB_STRUCT  *rr_cnfg)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr  != NULL &&
       rr_cnfg != NULL)
    {
        // Extension indicator
        bool ext = liblte_bits_2_value(ie_ptr, 1);

        liblte_rrc_unpack_rach_config_common_ie(ie_ptr, &rr_cnfg->rach_cnfg);

        // BCCH Config
        rr_cnfg->bcch_cnfg.modification_period_coeff = (LIBLTE_RRC_MODIFICATION_PERIOD_COEFF_ENUM)liblte_bits_2_value(ie_ptr, 2);

        // PCCH Config
        rr_cnfg->pcch_cnfg.default_paging_cycle = (LIBLTE_RRC_DEFAULT_PAGING_CYCLE_ENUM)liblte_bits_2_value(ie_ptr, 2);
        rr_cnfg->pcch_cnfg.nB                   = (LIBLTE_RRC_NB_ENUM)liblte_bits_2_value(ie_ptr, 3);

        liblte_rrc_unpack_prach_config_sib_ie(ie_ptr,        &rr_cnfg->prach_cnfg);
        liblte_rrc_unpack_pdsch_config_common_ie(ie_ptr,     &rr_cnfg->pdsch_cnfg);
        liblte_rrc_unpack_pusch_config_common_ie(ie_ptr,     &rr_cnfg->pusch_cnfg);
        liblte_rrc_unpack_pucch_config_common_ie(ie_ptr,     &rr_cnfg->pucch_cnfg);
        liblte_rrc_unpack_srs_ul_config_common_ie(ie_ptr,    &rr_cnfg->srs_ul_cnfg);
        liblte_rrc_unpack_ul_power_control_common_ie(ie_ptr, &rr_cnfg->ul_pwr_ctrl);

        // UL CP Length
        rr_cnfg->ul_cp_length = (LIBLTE_RRC_UL_CP_LENGTH_ENUM)liblte_bits_2_value(ie_ptr, 1);

        liblte_rrc_consume_noncrit_extension(ext, __func__, ie_ptr);
        
        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_pack_rr_config_common_ie(LIBLTE_RRC_RR_CONFIG_COMMON_STRUCT  *rr_cnfg,
                                                      uint8                              **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(rr_cnfg != NULL &&
       ie_ptr  != NULL)
    {
        // Extension indicator
        liblte_value_2_bits(0, ie_ptr, 1);

        // Optional indicators
        liblte_value_2_bits(rr_cnfg->rach_cnfg_present,   ie_ptr, 1);
        liblte_value_2_bits(rr_cnfg->pdsch_cnfg_present,  ie_ptr, 1);
        liblte_value_2_bits(rr_cnfg->phich_cnfg_present,  ie_ptr, 1);
        liblte_value_2_bits(rr_cnfg->pucch_cnfg_present,  ie_ptr, 1);
        liblte_value_2_bits(rr_cnfg->srs_ul_cnfg_present, ie_ptr, 1);
        liblte_value_2_bits(rr_cnfg->ul_pwr_ctrl_present, ie_ptr, 1);
        liblte_value_2_bits(rr_cnfg->ant_info_present,    ie_ptr, 1);
        liblte_value_2_bits(rr_cnfg->p_max_present,       ie_ptr, 1);
        liblte_value_2_bits(rr_cnfg->tdd_cnfg_present,    ie_ptr, 1);

        // RACH Config Common
        if(rr_cnfg->rach_cnfg_present)
        {
            liblte_rrc_pack_rach_config_common_ie(&rr_cnfg->rach_cnfg, ie_ptr);
        }

        // PRACH Config
        liblte_rrc_pack_prach_config_ie(&rr_cnfg->prach_cnfg, ie_ptr);

        // PDSCH Config Common
        if(rr_cnfg->pdsch_cnfg_present)
        {
            liblte_rrc_pack_pdsch_config_common_ie(&rr_cnfg->pdsch_cnfg, ie_ptr);
        }

        // PUSCH Config Common
        liblte_rrc_pack_pusch_config_common_ie(&rr_cnfg->pusch_cnfg, ie_ptr);

        // PHICH Config
        if(rr_cnfg->phich_cnfg_present)
        {
            liblte_rrc_pack_phich_config_ie(&rr_cnfg->phich_cnfg, ie_ptr);
        }

        // PUCCH Config Common
        if(rr_cnfg->pucch_cnfg_present)
        {
            liblte_rrc_pack_pucch_config_common_ie(&rr_cnfg->pucch_cnfg, ie_ptr);
        }

        // Sounding RS UL Config Common
        if(rr_cnfg->srs_ul_cnfg_present)
        {
            liblte_rrc_pack_srs_ul_config_common_ie(&rr_cnfg->srs_ul_cnfg, ie_ptr);
        }

        // Antenna Info Common
        if(rr_cnfg->ant_info_present)
        {
            liblte_rrc_pack_antenna_info_common_ie(rr_cnfg->ant_info, ie_ptr);
        }

        // P Max
        if(rr_cnfg->p_max_present)
        {
            liblte_rrc_pack_p_max_ie(rr_cnfg->p_max, ie_ptr);
        }

        // TDD Config
        if(rr_cnfg->tdd_cnfg_present)
        {
            liblte_rrc_pack_tdd_config_ie(&rr_cnfg->tdd_cnfg, ie_ptr);
        }

        // UL CP Length
        liblte_value_2_bits(rr_cnfg->ul_cp_length, ie_ptr, 1);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_unpack_rr_config_common_ie(uint8                              **ie_ptr,
                                                        LIBLTE_RRC_RR_CONFIG_COMMON_STRUCT  *rr_cnfg)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr  != NULL &&
       rr_cnfg != NULL)
    {
        // Extension indicator
        bool ext = liblte_bits_2_value(ie_ptr, 1);

        // Optional indicators
        rr_cnfg->rach_cnfg_present   = liblte_bits_2_value(ie_ptr, 1);
        rr_cnfg->pdsch_cnfg_present  = liblte_bits_2_value(ie_ptr, 1);
        rr_cnfg->phich_cnfg_present  = liblte_bits_2_value(ie_ptr, 1);
        rr_cnfg->pucch_cnfg_present  = liblte_bits_2_value(ie_ptr, 1);
        rr_cnfg->srs_ul_cnfg_present = liblte_bits_2_value(ie_ptr, 1);
        rr_cnfg->ul_pwr_ctrl_present = liblte_bits_2_value(ie_ptr, 1);
        rr_cnfg->ant_info_present    = liblte_bits_2_value(ie_ptr, 1);
        rr_cnfg->p_max_present       = liblte_bits_2_value(ie_ptr, 1);
        rr_cnfg->tdd_cnfg_present    = liblte_bits_2_value(ie_ptr, 1);

        // RACH Config Common
        if(rr_cnfg->rach_cnfg_present)
        {
            liblte_rrc_unpack_rach_config_common_ie(ie_ptr, &rr_cnfg->rach_cnfg);
        }

        // PRACH Config
        liblte_rrc_unpack_prach_config_ie(ie_ptr, &rr_cnfg->prach_cnfg);

        // PDSCH Config Common
        if(rr_cnfg->pdsch_cnfg_present)
        {
            liblte_rrc_unpack_pdsch_config_common_ie(ie_ptr, &rr_cnfg->pdsch_cnfg);
        }

        // PUSCH Config Common
        liblte_rrc_unpack_pusch_config_common_ie(ie_ptr, &rr_cnfg->pusch_cnfg);

        // PHICH Config
        if(rr_cnfg->phich_cnfg_present)
        {
            liblte_rrc_unpack_phich_config_ie(ie_ptr, &rr_cnfg->phich_cnfg);
        }

        // PUCCH Config Common
        if(rr_cnfg->pucch_cnfg_present)
        {
            liblte_rrc_unpack_pucch_config_common_ie(ie_ptr, &rr_cnfg->pucch_cnfg);
        }

        // Sounding RS UL Config Common
        if(rr_cnfg->srs_ul_cnfg_present)
        {
            liblte_rrc_unpack_srs_ul_config_common_ie(ie_ptr, &rr_cnfg->srs_ul_cnfg);
        }

        // Antenna Info Common
        if(rr_cnfg->ant_info_present)
        {
            liblte_rrc_unpack_antenna_info_common_ie(ie_ptr, &rr_cnfg->ant_info);
        }

        // P Max
        if(rr_cnfg->p_max_present)
        {
            liblte_rrc_unpack_p_max_ie(ie_ptr, &rr_cnfg->p_max);
        }

        // TDD Config
        if(rr_cnfg->tdd_cnfg_present)
        {
            liblte_rrc_unpack_tdd_config_ie(ie_ptr, &rr_cnfg->tdd_cnfg);
        }

        // UL CP Length
        rr_cnfg->ul_cp_length = (LIBLTE_RRC_UL_CP_LENGTH_ENUM)liblte_bits_2_value(ie_ptr, 1);

        liblte_rrc_consume_noncrit_extension(ext, __func__, ie_ptr);
        
        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: Radio Resource Config Dedicated

    Description: Sets up/Modifies/Releases RBs, modifies the MAC
                 main configuration, modifies the SPS configuration
                 and modifies dedicated physical configuration

    Document Reference: 36.331 v10.0.0 Section 6.3.2
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_rrc_pack_rr_config_dedicated_ie(LIBLTE_RRC_RR_CONFIG_DEDICATED_STRUCT  *rr_cnfg,
                                                         uint8                                 **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;
    uint32            i;

    if(rr_cnfg != NULL &&
       ie_ptr  != NULL)
    {
        // Extension indicator
        liblte_value_2_bits(rr_cnfg->rlf_timers_and_constants_present, ie_ptr, 1);

        // Optional indicators
        if(rr_cnfg->srb_to_add_mod_list_size != 0)
        {
            liblte_value_2_bits(1, ie_ptr, 1);
        }else{
            liblte_value_2_bits(0, ie_ptr, 1);
        }
        if(rr_cnfg->drb_to_add_mod_list_size != 0)
        {
            liblte_value_2_bits(1, ie_ptr, 1);
        }else{
            liblte_value_2_bits(0, ie_ptr, 1);
        }
        if(rr_cnfg->drb_to_release_list_size != 0)
        {
            liblte_value_2_bits(1, ie_ptr, 1);
        }else{
            liblte_value_2_bits(0, ie_ptr, 1);
        }
        liblte_value_2_bits(rr_cnfg->mac_main_cnfg_present, ie_ptr, 1);
        liblte_value_2_bits(rr_cnfg->sps_cnfg_present,      ie_ptr, 1);
        liblte_value_2_bits(rr_cnfg->phy_cnfg_ded_present,  ie_ptr, 1);

        // SRB To Add Mod List
        if(rr_cnfg->srb_to_add_mod_list_size != 0)
        {
            liblte_value_2_bits(rr_cnfg->srb_to_add_mod_list_size - 1, ie_ptr, 1);
        }
        for(i=0; i<rr_cnfg->srb_to_add_mod_list_size; i++)
        {
            // Extension indicator
            liblte_value_2_bits(0, ie_ptr, 1);

            // Optional indicators
            liblte_value_2_bits(rr_cnfg->srb_to_add_mod_list[i].rlc_cnfg_present, ie_ptr, 1);
            liblte_value_2_bits(rr_cnfg->srb_to_add_mod_list[i].lc_cnfg_present,  ie_ptr, 1);

            // SRB Identity
            liblte_value_2_bits(rr_cnfg->srb_to_add_mod_list[i].srb_id - 1, ie_ptr, 1);

            // RLC Config
            if(rr_cnfg->srb_to_add_mod_list[i].rlc_cnfg_present)
            {
                // Choice
                liblte_value_2_bits(rr_cnfg->srb_to_add_mod_list[i].rlc_default_cnfg_present, ie_ptr, 1);

                // Explicit Config
                if(!rr_cnfg->srb_to_add_mod_list[i].rlc_default_cnfg_present)
                {
                    liblte_rrc_pack_rlc_config_ie(&rr_cnfg->srb_to_add_mod_list[i].rlc_explicit_cnfg, ie_ptr);
                }
            }

            // Logical Channel Config
            if(rr_cnfg->srb_to_add_mod_list[i].lc_cnfg_present)
            {
                // Choice
                liblte_value_2_bits(rr_cnfg->srb_to_add_mod_list[i].lc_default_cnfg_present, ie_ptr, 1);

                // Explicit Config
                if(!rr_cnfg->srb_to_add_mod_list[i].lc_default_cnfg_present)
                {
                    liblte_rrc_pack_logical_channel_config_ie(&rr_cnfg->srb_to_add_mod_list[i].lc_explicit_cnfg, ie_ptr);
                }
            }
        }

        // DRB To Add Mod List
        if(rr_cnfg->drb_to_add_mod_list_size != 0)
        {
            liblte_value_2_bits(rr_cnfg->drb_to_add_mod_list_size - 1, ie_ptr, 4);
        }
        for(i=0; i<rr_cnfg->drb_to_add_mod_list_size; i++)
        {
            // Extension indicator
            liblte_value_2_bits(0, ie_ptr, 1);

            // Optional indicators
            liblte_value_2_bits(rr_cnfg->drb_to_add_mod_list[i].eps_bearer_id_present, ie_ptr, 1);
            liblte_value_2_bits(rr_cnfg->drb_to_add_mod_list[i].pdcp_cnfg_present,     ie_ptr, 1);
            liblte_value_2_bits(rr_cnfg->drb_to_add_mod_list[i].rlc_cnfg_present,      ie_ptr, 1);
            liblte_value_2_bits(rr_cnfg->drb_to_add_mod_list[i].lc_id_present,         ie_ptr, 1);
            liblte_value_2_bits(rr_cnfg->drb_to_add_mod_list[i].lc_cnfg_present,       ie_ptr, 1);

            // EPS Bearer Identity
            if(rr_cnfg->drb_to_add_mod_list[i].eps_bearer_id_present)
            {
                liblte_value_2_bits(rr_cnfg->drb_to_add_mod_list[i].eps_bearer_id, ie_ptr, 4);
            }

            // DRB Identity
            liblte_rrc_pack_drb_identity_ie(rr_cnfg->drb_to_add_mod_list[i].drb_id, ie_ptr);

            // PDCP Config
            if(rr_cnfg->drb_to_add_mod_list[i].pdcp_cnfg_present)
            {
                liblte_rrc_pack_pdcp_config_ie(&rr_cnfg->drb_to_add_mod_list[i].pdcp_cnfg, ie_ptr);
            }

            // RLC Config
            if(rr_cnfg->drb_to_add_mod_list[i].rlc_cnfg_present)
            {
                liblte_rrc_pack_rlc_config_ie(&rr_cnfg->drb_to_add_mod_list[i].rlc_cnfg, ie_ptr);
            }

            // Logical Channel Identity
            if(rr_cnfg->drb_to_add_mod_list[i].lc_id_present)
            {
                liblte_value_2_bits(rr_cnfg->drb_to_add_mod_list[i].lc_id - 3, ie_ptr, 3);
            }

            // Logical Channel Configuration
            if(rr_cnfg->drb_to_add_mod_list[i].lc_cnfg_present)
            {
                liblte_rrc_pack_logical_channel_config_ie(&rr_cnfg->drb_to_add_mod_list[i].lc_cnfg, ie_ptr);
            }
        }

        // DRB To Release List
        if(rr_cnfg->drb_to_release_list_size != 0)
        {
            liblte_value_2_bits(rr_cnfg->drb_to_release_list_size - 1, ie_ptr, 4);
        }
        for(i=0; i<rr_cnfg->drb_to_release_list_size; i++)
        {
            liblte_rrc_pack_drb_identity_ie(rr_cnfg->drb_to_release_list[i], ie_ptr);
        }

        // MAC Main Config
        if(rr_cnfg->mac_main_cnfg_present)
        {
            liblte_value_2_bits(rr_cnfg->mac_main_cnfg.default_value, ie_ptr, 1);
            if(!rr_cnfg->mac_main_cnfg.default_value)
            {
                liblte_rrc_pack_mac_main_config_ie(&rr_cnfg->mac_main_cnfg.explicit_value, ie_ptr);
            }
        }

        // SPS Config
        if(rr_cnfg->sps_cnfg_present)
        {
            liblte_rrc_pack_sps_config_ie(&rr_cnfg->sps_cnfg, ie_ptr);
        }

        // Physical Config Dedicated
        if(rr_cnfg->phy_cnfg_ded_present)
        {
            liblte_rrc_pack_physical_config_dedicated_ie(&rr_cnfg->phy_cnfg_ded, ie_ptr);
        }

        // Extension
        // Optional indicators
        liblte_value_2_bits(rr_cnfg->rlf_timers_and_constants_present, ie_ptr, 1);

        // RLF Timers and Constants
        if(rr_cnfg->rlf_timers_and_constants_present)
        {
            liblte_rrc_pack_rlf_timers_and_constants_ie(&rr_cnfg->rlf_timers_and_constants, ie_ptr);
        }

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_unpack_rr_config_dedicated_ie(uint8                                 **ie_ptr,
                                                           LIBLTE_RRC_RR_CONFIG_DEDICATED_STRUCT  *rr_cnfg)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;
    uint32            i;
    bool              ext;
    bool              srb_ext;
    bool              drb_ext;
    bool              srb_to_add_mod_list_present;
    bool              drb_to_add_mod_list_present;
    bool              drb_to_release_list_present;

    if(ie_ptr  != NULL &&
       rr_cnfg != NULL)
    {
        // Extension indicator
        ext = liblte_bits_2_value(ie_ptr, 1);

        // Optional indicators
        srb_to_add_mod_list_present    = liblte_bits_2_value(ie_ptr, 1);
        drb_to_add_mod_list_present    = liblte_bits_2_value(ie_ptr, 1);
        drb_to_release_list_present    = liblte_bits_2_value(ie_ptr, 1);
        rr_cnfg->mac_main_cnfg_present = liblte_bits_2_value(ie_ptr, 1);
        rr_cnfg->sps_cnfg_present      = liblte_bits_2_value(ie_ptr, 1);
        rr_cnfg->phy_cnfg_ded_present  = liblte_bits_2_value(ie_ptr, 1);

        // SRB To Add Mod List
        if(srb_to_add_mod_list_present)
        {
            rr_cnfg->srb_to_add_mod_list_size = liblte_bits_2_value(ie_ptr, 1) + 1;
            for(i=0; i<rr_cnfg->srb_to_add_mod_list_size; i++)
            {
                // Extension indicator
                srb_ext = liblte_bits_2_value(ie_ptr, 1);

                // Optional indicators
                rr_cnfg->srb_to_add_mod_list[i].rlc_cnfg_present = liblte_bits_2_value(ie_ptr, 1);
                rr_cnfg->srb_to_add_mod_list[i].lc_cnfg_present  = liblte_bits_2_value(ie_ptr, 1);

                // SRB Identity
                rr_cnfg->srb_to_add_mod_list[i].srb_id = liblte_bits_2_value(ie_ptr, 1) + 1;

                // RLC Config
                if(rr_cnfg->srb_to_add_mod_list[i].rlc_cnfg_present)
                {
                    // Choice
                    rr_cnfg->srb_to_add_mod_list[i].rlc_default_cnfg_present = liblte_bits_2_value(ie_ptr, 1);

                    // Explicit Config
                    if(!rr_cnfg->srb_to_add_mod_list[i].rlc_default_cnfg_present)
                    {
                        liblte_rrc_unpack_rlc_config_ie(ie_ptr, &rr_cnfg->srb_to_add_mod_list[i].rlc_explicit_cnfg);
                    }
                }

                // Logical Channel Config
                if(rr_cnfg->srb_to_add_mod_list[i].lc_cnfg_present)
                {
                    // Choice
                    rr_cnfg->srb_to_add_mod_list[i].lc_default_cnfg_present = liblte_bits_2_value(ie_ptr, 1);

                    // Explicit Config
                    if(!rr_cnfg->srb_to_add_mod_list[i].lc_default_cnfg_present)
                    {
                        liblte_rrc_unpack_logical_channel_config_ie(ie_ptr, &rr_cnfg->srb_to_add_mod_list[i].lc_explicit_cnfg);
                    }
                }

                // Consume non-critical extensions
                liblte_rrc_consume_noncrit_extension(srb_ext, __func__, ie_ptr);
            }
        }

        // DRB To Add Mod List
        if(drb_to_add_mod_list_present)
        {
            rr_cnfg->drb_to_add_mod_list_size = liblte_bits_2_value(ie_ptr, 4) + 1;
            for(i=0; i<rr_cnfg->drb_to_add_mod_list_size; i++)
            {
                // Extension indicator
                drb_ext = liblte_bits_2_value(ie_ptr, 1);

                // Optional indicators
                rr_cnfg->drb_to_add_mod_list[i].eps_bearer_id_present = liblte_bits_2_value(ie_ptr, 1);
                rr_cnfg->drb_to_add_mod_list[i].pdcp_cnfg_present     = liblte_bits_2_value(ie_ptr, 1);
                rr_cnfg->drb_to_add_mod_list[i].rlc_cnfg_present      = liblte_bits_2_value(ie_ptr, 1);
                rr_cnfg->drb_to_add_mod_list[i].lc_id_present         = liblte_bits_2_value(ie_ptr, 1);
                rr_cnfg->drb_to_add_mod_list[i].lc_cnfg_present       = liblte_bits_2_value(ie_ptr, 1);

                // EPS Bearer Identity
                if(rr_cnfg->drb_to_add_mod_list[i].eps_bearer_id_present)
                {
                    rr_cnfg->drb_to_add_mod_list[i].eps_bearer_id = liblte_bits_2_value(ie_ptr, 4);
                }

                // DRB Identity
                liblte_rrc_unpack_drb_identity_ie(ie_ptr, &rr_cnfg->drb_to_add_mod_list[i].drb_id);

                // PDCP Config
                if(rr_cnfg->drb_to_add_mod_list[i].pdcp_cnfg_present)
                {
                    liblte_rrc_unpack_pdcp_config_ie(ie_ptr, &rr_cnfg->drb_to_add_mod_list[i].pdcp_cnfg);
                }

                // RLC Config
                if(rr_cnfg->drb_to_add_mod_list[i].rlc_cnfg_present)
                {
                    liblte_rrc_unpack_rlc_config_ie(ie_ptr, &rr_cnfg->drb_to_add_mod_list[i].rlc_cnfg);
                }

                // Logical Channel Identity
                if(rr_cnfg->drb_to_add_mod_list[i].lc_id_present)
                {
                    rr_cnfg->drb_to_add_mod_list[i].lc_id = liblte_bits_2_value(ie_ptr, 3) + 3;
                }

                // Logical Channel Configuration
                if(rr_cnfg->drb_to_add_mod_list[i].lc_cnfg_present)
                {
                    liblte_rrc_unpack_logical_channel_config_ie(ie_ptr, &rr_cnfg->drb_to_add_mod_list[i].lc_cnfg);
                }

                // Consume non-critical extensions
                liblte_rrc_consume_noncrit_extension(drb_ext, __func__, ie_ptr);
            }
        }

        // DRB To Release List
        if(drb_to_release_list_present)
        {
            rr_cnfg->drb_to_release_list_size = liblte_bits_2_value(ie_ptr, 4) + 1;
            for(i=0; i<rr_cnfg->drb_to_release_list_size; i++)
            {
                liblte_rrc_unpack_drb_identity_ie(ie_ptr, &rr_cnfg->drb_to_release_list[i]);
            }
        }

        // MAC Main Config
        if(rr_cnfg->mac_main_cnfg_present)
        {
            rr_cnfg->mac_main_cnfg.default_value = liblte_bits_2_value(ie_ptr, 1);
            if(!rr_cnfg->mac_main_cnfg.default_value)
            {
                liblte_rrc_unpack_mac_main_config_ie(ie_ptr, &rr_cnfg->mac_main_cnfg.explicit_value);
            }
        }

        // SPS Config
        if(rr_cnfg->sps_cnfg_present)
        {
            liblte_rrc_unpack_sps_config_ie(ie_ptr, &rr_cnfg->sps_cnfg);
        }

        // Physical Config Dedicated
        if(rr_cnfg->phy_cnfg_ded_present)
        {
            liblte_rrc_unpack_physical_config_dedicated_ie(ie_ptr, &rr_cnfg->phy_cnfg_ded);
        }

        // Extension (FIXME: only handling r9 extensions)
#if 0
        if(ext)
        {
            // Optional indicators
            rr_cnfg->rlf_timers_and_constants_present = liblte_bits_2_value(ie_ptr, 1);

            // RLF Timers and Constants
            if(rr_cnfg->rlf_timers_and_constants_present)
            {
                liblte_rrc_unpack_rlf_timers_and_constants_ie(ie_ptr, &rr_cnfg->rlf_timers_and_constants);
            }
        }
#endif 
        // Modified by ismael, just skip extensions 
        liblte_rrc_consume_noncrit_extension(ext, __func__, ie_ptr);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: RLC Config

    Description: Specifies the RLC configuration of SRBs and DRBs

    Document Reference: 36.331 v10.0.0 Section 6.3.2
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_rrc_pack_rlc_config_ie(LIBLTE_RRC_RLC_CONFIG_STRUCT  *rlc_cnfg,
                                                uint8                        **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(rlc_cnfg != NULL &&
       ie_ptr   != NULL)
    {
        // Extension indicator
        liblte_value_2_bits(0, ie_ptr, 1);

        // Mode Choice
        liblte_value_2_bits(rlc_cnfg->rlc_mode, ie_ptr, 2);

        if(LIBLTE_RRC_RLC_MODE_AM == rlc_cnfg->rlc_mode)
        {
            // UL AM RLC
            {
                // T Poll Retransmit
                liblte_value_2_bits(rlc_cnfg->ul_am_rlc.t_poll_retx, ie_ptr, 6);

                // Poll PDU
                liblte_value_2_bits(rlc_cnfg->ul_am_rlc.poll_pdu, ie_ptr, 3);

                // Poll Byte
                liblte_value_2_bits(rlc_cnfg->ul_am_rlc.poll_byte, ie_ptr, 4);

                // Max Retransmission Threshold
                liblte_value_2_bits(rlc_cnfg->ul_am_rlc.max_retx_thresh, ie_ptr, 3);
            }

            // DL AM RLC
            {
                // T Reordering
                liblte_value_2_bits(rlc_cnfg->dl_am_rlc.t_reordering, ie_ptr, 5);

                // T Status Prohibit
                liblte_value_2_bits(rlc_cnfg->dl_am_rlc.t_status_prohibit, ie_ptr, 6);
            }
        }else if(LIBLTE_RRC_RLC_MODE_UM_BI == rlc_cnfg->rlc_mode){
            // UL UM RLC
            {
                // SN Field Length
                liblte_value_2_bits(rlc_cnfg->ul_um_bi_rlc.sn_field_len, ie_ptr, 1);
            }

            // DL UM RLC
            {
                // SN Field Length
                liblte_value_2_bits(rlc_cnfg->dl_um_bi_rlc.sn_field_len, ie_ptr, 1);

                // T Reordering
                liblte_value_2_bits(rlc_cnfg->dl_um_bi_rlc.t_reordering, ie_ptr, 5);
            }
        }else if(LIBLTE_RRC_RLC_MODE_UM_UNI_UL == rlc_cnfg->rlc_mode){
            // SN Field Length
            liblte_value_2_bits(rlc_cnfg->ul_um_uni_rlc.sn_field_len, ie_ptr, 1);
        }else{ // LIBLTE_RRC_RLC_MODE_UM_UNI_DL == rlc_cnfg->rlc_mode
            // SN Field Length
            liblte_value_2_bits(rlc_cnfg->dl_um_uni_rlc.sn_field_len, ie_ptr, 1);

            // T Reordering
            liblte_value_2_bits(rlc_cnfg->dl_um_uni_rlc.t_reordering, ie_ptr, 5);
        }

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_unpack_rlc_config_ie(uint8                        **ie_ptr,
                                                  LIBLTE_RRC_RLC_CONFIG_STRUCT  *rlc_cnfg)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr   != NULL &&
       rlc_cnfg != NULL)
    {
        // Extension indicator
        bool ext = liblte_bits_2_value(ie_ptr, 1);

        // Mode Choice
        rlc_cnfg->rlc_mode = (LIBLTE_RRC_RLC_MODE_ENUM)liblte_bits_2_value(ie_ptr, 2);

        if(LIBLTE_RRC_RLC_MODE_AM == rlc_cnfg->rlc_mode)
        {
            // UL AM RLC
            {
                // T Poll Retransmit
                rlc_cnfg->ul_am_rlc.t_poll_retx = (LIBLTE_RRC_T_POLL_RETRANSMIT_ENUM)liblte_bits_2_value(ie_ptr, 6);

                // Poll PDU
                rlc_cnfg->ul_am_rlc.poll_pdu = (LIBLTE_RRC_POLL_PDU_ENUM)liblte_bits_2_value(ie_ptr, 3);

                // Poll Byte
                rlc_cnfg->ul_am_rlc.poll_byte = (LIBLTE_RRC_POLL_BYTE_ENUM)liblte_bits_2_value(ie_ptr, 4);

                // Max Retransmission Threshold
                rlc_cnfg->ul_am_rlc.max_retx_thresh = (LIBLTE_RRC_MAX_RETX_THRESHOLD_ENUM)liblte_bits_2_value(ie_ptr, 3);
            }

            // DL AM RLC
            {
                // T Reordering
                rlc_cnfg->dl_am_rlc.t_reordering = (LIBLTE_RRC_T_REORDERING_ENUM)liblte_bits_2_value(ie_ptr, 5);

                // T Status Prohibit
                rlc_cnfg->dl_am_rlc.t_status_prohibit = (LIBLTE_RRC_T_STATUS_PROHIBIT_ENUM)liblte_bits_2_value(ie_ptr, 6);
            }
        }else if(LIBLTE_RRC_RLC_MODE_UM_BI == rlc_cnfg->rlc_mode){
            // UL UM RLC
            {
                // SN Field Length
                rlc_cnfg->ul_um_bi_rlc.sn_field_len = (LIBLTE_RRC_SN_FIELD_LENGTH_ENUM)liblte_bits_2_value(ie_ptr, 1);
            }

            // DL UM RLC
            {
                // SN Field Length
                rlc_cnfg->dl_um_bi_rlc.sn_field_len = (LIBLTE_RRC_SN_FIELD_LENGTH_ENUM)liblte_bits_2_value(ie_ptr, 1);

                // T Reordering
                rlc_cnfg->dl_um_bi_rlc.t_reordering = (LIBLTE_RRC_T_REORDERING_ENUM)liblte_bits_2_value(ie_ptr, 5);
            }
        }else if(LIBLTE_RRC_RLC_MODE_UM_UNI_UL == rlc_cnfg->rlc_mode){
            // SN Field Length
            rlc_cnfg->ul_um_uni_rlc.sn_field_len = (LIBLTE_RRC_SN_FIELD_LENGTH_ENUM)liblte_bits_2_value(ie_ptr, 1);
        }else{ // LIBLTE_RRC_RLC_MODE_UM_UNI_DL == rlc_cnfg->rlc_mode
            // SN Field Length
            rlc_cnfg->dl_um_uni_rlc.sn_field_len = (LIBLTE_RRC_SN_FIELD_LENGTH_ENUM)liblte_bits_2_value(ie_ptr, 1);

            // T Reordering
            rlc_cnfg->dl_um_uni_rlc.t_reordering = (LIBLTE_RRC_T_REORDERING_ENUM)liblte_bits_2_value(ie_ptr, 5);
        }

        liblte_rrc_consume_noncrit_extension(ext, __func__, ie_ptr);
        
        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: RLF Timers and Constants

    Description: Contains UE specific timers and constants applicable
                 for UEs in RRC_CONNECTED

    Document Reference: 36.331 v10.0.0 Section 6.3.2
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_rrc_pack_rlf_timers_and_constants_ie(LIBLTE_RRC_RLF_TIMERS_AND_CONSTANTS_STRUCT  *rlf_timers_and_constants,
                                                              uint8                                      **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(rlf_timers_and_constants != NULL &&
       ie_ptr                   != NULL)
    {
        // Release choice
        liblte_value_2_bits(1, ie_ptr, 1);

        // Extension
        liblte_value_2_bits(0, ie_ptr, 1);

        liblte_value_2_bits(rlf_timers_and_constants->t301, ie_ptr, 3);
        liblte_value_2_bits(rlf_timers_and_constants->t310, ie_ptr, 3);
        liblte_value_2_bits(rlf_timers_and_constants->n310, ie_ptr, 3);
        liblte_value_2_bits(rlf_timers_and_constants->t311, ie_ptr, 3);
        liblte_value_2_bits(rlf_timers_and_constants->n311, ie_ptr, 3);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_unpack_rlf_timers_and_constants_ie(uint8                                      **ie_ptr,
                                                                LIBLTE_RRC_RLF_TIMERS_AND_CONSTANTS_STRUCT  *rlf_timers_and_constants)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr                   != NULL &&
       rlf_timers_and_constants != NULL)
    {
        // Release choice
        liblte_bits_2_value(ie_ptr, 1);

        // Extension
        liblte_bits_2_value(ie_ptr, 1);

        rlf_timers_and_constants->t301 = (LIBLTE_RRC_T301_ENUM)liblte_bits_2_value(ie_ptr, 3);
        rlf_timers_and_constants->t310 = (LIBLTE_RRC_T310_ENUM)liblte_bits_2_value(ie_ptr, 3);
        rlf_timers_and_constants->n310 = (LIBLTE_RRC_N310_ENUM)liblte_bits_2_value(ie_ptr, 3);
        rlf_timers_and_constants->t311 = (LIBLTE_RRC_T311_ENUM)liblte_bits_2_value(ie_ptr, 3);
        rlf_timers_and_constants->n311 = (LIBLTE_RRC_N311_ENUM)liblte_bits_2_value(ie_ptr, 3);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: RN Subframe Config

    Description: Specifies the subframe configuration for an RN

    Document Reference: 36.331 v10.0.0 Section 6.3.2
*********************************************************************/
// FIXME

/*********************************************************************
    IE Name: Scheduling Request Config

    Description: Specifies the scheduling request related parameters

    Document Reference: 36.331 v10.0.0 Section 6.3.2
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_rrc_pack_scheduling_request_config_ie(LIBLTE_RRC_SCHEDULING_REQUEST_CONFIG_STRUCT  *sched_request_cnfg,
                                                               uint8                                       **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(sched_request_cnfg != NULL &&
       ie_ptr             != NULL)
    {
        // Setup
        liblte_value_2_bits(sched_request_cnfg->setup_present, ie_ptr, 1);
        if(sched_request_cnfg->setup_present)
        {
            // SR PUCCH Resource Index
            liblte_value_2_bits(sched_request_cnfg->sr_pucch_resource_idx, ie_ptr, 11);

            // SR Config Index
            liblte_value_2_bits(sched_request_cnfg->sr_cnfg_idx, ie_ptr, 8);

            // DRS Trans Max
            liblte_value_2_bits(sched_request_cnfg->dsr_trans_max, ie_ptr, 3);
        }

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_unpack_scheduling_request_config_ie(uint8                                       **ie_ptr,
                                                                 LIBLTE_RRC_SCHEDULING_REQUEST_CONFIG_STRUCT  *sched_request_cnfg)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr             != NULL &&
       sched_request_cnfg != NULL)
    {
        // Setup
        sched_request_cnfg->setup_present = liblte_bits_2_value(ie_ptr, 1);
        if(sched_request_cnfg->setup_present)
        {
            // SR PUCCH Resource Index
            sched_request_cnfg->sr_pucch_resource_idx = liblte_bits_2_value(ie_ptr, 11);

            // SR Config Index
            sched_request_cnfg->sr_cnfg_idx = liblte_bits_2_value(ie_ptr, 8);

            // DRS Trans Max
            sched_request_cnfg->dsr_trans_max = (LIBLTE_RRC_DSR_TRANS_MAX_ENUM)liblte_bits_2_value(ie_ptr, 3);
        }

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: Sounding RS UL Config

    Description: Specifies the uplink Sounding RS configuration for
                 periodic and aperiodic sounding

    Document Reference: 36.331 v10.0.0 Section 6.3.2
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_rrc_pack_srs_ul_config_common_ie(LIBLTE_RRC_SRS_UL_CONFIG_COMMON_STRUCT  *srs_ul_cnfg,
                                                          uint8                                  **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(srs_ul_cnfg != NULL &&
       ie_ptr      != NULL)
    {
        liblte_value_2_bits(srs_ul_cnfg->present, ie_ptr, 1);

        if(true == srs_ul_cnfg->present)
        {
            liblte_value_2_bits(srs_ul_cnfg->max_up_pts_present, ie_ptr, 1);

            liblte_value_2_bits(srs_ul_cnfg->bw_cnfg,           ie_ptr, 3);
            liblte_value_2_bits(srs_ul_cnfg->subfr_cnfg,        ie_ptr, 4);
            liblte_value_2_bits(srs_ul_cnfg->ack_nack_simul_tx, ie_ptr, 1);

            if(true == srs_ul_cnfg->max_up_pts_present)
            {
                liblte_value_2_bits(srs_ul_cnfg->max_up_pts, ie_ptr, 1);
            }
        }

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_unpack_srs_ul_config_common_ie(uint8                                  **ie_ptr,
                                                            LIBLTE_RRC_SRS_UL_CONFIG_COMMON_STRUCT  *srs_ul_cnfg)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr      != NULL &&
       srs_ul_cnfg != NULL)
    {
        srs_ul_cnfg->present = liblte_bits_2_value(ie_ptr, 1);

        if(true == srs_ul_cnfg->present)
        {
            srs_ul_cnfg->max_up_pts_present = liblte_bits_2_value(ie_ptr, 1);

            srs_ul_cnfg->bw_cnfg           = (LIBLTE_RRC_SRS_BW_CONFIG_ENUM)liblte_bits_2_value(ie_ptr, 3);
            srs_ul_cnfg->subfr_cnfg        = (LIBLTE_RRC_SRS_SUBFR_CONFIG_ENUM)liblte_bits_2_value(ie_ptr, 4);
            srs_ul_cnfg->ack_nack_simul_tx = liblte_bits_2_value(ie_ptr, 1);

            if(true == srs_ul_cnfg->max_up_pts_present)
            {
                srs_ul_cnfg->max_up_pts = liblte_bits_2_value(ie_ptr, 1);
            }
        }

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_pack_srs_ul_config_dedicated_ie(LIBLTE_RRC_SRS_UL_CONFIG_DEDICATED_STRUCT  *srs_ul_cnfg,
                                                             uint8                                     **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(srs_ul_cnfg != NULL &&
       ie_ptr      != NULL)
    {
        // Setup
        liblte_value_2_bits(srs_ul_cnfg->setup_present, ie_ptr, 1);
        if(srs_ul_cnfg->setup_present)
        {
            // SRS Bandwidth
            liblte_value_2_bits(srs_ul_cnfg->srs_bandwidth, ie_ptr, 2);

            // SRS Hopping Bandwidth
            liblte_value_2_bits(srs_ul_cnfg->srs_hopping_bandwidth, ie_ptr, 2);

            // Frequency Domain Position
            liblte_value_2_bits(srs_ul_cnfg->freq_domain_pos, ie_ptr, 5);

            // Duration
            liblte_value_2_bits(srs_ul_cnfg->duration, ie_ptr, 1);

            // SRS Config Index
            liblte_value_2_bits(srs_ul_cnfg->srs_cnfg_idx, ie_ptr, 10);

            // Transmission Comb
            liblte_value_2_bits(srs_ul_cnfg->tx_comb, ie_ptr, 1);

            // Cyclic Shift
            liblte_value_2_bits(srs_ul_cnfg->cyclic_shift, ie_ptr, 3);
        }

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_unpack_srs_ul_config_dedicated_ie(uint8                                     **ie_ptr,
                                                               LIBLTE_RRC_SRS_UL_CONFIG_DEDICATED_STRUCT  *srs_ul_cnfg)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr      != NULL &&
       srs_ul_cnfg != NULL)
    {
        // Setup
        srs_ul_cnfg->setup_present = liblte_bits_2_value(ie_ptr, 1);
        if(srs_ul_cnfg->setup_present)
        {
            // SRS Bandwidth
            srs_ul_cnfg->srs_bandwidth = (LIBLTE_RRC_SRS_BANDWIDTH_ENUM)liblte_bits_2_value(ie_ptr, 2);

            // SRS Hopping Bandwidth
            srs_ul_cnfg->srs_hopping_bandwidth = (LIBLTE_RRC_SRS_HOPPING_BANDWIDTH_ENUM)liblte_bits_2_value(ie_ptr, 2);

            // Frequency Domain Position
            srs_ul_cnfg->freq_domain_pos = liblte_bits_2_value(ie_ptr, 5);

            // Duration
            srs_ul_cnfg->duration = liblte_bits_2_value(ie_ptr, 1);

            // SRS Config Index
            srs_ul_cnfg->srs_cnfg_idx = liblte_bits_2_value(ie_ptr, 10);

            // Transmission Comb
            srs_ul_cnfg->tx_comb = liblte_bits_2_value(ie_ptr, 1);

            // Cyclic Shift
            srs_ul_cnfg->cyclic_shift = (LIBLTE_RRC_CYCLIC_SHIFT_ENUM)liblte_bits_2_value(ie_ptr, 3);
        }

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: SPS Config

    Description: Specifies the semi-persistent scheduling
                 configuration

    Document Reference: 36.331 v10.0.0 Section 6.3.2
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_rrc_pack_sps_config_ie(LIBLTE_RRC_SPS_CONFIG_STRUCT  *sps_cnfg,
                                                uint8                        **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;
    uint32            i;

    if(sps_cnfg != NULL &&
       ie_ptr   != NULL)
    {
        // Optional indicators
        liblte_value_2_bits(sps_cnfg->sps_c_rnti_present,  ie_ptr, 1);
        liblte_value_2_bits(sps_cnfg->sps_cnfg_dl_present, ie_ptr, 1);
        liblte_value_2_bits(sps_cnfg->sps_cnfg_ul_present, ie_ptr, 1);

        // SPS C-RNTI
        if(sps_cnfg->sps_c_rnti_present)
        {
            liblte_rrc_pack_c_rnti_ie(sps_cnfg->sps_c_rnti, ie_ptr);
        }

        // SPS Config DL
        if(sps_cnfg->sps_cnfg_dl_present)
        {
            liblte_value_2_bits(sps_cnfg->sps_cnfg_dl.setup_present, ie_ptr, 1);
            if(sps_cnfg->sps_cnfg_dl.setup_present)
            {
                // Extension indicator
                liblte_value_2_bits(0, ie_ptr, 1);

                // SPS Interval DL
                liblte_value_2_bits(sps_cnfg->sps_cnfg_dl.sps_interval_dl, ie_ptr, 4);

                // Number of Configured SPS Processes
                liblte_value_2_bits(sps_cnfg->sps_cnfg_dl.N_sps_processes - 1, ie_ptr, 3);

                // N1 PUCCH AN Persistent List
                liblte_value_2_bits(sps_cnfg->sps_cnfg_dl.n1_pucch_an_persistent_list_size - 1, ie_ptr, 2);
                for(i=0; i<sps_cnfg->sps_cnfg_dl.n1_pucch_an_persistent_list_size; i++)
                {
                    liblte_value_2_bits(sps_cnfg->sps_cnfg_dl.n1_pucch_an_persistent_list[i], ie_ptr, 11);
                }
            }
        }

        // SPS Config UL
        if(sps_cnfg->sps_cnfg_ul_present)
        {
            liblte_value_2_bits(sps_cnfg->sps_cnfg_ul.setup_present, ie_ptr, 1);
            if(sps_cnfg->sps_cnfg_ul.setup_present)
            {
                // Extension indicator
                liblte_value_2_bits(0, ie_ptr, 1);

                // Optional indicators
                liblte_value_2_bits(sps_cnfg->sps_cnfg_ul.p0_persistent_present,      ie_ptr, 1);
                liblte_value_2_bits(sps_cnfg->sps_cnfg_ul.two_intervals_cnfg_present, ie_ptr, 1);

                // SPS Interval UL
                liblte_value_2_bits(sps_cnfg->sps_cnfg_ul.sps_interval_ul, ie_ptr, 4);

                // Implicit Release After
                liblte_value_2_bits(sps_cnfg->sps_cnfg_ul.implicit_release_after, ie_ptr, 2);

                // P0 Persistent
                if(sps_cnfg->sps_cnfg_ul.p0_persistent_present)
                {
                    // P0 Nominal PUSCH Persistent
                    liblte_value_2_bits(sps_cnfg->sps_cnfg_ul.p0_nominal_pusch + 126, ie_ptr, 8);

                    // P0 UE PUSCH Persistent
                    liblte_value_2_bits(sps_cnfg->sps_cnfg_ul.p0_ue_pusch + 8, ie_ptr, 4);
                }

                // Two Intervals Config
                if(sps_cnfg->sps_cnfg_ul.two_intervals_cnfg_present)
                {
                    liblte_value_2_bits(sps_cnfg->sps_cnfg_ul.two_intervals_cnfg, ie_ptr, 1);
                }
            }
        }

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_unpack_sps_config_ie(uint8                        **ie_ptr,
                                                  LIBLTE_RRC_SPS_CONFIG_STRUCT  *sps_cnfg)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;
    uint32            i;

    if(ie_ptr   != NULL &&
       sps_cnfg != NULL)
    {
        // Optional indicators
        sps_cnfg->sps_c_rnti_present  = liblte_bits_2_value(ie_ptr, 1);
        sps_cnfg->sps_cnfg_dl_present = liblte_bits_2_value(ie_ptr, 1);
        sps_cnfg->sps_cnfg_ul_present = liblte_bits_2_value(ie_ptr, 1);

        // SPS C-RNTI
        if(sps_cnfg->sps_c_rnti_present)
        {
            liblte_rrc_unpack_c_rnti_ie(ie_ptr, &sps_cnfg->sps_c_rnti);
        }

        // SPS Config DL
        if(sps_cnfg->sps_cnfg_dl_present)
        {
            sps_cnfg->sps_cnfg_dl.setup_present = liblte_bits_2_value(ie_ptr, 1);
            if(sps_cnfg->sps_cnfg_dl.setup_present)
            {
                // Extension indicator
                bool ext = liblte_bits_2_value(ie_ptr, 1);

                // SPS Interval DL
                sps_cnfg->sps_cnfg_dl.sps_interval_dl = (LIBLTE_RRC_SPS_INTERVAL_DL_ENUM)liblte_bits_2_value(ie_ptr, 4);

                // Number of Configured SPS Processes
                sps_cnfg->sps_cnfg_dl.N_sps_processes = liblte_bits_2_value(ie_ptr, 3) + 1;

                // N1 PUCCH AN Persistent List
                sps_cnfg->sps_cnfg_dl.n1_pucch_an_persistent_list_size = liblte_bits_2_value(ie_ptr, 2) + 1;
                for(i=0; i<sps_cnfg->sps_cnfg_dl.n1_pucch_an_persistent_list_size; i++)
                {
                    sps_cnfg->sps_cnfg_dl.n1_pucch_an_persistent_list[i] = liblte_bits_2_value(ie_ptr, 11);
                }
                
                liblte_rrc_consume_noncrit_extension(ext, __func__, ie_ptr);
            }
        }

        // SPS Config UL
        if(sps_cnfg->sps_cnfg_ul_present)
        {
            sps_cnfg->sps_cnfg_ul.setup_present = liblte_bits_2_value(ie_ptr, 1);
            if(sps_cnfg->sps_cnfg_ul.setup_present)
            {
                // Extension indicator
                bool ext = liblte_bits_2_value(ie_ptr, 1);

                // Optional indicators
                sps_cnfg->sps_cnfg_ul.p0_persistent_present      = liblte_bits_2_value(ie_ptr, 1);
                sps_cnfg->sps_cnfg_ul.two_intervals_cnfg_present = liblte_bits_2_value(ie_ptr, 1);

                // SPS Interval UL
                sps_cnfg->sps_cnfg_ul.sps_interval_ul = (LIBLTE_RRC_SPS_INTERVAL_UL_ENUM)liblte_bits_2_value(ie_ptr, 4);

                // Implicit Release After
                sps_cnfg->sps_cnfg_ul.implicit_release_after = (LIBLTE_RRC_IMPLICIT_RELEASE_AFTER_ENUM)liblte_bits_2_value(ie_ptr, 2);

                // P0 Persistent
                if(sps_cnfg->sps_cnfg_ul.p0_persistent_present)
                {
                    // P0 Nominal PUSCH Persistent
                    sps_cnfg->sps_cnfg_ul.p0_nominal_pusch = liblte_bits_2_value(ie_ptr, 8) - 126;

                    // P0 UE PUSCH Persistent
                    sps_cnfg->sps_cnfg_ul.p0_ue_pusch = liblte_bits_2_value(ie_ptr, 4) - 8;
                }

                // Two Intervals Config
                if(sps_cnfg->sps_cnfg_ul.two_intervals_cnfg_present)
                {
                    sps_cnfg->sps_cnfg_ul.two_intervals_cnfg = (LIBLTE_RRC_TWO_INTERVALS_CONFIG_ENUM)liblte_bits_2_value(ie_ptr, 1);
                }
                
                liblte_rrc_consume_noncrit_extension(ext, __func__, ie_ptr);
            }
        }

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: TDD Config

    Description: Specifies the TDD specific physical channel
                 configuration

    Document Reference: 36.331 v10.0.0 Section 6.3.2
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_rrc_pack_tdd_config_ie(LIBLTE_RRC_TDD_CONFIG_STRUCT  *tdd_cnfg,
                                                uint8                        **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(tdd_cnfg != NULL &&
       ie_ptr   != NULL)
    {
        liblte_value_2_bits(tdd_cnfg->sf_assignment,       ie_ptr, 3);
        liblte_value_2_bits(tdd_cnfg->special_sf_patterns, ie_ptr, 4);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_unpack_tdd_config_ie(uint8                        **ie_ptr,
                                                  LIBLTE_RRC_TDD_CONFIG_STRUCT  *tdd_cnfg)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr   != NULL &&
       tdd_cnfg != NULL)
    {
        tdd_cnfg->sf_assignment       = (LIBLTE_RRC_SUBFRAME_ASSIGNMENT_ENUM)liblte_bits_2_value(ie_ptr, 3);
        tdd_cnfg->special_sf_patterns = (LIBLTE_RRC_SPECIAL_SUBFRAME_PATTERNS_ENUM)liblte_bits_2_value(ie_ptr, 4);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: Time Alignment Timer

    Description: Controls how long the UE is considered uplink time
                 aligned

    Document Reference: 36.331 v10.0.0 Section 6.3.2
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_rrc_pack_time_alignment_timer_ie(LIBLTE_RRC_TIME_ALIGNMENT_TIMER_ENUM   time_alignment_timer,
                                                          uint8                                **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr != NULL)
    {
        liblte_value_2_bits(time_alignment_timer, ie_ptr, 3);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_unpack_time_alignment_timer_ie(uint8                                **ie_ptr,
                                                            LIBLTE_RRC_TIME_ALIGNMENT_TIMER_ENUM  *time_alignment_timer)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr               != NULL &&
       time_alignment_timer != NULL)
    {
        *time_alignment_timer = (LIBLTE_RRC_TIME_ALIGNMENT_TIMER_ENUM)liblte_bits_2_value(ie_ptr, 3);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: TPC PDCCH Config

    Description: Specifies the RNTIs and indecies for PUCCH and PUSCH
                 power control

    Document Reference: 36.331 v10.0.0 Section 6.3.2
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_rrc_pack_tpc_pdcch_config_ie(LIBLTE_RRC_TPC_PDCCH_CONFIG_STRUCT  *tpc_pdcch_cnfg,
                                                      uint8                              **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(tpc_pdcch_cnfg != NULL &&
       ie_ptr         != NULL)
    {
        // Setup
        liblte_value_2_bits(tpc_pdcch_cnfg->setup_present, ie_ptr, 1);
        if(tpc_pdcch_cnfg->setup_present)
        {
            // TPC RNTI
            liblte_value_2_bits(tpc_pdcch_cnfg->tpc_rnti, ie_ptr, 16);

            // TPC Index
            liblte_value_2_bits(tpc_pdcch_cnfg->tpc_idx_choice, ie_ptr, 1);
            if(LIBLTE_RRC_TPC_INDEX_FORMAT_3 == tpc_pdcch_cnfg->tpc_idx_choice)
            {
                liblte_value_2_bits(tpc_pdcch_cnfg->tpc_idx - 1, ie_ptr, 4);
            }else{
                liblte_value_2_bits(tpc_pdcch_cnfg->tpc_idx - 1, ie_ptr, 5);
            }
        }

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_unpack_tpc_pdcch_config_ie(uint8                              **ie_ptr,
                                                        LIBLTE_RRC_TPC_PDCCH_CONFIG_STRUCT  *tpc_pdcch_cnfg)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr         != NULL &&
       tpc_pdcch_cnfg != NULL)
    {
        // Setup
        tpc_pdcch_cnfg->setup_present = liblte_bits_2_value(ie_ptr, 1);
        if(tpc_pdcch_cnfg->setup_present)
        {
            // TPC RNTI
            tpc_pdcch_cnfg->tpc_rnti = liblte_bits_2_value(ie_ptr, 16);

            // TPC Index
            tpc_pdcch_cnfg->tpc_idx_choice = (LIBLTE_RRC_TPC_INDEX_ENUM)liblte_bits_2_value(ie_ptr, 1);
            if(LIBLTE_RRC_TPC_INDEX_FORMAT_3 == tpc_pdcch_cnfg->tpc_idx_choice)
            {
                tpc_pdcch_cnfg->tpc_idx = liblte_bits_2_value(ie_ptr, 4) + 1;
            }else{
                tpc_pdcch_cnfg->tpc_idx = liblte_bits_2_value(ie_ptr, 5) + 1;
            }
        }

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: UL Antenna Info

    Description: Specifies the UL antenna configuration

    Document Reference: 36.331 v10.0.0 Section 6.3.2
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_rrc_pack_ul_antenna_info_ie(LIBLTE_RRC_UL_ANTENNA_INFO_STRUCT  *ul_ant_info,
                                                     uint8                             **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ul_ant_info != NULL &&
       ie_ptr      != NULL)
    {
        // Extension indicator
        liblte_value_2_bits(0, ie_ptr, 1);

        liblte_value_2_bits(ul_ant_info->ul_tx_mode,              ie_ptr, 3);
        liblte_value_2_bits(ul_ant_info->four_ant_port_activated, ie_ptr, 1);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_unpack_ul_antenna_info_ie(uint8                             **ie_ptr,
                                                       LIBLTE_RRC_UL_ANTENNA_INFO_STRUCT  *ul_ant_info)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr      != NULL &&
       ul_ant_info != NULL)
    {
        // Extension indicator
        bool ext = liblte_bits_2_value(ie_ptr, 1);

        ul_ant_info->ul_tx_mode              = (LIBLTE_RRC_UL_TRANSMISSION_MODE_R10_ENUM)liblte_bits_2_value(ie_ptr, 3);
        ul_ant_info->four_ant_port_activated = liblte_bits_2_value(ie_ptr, 1);

        liblte_rrc_consume_noncrit_extension(ext, __func__, ie_ptr);
        
        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: Uplink Power Control

    Description: Specifies the parameters for uplink power control in
                 the system information and in the dedicated
                 signalling

    Document Reference: 36.331 v10.0.0 Section 6.3.2
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_rrc_pack_ul_power_control_common_ie(LIBLTE_RRC_UL_POWER_CONTROL_COMMON_STRUCT  *ul_pwr_ctrl,
                                                             uint8                                     **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ul_pwr_ctrl != NULL &&
       ie_ptr      != NULL)
    {
        liblte_value_2_bits(ul_pwr_ctrl->p0_nominal_pusch + 126, ie_ptr, 8);
        liblte_value_2_bits(ul_pwr_ctrl->alpha,                  ie_ptr, 3);
        liblte_value_2_bits(ul_pwr_ctrl->p0_nominal_pucch + 127, ie_ptr, 5);

        // Delta F List
        liblte_value_2_bits(ul_pwr_ctrl->delta_flist_pucch.format_1,  ie_ptr, 2);
        liblte_value_2_bits(ul_pwr_ctrl->delta_flist_pucch.format_1b, ie_ptr, 2);
        liblte_value_2_bits(ul_pwr_ctrl->delta_flist_pucch.format_2,  ie_ptr, 2);
        liblte_value_2_bits(ul_pwr_ctrl->delta_flist_pucch.format_2a, ie_ptr, 2);
        liblte_value_2_bits(ul_pwr_ctrl->delta_flist_pucch.format_2b, ie_ptr, 2);

        liblte_value_2_bits((ul_pwr_ctrl->delta_preamble_msg3 / 2) + 1, ie_ptr, 3);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_unpack_ul_power_control_common_ie(uint8                                     **ie_ptr,
                                                               LIBLTE_RRC_UL_POWER_CONTROL_COMMON_STRUCT  *ul_pwr_ctrl)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr      != NULL &&
       ul_pwr_ctrl != NULL)
    {
        ul_pwr_ctrl->p0_nominal_pusch = liblte_bits_2_value(ie_ptr, 8) - 126;
        ul_pwr_ctrl->alpha            = (LIBLTE_RRC_UL_POWER_CONTROL_ALPHA_ENUM)liblte_bits_2_value(ie_ptr, 3);
        ul_pwr_ctrl->p0_nominal_pucch = liblte_bits_2_value(ie_ptr, 5) - 127;

        // Delta F List
        ul_pwr_ctrl->delta_flist_pucch.format_1  = (LIBLTE_RRC_DELTA_F_PUCCH_FORMAT_1_ENUM)liblte_bits_2_value(ie_ptr, 2);
        ul_pwr_ctrl->delta_flist_pucch.format_1b = (LIBLTE_RRC_DELTA_F_PUCCH_FORMAT_1B_ENUM)liblte_bits_2_value(ie_ptr, 2);
        ul_pwr_ctrl->delta_flist_pucch.format_2  = (LIBLTE_RRC_DELTA_F_PUCCH_FORMAT_2_ENUM)liblte_bits_2_value(ie_ptr, 2);
        ul_pwr_ctrl->delta_flist_pucch.format_2a = (LIBLTE_RRC_DELTA_F_PUCCH_FORMAT_2A_ENUM)liblte_bits_2_value(ie_ptr, 2);
        ul_pwr_ctrl->delta_flist_pucch.format_2b = (LIBLTE_RRC_DELTA_F_PUCCH_FORMAT_2B_ENUM)liblte_bits_2_value(ie_ptr, 2);

        ul_pwr_ctrl->delta_preamble_msg3 = (liblte_bits_2_value(ie_ptr, 3) - 1) * 2;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_pack_ul_power_control_dedicated_ie(LIBLTE_RRC_UL_POWER_CONTROL_DEDICATED_STRUCT  *ul_pwr_ctrl,
                                                                uint8                                        **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ul_pwr_ctrl != NULL &&
       ie_ptr      != NULL)
    {
        // Filter Coefficient default?
        liblte_value_2_bits(ul_pwr_ctrl->filter_coeff_present, ie_ptr, 1);

        // P0 UE PUSCH
        liblte_value_2_bits(ul_pwr_ctrl->p0_ue_pusch + 8, ie_ptr, 4);

        // Delta MCS Enabled
        liblte_value_2_bits(ul_pwr_ctrl->delta_mcs_en, ie_ptr, 1);

        // Accumulation Enabled
        liblte_value_2_bits(ul_pwr_ctrl->accumulation_en, ie_ptr, 1);

        // P0 UE PUCCH
        liblte_value_2_bits(ul_pwr_ctrl->p0_ue_pucch + 8, ie_ptr, 4);

        // P SRS Offset
        liblte_value_2_bits(ul_pwr_ctrl->p_srs_offset, ie_ptr, 4);

        // Filter Coefficient
        if(ul_pwr_ctrl->filter_coeff_present)
        {
          liblte_rrc_pack_filter_coefficient_ie(ul_pwr_ctrl->filter_coeff, ie_ptr);
        }

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_unpack_ul_power_control_dedicated_ie(uint8                                        **ie_ptr,
                                                                  LIBLTE_RRC_UL_POWER_CONTROL_DEDICATED_STRUCT  *ul_pwr_ctrl)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr      != NULL &&
       ul_pwr_ctrl != NULL)
    {
        // Filter Coefficient default?
        ul_pwr_ctrl->filter_coeff_present = liblte_bits_2_value(ie_ptr, 1);

        // P0 UE PUSCH
        ul_pwr_ctrl->p0_ue_pusch = liblte_bits_2_value(ie_ptr, 4) - 8;

        // Delta MCS Enabled
        ul_pwr_ctrl->delta_mcs_en = (LIBLTE_RRC_DELTA_MCS_ENABLED_ENUM)liblte_bits_2_value(ie_ptr, 1);

        // Accumulation Enabled
        ul_pwr_ctrl->accumulation_en = liblte_bits_2_value(ie_ptr, 1);

        // P0 UE PUCCH
        ul_pwr_ctrl->p0_ue_pucch = liblte_bits_2_value(ie_ptr, 4) - 8;

        // P SRS Offset
        ul_pwr_ctrl->p_srs_offset = liblte_bits_2_value(ie_ptr, 4);

        // Filter Coefficient
        if(ul_pwr_ctrl->filter_coeff_present)
        {
          liblte_rrc_unpack_filter_coefficient_ie(ie_ptr, &ul_pwr_ctrl->filter_coeff);
        }

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: System Information Block Type 2

    Description: Contains radio resource configuration that is common
                 for all UEs

    Document Reference: 36.331 v10.0.0 Section 6.3.1
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_rrc_pack_sys_info_block_type_2_ie(LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_2_STRUCT  *sib2,
                                                           uint8                                   **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;
    uint32            i;
    bool              mbsfn_subfr_cnfg_list_opt;

    if(sib2   != NULL &&
       ie_ptr != NULL)
    {
        // Extension indicator
        liblte_value_2_bits(0, ie_ptr, 1);

        // Optional indicators
        liblte_value_2_bits(sib2->ac_barring_info_present, ie_ptr, 1);
        if(0 != sib2->mbsfn_subfr_cnfg_list_size)
        {
            liblte_value_2_bits(1, ie_ptr, 1);
            mbsfn_subfr_cnfg_list_opt = true;
        }else{
            liblte_value_2_bits(0, ie_ptr, 1);
            mbsfn_subfr_cnfg_list_opt = false;
        }

        // AC Barring
        if(true == sib2->ac_barring_info_present)
        {
            liblte_value_2_bits(sib2->ac_barring_for_mo_signalling.enabled, ie_ptr, 1);
            liblte_value_2_bits(sib2->ac_barring_for_mo_data.enabled,       ie_ptr, 1);

            // AC Barring for emergency
            liblte_value_2_bits(sib2->ac_barring_for_emergency, ie_ptr, 1);

            // AC Barring for MO signalling
            if(true == sib2->ac_barring_for_mo_signalling.enabled)
            {
                liblte_value_2_bits(sib2->ac_barring_for_mo_signalling.factor,         ie_ptr, 4);
                liblte_value_2_bits(sib2->ac_barring_for_mo_signalling.time,           ie_ptr, 3);
                liblte_value_2_bits(sib2->ac_barring_for_mo_signalling.for_special_ac, ie_ptr, 5);
            }

            // AC Barring for MO data
            if(true == sib2->ac_barring_for_mo_data.enabled)
            {
                liblte_value_2_bits(sib2->ac_barring_for_mo_data.factor,         ie_ptr, 4);
                liblte_value_2_bits(sib2->ac_barring_for_mo_data.time,           ie_ptr, 3);
                liblte_value_2_bits(sib2->ac_barring_for_mo_data.for_special_ac, ie_ptr, 5);
            }
        }

        // Radio Resource Config Common
        liblte_rrc_pack_rr_config_common_sib_ie(&sib2->rr_config_common_sib, ie_ptr);

        // UE Timers and Constants
        liblte_rrc_pack_ue_timers_and_constants_ie(&sib2->ue_timers_and_constants, ie_ptr);

        // Frequency information
        {
            // Optional indicators
            liblte_value_2_bits(sib2->arfcn_value_eutra.present, ie_ptr, 1);
            liblte_value_2_bits(sib2->ul_bw.present,             ie_ptr, 1);

            // UL Carrier Frequency
            if(true == sib2->arfcn_value_eutra.present)
            {
                liblte_rrc_pack_arfcn_value_eutra_ie(sib2->arfcn_value_eutra.value, ie_ptr);
            }

            // UL Bandwidth
            if(true == sib2->ul_bw.present)
            {
                liblte_value_2_bits(sib2->ul_bw.bw, ie_ptr, 3);
            }

            // Additional Spectrum Emission
            liblte_rrc_pack_additional_spectrum_emission_ie(sib2->additional_spectrum_emission,
                                                            ie_ptr);
        }

        // MBSFN Subframe Config List
        if(true == mbsfn_subfr_cnfg_list_opt)
        {
            liblte_value_2_bits(sib2->mbsfn_subfr_cnfg_list_size - 1, ie_ptr, 3);
            for(i=0; i<sib2->mbsfn_subfr_cnfg_list_size; i++)
            {
                liblte_rrc_pack_mbsfn_subframe_config_ie(&sib2->mbsfn_subfr_cnfg_list[i], ie_ptr);
            }
        }

        // Time Alignment Timer Common
        liblte_rrc_pack_time_alignment_timer_ie(sib2->time_alignment_timer, ie_ptr);

        // FIXME: Not handling extensions

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_unpack_sys_info_block_type_2_ie(uint8                                   **ie_ptr,
                                                             LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_2_STRUCT  *sib2)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;
    uint32            i;
    uint8             ext_ind;
    bool              mbsfn_subfr_cnfg_list_opt;

    if(ie_ptr != NULL &&
       sib2   != NULL)
    {
        // Extension indicator
        ext_ind = liblte_bits_2_value(ie_ptr, 1);

        // Optional indicators
        sib2->ac_barring_info_present = liblte_bits_2_value(ie_ptr, 1);
        mbsfn_subfr_cnfg_list_opt     = liblte_bits_2_value(ie_ptr, 1);

        // AC Barring
        if(true == sib2->ac_barring_info_present)
        {
            // Optional indicators
            sib2->ac_barring_for_mo_signalling.enabled = liblte_bits_2_value(ie_ptr, 1);
            sib2->ac_barring_for_mo_data.enabled       = liblte_bits_2_value(ie_ptr, 1);

            // AC Barring for emergency
            sib2->ac_barring_for_emergency = liblte_bits_2_value(ie_ptr, 1);

            // AC Barring for MO signalling
            if(true == sib2->ac_barring_for_mo_signalling.enabled)
            {
                sib2->ac_barring_for_mo_signalling.factor         = (LIBLTE_RRC_AC_BARRING_FACTOR_ENUM)liblte_bits_2_value(ie_ptr, 4);
                sib2->ac_barring_for_mo_signalling.time           = (LIBLTE_RRC_AC_BARRING_TIME_ENUM)liblte_bits_2_value(ie_ptr, 3);
                sib2->ac_barring_for_mo_signalling.for_special_ac = liblte_bits_2_value(ie_ptr, 5);
            }

            // AC Barring for MO data
            if(true == sib2->ac_barring_for_mo_data.enabled)
            {
                sib2->ac_barring_for_mo_data.factor         = (LIBLTE_RRC_AC_BARRING_FACTOR_ENUM)liblte_bits_2_value(ie_ptr, 4);
                sib2->ac_barring_for_mo_data.time           = (LIBLTE_RRC_AC_BARRING_TIME_ENUM)liblte_bits_2_value(ie_ptr, 3);
                sib2->ac_barring_for_mo_data.for_special_ac = liblte_bits_2_value(ie_ptr, 5);
            }
        }else{
            sib2->ac_barring_for_emergency             = false;
            sib2->ac_barring_for_mo_signalling.enabled = false;
            sib2->ac_barring_for_mo_data.enabled       = false;
        }

        // Radio Resource Config Common
        liblte_rrc_unpack_rr_config_common_sib_ie(ie_ptr, &sib2->rr_config_common_sib);

        // UE Timers and Constants
        liblte_rrc_unpack_ue_timers_and_constants_ie(ie_ptr, &sib2->ue_timers_and_constants);

        // Frequency information
        {
            // Optional indicators
            sib2->arfcn_value_eutra.present = liblte_bits_2_value(ie_ptr, 1);
            sib2->ul_bw.present             = liblte_bits_2_value(ie_ptr, 1);

            // UL Carrier Frequency
            if(true == sib2->arfcn_value_eutra.present)
            {
                liblte_rrc_unpack_arfcn_value_eutra_ie(ie_ptr, &sib2->arfcn_value_eutra.value);
            }

            // UL Bandwidth
            if(true == sib2->ul_bw.present)
            {
                sib2->ul_bw.bw = (LIBLTE_RRC_UL_BW_ENUM)liblte_bits_2_value(ie_ptr, 3);
            }

            // Additional Spectrum Emission
            liblte_rrc_unpack_additional_spectrum_emission_ie(ie_ptr,
                                                              &sib2->additional_spectrum_emission);
        }

        // MBSFN Subframe Config List
        if(true == mbsfn_subfr_cnfg_list_opt)
        {
            sib2->mbsfn_subfr_cnfg_list_size = liblte_bits_2_value(ie_ptr, 3) + 1;
            for(i=0; i<sib2->mbsfn_subfr_cnfg_list_size; i++)
            {
                liblte_rrc_unpack_mbsfn_subframe_config_ie(ie_ptr, &sib2->mbsfn_subfr_cnfg_list[i]);
            }
        }else{
            sib2->mbsfn_subfr_cnfg_list_size = 0;
        }

        // Time Alignment Timer Common
        liblte_rrc_unpack_time_alignment_timer_ie(ie_ptr, &sib2->time_alignment_timer);

        // Extensions
        liblte_rrc_consume_noncrit_extension(ext_ind, __func__, ie_ptr);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: System Information Block Type 3

    Description: Contains cell reselection information common for
                 intra-frequency, inter-frequency, and/or inter-RAT
                 cell re-selection as well as intra-frequency cell
                 re-selection information other than neighboring
                 cell related

    Document Reference: 36.331 v10.0.0 Section 6.3.1
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_rrc_pack_sys_info_block_type_3_ie(LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_3_STRUCT  *sib3,
                                                           uint8                                   **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(sib3   != NULL &&
       ie_ptr != NULL)
    {
        // Extension indicator
        liblte_value_2_bits(0, ie_ptr, 1);

        // Cell reselection info common
        {
            // Optional indicator
            liblte_value_2_bits(sib3->speed_state_resel_params.present, ie_ptr, 1);

            liblte_value_2_bits(sib3->q_hyst, ie_ptr, 4);

            // Speed state reselection parameters
            if(true == sib3->speed_state_resel_params.present)
            {
                liblte_rrc_pack_mobility_state_parameters_ie(&sib3->speed_state_resel_params.mobility_state_params, ie_ptr);

                liblte_value_2_bits(sib3->speed_state_resel_params.q_hyst_sf.medium, ie_ptr, 2);
                liblte_value_2_bits(sib3->speed_state_resel_params.q_hyst_sf.high,   ie_ptr, 2);
            }
        }

        // Cell reselection serving frequency information
        {
            // Optional indicators
            liblte_value_2_bits(sib3->s_non_intra_search_present, ie_ptr, 1);

            if(true == sib3->s_non_intra_search_present)
            {
                liblte_rrc_pack_reselection_threshold_ie(sib3->s_non_intra_search, ie_ptr);
            }

            liblte_rrc_pack_reselection_threshold_ie(sib3->thresh_serving_low, ie_ptr);

            liblte_rrc_pack_cell_reselection_priority_ie(sib3->cell_resel_prio, ie_ptr);
        }

        // Intra frequency cell reselection information
        {
            // Optional indicators
            liblte_value_2_bits(sib3->p_max_present,            ie_ptr, 1);
            liblte_value_2_bits(sib3->s_intra_search_present,   ie_ptr, 1);
            liblte_value_2_bits(sib3->allowed_meas_bw_present,  ie_ptr, 1);
            liblte_value_2_bits(sib3->t_resel_eutra_sf_present, ie_ptr, 1);

            liblte_rrc_pack_q_rx_lev_min_ie(sib3->q_rx_lev_min, ie_ptr);

            if(true == sib3->p_max_present)
            {
                liblte_rrc_pack_p_max_ie(sib3->p_max, ie_ptr);
            }

            if(true == sib3->s_intra_search_present)
            {
                liblte_rrc_pack_reselection_threshold_ie(sib3->s_intra_search, ie_ptr);
            }

            if(true == sib3->allowed_meas_bw_present)
            {
                liblte_rrc_pack_allowed_meas_bandwidth_ie(sib3->allowed_meas_bw, ie_ptr);
            }

            liblte_rrc_pack_presence_antenna_port_1_ie(sib3->presence_ant_port_1, ie_ptr);

            liblte_rrc_pack_neigh_cell_config_ie(sib3->neigh_cell_cnfg, ie_ptr);

            liblte_rrc_pack_t_reselection_ie(sib3->t_resel_eutra, ie_ptr);

            if(true == sib3->t_resel_eutra_sf_present)
            {
                liblte_rrc_pack_speed_state_scale_factors_ie(&sib3->t_resel_eutra_sf, ie_ptr);
            }
        }

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_unpack_sys_info_block_type_3_ie(uint8                                   **ie_ptr,
                                                             LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_3_STRUCT  *sib3)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;
    bool              ext_ind;

    if(ie_ptr != NULL &&
       sib3   != NULL)
    {
        // Extension indicator
        ext_ind = liblte_bits_2_value(ie_ptr, 1);

        // Cell reselection info common
        {
            // Optional indicator
            sib3->speed_state_resel_params.present = liblte_bits_2_value(ie_ptr, 1);

            sib3->q_hyst = (LIBLTE_RRC_Q_HYST_ENUM)liblte_bits_2_value(ie_ptr, 4);

            // Speed state reselection parameters
            if(true == sib3->speed_state_resel_params.present)
            {
                liblte_rrc_unpack_mobility_state_parameters_ie(ie_ptr, &sib3->speed_state_resel_params.mobility_state_params);

                sib3->speed_state_resel_params.q_hyst_sf.medium = (LIBLTE_RRC_SF_MEDIUM_ENUM)liblte_bits_2_value(ie_ptr, 2);
                sib3->speed_state_resel_params.q_hyst_sf.high   = (LIBLTE_RRC_SF_HIGH_ENUM)liblte_bits_2_value(ie_ptr, 2);
            }
        }

        // Cell reselection serving frequency information
        {
            // Optional indicators
            sib3->s_non_intra_search_present = liblte_bits_2_value(ie_ptr, 1);

            if(true == sib3->s_non_intra_search_present)
            {
                liblte_rrc_unpack_reselection_threshold_ie(ie_ptr, &sib3->s_non_intra_search);
            }

            liblte_rrc_unpack_reselection_threshold_ie(ie_ptr, &sib3->thresh_serving_low);

            liblte_rrc_unpack_cell_reselection_priority_ie(ie_ptr, &sib3->cell_resel_prio);
        }

        // Intra frequency cell reselection information
        {
            // Optional indicators
            sib3->p_max_present            = liblte_bits_2_value(ie_ptr, 1);
            sib3->s_intra_search_present   = liblte_bits_2_value(ie_ptr, 1);
            sib3->allowed_meas_bw_present  = liblte_bits_2_value(ie_ptr, 1);
            sib3->t_resel_eutra_sf_present = liblte_bits_2_value(ie_ptr, 1);

            liblte_rrc_unpack_q_rx_lev_min_ie(ie_ptr, &sib3->q_rx_lev_min);

            if(true == sib3->p_max_present)
            {
                liblte_rrc_unpack_p_max_ie(ie_ptr, &sib3->p_max);
            }

            if(true == sib3->s_intra_search_present)
            {
                liblte_rrc_unpack_reselection_threshold_ie(ie_ptr, &sib3->s_intra_search);
            }

            if(true == sib3->allowed_meas_bw_present)
            {
                liblte_rrc_unpack_allowed_meas_bandwidth_ie(ie_ptr, &sib3->allowed_meas_bw);
            }

            liblte_rrc_unpack_presence_antenna_port_1_ie(ie_ptr, &sib3->presence_ant_port_1);

            liblte_rrc_unpack_neigh_cell_config_ie(ie_ptr, &sib3->neigh_cell_cnfg);

            liblte_rrc_unpack_t_reselection_ie(ie_ptr, &sib3->t_resel_eutra);

            if(true == sib3->t_resel_eutra_sf_present)
            {
                liblte_rrc_unpack_speed_state_scale_factors_ie(ie_ptr, &sib3->t_resel_eutra_sf);
            }
        }

        // Extensions
        liblte_rrc_consume_noncrit_extension(ext_ind, __func__, ie_ptr);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: System Information Block Type 4

    Description: Contains the neighboring cell related information
                 relevant only for intra-frequency cell reselection

    Document Reference: 36.331 v10.0.0 Section 6.3.1
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_rrc_pack_sys_info_block_type_4_ie(LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_4_STRUCT  *sib4,
                                                           uint8                                   **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;
    uint32            i;

    if(sib4   != NULL &&
       ie_ptr != NULL)
    {
        // Extension indicator
        liblte_value_2_bits(0, ie_ptr, 1);

        // Optional indicators
        if(0 != sib4->intra_freq_neigh_cell_list_size)
        {
            liblte_value_2_bits(1, ie_ptr, 1);
        }else{
            liblte_value_2_bits(0, ie_ptr, 1);
        }
        if(0 != sib4->intra_freq_black_cell_list_size)
        {
            liblte_value_2_bits(1, ie_ptr, 1);
        }else{
            liblte_value_2_bits(0, ie_ptr, 1);
        }
        liblte_value_2_bits(sib4->csg_phys_cell_id_range_present, ie_ptr, 1);

        if(0 != sib4->intra_freq_neigh_cell_list_size)
        {
            liblte_value_2_bits(sib4->intra_freq_neigh_cell_list_size - 1, ie_ptr, 4);
            for(i=0; i<sib4->intra_freq_neigh_cell_list_size; i++)
            {
                // Extension indicator
                liblte_value_2_bits(0, ie_ptr, 1);

                liblte_rrc_pack_phys_cell_id_ie(sib4->intra_freq_neigh_cell_list[i].phys_cell_id, ie_ptr);
                liblte_rrc_pack_q_offset_range_ie(sib4->intra_freq_neigh_cell_list[i].q_offset_range, ie_ptr);
            }
        }

        if(0 != sib4->intra_freq_black_cell_list_size)
        {
            liblte_value_2_bits(sib4->intra_freq_black_cell_list_size - 1, ie_ptr, 4);
            for(i=0; i<sib4->intra_freq_black_cell_list_size; i++)
            {
                liblte_rrc_pack_phys_cell_id_range_ie(&sib4->intra_freq_black_cell_list[i], ie_ptr);
            }
        }

        if(true == sib4->csg_phys_cell_id_range_present)
        {
            liblte_rrc_pack_phys_cell_id_range_ie(&sib4->csg_phys_cell_id_range, ie_ptr);
        }

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_unpack_sys_info_block_type_4_ie(uint8                                   **ie_ptr,
                                                             LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_4_STRUCT  *sib4)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;
    uint32            i;
    bool              ext_ind;
    bool              intra_freq_neigh_cell_list_opt;
    bool              intra_freq_black_cell_list_opt;

    if(ie_ptr != NULL &&
       sib4   != NULL)
    {
        // Extension indicator
        ext_ind = liblte_bits_2_value(ie_ptr, 1);

        // Optional indicators
        intra_freq_neigh_cell_list_opt       = liblte_bits_2_value(ie_ptr, 1);
        intra_freq_black_cell_list_opt       = liblte_bits_2_value(ie_ptr, 1);
        sib4->csg_phys_cell_id_range_present = liblte_bits_2_value(ie_ptr, 1);

        if(true == intra_freq_neigh_cell_list_opt)
        {
            sib4->intra_freq_neigh_cell_list_size = liblte_bits_2_value(ie_ptr, 4) + 1;
            for(i=0; i<sib4->intra_freq_neigh_cell_list_size; i++)
            {
                // Extension indicator
                liblte_bits_2_value(ie_ptr, 1);

                liblte_rrc_unpack_phys_cell_id_ie(ie_ptr, &sib4->intra_freq_neigh_cell_list[i].phys_cell_id);
                liblte_rrc_unpack_q_offset_range_ie(ie_ptr, &sib4->intra_freq_neigh_cell_list[i].q_offset_range);
            }
        }else{
            sib4->intra_freq_neigh_cell_list_size = 0;
        }

        if(true == intra_freq_black_cell_list_opt)
        {
            sib4->intra_freq_black_cell_list_size = liblte_bits_2_value(ie_ptr, 4) + 1;
            for(i=0; i<sib4->intra_freq_black_cell_list_size; i++)
            {
                liblte_rrc_unpack_phys_cell_id_range_ie(ie_ptr, &sib4->intra_freq_black_cell_list[i]);
            }
        }else{
            sib4->intra_freq_black_cell_list_size = 0;
        }

        if(true == sib4->csg_phys_cell_id_range_present)
        {
            liblte_rrc_unpack_phys_cell_id_range_ie(ie_ptr, &sib4->csg_phys_cell_id_range);
        }

        // Extension
        liblte_rrc_consume_noncrit_extension(ext_ind, __func__, ie_ptr);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: System Information Block Type 5

    Description: Contains information relevant only for
                 inter-frequency cell reselection, i.e. information
                 about other E-UTRA frequencies and inter-frequency
                 neighboring cells relevant for cell reselection

    Document Reference: 36.331 v10.0.0 Section 6.3.1
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_rrc_pack_sys_info_block_type_5_ie(LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_5_STRUCT  *sib5,
                                                           uint8                                   **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;
    uint32            i;
    uint32            j;

    if(sib5   != NULL &&
       ie_ptr != NULL)
    {
        // Extension indicator
        liblte_value_2_bits(0, ie_ptr, 1);

        liblte_value_2_bits(sib5->inter_freq_carrier_freq_list_size - 1, ie_ptr, 3);
        for(i=0; i<sib5->inter_freq_carrier_freq_list_size; i++)
        {
            // Extension indicator
            liblte_value_2_bits(0, ie_ptr, 1);

            // Optional indicators
            liblte_value_2_bits(sib5->inter_freq_carrier_freq_list[i].p_max_present,            ie_ptr, 1);
            liblte_value_2_bits(sib5->inter_freq_carrier_freq_list[i].t_resel_eutra_sf_present, ie_ptr, 1);
            liblte_value_2_bits(sib5->inter_freq_carrier_freq_list[i].cell_resel_prio_present,  ie_ptr, 1);
            if(0 != sib5->inter_freq_carrier_freq_list[i].inter_freq_neigh_cell_list_size)
            {
                liblte_value_2_bits(1, ie_ptr, 1);
            }else{
                liblte_value_2_bits(0, ie_ptr, 1);
            }
            if(0 != sib5->inter_freq_carrier_freq_list[i].inter_freq_black_cell_list_size)
            {
                liblte_value_2_bits(1, ie_ptr, 1);
            }else{
                liblte_value_2_bits(0, ie_ptr, 1);
            }

            liblte_rrc_pack_arfcn_value_eutra_ie(sib5->inter_freq_carrier_freq_list[i].dl_carrier_freq, ie_ptr);
            liblte_rrc_pack_q_rx_lev_min_ie(sib5->inter_freq_carrier_freq_list[i].q_rx_lev_min, ie_ptr);
            if(true == sib5->inter_freq_carrier_freq_list[i].p_max_present)
            {
                liblte_rrc_pack_p_max_ie(sib5->inter_freq_carrier_freq_list[i].p_max, ie_ptr);
            }
            liblte_rrc_pack_t_reselection_ie(sib5->inter_freq_carrier_freq_list[i].t_resel_eutra, ie_ptr);
            if(true == sib5->inter_freq_carrier_freq_list[i].t_resel_eutra_sf_present)
            {
                liblte_rrc_pack_speed_state_scale_factors_ie(&sib5->inter_freq_carrier_freq_list[i].t_resel_eutra_sf, ie_ptr);
            }
            liblte_rrc_pack_reselection_threshold_ie(sib5->inter_freq_carrier_freq_list[i].threshx_high, ie_ptr);
            liblte_rrc_pack_reselection_threshold_ie(sib5->inter_freq_carrier_freq_list[i].threshx_low, ie_ptr);
            liblte_rrc_pack_allowed_meas_bandwidth_ie(sib5->inter_freq_carrier_freq_list[i].allowed_meas_bw, ie_ptr);
            liblte_rrc_pack_presence_antenna_port_1_ie(sib5->inter_freq_carrier_freq_list[i].presence_ant_port_1, ie_ptr);
            if(true == sib5->inter_freq_carrier_freq_list[i].cell_resel_prio_present)
            {
                liblte_rrc_pack_cell_reselection_priority_ie(sib5->inter_freq_carrier_freq_list[i].cell_resel_prio, ie_ptr);
            }
            liblte_rrc_pack_neigh_cell_config_ie(sib5->inter_freq_carrier_freq_list[i].neigh_cell_cnfg, ie_ptr);
            liblte_rrc_pack_q_offset_range_ie(sib5->inter_freq_carrier_freq_list[i].q_offset_freq, ie_ptr);
            if(0 != sib5->inter_freq_carrier_freq_list[i].inter_freq_neigh_cell_list_size)
            {
                liblte_value_2_bits(sib5->inter_freq_carrier_freq_list[i].inter_freq_neigh_cell_list_size - 1, ie_ptr, 4);
                for(j=0; j<sib5->inter_freq_carrier_freq_list[i].inter_freq_neigh_cell_list_size; j++)
                {
                    liblte_rrc_pack_phys_cell_id_ie(sib5->inter_freq_carrier_freq_list[i].inter_freq_neigh_cell_list[j].phys_cell_id, ie_ptr);
                    liblte_rrc_pack_q_offset_range_ie(sib5->inter_freq_carrier_freq_list[i].inter_freq_neigh_cell_list[j].q_offset_cell, ie_ptr);
                }
            }
            if(0 != sib5->inter_freq_carrier_freq_list[i].inter_freq_black_cell_list_size)
            {
                liblte_value_2_bits(sib5->inter_freq_carrier_freq_list[i].inter_freq_black_cell_list_size - 1, ie_ptr, 4);
                for(j=0; j<sib5->inter_freq_carrier_freq_list[i].inter_freq_black_cell_list_size; j++)
                {
                    liblte_rrc_pack_phys_cell_id_range_ie(&sib5->inter_freq_carrier_freq_list[i].inter_freq_black_cell_list[j], ie_ptr);
                }
            }
        }

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_unpack_sys_info_block_type_5_ie(uint8                                   **ie_ptr,
                                                             LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_5_STRUCT  *sib5)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;
    uint32            i;
    uint32            j;
    bool              ext_ind;
    bool              inter_freq_carrier_freq_list_ext_ind;
    bool              q_offset_freq_opt;
    bool              inter_freq_neigh_cell_list_opt;
    bool              inter_freq_black_cell_list_opt;

    if(ie_ptr != NULL &&
       sib5   != NULL)
    {
        // Extension indicator
        ext_ind = liblte_bits_2_value(ie_ptr, 1);

        sib5->inter_freq_carrier_freq_list_size = liblte_bits_2_value(ie_ptr, 3) + 1;
        for(i=0; i<sib5->inter_freq_carrier_freq_list_size; i++)
        {
            // Extension indicator
            inter_freq_carrier_freq_list_ext_ind = liblte_bits_2_value(ie_ptr, 1);

            // Optional indicators
            sib5->inter_freq_carrier_freq_list[i].p_max_present            = liblte_bits_2_value(ie_ptr, 1);
            sib5->inter_freq_carrier_freq_list[i].t_resel_eutra_sf_present = liblte_bits_2_value(ie_ptr, 1);
            sib5->inter_freq_carrier_freq_list[i].cell_resel_prio_present  = liblte_bits_2_value(ie_ptr, 1);
            q_offset_freq_opt                                              = liblte_bits_2_value(ie_ptr, 1);
            inter_freq_neigh_cell_list_opt                                 = liblte_bits_2_value(ie_ptr, 1);
            inter_freq_black_cell_list_opt                                 = liblte_bits_2_value(ie_ptr, 1);

            liblte_rrc_unpack_arfcn_value_eutra_ie(ie_ptr, &sib5->inter_freq_carrier_freq_list[i].dl_carrier_freq);
            liblte_rrc_unpack_q_rx_lev_min_ie(ie_ptr, &sib5->inter_freq_carrier_freq_list[i].q_rx_lev_min);
            if(true == sib5->inter_freq_carrier_freq_list[i].p_max_present)
            {
                liblte_rrc_unpack_p_max_ie(ie_ptr, &sib5->inter_freq_carrier_freq_list[i].p_max);
            }
            liblte_rrc_unpack_t_reselection_ie(ie_ptr, &sib5->inter_freq_carrier_freq_list[i].t_resel_eutra);
            if(true == sib5->inter_freq_carrier_freq_list[i].t_resel_eutra_sf_present)
            {
                liblte_rrc_unpack_speed_state_scale_factors_ie(ie_ptr, &sib5->inter_freq_carrier_freq_list[i].t_resel_eutra_sf);
            }
            liblte_rrc_unpack_reselection_threshold_ie(ie_ptr, &sib5->inter_freq_carrier_freq_list[i].threshx_high);
            liblte_rrc_unpack_reselection_threshold_ie(ie_ptr, &sib5->inter_freq_carrier_freq_list[i].threshx_low);
            liblte_rrc_unpack_allowed_meas_bandwidth_ie(ie_ptr, &sib5->inter_freq_carrier_freq_list[i].allowed_meas_bw);
            liblte_rrc_unpack_presence_antenna_port_1_ie(ie_ptr, &sib5->inter_freq_carrier_freq_list[i].presence_ant_port_1);
            if(true == sib5->inter_freq_carrier_freq_list[i].cell_resel_prio_present)
            {
                liblte_rrc_unpack_cell_reselection_priority_ie(ie_ptr, &sib5->inter_freq_carrier_freq_list[i].cell_resel_prio);
            }
            liblte_rrc_unpack_neigh_cell_config_ie(ie_ptr, &sib5->inter_freq_carrier_freq_list[i].neigh_cell_cnfg);
            if(true == q_offset_freq_opt)
            {
                liblte_rrc_unpack_q_offset_range_ie(ie_ptr, &sib5->inter_freq_carrier_freq_list[i].q_offset_freq);
            }else{
                sib5->inter_freq_carrier_freq_list[i].q_offset_freq = LIBLTE_RRC_Q_OFFSET_RANGE_DB_0;
            }
            if(true == inter_freq_neigh_cell_list_opt)
            {
                sib5->inter_freq_carrier_freq_list[i].inter_freq_neigh_cell_list_size = liblte_bits_2_value(ie_ptr, 4) + 1;
                for(j=0; j<sib5->inter_freq_carrier_freq_list[i].inter_freq_neigh_cell_list_size; j++)
                {
                    liblte_rrc_unpack_phys_cell_id_ie(ie_ptr, &sib5->inter_freq_carrier_freq_list[i].inter_freq_neigh_cell_list[j].phys_cell_id);
                    liblte_rrc_unpack_q_offset_range_ie(ie_ptr, &sib5->inter_freq_carrier_freq_list[i].inter_freq_neigh_cell_list[j].q_offset_cell);
                }
            }else{
                sib5->inter_freq_carrier_freq_list[i].inter_freq_neigh_cell_list_size = 0;
            }
            if(true == inter_freq_black_cell_list_opt)
            {
                sib5->inter_freq_carrier_freq_list[i].inter_freq_black_cell_list_size = liblte_bits_2_value(ie_ptr, 4) + 1;
                for(j=0; j<sib5->inter_freq_carrier_freq_list[i].inter_freq_black_cell_list_size; j++)
                {
                    liblte_rrc_unpack_phys_cell_id_range_ie(ie_ptr, &sib5->inter_freq_carrier_freq_list[i].inter_freq_black_cell_list[j]);
                }
            }else{
                sib5->inter_freq_carrier_freq_list[i].inter_freq_black_cell_list_size = 0;
            }
        }
        
        liblte_rrc_consume_noncrit_extension(ext_ind, __func__, ie_ptr);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: System Information Block Type 6

    Description: Contains information relevant only for inter-RAT
                 cell reselection, i.e. information about UTRA
                 frequencies and UTRA neighboring cells relevant for
                 cell reselection

    Document Reference: 36.331 v10.0.0 Section 6.3.1
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_rrc_pack_sys_info_block_type_6_ie(LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_6_STRUCT  *sib6,
                                                           uint8                                   **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;
    uint32            i;

    if(sib6   != NULL &&
       ie_ptr != NULL)
    {
        // Extension indicator
        liblte_value_2_bits(0, ie_ptr, 1);

        // Optional indicators
        if(0 != sib6->carrier_freq_list_utra_fdd_size)
        {
            liblte_value_2_bits(1, ie_ptr, 1);
        }else{
            liblte_value_2_bits(0, ie_ptr, 1);
        }
        if(0 != sib6->carrier_freq_list_utra_tdd_size)
        {
            liblte_value_2_bits(1, ie_ptr, 1);
        }else{
            liblte_value_2_bits(0, ie_ptr, 1);
        }
        liblte_value_2_bits(sib6->t_resel_utra_sf_present, ie_ptr, 1);

        if(0 != sib6->carrier_freq_list_utra_fdd_size)
        {
            liblte_value_2_bits(sib6->carrier_freq_list_utra_fdd_size - 1, ie_ptr, 4);
            for(i=0; i<sib6->carrier_freq_list_utra_fdd_size; i++)
            {
                // Extension indicator
                liblte_value_2_bits(0, ie_ptr, 1);

                // Optional indicator
                liblte_value_2_bits(sib6->carrier_freq_list_utra_fdd[i].cell_resel_prio_present, ie_ptr, 1);

                liblte_rrc_pack_arfcn_value_utra_ie(sib6->carrier_freq_list_utra_fdd[i].carrier_freq, ie_ptr);
                if(true == sib6->carrier_freq_list_utra_fdd[i].cell_resel_prio_present)
                {
                    liblte_rrc_pack_cell_reselection_priority_ie(sib6->carrier_freq_list_utra_fdd[i].cell_resel_prio, ie_ptr);
                }
                liblte_rrc_pack_reselection_threshold_ie(sib6->carrier_freq_list_utra_fdd[i].threshx_high, ie_ptr);
                liblte_rrc_pack_reselection_threshold_ie(sib6->carrier_freq_list_utra_fdd[i].threshx_low, ie_ptr);
                liblte_value_2_bits(((sib6->carrier_freq_list_utra_fdd[i].q_rx_lev_min - 1) / 2) + 60, ie_ptr, 6);
                liblte_value_2_bits(sib6->carrier_freq_list_utra_fdd[i].p_max_utra + 50,               ie_ptr, 7);
                liblte_value_2_bits(sib6->carrier_freq_list_utra_fdd[i].q_qual_min + 24,               ie_ptr, 5);
            }
        }
        if(0 != sib6->carrier_freq_list_utra_tdd_size)
        {
            liblte_value_2_bits(sib6->carrier_freq_list_utra_tdd_size - 1, ie_ptr, 4);
            for(i=0; i<sib6->carrier_freq_list_utra_tdd_size; i++)
            {
                // Extension indicator
                liblte_value_2_bits(0, ie_ptr, 1);

                // Optional indicator
                liblte_value_2_bits(sib6->carrier_freq_list_utra_tdd[i].cell_resel_prio_present, ie_ptr, 1);

                liblte_rrc_pack_arfcn_value_utra_ie(sib6->carrier_freq_list_utra_tdd[i].carrier_freq, ie_ptr);
                if(true == sib6->carrier_freq_list_utra_tdd[i].cell_resel_prio_present)
                {
                    liblte_rrc_pack_cell_reselection_priority_ie(sib6->carrier_freq_list_utra_tdd[i].cell_resel_prio, ie_ptr);
                }
                liblte_rrc_pack_reselection_threshold_ie(sib6->carrier_freq_list_utra_tdd[i].threshx_high, ie_ptr);
                liblte_rrc_pack_reselection_threshold_ie(sib6->carrier_freq_list_utra_tdd[i].threshx_low, ie_ptr);
                liblte_value_2_bits(((sib6->carrier_freq_list_utra_tdd[i].q_rx_lev_min - 1) / 2) + 60, ie_ptr, 6);
                liblte_value_2_bits(sib6->carrier_freq_list_utra_tdd[i].p_max_utra + 50,               ie_ptr, 7);
            }
        }
        liblte_rrc_pack_t_reselection_ie(sib6->t_resel_utra, ie_ptr);
        if(true == sib6->t_resel_utra_sf_present)
        {
            liblte_rrc_pack_speed_state_scale_factors_ie(&sib6->t_resel_utra_sf, ie_ptr);
        }

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_unpack_sys_info_block_type_6_ie(uint8                                   **ie_ptr,
                                                             LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_6_STRUCT  *sib6)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;
    uint32            i;
    bool              ext_ind;
    bool              carrier_freq_list_utra_fdd_opt;
    bool              carrier_freq_list_utra_fdd_ext_ind;
    bool              carrier_freq_list_utra_tdd_opt;
    bool              carrier_freq_list_utra_tdd_ext_ind;

    if(ie_ptr != NULL &&
       sib6   != NULL)
    {
        // Extension indicator
        ext_ind = liblte_bits_2_value(ie_ptr, 1);

        // Optional indicators
        carrier_freq_list_utra_fdd_opt = liblte_bits_2_value(ie_ptr, 1);
        carrier_freq_list_utra_tdd_opt = liblte_bits_2_value(ie_ptr, 1);
        sib6->t_resel_utra_sf_present  = liblte_bits_2_value(ie_ptr, 1);

        if(true == carrier_freq_list_utra_fdd_opt)
        {
            sib6->carrier_freq_list_utra_fdd_size = liblte_bits_2_value(ie_ptr, 4) + 1;
            for(i=0; i<sib6->carrier_freq_list_utra_fdd_size; i++)
            {
                // Extension indicator
                carrier_freq_list_utra_fdd_ext_ind = liblte_bits_2_value(ie_ptr, 1);

                // Optional indicator
                sib6->carrier_freq_list_utra_fdd[i].cell_resel_prio_present = liblte_bits_2_value(ie_ptr, 1);

                liblte_rrc_unpack_arfcn_value_utra_ie(ie_ptr, &sib6->carrier_freq_list_utra_fdd[i].carrier_freq);
                if(true == sib6->carrier_freq_list_utra_fdd[i].cell_resel_prio_present)
                {
                    liblte_rrc_unpack_cell_reselection_priority_ie(ie_ptr, &sib6->carrier_freq_list_utra_fdd[i].cell_resel_prio);
                }
                liblte_rrc_unpack_reselection_threshold_ie(ie_ptr, &sib6->carrier_freq_list_utra_fdd[i].threshx_high);
                liblte_rrc_unpack_reselection_threshold_ie(ie_ptr, &sib6->carrier_freq_list_utra_fdd[i].threshx_low);
                sib6->carrier_freq_list_utra_fdd[i].q_rx_lev_min = (int8)((liblte_bits_2_value(ie_ptr, 6) - 60) * 2) + 1;
                sib6->carrier_freq_list_utra_fdd[i].p_max_utra   = (int8)liblte_bits_2_value(ie_ptr, 7) - 50;
                sib6->carrier_freq_list_utra_fdd[i].q_qual_min   = (int8)liblte_bits_2_value(ie_ptr, 5) - 24;
            }
        }else{
            sib6->carrier_freq_list_utra_fdd_size = 0;
        }
        if(true == carrier_freq_list_utra_tdd_opt)
        {
            sib6->carrier_freq_list_utra_tdd_size = liblte_bits_2_value(ie_ptr, 4) + 1;
            for(i=0; i<sib6->carrier_freq_list_utra_tdd_size; i++)
            {
                // Extension indicator
                carrier_freq_list_utra_tdd_ext_ind = liblte_bits_2_value(ie_ptr, 1);

                // Optional indicator
                sib6->carrier_freq_list_utra_tdd[i].cell_resel_prio_present = liblte_bits_2_value(ie_ptr, 1);

                liblte_rrc_unpack_arfcn_value_utra_ie(ie_ptr, &sib6->carrier_freq_list_utra_tdd[i].carrier_freq);
                if(true == sib6->carrier_freq_list_utra_tdd[i].cell_resel_prio_present)
                {
                    liblte_rrc_unpack_cell_reselection_priority_ie(ie_ptr, &sib6->carrier_freq_list_utra_tdd[i].cell_resel_prio);
                }
                liblte_rrc_unpack_reselection_threshold_ie(ie_ptr, &sib6->carrier_freq_list_utra_tdd[i].threshx_high);
                liblte_rrc_unpack_reselection_threshold_ie(ie_ptr, &sib6->carrier_freq_list_utra_tdd[i].threshx_low);
                sib6->carrier_freq_list_utra_tdd[i].q_rx_lev_min = (int8)((liblte_bits_2_value(ie_ptr, 6) * 2) + 1) - 60;
                sib6->carrier_freq_list_utra_tdd[i].p_max_utra   = (int8)liblte_bits_2_value(ie_ptr, 7) - 50;
            }
        }else{
            sib6->carrier_freq_list_utra_tdd_size = 0;
        }
        liblte_rrc_unpack_t_reselection_ie(ie_ptr, &sib6->t_resel_utra);
        if(true == sib6->t_resel_utra_sf_present)
        {
            liblte_rrc_unpack_speed_state_scale_factors_ie(ie_ptr, &sib6->t_resel_utra_sf);
        }
        
        liblte_rrc_consume_noncrit_extension(ext_ind, __func__, ie_ptr);
        
        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: System Information Block Type 7

    Description: Contains information relevant only for inter-RAT
                 cell reselection, i.e. information about GERAN
                 frequencies relevant for cell reselection

    Document Reference: 36.331 v10.0.0 Section 6.3.1
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_rrc_pack_sys_info_block_type_7_ie(LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_7_STRUCT  *sib7,
                                                           uint8                                   **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;
    uint32            i;

    if(sib7   != NULL &&
       ie_ptr != NULL)
    {
        // Extension indicator
        liblte_value_2_bits(0, ie_ptr, 1);

        // Optional indicators
        liblte_value_2_bits(sib7->t_resel_geran_sf_present, ie_ptr, 1);
        if(0 != sib7->carrier_freqs_info_list_size)
        {
            liblte_value_2_bits(1, ie_ptr, 1);
        }else{
            liblte_value_2_bits(0, ie_ptr, 1);
        }

        liblte_rrc_pack_t_reselection_ie(sib7->t_resel_geran, ie_ptr);
        if(true == sib7->t_resel_geran_sf_present)
        {
            liblte_rrc_pack_speed_state_scale_factors_ie(&sib7->t_resel_geran_sf, ie_ptr);
        }
        if(0 != sib7->carrier_freqs_info_list_size)
        {
            liblte_value_2_bits(sib7->carrier_freqs_info_list_size - 1, ie_ptr, 4);
            for(i=0; i<sib7->carrier_freqs_info_list_size; i++)
            {
                // Extension indicator
                liblte_value_2_bits(0, ie_ptr, 1);

                liblte_rrc_pack_carrier_freqs_geran_ie(&sib7->carrier_freqs_info_list[i].carrier_freqs, ie_ptr);

                // Common Info
                {
                    // Optional indicators
                    liblte_value_2_bits(sib7->carrier_freqs_info_list[i].cell_resel_prio_present, ie_ptr, 1);
                    liblte_value_2_bits(sib7->carrier_freqs_info_list[i].p_max_geran_present,     ie_ptr, 1);

                    if(true == sib7->carrier_freqs_info_list[i].cell_resel_prio_present)
                    {
                        liblte_rrc_pack_cell_reselection_priority_ie(sib7->carrier_freqs_info_list[i].cell_resel_prio, ie_ptr);
                    }
                    liblte_value_2_bits(sib7->carrier_freqs_info_list[i].ncc_permitted,            ie_ptr, 8);
                    liblte_value_2_bits((sib7->carrier_freqs_info_list[i].q_rx_lev_min + 115) / 2, ie_ptr, 6);
                    if(true == sib7->carrier_freqs_info_list[i].p_max_geran_present)
                    {
                        liblte_value_2_bits(sib7->carrier_freqs_info_list[i].p_max_geran, ie_ptr, 6);
                    }
                    liblte_rrc_pack_reselection_threshold_ie(sib7->carrier_freqs_info_list[i].threshx_high, ie_ptr);
                    liblte_rrc_pack_reselection_threshold_ie(sib7->carrier_freqs_info_list[i].threshx_low,  ie_ptr);
                }
            }
        }

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_unpack_sys_info_block_type_7_ie(uint8                                   **ie_ptr,
                                                             LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_7_STRUCT  *sib7)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;
    uint32            i;
    bool              ext_ind;
    bool              carrier_freqs_info_list_opt;
    bool              carrier_freqs_info_list_ext_ind;

    if(ie_ptr != NULL &&
       sib7   != NULL)
    {
        // Extension indicator
        ext_ind = liblte_bits_2_value(ie_ptr, 1);

        // Optional indicators
        sib7->t_resel_geran_sf_present = liblte_bits_2_value(ie_ptr, 1);
        carrier_freqs_info_list_opt    = liblte_bits_2_value(ie_ptr, 1);

        liblte_rrc_unpack_t_reselection_ie(ie_ptr, &sib7->t_resel_geran);
        if(true == sib7->t_resel_geran_sf_present)
        {
            liblte_rrc_unpack_speed_state_scale_factors_ie(ie_ptr, &sib7->t_resel_geran_sf);
        }
        if(true == carrier_freqs_info_list_opt)
        {
            sib7->carrier_freqs_info_list_size = liblte_bits_2_value(ie_ptr, 4) + 1;
            for(i=0; i<sib7->carrier_freqs_info_list_size; i++)
            {
                // Extension indicator
                carrier_freqs_info_list_ext_ind = liblte_bits_2_value(ie_ptr, 1);

                liblte_rrc_unpack_carrier_freqs_geran_ie(ie_ptr, &sib7->carrier_freqs_info_list[i].carrier_freqs);

                // Common Info
                {
                    // Optional indicators
                    sib7->carrier_freqs_info_list[i].cell_resel_prio_present = liblte_bits_2_value(ie_ptr, 1);
                    sib7->carrier_freqs_info_list[i].p_max_geran_present     = liblte_bits_2_value(ie_ptr, 1);

                    if(true == sib7->carrier_freqs_info_list[i].cell_resel_prio_present)
                    {
                        liblte_rrc_unpack_cell_reselection_priority_ie(ie_ptr, &sib7->carrier_freqs_info_list[i].cell_resel_prio);
                    }
                    sib7->carrier_freqs_info_list[i].ncc_permitted = liblte_bits_2_value(ie_ptr, 8);
                    sib7->carrier_freqs_info_list[i].q_rx_lev_min  = (liblte_bits_2_value(ie_ptr, 6) * 2) - 115;
                    if(true == sib7->carrier_freqs_info_list[i].p_max_geran_present)
                    {
                        sib7->carrier_freqs_info_list[i].p_max_geran = liblte_bits_2_value(ie_ptr, 6);
                    }
                    liblte_rrc_unpack_reselection_threshold_ie(ie_ptr, &sib7->carrier_freqs_info_list[i].threshx_high);
                    liblte_rrc_unpack_reselection_threshold_ie(ie_ptr, &sib7->carrier_freqs_info_list[i].threshx_low);
                }
            }
        }else{
            sib7->carrier_freqs_info_list_size = 0;
        }
        
        liblte_rrc_consume_noncrit_extension(ext_ind, __func__, ie_ptr);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: System Information Block Type 8

    Description: Contains information relevant only for inter-RAT
                 cell re-selection i.e. information about CDMA2000
                 frequencies and CDMA2000 neighboring cells relevant
                 for cell re-selection

    Document Reference: 36.331 v10.0.0 Section 6.3.1
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_rrc_pack_sys_info_block_type_8_ie(LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_8_STRUCT  *sib8,
                                                           uint8                                   **ie_ptr)
{
    LIBLTE_RRC_NEIGH_CELL_CDMA2000_STRUCT *neigh_cell_list;
    LIBLTE_ERROR_ENUM                      err = LIBLTE_ERROR_INVALID_INPUTS;
    uint32                                 i;
    uint32                                 j;
    uint32                                 k;

    if(sib8   != NULL &&
       ie_ptr != NULL)
    {
        // Extension indicator
        liblte_value_2_bits(0, ie_ptr, 1);

        // Optional indicators
        liblte_value_2_bits(sib8->sys_time_info_present,   ie_ptr, 1);
        liblte_value_2_bits(sib8->search_win_size_present, ie_ptr, 1);
        liblte_value_2_bits(sib8->params_hrpd_present,     ie_ptr, 1);
        liblte_value_2_bits(sib8->params_1xrtt_present,    ie_ptr, 1);

        if(true == sib8->sys_time_info_present)
        {
            liblte_rrc_pack_system_time_info_cdma2000_ie(&sib8->sys_time_info_cdma2000, ie_ptr);
        }

        if(true == sib8->search_win_size_present)
        {
            liblte_value_2_bits(sib8->search_win_size, ie_ptr, 4);
        }

        if(true == sib8->params_hrpd_present)
        {
            // Optional indicator
            liblte_value_2_bits(sib8->cell_resel_params_hrpd_present, ie_ptr, 1);

            liblte_rrc_pack_pre_registration_info_hrpd_ie(&sib8->pre_reg_info_hrpd, ie_ptr);

            if(true == sib8->cell_resel_params_hrpd_present)
            {
                // Optional indicator
                liblte_value_2_bits(sib8->cell_resel_params_hrpd.t_resel_cdma2000_sf_present, ie_ptr, 1);

                liblte_value_2_bits(sib8->cell_resel_params_hrpd.band_class_list_size - 1, ie_ptr, 5);
                for(i=0; i<sib8->cell_resel_params_hrpd.band_class_list_size; i++)
                {
                    // Extension indicator
                    liblte_value_2_bits(0, ie_ptr, 1);

                    // Optional indicator
                    liblte_value_2_bits(sib8->cell_resel_params_hrpd.band_class_list[i].cell_resel_prio_present, ie_ptr, 1);

                    liblte_rrc_pack_band_class_cdma2000_ie(sib8->cell_resel_params_hrpd.band_class_list[i].band_class, ie_ptr);
                    if(true == sib8->cell_resel_params_hrpd.band_class_list[i].cell_resel_prio_present)
                    {
                        liblte_rrc_pack_cell_reselection_priority_ie(sib8->cell_resel_params_hrpd.band_class_list[i].cell_resel_prio, ie_ptr);
                    }
                    liblte_value_2_bits(sib8->cell_resel_params_hrpd.band_class_list[i].thresh_x_high, ie_ptr, 6);
                    liblte_value_2_bits(sib8->cell_resel_params_hrpd.band_class_list[i].thresh_x_low,  ie_ptr, 6);
                }

                liblte_value_2_bits(sib8->cell_resel_params_hrpd.neigh_cell_list_size - 1, ie_ptr, 4);
                for(i=0; i<sib8->cell_resel_params_hrpd.neigh_cell_list_size; i++)
                {
                    neigh_cell_list = &sib8->cell_resel_params_hrpd.neigh_cell_list[i];
                    liblte_rrc_pack_band_class_cdma2000_ie(neigh_cell_list->band_class, ie_ptr);
                    liblte_value_2_bits(neigh_cell_list->neigh_cells_per_freq_list_size - 1, ie_ptr, 4);
                    for(j=0; j<neigh_cell_list->neigh_cells_per_freq_list_size; j++)
                    {
                        liblte_rrc_pack_arfcn_value_cdma2000_ie(neigh_cell_list->neigh_cells_per_freq_list[j].arfcn, ie_ptr);
                        liblte_value_2_bits(neigh_cell_list->neigh_cells_per_freq_list[j].phys_cell_id_list_size - 1, ie_ptr, 4);
                        for(k=0; k<neigh_cell_list->neigh_cells_per_freq_list[j].phys_cell_id_list_size; k++)
                        {
                            liblte_rrc_pack_phys_cell_id_cdma2000_ie(neigh_cell_list->neigh_cells_per_freq_list[j].phys_cell_id_list[k], ie_ptr);
                        }
                    }
                }
                liblte_rrc_pack_t_reselection_ie(sib8->cell_resel_params_hrpd.t_resel_cdma2000, ie_ptr);

                if(true == sib8->cell_resel_params_hrpd.t_resel_cdma2000_sf_present)
                {
                    liblte_rrc_pack_speed_state_scale_factors_ie(&sib8->cell_resel_params_hrpd.t_resel_cdma2000_sf, ie_ptr);
                }
            }
        }

        if(true == sib8->params_1xrtt_present)
        {
            // Optional indicators
            liblte_value_2_bits(sib8->csfb_reg_param_1xrtt_present,    ie_ptr, 1);
            liblte_value_2_bits(sib8->long_code_state_1xrtt_present,   ie_ptr, 1);
            liblte_value_2_bits(sib8->cell_resel_params_1xrtt_present, ie_ptr, 1);

            if(true == sib8->csfb_reg_param_1xrtt_present)
            {
                liblte_rrc_pack_csfb_registration_param_1xrtt_ie(&sib8->csfb_reg_param_1xrtt, ie_ptr);
            }

            if(true == sib8->long_code_state_1xrtt_present)
            {
                liblte_value_2_bits((uint32)(sib8->long_code_state_1xrtt >> 10),   ie_ptr, 32);
                liblte_value_2_bits((uint32)(sib8->long_code_state_1xrtt & 0x3FF), ie_ptr, 10);
            }

            if(true == sib8->cell_resel_params_1xrtt_present)
            {
                // Optional indicator
                liblte_value_2_bits(sib8->cell_resel_params_1xrtt.t_resel_cdma2000_sf_present, ie_ptr, 1);

                liblte_value_2_bits(sib8->cell_resel_params_1xrtt.band_class_list_size - 1, ie_ptr, 5);
                for(i=0; i<sib8->cell_resel_params_1xrtt.band_class_list_size; i++)
                {
                    // Extension indicator
                    liblte_value_2_bits(0, ie_ptr, 1);

                    // Optional indicator
                    liblte_value_2_bits(sib8->cell_resel_params_1xrtt.band_class_list[i].cell_resel_prio_present, ie_ptr, 1);

                    liblte_rrc_pack_band_class_cdma2000_ie(sib8->cell_resel_params_1xrtt.band_class_list[i].band_class, ie_ptr);
                    if(true == sib8->cell_resel_params_1xrtt.band_class_list[i].cell_resel_prio_present)
                    {
                        liblte_rrc_pack_cell_reselection_priority_ie(sib8->cell_resel_params_1xrtt.band_class_list[i].cell_resel_prio, ie_ptr);
                    }
                    liblte_value_2_bits(sib8->cell_resel_params_1xrtt.band_class_list[i].thresh_x_high, ie_ptr, 6);
                    liblte_value_2_bits(sib8->cell_resel_params_1xrtt.band_class_list[i].thresh_x_low,  ie_ptr, 6);
                }

                liblte_value_2_bits(sib8->cell_resel_params_1xrtt.neigh_cell_list_size - 1, ie_ptr, 4);
                for(i=0; i<sib8->cell_resel_params_1xrtt.neigh_cell_list_size; i++)
                {
                    neigh_cell_list = &sib8->cell_resel_params_1xrtt.neigh_cell_list[i];
                    liblte_rrc_pack_band_class_cdma2000_ie(neigh_cell_list->band_class, ie_ptr);
                    liblte_value_2_bits(neigh_cell_list->neigh_cells_per_freq_list_size - 1, ie_ptr, 4);
                    for(j=0; j<neigh_cell_list->neigh_cells_per_freq_list_size; j++)
                    {
                        liblte_rrc_pack_arfcn_value_cdma2000_ie(neigh_cell_list->neigh_cells_per_freq_list[j].arfcn, ie_ptr);
                        liblte_value_2_bits(neigh_cell_list->neigh_cells_per_freq_list[j].phys_cell_id_list_size - 1, ie_ptr, 4);
                        for(k=0; k<neigh_cell_list->neigh_cells_per_freq_list[j].phys_cell_id_list_size; k++)
                        {
                            liblte_rrc_pack_phys_cell_id_cdma2000_ie(neigh_cell_list->neigh_cells_per_freq_list[j].phys_cell_id_list[k], ie_ptr);
                        }
                    }
                }
                liblte_rrc_pack_t_reselection_ie(sib8->cell_resel_params_1xrtt.t_resel_cdma2000, ie_ptr);

                if(true == sib8->cell_resel_params_1xrtt.t_resel_cdma2000_sf_present)
                {
                    liblte_rrc_pack_speed_state_scale_factors_ie(&sib8->cell_resel_params_1xrtt.t_resel_cdma2000_sf, ie_ptr);
                }
            }
        }

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_unpack_sys_info_block_type_8_ie(uint8                                   **ie_ptr,
                                                             LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_8_STRUCT  *sib8)
{
    LIBLTE_RRC_NEIGH_CELL_CDMA2000_STRUCT *neigh_cell_list;
    LIBLTE_ERROR_ENUM                      err = LIBLTE_ERROR_INVALID_INPUTS;
    uint32                                 i;
    uint32                                 j;
    uint32                                 k;
    bool                                   ext_ind;

    if(ie_ptr != NULL &&
       sib8   != NULL)
    {
        // Extension indicator
        ext_ind = liblte_bits_2_value(ie_ptr, 1);

        // Optional indicators
        sib8->sys_time_info_present   = liblte_bits_2_value(ie_ptr, 1);
        sib8->search_win_size_present = liblte_bits_2_value(ie_ptr, 1);
        sib8->params_hrpd_present     = liblte_bits_2_value(ie_ptr, 1);
        sib8->params_1xrtt_present    = liblte_bits_2_value(ie_ptr, 1);

        if(true == sib8->sys_time_info_present)
        {
            liblte_rrc_unpack_system_time_info_cdma2000_ie(ie_ptr, &sib8->sys_time_info_cdma2000);
        }

        if(true == sib8->search_win_size_present)
        {
            sib8->search_win_size = liblte_bits_2_value(ie_ptr, 4);
        }

        if(true == sib8->params_hrpd_present)
        {
            // Optional indicator
            sib8->cell_resel_params_hrpd_present = liblte_bits_2_value(ie_ptr, 1);

            liblte_rrc_unpack_pre_registration_info_hrpd_ie(ie_ptr, &sib8->pre_reg_info_hrpd);

            if(true == sib8->cell_resel_params_hrpd_present)
            {
                // Optional indicator
                sib8->cell_resel_params_hrpd.t_resel_cdma2000_sf_present = liblte_bits_2_value(ie_ptr, 1);

                sib8->cell_resel_params_hrpd.band_class_list_size = liblte_bits_2_value(ie_ptr, 5) + 1;
                for(i=0; i<sib8->cell_resel_params_hrpd.band_class_list_size; i++)
                {
                    // Extension indicator
                    bool ext = liblte_bits_2_value(ie_ptr, 1);

                    // Optional indicator
                    sib8->cell_resel_params_hrpd.band_class_list[i].cell_resel_prio_present = liblte_bits_2_value(ie_ptr, 1);

                    liblte_rrc_unpack_band_class_cdma2000_ie(ie_ptr, &sib8->cell_resel_params_hrpd.band_class_list[i].band_class);
                    if(true == sib8->cell_resel_params_hrpd.band_class_list[i].cell_resel_prio_present)
                    {
                        liblte_rrc_unpack_cell_reselection_priority_ie(ie_ptr, &sib8->cell_resel_params_hrpd.band_class_list[i].cell_resel_prio);
                    }
                    sib8->cell_resel_params_hrpd.band_class_list[i].thresh_x_high = liblte_bits_2_value(ie_ptr, 6);
                    sib8->cell_resel_params_hrpd.band_class_list[i].thresh_x_low  = liblte_bits_2_value(ie_ptr, 6);
                    
                    liblte_rrc_consume_noncrit_extension(ext, __func__, ie_ptr);
                    
                }

                sib8->cell_resel_params_hrpd.neigh_cell_list_size = liblte_bits_2_value(ie_ptr, 4) + 1;
                for(i=0; i<sib8->cell_resel_params_hrpd.neigh_cell_list_size; i++)
                {
                    neigh_cell_list = &sib8->cell_resel_params_hrpd.neigh_cell_list[i];
                    liblte_rrc_unpack_band_class_cdma2000_ie(ie_ptr, &neigh_cell_list->band_class);
                    neigh_cell_list->neigh_cells_per_freq_list_size = liblte_bits_2_value(ie_ptr, 4) + 1;
                    for(j=0; j<neigh_cell_list->neigh_cells_per_freq_list_size; j++)
                    {
                        liblte_rrc_unpack_arfcn_value_cdma2000_ie(ie_ptr, &neigh_cell_list->neigh_cells_per_freq_list[j].arfcn);
                        neigh_cell_list->neigh_cells_per_freq_list[j].phys_cell_id_list_size = liblte_bits_2_value(ie_ptr, 4) + 1;
                        for(k=0; k<neigh_cell_list->neigh_cells_per_freq_list[j].phys_cell_id_list_size; k++)
                        {
                            liblte_rrc_unpack_phys_cell_id_cdma2000_ie(ie_ptr, &neigh_cell_list->neigh_cells_per_freq_list[j].phys_cell_id_list[k]);
                        }
                    }
                }
                liblte_rrc_unpack_t_reselection_ie(ie_ptr, &sib8->cell_resel_params_hrpd.t_resel_cdma2000);

                if(true == sib8->cell_resel_params_hrpd.t_resel_cdma2000_sf_present)
                {
                    liblte_rrc_unpack_speed_state_scale_factors_ie(ie_ptr, &sib8->cell_resel_params_hrpd.t_resel_cdma2000_sf);
                }
            }
        }else{
            sib8->cell_resel_params_hrpd_present = false;
        }

        if(true == sib8->params_1xrtt_present)
        {
            // Optional indicators
            sib8->csfb_reg_param_1xrtt_present    = liblte_bits_2_value(ie_ptr, 1);
            sib8->long_code_state_1xrtt_present   = liblte_bits_2_value(ie_ptr, 1);
            sib8->cell_resel_params_1xrtt_present = liblte_bits_2_value(ie_ptr, 1);

            if(true == sib8->csfb_reg_param_1xrtt_present)
            {
                liblte_rrc_unpack_csfb_registration_param_1xrtt_ie(ie_ptr, &sib8->csfb_reg_param_1xrtt);
            }

            if(true == sib8->long_code_state_1xrtt_present)
            {
                sib8->long_code_state_1xrtt  = (uint64)liblte_bits_2_value(ie_ptr, 32) << 10;
                sib8->long_code_state_1xrtt |= (uint64)liblte_bits_2_value(ie_ptr, 10);
            }

            if(true == sib8->cell_resel_params_1xrtt_present)
            {
                // Optional indicator
                sib8->cell_resel_params_1xrtt.t_resel_cdma2000_sf_present = liblte_bits_2_value(ie_ptr, 1);

                sib8->cell_resel_params_1xrtt.band_class_list_size = liblte_bits_2_value(ie_ptr, 5) + 1;
                for(i=0; i<sib8->cell_resel_params_1xrtt.band_class_list_size; i++)
                {
                    // Extension indicator
                    bool ext2 = liblte_bits_2_value(ie_ptr, 1);

                    // Optional indicator
                    sib8->cell_resel_params_1xrtt.band_class_list[i].cell_resel_prio_present = liblte_bits_2_value(ie_ptr, 1);

                    liblte_rrc_unpack_band_class_cdma2000_ie(ie_ptr, &sib8->cell_resel_params_1xrtt.band_class_list[i].band_class);
                    if(true == sib8->cell_resel_params_1xrtt.band_class_list[i].cell_resel_prio_present)
                    {
                        liblte_rrc_unpack_cell_reselection_priority_ie(ie_ptr, &sib8->cell_resel_params_1xrtt.band_class_list[i].cell_resel_prio);
                    }
                    sib8->cell_resel_params_1xrtt.band_class_list[i].thresh_x_high = liblte_bits_2_value(ie_ptr, 6);
                    sib8->cell_resel_params_1xrtt.band_class_list[i].thresh_x_low  = liblte_bits_2_value(ie_ptr, 6);
                    
                    liblte_rrc_consume_noncrit_extension(ext2, __func__, ie_ptr);
                }

                sib8->cell_resel_params_1xrtt.neigh_cell_list_size = liblte_bits_2_value(ie_ptr, 4) + 1;
                for(i=0; i<sib8->cell_resel_params_1xrtt.neigh_cell_list_size; i++)
                {
                    neigh_cell_list = &sib8->cell_resel_params_1xrtt.neigh_cell_list[i];
                    liblte_rrc_unpack_band_class_cdma2000_ie(ie_ptr, &neigh_cell_list->band_class);
                    neigh_cell_list->neigh_cells_per_freq_list_size = liblte_bits_2_value(ie_ptr, 4) + 1;
                    for(j=0; j<neigh_cell_list->neigh_cells_per_freq_list_size; j++)
                    {
                        liblte_rrc_unpack_arfcn_value_cdma2000_ie(ie_ptr, &neigh_cell_list->neigh_cells_per_freq_list[j].arfcn);
                        neigh_cell_list->neigh_cells_per_freq_list[j].phys_cell_id_list_size = liblte_bits_2_value(ie_ptr, 4) + 1;
                        for(k=0; k<neigh_cell_list->neigh_cells_per_freq_list[j].phys_cell_id_list_size; k++)
                        {
                            liblte_rrc_unpack_phys_cell_id_cdma2000_ie(ie_ptr, &neigh_cell_list->neigh_cells_per_freq_list[j].phys_cell_id_list[k]);
                        }
                    }
                }
                liblte_rrc_unpack_t_reselection_ie(ie_ptr, &sib8->cell_resel_params_1xrtt.t_resel_cdma2000);

                if(true == sib8->cell_resel_params_1xrtt.t_resel_cdma2000_sf_present)
                {
                    liblte_rrc_unpack_speed_state_scale_factors_ie(ie_ptr, &sib8->cell_resel_params_1xrtt.t_resel_cdma2000_sf);
                }
            }
        }else{
            sib8->csfb_reg_param_1xrtt_present    = false;
            sib8->long_code_state_1xrtt_present   = false;
            sib8->cell_resel_params_1xrtt_present = false;
        }

        liblte_rrc_consume_noncrit_extension(ext_ind, __func__, ie_ptr);
        
        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: System Information Block Type 9

    Description: Contains a home eNB name (HNB name)

    Document Reference: 36.331 v10.0.0 Section 6.3.1
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_rrc_pack_sys_info_block_type_9_ie(LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_9_STRUCT  *sib9,
                                                           uint8                                   **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;
    uint32            i;

    if(sib9   != NULL &&
       ie_ptr != NULL)
    {
        // Extension indicator
        liblte_value_2_bits(0, ie_ptr, 1);

        // Optional indicators
        liblte_value_2_bits(sib9->hnb_name_present,   ie_ptr, 1);

        if(true == sib9->hnb_name_present) {
            // Dynamic octet string - hnb_name
            liblte_value_2_bits(sib9->hnb_name_size - 1, ie_ptr, 6);

            // Octets
            for(i=0;i<sib9->hnb_name_size;i++) {
              liblte_value_2_bits(sib9->hnb_name[i], ie_ptr, 8);
            }
        }

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_unpack_sys_info_block_type_9_ie(uint8                                   **ie_ptr,
                                                             LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_9_STRUCT  *sib9)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;
    bool              ext_ind;
    uint32            i;

    if(ie_ptr != NULL &&
       sib9   != NULL)
    {
        // Extension indicator
        ext_ind = liblte_bits_2_value(ie_ptr, 1);

        // Optional indicators
        sib9->hnb_name_present   = liblte_bits_2_value(ie_ptr, 1);

        if(true == sib9->hnb_name_present) {
            // Dynamic octet string - hnb_name
            // Length
            sib9->hnb_name_size = liblte_bits_2_value(ie_ptr, 6) + 1;

            // Octets
            for(i=0;i<sib9->hnb_name_size;i++) {
              sib9->hnb_name[i] = liblte_bits_2_value(ie_ptr, 8);
            }
        }

        liblte_rrc_consume_noncrit_extension(ext_ind, __func__, ie_ptr);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: System Information Block Type 10

    Description: Contains an ETWS primary notification

    Document Reference: 36.331 v10.0.0 Section 6.3.1
*********************************************************************/
// FIXME

/*********************************************************************
    IE Name: System Information Block Type 11

    Description: Contains an ETWS secondary notification

    Document Reference: 36.331 v10.0.0 Section 6.3.1
*********************************************************************/
// FIXME

/*********************************************************************
    IE Name: System Information Block Type 12

    Description: Contains a CMAS notification

    Document Reference: 36.331 v10.0.0 Section 6.3.1
*********************************************************************/
// FIXME

/*********************************************************************
    IE Name: System Information Block Type 13

    Description: Contains the information required to acquire the
                 MBMS control information associated with one or more
                 MBSFN areas

    Document Reference: 36.331 v10.0.0 Section 6.3.1
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_rrc_pack_sys_info_block_type_13_ie(LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_13_STRUCT  *sib13,
                                                            uint8                                    **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;
    uint32            i;

    if(sib13  != NULL &&
       ie_ptr != NULL)
    {
        // Extension indicator
        liblte_value_2_bits(0, ie_ptr, 1);

        // Optional indicators
        liblte_value_2_bits(0, ie_ptr, 1);

        liblte_value_2_bits(sib13->mbsfn_area_info_list_r9_size - 1, ie_ptr, 3);
        for(i=0; i<sib13->mbsfn_area_info_list_r9_size; i++)
        {
            liblte_rrc_pack_mbsfn_area_info_ie(&sib13->mbsfn_area_info_list_r9[i], ie_ptr);
        }
        liblte_rrc_pack_mbsfn_notification_config_ie(&sib13->mbsfn_notification_config, ie_ptr);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_unpack_sys_info_block_type_13_ie(uint8                                    **ie_ptr,
                                                              LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_13_STRUCT  *sib13)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;
    uint32            i;
    bool              ext_ind;
    bool              non_crit_ext_present;

    if(ie_ptr != NULL &&
       sib13  != NULL)
    {
        // Extension indicator
        ext_ind = liblte_bits_2_value(ie_ptr, 1);

        // Optional indicators
        non_crit_ext_present = liblte_bits_2_value(ie_ptr, 1);

        sib13->mbsfn_area_info_list_r9_size = liblte_bits_2_value(ie_ptr, 3) + 1;
        for(i=0; i<sib13->mbsfn_area_info_list_r9_size; i++)
        {
            liblte_rrc_unpack_mbsfn_area_info_ie(ie_ptr, &sib13->mbsfn_area_info_list_r9[i]);
        }
        liblte_rrc_unpack_mbsfn_notification_config_ie(ie_ptr, &sib13->mbsfn_notification_config);

        liblte_rrc_consume_noncrit_extension(ext_ind, __func__, ie_ptr);
        
        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*******************************************************************************
                              MESSAGE FUNCTIONS
*******************************************************************************/

/*********************************************************************
    Message Name: UL Information Transfer

    Description: Used for the uplink transfer dedicated NAS
                 information

    Document Reference: 36.331 v10.0.0 Section 6.2.2 
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_rrc_pack_ul_information_transfer_msg(LIBLTE_RRC_UL_INFORMATION_TRANSFER_STRUCT *ul_info_transfer,
                                                              LIBLTE_BIT_MSG_STRUCT                     *msg)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr;

    if(ul_info_transfer != NULL &&
       msg              != NULL)
    {
        msg_ptr = msg->msg;

        // Extension choice
        liblte_value_2_bits(0, &msg_ptr, 1);

        // C1 choice
        liblte_value_2_bits(0, &msg_ptr, 2);

        // Optional indicator
        liblte_value_2_bits(0, &msg_ptr, 1);

        // Dedicated info type choice
        liblte_value_2_bits(ul_info_transfer->dedicated_info_type, &msg_ptr, 2);

        if(LIBLTE_RRC_UL_INFORMATION_TRANSFER_TYPE_NAS == ul_info_transfer->dedicated_info_type)
        {
            liblte_rrc_pack_dedicated_info_nas_ie(&ul_info_transfer->dedicated_info,
                                                  &msg_ptr);
        }else{
            liblte_rrc_pack_dedicated_info_cdma2000_ie(&ul_info_transfer->dedicated_info,
                                                       &msg_ptr);
        }

        // Fill in the number of bits used
        msg->N_bits = msg_ptr - msg->msg;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_unpack_ul_information_transfer_msg(LIBLTE_BIT_MSG_STRUCT                     *msg,
                                                                LIBLTE_RRC_UL_INFORMATION_TRANSFER_STRUCT *ul_info_transfer)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr;

    if(msg              != NULL &&
       ul_info_transfer != NULL)
    {
        msg_ptr = msg->msg;

        // Extension choice
        bool ext = liblte_bits_2_value(&msg_ptr, 1);

        // C1 choice
        liblte_bits_2_value(&msg_ptr, 2);

        // Optional indicator
        liblte_rrc_warning_not_handled(liblte_bits_2_value(&msg_ptr, 1), __func__);

        // Dedicated info type choice
        ul_info_transfer->dedicated_info_type = (LIBLTE_RRC_UL_INFORMATION_TRANSFER_TYPE_ENUM)liblte_bits_2_value(&msg_ptr, 2);

        if(LIBLTE_RRC_UL_INFORMATION_TRANSFER_TYPE_NAS == ul_info_transfer->dedicated_info_type)
        {
            liblte_rrc_unpack_dedicated_info_nas_ie(&msg_ptr,
                                                    &ul_info_transfer->dedicated_info);
        }else{
            liblte_rrc_unpack_dedicated_info_cdma2000_ie(&msg_ptr,
                                                         &ul_info_transfer->dedicated_info);
        }

        liblte_rrc_consume_noncrit_extension(ext, __func__, &msg_ptr);
        
        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    Message Name: UL Handover Preparation Transfer (CDMA2000)

    Description: Used for the uplink transfer of handover related
                 CDMA2000 information when requested by the higher
                 layers

    Document Reference: 36.331 v10.0.0 Section 6.2.2 
*********************************************************************/
// FIXME

/*********************************************************************
    Message Name: UE Information Response

    Description: Used by the UE to transfer the information requested
                 by the E-UTRAN

    Document Reference: 36.331 v10.0.0 Section 6.2.2 
*********************************************************************/
// FIXME

/*********************************************************************
    Message Name: UE Information Request

    Description: Used by E-UTRAN to retrieve information from the UE

    Document Reference: 36.331 v10.0.0 Section 6.2.2 
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_rrc_pack_ue_information_request_msg(LIBLTE_RRC_UE_INFORMATION_REQUEST_STRUCT *ue_info_req,
                                                             LIBLTE_BIT_MSG_STRUCT                    *msg)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr;

    if(ue_info_req != NULL &&
       msg         != NULL)
    {
        msg_ptr = msg->msg;

        // RRC Transaction ID
        liblte_rrc_pack_rrc_transaction_identifier_ie(ue_info_req->rrc_transaction_id,
                                                      &msg_ptr);

        // Extension choice
        liblte_value_2_bits(0, &msg_ptr, 1);

        // C1 choice
        liblte_value_2_bits(0, &msg_ptr, 2);

        // Optional indicator
        liblte_value_2_bits(0, &msg_ptr, 1);

        // RACH report required
        liblte_value_2_bits(ue_info_req->rach_report_req, &msg_ptr, 1);

        // RLF report required
        liblte_value_2_bits(ue_info_req->rlf_report_req, &msg_ptr, 1);

        // Fill in the number of bits used
        msg->N_bits = msg_ptr - msg->msg;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_unpack_ue_information_request_msg(LIBLTE_BIT_MSG_STRUCT                    *msg,
                                                               LIBLTE_RRC_UE_INFORMATION_REQUEST_STRUCT *ue_info_req)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr;

    if(msg         != NULL &&
       ue_info_req != NULL)
    {
        msg_ptr = msg->msg;

        // RRC Transaction ID
        liblte_rrc_unpack_rrc_transaction_identifier_ie(&msg_ptr,
                                                        &ue_info_req->rrc_transaction_id);

        // Extension choice
        bool ext = liblte_bits_2_value(&msg_ptr, 1);

        // C1 choice
        liblte_bits_2_value(&msg_ptr, 2);

        // Optional indicator
        liblte_rrc_warning_not_handled(liblte_bits_2_value(&msg_ptr, 1), __func__);

        // RACH report required
        ue_info_req->rach_report_req = liblte_bits_2_value(&msg_ptr, 1);

        // RLF report required
        ue_info_req->rlf_report_req = liblte_bits_2_value(&msg_ptr, 1);
        
        liblte_rrc_consume_noncrit_extension(ext, __func__, &msg_ptr);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    Message Name: UE Capability Information

    Description: Used to transfer UE radio access capabilities
                 requested by the E-UTRAN

    Document Reference: 36.331 v10.0.0 Section 6.2.2 
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_rrc_pack_ue_capability_information_msg(LIBLTE_RRC_UE_CAPABILITY_INFORMATION_STRUCT *ue_capability_info,
                                                                LIBLTE_BIT_MSG_STRUCT                       *msg)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr;
    uint32             i;

    if(ue_capability_info != NULL &&
       msg                != NULL)
    {
        msg_ptr = msg->msg;

        // RRC Transaction ID
        liblte_rrc_pack_rrc_transaction_identifier_ie(ue_capability_info->rrc_transaction_id,
                                                      &msg_ptr);

        // Extension choice
        liblte_value_2_bits(0, &msg_ptr, 1);

        // C1 choice
        liblte_value_2_bits(0, &msg_ptr, 3);

        // Optional indicator
        liblte_value_2_bits(0, &msg_ptr, 1);

        liblte_value_2_bits(ue_capability_info->N_ue_caps, &msg_ptr, 4);
        for(i=0; i<ue_capability_info->N_ue_caps; i++)
        {
            // RAT-Type
            liblte_value_2_bits(0, &msg_ptr, 1); //Optional indicator
            liblte_value_2_bits(ue_capability_info->ue_capability_rat[i].rat_type, &msg_ptr, 3);

            // Octet string
            LIBLTE_BIT_MSG_STRUCT  tmp;
            liblte_rrc_pack_ue_eutra_capability_ie(&ue_capability_info->ue_capability_rat[i].eutra_capability, &tmp);

            uint32 pad     = 8 - tmp.N_bits%8;
            uint32 n_bytes = (tmp.N_bits+pad) / 8;
            if(n_bytes < 128)
            {
                liblte_value_2_bits(0,       &msg_ptr, 1);
                liblte_value_2_bits(n_bytes, &msg_ptr, 7);
            }else if(n_bytes < 16383){
                liblte_value_2_bits(1,       &msg_ptr, 1);
                liblte_value_2_bits(0,       &msg_ptr, 1);
                liblte_value_2_bits(n_bytes, &msg_ptr, 14);
            }else{
                // FIXME: Unlikely to have more than 16K of octets
            }

            for(i=0; i<tmp.N_bits; i++)
            {
                *msg_ptr = tmp.msg[i];
                msg_ptr++;
            }

            msg_ptr += pad;
        }

        // Fill in the number of bits used
        msg->N_bits = msg_ptr - msg->msg;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_unpack_ue_capability_information_msg(LIBLTE_BIT_MSG_STRUCT                       *msg,
                                                                  LIBLTE_RRC_UE_CAPABILITY_INFORMATION_STRUCT *ue_capability_info)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr;
    uint32             i;

    if(msg                != NULL &&
       ue_capability_info != NULL)
    {
        msg_ptr = msg->msg;

        // RRC Transaction ID
        liblte_rrc_unpack_rrc_transaction_identifier_ie(&msg_ptr,
                                                        &ue_capability_info->rrc_transaction_id);

        // Extension choice
        bool ext = liblte_bits_2_value(&msg_ptr, 1);

        // C1 choice
        liblte_bits_2_value(&msg_ptr, 3);

        // Optional indicator
        liblte_rrc_warning_not_handled(liblte_bits_2_value(&msg_ptr, 1), __func__);

        ue_capability_info->N_ue_caps = liblte_bits_2_value(&msg_ptr, 4);
        for(i=0; i<ue_capability_info->N_ue_caps; i++)
        {
            liblte_bits_2_value(&msg_ptr, 1); //Optional indicator
            ue_capability_info->ue_capability_rat[i].rat_type = (LIBLTE_RRC_RAT_TYPE_ENUM)liblte_bits_2_value(&msg_ptr, 3);

            //Octet string
            uint32 n_bytes = 0;
            if(0 == liblte_bits_2_value(&msg_ptr, 1))
            {
                n_bytes = liblte_bits_2_value(&msg_ptr, 7);
            }else{
                if(0 == liblte_bits_2_value(&msg_ptr, 1))
                {
                    n_bytes = liblte_bits_2_value(&msg_ptr, 14);
                }else{
                    // FIXME: Unlikely to have more than 16K of octets
                    n_bytes = 0;
                }
            }

            liblte_rrc_unpack_ue_eutra_capability_ie(&msg_ptr, &ue_capability_info->ue_capability_rat[i].eutra_capability);
        }
        
        liblte_rrc_consume_noncrit_extension(ext, __func__, &msg_ptr);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}


/*********************************************************************
    Message Name: UE Capability Enquiry

    Description: Used to request the transfer of UE radio access
                 capabilities for E-UTRA as well as for other RATs

    Document Reference: 36.331 v10.0.0 Section 6.2.2 
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_rrc_pack_ue_capability_enquiry_msg(LIBLTE_RRC_UE_CAPABILITY_ENQUIRY_STRUCT *ue_cap_enquiry,
                                                            LIBLTE_BIT_MSG_STRUCT                   *msg)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr;
    uint32             i;

    if(ue_cap_enquiry != NULL &&
       msg            != NULL)
    {
        msg_ptr = msg->msg;

        // RRC Transaction ID
        liblte_rrc_pack_rrc_transaction_identifier_ie(ue_cap_enquiry->rrc_transaction_id,
                                                      &msg_ptr);

        // Extension choice
        liblte_value_2_bits(0, &msg_ptr, 1);

        // C1 choice
        liblte_value_2_bits(0, &msg_ptr, 2);

        // Optional indicator
        liblte_value_2_bits(0, &msg_ptr, 1);

        // UE-CapabilityRequest
        liblte_value_2_bits(ue_cap_enquiry->N_ue_cap_reqs - 1, &msg_ptr, 3);
        for(i=0; i<LIBLTE_RRC_MAX_RAT_CAPABILITIES; i++)
        {
            liblte_rrc_pack_rat_type_ie(ue_cap_enquiry->ue_capability_request[i], &msg_ptr);
        }

        // Fill in the number of bits used
        msg->N_bits = msg_ptr - msg->msg;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_unpack_ue_capability_enquiry_msg(LIBLTE_BIT_MSG_STRUCT                   *msg,
                                                              LIBLTE_RRC_UE_CAPABILITY_ENQUIRY_STRUCT *ue_cap_enquiry)
{
  LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
  uint8             *msg_ptr;
  uint32             i;

  if(msg         != NULL &&
     ue_cap_enquiry != NULL)
  {
        msg_ptr = msg->msg;

      // RRC Transaction ID
      liblte_rrc_unpack_rrc_transaction_identifier_ie(&msg_ptr,
                                                      &ue_cap_enquiry->rrc_transaction_id);

      // Extension choice
      bool ext = liblte_bits_2_value(&msg_ptr, 1);

      // C1 choice
      liblte_bits_2_value(&msg_ptr, 2);

      // Optional indicator
      liblte_rrc_warning_not_handled(liblte_bits_2_value(&msg_ptr, 1), __func__);

      ue_cap_enquiry->N_ue_cap_reqs = liblte_bits_2_value(&msg_ptr, 3) + 1;
      for(i=0; i<ue_cap_enquiry->N_ue_cap_reqs; i++)
      {
        liblte_rrc_unpack_rat_type_ie(&msg_ptr, &ue_cap_enquiry->ue_capability_request[i]);
      }
      
      liblte_rrc_consume_noncrit_extension(ext, __func__, &msg_ptr);

      err = LIBLTE_SUCCESS;
  }

  return(err);
}


/*********************************************************************
    Message Name: System Information Block Type 1

    Description: Contains information relevant when evaluating if a
                 UE is allowed to access a cell and defines the
                 scheduling of other system information

    Document Reference: 36.331 v10.0.0 Section 6.2.2
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_rrc_pack_sys_info_block_type_1_msg(LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_1_STRUCT *sib1,
                                                            LIBLTE_BIT_MSG_STRUCT                   *msg)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr;
    uint32             i;
    uint32             j;
    uint8              non_crit_ext_opt        = false;
    uint8              csg_id_opt              = false;
    uint8              q_rx_lev_min_offset_opt = false;
    uint8              extension               = false;

    if(sib1 != NULL &&
       msg  != NULL)
    {
        msg_ptr = msg->msg;

        // Optional indicators
        liblte_value_2_bits(sib1->p_max_present, &msg_ptr, 1);
        liblte_value_2_bits(sib1->tdd,           &msg_ptr, 1);
        liblte_value_2_bits(non_crit_ext_opt,    &msg_ptr, 1);

        // Cell Access Related Info
        liblte_value_2_bits(csg_id_opt,           &msg_ptr, 1);
        liblte_value_2_bits(sib1->N_plmn_ids - 1, &msg_ptr, 3);
        for(i=0; i<sib1->N_plmn_ids; i++)
        {
            liblte_rrc_pack_plmn_identity_ie(&sib1->plmn_id[i].id, &msg_ptr);
            liblte_value_2_bits(sib1->plmn_id[i].resv_for_oper, &msg_ptr, 1);
        }
        liblte_rrc_pack_tracking_area_code_ie(sib1->tracking_area_code, &msg_ptr);
        liblte_rrc_pack_cell_identity_ie(sib1->cell_id, &msg_ptr);
        liblte_value_2_bits(sib1->cell_barred,            &msg_ptr, 1);
        liblte_value_2_bits(sib1->intra_freq_reselection, &msg_ptr, 1);
        liblte_value_2_bits(sib1->csg_indication,         &msg_ptr, 1);
        if(true == csg_id_opt)
        {
            liblte_rrc_pack_csg_identity_ie(sib1->csg_id, &msg_ptr);
        }

        // Cell Selection Info
        liblte_value_2_bits(q_rx_lev_min_offset_opt, &msg_ptr, 1);
        liblte_rrc_pack_q_rx_lev_min_ie(sib1->q_rx_lev_min, &msg_ptr);
        if(true == q_rx_lev_min_offset_opt)
        {
            liblte_value_2_bits((sib1->q_rx_lev_min_offset / 2) - 1, &msg_ptr, 3);
        }

        // P Max
        if(true == sib1->p_max_present)
        {
            liblte_rrc_pack_p_max_ie(sib1->p_max, &msg_ptr);
        }

        // Freq Band Indicator
        liblte_value_2_bits(sib1->freq_band_indicator - 1, &msg_ptr, 6);

        // Scheduling Info List
        liblte_value_2_bits(sib1->N_sched_info - 1, &msg_ptr, 5);
        for(i=0; i<sib1->N_sched_info; i++)
        {
            liblte_value_2_bits(sib1->sched_info[i].si_periodicity,     &msg_ptr, 3);
            liblte_value_2_bits(sib1->sched_info[i].N_sib_mapping_info, &msg_ptr, 5);
            for(j=0; j<sib1->sched_info[i].N_sib_mapping_info; j++)
            {
                liblte_value_2_bits(extension,                                        &msg_ptr, 1);
                liblte_value_2_bits(sib1->sched_info[i].sib_mapping_info[j].sib_type, &msg_ptr, 4);
            }
        }

        // TDD Config
        if(true == sib1->tdd)
        {
            liblte_rrc_pack_tdd_config_ie(&sib1->tdd_cnfg, &msg_ptr);
        }

        // SI Window Length
        liblte_value_2_bits(sib1->si_window_length, &msg_ptr, 3);

        // System Info Value Tag
        liblte_value_2_bits(sib1->system_info_value_tag, &msg_ptr, 5);

        // Non Critical Extension
        // FIXME

        // Fill in the number of bits used
        msg->N_bits = msg_ptr - msg->msg;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_unpack_sys_info_block_type_1_msg(LIBLTE_BIT_MSG_STRUCT                   *msg,
                                                              LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_1_STRUCT *sib1,
                                                              uint32                                  *N_bits_used)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr;
    uint32             i;
    uint32             j;
    bool               tdd_config_opt;
    bool               non_crit_ext_opt;
    bool               csg_id_opt;
    bool               q_rx_lev_min_offset_opt;
    bool               extension;

    if(msg         != NULL &&
       sib1        != NULL &&
       N_bits_used != NULL)
    {
        msg_ptr = msg->msg;

        // Optional indicators
        sib1->p_max_present = liblte_bits_2_value(&msg_ptr, 1);
        tdd_config_opt      = liblte_bits_2_value(&msg_ptr, 1);
        non_crit_ext_opt    = liblte_bits_2_value(&msg_ptr, 1);

        // Cell Access Related Info
        csg_id_opt       = liblte_bits_2_value(&msg_ptr, 1);
        sib1->N_plmn_ids = liblte_bits_2_value(&msg_ptr, 3) + 1;
        for(i=0; i<sib1->N_plmn_ids; i++)
        {
            liblte_rrc_unpack_plmn_identity_ie(&msg_ptr, &sib1->plmn_id[i].id);
            if(LIBLTE_RRC_MCC_NOT_PRESENT == sib1->plmn_id[i].id.mcc &&
               0                          != i)
            {
                sib1->plmn_id[i].id.mcc = sib1->plmn_id[i-1].id.mcc;
            }
            sib1->plmn_id[i].resv_for_oper = (LIBLTE_RRC_RESV_FOR_OPER_ENUM)liblte_bits_2_value(&msg_ptr, 1);
        }
        liblte_rrc_unpack_tracking_area_code_ie(&msg_ptr, &sib1->tracking_area_code);
        liblte_rrc_unpack_cell_identity_ie(&msg_ptr, &sib1->cell_id);
        sib1->cell_barred            = (LIBLTE_RRC_CELL_BARRED_ENUM)liblte_bits_2_value(&msg_ptr, 1);
        sib1->intra_freq_reselection = (LIBLTE_RRC_INTRA_FREQ_RESELECTION_ENUM)liblte_bits_2_value(&msg_ptr, 1);
        sib1->csg_indication         = liblte_bits_2_value(&msg_ptr, 1);
        if(true == csg_id_opt)
        {
            liblte_rrc_unpack_csg_identity_ie(&msg_ptr, &sib1->csg_id);
        }else{
            sib1->csg_id = LIBLTE_RRC_CSG_IDENTITY_NOT_PRESENT;
        }

        // Cell Selection Info
        q_rx_lev_min_offset_opt = liblte_bits_2_value(&msg_ptr, 1);
        liblte_rrc_unpack_q_rx_lev_min_ie(&msg_ptr, &sib1->q_rx_lev_min);
        if(true == q_rx_lev_min_offset_opt)
        {
            sib1->q_rx_lev_min_offset = (liblte_bits_2_value(&msg_ptr, 3) + 1) * 2;
        }else{
            sib1->q_rx_lev_min_offset = 0;
        }

        // P Max
        if(true == sib1->p_max_present)
        {
            liblte_rrc_unpack_p_max_ie(&msg_ptr, &sib1->p_max);
        }

        // Freq Band Indicator
        sib1->freq_band_indicator = liblte_bits_2_value(&msg_ptr, 6) + 1;

        // Scheduling Info List
        sib1->N_sched_info = liblte_bits_2_value(&msg_ptr, 5) + 1;
        for(i=0; i<sib1->N_sched_info; i++)
        {
            sib1->sched_info[i].si_periodicity     = (LIBLTE_RRC_SI_PERIODICITY_ENUM)liblte_bits_2_value(&msg_ptr, 3);
            sib1->sched_info[i].N_sib_mapping_info = liblte_bits_2_value(&msg_ptr, 5);
            for(j=0; j<sib1->sched_info[i].N_sib_mapping_info; j++)
            {
                extension                                        = liblte_bits_2_value(&msg_ptr, 1);
                sib1->sched_info[i].sib_mapping_info[j].sib_type = (LIBLTE_RRC_SIB_TYPE_ENUM)liblte_bits_2_value(&msg_ptr, 4);
            }
        }

        // TDD Config
        if(true == tdd_config_opt)
        {
            sib1->tdd = true;
            liblte_rrc_unpack_tdd_config_ie(&msg_ptr, &sib1->tdd_cnfg);
        }else{
            sib1->tdd = false;
        }

        // SI Window Length
        sib1->si_window_length = (LIBLTE_RRC_SI_WINDOW_LENGTH_ENUM)liblte_bits_2_value(&msg_ptr, 3);

        // System Info Value Tag
        sib1->system_info_value_tag = liblte_bits_2_value(&msg_ptr, 5);

        // Non Critical Extension
        liblte_rrc_consume_noncrit_extension(non_crit_ext_opt, __func__, &msg_ptr);

        // N_bits_used
        *N_bits_used = msg_ptr - (msg->msg);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    Message Name: System Information

    Description: Conveys one or more System Information Blocks

    Document Reference: 36.331 v10.0.0 Section 6.2.2
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_rrc_pack_sys_info_msg(LIBLTE_RRC_SYS_INFO_MSG_STRUCT *sibs,
                                               LIBLTE_BIT_MSG_STRUCT          *msg)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr;
    uint8             *length_ptr;
    uint32             length;
    uint32             pad_bits;
    uint32             i;

    if(sibs != NULL &&
       msg  != NULL)
    {
        msg_ptr = msg->msg;

        // Critical extensions choice
        liblte_value_2_bits(0, &msg_ptr, 1);

        // Optional indicator
        liblte_value_2_bits(0, &msg_ptr, 1);

        // Number of SIBs present
        liblte_value_2_bits(sibs->N_sibs - 1, &msg_ptr, 5);

        for(i=0; i<sibs->N_sibs; i++)
        {
            if(sibs->sibs[i].sib_type < LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_12)
            {
                // Extension indicator
                liblte_value_2_bits(0, &msg_ptr, 1);

                liblte_value_2_bits(sibs->sibs[i].sib_type, &msg_ptr, 4);
                switch(sibs->sibs[i].sib_type)
                {
                case LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_2:
                    err = liblte_rrc_pack_sys_info_block_type_2_ie((LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_2_STRUCT *)&sibs->sibs[i].sib,
                                                                   &msg_ptr);
                    break;
                case LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_3:
                    err = liblte_rrc_pack_sys_info_block_type_3_ie((LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_3_STRUCT *)&sibs->sibs[i].sib,
                                                                   &msg_ptr);
                    break;
                case LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_4:
                    err = liblte_rrc_pack_sys_info_block_type_4_ie((LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_4_STRUCT *)&sibs->sibs[i].sib,
                                                                   &msg_ptr);
                    break;
                case LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_8:
                    err = liblte_rrc_pack_sys_info_block_type_8_ie((LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_8_STRUCT *)&sibs->sibs[i].sib,
                                                                   &msg_ptr);
                    break;
                case LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_9:
                    err = liblte_rrc_pack_sys_info_block_type_9_ie((LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_9_STRUCT *)&sibs->sibs[i].sib,
                                                                   &msg_ptr);
                    break;
                case LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_5:
                case LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_6:
                case LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_7:
                case LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_10:
                case LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_11:
                default:
                    printf("ERROR: Not handling sib type %u\n", sibs->sibs[i].sib_type);
                    err = LIBLTE_ERROR_INVALID_INPUTS;
                    break;
                }
            }else{
                // Extension indicator
                liblte_value_2_bits(1, &msg_ptr, 1);

                liblte_value_2_bits(sibs->sibs[i].sib_type - 10, &msg_ptr, 7);
                length_ptr = msg_ptr;
                liblte_value_2_bits(0, &msg_ptr, 8);
                switch(sibs->sibs[i].sib_type)
                {
                case LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_13:
                    err = liblte_rrc_pack_sys_info_block_type_13_ie((LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_13_STRUCT *)&sibs->sibs[i].sib,
                                                                    &msg_ptr);
                    break;
                case LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_12:
                default:
                    printf("ERROR: Not handling extended sib type %s\n", liblte_rrc_sys_info_block_type_text[sibs->sibs[i].sib_type]);
                    err = LIBLTE_ERROR_INVALID_INPUTS;
                    break;
                }
                length   = ((msg_ptr - length_ptr) / 8) - 1;
                pad_bits = (msg_ptr - length_ptr) % 8;
                if(0 != pad_bits)
                {
                    length++;
                }
                if(length < 128)
                {
                    liblte_value_2_bits(0,      &length_ptr, 1);
                    liblte_value_2_bits(length, &length_ptr, 7);
                }else{
                    msg_ptr = length_ptr;
                    liblte_value_2_bits(0,      &msg_ptr, 2);
                    liblte_value_2_bits(length, &msg_ptr, 14);
                    switch(sibs->sibs[i].sib_type)
                    {
                    case LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_13:
                        err = liblte_rrc_pack_sys_info_block_type_13_ie((LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_13_STRUCT *)&sibs->sibs[i].sib,
                                                                        &msg_ptr);
                        break;
                    case LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_12:
                    default:
                        printf("ERROR: Not handling extended sib type %s\n", liblte_rrc_sys_info_block_type_text[sibs->sibs[i].sib_type]);
                        err = LIBLTE_ERROR_INVALID_INPUTS;
                        break;
                    }
                }
                liblte_value_2_bits(0, &msg_ptr, pad_bits);
            }

            if(LIBLTE_SUCCESS != err)
            {
                break;
            }
        }

        // Fill in the number of bits used
        msg->N_bits = msg_ptr - msg->msg;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_unpack_sys_info_msg(LIBLTE_BIT_MSG_STRUCT          *msg,
                                                 LIBLTE_RRC_SYS_INFO_MSG_STRUCT *sibs)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr;
    uint8             *head_ptr;
    uint32             i;
    uint32             length_determinant_octets;
    uint8              non_crit_ext_opt;

    if(msg  != NULL &&
       sibs != NULL)
    {
        msg_ptr = msg->msg;

        // Critical extensions choice
        if(0 == liblte_bits_2_value(&msg_ptr, 1))
        {
            // Optional indicator
            non_crit_ext_opt = liblte_bits_2_value(&msg_ptr, 1);

            // Number of SIBs present
            sibs->N_sibs = liblte_bits_2_value(&msg_ptr, 5) + 1;

            for(i=0; i<sibs->N_sibs; i++)
            {
                // Extension indicator
                if(0 == liblte_bits_2_value(&msg_ptr, 1))
                {
                    sibs->sibs[i].sib_type = (LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_ENUM)liblte_bits_2_value(&msg_ptr, 4);
                    switch(sibs->sibs[i].sib_type)
                    {
                    case LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_2:
                        err = liblte_rrc_unpack_sys_info_block_type_2_ie(&msg_ptr,
                                                                         (LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_2_STRUCT *)&sibs->sibs[i].sib);
                        break;
                    case LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_3:
                        err = liblte_rrc_unpack_sys_info_block_type_3_ie(&msg_ptr,
                                                                         (LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_3_STRUCT *)&sibs->sibs[i].sib);
                        break;
                    case LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_4:
                        err = liblte_rrc_unpack_sys_info_block_type_4_ie(&msg_ptr,
                                                                         (LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_4_STRUCT *)&sibs->sibs[i].sib);
                        break;
                    case LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_5:
                        err = liblte_rrc_unpack_sys_info_block_type_5_ie(&msg_ptr,
                                                                         (LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_5_STRUCT *)&sibs->sibs[i].sib);
                        break;
                    case LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_6:
                        err = liblte_rrc_unpack_sys_info_block_type_6_ie(&msg_ptr,
                                                                         (LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_6_STRUCT *)&sibs->sibs[i].sib);
                        break;
                    case LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_7:
                        err = liblte_rrc_unpack_sys_info_block_type_7_ie(&msg_ptr,
                                                                         (LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_7_STRUCT *)&sibs->sibs[i].sib);
                        break;
                    case LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_8:
                        err = liblte_rrc_unpack_sys_info_block_type_8_ie(&msg_ptr,
                                                                         (LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_8_STRUCT *)&sibs->sibs[i].sib);
                        break;
                    case LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_9:
                        err = liblte_rrc_unpack_sys_info_block_type_9_ie(&msg_ptr,
                                                                         (LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_9_STRUCT *)&sibs->sibs[i].sib);
                        break;
                    case LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_10:
                    case LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_11:
                    default:
                        printf("ERROR: Not handling sib type %u\n", sibs->sibs[i].sib_type);
                        err = LIBLTE_ERROR_INVALID_INPUTS;
                        break;
                    }
                }else{
                    sibs->sibs[i].sib_type    = (LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_ENUM)(liblte_bits_2_value(&msg_ptr, 7) + 10);
                    length_determinant_octets = 0;
                    if(0 == liblte_bits_2_value(&msg_ptr, 1))
                    {
                        length_determinant_octets = liblte_bits_2_value(&msg_ptr, 7);
                    }else{
                        if(0 == liblte_bits_2_value(&msg_ptr, 1))
                        {
                            length_determinant_octets = liblte_bits_2_value(&msg_ptr, 14);
                        }else{
                            printf("ERROR: Not handling fragmented length determinants\n");
                        }
                    }
                    head_ptr = msg_ptr;
                    switch(sibs->sibs[i].sib_type)
                    {
                    case LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_13:
                        err = liblte_rrc_unpack_sys_info_block_type_13_ie(&msg_ptr,
                                                                          (LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_13_STRUCT *)&sibs->sibs[i].sib);
                        break;
                    case LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_12:
                    default:
                        printf("ERROR: Not handling extended sib type %s\n", liblte_rrc_sys_info_block_type_text[sibs->sibs[i].sib_type]);
                        err = LIBLTE_ERROR_INVALID_INPUTS;
                        break;
                    }
                    liblte_bits_2_value(&msg_ptr, (msg_ptr - head_ptr) % 8);
                }

                if(LIBLTE_SUCCESS != err)
                {
                    break;
                }
            }
            
            liblte_rrc_consume_noncrit_extension(non_crit_ext_opt, __func__, &msg_ptr);
                    
        }else{
            printf("ERROR: Not handling critical extensions in system information message\n");
        }
    }

    return(err);
}

/*********************************************************************
    Message Name: Security Mode Failure

    Description: Used to indicate an unsuccessful completion of a
                 security mode command

    Document Reference: 36.331 v10.0.0 Section 6.2.2 
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_rrc_pack_security_mode_failure_msg(LIBLTE_RRC_SECURITY_MODE_FAILURE_STRUCT *security_mode_failure,
                                                            LIBLTE_BIT_MSG_STRUCT                   *msg)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr;

    if(security_mode_failure != NULL &&
       msg                   != NULL)
    {
        msg_ptr = msg->msg;

        // RRC Transaction ID
        liblte_rrc_pack_rrc_transaction_identifier_ie(security_mode_failure->rrc_transaction_id,
                                                      &msg_ptr);

        // Extension choice
        liblte_value_2_bits(0, &msg_ptr, 1);

        // Optional indicator
        liblte_value_2_bits(0, &msg_ptr, 1);

        // Fill in the number of bits used
        msg->N_bits = msg_ptr - msg->msg;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_unpack_security_mode_failure_msg(LIBLTE_BIT_MSG_STRUCT                   *msg,
                                                              LIBLTE_RRC_SECURITY_MODE_FAILURE_STRUCT *security_mode_failure)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr;

    if(msg                   != NULL &&
       security_mode_failure != NULL)
    {
        msg_ptr = msg->msg;

        // RRC Transaction ID
        liblte_rrc_unpack_rrc_transaction_identifier_ie(&msg_ptr,
                                                        &security_mode_failure->rrc_transaction_id);

        // Extension choice
        bool ext = liblte_bits_2_value(&msg_ptr, 1);

        // Optional indicator
        liblte_rrc_warning_not_handled(liblte_bits_2_value(&msg_ptr, 1), __func__);

        liblte_rrc_consume_noncrit_extension(ext, __func__, &msg_ptr);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    Message Name: Security Mode Complete

    Description: Used to confirm the successful completion of a
                 security mode command

    Document Reference: 36.331 v10.0.0 Section 6.2.2 
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_rrc_pack_security_mode_complete_msg(LIBLTE_RRC_SECURITY_MODE_COMPLETE_STRUCT *security_mode_complete,
                                                             LIBLTE_BIT_MSG_STRUCT                    *msg)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr;

    if(security_mode_complete != NULL &&
       msg                    != NULL)
    {
        msg_ptr = msg->msg;

        // RRC Transaction ID
        liblte_rrc_pack_rrc_transaction_identifier_ie(security_mode_complete->rrc_transaction_id,
                                                      &msg_ptr);

        // Extension choice
        liblte_value_2_bits(0, &msg_ptr, 1);

        // Optional indicator
        liblte_value_2_bits(0, &msg_ptr, 1);

        // Fill in the number of bits used
        msg->N_bits = msg_ptr - msg->msg;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_unpack_security_mode_complete_msg(LIBLTE_BIT_MSG_STRUCT                    *msg,
                                                               LIBLTE_RRC_SECURITY_MODE_COMPLETE_STRUCT *security_mode_complete)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr;

    if(msg                    != NULL &&
       security_mode_complete != NULL)
    {
        msg_ptr = msg->msg;

        // RRC Transaction ID
        liblte_rrc_unpack_rrc_transaction_identifier_ie(&msg_ptr,
                                                        &security_mode_complete->rrc_transaction_id);

        // Extension choice
        bool ext = liblte_bits_2_value(&msg_ptr, 1);

        // Optional indicator
        liblte_rrc_warning_not_handled(liblte_bits_2_value(&msg_ptr, 1), __func__);

        liblte_rrc_consume_noncrit_extension(ext, __func__, &msg_ptr);
        
        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    Message Name: Security Mode Command

    Description: Used to command the activation of AS security

    Document Reference: 36.331 v10.0.0 Section 6.2.2 
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_rrc_pack_security_mode_command_msg(LIBLTE_RRC_SECURITY_MODE_COMMAND_STRUCT *security_mode_cmd,
                                                            LIBLTE_BIT_MSG_STRUCT                   *msg)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr;

    if(security_mode_cmd != NULL &&
       msg               != NULL)
    {
        msg_ptr = msg->msg;

        // RRC Transaction ID
        liblte_rrc_pack_rrc_transaction_identifier_ie(security_mode_cmd->rrc_transaction_id,
                                                      &msg_ptr);

        // Extension choice
        liblte_value_2_bits(0, &msg_ptr, 1);

        // C1 choice
        liblte_value_2_bits(0, &msg_ptr, 2);

        // Optional indicator
        liblte_value_2_bits(0, &msg_ptr, 1);

        // Extension indicator
        liblte_value_2_bits(0, &msg_ptr, 1);

        // Security Algorithms Config
        liblte_rrc_pack_security_algorithm_config_ie(&security_mode_cmd->sec_algs,
                                                     &msg_ptr);

        // Fill in the number of bits used
        msg->N_bits = msg_ptr - msg->msg;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_unpack_security_mode_command_msg(LIBLTE_BIT_MSG_STRUCT                   *msg,
                                                              LIBLTE_RRC_SECURITY_MODE_COMMAND_STRUCT *security_mode_cmd)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr;

    if(msg               != NULL &&
       security_mode_cmd != NULL)
    {
        msg_ptr = msg->msg;

        // RRC Transaction ID
        liblte_rrc_unpack_rrc_transaction_identifier_ie(&msg_ptr,
                                                        &security_mode_cmd->rrc_transaction_id);

        // Extension choice
        bool ext = liblte_bits_2_value(&msg_ptr, 1);

        // C1 choice
        liblte_bits_2_value(&msg_ptr, 2);

        // Optional indicator
        liblte_rrc_warning_not_handled(liblte_bits_2_value(&msg_ptr, 1), __func__);

        // Extension indicator
        bool ext2 = liblte_bits_2_value(&msg_ptr, 1);
       
        liblte_rrc_consume_noncrit_extension(ext2, __func__, &msg_ptr);
        
        // Security Algorithms Config
        liblte_rrc_unpack_security_algorithm_config_ie(&msg_ptr,
                                                       &security_mode_cmd->sec_algs);

        liblte_rrc_consume_noncrit_extension(ext, __func__, &msg_ptr);
        
        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    Message Name: RRC Connection Setup Complete

    Description: Used to confirm the successful completion of an RRC
                 connection

    Document Reference: 36.331 v10.0.0 Section 6.2.2 
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_rrc_pack_rrc_connection_setup_complete_msg(LIBLTE_RRC_CONNECTION_SETUP_COMPLETE_STRUCT *con_setup_complete,
                                                                    LIBLTE_BIT_MSG_STRUCT                       *msg)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr;

    if(con_setup_complete != NULL &&
       msg                != NULL)
    {
        msg_ptr = msg->msg;

        // RRC Transaction ID
        liblte_rrc_pack_rrc_transaction_identifier_ie(con_setup_complete->rrc_transaction_id,
                                                      &msg_ptr);

        // Extension choice
        liblte_value_2_bits(0, &msg_ptr, 1);

        // C1 choice
        liblte_value_2_bits(0, &msg_ptr, 2);

        // Optional indicators
        liblte_value_2_bits(con_setup_complete->registered_mme_present, &msg_ptr, 1);
        liblte_value_2_bits(0,                                          &msg_ptr, 1);

        // Selected PLMN identity
        liblte_value_2_bits(con_setup_complete->selected_plmn_id - 1, &msg_ptr, 3);

        // Registered MME
        if(con_setup_complete->registered_mme_present)
        {
            // Optional indicator
            liblte_value_2_bits(con_setup_complete->registered_mme.plmn_id_present, &msg_ptr, 1);

            // PLMN identity
            if(con_setup_complete->registered_mme.plmn_id_present)
            {
                liblte_rrc_pack_plmn_identity_ie(&con_setup_complete->registered_mme.plmn_id, &msg_ptr);
            }

            // MMEGI
            liblte_value_2_bits(con_setup_complete->registered_mme.mmegi, &msg_ptr, 16);

            // MMEC
            liblte_rrc_pack_mmec_ie(con_setup_complete->registered_mme.mmec, &msg_ptr);
        }

        // Dedicated info NAS
        liblte_rrc_pack_dedicated_info_nas_ie(&con_setup_complete->dedicated_info_nas,
                                              &msg_ptr);

        // Fill in the number of bits used
        msg->N_bits = msg_ptr - msg->msg;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_unpack_rrc_connection_setup_complete_msg(LIBLTE_BIT_MSG_STRUCT                       *msg,
                                                                      LIBLTE_RRC_CONNECTION_SETUP_COMPLETE_STRUCT *con_setup_complete)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr;

    if(msg                != NULL &&
       con_setup_complete != NULL)
    {
        msg_ptr = msg->msg;

        // RRC Transaction ID
        liblte_rrc_unpack_rrc_transaction_identifier_ie(&msg_ptr,
                                                        &con_setup_complete->rrc_transaction_id);

        // Extension choice
        bool ext = liblte_bits_2_value(&msg_ptr, 1);

        // C1 choice
        liblte_bits_2_value(&msg_ptr, 2);

        // Optional indicators
        con_setup_complete->registered_mme_present = liblte_bits_2_value(&msg_ptr, 1);
        liblte_rrc_warning_not_handled(liblte_bits_2_value(&msg_ptr, 1), __func__);
        
        // Selected PLMN identity
        con_setup_complete->selected_plmn_id = liblte_bits_2_value(&msg_ptr, 3) + 1;

        // Registered MME
        if(con_setup_complete->registered_mme_present)
        {
            // Optional indicator
            con_setup_complete->registered_mme.plmn_id_present = liblte_bits_2_value(&msg_ptr, 1);

            // PLMN identity
            if(con_setup_complete->registered_mme.plmn_id_present)
            {
                liblte_rrc_unpack_plmn_identity_ie(&msg_ptr, &con_setup_complete->registered_mme.plmn_id);
            }

            // MMEGI
            con_setup_complete->registered_mme.mmegi = liblte_bits_2_value(&msg_ptr, 16);

            // MMEC
            liblte_rrc_unpack_mmec_ie(&msg_ptr, &con_setup_complete->registered_mme.mmec);
        }

        // Dedicated info NAS
        liblte_rrc_unpack_dedicated_info_nas_ie(&msg_ptr,
                                                &con_setup_complete->dedicated_info_nas);

        liblte_rrc_consume_noncrit_extension(ext, __func__, &msg_ptr);
        
        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    Message Name: RRC Connection Setup

    Description: Used to establish SRB1

    Document Reference: 36.331 v10.0.0 Section 6.2.2 
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_rrc_pack_rrc_connection_setup_msg(LIBLTE_RRC_CONNECTION_SETUP_STRUCT *con_setup,
                                                           LIBLTE_BIT_MSG_STRUCT              *msg)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr;

    if(con_setup != NULL &&
       msg       != NULL)
    {
        msg_ptr = msg->msg;

        // RRC Transaction ID
        liblte_rrc_pack_rrc_transaction_identifier_ie(con_setup->rrc_transaction_id,
                                                      &msg_ptr);

        // Extension choice
        liblte_value_2_bits(0, &msg_ptr, 1);

        // C1 choice
        liblte_value_2_bits(0, &msg_ptr, 3);

        // Optional indicator
        liblte_value_2_bits(0, &msg_ptr, 1);

        // Radio Resource Config Dedicated
        liblte_rrc_pack_rr_config_dedicated_ie(&con_setup->rr_cnfg, &msg_ptr);

        // Fill in the number of bits used
        msg->N_bits = msg_ptr - msg->msg;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_unpack_rrc_connection_setup_msg(LIBLTE_BIT_MSG_STRUCT              *msg,
                                                             LIBLTE_RRC_CONNECTION_SETUP_STRUCT *con_setup)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr;

    if(msg       != NULL &&
       con_setup != NULL)
    {
        msg_ptr = msg->msg;

        // RRC Transaction ID
        liblte_rrc_unpack_rrc_transaction_identifier_ie(&msg_ptr,
                                                        &con_setup->rrc_transaction_id);

        // Critical extension indicator
        bool ext = liblte_bits_2_value(&msg_ptr, 1);

        // C1 choice
        liblte_bits_2_value(&msg_ptr, 3);

        // Optional indicator
        liblte_rrc_warning_not_handled(liblte_bits_2_value(&msg_ptr, 1), __func__);

        // Radio Resource Config Dedicated
        liblte_rrc_unpack_rr_config_dedicated_ie(&msg_ptr, &con_setup->rr_cnfg);

        liblte_rrc_consume_noncrit_extension(ext, __func__, &msg_ptr);
        
        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    Message Name: RRC Connection Request

    Description: Used to request the establishment of an RRC
                 connection

    Document Reference: 36.331 v10.0.0 Section 6.2.2
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_rrc_pack_rrc_connection_request_msg(LIBLTE_RRC_CONNECTION_REQUEST_STRUCT *con_req,
                                                             LIBLTE_BIT_MSG_STRUCT                *msg)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr;
    uint8              ext     = false;

    if(con_req != NULL &&
       msg     != NULL)
    {
        msg_ptr = msg->msg;

        // Extension choice
        liblte_value_2_bits(ext, &msg_ptr, 1);

        // UE Identity Type
        liblte_value_2_bits(con_req->ue_id_type, &msg_ptr, 1);

        // UE Identity
        if(LIBLTE_RRC_CON_REQ_UE_ID_TYPE_S_TMSI == con_req->ue_id_type)
        {
            liblte_rrc_pack_s_tmsi_ie((LIBLTE_RRC_S_TMSI_STRUCT *)&con_req->ue_id,
                                      &msg_ptr);
        }else{ // LIBLTE_RRC_CON_REQ_UE_ID_TYPE_RANDOM_VALUE == con_req->ue_id_type
            liblte_value_2_bits((uint32)(con_req->ue_id.random >> 32), &msg_ptr, 8);
            liblte_value_2_bits((uint32)(con_req->ue_id.random), &msg_ptr, 32);
        }

        // Establishment Cause
        liblte_value_2_bits(con_req->cause, &msg_ptr, 3);

        // Spare
        liblte_value_2_bits(0, &msg_ptr, 1);

        // Fill in the number of bits used
        msg->N_bits = msg_ptr - msg->msg;

        liblte_rrc_consume_noncrit_extension(ext, __func__, &msg_ptr);
        
        err = LIBLTE_SUCCESS;
      
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_unpack_rrc_connection_request_msg(LIBLTE_BIT_MSG_STRUCT                *msg,
                                                               LIBLTE_RRC_CONNECTION_REQUEST_STRUCT *con_req)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr;

    if(msg     != NULL &&
       con_req != NULL)
    {
        msg_ptr = msg->msg;

        // Extension Choice
        bool ext = liblte_bits_2_value(&msg_ptr, 1);
        
        // UE Identity Type
        con_req->ue_id_type = (LIBLTE_RRC_CON_REQ_UE_ID_TYPE_ENUM)liblte_bits_2_value(&msg_ptr, 1);

        // UE Identity
        if(LIBLTE_RRC_CON_REQ_UE_ID_TYPE_S_TMSI == con_req->ue_id_type)
        {
            liblte_rrc_unpack_s_tmsi_ie(&msg_ptr,
                                        (LIBLTE_RRC_S_TMSI_STRUCT *)&con_req->ue_id);
        }else{ // LIBLTE_RRC_CON_REQ_UE_ID_TYPE_RANDOM_VALUE == con_req->ue_id_type
            con_req->ue_id.random  = (uint64)liblte_bits_2_value(&msg_ptr, 8) << 32;
            con_req->ue_id.random |= liblte_bits_2_value(&msg_ptr, 32);
        }

        // Establishment Cause
        con_req->cause = (LIBLTE_RRC_CON_REQ_EST_CAUSE_ENUM)liblte_bits_2_value(&msg_ptr, 3);

        liblte_rrc_consume_noncrit_extension(ext, __func__, &msg_ptr);
        
        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    Message Name: RRC Connection Release

    Description: Used to command the release of an RRC connection

    Document Reference: 36.331 v10.0.0 Section 6.2.2 
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_rrc_pack_rrc_connection_release_msg(LIBLTE_RRC_CONNECTION_RELEASE_STRUCT *con_release,
                                                             LIBLTE_BIT_MSG_STRUCT                *msg)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr;

    if(con_release != NULL &&
       msg         != NULL)
    {
        msg_ptr = msg->msg;

        // RRC Transaction ID
        liblte_rrc_pack_rrc_transaction_identifier_ie(con_release->rrc_transaction_id,
                                                      &msg_ptr);

        // Extension choice
        liblte_value_2_bits(0, &msg_ptr, 1);

        // C1 choice
        liblte_value_2_bits(0, &msg_ptr, 2);

        // Optional indicators
        liblte_value_2_bits(0, &msg_ptr, 1);
        liblte_value_2_bits(0, &msg_ptr, 1);
        liblte_value_2_bits(0, &msg_ptr, 1);

        // Release cause
        liblte_value_2_bits(con_release->release_cause, &msg_ptr, 2);

        // Fill in the number of bits used
        msg->N_bits = msg_ptr - msg->msg;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_unpack_rrc_connection_release_msg(LIBLTE_BIT_MSG_STRUCT                *msg,
                                                               LIBLTE_RRC_CONNECTION_RELEASE_STRUCT *con_release)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr;

    if(msg         != NULL &&
       con_release != NULL)
    {
        msg_ptr = msg->msg;

        // RRC Transaction ID
        liblte_rrc_unpack_rrc_transaction_identifier_ie(&msg_ptr,
                                                        &con_release->rrc_transaction_id);

        // Extension choice
        bool ext = liblte_bits_2_value(&msg_ptr, 1);

        // C1 choice
        liblte_bits_2_value(&msg_ptr, 2);

        // Optional indicators
        liblte_rrc_warning_not_handled(liblte_bits_2_value(&msg_ptr, 1), __func__);
        liblte_rrc_warning_not_handled(liblte_bits_2_value(&msg_ptr, 1), __func__);
        liblte_rrc_warning_not_handled(liblte_bits_2_value(&msg_ptr, 1), __func__);

        // Release cause
        con_release->release_cause = (LIBLTE_RRC_RELEASE_CAUSE_ENUM)liblte_bits_2_value(&msg_ptr, 2);

        liblte_rrc_consume_noncrit_extension(ext, __func__, &msg_ptr);
        
        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    Message Name: RRC Connection Reject

    Description: Used to reject the RRC connection establishment

    Document Reference: 36.331 v10.0.0 Section 6.2.2 
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_rrc_pack_rrc_connection_reject_msg(LIBLTE_RRC_CONNECTION_REJECT_STRUCT *con_rej,
                                                            LIBLTE_BIT_MSG_STRUCT               *msg)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr;

    if(con_rej != NULL &&
       msg     != NULL)
    {
        msg_ptr = msg->msg;

        // Extension choice
        liblte_value_2_bits(0, &msg_ptr, 1);

        // C1 choice
        liblte_value_2_bits(0, &msg_ptr, 2);

        // Optional indicator
        liblte_value_2_bits(0, &msg_ptr, 1);

        // Wait Time
        liblte_value_2_bits(con_rej->wait_time - 1, &msg_ptr, 4);

        // Fill in the number of bits used
        msg->N_bits = msg_ptr - msg->msg;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_unpack_rrc_connection_reject_msg(LIBLTE_BIT_MSG_STRUCT               *msg,
                                                              LIBLTE_RRC_CONNECTION_REJECT_STRUCT *con_rej)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr;

    if(msg     != NULL &&
       con_rej != NULL)
    {
        msg_ptr = msg->msg;

        // Extension choice
        bool ext = liblte_bits_2_value(&msg_ptr, 1);

        // C1 choice
        liblte_bits_2_value(&msg_ptr, 2);

        // Optional indicator
        liblte_rrc_warning_not_handled(liblte_bits_2_value(&msg_ptr, 1), __func__);

        // Wait Time
        con_rej->wait_time = liblte_bits_2_value(&msg_ptr, 4) + 1;

        liblte_rrc_consume_noncrit_extension(ext, __func__, &msg_ptr);
        
        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    Message Name: RRC Connection Reestablishment Request

    Description: Used to request the reestablishment of an RRC
                 connection

    Document Reference: 36.331 v10.0.0 Section 6.2.2
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_rrc_pack_rrc_connection_reestablishment_request_msg(LIBLTE_RRC_CONNECTION_REESTABLISHMENT_REQUEST_STRUCT *con_reest_req,
                                                                             LIBLTE_BIT_MSG_STRUCT                                *msg)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr;
    uint8              ext     = false;

    if(con_reest_req != NULL &&
       msg           != NULL)
    {
        msg_ptr = msg->msg;

        // Extension choice
        liblte_value_2_bits(ext, &msg_ptr, 1);

        if(!ext)
        {
            // UE Identity
            liblte_rrc_pack_c_rnti_ie((uint16)con_reest_req->ue_id.c_rnti, &msg_ptr);
            liblte_rrc_pack_phys_cell_id_ie((uint16)con_reest_req->ue_id.phys_cell_id, &msg_ptr);
            liblte_rrc_pack_short_mac_i_ie((uint16)con_reest_req->ue_id.short_mac_i, &msg_ptr);

            // Reestablishment Cause
            liblte_value_2_bits(con_reest_req->cause, &msg_ptr, 2);

            // Spare
            liblte_value_2_bits(0, &msg_ptr, 2);

            // Fill in the number of bits used
            msg->N_bits = msg_ptr - msg->msg;

            err = LIBLTE_SUCCESS;
        }
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_unpack_rrc_connection_reestablishment_request_msg(LIBLTE_BIT_MSG_STRUCT                                *msg,
                                                                               LIBLTE_RRC_CONNECTION_REESTABLISHMENT_REQUEST_STRUCT *con_reest_req)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr;

    if(msg           != NULL &&
       con_reest_req != NULL)
    {
        msg_ptr = msg->msg;

        // Extension Choice
        bool ext = liblte_bits_2_value(&msg_ptr, 1);
        
        // UE Identity
        liblte_rrc_unpack_c_rnti_ie(&msg_ptr, &con_reest_req->ue_id.c_rnti);
        liblte_rrc_unpack_phys_cell_id_ie(&msg_ptr, &con_reest_req->ue_id.phys_cell_id);
        liblte_rrc_unpack_short_mac_i_ie(&msg_ptr, &con_reest_req->ue_id.short_mac_i);

        // Reestablishment Cause
        con_reest_req->cause = (LIBLTE_RRC_CON_REEST_REQ_CAUSE_ENUM)liblte_bits_2_value(&msg_ptr, 2);

        liblte_rrc_consume_noncrit_extension(ext, __func__, &msg_ptr);
        
        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    Message Name: RRC Connection Reestablishment Reject

    Description: Used to indicate the rejection of an RRC connection
                 reestablishment request

    Document Reference: 36.331 v10.0.0 Section 6.2.2 
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_rrc_pack_rrc_connection_reestablishment_reject_msg(LIBLTE_RRC_CONNECTION_REESTABLISHMENT_REJECT_STRUCT *con_reest_rej,
                                                                            LIBLTE_BIT_MSG_STRUCT                               *msg)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr;

    if(con_reest_rej != NULL &&
       msg           != NULL)
    {
        msg_ptr = msg->msg;

        // Extension choice
        liblte_value_2_bits(0, &msg_ptr, 1);

        // Optional indicator
        liblte_value_2_bits(0, &msg_ptr, 1);

        // Fill in the number of bits used
        msg->N_bits = msg_ptr - msg->msg;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_unpack_rrc_connection_reestablishment_reject_msg(LIBLTE_BIT_MSG_STRUCT                               *msg,
                                                                              LIBLTE_RRC_CONNECTION_REESTABLISHMENT_REJECT_STRUCT *con_reest_rej)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr;

    if(msg           != NULL &&
       con_reest_rej != NULL)
    {
        msg_ptr = msg->msg;

        // Extension choice
        bool ext = liblte_bits_2_value(&msg_ptr, 1);

        // Optional indicator
        liblte_rrc_warning_not_handled(liblte_bits_2_value(&msg_ptr, 1), __func__);

        liblte_rrc_consume_noncrit_extension(ext, __func__, &msg_ptr);
        
        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    Message Name: RRC Connection Reestablishment Complete

    Description: Used to confirm the successful completion of an RRC
                 connection reestablishment

    Document Reference: 36.331 v10.0.0 Section 6.2.2 
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_rrc_pack_rrc_connection_reestablishment_complete_msg(LIBLTE_RRC_CONNECTION_REESTABLISHMENT_COMPLETE_STRUCT *con_reest_complete,
                                                                              LIBLTE_BIT_MSG_STRUCT                                 *msg)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr;

    if(con_reest_complete != NULL &&
       msg                != NULL)
    {
        msg_ptr = msg->msg;

        // RRC Transaction ID
        liblte_rrc_pack_rrc_transaction_identifier_ie(con_reest_complete->rrc_transaction_id,
                                                      &msg_ptr);

        // Extension choice
        liblte_value_2_bits(0, &msg_ptr, 1);

        // Optional indicator
        liblte_value_2_bits(0, &msg_ptr, 1);

        // Fill in the number of bits used
        msg->N_bits = msg_ptr - msg->msg;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_unpack_rrc_connection_reestablishment_complete_msg(LIBLTE_BIT_MSG_STRUCT                                 *msg,
                                                                                LIBLTE_RRC_CONNECTION_REESTABLISHMENT_COMPLETE_STRUCT *con_reest_complete)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr;

    if(msg                != NULL &&
       con_reest_complete != NULL)
    {
        msg_ptr = msg->msg;

        // RRC Transaction ID
        liblte_rrc_unpack_rrc_transaction_identifier_ie(&msg_ptr,
                                                        &con_reest_complete->rrc_transaction_id);

        // Extension choice
        bool ext = liblte_bits_2_value(&msg_ptr, 1);

        // Optional indicator
        liblte_rrc_warning_not_handled(liblte_bits_2_value(&msg_ptr, 1), __func__);

        liblte_rrc_consume_noncrit_extension(ext, __func__, &msg_ptr);
        
        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    Message Name: RRC Connection Reestablishment

    Description: Used to resolve contention and to re-establish SRB1

    Document Reference: 36.331 v10.0.0 Section 6.2.2 
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_rrc_pack_rrc_connection_reestablishment_msg(LIBLTE_RRC_CONNECTION_REESTABLISHMENT_STRUCT *con_reest,
                                                                     LIBLTE_BIT_MSG_STRUCT                        *msg)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr;

    if(con_reest != NULL &&
       msg       != NULL)
    {
        msg_ptr = msg->msg;

        // RRC Transaction ID
        liblte_rrc_pack_rrc_transaction_identifier_ie(con_reest->rrc_transaction_id,
                                                      &msg_ptr);

        // Extension choice
        liblte_value_2_bits(0, &msg_ptr, 1);

        // C1 choice
        liblte_value_2_bits(0, &msg_ptr, 3);

        // Optional indicator
        liblte_value_2_bits(0, &msg_ptr, 1);

        // Radio Resource Config Dedicated
        liblte_rrc_pack_rr_config_dedicated_ie(&con_reest->rr_cnfg, &msg_ptr);

        // Next Hop Chaining Count
        liblte_rrc_pack_next_hop_chaining_count_ie(con_reest->next_hop_chaining_count, &msg_ptr);

        // Fill in the number of bits used
        msg->N_bits = msg_ptr - msg->msg;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_unpack_rrc_connection_reestablishment_msg(LIBLTE_BIT_MSG_STRUCT                        *msg,
                                                                       LIBLTE_RRC_CONNECTION_REESTABLISHMENT_STRUCT *con_reest)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr;

    if(msg       != NULL &&
       con_reest != NULL)
    {
        msg_ptr = msg->msg;

        // RRC Transaction ID
        liblte_rrc_unpack_rrc_transaction_identifier_ie(&msg_ptr,
                                                        &con_reest->rrc_transaction_id);

        // Extension choice
        bool ext = liblte_bits_2_value(&msg_ptr, 1);

        // C1 choice
        liblte_bits_2_value(&msg_ptr, 3);

        // Optional indicator
        liblte_rrc_warning_not_handled(liblte_bits_2_value(&msg_ptr, 1), __func__);

        // Radio Resource Config Dedicated
        liblte_rrc_unpack_rr_config_dedicated_ie(&msg_ptr, &con_reest->rr_cnfg);

        // Next Hop Chaining Count
        liblte_rrc_unpack_next_hop_chaining_count_ie(&msg_ptr, &con_reest->next_hop_chaining_count);

        liblte_rrc_consume_noncrit_extension(ext, __func__, &msg_ptr);
        
        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    Message Name: RRC Connection Reconfiguration Complete

    Description: Used to confirm the successful completion of an RRC
                 connection reconfiguration

    Document Reference: 36.331 v10.0.0 Section 6.2.2 
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_rrc_pack_rrc_connection_reconfiguration_complete_msg(LIBLTE_RRC_CONNECTION_RECONFIGURATION_COMPLETE_STRUCT *con_reconfig_complete,
                                                                              LIBLTE_BIT_MSG_STRUCT                                 *msg)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr;

    if(con_reconfig_complete != NULL &&
       msg                   != NULL)
    {
        msg_ptr = msg->msg;

        // RRC Transaction ID
        liblte_rrc_pack_rrc_transaction_identifier_ie(con_reconfig_complete->rrc_transaction_id,
                                                      &msg_ptr);

        // Extension choice
        liblte_value_2_bits(0, &msg_ptr, 1);

        // Optional indicator
        liblte_value_2_bits(0, &msg_ptr, 1);

        // Fill in the number of bits used
        msg->N_bits = msg_ptr - msg->msg;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_unpack_rrc_connection_reconfiguration_complete_msg(LIBLTE_BIT_MSG_STRUCT                                 *msg,
                                                                                LIBLTE_RRC_CONNECTION_RECONFIGURATION_COMPLETE_STRUCT *con_reconfig_complete)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr;

    if(msg                   != NULL &&
       con_reconfig_complete != NULL)
    {
        msg_ptr = msg->msg;

        // RRC Transaction ID
        liblte_rrc_unpack_rrc_transaction_identifier_ie(&msg_ptr,
                                                        &con_reconfig_complete->rrc_transaction_id);

        // Extension choice
        bool ext = liblte_bits_2_value(&msg_ptr, 1);

        // Optional indicator
        liblte_rrc_warning_not_handled(liblte_bits_2_value(&msg_ptr, 1), __func__);

        liblte_rrc_consume_noncrit_extension(ext, __func__, &msg_ptr);
        
        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    Message Name: RRC Connection Reconfiguration

    Description: Modifies an RRC connection

    Document Reference: 36.331 v10.0.0 Section 6.2.2 
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_rrc_pack_rrc_connection_reconfiguration_msg(LIBLTE_RRC_CONNECTION_RECONFIGURATION_STRUCT *con_reconfig,
                                                                     LIBLTE_BIT_MSG_STRUCT                        *msg)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr;
    uint32             i;

    if(con_reconfig != NULL &&
       msg          != NULL)
    {
        msg_ptr = msg->msg;

        // RRC Transaction ID
        liblte_rrc_pack_rrc_transaction_identifier_ie(con_reconfig->rrc_transaction_id, &msg_ptr);

        // Extension choice
        liblte_value_2_bits(0, &msg_ptr, 1);

        // C1 choice
        liblte_value_2_bits(0, &msg_ptr, 3);

        // Optional indicators
        liblte_value_2_bits(con_reconfig->meas_cnfg_present,     &msg_ptr, 1);
        liblte_value_2_bits(con_reconfig->mob_ctrl_info_present, &msg_ptr, 1);
        if(0 == con_reconfig->N_ded_info_nas)
        {
            liblte_value_2_bits(0, &msg_ptr, 1);
        }else{
            liblte_value_2_bits(1, &msg_ptr, 1);
        }
        liblte_value_2_bits(con_reconfig->rr_cnfg_ded_present, &msg_ptr, 1);
        liblte_value_2_bits(con_reconfig->sec_cnfg_ho_present, &msg_ptr, 1);
        liblte_value_2_bits(0, &msg_ptr, 1);

        // Meas Config
        if(con_reconfig->meas_cnfg_present)
        {
            liblte_rrc_pack_meas_config_ie(&con_reconfig->meas_cnfg, &msg_ptr);
        }

        // Mobility Control Info
        if(con_reconfig->mob_ctrl_info_present)
        {
            liblte_rrc_pack_mobility_control_info_ie(&con_reconfig->mob_ctrl_info, &msg_ptr);
        }

        // Dedicated Info NAS List
        if(0 != con_reconfig->N_ded_info_nas)
        {
            liblte_value_2_bits(con_reconfig->N_ded_info_nas - 1, &msg_ptr, 4);
        }
        for(i=0; i<con_reconfig->N_ded_info_nas; i++)
        {
            liblte_rrc_pack_dedicated_info_nas_ie(&con_reconfig->ded_info_nas_list[i], &msg_ptr);
        }

        // Radio Resource Config Dedicated
        if(con_reconfig->rr_cnfg_ded_present)
        {
            liblte_rrc_pack_rr_config_dedicated_ie(&con_reconfig->rr_cnfg_ded, &msg_ptr);
        }

        // Security Config HO
        if(con_reconfig->sec_cnfg_ho_present)
        {
            // Extension indicator
            liblte_value_2_bits(0, &msg_ptr, 1);

            // Handover Type
            liblte_value_2_bits(con_reconfig->sec_cnfg_ho.ho_type, &msg_ptr, 1);

            if(LIBLTE_RRC_HANDOVER_TYPE_INTRA_LTE == con_reconfig->sec_cnfg_ho.ho_type)
            {
                // Optional indicator
                liblte_value_2_bits(con_reconfig->sec_cnfg_ho.intra_lte.sec_alg_cnfg_present, &msg_ptr, 1);

                // Security Algorithm Config
                if(con_reconfig->sec_cnfg_ho.intra_lte.sec_alg_cnfg_present)
                {
                    liblte_rrc_pack_security_algorithm_config_ie(&con_reconfig->sec_cnfg_ho.intra_lte.sec_alg_cnfg, &msg_ptr);
                }

                // Key Change Indicator
                liblte_value_2_bits(con_reconfig->sec_cnfg_ho.intra_lte.key_change_ind, &msg_ptr, 1);

                // Next Hop Chaining Count
                liblte_rrc_pack_next_hop_chaining_count_ie(con_reconfig->sec_cnfg_ho.intra_lte.next_hop_chaining_count, &msg_ptr);
            }else{
                // Security Algorithm Config
                liblte_rrc_pack_security_algorithm_config_ie(&con_reconfig->sec_cnfg_ho.inter_rat.sec_alg_cnfg, &msg_ptr);

                // NAS Security Params To EUTRA
                for(i=0; i<6; i++)
                {
                    liblte_value_2_bits(con_reconfig->sec_cnfg_ho.inter_rat.nas_sec_param_to_eutra[i], &msg_ptr, 8);
                }
            }
        }

        // Fill in the number of bits used
        msg->N_bits = msg_ptr - msg->msg;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_unpack_rrc_connection_reconfiguration_msg(LIBLTE_BIT_MSG_STRUCT                        *msg,
                                                                       LIBLTE_RRC_CONNECTION_RECONFIGURATION_STRUCT *con_reconfig)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr;
    uint32             i;
    bool               ded_info_nas_list_present;

    if(msg          != NULL &&
       con_reconfig != NULL)
    {
        msg_ptr = msg->msg;

        // RRC Transaction ID
        liblte_rrc_unpack_rrc_transaction_identifier_ie(&msg_ptr, &con_reconfig->rrc_transaction_id);

        // Extension choice
        bool ext = liblte_bits_2_value(&msg_ptr, 1);

        // C1 choice
        liblte_bits_2_value(&msg_ptr, 3);

        // Optional indicators
        con_reconfig->meas_cnfg_present     = liblte_bits_2_value(&msg_ptr, 1);
        con_reconfig->mob_ctrl_info_present = liblte_bits_2_value(&msg_ptr, 1);
        ded_info_nas_list_present           = liblte_bits_2_value(&msg_ptr, 1);
        con_reconfig->rr_cnfg_ded_present   = liblte_bits_2_value(&msg_ptr, 1);
        con_reconfig->sec_cnfg_ho_present   = liblte_bits_2_value(&msg_ptr, 1);
        liblte_rrc_warning_not_handled(liblte_bits_2_value(&msg_ptr, 1), __func__);

        // Meas Config
        if(con_reconfig->meas_cnfg_present)
        {
            liblte_rrc_unpack_meas_config_ie(&msg_ptr, &con_reconfig->meas_cnfg);
        }

        // Mobility Control Info
        if(con_reconfig->mob_ctrl_info_present)
        {
            liblte_rrc_unpack_mobility_control_info_ie(&msg_ptr, &con_reconfig->mob_ctrl_info);
        }

        // Dedicated Info NAS List
        if(ded_info_nas_list_present)
        {
            con_reconfig->N_ded_info_nas = liblte_bits_2_value(&msg_ptr, 4) + 1;
            for(i=0; i<con_reconfig->N_ded_info_nas; i++)
            {
                liblte_rrc_unpack_dedicated_info_nas_ie(&msg_ptr, &con_reconfig->ded_info_nas_list[i]);
            }
        }else{
            con_reconfig->N_ded_info_nas = 0;
        }

        // Radio Resource Config Dedicated
        if(con_reconfig->rr_cnfg_ded_present)
        {
            liblte_rrc_unpack_rr_config_dedicated_ie(&msg_ptr, &con_reconfig->rr_cnfg_ded);
        }

        // Security Config HO
        if(con_reconfig->sec_cnfg_ho_present)
        {
            // Extension indicator
            bool ext2 = liblte_bits_2_value(&msg_ptr, 1);

            // Handover Type
            con_reconfig->sec_cnfg_ho.ho_type = (LIBLTE_RRC_HANDOVER_TYPE_ENUM)liblte_bits_2_value(&msg_ptr, 1);

            if(LIBLTE_RRC_HANDOVER_TYPE_INTRA_LTE == con_reconfig->sec_cnfg_ho.ho_type)
            {
                // Optional indicator
                con_reconfig->sec_cnfg_ho.intra_lte.sec_alg_cnfg_present = liblte_bits_2_value(&msg_ptr, 1);

                // Security Algorithm Config
                if(con_reconfig->sec_cnfg_ho.intra_lte.sec_alg_cnfg_present)
                {
                    liblte_rrc_unpack_security_algorithm_config_ie(&msg_ptr, &con_reconfig->sec_cnfg_ho.intra_lte.sec_alg_cnfg);
                }

                // Key Change Indicator
                con_reconfig->sec_cnfg_ho.intra_lte.key_change_ind = liblte_bits_2_value(&msg_ptr, 1);

                // Next Hop Chaining Count
                liblte_rrc_unpack_next_hop_chaining_count_ie(&msg_ptr, &con_reconfig->sec_cnfg_ho.intra_lte.next_hop_chaining_count);
            }else{
                // Security Algorithm Config
                liblte_rrc_unpack_security_algorithm_config_ie(&msg_ptr, &con_reconfig->sec_cnfg_ho.inter_rat.sec_alg_cnfg);

                // NAS Security Params To EUTRA
                for(i=0; i<6; i++)
                {
                    con_reconfig->sec_cnfg_ho.inter_rat.nas_sec_param_to_eutra[i] = liblte_bits_2_value(&msg_ptr, 8);
                }
            }
            
            liblte_rrc_consume_noncrit_extension(ext2, __func__, &msg_ptr);
        }
        
        liblte_rrc_consume_noncrit_extension(ext, __func__, &msg_ptr);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    Message Name: RN Reconfiguration Complete

    Description: Used to confirm the successful completion of an RN
                 reconfiguration

    Document Reference: 36.331 v10.0.0 Section 6.2.2 
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_rrc_pack_rn_reconfiguration_complete_msg(LIBLTE_RRC_RN_RECONFIGURATION_COMPLETE_STRUCT *rn_reconfig_complete,
                                                                  LIBLTE_BIT_MSG_STRUCT                         *msg)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr;

    if(rn_reconfig_complete != NULL &&
       msg                  != NULL)
    {
        msg_ptr = msg->msg;

        // RRC Transaction ID
        liblte_rrc_pack_rrc_transaction_identifier_ie(rn_reconfig_complete->rrc_transaction_id,
                                                      &msg_ptr);

        // Extension choice
        liblte_value_2_bits(0, &msg_ptr, 1);

        // C1 choice
        liblte_value_2_bits(0, &msg_ptr, 2);

        // Optional indicators
        liblte_value_2_bits(0, &msg_ptr, 1);
        liblte_value_2_bits(0, &msg_ptr, 1);

        // Fill in the number of bits used
        msg->N_bits = msg_ptr - msg->msg;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_unpack_rn_reconfiguration_complete_msg(LIBLTE_BIT_MSG_STRUCT                         *msg,
                                                                    LIBLTE_RRC_RN_RECONFIGURATION_COMPLETE_STRUCT *rn_reconfig_complete)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr;

    if(msg                  != NULL &&
       rn_reconfig_complete != NULL)
    {
        msg_ptr = msg->msg;

        // RRC Transaction ID
        liblte_rrc_unpack_rrc_transaction_identifier_ie(&msg_ptr,
                                                        &rn_reconfig_complete->rrc_transaction_id);

        // Extension choice
        bool ext = liblte_bits_2_value(&msg_ptr, 1);

        // C1 choice
        liblte_bits_2_value(&msg_ptr, 2);

        // Optional indicators
        liblte_rrc_warning_not_handled(liblte_bits_2_value(&msg_ptr, 1), __func__);
        liblte_rrc_warning_not_handled(liblte_bits_2_value(&msg_ptr, 1), __func__);

        liblte_rrc_consume_noncrit_extension(ext, __func__, &msg_ptr);
        
        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    Message Name: RN Reconfiguration

    Description: Modifies the RRC connection between the RN and the
                 E-UTRAN

    Document Reference: 36.331 v10.0.0 Section 6.2.2 
*********************************************************************/
// FIXME

/*********************************************************************
    Message Name: Proximity Indication

    Description: Used to indicate that the UE is entering or leaving
                 the proximity of one or more cells whose CSG IDs are
                 in the UEs CSG whitelist

    Document Reference: 36.331 v10.0.0 Section 6.2.2 
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_rrc_pack_proximity_indication_msg(LIBLTE_RRC_PROXIMITY_INDICATION_STRUCT *proximity_ind,
                                                           LIBLTE_BIT_MSG_STRUCT                  *msg)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr;

    if(proximity_ind != NULL &&
       msg           != NULL)
    {
        msg_ptr = msg->msg;

        // Extension choice
        liblte_value_2_bits(0, &msg_ptr, 1);

        // C1 choice
        liblte_value_2_bits(0, &msg_ptr, 2);

        // Optional indicator
        liblte_value_2_bits(0, &msg_ptr, 1);

        // Proximity indication type
        liblte_value_2_bits(proximity_ind->type, &msg_ptr, 1);

        // Carrier frequency type extension indicator
        liblte_value_2_bits(0, &msg_ptr, 1);

        // Carrier frequency choice
        liblte_value_2_bits(proximity_ind->carrier_freq_type, &msg_ptr, 1);

        // Carrier frequency
        if(LIBLTE_RRC_PROXIMITY_INDICATION_CARRIER_FREQ_TYPE_EUTRA == proximity_ind->carrier_freq_type)
        {
            liblte_rrc_pack_arfcn_value_eutra_ie(proximity_ind->carrier_freq,
                                                 &msg_ptr);
        }else{
            liblte_rrc_pack_arfcn_value_utra_ie(proximity_ind->carrier_freq,
                                                &msg_ptr);
        }

        // Fill in the number of bits used
        msg->N_bits = msg_ptr - msg->msg;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_unpack_proximity_indication_msg(LIBLTE_BIT_MSG_STRUCT                  *msg,
                                                             LIBLTE_RRC_PROXIMITY_INDICATION_STRUCT *proximity_ind)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr;

    if(msg           != NULL &&
       proximity_ind != NULL)
    {
        msg_ptr = msg->msg;

        // Extension choice
        bool ext = liblte_bits_2_value(&msg_ptr, 1);

        // C1 choice
        liblte_bits_2_value(&msg_ptr, 2);

        // Optional indicator
        liblte_rrc_warning_not_handled(liblte_bits_2_value(&msg_ptr, 1), __func__);

        // Proximity indication type
        proximity_ind->type = (LIBLTE_RRC_PROXIMITY_INDICATION_TYPE_ENUM)liblte_bits_2_value(&msg_ptr, 1);

        // Carrier frequency type extension indicator
        liblte_rrc_warning_not_handled(liblte_bits_2_value(&msg_ptr, 1), __func__);

        // Carrier frequency type
        proximity_ind->carrier_freq_type = (LIBLTE_RRC_PROXIMITY_INDICATION_CARRIER_FREQ_TYPE_ENUM)liblte_bits_2_value(&msg_ptr, 1);

        // Carrier frequency
        if(LIBLTE_RRC_PROXIMITY_INDICATION_CARRIER_FREQ_TYPE_EUTRA == proximity_ind->carrier_freq)
        {
            liblte_rrc_unpack_arfcn_value_eutra_ie(&msg_ptr,
                                                   &proximity_ind->carrier_freq);
        }else{
            liblte_rrc_unpack_arfcn_value_utra_ie(&msg_ptr,
                                                  &proximity_ind->carrier_freq);
        }
        
        liblte_rrc_consume_noncrit_extension(ext, __func__, &msg_ptr);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    Message Name: Paging

    Description: Used for the notification of one or more UEs

    Document Reference: 36.331 v10.0.0 Section 6.2.2
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_rrc_pack_paging_msg(LIBLTE_RRC_PAGING_STRUCT *page,
                                             LIBLTE_BIT_MSG_STRUCT    *msg)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr;
    uint32             i;
    uint32             j;

    if(page != NULL &&
       msg  != NULL)
    {
        msg_ptr = msg->msg;

        // Optional indicators
        if(page->paging_record_list_size != 0)
        {
            liblte_value_2_bits(1, &msg_ptr, 1);
        }else{
            liblte_value_2_bits(0, &msg_ptr, 1);
        }
        liblte_value_2_bits(page->system_info_modification_present, &msg_ptr, 1);
        liblte_value_2_bits(page->etws_indication_present,          &msg_ptr, 1);
        liblte_value_2_bits(page->non_crit_ext_present,             &msg_ptr, 1);

        if(page->paging_record_list_size != 0)
        {
            liblte_value_2_bits(page->paging_record_list_size - 1, &msg_ptr, 4);
            for(i=0; i<page->paging_record_list_size; i++)
            {
                // Extension indicator
                liblte_value_2_bits(0, &msg_ptr, 1);

                // Paging UE identity
                {
                    // Extension indicator
                    liblte_value_2_bits(0, &msg_ptr, 1);

                    liblte_value_2_bits(page->paging_record_list[i].ue_identity.ue_identity_type, &msg_ptr, 1);
                    if(LIBLTE_RRC_PAGING_UE_IDENTITY_TYPE_S_TMSI == page->paging_record_list[i].ue_identity.ue_identity_type)
                    {
                        liblte_rrc_pack_s_tmsi_ie(&page->paging_record_list[i].ue_identity.s_tmsi,
                                                  &msg_ptr);
                    }else{
                        liblte_value_2_bits(page->paging_record_list[i].ue_identity.imsi_size - 6, &msg_ptr, 4);
                        for(j=0; j<page->paging_record_list[i].ue_identity.imsi_size; j++)
                        {
                            liblte_value_2_bits(page->paging_record_list[i].ue_identity.imsi[j], &msg_ptr, 4);
                        }
                    }
                }

                liblte_value_2_bits(page->paging_record_list[i].cn_domain, &msg_ptr, 1);
            }
        }

        if(page->system_info_modification_present)
        {
            liblte_value_2_bits(page->system_info_modification, &msg_ptr, 1);
        }

        if(page->etws_indication_present)
        {
            liblte_value_2_bits(page->etws_indication, &msg_ptr, 1);
        }

        if(page->non_crit_ext_present)
        {
            // Optional indicators
            liblte_value_2_bits(page->non_crit_ext.late_non_crit_ext_present, &msg_ptr, 1);
            liblte_value_2_bits(page->non_crit_ext.non_crit_ext_present,      &msg_ptr, 1);

            if(page->non_crit_ext.late_non_crit_ext_present)
            {
                // FIXME
            }

            if(page->non_crit_ext.non_crit_ext_present)
            {
                // Optional indicators
                liblte_value_2_bits(page->non_crit_ext.non_crit_ext.cmas_ind_present,     &msg_ptr, 1);
                liblte_value_2_bits(page->non_crit_ext.non_crit_ext.non_crit_ext_present, &msg_ptr, 1);

                if(page->non_crit_ext.non_crit_ext.cmas_ind_present)
                {
                    liblte_value_2_bits(page->non_crit_ext.non_crit_ext.cmas_ind_r9, &msg_ptr, 1);
                }

                if(page->non_crit_ext.non_crit_ext.non_crit_ext_present)
                {
                    // FIXME
                }
            }
        }

        // Fill in the number of bits used
        msg->N_bits = msg_ptr - msg->msg;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_unpack_paging_msg(LIBLTE_BIT_MSG_STRUCT    *msg,
                                               LIBLTE_RRC_PAGING_STRUCT *page)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr;
    uint32             i;
    uint32             j;
    uint8              paging_record_list_present;

    if(msg  != NULL &&
       page != NULL)
    {
        msg_ptr = msg->msg;

        // Optional indicators
        paging_record_list_present             = liblte_bits_2_value(&msg_ptr, 1);
        page->system_info_modification_present = liblte_bits_2_value(&msg_ptr, 1);
        page->etws_indication_present          = liblte_bits_2_value(&msg_ptr, 1);
        page->non_crit_ext_present             = liblte_bits_2_value(&msg_ptr, 1);

        if(paging_record_list_present)
        {
            page->paging_record_list_size = liblte_bits_2_value(&msg_ptr, 4) + 1;
            for(i=0; i<page->paging_record_list_size; i++)
            {
                // Extension indicator
                bool ext = liblte_bits_2_value(&msg_ptr, 1);

                // Paging UE identity
                {
                    // Extension indicator
                    bool ext2 = liblte_bits_2_value(&msg_ptr, 1);

                    page->paging_record_list[i].ue_identity.ue_identity_type = (LIBLTE_RRC_PAGING_UE_IDENTITY_TYPE_ENUM)liblte_bits_2_value(&msg_ptr, 1);
                    if(LIBLTE_RRC_PAGING_UE_IDENTITY_TYPE_S_TMSI == page->paging_record_list[i].ue_identity.ue_identity_type)
                    {
                        liblte_rrc_unpack_s_tmsi_ie(&msg_ptr,
                                                    &page->paging_record_list[i].ue_identity.s_tmsi);
                    }else{
                        page->paging_record_list[i].ue_identity.imsi_size = liblte_bits_2_value(&msg_ptr, 4) + 6;
                        for(j=0; j<page->paging_record_list[i].ue_identity.imsi_size; j++)
                        {
                            page->paging_record_list[i].ue_identity.imsi[j] = liblte_bits_2_value(&msg_ptr, 4);
                        }
                    }
                    
                    liblte_rrc_consume_noncrit_extension(ext2, __func__, &msg_ptr);
                }

                page->paging_record_list[i].cn_domain = (LIBLTE_RRC_CN_DOMAIN_ENUM)liblte_bits_2_value(&msg_ptr, 1);
                
                liblte_rrc_consume_noncrit_extension(ext, __func__, &msg_ptr);
            }
        }

        if(page->system_info_modification_present)
        {
            page->system_info_modification = (LIBLTE_RRC_SYSTEM_INFO_MODIFICATION_ENUM)liblte_bits_2_value(&msg_ptr, 1);
        }

        if(page->etws_indication_present)
        {
            page->etws_indication = (LIBLTE_RRC_ETWS_INDICATION_ENUM)liblte_bits_2_value(&msg_ptr, 1);
        }

        if(page->non_crit_ext_present)
        {
            // Optional indicators
            page->non_crit_ext.late_non_crit_ext_present = liblte_bits_2_value(&msg_ptr, 1);
            page->non_crit_ext.non_crit_ext_present      = liblte_bits_2_value(&msg_ptr, 1);

            if(page->non_crit_ext.late_non_crit_ext_present)
            {
                // FIXME
                printf("Warning late non-crit-extension not handled in paging message\n");
            }

            if(page->non_crit_ext.non_crit_ext_present)
            {
                // Optional indicators
                page->non_crit_ext.non_crit_ext.cmas_ind_present     = liblte_bits_2_value(&msg_ptr, 1);
                page->non_crit_ext.non_crit_ext.non_crit_ext_present = liblte_bits_2_value(&msg_ptr, 1);

                if(page->non_crit_ext.non_crit_ext.cmas_ind_present)
                {
                    page->non_crit_ext.non_crit_ext.cmas_ind_r9 = (LIBLTE_RRC_CMAS_INDICATION_R9_ENUM)liblte_bits_2_value(&msg_ptr, 1);
                }

                if(page->non_crit_ext.non_crit_ext.non_crit_ext_present)
                {
                    // FIXME
                    printf("Warning non-crit-extension not handled in paging message\n");
                }
            }
        }

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    Message Name: Mobility From EUTRA Command

    Description: Used to command handover or a cell change from E-UTRA
                 to another RAT, or enhanced CS fallback to CDMA2000
                 1xRTT

    Document Reference: 36.331 v10.0.0 Section 6.2.2 
*********************************************************************/
// FIXME

/*********************************************************************
    Message Name: Measurement Report

    Description: Used for the indication of measurement results

    Document Reference: 36.331 v10.0.0 Section 6.2.2 
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_rrc_pack_cgi_info_ie(LIBLTE_RRC_CGI_INFO_STRUCT  *cgi_info,
                                              uint8                      **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(cgi_info != NULL &&
       ie_ptr   != NULL)
    {
        liblte_value_2_bits(cgi_info->have_plmn_identity_list, ie_ptr,  1);
        liblte_rrc_pack_cell_global_id_eutra_ie(&cgi_info->cell_global_id, ie_ptr);
        liblte_rrc_pack_tracking_area_code_ie(cgi_info->tracking_area_code, ie_ptr);
        if(cgi_info->have_plmn_identity_list) {
          liblte_value_2_bits(cgi_info->n_plmn_identity_list-1, ie_ptr,  3);
          for(uint32 i=0; i<cgi_info->n_plmn_identity_list; i++) {
            liblte_rrc_pack_plmn_identity_ie(&cgi_info->plmn_identity_list[i], ie_ptr);
          }
        }

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_unpack_cgi_info_ie(uint8                      **ie_ptr,
                                                LIBLTE_RRC_CGI_INFO_STRUCT  *cgi_info)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr   != NULL &&
       cgi_info != NULL)
    {
        cgi_info->have_plmn_identity_list = (bool)liblte_bits_2_value(ie_ptr,  1);
        liblte_rrc_unpack_cell_global_id_eutra_ie(ie_ptr, &cgi_info->cell_global_id);
        liblte_rrc_unpack_tracking_area_code_ie(ie_ptr, &cgi_info->tracking_area_code);
        if(cgi_info->have_plmn_identity_list) {
          cgi_info->n_plmn_identity_list = liblte_bits_2_value(ie_ptr,  3) + 1;
          for(uint32 i=0; i<cgi_info->n_plmn_identity_list; i++) {
            liblte_rrc_unpack_plmn_identity_ie(ie_ptr, &cgi_info->plmn_identity_list[i]);
          }
        }
        err = LIBLTE_SUCCESS;
    }

    return(err);
}

LIBLTE_ERROR_ENUM liblte_rrc_pack_meas_result_ie(LIBLTE_RRC_MEAS_RESULT_STRUCT  *meas_result,
                                                 uint8                         **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(meas_result != NULL &&
       ie_ptr      != NULL)
    {
        //ext
        liblte_value_2_bits(0, ie_ptr,  1);

        //options
        liblte_value_2_bits(meas_result->have_rsrp, ie_ptr, 1);
        liblte_value_2_bits(meas_result->have_rsrq, ie_ptr, 1);

        if(meas_result->have_rsrp) {
          liblte_rrc_pack_rsrp_range_ie(meas_result->rsrp_result, ie_ptr);
        }
        if(meas_result->have_rsrq) {
          liblte_rrc_pack_rsrq_range_ie(meas_result->rsrq_result, ie_ptr);
        }

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_unpack_meas_result_ie(uint8                         **ie_ptr,
                                                   LIBLTE_RRC_MEAS_RESULT_STRUCT  *meas_result)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr      != NULL &&
       meas_result != NULL)
    {
        //ext
        bool ext = liblte_bits_2_value(ie_ptr, 1);

        //options
        meas_result->have_rsrp = liblte_bits_2_value(ie_ptr, 1);
        meas_result->have_rsrq = liblte_bits_2_value(ie_ptr, 1);

        if(meas_result->have_rsrp) {
          liblte_rrc_unpack_rsrp_range_ie(ie_ptr, &meas_result->rsrp_result);
        }
        if(meas_result->have_rsrq) {
          liblte_rrc_unpack_rsrq_range_ie(ie_ptr, &meas_result->rsrq_result);
        }

        //skip extensions
        liblte_rrc_consume_noncrit_extension(ext, __func__, ie_ptr);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

LIBLTE_ERROR_ENUM liblte_rrc_pack_meas_result_eutra_ie(LIBLTE_RRC_MEAS_RESULT_EUTRA_STRUCT  *meas_result_eutra,
                                                       uint8                               **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(meas_result_eutra != NULL &&
       ie_ptr            != NULL)
    {
        liblte_value_2_bits(meas_result_eutra->have_cgi_info, ie_ptr,  1);
        liblte_rrc_pack_phys_cell_id_ie(meas_result_eutra->phys_cell_id, ie_ptr);
        if(meas_result_eutra->have_cgi_info) {
          liblte_rrc_pack_cgi_info_ie(&meas_result_eutra->cgi_info, ie_ptr);
        }
        liblte_rrc_pack_meas_result_ie(&meas_result_eutra->meas_result, ie_ptr);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_unpack_meas_result_eutra_ie(uint8                               **ie_ptr,
                                                         LIBLTE_RRC_MEAS_RESULT_EUTRA_STRUCT  *meas_result_eutra)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr            != NULL &&
       meas_result_eutra != NULL)
    {
        meas_result_eutra->have_cgi_info = liblte_bits_2_value(ie_ptr, 1);
        liblte_rrc_unpack_phys_cell_id_ie(ie_ptr, &meas_result_eutra->phys_cell_id);
        if(meas_result_eutra->have_cgi_info) {
          liblte_rrc_unpack_cgi_info_ie(ie_ptr, &meas_result_eutra->cgi_info);
        }
        liblte_rrc_unpack_meas_result_ie(ie_ptr, &meas_result_eutra->meas_result);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

LIBLTE_ERROR_ENUM liblte_rrc_pack_measurement_report_msg(LIBLTE_RRC_MEASUREMENT_REPORT_STRUCT *meas_report,
                                                         LIBLTE_BIT_MSG_STRUCT                *msg)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr;

    if(meas_report != NULL &&
       msg         != NULL)
    {
        msg_ptr = msg->msg;

        //MeasurementReport
        liblte_value_2_bits(0, &msg_ptr, 1); //critical extensions
        liblte_value_2_bits(0, &msg_ptr, 3); //c1

        //MeasurementReport-r8-IEs
        liblte_value_2_bits(0, &msg_ptr, 1); //non-critical extensions

        //MeasResults
        liblte_value_2_bits(0, &msg_ptr, 1); //ext
        liblte_value_2_bits(meas_report->have_meas_result_neigh_cells, &msg_ptr, 1);
        liblte_rrc_pack_meas_id_ie(meas_report->meas_id, &msg_ptr);
        liblte_rrc_pack_rsrp_range_ie(meas_report->pcell_rsrp_result, &msg_ptr);
        liblte_rrc_pack_rsrq_range_ie(meas_report->pcell_rsrq_result, &msg_ptr);
        if(meas_report->have_meas_result_neigh_cells) {
          liblte_value_2_bits(0, &msg_ptr, 1); //choice from before extension marker
          liblte_value_2_bits(meas_report->meas_result_neigh_cells_choice, &msg_ptr, 2);
          if(meas_report->meas_result_neigh_cells_choice != LIBLTE_RRC_MEAS_RESULT_LIST_EUTRA) {
            printf("NOT HANDLING %s\n", liblte_rrc_meas_reult_neigh_cells_text[meas_report->meas_result_neigh_cells_choice]);
          } else {
            //MeasResultListEUTRA
            liblte_value_2_bits(meas_report->meas_result_neigh_cells.eutra.n_result-1, &msg_ptr, 3);
            for(uint32 i=0; i<meas_report->meas_result_neigh_cells.eutra.n_result; i++) {
              liblte_rrc_pack_meas_result_eutra_ie(&meas_report->meas_result_neigh_cells.eutra.result_eutra_list[i], &msg_ptr);
            }
          }
        }

        // Fill in the number of bits used
        msg->N_bits = msg_ptr - msg->msg;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_unpack_measurement_report_msg(LIBLTE_BIT_MSG_STRUCT                *msg,
                                                           LIBLTE_RRC_MEASUREMENT_REPORT_STRUCT *meas_report)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr;

    if(msg      != NULL &&
       meas_report != NULL)
    {
        msg_ptr = msg->msg;

        //MeasurementReport
        bool crit_ext = liblte_bits_2_value(&msg_ptr, 1); //critical extensions
        liblte_bits_2_value(&msg_ptr, 3); //c1

        //MeasurementReport-r8-IEs
        bool non_crit_ext = liblte_bits_2_value(&msg_ptr, 1); //non-critical extensions

        //MeasResults
        bool ext = liblte_bits_2_value(&msg_ptr, 1);
        meas_report->have_meas_result_neigh_cells = liblte_bits_2_value(&msg_ptr, 1);
        liblte_rrc_unpack_meas_id_ie(&msg_ptr, &meas_report->meas_id);
        liblte_rrc_unpack_rsrp_range_ie(&msg_ptr, &meas_report->pcell_rsrp_result);
        liblte_rrc_unpack_rsrq_range_ie(&msg_ptr, &meas_report->pcell_rsrq_result);
        if(meas_report->have_meas_result_neigh_cells) {
          liblte_bits_2_value(&msg_ptr, 1); //choice from before extension marker
          meas_report->meas_result_neigh_cells_choice = (LIBLTE_RRC_MEAS_RESULT_NEIGH_CELLS_ENUM) liblte_bits_2_value(&msg_ptr, 2);
          if(meas_report->meas_result_neigh_cells_choice != LIBLTE_RRC_MEAS_RESULT_LIST_EUTRA) {
            printf("NOT HANDLING %s\n", liblte_rrc_meas_reult_neigh_cells_text[meas_report->meas_result_neigh_cells_choice]);
          } else {
            //MeasResultListEUTRA
            meas_report->meas_result_neigh_cells.eutra.n_result = liblte_bits_2_value(&msg_ptr, 3) + 1;
            for(uint32 i=0; i<meas_report->meas_result_neigh_cells.eutra.n_result; i++) {
              liblte_rrc_unpack_meas_result_eutra_ie(&msg_ptr, &meas_report->meas_result_neigh_cells.eutra.result_eutra_list[i]);
            }
          }
        }

        //skip extensions
        liblte_rrc_consume_noncrit_extension(crit_ext, __func__, &msg_ptr);
        liblte_rrc_consume_noncrit_extension(non_crit_ext, __func__, &msg_ptr);
        liblte_rrc_consume_noncrit_extension(ext, __func__, &msg_ptr);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    Message Name: MBSFN Area Configuration

    Description: Contains the MBMS control information applicable for
                 an MBSFN area

    Document Reference: 36.331 v10.0.0 Section 6.2.2
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_rrc_pack_mbsfn_area_configuration_r9_msg(LIBLTE_RRC_MBSFN_AREA_CONFIGURATION_R9_STRUCT *mbsfn_area_cnfg,
                                                                  LIBLTE_BIT_MSG_STRUCT                         *msg)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr;
    uint32             i;

    if(mbsfn_area_cnfg != NULL &&
       msg             != NULL)
    {
        msg_ptr = msg->msg;

        // Non-critical extension
        liblte_value_2_bits(0, &msg_ptr, 1);

        // commonsf_allocpatternlist_r9
        liblte_value_2_bits(mbsfn_area_cnfg->commonsf_allocpatternlist_r9_size-1, &msg_ptr, 3);
        for(i=0; i<mbsfn_area_cnfg->commonsf_allocpatternlist_r9_size; i++){
            liblte_rrc_pack_mbsfn_subframe_config_ie(&mbsfn_area_cnfg->commonsf_allocpatternlist_r9[i], &msg_ptr);
        }

        // commonsf_allocperiod_r9
        liblte_value_2_bits(mbsfn_area_cnfg->commonsf_allocperiod_r9, &msg_ptr, 3);

        // pmch_infolist_r9
        liblte_value_2_bits(mbsfn_area_cnfg->pmch_infolist_r9_size, &msg_ptr, 4);
        for(i=0; i<mbsfn_area_cnfg->pmch_infolist_r9_size; i++){
            liblte_rrc_pack_pmch_info_r9_ie(&mbsfn_area_cnfg->pmch_infolist_r9[i], &msg_ptr);
        }

        // Fill in the number of bits used
        msg->N_bits = msg_ptr - msg->msg;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_unpack_mbsfn_area_configuration_r9_msg(LIBLTE_BIT_MSG_STRUCT                         *msg,
                                                                    LIBLTE_RRC_MBSFN_AREA_CONFIGURATION_R9_STRUCT *mbsfn_area_cnfg)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr;
    uint32             i;
    bool               ext;

    if(msg             != NULL &&
       mbsfn_area_cnfg != NULL)
    {
        msg_ptr = msg->msg;

        // Non-critical extension
        ext = liblte_bits_2_value(&msg_ptr, 1);
        liblte_rrc_warning_not_handled(ext, __func__);

        // commonsf_allocpatternlist_r9
        mbsfn_area_cnfg->commonsf_allocpatternlist_r9_size = liblte_bits_2_value(&msg_ptr, 3) + 1;
        for(i=0; i<mbsfn_area_cnfg->commonsf_allocpatternlist_r9_size; i++){
            liblte_rrc_unpack_mbsfn_subframe_config_ie(&msg_ptr, &mbsfn_area_cnfg->commonsf_allocpatternlist_r9[i]);
        }

        // commonsf_allocperiod_r9
        mbsfn_area_cnfg->commonsf_allocperiod_r9 = (LIBLTE_RRC_MBSFN_COMMON_SF_ALLOC_PERIOD_R9_ENUM)liblte_bits_2_value(&msg_ptr, 3);

        // pmch_infolist_r9
        mbsfn_area_cnfg->pmch_infolist_r9_size = liblte_bits_2_value(&msg_ptr, 4);
        for(i=0; i<mbsfn_area_cnfg->pmch_infolist_r9_size; i++){
            liblte_rrc_unpack_pmch_info_r9_ie(&msg_ptr, &mbsfn_area_cnfg->pmch_infolist_r9[i]);
        }

        liblte_rrc_consume_noncrit_extension(ext, __func__, &msg_ptr);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}


/*********************************************************************
    Message Name: Master Information Block

    Description: Includes the system information transmitted on BCH

    Document Reference: 36.331 v10.0.0 Section 6.2.2 
*********************************************************************/
// Inlined with BCCH BCH Message

/*********************************************************************
    Message Name: Logged Measurements Configuration

    Description: Used by E-UTRAN to configure the UE to perform
                 logging of measurement results while in RRC_IDLE

    Document Reference: 36.331 v10.0.0 Section 6.2.2 
*********************************************************************/
// FIXME

/*********************************************************************
    Message Name: Handover From EUTRA Preparation Request (CDMA2000)

    Description: Used to trigger the handover preparation procedure
                 with a CDMA2000 RAT

    Document Reference: 36.331 v10.0.0 Section 6.2.2 
*********************************************************************/
// FIXME

/*********************************************************************
    Message Name: DL Information Transfer

    Description: Used for the downlink transfer of dedicated NAS
                 information

    Document Reference: 36.331 v10.0.0 Section 6.2.2 
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_rrc_pack_dl_information_transfer_msg(LIBLTE_RRC_DL_INFORMATION_TRANSFER_STRUCT *dl_info_transfer,
                                                              LIBLTE_BIT_MSG_STRUCT                     *msg)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr;

    if(dl_info_transfer != NULL &&
       msg              != NULL)
    {
        msg_ptr = msg->msg;

        // RRC Transaction ID
        liblte_rrc_pack_rrc_transaction_identifier_ie(dl_info_transfer->rrc_transaction_id,
                                                      &msg_ptr);

        // Extension choice
        liblte_value_2_bits(0, &msg_ptr, 1);

        // C1 choice
        liblte_value_2_bits(0, &msg_ptr, 2);

        // Optional indicator
        liblte_value_2_bits(0, &msg_ptr, 1);

        // Dedicated info type choice
        liblte_value_2_bits(dl_info_transfer->dedicated_info_type, &msg_ptr, 2);

        if(LIBLTE_RRC_DL_INFORMATION_TRANSFER_TYPE_NAS == dl_info_transfer->dedicated_info_type)
        {
            liblte_rrc_pack_dedicated_info_nas_ie(&dl_info_transfer->dedicated_info,
                                                  &msg_ptr);
        }else{
            liblte_rrc_pack_dedicated_info_cdma2000_ie(&dl_info_transfer->dedicated_info,
                                                       &msg_ptr);
        }

        // Fill in the number of bits used
        msg->N_bits = msg_ptr - msg->msg;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_unpack_dl_information_transfer_msg(LIBLTE_BIT_MSG_STRUCT                     *msg,
                                                                LIBLTE_RRC_DL_INFORMATION_TRANSFER_STRUCT *dl_info_transfer)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr;

    if(msg              != NULL &&
       dl_info_transfer != NULL)
    {
        msg_ptr = msg->msg;

        // RRC Transaction ID
        liblte_rrc_unpack_rrc_transaction_identifier_ie(&msg_ptr,
                                                        &dl_info_transfer->rrc_transaction_id);

        // Extension choice
        bool ext = liblte_bits_2_value(&msg_ptr, 1);

        // C1 choice
        liblte_bits_2_value(&msg_ptr, 2);

        // Optional indicator
        liblte_rrc_warning_not_handled(liblte_bits_2_value(&msg_ptr, 1), __func__);

        // Dedicated info type choice
        dl_info_transfer->dedicated_info_type = (LIBLTE_RRC_DL_INFORMATION_TRANSFER_TYPE_ENUM)liblte_bits_2_value(&msg_ptr, 2);

        if(LIBLTE_RRC_DL_INFORMATION_TRANSFER_TYPE_NAS == dl_info_transfer->dedicated_info_type)
        {
            liblte_rrc_unpack_dedicated_info_nas_ie(&msg_ptr,
                                                    &dl_info_transfer->dedicated_info);
        }else{
            liblte_rrc_unpack_dedicated_info_cdma2000_ie(&msg_ptr,
                                                         &dl_info_transfer->dedicated_info);
        }
        
        liblte_rrc_consume_noncrit_extension(ext, __func__, &msg_ptr);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    Message Name: CSFB Parameters Response CDMA2000

    Description: Used to provide the CDMA2000 1xRTT parameters to the
                 UE so the UE can register with the CDMA2000 1xRTT
                 network to support CSFB to CDMA2000 1xRTT

    Document Reference: 36.331 v10.0.0 Section 6.2.2 
*********************************************************************/
// FIXME

/*********************************************************************
    Message Name: CSFB Parameters Request CDMA2000

    Description: Used by the UE to obtain the CDMA2000 1xRTT
                 parameters from the network

    Document Reference: 36.331 v10.0.0 Section 6.2.2 
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_rrc_pack_csfb_parameters_request_cdma2000_msg(LIBLTE_RRC_CSFB_PARAMETERS_REQUEST_CDMA2000_STRUCT *csfb_params_req_cdma2000,
                                                                       LIBLTE_BIT_MSG_STRUCT                              *msg)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr;

    if(csfb_params_req_cdma2000 != NULL &&
       msg                      != NULL)
    {
        msg_ptr = msg->msg;

        // Extension choice
        liblte_value_2_bits(0, &msg_ptr, 1);

        // Optional indicator
        liblte_value_2_bits(0, &msg_ptr, 1);

        // Fill in the number of bits used
        msg->N_bits = msg_ptr - msg->msg;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_unpack_csfb_parameters_request_cdma2000_msg(LIBLTE_BIT_MSG_STRUCT                              *msg,
                                                                         LIBLTE_RRC_CSFB_PARAMETERS_REQUEST_CDMA2000_STRUCT *csfb_params_req_cdma2000)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr;

    if(msg                      != NULL &&
       csfb_params_req_cdma2000 != NULL)
    {
        msg_ptr = msg->msg;

        // Extension choice
        bool ext = liblte_bits_2_value(&msg_ptr, 1);

        // Optional indicator
        liblte_rrc_warning_not_handled(liblte_bits_2_value(&msg_ptr, 1), __func__);

        liblte_rrc_consume_noncrit_extension(ext, __func__, &msg_ptr);
        
        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    Message Name: Counter Check Response

    Description: Used by the UE to respond to a Counter Check message

    Document Reference: 36.331 v10.0.0 Section 6.2.2 
*********************************************************************/
// FIXME

/*********************************************************************
    Message Name: Counter Check

    Description: Used by the E-UTRAN to indicate the current COUNT MSB
                 values associated to each DRB and to request the UE
                 to compare these to its COUNT MSB values and to
                 report the comparison results to E-UTRAN

    Document Reference: 36.331 v10.0.0 Section 6.2.2 
*********************************************************************/
// FIXME

/*********************************************************************
    Message Name: BCCH BCH Message

    Description: Contains the set of RRC messages that may be sent
                 from the E-UTRAN to the UE via BCH on the BCCH
                 logical channel

    Document Reference: 36.331 v10.0.0 Sections 6.2.1 and 6.2.2
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_rrc_pack_bcch_bch_msg(LIBLTE_RRC_MIB_STRUCT *mib,
                                               LIBLTE_BIT_MSG_STRUCT *msg)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr;

    if(mib != NULL &&
       msg != NULL)
    {
        msg_ptr = msg->msg;

        // DL Bandwidth
        liblte_value_2_bits(mib->dl_bw, &msg_ptr, 3);

        // PHICH Config
        liblte_rrc_pack_phich_config_ie(&mib->phich_config, &msg_ptr);

        // SFN/4
        liblte_value_2_bits(mib->sfn_div_4, &msg_ptr, 8);

        // Spare
        liblte_value_2_bits(0, &msg_ptr, 10);

        // Fill in the number of bits used
        msg->N_bits = msg_ptr - msg->msg;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_unpack_bcch_bch_msg(LIBLTE_BIT_MSG_STRUCT *msg,
                                                 LIBLTE_RRC_MIB_STRUCT *mib)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr;

    if(msg != NULL &&
       mib != NULL)
    {
        msg_ptr = msg->msg;

        // DL Bandwidth
        mib->dl_bw = (LIBLTE_RRC_DL_BANDWIDTH_ENUM)liblte_bits_2_value(&msg_ptr, 3);

        // PHICH Config
        liblte_rrc_unpack_phich_config_ie(&msg_ptr, &mib->phich_config);

        // SFN/4
        mib->sfn_div_4 = liblte_bits_2_value(&msg_ptr, 8);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    Message Name: BCCH DLSCH Message

    Description: Contains the set of RRC messages that may be sent
                 from the E-UTRAN to the UE via DLSCH on the BCCH
                 logical channel

    Document Reference: 36.331 v10.0.0 Section 6.2.1
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_rrc_pack_bcch_dlsch_msg(LIBLTE_RRC_BCCH_DLSCH_MSG_STRUCT *bcch_dlsch_msg,
                                                 LIBLTE_BIT_MSG_STRUCT            *msg)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr;
    uint8              ext     = false;

    if(bcch_dlsch_msg != NULL &&
       msg            != NULL)
    {
        msg_ptr = msg->msg;

        // Extension indicator
        liblte_value_2_bits(ext, &msg_ptr, 1);

        if(LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_1 == bcch_dlsch_msg->sibs[0].sib_type)
        {
            // SIB1 Choice
            liblte_value_2_bits(1, &msg_ptr, 1);

            err = liblte_rrc_pack_sys_info_block_type_1_msg((LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_1_STRUCT *)&bcch_dlsch_msg->sibs[0].sib,
                                                            &global_msg);
            if(global_msg.N_bits <= (LIBLTE_MAX_MSG_SIZE_BITS - 2))
            {
                memcpy(msg_ptr, global_msg.msg, global_msg.N_bits);
                msg->N_bits = global_msg.N_bits + 2;
            }else{
                msg->N_bits = 0;
                err         = LIBLTE_ERROR_INVALID_INPUTS;
            }
        }else{
            // SIB1 Choice
            liblte_value_2_bits(0, &msg_ptr, 1);

            err = liblte_rrc_pack_sys_info_msg(bcch_dlsch_msg,
                                               &global_msg);
            if(global_msg.N_bits <= (LIBLTE_MAX_MSG_SIZE_BITS - 2))
            {
                memcpy(msg_ptr, global_msg.msg, global_msg.N_bits);
                msg->N_bits = global_msg.N_bits + 2;
            }else{
                msg->N_bits = 0;
                err         = LIBLTE_ERROR_INVALID_INPUTS;
            }
        }
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_unpack_bcch_dlsch_msg(LIBLTE_BIT_MSG_STRUCT            *msg,
                                                   LIBLTE_RRC_BCCH_DLSCH_MSG_STRUCT *bcch_dlsch_msg)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr;
    uint32             N_bits_used;
    uint8              ext;

    if(msg            != NULL &&
       bcch_dlsch_msg != NULL)
    {
        msg_ptr = msg->msg;

        // Extension indicator
        ext = liblte_bits_2_value(&msg_ptr, 1);

        // SIB1 Choice
        if(true == liblte_bits_2_value(&msg_ptr, 1))
        {
            bcch_dlsch_msg->N_sibs           = 1;
            bcch_dlsch_msg->sibs[0].sib_type = LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_1;
            if((msg->N_bits-(msg_ptr-msg->msg)) <= (LIBLTE_MAX_MSG_SIZE_BITS - 2))
            {
                memcpy(global_msg.msg, msg_ptr, msg->N_bits-(msg_ptr-msg->msg));
                global_msg.N_bits = msg->N_bits-(msg_ptr-msg->msg);
                err               = liblte_rrc_unpack_sys_info_block_type_1_msg(&global_msg,
                                                                                (LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_1_STRUCT *)&bcch_dlsch_msg->sibs[0].sib,
                                                                                &N_bits_used);
                msg_ptr += N_bits_used;
            }
        }else{
            if((msg->N_bits-(msg_ptr-msg->msg)) <= (LIBLTE_MAX_MSG_SIZE_BITS - 2))
            {
                memcpy(global_msg.msg, msg_ptr, msg->N_bits-(msg_ptr-msg->msg));
                err = liblte_rrc_unpack_sys_info_msg(&global_msg,
                                                     bcch_dlsch_msg);
            }
        }
        
        liblte_rrc_consume_noncrit_extension(ext, __func__, &msg_ptr);
    }

    return(err);
}

/*********************************************************************
    Message Name: MCCH Message

    Description: Contains the set of RRC messages that may be sent
                 from the E-UTRAN to the UE on the MCCH logical
                 channel

    Document Reference: 36.331 v10.0.0 Section 6.2.1
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_rrc_pack_mcch_msg(LIBLTE_RRC_MCCH_MSG_STRUCT *mcch_msg,
                                           LIBLTE_BIT_MSG_STRUCT      *msg)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr;
    uint8              ext     = false;

    if(mcch_msg != NULL &&
       msg      != NULL)
    {
        msg_ptr = msg->msg;

        // MCCH choice
        liblte_value_2_bits(0, &msg_ptr, 1);

        err = liblte_rrc_pack_mbsfn_area_configuration_r9_msg(mcch_msg,
                                                              &global_msg);
        if(global_msg.N_bits <= (LIBLTE_MAX_MSG_SIZE_BITS - 1))
        {
            memcpy(msg_ptr, global_msg.msg, global_msg.N_bits);
            msg->N_bits = global_msg.N_bits + 1;
        }else{
            msg->N_bits = 0;
            err         = LIBLTE_ERROR_INVALID_INPUTS;
        }
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_unpack_mcch_msg(LIBLTE_BIT_MSG_STRUCT      *msg,
                                             LIBLTE_RRC_MCCH_MSG_STRUCT *mcch_msg)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr;
    uint32             N_bits_used;

    if(msg      != NULL &&
       mcch_msg != NULL)
    {
        msg_ptr = msg->msg;

        // MCCH choice
        liblte_rrc_warning_not_handled(liblte_bits_2_value(&msg_ptr, 1), __func__);

        if((msg->N_bits-(msg_ptr-msg->msg)) <= (LIBLTE_MAX_MSG_SIZE_BITS - 1))
        {
            memcpy(global_msg.msg, msg_ptr, msg->N_bits-(msg_ptr-msg->msg));
            err = liblte_rrc_unpack_mbsfn_area_configuration_r9_msg(&global_msg,
                                                                    mcch_msg);
        }
    }

    return(err);
}

/*********************************************************************
    Message Name: PCCH Message

    Description: Contains the set of RRC messages that may be sent
                 from the E-UTRAN to the UE on the PCCH logical
                 channel

    Document Reference: 36.331 v10.0.0 Section 6.2.1
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_rrc_pack_pcch_msg(LIBLTE_RRC_PCCH_MSG_STRUCT *pcch_msg,
                                           LIBLTE_BIT_MSG_STRUCT      *msg)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr;
    uint8              ext     = false;

    if(pcch_msg != NULL &&
       msg      != NULL)
    {
        msg_ptr = msg->msg;

        // Paging choice
        liblte_value_2_bits(0, &msg_ptr, 1);

        err = liblte_rrc_pack_paging_msg(pcch_msg,
                                         &global_msg);
        if(global_msg.N_bits <= (LIBLTE_MAX_MSG_SIZE_BITS - 1))
        {
            memcpy(msg_ptr, global_msg.msg, global_msg.N_bits);
            msg->N_bits = global_msg.N_bits + 1;
        }else{
            msg->N_bits = 0;
            err         = LIBLTE_ERROR_INVALID_INPUTS;
        }
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_unpack_pcch_msg(LIBLTE_BIT_MSG_STRUCT      *msg,
                                             LIBLTE_RRC_PCCH_MSG_STRUCT *pcch_msg)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr;
    uint32             N_bits_used;

    if(msg      != NULL &&
       pcch_msg != NULL)
    {
        msg_ptr = msg->msg;

        // Paging choice
        liblte_rrc_warning_not_handled(liblte_bits_2_value(&msg_ptr, 1), __func__);

        if((msg->N_bits-(msg_ptr-msg->msg)) <= (LIBLTE_MAX_MSG_SIZE_BITS - 1))
        {
            memcpy(global_msg.msg, msg_ptr, msg->N_bits-(msg_ptr-msg->msg));
            err = liblte_rrc_unpack_paging_msg(&global_msg,
                                               pcch_msg);
        }
    }

    return(err);
}

/*********************************************************************
    Message Name: DL CCCH Message

    Description: Contains the set of RRC messages that may be sent
                 from the E-UTRAN to the UE on the downlink CCCH
                 logical channel

    Document Reference: 36.331 v10.0.0 Section 6.2.1
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_rrc_pack_dl_ccch_msg(LIBLTE_RRC_DL_CCCH_MSG_STRUCT *dl_ccch_msg,
                                              LIBLTE_BIT_MSG_STRUCT         *msg)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr;
    uint8              ext     = false;

    if(dl_ccch_msg != NULL &&
       msg         != NULL)
    {
        msg_ptr = msg->msg;

        // Extension indicator
        liblte_value_2_bits(ext, &msg_ptr, 1);

        // Message type choice
        liblte_value_2_bits(dl_ccch_msg->msg_type, &msg_ptr, 2);

        if(LIBLTE_RRC_DL_CCCH_MSG_TYPE_RRC_CON_REEST == dl_ccch_msg->msg_type)
        {
            err = liblte_rrc_pack_rrc_connection_reestablishment_msg((LIBLTE_RRC_CONNECTION_REESTABLISHMENT_STRUCT *)&dl_ccch_msg->msg,
                                                                     &global_msg);
        }else if(LIBLTE_RRC_DL_CCCH_MSG_TYPE_RRC_CON_REEST_REJ == dl_ccch_msg->msg_type){
            err = liblte_rrc_pack_rrc_connection_reestablishment_reject_msg((LIBLTE_RRC_CONNECTION_REESTABLISHMENT_REJECT_STRUCT *)&dl_ccch_msg->msg,
                                                                            &global_msg);
        }else if(LIBLTE_RRC_DL_CCCH_MSG_TYPE_RRC_CON_REJ == dl_ccch_msg->msg_type){
            err = liblte_rrc_pack_rrc_connection_reject_msg((LIBLTE_RRC_CONNECTION_REJECT_STRUCT *)&dl_ccch_msg->msg,
                                                            &global_msg);
        }else{ // LIBLTE_RRC_DL_CCCH_MSG_TYPE_RRC_CON_SETUP == dl_ccch_msg->msg_type
            err = liblte_rrc_pack_rrc_connection_setup_msg((LIBLTE_RRC_CONNECTION_SETUP_STRUCT *)&dl_ccch_msg->msg,
                                                           &global_msg);
        }

        if(global_msg.N_bits <= (LIBLTE_MAX_MSG_SIZE_BITS - 3))
        {
            memcpy(msg_ptr, global_msg.msg, global_msg.N_bits);
            msg->N_bits = global_msg.N_bits + 3;
        }else{
            msg->N_bits = 0;
            err         = LIBLTE_ERROR_INVALID_INPUTS;
        }
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_unpack_dl_ccch_msg(LIBLTE_BIT_MSG_STRUCT         *msg,
                                                LIBLTE_RRC_DL_CCCH_MSG_STRUCT *dl_ccch_msg)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr;
    uint8              ext;

    if(msg         != NULL &&
       dl_ccch_msg != NULL)
    {
        msg_ptr = msg->msg;

        // Extension indicator
        ext = liblte_bits_2_value(&msg_ptr, 1);

        // Message type choice
        dl_ccch_msg->msg_type = (LIBLTE_RRC_DL_CCCH_MSG_TYPE_ENUM)liblte_bits_2_value(&msg_ptr, 2);

        // Message
        memcpy(global_msg.msg, msg_ptr, msg->N_bits-(msg_ptr-msg->msg));
        global_msg.N_bits = msg->N_bits-(msg_ptr-msg->msg);
        if(LIBLTE_RRC_DL_CCCH_MSG_TYPE_RRC_CON_REEST == dl_ccch_msg->msg_type)
        {
            err = liblte_rrc_unpack_rrc_connection_reestablishment_msg(&global_msg,
                                                                       (LIBLTE_RRC_CONNECTION_REESTABLISHMENT_STRUCT *)&dl_ccch_msg->msg);
        }else if(LIBLTE_RRC_DL_CCCH_MSG_TYPE_RRC_CON_REEST_REJ == dl_ccch_msg->msg_type){
            err = liblte_rrc_unpack_rrc_connection_reestablishment_reject_msg(&global_msg,
                                                                              (LIBLTE_RRC_CONNECTION_REESTABLISHMENT_REJECT_STRUCT *)&dl_ccch_msg->msg);
        }else if(LIBLTE_RRC_DL_CCCH_MSG_TYPE_RRC_CON_REJ == dl_ccch_msg->msg_type){
            err = liblte_rrc_unpack_rrc_connection_reject_msg(&global_msg,
                                                              (LIBLTE_RRC_CONNECTION_REJECT_STRUCT *)&dl_ccch_msg->msg);
        }else{ // LIBLTE_RRC_DL_CCCH_MSG_TYPE_RRC_CON_SETUP == dl_ccch_msg->msg_type
            err = liblte_rrc_unpack_rrc_connection_setup_msg(&global_msg,
                                                             (LIBLTE_RRC_CONNECTION_SETUP_STRUCT *)&dl_ccch_msg->msg);
        }
        
        liblte_rrc_consume_noncrit_extension(ext, __func__, &msg_ptr);
    }

    return(err);
}

/*********************************************************************
    Message Name: DL DCCH Message

    Description: Contains the set of RRC messages that may be sent
                 from the E-UTRAN to the UE on the downlink DCCH
                 logical channel

    Document Reference: 36.331 v10.0.0 Section 6.2.1
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_rrc_pack_dl_dcch_msg(LIBLTE_RRC_DL_DCCH_MSG_STRUCT *dl_dcch_msg,
                                              LIBLTE_BIT_MSG_STRUCT         *msg)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr;
    uint8              ext     = false;

    if(dl_dcch_msg != NULL &&
       msg         != NULL)
    {
        msg_ptr = msg->msg;

        // Extension indicator
        liblte_value_2_bits(ext, &msg_ptr, 1);

        // Message type choice
        liblte_value_2_bits(dl_dcch_msg->msg_type, &msg_ptr, 4);

        // Message
        if(LIBLTE_RRC_DL_DCCH_MSG_TYPE_CSFB_PARAMS_RESP_CDMA2000 == dl_dcch_msg->msg_type)
        {
            printf("NOT HANDLING CSFB PARAMETERS RESPONSE CDMA2000\n");
//            err = liblte_rrc_pack_csfb_parameters_response_cdma2000_msg((LIBLTE_RRC_CSFB_PARAMETERS_RESPONSE_CDMA2000_STRUCT *)&dl_dcch_msg->msg,
//                                                                        &global_msg);
        }else if(LIBLTE_RRC_DL_DCCH_MSG_TYPE_DL_INFO_TRANSFER == dl_dcch_msg->msg_type){
            err = liblte_rrc_pack_dl_information_transfer_msg((LIBLTE_RRC_DL_INFORMATION_TRANSFER_STRUCT *)&dl_dcch_msg->msg,
                                                              &global_msg);
        }else if(LIBLTE_RRC_DL_DCCH_MSG_TYPE_HANDOVER_FROM_EUTRA_PREP_REQ == dl_dcch_msg->msg_type){
            printf("NOT HANDLING HANDOVER FROM EUTRA PREPARATION REQUEST\n");
//            err = liblte_rrc_pack_handover_from_eutra_preparation_request_msg((LIBLTE_RRC_HANDOVER_FROM_EUTRA_PREPARATION_REQUEST_STRUCT *)&dl_dcch_msg->msg,
//                                                                              &global_msg);
        }else if(LIBLTE_RRC_DL_DCCH_MSG_TYPE_MOBILITY_FROM_EUTRA_COMMAND == dl_dcch_msg->msg_type){
            printf("NOT HANDLING MOBILITY FROM EUTRA COMMAND\n");
//            err = liblte_rrc_pack_mobility_from_eutra_command_msg((LIBLTE_RRC_MOBILITY_FROM_EUTRA_COMMAND_STRUCT *)&dl_dcch_msg->msg,
//                                                                  &global_msg);
        }else if(LIBLTE_RRC_DL_DCCH_MSG_TYPE_RRC_CON_RECONFIG == dl_dcch_msg->msg_type){
            err = liblte_rrc_pack_rrc_connection_reconfiguration_msg((LIBLTE_RRC_CONNECTION_RECONFIGURATION_STRUCT *)&dl_dcch_msg->msg,
                                                                     &global_msg);
        }else if(LIBLTE_RRC_DL_DCCH_MSG_TYPE_RRC_CON_RELEASE == dl_dcch_msg->msg_type){
            err = liblte_rrc_pack_rrc_connection_release_msg((LIBLTE_RRC_CONNECTION_RELEASE_STRUCT *)&dl_dcch_msg->msg,
                                                             &global_msg);
        }else if(LIBLTE_RRC_DL_DCCH_MSG_TYPE_SECURITY_MODE_COMMAND == dl_dcch_msg->msg_type){
            err = liblte_rrc_pack_security_mode_command_msg((LIBLTE_RRC_SECURITY_MODE_COMMAND_STRUCT *)&dl_dcch_msg->msg,
                                                            &global_msg);
        }else if(LIBLTE_RRC_DL_DCCH_MSG_TYPE_UE_CAPABILITY_ENQUIRY == dl_dcch_msg->msg_type){
            err = liblte_rrc_pack_ue_capability_enquiry_msg((LIBLTE_RRC_UE_CAPABILITY_ENQUIRY_STRUCT *)&dl_dcch_msg->msg,
                                                            &global_msg);
        }else if(LIBLTE_RRC_DL_DCCH_MSG_TYPE_COUNTER_CHECK == dl_dcch_msg->msg_type){
            printf("NOT HANDLING COUNTER CHECK\n");
//            err = liblte_rrc_pack_counter_check_msg((LIBLTE_RRC_COUNTER_CHECK_STRUCT *)&dl_dcch_msg->msg,
//                                                    &global_msg);
        }else if(LIBLTE_RRC_DL_DCCH_MSG_TYPE_UE_INFO_REQ == dl_dcch_msg->msg_type){
            err = liblte_rrc_pack_ue_information_request_msg((LIBLTE_RRC_UE_INFORMATION_REQUEST_STRUCT *)&dl_dcch_msg->msg,
                                                             &global_msg);
        }else if(LIBLTE_RRC_DL_DCCH_MSG_TYPE_LOGGED_MEASUREMENTS_CONFIG == dl_dcch_msg->msg_type){
            printf("NOT HANDLING LOGGED MEASUREMENTS CONFIGURATION\n");
//            err = liblte_rrc_pack_logged_measurements_configuration_msg((LIBLTE_RRC_LOGGED_MEASUREMENTS_CONFIGURATION_STRUCT *)&dl_dcch_msg->msg,
//                                                                        &global_msg);
        }else{ // LIBLTE_RRC_DL_DCCH_MSG_TYPE_RN_RECONFIG == dl_dcch_msg->msg_type
            printf("NOT HANDLING RN RECONFIGURATION\n");
//            err = liblte_rrc_pack_rn_reconfiguration_msg((LIBLTE_RRC_RN_RECONFIGURATION_STRUCT *)&dl_dcch_msg->msg,
//                                                         &global_msg);
        }

        if(global_msg.N_bits <= (LIBLTE_MAX_MSG_SIZE_BITS - 5))
        {
            memcpy(msg_ptr, global_msg.msg, global_msg.N_bits);
            msg->N_bits = global_msg.N_bits + 5;
        }else{
            msg->N_bits = 0;
            err         = LIBLTE_ERROR_INVALID_INPUTS;
        }
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_unpack_dl_dcch_msg(LIBLTE_BIT_MSG_STRUCT         *msg,
                                                LIBLTE_RRC_DL_DCCH_MSG_STRUCT *dl_dcch_msg)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr;
    uint8              ext;

    if(msg         != NULL &&
       dl_dcch_msg != NULL)
    {
        msg_ptr = msg->msg;

        // Extension indicator
        ext = liblte_bits_2_value(&msg_ptr, 1);

        // Message type choice
        dl_dcch_msg->msg_type = (LIBLTE_RRC_DL_DCCH_MSG_TYPE_ENUM)liblte_bits_2_value(&msg_ptr, 4);

        // Message
        memcpy(global_msg.msg, msg_ptr, msg->N_bits-(msg_ptr-msg->msg));
        global_msg.N_bits = msg->N_bits-(msg_ptr-msg->msg);
        if(LIBLTE_RRC_DL_DCCH_MSG_TYPE_CSFB_PARAMS_RESP_CDMA2000 == dl_dcch_msg->msg_type)
        {
            printf("NOT HANDLING CSFB PARAMETERS RESPONSE CDMA2000\n");
//            err = liblte_rrc_unpack_csfb_parameters_response_cdma2000_msg(&global_msg,
//                                                                          (LIBLTE_RRC_CSFB_PARAMETERS_RESPONSE_CDMA2000_STRUCT *)&dl_dcch_msg->msg);
        }else if(LIBLTE_RRC_DL_DCCH_MSG_TYPE_DL_INFO_TRANSFER == dl_dcch_msg->msg_type){
            err = liblte_rrc_unpack_dl_information_transfer_msg(&global_msg,
                                                                (LIBLTE_RRC_DL_INFORMATION_TRANSFER_STRUCT *)&dl_dcch_msg->msg);
        }else if(LIBLTE_RRC_DL_DCCH_MSG_TYPE_HANDOVER_FROM_EUTRA_PREP_REQ == dl_dcch_msg->msg_type){
            printf("NOT HANDLING HANDOVER FROM EUTRA PREPARATION REQUEST\n");
//            err = liblte_rrc_unpack_handover_from_eutra_preparation_request_msg(&global_msg,
//                                                                                (LIBLTE_RRC_HANDOVER_FROM_EUTRA_PREPARATION_REQUEST_STRUCT *)&dl_dcch_msg->msg);
        }else if(LIBLTE_RRC_DL_DCCH_MSG_TYPE_MOBILITY_FROM_EUTRA_COMMAND == dl_dcch_msg->msg_type){
            printf("NOT HANDLING MOBILITY FROM EUTRA COMMAND\n");
//            err = liblte_rrc_unpack_mobility_from_eutra_command_msg(&global_msg,
//                                                                    (LIBLTE_RRC_MOBILITY_FROM_EUTRA_COMMAND_STRUCT *)&dl_dcch_msg->msg);
        }else if(LIBLTE_RRC_DL_DCCH_MSG_TYPE_RRC_CON_RECONFIG == dl_dcch_msg->msg_type){
            err = liblte_rrc_unpack_rrc_connection_reconfiguration_msg(&global_msg,
                                                                       (LIBLTE_RRC_CONNECTION_RECONFIGURATION_STRUCT *)&dl_dcch_msg->msg);
        }else if(LIBLTE_RRC_DL_DCCH_MSG_TYPE_RRC_CON_RELEASE == dl_dcch_msg->msg_type){
            err = liblte_rrc_unpack_rrc_connection_release_msg(&global_msg,
                                                               (LIBLTE_RRC_CONNECTION_RELEASE_STRUCT *)&dl_dcch_msg->msg);
        }else if(LIBLTE_RRC_DL_DCCH_MSG_TYPE_SECURITY_MODE_COMMAND == dl_dcch_msg->msg_type){
            err = liblte_rrc_unpack_security_mode_command_msg(&global_msg,
                                                              (LIBLTE_RRC_SECURITY_MODE_COMMAND_STRUCT *)&dl_dcch_msg->msg);
        }else if(LIBLTE_RRC_DL_DCCH_MSG_TYPE_UE_CAPABILITY_ENQUIRY == dl_dcch_msg->msg_type){
            err = liblte_rrc_unpack_ue_capability_enquiry_msg(&global_msg,
                                                              (LIBLTE_RRC_UE_CAPABILITY_ENQUIRY_STRUCT *)&dl_dcch_msg->msg);
        }else if(LIBLTE_RRC_DL_DCCH_MSG_TYPE_COUNTER_CHECK == dl_dcch_msg->msg_type){
            printf("NOT HANDLING COUNTER CHECK\n");
//            err = liblte_rrc_unpack_counter_check_msg(&global_msg,
//                                                      (LIBLTE_RRC_COUNTER_CHECK_STRUCT *)&dl_dcch_msg->msg);
        }else if(LIBLTE_RRC_DL_DCCH_MSG_TYPE_UE_INFO_REQ == dl_dcch_msg->msg_type){
            err = liblte_rrc_unpack_ue_information_request_msg(&global_msg,
                                                               (LIBLTE_RRC_UE_INFORMATION_REQUEST_STRUCT *)&dl_dcch_msg->msg);
        }else if(LIBLTE_RRC_DL_DCCH_MSG_TYPE_LOGGED_MEASUREMENTS_CONFIG == dl_dcch_msg->msg_type){
            printf("NOT HANDLING LOGGED MEASUREMENTS CONFIGURATION\n");
//            err = liblte_rrc_unpack_logged_measurements_configuration_msg(&global_msg,
//                                                                          (LIBLTE_RRC_LOGGED_MEASUREMENTS_CONFIGURATION_STRUCT *)&dl_dcch_msg->msg);
        }else{ // LIBLTE_RRC_DL_DCCH_MSG_TYPE_RN_RECONFIG == dl_dcch_msg->msg_type
            printf("NOT HANDLING RN RECONFIGURATION\n");
//            err = liblte_rrc_unpack_rn_reconfiguration_msg(&global_msg,
//                                                           (LIBLTE_RRC_RN_RECONFIGURATION_STRUCT *)&dl_dcch_msg->msg);
        }
        
        liblte_rrc_consume_noncrit_extension(ext, __func__, &msg_ptr);
    }

    return(err);
}

/*********************************************************************
    Message Name: UL CCCH Message

    Description: Contains the set of RRC messages that may be sent
                 from the UE to the E-UTRAN on the uplink CCCH
                 logical channel

    Document Reference: 36.331 v10.0.0 Section 6.2.1
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_rrc_pack_ul_ccch_msg(LIBLTE_RRC_UL_CCCH_MSG_STRUCT *ul_ccch_msg,
                                              LIBLTE_BIT_MSG_STRUCT         *msg)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr;
    uint8              ext     = false;

    if(ul_ccch_msg != NULL &&
       msg         != NULL)
    {
        msg_ptr = msg->msg;

        // Extension indicator
        liblte_value_2_bits(ext, &msg_ptr, 1);

        // Message type choice
        liblte_value_2_bits(ul_ccch_msg->msg_type, &msg_ptr, 1);

        if(LIBLTE_RRC_UL_CCCH_MSG_TYPE_RRC_CON_REEST_REQ == ul_ccch_msg->msg_type)
        {
            err = liblte_rrc_pack_rrc_connection_reestablishment_request_msg((LIBLTE_RRC_CONNECTION_REESTABLISHMENT_REQUEST_STRUCT *)&ul_ccch_msg->msg,
                                                                             &global_msg);
        }else{ // LIBLTE_RRC_UL_CCCH_MSG_TYPE_RRC_CON_REQ == ul_ccch_msg->msg_type
            err = liblte_rrc_pack_rrc_connection_request_msg((LIBLTE_RRC_CONNECTION_REQUEST_STRUCT *)&ul_ccch_msg->msg,
                                                             &global_msg);
        }

        if(global_msg.N_bits <= (LIBLTE_MAX_MSG_SIZE_BITS - 2))
        {
            memcpy(msg_ptr, global_msg.msg, global_msg.N_bits);
            msg->N_bits = global_msg.N_bits + 2;
        }else{
            msg->N_bits = 0;
            err         = LIBLTE_ERROR_INVALID_INPUTS;
        }
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_unpack_ul_ccch_msg(LIBLTE_BIT_MSG_STRUCT         *msg,
                                                LIBLTE_RRC_UL_CCCH_MSG_STRUCT *ul_ccch_msg)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr;
    uint8              ext;

    if(msg         != NULL &&
       ul_ccch_msg != NULL)
    {
        msg_ptr = msg->msg;

        // Extension indicator
        ext = liblte_bits_2_value(&msg_ptr, 1);

        // Message type choice
        ul_ccch_msg->msg_type = (LIBLTE_RRC_UL_CCCH_MSG_TYPE_ENUM)liblte_bits_2_value(&msg_ptr, 1);

        // Message
        memcpy(global_msg.msg, msg_ptr, msg->N_bits-(msg_ptr-msg->msg));
        global_msg.N_bits = msg->N_bits-(msg_ptr-msg->msg);
        if(LIBLTE_RRC_UL_CCCH_MSG_TYPE_RRC_CON_REEST_REQ == ul_ccch_msg->msg_type)
        {
            err = liblte_rrc_unpack_rrc_connection_reestablishment_request_msg(&global_msg,
                                                                               (LIBLTE_RRC_CONNECTION_REESTABLISHMENT_REQUEST_STRUCT *)&ul_ccch_msg->msg);
        }else{ // LIBLTE_RRC_UL_CCCH_MSG_TYPE_RRC_CON_REQ == ul_ccch_msg->msg_type
            err = liblte_rrc_unpack_rrc_connection_request_msg(&global_msg,
                                                               (LIBLTE_RRC_CONNECTION_REQUEST_STRUCT *)&ul_ccch_msg->msg);
        }
        
        liblte_rrc_consume_noncrit_extension(ext, __func__, &msg_ptr);
    }

    return(err);
}

/*********************************************************************
    Message Name: UL DCCH Message

    Description: Contains the set of RRC messages that may be sent
                 from the UE to the E-UTRAN on the uplink DCCH
                 logical channel

    Document Reference: 36.331 v10.0.0 Section 6.2.1
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_rrc_pack_ul_dcch_msg(LIBLTE_RRC_UL_DCCH_MSG_STRUCT *ul_dcch_msg,
                                              LIBLTE_BIT_MSG_STRUCT         *msg)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr;
    uint8              ext     = false;

    if(ul_dcch_msg != NULL &&
       msg         != NULL)
    {
        msg_ptr = msg->msg;

        // Extension indicator
        liblte_value_2_bits(ext, &msg_ptr, 1);

        // Message type choice
        liblte_value_2_bits(ul_dcch_msg->msg_type, &msg_ptr, 4);

        // Message
        if(LIBLTE_RRC_UL_DCCH_MSG_TYPE_CSFB_PARAMS_REQ_CDMA2000 == ul_dcch_msg->msg_type)
        {
            err = liblte_rrc_pack_csfb_parameters_request_cdma2000_msg((LIBLTE_RRC_CSFB_PARAMETERS_REQUEST_CDMA2000_STRUCT *)&ul_dcch_msg->msg,
                                                                       &global_msg);
        }else if(LIBLTE_RRC_UL_DCCH_MSG_TYPE_MEASUREMENT_REPORT == ul_dcch_msg->msg_type){
            err = liblte_rrc_pack_measurement_report_msg((LIBLTE_RRC_MEASUREMENT_REPORT_STRUCT *)&ul_dcch_msg->msg,
                                                         &global_msg);
        }else if(LIBLTE_RRC_UL_DCCH_MSG_TYPE_RRC_CON_RECONFIG_COMPLETE == ul_dcch_msg->msg_type){
            err = liblte_rrc_pack_rrc_connection_reconfiguration_complete_msg((LIBLTE_RRC_CONNECTION_RECONFIGURATION_COMPLETE_STRUCT *)&ul_dcch_msg->msg,
                                                                              &global_msg);
        }else if(LIBLTE_RRC_UL_DCCH_MSG_TYPE_RRC_CON_REEST_COMPLETE == ul_dcch_msg->msg_type){
            err = liblte_rrc_pack_rrc_connection_reestablishment_complete_msg((LIBLTE_RRC_CONNECTION_REESTABLISHMENT_COMPLETE_STRUCT *)&ul_dcch_msg->msg,
                                                                              &global_msg);
        }else if(LIBLTE_RRC_UL_DCCH_MSG_TYPE_RRC_CON_SETUP_COMPLETE == ul_dcch_msg->msg_type){
            err = liblte_rrc_pack_rrc_connection_setup_complete_msg((LIBLTE_RRC_CONNECTION_SETUP_COMPLETE_STRUCT *)&ul_dcch_msg->msg,
                                                                    &global_msg);
        }else if(LIBLTE_RRC_UL_DCCH_MSG_TYPE_SECURITY_MODE_COMPLETE == ul_dcch_msg->msg_type){
            err = liblte_rrc_pack_security_mode_complete_msg((LIBLTE_RRC_SECURITY_MODE_COMPLETE_STRUCT *)&ul_dcch_msg->msg,
                                                             &global_msg);
        }else if(LIBLTE_RRC_UL_DCCH_MSG_TYPE_SECURITY_MODE_FAILURE == ul_dcch_msg->msg_type){
            err = liblte_rrc_pack_security_mode_failure_msg((LIBLTE_RRC_SECURITY_MODE_FAILURE_STRUCT *)&ul_dcch_msg->msg,
                                                            &global_msg);
        }else if(LIBLTE_RRC_UL_DCCH_MSG_TYPE_UE_CAPABILITY_INFO == ul_dcch_msg->msg_type){
            err = liblte_rrc_pack_ue_capability_information_msg((LIBLTE_RRC_UE_CAPABILITY_INFORMATION_STRUCT *)&ul_dcch_msg->msg,
                                                                &global_msg);
        }else if(LIBLTE_RRC_UL_DCCH_MSG_TYPE_UL_HANDOVER_PREP_TRANSFER == ul_dcch_msg->msg_type){
            printf("NOT HANDLING UL HANDOVER PREPARATION TRANSFER\n");
//            err = liblte_rrc_pack_ul_handover_preparation_transfer_msg((LIBLTE_RRC_UL_HANDOVER_PREPARATION_TRANSFER_STRUCT *)&ul_dcch_msg->msg,
//                                                                       &global_msg);
        }else if(LIBLTE_RRC_UL_DCCH_MSG_TYPE_UL_INFO_TRANSFER == ul_dcch_msg->msg_type){
            err = liblte_rrc_pack_ul_information_transfer_msg((LIBLTE_RRC_UL_INFORMATION_TRANSFER_STRUCT *)&ul_dcch_msg->msg,
                                                              &global_msg);
        }else if(LIBLTE_RRC_UL_DCCH_MSG_TYPE_COUNTER_CHECK_RESP == ul_dcch_msg->msg_type){
            printf("NOT HANDLING COUNTER CHECK RESPONSE\n");
//            err = liblte_rrc_pack_counter_check_response_msg((LIBLTE_RRC_COUNTER_CHECK_RESPONSE_STRUCT *)&ul_dcch_msg->msg,
//                                                             &global_msg);
        }else if(LIBLTE_RRC_UL_DCCH_MSG_TYPE_UE_INFO_RESP == ul_dcch_msg->msg_type){
            printf("NOT HANDLING UE INFORMATION RESPONSE\n");
//            err = liblte_rrc_pack_ue_information_response_msg((LIBLTE_RRC_UE_INFORMATION_RESPONSE_STRUCT *)&ul_dcch_msg->msg,
//                                                              &global_msg);
        }else if(LIBLTE_RRC_UL_DCCH_MSG_TYPE_PROXIMITY_IND == ul_dcch_msg->msg_type){
            err = liblte_rrc_pack_proximity_indication_msg((LIBLTE_RRC_PROXIMITY_INDICATION_STRUCT *)&ul_dcch_msg->msg,
                                                           &global_msg);
        }else{ // LIBLTE_RRC_UL_DCCH_MSG_TYPE_RN_RECONFIG_COMPLETE == ul_dcch_msg->msg_type
            err = liblte_rrc_pack_rn_reconfiguration_complete_msg((LIBLTE_RRC_RN_RECONFIGURATION_COMPLETE_STRUCT *)&ul_dcch_msg->msg,
                                                                  &global_msg);
        }

        if(global_msg.N_bits <= (LIBLTE_MAX_MSG_SIZE_BITS - 5))
        {
            memcpy(msg_ptr, global_msg.msg, global_msg.N_bits);
            msg->N_bits = global_msg.N_bits + 5;
        }else{
            msg->N_bits = 0;
            err         = LIBLTE_ERROR_INVALID_INPUTS;
        }
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_rrc_unpack_ul_dcch_msg(LIBLTE_BIT_MSG_STRUCT         *msg,
                                                LIBLTE_RRC_UL_DCCH_MSG_STRUCT *ul_dcch_msg)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr;
    uint8              ext;

    if(msg         != NULL &&
       ul_dcch_msg != NULL)
    {
        msg_ptr = msg->msg;

        // Extension indicator
        ext = liblte_bits_2_value(&msg_ptr, 1);

        // Message type choice
        ul_dcch_msg->msg_type = (LIBLTE_RRC_UL_DCCH_MSG_TYPE_ENUM)liblte_bits_2_value(&msg_ptr, 4);

        // Message
        memcpy(global_msg.msg, msg_ptr, msg->N_bits-(msg_ptr-msg->msg));
        global_msg.N_bits = msg->N_bits-(msg_ptr-msg->msg);
        if(LIBLTE_RRC_UL_DCCH_MSG_TYPE_CSFB_PARAMS_REQ_CDMA2000 == ul_dcch_msg->msg_type)
        {
            err = liblte_rrc_unpack_csfb_parameters_request_cdma2000_msg(&global_msg,
                                                                         (LIBLTE_RRC_CSFB_PARAMETERS_REQUEST_CDMA2000_STRUCT *)&ul_dcch_msg->msg);
        }else if(LIBLTE_RRC_UL_DCCH_MSG_TYPE_MEASUREMENT_REPORT == ul_dcch_msg->msg_type){
            err = liblte_rrc_unpack_measurement_report_msg(&global_msg,
                                                           (LIBLTE_RRC_MEASUREMENT_REPORT_STRUCT *)&ul_dcch_msg->msg);
        }else if(LIBLTE_RRC_UL_DCCH_MSG_TYPE_RRC_CON_RECONFIG_COMPLETE == ul_dcch_msg->msg_type){
            err = liblte_rrc_unpack_rrc_connection_reconfiguration_complete_msg(&global_msg,
                                                                                (LIBLTE_RRC_CONNECTION_RECONFIGURATION_COMPLETE_STRUCT *)&ul_dcch_msg->msg);
        }else if(LIBLTE_RRC_UL_DCCH_MSG_TYPE_RRC_CON_REEST_COMPLETE == ul_dcch_msg->msg_type){
            err = liblte_rrc_unpack_rrc_connection_reestablishment_complete_msg(&global_msg,
                                                                                (LIBLTE_RRC_CONNECTION_REESTABLISHMENT_COMPLETE_STRUCT *)&ul_dcch_msg->msg);
        }else if(LIBLTE_RRC_UL_DCCH_MSG_TYPE_RRC_CON_SETUP_COMPLETE == ul_dcch_msg->msg_type){
            err = liblte_rrc_unpack_rrc_connection_setup_complete_msg(&global_msg,
                                                                      (LIBLTE_RRC_CONNECTION_SETUP_COMPLETE_STRUCT *)&ul_dcch_msg->msg);
        }else if(LIBLTE_RRC_UL_DCCH_MSG_TYPE_SECURITY_MODE_COMPLETE == ul_dcch_msg->msg_type){
            err = liblte_rrc_unpack_security_mode_complete_msg(&global_msg,
                                                               (LIBLTE_RRC_SECURITY_MODE_COMPLETE_STRUCT *)&ul_dcch_msg->msg);
        }else if(LIBLTE_RRC_UL_DCCH_MSG_TYPE_SECURITY_MODE_FAILURE == ul_dcch_msg->msg_type){
            err = liblte_rrc_unpack_security_mode_failure_msg(&global_msg,
                                                              (LIBLTE_RRC_SECURITY_MODE_FAILURE_STRUCT *)&ul_dcch_msg->msg);
        }else if(LIBLTE_RRC_UL_DCCH_MSG_TYPE_UE_CAPABILITY_INFO == ul_dcch_msg->msg_type){
            printf("NOT HANDLING UE CAPABILITY INFO\n");
//            err = liblte_rrc_unpack_ue_capability_information_msg(&global_msg,
//                                                                 (LIBLTE_RRC_UE_CAPABILITY_INFORMATION_STRUCT *)&ul_dcch_msg->msg);
        }else if(LIBLTE_RRC_UL_DCCH_MSG_TYPE_UL_HANDOVER_PREP_TRANSFER == ul_dcch_msg->msg_type){
            printf("NOT HANDLING UL HANDOVER PREPARATION TRANSFER\n");
//            err = liblte_rrc_unpack_ul_handover_preparation_transfer_msg(&global_msg,
//                                                                         (LIBLTE_RRC_UL_HANDOVER_PREPARATION_TRANSFER_STRUCT *)&ul_dcch_msg->msg);
        }else if(LIBLTE_RRC_UL_DCCH_MSG_TYPE_UL_INFO_TRANSFER == ul_dcch_msg->msg_type){
            err = liblte_rrc_unpack_ul_information_transfer_msg(&global_msg,
                                                                (LIBLTE_RRC_UL_INFORMATION_TRANSFER_STRUCT *)&ul_dcch_msg->msg);
        }else if(LIBLTE_RRC_UL_DCCH_MSG_TYPE_COUNTER_CHECK_RESP == ul_dcch_msg->msg_type){
            printf("NOT HANDLING COUNTER CHECK RESPONSE\n");
//            err = liblte_rrc_unpack_counter_check_response_msg(&global_msg,
//                                                               (LIBLTE_RRC_COUNTER_CHECK_RESPONSE_STRUCT *)&ul_dcch_msg->msg);
        }else if(LIBLTE_RRC_UL_DCCH_MSG_TYPE_UE_INFO_RESP == ul_dcch_msg->msg_type){
            printf("NOT HANDLING UE INFORMATION RESPONSE\n");
//            err = liblte_rrc_unpack_ue_information_response_msg(&global_msg,
//                                                                (LIBLTE_RRC_UE_INFORMATION_RESPONSE_STRUCT *)&ul_dcch_msg->msg);
        }else if(LIBLTE_RRC_UL_DCCH_MSG_TYPE_PROXIMITY_IND == ul_dcch_msg->msg_type){
            err = liblte_rrc_unpack_proximity_indication_msg(&global_msg,
                                                             (LIBLTE_RRC_PROXIMITY_INDICATION_STRUCT *)&ul_dcch_msg->msg);
        }else{ // LIBLTE_RRC_UL_DCCH_MSG_TYPE_RN_RECONFIG_COMPLETE == ul_dcch_msg->msg_type
            err = liblte_rrc_unpack_rn_reconfiguration_complete_msg(&global_msg,
                                                                    (LIBLTE_RRC_RN_RECONFIGURATION_COMPLETE_STRUCT *)&ul_dcch_msg->msg);
        }
        
        liblte_rrc_consume_noncrit_extension(ext, __func__, &msg_ptr);
        
    }

    return(err);
}
