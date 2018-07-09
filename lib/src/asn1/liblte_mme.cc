/*******************************************************************************

    Copyright 2014-2015 Ben Wojtowicz

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

    File: liblte_mme.cc

    Description: Contains all the implementations for the LTE Mobility
                 Management Entity library.

    Revision History
    ----------    -------------    --------------------------------------------
    06/15/2014    Ben Wojtowicz    Created file.
    08/03/2014    Ben Wojtowicz    Added more decoding/encoding.
    09/03/2014    Ben Wojtowicz    Added more decoding/encoding and fixed MCC
                                   and MNC packing.
    11/01/2014    Ben Wojtowicz    Added more decoding/encoding.
    11/29/2014    Ben Wojtowicz    Added more decoding/encoding.
    12/16/2014    Ben Wojtowicz    Added more decoding/encoding.
    12/24/2014    Ben Wojtowicz    Cleaned up the Time Zone and Time IE.
    02/15/2015    Ben Wojtowicz    Added more decoding/encoding.

*******************************************************************************/

/*******************************************************************************
                              INCLUDES
*******************************************************************************/

#include "srslte/asn1/liblte_mme.h"
#include "srslte/common/liblte_security.h"

/*******************************************************************************
                              DEFINES
*******************************************************************************/


/*******************************************************************************
                              TYPEDEFS
*******************************************************************************/


/*******************************************************************************
                              GLOBAL VARIABLES
*******************************************************************************/


/*******************************************************************************
                              INFORMATION ELEMENT FUNCTIONS
*******************************************************************************/

/*********************************************************************
    IE Name: Additional Information

    Description: Provides additional information to upper layers in
                 relation to the generic NAS message transport
                 mechanism.

    Document Reference: 24.301 v10.2.0 Section 9.9.2.0
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_mme_pack_additional_information_ie(LIBLTE_MME_ADDITIONAL_INFORMATION_STRUCT  *add_info,
                                                            uint8                                    **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;
    uint32            i;

    if(add_info != NULL &&
       ie_ptr   != NULL)
    {
        (*ie_ptr)[0] = add_info->N_octets;
        for(i=0; i<add_info->N_octets; i++)
        {
            (*ie_ptr)[1+i] = add_info->info[i];
        }
        *ie_ptr += add_info->N_octets + 1;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_mme_unpack_additional_information_ie(uint8                                    **ie_ptr,
                                                              LIBLTE_MME_ADDITIONAL_INFORMATION_STRUCT  *add_info)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;
    uint32            i;

    if(ie_ptr   != NULL &&
       add_info != NULL)
    {
        add_info->N_octets = (*ie_ptr)[0];
        for(i=0; i<add_info->N_octets; i++)
        {
            add_info->info[i] = (*ie_ptr)[1+i];
        }
        *ie_ptr += add_info->N_octets + 1;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: Device Properties

    Description: Indicates if the UE is configured for NAS signalling
                 low priority.

    Document Reference: 24.301 v10.2.0 Section 9.9.2.0A
                        24.008 v10.2.0 Section 10.5.7.8
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_mme_pack_device_properties_ie(LIBLTE_MME_DEVICE_PROPERTIES_ENUM   device_props,
                                                       uint8                               bit_offset,
                                                       uint8                             **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr != NULL)
    {
        **ie_ptr |= device_props << bit_offset;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_mme_unpack_device_properties_ie(uint8                             **ie_ptr,
                                                         uint8                               bit_offset,
                                                         LIBLTE_MME_DEVICE_PROPERTIES_ENUM  *device_props)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr       != NULL &&
       device_props != NULL)
    {
        *device_props = (LIBLTE_MME_DEVICE_PROPERTIES_ENUM)((**ie_ptr >> bit_offset) & 0x01);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: EPS Bearer Context Status

    Description: Indicates the state of each EPS bearer context that
                 can be identified by an EPS bearer identity.

    Document Reference: 24.301 v10.2.0 Section 9.9.2.1
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_mme_pack_eps_bearer_context_status_ie(LIBLTE_MME_EPS_BEARER_CONTEXT_STATUS_STRUCT  *ebcs,
                                                               uint8                                       **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ebcs   != NULL &&
       ie_ptr != NULL)
    {
        (*ie_ptr)[0]  = 2;
        (*ie_ptr)[1]  = (ebcs->ebi[7] << 7);
        (*ie_ptr)[1] |= (ebcs->ebi[6] << 6);
        (*ie_ptr)[1] |= (ebcs->ebi[5] << 5);
        (*ie_ptr)[2]  = (ebcs->ebi[15] << 7);
        (*ie_ptr)[2] |= (ebcs->ebi[14] << 6);
        (*ie_ptr)[2] |= (ebcs->ebi[13] << 5);
        (*ie_ptr)[2] |= (ebcs->ebi[12] << 4);
        (*ie_ptr)[2] |= (ebcs->ebi[11] << 3);
        (*ie_ptr)[2] |= (ebcs->ebi[10] << 2);
        (*ie_ptr)[2] |= (ebcs->ebi[9] << 1);
        (*ie_ptr)[2] |= ebcs->ebi[8];
        *ie_ptr      += 3;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_mme_unpack_eps_bearer_context_status_ie(uint8                                       **ie_ptr,
                                                                 LIBLTE_MME_EPS_BEARER_CONTEXT_STATUS_STRUCT  *ebcs)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr != NULL &&
       ebcs   != NULL)
    {
        ebcs->ebi[5]   = ((*ie_ptr)[1] >> 5) & 0x01;
        ebcs->ebi[6]   = ((*ie_ptr)[1] >> 6) & 0x01;
        ebcs->ebi[7]   = ((*ie_ptr)[1] >> 7) & 0x01;
        ebcs->ebi[8]   = (*ie_ptr)[2] & 0x01;
        ebcs->ebi[9]   = ((*ie_ptr)[2] >> 1) & 0x01;
        ebcs->ebi[10]  = ((*ie_ptr)[2] >> 2) & 0x01;
        ebcs->ebi[11]  = ((*ie_ptr)[2] >> 3) & 0x01;
        ebcs->ebi[12]  = ((*ie_ptr)[2] >> 4) & 0x01;
        ebcs->ebi[13]  = ((*ie_ptr)[2] >> 5) & 0x01;
        ebcs->ebi[14]  = ((*ie_ptr)[2] >> 6) & 0x01;
        ebcs->ebi[15]  = ((*ie_ptr)[2] >> 7) & 0x01;
        *ie_ptr       += 3;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: Location Area Identification

    Description: Provides an unambiguous identification of location
                 areas within the area covered by the 3GPP system.

    Document Reference: 24.301 v10.2.0 Section 9.9.2.2
                        24.008 v10.2.0 Section 10.5.1.3
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_mme_pack_location_area_id_ie(LIBLTE_MME_LOCATION_AREA_ID_STRUCT  *lai,
                                                      uint8                              **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(lai    != NULL &&
       ie_ptr != NULL)
    {
        (*ie_ptr)[0] = (((lai->mcc/10) % 10) << 4) | ((lai->mcc/100) % 10);
        if(lai->mnc < 100)
        {
            (*ie_ptr)[1] = 0xF0 | (lai->mcc % 10);
            (*ie_ptr)[2] = ((lai->mnc % 10) << 4) | ((lai->mnc/10) % 10);
        }else{
            (*ie_ptr)[1] = ((lai->mnc % 10) << 4) | (lai->mcc % 10);
            (*ie_ptr)[2] = (((lai->mnc/10) % 10) << 4) | ((lai->mnc/100) % 10);
        }
        (*ie_ptr)[3]  = (lai->lac >> 8) & 0xFF;
        (*ie_ptr)[4]  = lai->lac & 0xFF;
        *ie_ptr      += 5;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_mme_unpack_location_area_id_ie(uint8                              **ie_ptr,
                                                        LIBLTE_MME_LOCATION_AREA_ID_STRUCT  *lai)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr != NULL &&
       lai    != NULL)
    {
        lai->mcc  = ((*ie_ptr)[0] & 0x0F)*100;
        lai->mcc += (((*ie_ptr)[0] >> 4) & 0x0F)*10;
        lai->mcc += (*ie_ptr)[1] & 0x0F;
        if((((*ie_ptr)[1] >> 4) & 0x0F) == 0x0F)
        {
            lai->mnc  = ((*ie_ptr)[2] & 0x0F)*10;
            lai->mnc += ((*ie_ptr)[2] >> 4) & 0x0F;
        }else{
            lai->mnc  = ((*ie_ptr)[1] >> 4) & 0x0F;
            lai->mnc += ((*ie_ptr)[2] & 0x0F)*100;
            lai->mnc += (((*ie_ptr)[2] >> 4) & 0x0F)*10;
        }
        lai->lac  = (*ie_ptr)[3] << 8;
        lai->lac |= (*ie_ptr)[4];
        *ie_ptr  += 5;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: Mobile Identity

    Description: Provides either the IMSI, TMSI/P-TMSI/M-TMSI, IMEI,
                 IMEISV, or TMGI, associated with the optional MBMS
                 session identity.

    Document Reference: 24.301 v10.2.0 Section 9.9.2.3
                        24.008 v10.2.0 Section 10.5.1.4
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_mme_pack_mobile_id_ie(LIBLTE_MME_MOBILE_ID_STRUCT  *mobile_id,
                                               uint8                       **ie_ptr)
{
    LIBLTE_ERROR_ENUM  err = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *id = NULL;
    uint32             id32 = 0;
    uint32             i;
    uint8              length = 0;
    bool               odd = false;

    if(mobile_id != NULL &&
       ie_ptr    != NULL)
    {
        if(LIBLTE_MME_MOBILE_ID_TYPE_IMSI == mobile_id->type_of_id)
        {
            id = mobile_id->imsi;
            length = 8;
            odd = true;
        }else if(LIBLTE_MME_MOBILE_ID_TYPE_IMEI == mobile_id->type_of_id){
            id = mobile_id->imei;
            length = 8;
            odd = true;
        }else if(LIBLTE_MME_MOBILE_ID_TYPE_IMEISV == mobile_id->type_of_id){
            id  = mobile_id->imeisv;
            length = 9;
            odd = false;
        }else if(LIBLTE_MME_MOBILE_ID_TYPE_TMSI == mobile_id->type_of_id){
            id32 = mobile_id->tmsi;
            length = 4;
            odd = false;
        }
        }else{
            // FIXME: Not handling these IDs
            return(err);
        }

        // Length
        **ie_ptr = length;
        *ie_ptr += 1;
        if(LIBLTE_MME_MOBILE_ID_TYPE_TMSI != mobile_id->type_of_id)
        {
            // | Identity digit 1 | odd/even | Id type |
            if(odd)
            {
                **ie_ptr  = (id[0] << 4) | (1 << 3) | mobile_id->type_of_id;
            }else{
                **ie_ptr  = (id[0] << 4) | (0 << 3) | mobile_id->type_of_id;
            }
            *ie_ptr  += 1;

        
            // | Identity digit p+1 | Identity digit p |
            for(i=0; i<7; i++)
            {
                (*ie_ptr)[i] = (id[i*2+2] << 4) | id[i*2+1];
            }
            *ie_ptr += 7;
            if(!odd)
            {
              **ie_ptr = 0xF0 | id[15];
              *ie_ptr += 1;
            }

            err = LIBLTE_SUCCESS;
        }
        else{

          **ie_ptr  = (0xFF << 4) | (0 << 3) | mobile_id->type_of_id;
          *ie_ptr  += 1;
          //4-Byte based ids
          **ie_ptr  = (id32 >> 24) & 0xFF;
          *ie_ptr  += 1;
          **ie_ptr  = (id32 >> 16) & 0xFF;
          *ie_ptr  += 1;
          **ie_ptr  = (id32 >> 8) & 0xFF;
          *ie_ptr  += 1;
          **ie_ptr  = id32 & 0xFF;
          *ie_ptr  += 1;

          err = LIBLTE_SUCCESS;
        }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_mme_unpack_mobile_id_ie(uint8                       **ie_ptr,
                                                 LIBLTE_MME_MOBILE_ID_STRUCT  *mobile_id)
{
    LIBLTE_ERROR_ENUM  err = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *id;
    uint32             length;
    uint32             i;
    bool               odd = false;

    if(ie_ptr    != NULL &&
       mobile_id != NULL)
    {
        length   = **ie_ptr;
        *ie_ptr += 1;

        mobile_id->type_of_id = **ie_ptr & 0x07;

        if(LIBLTE_MME_MOBILE_ID_TYPE_IMSI == mobile_id->type_of_id)
        {
            id  = mobile_id->imsi;
            odd = true;
        }else if(LIBLTE_MME_MOBILE_ID_TYPE_IMEI == mobile_id->type_of_id){
            id  = mobile_id->imei;
            odd = true;
        }else if(LIBLTE_MME_MOBILE_ID_TYPE_IMEISV == mobile_id->type_of_id){
            id  = mobile_id->imeisv;
            odd = false;
        }else{
            // FIXME: Not handling these IDs
            return(err);
        }

        id[0]    = **ie_ptr >> 4;
        *ie_ptr += 1;
        for(i=0; i<7; i++)
        {
            id[i*2+1] = (*ie_ptr)[i] & 0x0F;
            id[i*2+2] = (*ie_ptr)[i] >> 4;
        }
        if(odd)
        {
            *ie_ptr += 7;
        }else{
            id[i*2+1]  = (*ie_ptr)[i] & 0xF;
            *ie_ptr   += 8;
        }

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: Mobile Station Classmark 2

    Description: Provides the network with information concerning
                 aspects of both high and low priority of the UE.

    Document Reference: 24.301 v10.2.0 Section 9.9.2.4
                        24.008 v10.2.0 Section 10.5.1.6
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_mme_pack_mobile_station_classmark_2_ie(LIBLTE_MME_MOBILE_STATION_CLASSMARK_2_STRUCT  *ms_cm2,
                                                                uint8                                        **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ms_cm2 != NULL &&
       ie_ptr != NULL)
    {
        (*ie_ptr)[0]  = 3;
        (*ie_ptr)[1]  = (ms_cm2->rev_lev & 0x03) << 5;
        (*ie_ptr)[1] |= ms_cm2->es_ind << 4;
        (*ie_ptr)[1] |= ms_cm2->a5_1 << 3;
        (*ie_ptr)[1] |= ms_cm2->rf_power_cap & 0x07;
        (*ie_ptr)[2]  = ms_cm2->ps_cap << 6;
        (*ie_ptr)[2] |= (ms_cm2->ss_screen_ind & 0x03) << 4;
        (*ie_ptr)[2] |= ms_cm2->sm_cap << 3;
        (*ie_ptr)[2] |= ms_cm2->vbs << 2;
        (*ie_ptr)[2] |= ms_cm2->vgcs << 1;
        (*ie_ptr)[2] |= ms_cm2->fc;
        (*ie_ptr)[3]  = ms_cm2->cm3 << 7;
        (*ie_ptr)[3] |= ms_cm2->lcsva_cap << 5;
        (*ie_ptr)[3] |= ms_cm2->ucs2 << 4;
        (*ie_ptr)[3] |= ms_cm2->solsa << 3;
        (*ie_ptr)[3] |= ms_cm2->cmsp << 2;
        (*ie_ptr)[3] |= ms_cm2->a5_3 << 1;
        (*ie_ptr)[3] |= ms_cm2->a5_2;
        *ie_ptr      += 4;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_mme_unpack_mobile_station_classmark_2_ie(uint8                                        **ie_ptr,
                                                                  LIBLTE_MME_MOBILE_STATION_CLASSMARK_2_STRUCT  *ms_cm2)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr != NULL &&
       ms_cm2 != NULL)
    {
        ms_cm2->rev_lev        = (LIBLTE_MME_REVISION_LEVEL_ENUM)(((*ie_ptr)[1] >> 5) & 0x03);
        ms_cm2->es_ind         = ((*ie_ptr)[1] >> 4) & 0x01;
        ms_cm2->a5_1           = ((*ie_ptr)[1] >> 3) & 0x01;
        ms_cm2->rf_power_cap   = (LIBLTE_MME_RF_POWER_CAPABILITY_ENUM)((*ie_ptr)[1] & 0x07);
        ms_cm2->ps_cap         = ((*ie_ptr)[2] >> 6) & 0x01;
        ms_cm2->ss_screen_ind  = (LIBLTE_MME_SS_SCREEN_INDICATOR_ENUM)(((*ie_ptr)[2] >> 4) & 0x03);
        ms_cm2->sm_cap         = ((*ie_ptr)[2] >> 3) & 0x01;
        ms_cm2->vbs            = ((*ie_ptr)[2] >> 2) & 0x01;
        ms_cm2->vgcs           = ((*ie_ptr)[2] >> 1) & 0x01;
        ms_cm2->fc             = (*ie_ptr)[2] & 0x01;
        ms_cm2->cm3            = ((*ie_ptr)[3] >> 7) & 0x01;
        ms_cm2->lcsva_cap      = ((*ie_ptr)[3] >> 5) & 0x01;
        ms_cm2->ucs2           = ((*ie_ptr)[3] >> 4) & 0x01;
        ms_cm2->solsa          = ((*ie_ptr)[3] >> 3) & 0x01;
        ms_cm2->cmsp           = ((*ie_ptr)[3] >> 2) & 0x01;
        ms_cm2->a5_3           = ((*ie_ptr)[3] >> 1) & 0x01;
        ms_cm2->a5_2           = (*ie_ptr)[3] & 0x01;
        *ie_ptr               += 4;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: Mobile Station Classmark 3

    Description: Provides the network with information concerning
                 aspects of the UE.

    Document Reference: 24.301 v10.2.0 Section 9.9.2.5
                        24.008 v10.2.0 Section 10.5.1.7
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_mme_pack_mobile_station_classmark_3_ie(LIBLTE_MME_MOBILE_STATION_CLASSMARK_3_STRUCT  *ms_cm3,
                                                                uint8                                        **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ms_cm3 != NULL &&
       ie_ptr != NULL)
    {
        // FIXME

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_mme_unpack_mobile_station_classmark_3_ie(uint8                                        **ie_ptr,
                                                                  LIBLTE_MME_MOBILE_STATION_CLASSMARK_3_STRUCT  *ms_cm3)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr != NULL &&
       ms_cm3 != NULL)
    {
        // FIXME

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: NAS Security Parameters From E-UTRA

    Description: Provides the UE with information that enables the UE
                 to create a mapped UMTS security context.

    Document Reference: 24.301 v10.2.0 Section 9.9.2.6
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_mme_pack_nas_security_parameters_from_eutra_ie(uint8   dl_nas_count,
                                                                        uint8 **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr != NULL)
    {
        (*ie_ptr)[0]  = dl_nas_count & 0x0F;
        *ie_ptr      += 1;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_mme_unpack_nas_security_parameters_from_eutra_ie(uint8 **ie_ptr,
                                                                          uint8  *dl_nas_count)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr       != NULL &&
       dl_nas_count != NULL)
    {
        *dl_nas_count  = (*ie_ptr)[0];
        *ie_ptr       += 1;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: NAS Security Parameters To E-UTRA

    Description: Provides the UE with parameters that enables the UE
                 to create a mapped EPS security context and take
                 this context into use after inter-system handover to
                 S1 mode.

    Document Reference: 24.301 v10.2.0 Section 9.9.2.7
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_mme_pack_nas_security_parameters_to_eutra_ie(LIBLTE_MME_NAS_SECURITY_PARAMETERS_TO_EUTRA_STRUCT  *sec_params,
                                                                      uint8                                              **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(sec_params != NULL &&
       ie_ptr     != NULL)
    {
        (*ie_ptr)[0]  = (sec_params->nonce_mme >> 24) & 0xFF;
        (*ie_ptr)[1]  = (sec_params->nonce_mme >> 16) & 0xFF;
        (*ie_ptr)[2]  = (sec_params->nonce_mme >> 8) & 0xFF;
        (*ie_ptr)[3]  = sec_params->nonce_mme & 0xFF;
        (*ie_ptr)[4]  = (sec_params->eea & 0x07) << 4;
        (*ie_ptr)[4] |= sec_params->eia & 0x07;
        (*ie_ptr)[5]  = (sec_params->tsc_flag & 0x01) << 3;
        (*ie_ptr)[5] |= sec_params->nas_ksi & 0x07;
        *ie_ptr      += 6;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_mme_unpack_nas_security_parameters_to_eutra_ie(uint8                                              **ie_ptr,
                                                                        LIBLTE_MME_NAS_SECURITY_PARAMETERS_TO_EUTRA_STRUCT  *sec_params)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr     != NULL &&
       sec_params != NULL)
    {
        sec_params->nonce_mme  = (*ie_ptr)[0] << 24;
        sec_params->nonce_mme |= (*ie_ptr)[1] << 16;
        sec_params->nonce_mme |= (*ie_ptr)[2] << 8;
        sec_params->nonce_mme |= (*ie_ptr)[3];
        sec_params->eea        = (LIBLTE_MME_TYPE_OF_CIPHERING_ALGORITHM_ENUM)(((*ie_ptr)[4] >> 4) & 0x07);
        sec_params->eia        = (LIBLTE_MME_TYPE_OF_INTEGRITY_ALGORITHM_ENUM)((*ie_ptr)[4] & 0x07);
        sec_params->tsc_flag   = (LIBLTE_MME_TYPE_OF_SECURITY_CONTEXT_FLAG_ENUM)(((*ie_ptr)[5] >> 3) & 0x01);
        sec_params->nas_ksi    = (*ie_ptr)[5] & 0x07;
        *ie_ptr               += 6;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: PLMN List

    Description: Provides a list of PLMN codes to the UE.

    Document Reference: 24.301 v10.2.0 Section 9.9.2.8
                        24.008 v10.2.0 Section 10.5.1.13
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_mme_pack_plmn_list_ie(LIBLTE_MME_PLMN_LIST_STRUCT  *plmn_list,
                                               uint8                       **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;
    uint32            i;

    if(plmn_list != NULL &&
       ie_ptr    != NULL)
    {
        (*ie_ptr)[0] = plmn_list->N_plmns * 3;
        for(i=0; i<plmn_list->N_plmns; i++)
        {
            (*ie_ptr)[i*3+0] = (((plmn_list->mcc[i]/10) % 10) << 4) | ((plmn_list->mcc[i]/100) % 10);
            if(plmn_list->mnc[i] < 100)
            {
                (*ie_ptr)[i*3+1] = 0xF0 | (plmn_list->mcc[i] % 10);
                (*ie_ptr)[i*3+2] = ((plmn_list->mnc[i] % 10) << 4) | ((plmn_list->mnc[i]/10) % 10);
            }else{
                (*ie_ptr)[i*3+1] = ((plmn_list->mnc[i] % 10) << 4) | (plmn_list->mcc[i] % 10);
                (*ie_ptr)[i*3+2] = (((plmn_list->mnc[i]/10) % 10) << 4) | ((plmn_list->mnc[i]/100) % 10);
            }
        }
        *ie_ptr += (plmn_list->N_plmns * 3) + 1;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_mme_unpack_plmn_list_ie(uint8                       **ie_ptr,
                                                 LIBLTE_MME_PLMN_LIST_STRUCT  *plmn_list)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;
    uint32            i;

    if(ie_ptr    != NULL &&
       plmn_list != NULL)
    {
        plmn_list->N_plmns = (*ie_ptr)[0] / 3;
        for(i=0; i<plmn_list->N_plmns; i++)
        {
            plmn_list->mcc[i]  = ((*ie_ptr)[i*3+0] & 0x0F)*100;
            plmn_list->mcc[i] += (((*ie_ptr)[i*3+0] >> 4) & 0x0F)*10;
            plmn_list->mcc[i] += (*ie_ptr)[i*3+1] & 0x0F;
            if((((*ie_ptr)[i*3+1] >> 4) & 0x0F) == 0x0F)
            {
                plmn_list->mnc[i]  = ((*ie_ptr)[i*3+2] & 0x0F)*10;
                plmn_list->mnc[i] += ((*ie_ptr)[i*3+2] >> 4) & 0x0F;
            }else{
                plmn_list->mnc[i]  = ((*ie_ptr)[i*3+1] >> 4) & 0x0F;
                plmn_list->mnc[i] += ((*ie_ptr)[i*3+2] & 0x0F)*100;
                plmn_list->mnc[i] += (((*ie_ptr)[i*3+2] >> 4) & 0x0F)*10;
            }
        }
        *ie_ptr += (*ie_ptr)[0] + 1;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: Spare Half Octet

    Description: Used in the description of EMM and ESM messages when
                 an odd number of half octet type 1 information
                 elements are used.

    Document Reference: 24.301 v10.2.0 Section 9.9.2.9
*********************************************************************/

/*********************************************************************
    IE Name: Supported Codec List

    Description: Provides the network with information about the
                 speech codecs supported by the UE.

    Document Reference: 24.301 v10.2.0 Section 9.9.2.10
                        24.008 v10.2.0 Section 10.5.4.32
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_mme_pack_supported_codec_list_ie(LIBLTE_MME_SUPPORTED_CODEC_LIST_STRUCT  *supported_codec_list,
                                                          uint8                                  **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;
    uint32            i;

    if(supported_codec_list != NULL &&
       ie_ptr               != NULL)
    {
        (*ie_ptr)[0] = supported_codec_list->N_supported_codecs*4;
        for(i=0; i<supported_codec_list->N_supported_codecs; i++)
        {
            (*ie_ptr)[1+i*4+0] = supported_codec_list->supported_codec[i].sys_id;
            (*ie_ptr)[1+i*4+1] = 2;
            (*ie_ptr)[1+i*4+2] = (supported_codec_list->supported_codec[i].codec_bitmap >> 8) & 0xFF;
            (*ie_ptr)[1+i*4+3] = supported_codec_list->supported_codec[i].codec_bitmap & 0xFF;
        }
        *ie_ptr += (supported_codec_list->N_supported_codecs*4) + 1;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_mme_unpack_supported_codec_list_ie(uint8                                  **ie_ptr,
                                                            LIBLTE_MME_SUPPORTED_CODEC_LIST_STRUCT  *supported_codec_list)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;
    uint32            i;

    if(ie_ptr               != NULL &&
       supported_codec_list != NULL)
    {
        supported_codec_list->N_supported_codecs = ((*ie_ptr)[0]/4);
        for(i=0; i<supported_codec_list->N_supported_codecs; i++)
        {
            supported_codec_list->supported_codec[i].sys_id        = (*ie_ptr)[1+i*4+0];
            supported_codec_list->supported_codec[i].codec_bitmap  = (*ie_ptr)[1+i*4+2] << 8;
            supported_codec_list->supported_codec[i].codec_bitmap |= (*ie_ptr)[1+i*4+3];
        }
        *ie_ptr += (supported_codec_list->N_supported_codecs*4) + 1;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: Additional Update Result

    Description: Provides additional information about the result of
                 a combined attached procedure or a combined tracking
                 area updating procedure.

    Document Reference: 24.301 v10.2.0 Section 9.9.3.0A
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_mme_pack_additional_update_result_ie(LIBLTE_MME_ADDITIONAL_UPDATE_RESULT_ENUM   result,
                                                              uint8                                      bit_offset,
                                                              uint8                                    **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr != NULL)
    {
        **ie_ptr |= result << bit_offset;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_mme_unpack_additional_update_result_ie(uint8                                    **ie_ptr,
                                                                uint8                                      bit_offset,
                                                                LIBLTE_MME_ADDITIONAL_UPDATE_RESULT_ENUM  *result)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(result != NULL &&
       ie_ptr != NULL)
    {
        *result = (LIBLTE_MME_ADDITIONAL_UPDATE_RESULT_ENUM)((**ie_ptr >> bit_offset) & 0x03);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: Additional Update Type

    Description: Provides additional information about the type of
                 request for a combined attach or a combined tracking
                 area updating procedure.

    Document Reference: 24.301 v10.2.0 Section 9.9.3.0B
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_mme_pack_additional_update_type_ie(LIBLTE_MME_ADDITIONAL_UPDATE_TYPE_ENUM   aut,
                                                            uint8                                    bit_offset,
                                                            uint8                                  **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr != NULL)
    {
        **ie_ptr |= aut << bit_offset;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_mme_unpack_additional_update_type_ie(uint8                                  **ie_ptr,
                                                              uint8                                    bit_offset,
                                                              LIBLTE_MME_ADDITIONAL_UPDATE_TYPE_ENUM  *aut)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr != NULL &&
       aut    != NULL)
    {
        *aut = (LIBLTE_MME_ADDITIONAL_UPDATE_TYPE_ENUM)((**ie_ptr >> bit_offset) & 0x01);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: Authentication Failure Parameter

    Description: Provides the network with the necessary information
                 to begin a re-authentication procedure in the case
                 of a 'Synch failure', following a UMTS or EPS
                 authentication challenge.

    Document Reference: 24.301 v10.2.0 Section 9.9.3.1
                        24.008 v10.2.0 Section 10.5.3.2.2
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_mme_pack_authentication_failure_parameter_ie(uint8  *auth_fail_param,
                                                                      uint8 **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;
    uint32            i;

    if(auth_fail_param != NULL &&
       ie_ptr          != NULL)
    {
        (*ie_ptr)[0] = 14;
        for(i=0; i<14; i++)
        {
            (*ie_ptr)[i+1] = auth_fail_param[i];
        }
        *ie_ptr += 15;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_mme_unpack_authentication_failure_parameter_ie(uint8 **ie_ptr,
                                                                        uint8  *auth_fail_param)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;
    uint32            i;

    if(ie_ptr          != NULL &&
       auth_fail_param != NULL)
    {
        for(i=0; i<14; i++)
        {
            auth_fail_param[i] = (*ie_ptr)[i+1];
        }
        *ie_ptr += 15;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: Authentication Parameter AUTN

    Description: Provides the UE with a means of authenticating the
                 network.

    Document Reference: 24.301 v10.2.0 Section 9.9.3.2
                        24.008 v10.2.0 Section 10.5.3.1.1
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_mme_pack_authentication_parameter_autn_ie(uint8  *autn,
                                                                   uint8 **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;
    uint32            i;

    if(autn   != NULL &&
       ie_ptr != NULL)
    {
        (*ie_ptr)[0] = 16;
        for(i=0; i<16; i++)
        {
            (*ie_ptr)[i+1] = autn[i];
        }
        *ie_ptr += 17;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_mme_unpack_authentication_parameter_autn_ie(uint8 **ie_ptr,
                                                                     uint8  *autn)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;
    uint32            i;

    if(ie_ptr != NULL &&
       autn   != NULL)
    {
        for(i=0; i<16; i++)
        {
            autn[i] = (*ie_ptr)[i+1];
        }
        *ie_ptr += 17;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: Authentication Parameter RAND

    Description: Provides the UE with a non-predictable number to be
                 used to calculate the authentication signature SRES
                 and the ciphering key Kc (for a GSM authentication
                 challenge), or the response RES and both the
                 ciphering key CK and the integrity key IK (for a
                 UMTS authentication challenge).

    Document Reference: 24.301 v10.2.0 Section 9.9.3.3
                        24.008 v10.2.0 Section 10.5.3.1
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_mme_pack_authentication_parameter_rand_ie(uint8  *rand_val,
                                                                   uint8 **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;
    uint32            i;

    if(rand_val != NULL &&
       ie_ptr   != NULL)
    {
        for(i=0; i<16; i++)
        {
            (*ie_ptr)[i] = rand_val[i];
        }
        *ie_ptr += 16;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_mme_unpack_authentication_parameter_rand_ie(uint8 **ie_ptr,
                                                                     uint8  *rand_val)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;
    uint32            i;

    if(ie_ptr   != NULL &&
       rand_val != NULL)
    {
        for(i=0; i<16; i++)
        {
            rand_val[i] = (*ie_ptr)[i];
        }
        *ie_ptr += 16;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: Authentication Response Parameter

    Description: Provides the network with the authentication
                 response calculated in the USIM.

    Document Reference: 24.301 v10.2.0 Section 9.9.3.4
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_mme_pack_authentication_response_parameter_ie(uint8  *res,
                                                                       int     res_len,
                                                                       uint8 **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;
    uint32            i;

    if(res    != NULL &&
       ie_ptr != NULL)
    {
        (*ie_ptr)[0] = res_len;
        *ie_ptr      += 1;
        for(i=0; i<res_len; i++)
        {
            (*ie_ptr)[i] = res[i];
        }
        *ie_ptr += res_len;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_mme_unpack_authentication_response_parameter_ie(uint8 **ie_ptr,
                                                                         uint8  *res)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;
    uint32            i;

    if(ie_ptr != NULL &&
       res    != NULL)
    {
        for(i=0; i<(*ie_ptr)[0]; i++)
        {
            res[i] = (*ie_ptr)[i+1];
        }
        *ie_ptr += (*ie_ptr)[0] + 1;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: Ciphering Key Sequence Number

    Description: Makes it possible for the network to identify the
                 ciphering key Kc which is stored in the UE without
                 invoking the authentication procedure.

    Document Reference: 24.301 v10.2.0 Section 9.9.3.4A
                        24.008 v10.2.0 Section 10.5.1.2
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_mme_pack_ciphering_key_sequence_number_ie(uint8   key_seq,
                                                                   uint8   bit_offset,
                                                                   uint8 **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr != NULL)
    {
        (*ie_ptr)[0] |= (key_seq & 0x07) << bit_offset;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_mme_unpack_ciphering_key_sequence_number_ie(uint8 **ie_ptr,
                                                                     uint8   bit_offset,
                                                                     uint8  *key_seq)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr  != NULL &&
       key_seq != NULL)
    {
        *key_seq = ((*ie_ptr)[0] >> bit_offset) & 0x07;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: CSFB Response

    Description: Indicates whether the UE accepts or rejects a paging
                 for CS fallback.

    Document Reference: 24.301 v10.2.0 Section 9.9.3.5
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_mme_pack_csfb_response_ie(uint8   csfb_resp,
                                                   uint8   bit_offset,
                                                   uint8 **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr != NULL)
    {
        (*ie_ptr)[0] |= (csfb_resp & 0x07) << bit_offset;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_mme_unpack_csfb_response_ie(uint8 **ie_ptr,
                                                     uint8   bit_offset,
                                                     uint8  *csfb_resp)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr    != NULL &&
       csfb_resp != NULL)
    {
        *csfb_resp = ((*ie_ptr)[0] & 0x07) >> bit_offset;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: Daylight Saving Time

    Description: Encodes the daylight saving time in steps of 1 hour.

    Document Reference: 24.301 v10.2.0 Section 9.9.3.6
                        24.008 v10.2.0 Section 10.5.3.12
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_mme_pack_daylight_saving_time_ie(LIBLTE_MME_DAYLIGHT_SAVING_TIME_ENUM   dst,
                                                          uint8                                **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr != NULL)
    {
        (*ie_ptr)[0]  = 1;
        (*ie_ptr)[1]  = dst & 0x03;
        *ie_ptr      += 2;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_mme_unpack_daylight_saving_time_ie(uint8                                **ie_ptr,
                                                            LIBLTE_MME_DAYLIGHT_SAVING_TIME_ENUM  *dst)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr != NULL &&
       dst    != NULL)
    {
        *dst     = (LIBLTE_MME_DAYLIGHT_SAVING_TIME_ENUM)((*ie_ptr)[1] & 0x03);
        *ie_ptr += 2;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: Detach Type

    Description: Indicates the type of detach.

    Document Reference: 24.301 v10.2.0 Section 9.9.3.7
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_mme_pack_detach_type_ie(LIBLTE_MME_DETACH_TYPE_STRUCT  *detach_type,
                                                 uint8                           bit_offset,
                                                 uint8                         **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(detach_type != NULL &&
       ie_ptr      != NULL)
    {
        (*ie_ptr)[0] |= (detach_type->switch_off & 0x01) << (3 + bit_offset);
        (*ie_ptr)[0] |= (detach_type->type_of_detach & 0x07) << bit_offset;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_mme_unpack_detach_type_ie(uint8                         **ie_ptr,
                                                   uint8                           bit_offset,
                                                   LIBLTE_MME_DETACH_TYPE_STRUCT  *detach_type)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr      != NULL &&
       detach_type != NULL)
    {
        detach_type->switch_off     = ((*ie_ptr)[0] >> (3 + bit_offset)) & 0x01;
        detach_type->type_of_detach = ((*ie_ptr)[0] >> bit_offset) & 0x07;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: DRX Parameter

    Description: Indicates whether the UE uses DRX mode or not.

    Document Reference: 24.301 v10.2.0 Section 9.9.3.8
                        24.008 v10.2.0 Section 10.5.5.6
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_mme_pack_drx_parameter_ie(LIBLTE_MME_DRX_PARAMETER_STRUCT  *drx_param,
                                                   uint8                           **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(drx_param != NULL &&
       ie_ptr    != NULL)
    {
        (*ie_ptr)[0]  = drx_param->split_pg_cycle_code;
        (*ie_ptr)[1]  = (drx_param->drx_cycle_len_coeff_and_value & 0x0F) << 4;
        (*ie_ptr)[1] |= (drx_param->split_on_ccch & 0x01) << 3;
        (*ie_ptr)[1] |= drx_param->non_drx_timer & 0x07;
        *ie_ptr      += 2;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_mme_unpack_drx_parameter_ie(uint8                           **ie_ptr,
                                                     LIBLTE_MME_DRX_PARAMETER_STRUCT  *drx_param)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr    != NULL &&
       drx_param != NULL)
    {
        drx_param->split_pg_cycle_code            = (*ie_ptr)[0];
        drx_param->drx_cycle_len_coeff_and_value  = ((*ie_ptr)[1] >> 4) & 0x0F;
        drx_param->split_on_ccch                  = ((*ie_ptr)[1] >> 3) & 0x01;
        drx_param->non_drx_timer                  = (LIBLTE_MME_NON_DRX_TIMER_ENUM)((*ie_ptr)[1] & 0x07);
        *ie_ptr                                  += 2;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: EMM Cause

    Description: Indicates the reason why an EMM request from the UE
                 is rejected by the network.

    Document Reference: 24.301 v10.2.0 Section 9.9.3.9
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_mme_pack_emm_cause_ie(uint8   emm_cause,
                                               uint8 **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr != NULL)
    {
        **ie_ptr  = emm_cause;
        *ie_ptr  += 1;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_mme_unpack_emm_cause_ie(uint8 **ie_ptr,
                                                 uint8  *emm_cause)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr    != NULL &&
       emm_cause != NULL)
    {
        *emm_cause  = **ie_ptr;
        *ie_ptr    += 1;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: EPS Attach Result

    Description: Specifies the result of an attach procedure.

    Document Reference: 24.301 v10.2.0 Section 9.9.3.10
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_mme_pack_eps_attach_result_ie(uint8   result,
                                                       uint8   bit_offset,
                                                       uint8 **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr != NULL)
    {
        **ie_ptr |= result << bit_offset;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_mme_unpack_eps_attach_result_ie(uint8 **ie_ptr,
                                                         uint8   bit_offset,
                                                         uint8  *result)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr != NULL &&
       result != NULL)
    {
        *result = (**ie_ptr >> bit_offset) & 0x07;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: EPS Attach Type

    Description: Indicates the type of the requested attach.

    Document Reference: 24.301 v10.2.0 Section 9.9.3.11
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_mme_pack_eps_attach_type_ie(uint8   attach_type,
                                                     uint8   bit_offset,
                                                     uint8 **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr != NULL)
    {
        **ie_ptr |= attach_type << bit_offset;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_mme_unpack_eps_attach_type_ie(uint8 **ie_ptr,
                                                       uint8   bit_offset,
                                                       uint8  *attach_type)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr      != NULL &&
       attach_type != NULL)
    {
        *attach_type = (**ie_ptr >> bit_offset) & 0x07;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: EPS Mobile Identity

    Description: Provides either the IMSI, the GUTI, or the IMEI.

    Document Reference: 24.301 v10.2.0 Section 9.9.3.12
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_mme_pack_eps_mobile_id_ie(LIBLTE_MME_EPS_MOBILE_ID_STRUCT  *eps_mobile_id,
                                                   uint8                           **ie_ptr)
{
    LIBLTE_ERROR_ENUM  err = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *id;
    uint32             i;

    if(eps_mobile_id != NULL &&
       ie_ptr        != NULL)
    {
        if(LIBLTE_MME_EPS_MOBILE_ID_TYPE_GUTI == eps_mobile_id->type_of_id)
        {
            **ie_ptr  = 11;
            *ie_ptr  += 1;
            **ie_ptr  = 0xF0 | eps_mobile_id->type_of_id;
            *ie_ptr  += 1;
            **ie_ptr  = (((eps_mobile_id->guti.mcc/10) % 10) << 4) | ((eps_mobile_id->guti.mcc/100) % 10);
            *ie_ptr  += 1;
            if(eps_mobile_id->guti.mnc < 100)
            {
                **ie_ptr  = 0xF0 | (eps_mobile_id->guti.mcc % 10);
                *ie_ptr  += 1;
                **ie_ptr  = ((eps_mobile_id->guti.mnc % 10) << 4) | ((eps_mobile_id->guti.mnc/10) % 10);
                *ie_ptr  += 1;
            }else{
                **ie_ptr  = ((eps_mobile_id->guti.mnc % 10) << 4) | (eps_mobile_id->guti.mcc % 10);
                *ie_ptr  += 1;
                **ie_ptr  = (((eps_mobile_id->guti.mnc/10) % 10) << 4) | ((eps_mobile_id->guti.mnc/100) % 10);
                *ie_ptr  += 1;
            }
            **ie_ptr  = (eps_mobile_id->guti.mme_group_id >> 8) & 0xFF;
            *ie_ptr  += 1;
            **ie_ptr  = eps_mobile_id->guti.mme_group_id & 0xFF;
            *ie_ptr  += 1;
            **ie_ptr  = eps_mobile_id->guti.mme_code;
            *ie_ptr  += 1;
            **ie_ptr  = (eps_mobile_id->guti.m_tmsi >> 24) & 0xFF;
            *ie_ptr  += 1;
            **ie_ptr  = (eps_mobile_id->guti.m_tmsi >> 16) & 0xFF;
            *ie_ptr  += 1;
            **ie_ptr  = (eps_mobile_id->guti.m_tmsi >> 8) & 0xFF;
            *ie_ptr  += 1;
            **ie_ptr  = eps_mobile_id->guti.m_tmsi & 0xFF;
            *ie_ptr  += 1;
        }else{
            if(LIBLTE_MME_EPS_MOBILE_ID_TYPE_IMSI == eps_mobile_id->type_of_id)
            {
                id = eps_mobile_id->imsi;
            }else{
                id = eps_mobile_id->imei;
            }

            **ie_ptr  = 8;
            *ie_ptr  += 1;
            **ie_ptr  = (id[0] << 4) | (1 << 3) | eps_mobile_id->type_of_id;
            *ie_ptr  += 1;
            for(i=0; i<7; i++)
            {
                **ie_ptr  = (id[i*2+2] << 4) | id[i*2+1];
                *ie_ptr  += 1;
            }
        }

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_mme_unpack_eps_mobile_id_ie(uint8                           **ie_ptr,
                                                     LIBLTE_MME_EPS_MOBILE_ID_STRUCT  *eps_mobile_id)
{
    LIBLTE_ERROR_ENUM  err = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *id;
    uint32             length;
    uint32             i;

    if(ie_ptr        != NULL &&
       eps_mobile_id != NULL)
    {
        length   = **ie_ptr;
        *ie_ptr += 1;

        eps_mobile_id->type_of_id = **ie_ptr & 0x07;

        if(LIBLTE_MME_EPS_MOBILE_ID_TYPE_GUTI == eps_mobile_id->type_of_id)
        {
            *ie_ptr                 += 1;
            eps_mobile_id->guti.mcc  = (**ie_ptr & 0x0F)*100;
            eps_mobile_id->guti.mcc += ((**ie_ptr >> 4) & 0x0F)*10;
            *ie_ptr                 += 1;
            eps_mobile_id->guti.mcc += **ie_ptr & 0x0F;
            if(((**ie_ptr >> 4) & 0x0F) == 0x0F)
            {
                *ie_ptr                 += 1;
                eps_mobile_id->guti.mnc  = (**ie_ptr & 0x0F)*10;
                eps_mobile_id->guti.mnc += (**ie_ptr >> 4) & 0x0F;
                *ie_ptr                 += 1;
            }else{
                eps_mobile_id->guti.mnc  = (**ie_ptr >> 4) & 0x0F;
                *ie_ptr                 += 1;
                eps_mobile_id->guti.mnc += (**ie_ptr & 0x0F)*100;
                eps_mobile_id->guti.mnc += ((**ie_ptr >> 4) & 0x0F)*10;
                *ie_ptr                 += 1;
            }
            eps_mobile_id->guti.mme_group_id  = **ie_ptr << 8;
            *ie_ptr                          += 1;
            eps_mobile_id->guti.mme_group_id |= **ie_ptr;
            *ie_ptr                          += 1;
            eps_mobile_id->guti.mme_code      = **ie_ptr;
            *ie_ptr                          += 1;
            eps_mobile_id->guti.m_tmsi        = **ie_ptr << 24;
            *ie_ptr                          += 1;
            eps_mobile_id->guti.m_tmsi       |= **ie_ptr << 16;
            *ie_ptr                          += 1;
            eps_mobile_id->guti.m_tmsi       |= **ie_ptr << 8;
            *ie_ptr                          += 1;
            eps_mobile_id->guti.m_tmsi       |= **ie_ptr;
            *ie_ptr                          += 1;
        }else{
            if(LIBLTE_MME_EPS_MOBILE_ID_TYPE_IMSI == eps_mobile_id->type_of_id)
            {
                id = eps_mobile_id->imsi;
            }else{
                id = eps_mobile_id->imei;
            }

            id[0]    = **ie_ptr >> 4;
            *ie_ptr += 1;
            for(i=0; i<7; i++)
            {
                id[i*2+1]  = **ie_ptr & 0x0F;
                id[i*2+2]  = **ie_ptr >> 4;
                *ie_ptr   += 1;
            }
        }

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: EPS Network Feature Support

    Description: Indicates whether certain features are supported by
                 the network.

    Document Reference: 24.301 v10.2.0 Section 9.9.3.12A
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_mme_pack_eps_network_feature_support_ie(LIBLTE_MME_EPS_NETWORK_FEATURE_SUPPORT_STRUCT  *eps_nfs,
                                                                 uint8                                         **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(eps_nfs != NULL &&
       ie_ptr  != NULL)
    {
        (*ie_ptr)[0]  = 1;
        (*ie_ptr)[1]  = eps_nfs->esrps << 5;
        (*ie_ptr)[1] |= (eps_nfs->cs_lcs & 0x03) << 3;
        (*ie_ptr)[1] |= eps_nfs->epc_lcs << 2;
        (*ie_ptr)[1] |= eps_nfs->emc_bs << 1;
        (*ie_ptr)[1] |= eps_nfs->ims_vops;
        *ie_ptr      += 2;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_mme_unpack_eps_network_feature_support_ie(uint8                                         **ie_ptr,
                                                                   LIBLTE_MME_EPS_NETWORK_FEATURE_SUPPORT_STRUCT  *eps_nfs)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr  != NULL &&
       eps_nfs != NULL)
    {
        eps_nfs->esrps     = ((*ie_ptr)[1] >> 5) & 0x01;
        eps_nfs->cs_lcs    = (LIBLTE_MME_CS_LCS_ENUM)(((*ie_ptr)[1] >> 3) & 0x03);
        eps_nfs->epc_lcs   = ((*ie_ptr)[1] >> 2) & 0x01;
        eps_nfs->emc_bs    = ((*ie_ptr)[1] >> 1) & 0x01;
        eps_nfs->ims_vops  = (*ie_ptr)[1] & 0x01;
        *ie_ptr           += 2;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: EPS Update Result

    Description: Specifies the result of the associated updating
                 procedure.

    Document Reference: 24.301 v10.2.0 Section 9.9.3.13
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_mme_pack_eps_update_result_ie(uint8   eps_update_res,
                                                       uint8   bit_offset,
                                                       uint8 **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr != NULL)
    {
        (*ie_ptr)[0] |= (eps_update_res & 0x07) << bit_offset;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_mme_unpack_eps_update_result_ie(uint8 **ie_ptr,
                                                         uint8   bit_offset,
                                                         uint8  *eps_update_res)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr         != NULL &&
       eps_update_res != NULL)
    {
        *eps_update_res = ((*ie_ptr)[0] >> bit_offset) & 0x07;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: EPS Update Type

    Description: Specifies the area the updating procedure is
                 associated with.

    Document Reference: 24.301 v10.2.0 Section 9.9.3.14
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_mme_pack_eps_update_type_ie(LIBLTE_MME_EPS_UPDATE_TYPE_STRUCT  *eps_update_type,
                                                     uint8                               bit_offset,
                                                     uint8                             **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(eps_update_type != NULL &&
       ie_ptr          != NULL)
    {
        (*ie_ptr)[0] |= (eps_update_type->active_flag & 0x01) << (bit_offset + 3);
        (*ie_ptr)[0] |= (eps_update_type->type & 0x07) << bit_offset;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_mme_unpack_eps_update_type_ie(uint8                             **ie_ptr,
                                                       uint8                               bit_offset,
                                                       LIBLTE_MME_EPS_UPDATE_TYPE_STRUCT  *eps_update_type)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr          != NULL &&
       eps_update_type != NULL)
    {
        eps_update_type->active_flag = ((*ie_ptr)[0] >> (bit_offset + 3)) & 0x01;
        eps_update_type->type        = (LIBLTE_MME_EPS_UPDATE_TYPE_ENUM)(((*ie_ptr)[0] >> bit_offset) & 0x07);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: ESM Message Container

    Description: Enables piggybacked transfer of a single ESM message
                 within an EMM message.

    Document Reference: 24.301 v10.2.0 Section 9.9.3.15
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_mme_pack_esm_message_container_ie(LIBLTE_BYTE_MSG_STRUCT  *esm_msg,
                                                           uint8                  **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;
    uint32            i;

    if(esm_msg != NULL &&
       ie_ptr  != NULL)
    {
        (*ie_ptr)[0] = esm_msg->N_bytes >> 8;
        (*ie_ptr)[1] = esm_msg->N_bytes & 0xFF;
        for(i=0; i<esm_msg->N_bytes; i++)
        {
            (*ie_ptr)[2+i] = esm_msg->msg[i];
        }
        *ie_ptr += esm_msg->N_bytes + 2;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_mme_unpack_esm_message_container_ie(uint8                  **ie_ptr,
                                                             LIBLTE_BYTE_MSG_STRUCT  *esm_msg)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;
    uint32            i;

    if(ie_ptr  != NULL &&
       esm_msg != NULL)
    {
        esm_msg->N_bytes  = (*ie_ptr)[0] << 8;
        esm_msg->N_bytes |= (*ie_ptr)[1];
        for(i=0; i<esm_msg->N_bytes; i++)
        {
            esm_msg->msg[i]  = (*ie_ptr)[2+i];
        }
        *ie_ptr += esm_msg->N_bytes + 2;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: GPRS Timer

    Description: Specifies GPRS specific timer values.

    Document Reference: 24.301 v10.2.0 Section 9.9.3.16
                        24.008 v10.2.0 Section 10.5.7.3
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_mme_pack_gprs_timer_ie(LIBLTE_MME_GPRS_TIMER_STRUCT  *timer,
                                                uint8                        **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(timer  != NULL &&
       ie_ptr != NULL)
    {
        **ie_ptr  = ((timer->unit & 0x07) << 5) | (timer->value & 0x1F);
        *ie_ptr  += 1;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_mme_unpack_gprs_timer_ie(uint8                        **ie_ptr,
                                                  LIBLTE_MME_GPRS_TIMER_STRUCT  *timer)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr != NULL &&
       timer  != NULL)
    {
        timer->unit   = **ie_ptr >> 5;
        timer->value  = **ie_ptr & 0x1F;
        *ie_ptr      += 1;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: GPRS Timer 2

    Description: Specifies GPRS specific timer values.

    Document Reference: 24.301 v10.2.0 Section 9.9.3.16A
                        24.008 v10.2.0 Section 10.5.7.4
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_mme_pack_gprs_timer_2_ie(uint8   value,
                                                  uint8 **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr != NULL)
    {
        **ie_ptr  = 1;
        *ie_ptr  += 1;
        **ie_ptr  = value;
        *ie_ptr  += 1;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_mme_unpack_gprs_timer_2_ie(uint8 **ie_ptr,
                                                    uint8  *value)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr != NULL &&
       value  != NULL)
    {
        *ie_ptr += 1;
        *value   = **ie_ptr;
        *ie_ptr += 1;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: GPRS Timer 3

    Description: Specifies GPRS specific timer values.

    Document Reference: 24.301 v10.2.0 Section 9.9.3.16B
                        24.008 v10.2.0 Section 10.5.7.4A
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_mme_pack_gprs_timer_3_ie(LIBLTE_MME_GPRS_TIMER_3_STRUCT  *timer,
                                                  uint8                          **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(timer  != NULL &&
       ie_ptr != NULL)
    {
        (*ie_ptr)[0]  = 1;
        (*ie_ptr)[1]  = ((timer->unit & 0x07) << 5) | (timer->value & 0x1F);
        *ie_ptr      += 2;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_mme_unpack_gprs_timer_3_ie(uint8                          **ie_ptr,
                                                    LIBLTE_MME_GPRS_TIMER_3_STRUCT  *timer)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr != NULL &&
       timer  != NULL)
    {
        timer->unit   = (*ie_ptr)[1] >> 5;
        timer->value  = (*ie_ptr)[1] & 0x1F;
        *ie_ptr      += 2;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: Identity Type 2

    Description: Specifies which identity is requested.

    Document Reference: 24.301 v10.2.0 Section 9.9.3.17
                        24.008 v10.2.0 Section 10.5.5.9
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_mme_pack_identity_type_2_ie(uint8   id_type,
                                                     uint8   bit_offset,
                                                     uint8 **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr != NULL)
    {
        **ie_ptr |= id_type << bit_offset;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_mme_unpack_identity_type_2_ie(uint8 **ie_ptr,
                                                       uint8   bit_offset,
                                                       uint8  *id_type)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr  != NULL &&
       id_type != NULL)
    {
        *id_type = (**ie_ptr >> bit_offset) & 0x07;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: IMEISV Request

    Description: Indicates that the IMEISV shall be included by the
                 UE in the authentication and ciphering response
                 message.

    Document Reference: 24.301 v10.2.0 Section 9.9.3.18
                        24.008 v10.2.0 Section 10.5.5.10
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_mme_pack_imeisv_request_ie(LIBLTE_MME_IMEISV_REQUEST_ENUM   imeisv_req,
                                                    uint8                            bit_offset,
                                                    uint8                          **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr != NULL)
    {
        **ie_ptr |= imeisv_req << bit_offset;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_mme_unpack_imeisv_request_ie(uint8                          **ie_ptr,
                                                      uint8                            bit_offset,
                                                      LIBLTE_MME_IMEISV_REQUEST_ENUM  *imeisv_req)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr     != NULL &&
       imeisv_req != NULL)
    {
        *imeisv_req = (LIBLTE_MME_IMEISV_REQUEST_ENUM)((**ie_ptr >> bit_offset) & 0x07);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: KSI And Sequence Number

    Description: Provides the network with the key set identifier
                 (KSI) value of the current EPS security context and
                 the 5 least significant bits of the NAS COUNT value
                 applicable for the message including this information
                 element.

    Document Reference: 24.301 v10.2.0 Section 9.9.3.19
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_mme_pack_ksi_and_sequence_number_ie(LIBLTE_MME_KSI_AND_SEQUENCE_NUMBER_STRUCT  *ksi_and_seq_num,
                                                             uint8                                     **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ksi_and_seq_num != NULL &&
       ie_ptr          != NULL)
    {
        (*ie_ptr)[0]  = (ksi_and_seq_num->ksi & 0x07) << 5;
        (*ie_ptr)[0] |= ksi_and_seq_num->seq_num & 0x1F;
        *ie_ptr      += 1;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_mme_unpack_ksi_and_sequence_number_ie(uint8                                     **ie_ptr,
                                                               LIBLTE_MME_KSI_AND_SEQUENCE_NUMBER_STRUCT  *ksi_and_seq_num)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr          != NULL &&
       ksi_and_seq_num != NULL)
    {
        ksi_and_seq_num->ksi      = ((*ie_ptr)[0] >> 5) & 0x07;
        ksi_and_seq_num->seq_num  = (*ie_ptr)[0] & 0x1F;
        *ie_ptr                  += 1;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: MS Network Capability

    Description: Provides the network with information concerning
                 aspects of the UE related to GPRS.

    Document Reference: 24.301 v10.2.0 Section 9.9.3.20
                        24.008 v10.2.0 Section 10.5.5.12
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_mme_pack_ms_network_capability_ie(LIBLTE_MME_MS_NETWORK_CAPABILITY_STRUCT  *ms_network_cap,
                                                           uint8                                   **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ms_network_cap != NULL &&
       ie_ptr         != NULL)
    {
        (*ie_ptr)[0]  = 3;
        (*ie_ptr)[1]  = ms_network_cap->gea[1]                << 7;
        (*ie_ptr)[1] |= ms_network_cap->sm_cap_ded            << 6;
        (*ie_ptr)[1] |= ms_network_cap->sm_cap_gprs           << 5;
        (*ie_ptr)[1] |= ms_network_cap->ucs2                  << 4;
        (*ie_ptr)[1] |= (ms_network_cap->ss_screening & 0x03) << 2;
        (*ie_ptr)[1] |= ms_network_cap->solsa                 << 1;
        (*ie_ptr)[1] |= ms_network_cap->revision;
        (*ie_ptr)[2]  = ms_network_cap->pfc    << 7;
        (*ie_ptr)[2] |= ms_network_cap->gea[2] << 6;
        (*ie_ptr)[2] |= ms_network_cap->gea[3] << 5;
        (*ie_ptr)[2] |= ms_network_cap->gea[4] << 4;
        (*ie_ptr)[2] |= ms_network_cap->gea[5] << 3;
        (*ie_ptr)[2] |= ms_network_cap->gea[6] << 2;
        (*ie_ptr)[2] |= ms_network_cap->gea[7] << 1;
        (*ie_ptr)[2] |= ms_network_cap->lcsva;
        (*ie_ptr)[3]  = ms_network_cap->ho_g2u_via_iu << 7;
        (*ie_ptr)[3] |= ms_network_cap->ho_g2e_via_s1 << 6;
        (*ie_ptr)[3] |= ms_network_cap->emm_comb      << 5;
        (*ie_ptr)[3] |= ms_network_cap->isr           << 4;
        (*ie_ptr)[3] |= ms_network_cap->srvcc         << 3;
        (*ie_ptr)[3] |= ms_network_cap->epc           << 2;
        (*ie_ptr)[3] |= ms_network_cap->nf            << 1;
        *ie_ptr      += 4;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_mme_unpack_ms_network_capability_ie(uint8                                   **ie_ptr,
                                                             LIBLTE_MME_MS_NETWORK_CAPABILITY_STRUCT  *ms_network_cap)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr         != NULL &&
       ms_network_cap != NULL)
    {
        ms_network_cap->gea[1]         = ((*ie_ptr)[1] >> 7) & 0x01;
        ms_network_cap->sm_cap_ded     = ((*ie_ptr)[1] >> 6) & 0x01;
        ms_network_cap->sm_cap_gprs    = ((*ie_ptr)[1] >> 5) & 0x01;
        ms_network_cap->ucs2           = ((*ie_ptr)[1] >> 4) & 0x01;
        ms_network_cap->ss_screening   = (LIBLTE_MME_SS_SCREENING_INDICATOR_ENUM)(((*ie_ptr)[1] >> 2) & 0x03);
        ms_network_cap->solsa          = ((*ie_ptr)[1] >> 1) & 0x01;
        ms_network_cap->revision       = (*ie_ptr)[1] & 0x01;
        ms_network_cap->pfc            = ((*ie_ptr)[2] >> 7) & 0x01;
        ms_network_cap->gea[2]         = ((*ie_ptr)[2] >> 6) & 0x01;
        ms_network_cap->gea[3]         = ((*ie_ptr)[2] >> 5) & 0x01;
        ms_network_cap->gea[4]         = ((*ie_ptr)[2] >> 4) & 0x01;
        ms_network_cap->gea[5]         = ((*ie_ptr)[2] >> 3) & 0x01;
        ms_network_cap->gea[6]         = ((*ie_ptr)[2] >> 2) & 0x01;
        ms_network_cap->gea[7]         = ((*ie_ptr)[2] >> 1) & 0x01;
        ms_network_cap->lcsva          = (*ie_ptr)[2] & 0x01;
        ms_network_cap->ho_g2u_via_iu  = ((*ie_ptr)[3] >> 7) & 0x01;
        ms_network_cap->ho_g2e_via_s1  = ((*ie_ptr)[3] >> 6) & 0x01;
        ms_network_cap->emm_comb       = ((*ie_ptr)[3] >> 5) & 0x01;
        ms_network_cap->isr            = ((*ie_ptr)[3] >> 4) & 0x01;
        ms_network_cap->srvcc          = ((*ie_ptr)[3] >> 3) & 0x01;
        ms_network_cap->epc            = ((*ie_ptr)[3] >> 2) & 0x01;
        ms_network_cap->nf             = ((*ie_ptr)[3] >> 1) & 0x01;
        *ie_ptr                       += (*ie_ptr)[0] + 1;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: NAS Key Set Identifier

    Description: Provides the NAS key set identifier that is allocated
                 by the network.

    Document Reference: 24.301 v10.2.0 Section 9.9.3.21
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_mme_pack_nas_key_set_id_ie(LIBLTE_MME_NAS_KEY_SET_ID_STRUCT  *nas_ksi,
                                                    uint8                              bit_offset,
                                                    uint8                            **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(nas_ksi != NULL &&
       ie_ptr  != NULL)
    {
        **ie_ptr |= nas_ksi->tsc_flag << (bit_offset + 3);
        **ie_ptr |= nas_ksi->nas_ksi  << bit_offset;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_mme_unpack_nas_key_set_id_ie(uint8                            **ie_ptr,
                                                      uint8                              bit_offset,
                                                      LIBLTE_MME_NAS_KEY_SET_ID_STRUCT  *nas_ksi)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr  != NULL &&
       nas_ksi != NULL)
    {
        nas_ksi->tsc_flag = (LIBLTE_MME_TYPE_OF_SECURITY_CONTEXT_FLAG_ENUM)((**ie_ptr >> (bit_offset + 3)) & 0x01);
        nas_ksi->nas_ksi  = (**ie_ptr >> bit_offset) & 0x07;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: NAS Message Container

    Description: Encapsulates the SMS messages transferred between
                 the UE and the network.

    Document Reference: 24.301 v10.2.0 Section 9.9.3.22
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_mme_pack_nas_message_container_ie(LIBLTE_BYTE_MSG_STRUCT  *nas_msg,
                                                           uint8                  **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;
    uint32            i;

    if(nas_msg != NULL &&
       ie_ptr  != NULL)
    {
        (*ie_ptr)[0] = nas_msg->N_bytes & 0xFF;
        for(i=0; i<nas_msg->N_bytes; i++)
        {
            (*ie_ptr)[1+i] = nas_msg->msg[i];
        }
        *ie_ptr += nas_msg->N_bytes + 1;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_mme_unpack_nas_message_container_ie(uint8                  **ie_ptr,
                                                             LIBLTE_BYTE_MSG_STRUCT  *nas_msg)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;
    uint32            i;

    if(ie_ptr  != NULL &&
       nas_msg != NULL)
    {
        nas_msg->N_bytes = (*ie_ptr)[0];
        for(i=0; i<nas_msg->N_bytes; i++)
        {
            nas_msg->msg[i] = (*ie_ptr)[1+i];
        }
        *ie_ptr += nas_msg->N_bytes + 1;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: NAS Security Algorithms

    Description: Indicates the algorithms to be used for ciphering
                 and integrity protection.

    Document Reference: 24.301 v10.2.0 Section 9.9.3.23
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_mme_pack_nas_security_algorithms_ie(LIBLTE_MME_NAS_SECURITY_ALGORITHMS_STRUCT  *nas_sec_algs,
                                                             uint8                                     **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(nas_sec_algs != NULL &&
       ie_ptr       != NULL)
    {
        **ie_ptr  = (nas_sec_algs->type_of_eea << 4) | (nas_sec_algs->type_of_eia);
        *ie_ptr  += 1;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_mme_unpack_nas_security_algorithms_ie(uint8                                     **ie_ptr,
                                                               LIBLTE_MME_NAS_SECURITY_ALGORITHMS_STRUCT  *nas_sec_algs)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr       != NULL &&
       nas_sec_algs != NULL)
    {
        nas_sec_algs->type_of_eea  = (LIBLTE_MME_TYPE_OF_CIPHERING_ALGORITHM_ENUM)((**ie_ptr >> 4) & 0x07);
        nas_sec_algs->type_of_eia  = (LIBLTE_MME_TYPE_OF_INTEGRITY_ALGORITHM_ENUM)(**ie_ptr & 0x07);
        *ie_ptr                   += 1;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: Network Name

    Description: Passes a text string to the UE.

    Document Reference: 24.301 v10.2.0 Section 9.9.3.24
                        24.008 v10.2.0 Section 10.5.3.5A
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_mme_pack_network_name_ie(LIBLTE_MME_NETWORK_NAME_STRUCT  *net_name,
                                                  uint8                          **ie_ptr)
{
    LIBLTE_ERROR_ENUM  err = LIBLTE_ERROR_INVALID_INPUTS;
    uint32             i;
    uint32             bit_offset;
    uint32             byte_offset;
    const char        *char_str = net_name->name;

    if(net_name != NULL &&
       ie_ptr   != NULL)
    {
        bit_offset  = 0;
        byte_offset = 2;
        for(i=0; i<strnlen(char_str, LIBLTE_STRING_LEN); i++)
        {
            if(char_str[i]  == 0x0A  ||
               char_str[i]  == 0x0D  ||
               (char_str[i] >= 0x20  &&
                char_str[i] <= 0x3F) ||
               (char_str[i] >= 0x41  &&
                char_str[i] <= 0x5A) ||
               (char_str[i] >= 0x61  &&
                char_str[i] <= 0x7A))
            {
                switch(bit_offset)
                {
                case 0:
                    (*ie_ptr)[byte_offset] = char_str[i];
                    bit_offset = 7;
                    break;
                case 1:
                    (*ie_ptr)[byte_offset] |= (char_str[i] << 1);
                    bit_offset = 0;
                    byte_offset++;
                    break;
                case 2:
                    (*ie_ptr)[byte_offset] |= ((char_str[i] << 2) & 0xFC);
                    byte_offset++;
                    (*ie_ptr)[byte_offset] = ((char_str[i] >> 6) & 0x01);
                    bit_offset = 1;
                    break;
                case 3:
                    (*ie_ptr)[byte_offset] |= ((char_str[i] << 3) & 0xF8);
                    byte_offset++;
                    (*ie_ptr)[byte_offset] = ((char_str[i] >> 5) & 0x03);
                    bit_offset = 2;
                    break;
                case 4:
                    (*ie_ptr)[byte_offset] |= ((char_str[i] << 4) & 0xF0);
                    byte_offset++;
                    (*ie_ptr)[byte_offset] = ((char_str[i] >> 4) & 0x07);
                    bit_offset = 3;
                    break;
                case 5:
                    (*ie_ptr)[byte_offset] |= ((char_str[i] << 5) & 0xE0);
                    byte_offset++;
                    (*ie_ptr)[byte_offset] = ((char_str[i] >> 3) & 0x0F);
                    bit_offset = 4;
                    break;
                case 6:
                    (*ie_ptr)[byte_offset] |= ((char_str[i] << 6) & 0xC0);
                    byte_offset++;
                    (*ie_ptr)[byte_offset] = ((char_str[i] >> 2) & 0x1F);
                    bit_offset = 5;
                    break;
                case 7:
                    (*ie_ptr)[byte_offset] |= ((char_str[i] << 7) & 0x80);
                    byte_offset++;
                    (*ie_ptr)[byte_offset] = ((char_str[i] >> 1) & 0x3F);
                    bit_offset = 6;
                    break;
                }
            }
        }
        if(0 == bit_offset)
        {
            (*ie_ptr)[0]  = byte_offset - 1;
            (*ie_ptr)[1]  = 0x80 | ((net_name->add_ci & 0x01) << 3);
            *ie_ptr      += byte_offset;
        }else{
            (*ie_ptr)[0]  = byte_offset;
            (*ie_ptr)[1]  = 0x80 | ((net_name->add_ci & 0x01) << 3) | ((8 - bit_offset) & 0x07);
            *ie_ptr      += byte_offset + 1;
        }

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_mme_unpack_network_name_ie(uint8                          **ie_ptr,
                                                    LIBLTE_MME_NETWORK_NAME_STRUCT  *net_name)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;
    uint32            i;
    uint32            bit_offset;
    uint32            byte_offset;
    uint32            N_bytes;
    uint8             spare_field;
    char              tmp_char;
    uint32            str_cnt;

    if(ie_ptr   != NULL &&
       net_name != NULL)
    {
        net_name->add_ci = (LIBLTE_MME_ADD_CI_ENUM)(((*ie_ptr)[1] >> 3) & 0x01);
        spare_field      = (*ie_ptr)[1] & 0x07;
        N_bytes          = (*ie_ptr)[0];
        bit_offset       = 0;
        byte_offset      = 2;
        str_cnt          = 0;

        while(byte_offset < N_bytes && str_cnt < LIBLTE_STRING_LEN)
        {
            switch(bit_offset)
            {
            case 0:
                tmp_char = (*ie_ptr)[byte_offset] & 0x7F;
                bit_offset = 7;
                break;
            case 1:
                tmp_char = ((*ie_ptr)[byte_offset] >> 1) & 0x7F;
                bit_offset = 0;
                byte_offset++;
                break;
            case 2:
                tmp_char = ((*ie_ptr)[byte_offset] >> 2) & 0x3F;
                byte_offset++;
                tmp_char |= ((*ie_ptr)[byte_offset] << 6) & 0x40;
                bit_offset = 1;
                break;
            case 3:
                tmp_char = ((*ie_ptr)[byte_offset] >> 3) & 0x1F;
                byte_offset++;
                tmp_char |= ((*ie_ptr)[byte_offset] << 5) & 0x60;
                bit_offset = 2;
                break;
            case 4:
                tmp_char = ((*ie_ptr)[byte_offset] >> 4) & 0x0F;
                byte_offset++;
                tmp_char |= ((*ie_ptr)[byte_offset] << 4) & 0x70;
                bit_offset = 3;
                break;
            case 5:
                tmp_char = ((*ie_ptr)[byte_offset] >> 5) & 0x07;
                byte_offset++;
                tmp_char |= ((*ie_ptr)[byte_offset] << 3) & 0x78;
                bit_offset = 4;
                break;
            case 6:
                tmp_char = ((*ie_ptr)[byte_offset] >> 6) & 0x03;
                byte_offset++;
                tmp_char |= ((*ie_ptr)[byte_offset] << 2) & 0x7C;
                bit_offset = 5;
                break;
            case 7:
                tmp_char = ((*ie_ptr)[byte_offset] >> 7) & 0x01;
                byte_offset++;
                tmp_char |= ((*ie_ptr)[byte_offset] << 1) & 0x7E;
                bit_offset = 6;
                break;
            }

            if(tmp_char  == 0x0A  ||
               tmp_char  == 0x0D  ||
               (tmp_char >= 0x20  &&
                tmp_char <= 0x3F) ||
               (tmp_char >= 0x41  &&
                tmp_char <= 0x5A) ||
               (tmp_char >= 0x61  &&
                tmp_char <= 0x7A))
            {
                if (str_cnt < LIBLTE_STRING_LEN) {
                    net_name->name[str_cnt] = tmp_char;
                    str_cnt++;
                }
            }
        }

        if(0  == bit_offset ||
           (1 == bit_offset &&
            0 == spare_field))
        {
            if(0 == bit_offset)
            {
                tmp_char = (*ie_ptr)[byte_offset] & 0x7F;
            }else{
                tmp_char = ((*ie_ptr)[byte_offset] >> 1) & 0x7F;
            }
            if(tmp_char  == 0x0A  ||
               tmp_char  == 0x0D  ||
               (tmp_char >= 0x20  &&
                tmp_char <= 0x3F) ||
               (tmp_char >= 0x41  &&
                tmp_char <= 0x5A) ||
               (tmp_char >= 0x61  &&
                tmp_char <= 0x7A))
            {
                if (str_cnt < LIBLTE_STRING_LEN) {
                    net_name->name[str_cnt] = tmp_char;
                    str_cnt++;
                }
            }
        }

        if (str_cnt < LIBLTE_STRING_LEN) {
            net_name->name[str_cnt] = '\0';
            str_cnt++;
        }

        *ie_ptr += byte_offset + 1;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: Nonce

    Description: Transfers a 32-bit nonce value to support deriving
                 a new mapped EPS security context.

    Document Reference: 24.301 v10.2.0 Section 9.9.3.25
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_mme_pack_nonce_ie(uint32   nonce,
                                           uint8  **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr != NULL)
    {
        (*ie_ptr)[0]  = (nonce >> 24) & 0xFF;
        (*ie_ptr)[1]  = (nonce >> 16) & 0xFF;
        (*ie_ptr)[2]  = (nonce >> 8) & 0xFF;
        (*ie_ptr)[3]  = nonce & 0xFF;
        *ie_ptr      += 4;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_mme_unpack_nonce_ie(uint8  **ie_ptr,
                                             uint32  *nonce)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr != NULL &&
       nonce  != NULL)
    {
        *nonce   = (*ie_ptr)[0] << 24;
        *nonce  |= (*ie_ptr)[1] << 16;
        *nonce  |= (*ie_ptr)[2] << 8;
        *nonce  |= (*ie_ptr)[3];
        *ie_ptr += 4;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: Paging Identity

    Description: Indicates the identity used for paging for non-EPS
                 services.

    Document Reference: 24.301 v10.2.0 Section 9.9.3.25A
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_mme_pack_paging_identity_ie(uint8   paging_id,
                                                     uint8 **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr != NULL)
    {
        (*ie_ptr)[0]  = paging_id & 0x01;
        *ie_ptr      += 1;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_mme_unpack_paging_identity_ie(uint8 **ie_ptr,
                                                       uint8  *paging_id)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr    != NULL &&
       paging_id != NULL)
    {
        *paging_id  = (*ie_ptr)[0] & 0x01;
        *ie_ptr    += 1;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: P-TMSI Signature

    Description: Identifies a GMM context of a UE.

    Document Reference: 24.301 v10.2.0 Section 9.9.3.26
                        24.008 v10.2.0 Section 10.5.5.8
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_mme_pack_p_tmsi_signature_ie(uint32   p_tmsi_signature,
                                                      uint8  **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr != NULL)
    {
        (*ie_ptr)[0]  = (p_tmsi_signature >> 24) & 0xFF;
        (*ie_ptr)[1]  = (p_tmsi_signature >> 16) & 0xFF;
        (*ie_ptr)[2]  = (p_tmsi_signature >> 8) & 0xFF;
        (*ie_ptr)[3]  = p_tmsi_signature & 0xFF;
        *ie_ptr      += 4;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_mme_unpack_p_tmsi_signature_ie(uint8  **ie_ptr,
                                                        uint32  *p_tmsi_signature)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr           != NULL &&
       p_tmsi_signature != NULL)
    {
        *p_tmsi_signature  = (*ie_ptr)[0] << 24;
        *p_tmsi_signature |= (*ie_ptr)[1] << 16;
        *p_tmsi_signature |= (*ie_ptr)[2] << 8;
        *p_tmsi_signature |= (*ie_ptr)[3];
        *ie_ptr           += 4;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: Service Type

    Description: Specifies the purpose of the service request
                 procedure.

    Document Reference: 24.301 v10.2.0 Section 9.9.3.27
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_mme_pack_service_type_ie(uint8   value,
                                                  uint8   bit_offset,
                                                  uint8 **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr != NULL)
    {
        (*ie_ptr)[0] |= (value & 0x0F) << bit_offset;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_mme_unpack_service_type_ie(uint8 **ie_ptr,
                                                    uint8   bit_offset,
                                                    uint8  *value)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr != NULL &&
       value  != NULL)
    {
        *value = ((*ie_ptr)[0] >> bit_offset) & 0x0F;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: Short MAC

    Description: Protects the integrity of a SERVICE REQUEST message.

    Document Reference: 24.301 v10.2.0 Section 9.9.3.28
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_mme_pack_short_mac_ie(uint16   short_mac,
                                               uint8  **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr != NULL)
    {
        (*ie_ptr)[0]  = (short_mac >> 8) & 0xFF;
        (*ie_ptr)[1]  = short_mac & 0xFF;
        *ie_ptr      += 2;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_mme_unpack_short_mac_ie(uint8  **ie_ptr,
                                                 uint16  *short_mac)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr    != NULL &&
       short_mac != NULL)
    {
        *short_mac  = (*ie_ptr)[0] << 8;
        *short_mac |= (*ie_ptr)[1];
        *ie_ptr    += 2;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: Time Zone

    Description: Encodes the offset between universal time and local
                 time in steps of 15 minutes.

    Document Reference: 24.301 v10.2.0 Section 9.9.3.29
                        24.008 v10.2.0 Section 10.5.3.8
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_mme_pack_time_zone_ie(uint8   tz,
                                               uint8 **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr != NULL)
    {
        (*ie_ptr)[0]  = tz;
        *ie_ptr      += 1;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_mme_unpack_time_zone_ie(uint8 **ie_ptr,
                                                 uint8  *tz)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr != NULL &&
       tz     != NULL)
    {
        *tz      = (*ie_ptr)[0];
        *ie_ptr += 1;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: Time Zone And Time

    Description: Encodes the offset between universal time and local
                 time in steps of 15 minutes and encodes the universal
                 time at which the IE may have been sent by the
                 network.

    Document Reference: 24.301 v10.2.0 Section 9.9.3.30
                        24.008 v10.2.0 Section 10.5.3.9
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_mme_pack_time_zone_and_time_ie(LIBLTE_MME_TIME_ZONE_AND_TIME_STRUCT  *ttz,
                                                        uint8                                **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ttz    != NULL &&
       ie_ptr != NULL)
    {
        (*ie_ptr)[0]  = ((ttz->year % 10) << 4) | ((ttz->year % 100) / 10);
        (*ie_ptr)[1]  = ((ttz->month % 10) << 4) | (ttz->month / 10);
        (*ie_ptr)[2]  = ((ttz->day % 10) << 4) | (ttz->day / 10);
        (*ie_ptr)[3]  = ((ttz->hour % 10) << 4) | (ttz->hour / 10);
        (*ie_ptr)[4]  = ((ttz->minute % 10) << 4) | (ttz->minute / 10);
        (*ie_ptr)[5]  = ((ttz->second % 10) << 4) | (ttz->second / 10);
        (*ie_ptr)[6]  = ttz->tz;
        *ie_ptr      += 7;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_mme_unpack_time_zone_and_time_ie(uint8                                **ie_ptr,
                                                          LIBLTE_MME_TIME_ZONE_AND_TIME_STRUCT  *ttz)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr != NULL &&
       ttz    != NULL)
    {
        ttz->year    = 2000 + (((*ie_ptr)[0] & 0x0F) * 10) + (((*ie_ptr)[0] >> 4) & 0x0F);
        ttz->month   = (((*ie_ptr)[1] & 0x0F) * 10) + (((*ie_ptr)[1] >> 4) & 0x0F);
        ttz->day     = (((*ie_ptr)[2] & 0x0F) * 10) + (((*ie_ptr)[2] >> 4) & 0x0F);
        ttz->hour    = (((*ie_ptr)[3] & 0x0F) * 10) + (((*ie_ptr)[3] >> 4) & 0x0F);
        ttz->minute  = (((*ie_ptr)[4] & 0x0F) * 10) + (((*ie_ptr)[4] >> 4) & 0x0F);
        ttz->second  = (((*ie_ptr)[5] & 0x0F) * 10) + (((*ie_ptr)[5] >> 4) & 0x0F);
        ttz->tz      = (*ie_ptr)[6];
        *ie_ptr     += 7;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: TMSI Status

    Description: Indicates whether a valid TMSI is available in the
                 UE or not.

    Document Reference: 24.301 v10.2.0 Section 9.9.3.31
                        24.008 v10.2.0 Section 10.5.5.4
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_mme_pack_tmsi_status_ie(LIBLTE_MME_TMSI_STATUS_ENUM   tmsi_status,
                                                 uint8                         bit_offset,
                                                 uint8                       **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr != NULL)
    {
        **ie_ptr |= tmsi_status << bit_offset;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_mme_unpack_tmsi_status_ie(uint8                       **ie_ptr,
                                                   uint8                         bit_offset,
                                                   LIBLTE_MME_TMSI_STATUS_ENUM  *tmsi_status)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr      != NULL &&
       tmsi_status != NULL)
    {
        *tmsi_status = (LIBLTE_MME_TMSI_STATUS_ENUM)((**ie_ptr >> bit_offset) & 0x01);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: Tracking Area Identity

    Description: Provides an unambiguous identification of tracking
                 areas within the area covered by the 3GPP system.

    Document Reference: 24.301 v10.2.0 Section 9.9.3.32
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_mme_pack_tracking_area_id_ie(LIBLTE_MME_TRACKING_AREA_ID_STRUCT  *tai,
                                                      uint8                              **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(tai    != NULL &&
       ie_ptr != NULL)
    {
        (*ie_ptr)[0] = (((tai->mcc/10) % 10) << 4) | ((tai->mcc/100) % 10);
        if(tai->mnc < 100)
        {
            (*ie_ptr)[1] = 0xF0 | (tai->mcc % 10);
            (*ie_ptr)[2] = ((tai->mnc % 10) << 4) | ((tai->mnc/10) % 10);
        }else{
            (*ie_ptr)[1] = ((tai->mnc % 10) << 4) | (tai->mcc % 10);
            (*ie_ptr)[2] = (((tai->mnc/10) % 10) << 4) | ((tai->mnc/100) % 10);
        }
        (*ie_ptr)[3]  = (tai->tac >> 8) & 0xFF;
        (*ie_ptr)[4]  = tai->tac & 0xFF;
        *ie_ptr      += 5;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_mme_unpack_tracking_area_id_ie(uint8                              **ie_ptr,
                                                        LIBLTE_MME_TRACKING_AREA_ID_STRUCT  *tai)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr != NULL &&
       tai    != NULL)
    {
        tai->mcc  = ((*ie_ptr)[0] & 0x0F)*100;
        tai->mcc += (((*ie_ptr)[0] >> 4) & 0x0F)*10;
        tai->mcc += (*ie_ptr)[1] & 0x0F;
        if((((*ie_ptr)[1] >> 4) & 0x0F) == 0x0F)
        {
            tai->mnc  = ((*ie_ptr)[2] & 0x0F)*10;
            tai->mnc += ((*ie_ptr)[2] >> 4) & 0x0F;
        }else{
            tai->mnc  = ((*ie_ptr)[1] >> 4) & 0x0F;
            tai->mnc += ((*ie_ptr)[2] & 0x0F)*100;
            tai->mnc += (((*ie_ptr)[2] >> 4) & 0x0F)*10;
        }
        tai->tac  = (*ie_ptr)[3] << 8;
        tai->tac |= (*ie_ptr)[4];
        *ie_ptr  += 5;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: Tracking Area Identity List

    Description: Transfers a list of tracking areas from the network
                 to the UE.

    Document Reference: 24.301 v10.2.0 Section 9.9.3.33
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_mme_pack_tracking_area_identity_list_ie(LIBLTE_MME_TRACKING_AREA_IDENTITY_LIST_STRUCT  *tai_list,
                                                                 uint8                                         **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;
    uint32            i;

    if(tai_list != NULL &&
       ie_ptr   != NULL)
    {
        (*ie_ptr)[0] = (tai_list->N_tais*5) + 1;
        // FIXME: Support all types
        if(1 == tai_list->N_tais)
        {
            (*ie_ptr)[1] = (LIBLTE_MME_TRACKING_AREA_IDENTITY_LIST_TYPE_ONE_PLMN_NON_CONSECUTIVE_TACS << 5) | ((tai_list->N_tais - 1) & 0x1F);
        }else{
            (*ie_ptr)[1] = (LIBLTE_MME_TRACKING_AREA_IDENTITY_LIST_TYPE_DIFFERENT_PLMNS << 5) | ((tai_list->N_tais - 1) & 0x1F);
        }
        *ie_ptr += 2;
        for(i=0; i<tai_list->N_tais; i++)
        {
            liblte_mme_pack_tracking_area_id_ie(&tai_list->tai[i], ie_ptr);
        }

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_mme_unpack_tracking_area_identity_list_ie(uint8                                         **ie_ptr,
                                                                   LIBLTE_MME_TRACKING_AREA_IDENTITY_LIST_STRUCT  *tai_list)
{
    LIBLTE_ERROR_ENUM                                err = LIBLTE_ERROR_INVALID_INPUTS;
    LIBLTE_MME_TRACKING_AREA_IDENTITY_LIST_TYPE_ENUM type;
    uint32                                           sent_length;
    uint32                                           length;
    uint32                                           i;
    uint32                                           N_elems;
    uint16                                           mcc;
    uint16                                           mnc;
    uint16                                           tac;

    if(ie_ptr   != NULL &&
       tai_list != NULL)
    {
        sent_length      = (*ie_ptr)[0] + 1;
        length           = 1;
        tai_list->N_tais = 0;
        while(length < sent_length)
        {
            type    = (LIBLTE_MME_TRACKING_AREA_IDENTITY_LIST_TYPE_ENUM)(((*ie_ptr)[length] >> 5) & 0x03);
            N_elems = (*ie_ptr)[length++] & 0x1F;
            if(LIBLTE_MME_TRACKING_AREA_IDENTITY_LIST_TYPE_ONE_PLMN_NON_CONSECUTIVE_TACS == type)
            {
                mcc  = ((*ie_ptr)[length] & 0x0F)*100;
                mcc += (((*ie_ptr)[length++] >> 4) & 0x0F)*10;
                mcc += (*ie_ptr)[length] & 0x0F;
                if((((*ie_ptr)[length] >> 4) & 0x0F) == 0x0F)
                {
                    length++;
                    mnc  = ((*ie_ptr)[length] & 0x0F)*10;
                    mnc += ((*ie_ptr)[length++] >> 4) & 0x0F;
                }else{
                    mnc  = ((*ie_ptr)[length++] >> 4) & 0x0F;
                    mnc += ((*ie_ptr)[length] & 0x0F)*100;
                    mnc += (((*ie_ptr)[length++] >> 4) & 0x0F)*10;
                }
                for(i=0; i<N_elems; i++)
                {
                    tai_list->tai[tai_list->N_tais].mcc  = mcc;
                    tai_list->tai[tai_list->N_tais].mnc  = mnc;
                    tai_list->tai[tai_list->N_tais].tac  = (*ie_ptr)[length++] << 8;
                    tai_list->tai[tai_list->N_tais].tac |= (*ie_ptr)[length++];
                    tai_list->N_tais++;
                }
            }else if(LIBLTE_MME_TRACKING_AREA_IDENTITY_LIST_TYPE_ONE_PLMN_CONSECUTIVE_TACS == type){
                mcc  = ((*ie_ptr)[length] & 0x0F)*100;
                mcc += (((*ie_ptr)[length++] >> 4) & 0x0F)*10;
                mcc += (*ie_ptr)[length] & 0x0F;
                if((((*ie_ptr)[length] >> 4) & 0x0F) == 0x0F)
                {
                    length++;
                    mnc  = ((*ie_ptr)[length] & 0x0F)*10;
                    mnc += ((*ie_ptr)[length++] >> 4) & 0x0F;
                }else{
                    mnc  = ((*ie_ptr)[length++] >> 4) & 0x0F;
                    mnc += ((*ie_ptr)[length] & 0x0F)*100;
                    mnc += (((*ie_ptr)[length++] >> 4) & 0x0F)*10;
                }
                tac  = (*ie_ptr)[length++] << 8;
                tac |= (*ie_ptr)[length++];
                for(i=0; i<N_elems; i++)
                {
                    tai_list->tai[tai_list->N_tais].mcc = mcc;
                    tai_list->tai[tai_list->N_tais].mnc = mnc;
                    tai_list->tai[tai_list->N_tais].tac = tac + i;
                    tai_list->N_tais++;
                }
            }else{
                for(i=0; i<N_elems; i++)
                {
                    tai_list->tai[tai_list->N_tais].mcc  = ((*ie_ptr)[length] & 0x0F)*100;
                    tai_list->tai[tai_list->N_tais].mcc += (((*ie_ptr)[length++] >> 4) & 0x0F)*10;
                    tai_list->tai[tai_list->N_tais].mcc += (*ie_ptr)[length] & 0x0F;
                    if((((*ie_ptr)[length] >> 4) & 0x0F) == 0x0F)
                    {
                        length++;
                        tai_list->tai[tai_list->N_tais].mnc  = ((*ie_ptr)[length] & 0x0F)*10;
                        tai_list->tai[tai_list->N_tais].mnc += ((*ie_ptr)[length++] >> 4) & 0x0F;
                    }else{
                        tai_list->tai[tai_list->N_tais].mnc  = ((*ie_ptr)[length++] >> 4) & 0x0F;
                        tai_list->tai[tai_list->N_tais].mnc += ((*ie_ptr)[length] & 0x0F)*100;
                        tai_list->tai[tai_list->N_tais].mnc += (((*ie_ptr)[length++] >> 4) & 0x0F)*10;
                    }
                    tai_list->tai[tai_list->N_tais].tac  = (*ie_ptr)[length++] << 8;
                    tai_list->tai[tai_list->N_tais].tac |= (*ie_ptr)[length++];
                    tai_list->N_tais++;
                }
            }
        }
        *ie_ptr += (*ie_ptr)[0] + 1;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: UE Network Capability

    Description: Provides the network with information concerning
                 aspects of the UE related to EPS or interworking with
                 GPRS.

    Document Reference: 24.301 v10.2.0 Section 9.9.3.34
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_mme_pack_ue_network_capability_ie(LIBLTE_MME_UE_NETWORK_CAPABILITY_STRUCT  *ue_network_cap,
                                                           uint8                                   **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ue_network_cap != NULL &&
       ie_ptr         != NULL)
    {
        if(ue_network_cap->uea_present        &&
           (ue_network_cap->ucs2_present      ||
            ue_network_cap->uia_present)      &&
           (ue_network_cap->lpp_present       ||
            ue_network_cap->lcs_present       ||
            ue_network_cap->onexsrvcc_present ||
            ue_network_cap->nf_present))
        {
            **ie_ptr = 5;
        }else if(ue_network_cap->uea_present   &&
                 (ue_network_cap->ucs2_present ||
                  ue_network_cap->uia_present)){
            **ie_ptr = 4;
        }else if(ue_network_cap->uea_present){
            **ie_ptr = 3;
        }else{
            **ie_ptr = 2;
        }
        *ie_ptr  += 1;
        **ie_ptr  = ue_network_cap->eea[0] << 7;
        **ie_ptr |= ue_network_cap->eea[1] << 6;
        **ie_ptr |= ue_network_cap->eea[2] << 5;
        **ie_ptr |= ue_network_cap->eea[3] << 4;
        **ie_ptr |= ue_network_cap->eea[4] << 3;
        **ie_ptr |= ue_network_cap->eea[5] << 2;
        **ie_ptr |= ue_network_cap->eea[6] << 1;
        **ie_ptr |= ue_network_cap->eea[7];
        *ie_ptr  += 1;
        **ie_ptr  = ue_network_cap->eia[0] << 7;
        **ie_ptr |= ue_network_cap->eia[1] << 6;
        **ie_ptr |= ue_network_cap->eia[2] << 5;
        **ie_ptr |= ue_network_cap->eia[3] << 4;
        **ie_ptr |= ue_network_cap->eia[4] << 3;
        **ie_ptr |= ue_network_cap->eia[5] << 2;
        **ie_ptr |= ue_network_cap->eia[6] << 1;
        **ie_ptr |= ue_network_cap->eia[7];
        *ie_ptr  += 1;
        if(ue_network_cap->uea_present)
        {
            **ie_ptr  = ue_network_cap->uea[0] << 7;
            **ie_ptr |= ue_network_cap->uea[1] << 6;
            **ie_ptr |= ue_network_cap->uea[2] << 5;
            **ie_ptr |= ue_network_cap->uea[3] << 4;
            **ie_ptr |= ue_network_cap->uea[4] << 3;
            **ie_ptr |= ue_network_cap->uea[5] << 2;
            **ie_ptr |= ue_network_cap->uea[6] << 1;
            **ie_ptr |= ue_network_cap->uea[7];
            *ie_ptr  += 1;
        }
        if(ue_network_cap->ucs2_present ||
           ue_network_cap->uia_present)
        {
            **ie_ptr  = ue_network_cap->ucs2   << 7;
            **ie_ptr |= ue_network_cap->uia[1] << 6;
            **ie_ptr |= ue_network_cap->uia[2] << 5;
            **ie_ptr |= ue_network_cap->uia[3] << 4;
            **ie_ptr |= ue_network_cap->uia[4] << 3;
            **ie_ptr |= ue_network_cap->uia[5] << 2;
            **ie_ptr |= ue_network_cap->uia[6] << 1;
            **ie_ptr |= ue_network_cap->uia[7];
            *ie_ptr  += 1;
        }
        if(ue_network_cap->lpp_present       ||
           ue_network_cap->lcs_present       ||
           ue_network_cap->onexsrvcc_present ||
           ue_network_cap->nf_present)
        {
            **ie_ptr  = ue_network_cap->lpp << 3;
            **ie_ptr |= ue_network_cap->lcs << 2;
            **ie_ptr |= ue_network_cap->onexsrvcc << 1;
            **ie_ptr |= ue_network_cap->nf;
            *ie_ptr  += 1;
        }

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_mme_unpack_ue_network_capability_ie(uint8                                   **ie_ptr,
                                                             LIBLTE_MME_UE_NETWORK_CAPABILITY_STRUCT  *ue_network_cap)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;
    uint32            length;

    if(ie_ptr         != NULL &&
       ue_network_cap != NULL)
    {
        length                  = **ie_ptr;
        *ie_ptr                += 1;
        ue_network_cap->eea[0]  = (**ie_ptr >> 7) & 0x01;
        ue_network_cap->eea[1]  = (**ie_ptr >> 6) & 0x01;
        ue_network_cap->eea[2]  = (**ie_ptr >> 5) & 0x01;
        ue_network_cap->eea[3]  = (**ie_ptr >> 4) & 0x01;
        ue_network_cap->eea[4]  = (**ie_ptr >> 3) & 0x01;
        ue_network_cap->eea[5]  = (**ie_ptr >> 2) & 0x01;
        ue_network_cap->eea[6]  = (**ie_ptr >> 1) & 0x01;
        ue_network_cap->eea[7]  = **ie_ptr & 0x01;
        *ie_ptr                += 1;
        ue_network_cap->eia[0]  = (**ie_ptr >> 7) & 0x01;
        ue_network_cap->eia[1]  = (**ie_ptr >> 6) & 0x01;
        ue_network_cap->eia[2]  = (**ie_ptr >> 5) & 0x01;
        ue_network_cap->eia[3]  = (**ie_ptr >> 4) & 0x01;
        ue_network_cap->eia[4]  = (**ie_ptr >> 3) & 0x01;
        ue_network_cap->eia[5]  = (**ie_ptr >> 2) & 0x01;
        ue_network_cap->eia[6]  = (**ie_ptr >> 1) & 0x01;
        ue_network_cap->eia[7]  = **ie_ptr & 0x01;
        *ie_ptr                += 1;
        if(length > 2)
        {
            ue_network_cap->uea[0]       = (**ie_ptr >> 7) & 0x01;
            ue_network_cap->uea[1]       = (**ie_ptr >> 6) & 0x01;
            ue_network_cap->uea[2]       = (**ie_ptr >> 5) & 0x01;
            ue_network_cap->uea[3]       = (**ie_ptr >> 4) & 0x01;
            ue_network_cap->uea[4]       = (**ie_ptr >> 3) & 0x01;
            ue_network_cap->uea[5]       = (**ie_ptr >> 2) & 0x01;
            ue_network_cap->uea[6]       = (**ie_ptr >> 1) & 0x01;
            ue_network_cap->uea[7]       = **ie_ptr & 0x01;
            ue_network_cap->uea_present  = true;
            *ie_ptr                     += 1;
        }else{
            ue_network_cap->uea_present = false;
        }
        if(length > 3)
        {
            ue_network_cap->ucs2          = (**ie_ptr >> 7) & 0x01;
            ue_network_cap->ucs2_present  = true;
            ue_network_cap->uia[1]        = (**ie_ptr >> 6) & 0x01;
            ue_network_cap->uia[2]        = (**ie_ptr >> 5) & 0x01;
            ue_network_cap->uia[3]        = (**ie_ptr >> 4) & 0x01;
            ue_network_cap->uia[4]        = (**ie_ptr >> 3) & 0x01;
            ue_network_cap->uia[5]        = (**ie_ptr >> 2) & 0x01;
            ue_network_cap->uia[6]        = (**ie_ptr >> 1) & 0x01;
            ue_network_cap->uia[7]        = **ie_ptr & 0x01;
            ue_network_cap->uia_present   = true;
            *ie_ptr                      += 1;
        }else{
            ue_network_cap->ucs2_present = false;
            ue_network_cap->uia_present  = false;
        }
        if(length > 4)
        {
            ue_network_cap->lpp                = (**ie_ptr >> 3) & 0x01;
            ue_network_cap->lpp_present        = true;
            ue_network_cap->lcs                = (**ie_ptr >> 2) & 0x01;
            ue_network_cap->lcs_present        = true;
            ue_network_cap->onexsrvcc          = (**ie_ptr >> 1) & 0x01;
            ue_network_cap->onexsrvcc_present  = true;
            ue_network_cap->nf                 = **ie_ptr >> 1;
            ue_network_cap->nf_present         = true;
            *ie_ptr                           += 1;
        }else{
            ue_network_cap->lpp_present       = false;
            ue_network_cap->lcs_present       = false;
            ue_network_cap->onexsrvcc_present = false;
            ue_network_cap->nf_present        = false;
        }
        if(length > 5)
        {
            *ie_ptr += length-5;
        }

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: UE Radio Capability Update Needed

    Description: Indicates whether the MME shall delete the stored
                 UE radio capability information, if any.

    Document Reference: 24.301 v10.2.0 Section 9.9.3.35
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_mme_pack_ue_radio_capability_update_needed_ie(uint8   urc_update,
                                                                       uint8   bit_offset,
                                                                       uint8 **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr != NULL)
    {
        (*ie_ptr)[0] |= (urc_update & 0x01) << bit_offset;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_mme_unpack_ue_radio_capability_update_needed_ie(uint8 **ie_ptr,
                                                                         uint8   bit_offset,
                                                                         uint8  *urc_update)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr     != NULL &&
       urc_update != NULL)
    {
        *urc_update = ((*ie_ptr)[0] >> bit_offset) & 0x01;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: UE Security Capability

    Description: Indicates which security algorithms are supported by
                 the UE in S1 mode.

    Document Reference: 24.301 v10.2.0 Section 9.9.3.36
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_mme_pack_ue_security_capabilities_ie(LIBLTE_MME_UE_SECURITY_CAPABILITIES_STRUCT  *ue_sec_cap,
                                                              uint8                                      **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;
    uint32            idx;

    if(ue_sec_cap != NULL &&
       ie_ptr     != NULL)
    {
        if(ue_sec_cap->uea_present &&
           ue_sec_cap->uia_present &&
           ue_sec_cap->gea_present)
        {
            (*ie_ptr)[0] = 5;
        }else if(ue_sec_cap->uea_present &&
                 ue_sec_cap->uia_present){
            (*ie_ptr)[0] = 4;
        }else if(ue_sec_cap->uea_present){
            (*ie_ptr)[0] = 3;
        }else{
            (*ie_ptr)[0] = 2;
        }
        idx             = 1;
        (*ie_ptr)[idx]  = ue_sec_cap->eea[0] << 7;
        (*ie_ptr)[idx] |= ue_sec_cap->eea[1] << 6;
        (*ie_ptr)[idx] |= ue_sec_cap->eea[2] << 5;
        (*ie_ptr)[idx] |= ue_sec_cap->eea[3] << 4;
        (*ie_ptr)[idx] |= ue_sec_cap->eea[4] << 3;
        (*ie_ptr)[idx] |= ue_sec_cap->eea[5] << 2;
        (*ie_ptr)[idx] |= ue_sec_cap->eea[6] << 1;
        (*ie_ptr)[idx] |= ue_sec_cap->eea[7];
        idx++;
        (*ie_ptr)[idx]  = ue_sec_cap->eia[0] << 7;
        (*ie_ptr)[idx] |= ue_sec_cap->eia[1] << 6;
        (*ie_ptr)[idx] |= ue_sec_cap->eia[2] << 5;
        (*ie_ptr)[idx] |= ue_sec_cap->eia[3] << 4;
        (*ie_ptr)[idx] |= ue_sec_cap->eia[4] << 3;
        (*ie_ptr)[idx] |= ue_sec_cap->eia[5] << 2;
        (*ie_ptr)[idx] |= ue_sec_cap->eia[6] << 1;
        (*ie_ptr)[idx] |= ue_sec_cap->eia[7];
        idx++;
        if(ue_sec_cap->uea_present)
        {
            (*ie_ptr)[idx]  = ue_sec_cap->uea[0] << 7;
            (*ie_ptr)[idx] |= ue_sec_cap->uea[1] << 6;
            (*ie_ptr)[idx] |= ue_sec_cap->uea[2] << 5;
            (*ie_ptr)[idx] |= ue_sec_cap->uea[3] << 4;
            (*ie_ptr)[idx] |= ue_sec_cap->uea[4] << 3;
            (*ie_ptr)[idx] |= ue_sec_cap->uea[5] << 2;
            (*ie_ptr)[idx] |= ue_sec_cap->uea[6] << 1;
            (*ie_ptr)[idx] |= ue_sec_cap->uea[7];
            idx++;
        }
        if(ue_sec_cap->uia_present)
        {
            (*ie_ptr)[idx]  = ue_sec_cap->uia[1] << 6;
            (*ie_ptr)[idx] |= ue_sec_cap->uia[2] << 5;
            (*ie_ptr)[idx] |= ue_sec_cap->uia[3] << 4;
            (*ie_ptr)[idx] |= ue_sec_cap->uia[4] << 3;
            (*ie_ptr)[idx] |= ue_sec_cap->uia[5] << 2;
            (*ie_ptr)[idx] |= ue_sec_cap->uia[6] << 1;
            (*ie_ptr)[idx] |= ue_sec_cap->uia[7];
            idx++;
        }
        if(ue_sec_cap->gea_present)
        {
            (*ie_ptr)[idx]  = ue_sec_cap->gea[1] << 6;
            (*ie_ptr)[idx] |= ue_sec_cap->gea[2] << 5;
            (*ie_ptr)[idx] |= ue_sec_cap->gea[3] << 4;
            (*ie_ptr)[idx] |= ue_sec_cap->gea[4] << 3;
            (*ie_ptr)[idx] |= ue_sec_cap->gea[5] << 2;
            (*ie_ptr)[idx] |= ue_sec_cap->gea[6] << 1;
            (*ie_ptr)[idx] |= ue_sec_cap->gea[7];
            idx++;
        }
        *ie_ptr += idx;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_mme_unpack_ue_security_capabilities_ie(uint8                                      **ie_ptr,
                                                                LIBLTE_MME_UE_SECURITY_CAPABILITIES_STRUCT  *ue_sec_cap)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;
    uint32            length;

    if(ie_ptr     != NULL &&
       ue_sec_cap != NULL)
    {
        length             = (*ie_ptr)[0];
        ue_sec_cap->eea[0] = ((*ie_ptr)[1] >> 7) & 0x01;
        ue_sec_cap->eea[1] = ((*ie_ptr)[1] >> 6) & 0x01;
        ue_sec_cap->eea[2] = ((*ie_ptr)[1] >> 5) & 0x01;
        ue_sec_cap->eea[3] = ((*ie_ptr)[1] >> 4) & 0x01;
        ue_sec_cap->eea[4] = ((*ie_ptr)[1] >> 3) & 0x01;
        ue_sec_cap->eea[5] = ((*ie_ptr)[1] >> 2) & 0x01;
        ue_sec_cap->eea[6] = ((*ie_ptr)[1] >> 1) & 0x01;
        ue_sec_cap->eea[7] = (*ie_ptr)[1] & 0x01;
        ue_sec_cap->eia[0] = ((*ie_ptr)[2] >> 7) & 0x01;
        ue_sec_cap->eia[1] = ((*ie_ptr)[2] >> 6) & 0x01;
        ue_sec_cap->eia[2] = ((*ie_ptr)[2] >> 5) & 0x01;
        ue_sec_cap->eia[3] = ((*ie_ptr)[2] >> 4) & 0x01;
        ue_sec_cap->eia[4] = ((*ie_ptr)[2] >> 3) & 0x01;
        ue_sec_cap->eia[5] = ((*ie_ptr)[2] >> 2) & 0x01;
        ue_sec_cap->eia[6] = ((*ie_ptr)[2] >> 1) & 0x01;
        ue_sec_cap->eia[7] = (*ie_ptr)[2] & 0x01;
        if(length > 2)
        {
            ue_sec_cap->uea[0]      = ((*ie_ptr)[3] >> 7) & 0x01;
            ue_sec_cap->uea[1]      = ((*ie_ptr)[3] >> 6) & 0x01;
            ue_sec_cap->uea[2]      = ((*ie_ptr)[3] >> 5) & 0x01;
            ue_sec_cap->uea[3]      = ((*ie_ptr)[3] >> 4) & 0x01;
            ue_sec_cap->uea[4]      = ((*ie_ptr)[3] >> 3) & 0x01;
            ue_sec_cap->uea[5]      = ((*ie_ptr)[3] >> 2) & 0x01;
            ue_sec_cap->uea[6]      = ((*ie_ptr)[3] >> 1) & 0x01;
            ue_sec_cap->uea[7]      = (*ie_ptr)[3] & 0x01;
            ue_sec_cap->uea_present = true;
        }else{
            ue_sec_cap->uea_present = false;
        }
        if(length > 3)
        {
            ue_sec_cap->uia[1]      = ((*ie_ptr)[4] >> 6) & 0x01;
            ue_sec_cap->uia[2]      = ((*ie_ptr)[4] >> 5) & 0x01;
            ue_sec_cap->uia[3]      = ((*ie_ptr)[4] >> 4) & 0x01;
            ue_sec_cap->uia[4]      = ((*ie_ptr)[4] >> 3) & 0x01;
            ue_sec_cap->uia[5]      = ((*ie_ptr)[4] >> 2) & 0x01;
            ue_sec_cap->uia[6]      = ((*ie_ptr)[4] >> 1) & 0x01;
            ue_sec_cap->uia[7]      = (*ie_ptr)[4] & 0x01;
            ue_sec_cap->uia_present = true;
        }else{
            ue_sec_cap->uia_present = false;
        }
        if(length > 4)
        {
            ue_sec_cap->gea[1]      = ((*ie_ptr)[5] >> 6) & 0x01;
            ue_sec_cap->gea[2]      = ((*ie_ptr)[5] >> 5) & 0x01;
            ue_sec_cap->gea[3]      = ((*ie_ptr)[5] >> 4) & 0x01;
            ue_sec_cap->gea[4]      = ((*ie_ptr)[5] >> 3) & 0x01;
            ue_sec_cap->gea[5]      = ((*ie_ptr)[5] >> 2) & 0x01;
            ue_sec_cap->gea[6]      = ((*ie_ptr)[5] >> 1) & 0x01;
            ue_sec_cap->gea[7]      = (*ie_ptr)[5] & 0x01;
            ue_sec_cap->gea_present = true;
        }else{
            ue_sec_cap->gea_present = false;
        }
        *ie_ptr += length + 1;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: Emergency Number List

    Description: Encodes emergency number(s) for use within the
                 country (as indicated by MCC) where the IE is
                 received.

    Document Reference: 24.301 v10.2.0 Section 9.9.3.37
                        24.008 v10.2.0 Section 10.5.3.13
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_mme_pack_emergency_number_list_ie(LIBLTE_MME_EMERGENCY_NUMBER_LIST_STRUCT  *emerg_num_list,
                                                           uint8                                   **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;
    uint32            i;
    uint32            j;
    uint32            length;

    if(emerg_num_list != NULL &&
       ie_ptr         != NULL)
    {
        length = 1;
        for(i=0; i<emerg_num_list->N_emerg_nums; i++)
        {
            if((emerg_num_list->emerg_num[i].N_emerg_num_digits % 2) == 0)
            {
                (*ie_ptr)[length++] = (emerg_num_list->emerg_num[i].N_emerg_num_digits/2) + 1;
                (*ie_ptr)[length++] = emerg_num_list->emerg_num[i].emerg_service_cat;
                for(j=0; j<emerg_num_list->emerg_num[i].N_emerg_num_digits/2; j++)
                {
                    (*ie_ptr)[length]    = emerg_num_list->emerg_num[i].emerg_num[j*2+0];
                    (*ie_ptr)[length++] |= emerg_num_list->emerg_num[i].emerg_num[j*2+1] << 4;
                }
            }else{
                (*ie_ptr)[length++] = (emerg_num_list->emerg_num[i].N_emerg_num_digits/2) + 2;
                (*ie_ptr)[length++] = emerg_num_list->emerg_num[i].emerg_service_cat;
                for(j=0; j<emerg_num_list->emerg_num[i].N_emerg_num_digits/2; j++)
                {
                    (*ie_ptr)[length]    = emerg_num_list->emerg_num[i].emerg_num[j*2+0];
                    (*ie_ptr)[length++] |= emerg_num_list->emerg_num[i].emerg_num[j*2+1] << 4;
                }
                (*ie_ptr)[length++] = 0xF0 | emerg_num_list->emerg_num[i].emerg_num[j*2];
            }
        }
        (*ie_ptr)[0]  = length - 2;
        *ie_ptr      += length - 1;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_mme_unpack_emergency_number_list_ie(uint8                                   **ie_ptr,
                                                             LIBLTE_MME_EMERGENCY_NUMBER_LIST_STRUCT  *emerg_num_list)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;
    uint32            sent_length;
    uint32            length;
    uint32            idx;
    uint32            i;

    if(ie_ptr         != NULL &&
       emerg_num_list != NULL)
    {
        sent_length                  = (*ie_ptr)[0] + 1;
        length                       = 1;
        emerg_num_list->N_emerg_nums = 0;
        while(length < sent_length)
        {
            idx                                               = emerg_num_list->N_emerg_nums;
            emerg_num_list->emerg_num[idx].N_emerg_num_digits = ((*ie_ptr)[length++] - 1) * 2;
            emerg_num_list->emerg_num[idx].emerg_service_cat  = (LIBLTE_MME_EMERGENCY_SERVICE_CATEGORY_ENUM)((*ie_ptr)[length++] & 0x1F);
            for(i=0; i<emerg_num_list->emerg_num[idx].N_emerg_num_digits/2; i++)
            {
                emerg_num_list->emerg_num[idx].emerg_num[i*2+0] = (*ie_ptr)[length] & 0x0F;
                emerg_num_list->emerg_num[idx].emerg_num[i*2+1] = (*ie_ptr)[length++] >> 4;
            }
            // Added by Ismael: if i==0 this is negative 
            if (i > 0) {
              if(emerg_num_list->emerg_num[idx].emerg_num[i*2-1] == 0x0F)
              {
                  emerg_num_list->emerg_num[idx].N_emerg_num_digits--;
              }
            }
            emerg_num_list->N_emerg_nums++;
        }
        *ie_ptr += (*ie_ptr)[0] + 1;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: CLI

    Description: Conveys information about the calling line for a
                 terminated call to a CS fallback capable UE.

    Document Reference: 24.301 v10.2.0 Section 9.9.3.38
*********************************************************************/
// FIXME

/*********************************************************************
    IE Name: SS Code

    Description: Conveys information related to a network initiated
                 supplementary service request to a CS fallback
                 capable UE.

    Document Reference: 24.301 v10.2.0 Section 9.9.3.39
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_mme_pack_ss_code_ie(uint8   code,
                                             uint8 **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr != NULL)
    {
        (*ie_ptr)[0]  = code;
        *ie_ptr      += 1;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_mme_unpack_ss_code_ie(uint8 **ie_ptr,
                                               uint8  *code)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr != NULL &&
       code   != NULL)
    {
        *code    = (*ie_ptr)[0];
        *ie_ptr += 1;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: LCS Indicator

    Description: Indicates that the origin of the message is due to a
                 LCS request and the type of this request to a CS
                 fallback capable UE.

    Document Reference: 24.301 v10.2.0 Section 9.9.3.40
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_mme_pack_lcs_indicator_ie(uint8   lcs_ind,
                                                   uint8 **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr != NULL)
    {
        (*ie_ptr)[0]  = lcs_ind;
        *ie_ptr      += 1;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_mme_unpack_lcs_indicator_ie(uint8 **ie_ptr,
                                                     uint8  *lcs_ind)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr  != NULL &&
       lcs_ind != NULL)
    {
        *lcs_ind  = (*ie_ptr)[0];
        *ie_ptr  += 1;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: LCS Client Identity

    Description: Conveys information related to the client of a LCS
                 request for a CS fallback capable UE.

    Document Reference: 24.301 v10.2.0 Section 9.9.3.41
*********************************************************************/
// FIXME

/*********************************************************************
    IE Name: Generic Message Container Type

    Description: Specifies the type of message contained in the
                 generic message container IE.

    Document Reference: 24.301 v10.2.0 Section 9.9.3.42
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_mme_pack_generic_message_container_type_ie(uint8   msg_cont_type,
                                                                    uint8 **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr != NULL)
    {
        (*ie_ptr)[0]  = msg_cont_type;
        *ie_ptr      += 1;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_mme_unpack_generic_message_container_type_ie(uint8 **ie_ptr,
                                                                      uint8  *msg_cont_type)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr        != NULL &&
       msg_cont_type != NULL)
    {
        *msg_cont_type  = (*ie_ptr)[0];
        *ie_ptr        += 1;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: Generic Message Container

    Description: Encapsulates the application message transferred
                 between the UE and the network.

    Document Reference: 24.301 v10.2.0 Section 9.9.3.43
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_mme_pack_generic_message_container_ie(LIBLTE_BYTE_MSG_STRUCT  *msg,
                                                               uint8                  **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;
    uint32            i;

    if(msg    != NULL &&
       ie_ptr != NULL)
    {
        (*ie_ptr)[0] = msg->N_bytes >> 8;
        (*ie_ptr)[1] = msg->N_bytes & 0xFF;
        for(i=0; i<msg->N_bytes; i++)
        {
            (*ie_ptr)[2+i] = msg->msg[i];
        }
        *ie_ptr += msg->N_bytes + 2;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_mme_unpack_generic_message_container_ie(uint8                  **ie_ptr,
                                                                 LIBLTE_BYTE_MSG_STRUCT  *msg)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;
    uint32            i;

    if(ie_ptr != NULL &&
       msg    != NULL)
    {
        msg->N_bytes  = (*ie_ptr)[0] << 8;
        msg->N_bytes |= (*ie_ptr)[1];
        for(i=0; i<msg->N_bytes; i++)
        {
            msg->msg[i] = (*ie_ptr)[2+i];
        }
        *ie_ptr += msg->N_bytes + 2;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: Voice Domain Preference and UE's Usage Setting

    Description: Provides the network with the UE's usage setting and
                 the voice domain preference for the E-UTRAN.

    Document Reference: 24.301 v10.2.0 Section 9.9.3.44
                        24.008 v10.2.0 Section 10.5.5.28
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_mme_pack_voice_domain_pref_and_ue_usage_setting_ie(LIBLTE_MME_VOICE_DOMAIN_PREF_AND_UE_USAGE_SETTING_STRUCT  *voice_domain_pref_and_ue_usage_setting,
                                                                            uint8                                                    **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(voice_domain_pref_and_ue_usage_setting != NULL &&
       ie_ptr                                 != NULL)
    {
        **ie_ptr  = 1;
        *ie_ptr  += 1;
        **ie_ptr  = voice_domain_pref_and_ue_usage_setting->ue_usage_setting << 2;
        **ie_ptr |= voice_domain_pref_and_ue_usage_setting->voice_domain_pref;
        *ie_ptr  += 1;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_mme_unpack_voice_domain_pref_and_ue_usage_setting_ie(uint8                                                    **ie_ptr,
                                                                              LIBLTE_MME_VOICE_DOMAIN_PREF_AND_UE_USAGE_SETTING_STRUCT  *voice_domain_pref_and_ue_usage_setting)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr                                 != NULL &&
       voice_domain_pref_and_ue_usage_setting != NULL)
    {
        *ie_ptr                                                   += 1;
        voice_domain_pref_and_ue_usage_setting->ue_usage_setting   = (LIBLTE_MME_UE_USAGE_SETTING_ENUM)((**ie_ptr >> 2) & 0x01);
        voice_domain_pref_and_ue_usage_setting->voice_domain_pref  = (LIBLTE_MME_VOICE_DOMAIN_PREF_ENUM)(**ie_ptr & 0x03);
        *ie_ptr                                                   += 1;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: GUTI Type

    Description: Indicates whether the GUTI included in the same
                 message in an information element of type EPS
                 mobility identity represents a native GUTI or a
                 mapped GUTI.

    Document Reference: 24.301 v10.2.0 Section 9.9.3.45
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_mme_pack_guti_type_ie(LIBLTE_MME_GUTI_TYPE_ENUM   guti_type,
                                               uint8                       bit_offset,
                                               uint8                     **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr != NULL)
    {
        **ie_ptr |= guti_type << bit_offset;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_mme_unpack_guti_type_ie(uint8                     **ie_ptr,
                                                 uint8                       bit_offset,
                                                 LIBLTE_MME_GUTI_TYPE_ENUM  *guti_type)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr    != NULL &&
       guti_type != NULL)
    {
        *guti_type = (LIBLTE_MME_GUTI_TYPE_ENUM)((**ie_ptr >> bit_offset) & 0x01);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: Access Point Name

    Description: Identifies the packet data network to which the GPRS
                 user wishes to connect and notifies the access point
                 of the packet data network that wishes to connect to
                 the UE.

    Document Reference: 24.301 v10.2.0 Section 9.9.4.1
                        24.008 v10.2.0 Section 10.5.6.1
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_mme_pack_access_point_name_ie(LIBLTE_MME_ACCESS_POINT_NAME_STRUCT  *apn,
                                                       uint8                               **ie_ptr)
{
    LIBLTE_ERROR_ENUM  err = LIBLTE_ERROR_INVALID_INPUTS;
    const char        *apn_str;
    uint32             i;
    uint32             len_idx;
    uint32             apn_idx;
    uint32             label_len;

    if(apn    != NULL &&
       ie_ptr != NULL)
    {
        apn_str      = apn->apn;
        (*ie_ptr)[0] = strnlen(apn->apn, LIBLTE_STRING_LEN)+1;
        len_idx      = 0;
        apn_idx      = 0;
        label_len    = 0;
        while(strnlen(apn->apn, LIBLTE_STRING_LEN) > apn_idx)
        {
            (*ie_ptr)[1+apn_idx+1] = (uint8)apn_str[apn_idx];
            apn_idx++;
            label_len++;

            if(apn_str[apn_idx] == '.')
            {
                (*ie_ptr)[1+len_idx] = label_len;
                label_len            = 0;
                len_idx              = apn_idx+1;
                apn_idx++;
            }
        }
        (*ie_ptr)[1+len_idx]  = label_len;
        *ie_ptr              += strnlen(apn->apn, LIBLTE_STRING_LEN) + 2;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_mme_unpack_access_point_name_ie(uint8                               **ie_ptr,
                                                         LIBLTE_MME_ACCESS_POINT_NAME_STRUCT  *apn)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;
    uint32            i;
    uint32            ie_idx;
    uint32            label_len;
    uint32            str_cnt;

    if(ie_ptr != NULL &&
       apn    != NULL)
    {
        ie_idx = 0;
        str_cnt = 0;
        while(ie_idx < (*ie_ptr)[0] && str_cnt < LIBLTE_STRING_LEN)
        {
            label_len = (*ie_ptr)[1+ie_idx];
            for(i=0; i<label_len && str_cnt < LIBLTE_STRING_LEN; i++)
            {
                apn->apn[str_cnt] = (char)((*ie_ptr)[1+ie_idx+i+1]);
                str_cnt++;
            }
            ie_idx += label_len + 1;
            if(ie_idx < (*ie_ptr)[0] && str_cnt < LIBLTE_STRING_LEN)
            {
                apn->apn[str_cnt] = '.';
                str_cnt++;
            }
        }
        if (str_cnt < LIBLTE_STRING_LEN) {
            apn->apn[str_cnt] = '\0';
        }
        *ie_ptr  += (*ie_ptr)[0] + 1;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: APN Aggregate Maximum Bit Rate

    Description: Indicates the initial subscribed APN-AMBR when the
                 UE establishes a PDN connection or indicates the new
                 APN-AMBR if it is changed by the network.

    Document Reference: 24.301 v10.2.0 Section 9.9.4.2
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_mme_pack_apn_aggregate_maximum_bit_rate_ie(LIBLTE_MME_APN_AGGREGATE_MAXIMUM_BIT_RATE_STRUCT  *apn_ambr,
                                                                    uint8                                            **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(apn_ambr != NULL &&
       ie_ptr   != NULL)
    {
        if(apn_ambr->ext_present &&
           apn_ambr->ext2_present)
        {
            (*ie_ptr)[0] = 6;
        }else if(apn_ambr->ext_present){
            (*ie_ptr)[0] = 4;
        }else{
            (*ie_ptr)[0] = 2;
        }
        (*ie_ptr)[1] = apn_ambr->apn_ambr_dl;
        (*ie_ptr)[2] = apn_ambr->apn_ambr_ul;
        if(apn_ambr->ext_present)
        {
            (*ie_ptr)[3] = apn_ambr->apn_ambr_dl_ext;
            (*ie_ptr)[4] = apn_ambr->apn_ambr_ul_ext;
        }
        if(apn_ambr->ext2_present)
        {
            (*ie_ptr)[5] = apn_ambr->apn_ambr_dl_ext2;
            (*ie_ptr)[6] = apn_ambr->apn_ambr_ul_ext2;
        }
        if(apn_ambr->ext_present &&
           apn_ambr->ext2_present)
        {
            *ie_ptr += 7;
        }else if(apn_ambr->ext_present){
            *ie_ptr += 5;
        }else{
            *ie_ptr += 3;
        }

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_mme_unpack_apn_aggregate_maximum_bit_rate_ie(uint8                                            **ie_ptr,
                                                                      LIBLTE_MME_APN_AGGREGATE_MAXIMUM_BIT_RATE_STRUCT  *apn_ambr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr   != NULL &&
       apn_ambr != NULL)
    {
        if(6 == (*ie_ptr)[0])
        {
            apn_ambr->ext_present  = true;
            apn_ambr->ext2_present = true;
        }else if(4 == (*ie_ptr)[0]){
            apn_ambr->ext_present  = true;
            apn_ambr->ext2_present = false;
        }else{
            apn_ambr->ext_present  = false;
            apn_ambr->ext2_present = false;
        }
        apn_ambr->apn_ambr_dl = (*ie_ptr)[1];
        apn_ambr->apn_ambr_ul = (*ie_ptr)[2];
        if(apn_ambr->ext_present)
        {
            apn_ambr->apn_ambr_dl_ext = (*ie_ptr)[3];
            apn_ambr->apn_ambr_ul_ext = (*ie_ptr)[4];
        }
        if(apn_ambr->ext2_present)
        {
            apn_ambr->apn_ambr_dl_ext2 = (*ie_ptr)[5];
            apn_ambr->apn_ambr_ul_ext2 = (*ie_ptr)[6];
        }
        if(apn_ambr->ext_present &&
           apn_ambr->ext2_present)
        {
            *ie_ptr += 7;
        }else if(apn_ambr->ext_present){
            *ie_ptr += 5;
        }else{
            *ie_ptr += 3;
        }

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: Connectivity Type

    Description: Specifies the type of connectivity selected by the
                 network for the PDN connection.

    Document Reference: 24.301 v10.2.0 Section 9.9.4.2A
                        24.008 v10.2.0 Section 10.5.6.19
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_mme_pack_connectivity_type_ie(uint8   con_type,
                                                       uint8   bit_offset,
                                                       uint8 **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr != NULL)
    {
        (*ie_ptr)[0] |= con_type << bit_offset;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_mme_unpack_connectivity_type_ie(uint8 **ie_ptr,
                                                         uint8   bit_offset,
                                                         uint8  *con_type)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr   != NULL &&
       con_type != NULL)
    {
        *con_type = ((*ie_ptr)[0] >> bit_offset) & 0x0F;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: EPS Quality Of Service

    Description: Specifies the QoS parameters for an EPS bearer
                 context.

    Document Reference: 24.301 v10.2.0 Section 9.9.4.3
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_mme_pack_eps_quality_of_service_ie(LIBLTE_MME_EPS_QUALITY_OF_SERVICE_STRUCT  *qos,
                                                            uint8                                    **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(qos    != NULL &&
       ie_ptr != NULL)
    {
        if(qos->br_present &&
           qos->br_ext_present)
        {
            (*ie_ptr)[0] = 9;
        }else if(qos->br_present){
            (*ie_ptr)[0] = 5;
        }else{
            (*ie_ptr)[0] = 1;
        }
        (*ie_ptr)[1] = qos->qci;
        if(qos->br_present)
        {
            (*ie_ptr)[2] = qos->mbr_ul;
            (*ie_ptr)[3] = qos->mbr_dl;
            (*ie_ptr)[4] = qos->gbr_ul;
            (*ie_ptr)[5] = qos->gbr_dl;
        }
        if(qos->br_ext_present)
        {
            (*ie_ptr)[6] = qos->mbr_ul_ext;
            (*ie_ptr)[7] = qos->mbr_dl_ext;
            (*ie_ptr)[8] = qos->gbr_ul_ext;
            (*ie_ptr)[9] = qos->gbr_dl_ext;
        }
        if(qos->br_present &&
           qos->br_ext_present)
        {
            *ie_ptr += 10;
        }else if(qos->br_present){
            *ie_ptr += 6;
        }else{
            *ie_ptr += 2;
        }

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_mme_unpack_eps_quality_of_service_ie(uint8                                    **ie_ptr,
                                                              LIBLTE_MME_EPS_QUALITY_OF_SERVICE_STRUCT  *qos)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr != NULL &&
       qos    != NULL)
    {
        if((*ie_ptr)[0] == 1)
        {
            qos->br_present     = false;
            qos->br_ext_present = false;
        }else if((*ie_ptr)[0] == 5){
            qos->br_present     = true;
            qos->br_ext_present = false;
        }else{
            qos->br_present     = true;
            qos->br_ext_present = true;
        }
        qos->qci = (*ie_ptr)[1];
        if(qos->br_present)
        {
            qos->mbr_ul = (*ie_ptr)[2];
            qos->mbr_dl = (*ie_ptr)[3];
            qos->gbr_ul = (*ie_ptr)[4];
            qos->gbr_dl = (*ie_ptr)[5];
        }
        if(qos->br_ext_present)
        {
            qos->mbr_ul_ext = (*ie_ptr)[6];
            qos->mbr_dl_ext = (*ie_ptr)[7];
            qos->gbr_ul_ext = (*ie_ptr)[8];
            qos->gbr_dl_ext = (*ie_ptr)[9];
        }
        if(qos->br_present &&
           qos->br_ext_present)
        {
            *ie_ptr += 10;
        }else if(qos->br_present){
            *ie_ptr += 6;
        }else{
            *ie_ptr += 2;
        }

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: ESM Cause

    Description: Indicates the reason why a session management request
                 is rejected.

    Document Reference: 24.301 v10.2.0 Section 9.9.4.4
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_mme_pack_esm_cause_ie(uint8   cause,
                                               uint8 **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr != NULL)
    {
        (*ie_ptr)[0]  = cause;
        *ie_ptr      += 1;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_mme_unpack_esm_cause_ie(uint8 **ie_ptr,
                                                 uint8  *cause)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr != NULL &&
       cause  != NULL)
    {
        *cause   = (*ie_ptr)[0];
        *ie_ptr += 1;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: ESM Information Transfer Flag

    Description: Indicates whether ESM information, i.e. protocol
                 configuration options or APN or both, is to be
                 transferred security protected.

    Document Reference: 24.301 v10.2.0 Section 9.9.4.5
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_mme_pack_esm_info_transfer_flag_ie(LIBLTE_MME_ESM_INFO_TRANSFER_FLAG_ENUM   esm_info_transfer_flag,
                                                            uint8                                    bit_offset,
                                                            uint8                                  **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr != NULL)
    {
        **ie_ptr |= esm_info_transfer_flag << bit_offset;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_mme_unpack_esm_info_transfer_flag_ie(uint8                                  **ie_ptr,
                                                              uint8                                    bit_offset,
                                                              LIBLTE_MME_ESM_INFO_TRANSFER_FLAG_ENUM  *esm_info_transfer_flag)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr                 != NULL &&
       esm_info_transfer_flag != NULL)
    {
        *esm_info_transfer_flag = (LIBLTE_MME_ESM_INFO_TRANSFER_FLAG_ENUM)((**ie_ptr >> bit_offset) & 0x01);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: Linked EPS Bearer Identity

    Description: Identifies the default bearer that is associated
                 with a dedicated EPS bearer or identifies the EPS
                 bearer (default or dedicated) with which one or more
                 packet filters specified in a traffic flow aggregate
                 are associated.

    Document Reference: 24.301 v10.2.0 Section 9.9.4.6
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_mme_pack_linked_eps_bearer_identity_ie(uint8   bearer_id,
                                                                uint8   bit_offset,
                                                                uint8 **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr != NULL)
    {
        (*ie_ptr)[0] |= (bearer_id & 0x0F) << bit_offset;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_mme_unpack_linked_eps_bearer_identity_ie(uint8 **ie_ptr,
                                                                  uint8   bit_offset,
                                                                  uint8  *bearer_id)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr    != NULL &&
       bearer_id != NULL)
    {
        *bearer_id = ((*ie_ptr)[0] >> bit_offset) & 0x0F;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: LLC Service Access Point Identifier

    Description: Identifies the service access point that is used for
                 the GPRS data transfer at LLC layer.

    Document Reference: 24.301 v10.2.0 Section 9.9.4.7
                        24.008 v10.2.0 Section 10.5.6.9
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_mme_pack_llc_service_access_point_identifier_ie(uint8   llc_sapi,
                                                                         uint8 **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr != NULL)
    {
        (*ie_ptr)[0]  = llc_sapi;
        *ie_ptr      += 1;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_mme_unpack_llc_service_access_point_identifier_ie(uint8 **ie_ptr,
                                                                           uint8  *llc_sapi)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr   != NULL &&
       llc_sapi != NULL)
    {
        *llc_sapi  = (*ie_ptr)[0];
        *ie_ptr   += 1;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: Notification Indicator

    Description: Informs the UE about an event which is relevant for
                 the upper layer using an EPS bearer context or
                 having requested a procedure transaction.

    Document Reference: 24.301 v10.2.0 Section 9.9.4.7A
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_mme_pack_notification_indicator_ie(uint8   notification_ind,
                                                            uint8 **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr != NULL)
    {
        (*ie_ptr)[0]  = 1;
        (*ie_ptr)[1]  = notification_ind;
        *ie_ptr      += 2;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_mme_unpack_notification_indicator_ie(uint8 **ie_ptr,
                                                              uint8  *notification_ind)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr           != NULL &&
       notification_ind != NULL)
    {
        *notification_ind  = (*ie_ptr)[1];
        *ie_ptr           += 2;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: Packet Flow Identifier

    Description: Indicates the packet flow identifier for a packet
                 flow context.

    Document Reference: 24.301 v10.2.0 Section 9.9.4.8
                        24.008 v10.2.0 Section 10.5.6.11
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_mme_pack_packet_flow_identifier_ie(uint8   packet_flow_id,
                                                            uint8 **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr != NULL)
    {
        (*ie_ptr)[0]  = 1;
        (*ie_ptr)[1]  = packet_flow_id;
        *ie_ptr      += 2;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_mme_unpack_packet_flow_identifier_ie(uint8 **ie_ptr,
                                                              uint8  *packet_flow_id)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr         != NULL &&
       packet_flow_id != NULL)
    {
        *packet_flow_id  = (*ie_ptr)[1];
        *ie_ptr         += (*ie_ptr)[0];

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: PDN Address

    Description: Assigns an IPv4 address to the UE associated with a
                 packet data network and provides the UE with an
                 interface identifier to be used to build the IPv6
                 link local address.

    Document Reference: 24.301 v10.2.0 Section 9.9.4.9
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_mme_pack_pdn_address_ie(LIBLTE_MME_PDN_ADDRESS_STRUCT  *pdn_addr,
                                                 uint8                         **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;
    uint32            i;

    if(pdn_addr != NULL &&
       ie_ptr   != NULL)
    {
        (*ie_ptr)[1] = 0x00 | (pdn_addr->pdn_type & 0x07);
        if(LIBLTE_MME_PDN_TYPE_IPV4 == pdn_addr->pdn_type)
        {
            for(i=0; i<4; i++)
            {
                (*ie_ptr)[2+i] = pdn_addr->addr[i];
            }
            (*ie_ptr)[0]  = 5;
            *ie_ptr      += 6;
        }else if(LIBLTE_MME_PDN_TYPE_IPV6 == pdn_addr->pdn_type){
            for(i=0; i<8; i++)
            {
                (*ie_ptr)[2+i] = pdn_addr->addr[i];
            }
            (*ie_ptr)[0]  = 9;
            *ie_ptr      += 10;
        }else{
            for(i=0; i<12; i++)
            {
                (*ie_ptr)[2+i] = pdn_addr->addr[i];
            }
            (*ie_ptr)[0]  = 13;
            *ie_ptr      += 14;
        }

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_mme_unpack_pdn_address_ie(uint8                         **ie_ptr,
                                                   LIBLTE_MME_PDN_ADDRESS_STRUCT  *pdn_addr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;
    uint32            i;

    if(ie_ptr   != NULL &&
       pdn_addr != NULL)
    {
        pdn_addr->pdn_type = (*ie_ptr)[1] & 0x07;
        if(LIBLTE_MME_PDN_TYPE_IPV4 == pdn_addr->pdn_type)
        {
            for(i=0; i<4; i++)
            {
                pdn_addr->addr[i] = (*ie_ptr)[2+i];
            }
        }else if(LIBLTE_MME_PDN_TYPE_IPV6 == pdn_addr->pdn_type){
            for(i=0; i<8; i++)
            {
                pdn_addr->addr[i] = (*ie_ptr)[2+i];
            }
        }else{
            for(i=0; i<12; i++)
            {
                pdn_addr->addr[i] = (*ie_ptr)[2+i];
            }
        }
        *ie_ptr += (*ie_ptr)[0] + 1;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: PDN Type

    Description: Indicates the IP version capability of the IP stack
                 associated with the UE.

    Document Reference: 24.301 v10.2.0 Section 9.9.4.10
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_mme_pack_pdn_type_ie(uint8   pdn_type,
                                              uint8   bit_offset,
                                              uint8 **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr != NULL)
    {
        **ie_ptr |= pdn_type << bit_offset;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_mme_unpack_pdn_type_ie(uint8 **ie_ptr,
                                                uint8   bit_offset,
                                                uint8  *pdn_type)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr   != NULL &&
       pdn_type != NULL)
    {
        *pdn_type = (**ie_ptr >> bit_offset) & 0x07;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: Protocol Configuration Options

    Description: Transfers external network protocol options
                 associated with a PDP context activation and
                 transfers additional (protocol) data (e.g.
                 configuration parameters, error codes or messages/
                 events) associated with an external protocol or an
                 application.

    Document Reference: 24.301 v10.2.0 Section 9.9.4.11
                        24.008 v10.2.0 Section 10.5.6.3
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_mme_pack_protocol_config_options_ie(LIBLTE_MME_PROTOCOL_CONFIG_OPTIONS_STRUCT  *protocol_cnfg_opts,
                                                             uint8                                     **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;
    uint32            idx;
    uint32            i;
    uint32            j;

    if(protocol_cnfg_opts != NULL &&
       ie_ptr             != NULL)
    {
        (*ie_ptr)[1] = 0x80;
        idx          = 2;
        for(i=0; i<protocol_cnfg_opts->N_opts; i++)
        {
            (*ie_ptr)[idx++] = protocol_cnfg_opts->opt[i].id >> 8;
            (*ie_ptr)[idx++] = protocol_cnfg_opts->opt[i].id & 0x00FF;
            (*ie_ptr)[idx++] = protocol_cnfg_opts->opt[i].len;
            for(j=0; j<protocol_cnfg_opts->opt[i].len; j++)
            {
                (*ie_ptr)[idx++] = protocol_cnfg_opts->opt[i].contents[j];
            }
        }
        (*ie_ptr)[0]  = idx - 1;
        *ie_ptr      += idx;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_mme_unpack_protocol_config_options_ie(uint8                                     **ie_ptr,
                                                               LIBLTE_MME_PROTOCOL_CONFIG_OPTIONS_STRUCT  *protocol_cnfg_opts)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;
    uint32            idx;
    uint32            i;

    if(ie_ptr             != NULL &&
       protocol_cnfg_opts != NULL)
    {
        idx                        = 2;
        protocol_cnfg_opts->N_opts = 0;
        while(idx < (*ie_ptr)[0])
        {
            protocol_cnfg_opts->opt[protocol_cnfg_opts->N_opts].id   = (*ie_ptr)[idx++] << 8;
            protocol_cnfg_opts->opt[protocol_cnfg_opts->N_opts].id  |= (*ie_ptr)[idx++];
            protocol_cnfg_opts->opt[protocol_cnfg_opts->N_opts].len  = (*ie_ptr)[idx++];
            for(i=0; i<protocol_cnfg_opts->opt[protocol_cnfg_opts->N_opts].len; i++)
            {
                protocol_cnfg_opts->opt[protocol_cnfg_opts->N_opts].contents[i] = (*ie_ptr)[idx++];
            }
            protocol_cnfg_opts->N_opts++;
        }
        *ie_ptr += (*ie_ptr)[0] + 1;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: Quality Of Service

    Description: Specifies the QoS parameters for a PDP context.

    Document Reference: 24.301 v10.2.0 Section 9.9.4.12
                        24.008 v10.2.0 Section 10.5.6.5
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_mme_pack_quality_of_service_ie(LIBLTE_MME_QUALITY_OF_SERVICE_STRUCT  *qos,
                                                        uint8                                **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(qos    != NULL &&
       ie_ptr != NULL)
    {
        if(qos->dl_ext_present &&
           qos->ul_ext_present)
        {
            (*ie_ptr)[0] = 16;
        }else if(qos->dl_ext_present){
            (*ie_ptr)[0] = 14;
        }else{
            (*ie_ptr)[0] = 12;
        }
        (*ie_ptr)[1]  = ((qos->delay_class & 0x07) << 3) | (qos->reliability_class & 0x07);
        (*ie_ptr)[2]  = ((qos->peak_throughput & 0x0F) << 4) | (qos->precedence_class & 0x07);
        (*ie_ptr)[3]  = qos->mean_throughput & 0x1F;
        (*ie_ptr)[4]  = ((qos->traffic_class & 0x07) << 5) | ((qos->delivery_order & 0x03) << 3) | (qos->delivery_of_erroneous_sdu & 0x03);
        (*ie_ptr)[5]  = qos->max_sdu_size;
        (*ie_ptr)[6]  = qos->mbr_ul;
        (*ie_ptr)[7]  = qos->mbr_dl;
        (*ie_ptr)[8]  = ((qos->residual_ber & 0x0F) << 4) | (qos->sdu_error_ratio & 0x0F);
        (*ie_ptr)[9]  = ((qos->transfer_delay & 0x3F) << 2) | (qos->traffic_handling_prio & 0x03);
        (*ie_ptr)[10] = qos->gbr_ul;
        (*ie_ptr)[11] = qos->gbr_dl;
        (*ie_ptr)[12] = ((qos->signalling_ind & 0x01) << 4) | (qos->source_stats_descriptor & 0x0F);
        if(qos->dl_ext_present)
        {
            (*ie_ptr)[13] = qos->mbr_dl_ext;
            (*ie_ptr)[14] = qos->gbr_dl_ext;
        }
        if(qos->ul_ext_present)
        {
            (*ie_ptr)[15] = qos->mbr_ul_ext;
            (*ie_ptr)[16] = qos->gbr_ul_ext;
        }
        if(qos->dl_ext_present &&
           qos->ul_ext_present)
        {
            *ie_ptr += 17;
        }else if(qos->dl_ext_present){
            *ie_ptr += 15;
        }else{
            *ie_ptr += 13;
        }

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_mme_unpack_quality_of_service_ie(uint8                                **ie_ptr,
                                                          LIBLTE_MME_QUALITY_OF_SERVICE_STRUCT  *qos)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr != NULL &&
       qos    != NULL)
    {
        if(16 == (*ie_ptr)[0])
        {
            qos->dl_ext_present = true;
            qos->ul_ext_present = true;
        }else if(14 == (*ie_ptr)[0]){
            qos->dl_ext_present = true;
            qos->ul_ext_present = false;
        }else{
            qos->dl_ext_present = false;
            qos->ul_ext_present = false;
        }
        qos->delay_class               = ((*ie_ptr)[1] >> 3) & 0x07;
        qos->reliability_class         = (*ie_ptr)[1] & 0x07;
        qos->peak_throughput           = (*ie_ptr)[2] >> 4;
        qos->precedence_class          = (*ie_ptr)[2] & 0x07;
        qos->mean_throughput           = (*ie_ptr)[3] & 0x1F;
        qos->traffic_class             = ((*ie_ptr)[4] >> 5) & 0x07;
        qos->delivery_order            = ((*ie_ptr)[4] >> 3) & 0x03;
        qos->delivery_of_erroneous_sdu = (*ie_ptr)[4] & 0x07;
        qos->max_sdu_size              = (*ie_ptr)[5];
        qos->mbr_ul                    = (*ie_ptr)[6];
        qos->mbr_dl                    = (*ie_ptr)[7];
        qos->residual_ber              = ((*ie_ptr)[8] >> 4) & 0x0F;
        qos->sdu_error_ratio           = (*ie_ptr)[8] & 0x0F;
        qos->transfer_delay            = ((*ie_ptr)[9] >> 2) & 0x3F;
        qos->traffic_handling_prio     = (*ie_ptr)[9] & 0x03;
        qos->gbr_ul                    = (*ie_ptr)[10];
        qos->gbr_dl                    = (*ie_ptr)[11];
        qos->signalling_ind            = ((*ie_ptr)[12] >> 4) & 0x01;
        qos->source_stats_descriptor   = (*ie_ptr)[12] & 0x0F;
        if(qos->dl_ext_present)
        {
            qos->mbr_dl_ext = (*ie_ptr)[13];
            qos->gbr_dl_ext = (*ie_ptr)[14];
        }
        if(qos->ul_ext_present)
        {
            qos->mbr_ul_ext = (*ie_ptr)[15];
            qos->gbr_ul_ext = (*ie_ptr)[16];
        }
        if(qos->dl_ext_present &&
           qos->ul_ext_present)
        {
            *ie_ptr += 17;
        }else if(qos->dl_ext_present){
            *ie_ptr += 15;
        }else{
            *ie_ptr += 13;
        }

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: Radio Priority

    Description: Specifies the priority level the UE shall use at the
                 lower layers for transmission of data related to a
                 PDP context or for mobile originated SMS
                 transmission.

    Document Reference: 24.301 v10.2.0 Section 9.9.4.13
                        24.008 v10.2.0 Section 10.5.7.2
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_mme_pack_radio_priority_ie(uint8   radio_prio,
                                                    uint8   bit_offset,
                                                    uint8 **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr != NULL)
    {
        (*ie_ptr)[0] |= radio_prio << bit_offset;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_mme_unpack_radio_priority_ie(uint8 **ie_ptr,
                                                      uint8   bit_offset,
                                                      uint8  *radio_prio)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr     != NULL &&
       radio_prio != NULL)
    {
        *radio_prio |= ((*ie_ptr)[0] >> bit_offset) & 0x07;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: Request Type

    Description: Indicates whether the UE requests to establish a new
                 connectivity to a PDN or keep the connection(s) to
                 which it has connected via non-3GPP access.

    Document Reference: 24.301 v10.2.0 Section 9.9.4.14
                        24.008 v10.2.0 Section 10.5.6.17
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_mme_pack_request_type_ie(uint8   req_type,
                                                  uint8   bit_offset,
                                                  uint8 **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr != NULL)
    {
        **ie_ptr |= req_type << bit_offset;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_mme_unpack_request_type_ie(uint8 **ie_ptr,
                                                    uint8   bit_offset,
                                                    uint8  *req_type)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr   != NULL &&
       req_type != NULL)
    {
        *req_type = (**ie_ptr >> bit_offset) & 0x07;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: Traffic Flow Aggregate Description

    Description: Specifies the aggregate of one or more packet filters
                 and their related parameters and operations.

    Document Reference: 24.301 v10.2.0 Section 9.9.4.15
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_mme_pack_traffic_flow_aggregate_description_ie(LIBLTE_MME_TRAFFIC_FLOW_AGGREGATE_DESCRIPTION_STRUCT  *tfad,
                                                                        uint8                                                **ie_ptr)
{
    return(liblte_mme_pack_traffic_flow_template_ie((LIBLTE_MME_TRAFFIC_FLOW_TEMPLATE_STRUCT *)tfad, ie_ptr));
}
LIBLTE_ERROR_ENUM liblte_mme_unpack_traffic_flow_aggregate_description_ie(uint8                                                **ie_ptr,
                                                                          LIBLTE_MME_TRAFFIC_FLOW_AGGREGATE_DESCRIPTION_STRUCT  *tfad)
{
    return(liblte_mme_unpack_traffic_flow_template_ie(ie_ptr, (LIBLTE_MME_TRAFFIC_FLOW_TEMPLATE_STRUCT *)tfad));
}

/*********************************************************************
    IE Name: Traffic Flow Template

    Description: Specifies the TFT parameters and operations for a
                 PDP context.

    Document Reference: 24.301 v10.2.0 Section 9.9.4.16
                        24.008 v10.2.0 Section 10.5.6.12
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_mme_pack_traffic_flow_template_ie(LIBLTE_MME_TRAFFIC_FLOW_TEMPLATE_STRUCT  *tft,
                                                           uint8                                   **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;
    uint32            idx;
    uint32            i;
    uint32            j;

    if(tft    != NULL &&
       ie_ptr != NULL)
    {
        idx            = 1;
        (*ie_ptr)[idx] = (tft->tft_op_code & 0x07) << 5;
        if(0 != tft->parameter_list_size)
        {
            (*ie_ptr)[idx] |= 0x10;
        }
        (*ie_ptr)[idx] |= tft->packet_filter_list_size & 0x0F;
        idx++;

        for(i=0; i<tft->packet_filter_list_size; i++)
        {
            (*ie_ptr)[idx]  = (tft->packet_filter_list[i].dir & 0x0F) << 4;
            (*ie_ptr)[idx] |= tft->packet_filter_list[i].id & 0x0F;
            idx++;
            if(LIBLTE_MME_TFT_OPERATION_CODE_DELETE_PACKET_FILTERS_FROM_EXISTING_TFT != tft->tft_op_code)
            {
                (*ie_ptr)[idx] = tft->packet_filter_list[i].eval_precedence;
                idx++;
                (*ie_ptr)[idx] = tft->packet_filter_list[i].filter_size;
                idx++;
                for(j=0; j<tft->packet_filter_list[i].filter_size; j++)
                {
                    (*ie_ptr)[idx] = tft->packet_filter_list[i].filter[j];
                    idx++;
                }
            }
        }

        for(i=0; i<tft->parameter_list_size; i++)
        {
            (*ie_ptr)[idx] = tft->parameter_list[i].id;
            idx++;
            (*ie_ptr)[idx] = tft->parameter_list[i].parameter_size;
            idx++;
            for(j=0; j<tft->parameter_list[i].parameter_size; j++)
            {
                (*ie_ptr)[idx] = tft->parameter_list[i].parameter[j];
                idx++;
            }
        }
        (*ie_ptr)[0]  = idx - 1;
        *ie_ptr      += idx;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_mme_unpack_traffic_flow_template_ie(uint8                                   **ie_ptr,
                                                             LIBLTE_MME_TRAFFIC_FLOW_TEMPLATE_STRUCT  *tft)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;
    uint32            idx;
    uint32            i;
    uint32            j;
    bool              param_list_present;

    if(ie_ptr != NULL &&
       tft    != NULL)
    {
        idx                          = 1;
        tft->tft_op_code             = (LIBLTE_MME_TFT_OPERATION_CODE_ENUM)(((*ie_ptr)[idx] >> 5) & 0x07);
        param_list_present           = ((*ie_ptr)[idx] >> 4) & 0x01;
        tft->packet_filter_list_size = (*ie_ptr)[idx] & 0x0F;
        idx++;

        for(i=0; i<tft->packet_filter_list_size; i++)
        {
            tft->packet_filter_list[i].dir = (LIBLTE_MME_TFT_PACKET_FILTER_DIRECTION_ENUM)(((*ie_ptr)[idx] >> 4) & 0x0F);
            tft->packet_filter_list[i].id  = (*ie_ptr)[idx] & 0x0F;
            idx++;
            if(LIBLTE_MME_TFT_OPERATION_CODE_DELETE_PACKET_FILTERS_FROM_EXISTING_TFT != tft->tft_op_code)
            {
                tft->packet_filter_list[i].eval_precedence = (*ie_ptr)[idx];
                idx++;
                tft->packet_filter_list[i].filter_size = (*ie_ptr)[idx];
                idx++;
                for(j=0; j<tft->packet_filter_list[i].filter_size; j++)
                {
                    tft->packet_filter_list[i].filter[j] = (*ie_ptr)[idx];
                    idx++;
                }
            }
        }

        if(param_list_present)
        {
            tft->parameter_list_size = 0;
            while(idx < (*ie_ptr)[0])
            {
                tft->parameter_list[tft->parameter_list_size].id = (*ie_ptr)[idx];
                idx++;
                tft->parameter_list[tft->parameter_list_size].parameter_size = (*ie_ptr)[idx];
                idx++;
                for(i=0; i<tft->parameter_list[tft->parameter_list_size].parameter_size; i++)
                {
                    tft->parameter_list[tft->parameter_list_size].parameter[i] = (*ie_ptr)[idx];
                    idx++;
                }
                tft->parameter_list_size++;
            }
        }

        *ie_ptr += (*ie_ptr)[0] + 1;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    IE Name: Transaction Identifier

    Description: Represents the corresponding PDP context in A/Gb
                 mode or Iu mode which is mapped from the EPS bearer
                 context.

    Document Reference: 24.301 v10.2.0 Section 9.9.4.17
                        24.008 v10.2.0 Section 10.5.6.7
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_mme_pack_transaction_identifier_ie(LIBLTE_MME_TRANSACTION_IDENTIFIER_STRUCT  *trans_id,
                                                            uint8                                    **ie_ptr)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(trans_id != NULL &&
       ie_ptr   != NULL)
    {
        if(LIBLTE_MME_TI_VALUE_IS_GIVEN_BY_TIE == trans_id->tio)
        {
            (*ie_ptr)[0] = 2;
        }else{
            (*ie_ptr)[0] = 1;
        }
        (*ie_ptr)[1] = ((trans_id->ti_flag & 0x01) << 7) | ((trans_id->tio & 0x07) << 4);
        if(LIBLTE_MME_TI_VALUE_IS_GIVEN_BY_TIE == trans_id->tio)
        {
            (*ie_ptr)[2]  = 0x80 | (trans_id->tie & 0x7F);
            *ie_ptr      += 3;
        }else{
            *ie_ptr += 2;
        }

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_mme_unpack_transaction_identifier_ie(uint8                                    **ie_ptr,
                                                              LIBLTE_MME_TRANSACTION_IDENTIFIER_STRUCT  *trans_id)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if(ie_ptr   != NULL &&
       trans_id != NULL)
    {
        trans_id->ti_flag = (*ie_ptr)[1] >> 7;
        trans_id->tio     = ((*ie_ptr)[1] >> 4) & 0x07;
        if(LIBLTE_MME_TI_VALUE_IS_GIVEN_BY_TIE == trans_id->tio)
        {
            trans_id->tie = (*ie_ptr)[2] & 0x7F;
        }
        *ie_ptr += (*ie_ptr)[0] + 1;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*******************************************************************************
                              MESSAGE FUNCTIONS
*******************************************************************************/

/*********************************************************************
    Message Name: Security Message Header (Plain NAS Message)

    Description: Security header for NAS messages.

    Document Reference: 24.301 v10.2.0 Section 9.1
*********************************************************************/

LIBLTE_ERROR_ENUM liblte_mme_parse_msg_sec_header(LIBLTE_BYTE_MSG_STRUCT *msg,
                               uint8 *pd,
                               uint8 *sec_hdr_type)
{

    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

    if (msg != NULL &&
        pd != NULL &&
        sec_hdr_type != NULL)
    {
        *sec_hdr_type = (uint8) ((msg->msg[0] & 0xF0) >> 4);
         err = LIBLTE_SUCCESS;
    }
    return(err);
}


/*********************************************************************
    Message Name: Message Header (Plain NAS Message)

    Description: Message header for plain NAS messages.

    Document Reference: 24.301 v10.2.0 Section 9.1
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_mme_parse_msg_header(LIBLTE_BYTE_MSG_STRUCT *msg,
                                              uint8                  *pd,
                                              uint8                  *msg_type)
{
    LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             sec_hdr_type;

    if(msg      != NULL &&
       pd       != NULL &&
       msg_type != NULL)
    {
        // Security Header Type
        sec_hdr_type = (msg->msg[0] & 0xF0) >> 4;

        // Protocol Discriminator
        *pd = msg->msg[0] & 0x0F;

        if(LIBLTE_MME_SECURITY_HDR_TYPE_SERVICE_REQUEST == sec_hdr_type)
        {
            *msg_type = LIBLTE_MME_SECURITY_HDR_TYPE_SERVICE_REQUEST;
        }else{
            if(LIBLTE_MME_PD_EPS_SESSION_MANAGEMENT == *pd)
            {
                // Message Type
                *msg_type = msg->msg[2];
            }else{
                if(LIBLTE_MME_SECURITY_HDR_TYPE_PLAIN_NAS == sec_hdr_type)
                {
                    // Message Type
                    *msg_type = msg->msg[1];
                }else{
                    // Protocol Discriminator
                    *pd = msg->msg[6] & 0x0F;

                    if(LIBLTE_MME_PD_EPS_SESSION_MANAGEMENT == *pd)
                    {
                        *msg_type = msg->msg[8];
                    }else{
                        *msg_type = msg->msg[7];
                    }
                }
            }
        }

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_mme_pack_security_protected_nas_msg(LIBLTE_BYTE_MSG_STRUCT *msg,
                                                             uint8                   sec_hdr_type,
                                                             uint32                  count,
                                                             LIBLTE_BYTE_MSG_STRUCT *sec_msg)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr = sec_msg->msg;
    uint32             i;

    if(msg     != NULL &&
       sec_msg != NULL)
    {
        if(LIBLTE_MME_SECURITY_HDR_TYPE_PLAIN_NAS != sec_hdr_type)
        {
            // Protocol Discriminator and Security Header Type
            *msg_ptr = (sec_hdr_type << 4) | (LIBLTE_MME_PD_EPS_MOBILITY_MANAGEMENT);
            msg_ptr++;

            // MAC will be filled in later
            msg_ptr += 4;

            // Sequence Number
            *msg_ptr = count & 0xFF;
            msg_ptr++;
        }

        // NAS Message
        for(i=0; i<msg->N_bytes; i++)
        {
            *msg_ptr = msg->msg[i];
            msg_ptr++;
        }

        // Fill in the number of bytes used
        sec_msg->N_bytes = msg_ptr - sec_msg->msg;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    Message Name: Attach Accept

    Description: Sent by the network to the UE to indicate that the
                 corresponding attach request has been accepted.

    Document Reference: 24.301 v10.2.0 Section 8.2.1
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_mme_pack_attach_accept_msg(LIBLTE_MME_ATTACH_ACCEPT_MSG_STRUCT *attach_accept,
                                                    uint8                                sec_hdr_type,
                                                    uint32                               count,
                                                    LIBLTE_BYTE_MSG_STRUCT              *msg)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr = msg->msg;

    if(attach_accept != NULL &&
       msg           != NULL)
    {
        if(LIBLTE_MME_SECURITY_HDR_TYPE_PLAIN_NAS != sec_hdr_type)
        {
            // Protocol Discriminator and Security Header Type
            *msg_ptr = (sec_hdr_type << 4) | (LIBLTE_MME_PD_EPS_MOBILITY_MANAGEMENT);
            msg_ptr++;

            // MAC will be filled in later
            msg_ptr += 4;

            // Sequence Number
            *msg_ptr = count & 0xFF;
            msg_ptr++;
        }

        // Protocol Discriminator and Security Header Type
        *msg_ptr = (LIBLTE_MME_SECURITY_HDR_TYPE_PLAIN_NAS << 4) | (LIBLTE_MME_PD_EPS_MOBILITY_MANAGEMENT);
        msg_ptr++;

        // Message Type
        *msg_ptr = LIBLTE_MME_MSG_TYPE_ATTACH_ACCEPT;
        msg_ptr++;

        // EPS Attach Result & Spare Half Octet
        *msg_ptr = 0;
        liblte_mme_pack_eps_attach_result_ie(attach_accept->eps_attach_result, 0, &msg_ptr);
        msg_ptr++;

        // T3412 Value
        liblte_mme_pack_gprs_timer_ie(&attach_accept->t3412, &msg_ptr);

        // TAI List
        liblte_mme_pack_tracking_area_identity_list_ie(&attach_accept->tai_list, &msg_ptr);

        // ESM Message Container
        liblte_mme_pack_esm_message_container_ie(&attach_accept->esm_msg, &msg_ptr);

        // GUTI
        if(attach_accept->guti_present)
        {
            *msg_ptr = LIBLTE_MME_GUTI_IEI;
            msg_ptr++;
            liblte_mme_pack_eps_mobile_id_ie(&attach_accept->guti, &msg_ptr);
        }

        // Location Area Identification
        if(attach_accept->lai_present)
        {
            *msg_ptr = LIBLTE_MME_LOCATION_AREA_IDENTIFICATION_IEI;
            msg_ptr++;
            liblte_mme_pack_location_area_id_ie(&attach_accept->lai, &msg_ptr);
        }

        // MS Identity
        if(attach_accept->ms_id_present)
        {
            *msg_ptr = LIBLTE_MME_MS_IDENTITY_IEI;
            msg_ptr++;
            liblte_mme_pack_mobile_id_ie(&attach_accept->ms_id, &msg_ptr);
        }

        // EMM Cause
        if(attach_accept->emm_cause_present)
        {
            *msg_ptr = LIBLTE_MME_EMM_CAUSE_IEI;
            msg_ptr++;
            liblte_mme_pack_emm_cause_ie(attach_accept->emm_cause, &msg_ptr);
        }

        // T3402 Value
        if(attach_accept->t3402_present)
        {
            *msg_ptr = LIBLTE_MME_T3402_VALUE_IEI;
            msg_ptr++;
            liblte_mme_pack_gprs_timer_ie(&attach_accept->t3402, &msg_ptr);
        }

        // T3423 Value
        if(attach_accept->t3423_present)
        {
            *msg_ptr = LIBLTE_MME_T3423_VALUE_IEI;
            msg_ptr++;
            liblte_mme_pack_gprs_timer_ie(&attach_accept->t3423, &msg_ptr);
        }

        // Equivalent PLMNs
        if(attach_accept->equivalent_plmns_present)
        {
            *msg_ptr = LIBLTE_MME_EQUIVALENT_PLMNS_IEI;
            msg_ptr++;
            liblte_mme_pack_plmn_list_ie(&attach_accept->equivalent_plmns, &msg_ptr);
        }

        // Emergency Number List
        if(attach_accept->emerg_num_list_present)
        {
            *msg_ptr = LIBLTE_MME_EMERGENCY_NUMBER_LIST_IEI;
            msg_ptr++;
            liblte_mme_pack_emergency_number_list_ie(&attach_accept->emerg_num_list, &msg_ptr);
        }

        // EPS Network Feature Support
        if(attach_accept->eps_network_feature_support_present)
        {
            *msg_ptr = LIBLTE_MME_EPS_NETWORK_FEATURE_SUPPORT_IEI;
            msg_ptr++;
            liblte_mme_pack_eps_network_feature_support_ie(&attach_accept->eps_network_feature_support, &msg_ptr);
        }

        // Additional Update Result
        if(attach_accept->additional_update_result_present)
        {
            *msg_ptr = LIBLTE_MME_ADDITIONAL_UPDATE_RESULT_IEI << 4;
            liblte_mme_pack_additional_update_result_ie(attach_accept->additional_update_result, 0, &msg_ptr);
            msg_ptr++;
        }

        // T3412 Extended Value
        if(attach_accept->t3412_ext_present)
        {
            *msg_ptr = LIBLTE_MME_T3412_EXTENDED_VALUE_IEI;
            msg_ptr++;
            liblte_mme_pack_gprs_timer_3_ie(&attach_accept->t3412_ext, &msg_ptr);
        }

        // Fill in the number of bytes used
        msg->N_bytes = msg_ptr - msg->msg;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_mme_unpack_attach_accept_msg(LIBLTE_BYTE_MSG_STRUCT              *msg,
                                                      LIBLTE_MME_ATTACH_ACCEPT_MSG_STRUCT *attach_accept)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr = msg->msg;
    uint8              sec_hdr_type;

    if(msg           != NULL &&
       attach_accept != NULL)
    {
        // Security Header Type
        sec_hdr_type = (msg->msg[0] & 0xF0) >> 4;
        if(LIBLTE_MME_SECURITY_HDR_TYPE_PLAIN_NAS == sec_hdr_type)
        {
            msg_ptr++;
        }else{
            msg_ptr += 7;
        }

        // Skip Message Type
        msg_ptr++;

        // EPS Attach Result & Spare Half Octet
        liblte_mme_unpack_eps_attach_result_ie(&msg_ptr, 0, &attach_accept->eps_attach_result);
        msg_ptr++;

        // T3412 Value
        liblte_mme_unpack_gprs_timer_ie(&msg_ptr, &attach_accept->t3412);

        // TAI List
        liblte_mme_unpack_tracking_area_identity_list_ie(&msg_ptr, &attach_accept->tai_list);

        // ESM Message Container
        liblte_mme_unpack_esm_message_container_ie(&msg_ptr, &attach_accept->esm_msg);

        // GUTI
        if(LIBLTE_MME_GUTI_IEI == *msg_ptr)
        {
            msg_ptr++;
            liblte_mme_unpack_eps_mobile_id_ie(&msg_ptr, &attach_accept->guti);
            attach_accept->guti_present = true;
        }else{
            attach_accept->guti_present = false;
        }

        // Location Area Identification
        if(LIBLTE_MME_LOCATION_AREA_IDENTIFICATION_IEI == *msg_ptr)
        {
            msg_ptr++;
            liblte_mme_unpack_location_area_id_ie(&msg_ptr, &attach_accept->lai);
            attach_accept->lai_present = true;
        }else{
            attach_accept->lai_present = false;
        }

        // MS Identity
        if(LIBLTE_MME_MS_IDENTITY_IEI == *msg_ptr)
        {
            msg_ptr++;
            liblte_mme_unpack_mobile_id_ie(&msg_ptr, &attach_accept->ms_id);
            attach_accept->ms_id_present = true;
        }else{
            attach_accept->ms_id_present = false;
        }

        // EMM Cause
        if(LIBLTE_MME_EMM_CAUSE_IEI == *msg_ptr)
        {
            msg_ptr++;
            liblte_mme_unpack_emm_cause_ie(&msg_ptr, &attach_accept->emm_cause);
            attach_accept->emm_cause_present = true;
        }else{
            attach_accept->emm_cause_present = false;
        }

        // T3402 Value
        if(LIBLTE_MME_T3402_VALUE_IEI == *msg_ptr)
        {
            msg_ptr++;
            liblte_mme_unpack_gprs_timer_ie(&msg_ptr, &attach_accept->t3402);
            attach_accept->t3402_present = true;
        }else{
            attach_accept->t3402_present = false;
        }

        // T3423 Value
        if(LIBLTE_MME_T3423_VALUE_IEI == *msg_ptr)
        {
            msg_ptr++;
            liblte_mme_unpack_gprs_timer_ie(&msg_ptr, &attach_accept->t3423);
            attach_accept->t3423_present = true;
        }else{
            attach_accept->t3423_present = false;
        }

        // Equivalent PLMNs
        if(LIBLTE_MME_EQUIVALENT_PLMNS_IEI == *msg_ptr)
        {
            msg_ptr++;
            liblte_mme_unpack_plmn_list_ie(&msg_ptr, &attach_accept->equivalent_plmns);
            attach_accept->equivalent_plmns_present = true;
        }else{
            attach_accept->equivalent_plmns_present = false;
        }

        // Emergency Number List
        if(LIBLTE_MME_EMERGENCY_NUMBER_LIST_IEI == *msg_ptr)
        {
            msg_ptr++;
            liblte_mme_unpack_emergency_number_list_ie(&msg_ptr, &attach_accept->emerg_num_list);
            attach_accept->emerg_num_list_present = true;
        }else{
            attach_accept->emerg_num_list_present = false;
        }

        // EPS Network Feature Support
        if(LIBLTE_MME_EPS_NETWORK_FEATURE_SUPPORT_IEI == *msg_ptr)
        {
            msg_ptr++;
            liblte_mme_unpack_eps_network_feature_support_ie(&msg_ptr, &attach_accept->eps_network_feature_support);
            attach_accept->eps_network_feature_support_present = true;
        }else{
            attach_accept->eps_network_feature_support_present = false;
        }

        // Additional Update Result
        if((LIBLTE_MME_ADDITIONAL_UPDATE_RESULT_IEI << 4) == (*msg_ptr & 0xF0))
        {
            liblte_mme_unpack_additional_update_result_ie(&msg_ptr, 0, &attach_accept->additional_update_result);
            msg_ptr++;
            attach_accept->additional_update_result_present = true;
        }else{
            attach_accept->additional_update_result_present = false;
        }

        // T3412 Extended Value
        if(LIBLTE_MME_T3412_EXTENDED_VALUE_IEI == *msg_ptr)
        {
            msg_ptr++;
            liblte_mme_unpack_gprs_timer_3_ie(&msg_ptr, &attach_accept->t3412_ext);
            attach_accept->t3412_ext_present = true;
        }else{
            attach_accept->t3412_ext_present = false;
        }

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    Message Name: Attach Complete

    Description: Sent by the UE to the network in response to an
                 ATTACH ACCEPT message.

    Document Reference: 24.301 v10.2.0 Section 8.2.2
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_mme_pack_attach_complete_msg(LIBLTE_MME_ATTACH_COMPLETE_MSG_STRUCT *attach_comp,
                                                      uint8                                  sec_hdr_type,
                                                      uint32                                 count,
                                                      LIBLTE_BYTE_MSG_STRUCT                *msg)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr = msg->msg;

    if(attach_comp != NULL &&
       msg         != NULL)
    {
        if(LIBLTE_MME_SECURITY_HDR_TYPE_PLAIN_NAS != sec_hdr_type)
        {
            // Protocol Discriminator and Security Header Type
            *msg_ptr = (sec_hdr_type << 4) | (LIBLTE_MME_PD_EPS_MOBILITY_MANAGEMENT);
            msg_ptr++;

            // MAC will be filled in later
            msg_ptr += 4;

            // Sequence Number
            *msg_ptr = count & 0xFF;
            msg_ptr++;
        }

        // Protocol Discriminator and Security Header Type
        *msg_ptr = (LIBLTE_MME_SECURITY_HDR_TYPE_PLAIN_NAS << 4) | (LIBLTE_MME_PD_EPS_MOBILITY_MANAGEMENT);
        msg_ptr++;

        // Message Type
        *msg_ptr = LIBLTE_MME_MSG_TYPE_ATTACH_COMPLETE;
        msg_ptr++;

        // ESM Message Container
        liblte_mme_pack_esm_message_container_ie(&attach_comp->esm_msg, &msg_ptr);

        // Fill in the number of bytes used
        msg->N_bytes = msg_ptr - msg->msg;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_mme_unpack_attach_complete_msg(LIBLTE_BYTE_MSG_STRUCT                *msg,
                                                        LIBLTE_MME_ATTACH_COMPLETE_MSG_STRUCT *attach_comp)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr = msg->msg;
    uint8              sec_hdr_type;

    if(msg         != NULL &&
       attach_comp != NULL)
    {
        // Security Header Type
        sec_hdr_type = (msg->msg[0] & 0xF0) >> 4;
        if(LIBLTE_MME_SECURITY_HDR_TYPE_PLAIN_NAS == sec_hdr_type)
        {
            msg_ptr++;
        }else{
            msg_ptr += 7;
        }

        // Skip Message Type
        msg_ptr++;

        // ESM Message Container
        liblte_mme_unpack_esm_message_container_ie(&msg_ptr, &attach_comp->esm_msg);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    Message Name: Attach Reject

    Description: Sent by the network to the UE to indicate that the
                 corresponding attach request has been rejected.

    Document Reference: 24.301 v10.2.0 Section 8.2.3
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_mme_pack_attach_reject_msg(LIBLTE_MME_ATTACH_REJECT_MSG_STRUCT *attach_rej,
                                                    LIBLTE_BYTE_MSG_STRUCT              *msg)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr = msg->msg;

    if(attach_rej != NULL &&
       msg        != NULL)
    {
        // Protocol Discriminator and Security Header Type
        *msg_ptr = (LIBLTE_MME_SECURITY_HDR_TYPE_PLAIN_NAS << 4) | (LIBLTE_MME_PD_EPS_MOBILITY_MANAGEMENT);
        msg_ptr++;

        // Message Type
        *msg_ptr = LIBLTE_MME_MSG_TYPE_ATTACH_REJECT;
        msg_ptr++;

        // EMM Cause
        liblte_mme_pack_emm_cause_ie(attach_rej->emm_cause, &msg_ptr);

        // ESM Message Container
        if(attach_rej->esm_msg_present)
        {
            *msg_ptr = LIBLTE_MME_ESM_MSG_CONTAINER_IEI;
            msg_ptr++;
            liblte_mme_pack_esm_message_container_ie(&attach_rej->esm_msg, &msg_ptr);
        }

        // T3446 Value
        if(attach_rej->t3446_value_present)
        {
            *msg_ptr = LIBLTE_MME_T3446_VALUE_IEI;
            msg_ptr++;
            liblte_mme_pack_gprs_timer_2_ie(attach_rej->t3446_value, &msg_ptr);
        }

        // Fill in the number of bytes used
        msg->N_bytes = msg_ptr - msg->msg;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_mme_unpack_attach_reject_msg(LIBLTE_BYTE_MSG_STRUCT              *msg,
                                                      LIBLTE_MME_ATTACH_REJECT_MSG_STRUCT *attach_rej)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr = msg->msg;
    uint8              sec_hdr_type;

    if(msg        != NULL &&
       attach_rej != NULL)
    {
        // Security Header Type
        sec_hdr_type = (msg->msg[0] & 0xF0) >> 4;
        if(LIBLTE_MME_SECURITY_HDR_TYPE_PLAIN_NAS == sec_hdr_type)
        {
            msg_ptr++;
        }else{
            msg_ptr += 7;
        }

        // Skip Message Type
        msg_ptr++;

        // EMM Cause
        liblte_mme_unpack_emm_cause_ie(&msg_ptr, &attach_rej->emm_cause);

        // ESM Message Container
        if(LIBLTE_MME_ESM_MSG_CONTAINER_IEI == *msg_ptr)
        {
            msg_ptr++;
            liblte_mme_unpack_esm_message_container_ie(&msg_ptr, &attach_rej->esm_msg);
            attach_rej->esm_msg_present = true;
        }else{
            attach_rej->esm_msg_present = false;
        }

        // T3446 Value
        if(LIBLTE_MME_T3446_VALUE_IEI == *msg_ptr)
        {
            msg_ptr++;
            liblte_mme_unpack_gprs_timer_2_ie(&msg_ptr, &attach_rej->t3446_value);
            attach_rej->t3446_value_present = true;
        }else{
            attach_rej->t3446_value_present = false;
        }

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    Message Name: Attach Request

    Description: Sent by the UE to the network to perform an attach
                 procedure.

    Document Reference: 24.301 v10.2.0 Section 8.2.4
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_mme_pack_attach_request_msg(LIBLTE_MME_ATTACH_REQUEST_MSG_STRUCT *attach_req,
                                                     LIBLTE_BYTE_MSG_STRUCT               *msg)
{
    return liblte_mme_pack_attach_request_msg(attach_req, LIBLTE_MME_SECURITY_HDR_TYPE_PLAIN_NAS, 0, msg);
}

LIBLTE_ERROR_ENUM liblte_mme_pack_attach_request_msg(LIBLTE_MME_ATTACH_REQUEST_MSG_STRUCT *attach_req,
                                                     uint8                                 sec_hdr_type,
                                                     uint32                                count,
                                                     LIBLTE_BYTE_MSG_STRUCT               *msg)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr = msg->msg;

    if(attach_req != NULL &&
       msg        != NULL)
    {
        if(LIBLTE_MME_SECURITY_HDR_TYPE_PLAIN_NAS != sec_hdr_type)
        {
            // Protocol Discriminator and Security Header Type
            *msg_ptr = (sec_hdr_type << 4) | (LIBLTE_MME_PD_EPS_MOBILITY_MANAGEMENT);
            msg_ptr++;

            // MAC will be filled in later
            msg_ptr += 4;

            // Sequence Number
            *msg_ptr = count & 0xFF;
            msg_ptr++;
        }

        // Protocol Discriminator and Security Header Type
        *msg_ptr = (LIBLTE_MME_SECURITY_HDR_TYPE_PLAIN_NAS << 4) | (LIBLTE_MME_PD_EPS_MOBILITY_MANAGEMENT);
        msg_ptr++;

        // Message Type
        *msg_ptr = LIBLTE_MME_MSG_TYPE_ATTACH_REQUEST;
        msg_ptr++;

        // EPS Attach Type & NAS Key Set Identifier
        *msg_ptr = 0;
        liblte_mme_pack_eps_attach_type_ie(attach_req->eps_attach_type, 0, &msg_ptr);
        liblte_mme_pack_nas_key_set_id_ie(&attach_req->nas_ksi, 4, &msg_ptr);
        msg_ptr++;

        // EPS Mobile ID
        liblte_mme_pack_eps_mobile_id_ie(&attach_req->eps_mobile_id, &msg_ptr);

        // UE Network Capability
        liblte_mme_pack_ue_network_capability_ie(&attach_req->ue_network_cap, &msg_ptr);

        // ESM Message Container
        liblte_mme_pack_esm_message_container_ie(&attach_req->esm_msg, &msg_ptr);

        // Old P-TMSI Signature
        if(attach_req->old_p_tmsi_signature_present)
        {
            *msg_ptr = LIBLTE_MME_P_TMSI_SIGNATURE_IEI;
            msg_ptr++;
            liblte_mme_pack_p_tmsi_signature_ie(attach_req->old_p_tmsi_signature, &msg_ptr);
        }

        // Additional GUTI
        if(attach_req->additional_guti_present)
        {
            *msg_ptr = LIBLTE_MME_ADDITIONAL_GUTI_IEI;
            msg_ptr++;
            liblte_mme_pack_eps_mobile_id_ie(&attach_req->additional_guti, &msg_ptr);
        }

        // Last Visited Registered TAI
        if(attach_req->last_visited_registered_tai_present)
        {
            *msg_ptr = LIBLTE_MME_LAST_VISITED_REGISTERED_TAI_IEI;
            msg_ptr++;
            liblte_mme_pack_tracking_area_id_ie(&attach_req->last_visited_registered_tai, &msg_ptr);
        }

        // DRX Parameter
        if(attach_req->drx_param_present)
        {
            *msg_ptr = LIBLTE_MME_DRX_PARAMETER_IEI;
            msg_ptr++;
            liblte_mme_pack_drx_parameter_ie(&attach_req->drx_param, &msg_ptr);
        }

        // MS Network Capability
        if(attach_req->ms_network_cap_present)
        {
            *msg_ptr = LIBLTE_MME_MS_NETWORK_CAPABILITY_IEI;
            msg_ptr++;
            liblte_mme_pack_ms_network_capability_ie(&attach_req->ms_network_cap, &msg_ptr);
        }

        // Old Location Area ID
        if(attach_req->old_lai_present)
        {
            *msg_ptr = LIBLTE_MME_LOCATION_AREA_IDENTIFICATION_IEI;
            msg_ptr++;
            liblte_mme_pack_location_area_id_ie(&attach_req->old_lai, &msg_ptr);
        }

        // TMSI Status
        if(attach_req->tmsi_status_present)
        {
            *msg_ptr = LIBLTE_MME_TMSI_STATUS_IEI << 4;
            liblte_mme_pack_tmsi_status_ie(attach_req->tmsi_status, 0, &msg_ptr);
            msg_ptr++;
        }

        // Mobile Station Classmark 2
        if(attach_req->ms_cm2_present)
        {
            *msg_ptr = LIBLTE_MME_MS_CLASSMARK_2_IEI;
            msg_ptr++;
            liblte_mme_pack_mobile_station_classmark_2_ie(&attach_req->ms_cm2, &msg_ptr);
        }

        // Mobile Station Classmark 3
        if(attach_req->ms_cm3_present)
        {
            *msg_ptr = LIBLTE_MME_MS_CLASSMARK_3_IEI;
            msg_ptr++;
            liblte_mme_pack_mobile_station_classmark_3_ie(&attach_req->ms_cm3, &msg_ptr);
        }

        // Supported Codecs
        if(attach_req->supported_codecs_present)
        {
            *msg_ptr = LIBLTE_MME_SUPPORTED_CODEC_LIST_IEI;
            msg_ptr++;
            liblte_mme_pack_supported_codec_list_ie(&attach_req->supported_codecs, &msg_ptr);
        }

        // Additional Update Type
        if(attach_req->additional_update_type_present)
        {
            *msg_ptr = LIBLTE_MME_ADDITIONAL_UPDATE_TYPE_IEI << 4;
            liblte_mme_pack_additional_update_type_ie(attach_req->additional_update_type, 0, &msg_ptr);
            msg_ptr++;
        }

        // Voice Domain Preference and UE's Usage Setting
        if(attach_req->voice_domain_pref_and_ue_usage_setting_present)
        {
            *msg_ptr = LIBLTE_MME_VOICE_DOMAIN_PREF_AND_UE_USAGE_SETTING_IEI;
            msg_ptr++;
            liblte_mme_pack_voice_domain_pref_and_ue_usage_setting_ie(&attach_req->voice_domain_pref_and_ue_usage_setting, &msg_ptr);
        }

        // Device Properties
        if(attach_req->device_properties_present)
        {
            *msg_ptr = LIBLTE_MME_ATTACH_REQUEST_DEVICE_PROPERTIES_IEI << 4;
            liblte_mme_pack_device_properties_ie(attach_req->device_properties, 0, &msg_ptr);
            msg_ptr++;
        }

        // Old GUTI Type
        if(attach_req->old_guti_type_present)
        {
            *msg_ptr = LIBLTE_MME_GUTI_TYPE_IEI << 4;
            liblte_mme_pack_guti_type_ie(attach_req->old_guti_type, 0, &msg_ptr);
            msg_ptr++;
        }

        // Fill in the number of bytes used
        msg->N_bytes = msg_ptr - msg->msg;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_mme_unpack_attach_request_msg(LIBLTE_BYTE_MSG_STRUCT               *msg,
                                                       LIBLTE_MME_ATTACH_REQUEST_MSG_STRUCT *attach_req)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr = msg->msg;
    uint8              sec_hdr_type;

    if(msg        != NULL &&
       attach_req != NULL)
    {
        // Security Header Type
        sec_hdr_type = (msg->msg[0] & 0xF0) >> 4;
        if(LIBLTE_MME_SECURITY_HDR_TYPE_PLAIN_NAS == sec_hdr_type)
        {
            msg_ptr++;
        }else{
            msg_ptr += 7;
        }

        // Skip Message Type
        msg_ptr++;

        // EPS Attach Type & NAS Key Set Identifier
        liblte_mme_unpack_eps_attach_type_ie(&msg_ptr, 0, &attach_req->eps_attach_type);
        liblte_mme_unpack_nas_key_set_id_ie(&msg_ptr, 4, &attach_req->nas_ksi);
        msg_ptr++;

        // EPS Mobile ID
        liblte_mme_unpack_eps_mobile_id_ie(&msg_ptr, &attach_req->eps_mobile_id);

        // UE Network Capability
        liblte_mme_unpack_ue_network_capability_ie(&msg_ptr, &attach_req->ue_network_cap);

        // ESM Message Container
        liblte_mme_unpack_esm_message_container_ie(&msg_ptr, &attach_req->esm_msg);

        // Old P-TMSI Signature
        if(LIBLTE_MME_P_TMSI_SIGNATURE_IEI == *msg_ptr)
        {
            msg_ptr++;
            liblte_mme_unpack_p_tmsi_signature_ie(&msg_ptr, &attach_req->old_p_tmsi_signature);
            attach_req->old_p_tmsi_signature_present = true;
        }else{
            attach_req->old_p_tmsi_signature_present = false;
        }

        // Additional GUTI
        if(LIBLTE_MME_ADDITIONAL_GUTI_IEI == *msg_ptr)
        {
            msg_ptr++;
            liblte_mme_unpack_eps_mobile_id_ie(&msg_ptr, &attach_req->additional_guti);
            attach_req->additional_guti_present = true;
        }else{
            attach_req->additional_guti_present = false;
        }

        // Last Visited Registered TAI
        if(LIBLTE_MME_LAST_VISITED_REGISTERED_TAI_IEI == *msg_ptr)
        {
            msg_ptr++;
            liblte_mme_unpack_tracking_area_id_ie(&msg_ptr, &attach_req->last_visited_registered_tai);
            attach_req->last_visited_registered_tai_present = true;
        }else{
            attach_req->last_visited_registered_tai_present = false;
        }

        // DRX Parameter
        if(LIBLTE_MME_DRX_PARAMETER_IEI == *msg_ptr)
        {
            msg_ptr++;
            liblte_mme_unpack_drx_parameter_ie(&msg_ptr, &attach_req->drx_param);
            attach_req->drx_param_present = true;
        }else{
            attach_req->drx_param_present = false;
        }

        // MS Network Capability
        if(LIBLTE_MME_MS_NETWORK_CAPABILITY_IEI == *msg_ptr)
        {
            msg_ptr++;
            liblte_mme_unpack_ms_network_capability_ie(&msg_ptr, &attach_req->ms_network_cap);
            attach_req->ms_network_cap_present = true;
        }else{
            attach_req->ms_network_cap_present = false;
        }

        // Old Location Area ID
        if(LIBLTE_MME_LOCATION_AREA_IDENTIFICATION_IEI == *msg_ptr)
        {
            msg_ptr++;
            liblte_mme_unpack_location_area_id_ie(&msg_ptr, &attach_req->old_lai);
            attach_req->old_lai_present = true;
        }else{
            attach_req->old_lai_present = false;
        }

        // TMSI Status
        if((LIBLTE_MME_TMSI_STATUS_IEI << 4) == (*msg_ptr & 0xF0))
        {
            liblte_mme_unpack_tmsi_status_ie(&msg_ptr, 0, &attach_req->tmsi_status);
            msg_ptr++;
            attach_req->tmsi_status_present = true;
        }else{
            attach_req->tmsi_status_present = false;
        }

        // Mobile Station Classmark 2
        if(LIBLTE_MME_MS_CLASSMARK_2_IEI == *msg_ptr)
        {
            msg_ptr++;
            liblte_mme_unpack_mobile_station_classmark_2_ie(&msg_ptr, &attach_req->ms_cm2);
            attach_req->ms_cm2_present = true;
        }else{
            attach_req->ms_cm2_present = false;
        }

        // Mobile Station Classmark 3
        if(LIBLTE_MME_MS_CLASSMARK_3_IEI == *msg_ptr)
        {
            msg_ptr++;
            liblte_mme_unpack_mobile_station_classmark_3_ie(&msg_ptr, &attach_req->ms_cm3);
            attach_req->ms_cm3_present = true;
        }else{
            attach_req->ms_cm3_present = false;
        }

        // Supported Codecs
        if(LIBLTE_MME_SUPPORTED_CODEC_LIST_IEI == *msg_ptr)
        {
            msg_ptr++;
            liblte_mme_unpack_supported_codec_list_ie(&msg_ptr, &attach_req->supported_codecs);
            attach_req->supported_codecs_present = true;
        }else{
            attach_req->supported_codecs_present = false;
        }

        // Additional Update Type
        if((LIBLTE_MME_ADDITIONAL_UPDATE_TYPE_IEI << 4) == (*msg_ptr & 0xF0))
        {
            liblte_mme_unpack_additional_update_type_ie(&msg_ptr, 0, &attach_req->additional_update_type);
            msg_ptr++;
            attach_req->additional_update_type_present = true;
        }else{
            attach_req->additional_update_type_present = false;
        }

        // Voice Domain Preference and UE's Usage Setting
        if(LIBLTE_MME_VOICE_DOMAIN_PREF_AND_UE_USAGE_SETTING_IEI == *msg_ptr)
        {
            msg_ptr++;
            liblte_mme_unpack_voice_domain_pref_and_ue_usage_setting_ie(&msg_ptr, &attach_req->voice_domain_pref_and_ue_usage_setting);
            attach_req->voice_domain_pref_and_ue_usage_setting_present = true;
        }else{
            attach_req->voice_domain_pref_and_ue_usage_setting_present = false;
        }

        // Device Properties
        if((LIBLTE_MME_ATTACH_REQUEST_DEVICE_PROPERTIES_IEI << 4) == (*msg_ptr & 0xF0))
        {
            liblte_mme_unpack_device_properties_ie(&msg_ptr, 0, &attach_req->device_properties);
            msg_ptr++;
            attach_req->device_properties_present = true;
        }else{
            attach_req->device_properties_present = false;
        }

        // Old GUTI Type
        if((LIBLTE_MME_GUTI_TYPE_IEI << 4) == (*msg_ptr & 0xF0))
        {
            liblte_mme_unpack_guti_type_ie(&msg_ptr, 0, &attach_req->old_guti_type);
            msg_ptr++;
            attach_req->old_guti_type_present = true;
        }else{
            attach_req->old_guti_type_present = false;
        }

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    Message Name: Authentication Failure

    Description: Sent by the UE to the network to indicate that
                 authentication of the network has failed.

    Document Reference: 24.301 v10.2.0 Section 8.2.5
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_mme_pack_authentication_failure_msg(LIBLTE_MME_AUTHENTICATION_FAILURE_MSG_STRUCT *auth_fail,
                                                             LIBLTE_BYTE_MSG_STRUCT                       *msg)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr = msg->msg;

    if(auth_fail != NULL &&
       msg       != NULL)
    {
        // Protocol Discriminator and Security Header Type
        *msg_ptr = (LIBLTE_MME_SECURITY_HDR_TYPE_PLAIN_NAS << 4) | (LIBLTE_MME_PD_EPS_MOBILITY_MANAGEMENT);
        msg_ptr++;

        // Message Type
        *msg_ptr = LIBLTE_MME_MSG_TYPE_AUTHENTICATION_FAILURE;
        msg_ptr++;

        // EMM Cause
        liblte_mme_pack_emm_cause_ie(auth_fail->emm_cause, &msg_ptr);

        // Authentication Failure Parameter
        if(auth_fail->auth_fail_param_present)
        {
            *msg_ptr = LIBLTE_MME_AUTHENTICATION_FAILURE_PARAMETER_IEI;
            msg_ptr++;
            liblte_mme_pack_authentication_failure_parameter_ie(auth_fail->auth_fail_param, &msg_ptr);
        }

        // Fill in the number of bytes used
        msg->N_bytes = msg_ptr - msg->msg;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_mme_unpack_authentication_failure_msg(LIBLTE_BYTE_MSG_STRUCT                       *msg,
                                                               LIBLTE_MME_AUTHENTICATION_FAILURE_MSG_STRUCT *auth_fail)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr = msg->msg;
    uint8              sec_hdr_type;

    if(msg       != NULL &&
       auth_fail != NULL)
    {
        // Security Header Type
        sec_hdr_type = (msg->msg[0] & 0xF0) >> 4;
        if(LIBLTE_MME_SECURITY_HDR_TYPE_PLAIN_NAS == sec_hdr_type)
        {
            msg_ptr++;
        }else{
            msg_ptr += 7;
        }

        // Skip Message Type
        msg_ptr++;

        // EMM Cause
        liblte_mme_unpack_emm_cause_ie(&msg_ptr, &auth_fail->emm_cause);

        // Authentication Failure Parameter
        if(LIBLTE_MME_AUTHENTICATION_FAILURE_PARAMETER_IEI == *msg_ptr)
        {
            msg_ptr++;
            liblte_mme_unpack_authentication_failure_parameter_ie(&msg_ptr, auth_fail->auth_fail_param);
            auth_fail->auth_fail_param_present = true;
        }else{
            auth_fail->auth_fail_param_present = false;
        }

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    Message Name: Authentication Reject

    Description: Sent by the network to the UE to indicate that the
                 authentication procedure has failed and that the UE
                 shall abort all activities.

    Document Reference: 24.301 v10.2.0 Section 8.2.6
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_mme_pack_authentication_reject_msg(LIBLTE_MME_AUTHENTICATION_REJECT_MSG_STRUCT *auth_reject,
                                                            LIBLTE_BYTE_MSG_STRUCT                      *msg)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr = msg->msg;

    if(auth_reject != NULL &&
       msg         != NULL)
    {
        // Protocol Discriminator and Security Header Type
        *msg_ptr = (LIBLTE_MME_SECURITY_HDR_TYPE_PLAIN_NAS << 4) | (LIBLTE_MME_PD_EPS_MOBILITY_MANAGEMENT);
        msg_ptr++;

        // Message Type
        *msg_ptr = LIBLTE_MME_MSG_TYPE_AUTHENTICATION_REJECT;
        msg_ptr++;

        // Fill in the number of bytes used
        msg->N_bytes = msg_ptr - msg->msg;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_mme_unpack_authentication_reject_msg(LIBLTE_BYTE_MSG_STRUCT                      *msg,
                                                              LIBLTE_MME_AUTHENTICATION_REJECT_MSG_STRUCT *auth_reject)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr = msg->msg;
    uint8              sec_hdr_type;

    if(msg         != NULL &&
       auth_reject != NULL)
    {
        // Security Header Type
        sec_hdr_type = (msg->msg[0] & 0xF0) >> 4;
        if(LIBLTE_MME_SECURITY_HDR_TYPE_PLAIN_NAS == sec_hdr_type)
        {
            msg_ptr++;
        }else{
            msg_ptr += 7;
        }

        // Skip Message Type
        msg_ptr++;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    Message Name: Authentication Request

    Description: Sent by the network to the UE to initiate
                 authentication of the UE identity.

    Document Reference: 24.301 v10.2.0 Section 8.2.7
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_mme_pack_authentication_request_msg(LIBLTE_MME_AUTHENTICATION_REQUEST_MSG_STRUCT *auth_req,
                                                             LIBLTE_BYTE_MSG_STRUCT                       *msg)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr = msg->msg;

    if(auth_req != NULL &&
       msg      != NULL)
    {
        // Protocol Discriminator and Security Header Type
        *msg_ptr = (LIBLTE_MME_SECURITY_HDR_TYPE_PLAIN_NAS << 4) | (LIBLTE_MME_PD_EPS_MOBILITY_MANAGEMENT);
        msg_ptr++;

        // Message Type
        *msg_ptr = LIBLTE_MME_MSG_TYPE_AUTHENTICATION_REQUEST;
        msg_ptr++;

        // NAS Key Set Identifier & Spare Half Octet
        *msg_ptr = 0;
        liblte_mme_pack_nas_key_set_id_ie(&auth_req->nas_ksi, 0, &msg_ptr);
        msg_ptr++;

        // Authentication Parameter RAND
        liblte_mme_pack_authentication_parameter_rand_ie(auth_req->rand, &msg_ptr);

        // Authentication Parameter AUTN
        liblte_mme_pack_authentication_parameter_autn_ie(auth_req->autn, &msg_ptr);

        // Fill in the number of bytes used
        msg->N_bytes = msg_ptr - msg->msg;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_mme_unpack_authentication_request_msg(LIBLTE_BYTE_MSG_STRUCT                       *msg,
                                                               LIBLTE_MME_AUTHENTICATION_REQUEST_MSG_STRUCT *auth_req)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr = msg->msg;
    uint8              sec_hdr_type;

    if(msg      != NULL &&
       auth_req != NULL)
    {
        // Security Header Type
        sec_hdr_type = (msg->msg[0] & 0xF0) >> 4;
        if(LIBLTE_MME_SECURITY_HDR_TYPE_PLAIN_NAS == sec_hdr_type)
        {
            msg_ptr++;
        }else{
            msg_ptr += 7;
        }

        // Skip Message Type
        msg_ptr++;

        // NAS Key Set Identifier & Spare Half Octet
        liblte_mme_unpack_nas_key_set_id_ie(&msg_ptr, 0, &auth_req->nas_ksi);
        msg_ptr++;

        // Authentication Parameter RAND
        liblte_mme_unpack_authentication_parameter_rand_ie(&msg_ptr, auth_req->rand);

        // Authentication Parameter AUTN
        liblte_mme_unpack_authentication_parameter_autn_ie(&msg_ptr, auth_req->autn);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    Message Name: Authentication Response

    Description: Sent by the UE to the network to deliver a calculated
                 authentication response to the network.

    Document Reference: 24.301 v10.2.0 Section 8.2.8
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_mme_pack_authentication_response_msg(LIBLTE_MME_AUTHENTICATION_RESPONSE_MSG_STRUCT *auth_resp,
                                                              LIBLTE_BYTE_MSG_STRUCT                        *msg)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr = msg->msg;

    if(auth_resp != NULL &&
       msg       != NULL)
    {
        // Protocol Discriminator and Security Header Type
        *msg_ptr = (LIBLTE_MME_SECURITY_HDR_TYPE_PLAIN_NAS << 4) | (LIBLTE_MME_PD_EPS_MOBILITY_MANAGEMENT);
        msg_ptr++;

        // Message Type
        *msg_ptr = LIBLTE_MME_MSG_TYPE_AUTHENTICATION_RESPONSE;
        msg_ptr++;

        // Authentication Response Parameter (RES)
        liblte_mme_pack_authentication_response_parameter_ie(auth_resp->res, auth_resp->res_len, &msg_ptr);

        // Fill in the number of bytes used
        msg->N_bytes = msg_ptr - msg->msg;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_mme_unpack_authentication_response_msg(LIBLTE_BYTE_MSG_STRUCT                        *msg,
                                                                LIBLTE_MME_AUTHENTICATION_RESPONSE_MSG_STRUCT *auth_resp)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr = msg->msg;
    uint8              sec_hdr_type;

    if(msg       != NULL &&
       auth_resp != NULL)
    {
        // Security Header Type
        sec_hdr_type = (msg->msg[0] & 0xF0) >> 4;
        if(LIBLTE_MME_SECURITY_HDR_TYPE_PLAIN_NAS == sec_hdr_type)
        {
            msg_ptr++;
        }else{
            msg_ptr += 7;
        }

        // Skip Message Type
        msg_ptr++;

        // Authentication Response Parameter (RES)
        liblte_mme_unpack_authentication_response_parameter_ie(&msg_ptr, auth_resp->res);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    Message Name: CS Service Notification

    Description: Sent by the network when a paging request with CS
                 call indicator was received via SGs for a UE, and a
                 NAS signalling connection is already established for
                 the UE.

    Document Reference: 24.301 v10.2.0 Section 8.2.9
*********************************************************************/
// FIXME

/*********************************************************************
    Message Name: Detach Accept

    Description: Sent by the network to indicate that the detach
                 procedure has been completed.

    Document Reference: 24.301 v10.2.0 Section 8.2.10
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_mme_pack_detach_accept_msg(LIBLTE_MME_DETACH_ACCEPT_MSG_STRUCT *detach_accept,
                                                    uint8                                sec_hdr_type,
                                                    uint32                               count,
                                                    LIBLTE_BYTE_MSG_STRUCT              *msg)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr = msg->msg;

    if(detach_accept != NULL &&
       msg           != NULL)
    {
        if(LIBLTE_MME_SECURITY_HDR_TYPE_PLAIN_NAS != sec_hdr_type)
        {
            // Protocol Discriminator and Security Header Type
            *msg_ptr = (sec_hdr_type << 4) | (LIBLTE_MME_PD_EPS_MOBILITY_MANAGEMENT);
            msg_ptr++;

            // MAC will be filled in later
            msg_ptr += 4;

            // Sequence Number
            *msg_ptr = count & 0xFF;
            msg_ptr++;
        }

        // Protocol Discriminator and Security Header Type
        *msg_ptr = (LIBLTE_MME_SECURITY_HDR_TYPE_PLAIN_NAS << 4) | (LIBLTE_MME_PD_EPS_MOBILITY_MANAGEMENT);
        msg_ptr++;

        // Message Type
        *msg_ptr = LIBLTE_MME_MSG_TYPE_DETACH_ACCEPT;
        msg_ptr++;

        // Fill in the number of bytes used
        msg->N_bytes = msg_ptr - msg->msg;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_mme_unpack_detach_accept_msg(LIBLTE_BYTE_MSG_STRUCT              *msg,
                                                      LIBLTE_MME_DETACH_ACCEPT_MSG_STRUCT *detach_accept)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr = msg->msg;
    uint8              sec_hdr_type;

    if(msg           != NULL &&
       detach_accept != NULL)
    {
        // Security Header Type
        sec_hdr_type = (msg->msg[0] & 0xF0) >> 4;
        if(LIBLTE_MME_SECURITY_HDR_TYPE_PLAIN_NAS == sec_hdr_type)
        {
            msg_ptr++;
        }else{
            msg_ptr += 7;
        }

        // Skip Message Type
        msg_ptr++;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    Message Name: Detach Request

    Description: Sent by the UE to request the release of an EMM
                 context.

    Document Reference: 24.301 v10.2.0 Section 8.2.11
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_mme_pack_detach_request_msg(LIBLTE_MME_DETACH_REQUEST_MSG_STRUCT *detach_req,
                                                     uint8                                 sec_hdr_type,
                                                     uint32                                count,
                                                     LIBLTE_BYTE_MSG_STRUCT               *msg)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr = msg->msg;

    if(detach_req != NULL &&
       msg        != NULL)
    {
        if(LIBLTE_MME_SECURITY_HDR_TYPE_PLAIN_NAS != sec_hdr_type)
        {
            // Protocol Discriminator and Security Header Type
            *msg_ptr = (sec_hdr_type << 4) | (LIBLTE_MME_PD_EPS_MOBILITY_MANAGEMENT);
            msg_ptr++;

            // MAC will be filled in later
            msg_ptr += 4;

            // Sequence Number
            *msg_ptr = count & 0xFF;
            msg_ptr++;
        }

        // Protocol Discriminator and Security Header Type
        *msg_ptr = (LIBLTE_MME_SECURITY_HDR_TYPE_PLAIN_NAS << 4) | (LIBLTE_MME_PD_EPS_MOBILITY_MANAGEMENT);
        msg_ptr++;

        // Message Type
        *msg_ptr = LIBLTE_MME_MSG_TYPE_DETACH_REQUEST;
        msg_ptr++;

        // Detach Type & NAS Key Set Identifier
        *msg_ptr = 0;
        liblte_mme_pack_detach_type_ie(&detach_req->detach_type, 0, &msg_ptr);
        liblte_mme_pack_nas_key_set_id_ie(&detach_req->nas_ksi, 4, &msg_ptr);
        msg_ptr++;

        // EPS Mobile ID
        liblte_mme_pack_eps_mobile_id_ie(&detach_req->eps_mobile_id, &msg_ptr);

        // Fill in the number of bytes used
        msg->N_bytes = msg_ptr - msg->msg;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_mme_unpack_detach_request_msg(LIBLTE_BYTE_MSG_STRUCT               *msg,
                                                       LIBLTE_MME_DETACH_REQUEST_MSG_STRUCT *detach_req)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr = msg->msg;
    uint8              sec_hdr_type;

    if(msg        != NULL &&
       detach_req != NULL)
    {
        // Security Header Type
        sec_hdr_type = (msg->msg[0] & 0xF0) >> 4;
        if(LIBLTE_MME_SECURITY_HDR_TYPE_PLAIN_NAS == sec_hdr_type)
        {
            msg_ptr++;
        }else{
            msg_ptr += 7;
        }

        // Skip Message Type
        msg_ptr++;

        // Detach Type & NAS Key Set Identifier
        liblte_mme_unpack_detach_type_ie(&msg_ptr, 0, &detach_req->detach_type);
        liblte_mme_unpack_nas_key_set_id_ie(&msg_ptr, 4, &detach_req->nas_ksi);
        msg_ptr++;

        // EPS Mobile ID
        liblte_mme_unpack_eps_mobile_id_ie(&msg_ptr, &detach_req->eps_mobile_id);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    Message Name: Downlink NAS Transport

    Description: Sent by the network to the UE in order to carry an
                 SMS message in encapsulated format.

    Document Reference: 24.301 v10.2.0 Section 8.2.12
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_mme_pack_downlink_nas_transport_msg(LIBLTE_MME_DOWNLINK_NAS_TRANSPORT_MSG_STRUCT *dl_nas_transport,
                                                             uint8                                         sec_hdr_type,
                                                             uint32                                        count,
                                                             LIBLTE_BYTE_MSG_STRUCT                       *msg)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr = msg->msg;

    if(dl_nas_transport != NULL &&
       msg              != NULL)
    {
        if(LIBLTE_MME_SECURITY_HDR_TYPE_PLAIN_NAS != sec_hdr_type)
        {
            // Protocol Discriminator and Security Header Type
            *msg_ptr = (sec_hdr_type << 4) | (LIBLTE_MME_PD_EPS_MOBILITY_MANAGEMENT);
            msg_ptr++;

            // MAC will be filled in later
            msg_ptr += 4;

            // Sequence Number
            *msg_ptr = count & 0xFF;
            msg_ptr++;
        }

        // Protocol Discriminator and Security Header Type
        *msg_ptr = (LIBLTE_MME_SECURITY_HDR_TYPE_PLAIN_NAS << 4) | (LIBLTE_MME_PD_EPS_MOBILITY_MANAGEMENT);
        msg_ptr++;

        // Message Type
        *msg_ptr = LIBLTE_MME_MSG_TYPE_DOWNLINK_NAS_TRANSPORT;
        msg_ptr++;

        // NAS Message Container
        liblte_mme_pack_nas_message_container_ie(&dl_nas_transport->nas_msg, &msg_ptr);

        // Fill in the number of bytes used
        msg->N_bytes = msg_ptr - msg->msg;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_mme_unpack_downlink_nas_transport_msg(LIBLTE_BYTE_MSG_STRUCT                       *msg,
                                                               LIBLTE_MME_DOWNLINK_NAS_TRANSPORT_MSG_STRUCT *dl_nas_transport)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr = msg->msg;
    uint8              sec_hdr_type;

    if(msg              != NULL &&
       dl_nas_transport != NULL)
    {
        // Security Header Type
        sec_hdr_type = (msg->msg[0] & 0xF0) >> 4;
        if(LIBLTE_MME_SECURITY_HDR_TYPE_PLAIN_NAS == sec_hdr_type)
        {
            msg_ptr++;
        }else{
            msg_ptr += 7;
        }

        // Skip Message Type
        msg_ptr++;

        // NAS Message Container
        liblte_mme_unpack_nas_message_container_ie(&msg_ptr, &dl_nas_transport->nas_msg);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    Message Name: EMM Information

    Description: Sent by the network at any time during EMM context is
                 established to send certain information to the UE.

    Document Reference: 24.301 v10.2.0 Section 8.2.13
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_mme_pack_emm_information_msg(LIBLTE_MME_EMM_INFORMATION_MSG_STRUCT *emm_info,
                                                      uint8                                  sec_hdr_type,
                                                      uint32                                 count,
                                                      LIBLTE_BYTE_MSG_STRUCT                *msg)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr = msg->msg;

    if(emm_info != NULL &&
       msg      != NULL)
    {
        if(LIBLTE_MME_SECURITY_HDR_TYPE_PLAIN_NAS != sec_hdr_type)
        {
            // Protocol Discriminator and Security Header Type
            *msg_ptr = (sec_hdr_type << 4) | (LIBLTE_MME_PD_EPS_MOBILITY_MANAGEMENT);
            msg_ptr++;

            // MAC will be filled in later
            msg_ptr += 4;

            // Sequence Number
            *msg_ptr = count & 0xFF;
            msg_ptr++;
        }

        // Protocol Discriminator and Security Header Type
        *msg_ptr = (LIBLTE_MME_SECURITY_HDR_TYPE_PLAIN_NAS << 4) | (LIBLTE_MME_PD_EPS_MOBILITY_MANAGEMENT);
        msg_ptr++;

        // Message Type
        *msg_ptr = LIBLTE_MME_MSG_TYPE_EMM_INFORMATION;
        msg_ptr++;

        // Full Name For Network
        if(emm_info->full_net_name_present)
        {
            *msg_ptr = LIBLTE_MME_FULL_NAME_FOR_NETWORK_IEI;
            msg_ptr++;
            liblte_mme_pack_network_name_ie(&emm_info->full_net_name, &msg_ptr);
        }

        // Short Name For Network
        if(emm_info->short_net_name_present)
        {
            *msg_ptr = LIBLTE_MME_SHORT_NAME_FOR_NETWORK_IEI;
            msg_ptr++;
            liblte_mme_pack_network_name_ie(&emm_info->short_net_name, &msg_ptr);
        }

        // Local Time Zone
        if(emm_info->local_time_zone_present)
        {
            *msg_ptr = LIBLTE_MME_LOCAL_TIME_ZONE_IEI;
            msg_ptr++;
            liblte_mme_pack_time_zone_ie(emm_info->local_time_zone, &msg_ptr);
        }

        // Universal Time And Local Time Zone
        if(emm_info->utc_and_local_time_zone_present)
        {
            *msg_ptr = LIBLTE_MME_UNIVERSAL_TIME_AND_LOCAL_TIME_ZONE_IEI;
            msg_ptr++;
            liblte_mme_pack_time_zone_and_time_ie(&emm_info->utc_and_local_time_zone, &msg_ptr);
        }

        // Network Daylight Saving Time
        if(emm_info->net_dst_present)
        {
            *msg_ptr = LIBLTE_MME_NETWORK_DAYLIGHT_SAVING_TIME_IEI;
            msg_ptr++;
            liblte_mme_pack_daylight_saving_time_ie(emm_info->net_dst, &msg_ptr);
        }

        // Fill in the number of bytes used
        msg->N_bytes = msg_ptr - msg->msg;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_mme_unpack_emm_information_msg(LIBLTE_BYTE_MSG_STRUCT                *msg,
                                                        LIBLTE_MME_EMM_INFORMATION_MSG_STRUCT *emm_info)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr = msg->msg;
    uint8              sec_hdr_type;

    if(msg      != NULL &&
       emm_info != NULL)
    {
        // Security Header Type
        sec_hdr_type = (msg->msg[0] & 0xF0) >> 4;
        if(LIBLTE_MME_SECURITY_HDR_TYPE_PLAIN_NAS == sec_hdr_type)
        {
            msg_ptr++;
        }else{
            msg_ptr += 7;
        }

        // Skip Message Type
        msg_ptr++;

        // Full Name For Network
        if(LIBLTE_MME_FULL_NAME_FOR_NETWORK_IEI == *msg_ptr)
        {
            msg_ptr++;
            liblte_mme_unpack_network_name_ie(&msg_ptr, &emm_info->full_net_name);
            emm_info->full_net_name_present = true;
        }else{
            emm_info->full_net_name_present = false;
        }

        // Short Name For Network
        if(LIBLTE_MME_SHORT_NAME_FOR_NETWORK_IEI == *msg_ptr)
        {
            msg_ptr++;
            liblte_mme_unpack_network_name_ie(&msg_ptr, &emm_info->short_net_name);
            emm_info->short_net_name_present = true;
        }else{
            emm_info->short_net_name_present = false;
        }

        // Local Time Zone
        if(LIBLTE_MME_LOCAL_TIME_ZONE_IEI == *msg_ptr)
        {
            msg_ptr++;
            liblte_mme_unpack_time_zone_ie(&msg_ptr, &emm_info->local_time_zone);
            emm_info->local_time_zone_present = true;
        }else{
            emm_info->local_time_zone_present = false;
        }

        // Universal Time And Local Time Zone
        if(LIBLTE_MME_UNIVERSAL_TIME_AND_LOCAL_TIME_ZONE_IEI == *msg_ptr)
        {
            msg_ptr++;
            liblte_mme_unpack_time_zone_and_time_ie(&msg_ptr, &emm_info->utc_and_local_time_zone);
            emm_info->utc_and_local_time_zone_present = true;
        }else{
            emm_info->utc_and_local_time_zone_present = false;
        }

        // Network Daylight Saving Time
        if(LIBLTE_MME_NETWORK_DAYLIGHT_SAVING_TIME_IEI == *msg_ptr)
        {
            msg_ptr++;
            liblte_mme_unpack_daylight_saving_time_ie(&msg_ptr, &emm_info->net_dst);
            emm_info->net_dst_present = true;
        }else{
            emm_info->net_dst_present = false;
        }

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    Message Name: EMM Status

    Description: Sent by the UE or by the network at any time to
                 report certain error conditions.

    Document Reference: 24.301 v10.2.0 Section 8.2.14
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_mme_pack_emm_status_msg(LIBLTE_MME_EMM_STATUS_MSG_STRUCT *emm_status,
                                                 uint8                             sec_hdr_type,
                                                 uint32                            count,
                                                 LIBLTE_BYTE_MSG_STRUCT           *msg)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr = msg->msg;

    if(emm_status != NULL &&
       msg        != NULL)
    {
        if(LIBLTE_MME_SECURITY_HDR_TYPE_PLAIN_NAS != sec_hdr_type)
        {
            // Protocol Discriminator and Security Header Type
            *msg_ptr = (sec_hdr_type << 4) | (LIBLTE_MME_PD_EPS_MOBILITY_MANAGEMENT);
            msg_ptr++;

            // MAC will be filled in later
            msg_ptr += 4;

            // Sequence Number
            *msg_ptr = count & 0xFF;
            msg_ptr++;
        }

        // Protocol Discriminator and Security Header Type
        *msg_ptr = (LIBLTE_MME_SECURITY_HDR_TYPE_PLAIN_NAS << 4) | (LIBLTE_MME_PD_EPS_MOBILITY_MANAGEMENT);
        msg_ptr++;

        // Message Type
        *msg_ptr = LIBLTE_MME_MSG_TYPE_EMM_STATUS;
        msg_ptr++;

        // EMM Cause
        liblte_mme_pack_emm_cause_ie(emm_status->emm_cause, &msg_ptr);

        // Fill in the number of bytes used
        msg->N_bytes = msg_ptr - msg->msg;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_mme_unpack_emm_status_msg(LIBLTE_BYTE_MSG_STRUCT           *msg,
                                                   LIBLTE_MME_EMM_STATUS_MSG_STRUCT *emm_status)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr = msg->msg;
    uint8              sec_hdr_type;

    if(msg        != NULL &&
       emm_status != NULL)
    {
        // Security Header Type
        sec_hdr_type = (msg->msg[0] & 0xF0) >> 4;
        if(LIBLTE_MME_SECURITY_HDR_TYPE_PLAIN_NAS == sec_hdr_type)
        {
            msg_ptr++;
        }else{
            msg_ptr += 7;
        }

        // Skip Message Type
        msg_ptr++;

        // EMM Cause
        liblte_mme_unpack_emm_cause_ie(&msg_ptr, &emm_status->emm_cause);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    Message Name: Extended Service Request

    Description: Sent by the UE to the network to initiate a CS
                 fallback or 1xCS fallback call or respond to a mobile
                 terminated CS fallback or 1xCS fallback request from
                 the network or to request the establishment of a NAS
                 signalling connection and of the radio and S1 bearers
                 for packet services, if the UE needs to provide
                 additional information that cannot be provided via a
                 SERVICE REQUEST message.

    Document Reference: 24.301 v10.2.0 Section 8.2.15
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_mme_pack_extended_service_request_msg(LIBLTE_MME_EXTENDED_SERVICE_REQUEST_MSG_STRUCT *ext_service_req,
                                                               uint8                                           sec_hdr_type,
                                                               uint32                                          count,
                                                               LIBLTE_BYTE_MSG_STRUCT                         *msg)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr = msg->msg;

    if(ext_service_req != NULL &&
       msg             != NULL)
    {
        if(LIBLTE_MME_SECURITY_HDR_TYPE_PLAIN_NAS != sec_hdr_type)
        {
            // Protocol Discriminator and Security Header Type
            *msg_ptr = (sec_hdr_type << 4) | (LIBLTE_MME_PD_EPS_MOBILITY_MANAGEMENT);
            msg_ptr++;

            // MAC will be filled in later
            msg_ptr += 4;

            // Sequence Number
            *msg_ptr = count & 0xFF;
            msg_ptr++;
        }

        // Protocol Discriminator and Security Header Type
        *msg_ptr = (LIBLTE_MME_SECURITY_HDR_TYPE_PLAIN_NAS << 4) | (LIBLTE_MME_PD_EPS_MOBILITY_MANAGEMENT);
        msg_ptr++;

        // Message Type
        *msg_ptr = LIBLTE_MME_MSG_TYPE_EXTENDED_SERVICE_REQUEST;
        msg_ptr++;

        // Service Type & NAS Key Set Identifier
        *msg_ptr = 0;
        liblte_mme_pack_service_type_ie(ext_service_req->service_type, 0, &msg_ptr);
        liblte_mme_pack_nas_key_set_id_ie(&ext_service_req->nas_ksi, 4, &msg_ptr);
        msg_ptr++;

        // M-TMSI
        liblte_mme_pack_mobile_id_ie(&ext_service_req->m_tmsi, &msg_ptr);

        // CSFB Response
        if(ext_service_req->csfb_resp_present)
        {
            *msg_ptr = LIBLTE_MME_CSFB_RESPONSE_IEI << 4;
            liblte_mme_pack_csfb_response_ie(ext_service_req->csfb_resp, 0, &msg_ptr);
            msg_ptr++;
        }

        // EPS Bearer Context Status
        if(ext_service_req->eps_bearer_context_status_present)
        {
            *msg_ptr = LIBLTE_MME_EPS_BEARER_CONTEXT_STATUS_IEI;
            msg_ptr++;
            liblte_mme_pack_eps_bearer_context_status_ie(&ext_service_req->eps_bearer_context_status, &msg_ptr);
        }

        // Device Properties
        if(ext_service_req->device_props_present)
        {
            *msg_ptr = LIBLTE_MME_EXTENDED_SERVICE_REQUEST_DEVICE_PROPERTIES_IEI << 4;
            liblte_mme_pack_device_properties_ie(ext_service_req->device_props, 0, &msg_ptr);
            msg_ptr++;
        }

        // Fill in the number of bytes used
        msg->N_bytes = msg_ptr - msg->msg;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_mme_unpack_extended_service_request_msg(LIBLTE_BYTE_MSG_STRUCT                         *msg,
                                                                 LIBLTE_MME_EXTENDED_SERVICE_REQUEST_MSG_STRUCT *ext_service_req)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr = msg->msg;
    uint8              sec_hdr_type;

    if(msg             != NULL &&
       ext_service_req != NULL)
    {
        // Security Header Type
        sec_hdr_type = (msg->msg[0] & 0xF0) >> 4;
        if(LIBLTE_MME_SECURITY_HDR_TYPE_PLAIN_NAS == sec_hdr_type)
        {
            msg_ptr++;
        }else{
            msg_ptr += 7;
        }

        // Skip Message Type
        msg_ptr++;

        // Service Type & NAS Key Set Identifier
        liblte_mme_unpack_service_type_ie(&msg_ptr, 0, &ext_service_req->service_type);
        liblte_mme_unpack_nas_key_set_id_ie(&msg_ptr, 4, &ext_service_req->nas_ksi);
        msg_ptr++;

        // M-TMSI
        liblte_mme_unpack_mobile_id_ie(&msg_ptr, &ext_service_req->m_tmsi);

        // CSFB Response
        if((LIBLTE_MME_CSFB_RESPONSE_IEI << 4) == (*msg_ptr & 0xF0))
        {
            liblte_mme_unpack_csfb_response_ie(&msg_ptr, 0, &ext_service_req->csfb_resp);
            msg_ptr++;
            ext_service_req->csfb_resp_present = true;
        }else{
            ext_service_req->csfb_resp_present = false;
        }

        // EPS Bearer Context Status
        if(LIBLTE_MME_EPS_BEARER_CONTEXT_STATUS_IEI == *msg_ptr)
        {
            msg_ptr++;
            liblte_mme_unpack_eps_bearer_context_status_ie(&msg_ptr, &ext_service_req->eps_bearer_context_status);
            ext_service_req->eps_bearer_context_status_present = true;
        }else{
            ext_service_req->eps_bearer_context_status_present = false;
        }

        // Device Properties
        if((LIBLTE_MME_EXTENDED_SERVICE_REQUEST_DEVICE_PROPERTIES_IEI << 4) == (*msg_ptr & 0xF0))
        {
            liblte_mme_unpack_device_properties_ie(&msg_ptr, 0, &ext_service_req->device_props);
            msg_ptr++;
            ext_service_req->device_props_present = true;
        }else{
            ext_service_req->device_props_present = false;
        }

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    Message Name: GUTI Reallocation Command

    Description: Sent by the network to the UE to reallocate a GUTI
                 and optionally provide a new TAI list.

    Document Reference: 24.301 v10.2.0 Section 8.2.16
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_mme_pack_guti_reallocation_command_msg(LIBLTE_MME_GUTI_REALLOCATION_COMMAND_MSG_STRUCT *guti_realloc_cmd,
                                                                uint8                                            sec_hdr_type,
                                                                uint32                                           count,
                                                                LIBLTE_BYTE_MSG_STRUCT                          *msg)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr = msg->msg;

    if(guti_realloc_cmd != NULL &&
       msg              != NULL)
    {
        if(LIBLTE_MME_SECURITY_HDR_TYPE_PLAIN_NAS != sec_hdr_type)
        {
            // Protocol Discriminator and Security Header Type
            *msg_ptr = (sec_hdr_type << 4) | (LIBLTE_MME_PD_EPS_MOBILITY_MANAGEMENT);
            msg_ptr++;

            // MAC will be filled in later
            msg_ptr += 4;

            // Sequence Number
            *msg_ptr = count & 0xFF;
            msg_ptr++;
        }

        // Protocol Discriminator and Security Header Type
        *msg_ptr = (LIBLTE_MME_SECURITY_HDR_TYPE_PLAIN_NAS << 4) | (LIBLTE_MME_PD_EPS_MOBILITY_MANAGEMENT);
        msg_ptr++;

        // Message Type
        *msg_ptr = LIBLTE_MME_MSG_TYPE_GUTI_REALLOCATION_COMMAND;
        msg_ptr++;

        // GUTI
        liblte_mme_pack_eps_mobile_id_ie(&guti_realloc_cmd->guti, &msg_ptr);

        // TAI List
        if(guti_realloc_cmd->tai_list_present)
        {
            *msg_ptr = LIBLTE_MME_TAI_LIST_IEI;
            msg_ptr++;
            liblte_mme_pack_tracking_area_identity_list_ie(&guti_realloc_cmd->tai_list, &msg_ptr);
        }

        // Fill in the number of bytes used
        msg->N_bytes = msg_ptr - msg->msg;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_mme_unpack_guti_reallocation_command_msg(LIBLTE_BYTE_MSG_STRUCT                          *msg,
                                                                  LIBLTE_MME_GUTI_REALLOCATION_COMMAND_MSG_STRUCT *guti_realloc_cmd)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr = msg->msg;
    uint8              sec_hdr_type;

    if(msg              != NULL &&
       guti_realloc_cmd != NULL)
    {
        // Security Header Type
        sec_hdr_type = (msg->msg[0] & 0xF0) >> 4;
        if(LIBLTE_MME_SECURITY_HDR_TYPE_PLAIN_NAS == sec_hdr_type)
        {
            msg_ptr++;
        }else{
            msg_ptr += 7;
        }

        // Skip Message Type
        msg_ptr++;

        // GUTI
        liblte_mme_unpack_eps_mobile_id_ie(&msg_ptr, &guti_realloc_cmd->guti);

        // TAI List
        if(LIBLTE_MME_TAI_LIST_IEI == *msg_ptr)
        {
            msg_ptr++;
            liblte_mme_unpack_tracking_area_identity_list_ie(&msg_ptr, &guti_realloc_cmd->tai_list);
            guti_realloc_cmd->tai_list_present = true;
        }else{
            guti_realloc_cmd->tai_list_present = false;
        }

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    Message Name: GUTI Reallocation Complete

    Description: Sent by the UE to the network to indicate that
                 reallocation of a GUTI has taken place.

    Document Reference: 24.301 v10.2.0 Section 8.2.17
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_mme_pack_guti_reallocation_complete_msg(LIBLTE_MME_GUTI_REALLOCATION_COMPLETE_MSG_STRUCT *guti_realloc_complete,
                                                                 uint8                                             sec_hdr_type,
                                                                 uint32                                            count,
                                                                 LIBLTE_BYTE_MSG_STRUCT                           *msg)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr = msg->msg;

    if(guti_realloc_complete != NULL &&
       msg                   != NULL)
    {
        if(LIBLTE_MME_SECURITY_HDR_TYPE_PLAIN_NAS != sec_hdr_type)
        {
            // Protocol Discriminator and Security Header Type
            *msg_ptr = (sec_hdr_type << 4) | (LIBLTE_MME_PD_EPS_MOBILITY_MANAGEMENT);
            msg_ptr++;

            // MAC will be filled in later
            msg_ptr += 4;

            // Sequence Number
            *msg_ptr = count & 0xFF;
            msg_ptr++;
        }

        // Protocol Discriminator and Security Header Type
        *msg_ptr = (LIBLTE_MME_SECURITY_HDR_TYPE_PLAIN_NAS << 4) | (LIBLTE_MME_PD_EPS_MOBILITY_MANAGEMENT);
        msg_ptr++;

        // Message Type
        *msg_ptr = LIBLTE_MME_MSG_TYPE_GUTI_REALLOCATION_COMPLETE;
        msg_ptr++;

        // Fill in the number of bytes used
        msg->N_bytes = msg_ptr - msg->msg;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_mme_unpack_guti_reallocation_complete_msg(LIBLTE_BYTE_MSG_STRUCT                           *msg,
                                                                   LIBLTE_MME_GUTI_REALLOCATION_COMPLETE_MSG_STRUCT *guti_realloc_complete)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr = msg->msg;
    uint8              sec_hdr_type;

    if(msg                   != NULL &&
       guti_realloc_complete != NULL)
    {
        // Security Header Type
        sec_hdr_type = (msg->msg[0] & 0xF0) >> 4;
        if(LIBLTE_MME_SECURITY_HDR_TYPE_PLAIN_NAS == sec_hdr_type)
        {
            msg_ptr++;
        }else{
            msg_ptr += 7;
        }

        // Skip Message Type
        msg_ptr++;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    Message Name: Identity Request

    Description: Sent by the network to the UE to request the UE to
                 provide the specified identity.

    Document Reference: 24.301 v10.2.0 Section 8.2.18
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_mme_pack_identity_request_msg(LIBLTE_MME_ID_REQUEST_MSG_STRUCT *id_req,
                                                       LIBLTE_BYTE_MSG_STRUCT           *msg)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr = msg->msg;

    if(id_req != NULL &&
       msg    != NULL)
    {
        // Protocol Discriminator and Security Header Type
        *msg_ptr = (LIBLTE_MME_SECURITY_HDR_TYPE_PLAIN_NAS << 4) | (LIBLTE_MME_PD_EPS_MOBILITY_MANAGEMENT);
        msg_ptr++;

        // Message Type
        *msg_ptr = LIBLTE_MME_MSG_TYPE_IDENTITY_REQUEST;
        msg_ptr++;

        // ID Type & Spare Half Octet
        *msg_ptr = 0;
        liblte_mme_pack_identity_type_2_ie(id_req->id_type, 0, &msg_ptr);
        msg_ptr++;

        // Fill in the number of bytes used
        msg->N_bytes = msg_ptr - msg->msg;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_mme_unpack_identity_request_msg(LIBLTE_BYTE_MSG_STRUCT           *msg,
                                                         LIBLTE_MME_ID_REQUEST_MSG_STRUCT *id_req)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr = msg->msg;
    uint8              sec_hdr_type;

    if(msg    != NULL &&
       id_req != NULL)
    {
        // Security Header Type
        sec_hdr_type = (msg->msg[0] & 0xF0) >> 4;
        if(LIBLTE_MME_SECURITY_HDR_TYPE_PLAIN_NAS == sec_hdr_type)
        {
            msg_ptr++;
        }else{
            msg_ptr += 7;
        }

        // Skip Message Type
        msg_ptr++;

        // ID Type & Spare Half Offset
        liblte_mme_unpack_identity_type_2_ie(&msg_ptr, 0, &id_req->id_type);
        msg_ptr++;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
/*********************************************************************
    Message Name: Identity Response

    Description: Sent by the UE to the network in response to an
                 IDENTITY REQUEST message and provides the requested
                 identity.

    Document Reference: 24.301 v10.2.0 Section 8.2.19
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_mme_pack_identity_response_msg(LIBLTE_MME_ID_RESPONSE_MSG_STRUCT *id_resp,
                                                        LIBLTE_BYTE_MSG_STRUCT            *msg)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr = msg->msg;

    if(id_resp != NULL &&
       msg     != NULL)
    {
        // Protocol Discriminator and Security Header Type
        *msg_ptr = (LIBLTE_MME_SECURITY_HDR_TYPE_PLAIN_NAS << 4) | (LIBLTE_MME_PD_EPS_MOBILITY_MANAGEMENT);
        msg_ptr++;

        // Message Type
        *msg_ptr = LIBLTE_MME_MSG_TYPE_IDENTITY_RESPONSE;
        msg_ptr++;

        // Mobile Identity
        liblte_mme_pack_mobile_id_ie(&id_resp->mobile_id, &msg_ptr);

        // Fill in the number of bytes used
        msg->N_bytes = msg_ptr - msg->msg;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

LIBLTE_ERROR_ENUM liblte_mme_unpack_identity_response_msg(LIBLTE_BYTE_MSG_STRUCT            *msg,
                                                          LIBLTE_MME_ID_RESPONSE_MSG_STRUCT *id_resp)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr = msg->msg;
    uint8              sec_hdr_type;

    if(msg     != NULL &&
       id_resp != NULL)
    {
        // Security Header Type
        sec_hdr_type = (msg->msg[0] & 0xF0) >> 4;
        if(LIBLTE_MME_SECURITY_HDR_TYPE_PLAIN_NAS == sec_hdr_type)
        {
            msg_ptr++;
        }else{
            msg_ptr += 7;
        }

        // Skip Message Type
        msg_ptr++;

        // Mobile Identity
        liblte_mme_unpack_mobile_id_ie(&msg_ptr, &id_resp->mobile_id);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    Message Name: Security Mode Command

    Description: Sent by the network to the UE to establish NAS
                 signalling security.

    Document Reference: 24.301 v10.2.0 Section 8.2.20
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_mme_pack_security_mode_command_msg(LIBLTE_MME_SECURITY_MODE_COMMAND_MSG_STRUCT *sec_mode_cmd,
                                                            uint8                                        sec_hdr_type,
                                                            uint32                                       count,
                                                            LIBLTE_BYTE_MSG_STRUCT                      *msg)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr = msg->msg;

    if(sec_mode_cmd != NULL &&
       msg          != NULL)
    {
        if(LIBLTE_MME_SECURITY_HDR_TYPE_PLAIN_NAS != sec_hdr_type)
        {
            // Protocol Discriminator and Security Header Type
            *msg_ptr = (sec_hdr_type << 4) | (LIBLTE_MME_PD_EPS_MOBILITY_MANAGEMENT);
            msg_ptr++;

            // MAC will be filled in later
            msg_ptr += 4;

            // Sequence Number
            *msg_ptr = count & 0xFF;
            msg_ptr++;
        }

        // Protocol Discriminator and Security Header Type
        *msg_ptr = (LIBLTE_MME_SECURITY_HDR_TYPE_PLAIN_NAS << 4) | (LIBLTE_MME_PD_EPS_MOBILITY_MANAGEMENT);
        msg_ptr++;

        // Message Type
        *msg_ptr = LIBLTE_MME_MSG_TYPE_SECURITY_MODE_COMMAND;
        msg_ptr++;

        // Selected NAS Security Algorithms
        liblte_mme_pack_nas_security_algorithms_ie(&sec_mode_cmd->selected_nas_sec_algs, &msg_ptr);

        // NAS Key Set Identifier & Spare Half Octet
        *msg_ptr = 0;
        liblte_mme_pack_nas_key_set_id_ie(&sec_mode_cmd->nas_ksi, 0, &msg_ptr);
        msg_ptr++;

        // Replayed UE Security Capabilities
        liblte_mme_pack_ue_security_capabilities_ie(&sec_mode_cmd->ue_security_cap, &msg_ptr);

        // IMEISV Request
        if(sec_mode_cmd->imeisv_req_present)
        {
            *msg_ptr = LIBLTE_MME_IMEISV_REQUEST_IEI << 4;
            liblte_mme_pack_imeisv_request_ie(sec_mode_cmd->imeisv_req, 0, &msg_ptr);
            msg_ptr++;
        }

        // Replayed NONCE_ue
        if(sec_mode_cmd->nonce_ue_present)
        {
            *msg_ptr = LIBLTE_MME_REPLAYED_NONCE_UE_IEI;
            msg_ptr++;
            liblte_mme_pack_nonce_ie(sec_mode_cmd->nonce_ue, &msg_ptr);
        }

        // NONCE_mme
        if(sec_mode_cmd->nonce_mme_present)
        {
            *msg_ptr = LIBLTE_MME_NONCE_MME_IEI;
            msg_ptr++;
            liblte_mme_pack_nonce_ie(sec_mode_cmd->nonce_mme, &msg_ptr);
        }

        // Fill in the number of bytes used
        msg->N_bytes = msg_ptr - msg->msg;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_mme_unpack_security_mode_command_msg(LIBLTE_BYTE_MSG_STRUCT                      *msg,
                                                              LIBLTE_MME_SECURITY_MODE_COMMAND_MSG_STRUCT *sec_mode_cmd)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr = msg->msg;
    uint8              sec_hdr_type;

    if(msg          != NULL &&
       sec_mode_cmd != NULL)
    {
        // Security Header Type
        sec_hdr_type = (msg->msg[0] & 0xF0) >> 4;
        if(LIBLTE_MME_SECURITY_HDR_TYPE_PLAIN_NAS == sec_hdr_type)
        {
            msg_ptr++;
        }else{
            msg_ptr += 7;
        }

        // Skip Message Type
        msg_ptr++;

        // Selected NAS Security Algorithms
        liblte_mme_unpack_nas_security_algorithms_ie(&msg_ptr, &sec_mode_cmd->selected_nas_sec_algs);

        // NAS Key Set Identifier & Spare Half Octet
        liblte_mme_unpack_nas_key_set_id_ie(&msg_ptr, 0, &sec_mode_cmd->nas_ksi);
        msg_ptr++;

        // Replayed UE Security Capabilities
        liblte_mme_unpack_ue_security_capabilities_ie(&msg_ptr, &sec_mode_cmd->ue_security_cap);

        // IMEISV Request
        if((LIBLTE_MME_IMEISV_REQUEST_IEI << 4) == (*msg_ptr & 0xF0))
        {
            liblte_mme_unpack_imeisv_request_ie(&msg_ptr, 0, &sec_mode_cmd->imeisv_req);
            msg_ptr++;
            sec_mode_cmd->imeisv_req_present = true;
        }else{
            sec_mode_cmd->imeisv_req_present = false;
        }

        // Replayed NONCE_ue
        if(LIBLTE_MME_REPLAYED_NONCE_UE_IEI == *msg_ptr)
        {
            msg_ptr++;
            liblte_mme_unpack_nonce_ie(&msg_ptr, &sec_mode_cmd->nonce_ue);
            sec_mode_cmd->nonce_ue_present = true;
        }else{
            sec_mode_cmd->nonce_ue_present = false;
        }

        // NONCE_mme
        if(LIBLTE_MME_NONCE_MME_IEI == *msg_ptr)
        {
            msg_ptr++;
            liblte_mme_unpack_nonce_ie(&msg_ptr, &sec_mode_cmd->nonce_mme);
            sec_mode_cmd->nonce_mme_present = true;
        }else{
            sec_mode_cmd->nonce_mme_present = false;
        }

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    Message Name: Security Mode Complete

    Description: Sent by the UE to the network in response to a
                 SECURITY MODE COMMAND message.

    Document Reference: 24.301 v10.2.0 Section 8.2.21
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_mme_pack_security_mode_complete_msg(LIBLTE_MME_SECURITY_MODE_COMPLETE_MSG_STRUCT *sec_mode_comp,
                                                             uint8                                         sec_hdr_type,
                                                             uint32                                        count,
                                                             LIBLTE_BYTE_MSG_STRUCT                       *msg)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr = msg->msg;

    if(sec_mode_comp != NULL &&
       msg           != NULL)
    {
        if(LIBLTE_MME_SECURITY_HDR_TYPE_PLAIN_NAS != sec_hdr_type)
        {
            // Protocol Discriminator and Security Header Type
            *msg_ptr = (sec_hdr_type << 4) | (LIBLTE_MME_PD_EPS_MOBILITY_MANAGEMENT);
            msg_ptr++;

            // MAC will be filled in later
            msg_ptr += 4;

            // Sequence Number
            *msg_ptr = count & 0xFF;
            msg_ptr++;
        }

        // Protocol Discriminator and Security Header Type
        *msg_ptr = (LIBLTE_MME_SECURITY_HDR_TYPE_PLAIN_NAS << 4) | (LIBLTE_MME_PD_EPS_MOBILITY_MANAGEMENT);
        msg_ptr++;

        // Message Type
        *msg_ptr = LIBLTE_MME_MSG_TYPE_SECURITY_MODE_COMPLETE;
        msg_ptr++;

        // IMEISV
        if(sec_mode_comp->imeisv_present)
        {
            *msg_ptr = LIBLTE_MME_IMEISV_IEI;
            msg_ptr++;
            liblte_mme_pack_mobile_id_ie(&sec_mode_comp->imeisv, &msg_ptr);
        }

        // Fill in the number of bytes used
        msg->N_bytes = msg_ptr - msg->msg;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_mme_unpack_security_mode_complete_msg(LIBLTE_BYTE_MSG_STRUCT                       *msg,
                                                               LIBLTE_MME_SECURITY_MODE_COMPLETE_MSG_STRUCT *sec_mode_comp)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr = msg->msg;
    uint8              sec_hdr_type;

    if(msg           != NULL &&
       sec_mode_comp != NULL)
    {
        // Security Header Type
        sec_hdr_type = (msg->msg[0] & 0xF0) >> 4;
        if(LIBLTE_MME_SECURITY_HDR_TYPE_PLAIN_NAS == sec_hdr_type)
        {
            msg_ptr++;
        }else{
            msg_ptr += 7;
        }

        // Skip Message Type
        msg_ptr++;

        // IMEISV
        if(LIBLTE_MME_IMEISV_IEI == *msg_ptr)
        {
            msg_ptr++;
            liblte_mme_unpack_mobile_id_ie(&msg_ptr, &sec_mode_comp->imeisv);
            sec_mode_comp->imeisv_present = true;
        }else{
            sec_mode_comp->imeisv_present = false;
        }

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    Message Name: Security Mode Reject

    Description: Sent by the UE to the network to indicate that the
                 corresponding security mode command has been
                 rejected.

    Document Reference: 24.301 v10.2.0 Section 8.2.22
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_mme_pack_security_mode_reject_msg(LIBLTE_MME_SECURITY_MODE_REJECT_MSG_STRUCT *sec_mode_rej,
                                                           LIBLTE_BYTE_MSG_STRUCT                     *msg)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr = msg->msg;

    if(sec_mode_rej != NULL &&
       msg          != NULL)
    {
        // Protocol Discriminator and Security Header Type
        *msg_ptr = (LIBLTE_MME_SECURITY_HDR_TYPE_PLAIN_NAS << 4) | (LIBLTE_MME_PD_EPS_MOBILITY_MANAGEMENT);
        msg_ptr++;

        // Message Type
        *msg_ptr = LIBLTE_MME_MSG_TYPE_SECURITY_MODE_REJECT;
        msg_ptr++;

        // EMM Cause
        liblte_mme_pack_emm_cause_ie(sec_mode_rej->emm_cause, &msg_ptr);

        // Fill in the number of bytes used
        msg->N_bytes = msg_ptr - msg->msg;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_mme_unpack_security_mode_reject_msg(LIBLTE_BYTE_MSG_STRUCT                     *msg,
                                                             LIBLTE_MME_SECURITY_MODE_REJECT_MSG_STRUCT *sec_mode_rej)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr = msg->msg;
    uint8              sec_hdr_type;

    if(msg          != NULL &&
       sec_mode_rej != NULL)
    {
        // Security Header Type
        sec_hdr_type = (msg->msg[0] & 0xF0) >> 4;
        if(LIBLTE_MME_SECURITY_HDR_TYPE_PLAIN_NAS == sec_hdr_type)
        {
            msg_ptr++;
        }else{
            msg_ptr += 7;
        }

        // Skip Message Type
        msg_ptr++;

        // EMM Cause
        liblte_mme_unpack_emm_cause_ie(&msg_ptr, &sec_mode_rej->emm_cause);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    Message Name: Service Reject

    Description: Sent by the network to the UE in order to reject the
                 service request procedure.

    Document Reference: 24.301 v10.2.0 Section 8.2.24
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_mme_pack_service_reject_msg(LIBLTE_MME_SERVICE_REJECT_MSG_STRUCT *service_rej,
                                                     uint8                                 sec_hdr_type,
                                                     uint32                                count,
                                                     LIBLTE_BYTE_MSG_STRUCT               *msg)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr = msg->msg;

    if(service_rej != NULL &&
       msg         != NULL)
    {
        if(LIBLTE_MME_SECURITY_HDR_TYPE_PLAIN_NAS != sec_hdr_type)
        {
            // Protocol Discriminator and Security Header Type
            *msg_ptr = (sec_hdr_type << 4) | (LIBLTE_MME_PD_EPS_MOBILITY_MANAGEMENT);
            msg_ptr++;

            // MAC will be filled in later
            msg_ptr += 4;

            // Sequence Number
            *msg_ptr = count & 0xFF;
            msg_ptr++;
        }

        // Protocol Discriminator and Security Header Type
        *msg_ptr = (LIBLTE_MME_SECURITY_HDR_TYPE_PLAIN_NAS << 4) | (LIBLTE_MME_PD_EPS_MOBILITY_MANAGEMENT);
        msg_ptr++;

        // Message Type
        *msg_ptr = LIBLTE_MME_MSG_TYPE_SERVICE_REJECT;
        msg_ptr++;

        // EMM Cause
        liblte_mme_pack_emm_cause_ie(service_rej->emm_cause, &msg_ptr);

        // T3442 Value
        if(service_rej->t3442_present)
        {
            *msg_ptr = LIBLTE_MME_T3442_VALUE_IEI;
            msg_ptr++;
            liblte_mme_pack_gprs_timer_ie(&service_rej->t3442, &msg_ptr);
        }

        // T3446 Value
        if(service_rej->t3446_present)
        {
            *msg_ptr = LIBLTE_MME_T3446_VALUE_IEI;
            msg_ptr++;
            liblte_mme_pack_gprs_timer_2_ie(service_rej->t3446, &msg_ptr);
        }

        // Fill in the number of bytes used
        msg->N_bytes = msg_ptr - msg->msg;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_mme_unpack_service_reject_msg(LIBLTE_BYTE_MSG_STRUCT               *msg,
                                                       LIBLTE_MME_SERVICE_REJECT_MSG_STRUCT *service_rej)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr = msg->msg;
    uint8              sec_hdr_type;

    if(msg         != NULL &&
       service_rej != NULL)
    {
        // Security Header Type
        sec_hdr_type = (msg->msg[0] & 0xF0) >> 4;
        if(LIBLTE_MME_SECURITY_HDR_TYPE_PLAIN_NAS == sec_hdr_type)
        {
            msg_ptr++;
        }else{
            msg_ptr += 7;
        }

        // Skip Message Type
        msg_ptr++;

        // EMM Cause
        liblte_mme_unpack_emm_cause_ie(&msg_ptr, &service_rej->emm_cause);

        // T3442 Value
        if(LIBLTE_MME_T3442_VALUE_IEI == *msg_ptr)
        {
            msg_ptr++;
            liblte_mme_unpack_gprs_timer_ie(&msg_ptr, &service_rej->t3442);
            service_rej->t3442_present = true;
        }else{
            service_rej->t3442_present = false;
        }

        // T3446 Value
        if(LIBLTE_MME_T3446_VALUE_IEI == *msg_ptr)
        {
            msg_ptr++;
            liblte_mme_unpack_gprs_timer_2_ie(&msg_ptr, &service_rej->t3446);
            service_rej->t3446_present = true;
        }else{
            service_rej->t3446_present = false;
        }

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    Message Name: Service Request

    Description: Sent by the UE to the network to request the
                 establishment of a NAS signalling connection and of
                 the radio and S1 bearers.

    Document Reference: 24.301 v10.2.0 Section 8.2.25
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_mme_pack_service_request_msg(LIBLTE_MME_SERVICE_REQUEST_MSG_STRUCT *service_req,
                                                      LIBLTE_BYTE_MSG_STRUCT                *msg)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr = msg->msg;

    if(service_req != NULL &&
       msg         != NULL)
    {
        // Protocol Discriminator and Security Header Type
        *msg_ptr = (LIBLTE_MME_SECURITY_HDR_TYPE_SERVICE_REQUEST << 4) | (LIBLTE_MME_PD_EPS_MOBILITY_MANAGEMENT);
        msg_ptr++;

        // KSI and Sequence Number
        liblte_mme_pack_ksi_and_sequence_number_ie(&service_req->ksi_and_seq_num, &msg_ptr);

        // Short MAC
        liblte_mme_pack_short_mac_ie(service_req->short_mac, &msg_ptr);

        // Fill in the number of bytes used
        msg->N_bytes = msg_ptr - msg->msg;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_mme_unpack_service_request_msg(LIBLTE_BYTE_MSG_STRUCT                *msg,
                                                        LIBLTE_MME_SERVICE_REQUEST_MSG_STRUCT *service_req)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr = msg->msg;

    if(msg         != NULL &&
       service_req != NULL)
    {
        // Protocol Discriminator and Security Header Type
        msg_ptr++;

        // KSI and Sequence Number
        liblte_mme_unpack_ksi_and_sequence_number_ie(&msg_ptr, &service_req->ksi_and_seq_num);

        // Short MAC
        liblte_mme_unpack_short_mac_ie(&msg_ptr, &service_req->short_mac);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    Message Name: Tracking Area Update Accept

    Description: Sent by the network to the UE to provide the UE with
                 EPS mobility management related data in response to
                 a tracking area update request message.

    Document Reference: 24.301 v10.2.0 Section 8.2.26
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_mme_pack_tracking_area_update_accept_msg(LIBLTE_MME_TRACKING_AREA_UPDATE_ACCEPT_MSG_STRUCT *ta_update_accept,
                                                                  uint8                                              sec_hdr_type,
                                                                  uint32                                             count,
                                                                  LIBLTE_BYTE_MSG_STRUCT                            *msg)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr = msg->msg;

    if(ta_update_accept != NULL &&
       msg              != NULL)
    {
        if(LIBLTE_MME_SECURITY_HDR_TYPE_PLAIN_NAS != sec_hdr_type)
        {
            // Protocol Discriminator and Security Header Type
            *msg_ptr = (sec_hdr_type << 4) | (LIBLTE_MME_PD_EPS_MOBILITY_MANAGEMENT);
            msg_ptr++;

            // MAC will be filled in later
            msg_ptr += 4;

            // Sequence Number
            *msg_ptr = count & 0xFF;
            msg_ptr++;
        }

        // Protocol Discriminator and Security Header Type
        *msg_ptr = (LIBLTE_MME_SECURITY_HDR_TYPE_PLAIN_NAS << 4) | (LIBLTE_MME_PD_EPS_MOBILITY_MANAGEMENT);
        msg_ptr++;

        // Message Type
        *msg_ptr = LIBLTE_MME_MSG_TYPE_TRACKING_AREA_UPDATE_ACCEPT;
        msg_ptr++;

        // EPS Update Result & Spare Half Octet
        *msg_ptr = 0;
        liblte_mme_pack_eps_update_result_ie(ta_update_accept->eps_update_result, 0, &msg_ptr);
        msg_ptr++;

        // T3412 Value
        if(ta_update_accept->t3412_present)
        {
            *msg_ptr = LIBLTE_MME_T3412_VALUE_IEI;
            msg_ptr++;
            liblte_mme_pack_gprs_timer_ie(&ta_update_accept->t3412, &msg_ptr);
        }

        // GUTI
        if(ta_update_accept->guti_present)
        {
            *msg_ptr = LIBLTE_MME_GUTI_IEI;
            msg_ptr++;
            liblte_mme_pack_eps_mobile_id_ie(&ta_update_accept->guti, &msg_ptr);
        }

        // TAI List
        if(ta_update_accept->tai_list_present)
        {
            *msg_ptr = LIBLTE_MME_TAI_LIST_IEI;
            msg_ptr++;
            liblte_mme_pack_tracking_area_identity_list_ie(&ta_update_accept->tai_list, &msg_ptr);
        }

        // EPS Bearer Context Status
        if(ta_update_accept->eps_bearer_context_status_present)
        {
            *msg_ptr = LIBLTE_MME_EPS_BEARER_CONTEXT_STATUS_IEI;
            msg_ptr++;
            liblte_mme_pack_eps_bearer_context_status_ie(&ta_update_accept->eps_bearer_context_status, &msg_ptr);
        }

        // Location Area Identification
        if(ta_update_accept->lai_present)
        {
            *msg_ptr = LIBLTE_MME_LOCATION_AREA_IDENTIFICATION_IEI;
            msg_ptr++;
            liblte_mme_pack_location_area_id_ie(&ta_update_accept->lai, &msg_ptr);
        }

        // MS Identity
        if(ta_update_accept->ms_id_present)
        {
            *msg_ptr = LIBLTE_MME_MS_IDENTITY_IEI;
            msg_ptr++;
            liblte_mme_pack_mobile_id_ie(&ta_update_accept->ms_id, &msg_ptr);
        }

        // EMM Cause
        if(ta_update_accept->emm_cause_present)
        {
            *msg_ptr = LIBLTE_MME_EMM_CAUSE_IEI;
            msg_ptr++;
            liblte_mme_pack_emm_cause_ie(ta_update_accept->emm_cause, &msg_ptr);
        }

        // T3402 Value
        if(ta_update_accept->t3402_present)
        {
            *msg_ptr = LIBLTE_MME_T3402_VALUE_IEI;
            msg_ptr++;
            liblte_mme_pack_gprs_timer_ie(&ta_update_accept->t3402, &msg_ptr);
        }

        // T3423 Value
        if(ta_update_accept->t3423_present)
        {
            *msg_ptr = LIBLTE_MME_T3423_VALUE_IEI;
            msg_ptr++;
            liblte_mme_pack_gprs_timer_ie(&ta_update_accept->t3423, &msg_ptr);
        }

        // Equivalent PLMNs
        if(ta_update_accept->equivalent_plmns_present)
        {
            *msg_ptr = LIBLTE_MME_EQUIVALENT_PLMNS_IEI;
            msg_ptr++;
            liblte_mme_pack_plmn_list_ie(&ta_update_accept->equivalent_plmns, &msg_ptr);
        }

        // Emergency Number List
        if(ta_update_accept->emerg_num_list_present)
        {
            *msg_ptr = LIBLTE_MME_EMERGENCY_NUMBER_LIST_IEI;
            msg_ptr++;
            liblte_mme_pack_emergency_number_list_ie(&ta_update_accept->emerg_num_list, &msg_ptr);
        }

        // EPS Network Feature Support
        if(ta_update_accept->eps_network_feature_support_present)
        {
            *msg_ptr = LIBLTE_MME_EPS_NETWORK_FEATURE_SUPPORT_IEI;
            msg_ptr++;
            liblte_mme_pack_eps_network_feature_support_ie(&ta_update_accept->eps_network_feature_support, &msg_ptr);
        }

        // Additional Update Result
        if(ta_update_accept->additional_update_result_present)
        {
            *msg_ptr = LIBLTE_MME_ADDITIONAL_UPDATE_RESULT_IEI << 4;
            liblte_mme_pack_additional_update_result_ie(ta_update_accept->additional_update_result, 0, &msg_ptr);
            msg_ptr++;
        }

        // T3412 Extended Value
        if(ta_update_accept->t3412_ext_present)
        {
            *msg_ptr = LIBLTE_MME_T3412_EXTENDED_VALUE_IEI;
            msg_ptr++;
            liblte_mme_pack_gprs_timer_3_ie(&ta_update_accept->t3412_ext, &msg_ptr);
        }

        // Fill in the number of bytes used
        msg->N_bytes = msg_ptr - msg->msg;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_mme_unpack_tracking_area_update_accept_msg(LIBLTE_BYTE_MSG_STRUCT                            *msg,
                                                                    LIBLTE_MME_TRACKING_AREA_UPDATE_ACCEPT_MSG_STRUCT *ta_update_accept)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr = msg->msg;
    uint8              sec_hdr_type;

    if(msg              != NULL &&
       ta_update_accept != NULL)
    {
        // Security Header Type
        sec_hdr_type = (msg->msg[0] & 0xF0) >> 4;
        if(LIBLTE_MME_SECURITY_HDR_TYPE_PLAIN_NAS == sec_hdr_type)
        {
            msg_ptr++;
        }else{
            msg_ptr += 7;
        }

        // Skip Message Type
        msg_ptr++;

        // EPS Update Result & Spare Half Octet
        liblte_mme_unpack_eps_update_result_ie(&msg_ptr, 0, &ta_update_accept->eps_update_result);
        msg_ptr++;

        // T3412 Value
        if(LIBLTE_MME_T3412_VALUE_IEI == *msg_ptr)
        {
            msg_ptr++;
            liblte_mme_unpack_gprs_timer_ie(&msg_ptr, &ta_update_accept->t3412);
            ta_update_accept->t3412_present = true;
        }else{
            ta_update_accept->t3412_present = false;
        }

        // GUTI
        if(LIBLTE_MME_GUTI_IEI == *msg_ptr)
        {
            msg_ptr++;
            liblte_mme_unpack_eps_mobile_id_ie(&msg_ptr, &ta_update_accept->guti);
            ta_update_accept->guti_present = true;
        }else{
            ta_update_accept->guti_present = false;
        }

        // TAI List
        if(LIBLTE_MME_TAI_LIST_IEI == *msg_ptr)
        {
            msg_ptr++;
            liblte_mme_unpack_tracking_area_identity_list_ie(&msg_ptr, &ta_update_accept->tai_list);
            ta_update_accept->tai_list_present = true;
        }else{
            ta_update_accept->tai_list_present = false;
        }

        // EPS Bearer Context Status
        if(LIBLTE_MME_EPS_BEARER_CONTEXT_STATUS_IEI == *msg_ptr)
        {
            msg_ptr++;
            liblte_mme_unpack_eps_bearer_context_status_ie(&msg_ptr, &ta_update_accept->eps_bearer_context_status);
            ta_update_accept->eps_bearer_context_status_present = true;
        }else{
            ta_update_accept->eps_bearer_context_status_present = false;
        }

        // Location Area Identification
        if(LIBLTE_MME_LOCATION_AREA_IDENTIFICATION_IEI == *msg_ptr)
        {
            msg_ptr++;
            liblte_mme_unpack_location_area_id_ie(&msg_ptr, &ta_update_accept->lai);
            ta_update_accept->lai_present = true;
        }else{
            ta_update_accept->lai_present = false;
        }

        // MS Identity
        if(LIBLTE_MME_MS_IDENTITY_IEI == *msg_ptr)
        {
            msg_ptr++;
            liblte_mme_unpack_mobile_id_ie(&msg_ptr, &ta_update_accept->ms_id);
            ta_update_accept->ms_id_present = true;
        }else{
            ta_update_accept->ms_id_present = false;
        }

        // EMM Cause
        if(LIBLTE_MME_EMM_CAUSE_IEI == *msg_ptr)
        {
            msg_ptr++;
            liblte_mme_unpack_emm_cause_ie(&msg_ptr, &ta_update_accept->emm_cause);
            ta_update_accept->emm_cause_present = true;
        }else{
            ta_update_accept->emm_cause_present = false;
        }

        // T3402 Value
        if(LIBLTE_MME_T3402_VALUE_IEI == *msg_ptr)
        {
            msg_ptr++;
            liblte_mme_unpack_gprs_timer_ie(&msg_ptr, &ta_update_accept->t3402);
            ta_update_accept->t3402_present = true;
        }else{
            ta_update_accept->t3402_present = false;
        }

        // T3423 Value
        if(LIBLTE_MME_T3423_VALUE_IEI == *msg_ptr)
        {
            msg_ptr++;
            liblte_mme_unpack_gprs_timer_ie(&msg_ptr, &ta_update_accept->t3423);
            ta_update_accept->t3423_present = true;
        }else{
            ta_update_accept->t3423_present = false;
        }

        // Equivalent PLMNs
        if(LIBLTE_MME_EQUIVALENT_PLMNS_IEI == *msg_ptr)
        {
            msg_ptr++;
            liblte_mme_unpack_plmn_list_ie(&msg_ptr, &ta_update_accept->equivalent_plmns);
            ta_update_accept->equivalent_plmns_present = true;
        }else{
            ta_update_accept->equivalent_plmns_present = false;
        }

        // Emergency Number List
        if(LIBLTE_MME_EMERGENCY_NUMBER_LIST_IEI == *msg_ptr)
        {
            msg_ptr++;
            liblte_mme_unpack_emergency_number_list_ie(&msg_ptr, &ta_update_accept->emerg_num_list);
            ta_update_accept->emerg_num_list_present = true;
        }else{
            ta_update_accept->emerg_num_list_present = false;
        }

        // EPS Network Feature Support
        if(LIBLTE_MME_EPS_NETWORK_FEATURE_SUPPORT_IEI == *msg_ptr)
        {
            msg_ptr++;
            liblte_mme_unpack_eps_network_feature_support_ie(&msg_ptr, &ta_update_accept->eps_network_feature_support);
            ta_update_accept->eps_network_feature_support_present = true;
        }else{
            ta_update_accept->eps_network_feature_support_present = false;
        }

        // Additional Update Result
        if((LIBLTE_MME_ADDITIONAL_UPDATE_RESULT_IEI << 4) == (*msg_ptr & 0xF0))
        {
            liblte_mme_unpack_additional_update_result_ie(&msg_ptr, 0, &ta_update_accept->additional_update_result);
            msg_ptr++;
            ta_update_accept->additional_update_result_present = true;
        }else{
            ta_update_accept->additional_update_result_present = false;
        }

        // T3412 Extended Value
        if(LIBLTE_MME_T3412_EXTENDED_VALUE_IEI == *msg_ptr)
        {
            msg_ptr++;
            liblte_mme_unpack_gprs_timer_3_ie(&msg_ptr, &ta_update_accept->t3412_ext);
            ta_update_accept->t3412_ext_present = true;
        }else{
            ta_update_accept->t3412_ext_present = false;
        }

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    Message Name: Tracking Area Update Complete

    Description: Sent by the UE to the network in response to a
                 tracking area update accept message if a GUTI has
                 been changed or a new TMSI has been assigned.

    Document Reference: 24.301 v10.2.0 Section 8.2.27
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_mme_pack_tracking_area_update_complete_msg(LIBLTE_MME_TRACKING_AREA_UPDATE_COMPLETE_MSG_STRUCT *ta_update_complete,
                                                                    uint8                                                sec_hdr_type,
                                                                    uint32                                               count,
                                                                    LIBLTE_BYTE_MSG_STRUCT                              *msg)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr = msg->msg;

    if(ta_update_complete != NULL &&
       msg                != NULL)
    {
        if(LIBLTE_MME_SECURITY_HDR_TYPE_PLAIN_NAS != sec_hdr_type)
        {
            // Protocol Discriminator and Security Header Type
            *msg_ptr = (sec_hdr_type << 4) | (LIBLTE_MME_PD_EPS_MOBILITY_MANAGEMENT);
            msg_ptr++;

            // MAC will be filled in later
            msg_ptr += 4;

            // Sequence Number
            *msg_ptr = count & 0xFF;
            msg_ptr++;
        }

        // Protocol Discriminator and Security Header Type
        *msg_ptr = (LIBLTE_MME_SECURITY_HDR_TYPE_PLAIN_NAS << 4) | (LIBLTE_MME_PD_EPS_MOBILITY_MANAGEMENT);
        msg_ptr++;

        // Message Type
        *msg_ptr = LIBLTE_MME_MSG_TYPE_TRACKING_AREA_UPDATE_COMPLETE;
        msg_ptr++;

        // Fill in the number of bytes used
        msg->N_bytes = msg_ptr - msg->msg;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_mme_unpack_tracking_area_update_complete_msg(LIBLTE_BYTE_MSG_STRUCT                              *msg,
                                                                      LIBLTE_MME_TRACKING_AREA_UPDATE_COMPLETE_MSG_STRUCT *ta_update_complete)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr = msg->msg;
    uint8              sec_hdr_type;

    if(msg                != NULL &&
       ta_update_complete != NULL)
    {
        // Security Header Type
        sec_hdr_type = (msg->msg[0] & 0xF0) >> 4;
        if(LIBLTE_MME_SECURITY_HDR_TYPE_PLAIN_NAS == sec_hdr_type)
        {
            msg_ptr++;
        }else{
            msg_ptr += 7;
        }

        // Skip Message Type
        msg_ptr++;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    Message Name: Tracking Area Update Reject

    Description: Sent by the network to the UE in order to reject the
                 tracking area updating procedure.

    Document Reference: 24.301 v10.2.0 Section 8.2.28
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_mme_pack_tracking_area_update_reject_msg(LIBLTE_MME_TRACKING_AREA_UPDATE_REJECT_MSG_STRUCT *ta_update_rej,
                                                                  uint8                                              sec_hdr_type,
                                                                  uint32                                             count,
                                                                  LIBLTE_BYTE_MSG_STRUCT                            *msg)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr = msg->msg;

    if(ta_update_rej != NULL &&
       msg           != NULL)
    {
        if(LIBLTE_MME_SECURITY_HDR_TYPE_PLAIN_NAS != sec_hdr_type)
        {
            // Protocol Discriminator and Security Header Type
            *msg_ptr = (sec_hdr_type << 4) | (LIBLTE_MME_PD_EPS_MOBILITY_MANAGEMENT);
            msg_ptr++;

            // MAC will be filled in later
            msg_ptr += 4;

            // Sequence Number
            *msg_ptr = count & 0xFF;
            msg_ptr++;
        }

        // Protocol Discriminator and Security Header Type
        *msg_ptr = (LIBLTE_MME_SECURITY_HDR_TYPE_PLAIN_NAS << 4) | (LIBLTE_MME_PD_EPS_MOBILITY_MANAGEMENT);
        msg_ptr++;

        // Message Type
        *msg_ptr = LIBLTE_MME_MSG_TYPE_TRACKING_AREA_UPDATE_REJECT;
        msg_ptr++;

        // EMM Cause
        liblte_mme_pack_emm_cause_ie(ta_update_rej->emm_cause, &msg_ptr);

        // T3446 Value
        if(ta_update_rej->t3446_present)
        {
            *msg_ptr = LIBLTE_MME_T3446_VALUE_IEI;
            msg_ptr++;
            liblte_mme_pack_gprs_timer_2_ie(ta_update_rej->t3446, &msg_ptr);
        }

        // Fill in the number of bytes used
        msg->N_bytes = msg_ptr - msg->msg;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_mme_unpack_tracking_area_update_reject_msg(LIBLTE_BYTE_MSG_STRUCT                            *msg,
                                                                    LIBLTE_MME_TRACKING_AREA_UPDATE_REJECT_MSG_STRUCT *ta_update_rej)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr = msg->msg;
    uint8              sec_hdr_type;

    if(msg           != NULL &&
       ta_update_rej != NULL)
    {
        // Security Header Type
        sec_hdr_type = (msg->msg[0] & 0xF0) >> 4;
        if(LIBLTE_MME_SECURITY_HDR_TYPE_PLAIN_NAS == sec_hdr_type)
        {
            msg_ptr++;
        }else{
            msg_ptr += 7;
        }

        // Skip Message Type
        msg_ptr++;

        // EMM Cause
        liblte_mme_unpack_emm_cause_ie(&msg_ptr, &ta_update_rej->emm_cause);

        // T3446 Value
        if(LIBLTE_MME_T3446_VALUE_IEI == *msg_ptr)
        {
            msg_ptr++;
            liblte_mme_unpack_gprs_timer_2_ie(&msg_ptr, &ta_update_rej->t3446);
            ta_update_rej->t3446_present = true;
        }else{
            ta_update_rej->t3446_present = false;
        }

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    Message Name: Tracking Area Update Request

    Description: Sent by the UE to the network to initiate a tracking
                 area updating procedure.

    Document Reference: 24.301 v10.2.0 Section 8.2.29
*********************************************************************/
// FIXME

/*********************************************************************
    Message Name: Uplink NAS Transport

    Description: Sent by the UE to the network in order to carry an
                 SMS message in encapsulated format.

    Document Reference: 24.301 v10.2.0 Section 8.2.30
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_mme_pack_uplink_nas_transport_msg(LIBLTE_MME_UPLINK_NAS_TRANSPORT_MSG_STRUCT *ul_nas_transport,
                                                           uint8                                       sec_hdr_type,
                                                           uint32                                      count,
                                                           LIBLTE_BYTE_MSG_STRUCT                     *msg)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr = msg->msg;

    if(ul_nas_transport != NULL &&
       msg              != NULL)
    {
        if(LIBLTE_MME_SECURITY_HDR_TYPE_PLAIN_NAS != sec_hdr_type)
        {
            // Protocol Discriminator and Security Header Type
            *msg_ptr = (sec_hdr_type << 4) | (LIBLTE_MME_PD_EPS_MOBILITY_MANAGEMENT);
            msg_ptr++;

            // MAC will be filled in later
            msg_ptr += 4;

            // Sequence Number
            *msg_ptr = count & 0xFF;
            msg_ptr++;
        }

        // Protocol Discriminator and Security Header Type
        *msg_ptr = (LIBLTE_MME_SECURITY_HDR_TYPE_PLAIN_NAS << 4) | (LIBLTE_MME_PD_EPS_MOBILITY_MANAGEMENT);
        msg_ptr++;

        // Message Type
        *msg_ptr = LIBLTE_MME_MSG_TYPE_UPLINK_NAS_TRANSPORT;
        msg_ptr++;

        // NAS Message Container
        liblte_mme_pack_nas_message_container_ie(&ul_nas_transport->nas_msg, &msg_ptr);

        // Fill in the number of bytes used
        msg->N_bytes = msg_ptr - msg->msg;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_mme_unpack_uplink_nas_transport_msg(LIBLTE_BYTE_MSG_STRUCT                     *msg,
                                                             LIBLTE_MME_UPLINK_NAS_TRANSPORT_MSG_STRUCT *ul_nas_transport)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr = msg->msg;
    uint8              sec_hdr_type;

    if(msg              != NULL &&
       ul_nas_transport != NULL)
    {
        // Security Header Type
        sec_hdr_type = (msg->msg[0] & 0xF0) >> 4;
        if(LIBLTE_MME_SECURITY_HDR_TYPE_PLAIN_NAS == sec_hdr_type)
        {
            msg_ptr++;
        }else{
            msg_ptr += 7;
        }

        // Skip Message Type
        msg_ptr++;

        // NAS Message Container
        liblte_mme_unpack_nas_message_container_ie(&msg_ptr, &ul_nas_transport->nas_msg);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    Message Name: Downlink Generic NAS Transport

    Description: Sent by the network to the UE in order to carry an
                 application message in encapsulated format.

    Document Reference: 24.301 v10.2.0 Section 8.2.31
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_mme_pack_downlink_generic_nas_transport_msg(LIBLTE_MME_DOWNLINK_GENERIC_NAS_TRANSPORT_MSG_STRUCT *dl_generic_nas_transport,
                                                                     uint8                                                 sec_hdr_type,
                                                                     uint32                                                count,
                                                                     LIBLTE_BYTE_MSG_STRUCT                               *msg)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr = msg->msg;

    if(dl_generic_nas_transport != NULL &&
       msg                      != NULL)
    {
        if(LIBLTE_MME_SECURITY_HDR_TYPE_PLAIN_NAS != sec_hdr_type)
        {
            // Protocol Discriminator and Security Header Type
            *msg_ptr = (sec_hdr_type << 4) | (LIBLTE_MME_PD_EPS_MOBILITY_MANAGEMENT);
            msg_ptr++;

            // MAC will be filled in later
            msg_ptr += 4;

            // Sequence Number
            *msg_ptr = count & 0xFF;
            msg_ptr++;
        }

        // Protocol Discriminator and Security Header Type
        *msg_ptr = (LIBLTE_MME_SECURITY_HDR_TYPE_PLAIN_NAS << 4) | (LIBLTE_MME_PD_EPS_MOBILITY_MANAGEMENT);
        msg_ptr++;

        // Message Type
        *msg_ptr = LIBLTE_MME_MSG_TYPE_DOWNLINK_GENERIC_NAS_TRANSPORT;
        msg_ptr++;

        // Generic Message Container Type
        liblte_mme_pack_generic_message_container_type_ie(dl_generic_nas_transport->generic_msg_cont_type, &msg_ptr);

        // Generic Message Container
        liblte_mme_pack_generic_message_container_ie(&dl_generic_nas_transport->generic_msg_cont, &msg_ptr);

        // Additional Information
        liblte_mme_pack_additional_information_ie(&dl_generic_nas_transport->add_info, &msg_ptr);

        // Fill in the number of bytes used
        msg->N_bytes = msg_ptr - msg->msg;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_mme_unpack_downlink_generic_nas_transport_msg(LIBLTE_BYTE_MSG_STRUCT                               *msg,
                                                                       LIBLTE_MME_DOWNLINK_GENERIC_NAS_TRANSPORT_MSG_STRUCT *dl_generic_nas_transport)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr = msg->msg;
    uint8              sec_hdr_type;

    if(msg                      != NULL &&
       dl_generic_nas_transport != NULL)
    {
        // Security Header Type
        sec_hdr_type = (msg->msg[0] & 0xF0) >> 4;
        if(LIBLTE_MME_SECURITY_HDR_TYPE_PLAIN_NAS == sec_hdr_type)
        {
            msg_ptr++;
        }else{
            msg_ptr += 7;
        }

        // Skip Message Type
        msg_ptr++;

        // Generic Message Container Type
        liblte_mme_unpack_generic_message_container_type_ie(&msg_ptr, &dl_generic_nas_transport->generic_msg_cont_type);

        // Generic Message Container
        liblte_mme_unpack_generic_message_container_ie(&msg_ptr, &dl_generic_nas_transport->generic_msg_cont);

        // Additional Information
        liblte_mme_unpack_additional_information_ie(&msg_ptr, &dl_generic_nas_transport->add_info);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    Message Name: Uplink Generic NAS Transport

    Description: Sent by the UE to the network in order to carry an
                 application protocol message in encapsulated format.

    Document Reference: 24.301 v10.2.0 Section 8.2.32
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_mme_pack_uplink_generic_nas_transport_msg(LIBLTE_MME_UPLINK_GENERIC_NAS_TRANSPORT_MSG_STRUCT *ul_generic_nas_transport,
                                                                   uint8                                               sec_hdr_type,
                                                                   uint32                                              count,
                                                                   LIBLTE_BYTE_MSG_STRUCT                             *msg)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr = msg->msg;

    if(ul_generic_nas_transport != NULL &&
       msg                      != NULL)
    {
        if(LIBLTE_MME_SECURITY_HDR_TYPE_PLAIN_NAS != sec_hdr_type)
        {
            // Protocol Discriminator and Security Header Type
            *msg_ptr = (sec_hdr_type << 4) | (LIBLTE_MME_PD_EPS_MOBILITY_MANAGEMENT);
            msg_ptr++;

            // MAC will be filled in later
            msg_ptr += 4;

            // Sequence Number
            *msg_ptr = count & 0xFF;
            msg_ptr++;
        }

        // Protocol Discriminator and Security Header Type
        *msg_ptr = (LIBLTE_MME_SECURITY_HDR_TYPE_PLAIN_NAS << 4) | (LIBLTE_MME_PD_EPS_MOBILITY_MANAGEMENT);
        msg_ptr++;

        // Message Type
        *msg_ptr = LIBLTE_MME_MSG_TYPE_UPLINK_GENERIC_NAS_TRANSPORT;
        msg_ptr++;

        // Generic Message Container Type
        liblte_mme_pack_generic_message_container_type_ie(ul_generic_nas_transport->generic_msg_cont_type, &msg_ptr);

        // Generic Message Container
        liblte_mme_pack_generic_message_container_ie(&ul_generic_nas_transport->generic_msg_cont, &msg_ptr);

        // Additional Information
        liblte_mme_pack_additional_information_ie(&ul_generic_nas_transport->add_info, &msg_ptr);

        // Fill in the number of bytes used
        msg->N_bytes = msg_ptr - msg->msg;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_mme_unpack_uplink_generic_nas_transport_msg(LIBLTE_BYTE_MSG_STRUCT                             *msg,
                                                                     LIBLTE_MME_UPLINK_GENERIC_NAS_TRANSPORT_MSG_STRUCT *ul_generic_nas_transport)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr = msg->msg;
    uint8              sec_hdr_type;

    if(msg                      != NULL &&
       ul_generic_nas_transport != NULL)
    {
        // Security Header Type
        sec_hdr_type = (msg->msg[0] & 0xF0) >> 4;
        if(LIBLTE_MME_SECURITY_HDR_TYPE_PLAIN_NAS == sec_hdr_type)
        {
            msg_ptr++;
        }else{
            msg_ptr += 7;
        }

        // Skip Message Type
        msg_ptr++;

        // Generic Message Container Type
        liblte_mme_unpack_generic_message_container_type_ie(&msg_ptr, &ul_generic_nas_transport->generic_msg_cont_type);

        // Generic Message Container
        liblte_mme_unpack_generic_message_container_ie(&msg_ptr, &ul_generic_nas_transport->generic_msg_cont);

        // Additional Information
        liblte_mme_unpack_additional_information_ie(&msg_ptr, &ul_generic_nas_transport->add_info);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    Message Name: Activate Dedicated EPS Bearer Context Accept

    Description: Sent by the UE to the network to acknowledge
                 activation of a dedicated EPS bearer context
                 associated with the same PDN address(es) and APN as
                 an already active EPS bearer context.

    Document Reference: 24.301 v10.2.0 Section 8.3.1
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_mme_pack_activate_dedicated_eps_bearer_context_accept_msg(LIBLTE_MME_ACTIVATE_DEDICATED_EPS_BEARER_CONTEXT_ACCEPT_MSG_STRUCT *act_ded_eps_bearer_context_accept,
                                                                                   LIBLTE_BYTE_MSG_STRUCT                                             *msg)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr = msg->msg;

    if(act_ded_eps_bearer_context_accept != NULL &&
       msg                               != NULL)
    {
        // Protocol Discriminator and EPS Bearer ID
        *msg_ptr = (act_ded_eps_bearer_context_accept->eps_bearer_id << 4) | (LIBLTE_MME_PD_EPS_SESSION_MANAGEMENT);
        msg_ptr++;

        // Procedure Transaction ID
        *msg_ptr = act_ded_eps_bearer_context_accept->proc_transaction_id;
        msg_ptr++;

        // Message Type
        *msg_ptr = LIBLTE_MME_MSG_TYPE_ACTIVATE_DEDICATED_EPS_BEARER_CONTEXT_ACCEPT;
        msg_ptr++;

        // Protocol Configuration Options
        if(act_ded_eps_bearer_context_accept->protocol_cnfg_opts_present)
        {
            *msg_ptr = LIBLTE_MME_PROTOCOL_CONFIGURATION_OPTIONS_IEI;
            msg_ptr++;
            liblte_mme_pack_protocol_config_options_ie(&act_ded_eps_bearer_context_accept->protocol_cnfg_opts, &msg_ptr);
        }

        // Fill in the number of bytes used
        msg->N_bytes = msg_ptr - msg->msg;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_mme_unpack_activate_dedicated_eps_bearer_context_accept_msg(LIBLTE_BYTE_MSG_STRUCT                                             *msg,
                                                                                     LIBLTE_MME_ACTIVATE_DEDICATED_EPS_BEARER_CONTEXT_ACCEPT_MSG_STRUCT *act_ded_eps_bearer_context_accept)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr = msg->msg;

    if(msg                               != NULL &&
       act_ded_eps_bearer_context_accept != NULL)
    {
        // EPS Bearer ID
        act_ded_eps_bearer_context_accept->eps_bearer_id = (*msg_ptr >> 4);
        msg_ptr++;

        // Procedure Transaction ID
        act_ded_eps_bearer_context_accept->proc_transaction_id = *msg_ptr;
        msg_ptr++;

        // Skip Message Type
        msg_ptr++;

        // Protocol Configuration Options
        if(LIBLTE_MME_PROTOCOL_CONFIGURATION_OPTIONS_IEI == *msg_ptr)
        {
            msg_ptr++;
            liblte_mme_unpack_protocol_config_options_ie(&msg_ptr, &act_ded_eps_bearer_context_accept->protocol_cnfg_opts);
            act_ded_eps_bearer_context_accept->protocol_cnfg_opts_present = true;
        }else{
            act_ded_eps_bearer_context_accept->protocol_cnfg_opts_present = false;
        }

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    Message Name: Activate Dedicated EPS Bearer Context Reject

    Description: Sent by the UE to the network to reject activation
                 of a dedicated EPS bearer context.

    Document Reference: 24.301 v10.2.0 Section 8.3.2
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_mme_pack_activate_dedicated_eps_bearer_context_reject_msg(LIBLTE_MME_ACTIVATE_DEDICATED_EPS_BEARER_CONTEXT_REJECT_MSG_STRUCT *act_ded_eps_bearer_context_rej,
                                                                                   LIBLTE_BYTE_MSG_STRUCT                                             *msg)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr = msg->msg;

    if(act_ded_eps_bearer_context_rej != NULL &&
       msg                            != NULL)
    {
        // Protocol Discriminator and EPS Bearer ID
        *msg_ptr = (act_ded_eps_bearer_context_rej->eps_bearer_id << 4) | (LIBLTE_MME_PD_EPS_SESSION_MANAGEMENT);
        msg_ptr++;

        // Procedure Transaction ID
        *msg_ptr = act_ded_eps_bearer_context_rej->proc_transaction_id;
        msg_ptr++;

        // Message Type
        *msg_ptr = LIBLTE_MME_MSG_TYPE_ACTIVATE_DEDICATED_EPS_BEARER_CONTEXT_REJECT;
        msg_ptr++;

        // ESM Cause
        liblte_mme_pack_esm_cause_ie(act_ded_eps_bearer_context_rej->esm_cause, &msg_ptr);

        // Protocol Configuration Options
        if(act_ded_eps_bearer_context_rej->protocol_cnfg_opts_present)
        {
            *msg_ptr = LIBLTE_MME_PROTOCOL_CONFIGURATION_OPTIONS_IEI;
            msg_ptr++;
            liblte_mme_pack_protocol_config_options_ie(&act_ded_eps_bearer_context_rej->protocol_cnfg_opts, &msg_ptr);
        }

        // Fill in the number of bytes used
        msg->N_bytes = msg_ptr - msg->msg;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_mme_unpack_activate_dedicated_eps_bearer_context_reject_msg(LIBLTE_BYTE_MSG_STRUCT                                             *msg,
                                                                                     LIBLTE_MME_ACTIVATE_DEDICATED_EPS_BEARER_CONTEXT_REJECT_MSG_STRUCT *act_ded_eps_bearer_context_rej)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr = msg->msg;

    if(msg                            != NULL &&
       act_ded_eps_bearer_context_rej != NULL)
    {
        // EPS Bearer ID
        act_ded_eps_bearer_context_rej->eps_bearer_id = (*msg_ptr >> 4);
        msg_ptr++;

        // Procedure Transaction ID
        act_ded_eps_bearer_context_rej->proc_transaction_id = *msg_ptr;
        msg_ptr++;

        // Skip Message Type
        msg_ptr++;

        // ESM Cause
        liblte_mme_unpack_esm_cause_ie(&msg_ptr, &act_ded_eps_bearer_context_rej->esm_cause);

        // Protocol Configuration Options
        if(LIBLTE_MME_PROTOCOL_CONFIGURATION_OPTIONS_IEI == *msg_ptr)
        {
            msg_ptr++;
            liblte_mme_unpack_protocol_config_options_ie(&msg_ptr, &act_ded_eps_bearer_context_rej->protocol_cnfg_opts);
            act_ded_eps_bearer_context_rej->protocol_cnfg_opts_present = true;
        }else{
            act_ded_eps_bearer_context_rej->protocol_cnfg_opts_present = false;
        }

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    Message Name: Activate Dedicated EPS Bearer Context Request

    Description: Sent by the network to the UE to request activation
                 of a dedicated EPS bearer context associated with
                 the same PDN address(es) and APN as an already
                 active default EPS bearer context.

    Document Reference: 24.301 v10.2.0 Section 8.3.3
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_mme_pack_activate_dedicated_eps_bearer_context_request_msg(LIBLTE_MME_ACTIVATE_DEDICATED_EPS_BEARER_CONTEXT_REQUEST_MSG_STRUCT *act_ded_eps_bearer_context_req,
                                                                                    LIBLTE_BYTE_MSG_STRUCT                                              *msg)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr = msg->msg;

    if(act_ded_eps_bearer_context_req != NULL &&
       msg                            != NULL)
    {
        // Protocol Discriminator and EPS Bearer ID
        *msg_ptr = (act_ded_eps_bearer_context_req->eps_bearer_id << 4) | (LIBLTE_MME_PD_EPS_SESSION_MANAGEMENT);
        msg_ptr++;

        // Procedure Transaction ID
        *msg_ptr = act_ded_eps_bearer_context_req->proc_transaction_id;
        msg_ptr++;

        // Message Type
        *msg_ptr = LIBLTE_MME_MSG_TYPE_ACTIVATE_DEDICATED_EPS_BEARER_CONTEXT_REQUEST;
        msg_ptr++;

        // Linked EPS Bearer Identity & Spare Half Octet
        *msg_ptr = 0;
        liblte_mme_pack_linked_eps_bearer_identity_ie(act_ded_eps_bearer_context_req->linked_eps_bearer_id, 0, &msg_ptr);
        msg_ptr++;

        // EPS QoS
        liblte_mme_pack_eps_quality_of_service_ie(&act_ded_eps_bearer_context_req->eps_qos, &msg_ptr);

        // TFT
        liblte_mme_pack_traffic_flow_template_ie(&act_ded_eps_bearer_context_req->tft, &msg_ptr);

        // Transaction Identifier
        if(act_ded_eps_bearer_context_req->transaction_id_present)
        {
            *msg_ptr = LIBLTE_MME_TRANSACTION_IDENTIFIER_IEI;
            msg_ptr++;
            liblte_mme_pack_transaction_identifier_ie(&act_ded_eps_bearer_context_req->transaction_id, &msg_ptr);
        }

        // Negotiated QoS
        if(act_ded_eps_bearer_context_req->negotiated_qos_present)
        {
            *msg_ptr = LIBLTE_MME_QUALITY_OF_SERVICE_IEI;
            msg_ptr++;
            liblte_mme_pack_quality_of_service_ie(&act_ded_eps_bearer_context_req->negotiated_qos, &msg_ptr);
        }

        // Negotiated LLC SAPI
        if(act_ded_eps_bearer_context_req->llc_sapi_present)
        {
            *msg_ptr = LIBLTE_MME_LLC_SAPI_IEI;
            msg_ptr++;
            liblte_mme_pack_llc_service_access_point_identifier_ie(act_ded_eps_bearer_context_req->llc_sapi, &msg_ptr);
        }

        // Radio Priority
        if(act_ded_eps_bearer_context_req->radio_prio_present)
        {
            *msg_ptr = LIBLTE_MME_RADIO_PRIORITY_IEI << 4;
            liblte_mme_pack_radio_priority_ie(act_ded_eps_bearer_context_req->radio_prio, 0, &msg_ptr);
            msg_ptr++;
        }

        // Packet Flow Identifier
        if(act_ded_eps_bearer_context_req->packet_flow_id_present)
        {
            *msg_ptr = LIBLTE_MME_PACKET_FLOW_IDENTIFIER_IEI;
            msg_ptr++;
            liblte_mme_pack_packet_flow_identifier_ie(act_ded_eps_bearer_context_req->packet_flow_id, &msg_ptr);
        }

        // Protocol Configuration Options
        if(act_ded_eps_bearer_context_req->protocol_cnfg_opts_present)
        {
            *msg_ptr = LIBLTE_MME_PROTOCOL_CONFIGURATION_OPTIONS_IEI;
            msg_ptr++;
            liblte_mme_pack_protocol_config_options_ie(&act_ded_eps_bearer_context_req->protocol_cnfg_opts, &msg_ptr);
        }

        // Fill in the number of bytes used
        msg->N_bytes = msg_ptr - msg->msg;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_mme_unpack_activate_dedicated_eps_bearer_context_request_msg(LIBLTE_BYTE_MSG_STRUCT                                              *msg,
                                                                                      LIBLTE_MME_ACTIVATE_DEDICATED_EPS_BEARER_CONTEXT_REQUEST_MSG_STRUCT *act_ded_eps_bearer_context_req)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr = msg->msg;

    if(msg                            != NULL &&
       act_ded_eps_bearer_context_req != NULL)
    {
        // EPS Bearer ID
        act_ded_eps_bearer_context_req->eps_bearer_id = (*msg_ptr >> 4);
        msg_ptr++;

        // Procedure Transaction ID
        act_ded_eps_bearer_context_req->proc_transaction_id = *msg_ptr;
        msg_ptr++;

        // Skip Message Type
        msg_ptr++;

        // Linked Bearer Identity & Spare Half Octet
        liblte_mme_unpack_linked_eps_bearer_identity_ie(&msg_ptr, 0, &act_ded_eps_bearer_context_req->linked_eps_bearer_id);
        msg_ptr++;

        // EPS QoS
        liblte_mme_unpack_eps_quality_of_service_ie(&msg_ptr, &act_ded_eps_bearer_context_req->eps_qos);

        // TFT
        liblte_mme_unpack_traffic_flow_template_ie(&msg_ptr, &act_ded_eps_bearer_context_req->tft);

        // Transaction Identifier
        if(LIBLTE_MME_TRANSACTION_IDENTIFIER_IEI == *msg_ptr)
        {
            msg_ptr++;
            liblte_mme_unpack_transaction_identifier_ie(&msg_ptr, &act_ded_eps_bearer_context_req->transaction_id);
            act_ded_eps_bearer_context_req->transaction_id_present = true;
        }else{
            act_ded_eps_bearer_context_req->transaction_id_present = false;
        }

        // Negotiated QoS
        if(LIBLTE_MME_QUALITY_OF_SERVICE_IEI == *msg_ptr)
        {
            msg_ptr++;
            liblte_mme_unpack_quality_of_service_ie(&msg_ptr, &act_ded_eps_bearer_context_req->negotiated_qos);
            act_ded_eps_bearer_context_req->negotiated_qos_present = true;
        }else{
            act_ded_eps_bearer_context_req->negotiated_qos_present = false;
        }

        // Negotiated LLC SAPI
        if(LIBLTE_MME_LLC_SAPI_IEI == *msg_ptr)
        {
            msg_ptr++;
            liblte_mme_unpack_llc_service_access_point_identifier_ie(&msg_ptr, &act_ded_eps_bearer_context_req->llc_sapi);
            act_ded_eps_bearer_context_req->llc_sapi_present = true;
        }else{
            act_ded_eps_bearer_context_req->llc_sapi_present = false;
        }

        // Radio Priority
        if((LIBLTE_MME_RADIO_PRIORITY_IEI << 4) == (*msg_ptr & 0xF0))
        {
            liblte_mme_unpack_radio_priority_ie(&msg_ptr, 0, &act_ded_eps_bearer_context_req->radio_prio);
            msg_ptr++;
            act_ded_eps_bearer_context_req->radio_prio_present = true;
        }else{
            act_ded_eps_bearer_context_req->radio_prio_present = false;
        }

        // Packet Flow Identifier
        if(LIBLTE_MME_PACKET_FLOW_IDENTIFIER_IEI == *msg_ptr)
        {
            msg_ptr++;
            liblte_mme_unpack_packet_flow_identifier_ie(&msg_ptr, &act_ded_eps_bearer_context_req->packet_flow_id);
            act_ded_eps_bearer_context_req->packet_flow_id_present = true;
        }else{
            act_ded_eps_bearer_context_req->packet_flow_id_present = false;
        }

        // Protocol Configuration Options
        if(LIBLTE_MME_PROTOCOL_CONFIGURATION_OPTIONS_IEI == *msg_ptr)
        {
            msg_ptr++;
            liblte_mme_unpack_protocol_config_options_ie(&msg_ptr, &act_ded_eps_bearer_context_req->protocol_cnfg_opts);
            act_ded_eps_bearer_context_req->protocol_cnfg_opts_present = true;
        }else{
            act_ded_eps_bearer_context_req->protocol_cnfg_opts_present = false;
        }

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    Message Name: Activate Default EPS Bearer Context Accept

    Description: Sent by the UE to the network to acknowledge
                 activation of a default EPS bearer context.

    Document Reference: 24.301 v10.2.0 Section 8.3.4
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_mme_pack_activate_default_eps_bearer_context_accept_msg(LIBLTE_MME_ACTIVATE_DEFAULT_EPS_BEARER_CONTEXT_ACCEPT_MSG_STRUCT *act_def_eps_bearer_context_accept,
                                                                                 LIBLTE_BYTE_MSG_STRUCT                                           *msg)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr = msg->msg;

    if(act_def_eps_bearer_context_accept != NULL &&
       msg                               != NULL)
    {
        // Protocol Discriminator and EPS Bearer ID
        *msg_ptr = (act_def_eps_bearer_context_accept->eps_bearer_id << 4) | (LIBLTE_MME_PD_EPS_SESSION_MANAGEMENT);
        msg_ptr++;

        // Procedure Transaction ID
        *msg_ptr = act_def_eps_bearer_context_accept->proc_transaction_id;
        msg_ptr++;

        // Message Type
        *msg_ptr = LIBLTE_MME_MSG_TYPE_ACTIVATE_DEFAULT_EPS_BEARER_CONTEXT_ACCEPT;
        msg_ptr++;

        // Protocol Configuration Options
        if(act_def_eps_bearer_context_accept->protocol_cnfg_opts_present)
        {
            *msg_ptr = LIBLTE_MME_PROTOCOL_CONFIGURATION_OPTIONS_IEI;
            msg_ptr++;
            liblte_mme_pack_protocol_config_options_ie(&act_def_eps_bearer_context_accept->protocol_cnfg_opts, &msg_ptr);
        }

        // Fill in the number of bytes used
        msg->N_bytes = msg_ptr - msg->msg;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_mme_unpack_activate_default_eps_bearer_context_accept_msg(LIBLTE_BYTE_MSG_STRUCT                                           *msg,
                                                                                   LIBLTE_MME_ACTIVATE_DEFAULT_EPS_BEARER_CONTEXT_ACCEPT_MSG_STRUCT *act_def_eps_bearer_context_accept)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr = msg->msg;

    if(msg                               != NULL &&
       act_def_eps_bearer_context_accept != NULL)
    {
        // EPS Bearer ID
        act_def_eps_bearer_context_accept->eps_bearer_id = (*msg_ptr >> 4);
        msg_ptr++;

        // Procedure Transaction ID
        act_def_eps_bearer_context_accept->proc_transaction_id = *msg_ptr;
        msg_ptr++;

        // Skip Message Type
        msg_ptr++;

        // Protocol Configuration Options
        if(LIBLTE_MME_PROTOCOL_CONFIGURATION_OPTIONS_IEI == *msg_ptr)
        {
            msg_ptr++;
            liblte_mme_unpack_protocol_config_options_ie(&msg_ptr, &act_def_eps_bearer_context_accept->protocol_cnfg_opts);
            act_def_eps_bearer_context_accept->protocol_cnfg_opts_present = true;
        }else{
            act_def_eps_bearer_context_accept->protocol_cnfg_opts_present = false;
        }

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    Message Name: Activate Default EPS Bearer Context Reject

    Description: Sent by the UE to the network to reject activation
                 of a default EPS bearer context.

    Document Reference: 24.301 v10.2.0 Section 8.3.5
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_mme_pack_activate_default_eps_bearer_context_reject_msg(LIBLTE_MME_ACTIVATE_DEFAULT_EPS_BEARER_CONTEXT_REJECT_MSG_STRUCT *act_def_eps_bearer_context_rej,
                                                                                 LIBLTE_BYTE_MSG_STRUCT                                           *msg)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr = msg->msg;

    if(act_def_eps_bearer_context_rej != NULL &&
       msg                            != NULL)
    {
        // Protocol Discriminator and EPS Bearer ID
        *msg_ptr = (act_def_eps_bearer_context_rej->eps_bearer_id << 4) | (LIBLTE_MME_PD_EPS_SESSION_MANAGEMENT);
        msg_ptr++;

        // Procedure Transaction ID
        *msg_ptr = act_def_eps_bearer_context_rej->proc_transaction_id;
        msg_ptr++;

        // Message Type
        *msg_ptr = LIBLTE_MME_MSG_TYPE_ACTIVATE_DEFAULT_EPS_BEARER_CONTEXT_REJECT;
        msg_ptr++;

        // ESM Cause
        liblte_mme_pack_esm_cause_ie(act_def_eps_bearer_context_rej->esm_cause, &msg_ptr);

        // Protocol Configuration Options
        if(act_def_eps_bearer_context_rej->protocol_cnfg_opts_present)
        {
            *msg_ptr = LIBLTE_MME_PROTOCOL_CONFIGURATION_OPTIONS_IEI;
            msg_ptr++;
            liblte_mme_pack_protocol_config_options_ie(&act_def_eps_bearer_context_rej->protocol_cnfg_opts, &msg_ptr);
        }

        // Fill in the number of bytes used
        msg->N_bytes = msg_ptr - msg->msg;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_mme_unpack_activate_default_eps_bearer_context_reject_msg(LIBLTE_BYTE_MSG_STRUCT                                           *msg,
                                                                                   LIBLTE_MME_ACTIVATE_DEFAULT_EPS_BEARER_CONTEXT_REJECT_MSG_STRUCT *act_def_eps_bearer_context_rej)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr = msg->msg;

    if(msg                            != NULL &&
       act_def_eps_bearer_context_rej != NULL)
    {
        // EPS Bearer ID
        act_def_eps_bearer_context_rej->eps_bearer_id = (*msg_ptr >> 4);
        msg_ptr++;

        // Procedure Transaction ID
        act_def_eps_bearer_context_rej->proc_transaction_id = *msg_ptr;
        msg_ptr++;

        // Skip Message Type
        msg_ptr++;

        // ESM Cause
        liblte_mme_unpack_esm_cause_ie(&msg_ptr, &act_def_eps_bearer_context_rej->esm_cause);

        // Protocol Configuration Options
        if(LIBLTE_MME_PROTOCOL_CONFIGURATION_OPTIONS_IEI == *msg_ptr)
        {
            msg_ptr++;
            liblte_mme_unpack_protocol_config_options_ie(&msg_ptr, &act_def_eps_bearer_context_rej->protocol_cnfg_opts);
            act_def_eps_bearer_context_rej->protocol_cnfg_opts_present = true;
        }else{
            act_def_eps_bearer_context_rej->protocol_cnfg_opts_present = false;
        }

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    Message Name: Activate Default EPS Bearer Context Request

    Description: Sent by the network to the UE to request activation
                 of a default EPS bearer context.

    Document Reference: 24.301 v10.2.0 Section 8.3.6
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_mme_pack_activate_default_eps_bearer_context_request_msg(LIBLTE_MME_ACTIVATE_DEFAULT_EPS_BEARER_CONTEXT_REQUEST_MSG_STRUCT *act_def_eps_bearer_context_req,
                                                                                  LIBLTE_BYTE_MSG_STRUCT                                            *msg)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr = msg->msg;

    if(act_def_eps_bearer_context_req != NULL &&
       msg                            != NULL)
    {
        // Protocol Discriminator and EPS Bearer ID
        *msg_ptr = (act_def_eps_bearer_context_req->eps_bearer_id << 4) | (LIBLTE_MME_PD_EPS_SESSION_MANAGEMENT);
        msg_ptr++;

        // Procedure Transaction ID
        *msg_ptr = act_def_eps_bearer_context_req->proc_transaction_id;
        msg_ptr++;

        // Message Type
        *msg_ptr = LIBLTE_MME_MSG_TYPE_ACTIVATE_DEFAULT_EPS_BEARER_CONTEXT_REQUEST;
        msg_ptr++;

        // EPS QoS
        liblte_mme_pack_eps_quality_of_service_ie(&act_def_eps_bearer_context_req->eps_qos, &msg_ptr);

        // Access Point Name
        liblte_mme_pack_access_point_name_ie(&act_def_eps_bearer_context_req->apn, &msg_ptr);

        // PDN Address
        liblte_mme_pack_pdn_address_ie(&act_def_eps_bearer_context_req->pdn_addr, &msg_ptr);

        // Transaction Identifier
        if(act_def_eps_bearer_context_req->transaction_id_present)
        {
            *msg_ptr = LIBLTE_MME_TRANSACTION_IDENTIFIER_IEI;
            msg_ptr++;
            liblte_mme_pack_transaction_identifier_ie(&act_def_eps_bearer_context_req->transaction_id, &msg_ptr);
        }

        // Negotiated QoS
        if(act_def_eps_bearer_context_req->negotiated_qos_present)
        {
            *msg_ptr = LIBLTE_MME_QUALITY_OF_SERVICE_IEI;
            msg_ptr++;
            liblte_mme_pack_quality_of_service_ie(&act_def_eps_bearer_context_req->negotiated_qos, &msg_ptr);
        }

        // Negotiated LLC SAPI
        if(act_def_eps_bearer_context_req->llc_sapi_present)
        {
            *msg_ptr = LIBLTE_MME_LLC_SAPI_IEI;
            msg_ptr++;
            liblte_mme_pack_llc_service_access_point_identifier_ie(act_def_eps_bearer_context_req->llc_sapi, &msg_ptr);
        }

        // Radio Priority
        if(act_def_eps_bearer_context_req->radio_prio_present)
        {
            *msg_ptr = LIBLTE_MME_RADIO_PRIORITY_IEI << 4;
            liblte_mme_pack_radio_priority_ie(act_def_eps_bearer_context_req->radio_prio, 0, &msg_ptr);
            msg_ptr++;
        }

        // Packet Flow Identifier
        if(act_def_eps_bearer_context_req->packet_flow_id_present)
        {
            *msg_ptr = LIBLTE_MME_PACKET_FLOW_IDENTIFIER_IEI;
            msg_ptr++;
            liblte_mme_pack_packet_flow_identifier_ie(act_def_eps_bearer_context_req->packet_flow_id, &msg_ptr);
        }

        // APN-AMBR
        if(act_def_eps_bearer_context_req->apn_ambr_present)
        {
            *msg_ptr = LIBLTE_MME_APN_AMBR_IEI;
            msg_ptr++;
            liblte_mme_pack_apn_aggregate_maximum_bit_rate_ie(&act_def_eps_bearer_context_req->apn_ambr, &msg_ptr);
        }

        // ESM Cause
        if(act_def_eps_bearer_context_req->esm_cause_present)
        {
            *msg_ptr = LIBLTE_MME_ESM_CAUSE_IEI;
            msg_ptr++;
            liblte_mme_pack_esm_cause_ie(act_def_eps_bearer_context_req->esm_cause, &msg_ptr);
        }

        // Protocol Configuration Options
        if(act_def_eps_bearer_context_req->protocol_cnfg_opts_present)
        {
            *msg_ptr = LIBLTE_MME_PROTOCOL_CONFIGURATION_OPTIONS_IEI;
            msg_ptr++;
            liblte_mme_pack_protocol_config_options_ie(&act_def_eps_bearer_context_req->protocol_cnfg_opts, &msg_ptr);
        }

        // Connectivity Type
        if(act_def_eps_bearer_context_req->connectivity_type_present)
        {
            *msg_ptr = LIBLTE_MME_CONNECTIVITY_TYPE_IEI << 4;
            liblte_mme_pack_connectivity_type_ie(act_def_eps_bearer_context_req->connectivity_type, 0, &msg_ptr);
            msg_ptr++;
        }

        // Fill in the number of bytes used
        msg->N_bytes = msg_ptr - msg->msg;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_mme_unpack_activate_default_eps_bearer_context_request_msg(LIBLTE_BYTE_MSG_STRUCT                                            *msg,
                                                                                    LIBLTE_MME_ACTIVATE_DEFAULT_EPS_BEARER_CONTEXT_REQUEST_MSG_STRUCT *act_def_eps_bearer_context_req)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr = msg->msg;

    if(msg                            != NULL &&
       act_def_eps_bearer_context_req != NULL)
    {
        // EPS Bearer ID
        act_def_eps_bearer_context_req->eps_bearer_id = (*msg_ptr >> 4);
        msg_ptr++;

        // Procedure Transaction ID
        act_def_eps_bearer_context_req->proc_transaction_id = *msg_ptr;
        msg_ptr++;

        // Skip Message Type
        msg_ptr++;

        // EPS QoS
        liblte_mme_unpack_eps_quality_of_service_ie(&msg_ptr, &act_def_eps_bearer_context_req->eps_qos);

        // Access Point Name
        liblte_mme_unpack_access_point_name_ie(&msg_ptr, &act_def_eps_bearer_context_req->apn);

        // PDN Address
        liblte_mme_unpack_pdn_address_ie(&msg_ptr, &act_def_eps_bearer_context_req->pdn_addr);

        // Transaction Identifier
        if(LIBLTE_MME_TRANSACTION_IDENTIFIER_IEI == *msg_ptr)
        {
            msg_ptr++;
            liblte_mme_unpack_transaction_identifier_ie(&msg_ptr, &act_def_eps_bearer_context_req->transaction_id);
            act_def_eps_bearer_context_req->transaction_id_present = true;
        }else{
            act_def_eps_bearer_context_req->transaction_id_present = false;
        }

        // Negotiated QoS
        if(LIBLTE_MME_QUALITY_OF_SERVICE_IEI == *msg_ptr)
        {
            msg_ptr++;
            liblte_mme_unpack_quality_of_service_ie(&msg_ptr, &act_def_eps_bearer_context_req->negotiated_qos);
            act_def_eps_bearer_context_req->negotiated_qos_present = true;
        }else{
            act_def_eps_bearer_context_req->negotiated_qos_present = false;
        }

        // Negotiated LLC SAPI
        if(LIBLTE_MME_LLC_SAPI_IEI == *msg_ptr)
        {
            msg_ptr++;
            liblte_mme_unpack_llc_service_access_point_identifier_ie(&msg_ptr, &act_def_eps_bearer_context_req->llc_sapi);
            act_def_eps_bearer_context_req->llc_sapi_present = true;
        }else{
            act_def_eps_bearer_context_req->llc_sapi_present = false;
        }

        // Radio Priority
        if((LIBLTE_MME_RADIO_PRIORITY_IEI << 4) == (*msg_ptr & 0xF0))
        {
            liblte_mme_unpack_radio_priority_ie(&msg_ptr, 0, &act_def_eps_bearer_context_req->radio_prio);
            msg_ptr++;
            act_def_eps_bearer_context_req->radio_prio_present = true;
        }else{
            act_def_eps_bearer_context_req->radio_prio_present = false;
        }

        // Packet Flow Identifier
        if(LIBLTE_MME_PACKET_FLOW_IDENTIFIER_IEI == *msg_ptr)
        {
            msg_ptr++;
            liblte_mme_unpack_packet_flow_identifier_ie(&msg_ptr, &act_def_eps_bearer_context_req->packet_flow_id);
            act_def_eps_bearer_context_req->packet_flow_id_present = true;
        }else{
            act_def_eps_bearer_context_req->packet_flow_id_present = false;
        }

        // APN-AMBR
        if(LIBLTE_MME_APN_AMBR_IEI == *msg_ptr)
        {
            msg_ptr++;
            liblte_mme_unpack_apn_aggregate_maximum_bit_rate_ie(&msg_ptr, &act_def_eps_bearer_context_req->apn_ambr);
            act_def_eps_bearer_context_req->apn_ambr_present = true;
        }else{
            act_def_eps_bearer_context_req->apn_ambr_present = false;
        }

        // ESM Cause
        if(LIBLTE_MME_ESM_CAUSE_IEI == *msg_ptr)
        {
            msg_ptr++;
            liblte_mme_unpack_esm_cause_ie(&msg_ptr, &act_def_eps_bearer_context_req->esm_cause);
            act_def_eps_bearer_context_req->esm_cause_present = true;
        }else{
            act_def_eps_bearer_context_req->esm_cause_present = false;
        }

        // Protocol Configuration Options
        if(LIBLTE_MME_PROTOCOL_CONFIGURATION_OPTIONS_IEI == *msg_ptr)
        {
            msg_ptr++;
            liblte_mme_unpack_protocol_config_options_ie(&msg_ptr, &act_def_eps_bearer_context_req->protocol_cnfg_opts);
            act_def_eps_bearer_context_req->protocol_cnfg_opts_present = true;
        }else{
            act_def_eps_bearer_context_req->protocol_cnfg_opts_present = false;
        }

        // Connectivity Type
        if((LIBLTE_MME_CONNECTIVITY_TYPE_IEI << 4) == (*msg_ptr & 0xF0))
        {
            liblte_mme_unpack_connectivity_type_ie(&msg_ptr, 0, &act_def_eps_bearer_context_req->connectivity_type);
            msg_ptr++;
            act_def_eps_bearer_context_req->connectivity_type_present = true;
        }else{
            act_def_eps_bearer_context_req->connectivity_type_present = false;
        }

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    Message Name: Bearer Resource Allocation Reject

    Description: Sent by the network to the UE to reject the
                 allocation of a dedicated bearer resource.

    Document Reference: 24.301 v10.2.0 Section 8.3.7
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_mme_pack_bearer_resource_allocation_reject_msg(LIBLTE_MME_BEARER_RESOURCE_ALLOCATION_REJECT_MSG_STRUCT *bearer_res_alloc_rej,
                                                                        LIBLTE_BYTE_MSG_STRUCT                                  *msg)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr = msg->msg;

    if(bearer_res_alloc_rej != NULL &&
       msg                  != NULL)
    {
        // Protocol Discriminator and EPS Bearer ID
        *msg_ptr = (bearer_res_alloc_rej->eps_bearer_id << 4) | (LIBLTE_MME_PD_EPS_SESSION_MANAGEMENT);
        msg_ptr++;

        // Procedure Transaction ID
        *msg_ptr = bearer_res_alloc_rej->proc_transaction_id;
        msg_ptr++;

        // Message Type
        *msg_ptr = LIBLTE_MME_MSG_TYPE_BEARER_RESOURCE_ALLOCATION_REJECT;
        msg_ptr++;

        // ESM Cause
        liblte_mme_pack_esm_cause_ie(bearer_res_alloc_rej->esm_cause, &msg_ptr);

        // Protocol Configuration Options
        if(bearer_res_alloc_rej->protocol_cnfg_opts_present)
        {
            *msg_ptr = LIBLTE_MME_PROTOCOL_CONFIGURATION_OPTIONS_IEI;
            msg_ptr++;
            liblte_mme_pack_protocol_config_options_ie(&bearer_res_alloc_rej->protocol_cnfg_opts, &msg_ptr);
        }

        // T3496 Value
        if(bearer_res_alloc_rej->t3496_present)
        {
            *msg_ptr = LIBLTE_MME_T3496_VALUE_IEI;
            msg_ptr++;
            liblte_mme_pack_gprs_timer_3_ie(&bearer_res_alloc_rej->t3496, &msg_ptr);
        }

        // Fill in the number of bytes used
        msg->N_bytes = msg_ptr - msg->msg;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_mme_unpack_bearer_resource_allocation_reject_msg(LIBLTE_BYTE_MSG_STRUCT                                  *msg,
                                                                          LIBLTE_MME_BEARER_RESOURCE_ALLOCATION_REJECT_MSG_STRUCT *bearer_res_alloc_rej)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr = msg->msg;

    if(msg                  != NULL &&
       bearer_res_alloc_rej != NULL)
    {
        // EPS Bearer ID
        bearer_res_alloc_rej->eps_bearer_id = (*msg_ptr >> 4);
        msg_ptr++;

        // Procedure Transaction ID
        bearer_res_alloc_rej->proc_transaction_id = *msg_ptr;
        msg_ptr++;

        // Skip Message Type
        msg_ptr++;

        // ESM Cause
        liblte_mme_unpack_esm_cause_ie(&msg_ptr, &bearer_res_alloc_rej->esm_cause);

        // Protocol Configuration Options
        if(LIBLTE_MME_PROTOCOL_CONFIGURATION_OPTIONS_IEI == *msg_ptr)
        {
            msg_ptr++;
            liblte_mme_unpack_protocol_config_options_ie(&msg_ptr, &bearer_res_alloc_rej->protocol_cnfg_opts);
            bearer_res_alloc_rej->protocol_cnfg_opts_present = true;
        }else{
            bearer_res_alloc_rej->protocol_cnfg_opts_present = false;
        }

        // T3496 Value
        if(LIBLTE_MME_T3496_VALUE_IEI == *msg_ptr)
        {
            msg_ptr++;
            liblte_mme_unpack_gprs_timer_3_ie(&msg_ptr, &bearer_res_alloc_rej->t3496);
            bearer_res_alloc_rej->t3496_present = true;
        }else{
            bearer_res_alloc_rej->t3496_present = false;
        }

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    Message Name: Bearer Resource Allocation Request

    Description: Sent by the UE to the network to request the
                 allocation of a dedicated bearer resource.

    Document Reference: 24.301 v10.2.0 Section 8.3.8
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_mme_pack_bearer_resource_allocation_request_msg(LIBLTE_MME_BEARER_RESOURCE_ALLOCATION_REQUEST_MSG_STRUCT *bearer_res_alloc_req,
                                                                         LIBLTE_BYTE_MSG_STRUCT                                   *msg)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr = msg->msg;

    if(bearer_res_alloc_req != NULL &&
       msg                  != NULL)
    {
        // Protocol Discriminator and EPS Bearer ID
        *msg_ptr = (bearer_res_alloc_req->eps_bearer_id << 4) | (LIBLTE_MME_PD_EPS_SESSION_MANAGEMENT);
        msg_ptr++;

        // Procedure Transaction ID
        *msg_ptr = bearer_res_alloc_req->proc_transaction_id;
        msg_ptr++;

        // Message Type
        *msg_ptr = LIBLTE_MME_MSG_TYPE_BEARER_RESOURCE_ALLOCATION_REQUEST;
        msg_ptr++;

        // Linked EPS Bearer Identity & Spare Half Octet
        liblte_mme_pack_linked_eps_bearer_identity_ie(bearer_res_alloc_req->linked_eps_bearer_id, 0, &msg_ptr);

        // Traffic Flow Aggregate
        liblte_mme_pack_traffic_flow_aggregate_description_ie(&bearer_res_alloc_req->tfa, &msg_ptr);

        // Required Traffic Flow QoS
        liblte_mme_pack_eps_quality_of_service_ie(&bearer_res_alloc_req->req_tf_qos, &msg_ptr);

        // Protocol Configuration Options
        if(bearer_res_alloc_req->protocol_cnfg_opts_present)
        {
            *msg_ptr = LIBLTE_MME_PROTOCOL_CONFIGURATION_OPTIONS_IEI;
            msg_ptr++;
            liblte_mme_pack_protocol_config_options_ie(&bearer_res_alloc_req->protocol_cnfg_opts, &msg_ptr);
        }

        // Device Properties
        if(bearer_res_alloc_req->device_properties_present)
        {
            *msg_ptr = LIBLTE_MME_BEARER_RESOURCE_ALLOCATION_REQUEST_DEVICE_PROPERTIES_IEI << 4;
            liblte_mme_pack_device_properties_ie(bearer_res_alloc_req->device_properties, 0, &msg_ptr);
            msg_ptr++;
        }

        // Fill in the number of bytes used
        msg->N_bytes = msg_ptr - msg->msg;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_mme_unpack_bearer_resource_allocation_request_msg(LIBLTE_BYTE_MSG_STRUCT                                   *msg,
                                                                           LIBLTE_MME_BEARER_RESOURCE_ALLOCATION_REQUEST_MSG_STRUCT *bearer_res_alloc_req)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr = msg->msg;

    if(msg                  != NULL &&
       bearer_res_alloc_req != NULL)
    {
        // EPS Bearer ID
        bearer_res_alloc_req->eps_bearer_id = (*msg_ptr >> 4);
        msg_ptr++;

        // Procedure Transaction ID
        bearer_res_alloc_req->proc_transaction_id = *msg_ptr;
        msg_ptr++;

        // Skip Message Type
        msg_ptr++;

        // Linked EPS Bearer Identity & Spare Half Octet
        liblte_mme_unpack_linked_eps_bearer_identity_ie(&msg_ptr, 0, &bearer_res_alloc_req->linked_eps_bearer_id);

        // Traffic Flow Aggregate
        liblte_mme_unpack_traffic_flow_aggregate_description_ie(&msg_ptr, &bearer_res_alloc_req->tfa);

        // Required Traffic Flow QoS
        liblte_mme_unpack_eps_quality_of_service_ie(&msg_ptr, &bearer_res_alloc_req->req_tf_qos);

        // Protocol Configuration Options
        if(LIBLTE_MME_PROTOCOL_CONFIGURATION_OPTIONS_IEI == *msg_ptr)
        {
            msg_ptr++;
            liblte_mme_unpack_protocol_config_options_ie(&msg_ptr, &bearer_res_alloc_req->protocol_cnfg_opts);
            bearer_res_alloc_req->protocol_cnfg_opts_present = true;
        }else{
            bearer_res_alloc_req->protocol_cnfg_opts_present = false;
        }

        // Device Properties
        if((LIBLTE_MME_BEARER_RESOURCE_ALLOCATION_REQUEST_DEVICE_PROPERTIES_IEI << 4) == *msg_ptr)
        {
            liblte_mme_unpack_device_properties_ie(&msg_ptr, 0, &bearer_res_alloc_req->device_properties);
            msg_ptr++;
            bearer_res_alloc_req->device_properties_present = true;
        }else{
            bearer_res_alloc_req->device_properties_present = false;
        }

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    Message Name: Bearer Resource Modification Reject

    Description: Sent by the network to the UE to reject the
                 modification of a dedicated bearer resource.

    Document Reference: 24.301 v10.2.0 Section 8.3.9
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_mme_pack_bearer_resource_modification_reject_msg(LIBLTE_MME_BEARER_RESOURCE_MODIFICATION_REJECT_MSG_STRUCT *bearer_res_mod_rej,
                                                                          LIBLTE_BYTE_MSG_STRUCT                                    *msg)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr = msg->msg;

    if(bearer_res_mod_rej != NULL &&
       msg                != NULL)
    {
        // Protocol Discriminator and EPS Bearer ID
        *msg_ptr = (bearer_res_mod_rej->eps_bearer_id << 4) | (LIBLTE_MME_PD_EPS_SESSION_MANAGEMENT);
        msg_ptr++;

        // Procedure Transaction ID
        *msg_ptr = bearer_res_mod_rej->proc_transaction_id;
        msg_ptr++;

        // Message Type
        *msg_ptr = LIBLTE_MME_MSG_TYPE_BEARER_RESOURCE_MODIFICATION_REJECT;
        msg_ptr++;

        // ESM Cause
        liblte_mme_pack_esm_cause_ie(bearer_res_mod_rej->esm_cause, &msg_ptr);

        // Protocol Configuration Options
        if(bearer_res_mod_rej->protocol_cnfg_opts_present)
        {
            *msg_ptr = LIBLTE_MME_PROTOCOL_CONFIGURATION_OPTIONS_IEI;
            msg_ptr++;
            liblte_mme_pack_protocol_config_options_ie(&bearer_res_mod_rej->protocol_cnfg_opts, &msg_ptr);
        }

        // T3496 Value
        if(bearer_res_mod_rej->t3496_present)
        {
            *msg_ptr = LIBLTE_MME_T3496_VALUE_IEI;
            msg_ptr++;
            liblte_mme_pack_gprs_timer_3_ie(&bearer_res_mod_rej->t3496, &msg_ptr);
        }

        // Fill in the number of bytes used
        msg->N_bytes = msg_ptr - msg->msg;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_mme_unpack_bearer_resource_modification_reject_msg(LIBLTE_BYTE_MSG_STRUCT                                    *msg,
                                                                            LIBLTE_MME_BEARER_RESOURCE_MODIFICATION_REJECT_MSG_STRUCT *bearer_res_mod_rej)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr = msg->msg;

    if(msg                != NULL &&
       bearer_res_mod_rej != NULL)
    {
        // EPS Bearer ID
        bearer_res_mod_rej->eps_bearer_id = (*msg_ptr >> 4);
        msg_ptr++;

        // Procedure Transaction ID
        bearer_res_mod_rej->proc_transaction_id = *msg_ptr;
        msg_ptr++;

        // Skip Message Type
        msg_ptr++;

        // ESM Cause
        liblte_mme_unpack_esm_cause_ie(&msg_ptr, &bearer_res_mod_rej->esm_cause);

        // Protocol Configuration Options
        if(LIBLTE_MME_PROTOCOL_CONFIGURATION_OPTIONS_IEI == *msg_ptr)
        {
            msg_ptr++;
            liblte_mme_unpack_protocol_config_options_ie(&msg_ptr, &bearer_res_mod_rej->protocol_cnfg_opts);
            bearer_res_mod_rej->protocol_cnfg_opts_present = true;
        }else{
            bearer_res_mod_rej->protocol_cnfg_opts_present = false;
        }

        // T3496 Value
        if(LIBLTE_MME_T3496_VALUE_IEI == *msg_ptr)
        {
            msg_ptr++;
            liblte_mme_unpack_gprs_timer_3_ie(&msg_ptr, &bearer_res_mod_rej->t3496);
            bearer_res_mod_rej->t3496_present = true;
        }else{
            bearer_res_mod_rej->t3496_present = false;
        }

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    Message Name: Bearer Resource Modification Request

    Description: Sent by the UE to the network to request the
                 modification of a dedicated bearer resource.

    Document Reference: 24.301 v10.2.0 Section 8.3.10
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_mme_pack_bearer_resource_modification_request_msg(LIBLTE_MME_BEARER_RESOURCE_MODIFICATION_REQUEST_MSG_STRUCT *bearer_res_mod_req,
                                                                           LIBLTE_BYTE_MSG_STRUCT                                     *msg)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr = msg->msg;

    if(bearer_res_mod_req != NULL &&
       msg                != NULL)
    {
        // Protocol Discriminator and EPS Bearer ID
        *msg_ptr = (bearer_res_mod_req->eps_bearer_id << 4) | (LIBLTE_MME_PD_EPS_SESSION_MANAGEMENT);
        msg_ptr++;

        // Procedure Transaction ID
        *msg_ptr = bearer_res_mod_req->proc_transaction_id;
        msg_ptr++;

        // Message Type
        *msg_ptr = LIBLTE_MME_MSG_TYPE_BEARER_RESOURCE_MODIFICATION_REQUEST;
        msg_ptr++;

        // EPS Bearer Identity For Packet Filter & Spare Half Octet
        liblte_mme_pack_linked_eps_bearer_identity_ie(bearer_res_mod_req->eps_bearer_id_for_packet_filter, 0, &msg_ptr);

        // Traffic Flow Aggregate
        liblte_mme_pack_traffic_flow_aggregate_description_ie(&bearer_res_mod_req->tfa, &msg_ptr);

        // Required Traffic Flow QoS
        if(bearer_res_mod_req->req_tf_qos_present)
        {
            *msg_ptr = LIBLTE_MME_EPS_QUALITY_OF_SERVICE_IEI;
            msg_ptr++;
            liblte_mme_pack_eps_quality_of_service_ie(&bearer_res_mod_req->req_tf_qos, &msg_ptr);
        }

        // ESM Cause
        if(bearer_res_mod_req->esm_cause_present)
        {
            *msg_ptr = LIBLTE_MME_ESM_CAUSE_IEI;
            msg_ptr++;
            liblte_mme_pack_esm_cause_ie(bearer_res_mod_req->esm_cause, &msg_ptr);
        }

        // Protocol Configuration Options
        if(bearer_res_mod_req->protocol_cnfg_opts_present)
        {
            *msg_ptr = LIBLTE_MME_PROTOCOL_CONFIGURATION_OPTIONS_IEI;
            msg_ptr++;
            liblte_mme_pack_protocol_config_options_ie(&bearer_res_mod_req->protocol_cnfg_opts, &msg_ptr);
        }

        // Device Properties
        if(bearer_res_mod_req->device_properties_present)
        {
            *msg_ptr = LIBLTE_MME_BEARER_RESOURCE_MODIFICATION_REQUEST_DEVICE_PROPERTIES_IEI << 4;
            liblte_mme_pack_device_properties_ie(bearer_res_mod_req->device_properties, 0, &msg_ptr);
            msg_ptr++;
        }

        // Fill in the number of bytes used
        msg->N_bytes = msg_ptr - msg->msg;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_mme_unpack_bearer_resource_modification_request_msg(LIBLTE_BYTE_MSG_STRUCT                                     *msg,
                                                                             LIBLTE_MME_BEARER_RESOURCE_MODIFICATION_REQUEST_MSG_STRUCT *bearer_res_mod_req)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr = msg->msg;

    if(msg                != NULL &&
       bearer_res_mod_req != NULL)
    {
        // EPS Bearer ID
        bearer_res_mod_req->eps_bearer_id = (*msg_ptr >> 4);
        msg_ptr++;

        // Procedure Transaction ID
        bearer_res_mod_req->proc_transaction_id = *msg_ptr;
        msg_ptr++;

        // Skip Message Type
        msg_ptr++;

        // EPS Bearer Identity For Packet Filter & Spare Half Octet
        liblte_mme_unpack_linked_eps_bearer_identity_ie(&msg_ptr, 0, &bearer_res_mod_req->eps_bearer_id_for_packet_filter);

        // Traffic Flow Aggregate
        liblte_mme_unpack_traffic_flow_aggregate_description_ie(&msg_ptr, &bearer_res_mod_req->tfa);

        // Required Traffic Flow QoS
        if(LIBLTE_MME_EPS_QUALITY_OF_SERVICE_IEI == *msg_ptr)
        {
            msg_ptr++;
            liblte_mme_unpack_eps_quality_of_service_ie(&msg_ptr, &bearer_res_mod_req->req_tf_qos);
            bearer_res_mod_req->req_tf_qos_present = true;
        }else{
            bearer_res_mod_req->req_tf_qos_present = false;
        }

        // ESM Cause
        if(LIBLTE_MME_ESM_CAUSE_IEI == *msg_ptr)
        {
            msg_ptr++;
            liblte_mme_unpack_esm_cause_ie(&msg_ptr, &bearer_res_mod_req->esm_cause);
            bearer_res_mod_req->esm_cause_present = true;
        }else{
            bearer_res_mod_req->esm_cause_present = false;
        }

        // Protocol Configuration Options
        if(LIBLTE_MME_PROTOCOL_CONFIGURATION_OPTIONS_IEI == *msg_ptr)
        {
            msg_ptr++;
            liblte_mme_unpack_protocol_config_options_ie(&msg_ptr, &bearer_res_mod_req->protocol_cnfg_opts);
            bearer_res_mod_req->protocol_cnfg_opts_present = true;
        }else{
            bearer_res_mod_req->protocol_cnfg_opts_present = false;
        }

        // Device Properties
        if((LIBLTE_MME_BEARER_RESOURCE_MODIFICATION_REQUEST_DEVICE_PROPERTIES_IEI << 4) == *msg_ptr)
        {
            liblte_mme_unpack_device_properties_ie(&msg_ptr, 0, &bearer_res_mod_req->device_properties);
            msg_ptr++;
            bearer_res_mod_req->device_properties_present = true;
        }else{
            bearer_res_mod_req->device_properties_present = false;
        }

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    Message Name: Deactivate EPS Bearer Context Accept

    Description: Sent by the UE to acknowledge deactivation of the
                 EPS bearer context requested in the corresponding
                 deactivate EPS bearer context request message.

    Document Reference: 24.301 v10.2.0 Section 8.3.11
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_mme_pack_deactivate_eps_bearer_context_accept_msg(LIBLTE_MME_DEACTIVATE_EPS_BEARER_CONTEXT_ACCEPT_MSG_STRUCT *deact_eps_bearer_context_accept,
                                                                           LIBLTE_BYTE_MSG_STRUCT                                     *msg)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr = msg->msg;

    if(deact_eps_bearer_context_accept != NULL &&
       msg                             != NULL)
    {
        // Protocol Discriminator and EPS Bearer ID
        *msg_ptr = (deact_eps_bearer_context_accept->eps_bearer_id << 4) | (LIBLTE_MME_PD_EPS_SESSION_MANAGEMENT);
        msg_ptr++;

        // Procedure Transaction ID
        *msg_ptr = deact_eps_bearer_context_accept->proc_transaction_id;
        msg_ptr++;

        // Message Type
        *msg_ptr = LIBLTE_MME_MSG_TYPE_DEACTIVATE_EPS_BEARER_CONTEXT_ACCEPT;
        msg_ptr++;

        // Protocol Configuration Options
        if(deact_eps_bearer_context_accept->protocol_cnfg_opts_present)
        {
            *msg_ptr = LIBLTE_MME_PROTOCOL_CONFIGURATION_OPTIONS_IEI;
            msg_ptr++;
            liblte_mme_pack_protocol_config_options_ie(&deact_eps_bearer_context_accept->protocol_cnfg_opts, &msg_ptr);
        }

        // Fill in the number of bytes used
        msg->N_bytes = msg_ptr - msg->msg;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_mme_unpack_deactivate_eps_bearer_context_accept_msg(LIBLTE_BYTE_MSG_STRUCT                                     *msg,
                                                                             LIBLTE_MME_DEACTIVATE_EPS_BEARER_CONTEXT_ACCEPT_MSG_STRUCT *deact_eps_bearer_context_accept)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr = msg->msg;

    if(msg                             != NULL &&
       deact_eps_bearer_context_accept != NULL)
    {
        // EPS Bearer ID
        deact_eps_bearer_context_accept->eps_bearer_id = (*msg_ptr >> 4);
        msg_ptr++;

        // Procedure Transaction ID
        deact_eps_bearer_context_accept->proc_transaction_id = *msg_ptr;
        msg_ptr++;

        // Skip Message Type
        msg_ptr++;

        // Protocol Configuration Options
        if(LIBLTE_MME_PROTOCOL_CONFIGURATION_OPTIONS_IEI == *msg_ptr)
        {
            msg_ptr++;
            liblte_mme_unpack_protocol_config_options_ie(&msg_ptr, &deact_eps_bearer_context_accept->protocol_cnfg_opts);
            deact_eps_bearer_context_accept->protocol_cnfg_opts_present = true;
        }else{
            deact_eps_bearer_context_accept->protocol_cnfg_opts_present = false;
        }

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    Message Name: Deactivate EPS Bearer Context Request

    Description: Sent by the network to request deactivation of an
                 EPS bearer context.

    Document Reference: 24.301 v10.2.0 Section 8.3.12
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_mme_pack_deactivate_eps_bearer_context_request_msg(LIBLTE_MME_DEACTIVATE_EPS_BEARER_CONTEXT_REQUEST_MSG_STRUCT *deact_eps_bearer_context_req,
                                                                            LIBLTE_BYTE_MSG_STRUCT                                      *msg)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr = msg->msg;

    if(deact_eps_bearer_context_req != NULL &&
       msg                          != NULL)
    {
        // Protocol Discriminator and EPS Bearer ID
        *msg_ptr = (deact_eps_bearer_context_req->eps_bearer_id << 4) | (LIBLTE_MME_PD_EPS_SESSION_MANAGEMENT);
        msg_ptr++;

        // Procedure Transaction ID
        *msg_ptr = deact_eps_bearer_context_req->proc_transaction_id;
        msg_ptr++;

        // Message Type
        *msg_ptr = LIBLTE_MME_MSG_TYPE_DEACTIVATE_EPS_BEARER_CONTEXT_REQUEST;
        msg_ptr++;

        // ESM Cause
        liblte_mme_pack_esm_cause_ie(deact_eps_bearer_context_req->esm_cause, &msg_ptr);

        // Protocol Configuration Options
        if(deact_eps_bearer_context_req->protocol_cnfg_opts_present)
        {
            *msg_ptr = LIBLTE_MME_PROTOCOL_CONFIGURATION_OPTIONS_IEI;
            msg_ptr++;
            liblte_mme_pack_protocol_config_options_ie(&deact_eps_bearer_context_req->protocol_cnfg_opts, &msg_ptr);
        }

        // Fill in the number of bytes used
        msg->N_bytes = msg_ptr - msg->msg;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_mme_unpack_deactivate_eps_bearer_context_request_msg(LIBLTE_BYTE_MSG_STRUCT                                      *msg,
                                                                              LIBLTE_MME_DEACTIVATE_EPS_BEARER_CONTEXT_REQUEST_MSG_STRUCT *deact_eps_bearer_context_req)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr = msg->msg;

    if(msg                          != NULL &&
       deact_eps_bearer_context_req != NULL)
    {
        // EPS Bearer ID
        deact_eps_bearer_context_req->eps_bearer_id = (*msg_ptr >> 4);
        msg_ptr++;

        // Procedure Transaction ID
        deact_eps_bearer_context_req->proc_transaction_id = *msg_ptr;
        msg_ptr++;

        // Skip Message Type
        msg_ptr++;

        // ESM Cause
        liblte_mme_unpack_esm_cause_ie(&msg_ptr, &deact_eps_bearer_context_req->esm_cause);

        // Protocol Configuration Options
        if(LIBLTE_MME_PROTOCOL_CONFIGURATION_OPTIONS_IEI == *msg_ptr)
        {
            msg_ptr++;
            liblte_mme_unpack_protocol_config_options_ie(&msg_ptr, &deact_eps_bearer_context_req->protocol_cnfg_opts);
            deact_eps_bearer_context_req->protocol_cnfg_opts_present = true;
        }else{
            deact_eps_bearer_context_req->protocol_cnfg_opts_present = false;
        }

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    Message Name: ESM Information Request

    Description: Sent by the network to the UE to request the UE to
                 provide ESM information, i.e. protocol configuration
                 options or APN or both. This function is being added
                 to support encryption and integrety protection on 
                 ESM information transfer.

    Document Reference: 24.301 v10.2.0 Section 8.3.13
*********************************************************************/
LIBLTE_ERROR_ENUM srslte_mme_pack_esm_information_request_msg(LIBLTE_MME_ESM_INFORMATION_REQUEST_MSG_STRUCT *esm_info_req,
                                                              uint8                                         sec_hdr_type,
                                                              uint32                                        count,
                                                              LIBLTE_BYTE_MSG_STRUCT                        *msg)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr = msg->msg;

    if(esm_info_req != NULL &&
       msg          != NULL)
    {

       if(LIBLTE_MME_SECURITY_HDR_TYPE_PLAIN_NAS != sec_hdr_type)
       {
           // Protocol Discriminator and Security Header Type
           *msg_ptr = (sec_hdr_type << 4) | (LIBLTE_MME_PD_EPS_MOBILITY_MANAGEMENT);
           msg_ptr++;

           // MAC will be filled in later
           msg_ptr += 4;

           // Sequence Number
           *msg_ptr = count & 0xFF;
           msg_ptr++;
        }
        // Protocol Discriminator and EPS Bearer ID
        *msg_ptr = (esm_info_req->eps_bearer_id << 4) | (LIBLTE_MME_PD_EPS_SESSION_MANAGEMENT);
        msg_ptr++;

        // Procedure Transaction ID
        *msg_ptr = esm_info_req->proc_transaction_id;
        msg_ptr++;

        // Message Type
        *msg_ptr = LIBLTE_MME_MSG_TYPE_ESM_INFORMATION_REQUEST;
        msg_ptr++;

        // Fill in the number of bytes used
        msg->N_bytes = msg_ptr - msg->msg;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}


/*********************************************************************
    Message Name: ESM Information Request

    Description: Sent by the network to the UE to request the UE to
                 provide ESM information, i.e. protocol configuration
                 options or APN or both.

    Document Reference: 24.301 v10.2.0 Section 8.3.13
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_mme_pack_esm_information_request_msg(LIBLTE_MME_ESM_INFORMATION_REQUEST_MSG_STRUCT *esm_info_req,
                                                              LIBLTE_BYTE_MSG_STRUCT                        *msg)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr = msg->msg;

    if(esm_info_req != NULL &&
       msg          != NULL)
    {
      
        // Protocol Discriminator and EPS Bearer ID
        *msg_ptr = (esm_info_req->eps_bearer_id << 4) | (LIBLTE_MME_PD_EPS_SESSION_MANAGEMENT);
        msg_ptr++;

        // Procedure Transaction ID
        *msg_ptr = esm_info_req->proc_transaction_id;
        msg_ptr++;

        // Message Type
        *msg_ptr = LIBLTE_MME_MSG_TYPE_ESM_INFORMATION_REQUEST;
        msg_ptr++;

        // Fill in the number of bytes used
        msg->N_bytes = msg_ptr - msg->msg;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}



LIBLTE_ERROR_ENUM liblte_mme_unpack_esm_information_request_msg(LIBLTE_BYTE_MSG_STRUCT                        *msg,
                                                                LIBLTE_MME_ESM_INFORMATION_REQUEST_MSG_STRUCT *esm_info_req)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr = msg->msg;
    uint8              sec_hdr_type;

    if(msg          != NULL &&
       esm_info_req != NULL)
    {
        // Security Header Type
        sec_hdr_type = (msg->msg[0] & 0xF0) >> 4;
        if(LIBLTE_MME_SECURITY_HDR_TYPE_PLAIN_NAS == sec_hdr_type) {
            msg_ptr++;
        } else{
            msg_ptr += 6;
        }

        // EPS Bearer ID
        esm_info_req->eps_bearer_id = (*msg_ptr >> 4);
        msg_ptr++;

        // Procedure Transaction ID
        esm_info_req->proc_transaction_id = *msg_ptr;
        msg_ptr++;

        // Skip Message Type
        msg_ptr++;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    Message Name: ESM Information Response

    Description: Sent by the UE to the network in response to an ESM
                 INFORMATION REQUEST message and provides the
                 requested ESM information.

    Document Reference: 24.301 v10.2.0 Section 8.3.14
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_mme_pack_esm_information_response_msg(LIBLTE_MME_ESM_INFORMATION_RESPONSE_MSG_STRUCT *esm_info_resp,
                                                               uint8                                         sec_hdr_type,
                                                               uint32                                        count,
                                                               LIBLTE_BYTE_MSG_STRUCT                         *msg)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr = msg->msg;

    if(esm_info_resp != NULL &&
       msg           != NULL)
    {
        if(LIBLTE_MME_SECURITY_HDR_TYPE_PLAIN_NAS != sec_hdr_type)
        {
          // Protocol Discriminator and Security Header Type
          *msg_ptr = (sec_hdr_type << 4) | (LIBLTE_MME_PD_EPS_MOBILITY_MANAGEMENT);
          msg_ptr++;

          // MAC will be filled in later
          msg_ptr += 4;

          // Sequence Number
          *msg_ptr = count & 0xFF;
          msg_ptr++;
        }

        // Protocol Discriminator and EPS Bearer ID
        *msg_ptr = (esm_info_resp->eps_bearer_id << 4) | (LIBLTE_MME_PD_EPS_SESSION_MANAGEMENT);
        msg_ptr++;

        // Procedure Transaction ID
        *msg_ptr = esm_info_resp->proc_transaction_id;
        msg_ptr++;

        // Message Type
        *msg_ptr = LIBLTE_MME_MSG_TYPE_ESM_INFORMATION_RESPONSE;
        msg_ptr++;

        // Access Point Name
        if(esm_info_resp->apn_present)
        {
            *msg_ptr = LIBLTE_MME_ACCESS_POINT_NAME_IEI;
            msg_ptr++;
            liblte_mme_pack_access_point_name_ie(&esm_info_resp->apn, &msg_ptr);
        }

        // Protocol Configuration Options
        if(esm_info_resp->protocol_cnfg_opts_present)
        {
            *msg_ptr = LIBLTE_MME_PROTOCOL_CONFIGURATION_OPTIONS_IEI;
            msg_ptr++;
            liblte_mme_pack_protocol_config_options_ie(&esm_info_resp->protocol_cnfg_opts, &msg_ptr);
        }

        // Fill in the number of bytes used
        msg->N_bytes = msg_ptr - msg->msg;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}


LIBLTE_ERROR_ENUM srslte_mme_unpack_esm_information_response_msg(LIBLTE_BYTE_MSG_STRUCT                         *msg,
                                                                 LIBLTE_MME_ESM_INFORMATION_RESPONSE_MSG_STRUCT *esm_info_resp)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr = msg->msg;
    uint8              sec_hdr_type;

    if(msg           != NULL &&
       esm_info_resp != NULL)
    {
        
        // Security Header Type
        sec_hdr_type = (msg->msg[0] & 0xF0) >> 4;
        if(LIBLTE_MME_SECURITY_HDR_TYPE_PLAIN_NAS == sec_hdr_type)
        {
          msg_ptr++;
        }else{
          msg_ptr += 6;
        }
        // EPS Bearer ID
        esm_info_resp->eps_bearer_id = (*msg_ptr >> 4);
        msg_ptr++;

        // Procedure Transaction ID
        esm_info_resp->proc_transaction_id = *msg_ptr;
        msg_ptr++;

        // Skip Message Type
        msg_ptr++;

        // Access Point Name
        if(LIBLTE_MME_ACCESS_POINT_NAME_IEI == *msg_ptr)
        {
            msg_ptr++;
            liblte_mme_unpack_access_point_name_ie(&msg_ptr, &esm_info_resp->apn);
            esm_info_resp->apn_present = true;
        }else{
            esm_info_resp->apn_present = false;
        }

        // Protocol Configuration Options
        if(LIBLTE_MME_PROTOCOL_CONFIGURATION_OPTIONS_IEI == *msg_ptr)
        {
            msg_ptr++;
            liblte_mme_unpack_protocol_config_options_ie(&msg_ptr, &esm_info_resp->protocol_cnfg_opts);
            esm_info_resp->protocol_cnfg_opts_present = true;
        }else{
            esm_info_resp->protocol_cnfg_opts_present = false;
        }

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

LIBLTE_ERROR_ENUM liblte_mme_unpack_esm_information_response_msg(LIBLTE_BYTE_MSG_STRUCT                         *msg,
                                                                 LIBLTE_MME_ESM_INFORMATION_RESPONSE_MSG_STRUCT *esm_info_resp)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr = msg->msg;

    if(msg           != NULL &&
       esm_info_resp != NULL)
    {
        // EPS Bearer ID
        esm_info_resp->eps_bearer_id = (*msg_ptr >> 4);
        msg_ptr++;

        // Procedure Transaction ID
        esm_info_resp->proc_transaction_id = *msg_ptr;
        msg_ptr++;

        // Skip Message Type
        msg_ptr++;

        // Access Point Name
        if(LIBLTE_MME_ACCESS_POINT_NAME_IEI == *msg_ptr)
        {
            msg_ptr++;
            liblte_mme_unpack_access_point_name_ie(&msg_ptr, &esm_info_resp->apn);
            esm_info_resp->apn_present = true;
        }else{
            esm_info_resp->apn_present = false;
        }

        // Protocol Configuration Options
        if(LIBLTE_MME_PROTOCOL_CONFIGURATION_OPTIONS_IEI == *msg_ptr)
        {
            msg_ptr++;
            liblte_mme_unpack_protocol_config_options_ie(&msg_ptr, &esm_info_resp->protocol_cnfg_opts);
            esm_info_resp->protocol_cnfg_opts_present = true;
        }else{
            esm_info_resp->protocol_cnfg_opts_present = false;
        }

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    Message Name: ESM Status

    Description: Sent by the network or the UE to pass information on
                 the status of the indicated EPS bearer context and
                 report certain error conditions.

    Document Reference: 24.301 v10.2.0 Section 8.3.15
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_mme_pack_esm_status_msg(LIBLTE_MME_ESM_STATUS_MSG_STRUCT *esm_status,
                                                 LIBLTE_BYTE_MSG_STRUCT           *msg)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr = msg->msg;

    if(esm_status != NULL &&
       msg        != NULL)
    {
        // Protocol Discriminator and EPS Bearer ID
        *msg_ptr = (esm_status->eps_bearer_id << 4) | (LIBLTE_MME_PD_EPS_SESSION_MANAGEMENT);
        msg_ptr++;

        // Procedure Transaction ID
        *msg_ptr = esm_status->proc_transaction_id;
        msg_ptr++;

        // Message Type
        *msg_ptr = LIBLTE_MME_MSG_TYPE_ESM_STATUS;
        msg_ptr++;

        // ESM Cause
        liblte_mme_pack_esm_cause_ie(esm_status->esm_cause, &msg_ptr);

        // Fill in the number of bytes used
        msg->N_bytes = msg_ptr - msg->msg;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_mme_unpack_esm_status_msg(LIBLTE_BYTE_MSG_STRUCT           *msg,
                                                   LIBLTE_MME_ESM_STATUS_MSG_STRUCT *esm_status)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr = msg->msg;

    if(msg        != NULL &&
       esm_status != NULL)
    {
        // EPS Bearer ID
        esm_status->eps_bearer_id = (*msg_ptr >> 4);
        msg_ptr++;

        // Procedure Transaction ID
        esm_status->proc_transaction_id = *msg_ptr;
        msg_ptr++;

        // Skip Message Type
        msg_ptr++;

        // ESM Cause
        liblte_mme_unpack_esm_cause_ie(&msg_ptr, &esm_status->esm_cause);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    Message Name: Modify EPS Bearer Context Accept

    Description: Sent by the UE to the network to acknowledge the
                 modification of an active EPS bearer context.

    Document Reference: 24.301 v10.2.0 Section 8.3.16
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_mme_pack_modify_eps_bearer_context_accept_msg(LIBLTE_MME_MODIFY_EPS_BEARER_CONTEXT_ACCEPT_MSG_STRUCT *mod_eps_bearer_context_accept,
                                                                       LIBLTE_BYTE_MSG_STRUCT                                 *msg)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr = msg->msg;

    if(mod_eps_bearer_context_accept != NULL &&
       msg                           != NULL)
    {
        // Protocol Discriminator and EPS Bearer ID
        *msg_ptr = (mod_eps_bearer_context_accept->eps_bearer_id << 4) | (LIBLTE_MME_PD_EPS_SESSION_MANAGEMENT);
        msg_ptr++;

        // Procedure Transaction ID
        *msg_ptr = mod_eps_bearer_context_accept->proc_transaction_id;
        msg_ptr++;

        // Message Type
        *msg_ptr = LIBLTE_MME_MSG_TYPE_MODIFY_EPS_BEARER_CONTEXT_ACCEPT;
        msg_ptr++;

        // Protocol Configuration Options
        if(mod_eps_bearer_context_accept->protocol_cnfg_opts_present)
        {
            *msg_ptr = LIBLTE_MME_PROTOCOL_CONFIGURATION_OPTIONS_IEI;
            msg_ptr++;
            liblte_mme_pack_protocol_config_options_ie(&mod_eps_bearer_context_accept->protocol_cnfg_opts, &msg_ptr);
        }

        // Fill in the number of bytes used
        msg->N_bytes = msg_ptr - msg->msg;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_mme_unpack_modify_eps_bearer_context_accept_msg(LIBLTE_BYTE_MSG_STRUCT                                 *msg,
                                                                         LIBLTE_MME_MODIFY_EPS_BEARER_CONTEXT_ACCEPT_MSG_STRUCT *mod_eps_bearer_context_accept)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr = msg->msg;

    if(msg                           != NULL &&
       mod_eps_bearer_context_accept != NULL)
    {
        // EPS Bearer ID
        mod_eps_bearer_context_accept->eps_bearer_id = (*msg_ptr >> 4);
        msg_ptr++;

        // Procedure Transaction ID
        mod_eps_bearer_context_accept->proc_transaction_id = *msg_ptr;
        msg_ptr++;

        // Skip Message Type
        msg_ptr++;

        // Protocol Configuration Options
        if(LIBLTE_MME_PROTOCOL_CONFIGURATION_OPTIONS_IEI == *msg_ptr)
        {
            msg_ptr++;
            liblte_mme_unpack_protocol_config_options_ie(&msg_ptr, &mod_eps_bearer_context_accept->protocol_cnfg_opts);
            mod_eps_bearer_context_accept->protocol_cnfg_opts_present = true;
        }else{
            mod_eps_bearer_context_accept->protocol_cnfg_opts_present = false;
        }

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    Message Name: Modify EPS Bearer Context Reject

    Description: Sent by the UE or the network to reject a
                 modification of an active EPS bearer context.

    Document Reference: 24.301 v10.2.0 Section 8.3.17
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_mme_pack_modify_eps_bearer_context_reject_msg(LIBLTE_MME_MODIFY_EPS_BEARER_CONTEXT_REJECT_MSG_STRUCT *mod_eps_bearer_context_rej,
                                                                       LIBLTE_BYTE_MSG_STRUCT                                 *msg)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr = msg->msg;

    if(mod_eps_bearer_context_rej != NULL &&
       msg                        != NULL)
    {
        // Protocol Discriminator and EPS Bearer ID
        *msg_ptr = (mod_eps_bearer_context_rej->eps_bearer_id << 4) | (LIBLTE_MME_PD_EPS_SESSION_MANAGEMENT);
        msg_ptr++;

        // Procedure Transaction ID
        *msg_ptr = mod_eps_bearer_context_rej->proc_transaction_id;
        msg_ptr++;

        // Message Type
        *msg_ptr = LIBLTE_MME_MSG_TYPE_MODIFY_EPS_BEARER_CONTEXT_REJECT;
        msg_ptr++;

        // ESM Cause
        liblte_mme_pack_esm_cause_ie(mod_eps_bearer_context_rej->esm_cause, &msg_ptr);

        // Protocol Configuration Options
        if(mod_eps_bearer_context_rej->protocol_cnfg_opts_present)
        {
            *msg_ptr = LIBLTE_MME_PROTOCOL_CONFIGURATION_OPTIONS_IEI;
            msg_ptr++;
            liblte_mme_pack_protocol_config_options_ie(&mod_eps_bearer_context_rej->protocol_cnfg_opts, &msg_ptr);
        }

        // Fill in the number of bytes used
        msg->N_bytes = msg_ptr - msg->msg;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_mme_unpack_modify_eps_bearer_context_reject_msg(LIBLTE_BYTE_MSG_STRUCT                                 *msg,
                                                                         LIBLTE_MME_MODIFY_EPS_BEARER_CONTEXT_REJECT_MSG_STRUCT *mod_eps_bearer_context_rej)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr = msg->msg;

    if(msg                        != NULL &&
       mod_eps_bearer_context_rej != NULL)
    {
        // EPS Bearer ID
        mod_eps_bearer_context_rej->eps_bearer_id = (*msg_ptr >> 4);
        msg_ptr++;

        // Procedure Transaction ID
        mod_eps_bearer_context_rej->proc_transaction_id = *msg_ptr;
        msg_ptr++;

        // Skip Message Type
        msg_ptr++;

        // ESM Cause
        liblte_mme_unpack_esm_cause_ie(&msg_ptr, &mod_eps_bearer_context_rej->esm_cause);

        // Protocol Configuration Options
        if(LIBLTE_MME_PROTOCOL_CONFIGURATION_OPTIONS_IEI == *msg_ptr)
        {
            msg_ptr++;
            liblte_mme_unpack_protocol_config_options_ie(&msg_ptr, &mod_eps_bearer_context_rej->protocol_cnfg_opts);
            mod_eps_bearer_context_rej->protocol_cnfg_opts_present = true;
        }else{
            mod_eps_bearer_context_rej->protocol_cnfg_opts_present = false;
        }

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    Message Name: Modify EPS Bearer Context Request

    Description: Sent by the network to the UE to request modification
                 of an active EPS bearer context.

    Document Reference: 24.301 v10.2.0 Section 8.3.18
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_mme_pack_modify_eps_bearer_context_request_msg(LIBLTE_MME_MODIFY_EPS_BEARER_CONTEXT_REQUEST_MSG_STRUCT *mod_eps_bearer_context_req,
                                                                        LIBLTE_BYTE_MSG_STRUCT                                  *msg)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr = msg->msg;

    if(mod_eps_bearer_context_req != NULL &&
       msg                        != NULL)
    {
        // Protocol Discriminator and EPS Bearer ID
        *msg_ptr = (mod_eps_bearer_context_req->eps_bearer_id << 4) | (LIBLTE_MME_PD_EPS_SESSION_MANAGEMENT);
        msg_ptr++;

        // Procedure Transaction ID
        *msg_ptr = mod_eps_bearer_context_req->proc_transaction_id;
        msg_ptr++;

        // Message Type
        *msg_ptr = LIBLTE_MME_MSG_TYPE_MODIFY_EPS_BEARER_CONTEXT_REQUEST;
        msg_ptr++;

        // New EPS QoS
        if(mod_eps_bearer_context_req->new_eps_qos_present)
        {
            *msg_ptr = LIBLTE_MME_EPS_QUALITY_OF_SERVICE_IEI;
            msg_ptr++;
            liblte_mme_pack_eps_quality_of_service_ie(&mod_eps_bearer_context_req->new_eps_qos, &msg_ptr);
        }

        // TFT
        if(mod_eps_bearer_context_req->tft_present)
        {
            *msg_ptr = LIBLTE_MME_TRAFFIC_FLOW_TEMPLATE_IEI;
            msg_ptr++;
            liblte_mme_pack_traffic_flow_template_ie(&mod_eps_bearer_context_req->tft, &msg_ptr);
        }

        // New QoS
        if(mod_eps_bearer_context_req->new_qos_present)
        {
            *msg_ptr = LIBLTE_MME_QUALITY_OF_SERVICE_IEI;
            msg_ptr++;
            liblte_mme_pack_quality_of_service_ie(&mod_eps_bearer_context_req->new_qos, &msg_ptr);
        }

        // Negotiated LLC SAPI
        if(mod_eps_bearer_context_req->negotiated_llc_sapi_present)
        {
            *msg_ptr = LIBLTE_MME_LLC_SAPI_IEI;
            msg_ptr++;
            liblte_mme_pack_llc_service_access_point_identifier_ie(mod_eps_bearer_context_req->negotiated_llc_sapi, &msg_ptr);
        }

        // Radio Priority
        if(mod_eps_bearer_context_req->radio_prio_present)
        {
            *msg_ptr = LIBLTE_MME_RADIO_PRIORITY_IEI << 4;
            liblte_mme_pack_radio_priority_ie(mod_eps_bearer_context_req->radio_prio, 0, &msg_ptr);
            msg_ptr++;
        }

        // Packet Flow Identifier
        if(mod_eps_bearer_context_req->packet_flow_id_present)
        {
            *msg_ptr = LIBLTE_MME_PACKET_FLOW_IDENTIFIER_IEI;
            msg_ptr++;
            liblte_mme_pack_packet_flow_identifier_ie(mod_eps_bearer_context_req->packet_flow_id, &msg_ptr);
        }

        // APN-AMBR
        if(mod_eps_bearer_context_req->apn_ambr_present)
        {
            *msg_ptr = LIBLTE_MME_APN_AMBR_IEI;
            msg_ptr++;
            liblte_mme_pack_apn_aggregate_maximum_bit_rate_ie(&mod_eps_bearer_context_req->apn_ambr, &msg_ptr);
        }

        // Protocol Configuration Options
        if(mod_eps_bearer_context_req->protocol_cnfg_opts_present)
        {
            *msg_ptr = LIBLTE_MME_PROTOCOL_CONFIGURATION_OPTIONS_IEI;
            msg_ptr++;
            liblte_mme_pack_protocol_config_options_ie(&mod_eps_bearer_context_req->protocol_cnfg_opts, &msg_ptr);
        }

        // Fill in the number of bytes used
        msg->N_bytes = msg_ptr - msg->msg;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_mme_unpack_modify_eps_bearer_context_request_msg(LIBLTE_BYTE_MSG_STRUCT                                  *msg,
                                                                          LIBLTE_MME_MODIFY_EPS_BEARER_CONTEXT_REQUEST_MSG_STRUCT *mod_eps_bearer_context_req)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr = msg->msg;

    if(msg                        != NULL &&
       mod_eps_bearer_context_req != NULL)
    {
        // EPS Bearer ID
        mod_eps_bearer_context_req->eps_bearer_id = (*msg_ptr >> 4);
        msg_ptr++;

        // Procedure Transaction ID
        mod_eps_bearer_context_req->proc_transaction_id = *msg_ptr;
        msg_ptr++;

        // Skip Message Type
        msg_ptr++;

        // New EPS QoS
        if(LIBLTE_MME_EPS_QUALITY_OF_SERVICE_IEI == *msg_ptr)
        {
            msg_ptr++;
            liblte_mme_unpack_eps_quality_of_service_ie(&msg_ptr, &mod_eps_bearer_context_req->new_eps_qos);
            mod_eps_bearer_context_req->new_eps_qos_present = true;
        }else{
            mod_eps_bearer_context_req->new_eps_qos_present = false;
        }

        // TFT
        if(LIBLTE_MME_TRAFFIC_FLOW_TEMPLATE_IEI == *msg_ptr)
        {
            msg_ptr++;
            liblte_mme_unpack_traffic_flow_template_ie(&msg_ptr, &mod_eps_bearer_context_req->tft);
            mod_eps_bearer_context_req->tft_present = true;
        }else{
            mod_eps_bearer_context_req->tft_present = false;
        }

        // New QoS
        if(LIBLTE_MME_QUALITY_OF_SERVICE_IEI == *msg_ptr)
        {
            msg_ptr++;
            liblte_mme_unpack_quality_of_service_ie(&msg_ptr, &mod_eps_bearer_context_req->new_qos);
            mod_eps_bearer_context_req->new_qos_present = true;
        }else{
            mod_eps_bearer_context_req->new_qos_present = false;
        }

        // Negotiated LLC SAPI
        if(LIBLTE_MME_LLC_SAPI_IEI == *msg_ptr)
        {
            msg_ptr++;
            liblte_mme_unpack_llc_service_access_point_identifier_ie(&msg_ptr, &mod_eps_bearer_context_req->negotiated_llc_sapi);
            mod_eps_bearer_context_req->negotiated_llc_sapi_present = true;
        }else{
            mod_eps_bearer_context_req->negotiated_llc_sapi_present = false;
        }

        // Radio Priority
        if((LIBLTE_MME_RADIO_PRIORITY_IEI << 4) == *msg_ptr)
        {
            liblte_mme_unpack_radio_priority_ie(&msg_ptr, 0, &mod_eps_bearer_context_req->radio_prio);
            msg_ptr++;
            mod_eps_bearer_context_req->radio_prio_present = true;
        }else{
            mod_eps_bearer_context_req->radio_prio_present = false;
        }

        // Packet Flow Identifier
        if(LIBLTE_MME_PACKET_FLOW_IDENTIFIER_IEI == *msg_ptr)
        {
            msg_ptr++;
            liblte_mme_unpack_packet_flow_identifier_ie(&msg_ptr, &mod_eps_bearer_context_req->packet_flow_id);
            mod_eps_bearer_context_req->packet_flow_id_present = true;
        }else{
            mod_eps_bearer_context_req->packet_flow_id_present = false;
        }

        // APN-AMBR
        if(LIBLTE_MME_APN_AMBR_IEI == *msg_ptr)
        {
            msg_ptr++;
            liblte_mme_unpack_apn_aggregate_maximum_bit_rate_ie(&msg_ptr, &mod_eps_bearer_context_req->apn_ambr);
            mod_eps_bearer_context_req->apn_ambr_present = true;
        }else{
            mod_eps_bearer_context_req->apn_ambr_present = false;
        }

        // Protocol Configuration Options
        if(LIBLTE_MME_PROTOCOL_CONFIGURATION_OPTIONS_IEI == *msg_ptr)
        {
            msg_ptr++;
            liblte_mme_unpack_protocol_config_options_ie(&msg_ptr, &mod_eps_bearer_context_req->protocol_cnfg_opts);
            mod_eps_bearer_context_req->protocol_cnfg_opts_present = true;
        }else{
            mod_eps_bearer_context_req->protocol_cnfg_opts_present = false;
        }

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    Message Name: Notification

    Description: Sent by the network to inform the UE about events
                 which are relevant for the upper layer using an EPS
                 bearer context or having requested a procedure
                 transaction.

    Document Reference: 24.301 v10.2.0 Section 8.3.18A
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_mme_pack_notification_msg(LIBLTE_MME_NOTIFICATION_MSG_STRUCT *notification,
                                                   LIBLTE_BYTE_MSG_STRUCT             *msg)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr = msg->msg;

    if(notification != NULL &&
       msg          != NULL)
    {
        // Protocol Discriminator and EPS Bearer ID
        *msg_ptr = (notification->eps_bearer_id << 4) | (LIBLTE_MME_PD_EPS_SESSION_MANAGEMENT);
        msg_ptr++;

        // Procedure Transaction ID
        *msg_ptr = notification->proc_transaction_id;
        msg_ptr++;

        // Message Type
        *msg_ptr = LIBLTE_MME_MSG_TYPE_NOTIFICATION;
        msg_ptr++;

        // Notification Indicator
        liblte_mme_pack_notification_indicator_ie(notification->notification_ind, &msg_ptr);

        // Fill in the number of bytes used
        msg->N_bytes = msg_ptr - msg->msg;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_mme_unpack_notification_msg(LIBLTE_BYTE_MSG_STRUCT             *msg,
                                                     LIBLTE_MME_NOTIFICATION_MSG_STRUCT *notification)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr = msg->msg;

    if(msg          != NULL &&
       notification != NULL)
    {
        // EPS Bearer ID
        notification->eps_bearer_id = (*msg_ptr >> 4);
        msg_ptr++;

        // Procedure Transaction ID
        notification->proc_transaction_id = *msg_ptr;
        msg_ptr++;

        // Skip Message Type
        msg_ptr++;

        // Notification Indicator
        liblte_mme_unpack_notification_indicator_ie(&msg_ptr, &notification->notification_ind);

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    Message Name: PDN Connectivity Reject

    Description: Sent by the network to the UE to reject establishment
                 of a PDN connection.

    Document Reference: 24.301 v10.2.0 Section 8.3.19
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_mme_pack_pdn_connectivity_reject_msg(LIBLTE_MME_PDN_CONNECTIVITY_REJECT_MSG_STRUCT *pdn_con_rej,
                                                              LIBLTE_BYTE_MSG_STRUCT                        *msg)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr = msg->msg;

    if(pdn_con_rej != NULL &&
       msg         != NULL)
    {
        // Protocol Discriminator and EPS Bearer ID
        *msg_ptr = (pdn_con_rej->eps_bearer_id << 4) | (LIBLTE_MME_PD_EPS_SESSION_MANAGEMENT);
        msg_ptr++;

        // Procedure Transaction ID
        *msg_ptr = pdn_con_rej->proc_transaction_id;
        msg_ptr++;

        // Message Type
        *msg_ptr = LIBLTE_MME_MSG_TYPE_PDN_CONNECTIVITY_REJECT;
        msg_ptr++;

        // ESM Cause
        liblte_mme_pack_esm_cause_ie(pdn_con_rej->esm_cause, &msg_ptr);

        // Protocol Configuration Options
        if(pdn_con_rej->protocol_cnfg_opts_present)
        {
            *msg_ptr = LIBLTE_MME_PROTOCOL_CONFIGURATION_OPTIONS_IEI;
            msg_ptr++;
            liblte_mme_pack_protocol_config_options_ie(&pdn_con_rej->protocol_cnfg_opts, &msg_ptr);
        }

        // T3496 Value
        if(pdn_con_rej->t3496_present)
        {
            *msg_ptr = LIBLTE_MME_T3496_VALUE_IEI;
            msg_ptr++;
            liblte_mme_pack_gprs_timer_3_ie(&pdn_con_rej->t3496, &msg_ptr);
        }

        // Fill in the number of bytes used
        msg->N_bytes = msg_ptr - msg->msg;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_mme_unpack_pdn_connectivity_reject_msg(LIBLTE_BYTE_MSG_STRUCT                        *msg,
                                                                LIBLTE_MME_PDN_CONNECTIVITY_REJECT_MSG_STRUCT *pdn_con_rej)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr = msg->msg;

    if(msg         != NULL &&
       pdn_con_rej != NULL)
    {
        // EPS Bearer ID
        pdn_con_rej->eps_bearer_id = (*msg_ptr >> 4);
        msg_ptr++;

        // Procedure Transaction ID
        pdn_con_rej->proc_transaction_id = *msg_ptr;
        msg_ptr++;

        // Skip Message Type
        msg_ptr++;

        // ESM Cause
        liblte_mme_unpack_esm_cause_ie(&msg_ptr, &pdn_con_rej->esm_cause);

        // Protocol Configuration Options
        if(LIBLTE_MME_PROTOCOL_CONFIGURATION_OPTIONS_IEI == *msg_ptr)
        {
            msg_ptr++;
            liblte_mme_unpack_protocol_config_options_ie(&msg_ptr, &pdn_con_rej->protocol_cnfg_opts);
            pdn_con_rej->protocol_cnfg_opts_present = true;
        }else{
            pdn_con_rej->protocol_cnfg_opts_present = false;
        }

        // T3496 Value
        if(LIBLTE_MME_T3496_VALUE_IEI == *msg_ptr)
        {
            msg_ptr++;
            liblte_mme_unpack_gprs_timer_3_ie(&msg_ptr, &pdn_con_rej->t3496);
            pdn_con_rej->t3496_present = true;
        }else{
            pdn_con_rej->t3496_present = false;
        }

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    Message Name: PDN Connectivity Request

    Description: Sent by the UE to the network to initiate
                 establishment of a PDN connection.

    Document Reference: 24.301 v10.2.0 Section 8.3.20
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_mme_pack_pdn_connectivity_request_msg(LIBLTE_MME_PDN_CONNECTIVITY_REQUEST_MSG_STRUCT *pdn_con_req,
                                                               LIBLTE_BYTE_MSG_STRUCT                         *msg)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr = msg->msg;

    if(pdn_con_req != NULL &&
       msg         != NULL)
    {
        // Protocol Discriminator and EPS Bearer ID
        *msg_ptr = (pdn_con_req->eps_bearer_id << 4) | (LIBLTE_MME_PD_EPS_SESSION_MANAGEMENT);
        msg_ptr++;

        // Procedure Transaction ID
        *msg_ptr = pdn_con_req->proc_transaction_id;
        msg_ptr++;

        // Message Type
        *msg_ptr = LIBLTE_MME_MSG_TYPE_PDN_CONNECTIVITY_REQUEST;
        msg_ptr++;

        // Request Type & PDN Type
        *msg_ptr = 0;
        liblte_mme_pack_request_type_ie(pdn_con_req->request_type, 0, &msg_ptr);
        liblte_mme_pack_pdn_type_ie(pdn_con_req->pdn_type, 4, &msg_ptr);
        msg_ptr++;

        // ESM Information Transfer Flag
        if(pdn_con_req->esm_info_transfer_flag_present)
        {
            *msg_ptr = LIBLTE_MME_ESM_INFO_TRANSFER_FLAG_IEI << 4;
            liblte_mme_pack_esm_info_transfer_flag_ie(pdn_con_req->esm_info_transfer_flag, 0, &msg_ptr);
            msg_ptr++;
        }

        // Access Point Name
        if(pdn_con_req->apn_present)
        {
            *msg_ptr = LIBLTE_MME_ACCESS_POINT_NAME_IEI;
            msg_ptr++;
            liblte_mme_pack_access_point_name_ie(&pdn_con_req->apn, &msg_ptr);
        }

        // Protocol Configuration Options
        if(pdn_con_req->protocol_cnfg_opts_present)
        {
            *msg_ptr = LIBLTE_MME_PROTOCOL_CONFIGURATION_OPTIONS_IEI;
            msg_ptr++;
            liblte_mme_pack_protocol_config_options_ie(&pdn_con_req->protocol_cnfg_opts, &msg_ptr);
        }

        // Device Properties
        if(pdn_con_req->device_properties_present)
        {
            *msg_ptr = LIBLTE_MME_PDN_CONNECTIVITY_REQUEST_DEVICE_PROPERTIES_IEI << 4;
            liblte_mme_pack_device_properties_ie(pdn_con_req->device_properties, 0, &msg_ptr);
            msg_ptr++;
        }

        // Fill in the number of bytes used
        msg->N_bytes = msg_ptr - msg->msg;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_mme_unpack_pdn_connectivity_request_msg(LIBLTE_BYTE_MSG_STRUCT                         *msg,
                                                                 LIBLTE_MME_PDN_CONNECTIVITY_REQUEST_MSG_STRUCT *pdn_con_req)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr = msg->msg;

    if(msg         != NULL &&
       pdn_con_req != NULL)
    {
        // EPS Bearer ID
        pdn_con_req->eps_bearer_id = (*msg_ptr >> 4);
        msg_ptr++;

        // Procedure Transaction ID
        pdn_con_req->proc_transaction_id = *msg_ptr;
        msg_ptr++;

        // Skip Message Type
        msg_ptr++;

        // Request Type & PDN Type
        liblte_mme_unpack_request_type_ie(&msg_ptr, 0, &pdn_con_req->request_type);
        liblte_mme_unpack_pdn_type_ie(&msg_ptr, 4, &pdn_con_req->pdn_type);
        msg_ptr++;

        // ESM Information Transfer Flag
        if((LIBLTE_MME_ESM_INFO_TRANSFER_FLAG_IEI << 4) == (*msg_ptr & 0xF0))
        {
            liblte_mme_unpack_esm_info_transfer_flag_ie(&msg_ptr, 0, &pdn_con_req->esm_info_transfer_flag);
            msg_ptr++;
            pdn_con_req->esm_info_transfer_flag_present = true;
        }else{
            pdn_con_req->esm_info_transfer_flag_present = false;
        }

        // Access Point Name
        if(LIBLTE_MME_ACCESS_POINT_NAME_IEI == *msg_ptr)
        {
            msg_ptr++;
            liblte_mme_unpack_access_point_name_ie(&msg_ptr, &pdn_con_req->apn);
            pdn_con_req->apn_present = true;
        }else{
            pdn_con_req->apn_present = false;
        }

        // Protocol Configuration Options
        if(LIBLTE_MME_PROTOCOL_CONFIGURATION_OPTIONS_IEI == *msg_ptr)
        {
            msg_ptr++;
            liblte_mme_unpack_protocol_config_options_ie(&msg_ptr, &pdn_con_req->protocol_cnfg_opts);
            pdn_con_req->protocol_cnfg_opts_present = true;
        }else{
            pdn_con_req->protocol_cnfg_opts_present = false;
        }

        // Device Properties
        if((LIBLTE_MME_PDN_CONNECTIVITY_REQUEST_DEVICE_PROPERTIES_IEI << 4) == (*msg_ptr & 0xF0))
        {
            liblte_mme_unpack_device_properties_ie(&msg_ptr, 0, &pdn_con_req->device_properties);
            msg_ptr++;
            pdn_con_req->device_properties_present = true;
        }else{
            pdn_con_req->device_properties_present = false;
        }

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    Message Name: PDN Disconnect Reject

    Description: Sent by the network to the UE to reject release of a
                 PDN connection.

    Document Reference: 24.301 v10.2.0 Section 8.3.21
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_mme_pack_pdn_disconnect_reject_msg(LIBLTE_MME_PDN_DISCONNECT_REJECT_MSG_STRUCT *pdn_discon_rej,
                                                            LIBLTE_BYTE_MSG_STRUCT                      *msg)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr = msg->msg;

    if(pdn_discon_rej != NULL &&
       msg            != NULL)
    {
        // Protocol Discriminator and EPS Bearer ID
        *msg_ptr = (pdn_discon_rej->eps_bearer_id << 4) | (LIBLTE_MME_PD_EPS_SESSION_MANAGEMENT);
        msg_ptr++;

        // Procedure Transaction ID
        *msg_ptr = pdn_discon_rej->proc_transaction_id;
        msg_ptr++;

        // Message Type
        *msg_ptr = LIBLTE_MME_MSG_TYPE_PDN_DISCONNECT_REJECT;
        msg_ptr++;

        // ESM Cause
        liblte_mme_pack_esm_cause_ie(pdn_discon_rej->esm_cause, &msg_ptr);

        // Protocol Configuration Options
        if(pdn_discon_rej->protocol_cnfg_opts_present)
        {
            *msg_ptr = LIBLTE_MME_PROTOCOL_CONFIGURATION_OPTIONS_IEI;
            msg_ptr++;
            liblte_mme_pack_protocol_config_options_ie(&pdn_discon_rej->protocol_cnfg_opts, &msg_ptr);
        }

        // Fill in the number of bytes used
        msg->N_bytes = msg_ptr - msg->msg;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_mme_unpack_pdn_disconnect_reject_msg(LIBLTE_BYTE_MSG_STRUCT                      *msg,
                                                              LIBLTE_MME_PDN_DISCONNECT_REJECT_MSG_STRUCT *pdn_discon_rej)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr = msg->msg;

    if(msg            != NULL &&
       pdn_discon_rej != NULL)
    {
        // EPS Bearer ID
        pdn_discon_rej->eps_bearer_id = (*msg_ptr >> 4);
        msg_ptr++;

        // Procedure Transaction ID
        pdn_discon_rej->proc_transaction_id = *msg_ptr;
        msg_ptr++;

        // Skip Message Type
        msg_ptr++;

        // ESM Cause
        liblte_mme_unpack_esm_cause_ie(&msg_ptr, &pdn_discon_rej->esm_cause);

        // Protocol Configuration Options
        if(LIBLTE_MME_PROTOCOL_CONFIGURATION_OPTIONS_IEI == *msg_ptr)
        {
            msg_ptr++;
            liblte_mme_unpack_protocol_config_options_ie(&msg_ptr, &pdn_discon_rej->protocol_cnfg_opts);
            pdn_discon_rej->protocol_cnfg_opts_present = true;
        }else{
            pdn_discon_rej->protocol_cnfg_opts_present = false;
        }

        err = LIBLTE_SUCCESS;
    }

    return(err);
}

/*********************************************************************
    Message Name: PDN Disconnect Request

    Description: Sent by the UE to the network to initiate release of
                 a PDN connection.

    Document Reference: 24.301 v10.2.0 Section 8.3.22
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_mme_pack_pdn_disconnect_request_msg(LIBLTE_MME_PDN_DISCONNECT_REQUEST_MSG_STRUCT *pdn_discon_req,
                                                             LIBLTE_BYTE_MSG_STRUCT                       *msg)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr = msg->msg;

    if(pdn_discon_req != NULL &&
       msg            != NULL)
    {
        // Protocol Discriminator and EPS Bearer ID
        *msg_ptr = (pdn_discon_req->eps_bearer_id << 4) | (LIBLTE_MME_PD_EPS_SESSION_MANAGEMENT);
        msg_ptr++;

        // Procedure Transaction ID
        *msg_ptr = pdn_discon_req->proc_transaction_id;
        msg_ptr++;

        // Message Type
        *msg_ptr = LIBLTE_MME_MSG_TYPE_PDN_DISCONNECT_REQUEST;
        msg_ptr++;

        // Linked EPS Bearer Identity & Spare Half Octet
        *msg_ptr = 0;
        liblte_mme_pack_linked_eps_bearer_identity_ie(pdn_discon_req->linked_eps_bearer_id, 0, &msg_ptr);
        msg_ptr++;

        // Protocol Configuration Options
        if(pdn_discon_req->protocol_cnfg_opts_present)
        {
            *msg_ptr = LIBLTE_MME_PROTOCOL_CONFIGURATION_OPTIONS_IEI;
            msg_ptr++;
            liblte_mme_pack_protocol_config_options_ie(&pdn_discon_req->protocol_cnfg_opts, &msg_ptr);
        }

        // Fill in the number of bytes used
        msg->N_bytes = msg_ptr - msg->msg;

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_mme_unpack_pdn_disconnect_request_msg(LIBLTE_BYTE_MSG_STRUCT                       *msg,
                                                               LIBLTE_MME_PDN_DISCONNECT_REQUEST_MSG_STRUCT *pdn_discon_req)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *msg_ptr = msg->msg;

    if(msg            != NULL &&
       pdn_discon_req != NULL)
    {
        // EPS Bearer ID
        pdn_discon_req->eps_bearer_id = (*msg_ptr >> 4);
        msg_ptr++;

        // Procedure Transaction ID
        pdn_discon_req->proc_transaction_id = *msg_ptr;
        msg_ptr++;

        // Skip Message Type
        msg_ptr++;

        // Linked EPS Bearer Identity & Spare Half Octet
        liblte_mme_unpack_linked_eps_bearer_identity_ie(&msg_ptr, 0, &pdn_discon_req->linked_eps_bearer_id);
        msg_ptr++;

        // Protocol Configuration Options
        if(LIBLTE_MME_PROTOCOL_CONFIGURATION_OPTIONS_IEI == *msg_ptr)
        {
            msg_ptr++;
            liblte_mme_unpack_protocol_config_options_ie(&msg_ptr, &pdn_discon_req->protocol_cnfg_opts);
            pdn_discon_req->protocol_cnfg_opts_present = true;
        }else{
            pdn_discon_req->protocol_cnfg_opts_present = false;
        }

        err = LIBLTE_SUCCESS;
    }

    return(err);
}
