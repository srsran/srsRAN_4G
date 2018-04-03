/*******************************************************************************
/*
/* Copyright 2016 Software Radio Systems Limited
/*
********************************************************************************/

#ifndef SRSLTE_LIBLTE_S1AP_H
#define SRSLTE_LIBLTE_S1AP_H

/*******************************************************************************
/* Warnings/Todos
********************************************************************************/
// Extensions are not yet handled correctly
// Dynamic Sequence Of types have max 32 elements to reduce memory footprint
// Container Lists are not yet handled correctly e.g. E-RAB-IE-ContainerList

/*******************************************************************************
/* INCLUDES
********************************************************************************/

#include "liblte_common.h"

/*******************************************************************************
                              LOGGING
*******************************************************************************/

typedef void (*log_handler_t)(void *ctx, char *str);

void liblte_log_register_handler(void *ctx, log_handler_t handler);


/*******************************************************************************
/* MAX defines
********************************************************************************/
#define LIBLTE_S1AP_MAXPRIVATEIES                                      65535
#define LIBLTE_S1AP_MAXPROTOCOLIES                                     65535
#define LIBLTE_S1AP_MAXNOOFE_RABS                                      256
#define LIBLTE_S1AP_MAXNOOFTACS                                        256
#define LIBLTE_S1AP_MAXNOOFBPLMNS                                      6
#define LIBLTE_S1AP_MAXNOOFEPLMNS                                      15
#define LIBLTE_S1AP_MAXNOOFFORBLACS                                    4096
#define LIBLTE_S1AP_MAXNOOFINDIVIDUALS1CONNECTIONSTORESET              256
#define LIBLTE_S1AP_MAXNOOFTAIFORWARNING                               65535
#define LIBLTE_S1AP_MAXNOOFEMERGENCYAREAID                             65535
#define LIBLTE_S1AP_MAXNOOFCELLINEAI                                   65535
#define LIBLTE_S1AP_MAXNOOFENBX2EXTTLAS                                16
#define LIBLTE_S1AP_MAXNOOFRATS                                        8
#define LIBLTE_S1AP_MAXNOOFMMECS                                       256
#define LIBLTE_S1AP_MAXNOOFTAFORMDT                                    8
#define LIBLTE_S1AP_MAXNOOFCELLSFORRESTART                             256
#define LIBLTE_S1AP_MAXNOOFRESTARTEMERGENCYAREAIDS                     256
#define LIBLTE_S1AP_MAXNOOFCSGS                                        256
#define LIBLTE_S1AP_MAXNOOFERRORS                                      256
#define LIBLTE_S1AP_MAXNOOFEPLMNSPLUSONE                               16
#define LIBLTE_S1AP_MAXNOOFCELLS                                       16
#define LIBLTE_S1AP_MAXNOOFCELLINTAI                                   65535
#define LIBLTE_S1AP_MAXNOOFENBX2GTPTLAS                                16
#define LIBLTE_S1AP_MAXNOOFCELLIDFORMDT                                32
#define LIBLTE_S1AP_MAXNOOFRESTARTTAIS                                 2048
#define LIBLTE_S1AP_MAXPROTOCOLEXTENSIONS                              65535
#define LIBLTE_S1AP_MAXNOOFPLMNSPERMME                                 32
#define LIBLTE_S1AP_MAXNOOFCELLID                                      65535
#define LIBLTE_S1AP_MAXNOOFGROUPIDS                                    65535
#define LIBLTE_S1AP_MAXNOOFTAIS                                        256
#define LIBLTE_S1AP_MAXNOOFENBX2TLAS                                   2
#define LIBLTE_S1AP_MAXNOOFMDTPLMNS                                    16
#define LIBLTE_S1AP_MAXNOOFFORBTACS                                    4096

/*******************************************************************************
/* Elementary Procedures
********************************************************************************/
typedef enum{
  LIBLTE_S1AP_PROC_ID_HANDOVERPREPARATION                             = 0,
  LIBLTE_S1AP_PROC_ID_HANDOVERRESOURCEALLOCATION                      = 1,
  LIBLTE_S1AP_PROC_ID_HANDOVERNOTIFICATION                            = 2,
  LIBLTE_S1AP_PROC_ID_PATHSWITCHREQUEST                               = 3,
  LIBLTE_S1AP_PROC_ID_HANDOVERCANCEL                                  = 4,
  LIBLTE_S1AP_PROC_ID_E_RABSETUP                                      = 5,
  LIBLTE_S1AP_PROC_ID_E_RABMODIFY                                     = 6,
  LIBLTE_S1AP_PROC_ID_E_RABRELEASE                                    = 7,
  LIBLTE_S1AP_PROC_ID_E_RABRELEASEINDICATION                          = 8,
  LIBLTE_S1AP_PROC_ID_INITIALCONTEXTSETUP                             = 9,
  LIBLTE_S1AP_PROC_ID_PAGING                                          = 10,
  LIBLTE_S1AP_PROC_ID_DOWNLINKNASTRANSPORT                            = 11,
  LIBLTE_S1AP_PROC_ID_INITIALUEMESSAGE                                = 12,
  LIBLTE_S1AP_PROC_ID_UPLINKNASTRANSPORT                              = 13,
  LIBLTE_S1AP_PROC_ID_RESET                                           = 14,
  LIBLTE_S1AP_PROC_ID_ERRORINDICATION                                 = 15,
  LIBLTE_S1AP_PROC_ID_NASNONDELIVERYINDICATION                        = 16,
  LIBLTE_S1AP_PROC_ID_S1SETUP                                         = 17,
  LIBLTE_S1AP_PROC_ID_UECONTEXTRELEASEREQUEST                         = 18,
  LIBLTE_S1AP_PROC_ID_DOWNLINKS1CDMA2000TUNNELING                     = 19,
  LIBLTE_S1AP_PROC_ID_UPLINKS1CDMA2000TUNNELING                       = 20,
  LIBLTE_S1AP_PROC_ID_UECONTEXTMODIFICATION                           = 21,
  LIBLTE_S1AP_PROC_ID_UECAPABILITYINFOINDICATION                      = 22,
  LIBLTE_S1AP_PROC_ID_UECONTEXTRELEASE                                = 23,
  LIBLTE_S1AP_PROC_ID_ENBSTATUSTRANSFER                               = 24,
  LIBLTE_S1AP_PROC_ID_MMESTATUSTRANSFER                               = 25,
  LIBLTE_S1AP_PROC_ID_DEACTIVATETRACE                                 = 26,
  LIBLTE_S1AP_PROC_ID_TRACESTART                                      = 27,
  LIBLTE_S1AP_PROC_ID_TRACEFAILUREINDICATION                          = 28,
  LIBLTE_S1AP_PROC_ID_ENBCONFIGURATIONUPDATE                          = 29,
  LIBLTE_S1AP_PROC_ID_MMECONFIGURATIONUPDATE                          = 30,
  LIBLTE_S1AP_PROC_ID_LOCATIONREPORTINGCONTROL                        = 31,
  LIBLTE_S1AP_PROC_ID_LOCATIONREPORTINGFAILUREINDICATION              = 32,
  LIBLTE_S1AP_PROC_ID_LOCATIONREPORT                                  = 33,
  LIBLTE_S1AP_PROC_ID_OVERLOADSTART                                   = 34,
  LIBLTE_S1AP_PROC_ID_OVERLOADSTOP                                    = 35,
  LIBLTE_S1AP_PROC_ID_WRITEREPLACEWARNING                             = 36,
  LIBLTE_S1AP_PROC_ID_ENBDIRECTINFORMATIONTRANSFER                    = 37,
  LIBLTE_S1AP_PROC_ID_MMEDIRECTINFORMATIONTRANSFER                    = 38,
  LIBLTE_S1AP_PROC_ID_PRIVATEMESSAGE                                  = 39,
  LIBLTE_S1AP_PROC_ID_ENBCONFIGURATIONTRANSFER                        = 40,
  LIBLTE_S1AP_PROC_ID_MMECONFIGURATIONTRANSFER                        = 41,
  LIBLTE_S1AP_PROC_ID_CELLTRAFFICTRACE                                = 42,
  LIBLTE_S1AP_PROC_ID_KILL                                            = 43,
  LIBLTE_S1AP_PROC_ID_DOWNLINKUEASSOCIATEDLPPATRANSPORT               = 44,
  LIBLTE_S1AP_PROC_ID_UPLINKUEASSOCIATEDLPPATRANSPORT                 = 45,
  LIBLTE_S1AP_PROC_ID_DOWNLINKNONUEASSOCIATEDLPPATRANSPORT            = 46,
  LIBLTE_S1AP_PROC_ID_UPLINKNONUEASSOCIATEDLPPATRANSPORT              = 47,
  LIBLTE_S1AP_PROC_ID_UERADIOCAPABILITYMATCH                          = 48,
  LIBLTE_S1AP_PROC_ID_PWSRESTARTINDICATION                            = 49,
  LIBLTE_S1AP_PROC_N_ITEMS,
}LIBLTE_S1AP_PROC_ENUM;
static const char liblte_s1ap_proc_text[LIBLTE_S1AP_PROC_N_ITEMS][64] = { 
  "id-HandoverPreparation",
  "id-HandoverResourceAllocation",
  "id-HandoverNotification",
  "id-PathSwitchRequest",
  "id-HandoverCancel",
  "id-E-RABSetup",
  "id-E-RABModify",
  "id-E-RABRelease",
  "id-E-RABReleaseIndication",
  "id-InitialContextSetup",
  "id-Paging",
  "id-downlinkNASTransport",
  "id-initialUEMessage",
  "id-uplinkNASTransport",
  "id-Reset",
  "id-ErrorIndication",
  "id-NASNonDeliveryIndication",
  "id-S1Setup",
  "id-UEContextReleaseRequest",
  "id-DownlinkS1cdma2000tunneling",
  "id-UplinkS1cdma2000tunneling",
  "id-UEContextModification",
  "id-UECapabilityInfoIndication",
  "id-UEContextRelease",
  "id-eNBStatusTransfer",
  "id-MMEStatusTransfer",
  "id-DeactivateTrace",
  "id-TraceStart",
  "id-TraceFailureIndication",
  "id-ENBConfigurationUpdate",
  "id-MMEConfigurationUpdate",
  "id-LocationReportingControl",
  "id-LocationReportingFailureIndication",
  "id-LocationReport",
  "id-OverloadStart",
  "id-OverloadStop",
  "id-WriteReplaceWarning",
  "id-eNBDirectInformationTransfer",
  "id-MMEDirectInformationTransfer",
  "id-PrivateMessage",
  "id-eNBConfigurationTransfer",
  "id-MMEConfigurationTransfer",
  "id-CellTrafficTrace",
  "id-Kill",
  "id-downlinkUEAssociatedLPPaTransport",
  "id-uplinkUEAssociatedLPPaTransport",
  "id-downlinkNonUEAssociatedLPPaTransport",
  "id-uplinkNonUEAssociatedLPPaTransport",
  "id-UERadioCapabilityMatch",
  "id-PWSRestartIndication",
};



/*******************************************************************************
/* ProtocolIE Ids
********************************************************************************/
typedef enum{
  LIBLTE_S1AP_IE_ID_MME_UE_S1AP_ID                                  = 0,
  LIBLTE_S1AP_IE_ID_HANDOVERTYPE                                    = 1,
  LIBLTE_S1AP_IE_ID_CAUSE                                           = 2,
  LIBLTE_S1AP_IE_ID_SOURCEID                                        = 3,
  LIBLTE_S1AP_IE_ID_TARGETID                                        = 4,
  LIBLTE_S1AP_IE_ID_SPARE5                                          = 5,
  LIBLTE_S1AP_IE_ID_SPARE6                                          = 6,
  LIBLTE_S1AP_IE_ID_SPARE7                                          = 7,
  LIBLTE_S1AP_IE_ID_ENB_UE_S1AP_ID                                  = 8,
  LIBLTE_S1AP_IE_ID_SPARE9                                          = 9,
  LIBLTE_S1AP_IE_ID_SPARE10                                         = 10,
  LIBLTE_S1AP_IE_ID_SPARE11                                         = 11,
  LIBLTE_S1AP_IE_ID_E_RABSUBJECTTODATAFORWARDINGLIST                = 12,
  LIBLTE_S1AP_IE_ID_E_RABTORELEASELISTHOCMD                         = 13,
  LIBLTE_S1AP_IE_ID_E_RABDATAFORWARDINGITEM                         = 14,
  LIBLTE_S1AP_IE_ID_E_RABRELEASEITEMBEARERRELCOMP                   = 15,
  LIBLTE_S1AP_IE_ID_E_RABTOBESETUPLISTBEARERSUREQ                   = 16,
  LIBLTE_S1AP_IE_ID_E_RABTOBESETUPITEMBEARERSUREQ                   = 17,
  LIBLTE_S1AP_IE_ID_E_RABADMITTEDLIST                               = 18,
  LIBLTE_S1AP_IE_ID_E_RABFAILEDTOSETUPLISTHOREQACK                  = 19,
  LIBLTE_S1AP_IE_ID_E_RABADMITTEDITEM                               = 20,
  LIBLTE_S1AP_IE_ID_E_RABFAILEDTOSETUPITEMHOREQACK                  = 21,
  LIBLTE_S1AP_IE_ID_E_RABTOBESWITCHEDDLLIST                         = 22,
  LIBLTE_S1AP_IE_ID_E_RABTOBESWITCHEDDLITEM                         = 23,
  LIBLTE_S1AP_IE_ID_E_RABTOBESETUPLISTCTXTSUREQ                     = 24,
  LIBLTE_S1AP_IE_ID_TRACEACTIVATION                                 = 25,
  LIBLTE_S1AP_IE_ID_NAS_PDU                                         = 26,
  LIBLTE_S1AP_IE_ID_E_RABTOBESETUPITEMHOREQ                         = 27,
  LIBLTE_S1AP_IE_ID_E_RABSETUPLISTBEARERSURES                       = 28,
  LIBLTE_S1AP_IE_ID_E_RABFAILEDTOSETUPLISTBEARERSURES               = 29,
  LIBLTE_S1AP_IE_ID_E_RABTOBEMODIFIEDLISTBEARERMODREQ               = 30,
  LIBLTE_S1AP_IE_ID_E_RABMODIFYLISTBEARERMODRES                     = 31,
  LIBLTE_S1AP_IE_ID_E_RABFAILEDTOMODIFYLIST                         = 32,
  LIBLTE_S1AP_IE_ID_E_RABTOBERELEASEDLIST                           = 33,
  LIBLTE_S1AP_IE_ID_E_RABFAILEDTORELEASELIST                        = 34,
  LIBLTE_S1AP_IE_ID_E_RABITEM                                       = 35,
  LIBLTE_S1AP_IE_ID_E_RABTOBEMODIFIEDITEMBEARERMODREQ               = 36,
  LIBLTE_S1AP_IE_ID_E_RABMODIFYITEMBEARERMODRES                     = 37,
  LIBLTE_S1AP_IE_ID_E_RABRELEASEITEM                                = 38,
  LIBLTE_S1AP_IE_ID_E_RABSETUPITEMBEARERSURES                       = 39,
  LIBLTE_S1AP_IE_ID_SECURITYCONTEXT                                 = 40,
  LIBLTE_S1AP_IE_ID_HANDOVERRESTRICTIONLIST                         = 41,
  LIBLTE_S1AP_IE_ID_SPARE42                                         = 42,
  LIBLTE_S1AP_IE_ID_UEPAGINGID                                      = 43,
  LIBLTE_S1AP_IE_ID_PAGINGDRX                                       = 44,
  LIBLTE_S1AP_IE_ID_SPARE45                                         = 45,
  LIBLTE_S1AP_IE_ID_TAILIST                                         = 46,
  LIBLTE_S1AP_IE_ID_TAIITEM                                         = 47,
  LIBLTE_S1AP_IE_ID_E_RABFAILEDTOSETUPLISTCTXTSURES                 = 48,
  LIBLTE_S1AP_IE_ID_E_RABRELEASEITEMHOCMD                           = 49,
  LIBLTE_S1AP_IE_ID_E_RABSETUPITEMCTXTSURES                         = 50,
  LIBLTE_S1AP_IE_ID_E_RABSETUPLISTCTXTSURES                         = 51,
  LIBLTE_S1AP_IE_ID_E_RABTOBESETUPITEMCTXTSUREQ                     = 52,
  LIBLTE_S1AP_IE_ID_E_RABTOBESETUPLISTHOREQ                         = 53,
  LIBLTE_S1AP_IE_ID_SPARE54                                         = 54,
  LIBLTE_S1AP_IE_ID_GERANTOLTEHOINFORMATIONRES                      = 55,
  LIBLTE_S1AP_IE_ID_SPARE56                                         = 56,
  LIBLTE_S1AP_IE_ID_UTRANTOLTEHOINFORMATIONRES                      = 57,
  LIBLTE_S1AP_IE_ID_CRITICALITYDIAGNOSTICS                          = 58,
  LIBLTE_S1AP_IE_ID_GLOBAL_ENB_ID                                   = 59,
  LIBLTE_S1AP_IE_ID_ENBNAME                                         = 60,
  LIBLTE_S1AP_IE_ID_MMENAME                                         = 61,
  LIBLTE_S1AP_IE_ID_SPARE62                                         = 62,
  LIBLTE_S1AP_IE_ID_SERVEDPLMNS                                     = 63,
  LIBLTE_S1AP_IE_ID_SUPPORTEDTAS                                    = 64,
  LIBLTE_S1AP_IE_ID_TIMETOWAIT                                      = 65,
  LIBLTE_S1AP_IE_ID_UEAGGREGATEMAXIMUMBITRATE                       = 66,
  LIBLTE_S1AP_IE_ID_TAI                                             = 67,
  LIBLTE_S1AP_IE_ID_SPARE68                                         = 68,
  LIBLTE_S1AP_IE_ID_E_RABRELEASELISTBEARERRELCOMP                   = 69,
  LIBLTE_S1AP_IE_ID_CDMA2000PDU                                     = 70,
  LIBLTE_S1AP_IE_ID_CDMA2000RATTYPE                                 = 71,
  LIBLTE_S1AP_IE_ID_CDMA2000SECTORID                                = 72,
  LIBLTE_S1AP_IE_ID_SECURITYKEY                                     = 73,
  LIBLTE_S1AP_IE_ID_UERADIOCAPABILITY                               = 74,
  LIBLTE_S1AP_IE_ID_GUMMEI_ID                                       = 75,
  LIBLTE_S1AP_IE_ID_SPARE76                                         = 76,
  LIBLTE_S1AP_IE_ID_SPARE77                                         = 77,
  LIBLTE_S1AP_IE_ID_E_RABINFORMATIONLISTITEM                        = 78,
  LIBLTE_S1AP_IE_ID_DIRECT_FORWARDING_PATH_AVAILABILITY             = 79,
  LIBLTE_S1AP_IE_ID_UEIDENTITYINDEXVALUE                            = 80,
  LIBLTE_S1AP_IE_ID_SPARE81                                         = 81,
  LIBLTE_S1AP_IE_ID_SPARE82                                         = 82,
  LIBLTE_S1AP_IE_ID_CDMA2000HOSTATUS                                = 83,
  LIBLTE_S1AP_IE_ID_CDMA2000HOREQUIREDINDICATION                    = 84,
  LIBLTE_S1AP_IE_ID_SPARE85                                         = 85,
  LIBLTE_S1AP_IE_ID_E_UTRAN_TRACE_ID                                = 86,
  LIBLTE_S1AP_IE_ID_RELATIVEMMECAPACITY                             = 87,
  LIBLTE_S1AP_IE_ID_SOURCEMME_UE_S1AP_ID                            = 88,
  LIBLTE_S1AP_IE_ID_BEARERS_SUBJECTTOSTATUSTRANSFER_ITEM            = 89,
  LIBLTE_S1AP_IE_ID_ENB_STATUSTRANSFER_TRANSPARENTCONTAINER         = 90,
  LIBLTE_S1AP_IE_ID_UE_ASSOCIATEDLOGICALS1_CONNECTIONITEM           = 91,
  LIBLTE_S1AP_IE_ID_RESETTYPE                                       = 92,
  LIBLTE_S1AP_IE_ID_UE_ASSOCIATEDLOGICALS1_CONNECTIONLISTRESACK     = 93,
  LIBLTE_S1AP_IE_ID_E_RABTOBESWITCHEDULITEM                         = 94,
  LIBLTE_S1AP_IE_ID_E_RABTOBESWITCHEDULLIST                         = 95,
  LIBLTE_S1AP_IE_ID_S_TMSI                                          = 96,
  LIBLTE_S1AP_IE_ID_CDMA2000ONEXRAND                                = 97,
  LIBLTE_S1AP_IE_ID_REQUESTTYPE                                     = 98,
  LIBLTE_S1AP_IE_ID_UE_S1AP_IDS                                     = 99,
  LIBLTE_S1AP_IE_ID_EUTRAN_CGI                                      = 100,
  LIBLTE_S1AP_IE_ID_OVERLOADRESPONSE                                = 101,
  LIBLTE_S1AP_IE_ID_CDMA2000ONEXSRVCCINFO                           = 102,
  LIBLTE_S1AP_IE_ID_E_RABFAILEDTOBERELEASEDLIST                     = 103,
  LIBLTE_S1AP_IE_ID_SOURCE_TOTARGET_TRANSPARENTCONTAINER            = 104,
  LIBLTE_S1AP_IE_ID_SERVEDGUMMEIS                                   = 105,
  LIBLTE_S1AP_IE_ID_SUBSCRIBERPROFILEIDFORRFP                       = 106,
  LIBLTE_S1AP_IE_ID_UESECURITYCAPABILITIES                          = 107,
  LIBLTE_S1AP_IE_ID_CSFALLBACKINDICATOR                             = 108,
  LIBLTE_S1AP_IE_ID_CNDOMAIN                                        = 109,
  LIBLTE_S1AP_IE_ID_E_RABRELEASEDLIST                               = 110,
  LIBLTE_S1AP_IE_ID_MESSAGEIDENTIFIER                               = 111,
  LIBLTE_S1AP_IE_ID_SERIALNUMBER                                    = 112,
  LIBLTE_S1AP_IE_ID_WARNINGAREALIST                                 = 113,
  LIBLTE_S1AP_IE_ID_REPETITIONPERIOD                                = 114,
  LIBLTE_S1AP_IE_ID_NUMBEROFBROADCASTREQUEST                        = 115,
  LIBLTE_S1AP_IE_ID_WARNINGTYPE                                     = 116,
  LIBLTE_S1AP_IE_ID_WARNINGSECURITYINFO                             = 117,
  LIBLTE_S1AP_IE_ID_DATACODINGSCHEME                                = 118,
  LIBLTE_S1AP_IE_ID_WARNINGMESSAGECONTENTS                          = 119,
  LIBLTE_S1AP_IE_ID_BROADCASTCOMPLETEDAREALIST                      = 120,
  LIBLTE_S1AP_IE_ID_INTER_SYSTEMINFORMATIONTRANSFERTYPEEDT          = 121,
  LIBLTE_S1AP_IE_ID_INTER_SYSTEMINFORMATIONTRANSFERTYPEMDT          = 122,
  LIBLTE_S1AP_IE_ID_TARGET_TOSOURCE_TRANSPARENTCONTAINER            = 123,
  LIBLTE_S1AP_IE_ID_SRVCCOPERATIONPOSSIBLE                          = 124,
  LIBLTE_S1AP_IE_ID_SRVCCHOINDICATION                               = 125,
  LIBLTE_S1AP_IE_ID_NAS_DOWNLINKCOUNT                               = 126,
  LIBLTE_S1AP_IE_ID_CSG_ID                                          = 127,
  LIBLTE_S1AP_IE_ID_CSG_IDLIST                                      = 128,
  LIBLTE_S1AP_IE_ID_SONCONFIGURATIONTRANSFERECT                     = 129,
  LIBLTE_S1AP_IE_ID_SONCONFIGURATIONTRANSFERMCT                     = 130,
  LIBLTE_S1AP_IE_ID_TRACECOLLECTIONENTITYIPADDRESS                  = 131,
  LIBLTE_S1AP_IE_ID_MSCLASSMARK2                                    = 132,
  LIBLTE_S1AP_IE_ID_MSCLASSMARK3                                    = 133,
  LIBLTE_S1AP_IE_ID_RRC_ESTABLISHMENT_CAUSE                         = 134,
  LIBLTE_S1AP_IE_ID_NASSECURITYPARAMETERSFROME_UTRAN                = 135,
  LIBLTE_S1AP_IE_ID_NASSECURITYPARAMETERSTOE_UTRAN                  = 136,
  LIBLTE_S1AP_IE_ID_DEFAULTPAGINGDRX                                = 137,
  LIBLTE_S1AP_IE_ID_SOURCE_TOTARGET_TRANSPARENTCONTAINER_SECONDARY  = 138,
  LIBLTE_S1AP_IE_ID_TARGET_TOSOURCE_TRANSPARENTCONTAINER_SECONDARY  = 139,
  LIBLTE_S1AP_IE_ID_EUTRANROUNDTRIPDELAYESTIMATIONINFO              = 140,
  LIBLTE_S1AP_IE_ID_BROADCASTCANCELLEDAREALIST                      = 141,
  LIBLTE_S1AP_IE_ID_CONCURRENTWARNINGMESSAGEINDICATOR               = 142,
  LIBLTE_S1AP_IE_ID_DATA_FORWARDING_NOT_POSSIBLE                    = 143,
  LIBLTE_S1AP_IE_ID_EXTENDEDREPETITIONPERIOD                        = 144,
  LIBLTE_S1AP_IE_ID_CELLACCESSMODE                                  = 145,
  LIBLTE_S1AP_IE_ID_CSGMEMBERSHIPSTATUS                             = 146,
  LIBLTE_S1AP_IE_ID_LPPA_PDU                                        = 147,
  LIBLTE_S1AP_IE_ID_ROUTING_ID                                      = 148,
  LIBLTE_S1AP_IE_ID_TIME_SYNCHRONIZATION_INFO                       = 149,
  LIBLTE_S1AP_IE_ID_PS_SERVICENOTAVAILABLE                          = 150,
  LIBLTE_S1AP_IE_ID_PAGINGPRIORITY                                  = 151,
  LIBLTE_S1AP_IE_ID_X2TNLCONFIGURATIONINFO                          = 152,
  LIBLTE_S1AP_IE_ID_ENBX2EXTENDEDTRANSPORTLAYERADDRESSES            = 153,
  LIBLTE_S1AP_IE_ID_GUMMEILIST                                      = 154,
  LIBLTE_S1AP_IE_ID_GW_TRANSPORTLAYERADDRESS                        = 155,
  LIBLTE_S1AP_IE_ID_CORRELATION_ID                                  = 156,
  LIBLTE_S1AP_IE_ID_SOURCEMME_GUMMEI                                = 157,
  LIBLTE_S1AP_IE_ID_MME_UE_S1AP_ID_2                                = 158,
  LIBLTE_S1AP_IE_ID_REGISTEREDLAI                                   = 159,
  LIBLTE_S1AP_IE_ID_RELAYNODE_INDICATOR                             = 160,
  LIBLTE_S1AP_IE_ID_TRAFFICLOADREDUCTIONINDICATION                  = 161,
  LIBLTE_S1AP_IE_ID_MDTCONFIGURATION                                = 162,
  LIBLTE_S1AP_IE_ID_MMERELAYSUPPORTINDICATOR                        = 163,
  LIBLTE_S1AP_IE_ID_GWCONTEXTRELEASEINDICATION                      = 164,
  LIBLTE_S1AP_IE_ID_MANAGEMENTBASEDMDTALLOWED                       = 165,
  LIBLTE_S1AP_IE_ID_PRIVACYINDICATOR                                = 166,
  LIBLTE_S1AP_IE_ID_TIME_UE_STAYEDINCELL_ENHANCEDGRANULARITY        = 167,
  LIBLTE_S1AP_IE_ID_HO_CAUSE                                        = 168,
  LIBLTE_S1AP_IE_ID_VOICESUPPORTMATCHINDICATOR                      = 169,
  LIBLTE_S1AP_IE_ID_GUMMEITYPE                                      = 170,
  LIBLTE_S1AP_IE_ID_M3CONFIGURATION                                 = 171,
  LIBLTE_S1AP_IE_ID_M4CONFIGURATION                                 = 172,
  LIBLTE_S1AP_IE_ID_M5CONFIGURATION                                 = 173,
  LIBLTE_S1AP_IE_ID_MDT_LOCATION_INFO                               = 174,
  LIBLTE_S1AP_IE_ID_MOBILITYINFORMATION                             = 175,
  LIBLTE_S1AP_IE_ID_TUNNEL_INFORMATION_FOR_BBF                      = 176,
  LIBLTE_S1AP_IE_ID_MANAGEMENTBASEDMDTPLMNLIST                      = 177,
  LIBLTE_S1AP_IE_ID_SIGNALLINGBASEDMDTPLMNLIST                      = 178,
  LIBLTE_S1AP_IE_ID_ULCOUNTVALUEEXTENDED                            = 179,
  LIBLTE_S1AP_IE_ID_DLCOUNTVALUEEXTENDED                            = 180,
  LIBLTE_S1AP_IE_ID_RECEIVESTATUSOFULPDCPSDUSEXTENDED               = 181,
  LIBLTE_S1AP_IE_ID_ECGILISTFORRESTART                              = 182,
  LIBLTE_S1AP_IE_ID_SIPTO_CORRELATION_ID                            = 183,
  LIBLTE_S1AP_IE_ID_SIPTO_L_GW_TRANSPORTLAYERADDRESS                = 184,
  LIBLTE_S1AP_IE_ID_TRANSPORTINFORMATION                            = 185,
  LIBLTE_S1AP_IE_ID_LHN_ID                                          = 186,
  LIBLTE_S1AP_IE_ID_ADDITIONALCSFALLBACKINDICATOR                   = 187,
  LIBLTE_S1AP_IE_ID_TAILISTFORRESTART                               = 188,
  LIBLTE_S1AP_IE_ID_USERLOCATIONINFORMATION                         = 189,
  LIBLTE_S1AP_IE_ID_EMERGENCYAREAIDLISTFORRESTART                   = 190,
  LIBLTE_S1AP_IE_ID_KILLALLWARNINGMESSAGES                          = 191,
  LIBLTE_S1AP_IE_N_ITEMS,
}LIBLTE_S1AP_IE_ENUM;
static const char liblte_s1ap_ie_text[LIBLTE_S1AP_IE_N_ITEMS][64] = { 
  "id-MME-UE-S1AP-ID",
  "id-HandoverType",
  "id-Cause",
  "id-SourceID",
  "id-TargetID",
  "id-spare5",
  "id-spare6",
  "id-spare7",
  "id-eNB-UE-S1AP-ID",
  "id-spare9",
  "id-spare10",
  "id-spare11",
  "id-E-RABSubjecttoDataForwardingList",
  "id-E-RABtoReleaseListHOCmd",
  "id-E-RABDataForwardingItem",
  "id-E-RABReleaseItemBearerRelComp",
  "id-E-RABToBeSetupListBearerSUReq",
  "id-E-RABToBeSetupItemBearerSUReq",
  "id-E-RABAdmittedList",
  "id-E-RABFailedToSetupListHOReqAck",
  "id-E-RABAdmittedItem",
  "id-E-RABFailedtoSetupItemHOReqAck",
  "id-E-RABToBeSwitchedDLList",
  "id-E-RABToBeSwitchedDLItem",
  "id-E-RABToBeSetupListCtxtSUReq",
  "id-TraceActivation",
  "id-NAS-PDU",
  "id-E-RABToBeSetupItemHOReq",
  "id-E-RABSetupListBearerSURes",
  "id-E-RABFailedToSetupListBearerSURes",
  "id-E-RABToBeModifiedListBearerModReq",
  "id-E-RABModifyListBearerModRes",
  "id-E-RABFailedToModifyList",
  "id-E-RABToBeReleasedList",
  "id-E-RABFailedToReleaseList",
  "id-E-RABItem",
  "id-E-RABToBeModifiedItemBearerModReq",
  "id-E-RABModifyItemBearerModRes",
  "id-E-RABReleaseItem",
  "id-E-RABSetupItemBearerSURes",
  "id-SecurityContext",
  "id-HandoverRestrictionList",
  "id-spare42",
  "id-UEPagingID",
  "id-pagingDRX",
  "id-spare45",
  "id-TAIList",
  "id-TAIItem",
  "id-E-RABFailedToSetupListCtxtSURes",
  "id-E-RABReleaseItemHOCmd",
  "id-E-RABSetupItemCtxtSURes",
  "id-E-RABSetupListCtxtSURes",
  "id-E-RABToBeSetupItemCtxtSUReq",
  "id-E-RABToBeSetupListHOReq",
  "id-spare54",
  "id-GERANtoLTEHOInformationRes",
  "id-spare56",
  "id-UTRANtoLTEHOInformationRes",
  "id-CriticalityDiagnostics",
  "id-Global-ENB-ID",
  "id-eNBname",
  "id-MMEname",
  "id-spare62",
  "id-ServedPLMNs",
  "id-SupportedTAs",
  "id-TimeToWait",
  "id-uEaggregateMaximumBitrate",
  "id-TAI",
  "id-spare68",
  "id-E-RABReleaseListBearerRelComp",
  "id-cdma2000PDU",
  "id-cdma2000RATType",
  "id-cdma2000SectorID",
  "id-SecurityKey",
  "id-UERadioCapability",
  "id-GUMMEI-ID",
  "id-spare76",
  "id-spare77",
  "id-E-RABInformationListItem",
  "id-Direct-Forwarding-Path-Availability",
  "id-UEIdentityIndexValue",
  "id-spare81",
  "id-spare82",
  "id-cdma2000HOStatus",
  "id-cdma2000HORequiredIndication",
  "id-spare85",
  "id-E-UTRAN-Trace-ID",
  "id-RelativeMMECapacity",
  "id-SourceMME-UE-S1AP-ID",
  "id-Bearers-SubjectToStatusTransfer-Item",
  "id-eNB-StatusTransfer-TransparentContainer",
  "id-UE-associatedLogicalS1-ConnectionItem",
  "id-ResetType",
  "id-UE-associatedLogicalS1-ConnectionListResAck",
  "id-E-RABToBeSwitchedULItem",
  "id-E-RABToBeSwitchedULList",
  "id-S-TMSI",
  "id-cdma2000OneXRAND",
  "id-RequestType",
  "id-UE-S1AP-IDs",
  "id-EUTRAN-CGI",
  "id-OverloadResponse",
  "id-cdma2000OneXSRVCCInfo",
  "id-E-RABFailedToBeReleasedList",
  "id-Source-ToTarget-TransparentContainer",
  "id-ServedGUMMEIs",
  "id-SubscriberProfileIDforRFP",
  "id-UESecurityCapabilities",
  "id-CSFallbackIndicator",
  "id-CNDomain",
  "id-E-RABReleasedList",
  "id-MessageIdentifier",
  "id-SerialNumber",
  "id-WarningAreaList",
  "id-RepetitionPeriod",
  "id-NumberofBroadcastRequest",
  "id-WarningType",
  "id-WarningSecurityInfo",
  "id-DataCodingScheme",
  "id-WarningMessageContents",
  "id-BroadcastCompletedAreaList",
  "id-Inter-SystemInformationTransferTypeEDT",
  "id-Inter-SystemInformationTransferTypeMDT",
  "id-Target-ToSource-TransparentContainer",
  "id-SRVCCOperationPossible",
  "id-SRVCCHOIndication",
  "id-NAS-DownlinkCount",
  "id-CSG-Id",
  "id-CSG-IdList",
  "id-SONConfigurationTransferECT",
  "id-SONConfigurationTransferMCT",
  "id-TraceCollectionEntityIPAddress",
  "id-MSClassmark2",
  "id-MSClassmark3",
  "id-RRC-Establishment-Cause",
  "id-NASSecurityParametersfromE-UTRAN",
  "id-NASSecurityParameterstoE-UTRAN",
  "id-DefaultPagingDRX",
  "id-Source-ToTarget-TransparentContainer-Secondary",
  "id-Target-ToSource-TransparentContainer-Secondary",
  "id-EUTRANRoundTripDelayEstimationInfo",
  "id-BroadcastCancelledAreaList",
  "id-ConcurrentWarningMessageIndicator",
  "id-Data-Forwarding-Not-Possible",
  "id-ExtendedRepetitionPeriod",
  "id-CellAccessMode",
  "id-CSGMembershipStatus",
  "id-LPPa-PDU",
  "id-Routing-ID",
  "id-Time-Synchronization-Info",
  "id-PS-ServiceNotAvailable",
  "id-PagingPriority",
  "id-x2TNLConfigurationInfo",
  "id-eNBX2ExtendedTransportLayerAddresses",
  "id-GUMMEIList",
  "id-GW-TransportLayerAddress",
  "id-Correlation-ID",
  "id-SourceMME-GUMMEI",
  "id-MME-UE-S1AP-ID-2",
  "id-RegisteredLAI",
  "id-RelayNode-Indicator",
  "id-TrafficLoadReductionIndication",
  "id-MDTConfiguration",
  "id-MMERelaySupportIndicator",
  "id-GWContextReleaseIndication",
  "id-ManagementBasedMDTAllowed",
  "id-PrivacyIndicator",
  "id-Time-UE-StayedInCell-EnhancedGranularity",
  "id-HO-Cause",
  "id-VoiceSupportMatchIndicator",
  "id-GUMMEIType",
  "id-M3Configuration",
  "id-M4Configuration",
  "id-M5Configuration",
  "id-MDT-Location-Info",
  "id-MobilityInformation",
  "id-Tunnel-Information-for-BBF",
  "id-ManagementBasedMDTPLMNList",
  "id-SignallingBasedMDTPLMNList",
  "id-ULCOUNTValueExtended",
  "id-DLCOUNTValueExtended",
  "id-ReceiveStatusOfULPDCPSDUsExtended",
  "id-ECGIListForRestart",
  "id-SIPTO-Correlation-ID",
  "id-SIPTO-L-GW-TransportLayerAddress",
  "id-TransportInformation",
  "id-LHN-ID",
  "id-AdditionalCSFallbackIndicator",
  "id-TAIListForRestart",
  "id-UserLocationInformation",
  "id-EmergencyAreaIDListForRestart",
  "id-KillAllWarningMessages",
};


/*******************************************************************************
/* ProtocolIE Criticality ENUMERATED
********************************************************************************/
typedef enum{
  LIBLTE_S1AP_CRITICALITY_REJECT,
  LIBLTE_S1AP_CRITICALITY_IGNORE,
  LIBLTE_S1AP_CRITICALITY_NOTIFY,
  LIBLTE_S1AP_CRITICALITY_N_ITEMS,
}LIBLTE_S1AP_CRITICALITY_ENUM;
static const char liblte_s1ap_criticality_text[LIBLTE_S1AP_CRITICALITY_N_ITEMS][80] = {
  "reject",
  "ignore",
  "notify",
};

LIBLTE_ERROR_ENUM liblte_s1ap_pack_criticality(
  LIBLTE_S1AP_CRITICALITY_ENUM                                 *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_criticality(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_CRITICALITY_ENUM                                 *ie);

/*******************************************************************************
/* ProtocolIE local INTEGER
********************************************************************************/
typedef struct{
uint16_t local;
}LIBLTE_S1AP_LOCAL_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_local(
  LIBLTE_S1AP_LOCAL_STRUCT                                     *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_local(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_LOCAL_STRUCT                                     *ie);

/*******************************************************************************
/* ProtocolIE PrivateIE_ID CHOICE
********************************************************************************/
typedef enum{
  LIBLTE_S1AP_PRIVATEIE_ID_CHOICE_LOCAL,
  LIBLTE_S1AP_PRIVATEIE_ID_CHOICE_GLOBAL,
  LIBLTE_S1AP_PRIVATEIE_ID_CHOICE_N_ITEMS,
}LIBLTE_S1AP_PRIVATEIE_ID_CHOICE_ENUM;
static const char liblte_s1ap_privateie_id_choice_text[LIBLTE_S1AP_PRIVATEIE_ID_CHOICE_N_ITEMS][50] = {
  "local",
  "global",
};

typedef union{
  LIBLTE_S1AP_LOCAL_STRUCT                                     local;
  LIBLTE_ASN1_OID_STRUCT                                       global;
}LIBLTE_S1AP_PRIVATEIE_ID_CHOICE_UNION;

typedef struct{
  LIBLTE_S1AP_PRIVATEIE_ID_CHOICE_UNION choice;
  LIBLTE_S1AP_PRIVATEIE_ID_CHOICE_ENUM  choice_type;
}LIBLTE_S1AP_PRIVATEIE_ID_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_privateie_id(
  LIBLTE_S1AP_PRIVATEIE_ID_STRUCT                              *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_privateie_id(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_PRIVATEIE_ID_STRUCT                              *ie);

/*******************************************************************************
/* ProtocolIE ProtocolExtensionID INTEGER
********************************************************************************/
typedef struct{
uint16_t ProtocolExtensionID;
}LIBLTE_S1AP_PROTOCOLEXTENSIONID_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_protocolextensionid(
  LIBLTE_S1AP_PROTOCOLEXTENSIONID_STRUCT                       *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_protocolextensionid(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_PROTOCOLEXTENSIONID_STRUCT                       *ie);

/*******************************************************************************
/* ProtocolIE TriggeringMessage ENUMERATED
********************************************************************************/
typedef enum{
  LIBLTE_S1AP_TRIGGERINGMESSAGE_INITIATING_MESSAGE,
  LIBLTE_S1AP_TRIGGERINGMESSAGE_SUCCESSFUL_OUTCOME,
  LIBLTE_S1AP_TRIGGERINGMESSAGE_UNSUCCESSFULL_OUTCOME,
  LIBLTE_S1AP_TRIGGERINGMESSAGE_N_ITEMS,
}LIBLTE_S1AP_TRIGGERINGMESSAGE_ENUM;
static const char liblte_s1ap_triggeringmessage_text[LIBLTE_S1AP_TRIGGERINGMESSAGE_N_ITEMS][80] = {
  "initiating-message",
  "successful-outcome",
  "unsuccessfull-outcome",
};

LIBLTE_ERROR_ENUM liblte_s1ap_pack_triggeringmessage(
  LIBLTE_S1AP_TRIGGERINGMESSAGE_ENUM                           *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_triggeringmessage(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_TRIGGERINGMESSAGE_ENUM                           *ie);

/*******************************************************************************
/* ProtocolIE Presence ENUMERATED
********************************************************************************/
typedef enum{
  LIBLTE_S1AP_PRESENCE_OPTIONAL,
  LIBLTE_S1AP_PRESENCE_CONDITIONAL,
  LIBLTE_S1AP_PRESENCE_MANDATORY,
  LIBLTE_S1AP_PRESENCE_N_ITEMS,
}LIBLTE_S1AP_PRESENCE_ENUM;
static const char liblte_s1ap_presence_text[LIBLTE_S1AP_PRESENCE_N_ITEMS][80] = {
  "optional",
  "conditional",
  "mandatory",
};

LIBLTE_ERROR_ENUM liblte_s1ap_pack_presence(
  LIBLTE_S1AP_PRESENCE_ENUM                                    *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_presence(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_PRESENCE_ENUM                                    *ie);

/*******************************************************************************
/* ProtocolIE ProtocolIE_ID INTEGER
********************************************************************************/
typedef struct{
uint16_t ProtocolIE_ID;
}LIBLTE_S1AP_PROTOCOLIE_ID_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_protocolie_id(
  LIBLTE_S1AP_PROTOCOLIE_ID_STRUCT                             *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_protocolie_id(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_PROTOCOLIE_ID_STRUCT                             *ie);

/*******************************************************************************
/* ProtocolIE ProcedureCode INTEGER
********************************************************************************/
typedef struct{
uint8_t ProcedureCode;
}LIBLTE_S1AP_PROCEDURECODE_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_procedurecode(
  LIBLTE_S1AP_PROCEDURECODE_STRUCT                             *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_procedurecode(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_PROCEDURECODE_STRUCT                             *ie);

/*******************************************************************************
/* ProtocolIE ProtocolIE_Field SEQUENCE
********************************************************************************/
typedef struct{
  LIBLTE_S1AP_PROTOCOLIE_ID_STRUCT                             id;
  LIBLTE_S1AP_CRITICALITY_ENUM                                 criticality;
  LIBLTE_ASN1_OPEN_TYPE_STRUCT                                 value;
}LIBLTE_S1AP_PROTOCOLIE_FIELD_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_protocolie_field(
  LIBLTE_S1AP_PROTOCOLIE_FIELD_STRUCT                          *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_protocolie_field(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_PROTOCOLIE_FIELD_STRUCT                          *ie);

/*******************************************************************************
/* ProtocolIE ProtocolExtensionField SEQUENCE
********************************************************************************/
typedef struct{
  LIBLTE_S1AP_PROTOCOLEXTENSIONID_STRUCT                       id;
  LIBLTE_S1AP_CRITICALITY_ENUM                                 criticality;
  LIBLTE_ASN1_OPEN_TYPE_STRUCT                                 extensionValue;
}LIBLTE_S1AP_PROTOCOLEXTENSIONFIELD_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_protocolextensionfield(
  LIBLTE_S1AP_PROTOCOLEXTENSIONFIELD_STRUCT                    *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_protocolextensionfield(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_PROTOCOLEXTENSIONFIELD_STRUCT                    *ie);

/*******************************************************************************
/* ProtocolIE ProtocolIE_FieldPair SEQUENCE
********************************************************************************/
typedef struct{
  LIBLTE_S1AP_PROTOCOLIE_ID_STRUCT                             id;
  LIBLTE_S1AP_CRITICALITY_ENUM                                 firstCriticality;
  LIBLTE_ASN1_OPEN_TYPE_STRUCT                                 firstValue;
  LIBLTE_S1AP_CRITICALITY_ENUM                                 secondCriticality;
  LIBLTE_ASN1_OPEN_TYPE_STRUCT                                 secondValue;
}LIBLTE_S1AP_PROTOCOLIE_FIELDPAIR_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_protocolie_fieldpair(
  LIBLTE_S1AP_PROTOCOLIE_FIELDPAIR_STRUCT                      *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_protocolie_fieldpair(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_PROTOCOLIE_FIELDPAIR_STRUCT                      *ie);

/*******************************************************************************
/* ProtocolIE ProtocolExtensionContainer DYNAMIC SEQUENCE OF
********************************************************************************/
// lb:1, ub:65535
typedef struct{
  uint32_t                                                     len;
  LIBLTE_S1AP_PROTOCOLEXTENSIONFIELD_STRUCT                    buffer[32]; //WARNING: Artificial limit to reduce memory footprint
}LIBLTE_S1AP_PROTOCOLEXTENSIONCONTAINER_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_protocolextensioncontainer(
  LIBLTE_S1AP_PROTOCOLEXTENSIONCONTAINER_STRUCT                *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_protocolextensioncontainer(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_PROTOCOLEXTENSIONCONTAINER_STRUCT                *ie);

/*******************************************************************************
/* ProtocolIE ProtocolIE_ContainerPair DYNAMIC SEQUENCE OF
********************************************************************************/
// lb:0, ub:65535
typedef struct{
  uint32_t                                                     len;
  LIBLTE_S1AP_PROTOCOLIE_FIELDPAIR_STRUCT                      buffer[32]; //WARNING: Artificial limit to reduce memory footprint
}LIBLTE_S1AP_PROTOCOLIE_CONTAINERPAIR_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_protocolie_containerpair(
  LIBLTE_S1AP_PROTOCOLIE_CONTAINERPAIR_STRUCT                  *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_protocolie_containerpair(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_PROTOCOLIE_CONTAINERPAIR_STRUCT                  *ie);

/*******************************************************************************
/* ProtocolIE ProtocolIE_ContainerPairList DYNAMIC SEQUENCE OF
********************************************************************************/
// lb:None, ub:None
typedef struct{
  uint32_t                                                     len;
  LIBLTE_S1AP_PROTOCOLIE_CONTAINERPAIR_STRUCT                  buffer[32]; //WARNING: Artificial limit to reduce memory footprint
}LIBLTE_S1AP_PROTOCOLIE_CONTAINERPAIRLIST_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_protocolie_containerpairlist(
  LIBLTE_S1AP_PROTOCOLIE_CONTAINERPAIRLIST_STRUCT              *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_protocolie_containerpairlist(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_PROTOCOLIE_CONTAINERPAIRLIST_STRUCT              *ie);

/*******************************************************************************
/* ProtocolIE PrivateIE_Field SEQUENCE
********************************************************************************/
typedef struct{
  LIBLTE_S1AP_PRIVATEIE_ID_STRUCT                              id;
  LIBLTE_S1AP_CRITICALITY_ENUM                                 criticality;
  LIBLTE_ASN1_OPEN_TYPE_STRUCT                                 value;
}LIBLTE_S1AP_PRIVATEIE_FIELD_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_privateie_field(
  LIBLTE_S1AP_PRIVATEIE_FIELD_STRUCT                           *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_privateie_field(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_PRIVATEIE_FIELD_STRUCT                           *ie);

/*******************************************************************************
/* ProtocolIE ProtocolIE_SingleContainer SEQUENCE
********************************************************************************/
typedef struct{
  LIBLTE_S1AP_PROTOCOLIE_ID_STRUCT                             id;
  LIBLTE_S1AP_CRITICALITY_ENUM                                 criticality;
  LIBLTE_ASN1_OPEN_TYPE_STRUCT                                 value;
}LIBLTE_S1AP_PROTOCOLIE_SINGLECONTAINER_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_protocolie_singlecontainer(
  LIBLTE_S1AP_PROTOCOLIE_SINGLECONTAINER_STRUCT                *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_protocolie_singlecontainer(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_PROTOCOLIE_SINGLECONTAINER_STRUCT                *ie);

/*******************************************************************************
/* ProtocolIE PrivateIE_Container DYNAMIC SEQUENCE OF
********************************************************************************/
// lb:1, ub:65535
typedef struct{
  uint32_t                                                     len;
  LIBLTE_S1AP_PRIVATEIE_FIELD_STRUCT                           buffer[32]; //WARNING: Artificial limit to reduce memory footprint
}LIBLTE_S1AP_PRIVATEIE_CONTAINER_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_privateie_container(
  LIBLTE_S1AP_PRIVATEIE_CONTAINER_STRUCT                       *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_privateie_container(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_PRIVATEIE_CONTAINER_STRUCT                       *ie);

/*******************************************************************************
/* ProtocolIE BitRate INTEGER
********************************************************************************/
typedef struct{
uint32_t BitRate;
}LIBLTE_S1AP_BITRATE_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_bitrate(
  LIBLTE_S1AP_BITRATE_STRUCT                                   *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_bitrate(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_BITRATE_STRUCT                                   *ie);

/*******************************************************************************
/* ProtocolIE CauseMisc ENUMERATED
********************************************************************************/
typedef enum{
  LIBLTE_S1AP_CAUSEMISC_CONTROL_PROCESSING_OVERLOAD,
  LIBLTE_S1AP_CAUSEMISC_NOT_ENOUGH_USER_PLANE_PROCESSING_RESOURCES,
  LIBLTE_S1AP_CAUSEMISC_HARDWARE_FAILURE,
  LIBLTE_S1AP_CAUSEMISC_OM_INTERVENTION,
  LIBLTE_S1AP_CAUSEMISC_UNSPECIFIED,
  LIBLTE_S1AP_CAUSEMISC_UNKNOWN_PLMN,
  LIBLTE_S1AP_CAUSEMISC_N_ITEMS,
}LIBLTE_S1AP_CAUSEMISC_ENUM;
static const char liblte_s1ap_causemisc_text[LIBLTE_S1AP_CAUSEMISC_N_ITEMS][80] = {
  "control-processing-overload",
  "not-enough-user-plane-processing-resources",
  "hardware-failure",
  "om-intervention",
  "unspecified",
  "unknown-PLMN",
};

typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_CAUSEMISC_ENUM e;
}LIBLTE_S1AP_CAUSEMISC_ENUM_EXT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_causemisc(
  LIBLTE_S1AP_CAUSEMISC_ENUM_EXT                               *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_causemisc(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_CAUSEMISC_ENUM_EXT                               *ie);

/*******************************************************************************
/* ProtocolIE CauseRadioNetwork ENUMERATED
********************************************************************************/
typedef enum{
  LIBLTE_S1AP_CAUSERADIONETWORK_UNSPECIFIED,
  LIBLTE_S1AP_CAUSERADIONETWORK_TX2RELOCOVERALL_EXPIRY,
  LIBLTE_S1AP_CAUSERADIONETWORK_SUCCESSFUL_HANDOVER,
  LIBLTE_S1AP_CAUSERADIONETWORK_RELEASE_DUE_TO_EUTRAN_GENERATED_REASON,
  LIBLTE_S1AP_CAUSERADIONETWORK_HANDOVER_CANCELLED,
  LIBLTE_S1AP_CAUSERADIONETWORK_PARTIAL_HANDOVER,
  LIBLTE_S1AP_CAUSERADIONETWORK_HO_FAILURE_IN_TARGET_EPC_ENB_OR_TARGET_SYSTEM,
  LIBLTE_S1AP_CAUSERADIONETWORK_HO_TARGET_NOT_ALLOWED,
  LIBLTE_S1AP_CAUSERADIONETWORK_TS1RELOCOVERALL_EXPIRY,
  LIBLTE_S1AP_CAUSERADIONETWORK_TS1RELOCPREP_EXPIRY,
  LIBLTE_S1AP_CAUSERADIONETWORK_CELL_NOT_AVAILABLE,
  LIBLTE_S1AP_CAUSERADIONETWORK_UNKNOWN_TARGETID,
  LIBLTE_S1AP_CAUSERADIONETWORK_NO_RADIO_RESOURCES_AVAILABLE_IN_TARGET_CELL,
  LIBLTE_S1AP_CAUSERADIONETWORK_UNKNOWN_MME_UE_S1AP_ID,
  LIBLTE_S1AP_CAUSERADIONETWORK_UNKNOWN_ENB_UE_S1AP_ID,
  LIBLTE_S1AP_CAUSERADIONETWORK_UNKNOWN_PAIR_UE_S1AP_ID,
  LIBLTE_S1AP_CAUSERADIONETWORK_HANDOVER_DESIRABLE_FOR_RADIO_REASON,
  LIBLTE_S1AP_CAUSERADIONETWORK_TIME_CRITICAL_HANDOVER,
  LIBLTE_S1AP_CAUSERADIONETWORK_RESOURCE_OPTIMISATION_HANDOVER,
  LIBLTE_S1AP_CAUSERADIONETWORK_REDUCE_LOAD_IN_SERVING_CELL,
  LIBLTE_S1AP_CAUSERADIONETWORK_USER_INACTIVITY,
  LIBLTE_S1AP_CAUSERADIONETWORK_RADIO_CONNECTION_WITH_UE_LOST,
  LIBLTE_S1AP_CAUSERADIONETWORK_LOAD_BALANCING_TAU_REQUIRED,
  LIBLTE_S1AP_CAUSERADIONETWORK_CS_FALLBACK_TRIGGERED,
  LIBLTE_S1AP_CAUSERADIONETWORK_UE_NOT_AVAILABLE_FOR_PS_SERVICE,
  LIBLTE_S1AP_CAUSERADIONETWORK_RADIO_RESOURCES_NOT_AVAILABLE,
  LIBLTE_S1AP_CAUSERADIONETWORK_FAILURE_IN_RADIO_INTERFACE_PROCEDURE,
  LIBLTE_S1AP_CAUSERADIONETWORK_INVALID_QOS_COMBINATION,
  LIBLTE_S1AP_CAUSERADIONETWORK_INTERRAT_REDIRECTION,
  LIBLTE_S1AP_CAUSERADIONETWORK_INTERACTION_WITH_OTHER_PROCEDURE,
  LIBLTE_S1AP_CAUSERADIONETWORK_UNKNOWN_E_RAB_ID,
  LIBLTE_S1AP_CAUSERADIONETWORK_MULTIPLE_E_RAB_ID_INSTANCES,
  LIBLTE_S1AP_CAUSERADIONETWORK_ENCRYPTION_AND_OR_INTEGRITY_PROTECTION_ALGORITHMS_NOT_SUPPORTED,
  LIBLTE_S1AP_CAUSERADIONETWORK_S1_INTRA_SYSTEM_HANDOVER_TRIGGERED,
  LIBLTE_S1AP_CAUSERADIONETWORK_S1_INTER_SYSTEM_HANDOVER_TRIGGERED,
  LIBLTE_S1AP_CAUSERADIONETWORK_X2_HANDOVER_TRIGGERED,
  LIBLTE_S1AP_CAUSERADIONETWORK_REDIRECTION_TOWARDS_1XRTT,
  LIBLTE_S1AP_CAUSERADIONETWORK_NOT_SUPPORTED_QCI_VALUE,
  LIBLTE_S1AP_CAUSERADIONETWORK_INVALID_CSG_ID,
  LIBLTE_S1AP_CAUSERADIONETWORK_N_ITEMS,
}LIBLTE_S1AP_CAUSERADIONETWORK_ENUM;
static const char liblte_s1ap_causeradionetwork_text[LIBLTE_S1AP_CAUSERADIONETWORK_N_ITEMS][80] = {
  "unspecified",
  "tx2relocoverall-expiry",
  "successful-handover",
  "release-due-to-eutran-generated-reason",
  "handover-cancelled",
  "partial-handover",
  "ho-failure-in-target-EPC-eNB-or-target-system",
  "ho-target-not-allowed",
  "tS1relocoverall-expiry",
  "tS1relocprep-expiry",
  "cell-not-available",
  "unknown-targetID",
  "no-radio-resources-available-in-target-cell",
  "unknown-mme-ue-s1ap-id",
  "unknown-enb-ue-s1ap-id",
  "unknown-pair-ue-s1ap-id",
  "handover-desirable-for-radio-reason",
  "time-critical-handover",
  "resource-optimisation-handover",
  "reduce-load-in-serving-cell",
  "user-inactivity",
  "radio-connection-with-ue-lost",
  "load-balancing-tau-required",
  "cs-fallback-triggered",
  "ue-not-available-for-ps-service",
  "radio-resources-not-available",
  "failure-in-radio-interface-procedure",
  "invalid-qos-combination",
  "interrat-redirection",
  "interaction-with-other-procedure",
  "unknown-E-RAB-ID",
  "multiple-E-RAB-ID-instances",
  "encryption-and-or-integrity-protection-algorithms-not-supported",
  "s1-intra-system-handover-triggered",
  "s1-inter-system-handover-triggered",
  "x2-handover-triggered",
  "redirection-towards-1xRTT",
  "not-supported-QCI-value",
  "invalid-CSG-Id",
};

typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_CAUSERADIONETWORK_ENUM e;
}LIBLTE_S1AP_CAUSERADIONETWORK_ENUM_EXT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_causeradionetwork(
  LIBLTE_S1AP_CAUSERADIONETWORK_ENUM_EXT                       *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_causeradionetwork(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_CAUSERADIONETWORK_ENUM_EXT                       *ie);

/*******************************************************************************
/* ProtocolIE CauseNas ENUMERATED
********************************************************************************/
typedef enum{
  LIBLTE_S1AP_CAUSENAS_NORMAL_RELEASE,
  LIBLTE_S1AP_CAUSENAS_AUTHENTICATION_FAILURE,
  LIBLTE_S1AP_CAUSENAS_DETACH,
  LIBLTE_S1AP_CAUSENAS_UNSPECIFIED,
  LIBLTE_S1AP_CAUSENAS_CSG_SUBSCRIPTION_EXPIRY,
  LIBLTE_S1AP_CAUSENAS_N_ITEMS,
}LIBLTE_S1AP_CAUSENAS_ENUM;
static const char liblte_s1ap_causenas_text[LIBLTE_S1AP_CAUSENAS_N_ITEMS][80] = {
  "normal-release",
  "authentication-failure",
  "detach",
  "unspecified",
  "csg-subscription-expiry",
};

typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_CAUSENAS_ENUM e;
}LIBLTE_S1AP_CAUSENAS_ENUM_EXT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_causenas(
  LIBLTE_S1AP_CAUSENAS_ENUM_EXT                                *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_causenas(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_CAUSENAS_ENUM_EXT                                *ie);

/*******************************************************************************
/* ProtocolIE CellIdentity STATIC BIT STRING
********************************************************************************/
#define LIBLTE_S1AP_CELLIDENTITY_BIT_STRING_LEN 28
typedef struct{
  uint8_t  buffer[28];
}LIBLTE_S1AP_CELLIDENTITY_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_cellidentity(
  LIBLTE_S1AP_CELLIDENTITY_STRUCT                              *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_cellidentity(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_CELLIDENTITY_STRUCT                              *ie);

/*******************************************************************************
/* ProtocolIE Cdma2000PDU DYNAMIC OCTET STRING
********************************************************************************/
// lb:0, ub:16318
typedef struct{
  uint32_t n_octets;
  uint8_t  buffer[16318];
}LIBLTE_S1AP_CDMA2000PDU_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_cdma2000pdu(
  LIBLTE_S1AP_CDMA2000PDU_STRUCT                               *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_cdma2000pdu(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_CDMA2000PDU_STRUCT                               *ie);

/*******************************************************************************
/* ProtocolIE Cdma2000SectorID DYNAMIC OCTET STRING
********************************************************************************/
// lb:0, ub:16318
typedef struct{
  uint32_t n_octets;
  uint8_t  buffer[16318];
}LIBLTE_S1AP_CDMA2000SECTORID_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_cdma2000sectorid(
  LIBLTE_S1AP_CDMA2000SECTORID_STRUCT                          *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_cdma2000sectorid(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_CDMA2000SECTORID_STRUCT                          *ie);

/*******************************************************************************
/* ProtocolIE Cdma2000HORequiredIndication ENUMERATED
********************************************************************************/
typedef enum{
  LIBLTE_S1AP_CDMA2000HOREQUIREDINDICATION_TRUE,
  LIBLTE_S1AP_CDMA2000HOREQUIREDINDICATION_N_ITEMS,
}LIBLTE_S1AP_CDMA2000HOREQUIREDINDICATION_ENUM;
static const char liblte_s1ap_cdma2000horequiredindication_text[LIBLTE_S1AP_CDMA2000HOREQUIREDINDICATION_N_ITEMS][80] = {
  "true",
};

typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_CDMA2000HOREQUIREDINDICATION_ENUM e;
}LIBLTE_S1AP_CDMA2000HOREQUIREDINDICATION_ENUM_EXT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_cdma2000horequiredindication(
  LIBLTE_S1AP_CDMA2000HOREQUIREDINDICATION_ENUM_EXT            *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_cdma2000horequiredindication(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_CDMA2000HOREQUIREDINDICATION_ENUM_EXT            *ie);

/*******************************************************************************
/* ProtocolIE Cdma2000OneXMSI DYNAMIC OCTET STRING
********************************************************************************/
// lb:0, ub:16318
typedef struct{
  uint32_t n_octets;
  uint8_t  buffer[16318];
}LIBLTE_S1AP_CDMA2000ONEXMSI_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_cdma2000onexmsi(
  LIBLTE_S1AP_CDMA2000ONEXMSI_STRUCT                           *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_cdma2000onexmsi(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_CDMA2000ONEXMSI_STRUCT                           *ie);

/*******************************************************************************
/* ProtocolIE Cdma2000OneXRAND DYNAMIC OCTET STRING
********************************************************************************/
// lb:0, ub:16318
typedef struct{
  uint32_t n_octets;
  uint8_t  buffer[16318];
}LIBLTE_S1AP_CDMA2000ONEXRAND_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_cdma2000onexrand(
  LIBLTE_S1AP_CDMA2000ONEXRAND_STRUCT                          *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_cdma2000onexrand(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_CDMA2000ONEXRAND_STRUCT                          *ie);

/*******************************************************************************
/* ProtocolIE CNDomain ENUMERATED
********************************************************************************/
typedef enum{
  LIBLTE_S1AP_CNDOMAIN_PS,
  LIBLTE_S1AP_CNDOMAIN_CS,
  LIBLTE_S1AP_CNDOMAIN_N_ITEMS,
}LIBLTE_S1AP_CNDOMAIN_ENUM;
static const char liblte_s1ap_cndomain_text[LIBLTE_S1AP_CNDOMAIN_N_ITEMS][80] = {
  "ps",
  "cs",
};

LIBLTE_ERROR_ENUM liblte_s1ap_pack_cndomain(
  LIBLTE_S1AP_CNDOMAIN_ENUM                                    *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_cndomain(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_CNDOMAIN_ENUM                                    *ie);

/*******************************************************************************
/* ProtocolIE Correlation_ID STATIC OCTET STRING
********************************************************************************/
#define LIBLTE_S1AP_CORRELATION_ID_OCTET_STRING_LEN 4
typedef struct{
  uint8_t  buffer[4];
}LIBLTE_S1AP_CORRELATION_ID_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_correlation_id(
  LIBLTE_S1AP_CORRELATION_ID_STRUCT                            *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_correlation_id(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_CORRELATION_ID_STRUCT                            *ie);

/*******************************************************************************
/* ProtocolIE AdditionalCSFallbackIndicator ENUMERATED
********************************************************************************/
typedef enum{
  LIBLTE_S1AP_ADDITIONALCSFALLBACKINDICATOR_NO_RESTRICTION,
  LIBLTE_S1AP_ADDITIONALCSFALLBACKINDICATOR_RESTRICTION,
  LIBLTE_S1AP_ADDITIONALCSFALLBACKINDICATOR_N_ITEMS,
}LIBLTE_S1AP_ADDITIONALCSFALLBACKINDICATOR_ENUM;
static const char liblte_s1ap_additionalcsfallbackindicator_text[LIBLTE_S1AP_ADDITIONALCSFALLBACKINDICATOR_N_ITEMS][80] = {
  "no-restriction",
  "restriction",
};

typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_ADDITIONALCSFALLBACKINDICATOR_ENUM e;
}LIBLTE_S1AP_ADDITIONALCSFALLBACKINDICATOR_ENUM_EXT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_additionalcsfallbackindicator(
  LIBLTE_S1AP_ADDITIONALCSFALLBACKINDICATOR_ENUM_EXT           *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_additionalcsfallbackindicator(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_ADDITIONALCSFALLBACKINDICATOR_ENUM_EXT           *ie);

/*******************************************************************************
/* ProtocolIE DL_Forwarding ENUMERATED
********************************************************************************/
typedef enum{
  LIBLTE_S1AP_DL_FORWARDING_DL_FORWARDING_PROPOSED,
  LIBLTE_S1AP_DL_FORWARDING_N_ITEMS,
}LIBLTE_S1AP_DL_FORWARDING_ENUM;
static const char liblte_s1ap_dl_forwarding_text[LIBLTE_S1AP_DL_FORWARDING_N_ITEMS][80] = {
  "dL-Forwarding-proposed",
};

typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_DL_FORWARDING_ENUM e;
}LIBLTE_S1AP_DL_FORWARDING_ENUM_EXT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_dl_forwarding(
  LIBLTE_S1AP_DL_FORWARDING_ENUM_EXT                           *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_dl_forwarding(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_DL_FORWARDING_ENUM_EXT                           *ie);

/*******************************************************************************
/* ProtocolIE Data_Forwarding_Not_Possible ENUMERATED
********************************************************************************/
typedef enum{
  LIBLTE_S1AP_DATA_FORWARDING_NOT_POSSIBLE_DATA_FORWARDING_NOT_POSSIBLE,
  LIBLTE_S1AP_DATA_FORWARDING_NOT_POSSIBLE_N_ITEMS,
}LIBLTE_S1AP_DATA_FORWARDING_NOT_POSSIBLE_ENUM;
static const char liblte_s1ap_data_forwarding_not_possible_text[LIBLTE_S1AP_DATA_FORWARDING_NOT_POSSIBLE_N_ITEMS][80] = {
  "data-Forwarding-not-Possible",
};

typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_DATA_FORWARDING_NOT_POSSIBLE_ENUM e;
}LIBLTE_S1AP_DATA_FORWARDING_NOT_POSSIBLE_ENUM_EXT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_data_forwarding_not_possible(
  LIBLTE_S1AP_DATA_FORWARDING_NOT_POSSIBLE_ENUM_EXT            *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_data_forwarding_not_possible(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_DATA_FORWARDING_NOT_POSSIBLE_ENUM_EXT            *ie);

/*******************************************************************************
/* ProtocolIE EmergencyAreaID STATIC OCTET STRING
********************************************************************************/
#define LIBLTE_S1AP_EMERGENCYAREAID_OCTET_STRING_LEN 3
typedef struct{
  uint8_t  buffer[3];
}LIBLTE_S1AP_EMERGENCYAREAID_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_emergencyareaid(
  LIBLTE_S1AP_EMERGENCYAREAID_STRUCT                           *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_emergencyareaid(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_EMERGENCYAREAID_STRUCT                           *ie);

/*******************************************************************************
/* ProtocolIE macroENB_ID STATIC BIT STRING
********************************************************************************/
#define LIBLTE_S1AP_MACROENB_ID_BIT_STRING_LEN 20
typedef struct{
  uint8_t  buffer[20];
}LIBLTE_S1AP_MACROENB_ID_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_macroenb_id(
  LIBLTE_S1AP_MACROENB_ID_STRUCT                               *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_macroenb_id(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MACROENB_ID_STRUCT                               *ie);

/*******************************************************************************
/* ProtocolIE homeENB_ID STATIC BIT STRING
********************************************************************************/
#define LIBLTE_S1AP_HOMEENB_ID_BIT_STRING_LEN 28
typedef struct{
  uint8_t  buffer[28];
}LIBLTE_S1AP_HOMEENB_ID_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_homeenb_id(
  LIBLTE_S1AP_HOMEENB_ID_STRUCT                                *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_homeenb_id(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_HOMEENB_ID_STRUCT                                *ie);

/*******************************************************************************
/* ProtocolIE ENB_ID CHOICE
********************************************************************************/
typedef enum{
  LIBLTE_S1AP_ENB_ID_CHOICE_MACROENB_ID,
  LIBLTE_S1AP_ENB_ID_CHOICE_HOMEENB_ID,
  LIBLTE_S1AP_ENB_ID_CHOICE_N_ITEMS,
}LIBLTE_S1AP_ENB_ID_CHOICE_ENUM;
static const char liblte_s1ap_enb_id_choice_text[LIBLTE_S1AP_ENB_ID_CHOICE_N_ITEMS][50] = {
  "macroENB_ID",
  "homeENB_ID",
};

typedef union{
  LIBLTE_S1AP_MACROENB_ID_STRUCT                               macroENB_ID;
  LIBLTE_S1AP_HOMEENB_ID_STRUCT                                homeENB_ID;
}LIBLTE_S1AP_ENB_ID_CHOICE_UNION;

typedef struct{
  bool                            ext;
  LIBLTE_S1AP_ENB_ID_CHOICE_UNION choice;
  LIBLTE_S1AP_ENB_ID_CHOICE_ENUM  choice_type;
}LIBLTE_S1AP_ENB_ID_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_enb_id(
  LIBLTE_S1AP_ENB_ID_STRUCT                                    *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_enb_id(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_ENB_ID_STRUCT                                    *ie);

/*******************************************************************************
/* ProtocolIE ENBname PrintableString
********************************************************************************/
typedef struct{
  bool     ext;
  uint32_t n_octets;
  uint8_t  buffer[150];
}LIBLTE_S1AP_ENBNAME_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_enbname(
  LIBLTE_S1AP_ENBNAME_STRUCT                                   *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_enbname(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_ENBNAME_STRUCT                                   *ie);

/*******************************************************************************
/* ProtocolIE EncryptionAlgorithms STATIC BIT STRING
********************************************************************************/
#define LIBLTE_S1AP_ENCRYPTIONALGORITHMS_BIT_STRING_LEN 16
typedef struct{
  bool     ext;
  uint8_t  buffer[16];
}LIBLTE_S1AP_ENCRYPTIONALGORITHMS_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_encryptionalgorithms(
  LIBLTE_S1AP_ENCRYPTIONALGORITHMS_STRUCT                      *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_encryptionalgorithms(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_ENCRYPTIONALGORITHMS_STRUCT                      *ie);

/*******************************************************************************
/* ProtocolIE EventType ENUMERATED
********************************************************************************/
typedef enum{
  LIBLTE_S1AP_EVENTTYPE_DIRECT,
  LIBLTE_S1AP_EVENTTYPE_CHANGE_OF_SERVE_CELL,
  LIBLTE_S1AP_EVENTTYPE_STOP_CHANGE_OF_SERVE_CELL,
  LIBLTE_S1AP_EVENTTYPE_N_ITEMS,
}LIBLTE_S1AP_EVENTTYPE_ENUM;
static const char liblte_s1ap_eventtype_text[LIBLTE_S1AP_EVENTTYPE_N_ITEMS][80] = {
  "direct",
  "change-of-serve-cell",
  "stop-change-of-serve-cell",
};

typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_EVENTTYPE_ENUM e;
}LIBLTE_S1AP_EVENTTYPE_ENUM_EXT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_eventtype(
  LIBLTE_S1AP_EVENTTYPE_ENUM_EXT                               *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_eventtype(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_EVENTTYPE_ENUM_EXT                               *ie);

/*******************************************************************************
/* ProtocolIE ExtendedRNC_ID INTEGER
********************************************************************************/
typedef struct{
uint16_t ExtendedRNC_ID;
}LIBLTE_S1AP_EXTENDEDRNC_ID_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_extendedrnc_id(
  LIBLTE_S1AP_EXTENDEDRNC_ID_STRUCT                            *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_extendedrnc_id(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_EXTENDEDRNC_ID_STRUCT                            *ie);

/*******************************************************************************
/* ProtocolIE ForbiddenInterRATs ENUMERATED
********************************************************************************/
typedef enum{
  LIBLTE_S1AP_FORBIDDENINTERRATS_ALL,
  LIBLTE_S1AP_FORBIDDENINTERRATS_GERAN,
  LIBLTE_S1AP_FORBIDDENINTERRATS_UTRAN,
  LIBLTE_S1AP_FORBIDDENINTERRATS_CDMA2000,
  LIBLTE_S1AP_FORBIDDENINTERRATS_GERANANDUTRAN,
  LIBLTE_S1AP_FORBIDDENINTERRATS_CDMA2000ANDUTRAN,
  LIBLTE_S1AP_FORBIDDENINTERRATS_N_ITEMS,
}LIBLTE_S1AP_FORBIDDENINTERRATS_ENUM;
static const char liblte_s1ap_forbiddeninterrats_text[LIBLTE_S1AP_FORBIDDENINTERRATS_N_ITEMS][80] = {
  "all",
  "geran",
  "utran",
  "cdma2000",
  "geranandutran",
  "cdma2000andutran",
};

typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_FORBIDDENINTERRATS_ENUM e;
}LIBLTE_S1AP_FORBIDDENINTERRATS_ENUM_EXT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_forbiddeninterrats(
  LIBLTE_S1AP_FORBIDDENINTERRATS_ENUM_EXT                      *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_forbiddeninterrats(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_FORBIDDENINTERRATS_ENUM_EXT                      *ie);

/*******************************************************************************
/* ProtocolIE GWContextReleaseIndication ENUMERATED
********************************************************************************/
typedef enum{
  LIBLTE_S1AP_GWCONTEXTRELEASEINDICATION_TRUE,
  LIBLTE_S1AP_GWCONTEXTRELEASEINDICATION_N_ITEMS,
}LIBLTE_S1AP_GWCONTEXTRELEASEINDICATION_ENUM;
static const char liblte_s1ap_gwcontextreleaseindication_text[LIBLTE_S1AP_GWCONTEXTRELEASEINDICATION_N_ITEMS][80] = {
  "true",
};

typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_GWCONTEXTRELEASEINDICATION_ENUM e;
}LIBLTE_S1AP_GWCONTEXTRELEASEINDICATION_ENUM_EXT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_gwcontextreleaseindication(
  LIBLTE_S1AP_GWCONTEXTRELEASEINDICATION_ENUM_EXT              *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_gwcontextreleaseindication(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_GWCONTEXTRELEASEINDICATION_ENUM_EXT              *ie);

/*******************************************************************************
/* ProtocolIE HFN INTEGER
********************************************************************************/
typedef struct{
uint32_t HFN;
}LIBLTE_S1AP_HFN_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_hfn(
  LIBLTE_S1AP_HFN_STRUCT                                       *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_hfn(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_HFN_STRUCT                                       *ie);

/*******************************************************************************
/* ProtocolIE IMSI DYNAMIC OCTET STRING
********************************************************************************/
// lb:3, ub:8
typedef struct{
  uint32_t n_octets;
  uint8_t  buffer[8];
}LIBLTE_S1AP_IMSI_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_imsi(
  LIBLTE_S1AP_IMSI_STRUCT                                      *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_imsi(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_IMSI_STRUCT                                      *ie);

/*******************************************************************************
/* ProtocolIE InterfacesToTrace STATIC BIT STRING
********************************************************************************/
#define LIBLTE_S1AP_INTERFACESTOTRACE_BIT_STRING_LEN 8
typedef struct{
  uint8_t  buffer[8];
}LIBLTE_S1AP_INTERFACESTOTRACE_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_interfacestotrace(
  LIBLTE_S1AP_INTERFACESTOTRACE_STRUCT                         *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_interfacestotrace(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_INTERFACESTOTRACE_STRUCT                         *ie);

/*******************************************************************************
/* ProtocolIE LAC STATIC OCTET STRING
********************************************************************************/
#define LIBLTE_S1AP_LAC_OCTET_STRING_LEN 2
typedef struct{
  uint8_t  buffer[2];
}LIBLTE_S1AP_LAC_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_lac(
  LIBLTE_S1AP_LAC_STRUCT                                       *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_lac(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_LAC_STRUCT                                       *ie);

/*******************************************************************************
/* ProtocolIE LastVisitedUTRANCellInformation DYNAMIC OCTET STRING
********************************************************************************/
// lb:0, ub:16318
typedef struct{
  uint32_t n_octets;
  uint8_t  buffer[16318];
}LIBLTE_S1AP_LASTVISITEDUTRANCELLINFORMATION_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_lastvisitedutrancellinformation(
  LIBLTE_S1AP_LASTVISITEDUTRANCELLINFORMATION_STRUCT           *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_lastvisitedutrancellinformation(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_LASTVISITEDUTRANCELLINFORMATION_STRUCT           *ie);

/*******************************************************************************
/* ProtocolIE L3_Information DYNAMIC OCTET STRING
********************************************************************************/
// lb:0, ub:16318
typedef struct{
  uint32_t n_octets;
  uint8_t  buffer[16318];
}LIBLTE_S1AP_L3_INFORMATION_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_l3_information(
  LIBLTE_S1AP_L3_INFORMATION_STRUCT                            *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_l3_information(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_L3_INFORMATION_STRUCT                            *ie);

/*******************************************************************************
/* ProtocolIE LHN_ID DYNAMIC OCTET STRING
********************************************************************************/
// lb:32, ub:256
typedef struct{
  uint32_t n_octets;
  uint8_t  buffer[256];
}LIBLTE_S1AP_LHN_ID_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_lhn_id(
  LIBLTE_S1AP_LHN_ID_STRUCT                                    *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_lhn_id(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_LHN_ID_STRUCT                                    *ie);

/*******************************************************************************
/* ProtocolIE LoggingDuration ENUMERATED
********************************************************************************/
typedef enum{
  LIBLTE_S1AP_LOGGINGDURATION_M10,
  LIBLTE_S1AP_LOGGINGDURATION_M20,
  LIBLTE_S1AP_LOGGINGDURATION_M40,
  LIBLTE_S1AP_LOGGINGDURATION_M60,
  LIBLTE_S1AP_LOGGINGDURATION_M90,
  LIBLTE_S1AP_LOGGINGDURATION_M120,
  LIBLTE_S1AP_LOGGINGDURATION_N_ITEMS,
}LIBLTE_S1AP_LOGGINGDURATION_ENUM;
static const char liblte_s1ap_loggingduration_text[LIBLTE_S1AP_LOGGINGDURATION_N_ITEMS][80] = {
  "m10",
  "m20",
  "m40",
  "m60",
  "m90",
  "m120",
};

LIBLTE_ERROR_ENUM liblte_s1ap_pack_loggingduration(
  LIBLTE_S1AP_LOGGINGDURATION_ENUM                             *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_loggingduration(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_LOGGINGDURATION_ENUM                             *ie);

/*******************************************************************************
/* ProtocolIE MDT_Activation ENUMERATED
********************************************************************************/
typedef enum{
  LIBLTE_S1AP_MDT_ACTIVATION_IMMEDIATE_MDT_ONLY,
  LIBLTE_S1AP_MDT_ACTIVATION_IMMEDIATE_MDT_AND_TRACE,
  LIBLTE_S1AP_MDT_ACTIVATION_LOGGED_MDT_ONLY,
  LIBLTE_S1AP_MDT_ACTIVATION_N_ITEMS,
}LIBLTE_S1AP_MDT_ACTIVATION_ENUM;
static const char liblte_s1ap_mdt_activation_text[LIBLTE_S1AP_MDT_ACTIVATION_N_ITEMS][80] = {
  "immediate-MDT-only",
  "immediate-MDT-and-Trace",
  "logged-MDT-only",
};

typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_MDT_ACTIVATION_ENUM e;
}LIBLTE_S1AP_MDT_ACTIVATION_ENUM_EXT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_mdt_activation(
  LIBLTE_S1AP_MDT_ACTIVATION_ENUM_EXT                          *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_mdt_activation(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MDT_ACTIVATION_ENUM_EXT                          *ie);

/*******************************************************************************
/* ProtocolIE ManagementBasedMDTAllowed ENUMERATED
********************************************************************************/
typedef enum{
  LIBLTE_S1AP_MANAGEMENTBASEDMDTALLOWED_ALLOWED,
  LIBLTE_S1AP_MANAGEMENTBASEDMDTALLOWED_N_ITEMS,
}LIBLTE_S1AP_MANAGEMENTBASEDMDTALLOWED_ENUM;
static const char liblte_s1ap_managementbasedmdtallowed_text[LIBLTE_S1AP_MANAGEMENTBASEDMDTALLOWED_N_ITEMS][80] = {
  "allowed",
};

typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_MANAGEMENTBASEDMDTALLOWED_ENUM e;
}LIBLTE_S1AP_MANAGEMENTBASEDMDTALLOWED_ENUM_EXT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_managementbasedmdtallowed(
  LIBLTE_S1AP_MANAGEMENTBASEDMDTALLOWED_ENUM_EXT               *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_managementbasedmdtallowed(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MANAGEMENTBASEDMDTALLOWED_ENUM_EXT               *ie);

/*******************************************************************************
/* ProtocolIE PrivacyIndicator ENUMERATED
********************************************************************************/
typedef enum{
  LIBLTE_S1AP_PRIVACYINDICATOR_IMMEDIATE_MDT,
  LIBLTE_S1AP_PRIVACYINDICATOR_LOGGED_MDT,
  LIBLTE_S1AP_PRIVACYINDICATOR_N_ITEMS,
}LIBLTE_S1AP_PRIVACYINDICATOR_ENUM;
static const char liblte_s1ap_privacyindicator_text[LIBLTE_S1AP_PRIVACYINDICATOR_N_ITEMS][80] = {
  "immediate-MDT",
  "logged-MDT",
};

typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_PRIVACYINDICATOR_ENUM e;
}LIBLTE_S1AP_PRIVACYINDICATOR_ENUM_EXT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_privacyindicator(
  LIBLTE_S1AP_PRIVACYINDICATOR_ENUM_EXT                        *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_privacyindicator(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_PRIVACYINDICATOR_ENUM_EXT                        *ie);

/*******************************************************************************
/* ProtocolIE MeasurementsToActivate STATIC BIT STRING
********************************************************************************/
#define LIBLTE_S1AP_MEASUREMENTSTOACTIVATE_BIT_STRING_LEN 8
typedef struct{
  uint8_t  buffer[8];
}LIBLTE_S1AP_MEASUREMENTSTOACTIVATE_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_measurementstoactivate(
  LIBLTE_S1AP_MEASUREMENTSTOACTIVATE_STRUCT                    *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_measurementstoactivate(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MEASUREMENTSTOACTIVATE_STRUCT                    *ie);

/*******************************************************************************
/* ProtocolIE MessageIdentifier STATIC BIT STRING
********************************************************************************/
#define LIBLTE_S1AP_MESSAGEIDENTIFIER_BIT_STRING_LEN 16
typedef struct{
  uint8_t  buffer[16];
}LIBLTE_S1AP_MESSAGEIDENTIFIER_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_messageidentifier(
  LIBLTE_S1AP_MESSAGEIDENTIFIER_STRUCT                         *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_messageidentifier(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MESSAGEIDENTIFIER_STRUCT                         *ie);

/*******************************************************************************
/* ProtocolIE MMEname PrintableString
********************************************************************************/
typedef struct{
  bool     ext;
  uint32_t n_octets;
  uint8_t  buffer[150];
}LIBLTE_S1AP_MMENAME_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_mmename(
  LIBLTE_S1AP_MMENAME_STRUCT                                   *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_mmename(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MMENAME_STRUCT                                   *ie);

/*******************************************************************************
/* ProtocolIE MME_Group_ID STATIC OCTET STRING
********************************************************************************/
#define LIBLTE_S1AP_MME_GROUP_ID_OCTET_STRING_LEN 2
typedef struct{
  uint8_t  buffer[2];
}LIBLTE_S1AP_MME_GROUP_ID_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_mme_group_id(
  LIBLTE_S1AP_MME_GROUP_ID_STRUCT                              *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_mme_group_id(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MME_GROUP_ID_STRUCT                              *ie);

/*******************************************************************************
/* ProtocolIE MME_UE_S1AP_ID INTEGER
********************************************************************************/
typedef struct{
uint32_t MME_UE_S1AP_ID;
}LIBLTE_S1AP_MME_UE_S1AP_ID_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_mme_ue_s1ap_id(
  LIBLTE_S1AP_MME_UE_S1AP_ID_STRUCT                            *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_mme_ue_s1ap_id(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MME_UE_S1AP_ID_STRUCT                            *ie);

/*******************************************************************************
/* ProtocolIE MSClassmark2 DYNAMIC OCTET STRING
********************************************************************************/
// lb:0, ub:16318
typedef struct{
  uint32_t n_octets;
  uint8_t  buffer[16318];
}LIBLTE_S1AP_MSCLASSMARK2_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_msclassmark2(
  LIBLTE_S1AP_MSCLASSMARK2_STRUCT                              *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_msclassmark2(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MSCLASSMARK2_STRUCT                              *ie);

/*******************************************************************************
/* ProtocolIE NAS_PDU DYNAMIC OCTET STRING
********************************************************************************/
// lb:0, ub:16318
typedef struct{
  uint32_t n_octets;
  uint8_t  buffer[16318];
}LIBLTE_S1AP_NAS_PDU_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_nas_pdu(
  LIBLTE_S1AP_NAS_PDU_STRUCT                                   *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_nas_pdu(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_NAS_PDU_STRUCT                                   *ie);

/*******************************************************************************
/* ProtocolIE NASSecurityParameterstoE_UTRAN DYNAMIC OCTET STRING
********************************************************************************/
// lb:0, ub:16318
typedef struct{
  uint32_t n_octets;
  uint8_t  buffer[16318];
}LIBLTE_S1AP_NASSECURITYPARAMETERSTOE_UTRAN_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_nassecurityparameterstoe_utran(
  LIBLTE_S1AP_NASSECURITYPARAMETERSTOE_UTRAN_STRUCT            *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_nassecurityparameterstoe_utran(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_NASSECURITYPARAMETERSTOE_UTRAN_STRUCT            *ie);

/*******************************************************************************
/* ProtocolIE NumberOfBroadcasts INTEGER
********************************************************************************/
typedef struct{
uint16_t NumberOfBroadcasts;
}LIBLTE_S1AP_NUMBEROFBROADCASTS_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_numberofbroadcasts(
  LIBLTE_S1AP_NUMBEROFBROADCASTS_STRUCT                        *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_numberofbroadcasts(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_NUMBEROFBROADCASTS_STRUCT                        *ie);

/*******************************************************************************
/* ProtocolIE OverloadAction ENUMERATED
********************************************************************************/
typedef enum{
  LIBLTE_S1AP_OVERLOADACTION_REJECT_NON_EMERGENCY_MO_DT,
  LIBLTE_S1AP_OVERLOADACTION_REJECT_RRC_CR_SIGNALLING,
  LIBLTE_S1AP_OVERLOADACTION_PERMIT_EMERGENCY_SESSIONS_AND_MOBILE_TERMINATED_SERVICES_ONLY,
  LIBLTE_S1AP_OVERLOADACTION_PERMIT_HIGH_PRIORITY_SESSIONS_AND_MOBILE_TERMINATED_SERVICES_ONLY,
  LIBLTE_S1AP_OVERLOADACTION_REJECT_DELAY_TOLERANT_ACCESS,
  LIBLTE_S1AP_OVERLOADACTION_N_ITEMS,
}LIBLTE_S1AP_OVERLOADACTION_ENUM;
static const char liblte_s1ap_overloadaction_text[LIBLTE_S1AP_OVERLOADACTION_N_ITEMS][80] = {
  "reject-non-emergency-mo-dt",
  "reject-rrc-cr-signalling",
  "permit-emergency-sessions-and-mobile-terminated-services-only",
  "permit-high-priority-sessions-and-mobile-terminated-services-only",
  "reject-delay-tolerant-access",
};

typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_OVERLOADACTION_ENUM e;
}LIBLTE_S1AP_OVERLOADACTION_ENUM_EXT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_overloadaction(
  LIBLTE_S1AP_OVERLOADACTION_ENUM_EXT                          *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_overloadaction(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_OVERLOADACTION_ENUM_EXT                          *ie);

/*******************************************************************************
/* ProtocolIE PagingDRX ENUMERATED
********************************************************************************/
typedef enum{
  LIBLTE_S1AP_PAGINGDRX_V32,
  LIBLTE_S1AP_PAGINGDRX_V64,
  LIBLTE_S1AP_PAGINGDRX_V128,
  LIBLTE_S1AP_PAGINGDRX_V256,
  LIBLTE_S1AP_PAGINGDRX_N_ITEMS,
}LIBLTE_S1AP_PAGINGDRX_ENUM;
static const char liblte_s1ap_pagingdrx_text[LIBLTE_S1AP_PAGINGDRX_N_ITEMS][80] = {
  "v32",
  "v64",
  "v128",
  "v256",
};

typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_PAGINGDRX_ENUM e;
}LIBLTE_S1AP_PAGINGDRX_ENUM_EXT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_pagingdrx(
  LIBLTE_S1AP_PAGINGDRX_ENUM_EXT                               *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_pagingdrx(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_PAGINGDRX_ENUM_EXT                               *ie);

/*******************************************************************************
/* ProtocolIE PDCP_SN INTEGER
********************************************************************************/
typedef struct{
uint16_t PDCP_SN;
}LIBLTE_S1AP_PDCP_SN_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_pdcp_sn(
  LIBLTE_S1AP_PDCP_SN_STRUCT                                   *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_pdcp_sn(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_PDCP_SN_STRUCT                                   *ie);

/*******************************************************************************
/* ProtocolIE Port_Number STATIC OCTET STRING
********************************************************************************/
#define LIBLTE_S1AP_PORT_NUMBER_OCTET_STRING_LEN 2
typedef struct{
  uint8_t  buffer[2];
}LIBLTE_S1AP_PORT_NUMBER_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_port_number(
  LIBLTE_S1AP_PORT_NUMBER_STRUCT                               *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_port_number(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_PORT_NUMBER_STRUCT                               *ie);

/*******************************************************************************
/* ProtocolIE Pre_emptionVulnerability ENUMERATED
********************************************************************************/
typedef enum{
  LIBLTE_S1AP_PRE_EMPTIONVULNERABILITY_NOT_PRE_EMPTABLE,
  LIBLTE_S1AP_PRE_EMPTIONVULNERABILITY_PRE_EMPTABLE,
  LIBLTE_S1AP_PRE_EMPTIONVULNERABILITY_N_ITEMS,
}LIBLTE_S1AP_PRE_EMPTIONVULNERABILITY_ENUM;
static const char liblte_s1ap_pre_emptionvulnerability_text[LIBLTE_S1AP_PRE_EMPTIONVULNERABILITY_N_ITEMS][80] = {
  "not-pre-emptable",
  "pre-emptable",
};

LIBLTE_ERROR_ENUM liblte_s1ap_pack_pre_emptionvulnerability(
  LIBLTE_S1AP_PRE_EMPTIONVULNERABILITY_ENUM                    *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_pre_emptionvulnerability(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_PRE_EMPTIONVULNERABILITY_ENUM                    *ie);

/*******************************************************************************
/* ProtocolIE PS_ServiceNotAvailable ENUMERATED
********************************************************************************/
typedef enum{
  LIBLTE_S1AP_PS_SERVICENOTAVAILABLE_PS_SERVICE_NOT_AVAILABLE,
  LIBLTE_S1AP_PS_SERVICENOTAVAILABLE_N_ITEMS,
}LIBLTE_S1AP_PS_SERVICENOTAVAILABLE_ENUM;
static const char liblte_s1ap_ps_servicenotavailable_text[LIBLTE_S1AP_PS_SERVICENOTAVAILABLE_N_ITEMS][80] = {
  "ps-service-not-available",
};

typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_PS_SERVICENOTAVAILABLE_ENUM e;
}LIBLTE_S1AP_PS_SERVICENOTAVAILABLE_ENUM_EXT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_ps_servicenotavailable(
  LIBLTE_S1AP_PS_SERVICENOTAVAILABLE_ENUM_EXT                  *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_ps_servicenotavailable(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_PS_SERVICENOTAVAILABLE_ENUM_EXT                  *ie);

/*******************************************************************************
/* ProtocolIE ReceiveStatusofULPDCPSDUs STATIC BIT STRING
********************************************************************************/
#define LIBLTE_S1AP_RECEIVESTATUSOFULPDCPSDUS_BIT_STRING_LEN 4096
typedef struct{
  uint8_t  buffer[4096];
}LIBLTE_S1AP_RECEIVESTATUSOFULPDCPSDUS_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_receivestatusofulpdcpsdus(
  LIBLTE_S1AP_RECEIVESTATUSOFULPDCPSDUS_STRUCT                 *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_receivestatusofulpdcpsdus(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_RECEIVESTATUSOFULPDCPSDUS_STRUCT                 *ie);

/*******************************************************************************
/* ProtocolIE RelativeMMECapacity INTEGER
********************************************************************************/
typedef struct{
uint8_t RelativeMMECapacity;
}LIBLTE_S1AP_RELATIVEMMECAPACITY_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_relativemmecapacity(
  LIBLTE_S1AP_RELATIVEMMECAPACITY_STRUCT                       *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_relativemmecapacity(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_RELATIVEMMECAPACITY_STRUCT                       *ie);

/*******************************************************************************
/* ProtocolIE RAC STATIC OCTET STRING
********************************************************************************/
#define LIBLTE_S1AP_RAC_OCTET_STRING_LEN 1
typedef struct{
  uint8_t  buffer[1];
}LIBLTE_S1AP_RAC_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_rac(
  LIBLTE_S1AP_RAC_STRUCT                                       *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_rac(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_RAC_STRUCT                                       *ie);

/*******************************************************************************
/* ProtocolIE ReportIntervalMDT ENUMERATED
********************************************************************************/
typedef enum{
  LIBLTE_S1AP_REPORTINTERVALMDT_MS120,
  LIBLTE_S1AP_REPORTINTERVALMDT_MS240,
  LIBLTE_S1AP_REPORTINTERVALMDT_MS480,
  LIBLTE_S1AP_REPORTINTERVALMDT_MS640,
  LIBLTE_S1AP_REPORTINTERVALMDT_MS1024,
  LIBLTE_S1AP_REPORTINTERVALMDT_MS2048,
  LIBLTE_S1AP_REPORTINTERVALMDT_MS5120,
  LIBLTE_S1AP_REPORTINTERVALMDT_MS10240,
  LIBLTE_S1AP_REPORTINTERVALMDT_MIN1,
  LIBLTE_S1AP_REPORTINTERVALMDT_MIN6,
  LIBLTE_S1AP_REPORTINTERVALMDT_MIN12,
  LIBLTE_S1AP_REPORTINTERVALMDT_MIN30,
  LIBLTE_S1AP_REPORTINTERVALMDT_MIN60,
  LIBLTE_S1AP_REPORTINTERVALMDT_N_ITEMS,
}LIBLTE_S1AP_REPORTINTERVALMDT_ENUM;
static const char liblte_s1ap_reportintervalmdt_text[LIBLTE_S1AP_REPORTINTERVALMDT_N_ITEMS][80] = {
  "ms120",
  "ms240",
  "ms480",
  "ms640",
  "ms1024",
  "ms2048",
  "ms5120",
  "ms10240",
  "min1",
  "min6",
  "min12",
  "min30",
  "min60",
};

LIBLTE_ERROR_ENUM liblte_s1ap_pack_reportintervalmdt(
  LIBLTE_S1AP_REPORTINTERVALMDT_ENUM                           *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_reportintervalmdt(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_REPORTINTERVALMDT_ENUM                           *ie);

/*******************************************************************************
/* ProtocolIE ReportArea ENUMERATED
********************************************************************************/
typedef enum{
  LIBLTE_S1AP_REPORTAREA_ECGI,
  LIBLTE_S1AP_REPORTAREA_N_ITEMS,
}LIBLTE_S1AP_REPORTAREA_ENUM;
static const char liblte_s1ap_reportarea_text[LIBLTE_S1AP_REPORTAREA_N_ITEMS][80] = {
  "ecgi",
};

typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_REPORTAREA_ENUM e;
}LIBLTE_S1AP_REPORTAREA_ENUM_EXT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_reportarea(
  LIBLTE_S1AP_REPORTAREA_ENUM_EXT                              *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_reportarea(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_REPORTAREA_ENUM_EXT                              *ie);

/*******************************************************************************
/* ProtocolIE RNC_ID INTEGER
********************************************************************************/
typedef struct{
uint16_t RNC_ID;
}LIBLTE_S1AP_RNC_ID_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_rnc_id(
  LIBLTE_S1AP_RNC_ID_STRUCT                                    *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_rnc_id(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_RNC_ID_STRUCT                                    *ie);

/*******************************************************************************
/* ProtocolIE RRC_Establishment_Cause ENUMERATED
********************************************************************************/
typedef enum{
  LIBLTE_S1AP_RRC_ESTABLISHMENT_CAUSE_EMERGENCY,
  LIBLTE_S1AP_RRC_ESTABLISHMENT_CAUSE_HIGHPRIORITYACCESS,
  LIBLTE_S1AP_RRC_ESTABLISHMENT_CAUSE_MT_ACCESS,
  LIBLTE_S1AP_RRC_ESTABLISHMENT_CAUSE_MO_SIGNALLING,
  LIBLTE_S1AP_RRC_ESTABLISHMENT_CAUSE_MO_DATA,
  LIBLTE_S1AP_RRC_ESTABLISHMENT_CAUSE_DELAY_TOLERANTACCESS,
  LIBLTE_S1AP_RRC_ESTABLISHMENT_CAUSE_N_ITEMS,
}LIBLTE_S1AP_RRC_ESTABLISHMENT_CAUSE_ENUM;
static const char liblte_s1ap_rrc_establishment_cause_text[LIBLTE_S1AP_RRC_ESTABLISHMENT_CAUSE_N_ITEMS][80] = {
  "emergency",
  "highPriorityAccess",
  "mt-Access",
  "mo-Signalling",
  "mo-Data",
  "delay-TolerantAccess",
};

typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_RRC_ESTABLISHMENT_CAUSE_ENUM e;
}LIBLTE_S1AP_RRC_ESTABLISHMENT_CAUSE_ENUM_EXT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_rrc_establishment_cause(
  LIBLTE_S1AP_RRC_ESTABLISHMENT_CAUSE_ENUM_EXT                 *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_rrc_establishment_cause(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_RRC_ESTABLISHMENT_CAUSE_ENUM_EXT                 *ie);

/*******************************************************************************
/* ProtocolIE Routing_ID INTEGER
********************************************************************************/
typedef struct{
uint8_t Routing_ID;
}LIBLTE_S1AP_ROUTING_ID_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_routing_id(
  LIBLTE_S1AP_ROUTING_ID_STRUCT                                *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_routing_id(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_ROUTING_ID_STRUCT                                *ie);

/*******************************************************************************
/* ProtocolIE SONInformationRequest ENUMERATED
********************************************************************************/
typedef enum{
  LIBLTE_S1AP_SONINFORMATIONREQUEST_X2TNL_CONFIGURATION_INFO,
  LIBLTE_S1AP_SONINFORMATIONREQUEST_TIME_SYNCHRONIZATION_INFO,
  LIBLTE_S1AP_SONINFORMATIONREQUEST_N_ITEMS,
}LIBLTE_S1AP_SONINFORMATIONREQUEST_ENUM;
static const char liblte_s1ap_soninformationrequest_text[LIBLTE_S1AP_SONINFORMATIONREQUEST_N_ITEMS][80] = {
  "x2TNL-Configuration-Info",
  "time-Synchronization-Info",
};

typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_SONINFORMATIONREQUEST_ENUM e;
}LIBLTE_S1AP_SONINFORMATIONREQUEST_ENUM_EXT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_soninformationrequest(
  LIBLTE_S1AP_SONINFORMATIONREQUEST_ENUM_EXT                   *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_soninformationrequest(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_SONINFORMATIONREQUEST_ENUM_EXT                   *ie);

/*******************************************************************************
/* ProtocolIE Source_ToTarget_TransparentContainer DYNAMIC OCTET STRING
********************************************************************************/
// lb:0, ub:16318
typedef struct{
  uint32_t n_octets;
  uint8_t  buffer[16318];
}LIBLTE_S1AP_SOURCE_TOTARGET_TRANSPARENTCONTAINER_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_source_totarget_transparentcontainer(
  LIBLTE_S1AP_SOURCE_TOTARGET_TRANSPARENTCONTAINER_STRUCT      *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_source_totarget_transparentcontainer(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_SOURCE_TOTARGET_TRANSPARENTCONTAINER_STRUCT      *ie);

/*******************************************************************************
/* ProtocolIE SRVCCHOIndication ENUMERATED
********************************************************************************/
typedef enum{
  LIBLTE_S1AP_SRVCCHOINDICATION_PSANDCS,
  LIBLTE_S1AP_SRVCCHOINDICATION_CSONLY,
  LIBLTE_S1AP_SRVCCHOINDICATION_N_ITEMS,
}LIBLTE_S1AP_SRVCCHOINDICATION_ENUM;
static const char liblte_s1ap_srvcchoindication_text[LIBLTE_S1AP_SRVCCHOINDICATION_N_ITEMS][80] = {
  "pSandCS",
  "cSonly",
};

typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_SRVCCHOINDICATION_ENUM e;
}LIBLTE_S1AP_SRVCCHOINDICATION_ENUM_EXT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_srvcchoindication(
  LIBLTE_S1AP_SRVCCHOINDICATION_ENUM_EXT                       *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_srvcchoindication(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_SRVCCHOINDICATION_ENUM_EXT                       *ie);

/*******************************************************************************
/* ProtocolIE SourceRNC_ToTargetRNC_TransparentContainer DYNAMIC OCTET STRING
********************************************************************************/
// lb:0, ub:16318
typedef struct{
  uint32_t n_octets;
  uint8_t  buffer[16318];
}LIBLTE_S1AP_SOURCERNC_TOTARGETRNC_TRANSPARENTCONTAINER_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_sourcernc_totargetrnc_transparentcontainer(
  LIBLTE_S1AP_SOURCERNC_TOTARGETRNC_TRANSPARENTCONTAINER_STRUCT *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_sourcernc_totargetrnc_transparentcontainer(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_SOURCERNC_TOTARGETRNC_TRANSPARENTCONTAINER_STRUCT *ie);

/*******************************************************************************
/* ProtocolIE SubscriberProfileIDforRFP INTEGER
********************************************************************************/
typedef struct{
uint8_t SubscriberProfileIDforRFP;
}LIBLTE_S1AP_SUBSCRIBERPROFILEIDFORRFP_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_subscriberprofileidforrfp(
  LIBLTE_S1AP_SUBSCRIBERPROFILEIDFORRFP_STRUCT                 *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_subscriberprofileidforrfp(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_SUBSCRIBERPROFILEIDFORRFP_STRUCT                 *ie);

/*******************************************************************************
/* ProtocolIE SynchronizationStatus ENUMERATED
********************************************************************************/
typedef enum{
  LIBLTE_S1AP_SYNCHRONIZATIONSTATUS_SYNCHRONOUS,
  LIBLTE_S1AP_SYNCHRONIZATIONSTATUS_ASYNCHRONOUS,
  LIBLTE_S1AP_SYNCHRONIZATIONSTATUS_N_ITEMS,
}LIBLTE_S1AP_SYNCHRONIZATIONSTATUS_ENUM;
static const char liblte_s1ap_synchronizationstatus_text[LIBLTE_S1AP_SYNCHRONIZATIONSTATUS_N_ITEMS][80] = {
  "synchronous",
  "asynchronous",
};

typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_SYNCHRONIZATIONSTATUS_ENUM e;
}LIBLTE_S1AP_SYNCHRONIZATIONSTATUS_ENUM_EXT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_synchronizationstatus(
  LIBLTE_S1AP_SYNCHRONIZATIONSTATUS_ENUM_EXT                   *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_synchronizationstatus(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_SYNCHRONIZATIONSTATUS_ENUM_EXT                   *ie);

/*******************************************************************************
/* ProtocolIE TargetRNC_ToSourceRNC_TransparentContainer DYNAMIC OCTET STRING
********************************************************************************/
// lb:0, ub:16318
typedef struct{
  uint32_t n_octets;
  uint8_t  buffer[16318];
}LIBLTE_S1AP_TARGETRNC_TOSOURCERNC_TRANSPARENTCONTAINER_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_targetrnc_tosourcernc_transparentcontainer(
  LIBLTE_S1AP_TARGETRNC_TOSOURCERNC_TRANSPARENTCONTAINER_STRUCT *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_targetrnc_tosourcernc_transparentcontainer(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_TARGETRNC_TOSOURCERNC_TRANSPARENTCONTAINER_STRUCT *ie);

/*******************************************************************************
/* ProtocolIE Threshold_RSRQ INTEGER
********************************************************************************/
typedef struct{
uint8_t Threshold_RSRQ;
}LIBLTE_S1AP_THRESHOLD_RSRQ_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_threshold_rsrq(
  LIBLTE_S1AP_THRESHOLD_RSRQ_STRUCT                            *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_threshold_rsrq(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_THRESHOLD_RSRQ_STRUCT                            *ie);

/*******************************************************************************
/* ProtocolIE Time_UE_StayedInCell INTEGER
********************************************************************************/
typedef struct{
uint16_t Time_UE_StayedInCell;
}LIBLTE_S1AP_TIME_UE_STAYEDINCELL_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_time_ue_stayedincell(
  LIBLTE_S1AP_TIME_UE_STAYEDINCELL_STRUCT                      *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_time_ue_stayedincell(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_TIME_UE_STAYEDINCELL_STRUCT                      *ie);

/*******************************************************************************
/* ProtocolIE TransportLayerAddress DYNAMIC BIT STRING
********************************************************************************/
// lb:1, ub:160
typedef struct{
  bool     ext;
  uint32_t n_bits;
  uint8_t  buffer[160];
}LIBLTE_S1AP_TRANSPORTLAYERADDRESS_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_transportlayeraddress(
  LIBLTE_S1AP_TRANSPORTLAYERADDRESS_STRUCT                     *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_transportlayeraddress(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_TRANSPORTLAYERADDRESS_STRUCT                     *ie);

/*******************************************************************************
/* ProtocolIE TraceDepth ENUMERATED
********************************************************************************/
typedef enum{
  LIBLTE_S1AP_TRACEDEPTH_MINIMUM,
  LIBLTE_S1AP_TRACEDEPTH_MEDIUM,
  LIBLTE_S1AP_TRACEDEPTH_MAXIMUM,
  LIBLTE_S1AP_TRACEDEPTH_MINIMUMWITHOUTVENDORSPECIFICEXTENSION,
  LIBLTE_S1AP_TRACEDEPTH_MEDIUMWITHOUTVENDORSPECIFICEXTENSION,
  LIBLTE_S1AP_TRACEDEPTH_MAXIMUMWITHOUTVENDORSPECIFICEXTENSION,
  LIBLTE_S1AP_TRACEDEPTH_N_ITEMS,
}LIBLTE_S1AP_TRACEDEPTH_ENUM;
static const char liblte_s1ap_tracedepth_text[LIBLTE_S1AP_TRACEDEPTH_N_ITEMS][80] = {
  "minimum",
  "medium",
  "maximum",
  "minimumWithoutVendorSpecificExtension",
  "mediumWithoutVendorSpecificExtension",
  "maximumWithoutVendorSpecificExtension",
};

typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_TRACEDEPTH_ENUM e;
}LIBLTE_S1AP_TRACEDEPTH_ENUM_EXT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_tracedepth(
  LIBLTE_S1AP_TRACEDEPTH_ENUM_EXT                              *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_tracedepth(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_TRACEDEPTH_ENUM_EXT                              *ie);

/*******************************************************************************
/* ProtocolIE TrafficLoadReductionIndication INTEGER
********************************************************************************/
typedef struct{
uint8_t TrafficLoadReductionIndication;
}LIBLTE_S1AP_TRAFFICLOADREDUCTIONINDICATION_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_trafficloadreductionindication(
  LIBLTE_S1AP_TRAFFICLOADREDUCTIONINDICATION_STRUCT            *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_trafficloadreductionindication(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_TRAFFICLOADREDUCTIONINDICATION_STRUCT            *ie);

/*******************************************************************************
/* ProtocolIE UERadioCapability DYNAMIC OCTET STRING
********************************************************************************/
// lb:0, ub:16318
typedef struct{
  uint32_t n_octets;
  uint8_t  buffer[16318];
}LIBLTE_S1AP_UERADIOCAPABILITY_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_ueradiocapability(
  LIBLTE_S1AP_UERADIOCAPABILITY_STRUCT                         *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_ueradiocapability(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_UERADIOCAPABILITY_STRUCT                         *ie);

/*******************************************************************************
/* ProtocolIE WarningType STATIC OCTET STRING
********************************************************************************/
#define LIBLTE_S1AP_WARNINGTYPE_OCTET_STRING_LEN 2
typedef struct{
  uint8_t  buffer[2];
}LIBLTE_S1AP_WARNINGTYPE_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_warningtype(
  LIBLTE_S1AP_WARNINGTYPE_STRUCT                               *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_warningtype(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_WARNINGTYPE_STRUCT                               *ie);

/*******************************************************************************
/* ProtocolIE WarningMessageContents DYNAMIC OCTET STRING
********************************************************************************/
// lb:1, ub:9600
typedef struct{
  uint32_t n_octets;
  uint8_t  buffer[9600];
}LIBLTE_S1AP_WARNINGMESSAGECONTENTS_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_warningmessagecontents(
  LIBLTE_S1AP_WARNINGMESSAGECONTENTS_STRUCT                    *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_warningmessagecontents(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_WARNINGMESSAGECONTENTS_STRUCT                    *ie);

/*******************************************************************************
/* ProtocolIE CauseProtocol ENUMERATED
********************************************************************************/
typedef enum{
  LIBLTE_S1AP_CAUSEPROTOCOL_TRANSFER_SYNTAX_ERROR,
  LIBLTE_S1AP_CAUSEPROTOCOL_ABSTRACT_SYNTAX_ERROR_REJECT,
  LIBLTE_S1AP_CAUSEPROTOCOL_ABSTRACT_SYNTAX_ERROR_IGNORE_AND_NOTIFY,
  LIBLTE_S1AP_CAUSEPROTOCOL_MESSAGE_NOT_COMPATIBLE_WITH_RECEIVER_STATE,
  LIBLTE_S1AP_CAUSEPROTOCOL_SEMANTIC_ERROR,
  LIBLTE_S1AP_CAUSEPROTOCOL_ABSTRACT_SYNTAX_ERROR_FALSELY_CONSTRUCTED_MESSAGE,
  LIBLTE_S1AP_CAUSEPROTOCOL_UNSPECIFIED,
  LIBLTE_S1AP_CAUSEPROTOCOL_N_ITEMS,
}LIBLTE_S1AP_CAUSEPROTOCOL_ENUM;
static const char liblte_s1ap_causeprotocol_text[LIBLTE_S1AP_CAUSEPROTOCOL_N_ITEMS][80] = {
  "transfer-syntax-error",
  "abstract-syntax-error-reject",
  "abstract-syntax-error-ignore-and-notify",
  "message-not-compatible-with-receiver-state",
  "semantic-error",
  "abstract-syntax-error-falsely-constructed-message",
  "unspecified",
};

typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_CAUSEPROTOCOL_ENUM e;
}LIBLTE_S1AP_CAUSEPROTOCOL_ENUM_EXT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_causeprotocol(
  LIBLTE_S1AP_CAUSEPROTOCOL_ENUM_EXT                           *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_causeprotocol(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_CAUSEPROTOCOL_ENUM_EXT                           *ie);

/*******************************************************************************
/* ProtocolIE CellAccessMode ENUMERATED
********************************************************************************/
typedef enum{
  LIBLTE_S1AP_CELLACCESSMODE_HYBRID,
  LIBLTE_S1AP_CELLACCESSMODE_N_ITEMS,
}LIBLTE_S1AP_CELLACCESSMODE_ENUM;
static const char liblte_s1ap_cellaccessmode_text[LIBLTE_S1AP_CELLACCESSMODE_N_ITEMS][80] = {
  "hybrid",
};

typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_CELLACCESSMODE_ENUM e;
}LIBLTE_S1AP_CELLACCESSMODE_ENUM_EXT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_cellaccessmode(
  LIBLTE_S1AP_CELLACCESSMODE_ENUM_EXT                          *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_cellaccessmode(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_CELLACCESSMODE_ENUM_EXT                          *ie);

/*******************************************************************************
/* ProtocolIE Cdma2000RATType ENUMERATED
********************************************************************************/
typedef enum{
  LIBLTE_S1AP_CDMA2000RATTYPE_HRPD,
  LIBLTE_S1AP_CDMA2000RATTYPE_ONEXRTT,
  LIBLTE_S1AP_CDMA2000RATTYPE_N_ITEMS,
}LIBLTE_S1AP_CDMA2000RATTYPE_ENUM;
static const char liblte_s1ap_cdma2000rattype_text[LIBLTE_S1AP_CDMA2000RATTYPE_N_ITEMS][80] = {
  "hRPD",
  "onexRTT",
};

typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_CDMA2000RATTYPE_ENUM e;
}LIBLTE_S1AP_CDMA2000RATTYPE_ENUM_EXT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_cdma2000rattype(
  LIBLTE_S1AP_CDMA2000RATTYPE_ENUM_EXT                         *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_cdma2000rattype(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_CDMA2000RATTYPE_ENUM_EXT                         *ie);

/*******************************************************************************
/* ProtocolIE Cdma2000OneXMEID DYNAMIC OCTET STRING
********************************************************************************/
// lb:0, ub:16318
typedef struct{
  uint32_t n_octets;
  uint8_t  buffer[16318];
}LIBLTE_S1AP_CDMA2000ONEXMEID_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_cdma2000onexmeid(
  LIBLTE_S1AP_CDMA2000ONEXMEID_STRUCT                          *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_cdma2000onexmeid(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_CDMA2000ONEXMEID_STRUCT                          *ie);

/*******************************************************************************
/* ProtocolIE Cell_Size ENUMERATED
********************************************************************************/
typedef enum{
  LIBLTE_S1AP_CELL_SIZE_VERYSMALL,
  LIBLTE_S1AP_CELL_SIZE_SMALL,
  LIBLTE_S1AP_CELL_SIZE_MEDIUM,
  LIBLTE_S1AP_CELL_SIZE_LARGE,
  LIBLTE_S1AP_CELL_SIZE_N_ITEMS,
}LIBLTE_S1AP_CELL_SIZE_ENUM;
static const char liblte_s1ap_cell_size_text[LIBLTE_S1AP_CELL_SIZE_N_ITEMS][80] = {
  "verysmall",
  "small",
  "medium",
  "large",
};

typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_CELL_SIZE_ENUM e;
}LIBLTE_S1AP_CELL_SIZE_ENUM_EXT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_cell_size(
  LIBLTE_S1AP_CELL_SIZE_ENUM_EXT                               *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_cell_size(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_CELL_SIZE_ENUM_EXT                               *ie);

/*******************************************************************************
/* ProtocolIE CI STATIC OCTET STRING
********************************************************************************/
#define LIBLTE_S1AP_CI_OCTET_STRING_LEN 2
typedef struct{
  uint8_t  buffer[2];
}LIBLTE_S1AP_CI_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_ci(
  LIBLTE_S1AP_CI_STRUCT                                        *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_ci(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_CI_STRUCT                                        *ie);

/*******************************************************************************
/* ProtocolIE CSFallbackIndicator ENUMERATED
********************************************************************************/
typedef enum{
  LIBLTE_S1AP_CSFALLBACKINDICATOR_CS_FALLBACK_REQUIRED,
  LIBLTE_S1AP_CSFALLBACKINDICATOR_CS_FALLBACK_HIGH_PRIORITY,
  LIBLTE_S1AP_CSFALLBACKINDICATOR_N_ITEMS,
}LIBLTE_S1AP_CSFALLBACKINDICATOR_ENUM;
static const char liblte_s1ap_csfallbackindicator_text[LIBLTE_S1AP_CSFALLBACKINDICATOR_N_ITEMS][80] = {
  "cs-fallback-required",
  "cs-fallback-high-priority",
};

typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_CSFALLBACKINDICATOR_ENUM e;
}LIBLTE_S1AP_CSFALLBACKINDICATOR_ENUM_EXT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_csfallbackindicator(
  LIBLTE_S1AP_CSFALLBACKINDICATOR_ENUM_EXT                     *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_csfallbackindicator(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_CSFALLBACKINDICATOR_ENUM_EXT                     *ie);

/*******************************************************************************
/* ProtocolIE CSGMembershipStatus ENUMERATED
********************************************************************************/
typedef enum{
  LIBLTE_S1AP_CSGMEMBERSHIPSTATUS_MEMBER,
  LIBLTE_S1AP_CSGMEMBERSHIPSTATUS_NOT_MEMBER,
  LIBLTE_S1AP_CSGMEMBERSHIPSTATUS_N_ITEMS,
}LIBLTE_S1AP_CSGMEMBERSHIPSTATUS_ENUM;
static const char liblte_s1ap_csgmembershipstatus_text[LIBLTE_S1AP_CSGMEMBERSHIPSTATUS_N_ITEMS][80] = {
  "member",
  "not-member",
};

LIBLTE_ERROR_ENUM liblte_s1ap_pack_csgmembershipstatus(
  LIBLTE_S1AP_CSGMEMBERSHIPSTATUS_ENUM                         *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_csgmembershipstatus(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_CSGMEMBERSHIPSTATUS_ENUM                         *ie);

/*******************************************************************************
/* ProtocolIE DataCodingScheme STATIC BIT STRING
********************************************************************************/
#define LIBLTE_S1AP_DATACODINGSCHEME_BIT_STRING_LEN 8
typedef struct{
  uint8_t  buffer[8];
}LIBLTE_S1AP_DATACODINGSCHEME_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_datacodingscheme(
  LIBLTE_S1AP_DATACODINGSCHEME_STRUCT                          *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_datacodingscheme(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_DATACODINGSCHEME_STRUCT                          *ie);

/*******************************************************************************
/* ProtocolIE EmergencyAreaIDList DYNAMIC SEQUENCE OF
********************************************************************************/
// lb:1, ub:65535
typedef struct{
  uint32_t                                                     len;
  LIBLTE_S1AP_EMERGENCYAREAID_STRUCT                           buffer[32]; //WARNING: Artificial limit to reduce memory footprint
}LIBLTE_S1AP_EMERGENCYAREAIDLIST_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_emergencyareaidlist(
  LIBLTE_S1AP_EMERGENCYAREAIDLIST_STRUCT                       *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_emergencyareaidlist(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_EMERGENCYAREAIDLIST_STRUCT                       *ie);

/*******************************************************************************
/* ProtocolIE EmergencyAreaIDListForRestart DYNAMIC SEQUENCE OF
********************************************************************************/
// lb:1, ub:256
typedef struct{
  uint32_t                                                     len;
  LIBLTE_S1AP_EMERGENCYAREAID_STRUCT                           buffer[32]; //WARNING: Artificial limit to reduce memory footprint
}LIBLTE_S1AP_EMERGENCYAREAIDLISTFORRESTART_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_emergencyareaidlistforrestart(
  LIBLTE_S1AP_EMERGENCYAREAIDLISTFORRESTART_STRUCT             *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_emergencyareaidlistforrestart(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_EMERGENCYAREAIDLISTFORRESTART_STRUCT             *ie);

/*******************************************************************************
/* ProtocolIE ENB_UE_S1AP_ID INTEGER
********************************************************************************/
typedef struct{
uint32_t ENB_UE_S1AP_ID;
}LIBLTE_S1AP_ENB_UE_S1AP_ID_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_enb_ue_s1ap_id(
  LIBLTE_S1AP_ENB_UE_S1AP_ID_STRUCT                            *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_enb_ue_s1ap_id(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_ENB_UE_S1AP_ID_STRUCT                            *ie);

/*******************************************************************************
/* ProtocolIE E_RAB_ID INTEGER
********************************************************************************/
typedef struct{
  bool     ext;
uint8_t E_RAB_ID;
}LIBLTE_S1AP_E_RAB_ID_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_e_rab_id(
  LIBLTE_S1AP_E_RAB_ID_STRUCT                                  *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_e_rab_id(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_E_RAB_ID_STRUCT                                  *ie);

/*******************************************************************************
/* ProtocolIE E_RABInformationListItem SEQUENCE
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_E_RAB_ID_STRUCT                                  e_RAB_ID;
  LIBLTE_S1AP_DL_FORWARDING_ENUM_EXT                           dL_Forwarding;
  bool                                                         dL_Forwarding_present;
  LIBLTE_S1AP_PROTOCOLEXTENSIONCONTAINER_STRUCT                iE_Extensions;
  bool                                                         iE_Extensions_present;
}LIBLTE_S1AP_E_RABINFORMATIONLISTITEM_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_e_rabinformationlistitem(
  LIBLTE_S1AP_E_RABINFORMATIONLISTITEM_STRUCT                  *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_e_rabinformationlistitem(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_E_RABINFORMATIONLISTITEM_STRUCT                  *ie);

/*******************************************************************************
/* ProtocolIE EUTRANRoundTripDelayEstimationInfo INTEGER
********************************************************************************/
typedef struct{
uint16_t EUTRANRoundTripDelayEstimationInfo;
}LIBLTE_S1AP_EUTRANROUNDTRIPDELAYESTIMATIONINFO_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_eutranroundtripdelayestimationinfo(
  LIBLTE_S1AP_EUTRANROUNDTRIPDELAYESTIMATIONINFO_STRUCT        *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_eutranroundtripdelayestimationinfo(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_EUTRANROUNDTRIPDELAYESTIMATIONINFO_STRUCT        *ie);

/*******************************************************************************
/* ProtocolIE ForbiddenLACs DYNAMIC SEQUENCE OF
********************************************************************************/
// lb:1, ub:4096
typedef struct{
  uint32_t                                                     len;
  LIBLTE_S1AP_LAC_STRUCT                                       buffer[32]; //WARNING: Artificial limit to reduce memory footprint
}LIBLTE_S1AP_FORBIDDENLACS_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_forbiddenlacs(
  LIBLTE_S1AP_FORBIDDENLACS_STRUCT                             *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_forbiddenlacs(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_FORBIDDENLACS_STRUCT                             *ie);

/*******************************************************************************
/* ProtocolIE GTP_TEID STATIC OCTET STRING
********************************************************************************/
#define LIBLTE_S1AP_GTP_TEID_OCTET_STRING_LEN 4
typedef struct{
  uint8_t  buffer[4];
}LIBLTE_S1AP_GTP_TEID_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_gtp_teid(
  LIBLTE_S1AP_GTP_TEID_STRUCT                                  *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_gtp_teid(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_GTP_TEID_STRUCT                                  *ie);

/*******************************************************************************
/* ProtocolIE GUMMEIType ENUMERATED
********************************************************************************/
typedef enum{
  LIBLTE_S1AP_GUMMEITYPE_NATIVE,
  LIBLTE_S1AP_GUMMEITYPE_MAPPED,
  LIBLTE_S1AP_GUMMEITYPE_N_ITEMS,
}LIBLTE_S1AP_GUMMEITYPE_ENUM;
static const char liblte_s1ap_gummeitype_text[LIBLTE_S1AP_GUMMEITYPE_N_ITEMS][80] = {
  "native",
  "mapped",
};

typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_GUMMEITYPE_ENUM e;
}LIBLTE_S1AP_GUMMEITYPE_ENUM_EXT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_gummeitype(
  LIBLTE_S1AP_GUMMEITYPE_ENUM_EXT                              *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_gummeitype(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_GUMMEITYPE_ENUM_EXT                              *ie);

/*******************************************************************************
/* ProtocolIE HandoverType ENUMERATED
********************************************************************************/
typedef enum{
  LIBLTE_S1AP_HANDOVERTYPE_INTRALTE,
  LIBLTE_S1AP_HANDOVERTYPE_LTETOUTRAN,
  LIBLTE_S1AP_HANDOVERTYPE_LTETOGERAN,
  LIBLTE_S1AP_HANDOVERTYPE_UTRANTOLTE,
  LIBLTE_S1AP_HANDOVERTYPE_GERANTOLTE,
  LIBLTE_S1AP_HANDOVERTYPE_N_ITEMS,
}LIBLTE_S1AP_HANDOVERTYPE_ENUM;
static const char liblte_s1ap_handovertype_text[LIBLTE_S1AP_HANDOVERTYPE_N_ITEMS][80] = {
  "intralte",
  "ltetoutran",
  "ltetogeran",
  "utrantolte",
  "gerantolte",
};

typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_HANDOVERTYPE_ENUM e;
}LIBLTE_S1AP_HANDOVERTYPE_ENUM_EXT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_handovertype(
  LIBLTE_S1AP_HANDOVERTYPE_ENUM_EXT                            *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_handovertype(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_HANDOVERTYPE_ENUM_EXT                            *ie);

/*******************************************************************************
/* ProtocolIE IntegrityProtectionAlgorithms STATIC BIT STRING
********************************************************************************/
#define LIBLTE_S1AP_INTEGRITYPROTECTIONALGORITHMS_BIT_STRING_LEN 16
typedef struct{
  bool     ext;
  uint8_t  buffer[16];
}LIBLTE_S1AP_INTEGRITYPROTECTIONALGORITHMS_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_integrityprotectionalgorithms(
  LIBLTE_S1AP_INTEGRITYPROTECTIONALGORITHMS_STRUCT             *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_integrityprotectionalgorithms(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_INTEGRITYPROTECTIONALGORITHMS_STRUCT             *ie);

//TODO: Type undefined NULL

/*******************************************************************************
/* ProtocolIE LastVisitedGERANCellInformation CHOICE
********************************************************************************/
typedef enum{
  LIBLTE_S1AP_LASTVISITEDGERANCELLINFORMATION_CHOICE_UNDEFINED,
  LIBLTE_S1AP_LASTVISITEDGERANCELLINFORMATION_CHOICE_N_ITEMS,
}LIBLTE_S1AP_LASTVISITEDGERANCELLINFORMATION_CHOICE_ENUM;
static const char liblte_s1ap_lastvisitedgerancellinformation_choice_text[LIBLTE_S1AP_LASTVISITEDGERANCELLINFORMATION_CHOICE_N_ITEMS][50] = {
  "undefined",
};

typedef union{
  //TODO: NULL                                                 undefined;
}LIBLTE_S1AP_LASTVISITEDGERANCELLINFORMATION_CHOICE_UNION;

typedef struct{
  bool                            ext;
  LIBLTE_S1AP_LASTVISITEDGERANCELLINFORMATION_CHOICE_UNION choice;
  LIBLTE_S1AP_LASTVISITEDGERANCELLINFORMATION_CHOICE_ENUM  choice_type;
}LIBLTE_S1AP_LASTVISITEDGERANCELLINFORMATION_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_lastvisitedgerancellinformation(
  LIBLTE_S1AP_LASTVISITEDGERANCELLINFORMATION_STRUCT           *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_lastvisitedgerancellinformation(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_LASTVISITEDGERANCELLINFORMATION_STRUCT           *ie);

/*******************************************************************************
/* ProtocolIE Links_to_log ENUMERATED
********************************************************************************/
typedef enum{
  LIBLTE_S1AP_LINKS_TO_LOG_UPLINK,
  LIBLTE_S1AP_LINKS_TO_LOG_DOWNLINK,
  LIBLTE_S1AP_LINKS_TO_LOG_BOTH_UPLINK_AND_DOWNLINK,
  LIBLTE_S1AP_LINKS_TO_LOG_N_ITEMS,
}LIBLTE_S1AP_LINKS_TO_LOG_ENUM;
static const char liblte_s1ap_links_to_log_text[LIBLTE_S1AP_LINKS_TO_LOG_N_ITEMS][80] = {
  "uplink",
  "downlink",
  "both-uplink-and-downlink",
};

typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_LINKS_TO_LOG_ENUM e;
}LIBLTE_S1AP_LINKS_TO_LOG_ENUM_EXT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_links_to_log(
  LIBLTE_S1AP_LINKS_TO_LOG_ENUM_EXT                            *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_links_to_log(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_LINKS_TO_LOG_ENUM_EXT                            *ie);

/*******************************************************************************
/* ProtocolIE LoggingInterval ENUMERATED
********************************************************************************/
typedef enum{
  LIBLTE_S1AP_LOGGINGINTERVAL_MS128,
  LIBLTE_S1AP_LOGGINGINTERVAL_MS256,
  LIBLTE_S1AP_LOGGINGINTERVAL_MS512,
  LIBLTE_S1AP_LOGGINGINTERVAL_MS1024,
  LIBLTE_S1AP_LOGGINGINTERVAL_MS2048,
  LIBLTE_S1AP_LOGGINGINTERVAL_MS3072,
  LIBLTE_S1AP_LOGGINGINTERVAL_MS4096,
  LIBLTE_S1AP_LOGGINGINTERVAL_MS6144,
  LIBLTE_S1AP_LOGGINGINTERVAL_N_ITEMS,
}LIBLTE_S1AP_LOGGINGINTERVAL_ENUM;
static const char liblte_s1ap_logginginterval_text[LIBLTE_S1AP_LOGGINGINTERVAL_N_ITEMS][80] = {
  "ms128",
  "ms256",
  "ms512",
  "ms1024",
  "ms2048",
  "ms3072",
  "ms4096",
  "ms6144",
};

LIBLTE_ERROR_ENUM liblte_s1ap_pack_logginginterval(
  LIBLTE_S1AP_LOGGINGINTERVAL_ENUM                             *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_logginginterval(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_LOGGINGINTERVAL_ENUM                             *ie);

/*******************************************************************************
/* ProtocolIE M3period ENUMERATED
********************************************************************************/
typedef enum{
  LIBLTE_S1AP_M3PERIOD_MS100,
  LIBLTE_S1AP_M3PERIOD_MS1000,
  LIBLTE_S1AP_M3PERIOD_MS10000,
  LIBLTE_S1AP_M3PERIOD_N_ITEMS,
}LIBLTE_S1AP_M3PERIOD_ENUM;
static const char liblte_s1ap_m3period_text[LIBLTE_S1AP_M3PERIOD_N_ITEMS][80] = {
  "ms100",
  "ms1000",
  "ms10000",
};

typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_M3PERIOD_ENUM e;
}LIBLTE_S1AP_M3PERIOD_ENUM_EXT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_m3period(
  LIBLTE_S1AP_M3PERIOD_ENUM_EXT                                *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_m3period(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_M3PERIOD_ENUM_EXT                                *ie);

/*******************************************************************************
/* ProtocolIE M4period ENUMERATED
********************************************************************************/
typedef enum{
  LIBLTE_S1AP_M4PERIOD_MS1024,
  LIBLTE_S1AP_M4PERIOD_MS2048,
  LIBLTE_S1AP_M4PERIOD_MS5120,
  LIBLTE_S1AP_M4PERIOD_MS10240,
  LIBLTE_S1AP_M4PERIOD_MIN1,
  LIBLTE_S1AP_M4PERIOD_N_ITEMS,
}LIBLTE_S1AP_M4PERIOD_ENUM;
static const char liblte_s1ap_m4period_text[LIBLTE_S1AP_M4PERIOD_N_ITEMS][80] = {
  "ms1024",
  "ms2048",
  "ms5120",
  "ms10240",
  "min1",
};

typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_M4PERIOD_ENUM e;
}LIBLTE_S1AP_M4PERIOD_ENUM_EXT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_m4period(
  LIBLTE_S1AP_M4PERIOD_ENUM_EXT                                *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_m4period(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_M4PERIOD_ENUM_EXT                                *ie);

/*******************************************************************************
/* ProtocolIE M5period ENUMERATED
********************************************************************************/
typedef enum{
  LIBLTE_S1AP_M5PERIOD_MS1024,
  LIBLTE_S1AP_M5PERIOD_MS2048,
  LIBLTE_S1AP_M5PERIOD_MS5120,
  LIBLTE_S1AP_M5PERIOD_MS10240,
  LIBLTE_S1AP_M5PERIOD_MIN1,
  LIBLTE_S1AP_M5PERIOD_N_ITEMS,
}LIBLTE_S1AP_M5PERIOD_ENUM;
static const char liblte_s1ap_m5period_text[LIBLTE_S1AP_M5PERIOD_N_ITEMS][80] = {
  "ms1024",
  "ms2048",
  "ms5120",
  "ms10240",
  "min1",
};

typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_M5PERIOD_ENUM e;
}LIBLTE_S1AP_M5PERIOD_ENUM_EXT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_m5period(
  LIBLTE_S1AP_M5PERIOD_ENUM_EXT                                *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_m5period(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_M5PERIOD_ENUM_EXT                                *ie);

/*******************************************************************************
/* ProtocolIE MobilityInformation STATIC BIT STRING
********************************************************************************/
#define LIBLTE_S1AP_MOBILITYINFORMATION_BIT_STRING_LEN 32
typedef struct{
  uint8_t  buffer[32];
}LIBLTE_S1AP_MOBILITYINFORMATION_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_mobilityinformation(
  LIBLTE_S1AP_MOBILITYINFORMATION_STRUCT                       *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_mobilityinformation(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MOBILITYINFORMATION_STRUCT                       *ie);

/*******************************************************************************
/* ProtocolIE MME_Code STATIC OCTET STRING
********************************************************************************/
#define LIBLTE_S1AP_MME_CODE_OCTET_STRING_LEN 1
typedef struct{
  uint8_t  buffer[1];
}LIBLTE_S1AP_MME_CODE_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_mme_code(
  LIBLTE_S1AP_MME_CODE_STRUCT                                  *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_mme_code(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MME_CODE_STRUCT                                  *ie);

/*******************************************************************************
/* ProtocolIE MSClassmark3 DYNAMIC OCTET STRING
********************************************************************************/
// lb:0, ub:16318
typedef struct{
  uint32_t n_octets;
  uint8_t  buffer[16318];
}LIBLTE_S1AP_MSCLASSMARK3_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_msclassmark3(
  LIBLTE_S1AP_MSCLASSMARK3_STRUCT                              *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_msclassmark3(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MSCLASSMARK3_STRUCT                              *ie);

/*******************************************************************************
/* ProtocolIE NumberofBroadcastRequest INTEGER
********************************************************************************/
typedef struct{
uint16_t NumberofBroadcastRequest;
}LIBLTE_S1AP_NUMBEROFBROADCASTREQUEST_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_numberofbroadcastrequest(
  LIBLTE_S1AP_NUMBEROFBROADCASTREQUEST_STRUCT                  *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_numberofbroadcastrequest(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_NUMBEROFBROADCASTREQUEST_STRUCT                  *ie);

/*******************************************************************************
/* ProtocolIE OverloadResponse CHOICE
********************************************************************************/
typedef enum{
  LIBLTE_S1AP_OVERLOADRESPONSE_CHOICE_OVERLOADACTION,
  LIBLTE_S1AP_OVERLOADRESPONSE_CHOICE_N_ITEMS,
}LIBLTE_S1AP_OVERLOADRESPONSE_CHOICE_ENUM;
static const char liblte_s1ap_overloadresponse_choice_text[LIBLTE_S1AP_OVERLOADRESPONSE_CHOICE_N_ITEMS][50] = {
  "overloadAction",
};

typedef union{
  LIBLTE_S1AP_OVERLOADACTION_ENUM_EXT                          overloadAction;
}LIBLTE_S1AP_OVERLOADRESPONSE_CHOICE_UNION;

typedef struct{
  bool                            ext;
  LIBLTE_S1AP_OVERLOADRESPONSE_CHOICE_UNION choice;
  LIBLTE_S1AP_OVERLOADRESPONSE_CHOICE_ENUM  choice_type;
}LIBLTE_S1AP_OVERLOADRESPONSE_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_overloadresponse(
  LIBLTE_S1AP_OVERLOADRESPONSE_STRUCT                          *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_overloadresponse(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_OVERLOADRESPONSE_STRUCT                          *ie);

/*******************************************************************************
/* ProtocolIE PDCP_SNExtended INTEGER
********************************************************************************/
typedef struct{
uint16_t PDCP_SNExtended;
}LIBLTE_S1AP_PDCP_SNEXTENDED_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_pdcp_snextended(
  LIBLTE_S1AP_PDCP_SNEXTENDED_STRUCT                           *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_pdcp_snextended(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_PDCP_SNEXTENDED_STRUCT                           *ie);

/*******************************************************************************
/* ProtocolIE Pre_emptionCapability ENUMERATED
********************************************************************************/
typedef enum{
  LIBLTE_S1AP_PRE_EMPTIONCAPABILITY_SHALL_NOT_TRIGGER_PRE_EMPTION,
  LIBLTE_S1AP_PRE_EMPTIONCAPABILITY_MAY_TRIGGER_PRE_EMPTION,
  LIBLTE_S1AP_PRE_EMPTIONCAPABILITY_N_ITEMS,
}LIBLTE_S1AP_PRE_EMPTIONCAPABILITY_ENUM;
static const char liblte_s1ap_pre_emptioncapability_text[LIBLTE_S1AP_PRE_EMPTIONCAPABILITY_N_ITEMS][80] = {
  "shall-not-trigger-pre-emption",
  "may-trigger-pre-emption",
};

LIBLTE_ERROR_ENUM liblte_s1ap_pack_pre_emptioncapability(
  LIBLTE_S1AP_PRE_EMPTIONCAPABILITY_ENUM                       *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_pre_emptioncapability(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_PRE_EMPTIONCAPABILITY_ENUM                       *ie);

/*******************************************************************************
/* ProtocolIE QCI INTEGER
********************************************************************************/
typedef struct{
uint8_t QCI;
}LIBLTE_S1AP_QCI_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_qci(
  LIBLTE_S1AP_QCI_STRUCT                                       *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_qci(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_QCI_STRUCT                                       *ie);

/*******************************************************************************
/* ProtocolIE RelayNode_Indicator ENUMERATED
********************************************************************************/
typedef enum{
  LIBLTE_S1AP_RELAYNODE_INDICATOR_TRUE,
  LIBLTE_S1AP_RELAYNODE_INDICATOR_N_ITEMS,
}LIBLTE_S1AP_RELAYNODE_INDICATOR_ENUM;
static const char liblte_s1ap_relaynode_indicator_text[LIBLTE_S1AP_RELAYNODE_INDICATOR_N_ITEMS][80] = {
  "true",
};

typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_RELAYNODE_INDICATOR_ENUM e;
}LIBLTE_S1AP_RELAYNODE_INDICATOR_ENUM_EXT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_relaynode_indicator(
  LIBLTE_S1AP_RELAYNODE_INDICATOR_ENUM_EXT                     *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_relaynode_indicator(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_RELAYNODE_INDICATOR_ENUM_EXT                     *ie);

/*******************************************************************************
/* ProtocolIE M1ReportingTrigger ENUMERATED
********************************************************************************/
typedef enum{
  LIBLTE_S1AP_M1REPORTINGTRIGGER_PERIODIC,
  LIBLTE_S1AP_M1REPORTINGTRIGGER_A2EVENTTRIGGERED,
  LIBLTE_S1AP_M1REPORTINGTRIGGER_A2EVENTTRIGGERED_PERIODIC,
  LIBLTE_S1AP_M1REPORTINGTRIGGER_N_ITEMS,
}LIBLTE_S1AP_M1REPORTINGTRIGGER_ENUM;
static const char liblte_s1ap_m1reportingtrigger_text[LIBLTE_S1AP_M1REPORTINGTRIGGER_N_ITEMS][80] = {
  "periodic",
  "a2eventtriggered",
  "a2eventtriggered-periodic",
};

typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_M1REPORTINGTRIGGER_ENUM e;
}LIBLTE_S1AP_M1REPORTINGTRIGGER_ENUM_EXT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_m1reportingtrigger(
  LIBLTE_S1AP_M1REPORTINGTRIGGER_ENUM_EXT                      *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_m1reportingtrigger(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_M1REPORTINGTRIGGER_ENUM_EXT                      *ie);

/*******************************************************************************
/* ProtocolIE RIMInformation DYNAMIC OCTET STRING
********************************************************************************/
// lb:0, ub:16318
typedef struct{
  uint32_t n_octets;
  uint8_t  buffer[16318];
}LIBLTE_S1AP_RIMINFORMATION_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_riminformation(
  LIBLTE_S1AP_RIMINFORMATION_STRUCT                            *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_riminformation(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_RIMINFORMATION_STRUCT                            *ie);

/*******************************************************************************
/* ProtocolIE RepetitionPeriod INTEGER
********************************************************************************/
typedef struct{
uint16_t RepetitionPeriod;
}LIBLTE_S1AP_REPETITIONPERIOD_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_repetitionperiod(
  LIBLTE_S1AP_REPETITIONPERIOD_STRUCT                          *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_repetitionperiod(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_REPETITIONPERIOD_STRUCT                          *ie);

/*******************************************************************************
/* ProtocolIE SecurityKey STATIC BIT STRING
********************************************************************************/
#define LIBLTE_S1AP_SECURITYKEY_BIT_STRING_LEN 256
typedef struct{
  uint8_t  buffer[256];
}LIBLTE_S1AP_SECURITYKEY_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_securitykey(
  LIBLTE_S1AP_SECURITYKEY_STRUCT                               *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_securitykey(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_SECURITYKEY_STRUCT                               *ie);

/*******************************************************************************
/* ProtocolIE SerialNumber STATIC BIT STRING
********************************************************************************/
#define LIBLTE_S1AP_SERIALNUMBER_BIT_STRING_LEN 16
typedef struct{
  uint8_t  buffer[16];
}LIBLTE_S1AP_SERIALNUMBER_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_serialnumber(
  LIBLTE_S1AP_SERIALNUMBER_STRUCT                              *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_serialnumber(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_SERIALNUMBER_STRUCT                              *ie);

/*******************************************************************************
/* ProtocolIE SourceBSS_ToTargetBSS_TransparentContainer DYNAMIC OCTET STRING
********************************************************************************/
// lb:0, ub:16318
typedef struct{
  uint32_t n_octets;
  uint8_t  buffer[16318];
}LIBLTE_S1AP_SOURCEBSS_TOTARGETBSS_TRANSPARENTCONTAINER_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_sourcebss_totargetbss_transparentcontainer(
  LIBLTE_S1AP_SOURCEBSS_TOTARGETBSS_TRANSPARENTCONTAINER_STRUCT *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_sourcebss_totargetbss_transparentcontainer(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_SOURCEBSS_TOTARGETBSS_TRANSPARENTCONTAINER_STRUCT *ie);

/*******************************************************************************
/* ProtocolIE SRVCCOperationPossible ENUMERATED
********************************************************************************/
typedef enum{
  LIBLTE_S1AP_SRVCCOPERATIONPOSSIBLE_POSSIBLE,
  LIBLTE_S1AP_SRVCCOPERATIONPOSSIBLE_N_ITEMS,
}LIBLTE_S1AP_SRVCCOPERATIONPOSSIBLE_ENUM;
static const char liblte_s1ap_srvccoperationpossible_text[LIBLTE_S1AP_SRVCCOPERATIONPOSSIBLE_N_ITEMS][80] = {
  "possible",
};

typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_SRVCCOPERATIONPOSSIBLE_ENUM e;
}LIBLTE_S1AP_SRVCCOPERATIONPOSSIBLE_ENUM_EXT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_srvccoperationpossible(
  LIBLTE_S1AP_SRVCCOPERATIONPOSSIBLE_ENUM_EXT                  *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_srvccoperationpossible(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_SRVCCOPERATIONPOSSIBLE_ENUM_EXT                  *ie);

/*******************************************************************************
/* ProtocolIE ServedGroupIDs DYNAMIC SEQUENCE OF
********************************************************************************/
// lb:1, ub:65535
typedef struct{
  uint32_t                                                     len;
  LIBLTE_S1AP_MME_GROUP_ID_STRUCT                              buffer[32]; //WARNING: Artificial limit to reduce memory footprint
}LIBLTE_S1AP_SERVEDGROUPIDS_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_servedgroupids(
  LIBLTE_S1AP_SERVEDGROUPIDS_STRUCT                            *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_servedgroupids(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_SERVEDGROUPIDS_STRUCT                            *ie);

/*******************************************************************************
/* ProtocolIE StratumLevel INTEGER
********************************************************************************/
typedef struct{
  bool     ext;
uint8_t StratumLevel;
}LIBLTE_S1AP_STRATUMLEVEL_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_stratumlevel(
  LIBLTE_S1AP_STRATUMLEVEL_STRUCT                              *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_stratumlevel(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_STRATUMLEVEL_STRUCT                              *ie);

/*******************************************************************************
/* ProtocolIE TAC STATIC OCTET STRING
********************************************************************************/
#define LIBLTE_S1AP_TAC_OCTET_STRING_LEN 2
typedef struct{
  uint8_t  buffer[2];
}LIBLTE_S1AP_TAC_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_tac(
  LIBLTE_S1AP_TAC_STRUCT                                       *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_tac(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_TAC_STRUCT                                       *ie);

/*******************************************************************************
/* ProtocolIE TAListforMDT DYNAMIC SEQUENCE OF
********************************************************************************/
// lb:1, ub:8
typedef struct{
  uint32_t                                                     len;
  LIBLTE_S1AP_TAC_STRUCT                                       buffer[32]; //WARNING: Artificial limit to reduce memory footprint
}LIBLTE_S1AP_TALISTFORMDT_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_talistformdt(
  LIBLTE_S1AP_TALISTFORMDT_STRUCT                              *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_talistformdt(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_TALISTFORMDT_STRUCT                              *ie);

/*******************************************************************************
/* ProtocolIE TBCD_STRING STATIC OCTET STRING
********************************************************************************/
#define LIBLTE_S1AP_TBCD_STRING_OCTET_STRING_LEN 3
typedef struct{
  uint8_t  buffer[3];
}LIBLTE_S1AP_TBCD_STRING_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_tbcd_string(
  LIBLTE_S1AP_TBCD_STRING_STRUCT                               *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_tbcd_string(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_TBCD_STRING_STRUCT                               *ie);

/*******************************************************************************
/* ProtocolIE Target_ToSource_TransparentContainer DYNAMIC OCTET STRING
********************************************************************************/
// lb:0, ub:16318
typedef struct{
  uint32_t n_octets;
  uint8_t  buffer[16318];
}LIBLTE_S1AP_TARGET_TOSOURCE_TRANSPARENTCONTAINER_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_target_tosource_transparentcontainer(
  LIBLTE_S1AP_TARGET_TOSOURCE_TRANSPARENTCONTAINER_STRUCT      *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_target_tosource_transparentcontainer(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_TARGET_TOSOURCE_TRANSPARENTCONTAINER_STRUCT      *ie);

/*******************************************************************************
/* ProtocolIE Threshold_RSRP INTEGER
********************************************************************************/
typedef struct{
uint8_t Threshold_RSRP;
}LIBLTE_S1AP_THRESHOLD_RSRP_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_threshold_rsrp(
  LIBLTE_S1AP_THRESHOLD_RSRP_STRUCT                            *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_threshold_rsrp(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_THRESHOLD_RSRP_STRUCT                            *ie);

/*******************************************************************************
/* ProtocolIE Time_UE_StayedInCell_EnhancedGranularity INTEGER
********************************************************************************/
typedef struct{
uint16_t Time_UE_StayedInCell_EnhancedGranularity;
}LIBLTE_S1AP_TIME_UE_STAYEDINCELL_ENHANCEDGRANULARITY_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_time_ue_stayedincell_enhancedgranularity(
  LIBLTE_S1AP_TIME_UE_STAYEDINCELL_ENHANCEDGRANULARITY_STRUCT  *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_time_ue_stayedincell_enhancedgranularity(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_TIME_UE_STAYEDINCELL_ENHANCEDGRANULARITY_STRUCT  *ie);

/*******************************************************************************
/* ProtocolIE E_UTRAN_Trace_ID STATIC OCTET STRING
********************************************************************************/
#define LIBLTE_S1AP_E_UTRAN_TRACE_ID_OCTET_STRING_LEN 8
typedef struct{
  uint8_t  buffer[8];
}LIBLTE_S1AP_E_UTRAN_TRACE_ID_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_e_utran_trace_id(
  LIBLTE_S1AP_E_UTRAN_TRACE_ID_STRUCT                          *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_e_utran_trace_id(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_E_UTRAN_TRACE_ID_STRUCT                          *ie);

/*******************************************************************************
/* ProtocolIE TypeOfError ENUMERATED
********************************************************************************/
typedef enum{
  LIBLTE_S1AP_TYPEOFERROR_NOT_UNDERSTOOD,
  LIBLTE_S1AP_TYPEOFERROR_MISSING,
  LIBLTE_S1AP_TYPEOFERROR_N_ITEMS,
}LIBLTE_S1AP_TYPEOFERROR_ENUM;
static const char liblte_s1ap_typeoferror_text[LIBLTE_S1AP_TYPEOFERROR_N_ITEMS][80] = {
  "not-understood",
  "missing",
};

typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_TYPEOFERROR_ENUM e;
}LIBLTE_S1AP_TYPEOFERROR_ENUM_EXT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_typeoferror(
  LIBLTE_S1AP_TYPEOFERROR_ENUM_EXT                             *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_typeoferror(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_TYPEOFERROR_ENUM_EXT                             *ie);

/*******************************************************************************
/* ProtocolIE UEAggregateMaximumBitrate SEQUENCE
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_BITRATE_STRUCT                                   uEaggregateMaximumBitRateDL;
  LIBLTE_S1AP_BITRATE_STRUCT                                   uEaggregateMaximumBitRateUL;
  LIBLTE_S1AP_PROTOCOLEXTENSIONCONTAINER_STRUCT                iE_Extensions;
  bool                                                         iE_Extensions_present;
}LIBLTE_S1AP_UEAGGREGATEMAXIMUMBITRATE_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_ueaggregatemaximumbitrate(
  LIBLTE_S1AP_UEAGGREGATEMAXIMUMBITRATE_STRUCT                 *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_ueaggregatemaximumbitrate(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_UEAGGREGATEMAXIMUMBITRATE_STRUCT                 *ie);

/*******************************************************************************
/* ProtocolIE UE_S1AP_ID_pair SEQUENCE
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_MME_UE_S1AP_ID_STRUCT                            mME_UE_S1AP_ID;
  LIBLTE_S1AP_ENB_UE_S1AP_ID_STRUCT                            eNB_UE_S1AP_ID;
  LIBLTE_S1AP_PROTOCOLEXTENSIONCONTAINER_STRUCT                iE_Extensions;
  bool                                                         iE_Extensions_present;
}LIBLTE_S1AP_UE_S1AP_ID_PAIR_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_ue_s1ap_id_pair(
  LIBLTE_S1AP_UE_S1AP_ID_PAIR_STRUCT                           *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_ue_s1ap_id_pair(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_UE_S1AP_ID_PAIR_STRUCT                           *ie);

/*******************************************************************************
/* ProtocolIE UEIdentityIndexValue STATIC BIT STRING
********************************************************************************/
#define LIBLTE_S1AP_UEIDENTITYINDEXVALUE_BIT_STRING_LEN 10
typedef struct{
  uint8_t  buffer[10];
}LIBLTE_S1AP_UEIDENTITYINDEXVALUE_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_ueidentityindexvalue(
  LIBLTE_S1AP_UEIDENTITYINDEXVALUE_STRUCT                      *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_ueidentityindexvalue(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_UEIDENTITYINDEXVALUE_STRUCT                      *ie);

/*******************************************************************************
/* ProtocolIE UESecurityCapabilities SEQUENCE
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_ENCRYPTIONALGORITHMS_STRUCT                      encryptionAlgorithms;
  LIBLTE_S1AP_INTEGRITYPROTECTIONALGORITHMS_STRUCT             integrityProtectionAlgorithms;
  LIBLTE_S1AP_PROTOCOLEXTENSIONCONTAINER_STRUCT                iE_Extensions;
  bool                                                         iE_Extensions_present;
}LIBLTE_S1AP_UESECURITYCAPABILITIES_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_uesecuritycapabilities(
  LIBLTE_S1AP_UESECURITYCAPABILITIES_STRUCT                    *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_uesecuritycapabilities(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_UESECURITYCAPABILITIES_STRUCT                    *ie);

/*******************************************************************************
/* ProtocolIE VoiceSupportMatchIndicator ENUMERATED
********************************************************************************/
typedef enum{
  LIBLTE_S1AP_VOICESUPPORTMATCHINDICATOR_SUPPORTED,
  LIBLTE_S1AP_VOICESUPPORTMATCHINDICATOR_NOT_SUPPORTED,
  LIBLTE_S1AP_VOICESUPPORTMATCHINDICATOR_N_ITEMS,
}LIBLTE_S1AP_VOICESUPPORTMATCHINDICATOR_ENUM;
static const char liblte_s1ap_voicesupportmatchindicator_text[LIBLTE_S1AP_VOICESUPPORTMATCHINDICATOR_N_ITEMS][80] = {
  "supported",
  "not-supported",
};

typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_VOICESUPPORTMATCHINDICATOR_ENUM e;
}LIBLTE_S1AP_VOICESUPPORTMATCHINDICATOR_ENUM_EXT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_voicesupportmatchindicator(
  LIBLTE_S1AP_VOICESUPPORTMATCHINDICATOR_ENUM_EXT              *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_voicesupportmatchindicator(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_VOICESUPPORTMATCHINDICATOR_ENUM_EXT              *ie);

/*******************************************************************************
/* ProtocolIE WarningSecurityInfo STATIC OCTET STRING
********************************************************************************/
#define LIBLTE_S1AP_WARNINGSECURITYINFO_OCTET_STRING_LEN 50
typedef struct{
  uint8_t  buffer[50];
}LIBLTE_S1AP_WARNINGSECURITYINFO_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_warningsecurityinfo(
  LIBLTE_S1AP_WARNINGSECURITYINFO_STRUCT                       *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_warningsecurityinfo(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_WARNINGSECURITYINFO_STRUCT                       *ie);

/*******************************************************************************
/* ProtocolIE ENBX2GTPTLAs DYNAMIC SEQUENCE OF
********************************************************************************/
// lb:1, ub:16
typedef struct{
  uint32_t                                                     len;
  LIBLTE_S1AP_TRANSPORTLAYERADDRESS_STRUCT                     buffer[32]; //WARNING: Artificial limit to reduce memory footprint
}LIBLTE_S1AP_ENBX2GTPTLAS_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_enbx2gtptlas(
  LIBLTE_S1AP_ENBX2GTPTLAS_STRUCT                              *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_enbx2gtptlas(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_ENBX2GTPTLAS_STRUCT                              *ie);

/*******************************************************************************
/* ProtocolIE CauseTransport ENUMERATED
********************************************************************************/
typedef enum{
  LIBLTE_S1AP_CAUSETRANSPORT_TRANSPORT_RESOURCE_UNAVAILABLE,
  LIBLTE_S1AP_CAUSETRANSPORT_UNSPECIFIED,
  LIBLTE_S1AP_CAUSETRANSPORT_N_ITEMS,
}LIBLTE_S1AP_CAUSETRANSPORT_ENUM;
static const char liblte_s1ap_causetransport_text[LIBLTE_S1AP_CAUSETRANSPORT_N_ITEMS][80] = {
  "transport-resource-unavailable",
  "unspecified",
};

typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_CAUSETRANSPORT_ENUM e;
}LIBLTE_S1AP_CAUSETRANSPORT_ENUM_EXT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_causetransport(
  LIBLTE_S1AP_CAUSETRANSPORT_ENUM_EXT                          *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_causetransport(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_CAUSETRANSPORT_ENUM_EXT                          *ie);

/*******************************************************************************
/* ProtocolIE Cdma2000HOStatus ENUMERATED
********************************************************************************/
typedef enum{
  LIBLTE_S1AP_CDMA2000HOSTATUS_HOSUCCESS,
  LIBLTE_S1AP_CDMA2000HOSTATUS_HOFAILURE,
  LIBLTE_S1AP_CDMA2000HOSTATUS_N_ITEMS,
}LIBLTE_S1AP_CDMA2000HOSTATUS_ENUM;
static const char liblte_s1ap_cdma2000hostatus_text[LIBLTE_S1AP_CDMA2000HOSTATUS_N_ITEMS][80] = {
  "hOSuccess",
  "hOFailure",
};

typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_CDMA2000HOSTATUS_ENUM e;
}LIBLTE_S1AP_CDMA2000HOSTATUS_ENUM_EXT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_cdma2000hostatus(
  LIBLTE_S1AP_CDMA2000HOSTATUS_ENUM_EXT                        *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_cdma2000hostatus(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_CDMA2000HOSTATUS_ENUM_EXT                        *ie);

/*******************************************************************************
/* ProtocolIE Cdma2000OneXPilot DYNAMIC OCTET STRING
********************************************************************************/
// lb:0, ub:16318
typedef struct{
  uint32_t n_octets;
  uint8_t  buffer[16318];
}LIBLTE_S1AP_CDMA2000ONEXPILOT_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_cdma2000onexpilot(
  LIBLTE_S1AP_CDMA2000ONEXPILOT_STRUCT                         *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_cdma2000onexpilot(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_CDMA2000ONEXPILOT_STRUCT                         *ie);

/*******************************************************************************
/* ProtocolIE ConcurrentWarningMessageIndicator ENUMERATED
********************************************************************************/
typedef enum{
  LIBLTE_S1AP_CONCURRENTWARNINGMESSAGEINDICATOR_TRUE,
  LIBLTE_S1AP_CONCURRENTWARNINGMESSAGEINDICATOR_N_ITEMS,
}LIBLTE_S1AP_CONCURRENTWARNINGMESSAGEINDICATOR_ENUM;
static const char liblte_s1ap_concurrentwarningmessageindicator_text[LIBLTE_S1AP_CONCURRENTWARNINGMESSAGEINDICATOR_N_ITEMS][80] = {
  "true",
};

LIBLTE_ERROR_ENUM liblte_s1ap_pack_concurrentwarningmessageindicator(
  LIBLTE_S1AP_CONCURRENTWARNINGMESSAGEINDICATOR_ENUM           *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_concurrentwarningmessageindicator(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_CONCURRENTWARNINGMESSAGEINDICATOR_ENUM           *ie);

/*******************************************************************************
/* ProtocolIE COUNTvalue SEQUENCE
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_PDCP_SN_STRUCT                                   pDCP_SN;
  LIBLTE_S1AP_HFN_STRUCT                                       hFN;
  LIBLTE_S1AP_PROTOCOLEXTENSIONCONTAINER_STRUCT                iE_Extensions;
  bool                                                         iE_Extensions_present;
}LIBLTE_S1AP_COUNTVALUE_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_countvalue(
  LIBLTE_S1AP_COUNTVALUE_STRUCT                                *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_countvalue(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_COUNTVALUE_STRUCT                                *ie);

/*******************************************************************************
/* ProtocolIE CriticalityDiagnostics_IE_Item SEQUENCE
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_CRITICALITY_ENUM                                 iECriticality;
  LIBLTE_S1AP_PROTOCOLIE_ID_STRUCT                             iE_ID;
  LIBLTE_S1AP_TYPEOFERROR_ENUM_EXT                             typeOfError;
  LIBLTE_S1AP_PROTOCOLEXTENSIONCONTAINER_STRUCT                iE_Extensions;
  bool                                                         iE_Extensions_present;
}LIBLTE_S1AP_CRITICALITYDIAGNOSTICS_IE_ITEM_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_criticalitydiagnostics_ie_item(
  LIBLTE_S1AP_CRITICALITYDIAGNOSTICS_IE_ITEM_STRUCT            *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_criticalitydiagnostics_ie_item(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_CRITICALITYDIAGNOSTICS_IE_ITEM_STRUCT            *ie);

/*******************************************************************************
/* ProtocolIE ENBX2TLAs DYNAMIC SEQUENCE OF
********************************************************************************/
// lb:1, ub:2
typedef struct{
  uint32_t                                                     len;
  LIBLTE_S1AP_TRANSPORTLAYERADDRESS_STRUCT                     buffer[32]; //WARNING: Artificial limit to reduce memory footprint
}LIBLTE_S1AP_ENBX2TLAS_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_enbx2tlas(
  LIBLTE_S1AP_ENBX2TLAS_STRUCT                                 *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_enbx2tlas(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_ENBX2TLAS_STRUCT                                 *ie);

/*******************************************************************************
/* ProtocolIE ExtendedRepetitionPeriod INTEGER
********************************************************************************/
typedef struct{
uint32_t ExtendedRepetitionPeriod;
}LIBLTE_S1AP_EXTENDEDREPETITIONPERIOD_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_extendedrepetitionperiod(
  LIBLTE_S1AP_EXTENDEDREPETITIONPERIOD_STRUCT                  *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_extendedrepetitionperiod(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_EXTENDEDREPETITIONPERIOD_STRUCT                  *ie);

/*******************************************************************************
/* ProtocolIE ForbiddenTACs DYNAMIC SEQUENCE OF
********************************************************************************/
// lb:1, ub:4096
typedef struct{
  uint32_t                                                     len;
  LIBLTE_S1AP_TAC_STRUCT                                       buffer[32]; //WARNING: Artificial limit to reduce memory footprint
}LIBLTE_S1AP_FORBIDDENTACS_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_forbiddentacs(
  LIBLTE_S1AP_FORBIDDENTACS_STRUCT                             *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_forbiddentacs(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_FORBIDDENTACS_STRUCT                             *ie);

/*******************************************************************************
/* ProtocolIE GBR_QosInformation SEQUENCE
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_BITRATE_STRUCT                                   e_RAB_MaximumBitrateDL;
  LIBLTE_S1AP_BITRATE_STRUCT                                   e_RAB_MaximumBitrateUL;
  LIBLTE_S1AP_BITRATE_STRUCT                                   e_RAB_GuaranteedBitrateDL;
  LIBLTE_S1AP_BITRATE_STRUCT                                   e_RAB_GuaranteedBitrateUL;
  LIBLTE_S1AP_PROTOCOLEXTENSIONCONTAINER_STRUCT                iE_Extensions;
  bool                                                         iE_Extensions_present;
}LIBLTE_S1AP_GBR_QOSINFORMATION_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_gbr_qosinformation(
  LIBLTE_S1AP_GBR_QOSINFORMATION_STRUCT                        *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_gbr_qosinformation(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_GBR_QOSINFORMATION_STRUCT                        *ie);

/*******************************************************************************
/* ProtocolIE HFNModified INTEGER
********************************************************************************/
typedef struct{
uint32_t HFNModified;
}LIBLTE_S1AP_HFNMODIFIED_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_hfnmodified(
  LIBLTE_S1AP_HFNMODIFIED_STRUCT                               *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_hfnmodified(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_HFNMODIFIED_STRUCT                               *ie);

/*******************************************************************************
/* ProtocolIE KillAllWarningMessages ENUMERATED
********************************************************************************/
typedef enum{
  LIBLTE_S1AP_KILLALLWARNINGMESSAGES_TRUE,
  LIBLTE_S1AP_KILLALLWARNINGMESSAGES_N_ITEMS,
}LIBLTE_S1AP_KILLALLWARNINGMESSAGES_ENUM;
static const char liblte_s1ap_killallwarningmessages_text[LIBLTE_S1AP_KILLALLWARNINGMESSAGES_N_ITEMS][80] = {
  "true",
};

LIBLTE_ERROR_ENUM liblte_s1ap_pack_killallwarningmessages(
  LIBLTE_S1AP_KILLALLWARNINGMESSAGES_ENUM                      *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_killallwarningmessages(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_KILLALLWARNINGMESSAGES_ENUM                      *ie);

/*******************************************************************************
/* ProtocolIE LPPa_PDU DYNAMIC OCTET STRING
********************************************************************************/
// lb:0, ub:16318
typedef struct{
  uint32_t n_octets;
  uint8_t  buffer[16318];
}LIBLTE_S1AP_LPPA_PDU_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_lppa_pdu(
  LIBLTE_S1AP_LPPA_PDU_STRUCT                                  *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_lppa_pdu(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_LPPA_PDU_STRUCT                                  *ie);

/*******************************************************************************
/* ProtocolIE M3Configuration SEQUENCE
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_M3PERIOD_ENUM_EXT                                m3period;
  LIBLTE_S1AP_PROTOCOLEXTENSIONCONTAINER_STRUCT                iE_Extensions;
  bool                                                         iE_Extensions_present;
}LIBLTE_S1AP_M3CONFIGURATION_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_m3configuration(
  LIBLTE_S1AP_M3CONFIGURATION_STRUCT                           *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_m3configuration(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_M3CONFIGURATION_STRUCT                           *ie);

/*******************************************************************************
/* ProtocolIE M5Configuration SEQUENCE
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_M5PERIOD_ENUM_EXT                                m5period;
  LIBLTE_S1AP_LINKS_TO_LOG_ENUM_EXT                            m5_links_to_log;
  LIBLTE_S1AP_PROTOCOLEXTENSIONCONTAINER_STRUCT                iE_Extensions;
  bool                                                         iE_Extensions_present;
}LIBLTE_S1AP_M5CONFIGURATION_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_m5configuration(
  LIBLTE_S1AP_M5CONFIGURATION_STRUCT                           *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_m5configuration(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_M5CONFIGURATION_STRUCT                           *ie);

/*******************************************************************************
/* ProtocolIE MeasurementThresholdA2 CHOICE
********************************************************************************/
typedef enum{
  LIBLTE_S1AP_MEASUREMENTTHRESHOLDA2_CHOICE_THRESHOLD_RSRP,
  LIBLTE_S1AP_MEASUREMENTTHRESHOLDA2_CHOICE_THRESHOLD_RSRQ,
  LIBLTE_S1AP_MEASUREMENTTHRESHOLDA2_CHOICE_N_ITEMS,
}LIBLTE_S1AP_MEASUREMENTTHRESHOLDA2_CHOICE_ENUM;
static const char liblte_s1ap_measurementthresholda2_choice_text[LIBLTE_S1AP_MEASUREMENTTHRESHOLDA2_CHOICE_N_ITEMS][50] = {
  "threshold_RSRP",
  "threshold_RSRQ",
};

typedef union{
  LIBLTE_S1AP_THRESHOLD_RSRP_STRUCT                            threshold_RSRP;
  LIBLTE_S1AP_THRESHOLD_RSRQ_STRUCT                            threshold_RSRQ;
}LIBLTE_S1AP_MEASUREMENTTHRESHOLDA2_CHOICE_UNION;

typedef struct{
  bool                            ext;
  LIBLTE_S1AP_MEASUREMENTTHRESHOLDA2_CHOICE_UNION choice;
  LIBLTE_S1AP_MEASUREMENTTHRESHOLDA2_CHOICE_ENUM  choice_type;
}LIBLTE_S1AP_MEASUREMENTTHRESHOLDA2_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_measurementthresholda2(
  LIBLTE_S1AP_MEASUREMENTTHRESHOLDA2_STRUCT                    *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_measurementthresholda2(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MEASUREMENTTHRESHOLDA2_STRUCT                    *ie);

/*******************************************************************************
/* ProtocolIE M_TMSI STATIC OCTET STRING
********************************************************************************/
#define LIBLTE_S1AP_M_TMSI_OCTET_STRING_LEN 4
typedef struct{
  uint8_t  buffer[4];
}LIBLTE_S1AP_M_TMSI_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_m_tmsi(
  LIBLTE_S1AP_M_TMSI_STRUCT                                    *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_m_tmsi(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_M_TMSI_STRUCT                                    *ie);

/*******************************************************************************
/* ProtocolIE OldBSS_ToNewBSS_Information DYNAMIC OCTET STRING
********************************************************************************/
// lb:0, ub:16318
typedef struct{
  uint32_t n_octets;
  uint8_t  buffer[16318];
}LIBLTE_S1AP_OLDBSS_TONEWBSS_INFORMATION_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_oldbss_tonewbss_information(
  LIBLTE_S1AP_OLDBSS_TONEWBSS_INFORMATION_STRUCT               *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_oldbss_tonewbss_information(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_OLDBSS_TONEWBSS_INFORMATION_STRUCT               *ie);

/*******************************************************************************
/* ProtocolIE PLMNidentity STATIC OCTET STRING
********************************************************************************/
#define LIBLTE_S1AP_PLMNIDENTITY_OCTET_STRING_LEN 3
typedef struct{
  uint8_t  buffer[3];
}LIBLTE_S1AP_PLMNIDENTITY_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_plmnidentity(
  LIBLTE_S1AP_PLMNIDENTITY_STRUCT                              *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_plmnidentity(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_PLMNIDENTITY_STRUCT                              *ie);

/*******************************************************************************
/* ProtocolIE ReceiveStatusOfULPDCPSDUsExtended DYNAMIC BIT STRING
********************************************************************************/
// lb:1, ub:16384
typedef struct{
  uint32_t n_bits;
  uint8_t  buffer[16384];
}LIBLTE_S1AP_RECEIVESTATUSOFULPDCPSDUSEXTENDED_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_receivestatusofulpdcpsdusextended(
  LIBLTE_S1AP_RECEIVESTATUSOFULPDCPSDUSEXTENDED_STRUCT         *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_receivestatusofulpdcpsdusextended(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_RECEIVESTATUSOFULPDCPSDUSEXTENDED_STRUCT         *ie);

/*******************************************************************************
/* ProtocolIE RequestType SEQUENCE
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_EVENTTYPE_ENUM_EXT                               eventType;
  LIBLTE_S1AP_REPORTAREA_ENUM_EXT                              reportArea;
  LIBLTE_S1AP_PROTOCOLEXTENSIONCONTAINER_STRUCT                iE_Extensions;
  bool                                                         iE_Extensions_present;
}LIBLTE_S1AP_REQUESTTYPE_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_requesttype(
  LIBLTE_S1AP_REQUESTTYPE_STRUCT                               *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_requesttype(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_REQUESTTYPE_STRUCT                               *ie);

/*******************************************************************************
/* ProtocolIE RRC_Container DYNAMIC OCTET STRING
********************************************************************************/
// lb:0, ub:16318
typedef struct{
  uint32_t n_octets;
  uint8_t  buffer[16318];
}LIBLTE_S1AP_RRC_CONTAINER_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_rrc_container(
  LIBLTE_S1AP_RRC_CONTAINER_STRUCT                             *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_rrc_container(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_RRC_CONTAINER_STRUCT                             *ie);

/*******************************************************************************
/* ProtocolIE nextHopChainingCount INTEGER
********************************************************************************/
typedef struct{
uint8_t nextHopChainingCount;
}LIBLTE_S1AP_NEXTHOPCHAININGCOUNT_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_nexthopchainingcount(
  LIBLTE_S1AP_NEXTHOPCHAININGCOUNT_STRUCT                      *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_nexthopchainingcount(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_NEXTHOPCHAININGCOUNT_STRUCT                      *ie);

/*******************************************************************************
/* ProtocolIE SecurityContext SEQUENCE
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_NEXTHOPCHAININGCOUNT_STRUCT                      nextHopChainingCount;
  LIBLTE_S1AP_SECURITYKEY_STRUCT                               nextHopParameter;
  LIBLTE_S1AP_PROTOCOLEXTENSIONCONTAINER_STRUCT                iE_Extensions;
  bool                                                         iE_Extensions_present;
}LIBLTE_S1AP_SECURITYCONTEXT_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_securitycontext(
  LIBLTE_S1AP_SECURITYCONTEXT_STRUCT                           *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_securitycontext(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_SECURITYCONTEXT_STRUCT                           *ie);

/*******************************************************************************
/* ProtocolIE ServedMMECs DYNAMIC SEQUENCE OF
********************************************************************************/
// lb:1, ub:256
typedef struct{
  uint32_t                                                     len;
  LIBLTE_S1AP_MME_CODE_STRUCT                                  buffer[32]; //WARNING: Artificial limit to reduce memory footprint
}LIBLTE_S1AP_SERVEDMMECS_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_servedmmecs(
  LIBLTE_S1AP_SERVEDMMECS_STRUCT                               *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_servedmmecs(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_SERVEDMMECS_STRUCT                               *ie);

/*******************************************************************************
/* ProtocolIE TimeSynchronizationInfo SEQUENCE
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_STRATUMLEVEL_STRUCT                              stratumLevel;
  LIBLTE_S1AP_SYNCHRONIZATIONSTATUS_ENUM_EXT                   synchronizationStatus;
  LIBLTE_S1AP_PROTOCOLEXTENSIONCONTAINER_STRUCT                iE_Extensions;
  bool                                                         iE_Extensions_present;
}LIBLTE_S1AP_TIMESYNCHRONIZATIONINFO_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_timesynchronizationinfo(
  LIBLTE_S1AP_TIMESYNCHRONIZATIONINFO_STRUCT                   *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_timesynchronizationinfo(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_TIMESYNCHRONIZATIONINFO_STRUCT                   *ie);

/*******************************************************************************
/* ProtocolIE TAI SEQUENCE
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_TBCD_STRING_STRUCT                               pLMNidentity;
  LIBLTE_S1AP_TAC_STRUCT                                       tAC;
  LIBLTE_S1AP_PROTOCOLEXTENSIONCONTAINER_STRUCT                iE_Extensions;
  bool                                                         iE_Extensions_present;
}LIBLTE_S1AP_TAI_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_tai(
  LIBLTE_S1AP_TAI_STRUCT                                       *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_tai(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_TAI_STRUCT                                       *ie);

/*******************************************************************************
/* ProtocolIE TABasedMDT SEQUENCE
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_TALISTFORMDT_STRUCT                              tAListforMDT;
  LIBLTE_S1AP_PROTOCOLEXTENSIONCONTAINER_STRUCT                iE_Extensions;
  bool                                                         iE_Extensions_present;
}LIBLTE_S1AP_TABASEDMDT_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_tabasedmdt(
  LIBLTE_S1AP_TABASEDMDT_STRUCT                                *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_tabasedmdt(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_TABASEDMDT_STRUCT                                *ie);

/*******************************************************************************
/* ProtocolIE TargeteNB_ToSourceeNB_TransparentContainer SEQUENCE
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_RRC_CONTAINER_STRUCT                             rRC_Container;
  LIBLTE_S1AP_PROTOCOLEXTENSIONCONTAINER_STRUCT                iE_Extensions;
  bool                                                         iE_Extensions_present;
}LIBLTE_S1AP_TARGETENB_TOSOURCEENB_TRANSPARENTCONTAINER_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_targetenb_tosourceenb_transparentcontainer(
  LIBLTE_S1AP_TARGETENB_TOSOURCEENB_TRANSPARENTCONTAINER_STRUCT *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_targetenb_tosourceenb_transparentcontainer(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_TARGETENB_TOSOURCEENB_TRANSPARENTCONTAINER_STRUCT *ie);

/*******************************************************************************
/* ProtocolIE M1ThresholdEventA2 SEQUENCE
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_MEASUREMENTTHRESHOLDA2_STRUCT                    measurementThreshold;
  LIBLTE_S1AP_PROTOCOLEXTENSIONCONTAINER_STRUCT                iE_Extensions;
  bool                                                         iE_Extensions_present;
}LIBLTE_S1AP_M1THRESHOLDEVENTA2_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_m1thresholdeventa2(
  LIBLTE_S1AP_M1THRESHOLDEVENTA2_STRUCT                        *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_m1thresholdeventa2(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_M1THRESHOLDEVENTA2_STRUCT                        *ie);

/*******************************************************************************
/* ProtocolIE TransportInformation SEQUENCE
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_TRANSPORTLAYERADDRESS_STRUCT                     transportLayerAddress;
  LIBLTE_S1AP_GTP_TEID_STRUCT                                  uL_GTP_TEID;
}LIBLTE_S1AP_TRANSPORTINFORMATION_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_transportinformation(
  LIBLTE_S1AP_TRANSPORTINFORMATION_STRUCT                      *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_transportinformation(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_TRANSPORTINFORMATION_STRUCT                      *ie);

/*******************************************************************************
/* ProtocolIE TunnelInformation SEQUENCE
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_TRANSPORTLAYERADDRESS_STRUCT                     transportLayerAddress;
  LIBLTE_S1AP_PORT_NUMBER_STRUCT                               uDP_Port_Number;
  bool                                                         uDP_Port_Number_present;
  LIBLTE_S1AP_PROTOCOLEXTENSIONCONTAINER_STRUCT                iE_Extensions;
  bool                                                         iE_Extensions_present;
}LIBLTE_S1AP_TUNNELINFORMATION_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_tunnelinformation(
  LIBLTE_S1AP_TUNNELINFORMATION_STRUCT                         *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_tunnelinformation(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_TUNNELINFORMATION_STRUCT                         *ie);

/*******************************************************************************
/* ProtocolIE UE_S1AP_IDs CHOICE
********************************************************************************/
typedef enum{
  LIBLTE_S1AP_UE_S1AP_IDS_CHOICE_UE_S1AP_ID_PAIR,
  LIBLTE_S1AP_UE_S1AP_IDS_CHOICE_MME_UE_S1AP_ID,
  LIBLTE_S1AP_UE_S1AP_IDS_CHOICE_N_ITEMS,
}LIBLTE_S1AP_UE_S1AP_IDS_CHOICE_ENUM;
static const char liblte_s1ap_ue_s1ap_ids_choice_text[LIBLTE_S1AP_UE_S1AP_IDS_CHOICE_N_ITEMS][50] = {
  "uE_S1AP_ID_pair",
  "mME_UE_S1AP_ID",
};

typedef union{
  LIBLTE_S1AP_UE_S1AP_ID_PAIR_STRUCT                           uE_S1AP_ID_pair;
  LIBLTE_S1AP_MME_UE_S1AP_ID_STRUCT                            mME_UE_S1AP_ID;
}LIBLTE_S1AP_UE_S1AP_IDS_CHOICE_UNION;

typedef struct{
  bool                            ext;
  LIBLTE_S1AP_UE_S1AP_IDS_CHOICE_UNION choice;
  LIBLTE_S1AP_UE_S1AP_IDS_CHOICE_ENUM  choice_type;
}LIBLTE_S1AP_UE_S1AP_IDS_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_ue_s1ap_ids(
  LIBLTE_S1AP_UE_S1AP_IDS_STRUCT                               *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_ue_s1ap_ids(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_UE_S1AP_IDS_STRUCT                               *ie);

/*******************************************************************************
/* ProtocolIE ENBX2ExtTLA SEQUENCE
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_TRANSPORTLAYERADDRESS_STRUCT                     iPsecTLA;
  bool                                                         iPsecTLA_present;
  LIBLTE_S1AP_ENBX2GTPTLAS_STRUCT                              gTPTLAa;
  bool                                                         gTPTLAa_present;
  LIBLTE_S1AP_PROTOCOLEXTENSIONCONTAINER_STRUCT                iE_Extensions;
  bool                                                         iE_Extensions_present;
}LIBLTE_S1AP_ENBX2EXTTLA_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_enbx2exttla(
  LIBLTE_S1AP_ENBX2EXTTLA_STRUCT                               *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_enbx2exttla(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_ENBX2EXTTLA_STRUCT                               *ie);

/*******************************************************************************
/* ProtocolIE BPLMNs DYNAMIC SEQUENCE OF
********************************************************************************/
// lb:1, ub:6
typedef struct{
  uint32_t                                                     len;
  LIBLTE_S1AP_TBCD_STRING_STRUCT                               buffer[32]; //WARNING: Artificial limit to reduce memory footprint
}LIBLTE_S1AP_BPLMNS_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_bplmns(
  LIBLTE_S1AP_BPLMNS_STRUCT                                    *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_bplmns(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_BPLMNS_STRUCT                                    *ie);

/*******************************************************************************
/* ProtocolIE Cause CHOICE
********************************************************************************/
typedef enum{
  LIBLTE_S1AP_CAUSE_CHOICE_RADIONETWORK,
  LIBLTE_S1AP_CAUSE_CHOICE_TRANSPORT,
  LIBLTE_S1AP_CAUSE_CHOICE_NAS,
  LIBLTE_S1AP_CAUSE_CHOICE_PROTOCOL,
  LIBLTE_S1AP_CAUSE_CHOICE_MISC,
  LIBLTE_S1AP_CAUSE_CHOICE_N_ITEMS,
}LIBLTE_S1AP_CAUSE_CHOICE_ENUM;
static const char liblte_s1ap_cause_choice_text[LIBLTE_S1AP_CAUSE_CHOICE_N_ITEMS][50] = {
  "radioNetwork",
  "transport",
  "nas",
  "protocol",
  "misc",
};

typedef union{
  LIBLTE_S1AP_CAUSERADIONETWORK_ENUM_EXT                       radioNetwork;
  LIBLTE_S1AP_CAUSETRANSPORT_ENUM_EXT                          transport;
  LIBLTE_S1AP_CAUSENAS_ENUM_EXT                                nas;
  LIBLTE_S1AP_CAUSEPROTOCOL_ENUM_EXT                           protocol;
  LIBLTE_S1AP_CAUSEMISC_ENUM_EXT                               misc;
}LIBLTE_S1AP_CAUSE_CHOICE_UNION;

typedef struct{
  bool                            ext;
  LIBLTE_S1AP_CAUSE_CHOICE_UNION choice;
  LIBLTE_S1AP_CAUSE_CHOICE_ENUM  choice_type;
}LIBLTE_S1AP_CAUSE_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_cause(
  LIBLTE_S1AP_CAUSE_STRUCT                                     *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_cause(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_CAUSE_STRUCT                                     *ie);

/*******************************************************************************
/* ProtocolIE Cdma2000OneXSRVCCInfo SEQUENCE
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_CDMA2000ONEXMEID_STRUCT                          cdma2000OneXMEID;
  LIBLTE_S1AP_CDMA2000ONEXMSI_STRUCT                           cdma2000OneXMSI;
  LIBLTE_S1AP_CDMA2000ONEXPILOT_STRUCT                         cdma2000OneXPilot;
  LIBLTE_S1AP_PROTOCOLEXTENSIONCONTAINER_STRUCT                iE_Extensions;
  bool                                                         iE_Extensions_present;
}LIBLTE_S1AP_CDMA2000ONEXSRVCCINFO_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_cdma2000onexsrvccinfo(
  LIBLTE_S1AP_CDMA2000ONEXSRVCCINFO_STRUCT                     *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_cdma2000onexsrvccinfo(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_CDMA2000ONEXSRVCCINFO_STRUCT                     *ie);

/*******************************************************************************
/* ProtocolIE CGI SEQUENCE
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_TBCD_STRING_STRUCT                               pLMNidentity;
  LIBLTE_S1AP_LAC_STRUCT                                       lAC;
  LIBLTE_S1AP_CI_STRUCT                                        cI;
  LIBLTE_S1AP_RAC_STRUCT                                       rAC;
  bool                                                         rAC_present;
  LIBLTE_S1AP_PROTOCOLEXTENSIONCONTAINER_STRUCT                iE_Extensions;
  bool                                                         iE_Extensions_present;
}LIBLTE_S1AP_CGI_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_cgi(
  LIBLTE_S1AP_CGI_STRUCT                                       *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_cgi(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_CGI_STRUCT                                       *ie);

/*******************************************************************************
/* ProtocolIE COUNTValueExtended SEQUENCE
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_PDCP_SNEXTENDED_STRUCT                           pDCP_SNExtended;
  LIBLTE_S1AP_HFNMODIFIED_STRUCT                               hFNModified;
  LIBLTE_S1AP_PROTOCOLEXTENSIONCONTAINER_STRUCT                iE_Extensions;
  bool                                                         iE_Extensions_present;
}LIBLTE_S1AP_COUNTVALUEEXTENDED_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_countvalueextended(
  LIBLTE_S1AP_COUNTVALUEEXTENDED_STRUCT                        *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_countvalueextended(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_COUNTVALUEEXTENDED_STRUCT                        *ie);

/*******************************************************************************
/* ProtocolIE CriticalityDiagnostics_IE_List DYNAMIC SEQUENCE OF
********************************************************************************/
// lb:1, ub:256
typedef struct{
  uint32_t                                                     len;
  LIBLTE_S1AP_CRITICALITYDIAGNOSTICS_IE_ITEM_STRUCT            buffer[32]; //WARNING: Artificial limit to reduce memory footprint
}LIBLTE_S1AP_CRITICALITYDIAGNOSTICS_IE_LIST_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_criticalitydiagnostics_ie_list(
  LIBLTE_S1AP_CRITICALITYDIAGNOSTICS_IE_LIST_STRUCT            *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_criticalitydiagnostics_ie_list(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_CRITICALITYDIAGNOSTICS_IE_LIST_STRUCT            *ie);

/*******************************************************************************
/* ProtocolIE Global_ENB_ID SEQUENCE
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_TBCD_STRING_STRUCT                               pLMNidentity;
  LIBLTE_S1AP_ENB_ID_STRUCT                                    eNB_ID;
  LIBLTE_S1AP_PROTOCOLEXTENSIONCONTAINER_STRUCT                iE_Extensions;
  bool                                                         iE_Extensions_present;
}LIBLTE_S1AP_GLOBAL_ENB_ID_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_global_enb_id(
  LIBLTE_S1AP_GLOBAL_ENB_ID_STRUCT                             *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_global_enb_id(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_GLOBAL_ENB_ID_STRUCT                             *ie);

/*******************************************************************************
/* ProtocolIE EPLMNs DYNAMIC SEQUENCE OF
********************************************************************************/
// lb:1, ub:15
typedef struct{
  uint32_t                                                     len;
  LIBLTE_S1AP_TBCD_STRING_STRUCT                               buffer[32]; //WARNING: Artificial limit to reduce memory footprint
}LIBLTE_S1AP_EPLMNS_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_eplmns(
  LIBLTE_S1AP_EPLMNS_STRUCT                                    *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_eplmns(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_EPLMNS_STRUCT                                    *ie);

/*******************************************************************************
/* ProtocolIE E_RABItem SEQUENCE
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_E_RAB_ID_STRUCT                                  e_RAB_ID;
  LIBLTE_S1AP_CAUSE_STRUCT                                     cause;
  LIBLTE_S1AP_PROTOCOLEXTENSIONCONTAINER_STRUCT                iE_Extensions;
  bool                                                         iE_Extensions_present;
}LIBLTE_S1AP_E_RABITEM_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_e_rabitem(
  LIBLTE_S1AP_E_RABITEM_STRUCT                                 *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_e_rabitem(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_E_RABITEM_STRUCT                                 *ie);

/*******************************************************************************
/* ProtocolIE EUTRAN_CGI SEQUENCE
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_TBCD_STRING_STRUCT                               pLMNidentity;
  LIBLTE_S1AP_CELLIDENTITY_STRUCT                              cell_ID;
  LIBLTE_S1AP_PROTOCOLEXTENSIONCONTAINER_STRUCT                iE_Extensions;
  bool                                                         iE_Extensions_present;
}LIBLTE_S1AP_EUTRAN_CGI_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_eutran_cgi(
  LIBLTE_S1AP_EUTRAN_CGI_STRUCT                                *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_eutran_cgi(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_EUTRAN_CGI_STRUCT                                *ie);

/*******************************************************************************
/* ProtocolIE ForbiddenTAs_Item SEQUENCE
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_TBCD_STRING_STRUCT                               pLMN_Identity;
  LIBLTE_S1AP_FORBIDDENTACS_STRUCT                             forbiddenTACs;
  LIBLTE_S1AP_PROTOCOLEXTENSIONCONTAINER_STRUCT                iE_Extensions;
  bool                                                         iE_Extensions_present;
}LIBLTE_S1AP_FORBIDDENTAS_ITEM_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_forbiddentas_item(
  LIBLTE_S1AP_FORBIDDENTAS_ITEM_STRUCT                         *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_forbiddentas_item(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_FORBIDDENTAS_ITEM_STRUCT                         *ie);

/*******************************************************************************
/* ProtocolIE ForbiddenLAs_Item SEQUENCE
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_TBCD_STRING_STRUCT                               pLMN_Identity;
  LIBLTE_S1AP_FORBIDDENLACS_STRUCT                             forbiddenLACs;
  LIBLTE_S1AP_PROTOCOLEXTENSIONCONTAINER_STRUCT                iE_Extensions;
  bool                                                         iE_Extensions_present;
}LIBLTE_S1AP_FORBIDDENLAS_ITEM_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_forbiddenlas_item(
  LIBLTE_S1AP_FORBIDDENLAS_ITEM_STRUCT                         *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_forbiddenlas_item(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_FORBIDDENLAS_ITEM_STRUCT                         *ie);

/*******************************************************************************
/* ProtocolIE LAI SEQUENCE
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_TBCD_STRING_STRUCT                               pLMNidentity;
  LIBLTE_S1AP_LAC_STRUCT                                       lAC;
  LIBLTE_S1AP_PROTOCOLEXTENSIONCONTAINER_STRUCT                iE_Extensions;
  bool                                                         iE_Extensions_present;
}LIBLTE_S1AP_LAI_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_lai(
  LIBLTE_S1AP_LAI_STRUCT                                       *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_lai(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_LAI_STRUCT                                       *ie);

/*******************************************************************************
/* ProtocolIE M4Configuration SEQUENCE
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_M4PERIOD_ENUM_EXT                                m4period;
  LIBLTE_S1AP_LINKS_TO_LOG_ENUM_EXT                            m4_links_to_log;
  LIBLTE_S1AP_PROTOCOLEXTENSIONCONTAINER_STRUCT                iE_Extensions;
  bool                                                         iE_Extensions_present;
}LIBLTE_S1AP_M4CONFIGURATION_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_m4configuration(
  LIBLTE_S1AP_M4CONFIGURATION_STRUCT                           *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_m4configuration(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_M4CONFIGURATION_STRUCT                           *ie);

/*******************************************************************************
/* ProtocolIE MDTPLMNList DYNAMIC SEQUENCE OF
********************************************************************************/
// lb:1, ub:16
typedef struct{
  uint32_t                                                     len;
  LIBLTE_S1AP_TBCD_STRING_STRUCT                               buffer[32]; //WARNING: Artificial limit to reduce memory footprint
}LIBLTE_S1AP_MDTPLMNLIST_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_mdtplmnlist(
  LIBLTE_S1AP_MDTPLMNLIST_STRUCT                               *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_mdtplmnlist(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MDTPLMNLIST_STRUCT                               *ie);

/*******************************************************************************
/* ProtocolIE MMERelaySupportIndicator ENUMERATED
********************************************************************************/
typedef enum{
  LIBLTE_S1AP_MMERELAYSUPPORTINDICATOR_TRUE,
  LIBLTE_S1AP_MMERELAYSUPPORTINDICATOR_N_ITEMS,
}LIBLTE_S1AP_MMERELAYSUPPORTINDICATOR_ENUM;
static const char liblte_s1ap_mmerelaysupportindicator_text[LIBLTE_S1AP_MMERELAYSUPPORTINDICATOR_N_ITEMS][80] = {
  "true",
};

typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_MMERELAYSUPPORTINDICATOR_ENUM e;
}LIBLTE_S1AP_MMERELAYSUPPORTINDICATOR_ENUM_EXT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_mmerelaysupportindicator(
  LIBLTE_S1AP_MMERELAYSUPPORTINDICATOR_ENUM_EXT                *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_mmerelaysupportindicator(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MMERELAYSUPPORTINDICATOR_ENUM_EXT                *ie);

/*******************************************************************************
/* ProtocolIE PagingPriority ENUMERATED
********************************************************************************/
typedef enum{
  LIBLTE_S1AP_PAGINGPRIORITY_PRIOLEVEL1,
  LIBLTE_S1AP_PAGINGPRIORITY_PRIOLEVEL2,
  LIBLTE_S1AP_PAGINGPRIORITY_PRIOLEVEL3,
  LIBLTE_S1AP_PAGINGPRIORITY_PRIOLEVEL4,
  LIBLTE_S1AP_PAGINGPRIORITY_PRIOLEVEL5,
  LIBLTE_S1AP_PAGINGPRIORITY_PRIOLEVEL6,
  LIBLTE_S1AP_PAGINGPRIORITY_PRIOLEVEL7,
  LIBLTE_S1AP_PAGINGPRIORITY_PRIOLEVEL8,
  LIBLTE_S1AP_PAGINGPRIORITY_N_ITEMS,
}LIBLTE_S1AP_PAGINGPRIORITY_ENUM;
static const char liblte_s1ap_pagingpriority_text[LIBLTE_S1AP_PAGINGPRIORITY_N_ITEMS][80] = {
  "priolevel1",
  "priolevel2",
  "priolevel3",
  "priolevel4",
  "priolevel5",
  "priolevel6",
  "priolevel7",
  "priolevel8",
};

typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_PAGINGPRIORITY_ENUM e;
}LIBLTE_S1AP_PAGINGPRIORITY_ENUM_EXT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_pagingpriority(
  LIBLTE_S1AP_PAGINGPRIORITY_ENUM_EXT                          *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_pagingpriority(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_PAGINGPRIORITY_ENUM_EXT                          *ie);

/*******************************************************************************
/* ProtocolIE PriorityLevel INTEGER
********************************************************************************/
typedef struct{
uint8_t PriorityLevel;
}LIBLTE_S1AP_PRIORITYLEVEL_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_prioritylevel(
  LIBLTE_S1AP_PRIORITYLEVEL_STRUCT                             *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_prioritylevel(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_PRIORITYLEVEL_STRUCT                             *ie);

/*******************************************************************************
/* ProtocolIE ECGIListForRestart DYNAMIC SEQUENCE OF
********************************************************************************/
// lb:1, ub:256
typedef struct{
  uint32_t                                                     len;
  LIBLTE_S1AP_EUTRAN_CGI_STRUCT                                buffer[32]; //WARNING: Artificial limit to reduce memory footprint
}LIBLTE_S1AP_ECGILISTFORRESTART_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_ecgilistforrestart(
  LIBLTE_S1AP_ECGILISTFORRESTART_STRUCT                        *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_ecgilistforrestart(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_ECGILISTFORRESTART_STRUCT                        *ie);

/*******************************************************************************
/* ProtocolIE SourceeNB_ID SEQUENCE
********************************************************************************/
typedef struct{
  LIBLTE_S1AP_GLOBAL_ENB_ID_STRUCT                             global_ENB_ID;
  LIBLTE_S1AP_TAI_STRUCT                                       selected_TAI;
  LIBLTE_S1AP_PROTOCOLEXTENSIONCONTAINER_STRUCT                iE_Extensions;
  bool                                                         iE_Extensions_present;
}LIBLTE_S1AP_SOURCEENB_ID_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_sourceenb_id(
  LIBLTE_S1AP_SOURCEENB_ID_STRUCT                              *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_sourceenb_id(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_SOURCEENB_ID_STRUCT                              *ie);

/*******************************************************************************
/* ProtocolIE ServedPLMNs DYNAMIC SEQUENCE OF
********************************************************************************/
// lb:1, ub:32
typedef struct{
  uint32_t                                                     len;
  LIBLTE_S1AP_TBCD_STRING_STRUCT                               buffer[32]; //WARNING: Artificial limit to reduce memory footprint
}LIBLTE_S1AP_SERVEDPLMNS_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_servedplmns(
  LIBLTE_S1AP_SERVEDPLMNS_STRUCT                               *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_servedplmns(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_SERVEDPLMNS_STRUCT                               *ie);

/*******************************************************************************
/* ProtocolIE SupportedTAs_Item SEQUENCE
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_TAC_STRUCT                                       tAC;
  LIBLTE_S1AP_BPLMNS_STRUCT                                    broadcastPLMNs;
  LIBLTE_S1AP_PROTOCOLEXTENSIONCONTAINER_STRUCT                iE_Extensions;
  bool                                                         iE_Extensions_present;
}LIBLTE_S1AP_SUPPORTEDTAS_ITEM_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_supportedtas_item(
  LIBLTE_S1AP_SUPPORTEDTAS_ITEM_STRUCT                         *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_supportedtas_item(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_SUPPORTEDTAS_ITEM_STRUCT                         *ie);

/*******************************************************************************
/* ProtocolIE TAIListforMDT DYNAMIC SEQUENCE OF
********************************************************************************/
// lb:1, ub:8
typedef struct{
  uint32_t                                                     len;
  LIBLTE_S1AP_TAI_STRUCT                                       buffer[32]; //WARNING: Artificial limit to reduce memory footprint
}LIBLTE_S1AP_TAILISTFORMDT_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_tailistformdt(
  LIBLTE_S1AP_TAILISTFORMDT_STRUCT                             *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_tailistformdt(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_TAILISTFORMDT_STRUCT                             *ie);

/*******************************************************************************
/* ProtocolIE CompletedCellinTAI_Item SEQUENCE
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_EUTRAN_CGI_STRUCT                                eCGI;
  LIBLTE_S1AP_PROTOCOLEXTENSIONCONTAINER_STRUCT                iE_Extensions;
  bool                                                         iE_Extensions_present;
}LIBLTE_S1AP_COMPLETEDCELLINTAI_ITEM_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_completedcellintai_item(
  LIBLTE_S1AP_COMPLETEDCELLINTAI_ITEM_STRUCT                   *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_completedcellintai_item(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_COMPLETEDCELLINTAI_ITEM_STRUCT                   *ie);

/*******************************************************************************
/* ProtocolIE TargeteNB_ID SEQUENCE
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_GLOBAL_ENB_ID_STRUCT                             global_ENB_ID;
  LIBLTE_S1AP_TAI_STRUCT                                       selected_TAI;
  LIBLTE_S1AP_PROTOCOLEXTENSIONCONTAINER_STRUCT                iE_Extensions;
  bool                                                         iE_Extensions_present;
}LIBLTE_S1AP_TARGETENB_ID_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_targetenb_id(
  LIBLTE_S1AP_TARGETENB_ID_STRUCT                              *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_targetenb_id(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_TARGETENB_ID_STRUCT                              *ie);

/*******************************************************************************
/* ProtocolIE TargetBSS_ToSourceBSS_TransparentContainer DYNAMIC OCTET STRING
********************************************************************************/
// lb:0, ub:16318
typedef struct{
  uint32_t n_octets;
  uint8_t  buffer[16318];
}LIBLTE_S1AP_TARGETBSS_TOSOURCEBSS_TRANSPARENTCONTAINER_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_targetbss_tosourcebss_transparentcontainer(
  LIBLTE_S1AP_TARGETBSS_TOSOURCEBSS_TRANSPARENTCONTAINER_STRUCT *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_targetbss_tosourcebss_transparentcontainer(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_TARGETBSS_TOSOURCEBSS_TRANSPARENTCONTAINER_STRUCT *ie);

/*******************************************************************************
/* ProtocolIE TAIListForRestart DYNAMIC SEQUENCE OF
********************************************************************************/
// lb:1, ub:2048
typedef struct{
  uint32_t                                                     len;
  LIBLTE_S1AP_TAI_STRUCT                                       buffer[32]; //WARNING: Artificial limit to reduce memory footprint
}LIBLTE_S1AP_TAILISTFORRESTART_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_tailistforrestart(
  LIBLTE_S1AP_TAILISTFORRESTART_STRUCT                         *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_tailistforrestart(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_TAILISTFORRESTART_STRUCT                         *ie);

/*******************************************************************************
/* ProtocolIE UserLocationInformation SEQUENCE
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_EUTRAN_CGI_STRUCT                                eutran_cgi;
  LIBLTE_S1AP_TAI_STRUCT                                       tai;
  LIBLTE_S1AP_PROTOCOLEXTENSIONCONTAINER_STRUCT                iE_Extensions;
  bool                                                         iE_Extensions_present;
}LIBLTE_S1AP_USERLOCATIONINFORMATION_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_userlocationinformation(
  LIBLTE_S1AP_USERLOCATIONINFORMATION_STRUCT                   *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_userlocationinformation(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_USERLOCATIONINFORMATION_STRUCT                   *ie);

/*******************************************************************************
/* ProtocolIE ENBX2ExtTLAs DYNAMIC SEQUENCE OF
********************************************************************************/
// lb:1, ub:16
typedef struct{
  uint32_t                                                     len;
  LIBLTE_S1AP_ENBX2EXTTLA_STRUCT                               buffer[32]; //WARNING: Artificial limit to reduce memory footprint
}LIBLTE_S1AP_ENBX2EXTTLAS_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_enbx2exttlas(
  LIBLTE_S1AP_ENBX2EXTTLAS_STRUCT                              *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_enbx2exttlas(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_ENBX2EXTTLAS_STRUCT                              *ie);

/*******************************************************************************
/* ProtocolIE AllocationAndRetentionPriority SEQUENCE
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_PRIORITYLEVEL_STRUCT                             priorityLevel;
  LIBLTE_S1AP_PRE_EMPTIONCAPABILITY_ENUM                       pre_emptionCapability;
  LIBLTE_S1AP_PRE_EMPTIONVULNERABILITY_ENUM                    pre_emptionVulnerability;
  LIBLTE_S1AP_PROTOCOLEXTENSIONCONTAINER_STRUCT                iE_Extensions;
  bool                                                         iE_Extensions_present;
}LIBLTE_S1AP_ALLOCATIONANDRETENTIONPRIORITY_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_allocationandretentionpriority(
  LIBLTE_S1AP_ALLOCATIONANDRETENTIONPRIORITY_STRUCT            *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_allocationandretentionpriority(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_ALLOCATIONANDRETENTIONPRIORITY_STRUCT            *ie);

/*******************************************************************************
/* ProtocolIE CancelledCellinEAI_Item SEQUENCE
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_EUTRAN_CGI_STRUCT                                eCGI;
  LIBLTE_S1AP_NUMBEROFBROADCASTS_STRUCT                        numberOfBroadcasts;
  LIBLTE_S1AP_PROTOCOLEXTENSIONCONTAINER_STRUCT                iE_Extensions;
  bool                                                         iE_Extensions_present;
}LIBLTE_S1AP_CANCELLEDCELLINEAI_ITEM_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_cancelledcellineai_item(
  LIBLTE_S1AP_CANCELLEDCELLINEAI_ITEM_STRUCT                   *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_cancelledcellineai_item(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_CANCELLEDCELLINEAI_ITEM_STRUCT                   *ie);

/*******************************************************************************
/* ProtocolIE CancelledCellinTAI_Item SEQUENCE
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_EUTRAN_CGI_STRUCT                                eCGI;
  LIBLTE_S1AP_NUMBEROFBROADCASTS_STRUCT                        numberOfBroadcasts;
  LIBLTE_S1AP_PROTOCOLEXTENSIONCONTAINER_STRUCT                iE_Extensions;
  bool                                                         iE_Extensions_present;
}LIBLTE_S1AP_CANCELLEDCELLINTAI_ITEM_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_cancelledcellintai_item(
  LIBLTE_S1AP_CANCELLEDCELLINTAI_ITEM_STRUCT                   *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_cancelledcellintai_item(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_CANCELLEDCELLINTAI_ITEM_STRUCT                   *ie);

/*******************************************************************************
/* ProtocolIE CellID_Broadcast_Item SEQUENCE
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_EUTRAN_CGI_STRUCT                                eCGI;
  LIBLTE_S1AP_PROTOCOLEXTENSIONCONTAINER_STRUCT                iE_Extensions;
  bool                                                         iE_Extensions_present;
}LIBLTE_S1AP_CELLID_BROADCAST_ITEM_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_cellid_broadcast_item(
  LIBLTE_S1AP_CELLID_BROADCAST_ITEM_STRUCT                     *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_cellid_broadcast_item(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_CELLID_BROADCAST_ITEM_STRUCT                     *ie);

/*******************************************************************************
/* ProtocolIE CellID_Cancelled_Item SEQUENCE
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_EUTRAN_CGI_STRUCT                                eCGI;
  LIBLTE_S1AP_NUMBEROFBROADCASTS_STRUCT                        numberOfBroadcasts;
  LIBLTE_S1AP_PROTOCOLEXTENSIONCONTAINER_STRUCT                iE_Extensions;
  bool                                                         iE_Extensions_present;
}LIBLTE_S1AP_CELLID_CANCELLED_ITEM_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_cellid_cancelled_item(
  LIBLTE_S1AP_CELLID_CANCELLED_ITEM_STRUCT                     *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_cellid_cancelled_item(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_CELLID_CANCELLED_ITEM_STRUCT                     *ie);

/*******************************************************************************
/* ProtocolIE CellIdListforMDT DYNAMIC SEQUENCE OF
********************************************************************************/
// lb:1, ub:32
typedef struct{
  uint32_t                                                     len;
  LIBLTE_S1AP_EUTRAN_CGI_STRUCT                                buffer[32]; //WARNING: Artificial limit to reduce memory footprint
}LIBLTE_S1AP_CELLIDLISTFORMDT_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_cellidlistformdt(
  LIBLTE_S1AP_CELLIDLISTFORMDT_STRUCT                          *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_cellidlistformdt(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_CELLIDLISTFORMDT_STRUCT                          *ie);

/*******************************************************************************
/* ProtocolIE CSG_Id STATIC BIT STRING
********************************************************************************/
#define LIBLTE_S1AP_CSG_ID_BIT_STRING_LEN 27
typedef struct{
  uint8_t  buffer[27];
}LIBLTE_S1AP_CSG_ID_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_csg_id(
  LIBLTE_S1AP_CSG_ID_STRUCT                                    *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_csg_id(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_CSG_ID_STRUCT                                    *ie);

/*******************************************************************************
/* ProtocolIE CSG_IdList_Item SEQUENCE
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_CSG_ID_STRUCT                                    cSG_Id;
  LIBLTE_S1AP_PROTOCOLEXTENSIONCONTAINER_STRUCT                iE_Extensions;
  bool                                                         iE_Extensions_present;
}LIBLTE_S1AP_CSG_IDLIST_ITEM_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_csg_idlist_item(
  LIBLTE_S1AP_CSG_IDLIST_ITEM_STRUCT                           *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_csg_idlist_item(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_CSG_IDLIST_ITEM_STRUCT                           *ie);

/*******************************************************************************
/* ProtocolIE Direct_Forwarding_Path_Availability ENUMERATED
********************************************************************************/
typedef enum{
  LIBLTE_S1AP_DIRECT_FORWARDING_PATH_AVAILABILITY_DIRECTPATHAVAILABLE,
  LIBLTE_S1AP_DIRECT_FORWARDING_PATH_AVAILABILITY_N_ITEMS,
}LIBLTE_S1AP_DIRECT_FORWARDING_PATH_AVAILABILITY_ENUM;
static const char liblte_s1ap_direct_forwarding_path_availability_text[LIBLTE_S1AP_DIRECT_FORWARDING_PATH_AVAILABILITY_N_ITEMS][80] = {
  "directPathAvailable",
};

typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_DIRECT_FORWARDING_PATH_AVAILABILITY_ENUM e;
}LIBLTE_S1AP_DIRECT_FORWARDING_PATH_AVAILABILITY_ENUM_EXT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_direct_forwarding_path_availability(
  LIBLTE_S1AP_DIRECT_FORWARDING_PATH_AVAILABILITY_ENUM_EXT     *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_direct_forwarding_path_availability(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_DIRECT_FORWARDING_PATH_AVAILABILITY_ENUM_EXT     *ie);

/*******************************************************************************
/* ProtocolIE CompletedCellinEAI_Item SEQUENCE
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_EUTRAN_CGI_STRUCT                                eCGI;
  LIBLTE_S1AP_PROTOCOLEXTENSIONCONTAINER_STRUCT                iE_Extensions;
  bool                                                         iE_Extensions_present;
}LIBLTE_S1AP_COMPLETEDCELLINEAI_ITEM_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_completedcellineai_item(
  LIBLTE_S1AP_COMPLETEDCELLINEAI_ITEM_STRUCT                   *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_completedcellineai_item(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_COMPLETEDCELLINEAI_ITEM_STRUCT                   *ie);

/*******************************************************************************
/* Protocol Container List E_RABInformationList DYNAMIC SEQUENCE OF
********************************************************************************/
// lb:1, ub:256
typedef struct{
  uint32_t                                                     len;
  LIBLTE_S1AP_E_RABINFORMATIONLISTITEM_STRUCT                  buffer[32]; //WARNING: Artificial limit to reduce memory footprint
}LIBLTE_S1AP_E_RABINFORMATIONLIST_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_e_rabinformationlist(
  LIBLTE_S1AP_E_RABINFORMATIONLIST_STRUCT                      *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_e_rabinformationlist(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_E_RABINFORMATIONLIST_STRUCT                      *ie);

/*******************************************************************************
/* ProtocolIE ForbiddenTAs DYNAMIC SEQUENCE OF
********************************************************************************/
// lb:1, ub:16
typedef struct{
  uint32_t                                                     len;
  LIBLTE_S1AP_FORBIDDENTAS_ITEM_STRUCT                         buffer[32]; //WARNING: Artificial limit to reduce memory footprint
}LIBLTE_S1AP_FORBIDDENTAS_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_forbiddentas(
  LIBLTE_S1AP_FORBIDDENTAS_STRUCT                              *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_forbiddentas(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_FORBIDDENTAS_STRUCT                              *ie);

/*******************************************************************************
/* ProtocolIE GUMMEI SEQUENCE
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_TBCD_STRING_STRUCT                               pLMN_Identity;
  LIBLTE_S1AP_MME_GROUP_ID_STRUCT                              mME_Group_ID;
  LIBLTE_S1AP_MME_CODE_STRUCT                                  mME_Code;
  LIBLTE_S1AP_PROTOCOLEXTENSIONCONTAINER_STRUCT                iE_Extensions;
  bool                                                         iE_Extensions_present;
}LIBLTE_S1AP_GUMMEI_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_gummei(
  LIBLTE_S1AP_GUMMEI_STRUCT                                    *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_gummei(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_GUMMEI_STRUCT                                    *ie);

/*******************************************************************************
/* ProtocolIE LoggedMDT SEQUENCE
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_LOGGINGINTERVAL_ENUM                             loggingInterval;
  LIBLTE_S1AP_LOGGINGDURATION_ENUM                             loggingDuration;
  LIBLTE_S1AP_PROTOCOLEXTENSIONCONTAINER_STRUCT                iE_Extensions;
  bool                                                         iE_Extensions_present;
}LIBLTE_S1AP_LOGGEDMDT_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_loggedmdt(
  LIBLTE_S1AP_LOGGEDMDT_STRUCT                                 *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_loggedmdt(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_LOGGEDMDT_STRUCT                                 *ie);

/*******************************************************************************
/* ProtocolIE NASSecurityParametersfromE_UTRAN DYNAMIC OCTET STRING
********************************************************************************/
// lb:0, ub:16318
typedef struct{
  uint32_t n_octets;
  uint8_t  buffer[16318];
}LIBLTE_S1AP_NASSECURITYPARAMETERSFROME_UTRAN_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_nassecurityparametersfrome_utran(
  LIBLTE_S1AP_NASSECURITYPARAMETERSFROME_UTRAN_STRUCT          *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_nassecurityparametersfrome_utran(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_NASSECURITYPARAMETERSFROME_UTRAN_STRUCT          *ie);

/*******************************************************************************
/* ProtocolIE ReportAmountMDT ENUMERATED
********************************************************************************/
typedef enum{
  LIBLTE_S1AP_REPORTAMOUNTMDT_R1,
  LIBLTE_S1AP_REPORTAMOUNTMDT_R2,
  LIBLTE_S1AP_REPORTAMOUNTMDT_R4,
  LIBLTE_S1AP_REPORTAMOUNTMDT_R8,
  LIBLTE_S1AP_REPORTAMOUNTMDT_R16,
  LIBLTE_S1AP_REPORTAMOUNTMDT_R32,
  LIBLTE_S1AP_REPORTAMOUNTMDT_R64,
  LIBLTE_S1AP_REPORTAMOUNTMDT_RINFINITY,
  LIBLTE_S1AP_REPORTAMOUNTMDT_N_ITEMS,
}LIBLTE_S1AP_REPORTAMOUNTMDT_ENUM;
static const char liblte_s1ap_reportamountmdt_text[LIBLTE_S1AP_REPORTAMOUNTMDT_N_ITEMS][80] = {
  "r1",
  "r2",
  "r4",
  "r8",
  "r16",
  "r32",
  "r64",
  "rinfinity",
};

LIBLTE_ERROR_ENUM liblte_s1ap_pack_reportamountmdt(
  LIBLTE_S1AP_REPORTAMOUNTMDT_ENUM                             *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_reportamountmdt(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_REPORTAMOUNTMDT_ENUM                             *ie);

/*******************************************************************************
/* ProtocolIE ServedGUMMEIsItem SEQUENCE
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_SERVEDPLMNS_STRUCT                               servedPLMNs;
  LIBLTE_S1AP_SERVEDGROUPIDS_STRUCT                            servedGroupIDs;
  LIBLTE_S1AP_SERVEDMMECS_STRUCT                               servedMMECs;
  LIBLTE_S1AP_PROTOCOLEXTENSIONCONTAINER_STRUCT                iE_Extensions;
  bool                                                         iE_Extensions_present;
}LIBLTE_S1AP_SERVEDGUMMEISITEM_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_servedgummeisitem(
  LIBLTE_S1AP_SERVEDGUMMEISITEM_STRUCT                         *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_servedgummeisitem(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_SERVEDGUMMEISITEM_STRUCT                         *ie);

/*******************************************************************************
/* ProtocolIE S_TMSI SEQUENCE
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_MME_CODE_STRUCT                                  mMEC;
  LIBLTE_S1AP_M_TMSI_STRUCT                                    m_TMSI;
  LIBLTE_S1AP_PROTOCOLEXTENSIONCONTAINER_STRUCT                iE_Extensions;
  bool                                                         iE_Extensions_present;
}LIBLTE_S1AP_S_TMSI_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_s_tmsi(
  LIBLTE_S1AP_S_TMSI_STRUCT                                    *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_s_tmsi(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_S_TMSI_STRUCT                                    *ie);

/*******************************************************************************
/* ProtocolIE TAIListforWarning DYNAMIC SEQUENCE OF
********************************************************************************/
// lb:1, ub:65535
typedef struct{
  uint32_t                                                     len;
  LIBLTE_S1AP_TAI_STRUCT                                       buffer[32]; //WARNING: Artificial limit to reduce memory footprint
}LIBLTE_S1AP_TAILISTFORWARNING_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_tailistforwarning(
  LIBLTE_S1AP_TAILISTFORWARNING_STRUCT                         *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_tailistforwarning(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_TAILISTFORWARNING_STRUCT                         *ie);

/*******************************************************************************
/* ProtocolIE CompletedCellinTAI DYNAMIC SEQUENCE OF
********************************************************************************/
// lb:1, ub:65535
typedef struct{
  uint32_t                                                     len;
  LIBLTE_S1AP_COMPLETEDCELLINTAI_ITEM_STRUCT                   buffer[32]; //WARNING: Artificial limit to reduce memory footprint
}LIBLTE_S1AP_COMPLETEDCELLINTAI_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_completedcellintai(
  LIBLTE_S1AP_COMPLETEDCELLINTAI_STRUCT                        *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_completedcellintai(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_COMPLETEDCELLINTAI_STRUCT                        *ie);

/*******************************************************************************
/* ProtocolIE TargetRNC_ID SEQUENCE
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_LAI_STRUCT                                       lAI;
  LIBLTE_S1AP_RAC_STRUCT                                       rAC;
  bool                                                         rAC_present;
  LIBLTE_S1AP_RNC_ID_STRUCT                                    rNC_ID;
  LIBLTE_S1AP_EXTENDEDRNC_ID_STRUCT                            extendedRNC_ID;
  bool                                                         extendedRNC_ID_present;
  LIBLTE_S1AP_PROTOCOLEXTENSIONCONTAINER_STRUCT                iE_Extensions;
  bool                                                         iE_Extensions_present;
}LIBLTE_S1AP_TARGETRNC_ID_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_targetrnc_id(
  LIBLTE_S1AP_TARGETRNC_ID_STRUCT                              *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_targetrnc_id(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_TARGETRNC_ID_STRUCT                              *ie);

/*******************************************************************************
/* ProtocolIE UE_associatedLogicalS1_ConnectionItem SEQUENCE
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_MME_UE_S1AP_ID_STRUCT                            mME_UE_S1AP_ID;
  bool                                                         mME_UE_S1AP_ID_present;
  LIBLTE_S1AP_ENB_UE_S1AP_ID_STRUCT                            eNB_UE_S1AP_ID;
  bool                                                         eNB_UE_S1AP_ID_present;
  LIBLTE_S1AP_PROTOCOLEXTENSIONCONTAINER_STRUCT                iE_Extensions;
  bool                                                         iE_Extensions_present;
}LIBLTE_S1AP_UE_ASSOCIATEDLOGICALS1_CONNECTIONITEM_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_ue_associatedlogicals1_connectionitem(
  LIBLTE_S1AP_UE_ASSOCIATEDLOGICALS1_CONNECTIONITEM_STRUCT     *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_ue_associatedlogicals1_connectionitem(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_UE_ASSOCIATEDLOGICALS1_CONNECTIONITEM_STRUCT     *ie);

/*******************************************************************************
/* ProtocolIE UEPagingID CHOICE
********************************************************************************/
typedef enum{
  LIBLTE_S1AP_UEPAGINGID_CHOICE_S_TMSI,
  LIBLTE_S1AP_UEPAGINGID_CHOICE_IMSI,
  LIBLTE_S1AP_UEPAGINGID_CHOICE_N_ITEMS,
}LIBLTE_S1AP_UEPAGINGID_CHOICE_ENUM;
static const char liblte_s1ap_uepagingid_choice_text[LIBLTE_S1AP_UEPAGINGID_CHOICE_N_ITEMS][50] = {
  "s_TMSI",
  "iMSI",
};

typedef union{
  LIBLTE_S1AP_S_TMSI_STRUCT                                    s_TMSI;
  LIBLTE_S1AP_IMSI_STRUCT                                      iMSI;
}LIBLTE_S1AP_UEPAGINGID_CHOICE_UNION;

typedef struct{
  bool                            ext;
  LIBLTE_S1AP_UEPAGINGID_CHOICE_UNION choice;
  LIBLTE_S1AP_UEPAGINGID_CHOICE_ENUM  choice_type;
}LIBLTE_S1AP_UEPAGINGID_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_uepagingid(
  LIBLTE_S1AP_UEPAGINGID_STRUCT                                *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_uepagingid(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_UEPAGINGID_STRUCT                                *ie);

/*******************************************************************************
/* ProtocolIE Bearers_SubjectToStatusTransfer_Item SEQUENCE
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_E_RAB_ID_STRUCT                                  e_RAB_ID;
  LIBLTE_S1AP_COUNTVALUE_STRUCT                                uL_COUNTvalue;
  LIBLTE_S1AP_COUNTVALUE_STRUCT                                dL_COUNTvalue;
  LIBLTE_S1AP_RECEIVESTATUSOFULPDCPSDUS_STRUCT                 receiveStatusofULPDCPSDUs;
  bool                                                         receiveStatusofULPDCPSDUs_present;
  LIBLTE_S1AP_PROTOCOLEXTENSIONCONTAINER_STRUCT                iE_Extensions;
  bool                                                         iE_Extensions_present;
}LIBLTE_S1AP_BEARERS_SUBJECTTOSTATUSTRANSFER_ITEM_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_bearers_subjecttostatustransfer_item(
  LIBLTE_S1AP_BEARERS_SUBJECTTOSTATUSTRANSFER_ITEM_STRUCT      *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_bearers_subjecttostatustransfer_item(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_BEARERS_SUBJECTTOSTATUSTRANSFER_ITEM_STRUCT      *ie);

/*******************************************************************************
/* ProtocolIE CancelledCellinEAI DYNAMIC SEQUENCE OF
********************************************************************************/
// lb:1, ub:65535
typedef struct{
  uint32_t                                                     len;
  LIBLTE_S1AP_CANCELLEDCELLINEAI_ITEM_STRUCT                   buffer[32]; //WARNING: Artificial limit to reduce memory footprint
}LIBLTE_S1AP_CANCELLEDCELLINEAI_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_cancelledcellineai(
  LIBLTE_S1AP_CANCELLEDCELLINEAI_STRUCT                        *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_cancelledcellineai(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_CANCELLEDCELLINEAI_STRUCT                        *ie);

/*******************************************************************************
/* ProtocolIE CellID_Broadcast DYNAMIC SEQUENCE OF
********************************************************************************/
// lb:1, ub:65535
typedef struct{
  uint32_t                                                     len;
  LIBLTE_S1AP_CELLID_BROADCAST_ITEM_STRUCT                     buffer[32]; //WARNING: Artificial limit to reduce memory footprint
}LIBLTE_S1AP_CELLID_BROADCAST_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_cellid_broadcast(
  LIBLTE_S1AP_CELLID_BROADCAST_STRUCT                          *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_cellid_broadcast(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_CELLID_BROADCAST_STRUCT                          *ie);

/*******************************************************************************
/* ProtocolIE CellBasedMDT SEQUENCE
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_CELLIDLISTFORMDT_STRUCT                          cellIdListforMDT;
  LIBLTE_S1AP_PROTOCOLEXTENSIONCONTAINER_STRUCT                iE_Extensions;
  bool                                                         iE_Extensions_present;
}LIBLTE_S1AP_CELLBASEDMDT_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_cellbasedmdt(
  LIBLTE_S1AP_CELLBASEDMDT_STRUCT                              *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_cellbasedmdt(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_CELLBASEDMDT_STRUCT                              *ie);

/*******************************************************************************
/* ProtocolIE CSG_IdList DYNAMIC SEQUENCE OF
********************************************************************************/
// lb:1, ub:256
typedef struct{
  uint32_t                                                     len;
  LIBLTE_S1AP_CSG_IDLIST_ITEM_STRUCT                           buffer[32]; //WARNING: Artificial limit to reduce memory footprint
}LIBLTE_S1AP_CSG_IDLIST_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_csg_idlist(
  LIBLTE_S1AP_CSG_IDLIST_STRUCT                                *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_csg_idlist(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_CSG_IDLIST_STRUCT                                *ie);

/*******************************************************************************
/* ProtocolIE ECGIList DYNAMIC SEQUENCE OF
********************************************************************************/
// lb:1, ub:65535
typedef struct{
  uint32_t                                                     len;
  LIBLTE_S1AP_EUTRAN_CGI_STRUCT                                buffer[32]; //WARNING: Artificial limit to reduce memory footprint
}LIBLTE_S1AP_ECGILIST_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_ecgilist(
  LIBLTE_S1AP_ECGILIST_STRUCT                                  *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_ecgilist(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_ECGILIST_STRUCT                                  *ie);

/*******************************************************************************
/* ProtocolIE EmergencyAreaID_Cancelled_Item SEQUENCE
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_EMERGENCYAREAID_STRUCT                           emergencyAreaID;
  LIBLTE_S1AP_CANCELLEDCELLINEAI_STRUCT                        cancelledCellinEAI;
  LIBLTE_S1AP_PROTOCOLEXTENSIONCONTAINER_STRUCT                iE_Extensions;
  bool                                                         iE_Extensions_present;
}LIBLTE_S1AP_EMERGENCYAREAID_CANCELLED_ITEM_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_emergencyareaid_cancelled_item(
  LIBLTE_S1AP_EMERGENCYAREAID_CANCELLED_ITEM_STRUCT            *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_emergencyareaid_cancelled_item(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_EMERGENCYAREAID_CANCELLED_ITEM_STRUCT            *ie);

/*******************************************************************************
/* ProtocolIE GERAN_Cell_ID SEQUENCE
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_LAI_STRUCT                                       lAI;
  LIBLTE_S1AP_RAC_STRUCT                                       rAC;
  LIBLTE_S1AP_CI_STRUCT                                        cI;
  LIBLTE_S1AP_PROTOCOLEXTENSIONCONTAINER_STRUCT                iE_Extensions;
  bool                                                         iE_Extensions_present;
}LIBLTE_S1AP_GERAN_CELL_ID_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_geran_cell_id(
  LIBLTE_S1AP_GERAN_CELL_ID_STRUCT                             *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_geran_cell_id(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_GERAN_CELL_ID_STRUCT                             *ie);

/*******************************************************************************
/* Protocol Container List E_RABList DYNAMIC SEQUENCE OF
********************************************************************************/
// lb:1, ub:256
typedef struct{
  uint32_t                                                     len;
  LIBLTE_S1AP_E_RABITEM_STRUCT                                 buffer[32]; //WARNING: Artificial limit to reduce memory footprint
}LIBLTE_S1AP_E_RABLIST_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_e_rablist(
  LIBLTE_S1AP_E_RABLIST_STRUCT                                 *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_e_rablist(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_E_RABLIST_STRUCT                                 *ie);

/*******************************************************************************
/* ProtocolIE ForbiddenLAs DYNAMIC SEQUENCE OF
********************************************************************************/
// lb:1, ub:16
typedef struct{
  uint32_t                                                     len;
  LIBLTE_S1AP_FORBIDDENLAS_ITEM_STRUCT                         buffer[32]; //WARNING: Artificial limit to reduce memory footprint
}LIBLTE_S1AP_FORBIDDENLAS_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_forbiddenlas(
  LIBLTE_S1AP_FORBIDDENLAS_STRUCT                              *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_forbiddenlas(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_FORBIDDENLAS_STRUCT                              *ie);

/*******************************************************************************
/* ProtocolIE MDT_Location_Info STATIC BIT STRING
********************************************************************************/
#define LIBLTE_S1AP_MDT_LOCATION_INFO_BIT_STRING_LEN 8
typedef struct{
  uint8_t  buffer[8];
}LIBLTE_S1AP_MDT_LOCATION_INFO_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_mdt_location_info(
  LIBLTE_S1AP_MDT_LOCATION_INFO_STRUCT                         *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_mdt_location_info(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MDT_LOCATION_INFO_STRUCT                         *ie);

/*******************************************************************************
/* ProtocolIE M1PeriodicReporting SEQUENCE
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_REPORTINTERVALMDT_ENUM                           reportInterval;
  LIBLTE_S1AP_REPORTAMOUNTMDT_ENUM                             reportAmount;
  LIBLTE_S1AP_PROTOCOLEXTENSIONCONTAINER_STRUCT                iE_Extensions;
  bool                                                         iE_Extensions_present;
}LIBLTE_S1AP_M1PERIODICREPORTING_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_m1periodicreporting(
  LIBLTE_S1AP_M1PERIODICREPORTING_STRUCT                       *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_m1periodicreporting(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_M1PERIODICREPORTING_STRUCT                       *ie);

/*******************************************************************************
/* ProtocolIE eHRPD_Sector_ID DYNAMIC OCTET STRING
********************************************************************************/
// lb:0, ub:16318
typedef struct{
  uint32_t n_octets;
  uint8_t  buffer[16318];
}LIBLTE_S1AP_EHRPD_SECTOR_ID_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_ehrpd_sector_id(
  LIBLTE_S1AP_EHRPD_SECTOR_ID_STRUCT                           *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_ehrpd_sector_id(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_EHRPD_SECTOR_ID_STRUCT                           *ie);

/*******************************************************************************
/* ProtocolIE RIMRoutingAddress CHOICE
********************************************************************************/
typedef enum{
  LIBLTE_S1AP_RIMROUTINGADDRESS_CHOICE_GERAN_CELL_ID,
  LIBLTE_S1AP_RIMROUTINGADDRESS_CHOICE_TARGETRNC_ID,
  LIBLTE_S1AP_RIMROUTINGADDRESS_CHOICE_EHRPD_SECTOR_ID,
  LIBLTE_S1AP_RIMROUTINGADDRESS_CHOICE_N_ITEMS,
}LIBLTE_S1AP_RIMROUTINGADDRESS_CHOICE_ENUM;
static const char liblte_s1ap_rimroutingaddress_choice_text[LIBLTE_S1AP_RIMROUTINGADDRESS_CHOICE_N_ITEMS][50] = {
  "gERAN_Cell_ID",
  "targetRNC_ID",
  "eHRPD_Sector_ID",
};

typedef union{
  LIBLTE_S1AP_GERAN_CELL_ID_STRUCT                             gERAN_Cell_ID;
  LIBLTE_S1AP_TARGETRNC_ID_STRUCT                              targetRNC_ID;
  LIBLTE_S1AP_EHRPD_SECTOR_ID_STRUCT                           eHRPD_Sector_ID;
}LIBLTE_S1AP_RIMROUTINGADDRESS_CHOICE_UNION;

typedef struct{
  bool                            ext;
  LIBLTE_S1AP_RIMROUTINGADDRESS_CHOICE_UNION choice;
  LIBLTE_S1AP_RIMROUTINGADDRESS_CHOICE_ENUM  choice_type;
}LIBLTE_S1AP_RIMROUTINGADDRESS_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_rimroutingaddress(
  LIBLTE_S1AP_RIMROUTINGADDRESS_STRUCT                         *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_rimroutingaddress(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_RIMROUTINGADDRESS_STRUCT                         *ie);

/*******************************************************************************
/* ProtocolIE ServedGUMMEIs DYNAMIC SEQUENCE OF
********************************************************************************/
// lb:1, ub:8
typedef struct{
  uint32_t                                                     len;
  LIBLTE_S1AP_SERVEDGUMMEISITEM_STRUCT                         buffer[32]; //WARNING: Artificial limit to reduce memory footprint
}LIBLTE_S1AP_SERVEDGUMMEIS_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_servedgummeis(
  LIBLTE_S1AP_SERVEDGUMMEIS_STRUCT                             *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_servedgummeis(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_SERVEDGUMMEIS_STRUCT                             *ie);

/*******************************************************************************
/* ProtocolIE TAIBasedMDT SEQUENCE
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_TAILISTFORMDT_STRUCT                             tAIListforMDT;
  LIBLTE_S1AP_PROTOCOLEXTENSIONCONTAINER_STRUCT                iE_Extensions;
  bool                                                         iE_Extensions_present;
}LIBLTE_S1AP_TAIBASEDMDT_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_taibasedmdt(
  LIBLTE_S1AP_TAIBASEDMDT_STRUCT                               *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_taibasedmdt(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_TAIBASEDMDT_STRUCT                               *ie);

/*******************************************************************************
/* ProtocolIE TAI_Broadcast_Item SEQUENCE
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_TAI_STRUCT                                       tAI;
  LIBLTE_S1AP_COMPLETEDCELLINTAI_STRUCT                        completedCellinTAI;
  LIBLTE_S1AP_PROTOCOLEXTENSIONCONTAINER_STRUCT                iE_Extensions;
  bool                                                         iE_Extensions_present;
}LIBLTE_S1AP_TAI_BROADCAST_ITEM_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_tai_broadcast_item(
  LIBLTE_S1AP_TAI_BROADCAST_ITEM_STRUCT                        *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_tai_broadcast_item(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_TAI_BROADCAST_ITEM_STRUCT                        *ie);

/*******************************************************************************
/* ProtocolIE TargetID CHOICE
********************************************************************************/
typedef enum{
  LIBLTE_S1AP_TARGETID_CHOICE_TARGETENB_ID,
  LIBLTE_S1AP_TARGETID_CHOICE_TARGETRNC_ID,
  LIBLTE_S1AP_TARGETID_CHOICE_CGI,
  LIBLTE_S1AP_TARGETID_CHOICE_N_ITEMS,
}LIBLTE_S1AP_TARGETID_CHOICE_ENUM;
static const char liblte_s1ap_targetid_choice_text[LIBLTE_S1AP_TARGETID_CHOICE_N_ITEMS][50] = {
  "targeteNB_ID",
  "targetRNC_ID",
  "cGI",
};

typedef union{
  LIBLTE_S1AP_TARGETENB_ID_STRUCT                              targeteNB_ID;
  LIBLTE_S1AP_TARGETRNC_ID_STRUCT                              targetRNC_ID;
  LIBLTE_S1AP_CGI_STRUCT                                       cGI;
}LIBLTE_S1AP_TARGETID_CHOICE_UNION;

typedef struct{
  bool                            ext;
  LIBLTE_S1AP_TARGETID_CHOICE_UNION choice;
  LIBLTE_S1AP_TARGETID_CHOICE_ENUM  choice_type;
}LIBLTE_S1AP_TARGETID_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_targetid(
  LIBLTE_S1AP_TARGETID_STRUCT                                  *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_targetid(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_TARGETID_STRUCT                                  *ie);

/*******************************************************************************
/* ProtocolIE WarningAreaList CHOICE
********************************************************************************/
typedef enum{
  LIBLTE_S1AP_WARNINGAREALIST_CHOICE_CELLIDLIST,
  LIBLTE_S1AP_WARNINGAREALIST_CHOICE_TRACKINGAREALISTFORWARNING,
  LIBLTE_S1AP_WARNINGAREALIST_CHOICE_EMERGENCYAREAIDLIST,
  LIBLTE_S1AP_WARNINGAREALIST_CHOICE_N_ITEMS,
}LIBLTE_S1AP_WARNINGAREALIST_CHOICE_ENUM;
static const char liblte_s1ap_warningarealist_choice_text[LIBLTE_S1AP_WARNINGAREALIST_CHOICE_N_ITEMS][50] = {
  "cellIDList",
  "trackingAreaListforWarning",
  "emergencyAreaIDList",
};

typedef union{
  LIBLTE_S1AP_ECGILIST_STRUCT                                  cellIDList;
  LIBLTE_S1AP_TAILISTFORWARNING_STRUCT                         trackingAreaListforWarning;
  LIBLTE_S1AP_EMERGENCYAREAIDLIST_STRUCT                       emergencyAreaIDList;
}LIBLTE_S1AP_WARNINGAREALIST_CHOICE_UNION;

typedef struct{
  bool                            ext;
  LIBLTE_S1AP_WARNINGAREALIST_CHOICE_UNION choice;
  LIBLTE_S1AP_WARNINGAREALIST_CHOICE_ENUM  choice_type;
}LIBLTE_S1AP_WARNINGAREALIST_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_warningarealist(
  LIBLTE_S1AP_WARNINGAREALIST_STRUCT                           *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_warningarealist(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_WARNINGAREALIST_STRUCT                           *ie);

//TODO: Type pLMNWide NULL

/*******************************************************************************
/* ProtocolIE AreaScopeOfMDT CHOICE
********************************************************************************/
typedef enum{
  LIBLTE_S1AP_AREASCOPEOFMDT_CHOICE_CELLBASED,
  LIBLTE_S1AP_AREASCOPEOFMDT_CHOICE_TABASED,
  LIBLTE_S1AP_AREASCOPEOFMDT_CHOICE_PLMNWIDE,
  LIBLTE_S1AP_AREASCOPEOFMDT_CHOICE_TAIBASED,
  LIBLTE_S1AP_AREASCOPEOFMDT_CHOICE_N_ITEMS,
}LIBLTE_S1AP_AREASCOPEOFMDT_CHOICE_ENUM;
static const char liblte_s1ap_areascopeofmdt_choice_text[LIBLTE_S1AP_AREASCOPEOFMDT_CHOICE_N_ITEMS][50] = {
  "cellBased",
  "tABased",
  "pLMNWide",
  "tAIBased",
};

typedef union{
  LIBLTE_S1AP_CELLBASEDMDT_STRUCT                              cellBased;
  LIBLTE_S1AP_TABASEDMDT_STRUCT                                tABased;
  //TODO: NULL                                                 pLMNWide;
  LIBLTE_S1AP_TAIBASEDMDT_STRUCT                               tAIBased;
}LIBLTE_S1AP_AREASCOPEOFMDT_CHOICE_UNION;

typedef struct{
  bool                            ext;
  LIBLTE_S1AP_AREASCOPEOFMDT_CHOICE_UNION choice;
  LIBLTE_S1AP_AREASCOPEOFMDT_CHOICE_ENUM  choice_type;
}LIBLTE_S1AP_AREASCOPEOFMDT_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_areascopeofmdt(
  LIBLTE_S1AP_AREASCOPEOFMDT_STRUCT                            *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_areascopeofmdt(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_AREASCOPEOFMDT_STRUCT                            *ie);

/*******************************************************************************
/* ProtocolIE CancelledCellinTAI DYNAMIC SEQUENCE OF
********************************************************************************/
// lb:1, ub:65535
typedef struct{
  uint32_t                                                     len;
  LIBLTE_S1AP_CANCELLEDCELLINTAI_ITEM_STRUCT                   buffer[32]; //WARNING: Artificial limit to reduce memory footprint
}LIBLTE_S1AP_CANCELLEDCELLINTAI_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_cancelledcellintai(
  LIBLTE_S1AP_CANCELLEDCELLINTAI_STRUCT                        *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_cancelledcellintai(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_CANCELLEDCELLINTAI_STRUCT                        *ie);

/*******************************************************************************
/* ProtocolIE CellType SEQUENCE
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_CELL_SIZE_ENUM_EXT                               cell_Size;
  LIBLTE_S1AP_PROTOCOLEXTENSIONCONTAINER_STRUCT                iE_Extensions;
  bool                                                         iE_Extensions_present;
}LIBLTE_S1AP_CELLTYPE_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_celltype(
  LIBLTE_S1AP_CELLTYPE_STRUCT                                  *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_celltype(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_CELLTYPE_STRUCT                                  *ie);

/*******************************************************************************
/* ProtocolIE EmergencyAreaID_Cancelled DYNAMIC SEQUENCE OF
********************************************************************************/
// lb:1, ub:65535
typedef struct{
  uint32_t                                                     len;
  LIBLTE_S1AP_EMERGENCYAREAID_CANCELLED_ITEM_STRUCT            buffer[32]; //WARNING: Artificial limit to reduce memory footprint
}LIBLTE_S1AP_EMERGENCYAREAID_CANCELLED_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_emergencyareaid_cancelled(
  LIBLTE_S1AP_EMERGENCYAREAID_CANCELLED_STRUCT                 *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_emergencyareaid_cancelled(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_EMERGENCYAREAID_CANCELLED_STRUCT                 *ie);

/*******************************************************************************
/* ProtocolIE GUMMEIList DYNAMIC SEQUENCE OF
********************************************************************************/
// lb:1, ub:256
typedef struct{
  uint32_t                                                     len;
  LIBLTE_S1AP_GUMMEI_STRUCT                                    buffer[32]; //WARNING: Artificial limit to reduce memory footprint
}LIBLTE_S1AP_GUMMEILIST_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_gummeilist(
  LIBLTE_S1AP_GUMMEILIST_STRUCT                                *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_gummeilist(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_GUMMEILIST_STRUCT                                *ie);

/*******************************************************************************
/* ProtocolIE E_RABLevelQoSParameters SEQUENCE
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_QCI_STRUCT                                       qCI;
  LIBLTE_S1AP_ALLOCATIONANDRETENTIONPRIORITY_STRUCT            allocationRetentionPriority;
  LIBLTE_S1AP_GBR_QOSINFORMATION_STRUCT                        gbrQosInformation;
  bool                                                         gbrQosInformation_present;
  LIBLTE_S1AP_PROTOCOLEXTENSIONCONTAINER_STRUCT                iE_Extensions;
  bool                                                         iE_Extensions_present;
}LIBLTE_S1AP_E_RABLEVELQOSPARAMETERS_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_e_rablevelqosparameters(
  LIBLTE_S1AP_E_RABLEVELQOSPARAMETERS_STRUCT                   *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_e_rablevelqosparameters(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_E_RABLEVELQOSPARAMETERS_STRUCT                   *ie);

/*******************************************************************************
/* ProtocolIE LastVisitedEUTRANCellInformation SEQUENCE
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_EUTRAN_CGI_STRUCT                                global_Cell_ID;
  LIBLTE_S1AP_CELLTYPE_STRUCT                                  cellType;
  LIBLTE_S1AP_TIME_UE_STAYEDINCELL_STRUCT                      time_UE_StayedInCell;
  LIBLTE_S1AP_PROTOCOLEXTENSIONCONTAINER_STRUCT                iE_Extensions;
  bool                                                         iE_Extensions_present;
}LIBLTE_S1AP_LASTVISITEDEUTRANCELLINFORMATION_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_lastvisitedeutrancellinformation(
  LIBLTE_S1AP_LASTVISITEDEUTRANCELLINFORMATION_STRUCT          *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_lastvisitedeutrancellinformation(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_LASTVISITEDEUTRANCELLINFORMATION_STRUCT          *ie);

/*******************************************************************************
/* ProtocolIE RIMTransfer SEQUENCE
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_RIMINFORMATION_STRUCT                            rIMInformation;
  LIBLTE_S1AP_RIMROUTINGADDRESS_STRUCT                         rIMRoutingAddress;
  bool                                                         rIMRoutingAddress_present;
  LIBLTE_S1AP_PROTOCOLEXTENSIONCONTAINER_STRUCT                iE_Extensions;
  bool                                                         iE_Extensions_present;
}LIBLTE_S1AP_RIMTRANSFER_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_rimtransfer(
  LIBLTE_S1AP_RIMTRANSFER_STRUCT                               *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_rimtransfer(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_RIMTRANSFER_STRUCT                               *ie);

/*******************************************************************************
/* ProtocolIE SupportedTAs DYNAMIC SEQUENCE OF
********************************************************************************/
// lb:1, ub:256
typedef struct{
  uint32_t                                                     len;
  LIBLTE_S1AP_SUPPORTEDTAS_ITEM_STRUCT                         buffer[32]; //WARNING: Artificial limit to reduce memory footprint
}LIBLTE_S1AP_SUPPORTEDTAS_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_supportedtas(
  LIBLTE_S1AP_SUPPORTEDTAS_STRUCT                              *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_supportedtas(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_SUPPORTEDTAS_STRUCT                              *ie);

/*******************************************************************************
/* ProtocolIE TAI_Cancelled_Item SEQUENCE
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_TAI_STRUCT                                       tAI;
  LIBLTE_S1AP_CANCELLEDCELLINTAI_STRUCT                        cancelledCellinTAI;
  LIBLTE_S1AP_PROTOCOLEXTENSIONCONTAINER_STRUCT                iE_Extensions;
  bool                                                         iE_Extensions_present;
}LIBLTE_S1AP_TAI_CANCELLED_ITEM_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_tai_cancelled_item(
  LIBLTE_S1AP_TAI_CANCELLED_ITEM_STRUCT                        *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_tai_cancelled_item(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_TAI_CANCELLED_ITEM_STRUCT                        *ie);

/*******************************************************************************
/* ProtocolIE X2TNLConfigurationInfo SEQUENCE
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_ENBX2TLAS_STRUCT                                 eNBX2TransportLayerAddresses;
  LIBLTE_S1AP_PROTOCOLEXTENSIONCONTAINER_STRUCT                iE_Extensions;
  bool                                                         iE_Extensions_present;
}LIBLTE_S1AP_X2TNLCONFIGURATIONINFO_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_x2tnlconfigurationinfo(
  LIBLTE_S1AP_X2TNLCONFIGURATIONINFO_STRUCT                    *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_x2tnlconfigurationinfo(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_X2TNLCONFIGURATIONINFO_STRUCT                    *ie);

/*******************************************************************************
/* Protocol Container List Bearers_SubjectToStatusTransferList DYNAMIC SEQUENCE OF
********************************************************************************/
// lb:1, ub:256
typedef struct{
  uint32_t                                                     len;
  LIBLTE_S1AP_BEARERS_SUBJECTTOSTATUSTRANSFER_ITEM_STRUCT      buffer[32]; //WARNING: Artificial limit to reduce memory footprint
}LIBLTE_S1AP_BEARERS_SUBJECTTOSTATUSTRANSFERLIST_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_bearers_subjecttostatustransferlist(
  LIBLTE_S1AP_BEARERS_SUBJECTTOSTATUSTRANSFERLIST_STRUCT       *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_bearers_subjecttostatustransferlist(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_BEARERS_SUBJECTTOSTATUSTRANSFERLIST_STRUCT       *ie);

/*******************************************************************************
/* ProtocolIE CellID_Cancelled DYNAMIC SEQUENCE OF
********************************************************************************/
// lb:1, ub:65535
typedef struct{
  uint32_t                                                     len;
  LIBLTE_S1AP_CELLID_CANCELLED_ITEM_STRUCT                     buffer[32]; //WARNING: Artificial limit to reduce memory footprint
}LIBLTE_S1AP_CELLID_CANCELLED_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_cellid_cancelled(
  LIBLTE_S1AP_CELLID_CANCELLED_STRUCT                          *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_cellid_cancelled(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_CELLID_CANCELLED_STRUCT                          *ie);

/*******************************************************************************
/* ProtocolIE CompletedCellinEAI DYNAMIC SEQUENCE OF
********************************************************************************/
// lb:1, ub:65535
typedef struct{
  uint32_t                                                     len;
  LIBLTE_S1AP_COMPLETEDCELLINEAI_ITEM_STRUCT                   buffer[32]; //WARNING: Artificial limit to reduce memory footprint
}LIBLTE_S1AP_COMPLETEDCELLINEAI_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_completedcellineai(
  LIBLTE_S1AP_COMPLETEDCELLINEAI_STRUCT                        *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_completedcellineai(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_COMPLETEDCELLINEAI_STRUCT                        *ie);

/*******************************************************************************
/* ProtocolIE HandoverRestrictionList SEQUENCE
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_TBCD_STRING_STRUCT                               servingPLMN;
  LIBLTE_S1AP_EPLMNS_STRUCT                                    equivalentPLMNs;
  bool                                                         equivalentPLMNs_present;
  LIBLTE_S1AP_FORBIDDENTAS_STRUCT                              forbiddenTAs;
  bool                                                         forbiddenTAs_present;
  LIBLTE_S1AP_FORBIDDENLAS_STRUCT                              forbiddenLAs;
  bool                                                         forbiddenLAs_present;
  LIBLTE_S1AP_FORBIDDENINTERRATS_ENUM_EXT                      forbiddenInterRATs;
  bool                                                         forbiddenInterRATs_present;
  LIBLTE_S1AP_PROTOCOLEXTENSIONCONTAINER_STRUCT                iE_Extensions;
  bool                                                         iE_Extensions_present;
}LIBLTE_S1AP_HANDOVERRESTRICTIONLIST_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_handoverrestrictionlist(
  LIBLTE_S1AP_HANDOVERRESTRICTIONLIST_STRUCT                   *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_handoverrestrictionlist(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_HANDOVERRESTRICTIONLIST_STRUCT                   *ie);

/*******************************************************************************
/* ProtocolIE LastVisitedCell_Item CHOICE
********************************************************************************/
typedef enum{
  LIBLTE_S1AP_LASTVISITEDCELL_ITEM_CHOICE_E_UTRAN_CELL,
  LIBLTE_S1AP_LASTVISITEDCELL_ITEM_CHOICE_UTRAN_CELL,
  LIBLTE_S1AP_LASTVISITEDCELL_ITEM_CHOICE_GERAN_CELL,
  LIBLTE_S1AP_LASTVISITEDCELL_ITEM_CHOICE_N_ITEMS,
}LIBLTE_S1AP_LASTVISITEDCELL_ITEM_CHOICE_ENUM;
static const char liblte_s1ap_lastvisitedcell_item_choice_text[LIBLTE_S1AP_LASTVISITEDCELL_ITEM_CHOICE_N_ITEMS][50] = {
  "e_UTRAN_Cell",
  "uTRAN_Cell",
  "gERAN_Cell",
};

typedef union{
  LIBLTE_S1AP_LASTVISITEDEUTRANCELLINFORMATION_STRUCT          e_UTRAN_Cell;
  LIBLTE_S1AP_LASTVISITEDUTRANCELLINFORMATION_STRUCT           uTRAN_Cell;
  LIBLTE_S1AP_LASTVISITEDGERANCELLINFORMATION_STRUCT           gERAN_Cell;
}LIBLTE_S1AP_LASTVISITEDCELL_ITEM_CHOICE_UNION;

typedef struct{
  bool                            ext;
  LIBLTE_S1AP_LASTVISITEDCELL_ITEM_CHOICE_UNION choice;
  LIBLTE_S1AP_LASTVISITEDCELL_ITEM_CHOICE_ENUM  choice_type;
}LIBLTE_S1AP_LASTVISITEDCELL_ITEM_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_lastvisitedcell_item(
  LIBLTE_S1AP_LASTVISITEDCELL_ITEM_STRUCT                      *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_lastvisitedcell_item(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_LASTVISITEDCELL_ITEM_STRUCT                      *ie);

/*******************************************************************************
/* ProtocolIE SONInformationReply SEQUENCE
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_X2TNLCONFIGURATIONINFO_STRUCT                    x2TNLConfigurationInfo;
  bool                                                         x2TNLConfigurationInfo_present;
  LIBLTE_S1AP_PROTOCOLEXTENSIONCONTAINER_STRUCT                iE_Extensions;
  bool                                                         iE_Extensions_present;
}LIBLTE_S1AP_SONINFORMATIONREPLY_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_soninformationreply(
  LIBLTE_S1AP_SONINFORMATIONREPLY_STRUCT                       *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_soninformationreply(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_SONINFORMATIONREPLY_STRUCT                       *ie);

/*******************************************************************************
/* ProtocolIE TAI_Broadcast DYNAMIC SEQUENCE OF
********************************************************************************/
// lb:1, ub:65535
typedef struct{
  uint32_t                                                     len;
  LIBLTE_S1AP_TAI_BROADCAST_ITEM_STRUCT                        buffer[32]; //WARNING: Artificial limit to reduce memory footprint
}LIBLTE_S1AP_TAI_BROADCAST_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_tai_broadcast(
  LIBLTE_S1AP_TAI_BROADCAST_STRUCT                             *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_tai_broadcast(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_TAI_BROADCAST_STRUCT                             *ie);

/*******************************************************************************
/* ProtocolIE TimeToWait ENUMERATED
********************************************************************************/
typedef enum{
  LIBLTE_S1AP_TIMETOWAIT_V1S,
  LIBLTE_S1AP_TIMETOWAIT_V2S,
  LIBLTE_S1AP_TIMETOWAIT_V5S,
  LIBLTE_S1AP_TIMETOWAIT_V10S,
  LIBLTE_S1AP_TIMETOWAIT_V20S,
  LIBLTE_S1AP_TIMETOWAIT_V60S,
  LIBLTE_S1AP_TIMETOWAIT_N_ITEMS,
}LIBLTE_S1AP_TIMETOWAIT_ENUM;
static const char liblte_s1ap_timetowait_text[LIBLTE_S1AP_TIMETOWAIT_N_ITEMS][80] = {
  "v1s",
  "v2s",
  "v5s",
  "v10s",
  "v20s",
  "v60s",
};

typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_TIMETOWAIT_ENUM e;
}LIBLTE_S1AP_TIMETOWAIT_ENUM_EXT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_timetowait(
  LIBLTE_S1AP_TIMETOWAIT_ENUM_EXT                              *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_timetowait(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_TIMETOWAIT_ENUM_EXT                              *ie);

/*******************************************************************************
/* ProtocolIE UE_HistoryInformation DYNAMIC SEQUENCE OF
********************************************************************************/
// lb:1, ub:16
typedef struct{
  uint32_t                                                     len;
  LIBLTE_S1AP_LASTVISITEDCELL_ITEM_STRUCT                      buffer[32]; //WARNING: Artificial limit to reduce memory footprint
}LIBLTE_S1AP_UE_HISTORYINFORMATION_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_ue_historyinformation(
  LIBLTE_S1AP_UE_HISTORYINFORMATION_STRUCT                     *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_ue_historyinformation(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_UE_HISTORYINFORMATION_STRUCT                     *ie);

/*******************************************************************************
/* ProtocolIE CriticalityDiagnostics SEQUENCE
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_PROCEDURECODE_STRUCT                             procedureCode;
  bool                                                         procedureCode_present;
  LIBLTE_S1AP_TRIGGERINGMESSAGE_ENUM                           triggeringMessage;
  bool                                                         triggeringMessage_present;
  LIBLTE_S1AP_CRITICALITY_ENUM                                 procedureCriticality;
  bool                                                         procedureCriticality_present;
  LIBLTE_S1AP_CRITICALITYDIAGNOSTICS_IE_LIST_STRUCT            iEsCriticalityDiagnostics;
  bool                                                         iEsCriticalityDiagnostics_present;
  LIBLTE_S1AP_PROTOCOLEXTENSIONCONTAINER_STRUCT                iE_Extensions;
  bool                                                         iE_Extensions_present;
}LIBLTE_S1AP_CRITICALITYDIAGNOSTICS_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_criticalitydiagnostics(
  LIBLTE_S1AP_CRITICALITYDIAGNOSTICS_STRUCT                    *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_criticalitydiagnostics(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_CRITICALITYDIAGNOSTICS_STRUCT                    *ie);

/*******************************************************************************
/* ProtocolIE EmergencyAreaID_Broadcast_Item SEQUENCE
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_EMERGENCYAREAID_STRUCT                           emergencyAreaID;
  LIBLTE_S1AP_COMPLETEDCELLINEAI_STRUCT                        completedCellinEAI;
  LIBLTE_S1AP_PROTOCOLEXTENSIONCONTAINER_STRUCT                iE_Extensions;
  bool                                                         iE_Extensions_present;
}LIBLTE_S1AP_EMERGENCYAREAID_BROADCAST_ITEM_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_emergencyareaid_broadcast_item(
  LIBLTE_S1AP_EMERGENCYAREAID_BROADCAST_ITEM_STRUCT            *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_emergencyareaid_broadcast_item(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_EMERGENCYAREAID_BROADCAST_ITEM_STRUCT            *ie);

/*******************************************************************************
/* ProtocolIE ImmediateMDT SEQUENCE
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_MEASUREMENTSTOACTIVATE_STRUCT                    measurementsToActivate;
  LIBLTE_S1AP_M1REPORTINGTRIGGER_ENUM_EXT                      m1reportingTrigger;
  LIBLTE_S1AP_M1THRESHOLDEVENTA2_STRUCT                        m1thresholdeventA2;
  bool                                                         m1thresholdeventA2_present;
  LIBLTE_S1AP_M1PERIODICREPORTING_STRUCT                       m1periodicReporting;
  bool                                                         m1periodicReporting_present;
  LIBLTE_S1AP_PROTOCOLEXTENSIONCONTAINER_STRUCT                iE_Extensions;
  bool                                                         iE_Extensions_present;
}LIBLTE_S1AP_IMMEDIATEMDT_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_immediatemdt(
  LIBLTE_S1AP_IMMEDIATEMDT_STRUCT                              *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_immediatemdt(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_IMMEDIATEMDT_STRUCT                              *ie);

/*******************************************************************************
/* ProtocolIE MDTMode CHOICE
********************************************************************************/
typedef enum{
  LIBLTE_S1AP_MDTMODE_CHOICE_IMMEDIATEMDT,
  LIBLTE_S1AP_MDTMODE_CHOICE_LOGGEDMDT,
  LIBLTE_S1AP_MDTMODE_CHOICE_N_ITEMS,
}LIBLTE_S1AP_MDTMODE_CHOICE_ENUM;
static const char liblte_s1ap_mdtmode_choice_text[LIBLTE_S1AP_MDTMODE_CHOICE_N_ITEMS][50] = {
  "immediateMDT",
  "loggedMDT",
};

typedef union{
  LIBLTE_S1AP_IMMEDIATEMDT_STRUCT                              immediateMDT;
  LIBLTE_S1AP_LOGGEDMDT_STRUCT                                 loggedMDT;
}LIBLTE_S1AP_MDTMODE_CHOICE_UNION;

typedef struct{
  bool                            ext;
  LIBLTE_S1AP_MDTMODE_CHOICE_UNION choice;
  LIBLTE_S1AP_MDTMODE_CHOICE_ENUM  choice_type;
}LIBLTE_S1AP_MDTMODE_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_mdtmode(
  LIBLTE_S1AP_MDTMODE_STRUCT                                   *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_mdtmode(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MDTMODE_STRUCT                                   *ie);

/*******************************************************************************
/* ProtocolIE SourceeNB_ToTargeteNB_TransparentContainer SEQUENCE
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_RRC_CONTAINER_STRUCT                             rRC_Container;
  LIBLTE_S1AP_E_RABINFORMATIONLIST_STRUCT                      e_RABInformationList;
  bool                                                         e_RABInformationList_present;
  LIBLTE_S1AP_EUTRAN_CGI_STRUCT                                targetCell_ID;
  LIBLTE_S1AP_SUBSCRIBERPROFILEIDFORRFP_STRUCT                 subscriberProfileIDforRFP;
  bool                                                         subscriberProfileIDforRFP_present;
  LIBLTE_S1AP_UE_HISTORYINFORMATION_STRUCT                     uE_HistoryInformation;
  LIBLTE_S1AP_PROTOCOLEXTENSIONCONTAINER_STRUCT                iE_Extensions;
  bool                                                         iE_Extensions_present;
}LIBLTE_S1AP_SOURCEENB_TOTARGETENB_TRANSPARENTCONTAINER_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_sourceenb_totargetenb_transparentcontainer(
  LIBLTE_S1AP_SOURCEENB_TOTARGETENB_TRANSPARENTCONTAINER_STRUCT *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_sourceenb_totargetenb_transparentcontainer(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_SOURCEENB_TOTARGETENB_TRANSPARENTCONTAINER_STRUCT *ie);

/*******************************************************************************
/* ProtocolIE EmergencyAreaID_Broadcast DYNAMIC SEQUENCE OF
********************************************************************************/
// lb:1, ub:65535
typedef struct{
  uint32_t                                                     len;
  LIBLTE_S1AP_EMERGENCYAREAID_BROADCAST_ITEM_STRUCT            buffer[32]; //WARNING: Artificial limit to reduce memory footprint
}LIBLTE_S1AP_EMERGENCYAREAID_BROADCAST_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_emergencyareaid_broadcast(
  LIBLTE_S1AP_EMERGENCYAREAID_BROADCAST_STRUCT                 *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_emergencyareaid_broadcast(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_EMERGENCYAREAID_BROADCAST_STRUCT                 *ie);

/*******************************************************************************
/* ProtocolIE MDT_Configuration SEQUENCE
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_MDT_ACTIVATION_ENUM_EXT                          mdt_Activation;
  LIBLTE_S1AP_AREASCOPEOFMDT_STRUCT                            areaScopeOfMDT;
  LIBLTE_S1AP_MDTMODE_STRUCT                                   mDTMode;
  LIBLTE_S1AP_PROTOCOLEXTENSIONCONTAINER_STRUCT                iE_Extensions;
  bool                                                         iE_Extensions_present;
}LIBLTE_S1AP_MDT_CONFIGURATION_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_mdt_configuration(
  LIBLTE_S1AP_MDT_CONFIGURATION_STRUCT                         *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_mdt_configuration(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MDT_CONFIGURATION_STRUCT                         *ie);

/*******************************************************************************
/* ProtocolIE TAI_Cancelled DYNAMIC SEQUENCE OF
********************************************************************************/
// lb:1, ub:65535
typedef struct{
  uint32_t                                                     len;
  LIBLTE_S1AP_TAI_CANCELLED_ITEM_STRUCT                        buffer[32]; //WARNING: Artificial limit to reduce memory footprint
}LIBLTE_S1AP_TAI_CANCELLED_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_tai_cancelled(
  LIBLTE_S1AP_TAI_CANCELLED_STRUCT                             *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_tai_cancelled(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_TAI_CANCELLED_STRUCT                             *ie);

/*******************************************************************************
/* ProtocolIE BroadcastCancelledAreaList CHOICE
********************************************************************************/
typedef enum{
  LIBLTE_S1AP_BROADCASTCANCELLEDAREALIST_CHOICE_CELLID_CANCELLED,
  LIBLTE_S1AP_BROADCASTCANCELLEDAREALIST_CHOICE_TAI_CANCELLED,
  LIBLTE_S1AP_BROADCASTCANCELLEDAREALIST_CHOICE_EMERGENCYAREAID_CANCELLED,
  LIBLTE_S1AP_BROADCASTCANCELLEDAREALIST_CHOICE_N_ITEMS,
}LIBLTE_S1AP_BROADCASTCANCELLEDAREALIST_CHOICE_ENUM;
static const char liblte_s1ap_broadcastcancelledarealist_choice_text[LIBLTE_S1AP_BROADCASTCANCELLEDAREALIST_CHOICE_N_ITEMS][50] = {
  "cellID_Cancelled",
  "tAI_Cancelled",
  "emergencyAreaID_Cancelled",
};

typedef union{
  LIBLTE_S1AP_CELLID_CANCELLED_STRUCT                          cellID_Cancelled;
  LIBLTE_S1AP_TAI_CANCELLED_STRUCT                             tAI_Cancelled;
  LIBLTE_S1AP_EMERGENCYAREAID_CANCELLED_STRUCT                 emergencyAreaID_Cancelled;
}LIBLTE_S1AP_BROADCASTCANCELLEDAREALIST_CHOICE_UNION;

typedef struct{
  bool                            ext;
  LIBLTE_S1AP_BROADCASTCANCELLEDAREALIST_CHOICE_UNION choice;
  LIBLTE_S1AP_BROADCASTCANCELLEDAREALIST_CHOICE_ENUM  choice_type;
}LIBLTE_S1AP_BROADCASTCANCELLEDAREALIST_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_broadcastcancelledarealist(
  LIBLTE_S1AP_BROADCASTCANCELLEDAREALIST_STRUCT                *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_broadcastcancelledarealist(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_BROADCASTCANCELLEDAREALIST_STRUCT                *ie);

/*******************************************************************************
/* ProtocolIE ENB_StatusTransfer_TransparentContainer SEQUENCE
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_BEARERS_SUBJECTTOSTATUSTRANSFERLIST_STRUCT       bearers_SubjectToStatusTransferList;
  LIBLTE_S1AP_PROTOCOLEXTENSIONCONTAINER_STRUCT                iE_Extensions;
  bool                                                         iE_Extensions_present;
}LIBLTE_S1AP_ENB_STATUSTRANSFER_TRANSPARENTCONTAINER_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_enb_statustransfer_transparentcontainer(
  LIBLTE_S1AP_ENB_STATUSTRANSFER_TRANSPARENTCONTAINER_STRUCT   *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_enb_statustransfer_transparentcontainer(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_ENB_STATUSTRANSFER_TRANSPARENTCONTAINER_STRUCT   *ie);

/*******************************************************************************
/* ProtocolIE TraceActivation SEQUENCE
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_E_UTRAN_TRACE_ID_STRUCT                          e_UTRAN_Trace_ID;
  LIBLTE_S1AP_INTERFACESTOTRACE_STRUCT                         interfacesToTrace;
  LIBLTE_S1AP_TRACEDEPTH_ENUM_EXT                              traceDepth;
  LIBLTE_S1AP_TRANSPORTLAYERADDRESS_STRUCT                     traceCollectionEntityIPAddress;
  LIBLTE_S1AP_PROTOCOLEXTENSIONCONTAINER_STRUCT                iE_Extensions;
  bool                                                         iE_Extensions_present;
}LIBLTE_S1AP_TRACEACTIVATION_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_traceactivation(
  LIBLTE_S1AP_TRACEACTIVATION_STRUCT                           *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_traceactivation(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_TRACEACTIVATION_STRUCT                           *ie);

/*******************************************************************************
/* ProtocolIE BroadcastCompletedAreaList CHOICE
********************************************************************************/
typedef enum{
  LIBLTE_S1AP_BROADCASTCOMPLETEDAREALIST_CHOICE_CELLID_BROADCAST,
  LIBLTE_S1AP_BROADCASTCOMPLETEDAREALIST_CHOICE_TAI_BROADCAST,
  LIBLTE_S1AP_BROADCASTCOMPLETEDAREALIST_CHOICE_EMERGENCYAREAID_BROADCAST,
  LIBLTE_S1AP_BROADCASTCOMPLETEDAREALIST_CHOICE_N_ITEMS,
}LIBLTE_S1AP_BROADCASTCOMPLETEDAREALIST_CHOICE_ENUM;
static const char liblte_s1ap_broadcastcompletedarealist_choice_text[LIBLTE_S1AP_BROADCASTCOMPLETEDAREALIST_CHOICE_N_ITEMS][50] = {
  "cellID_Broadcast",
  "tAI_Broadcast",
  "emergencyAreaID_Broadcast",
};

typedef union{
  LIBLTE_S1AP_CELLID_BROADCAST_STRUCT                          cellID_Broadcast;
  LIBLTE_S1AP_TAI_BROADCAST_STRUCT                             tAI_Broadcast;
  LIBLTE_S1AP_EMERGENCYAREAID_BROADCAST_STRUCT                 emergencyAreaID_Broadcast;
}LIBLTE_S1AP_BROADCASTCOMPLETEDAREALIST_CHOICE_UNION;

typedef struct{
  bool                            ext;
  LIBLTE_S1AP_BROADCASTCOMPLETEDAREALIST_CHOICE_UNION choice;
  LIBLTE_S1AP_BROADCASTCOMPLETEDAREALIST_CHOICE_ENUM  choice_type;
}LIBLTE_S1AP_BROADCASTCOMPLETEDAREALIST_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_broadcastcompletedarealist(
  LIBLTE_S1AP_BROADCASTCOMPLETEDAREALIST_STRUCT                *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_broadcastcompletedarealist(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_BROADCASTCOMPLETEDAREALIST_STRUCT                *ie);

/*******************************************************************************
/* ProtocolIE SONInformation CHOICE
********************************************************************************/
typedef enum{
  LIBLTE_S1AP_SONINFORMATION_CHOICE_SONINFORMATIONREQUEST,
  LIBLTE_S1AP_SONINFORMATION_CHOICE_SONINFORMATIONREPLY,
  LIBLTE_S1AP_SONINFORMATION_CHOICE_N_ITEMS,
}LIBLTE_S1AP_SONINFORMATION_CHOICE_ENUM;
static const char liblte_s1ap_soninformation_choice_text[LIBLTE_S1AP_SONINFORMATION_CHOICE_N_ITEMS][50] = {
  "sONInformationRequest",
  "sONInformationReply",
};

typedef union{
  LIBLTE_S1AP_SONINFORMATIONREQUEST_ENUM_EXT                   sONInformationRequest;
  LIBLTE_S1AP_SONINFORMATIONREPLY_STRUCT                       sONInformationReply;
}LIBLTE_S1AP_SONINFORMATION_CHOICE_UNION;

typedef struct{
  bool                            ext;
  LIBLTE_S1AP_SONINFORMATION_CHOICE_UNION choice;
  LIBLTE_S1AP_SONINFORMATION_CHOICE_ENUM  choice_type;
}LIBLTE_S1AP_SONINFORMATION_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_soninformation(
  LIBLTE_S1AP_SONINFORMATION_STRUCT                            *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_soninformation(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_SONINFORMATION_STRUCT                            *ie);

/*******************************************************************************
/* ProtocolIE SONConfigurationTransfer SEQUENCE
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_TARGETENB_ID_STRUCT                              targeteNB_ID;
  LIBLTE_S1AP_SOURCEENB_ID_STRUCT                              sourceeNB_ID;
  LIBLTE_S1AP_SONINFORMATION_STRUCT                            sONInformation;
  LIBLTE_S1AP_PROTOCOLEXTENSIONCONTAINER_STRUCT                iE_Extensions;
  bool                                                         iE_Extensions_present;
}LIBLTE_S1AP_SONCONFIGURATIONTRANSFER_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_sonconfigurationtransfer(
  LIBLTE_S1AP_SONCONFIGURATIONTRANSFER_STRUCT                  *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_sonconfigurationtransfer(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_SONCONFIGURATIONTRANSFER_STRUCT                  *ie);

/*******************************************************************************
/* ProtocolIE ResetAll ENUMERATED
********************************************************************************/
typedef enum{
  LIBLTE_S1AP_RESETALL_RESET_ALL,
  LIBLTE_S1AP_RESETALL_N_ITEMS,
}LIBLTE_S1AP_RESETALL_ENUM;
static const char liblte_s1ap_resetall_text[LIBLTE_S1AP_RESETALL_N_ITEMS][80] = {
  "reset-all",
};

typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_RESETALL_ENUM e;
}LIBLTE_S1AP_RESETALL_ENUM_EXT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_resetall(
  LIBLTE_S1AP_RESETALL_ENUM_EXT                                *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_resetall(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_RESETALL_ENUM_EXT                                *ie);

/*******************************************************************************
/* ProtocolIE Inter_SystemInformationTransferType CHOICE
********************************************************************************/
typedef enum{
  LIBLTE_S1AP_INTER_SYSTEMINFORMATIONTRANSFERTYPE_CHOICE_RIMTRANSFER,
  LIBLTE_S1AP_INTER_SYSTEMINFORMATIONTRANSFERTYPE_CHOICE_N_ITEMS,
}LIBLTE_S1AP_INTER_SYSTEMINFORMATIONTRANSFERTYPE_CHOICE_ENUM;
static const char liblte_s1ap_inter_systeminformationtransfertype_choice_text[LIBLTE_S1AP_INTER_SYSTEMINFORMATIONTRANSFERTYPE_CHOICE_N_ITEMS][50] = {
  "rIMTransfer",
};

typedef union{
  LIBLTE_S1AP_RIMTRANSFER_STRUCT                               rIMTransfer;
}LIBLTE_S1AP_INTER_SYSTEMINFORMATIONTRANSFERTYPE_CHOICE_UNION;

typedef struct{
  bool                            ext;
  LIBLTE_S1AP_INTER_SYSTEMINFORMATIONTRANSFERTYPE_CHOICE_UNION choice;
  LIBLTE_S1AP_INTER_SYSTEMINFORMATIONTRANSFERTYPE_CHOICE_ENUM  choice_type;
}LIBLTE_S1AP_INTER_SYSTEMINFORMATIONTRANSFERTYPE_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_inter_systeminformationtransfertype(
  LIBLTE_S1AP_INTER_SYSTEMINFORMATIONTRANSFERTYPE_STRUCT       *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_inter_systeminformationtransfertype(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_INTER_SYSTEMINFORMATIONTRANSFERTYPE_STRUCT       *ie);

/*******************************************************************************
/* ProtocolIE E_RAB_IE_ContainerPairList DYNAMIC SEQUENCE OF
********************************************************************************/
// lb:1, ub:256
typedef struct{
  uint32_t                                                     len;
  LIBLTE_S1AP_PROTOCOLIE_CONTAINERPAIR_STRUCT                  buffer[32]; //WARNING: Artificial limit to reduce memory footprint
}LIBLTE_S1AP_E_RAB_IE_CONTAINERPAIRLIST_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_e_rab_ie_containerpairlist(
  LIBLTE_S1AP_E_RAB_IE_CONTAINERPAIRLIST_STRUCT                *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_e_rab_ie_containerpairlist(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_E_RAB_IE_CONTAINERPAIRLIST_STRUCT                *ie);

/*******************************************************************************
/* ProtocolIE E_RABDataForwardingItem SEQUENCE
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_E_RAB_ID_STRUCT                                  e_RAB_ID;
  LIBLTE_S1AP_TRANSPORTLAYERADDRESS_STRUCT                     dL_transportLayerAddress;
  bool                                                         dL_transportLayerAddress_present;
  LIBLTE_S1AP_GTP_TEID_STRUCT                                  dL_gTP_TEID;
  bool                                                         dL_gTP_TEID_present;
  LIBLTE_S1AP_TRANSPORTLAYERADDRESS_STRUCT                     uL_TransportLayerAddress;
  bool                                                         uL_TransportLayerAddress_present;
  LIBLTE_S1AP_GTP_TEID_STRUCT                                  uL_GTP_TEID;
  bool                                                         uL_GTP_TEID_present;
  LIBLTE_S1AP_PROTOCOLEXTENSIONCONTAINER_STRUCT                iE_Extensions;
  bool                                                         iE_Extensions_present;
}LIBLTE_S1AP_E_RABDATAFORWARDINGITEM_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_e_rabdataforwardingitem(
  LIBLTE_S1AP_E_RABDATAFORWARDINGITEM_STRUCT                   *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_e_rabdataforwardingitem(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_E_RABDATAFORWARDINGITEM_STRUCT                   *ie);

/*******************************************************************************
/* ProtocolIE E_RABToBeSetupItemHOReq SEQUENCE
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_E_RAB_ID_STRUCT                                  e_RAB_ID;
  LIBLTE_S1AP_TRANSPORTLAYERADDRESS_STRUCT                     transportLayerAddress;
  LIBLTE_S1AP_GTP_TEID_STRUCT                                  gTP_TEID;
  LIBLTE_S1AP_E_RABLEVELQOSPARAMETERS_STRUCT                   e_RABlevelQosParameters;
  LIBLTE_S1AP_PROTOCOLEXTENSIONCONTAINER_STRUCT                iE_Extensions;
  bool                                                         iE_Extensions_present;
}LIBLTE_S1AP_E_RABTOBESETUPITEMHOREQ_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_e_rabtobesetupitemhoreq(
  LIBLTE_S1AP_E_RABTOBESETUPITEMHOREQ_STRUCT                   *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_e_rabtobesetupitemhoreq(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_E_RABTOBESETUPITEMHOREQ_STRUCT                   *ie);

/*******************************************************************************
/* ProtocolIE E_RABAdmittedItem SEQUENCE
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_E_RAB_ID_STRUCT                                  e_RAB_ID;
  LIBLTE_S1AP_TRANSPORTLAYERADDRESS_STRUCT                     transportLayerAddress;
  LIBLTE_S1AP_GTP_TEID_STRUCT                                  gTP_TEID;
  LIBLTE_S1AP_TRANSPORTLAYERADDRESS_STRUCT                     dL_transportLayerAddress;
  bool                                                         dL_transportLayerAddress_present;
  LIBLTE_S1AP_GTP_TEID_STRUCT                                  dL_gTP_TEID;
  bool                                                         dL_gTP_TEID_present;
  LIBLTE_S1AP_TRANSPORTLAYERADDRESS_STRUCT                     uL_TransportLayerAddress;
  bool                                                         uL_TransportLayerAddress_present;
  LIBLTE_S1AP_GTP_TEID_STRUCT                                  uL_GTP_TEID;
  bool                                                         uL_GTP_TEID_present;
  LIBLTE_S1AP_PROTOCOLEXTENSIONCONTAINER_STRUCT                iE_Extensions;
  bool                                                         iE_Extensions_present;
}LIBLTE_S1AP_E_RABADMITTEDITEM_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_e_rabadmitteditem(
  LIBLTE_S1AP_E_RABADMITTEDITEM_STRUCT                         *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_e_rabadmitteditem(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_E_RABADMITTEDITEM_STRUCT                         *ie);

/*******************************************************************************
/* ProtocolIE E_RABFailedToSetupItemHOReqAck SEQUENCE
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_E_RAB_ID_STRUCT                                  e_RAB_ID;
  LIBLTE_S1AP_CAUSE_STRUCT                                     cause;
  LIBLTE_S1AP_PROTOCOLEXTENSIONCONTAINER_STRUCT                iE_Extensions;
  bool                                                         iE_Extensions_present;
}LIBLTE_S1AP_E_RABFAILEDTOSETUPITEMHOREQACK_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_e_rabfailedtosetupitemhoreqack(
  LIBLTE_S1AP_E_RABFAILEDTOSETUPITEMHOREQACK_STRUCT            *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_e_rabfailedtosetupitemhoreqack(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_E_RABFAILEDTOSETUPITEMHOREQACK_STRUCT            *ie);

/*******************************************************************************
/* ProtocolIE E_RABToBeSwitchedDLItem SEQUENCE
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_E_RAB_ID_STRUCT                                  e_RAB_ID;
  LIBLTE_S1AP_TRANSPORTLAYERADDRESS_STRUCT                     transportLayerAddress;
  LIBLTE_S1AP_GTP_TEID_STRUCT                                  gTP_TEID;
  LIBLTE_S1AP_PROTOCOLEXTENSIONCONTAINER_STRUCT                iE_Extensions;
  bool                                                         iE_Extensions_present;
}LIBLTE_S1AP_E_RABTOBESWITCHEDDLITEM_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_e_rabtobeswitcheddlitem(
  LIBLTE_S1AP_E_RABTOBESWITCHEDDLITEM_STRUCT                   *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_e_rabtobeswitcheddlitem(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_E_RABTOBESWITCHEDDLITEM_STRUCT                   *ie);

/*******************************************************************************
/* ProtocolIE E_RABToBeSwitchedULItem SEQUENCE
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_E_RAB_ID_STRUCT                                  e_RAB_ID;
  LIBLTE_S1AP_TRANSPORTLAYERADDRESS_STRUCT                     transportLayerAddress;
  LIBLTE_S1AP_GTP_TEID_STRUCT                                  gTP_TEID;
  LIBLTE_S1AP_PROTOCOLEXTENSIONCONTAINER_STRUCT                iE_Extensions;
  bool                                                         iE_Extensions_present;
}LIBLTE_S1AP_E_RABTOBESWITCHEDULITEM_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_e_rabtobeswitchedulitem(
  LIBLTE_S1AP_E_RABTOBESWITCHEDULITEM_STRUCT                   *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_e_rabtobeswitchedulitem(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_E_RABTOBESWITCHEDULITEM_STRUCT                   *ie);

/*******************************************************************************
/* ProtocolIE E_RABToBeSetupItemBearerSUReq SEQUENCE
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_E_RAB_ID_STRUCT                                  e_RAB_ID;
  LIBLTE_S1AP_E_RABLEVELQOSPARAMETERS_STRUCT                   e_RABlevelQoSParameters;
  LIBLTE_S1AP_TRANSPORTLAYERADDRESS_STRUCT                     transportLayerAddress;
  LIBLTE_S1AP_GTP_TEID_STRUCT                                  gTP_TEID;
  LIBLTE_S1AP_NAS_PDU_STRUCT                                   nAS_PDU;
  LIBLTE_S1AP_PROTOCOLEXTENSIONCONTAINER_STRUCT                iE_Extensions;
  bool                                                         iE_Extensions_present;
}LIBLTE_S1AP_E_RABTOBESETUPITEMBEARERSUREQ_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_e_rabtobesetupitembearersureq(
  LIBLTE_S1AP_E_RABTOBESETUPITEMBEARERSUREQ_STRUCT             *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_e_rabtobesetupitembearersureq(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_E_RABTOBESETUPITEMBEARERSUREQ_STRUCT             *ie);

/*******************************************************************************
/* ProtocolIE E_RABSetupItemBearerSURes SEQUENCE
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_E_RAB_ID_STRUCT                                  e_RAB_ID;
  LIBLTE_S1AP_TRANSPORTLAYERADDRESS_STRUCT                     transportLayerAddress;
  LIBLTE_S1AP_GTP_TEID_STRUCT                                  gTP_TEID;
  LIBLTE_S1AP_PROTOCOLEXTENSIONCONTAINER_STRUCT                iE_Extensions;
  bool                                                         iE_Extensions_present;
}LIBLTE_S1AP_E_RABSETUPITEMBEARERSURES_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_e_rabsetupitembearersures(
  LIBLTE_S1AP_E_RABSETUPITEMBEARERSURES_STRUCT                 *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_e_rabsetupitembearersures(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_E_RABSETUPITEMBEARERSURES_STRUCT                 *ie);

/*******************************************************************************
/* ProtocolIE E_RABToBeModifiedItemBearerModReq SEQUENCE
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_E_RAB_ID_STRUCT                                  e_RAB_ID;
  LIBLTE_S1AP_E_RABLEVELQOSPARAMETERS_STRUCT                   e_RABLevelQoSParameters;
  LIBLTE_S1AP_NAS_PDU_STRUCT                                   nAS_PDU;
  LIBLTE_S1AP_PROTOCOLEXTENSIONCONTAINER_STRUCT                iE_Extensions;
  bool                                                         iE_Extensions_present;
}LIBLTE_S1AP_E_RABTOBEMODIFIEDITEMBEARERMODREQ_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_e_rabtobemodifieditembearermodreq(
  LIBLTE_S1AP_E_RABTOBEMODIFIEDITEMBEARERMODREQ_STRUCT         *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_e_rabtobemodifieditembearermodreq(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_E_RABTOBEMODIFIEDITEMBEARERMODREQ_STRUCT         *ie);

/*******************************************************************************
/* ProtocolIE E_RABModifyItemBearerModRes SEQUENCE
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_E_RAB_ID_STRUCT                                  e_RAB_ID;
  LIBLTE_S1AP_PROTOCOLEXTENSIONCONTAINER_STRUCT                iE_Extensions;
  bool                                                         iE_Extensions_present;
}LIBLTE_S1AP_E_RABMODIFYITEMBEARERMODRES_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_e_rabmodifyitembearermodres(
  LIBLTE_S1AP_E_RABMODIFYITEMBEARERMODRES_STRUCT               *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_e_rabmodifyitembearermodres(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_E_RABMODIFYITEMBEARERMODRES_STRUCT               *ie);

/*******************************************************************************
/* ProtocolIE E_RABReleaseItemBearerRelComp SEQUENCE
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_E_RAB_ID_STRUCT                                  e_RAB_ID;
  LIBLTE_S1AP_PROTOCOLEXTENSIONCONTAINER_STRUCT                iE_Extensions;
  bool                                                         iE_Extensions_present;
}LIBLTE_S1AP_E_RABRELEASEITEMBEARERRELCOMP_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_e_rabreleaseitembearerrelcomp(
  LIBLTE_S1AP_E_RABRELEASEITEMBEARERRELCOMP_STRUCT             *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_e_rabreleaseitembearerrelcomp(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_E_RABRELEASEITEMBEARERRELCOMP_STRUCT             *ie);

/*******************************************************************************
/* ProtocolIE E_RABToBeSetupItemCtxtSUReq SEQUENCE
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_E_RAB_ID_STRUCT                                  e_RAB_ID;
  LIBLTE_S1AP_E_RABLEVELQOSPARAMETERS_STRUCT                   e_RABlevelQoSParameters;
  LIBLTE_S1AP_TRANSPORTLAYERADDRESS_STRUCT                     transportLayerAddress;
  LIBLTE_S1AP_GTP_TEID_STRUCT                                  gTP_TEID;
  LIBLTE_S1AP_NAS_PDU_STRUCT                                   nAS_PDU;
  bool                                                         nAS_PDU_present;
  LIBLTE_S1AP_PROTOCOLEXTENSIONCONTAINER_STRUCT                iE_Extensions;
  bool                                                         iE_Extensions_present;
}LIBLTE_S1AP_E_RABTOBESETUPITEMCTXTSUREQ_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_e_rabtobesetupitemctxtsureq(
  LIBLTE_S1AP_E_RABTOBESETUPITEMCTXTSUREQ_STRUCT               *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_e_rabtobesetupitemctxtsureq(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_E_RABTOBESETUPITEMCTXTSUREQ_STRUCT               *ie);

/*******************************************************************************
/* ProtocolIE E_RABSetupItemCtxtSURes SEQUENCE
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_E_RAB_ID_STRUCT                                  e_RAB_ID;
  LIBLTE_S1AP_TRANSPORTLAYERADDRESS_STRUCT                     transportLayerAddress;
  LIBLTE_S1AP_GTP_TEID_STRUCT                                  gTP_TEID;
  LIBLTE_S1AP_PROTOCOLEXTENSIONCONTAINER_STRUCT                iE_Extensions;
  bool                                                         iE_Extensions_present;
}LIBLTE_S1AP_E_RABSETUPITEMCTXTSURES_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_e_rabsetupitemctxtsures(
  LIBLTE_S1AP_E_RABSETUPITEMCTXTSURES_STRUCT                   *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_e_rabsetupitemctxtsures(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_E_RABSETUPITEMCTXTSURES_STRUCT                   *ie);

/*******************************************************************************
/* ProtocolIE TAIItem SEQUENCE
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_TAI_STRUCT                                       tAI;
  LIBLTE_S1AP_PROTOCOLEXTENSIONCONTAINER_STRUCT                iE_Extensions;
  bool                                                         iE_Extensions_present;
}LIBLTE_S1AP_TAIITEM_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_taiitem(
  LIBLTE_S1AP_TAIITEM_STRUCT                                   *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_taiitem(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_TAIITEM_STRUCT                                   *ie);

/*******************************************************************************
/* Protocol Container List UE_associatedLogicalS1_ConnectionListRes DYNAMIC SEQUENCE OF
********************************************************************************/
// lb:1, ub:256
typedef struct{
  uint32_t                                                     len;
  LIBLTE_S1AP_UE_ASSOCIATEDLOGICALS1_CONNECTIONITEM_STRUCT     buffer[32]; //WARNING: Artificial limit to reduce memory footprint
}LIBLTE_S1AP_UE_ASSOCIATEDLOGICALS1_CONNECTIONLISTRES_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_ue_associatedlogicals1_connectionlistres(
  LIBLTE_S1AP_UE_ASSOCIATEDLOGICALS1_CONNECTIONLISTRES_STRUCT  *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_ue_associatedlogicals1_connectionlistres(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_UE_ASSOCIATEDLOGICALS1_CONNECTIONLISTRES_STRUCT  *ie);

/*******************************************************************************
/* Protocol Container List UE_associatedLogicalS1_ConnectionListResAck DYNAMIC SEQUENCE OF
********************************************************************************/
// lb:1, ub:256
typedef struct{
  uint32_t                                                     len;
  LIBLTE_S1AP_UE_ASSOCIATEDLOGICALS1_CONNECTIONITEM_STRUCT     buffer[32]; //WARNING: Artificial limit to reduce memory footprint
}LIBLTE_S1AP_UE_ASSOCIATEDLOGICALS1_CONNECTIONLISTRESACK_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_ue_associatedlogicals1_connectionlistresack(
  LIBLTE_S1AP_UE_ASSOCIATEDLOGICALS1_CONNECTIONLISTRESACK_STRUCT *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_ue_associatedlogicals1_connectionlistresack(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_UE_ASSOCIATEDLOGICALS1_CONNECTIONLISTRESACK_STRUCT *ie);

/*******************************************************************************
/* ProtocolIE PrivateMessage SEQUENCE
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_PRIVATEIE_CONTAINER_STRUCT                       privateIEs;
}LIBLTE_S1AP_PRIVATEMESSAGE_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_privatemessage(
  LIBLTE_S1AP_PRIVATEMESSAGE_STRUCT                            *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_privatemessage(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_PRIVATEMESSAGE_STRUCT                            *ie);

/*******************************************************************************
/* ProtocolIE ResetType CHOICE
********************************************************************************/
typedef enum{
  LIBLTE_S1AP_RESETTYPE_CHOICE_S1_INTERFACE,
  LIBLTE_S1AP_RESETTYPE_CHOICE_PARTOFS1_INTERFACE,
  LIBLTE_S1AP_RESETTYPE_CHOICE_N_ITEMS,
}LIBLTE_S1AP_RESETTYPE_CHOICE_ENUM;
static const char liblte_s1ap_resettype_choice_text[LIBLTE_S1AP_RESETTYPE_CHOICE_N_ITEMS][50] = {
  "s1_Interface",
  "partOfS1_Interface",
};

typedef union{
  LIBLTE_S1AP_RESETALL_ENUM_EXT                                s1_Interface;
  LIBLTE_S1AP_UE_ASSOCIATEDLOGICALS1_CONNECTIONLISTRES_STRUCT  partOfS1_Interface;
}LIBLTE_S1AP_RESETTYPE_CHOICE_UNION;

typedef struct{
  bool                            ext;
  LIBLTE_S1AP_RESETTYPE_CHOICE_UNION choice;
  LIBLTE_S1AP_RESETTYPE_CHOICE_ENUM  choice_type;
}LIBLTE_S1AP_RESETTYPE_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_resettype(
  LIBLTE_S1AP_RESETTYPE_STRUCT                                 *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_resettype(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_RESETTYPE_STRUCT                                 *ie);

/*******************************************************************************
/* Protocol Container List E_RABSubjecttoDataForwardingList DYNAMIC SEQUENCE OF
********************************************************************************/
// lb:1, ub:256
typedef struct{
  uint32_t                                                     len;
  LIBLTE_S1AP_E_RABDATAFORWARDINGITEM_STRUCT                   buffer[32]; //WARNING: Artificial limit to reduce memory footprint
}LIBLTE_S1AP_E_RABSUBJECTTODATAFORWARDINGLIST_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_e_rabsubjecttodataforwardinglist(
  LIBLTE_S1AP_E_RABSUBJECTTODATAFORWARDINGLIST_STRUCT          *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_e_rabsubjecttodataforwardinglist(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_E_RABSUBJECTTODATAFORWARDINGLIST_STRUCT          *ie);

/*******************************************************************************
/* Protocol Container List E_RABToBeSetupListHOReq DYNAMIC SEQUENCE OF
********************************************************************************/
// lb:1, ub:256
typedef struct{
  uint32_t                                                     len;
  LIBLTE_S1AP_E_RABTOBESETUPITEMHOREQ_STRUCT                   buffer[32]; //WARNING: Artificial limit to reduce memory footprint
}LIBLTE_S1AP_E_RABTOBESETUPLISTHOREQ_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_e_rabtobesetuplisthoreq(
  LIBLTE_S1AP_E_RABTOBESETUPLISTHOREQ_STRUCT                   *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_e_rabtobesetuplisthoreq(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_E_RABTOBESETUPLISTHOREQ_STRUCT                   *ie);

/*******************************************************************************
/* Protocol Container List E_RABAdmittedList DYNAMIC SEQUENCE OF
********************************************************************************/
// lb:1, ub:256
typedef struct{
  uint32_t                                                     len;
  LIBLTE_S1AP_E_RABADMITTEDITEM_STRUCT                         buffer[32]; //WARNING: Artificial limit to reduce memory footprint
}LIBLTE_S1AP_E_RABADMITTEDLIST_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_e_rabadmittedlist(
  LIBLTE_S1AP_E_RABADMITTEDLIST_STRUCT                         *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_e_rabadmittedlist(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_E_RABADMITTEDLIST_STRUCT                         *ie);

/*******************************************************************************
/* Protocol Container List E_RABToBeSwitchedDLList DYNAMIC SEQUENCE OF
********************************************************************************/
// lb:1, ub:256
typedef struct{
  uint32_t                                                     len;
  LIBLTE_S1AP_E_RABTOBESWITCHEDDLITEM_STRUCT                   buffer[32]; //WARNING: Artificial limit to reduce memory footprint
}LIBLTE_S1AP_E_RABTOBESWITCHEDDLLIST_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_e_rabtobeswitcheddllist(
  LIBLTE_S1AP_E_RABTOBESWITCHEDDLLIST_STRUCT                   *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_e_rabtobeswitcheddllist(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_E_RABTOBESWITCHEDDLLIST_STRUCT                   *ie);

/*******************************************************************************
/* Protocol Container List E_RABToBeSwitchedULList DYNAMIC SEQUENCE OF
********************************************************************************/
// lb:1, ub:256
typedef struct{
  uint32_t                                                     len;
  LIBLTE_S1AP_E_RABTOBESWITCHEDULITEM_STRUCT                   buffer[32]; //WARNING: Artificial limit to reduce memory footprint
}LIBLTE_S1AP_E_RABTOBESWITCHEDULLIST_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_e_rabtobeswitchedullist(
  LIBLTE_S1AP_E_RABTOBESWITCHEDULLIST_STRUCT                   *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_e_rabtobeswitchedullist(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_E_RABTOBESWITCHEDULLIST_STRUCT                   *ie);

/*******************************************************************************
/* Protocol Container List E_RABToBeSetupListBearerSUReq DYNAMIC SEQUENCE OF
********************************************************************************/
// lb:1, ub:256
typedef struct{
  uint32_t                                                     len;
  LIBLTE_S1AP_E_RABTOBESETUPITEMBEARERSUREQ_STRUCT             buffer[32]; //WARNING: Artificial limit to reduce memory footprint
}LIBLTE_S1AP_E_RABTOBESETUPLISTBEARERSUREQ_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_e_rabtobesetuplistbearersureq(
  LIBLTE_S1AP_E_RABTOBESETUPLISTBEARERSUREQ_STRUCT             *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_e_rabtobesetuplistbearersureq(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_E_RABTOBESETUPLISTBEARERSUREQ_STRUCT             *ie);

/*******************************************************************************
/* Protocol Container List E_RABSetupListBearerSURes DYNAMIC SEQUENCE OF
********************************************************************************/
// lb:1, ub:256
typedef struct{
  uint32_t                                                     len;
  LIBLTE_S1AP_E_RABSETUPITEMBEARERSURES_STRUCT                 buffer[32]; //WARNING: Artificial limit to reduce memory footprint
}LIBLTE_S1AP_E_RABSETUPLISTBEARERSURES_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_e_rabsetuplistbearersures(
  LIBLTE_S1AP_E_RABSETUPLISTBEARERSURES_STRUCT                 *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_e_rabsetuplistbearersures(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_E_RABSETUPLISTBEARERSURES_STRUCT                 *ie);

/*******************************************************************************
/* Protocol Container List E_RABToBeModifiedListBearerModReq DYNAMIC SEQUENCE OF
********************************************************************************/
// lb:1, ub:256
typedef struct{
  uint32_t                                                     len;
  LIBLTE_S1AP_E_RABTOBEMODIFIEDITEMBEARERMODREQ_STRUCT         buffer[32]; //WARNING: Artificial limit to reduce memory footprint
}LIBLTE_S1AP_E_RABTOBEMODIFIEDLISTBEARERMODREQ_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_e_rabtobemodifiedlistbearermodreq(
  LIBLTE_S1AP_E_RABTOBEMODIFIEDLISTBEARERMODREQ_STRUCT         *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_e_rabtobemodifiedlistbearermodreq(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_E_RABTOBEMODIFIEDLISTBEARERMODREQ_STRUCT         *ie);

/*******************************************************************************
/* Protocol Container List E_RABModifyListBearerModRes DYNAMIC SEQUENCE OF
********************************************************************************/
// lb:1, ub:256
typedef struct{
  uint32_t                                                     len;
  LIBLTE_S1AP_E_RABMODIFYITEMBEARERMODRES_STRUCT               buffer[32]; //WARNING: Artificial limit to reduce memory footprint
}LIBLTE_S1AP_E_RABMODIFYLISTBEARERMODRES_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_e_rabmodifylistbearermodres(
  LIBLTE_S1AP_E_RABMODIFYLISTBEARERMODRES_STRUCT               *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_e_rabmodifylistbearermodres(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_E_RABMODIFYLISTBEARERMODRES_STRUCT               *ie);

/*******************************************************************************
/* Protocol Container List E_RABReleaseListBearerRelComp DYNAMIC SEQUENCE OF
********************************************************************************/
// lb:1, ub:256
typedef struct{
  uint32_t                                                     len;
  LIBLTE_S1AP_E_RABRELEASEITEMBEARERRELCOMP_STRUCT             buffer[32]; //WARNING: Artificial limit to reduce memory footprint
}LIBLTE_S1AP_E_RABRELEASELISTBEARERRELCOMP_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_e_rabreleaselistbearerrelcomp(
  LIBLTE_S1AP_E_RABRELEASELISTBEARERRELCOMP_STRUCT             *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_e_rabreleaselistbearerrelcomp(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_E_RABRELEASELISTBEARERRELCOMP_STRUCT             *ie);

/*******************************************************************************
/* Protocol Container List E_RABToBeSetupListCtxtSUReq DYNAMIC SEQUENCE OF
********************************************************************************/
// lb:1, ub:256
typedef struct{
  uint32_t                                                     len;
  LIBLTE_S1AP_E_RABTOBESETUPITEMCTXTSUREQ_STRUCT               buffer[32]; //WARNING: Artificial limit to reduce memory footprint
}LIBLTE_S1AP_E_RABTOBESETUPLISTCTXTSUREQ_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_e_rabtobesetuplistctxtsureq(
  LIBLTE_S1AP_E_RABTOBESETUPLISTCTXTSUREQ_STRUCT               *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_e_rabtobesetuplistctxtsureq(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_E_RABTOBESETUPLISTCTXTSUREQ_STRUCT               *ie);

/*******************************************************************************
/* Protocol Container List E_RABSetupListCtxtSURes DYNAMIC SEQUENCE OF
********************************************************************************/
// lb:1, ub:256
typedef struct{
  uint32_t                                                     len;
  LIBLTE_S1AP_E_RABSETUPITEMCTXTSURES_STRUCT                   buffer[32]; //WARNING: Artificial limit to reduce memory footprint
}LIBLTE_S1AP_E_RABSETUPLISTCTXTSURES_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_e_rabsetuplistctxtsures(
  LIBLTE_S1AP_E_RABSETUPLISTCTXTSURES_STRUCT                   *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_e_rabsetuplistctxtsures(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_E_RABSETUPLISTCTXTSURES_STRUCT                   *ie);

/*******************************************************************************
/* Protocol Container List TAIList DYNAMIC SEQUENCE OF
********************************************************************************/
// lb:1, ub:256
typedef struct{
  uint32_t                                                     len;
  LIBLTE_S1AP_TAIITEM_STRUCT                                   buffer[32]; //WARNING: Artificial limit to reduce memory footprint
}LIBLTE_S1AP_TAILIST_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_tailist(
  LIBLTE_S1AP_TAILIST_STRUCT                                   *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_tailist(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_TAILIST_STRUCT                                   *ie);

/*******************************************************************************
/* Protocol Container List E_RABFailedtoSetupListHOReqAck DYNAMIC SEQUENCE OF
********************************************************************************/
// lb:1, ub:256
typedef struct{
  uint32_t                                                     len;
  LIBLTE_S1AP_E_RABFAILEDTOSETUPITEMHOREQACK_STRUCT            buffer[32]; //WARNING: Artificial limit to reduce memory footprint
}LIBLTE_S1AP_E_RABFAILEDTOSETUPLISTHOREQACK_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_e_rabfailedtosetuplisthoreqack(
  LIBLTE_S1AP_E_RABFAILEDTOSETUPLISTHOREQACK_STRUCT            *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_e_rabfailedtosetuplisthoreqack(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_E_RABFAILEDTOSETUPLISTHOREQACK_STRUCT            *ie);

/*******************************************************************************
/* Protocol Message AllocationAndRetentionPriority_Ext STRUCT
********************************************************************************/
typedef struct{
  bool                                                         ext;
}LIBLTE_S1AP_MESSAGE_ALLOCATIONANDRETENTIONPRIORITY_EXT_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_allocationandretentionpriority_ext(
  LIBLTE_S1AP_MESSAGE_ALLOCATIONANDRETENTIONPRIORITY_EXT_STRUCT *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_allocationandretentionpriority_ext(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MESSAGE_ALLOCATIONANDRETENTIONPRIORITY_EXT_STRUCT *ie);

/*******************************************************************************
/* Protocol Message CancelledCellinEAI_Item_Ext STRUCT
********************************************************************************/
typedef struct{
  bool                                                         ext;
}LIBLTE_S1AP_MESSAGE_CANCELLEDCELLINEAI_ITEM_EXT_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_cancelledcellineai_item_ext(
  LIBLTE_S1AP_MESSAGE_CANCELLEDCELLINEAI_ITEM_EXT_STRUCT       *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_cancelledcellineai_item_ext(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MESSAGE_CANCELLEDCELLINEAI_ITEM_EXT_STRUCT       *ie);

/*******************************************************************************
/* Protocol Message CancelledCellinTAI_Item_Ext STRUCT
********************************************************************************/
typedef struct{
  bool                                                         ext;
}LIBLTE_S1AP_MESSAGE_CANCELLEDCELLINTAI_ITEM_EXT_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_cancelledcellintai_item_ext(
  LIBLTE_S1AP_MESSAGE_CANCELLEDCELLINTAI_ITEM_EXT_STRUCT       *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_cancelledcellintai_item_ext(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MESSAGE_CANCELLEDCELLINTAI_ITEM_EXT_STRUCT       *ie);

/*******************************************************************************
/* Protocol Message CellID_Broadcast_Item_Ext STRUCT
********************************************************************************/
typedef struct{
  bool                                                         ext;
}LIBLTE_S1AP_MESSAGE_CELLID_BROADCAST_ITEM_EXT_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_cellid_broadcast_item_ext(
  LIBLTE_S1AP_MESSAGE_CELLID_BROADCAST_ITEM_EXT_STRUCT         *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_cellid_broadcast_item_ext(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MESSAGE_CELLID_BROADCAST_ITEM_EXT_STRUCT         *ie);

/*******************************************************************************
/* Protocol Message CellID_Cancelled_Item_Ext STRUCT
********************************************************************************/
typedef struct{
  bool                                                         ext;
}LIBLTE_S1AP_MESSAGE_CELLID_CANCELLED_ITEM_EXT_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_cellid_cancelled_item_ext(
  LIBLTE_S1AP_MESSAGE_CELLID_CANCELLED_ITEM_EXT_STRUCT         *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_cellid_cancelled_item_ext(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MESSAGE_CELLID_CANCELLED_ITEM_EXT_STRUCT         *ie);

/*******************************************************************************
/* Protocol Message CellBasedMDT_Ext STRUCT
********************************************************************************/
typedef struct{
  bool                                                         ext;
}LIBLTE_S1AP_MESSAGE_CELLBASEDMDT_EXT_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_cellbasedmdt_ext(
  LIBLTE_S1AP_MESSAGE_CELLBASEDMDT_EXT_STRUCT                  *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_cellbasedmdt_ext(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MESSAGE_CELLBASEDMDT_EXT_STRUCT                  *ie);

/*******************************************************************************
/* Protocol Message Cdma2000OneXSRVCCInfo_Ext STRUCT
********************************************************************************/
typedef struct{
  bool                                                         ext;
}LIBLTE_S1AP_MESSAGE_CDMA2000ONEXSRVCCINFO_EXT_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_cdma2000onexsrvccinfo_ext(
  LIBLTE_S1AP_MESSAGE_CDMA2000ONEXSRVCCINFO_EXT_STRUCT         *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_cdma2000onexsrvccinfo_ext(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MESSAGE_CDMA2000ONEXSRVCCINFO_EXT_STRUCT         *ie);

/*******************************************************************************
/* Protocol Message CellType_Ext STRUCT
********************************************************************************/
typedef struct{
  bool                                                         ext;
}LIBLTE_S1AP_MESSAGE_CELLTYPE_EXT_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_celltype_ext(
  LIBLTE_S1AP_MESSAGE_CELLTYPE_EXT_STRUCT                      *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_celltype_ext(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MESSAGE_CELLTYPE_EXT_STRUCT                      *ie);

/*******************************************************************************
/* Protocol Message CGI_Ext STRUCT
********************************************************************************/
typedef struct{
  bool                                                         ext;
}LIBLTE_S1AP_MESSAGE_CGI_EXT_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_cgi_ext(
  LIBLTE_S1AP_MESSAGE_CGI_EXT_STRUCT                           *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_cgi_ext(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MESSAGE_CGI_EXT_STRUCT                           *ie);

/*******************************************************************************
/* Protocol Message CSG_IdList_Item_Ext STRUCT
********************************************************************************/
typedef struct{
  bool                                                         ext;
}LIBLTE_S1AP_MESSAGE_CSG_IDLIST_ITEM_EXT_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_csg_idlist_item_ext(
  LIBLTE_S1AP_MESSAGE_CSG_IDLIST_ITEM_EXT_STRUCT               *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_csg_idlist_item_ext(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MESSAGE_CSG_IDLIST_ITEM_EXT_STRUCT               *ie);

/*******************************************************************************
/* Protocol Message COUNTvalue_Ext STRUCT
********************************************************************************/
typedef struct{
  bool                                                         ext;
}LIBLTE_S1AP_MESSAGE_COUNTVALUE_EXT_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_countvalue_ext(
  LIBLTE_S1AP_MESSAGE_COUNTVALUE_EXT_STRUCT                    *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_countvalue_ext(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MESSAGE_COUNTVALUE_EXT_STRUCT                    *ie);

/*******************************************************************************
/* Protocol Message COUNTValueExtended_Ext STRUCT
********************************************************************************/
typedef struct{
  bool                                                         ext;
}LIBLTE_S1AP_MESSAGE_COUNTVALUEEXTENDED_EXT_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_countvalueextended_ext(
  LIBLTE_S1AP_MESSAGE_COUNTVALUEEXTENDED_EXT_STRUCT            *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_countvalueextended_ext(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MESSAGE_COUNTVALUEEXTENDED_EXT_STRUCT            *ie);

/*******************************************************************************
/* Protocol Message CriticalityDiagnostics_Ext STRUCT
********************************************************************************/
typedef struct{
  bool                                                         ext;
}LIBLTE_S1AP_MESSAGE_CRITICALITYDIAGNOSTICS_EXT_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_criticalitydiagnostics_ext(
  LIBLTE_S1AP_MESSAGE_CRITICALITYDIAGNOSTICS_EXT_STRUCT        *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_criticalitydiagnostics_ext(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MESSAGE_CRITICALITYDIAGNOSTICS_EXT_STRUCT        *ie);

/*******************************************************************************
/* Protocol Message CriticalityDiagnostics_IE_Item_Ext STRUCT
********************************************************************************/
typedef struct{
  bool                                                         ext;
}LIBLTE_S1AP_MESSAGE_CRITICALITYDIAGNOSTICS_IE_ITEM_EXT_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_criticalitydiagnostics_ie_item_ext(
  LIBLTE_S1AP_MESSAGE_CRITICALITYDIAGNOSTICS_IE_ITEM_EXT_STRUCT *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_criticalitydiagnostics_ie_item_ext(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MESSAGE_CRITICALITYDIAGNOSTICS_IE_ITEM_EXT_STRUCT *ie);

/*******************************************************************************
/* Protocol Message EmergencyAreaID_Broadcast_Item_Ext STRUCT
********************************************************************************/
typedef struct{
  bool                                                         ext;
}LIBLTE_S1AP_MESSAGE_EMERGENCYAREAID_BROADCAST_ITEM_EXT_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_emergencyareaid_broadcast_item_ext(
  LIBLTE_S1AP_MESSAGE_EMERGENCYAREAID_BROADCAST_ITEM_EXT_STRUCT *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_emergencyareaid_broadcast_item_ext(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MESSAGE_EMERGENCYAREAID_BROADCAST_ITEM_EXT_STRUCT *ie);

/*******************************************************************************
/* Protocol Message EmergencyAreaID_Cancelled_Item_Ext STRUCT
********************************************************************************/
typedef struct{
  bool                                                         ext;
}LIBLTE_S1AP_MESSAGE_EMERGENCYAREAID_CANCELLED_ITEM_EXT_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_emergencyareaid_cancelled_item_ext(
  LIBLTE_S1AP_MESSAGE_EMERGENCYAREAID_CANCELLED_ITEM_EXT_STRUCT *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_emergencyareaid_cancelled_item_ext(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MESSAGE_EMERGENCYAREAID_CANCELLED_ITEM_EXT_STRUCT *ie);

/*******************************************************************************
/* Protocol Message CompletedCellinEAI_Item_Ext STRUCT
********************************************************************************/
typedef struct{
  bool                                                         ext;
}LIBLTE_S1AP_MESSAGE_COMPLETEDCELLINEAI_ITEM_EXT_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_completedcellineai_item_ext(
  LIBLTE_S1AP_MESSAGE_COMPLETEDCELLINEAI_ITEM_EXT_STRUCT       *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_completedcellineai_item_ext(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MESSAGE_COMPLETEDCELLINEAI_ITEM_EXT_STRUCT       *ie);

/*******************************************************************************
/* Protocol Message GERAN_Cell_ID_Ext STRUCT
********************************************************************************/
typedef struct{
  bool                                                         ext;
}LIBLTE_S1AP_MESSAGE_GERAN_CELL_ID_EXT_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_geran_cell_id_ext(
  LIBLTE_S1AP_MESSAGE_GERAN_CELL_ID_EXT_STRUCT                 *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_geran_cell_id_ext(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MESSAGE_GERAN_CELL_ID_EXT_STRUCT                 *ie);

/*******************************************************************************
/* Protocol Message GlobalENB_ID_Ext STRUCT
********************************************************************************/
typedef struct{
  bool                                                         ext;
}LIBLTE_S1AP_MESSAGE_GLOBALENB_ID_EXT_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_globalenb_id_ext(
  LIBLTE_S1AP_MESSAGE_GLOBALENB_ID_EXT_STRUCT                  *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_globalenb_id_ext(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MESSAGE_GLOBALENB_ID_EXT_STRUCT                  *ie);

/*******************************************************************************
/* Protocol Message ENB_StatusTransfer_TransparentContainer_Ext STRUCT
********************************************************************************/
typedef struct{
  bool                                                         ext;
}LIBLTE_S1AP_MESSAGE_ENB_STATUSTRANSFER_TRANSPARENTCONTAINER_EXT_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_enb_statustransfer_transparentcontainer_ext(
  LIBLTE_S1AP_MESSAGE_ENB_STATUSTRANSFER_TRANSPARENTCONTAINER_EXT_STRUCT *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_enb_statustransfer_transparentcontainer_ext(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MESSAGE_ENB_STATUSTRANSFER_TRANSPARENTCONTAINER_EXT_STRUCT *ie);

/*******************************************************************************
/* Protocol Message E_RABInformationListItem_Ext STRUCT
********************************************************************************/
typedef struct{
  bool                                                         ext;
}LIBLTE_S1AP_MESSAGE_E_RABINFORMATIONLISTITEM_EXT_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_e_rabinformationlistitem_ext(
  LIBLTE_S1AP_MESSAGE_E_RABINFORMATIONLISTITEM_EXT_STRUCT      *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_e_rabinformationlistitem_ext(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MESSAGE_E_RABINFORMATIONLISTITEM_EXT_STRUCT      *ie);

/*******************************************************************************
/* Protocol Message E_RABItem_Ext STRUCT
********************************************************************************/
typedef struct{
  bool                                                         ext;
}LIBLTE_S1AP_MESSAGE_E_RABITEM_EXT_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_e_rabitem_ext(
  LIBLTE_S1AP_MESSAGE_E_RABITEM_EXT_STRUCT                     *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_e_rabitem_ext(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MESSAGE_E_RABITEM_EXT_STRUCT                     *ie);

/*******************************************************************************
/* Protocol Message E_RABQoSParameters_Ext STRUCT
********************************************************************************/
typedef struct{
  bool                                                         ext;
}LIBLTE_S1AP_MESSAGE_E_RABQOSPARAMETERS_EXT_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_e_rabqosparameters_ext(
  LIBLTE_S1AP_MESSAGE_E_RABQOSPARAMETERS_EXT_STRUCT            *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_e_rabqosparameters_ext(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MESSAGE_E_RABQOSPARAMETERS_EXT_STRUCT            *ie);

/*******************************************************************************
/* Protocol Message EUTRAN_CGI_Ext STRUCT
********************************************************************************/
typedef struct{
  bool                                                         ext;
}LIBLTE_S1AP_MESSAGE_EUTRAN_CGI_EXT_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_eutran_cgi_ext(
  LIBLTE_S1AP_MESSAGE_EUTRAN_CGI_EXT_STRUCT                    *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_eutran_cgi_ext(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MESSAGE_EUTRAN_CGI_EXT_STRUCT                    *ie);

/*******************************************************************************
/* Protocol Message ForbiddenTAs_Item_Ext STRUCT
********************************************************************************/
typedef struct{
  bool                                                         ext;
}LIBLTE_S1AP_MESSAGE_FORBIDDENTAS_ITEM_EXT_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_forbiddentas_item_ext(
  LIBLTE_S1AP_MESSAGE_FORBIDDENTAS_ITEM_EXT_STRUCT             *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_forbiddentas_item_ext(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MESSAGE_FORBIDDENTAS_ITEM_EXT_STRUCT             *ie);

/*******************************************************************************
/* Protocol Message ForbiddenLAs_Item_Ext STRUCT
********************************************************************************/
typedef struct{
  bool                                                         ext;
}LIBLTE_S1AP_MESSAGE_FORBIDDENLAS_ITEM_EXT_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_forbiddenlas_item_ext(
  LIBLTE_S1AP_MESSAGE_FORBIDDENLAS_ITEM_EXT_STRUCT             *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_forbiddenlas_item_ext(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MESSAGE_FORBIDDENLAS_ITEM_EXT_STRUCT             *ie);

/*******************************************************************************
/* Protocol Message GBR_QosInformation_Ext STRUCT
********************************************************************************/
typedef struct{
  bool                                                         ext;
}LIBLTE_S1AP_MESSAGE_GBR_QOSINFORMATION_EXT_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_gbr_qosinformation_ext(
  LIBLTE_S1AP_MESSAGE_GBR_QOSINFORMATION_EXT_STRUCT            *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_gbr_qosinformation_ext(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MESSAGE_GBR_QOSINFORMATION_EXT_STRUCT            *ie);

/*******************************************************************************
/* Protocol Message GUMMEI_Ext STRUCT
********************************************************************************/
typedef struct{
  bool                                                         ext;
}LIBLTE_S1AP_MESSAGE_GUMMEI_EXT_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_gummei_ext(
  LIBLTE_S1AP_MESSAGE_GUMMEI_EXT_STRUCT                        *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_gummei_ext(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MESSAGE_GUMMEI_EXT_STRUCT                        *ie);

/*******************************************************************************
/* Protocol Message HandoverRestrictionList_Ext STRUCT
********************************************************************************/
typedef struct{
  bool                                                         ext;
}LIBLTE_S1AP_MESSAGE_HANDOVERRESTRICTIONLIST_EXT_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_handoverrestrictionlist_ext(
  LIBLTE_S1AP_MESSAGE_HANDOVERRESTRICTIONLIST_EXT_STRUCT       *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_handoverrestrictionlist_ext(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MESSAGE_HANDOVERRESTRICTIONLIST_EXT_STRUCT       *ie);

/*******************************************************************************
/* Protocol Message LAI_Ext STRUCT
********************************************************************************/
typedef struct{
  bool                                                         ext;
}LIBLTE_S1AP_MESSAGE_LAI_EXT_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_lai_ext(
  LIBLTE_S1AP_MESSAGE_LAI_EXT_STRUCT                           *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_lai_ext(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MESSAGE_LAI_EXT_STRUCT                           *ie);

/*******************************************************************************
/* Protocol Message LoggedMDT_Ext STRUCT
********************************************************************************/
typedef struct{
  bool                                                         ext;
}LIBLTE_S1AP_MESSAGE_LOGGEDMDT_EXT_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_loggedmdt_ext(
  LIBLTE_S1AP_MESSAGE_LOGGEDMDT_EXT_STRUCT                     *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_loggedmdt_ext(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MESSAGE_LOGGEDMDT_EXT_STRUCT                     *ie);

/*******************************************************************************
/* Protocol Message M3Configuration_Ext STRUCT
********************************************************************************/
typedef struct{
  bool                                                         ext;
}LIBLTE_S1AP_MESSAGE_M3CONFIGURATION_EXT_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_m3configuration_ext(
  LIBLTE_S1AP_MESSAGE_M3CONFIGURATION_EXT_STRUCT               *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_m3configuration_ext(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MESSAGE_M3CONFIGURATION_EXT_STRUCT               *ie);

/*******************************************************************************
/* Protocol Message M4Configuration_Ext STRUCT
********************************************************************************/
typedef struct{
  bool                                                         ext;
}LIBLTE_S1AP_MESSAGE_M4CONFIGURATION_EXT_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_m4configuration_ext(
  LIBLTE_S1AP_MESSAGE_M4CONFIGURATION_EXT_STRUCT               *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_m4configuration_ext(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MESSAGE_M4CONFIGURATION_EXT_STRUCT               *ie);

/*******************************************************************************
/* Protocol Message M5Configuration_Ext STRUCT
********************************************************************************/
typedef struct{
  bool                                                         ext;
}LIBLTE_S1AP_MESSAGE_M5CONFIGURATION_EXT_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_m5configuration_ext(
  LIBLTE_S1AP_MESSAGE_M5CONFIGURATION_EXT_STRUCT               *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_m5configuration_ext(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MESSAGE_M5CONFIGURATION_EXT_STRUCT               *ie);

/*******************************************************************************
/* Protocol Message M1PeriodicReporting_Ext STRUCT
********************************************************************************/
typedef struct{
  bool                                                         ext;
}LIBLTE_S1AP_MESSAGE_M1PERIODICREPORTING_EXT_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_m1periodicreporting_ext(
  LIBLTE_S1AP_MESSAGE_M1PERIODICREPORTING_EXT_STRUCT           *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_m1periodicreporting_ext(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MESSAGE_M1PERIODICREPORTING_EXT_STRUCT           *ie);

/*******************************************************************************
/* Protocol Message RequestType_Ext STRUCT
********************************************************************************/
typedef struct{
  bool                                                         ext;
}LIBLTE_S1AP_MESSAGE_REQUESTTYPE_EXT_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_requesttype_ext(
  LIBLTE_S1AP_MESSAGE_REQUESTTYPE_EXT_STRUCT                   *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_requesttype_ext(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MESSAGE_REQUESTTYPE_EXT_STRUCT                   *ie);

/*******************************************************************************
/* Protocol Message RIMTransfer_Ext STRUCT
********************************************************************************/
typedef struct{
  bool                                                         ext;
}LIBLTE_S1AP_MESSAGE_RIMTRANSFER_EXT_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_rimtransfer_ext(
  LIBLTE_S1AP_MESSAGE_RIMTRANSFER_EXT_STRUCT                   *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_rimtransfer_ext(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MESSAGE_RIMTRANSFER_EXT_STRUCT                   *ie);

/*******************************************************************************
/* Protocol Message SecurityContext_Ext STRUCT
********************************************************************************/
typedef struct{
  bool                                                         ext;
}LIBLTE_S1AP_MESSAGE_SECURITYCONTEXT_EXT_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_securitycontext_ext(
  LIBLTE_S1AP_MESSAGE_SECURITYCONTEXT_EXT_STRUCT               *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_securitycontext_ext(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MESSAGE_SECURITYCONTEXT_EXT_STRUCT               *ie);

/*******************************************************************************
/* Protocol Message SourceeNB_ID_Ext STRUCT
********************************************************************************/
typedef struct{
  bool                                                         ext;
}LIBLTE_S1AP_MESSAGE_SOURCEENB_ID_EXT_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_sourceenb_id_ext(
  LIBLTE_S1AP_MESSAGE_SOURCEENB_ID_EXT_STRUCT                  *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_sourceenb_id_ext(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MESSAGE_SOURCEENB_ID_EXT_STRUCT                  *ie);

/*******************************************************************************
/* Protocol Message ServedGUMMEIsItem_Ext STRUCT
********************************************************************************/
typedef struct{
  bool                                                         ext;
}LIBLTE_S1AP_MESSAGE_SERVEDGUMMEISITEM_EXT_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_servedgummeisitem_ext(
  LIBLTE_S1AP_MESSAGE_SERVEDGUMMEISITEM_EXT_STRUCT             *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_servedgummeisitem_ext(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MESSAGE_SERVEDGUMMEISITEM_EXT_STRUCT             *ie);

/*******************************************************************************
/* Protocol Message SupportedTAs_Item_Ext STRUCT
********************************************************************************/
typedef struct{
  bool                                                         ext;
}LIBLTE_S1AP_MESSAGE_SUPPORTEDTAS_ITEM_EXT_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_supportedtas_item_ext(
  LIBLTE_S1AP_MESSAGE_SUPPORTEDTAS_ITEM_EXT_STRUCT             *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_supportedtas_item_ext(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MESSAGE_SUPPORTEDTAS_ITEM_EXT_STRUCT             *ie);

/*******************************************************************************
/* Protocol Message TimeSynchronizationInfo_Ext STRUCT
********************************************************************************/
typedef struct{
  bool                                                         ext;
}LIBLTE_S1AP_MESSAGE_TIMESYNCHRONIZATIONINFO_EXT_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_timesynchronizationinfo_ext(
  LIBLTE_S1AP_MESSAGE_TIMESYNCHRONIZATIONINFO_EXT_STRUCT       *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_timesynchronizationinfo_ext(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MESSAGE_TIMESYNCHRONIZATIONINFO_EXT_STRUCT       *ie);

/*******************************************************************************
/* Protocol Message S_TMSI_Ext STRUCT
********************************************************************************/
typedef struct{
  bool                                                         ext;
}LIBLTE_S1AP_MESSAGE_S_TMSI_EXT_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_s_tmsi_ext(
  LIBLTE_S1AP_MESSAGE_S_TMSI_EXT_STRUCT                        *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_s_tmsi_ext(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MESSAGE_S_TMSI_EXT_STRUCT                        *ie);

/*******************************************************************************
/* Protocol Message TAIBasedMDT_Ext STRUCT
********************************************************************************/
typedef struct{
  bool                                                         ext;
}LIBLTE_S1AP_MESSAGE_TAIBASEDMDT_EXT_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_taibasedmdt_ext(
  LIBLTE_S1AP_MESSAGE_TAIBASEDMDT_EXT_STRUCT                   *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_taibasedmdt_ext(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MESSAGE_TAIBASEDMDT_EXT_STRUCT                   *ie);

/*******************************************************************************
/* Protocol Message TAI_Ext STRUCT
********************************************************************************/
typedef struct{
  bool                                                         ext;
}LIBLTE_S1AP_MESSAGE_TAI_EXT_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_tai_ext(
  LIBLTE_S1AP_MESSAGE_TAI_EXT_STRUCT                           *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_tai_ext(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MESSAGE_TAI_EXT_STRUCT                           *ie);

/*******************************************************************************
/* Protocol Message TAI_Broadcast_Item_Ext STRUCT
********************************************************************************/
typedef struct{
  bool                                                         ext;
}LIBLTE_S1AP_MESSAGE_TAI_BROADCAST_ITEM_EXT_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_tai_broadcast_item_ext(
  LIBLTE_S1AP_MESSAGE_TAI_BROADCAST_ITEM_EXT_STRUCT            *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_tai_broadcast_item_ext(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MESSAGE_TAI_BROADCAST_ITEM_EXT_STRUCT            *ie);

/*******************************************************************************
/* Protocol Message TAI_Cancelled_Item_Ext STRUCT
********************************************************************************/
typedef struct{
  bool                                                         ext;
}LIBLTE_S1AP_MESSAGE_TAI_CANCELLED_ITEM_EXT_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_tai_cancelled_item_ext(
  LIBLTE_S1AP_MESSAGE_TAI_CANCELLED_ITEM_EXT_STRUCT            *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_tai_cancelled_item_ext(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MESSAGE_TAI_CANCELLED_ITEM_EXT_STRUCT            *ie);

/*******************************************************************************
/* Protocol Message TABasedMDT_Ext STRUCT
********************************************************************************/
typedef struct{
  bool                                                         ext;
}LIBLTE_S1AP_MESSAGE_TABASEDMDT_EXT_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_tabasedmdt_ext(
  LIBLTE_S1AP_MESSAGE_TABASEDMDT_EXT_STRUCT                    *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_tabasedmdt_ext(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MESSAGE_TABASEDMDT_EXT_STRUCT                    *ie);

/*******************************************************************************
/* Protocol Message CompletedCellinTAI_Item_Ext STRUCT
********************************************************************************/
typedef struct{
  bool                                                         ext;
}LIBLTE_S1AP_MESSAGE_COMPLETEDCELLINTAI_ITEM_EXT_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_completedcellintai_item_ext(
  LIBLTE_S1AP_MESSAGE_COMPLETEDCELLINTAI_ITEM_EXT_STRUCT       *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_completedcellintai_item_ext(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MESSAGE_COMPLETEDCELLINTAI_ITEM_EXT_STRUCT       *ie);

/*******************************************************************************
/* Protocol Message TargeteNB_ID_Ext STRUCT
********************************************************************************/
typedef struct{
  bool                                                         ext;
}LIBLTE_S1AP_MESSAGE_TARGETENB_ID_EXT_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_targetenb_id_ext(
  LIBLTE_S1AP_MESSAGE_TARGETENB_ID_EXT_STRUCT                  *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_targetenb_id_ext(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MESSAGE_TARGETENB_ID_EXT_STRUCT                  *ie);

/*******************************************************************************
/* Protocol Message TargetRNC_ID_Ext STRUCT
********************************************************************************/
typedef struct{
  bool                                                         ext;
}LIBLTE_S1AP_MESSAGE_TARGETRNC_ID_EXT_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_targetrnc_id_ext(
  LIBLTE_S1AP_MESSAGE_TARGETRNC_ID_EXT_STRUCT                  *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_targetrnc_id_ext(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MESSAGE_TARGETRNC_ID_EXT_STRUCT                  *ie);

/*******************************************************************************
/* Protocol Message TargeteNB_ToSourceeNB_TransparentContainer_Ext STRUCT
********************************************************************************/
typedef struct{
  bool                                                         ext;
}LIBLTE_S1AP_MESSAGE_TARGETENB_TOSOURCEENB_TRANSPARENTCONTAINER_EXT_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_targetenb_tosourceenb_transparentcontainer_ext(
  LIBLTE_S1AP_MESSAGE_TARGETENB_TOSOURCEENB_TRANSPARENTCONTAINER_EXT_STRUCT *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_targetenb_tosourceenb_transparentcontainer_ext(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MESSAGE_TARGETENB_TOSOURCEENB_TRANSPARENTCONTAINER_EXT_STRUCT *ie);

/*******************************************************************************
/* Protocol Message M1ThresholdEventA2_Ext STRUCT
********************************************************************************/
typedef struct{
  bool                                                         ext;
}LIBLTE_S1AP_MESSAGE_M1THRESHOLDEVENTA2_EXT_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_m1thresholdeventa2_ext(
  LIBLTE_S1AP_MESSAGE_M1THRESHOLDEVENTA2_EXT_STRUCT            *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_m1thresholdeventa2_ext(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MESSAGE_M1THRESHOLDEVENTA2_EXT_STRUCT            *ie);

/*******************************************************************************
/* Protocol Message Tunnel_Information_Ext STRUCT
********************************************************************************/
typedef struct{
  bool                                                         ext;
}LIBLTE_S1AP_MESSAGE_TUNNEL_INFORMATION_EXT_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_tunnel_information_ext(
  LIBLTE_S1AP_MESSAGE_TUNNEL_INFORMATION_EXT_STRUCT            *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_tunnel_information_ext(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MESSAGE_TUNNEL_INFORMATION_EXT_STRUCT            *ie);

/*******************************************************************************
/* Protocol Message UEAggregate_MaximumBitrates_Ext STRUCT
********************************************************************************/
typedef struct{
  bool                                                         ext;
}LIBLTE_S1AP_MESSAGE_UEAGGREGATE_MAXIMUMBITRATES_EXT_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_ueaggregate_maximumbitrates_ext(
  LIBLTE_S1AP_MESSAGE_UEAGGREGATE_MAXIMUMBITRATES_EXT_STRUCT   *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_ueaggregate_maximumbitrates_ext(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MESSAGE_UEAGGREGATE_MAXIMUMBITRATES_EXT_STRUCT   *ie);

/*******************************************************************************
/* Protocol Message UE_S1AP_ID_pair_Ext STRUCT
********************************************************************************/
typedef struct{
  bool                                                         ext;
}LIBLTE_S1AP_MESSAGE_UE_S1AP_ID_PAIR_EXT_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_ue_s1ap_id_pair_ext(
  LIBLTE_S1AP_MESSAGE_UE_S1AP_ID_PAIR_EXT_STRUCT               *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_ue_s1ap_id_pair_ext(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MESSAGE_UE_S1AP_ID_PAIR_EXT_STRUCT               *ie);

/*******************************************************************************
/* Protocol Message UE_associatedLogicalS1_ConnectionItemExt STRUCT
********************************************************************************/
typedef struct{
  bool                                                         ext;
}LIBLTE_S1AP_MESSAGE_UE_ASSOCIATEDLOGICALS1_CONNECTIONITEMEXT_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_ue_associatedlogicals1_connectionitemext(
  LIBLTE_S1AP_MESSAGE_UE_ASSOCIATEDLOGICALS1_CONNECTIONITEMEXT_STRUCT *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_ue_associatedlogicals1_connectionitemext(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MESSAGE_UE_ASSOCIATEDLOGICALS1_CONNECTIONITEMEXT_STRUCT *ie);

/*******************************************************************************
/* Protocol Message UESecurityCapabilities_Ext STRUCT
********************************************************************************/
typedef struct{
  bool                                                         ext;
}LIBLTE_S1AP_MESSAGE_UESECURITYCAPABILITIES_EXT_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_uesecuritycapabilities_ext(
  LIBLTE_S1AP_MESSAGE_UESECURITYCAPABILITIES_EXT_STRUCT        *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_uesecuritycapabilities_ext(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MESSAGE_UESECURITYCAPABILITIES_EXT_STRUCT        *ie);

/*******************************************************************************
/* Protocol Message UserLocationInformation_Ext STRUCT
********************************************************************************/
typedef struct{
  bool                                                         ext;
}LIBLTE_S1AP_MESSAGE_USERLOCATIONINFORMATION_EXT_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_userlocationinformation_ext(
  LIBLTE_S1AP_MESSAGE_USERLOCATIONINFORMATION_EXT_STRUCT       *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_userlocationinformation_ext(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MESSAGE_USERLOCATIONINFORMATION_EXT_STRUCT       *ie);

/*******************************************************************************
/* Protocol Message ENBX2ExtTLA_Ext STRUCT
********************************************************************************/
typedef struct{
  bool                                                         ext;
}LIBLTE_S1AP_MESSAGE_ENBX2EXTTLA_EXT_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_enbx2exttla_ext(
  LIBLTE_S1AP_MESSAGE_ENBX2EXTTLA_EXT_STRUCT                   *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_enbx2exttla_ext(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MESSAGE_ENBX2EXTTLA_EXT_STRUCT                   *ie);

/*******************************************************************************
/* Protocol Message SourceeNB_ToTargeteNB_TransparentContainer_Ext STRUCT
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_MOBILITYINFORMATION_STRUCT                       MobilityInformation;
  bool                                                         MobilityInformation_present;
}LIBLTE_S1AP_MESSAGE_SOURCEENB_TOTARGETENB_TRANSPARENTCONTAINER_EXT_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_sourceenb_totargetenb_transparentcontainer_ext(
  LIBLTE_S1AP_MESSAGE_SOURCEENB_TOTARGETENB_TRANSPARENTCONTAINER_EXT_STRUCT *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_sourceenb_totargetenb_transparentcontainer_ext(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MESSAGE_SOURCEENB_TOTARGETENB_TRANSPARENTCONTAINER_EXT_STRUCT *ie);

/*******************************************************************************
/* Protocol Message E_RABInformationList STRUCT
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_E_RABINFORMATIONLISTITEM_STRUCT                  E_RABInformationListItem;
}LIBLTE_S1AP_MESSAGE_E_RABINFORMATIONLIST_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_e_rabinformationlist(
  LIBLTE_S1AP_MESSAGE_E_RABINFORMATIONLIST_STRUCT              *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_e_rabinformationlist(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MESSAGE_E_RABINFORMATIONLIST_STRUCT              *ie);

/*******************************************************************************
/* Protocol Message LastVisitedEUTRANCellInformation_Ext STRUCT
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_TIME_UE_STAYEDINCELL_ENHANCEDGRANULARITY_STRUCT  Time_UE_StayedInCell_EnhancedGranularity;
  bool                                                         Time_UE_StayedInCell_EnhancedGranularity_present;
  LIBLTE_S1AP_CAUSE_STRUCT                                     HO_Cause;
  bool                                                         HO_Cause_present;
}LIBLTE_S1AP_MESSAGE_LASTVISITEDEUTRANCELLINFORMATION_EXT_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_lastvisitedeutrancellinformation_ext(
  LIBLTE_S1AP_MESSAGE_LASTVISITEDEUTRANCELLINFORMATION_EXT_STRUCT *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_lastvisitedeutrancellinformation_ext(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MESSAGE_LASTVISITEDEUTRANCELLINFORMATION_EXT_STRUCT *ie);

/*******************************************************************************
/* Protocol Message SONInformationReply_Ext STRUCT
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_TIMESYNCHRONIZATIONINFO_STRUCT                   Time_Synchronization_Info;
  bool                                                         Time_Synchronization_Info_present;
}LIBLTE_S1AP_MESSAGE_SONINFORMATIONREPLY_EXT_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_soninformationreply_ext(
  LIBLTE_S1AP_MESSAGE_SONINFORMATIONREPLY_EXT_STRUCT           *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_soninformationreply_ext(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MESSAGE_SONINFORMATIONREPLY_EXT_STRUCT           *ie);

/*******************************************************************************
/* Protocol Message Bearers_SubjectToStatusTransfer_ItemExt STRUCT
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_COUNTVALUEEXTENDED_STRUCT                        ULCOUNTValueExtended;
  bool                                                         ULCOUNTValueExtended_present;
  LIBLTE_S1AP_COUNTVALUEEXTENDED_STRUCT                        DLCOUNTValueExtended;
  bool                                                         DLCOUNTValueExtended_present;
  LIBLTE_S1AP_RECEIVESTATUSOFULPDCPSDUSEXTENDED_STRUCT         ReceiveStatusOfULPDCPSDUsExtended;
  bool                                                         ReceiveStatusOfULPDCPSDUsExtended_present;
}LIBLTE_S1AP_MESSAGE_BEARERS_SUBJECTTOSTATUSTRANSFER_ITEMEXT_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_bearers_subjecttostatustransfer_itemext(
  LIBLTE_S1AP_MESSAGE_BEARERS_SUBJECTTOSTATUSTRANSFER_ITEMEXT_STRUCT *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_bearers_subjecttostatustransfer_itemext(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MESSAGE_BEARERS_SUBJECTTOSTATUSTRANSFER_ITEMEXT_STRUCT *ie);

/*******************************************************************************
/* Protocol Message E_RABItem STRUCT
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_E_RABITEM_STRUCT                                 E_RABItem;
}LIBLTE_S1AP_MESSAGE_E_RABITEM_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_e_rabitem(
  LIBLTE_S1AP_MESSAGE_E_RABITEM_STRUCT                         *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_e_rabitem(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MESSAGE_E_RABITEM_STRUCT                         *ie);

/*******************************************************************************
/* Protocol Message MDT_Configuration_Ext STRUCT
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_MDTPLMNLIST_STRUCT                               SignallingBasedMDTPLMNList;
  bool                                                         SignallingBasedMDTPLMNList_present;
}LIBLTE_S1AP_MESSAGE_MDT_CONFIGURATION_EXT_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_mdt_configuration_ext(
  LIBLTE_S1AP_MESSAGE_MDT_CONFIGURATION_EXT_STRUCT             *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_mdt_configuration_ext(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MESSAGE_MDT_CONFIGURATION_EXT_STRUCT             *ie);

/*******************************************************************************
/* Protocol Message X2TNLConfigurationInfo_Ext STRUCT
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_ENBX2EXTTLAS_STRUCT                              eNBX2ExtendedTransportLayerAddresses;
  bool                                                         eNBX2ExtendedTransportLayerAddresses_present;
}LIBLTE_S1AP_MESSAGE_X2TNLCONFIGURATIONINFO_EXT_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_x2tnlconfigurationinfo_ext(
  LIBLTE_S1AP_MESSAGE_X2TNLCONFIGURATIONINFO_EXT_STRUCT        *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_x2tnlconfigurationinfo_ext(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MESSAGE_X2TNLCONFIGURATIONINFO_EXT_STRUCT        *ie);

/*******************************************************************************
/* Protocol Message Bearers_SubjectToStatusTransfer_Item STRUCT
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_BEARERS_SUBJECTTOSTATUSTRANSFER_ITEM_STRUCT      Bearers_SubjectToStatusTransfer_Item;
}LIBLTE_S1AP_MESSAGE_BEARERS_SUBJECTTOSTATUSTRANSFER_ITEM_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_bearers_subjecttostatustransfer_item(
  LIBLTE_S1AP_MESSAGE_BEARERS_SUBJECTTOSTATUSTRANSFER_ITEM_STRUCT *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_bearers_subjecttostatustransfer_item(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MESSAGE_BEARERS_SUBJECTTOSTATUSTRANSFER_ITEM_STRUCT *ie);

/*******************************************************************************
/* Protocol Message ImmediateMDT_Ext STRUCT
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_M3CONFIGURATION_STRUCT                           M3Configuration;
  bool                                                         M3Configuration_present;
  LIBLTE_S1AP_M4CONFIGURATION_STRUCT                           M4Configuration;
  bool                                                         M4Configuration_present;
  LIBLTE_S1AP_M5CONFIGURATION_STRUCT                           M5Configuration;
  bool                                                         M5Configuration_present;
  LIBLTE_S1AP_MDT_LOCATION_INFO_STRUCT                         MDT_Location_Info;
  bool                                                         MDT_Location_Info_present;
}LIBLTE_S1AP_MESSAGE_IMMEDIATEMDT_EXT_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_immediatemdt_ext(
  LIBLTE_S1AP_MESSAGE_IMMEDIATEMDT_EXT_STRUCT                  *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_immediatemdt_ext(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MESSAGE_IMMEDIATEMDT_EXT_STRUCT                  *ie);

/*******************************************************************************
/* Protocol Message SONConfigurationTransfer_Ext STRUCT
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_X2TNLCONFIGURATIONINFO_STRUCT                    x2TNLConfigurationInfo;
  bool                                                         x2TNLConfigurationInfo_present;
}LIBLTE_S1AP_MESSAGE_SONCONFIGURATIONTRANSFER_EXT_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_sonconfigurationtransfer_ext(
  LIBLTE_S1AP_MESSAGE_SONCONFIGURATIONTRANSFER_EXT_STRUCT      *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_sonconfigurationtransfer_ext(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MESSAGE_SONCONFIGURATIONTRANSFER_EXT_STRUCT      *ie);

/*******************************************************************************
/* Protocol Message TraceActivation_Ext STRUCT
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_MDT_CONFIGURATION_STRUCT                         MDTConfiguration;
  bool                                                         MDTConfiguration_present;
}LIBLTE_S1AP_MESSAGE_TRACEACTIVATION_EXT_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_traceactivation_ext(
  LIBLTE_S1AP_MESSAGE_TRACEACTIVATION_EXT_STRUCT               *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_traceactivation_ext(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MESSAGE_TRACEACTIVATION_EXT_STRUCT               *ie);

/*******************************************************************************
/* Protocol Message HandoverRequired STRUCT
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_MME_UE_S1AP_ID_STRUCT                            MME_UE_S1AP_ID;
  LIBLTE_S1AP_ENB_UE_S1AP_ID_STRUCT                            eNB_UE_S1AP_ID;
  LIBLTE_S1AP_HANDOVERTYPE_ENUM_EXT                            HandoverType;
  LIBLTE_S1AP_CAUSE_STRUCT                                     Cause;
  LIBLTE_S1AP_TARGETID_STRUCT                                  TargetID;
  LIBLTE_S1AP_DIRECT_FORWARDING_PATH_AVAILABILITY_ENUM_EXT     Direct_Forwarding_Path_Availability;
  bool                                                         Direct_Forwarding_Path_Availability_present;
  LIBLTE_S1AP_SRVCCHOINDICATION_ENUM_EXT                       SRVCCHOIndication;
  bool                                                         SRVCCHOIndication_present;
  LIBLTE_S1AP_SOURCE_TOTARGET_TRANSPARENTCONTAINER_STRUCT      Source_ToTarget_TransparentContainer;
  LIBLTE_S1AP_SOURCE_TOTARGET_TRANSPARENTCONTAINER_STRUCT      Source_ToTarget_TransparentContainer_Secondary;
  bool                                                         Source_ToTarget_TransparentContainer_Secondary_present;
  LIBLTE_S1AP_MSCLASSMARK2_STRUCT                              MSClassmark2;
  bool                                                         MSClassmark2_present;
  LIBLTE_S1AP_MSCLASSMARK3_STRUCT                              MSClassmark3;
  bool                                                         MSClassmark3_present;
  LIBLTE_S1AP_CSG_ID_STRUCT                                    CSG_Id;
  bool                                                         CSG_Id_present;
  LIBLTE_S1AP_CELLACCESSMODE_ENUM_EXT                          CellAccessMode;
  bool                                                         CellAccessMode_present;
  LIBLTE_S1AP_PS_SERVICENOTAVAILABLE_ENUM_EXT                  PS_ServiceNotAvailable;
  bool                                                         PS_ServiceNotAvailable_present;
}LIBLTE_S1AP_MESSAGE_HANDOVERREQUIRED_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_handoverrequired(
  LIBLTE_S1AP_MESSAGE_HANDOVERREQUIRED_STRUCT                  *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_handoverrequired(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MESSAGE_HANDOVERREQUIRED_STRUCT                  *ie);

/*******************************************************************************
/* Protocol Message E_RABDataForwardingItem_Ext STRUCT
********************************************************************************/
typedef struct{
  bool                                                         ext;
}LIBLTE_S1AP_MESSAGE_E_RABDATAFORWARDINGITEM_EXT_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_e_rabdataforwardingitem_ext(
  LIBLTE_S1AP_MESSAGE_E_RABDATAFORWARDINGITEM_EXT_STRUCT       *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_e_rabdataforwardingitem_ext(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MESSAGE_E_RABDATAFORWARDINGITEM_EXT_STRUCT       *ie);

/*******************************************************************************
/* Protocol Message HandoverPreparationFailure STRUCT
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_MME_UE_S1AP_ID_STRUCT                            MME_UE_S1AP_ID;
  LIBLTE_S1AP_ENB_UE_S1AP_ID_STRUCT                            eNB_UE_S1AP_ID;
  LIBLTE_S1AP_CAUSE_STRUCT                                     Cause;
  LIBLTE_S1AP_CRITICALITYDIAGNOSTICS_STRUCT                    CriticalityDiagnostics;
  bool                                                         CriticalityDiagnostics_present;
}LIBLTE_S1AP_MESSAGE_HANDOVERPREPARATIONFAILURE_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_handoverpreparationfailure(
  LIBLTE_S1AP_MESSAGE_HANDOVERPREPARATIONFAILURE_STRUCT        *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_handoverpreparationfailure(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MESSAGE_HANDOVERPREPARATIONFAILURE_STRUCT        *ie);

/*******************************************************************************
/* Protocol Message E_RABToBeSetupItemHOReq_Ext STRUCT
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_DATA_FORWARDING_NOT_POSSIBLE_ENUM_EXT            Data_Forwarding_Not_Possible;
  bool                                                         Data_Forwarding_Not_Possible_present;
}LIBLTE_S1AP_MESSAGE_E_RABTOBESETUPITEMHOREQ_EXT_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_e_rabtobesetupitemhoreq_ext(
  LIBLTE_S1AP_MESSAGE_E_RABTOBESETUPITEMHOREQ_EXT_STRUCT       *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_e_rabtobesetupitemhoreq_ext(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MESSAGE_E_RABTOBESETUPITEMHOREQ_EXT_STRUCT       *ie);

/*******************************************************************************
/* Protocol Message E_RABAdmittedItem_Ext STRUCT
********************************************************************************/
typedef struct{
  bool                                                         ext;
}LIBLTE_S1AP_MESSAGE_E_RABADMITTEDITEM_EXT_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_e_rabadmitteditem_ext(
  LIBLTE_S1AP_MESSAGE_E_RABADMITTEDITEM_EXT_STRUCT             *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_e_rabadmitteditem_ext(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MESSAGE_E_RABADMITTEDITEM_EXT_STRUCT             *ie);

/*******************************************************************************
/* Protocol Message E_RABFailedToSetupItemHOReqAckExt STRUCT
********************************************************************************/
typedef struct{
  bool                                                         ext;
}LIBLTE_S1AP_MESSAGE_E_RABFAILEDTOSETUPITEMHOREQACKEXT_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_e_rabfailedtosetupitemhoreqackext(
  LIBLTE_S1AP_MESSAGE_E_RABFAILEDTOSETUPITEMHOREQACKEXT_STRUCT *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_e_rabfailedtosetupitemhoreqackext(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MESSAGE_E_RABFAILEDTOSETUPITEMHOREQACKEXT_STRUCT *ie);

/*******************************************************************************
/* Protocol Message HandoverFailure STRUCT
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_MME_UE_S1AP_ID_STRUCT                            MME_UE_S1AP_ID;
  LIBLTE_S1AP_CAUSE_STRUCT                                     Cause;
  LIBLTE_S1AP_CRITICALITYDIAGNOSTICS_STRUCT                    CriticalityDiagnostics;
  bool                                                         CriticalityDiagnostics_present;
}LIBLTE_S1AP_MESSAGE_HANDOVERFAILURE_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_handoverfailure(
  LIBLTE_S1AP_MESSAGE_HANDOVERFAILURE_STRUCT                   *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_handoverfailure(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MESSAGE_HANDOVERFAILURE_STRUCT                   *ie);

/*******************************************************************************
/* Protocol Message HandoverNotify STRUCT
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_MME_UE_S1AP_ID_STRUCT                            MME_UE_S1AP_ID;
  LIBLTE_S1AP_ENB_UE_S1AP_ID_STRUCT                            eNB_UE_S1AP_ID;
  LIBLTE_S1AP_EUTRAN_CGI_STRUCT                                EUTRAN_CGI;
  LIBLTE_S1AP_TAI_STRUCT                                       TAI;
  LIBLTE_S1AP_TUNNELINFORMATION_STRUCT                         Tunnel_Information_for_BBF;
  bool                                                         Tunnel_Information_for_BBF_present;
  LIBLTE_S1AP_LHN_ID_STRUCT                                    LHN_ID;
  bool                                                         LHN_ID_present;
}LIBLTE_S1AP_MESSAGE_HANDOVERNOTIFY_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_handovernotify(
  LIBLTE_S1AP_MESSAGE_HANDOVERNOTIFY_STRUCT                    *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_handovernotify(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MESSAGE_HANDOVERNOTIFY_STRUCT                    *ie);

/*******************************************************************************
/* Protocol Message E_RABToBeSwitchedDLItem_Ext STRUCT
********************************************************************************/
typedef struct{
  bool                                                         ext;
}LIBLTE_S1AP_MESSAGE_E_RABTOBESWITCHEDDLITEM_EXT_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_e_rabtobeswitcheddlitem_ext(
  LIBLTE_S1AP_MESSAGE_E_RABTOBESWITCHEDDLITEM_EXT_STRUCT       *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_e_rabtobeswitcheddlitem_ext(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MESSAGE_E_RABTOBESWITCHEDDLITEM_EXT_STRUCT       *ie);

/*******************************************************************************
/* Protocol Message E_RABToBeSwitchedULItem_Ext STRUCT
********************************************************************************/
typedef struct{
  bool                                                         ext;
}LIBLTE_S1AP_MESSAGE_E_RABTOBESWITCHEDULITEM_EXT_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_e_rabtobeswitchedulitem_ext(
  LIBLTE_S1AP_MESSAGE_E_RABTOBESWITCHEDULITEM_EXT_STRUCT       *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_e_rabtobeswitchedulitem_ext(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MESSAGE_E_RABTOBESWITCHEDULITEM_EXT_STRUCT       *ie);

/*******************************************************************************
/* Protocol Message PathSwitchRequestFailure STRUCT
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_MME_UE_S1AP_ID_STRUCT                            MME_UE_S1AP_ID;
  LIBLTE_S1AP_ENB_UE_S1AP_ID_STRUCT                            eNB_UE_S1AP_ID;
  LIBLTE_S1AP_CAUSE_STRUCT                                     Cause;
  LIBLTE_S1AP_CRITICALITYDIAGNOSTICS_STRUCT                    CriticalityDiagnostics;
  bool                                                         CriticalityDiagnostics_present;
}LIBLTE_S1AP_MESSAGE_PATHSWITCHREQUESTFAILURE_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_pathswitchrequestfailure(
  LIBLTE_S1AP_MESSAGE_PATHSWITCHREQUESTFAILURE_STRUCT          *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_pathswitchrequestfailure(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MESSAGE_PATHSWITCHREQUESTFAILURE_STRUCT          *ie);

/*******************************************************************************
/* Protocol Message HandoverCancel STRUCT
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_MME_UE_S1AP_ID_STRUCT                            MME_UE_S1AP_ID;
  LIBLTE_S1AP_ENB_UE_S1AP_ID_STRUCT                            eNB_UE_S1AP_ID;
  LIBLTE_S1AP_CAUSE_STRUCT                                     Cause;
}LIBLTE_S1AP_MESSAGE_HANDOVERCANCEL_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_handovercancel(
  LIBLTE_S1AP_MESSAGE_HANDOVERCANCEL_STRUCT                    *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_handovercancel(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MESSAGE_HANDOVERCANCEL_STRUCT                    *ie);

/*******************************************************************************
/* Protocol Message HandoverCancelAcknowledge STRUCT
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_MME_UE_S1AP_ID_STRUCT                            MME_UE_S1AP_ID;
  LIBLTE_S1AP_ENB_UE_S1AP_ID_STRUCT                            eNB_UE_S1AP_ID;
  LIBLTE_S1AP_CRITICALITYDIAGNOSTICS_STRUCT                    CriticalityDiagnostics;
  bool                                                         CriticalityDiagnostics_present;
}LIBLTE_S1AP_MESSAGE_HANDOVERCANCELACKNOWLEDGE_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_handovercancelacknowledge(
  LIBLTE_S1AP_MESSAGE_HANDOVERCANCELACKNOWLEDGE_STRUCT         *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_handovercancelacknowledge(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MESSAGE_HANDOVERCANCELACKNOWLEDGE_STRUCT         *ie);

/*******************************************************************************
/* Protocol Message E_RABToBeSetupItemBearerSUReqExt STRUCT
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_CORRELATION_ID_STRUCT                            Correlation_ID;
  bool                                                         Correlation_ID_present;
  LIBLTE_S1AP_CORRELATION_ID_STRUCT                            SIPTO_Correlation_ID;
  bool                                                         SIPTO_Correlation_ID_present;
}LIBLTE_S1AP_MESSAGE_E_RABTOBESETUPITEMBEARERSUREQEXT_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_e_rabtobesetupitembearersureqext(
  LIBLTE_S1AP_MESSAGE_E_RABTOBESETUPITEMBEARERSUREQEXT_STRUCT  *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_e_rabtobesetupitembearersureqext(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MESSAGE_E_RABTOBESETUPITEMBEARERSUREQEXT_STRUCT  *ie);

/*******************************************************************************
/* Protocol Message E_RABSetupItemBearerSUResExt STRUCT
********************************************************************************/
typedef struct{
  bool                                                         ext;
}LIBLTE_S1AP_MESSAGE_E_RABSETUPITEMBEARERSURESEXT_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_e_rabsetupitembearersuresext(
  LIBLTE_S1AP_MESSAGE_E_RABSETUPITEMBEARERSURESEXT_STRUCT      *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_e_rabsetupitembearersuresext(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MESSAGE_E_RABSETUPITEMBEARERSURESEXT_STRUCT      *ie);

/*******************************************************************************
/* Protocol Message E_RABToBeModifyItemBearerModReqExt STRUCT
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_TRANSPORTINFORMATION_STRUCT                      TransportInformation;
  bool                                                         TransportInformation_present;
}LIBLTE_S1AP_MESSAGE_E_RABTOBEMODIFYITEMBEARERMODREQEXT_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_e_rabtobemodifyitembearermodreqext(
  LIBLTE_S1AP_MESSAGE_E_RABTOBEMODIFYITEMBEARERMODREQEXT_STRUCT *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_e_rabtobemodifyitembearermodreqext(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MESSAGE_E_RABTOBEMODIFYITEMBEARERMODREQEXT_STRUCT *ie);

/*******************************************************************************
/* Protocol Message E_RABModifyItemBearerModResExt STRUCT
********************************************************************************/
typedef struct{
  bool                                                         ext;
}LIBLTE_S1AP_MESSAGE_E_RABMODIFYITEMBEARERMODRESEXT_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_e_rabmodifyitembearermodresext(
  LIBLTE_S1AP_MESSAGE_E_RABMODIFYITEMBEARERMODRESEXT_STRUCT    *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_e_rabmodifyitembearermodresext(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MESSAGE_E_RABMODIFYITEMBEARERMODRESEXT_STRUCT    *ie);

/*******************************************************************************
/* Protocol Message E_RABReleaseCommand STRUCT
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_MME_UE_S1AP_ID_STRUCT                            MME_UE_S1AP_ID;
  LIBLTE_S1AP_ENB_UE_S1AP_ID_STRUCT                            eNB_UE_S1AP_ID;
  LIBLTE_S1AP_UEAGGREGATEMAXIMUMBITRATE_STRUCT                 uEaggregateMaximumBitrate;
  bool                                                         uEaggregateMaximumBitrate_present;
  LIBLTE_S1AP_E_RABLIST_STRUCT                                 E_RABToBeReleasedList;
  LIBLTE_S1AP_NAS_PDU_STRUCT                                   NAS_PDU;
  bool                                                         NAS_PDU_present;
}LIBLTE_S1AP_MESSAGE_E_RABRELEASECOMMAND_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_e_rabreleasecommand(
  LIBLTE_S1AP_MESSAGE_E_RABRELEASECOMMAND_STRUCT               *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_e_rabreleasecommand(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MESSAGE_E_RABRELEASECOMMAND_STRUCT               *ie);

/*******************************************************************************
/* Protocol Message E_RABReleaseItemBearerRelCompExt STRUCT
********************************************************************************/
typedef struct{
  bool                                                         ext;
}LIBLTE_S1AP_MESSAGE_E_RABRELEASEITEMBEARERRELCOMPEXT_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_e_rabreleaseitembearerrelcompext(
  LIBLTE_S1AP_MESSAGE_E_RABRELEASEITEMBEARERRELCOMPEXT_STRUCT  *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_e_rabreleaseitembearerrelcompext(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MESSAGE_E_RABRELEASEITEMBEARERRELCOMPEXT_STRUCT  *ie);

/*******************************************************************************
/* Protocol Message E_RABReleaseIndication STRUCT
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_MME_UE_S1AP_ID_STRUCT                            MME_UE_S1AP_ID;
  LIBLTE_S1AP_ENB_UE_S1AP_ID_STRUCT                            eNB_UE_S1AP_ID;
  LIBLTE_S1AP_E_RABLIST_STRUCT                                 E_RABReleasedList;
  LIBLTE_S1AP_USERLOCATIONINFORMATION_STRUCT                   UserLocationInformation;
  bool                                                         UserLocationInformation_present;
}LIBLTE_S1AP_MESSAGE_E_RABRELEASEINDICATION_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_e_rabreleaseindication(
  LIBLTE_S1AP_MESSAGE_E_RABRELEASEINDICATION_STRUCT            *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_e_rabreleaseindication(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MESSAGE_E_RABRELEASEINDICATION_STRUCT            *ie);

/*******************************************************************************
/* Protocol Message E_RABToBeSetupItemCtxtSUReqExt STRUCT
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_CORRELATION_ID_STRUCT                            Correlation_ID;
  bool                                                         Correlation_ID_present;
  LIBLTE_S1AP_CORRELATION_ID_STRUCT                            SIPTO_Correlation_ID;
  bool                                                         SIPTO_Correlation_ID_present;
}LIBLTE_S1AP_MESSAGE_E_RABTOBESETUPITEMCTXTSUREQEXT_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_e_rabtobesetupitemctxtsureqext(
  LIBLTE_S1AP_MESSAGE_E_RABTOBESETUPITEMCTXTSUREQEXT_STRUCT    *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_e_rabtobesetupitemctxtsureqext(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MESSAGE_E_RABTOBESETUPITEMCTXTSUREQEXT_STRUCT    *ie);

/*******************************************************************************
/* Protocol Message E_RABSetupItemCtxtSUResExt STRUCT
********************************************************************************/
typedef struct{
  bool                                                         ext;
}LIBLTE_S1AP_MESSAGE_E_RABSETUPITEMCTXTSURESEXT_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_e_rabsetupitemctxtsuresext(
  LIBLTE_S1AP_MESSAGE_E_RABSETUPITEMCTXTSURESEXT_STRUCT        *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_e_rabsetupitemctxtsuresext(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MESSAGE_E_RABSETUPITEMCTXTSURESEXT_STRUCT        *ie);

/*******************************************************************************
/* Protocol Message InitialContextSetupFailure STRUCT
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_MME_UE_S1AP_ID_STRUCT                            MME_UE_S1AP_ID;
  LIBLTE_S1AP_ENB_UE_S1AP_ID_STRUCT                            eNB_UE_S1AP_ID;
  LIBLTE_S1AP_CAUSE_STRUCT                                     Cause;
  LIBLTE_S1AP_CRITICALITYDIAGNOSTICS_STRUCT                    CriticalityDiagnostics;
  bool                                                         CriticalityDiagnostics_present;
}LIBLTE_S1AP_MESSAGE_INITIALCONTEXTSETUPFAILURE_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_initialcontextsetupfailure(
  LIBLTE_S1AP_MESSAGE_INITIALCONTEXTSETUPFAILURE_STRUCT        *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_initialcontextsetupfailure(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MESSAGE_INITIALCONTEXTSETUPFAILURE_STRUCT        *ie);

/*******************************************************************************
/* Protocol Message TAIItemExt STRUCT
********************************************************************************/
typedef struct{
  bool                                                         ext;
}LIBLTE_S1AP_MESSAGE_TAIITEMEXT_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_taiitemext(
  LIBLTE_S1AP_MESSAGE_TAIITEMEXT_STRUCT                        *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_taiitemext(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MESSAGE_TAIITEMEXT_STRUCT                        *ie);

/*******************************************************************************
/* Protocol Message UEContextReleaseRequest STRUCT
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_MME_UE_S1AP_ID_STRUCT                            MME_UE_S1AP_ID;
  LIBLTE_S1AP_ENB_UE_S1AP_ID_STRUCT                            eNB_UE_S1AP_ID;
  LIBLTE_S1AP_CAUSE_STRUCT                                     Cause;
  LIBLTE_S1AP_GWCONTEXTRELEASEINDICATION_ENUM_EXT              GWContextReleaseIndication;
  bool                                                         GWContextReleaseIndication_present;
}LIBLTE_S1AP_MESSAGE_UECONTEXTRELEASEREQUEST_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_uecontextreleaserequest(
  LIBLTE_S1AP_MESSAGE_UECONTEXTRELEASEREQUEST_STRUCT           *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_uecontextreleaserequest(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MESSAGE_UECONTEXTRELEASEREQUEST_STRUCT           *ie);

/*******************************************************************************
/* Protocol Message UEContextReleaseCommand STRUCT
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_UE_S1AP_IDS_STRUCT                               UE_S1AP_IDs;
  LIBLTE_S1AP_CAUSE_STRUCT                                     Cause;
}LIBLTE_S1AP_MESSAGE_UECONTEXTRELEASECOMMAND_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_uecontextreleasecommand(
  LIBLTE_S1AP_MESSAGE_UECONTEXTRELEASECOMMAND_STRUCT           *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_uecontextreleasecommand(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MESSAGE_UECONTEXTRELEASECOMMAND_STRUCT           *ie);

/*******************************************************************************
/* Protocol Message UEContextReleaseComplete STRUCT
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_MME_UE_S1AP_ID_STRUCT                            MME_UE_S1AP_ID;
  LIBLTE_S1AP_ENB_UE_S1AP_ID_STRUCT                            eNB_UE_S1AP_ID;
  LIBLTE_S1AP_CRITICALITYDIAGNOSTICS_STRUCT                    CriticalityDiagnostics;
  bool                                                         CriticalityDiagnostics_present;
  LIBLTE_S1AP_USERLOCATIONINFORMATION_STRUCT                   UserLocationInformation;
  bool                                                         UserLocationInformation_present;
}LIBLTE_S1AP_MESSAGE_UECONTEXTRELEASECOMPLETE_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_uecontextreleasecomplete(
  LIBLTE_S1AP_MESSAGE_UECONTEXTRELEASECOMPLETE_STRUCT          *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_uecontextreleasecomplete(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MESSAGE_UECONTEXTRELEASECOMPLETE_STRUCT          *ie);

/*******************************************************************************
/* Protocol Message UEContextModificationRequest STRUCT
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_MME_UE_S1AP_ID_STRUCT                            MME_UE_S1AP_ID;
  LIBLTE_S1AP_ENB_UE_S1AP_ID_STRUCT                            eNB_UE_S1AP_ID;
  LIBLTE_S1AP_SECURITYKEY_STRUCT                               SecurityKey;
  bool                                                         SecurityKey_present;
  LIBLTE_S1AP_SUBSCRIBERPROFILEIDFORRFP_STRUCT                 SubscriberProfileIDforRFP;
  bool                                                         SubscriberProfileIDforRFP_present;
  LIBLTE_S1AP_UEAGGREGATEMAXIMUMBITRATE_STRUCT                 uEaggregateMaximumBitrate;
  bool                                                         uEaggregateMaximumBitrate_present;
  LIBLTE_S1AP_CSFALLBACKINDICATOR_ENUM_EXT                     CSFallbackIndicator;
  bool                                                         CSFallbackIndicator_present;
  LIBLTE_S1AP_UESECURITYCAPABILITIES_STRUCT                    UESecurityCapabilities;
  bool                                                         UESecurityCapabilities_present;
  LIBLTE_S1AP_CSGMEMBERSHIPSTATUS_ENUM                         CSGMembershipStatus;
  bool                                                         CSGMembershipStatus_present;
  LIBLTE_S1AP_LAI_STRUCT                                       RegisteredLAI;
  bool                                                         RegisteredLAI_present;
  LIBLTE_S1AP_ADDITIONALCSFALLBACKINDICATOR_ENUM_EXT           AdditionalCSFallbackIndicator;
  bool                                                         AdditionalCSFallbackIndicator_present;
}LIBLTE_S1AP_MESSAGE_UECONTEXTMODIFICATIONREQUEST_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_uecontextmodificationrequest(
  LIBLTE_S1AP_MESSAGE_UECONTEXTMODIFICATIONREQUEST_STRUCT      *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_uecontextmodificationrequest(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MESSAGE_UECONTEXTMODIFICATIONREQUEST_STRUCT      *ie);

/*******************************************************************************
/* Protocol Message UEContextModificationResponse STRUCT
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_MME_UE_S1AP_ID_STRUCT                            MME_UE_S1AP_ID;
  LIBLTE_S1AP_ENB_UE_S1AP_ID_STRUCT                            eNB_UE_S1AP_ID;
  LIBLTE_S1AP_CRITICALITYDIAGNOSTICS_STRUCT                    CriticalityDiagnostics;
  bool                                                         CriticalityDiagnostics_present;
}LIBLTE_S1AP_MESSAGE_UECONTEXTMODIFICATIONRESPONSE_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_uecontextmodificationresponse(
  LIBLTE_S1AP_MESSAGE_UECONTEXTMODIFICATIONRESPONSE_STRUCT     *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_uecontextmodificationresponse(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MESSAGE_UECONTEXTMODIFICATIONRESPONSE_STRUCT     *ie);

/*******************************************************************************
/* Protocol Message UEContextModificationFailure STRUCT
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_MME_UE_S1AP_ID_STRUCT                            MME_UE_S1AP_ID;
  LIBLTE_S1AP_ENB_UE_S1AP_ID_STRUCT                            eNB_UE_S1AP_ID;
  LIBLTE_S1AP_CAUSE_STRUCT                                     Cause;
  LIBLTE_S1AP_CRITICALITYDIAGNOSTICS_STRUCT                    CriticalityDiagnostics;
  bool                                                         CriticalityDiagnostics_present;
}LIBLTE_S1AP_MESSAGE_UECONTEXTMODIFICATIONFAILURE_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_uecontextmodificationfailure(
  LIBLTE_S1AP_MESSAGE_UECONTEXTMODIFICATIONFAILURE_STRUCT      *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_uecontextmodificationfailure(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MESSAGE_UECONTEXTMODIFICATIONFAILURE_STRUCT      *ie);

/*******************************************************************************
/* Protocol Message UERadioCapabilityMatchRequest STRUCT
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_MME_UE_S1AP_ID_STRUCT                            MME_UE_S1AP_ID;
  LIBLTE_S1AP_ENB_UE_S1AP_ID_STRUCT                            eNB_UE_S1AP_ID;
  LIBLTE_S1AP_UERADIOCAPABILITY_STRUCT                         UERadioCapability;
  bool                                                         UERadioCapability_present;
}LIBLTE_S1AP_MESSAGE_UERADIOCAPABILITYMATCHREQUEST_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_ueradiocapabilitymatchrequest(
  LIBLTE_S1AP_MESSAGE_UERADIOCAPABILITYMATCHREQUEST_STRUCT     *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_ueradiocapabilitymatchrequest(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MESSAGE_UERADIOCAPABILITYMATCHREQUEST_STRUCT     *ie);

/*******************************************************************************
/* Protocol Message UERadioCapabilityMatchResponse STRUCT
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_MME_UE_S1AP_ID_STRUCT                            MME_UE_S1AP_ID;
  LIBLTE_S1AP_ENB_UE_S1AP_ID_STRUCT                            eNB_UE_S1AP_ID;
  LIBLTE_S1AP_VOICESUPPORTMATCHINDICATOR_ENUM_EXT              VoiceSupportMatchIndicator;
  LIBLTE_S1AP_CRITICALITYDIAGNOSTICS_STRUCT                    CriticalityDiagnostics;
  bool                                                         CriticalityDiagnostics_present;
}LIBLTE_S1AP_MESSAGE_UERADIOCAPABILITYMATCHRESPONSE_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_ueradiocapabilitymatchresponse(
  LIBLTE_S1AP_MESSAGE_UERADIOCAPABILITYMATCHRESPONSE_STRUCT    *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_ueradiocapabilitymatchresponse(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MESSAGE_UERADIOCAPABILITYMATCHRESPONSE_STRUCT    *ie);

/*******************************************************************************
/* Protocol Message DownlinkNASTransport STRUCT
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_MME_UE_S1AP_ID_STRUCT                            MME_UE_S1AP_ID;
  LIBLTE_S1AP_ENB_UE_S1AP_ID_STRUCT                            eNB_UE_S1AP_ID;
  LIBLTE_S1AP_NAS_PDU_STRUCT                                   NAS_PDU;
  LIBLTE_S1AP_HANDOVERRESTRICTIONLIST_STRUCT                   HandoverRestrictionList;
  bool                                                         HandoverRestrictionList_present;
  LIBLTE_S1AP_SUBSCRIBERPROFILEIDFORRFP_STRUCT                 SubscriberProfileIDforRFP;
  bool                                                         SubscriberProfileIDforRFP_present;
}LIBLTE_S1AP_MESSAGE_DOWNLINKNASTRANSPORT_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_downlinknastransport(
  LIBLTE_S1AP_MESSAGE_DOWNLINKNASTRANSPORT_STRUCT              *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_downlinknastransport(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MESSAGE_DOWNLINKNASTRANSPORT_STRUCT              *ie);

/*******************************************************************************
/* Protocol Message InitialUEMessage STRUCT
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_ENB_UE_S1AP_ID_STRUCT                            eNB_UE_S1AP_ID;
  LIBLTE_S1AP_NAS_PDU_STRUCT                                   NAS_PDU;
  LIBLTE_S1AP_TAI_STRUCT                                       TAI;
  LIBLTE_S1AP_EUTRAN_CGI_STRUCT                                EUTRAN_CGI;
  LIBLTE_S1AP_RRC_ESTABLISHMENT_CAUSE_ENUM_EXT                 RRC_Establishment_Cause;
  LIBLTE_S1AP_S_TMSI_STRUCT                                    S_TMSI;
  bool                                                         S_TMSI_present;
  LIBLTE_S1AP_CSG_ID_STRUCT                                    CSG_Id;
  bool                                                         CSG_Id_present;
  LIBLTE_S1AP_GUMMEI_STRUCT                                    GUMMEI_ID;
  bool                                                         GUMMEI_ID_present;
  LIBLTE_S1AP_CELLACCESSMODE_ENUM_EXT                          CellAccessMode;
  bool                                                         CellAccessMode_present;
  LIBLTE_S1AP_TRANSPORTLAYERADDRESS_STRUCT                     GW_TransportLayerAddress;
  bool                                                         GW_TransportLayerAddress_present;
  LIBLTE_S1AP_RELAYNODE_INDICATOR_ENUM_EXT                     RelayNode_Indicator;
  bool                                                         RelayNode_Indicator_present;
  LIBLTE_S1AP_GUMMEITYPE_ENUM_EXT                              GUMMEIType;
  bool                                                         GUMMEIType_present;
  LIBLTE_S1AP_TUNNELINFORMATION_STRUCT                         Tunnel_Information_for_BBF;
  bool                                                         Tunnel_Information_for_BBF_present;
  LIBLTE_S1AP_TRANSPORTLAYERADDRESS_STRUCT                     SIPTO_L_GW_TransportLayerAddress;
  bool                                                         SIPTO_L_GW_TransportLayerAddress_present;
  LIBLTE_S1AP_LHN_ID_STRUCT                                    LHN_ID;
  bool                                                         LHN_ID_present;
}LIBLTE_S1AP_MESSAGE_INITIALUEMESSAGE_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_initialuemessage(
  LIBLTE_S1AP_MESSAGE_INITIALUEMESSAGE_STRUCT                  *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_initialuemessage(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MESSAGE_INITIALUEMESSAGE_STRUCT                  *ie);

/*******************************************************************************
/* Protocol Message UplinkNASTransport STRUCT
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_MME_UE_S1AP_ID_STRUCT                            MME_UE_S1AP_ID;
  LIBLTE_S1AP_ENB_UE_S1AP_ID_STRUCT                            eNB_UE_S1AP_ID;
  LIBLTE_S1AP_NAS_PDU_STRUCT                                   NAS_PDU;
  LIBLTE_S1AP_EUTRAN_CGI_STRUCT                                EUTRAN_CGI;
  LIBLTE_S1AP_TAI_STRUCT                                       TAI;
  LIBLTE_S1AP_TRANSPORTLAYERADDRESS_STRUCT                     GW_TransportLayerAddress;
  bool                                                         GW_TransportLayerAddress_present;
  LIBLTE_S1AP_TRANSPORTLAYERADDRESS_STRUCT                     SIPTO_L_GW_TransportLayerAddress;
  bool                                                         SIPTO_L_GW_TransportLayerAddress_present;
  LIBLTE_S1AP_LHN_ID_STRUCT                                    LHN_ID;
  bool                                                         LHN_ID_present;
}LIBLTE_S1AP_MESSAGE_UPLINKNASTRANSPORT_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_uplinknastransport(
  LIBLTE_S1AP_MESSAGE_UPLINKNASTRANSPORT_STRUCT                *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_uplinknastransport(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MESSAGE_UPLINKNASTRANSPORT_STRUCT                *ie);

/*******************************************************************************
/* Protocol Message NASNonDeliveryIndication STRUCT
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_MME_UE_S1AP_ID_STRUCT                            MME_UE_S1AP_ID;
  LIBLTE_S1AP_ENB_UE_S1AP_ID_STRUCT                            eNB_UE_S1AP_ID;
  LIBLTE_S1AP_NAS_PDU_STRUCT                                   NAS_PDU;
  LIBLTE_S1AP_CAUSE_STRUCT                                     Cause;
}LIBLTE_S1AP_MESSAGE_NASNONDELIVERYINDICATION_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_nasnondeliveryindication(
  LIBLTE_S1AP_MESSAGE_NASNONDELIVERYINDICATION_STRUCT          *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_nasnondeliveryindication(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MESSAGE_NASNONDELIVERYINDICATION_STRUCT          *ie);

/*******************************************************************************
/* Protocol Message UE_associatedLogicalS1_ConnectionItem STRUCT
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_UE_ASSOCIATEDLOGICALS1_CONNECTIONITEM_STRUCT     UE_associatedLogicalS1_ConnectionItem;
}LIBLTE_S1AP_MESSAGE_UE_ASSOCIATEDLOGICALS1_CONNECTIONITEM_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_ue_associatedlogicals1_connectionitem(
  LIBLTE_S1AP_MESSAGE_UE_ASSOCIATEDLOGICALS1_CONNECTIONITEM_STRUCT *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_ue_associatedlogicals1_connectionitem(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MESSAGE_UE_ASSOCIATEDLOGICALS1_CONNECTIONITEM_STRUCT *ie);

/*******************************************************************************
/* Protocol Message UE_associatedLogicalS1_ConnectionItemRes STRUCT
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_UE_ASSOCIATEDLOGICALS1_CONNECTIONITEM_STRUCT     UE_associatedLogicalS1_ConnectionItem;
}LIBLTE_S1AP_MESSAGE_UE_ASSOCIATEDLOGICALS1_CONNECTIONITEMRES_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_ue_associatedlogicals1_connectionitemres(
  LIBLTE_S1AP_MESSAGE_UE_ASSOCIATEDLOGICALS1_CONNECTIONITEMRES_STRUCT *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_ue_associatedlogicals1_connectionitemres(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MESSAGE_UE_ASSOCIATEDLOGICALS1_CONNECTIONITEMRES_STRUCT *ie);

/*******************************************************************************
/* Protocol Message ErrorIndication STRUCT
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_MME_UE_S1AP_ID_STRUCT                            MME_UE_S1AP_ID;
  bool                                                         MME_UE_S1AP_ID_present;
  LIBLTE_S1AP_ENB_UE_S1AP_ID_STRUCT                            eNB_UE_S1AP_ID;
  bool                                                         eNB_UE_S1AP_ID_present;
  LIBLTE_S1AP_CAUSE_STRUCT                                     Cause;
  bool                                                         Cause_present;
  LIBLTE_S1AP_CRITICALITYDIAGNOSTICS_STRUCT                    CriticalityDiagnostics;
  bool                                                         CriticalityDiagnostics_present;
}LIBLTE_S1AP_MESSAGE_ERRORINDICATION_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_errorindication(
  LIBLTE_S1AP_MESSAGE_ERRORINDICATION_STRUCT                   *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_errorindication(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MESSAGE_ERRORINDICATION_STRUCT                   *ie);

/*******************************************************************************
/* Protocol Message S1SetupRequest STRUCT
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_GLOBAL_ENB_ID_STRUCT                             Global_ENB_ID;
  LIBLTE_S1AP_ENBNAME_STRUCT                                   eNBname;
  bool                                                         eNBname_present;
  LIBLTE_S1AP_SUPPORTEDTAS_STRUCT                              SupportedTAs;
  LIBLTE_S1AP_PAGINGDRX_ENUM_EXT                               DefaultPagingDRX;
  LIBLTE_S1AP_CSG_IDLIST_STRUCT                                CSG_IdList;
  bool                                                         CSG_IdList_present;
}LIBLTE_S1AP_MESSAGE_S1SETUPREQUEST_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_s1setuprequest(
  LIBLTE_S1AP_MESSAGE_S1SETUPREQUEST_STRUCT                    *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_s1setuprequest(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MESSAGE_S1SETUPREQUEST_STRUCT                    *ie);

/*******************************************************************************
/* Protocol Message S1SetupResponse STRUCT
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_MMENAME_STRUCT                                   MMEname;
  bool                                                         MMEname_present;
  LIBLTE_S1AP_SERVEDGUMMEIS_STRUCT                             ServedGUMMEIs;
  LIBLTE_S1AP_RELATIVEMMECAPACITY_STRUCT                       RelativeMMECapacity;
  LIBLTE_S1AP_MMERELAYSUPPORTINDICATOR_ENUM_EXT                MMERelaySupportIndicator;
  bool                                                         MMERelaySupportIndicator_present;
  LIBLTE_S1AP_CRITICALITYDIAGNOSTICS_STRUCT                    CriticalityDiagnostics;
  bool                                                         CriticalityDiagnostics_present;
}LIBLTE_S1AP_MESSAGE_S1SETUPRESPONSE_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_s1setupresponse(
  LIBLTE_S1AP_MESSAGE_S1SETUPRESPONSE_STRUCT                   *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_s1setupresponse(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MESSAGE_S1SETUPRESPONSE_STRUCT                   *ie);

/*******************************************************************************
/* Protocol Message S1SetupFailure STRUCT
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_CAUSE_STRUCT                                     Cause;
  LIBLTE_S1AP_TIMETOWAIT_ENUM_EXT                              TimeToWait;
  bool                                                         TimeToWait_present;
  LIBLTE_S1AP_CRITICALITYDIAGNOSTICS_STRUCT                    CriticalityDiagnostics;
  bool                                                         CriticalityDiagnostics_present;
}LIBLTE_S1AP_MESSAGE_S1SETUPFAILURE_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_s1setupfailure(
  LIBLTE_S1AP_MESSAGE_S1SETUPFAILURE_STRUCT                    *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_s1setupfailure(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MESSAGE_S1SETUPFAILURE_STRUCT                    *ie);

/*******************************************************************************
/* Protocol Message ENBConfigurationUpdate STRUCT
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_ENBNAME_STRUCT                                   eNBname;
  bool                                                         eNBname_present;
  LIBLTE_S1AP_SUPPORTEDTAS_STRUCT                              SupportedTAs;
  bool                                                         SupportedTAs_present;
  LIBLTE_S1AP_CSG_IDLIST_STRUCT                                CSG_IdList;
  bool                                                         CSG_IdList_present;
  LIBLTE_S1AP_PAGINGDRX_ENUM_EXT                               DefaultPagingDRX;
  bool                                                         DefaultPagingDRX_present;
}LIBLTE_S1AP_MESSAGE_ENBCONFIGURATIONUPDATE_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_enbconfigurationupdate(
  LIBLTE_S1AP_MESSAGE_ENBCONFIGURATIONUPDATE_STRUCT            *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_enbconfigurationupdate(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MESSAGE_ENBCONFIGURATIONUPDATE_STRUCT            *ie);

/*******************************************************************************
/* Protocol Message ENBConfigurationUpdateAcknowledge STRUCT
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_CRITICALITYDIAGNOSTICS_STRUCT                    CriticalityDiagnostics;
  bool                                                         CriticalityDiagnostics_present;
}LIBLTE_S1AP_MESSAGE_ENBCONFIGURATIONUPDATEACKNOWLEDGE_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_enbconfigurationupdateacknowledge(
  LIBLTE_S1AP_MESSAGE_ENBCONFIGURATIONUPDATEACKNOWLEDGE_STRUCT *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_enbconfigurationupdateacknowledge(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MESSAGE_ENBCONFIGURATIONUPDATEACKNOWLEDGE_STRUCT *ie);

/*******************************************************************************
/* Protocol Message ENBConfigurationUpdateFailure STRUCT
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_CAUSE_STRUCT                                     Cause;
  LIBLTE_S1AP_TIMETOWAIT_ENUM_EXT                              TimeToWait;
  bool                                                         TimeToWait_present;
  LIBLTE_S1AP_CRITICALITYDIAGNOSTICS_STRUCT                    CriticalityDiagnostics;
  bool                                                         CriticalityDiagnostics_present;
}LIBLTE_S1AP_MESSAGE_ENBCONFIGURATIONUPDATEFAILURE_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_enbconfigurationupdatefailure(
  LIBLTE_S1AP_MESSAGE_ENBCONFIGURATIONUPDATEFAILURE_STRUCT     *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_enbconfigurationupdatefailure(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MESSAGE_ENBCONFIGURATIONUPDATEFAILURE_STRUCT     *ie);

/*******************************************************************************
/* Protocol Message MMEConfigurationUpdate STRUCT
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_MMENAME_STRUCT                                   MMEname;
  bool                                                         MMEname_present;
  LIBLTE_S1AP_SERVEDGUMMEIS_STRUCT                             ServedGUMMEIs;
  bool                                                         ServedGUMMEIs_present;
  LIBLTE_S1AP_RELATIVEMMECAPACITY_STRUCT                       RelativeMMECapacity;
  bool                                                         RelativeMMECapacity_present;
}LIBLTE_S1AP_MESSAGE_MMECONFIGURATIONUPDATE_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_mmeconfigurationupdate(
  LIBLTE_S1AP_MESSAGE_MMECONFIGURATIONUPDATE_STRUCT            *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_mmeconfigurationupdate(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MESSAGE_MMECONFIGURATIONUPDATE_STRUCT            *ie);

/*******************************************************************************
/* Protocol Message MMEConfigurationUpdateAcknowledge STRUCT
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_CRITICALITYDIAGNOSTICS_STRUCT                    CriticalityDiagnostics;
  bool                                                         CriticalityDiagnostics_present;
}LIBLTE_S1AP_MESSAGE_MMECONFIGURATIONUPDATEACKNOWLEDGE_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_mmeconfigurationupdateacknowledge(
  LIBLTE_S1AP_MESSAGE_MMECONFIGURATIONUPDATEACKNOWLEDGE_STRUCT *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_mmeconfigurationupdateacknowledge(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MESSAGE_MMECONFIGURATIONUPDATEACKNOWLEDGE_STRUCT *ie);

/*******************************************************************************
/* Protocol Message MMEConfigurationUpdateFailure STRUCT
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_CAUSE_STRUCT                                     Cause;
  LIBLTE_S1AP_TIMETOWAIT_ENUM_EXT                              TimeToWait;
  bool                                                         TimeToWait_present;
  LIBLTE_S1AP_CRITICALITYDIAGNOSTICS_STRUCT                    CriticalityDiagnostics;
  bool                                                         CriticalityDiagnostics_present;
}LIBLTE_S1AP_MESSAGE_MMECONFIGURATIONUPDATEFAILURE_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_mmeconfigurationupdatefailure(
  LIBLTE_S1AP_MESSAGE_MMECONFIGURATIONUPDATEFAILURE_STRUCT     *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_mmeconfigurationupdatefailure(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MESSAGE_MMECONFIGURATIONUPDATEFAILURE_STRUCT     *ie);

/*******************************************************************************
/* Protocol Message UplinkS1cdma2000tunneling STRUCT
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_MME_UE_S1AP_ID_STRUCT                            MME_UE_S1AP_ID;
  LIBLTE_S1AP_ENB_UE_S1AP_ID_STRUCT                            eNB_UE_S1AP_ID;
  LIBLTE_S1AP_CDMA2000RATTYPE_ENUM_EXT                         cdma2000RATType;
  LIBLTE_S1AP_CDMA2000SECTORID_STRUCT                          cdma2000SectorID;
  LIBLTE_S1AP_CDMA2000HOREQUIREDINDICATION_ENUM_EXT            cdma2000HORequiredIndication;
  bool                                                         cdma2000HORequiredIndication_present;
  LIBLTE_S1AP_CDMA2000ONEXSRVCCINFO_STRUCT                     cdma2000OneXSRVCCInfo;
  bool                                                         cdma2000OneXSRVCCInfo_present;
  LIBLTE_S1AP_CDMA2000ONEXRAND_STRUCT                          cdma2000OneXRAND;
  bool                                                         cdma2000OneXRAND_present;
  LIBLTE_S1AP_CDMA2000PDU_STRUCT                               cdma2000PDU;
  LIBLTE_S1AP_EUTRANROUNDTRIPDELAYESTIMATIONINFO_STRUCT        EUTRANRoundTripDelayEstimationInfo;
  bool                                                         EUTRANRoundTripDelayEstimationInfo_present;
}LIBLTE_S1AP_MESSAGE_UPLINKS1CDMA2000TUNNELING_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_uplinks1cdma2000tunneling(
  LIBLTE_S1AP_MESSAGE_UPLINKS1CDMA2000TUNNELING_STRUCT         *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_uplinks1cdma2000tunneling(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MESSAGE_UPLINKS1CDMA2000TUNNELING_STRUCT         *ie);

/*******************************************************************************
/* Protocol Message UECapabilityInfoIndication STRUCT
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_MME_UE_S1AP_ID_STRUCT                            MME_UE_S1AP_ID;
  LIBLTE_S1AP_ENB_UE_S1AP_ID_STRUCT                            eNB_UE_S1AP_ID;
  LIBLTE_S1AP_UERADIOCAPABILITY_STRUCT                         UERadioCapability;
}LIBLTE_S1AP_MESSAGE_UECAPABILITYINFOINDICATION_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_uecapabilityinfoindication(
  LIBLTE_S1AP_MESSAGE_UECAPABILITYINFOINDICATION_STRUCT        *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_uecapabilityinfoindication(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MESSAGE_UECAPABILITYINFOINDICATION_STRUCT        *ie);

/*******************************************************************************
/* Protocol Message ENBStatusTransfer STRUCT
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_MME_UE_S1AP_ID_STRUCT                            MME_UE_S1AP_ID;
  LIBLTE_S1AP_ENB_UE_S1AP_ID_STRUCT                            eNB_UE_S1AP_ID;
  LIBLTE_S1AP_ENB_STATUSTRANSFER_TRANSPARENTCONTAINER_STRUCT   eNB_StatusTransfer_TransparentContainer;
}LIBLTE_S1AP_MESSAGE_ENBSTATUSTRANSFER_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_enbstatustransfer(
  LIBLTE_S1AP_MESSAGE_ENBSTATUSTRANSFER_STRUCT                 *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_enbstatustransfer(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MESSAGE_ENBSTATUSTRANSFER_STRUCT                 *ie);

/*******************************************************************************
/* Protocol Message MMEStatusTransfer STRUCT
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_MME_UE_S1AP_ID_STRUCT                            MME_UE_S1AP_ID;
  LIBLTE_S1AP_ENB_UE_S1AP_ID_STRUCT                            eNB_UE_S1AP_ID;
  LIBLTE_S1AP_ENB_STATUSTRANSFER_TRANSPARENTCONTAINER_STRUCT   eNB_StatusTransfer_TransparentContainer;
}LIBLTE_S1AP_MESSAGE_MMESTATUSTRANSFER_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_mmestatustransfer(
  LIBLTE_S1AP_MESSAGE_MMESTATUSTRANSFER_STRUCT                 *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_mmestatustransfer(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MESSAGE_MMESTATUSTRANSFER_STRUCT                 *ie);

/*******************************************************************************
/* Protocol Message TraceStart STRUCT
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_MME_UE_S1AP_ID_STRUCT                            MME_UE_S1AP_ID;
  LIBLTE_S1AP_ENB_UE_S1AP_ID_STRUCT                            eNB_UE_S1AP_ID;
  LIBLTE_S1AP_TRACEACTIVATION_STRUCT                           TraceActivation;
}LIBLTE_S1AP_MESSAGE_TRACESTART_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_tracestart(
  LIBLTE_S1AP_MESSAGE_TRACESTART_STRUCT                        *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_tracestart(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MESSAGE_TRACESTART_STRUCT                        *ie);

/*******************************************************************************
/* Protocol Message TraceFailureIndication STRUCT
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_MME_UE_S1AP_ID_STRUCT                            MME_UE_S1AP_ID;
  LIBLTE_S1AP_ENB_UE_S1AP_ID_STRUCT                            eNB_UE_S1AP_ID;
  LIBLTE_S1AP_E_UTRAN_TRACE_ID_STRUCT                          E_UTRAN_Trace_ID;
  LIBLTE_S1AP_CAUSE_STRUCT                                     Cause;
}LIBLTE_S1AP_MESSAGE_TRACEFAILUREINDICATION_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_tracefailureindication(
  LIBLTE_S1AP_MESSAGE_TRACEFAILUREINDICATION_STRUCT            *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_tracefailureindication(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MESSAGE_TRACEFAILUREINDICATION_STRUCT            *ie);

/*******************************************************************************
/* Protocol Message DeactivateTrace STRUCT
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_MME_UE_S1AP_ID_STRUCT                            MME_UE_S1AP_ID;
  LIBLTE_S1AP_ENB_UE_S1AP_ID_STRUCT                            eNB_UE_S1AP_ID;
  LIBLTE_S1AP_E_UTRAN_TRACE_ID_STRUCT                          E_UTRAN_Trace_ID;
}LIBLTE_S1AP_MESSAGE_DEACTIVATETRACE_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_deactivatetrace(
  LIBLTE_S1AP_MESSAGE_DEACTIVATETRACE_STRUCT                   *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_deactivatetrace(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MESSAGE_DEACTIVATETRACE_STRUCT                   *ie);

/*******************************************************************************
/* Protocol Message CellTrafficTrace STRUCT
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_MME_UE_S1AP_ID_STRUCT                            MME_UE_S1AP_ID;
  LIBLTE_S1AP_ENB_UE_S1AP_ID_STRUCT                            eNB_UE_S1AP_ID;
  LIBLTE_S1AP_E_UTRAN_TRACE_ID_STRUCT                          E_UTRAN_Trace_ID;
  LIBLTE_S1AP_EUTRAN_CGI_STRUCT                                EUTRAN_CGI;
  LIBLTE_S1AP_TRANSPORTLAYERADDRESS_STRUCT                     TraceCollectionEntityIPAddress;
  LIBLTE_S1AP_PRIVACYINDICATOR_ENUM_EXT                        PrivacyIndicator;
  bool                                                         PrivacyIndicator_present;
}LIBLTE_S1AP_MESSAGE_CELLTRAFFICTRACE_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_celltraffictrace(
  LIBLTE_S1AP_MESSAGE_CELLTRAFFICTRACE_STRUCT                  *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_celltraffictrace(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MESSAGE_CELLTRAFFICTRACE_STRUCT                  *ie);

/*******************************************************************************
/* Protocol Message LocationReportingControl STRUCT
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_MME_UE_S1AP_ID_STRUCT                            MME_UE_S1AP_ID;
  LIBLTE_S1AP_ENB_UE_S1AP_ID_STRUCT                            eNB_UE_S1AP_ID;
  LIBLTE_S1AP_REQUESTTYPE_STRUCT                               RequestType;
}LIBLTE_S1AP_MESSAGE_LOCATIONREPORTINGCONTROL_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_locationreportingcontrol(
  LIBLTE_S1AP_MESSAGE_LOCATIONREPORTINGCONTROL_STRUCT          *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_locationreportingcontrol(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MESSAGE_LOCATIONREPORTINGCONTROL_STRUCT          *ie);

/*******************************************************************************
/* Protocol Message LocationReportingFailureIndication STRUCT
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_MME_UE_S1AP_ID_STRUCT                            MME_UE_S1AP_ID;
  LIBLTE_S1AP_ENB_UE_S1AP_ID_STRUCT                            eNB_UE_S1AP_ID;
  LIBLTE_S1AP_CAUSE_STRUCT                                     Cause;
}LIBLTE_S1AP_MESSAGE_LOCATIONREPORTINGFAILUREINDICATION_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_locationreportingfailureindication(
  LIBLTE_S1AP_MESSAGE_LOCATIONREPORTINGFAILUREINDICATION_STRUCT *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_locationreportingfailureindication(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MESSAGE_LOCATIONREPORTINGFAILUREINDICATION_STRUCT *ie);

/*******************************************************************************
/* Protocol Message LocationReport STRUCT
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_MME_UE_S1AP_ID_STRUCT                            MME_UE_S1AP_ID;
  LIBLTE_S1AP_ENB_UE_S1AP_ID_STRUCT                            eNB_UE_S1AP_ID;
  LIBLTE_S1AP_EUTRAN_CGI_STRUCT                                EUTRAN_CGI;
  LIBLTE_S1AP_TAI_STRUCT                                       TAI;
  LIBLTE_S1AP_REQUESTTYPE_STRUCT                               RequestType;
}LIBLTE_S1AP_MESSAGE_LOCATIONREPORT_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_locationreport(
  LIBLTE_S1AP_MESSAGE_LOCATIONREPORT_STRUCT                    *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_locationreport(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MESSAGE_LOCATIONREPORT_STRUCT                    *ie);

/*******************************************************************************
/* Protocol Message OverloadStart STRUCT
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_OVERLOADRESPONSE_STRUCT                          OverloadResponse;
  LIBLTE_S1AP_GUMMEILIST_STRUCT                                GUMMEIList;
  bool                                                         GUMMEIList_present;
  LIBLTE_S1AP_TRAFFICLOADREDUCTIONINDICATION_STRUCT            TrafficLoadReductionIndication;
  bool                                                         TrafficLoadReductionIndication_present;
}LIBLTE_S1AP_MESSAGE_OVERLOADSTART_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_overloadstart(
  LIBLTE_S1AP_MESSAGE_OVERLOADSTART_STRUCT                     *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_overloadstart(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MESSAGE_OVERLOADSTART_STRUCT                     *ie);

/*******************************************************************************
/* Protocol Message OverloadStop STRUCT
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_GUMMEILIST_STRUCT                                GUMMEIList;
  bool                                                         GUMMEIList_present;
}LIBLTE_S1AP_MESSAGE_OVERLOADSTOP_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_overloadstop(
  LIBLTE_S1AP_MESSAGE_OVERLOADSTOP_STRUCT                      *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_overloadstop(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MESSAGE_OVERLOADSTOP_STRUCT                      *ie);

/*******************************************************************************
/* Protocol Message WriteReplaceWarningRequest STRUCT
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_MESSAGEIDENTIFIER_STRUCT                         MessageIdentifier;
  LIBLTE_S1AP_SERIALNUMBER_STRUCT                              SerialNumber;
  LIBLTE_S1AP_WARNINGAREALIST_STRUCT                           WarningAreaList;
  bool                                                         WarningAreaList_present;
  LIBLTE_S1AP_REPETITIONPERIOD_STRUCT                          RepetitionPeriod;
  LIBLTE_S1AP_EXTENDEDREPETITIONPERIOD_STRUCT                  ExtendedRepetitionPeriod;
  bool                                                         ExtendedRepetitionPeriod_present;
  LIBLTE_S1AP_NUMBEROFBROADCASTREQUEST_STRUCT                  NumberofBroadcastRequest;
  LIBLTE_S1AP_WARNINGTYPE_STRUCT                               WarningType;
  bool                                                         WarningType_present;
  LIBLTE_S1AP_WARNINGSECURITYINFO_STRUCT                       WarningSecurityInfo;
  bool                                                         WarningSecurityInfo_present;
  LIBLTE_S1AP_DATACODINGSCHEME_STRUCT                          DataCodingScheme;
  bool                                                         DataCodingScheme_present;
  LIBLTE_S1AP_WARNINGMESSAGECONTENTS_STRUCT                    WarningMessageContents;
  bool                                                         WarningMessageContents_present;
  LIBLTE_S1AP_CONCURRENTWARNINGMESSAGEINDICATOR_ENUM           ConcurrentWarningMessageIndicator;
  bool                                                         ConcurrentWarningMessageIndicator_present;
}LIBLTE_S1AP_MESSAGE_WRITEREPLACEWARNINGREQUEST_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_writereplacewarningrequest(
  LIBLTE_S1AP_MESSAGE_WRITEREPLACEWARNINGREQUEST_STRUCT        *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_writereplacewarningrequest(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MESSAGE_WRITEREPLACEWARNINGREQUEST_STRUCT        *ie);

/*******************************************************************************
/* Protocol Message WriteReplaceWarningResponse STRUCT
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_MESSAGEIDENTIFIER_STRUCT                         MessageIdentifier;
  LIBLTE_S1AP_SERIALNUMBER_STRUCT                              SerialNumber;
  LIBLTE_S1AP_BROADCASTCOMPLETEDAREALIST_STRUCT                BroadcastCompletedAreaList;
  bool                                                         BroadcastCompletedAreaList_present;
  LIBLTE_S1AP_CRITICALITYDIAGNOSTICS_STRUCT                    CriticalityDiagnostics;
  bool                                                         CriticalityDiagnostics_present;
}LIBLTE_S1AP_MESSAGE_WRITEREPLACEWARNINGRESPONSE_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_writereplacewarningresponse(
  LIBLTE_S1AP_MESSAGE_WRITEREPLACEWARNINGRESPONSE_STRUCT       *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_writereplacewarningresponse(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MESSAGE_WRITEREPLACEWARNINGRESPONSE_STRUCT       *ie);

/*******************************************************************************
/* Protocol Message MMEDirectInformationTransfer STRUCT
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_INTER_SYSTEMINFORMATIONTRANSFERTYPE_STRUCT       Inter_SystemInformationTransferTypeMDT;
}LIBLTE_S1AP_MESSAGE_MMEDIRECTINFORMATIONTRANSFER_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_mmedirectinformationtransfer(
  LIBLTE_S1AP_MESSAGE_MMEDIRECTINFORMATIONTRANSFER_STRUCT      *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_mmedirectinformationtransfer(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MESSAGE_MMEDIRECTINFORMATIONTRANSFER_STRUCT      *ie);

/*******************************************************************************
/* Protocol Message ENBConfigurationTransfer STRUCT
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_SONCONFIGURATIONTRANSFER_STRUCT                  SONConfigurationTransferECT;
  bool                                                         SONConfigurationTransferECT_present;
}LIBLTE_S1AP_MESSAGE_ENBCONFIGURATIONTRANSFER_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_enbconfigurationtransfer(
  LIBLTE_S1AP_MESSAGE_ENBCONFIGURATIONTRANSFER_STRUCT          *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_enbconfigurationtransfer(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MESSAGE_ENBCONFIGURATIONTRANSFER_STRUCT          *ie);

/*******************************************************************************
/* Protocol Message MMEConfigurationTransfer STRUCT
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_SONCONFIGURATIONTRANSFER_STRUCT                  SONConfigurationTransferMCT;
  bool                                                         SONConfigurationTransferMCT_present;
}LIBLTE_S1AP_MESSAGE_MMECONFIGURATIONTRANSFER_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_mmeconfigurationtransfer(
  LIBLTE_S1AP_MESSAGE_MMECONFIGURATIONTRANSFER_STRUCT          *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_mmeconfigurationtransfer(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MESSAGE_MMECONFIGURATIONTRANSFER_STRUCT          *ie);

/*******************************************************************************
/* Protocol Message PrivateMessage STRUCT
********************************************************************************/
typedef struct{
  bool                                                         ext;
}LIBLTE_S1AP_MESSAGE_PRIVATEMESSAGE_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_privatemessage(
  LIBLTE_S1AP_MESSAGE_PRIVATEMESSAGE_STRUCT                    *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_privatemessage(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MESSAGE_PRIVATEMESSAGE_STRUCT                    *ie);

/*******************************************************************************
/* Protocol Message KillRequest STRUCT
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_MESSAGEIDENTIFIER_STRUCT                         MessageIdentifier;
  LIBLTE_S1AP_SERIALNUMBER_STRUCT                              SerialNumber;
  LIBLTE_S1AP_WARNINGAREALIST_STRUCT                           WarningAreaList;
  bool                                                         WarningAreaList_present;
  LIBLTE_S1AP_KILLALLWARNINGMESSAGES_ENUM                      KillAllWarningMessages;
  bool                                                         KillAllWarningMessages_present;
}LIBLTE_S1AP_MESSAGE_KILLREQUEST_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_killrequest(
  LIBLTE_S1AP_MESSAGE_KILLREQUEST_STRUCT                       *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_killrequest(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MESSAGE_KILLREQUEST_STRUCT                       *ie);

/*******************************************************************************
/* Protocol Message KillResponse STRUCT
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_MESSAGEIDENTIFIER_STRUCT                         MessageIdentifier;
  LIBLTE_S1AP_SERIALNUMBER_STRUCT                              SerialNumber;
  LIBLTE_S1AP_BROADCASTCANCELLEDAREALIST_STRUCT                BroadcastCancelledAreaList;
  bool                                                         BroadcastCancelledAreaList_present;
  LIBLTE_S1AP_CRITICALITYDIAGNOSTICS_STRUCT                    CriticalityDiagnostics;
  bool                                                         CriticalityDiagnostics_present;
}LIBLTE_S1AP_MESSAGE_KILLRESPONSE_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_killresponse(
  LIBLTE_S1AP_MESSAGE_KILLRESPONSE_STRUCT                      *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_killresponse(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MESSAGE_KILLRESPONSE_STRUCT                      *ie);

/*******************************************************************************
/* Protocol Message PWSRestartIndication STRUCT
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_ECGILISTFORRESTART_STRUCT                        ECGIListForRestart;
  LIBLTE_S1AP_GLOBAL_ENB_ID_STRUCT                             Global_ENB_ID;
  LIBLTE_S1AP_TAILISTFORRESTART_STRUCT                         TAIListForRestart;
  LIBLTE_S1AP_EMERGENCYAREAIDLISTFORRESTART_STRUCT             EmergencyAreaIDListForRestart;
  bool                                                         EmergencyAreaIDListForRestart_present;
}LIBLTE_S1AP_MESSAGE_PWSRESTARTINDICATION_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_pwsrestartindication(
  LIBLTE_S1AP_MESSAGE_PWSRESTARTINDICATION_STRUCT              *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_pwsrestartindication(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MESSAGE_PWSRESTARTINDICATION_STRUCT              *ie);

/*******************************************************************************
/* Protocol Message DownlinkUEAssociatedLPPaTransport STRUCT
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_MME_UE_S1AP_ID_STRUCT                            MME_UE_S1AP_ID;
  LIBLTE_S1AP_ENB_UE_S1AP_ID_STRUCT                            eNB_UE_S1AP_ID;
  LIBLTE_S1AP_ROUTING_ID_STRUCT                                Routing_ID;
  LIBLTE_S1AP_LPPA_PDU_STRUCT                                  LPPa_PDU;
}LIBLTE_S1AP_MESSAGE_DOWNLINKUEASSOCIATEDLPPATRANSPORT_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_downlinkueassociatedlppatransport(
  LIBLTE_S1AP_MESSAGE_DOWNLINKUEASSOCIATEDLPPATRANSPORT_STRUCT *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_downlinkueassociatedlppatransport(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MESSAGE_DOWNLINKUEASSOCIATEDLPPATRANSPORT_STRUCT *ie);

/*******************************************************************************
/* Protocol Message UplinkUEAssociatedLPPaTransport STRUCT
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_MME_UE_S1AP_ID_STRUCT                            MME_UE_S1AP_ID;
  LIBLTE_S1AP_ENB_UE_S1AP_ID_STRUCT                            eNB_UE_S1AP_ID;
  LIBLTE_S1AP_ROUTING_ID_STRUCT                                Routing_ID;
  LIBLTE_S1AP_LPPA_PDU_STRUCT                                  LPPa_PDU;
}LIBLTE_S1AP_MESSAGE_UPLINKUEASSOCIATEDLPPATRANSPORT_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_uplinkueassociatedlppatransport(
  LIBLTE_S1AP_MESSAGE_UPLINKUEASSOCIATEDLPPATRANSPORT_STRUCT   *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_uplinkueassociatedlppatransport(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MESSAGE_UPLINKUEASSOCIATEDLPPATRANSPORT_STRUCT   *ie);

/*******************************************************************************
/* Protocol Message DownlinkNonUEAssociatedLPPaTransport STRUCT
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_ROUTING_ID_STRUCT                                Routing_ID;
  LIBLTE_S1AP_LPPA_PDU_STRUCT                                  LPPa_PDU;
}LIBLTE_S1AP_MESSAGE_DOWNLINKNONUEASSOCIATEDLPPATRANSPORT_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_downlinknonueassociatedlppatransport(
  LIBLTE_S1AP_MESSAGE_DOWNLINKNONUEASSOCIATEDLPPATRANSPORT_STRUCT *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_downlinknonueassociatedlppatransport(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MESSAGE_DOWNLINKNONUEASSOCIATEDLPPATRANSPORT_STRUCT *ie);

/*******************************************************************************
/* Protocol Message UplinkNonUEAssociatedLPPaTransport STRUCT
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_ROUTING_ID_STRUCT                                Routing_ID;
  LIBLTE_S1AP_LPPA_PDU_STRUCT                                  LPPa_PDU;
}LIBLTE_S1AP_MESSAGE_UPLINKNONUEASSOCIATEDLPPATRANSPORT_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_uplinknonueassociatedlppatransport(
  LIBLTE_S1AP_MESSAGE_UPLINKNONUEASSOCIATEDLPPATRANSPORT_STRUCT *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_uplinknonueassociatedlppatransport(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MESSAGE_UPLINKNONUEASSOCIATEDLPPATRANSPORT_STRUCT *ie);

/*******************************************************************************
/* Protocol Message ENBDirectInformationTransfer STRUCT
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_INTER_SYSTEMINFORMATIONTRANSFERTYPE_STRUCT       Inter_SystemInformationTransferTypeEDT;
}LIBLTE_S1AP_MESSAGE_ENBDIRECTINFORMATIONTRANSFER_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_enbdirectinformationtransfer(
  LIBLTE_S1AP_MESSAGE_ENBDIRECTINFORMATIONTRANSFER_STRUCT      *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_enbdirectinformationtransfer(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MESSAGE_ENBDIRECTINFORMATIONTRANSFER_STRUCT      *ie);

/*******************************************************************************
/* Protocol Message E_RABDataForwardingItem STRUCT
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_E_RABDATAFORWARDINGITEM_STRUCT                   E_RABDataForwardingItem;
}LIBLTE_S1AP_MESSAGE_E_RABDATAFORWARDINGITEM_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_e_rabdataforwardingitem(
  LIBLTE_S1AP_MESSAGE_E_RABDATAFORWARDINGITEM_STRUCT           *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_e_rabdataforwardingitem(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MESSAGE_E_RABDATAFORWARDINGITEM_STRUCT           *ie);

/*******************************************************************************
/* Protocol Message E_RABToBeSetupItemHOReq STRUCT
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_E_RABTOBESETUPITEMHOREQ_STRUCT                   E_RABToBeSetupItemHOReq;
}LIBLTE_S1AP_MESSAGE_E_RABTOBESETUPITEMHOREQ_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_e_rabtobesetupitemhoreq(
  LIBLTE_S1AP_MESSAGE_E_RABTOBESETUPITEMHOREQ_STRUCT           *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_e_rabtobesetupitemhoreq(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MESSAGE_E_RABTOBESETUPITEMHOREQ_STRUCT           *ie);

/*******************************************************************************
/* Protocol Message E_RABAdmittedItem STRUCT
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_E_RABADMITTEDITEM_STRUCT                         E_RABAdmittedItem;
}LIBLTE_S1AP_MESSAGE_E_RABADMITTEDITEM_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_e_rabadmitteditem(
  LIBLTE_S1AP_MESSAGE_E_RABADMITTEDITEM_STRUCT                 *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_e_rabadmitteditem(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MESSAGE_E_RABADMITTEDITEM_STRUCT                 *ie);

/*******************************************************************************
/* Protocol Message E_RABFailedtoSetupItemHOReqAck STRUCT
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_E_RABFAILEDTOSETUPITEMHOREQACK_STRUCT            E_RABFailedtoSetupItemHOReqAck;
}LIBLTE_S1AP_MESSAGE_E_RABFAILEDTOSETUPITEMHOREQACK_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_e_rabfailedtosetupitemhoreqack(
  LIBLTE_S1AP_MESSAGE_E_RABFAILEDTOSETUPITEMHOREQACK_STRUCT    *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_e_rabfailedtosetupitemhoreqack(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MESSAGE_E_RABFAILEDTOSETUPITEMHOREQACK_STRUCT    *ie);

/*******************************************************************************
/* Protocol Message E_RABToBeSwitchedDLItem STRUCT
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_E_RABTOBESWITCHEDDLITEM_STRUCT                   E_RABToBeSwitchedDLItem;
}LIBLTE_S1AP_MESSAGE_E_RABTOBESWITCHEDDLITEM_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_e_rabtobeswitcheddlitem(
  LIBLTE_S1AP_MESSAGE_E_RABTOBESWITCHEDDLITEM_STRUCT           *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_e_rabtobeswitcheddlitem(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MESSAGE_E_RABTOBESWITCHEDDLITEM_STRUCT           *ie);

/*******************************************************************************
/* Protocol Message E_RABToBeSwitchedULItem STRUCT
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_E_RABTOBESWITCHEDULITEM_STRUCT                   E_RABToBeSwitchedULItem;
}LIBLTE_S1AP_MESSAGE_E_RABTOBESWITCHEDULITEM_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_e_rabtobeswitchedulitem(
  LIBLTE_S1AP_MESSAGE_E_RABTOBESWITCHEDULITEM_STRUCT           *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_e_rabtobeswitchedulitem(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MESSAGE_E_RABTOBESWITCHEDULITEM_STRUCT           *ie);

/*******************************************************************************
/* Protocol Message E_RABToBeSetupItemBearerSUReq STRUCT
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_E_RABTOBESETUPITEMBEARERSUREQ_STRUCT             E_RABToBeSetupItemBearerSUReq;
}LIBLTE_S1AP_MESSAGE_E_RABTOBESETUPITEMBEARERSUREQ_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_e_rabtobesetupitembearersureq(
  LIBLTE_S1AP_MESSAGE_E_RABTOBESETUPITEMBEARERSUREQ_STRUCT     *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_e_rabtobesetupitembearersureq(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MESSAGE_E_RABTOBESETUPITEMBEARERSUREQ_STRUCT     *ie);

/*******************************************************************************
/* Protocol Message E_RABSetupItemBearerSURes STRUCT
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_E_RABSETUPITEMBEARERSURES_STRUCT                 E_RABSetupItemBearerSURes;
}LIBLTE_S1AP_MESSAGE_E_RABSETUPITEMBEARERSURES_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_e_rabsetupitembearersures(
  LIBLTE_S1AP_MESSAGE_E_RABSETUPITEMBEARERSURES_STRUCT         *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_e_rabsetupitembearersures(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MESSAGE_E_RABSETUPITEMBEARERSURES_STRUCT         *ie);

/*******************************************************************************
/* Protocol Message E_RABToBeModifiedItemBearerModReq STRUCT
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_E_RABTOBEMODIFIEDITEMBEARERMODREQ_STRUCT         E_RABToBeModifiedItemBearerModReq;
}LIBLTE_S1AP_MESSAGE_E_RABTOBEMODIFIEDITEMBEARERMODREQ_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_e_rabtobemodifieditembearermodreq(
  LIBLTE_S1AP_MESSAGE_E_RABTOBEMODIFIEDITEMBEARERMODREQ_STRUCT *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_e_rabtobemodifieditembearermodreq(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MESSAGE_E_RABTOBEMODIFIEDITEMBEARERMODREQ_STRUCT *ie);

/*******************************************************************************
/* Protocol Message E_RABModifyItemBearerModRes STRUCT
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_E_RABMODIFYITEMBEARERMODRES_STRUCT               E_RABModifyItemBearerModRes;
}LIBLTE_S1AP_MESSAGE_E_RABMODIFYITEMBEARERMODRES_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_e_rabmodifyitembearermodres(
  LIBLTE_S1AP_MESSAGE_E_RABMODIFYITEMBEARERMODRES_STRUCT       *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_e_rabmodifyitembearermodres(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MESSAGE_E_RABMODIFYITEMBEARERMODRES_STRUCT       *ie);

/*******************************************************************************
/* Protocol Message E_RABReleaseItemBearerRelComp STRUCT
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_E_RABRELEASEITEMBEARERRELCOMP_STRUCT             E_RABReleaseItemBearerRelComp;
}LIBLTE_S1AP_MESSAGE_E_RABRELEASEITEMBEARERRELCOMP_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_e_rabreleaseitembearerrelcomp(
  LIBLTE_S1AP_MESSAGE_E_RABRELEASEITEMBEARERRELCOMP_STRUCT     *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_e_rabreleaseitembearerrelcomp(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MESSAGE_E_RABRELEASEITEMBEARERRELCOMP_STRUCT     *ie);

/*******************************************************************************
/* Protocol Message E_RABToBeSetupItemCtxtSUReq STRUCT
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_E_RABTOBESETUPITEMCTXTSUREQ_STRUCT               E_RABToBeSetupItemCtxtSUReq;
}LIBLTE_S1AP_MESSAGE_E_RABTOBESETUPITEMCTXTSUREQ_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_e_rabtobesetupitemctxtsureq(
  LIBLTE_S1AP_MESSAGE_E_RABTOBESETUPITEMCTXTSUREQ_STRUCT       *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_e_rabtobesetupitemctxtsureq(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MESSAGE_E_RABTOBESETUPITEMCTXTSUREQ_STRUCT       *ie);

/*******************************************************************************
/* Protocol Message E_RABSetupItemCtxtSURes STRUCT
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_E_RABSETUPITEMCTXTSURES_STRUCT                   E_RABSetupItemCtxtSURes;
}LIBLTE_S1AP_MESSAGE_E_RABSETUPITEMCTXTSURES_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_e_rabsetupitemctxtsures(
  LIBLTE_S1AP_MESSAGE_E_RABSETUPITEMCTXTSURES_STRUCT           *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_e_rabsetupitemctxtsures(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MESSAGE_E_RABSETUPITEMCTXTSURES_STRUCT           *ie);

/*******************************************************************************
/* Protocol Message TAIItem STRUCT
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_TAIITEM_STRUCT                                   TAIItem;
}LIBLTE_S1AP_MESSAGE_TAIITEM_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_taiitem(
  LIBLTE_S1AP_MESSAGE_TAIITEM_STRUCT                           *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_taiitem(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MESSAGE_TAIITEM_STRUCT                           *ie);

/*******************************************************************************
/* Protocol Message ResetAcknowledge STRUCT
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_UE_ASSOCIATEDLOGICALS1_CONNECTIONLISTRESACK_STRUCT UE_associatedLogicalS1_ConnectionListResAck;
  bool                                                         UE_associatedLogicalS1_ConnectionListResAck_present;
  LIBLTE_S1AP_CRITICALITYDIAGNOSTICS_STRUCT                    CriticalityDiagnostics;
  bool                                                         CriticalityDiagnostics_present;
}LIBLTE_S1AP_MESSAGE_RESETACKNOWLEDGE_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_resetacknowledge(
  LIBLTE_S1AP_MESSAGE_RESETACKNOWLEDGE_STRUCT                  *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_resetacknowledge(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MESSAGE_RESETACKNOWLEDGE_STRUCT                  *ie);

/*******************************************************************************
/* Protocol Message Reset STRUCT
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_CAUSE_STRUCT                                     Cause;
  LIBLTE_S1AP_RESETTYPE_STRUCT                                 ResetType;
}LIBLTE_S1AP_MESSAGE_RESET_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_reset(
  LIBLTE_S1AP_MESSAGE_RESET_STRUCT                             *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_reset(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MESSAGE_RESET_STRUCT                             *ie);

/*******************************************************************************
/* Protocol Message DownlinkS1cdma2000tunneling STRUCT
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_MME_UE_S1AP_ID_STRUCT                            MME_UE_S1AP_ID;
  LIBLTE_S1AP_ENB_UE_S1AP_ID_STRUCT                            eNB_UE_S1AP_ID;
  LIBLTE_S1AP_E_RABSUBJECTTODATAFORWARDINGLIST_STRUCT          E_RABSubjecttoDataForwardingList;
  bool                                                         E_RABSubjecttoDataForwardingList_present;
  LIBLTE_S1AP_CDMA2000HOSTATUS_ENUM_EXT                        cdma2000HOStatus;
  bool                                                         cdma2000HOStatus_present;
  LIBLTE_S1AP_CDMA2000RATTYPE_ENUM_EXT                         cdma2000RATType;
  LIBLTE_S1AP_CDMA2000PDU_STRUCT                               cdma2000PDU;
}LIBLTE_S1AP_MESSAGE_DOWNLINKS1CDMA2000TUNNELING_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_downlinks1cdma2000tunneling(
  LIBLTE_S1AP_MESSAGE_DOWNLINKS1CDMA2000TUNNELING_STRUCT       *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_downlinks1cdma2000tunneling(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MESSAGE_DOWNLINKS1CDMA2000TUNNELING_STRUCT       *ie);

/*******************************************************************************
/* Protocol Message HandoverCommand STRUCT
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_MME_UE_S1AP_ID_STRUCT                            MME_UE_S1AP_ID;
  LIBLTE_S1AP_ENB_UE_S1AP_ID_STRUCT                            eNB_UE_S1AP_ID;
  LIBLTE_S1AP_HANDOVERTYPE_ENUM_EXT                            HandoverType;
  LIBLTE_S1AP_NASSECURITYPARAMETERSFROME_UTRAN_STRUCT          NASSecurityParametersfromE_UTRAN;
  bool                                                         NASSecurityParametersfromE_UTRAN_present;
  LIBLTE_S1AP_E_RABSUBJECTTODATAFORWARDINGLIST_STRUCT          E_RABSubjecttoDataForwardingList;
  bool                                                         E_RABSubjecttoDataForwardingList_present;
  LIBLTE_S1AP_E_RABLIST_STRUCT                                 E_RABtoReleaseListHOCmd;
  bool                                                         E_RABtoReleaseListHOCmd_present;
  LIBLTE_S1AP_TARGET_TOSOURCE_TRANSPARENTCONTAINER_STRUCT      Target_ToSource_TransparentContainer;
  LIBLTE_S1AP_TARGET_TOSOURCE_TRANSPARENTCONTAINER_STRUCT      Target_ToSource_TransparentContainer_Secondary;
  bool                                                         Target_ToSource_TransparentContainer_Secondary_present;
  LIBLTE_S1AP_CRITICALITYDIAGNOSTICS_STRUCT                    CriticalityDiagnostics;
  bool                                                         CriticalityDiagnostics_present;
}LIBLTE_S1AP_MESSAGE_HANDOVERCOMMAND_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_handovercommand(
  LIBLTE_S1AP_MESSAGE_HANDOVERCOMMAND_STRUCT                   *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_handovercommand(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MESSAGE_HANDOVERCOMMAND_STRUCT                   *ie);

/*******************************************************************************
/* Protocol Message HandoverRequest STRUCT
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_MME_UE_S1AP_ID_STRUCT                            MME_UE_S1AP_ID;
  LIBLTE_S1AP_HANDOVERTYPE_ENUM_EXT                            HandoverType;
  LIBLTE_S1AP_CAUSE_STRUCT                                     Cause;
  LIBLTE_S1AP_UEAGGREGATEMAXIMUMBITRATE_STRUCT                 uEaggregateMaximumBitrate;
  LIBLTE_S1AP_E_RABTOBESETUPLISTHOREQ_STRUCT                   E_RABToBeSetupListHOReq;
  LIBLTE_S1AP_SOURCE_TOTARGET_TRANSPARENTCONTAINER_STRUCT      Source_ToTarget_TransparentContainer;
  LIBLTE_S1AP_UESECURITYCAPABILITIES_STRUCT                    UESecurityCapabilities;
  LIBLTE_S1AP_HANDOVERRESTRICTIONLIST_STRUCT                   HandoverRestrictionList;
  bool                                                         HandoverRestrictionList_present;
  LIBLTE_S1AP_TRACEACTIVATION_STRUCT                           TraceActivation;
  bool                                                         TraceActivation_present;
  LIBLTE_S1AP_REQUESTTYPE_STRUCT                               RequestType;
  bool                                                         RequestType_present;
  LIBLTE_S1AP_SRVCCOPERATIONPOSSIBLE_ENUM_EXT                  SRVCCOperationPossible;
  bool                                                         SRVCCOperationPossible_present;
  LIBLTE_S1AP_SECURITYCONTEXT_STRUCT                           SecurityContext;
  LIBLTE_S1AP_NASSECURITYPARAMETERSTOE_UTRAN_STRUCT            NASSecurityParameterstoE_UTRAN;
  bool                                                         NASSecurityParameterstoE_UTRAN_present;
  LIBLTE_S1AP_CSG_ID_STRUCT                                    CSG_Id;
  bool                                                         CSG_Id_present;
  LIBLTE_S1AP_CSGMEMBERSHIPSTATUS_ENUM                         CSGMembershipStatus;
  bool                                                         CSGMembershipStatus_present;
  LIBLTE_S1AP_GUMMEI_STRUCT                                    GUMMEI_ID;
  bool                                                         GUMMEI_ID_present;
  LIBLTE_S1AP_MME_UE_S1AP_ID_STRUCT                            MME_UE_S1AP_ID_2;
  bool                                                         MME_UE_S1AP_ID_2_present;
  LIBLTE_S1AP_MANAGEMENTBASEDMDTALLOWED_ENUM_EXT               ManagementBasedMDTAllowed;
  bool                                                         ManagementBasedMDTAllowed_present;
  LIBLTE_S1AP_MDTPLMNLIST_STRUCT                               ManagementBasedMDTPLMNList;
  bool                                                         ManagementBasedMDTPLMNList_present;
}LIBLTE_S1AP_MESSAGE_HANDOVERREQUEST_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_handoverrequest(
  LIBLTE_S1AP_MESSAGE_HANDOVERREQUEST_STRUCT                   *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_handoverrequest(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MESSAGE_HANDOVERREQUEST_STRUCT                   *ie);

/*******************************************************************************
/* Protocol Message PathSwitchRequest STRUCT
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_ENB_UE_S1AP_ID_STRUCT                            eNB_UE_S1AP_ID;
  LIBLTE_S1AP_E_RABTOBESWITCHEDDLLIST_STRUCT                   E_RABToBeSwitchedDLList;
  LIBLTE_S1AP_MME_UE_S1AP_ID_STRUCT                            SourceMME_UE_S1AP_ID;
  LIBLTE_S1AP_EUTRAN_CGI_STRUCT                                EUTRAN_CGI;
  LIBLTE_S1AP_TAI_STRUCT                                       TAI;
  LIBLTE_S1AP_UESECURITYCAPABILITIES_STRUCT                    UESecurityCapabilities;
  LIBLTE_S1AP_CSG_ID_STRUCT                                    CSG_Id;
  bool                                                         CSG_Id_present;
  LIBLTE_S1AP_CELLACCESSMODE_ENUM_EXT                          CellAccessMode;
  bool                                                         CellAccessMode_present;
  LIBLTE_S1AP_GUMMEI_STRUCT                                    SourceMME_GUMMEI;
  bool                                                         SourceMME_GUMMEI_present;
  LIBLTE_S1AP_CSGMEMBERSHIPSTATUS_ENUM                         CSGMembershipStatus;
  bool                                                         CSGMembershipStatus_present;
  LIBLTE_S1AP_TUNNELINFORMATION_STRUCT                         Tunnel_Information_for_BBF;
  bool                                                         Tunnel_Information_for_BBF_present;
  LIBLTE_S1AP_LHN_ID_STRUCT                                    LHN_ID;
  bool                                                         LHN_ID_present;
}LIBLTE_S1AP_MESSAGE_PATHSWITCHREQUEST_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_pathswitchrequest(
  LIBLTE_S1AP_MESSAGE_PATHSWITCHREQUEST_STRUCT                 *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_pathswitchrequest(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MESSAGE_PATHSWITCHREQUEST_STRUCT                 *ie);

/*******************************************************************************
/* Protocol Message PathSwitchRequestAcknowledge STRUCT
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_MME_UE_S1AP_ID_STRUCT                            MME_UE_S1AP_ID;
  LIBLTE_S1AP_ENB_UE_S1AP_ID_STRUCT                            eNB_UE_S1AP_ID;
  LIBLTE_S1AP_UEAGGREGATEMAXIMUMBITRATE_STRUCT                 uEaggregateMaximumBitrate;
  bool                                                         uEaggregateMaximumBitrate_present;
  LIBLTE_S1AP_E_RABTOBESWITCHEDULLIST_STRUCT                   E_RABToBeSwitchedULList;
  bool                                                         E_RABToBeSwitchedULList_present;
  LIBLTE_S1AP_E_RABLIST_STRUCT                                 E_RABToBeReleasedList;
  bool                                                         E_RABToBeReleasedList_present;
  LIBLTE_S1AP_SECURITYCONTEXT_STRUCT                           SecurityContext;
  LIBLTE_S1AP_CRITICALITYDIAGNOSTICS_STRUCT                    CriticalityDiagnostics;
  bool                                                         CriticalityDiagnostics_present;
  LIBLTE_S1AP_MME_UE_S1AP_ID_STRUCT                            MME_UE_S1AP_ID_2;
  bool                                                         MME_UE_S1AP_ID_2_present;
  LIBLTE_S1AP_CSGMEMBERSHIPSTATUS_ENUM                         CSGMembershipStatus;
  bool                                                         CSGMembershipStatus_present;
}LIBLTE_S1AP_MESSAGE_PATHSWITCHREQUESTACKNOWLEDGE_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_pathswitchrequestacknowledge(
  LIBLTE_S1AP_MESSAGE_PATHSWITCHREQUESTACKNOWLEDGE_STRUCT      *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_pathswitchrequestacknowledge(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MESSAGE_PATHSWITCHREQUESTACKNOWLEDGE_STRUCT      *ie);

/*******************************************************************************
/* Protocol Message E_RABSetupRequest STRUCT
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_MME_UE_S1AP_ID_STRUCT                            MME_UE_S1AP_ID;
  LIBLTE_S1AP_ENB_UE_S1AP_ID_STRUCT                            eNB_UE_S1AP_ID;
  LIBLTE_S1AP_UEAGGREGATEMAXIMUMBITRATE_STRUCT                 uEaggregateMaximumBitrate;
  bool                                                         uEaggregateMaximumBitrate_present;
  LIBLTE_S1AP_E_RABTOBESETUPLISTBEARERSUREQ_STRUCT             E_RABToBeSetupListBearerSUReq;
}LIBLTE_S1AP_MESSAGE_E_RABSETUPREQUEST_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_e_rabsetuprequest(
  LIBLTE_S1AP_MESSAGE_E_RABSETUPREQUEST_STRUCT                 *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_e_rabsetuprequest(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MESSAGE_E_RABSETUPREQUEST_STRUCT                 *ie);

/*******************************************************************************
/* Protocol Message E_RABSetupResponse STRUCT
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_MME_UE_S1AP_ID_STRUCT                            MME_UE_S1AP_ID;
  LIBLTE_S1AP_ENB_UE_S1AP_ID_STRUCT                            eNB_UE_S1AP_ID;
  LIBLTE_S1AP_E_RABSETUPLISTBEARERSURES_STRUCT                 E_RABSetupListBearerSURes;
  bool                                                         E_RABSetupListBearerSURes_present;
  LIBLTE_S1AP_E_RABLIST_STRUCT                                 E_RABFailedToSetupListBearerSURes;
  bool                                                         E_RABFailedToSetupListBearerSURes_present;
  LIBLTE_S1AP_CRITICALITYDIAGNOSTICS_STRUCT                    CriticalityDiagnostics;
  bool                                                         CriticalityDiagnostics_present;
}LIBLTE_S1AP_MESSAGE_E_RABSETUPRESPONSE_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_e_rabsetupresponse(
  LIBLTE_S1AP_MESSAGE_E_RABSETUPRESPONSE_STRUCT                *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_e_rabsetupresponse(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MESSAGE_E_RABSETUPRESPONSE_STRUCT                *ie);

/*******************************************************************************
/* Protocol Message E_RABModifyRequest STRUCT
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_MME_UE_S1AP_ID_STRUCT                            MME_UE_S1AP_ID;
  LIBLTE_S1AP_ENB_UE_S1AP_ID_STRUCT                            eNB_UE_S1AP_ID;
  LIBLTE_S1AP_UEAGGREGATEMAXIMUMBITRATE_STRUCT                 uEaggregateMaximumBitrate;
  bool                                                         uEaggregateMaximumBitrate_present;
  LIBLTE_S1AP_E_RABTOBEMODIFIEDLISTBEARERMODREQ_STRUCT         E_RABToBeModifiedListBearerModReq;
}LIBLTE_S1AP_MESSAGE_E_RABMODIFYREQUEST_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_e_rabmodifyrequest(
  LIBLTE_S1AP_MESSAGE_E_RABMODIFYREQUEST_STRUCT                *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_e_rabmodifyrequest(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MESSAGE_E_RABMODIFYREQUEST_STRUCT                *ie);

/*******************************************************************************
/* Protocol Message E_RABModifyResponse STRUCT
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_MME_UE_S1AP_ID_STRUCT                            MME_UE_S1AP_ID;
  LIBLTE_S1AP_ENB_UE_S1AP_ID_STRUCT                            eNB_UE_S1AP_ID;
  LIBLTE_S1AP_E_RABMODIFYLISTBEARERMODRES_STRUCT               E_RABModifyListBearerModRes;
  bool                                                         E_RABModifyListBearerModRes_present;
  LIBLTE_S1AP_E_RABLIST_STRUCT                                 E_RABFailedToModifyList;
  bool                                                         E_RABFailedToModifyList_present;
  LIBLTE_S1AP_CRITICALITYDIAGNOSTICS_STRUCT                    CriticalityDiagnostics;
  bool                                                         CriticalityDiagnostics_present;
}LIBLTE_S1AP_MESSAGE_E_RABMODIFYRESPONSE_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_e_rabmodifyresponse(
  LIBLTE_S1AP_MESSAGE_E_RABMODIFYRESPONSE_STRUCT               *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_e_rabmodifyresponse(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MESSAGE_E_RABMODIFYRESPONSE_STRUCT               *ie);

/*******************************************************************************
/* Protocol Message E_RABReleaseResponse STRUCT
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_MME_UE_S1AP_ID_STRUCT                            MME_UE_S1AP_ID;
  LIBLTE_S1AP_ENB_UE_S1AP_ID_STRUCT                            eNB_UE_S1AP_ID;
  LIBLTE_S1AP_E_RABRELEASELISTBEARERRELCOMP_STRUCT             E_RABReleaseListBearerRelComp;
  bool                                                         E_RABReleaseListBearerRelComp_present;
  LIBLTE_S1AP_E_RABLIST_STRUCT                                 E_RABFailedToReleaseList;
  bool                                                         E_RABFailedToReleaseList_present;
  LIBLTE_S1AP_CRITICALITYDIAGNOSTICS_STRUCT                    CriticalityDiagnostics;
  bool                                                         CriticalityDiagnostics_present;
  LIBLTE_S1AP_USERLOCATIONINFORMATION_STRUCT                   UserLocationInformation;
  bool                                                         UserLocationInformation_present;
}LIBLTE_S1AP_MESSAGE_E_RABRELEASERESPONSE_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_e_rabreleaseresponse(
  LIBLTE_S1AP_MESSAGE_E_RABRELEASERESPONSE_STRUCT              *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_e_rabreleaseresponse(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MESSAGE_E_RABRELEASERESPONSE_STRUCT              *ie);

/*******************************************************************************
/* Protocol Message InitialContextSetupRequest STRUCT
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_MME_UE_S1AP_ID_STRUCT                            MME_UE_S1AP_ID;
  LIBLTE_S1AP_ENB_UE_S1AP_ID_STRUCT                            eNB_UE_S1AP_ID;
  LIBLTE_S1AP_UEAGGREGATEMAXIMUMBITRATE_STRUCT                 uEaggregateMaximumBitrate;
  LIBLTE_S1AP_E_RABTOBESETUPLISTCTXTSUREQ_STRUCT               E_RABToBeSetupListCtxtSUReq;
  LIBLTE_S1AP_UESECURITYCAPABILITIES_STRUCT                    UESecurityCapabilities;
  LIBLTE_S1AP_SECURITYKEY_STRUCT                               SecurityKey;
  LIBLTE_S1AP_TRACEACTIVATION_STRUCT                           TraceActivation;
  bool                                                         TraceActivation_present;
  LIBLTE_S1AP_HANDOVERRESTRICTIONLIST_STRUCT                   HandoverRestrictionList;
  bool                                                         HandoverRestrictionList_present;
  LIBLTE_S1AP_UERADIOCAPABILITY_STRUCT                         UERadioCapability;
  bool                                                         UERadioCapability_present;
  LIBLTE_S1AP_SUBSCRIBERPROFILEIDFORRFP_STRUCT                 SubscriberProfileIDforRFP;
  bool                                                         SubscriberProfileIDforRFP_present;
  LIBLTE_S1AP_CSFALLBACKINDICATOR_ENUM_EXT                     CSFallbackIndicator;
  bool                                                         CSFallbackIndicator_present;
  LIBLTE_S1AP_SRVCCOPERATIONPOSSIBLE_ENUM_EXT                  SRVCCOperationPossible;
  bool                                                         SRVCCOperationPossible_present;
  LIBLTE_S1AP_CSGMEMBERSHIPSTATUS_ENUM                         CSGMembershipStatus;
  bool                                                         CSGMembershipStatus_present;
  LIBLTE_S1AP_LAI_STRUCT                                       RegisteredLAI;
  bool                                                         RegisteredLAI_present;
  LIBLTE_S1AP_GUMMEI_STRUCT                                    GUMMEI_ID;
  bool                                                         GUMMEI_ID_present;
  LIBLTE_S1AP_MME_UE_S1AP_ID_STRUCT                            MME_UE_S1AP_ID_2;
  bool                                                         MME_UE_S1AP_ID_2_present;
  LIBLTE_S1AP_MANAGEMENTBASEDMDTALLOWED_ENUM_EXT               ManagementBasedMDTAllowed;
  bool                                                         ManagementBasedMDTAllowed_present;
  LIBLTE_S1AP_MDTPLMNLIST_STRUCT                               ManagementBasedMDTPLMNList;
  bool                                                         ManagementBasedMDTPLMNList_present;
  LIBLTE_S1AP_ADDITIONALCSFALLBACKINDICATOR_ENUM_EXT           AdditionalCSFallbackIndicator;
  bool                                                         AdditionalCSFallbackIndicator_present;
}LIBLTE_S1AP_MESSAGE_INITIALCONTEXTSETUPREQUEST_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_initialcontextsetuprequest(
  LIBLTE_S1AP_MESSAGE_INITIALCONTEXTSETUPREQUEST_STRUCT        *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_initialcontextsetuprequest(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MESSAGE_INITIALCONTEXTSETUPREQUEST_STRUCT        *ie);

/*******************************************************************************
/* Protocol Message InitialContextSetupResponse STRUCT
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_MME_UE_S1AP_ID_STRUCT                            MME_UE_S1AP_ID;
  LIBLTE_S1AP_ENB_UE_S1AP_ID_STRUCT                            eNB_UE_S1AP_ID;
  LIBLTE_S1AP_E_RABSETUPLISTCTXTSURES_STRUCT                   E_RABSetupListCtxtSURes;
  LIBLTE_S1AP_E_RABLIST_STRUCT                                 E_RABFailedToSetupListCtxtSURes;
  bool                                                         E_RABFailedToSetupListCtxtSURes_present;
  LIBLTE_S1AP_CRITICALITYDIAGNOSTICS_STRUCT                    CriticalityDiagnostics;
  bool                                                         CriticalityDiagnostics_present;
}LIBLTE_S1AP_MESSAGE_INITIALCONTEXTSETUPRESPONSE_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_initialcontextsetupresponse(
  LIBLTE_S1AP_MESSAGE_INITIALCONTEXTSETUPRESPONSE_STRUCT       *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_initialcontextsetupresponse(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MESSAGE_INITIALCONTEXTSETUPRESPONSE_STRUCT       *ie);

/*******************************************************************************
/* Protocol Message Paging STRUCT
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_UEIDENTITYINDEXVALUE_STRUCT                      UEIdentityIndexValue;
  LIBLTE_S1AP_UEPAGINGID_STRUCT                                UEPagingID;
  LIBLTE_S1AP_PAGINGDRX_ENUM_EXT                               pagingDRX;
  bool                                                         pagingDRX_present;
  LIBLTE_S1AP_CNDOMAIN_ENUM                                    CNDomain;
  LIBLTE_S1AP_TAILIST_STRUCT                                   TAIList;
  LIBLTE_S1AP_CSG_IDLIST_STRUCT                                CSG_IdList;
  bool                                                         CSG_IdList_present;
  LIBLTE_S1AP_PAGINGPRIORITY_ENUM_EXT                          PagingPriority;
  bool                                                         PagingPriority_present;
}LIBLTE_S1AP_MESSAGE_PAGING_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_paging(
  LIBLTE_S1AP_MESSAGE_PAGING_STRUCT                            *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_paging(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MESSAGE_PAGING_STRUCT                            *ie);

/*******************************************************************************
/* Protocol Message HandoverRequestAcknowledge STRUCT
********************************************************************************/
typedef struct{
  bool                                                         ext;
  LIBLTE_S1AP_MME_UE_S1AP_ID_STRUCT                            MME_UE_S1AP_ID;
  LIBLTE_S1AP_ENB_UE_S1AP_ID_STRUCT                            eNB_UE_S1AP_ID;
  LIBLTE_S1AP_E_RABADMITTEDLIST_STRUCT                         E_RABAdmittedList;
  LIBLTE_S1AP_E_RABFAILEDTOSETUPLISTHOREQACK_STRUCT            E_RABFailedToSetupListHOReqAck;
  bool                                                         E_RABFailedToSetupListHOReqAck_present;
  LIBLTE_S1AP_TARGET_TOSOURCE_TRANSPARENTCONTAINER_STRUCT      Target_ToSource_TransparentContainer;
  LIBLTE_S1AP_CSG_ID_STRUCT                                    CSG_Id;
  bool                                                         CSG_Id_present;
  LIBLTE_S1AP_CRITICALITYDIAGNOSTICS_STRUCT                    CriticalityDiagnostics;
  bool                                                         CriticalityDiagnostics_present;
  LIBLTE_S1AP_CELLACCESSMODE_ENUM_EXT                          CellAccessMode;
  bool                                                         CellAccessMode_present;
}LIBLTE_S1AP_MESSAGE_HANDOVERREQUESTACKNOWLEDGE_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_handoverrequestacknowledge(
  LIBLTE_S1AP_MESSAGE_HANDOVERREQUESTACKNOWLEDGE_STRUCT        *ie,
  uint8_t                                                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_handoverrequestacknowledge(
  uint8_t                                                     **ptr,
  LIBLTE_S1AP_MESSAGE_HANDOVERREQUESTACKNOWLEDGE_STRUCT        *ie);

/*******************************************************************************
/* Procedure code criticality lookups
********************************************************************************/
static const LIBLTE_S1AP_CRITICALITY_ENUM liblte_s1ap_procedure_criticality[50] = {
  LIBLTE_S1AP_CRITICALITY_REJECT,
  LIBLTE_S1AP_CRITICALITY_REJECT,
  LIBLTE_S1AP_CRITICALITY_IGNORE,
  LIBLTE_S1AP_CRITICALITY_REJECT,
  LIBLTE_S1AP_CRITICALITY_REJECT,
  LIBLTE_S1AP_CRITICALITY_REJECT,
  LIBLTE_S1AP_CRITICALITY_REJECT,
  LIBLTE_S1AP_CRITICALITY_REJECT,
  LIBLTE_S1AP_CRITICALITY_IGNORE,
  LIBLTE_S1AP_CRITICALITY_REJECT,
  LIBLTE_S1AP_CRITICALITY_IGNORE,
  LIBLTE_S1AP_CRITICALITY_IGNORE,
  LIBLTE_S1AP_CRITICALITY_IGNORE,
  LIBLTE_S1AP_CRITICALITY_IGNORE,
  LIBLTE_S1AP_CRITICALITY_REJECT,
  LIBLTE_S1AP_CRITICALITY_IGNORE,
  LIBLTE_S1AP_CRITICALITY_IGNORE,
  LIBLTE_S1AP_CRITICALITY_REJECT,
  LIBLTE_S1AP_CRITICALITY_IGNORE,
  LIBLTE_S1AP_CRITICALITY_IGNORE,
  LIBLTE_S1AP_CRITICALITY_IGNORE,
  LIBLTE_S1AP_CRITICALITY_REJECT,
  LIBLTE_S1AP_CRITICALITY_IGNORE,
  LIBLTE_S1AP_CRITICALITY_REJECT,
  LIBLTE_S1AP_CRITICALITY_IGNORE,
  LIBLTE_S1AP_CRITICALITY_IGNORE,
  LIBLTE_S1AP_CRITICALITY_IGNORE,
  LIBLTE_S1AP_CRITICALITY_IGNORE,
  LIBLTE_S1AP_CRITICALITY_IGNORE,
  LIBLTE_S1AP_CRITICALITY_REJECT,
  LIBLTE_S1AP_CRITICALITY_REJECT,
  LIBLTE_S1AP_CRITICALITY_IGNORE,
  LIBLTE_S1AP_CRITICALITY_IGNORE,
  LIBLTE_S1AP_CRITICALITY_IGNORE,
  LIBLTE_S1AP_CRITICALITY_IGNORE,
  LIBLTE_S1AP_CRITICALITY_REJECT,
  LIBLTE_S1AP_CRITICALITY_REJECT,
  LIBLTE_S1AP_CRITICALITY_IGNORE,
  LIBLTE_S1AP_CRITICALITY_IGNORE,
  LIBLTE_S1AP_CRITICALITY_IGNORE,
  LIBLTE_S1AP_CRITICALITY_IGNORE,
  LIBLTE_S1AP_CRITICALITY_IGNORE,
  LIBLTE_S1AP_CRITICALITY_IGNORE,
  LIBLTE_S1AP_CRITICALITY_REJECT,
  LIBLTE_S1AP_CRITICALITY_IGNORE,
  LIBLTE_S1AP_CRITICALITY_IGNORE,
  LIBLTE_S1AP_CRITICALITY_IGNORE,
  LIBLTE_S1AP_CRITICALITY_IGNORE,
  LIBLTE_S1AP_CRITICALITY_REJECT,
  LIBLTE_S1AP_CRITICALITY_IGNORE,
};

/*******************************************************************************
/* ProtocolIE-Field
********************************************************************************/
LIBLTE_ERROR_ENUM liblte_s1ap_pack_protocolie_header(
  uint32_t                      len,
  uint32_t                      ie_id,
  LIBLTE_S1AP_CRITICALITY_ENUM  crit,
  uint8_t                     **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_protocolie_header(
  uint8_t                     **ptr,
  uint32_t                     *ie_id,
  LIBLTE_S1AP_CRITICALITY_ENUM *crit,
  uint32_t                     *len);

/*******************************************************************************
/* InitiatingMessage CHOICE
********************************************************************************/
typedef enum{
  LIBLTE_S1AP_INITIATINGMESSAGE_CHOICE_LOCATIONREPORTINGCONTROL,
  LIBLTE_S1AP_INITIATINGMESSAGE_CHOICE_DOWNLINKS1CDMA2000TUNNELING,
  LIBLTE_S1AP_INITIATINGMESSAGE_CHOICE_S1SETUPREQUEST,
  LIBLTE_S1AP_INITIATINGMESSAGE_CHOICE_UECAPABILITYINFOINDICATION,
  LIBLTE_S1AP_INITIATINGMESSAGE_CHOICE_LOCATIONREPORT,
  LIBLTE_S1AP_INITIATINGMESSAGE_CHOICE_UPLINKNONUEASSOCIATEDLPPATRANSPORT,
  LIBLTE_S1AP_INITIATINGMESSAGE_CHOICE_UPLINKS1CDMA2000TUNNELING,
  LIBLTE_S1AP_INITIATINGMESSAGE_CHOICE_MMECONFIGURATIONTRANSFER,
  LIBLTE_S1AP_INITIATINGMESSAGE_CHOICE_TRACESTART,
  LIBLTE_S1AP_INITIATINGMESSAGE_CHOICE_HANDOVERCANCEL,
  LIBLTE_S1AP_INITIATINGMESSAGE_CHOICE_UERADIOCAPABILITYMATCHREQUEST,
  LIBLTE_S1AP_INITIATINGMESSAGE_CHOICE_DOWNLINKNASTRANSPORT,
  LIBLTE_S1AP_INITIATINGMESSAGE_CHOICE_INITIALCONTEXTSETUPREQUEST,
  LIBLTE_S1AP_INITIATINGMESSAGE_CHOICE_HANDOVERREQUIRED,
  LIBLTE_S1AP_INITIATINGMESSAGE_CHOICE_MMEDIRECTINFORMATIONTRANSFER,
  LIBLTE_S1AP_INITIATINGMESSAGE_CHOICE_TRACEFAILUREINDICATION,
  LIBLTE_S1AP_INITIATINGMESSAGE_CHOICE_MMECONFIGURATIONUPDATE,
  LIBLTE_S1AP_INITIATINGMESSAGE_CHOICE_WRITEREPLACEWARNINGREQUEST,
  LIBLTE_S1AP_INITIATINGMESSAGE_CHOICE_ENBDIRECTINFORMATIONTRANSFER,
  LIBLTE_S1AP_INITIATINGMESSAGE_CHOICE_DOWNLINKUEASSOCIATEDLPPATRANSPORT,
  LIBLTE_S1AP_INITIATINGMESSAGE_CHOICE_E_RABRELEASECOMMAND,
  LIBLTE_S1AP_INITIATINGMESSAGE_CHOICE_NASNONDELIVERYINDICATION,
  LIBLTE_S1AP_INITIATINGMESSAGE_CHOICE_ENBCONFIGURATIONUPDATE,
  LIBLTE_S1AP_INITIATINGMESSAGE_CHOICE_UPLINKUEASSOCIATEDLPPATRANSPORT,
  LIBLTE_S1AP_INITIATINGMESSAGE_CHOICE_INITIALUEMESSAGE,
  LIBLTE_S1AP_INITIATINGMESSAGE_CHOICE_E_RABMODIFYREQUEST,
  LIBLTE_S1AP_INITIATINGMESSAGE_CHOICE_UECONTEXTMODIFICATIONREQUEST,
  LIBLTE_S1AP_INITIATINGMESSAGE_CHOICE_E_RABSETUPREQUEST,
  LIBLTE_S1AP_INITIATINGMESSAGE_CHOICE_RESET,
  LIBLTE_S1AP_INITIATINGMESSAGE_CHOICE_OVERLOADSTART,
  LIBLTE_S1AP_INITIATINGMESSAGE_CHOICE_E_RABRELEASEINDICATION,
  LIBLTE_S1AP_INITIATINGMESSAGE_CHOICE_LOCATIONREPORTINGFAILUREINDICATION,
  LIBLTE_S1AP_INITIATINGMESSAGE_CHOICE_DEACTIVATETRACE,
  LIBLTE_S1AP_INITIATINGMESSAGE_CHOICE_PATHSWITCHREQUEST,
  LIBLTE_S1AP_INITIATINGMESSAGE_CHOICE_HANDOVERREQUEST,
  LIBLTE_S1AP_INITIATINGMESSAGE_CHOICE_DOWNLINKNONUEASSOCIATEDLPPATRANSPORT,
  LIBLTE_S1AP_INITIATINGMESSAGE_CHOICE_OVERLOADSTOP,
  LIBLTE_S1AP_INITIATINGMESSAGE_CHOICE_PAGING,
  LIBLTE_S1AP_INITIATINGMESSAGE_CHOICE_HANDOVERNOTIFY,
  LIBLTE_S1AP_INITIATINGMESSAGE_CHOICE_PWSRESTARTINDICATION,
  LIBLTE_S1AP_INITIATINGMESSAGE_CHOICE_UECONTEXTRELEASEREQUEST,
  LIBLTE_S1AP_INITIATINGMESSAGE_CHOICE_UPLINKNASTRANSPORT,
  LIBLTE_S1AP_INITIATINGMESSAGE_CHOICE_ENBCONFIGURATIONTRANSFER,
  LIBLTE_S1AP_INITIATINGMESSAGE_CHOICE_MMESTATUSTRANSFER,
  LIBLTE_S1AP_INITIATINGMESSAGE_CHOICE_CELLTRAFFICTRACE,
  LIBLTE_S1AP_INITIATINGMESSAGE_CHOICE_UECONTEXTRELEASECOMMAND,
  LIBLTE_S1AP_INITIATINGMESSAGE_CHOICE_KILLREQUEST,
  LIBLTE_S1AP_INITIATINGMESSAGE_CHOICE_PRIVATEMESSAGE,
  LIBLTE_S1AP_INITIATINGMESSAGE_CHOICE_ENBSTATUSTRANSFER,
  LIBLTE_S1AP_INITIATINGMESSAGE_CHOICE_ERRORINDICATION,
  LIBLTE_S1AP_INITIATINGMESSAGE_CHOICE_N_ITEMS,
}LIBLTE_S1AP_INITIATINGMESSAGE_CHOICE_ENUM;
static const char liblte_s1ap_initiatingmessage_choice_text[LIBLTE_S1AP_INITIATINGMESSAGE_CHOICE_N_ITEMS][50] = {
  "LocationReportingControl",
  "DownlinkS1cdma2000tunneling",
  "S1SetupRequest",
  "UECapabilityInfoIndication",
  "LocationReport",
  "UplinkNonUEAssociatedLPPaTransport",
  "UplinkS1cdma2000tunneling",
  "MMEConfigurationTransfer",
  "TraceStart",
  "HandoverCancel",
  "UERadioCapabilityMatchRequest",
  "DownlinkNASTransport",
  "InitialContextSetupRequest",
  "HandoverRequired",
  "MMEDirectInformationTransfer",
  "TraceFailureIndication",
  "MMEConfigurationUpdate",
  "WriteReplaceWarningRequest",
  "ENBDirectInformationTransfer",
  "DownlinkUEAssociatedLPPaTransport",
  "E-RABReleaseCommand",
  "NASNonDeliveryIndication",
  "ENBConfigurationUpdate",
  "UplinkUEAssociatedLPPaTransport",
  "InitialUEMessage",
  "E-RABModifyRequest",
  "UEContextModificationRequest",
  "E-RABSetupRequest",
  "Reset",
  "OverloadStart",
  "E-RABReleaseIndication",
  "LocationReportingFailureIndication",
  "DeactivateTrace",
  "PathSwitchRequest",
  "HandoverRequest",
  "DownlinkNonUEAssociatedLPPaTransport",
  "OverloadStop",
  "Paging",
  "HandoverNotify",
  "PWSRestartIndication",
  "UEContextReleaseRequest",
  "UplinkNASTransport",
  "ENBConfigurationTransfer",
  "MMEStatusTransfer",
  "CellTrafficTrace",
  "UEContextReleaseCommand",
  "KillRequest",
  "PrivateMessage",
  "ENBStatusTransfer",
  "ErrorIndication",
};

typedef union{
  LIBLTE_S1AP_MESSAGE_LOCATIONREPORTINGCONTROL_STRUCT                 LocationReportingControl;
  LIBLTE_S1AP_MESSAGE_DOWNLINKS1CDMA2000TUNNELING_STRUCT              DownlinkS1cdma2000tunneling;
  LIBLTE_S1AP_MESSAGE_S1SETUPREQUEST_STRUCT                           S1SetupRequest;
  LIBLTE_S1AP_MESSAGE_UECAPABILITYINFOINDICATION_STRUCT               UECapabilityInfoIndication;
  LIBLTE_S1AP_MESSAGE_LOCATIONREPORT_STRUCT                           LocationReport;
  LIBLTE_S1AP_MESSAGE_UPLINKNONUEASSOCIATEDLPPATRANSPORT_STRUCT       UplinkNonUEAssociatedLPPaTransport;
  LIBLTE_S1AP_MESSAGE_UPLINKS1CDMA2000TUNNELING_STRUCT                UplinkS1cdma2000tunneling;
  LIBLTE_S1AP_MESSAGE_MMECONFIGURATIONTRANSFER_STRUCT                 MMEConfigurationTransfer;
  LIBLTE_S1AP_MESSAGE_TRACESTART_STRUCT                               TraceStart;
  LIBLTE_S1AP_MESSAGE_HANDOVERCANCEL_STRUCT                           HandoverCancel;
  LIBLTE_S1AP_MESSAGE_UERADIOCAPABILITYMATCHREQUEST_STRUCT            UERadioCapabilityMatchRequest;
  LIBLTE_S1AP_MESSAGE_DOWNLINKNASTRANSPORT_STRUCT                     DownlinkNASTransport;
  LIBLTE_S1AP_MESSAGE_INITIALCONTEXTSETUPREQUEST_STRUCT               InitialContextSetupRequest;
  LIBLTE_S1AP_MESSAGE_HANDOVERREQUIRED_STRUCT                         HandoverRequired;
  LIBLTE_S1AP_MESSAGE_MMEDIRECTINFORMATIONTRANSFER_STRUCT             MMEDirectInformationTransfer;
  LIBLTE_S1AP_MESSAGE_TRACEFAILUREINDICATION_STRUCT                   TraceFailureIndication;
  LIBLTE_S1AP_MESSAGE_MMECONFIGURATIONUPDATE_STRUCT                   MMEConfigurationUpdate;
  LIBLTE_S1AP_MESSAGE_WRITEREPLACEWARNINGREQUEST_STRUCT               WriteReplaceWarningRequest;
  LIBLTE_S1AP_MESSAGE_ENBDIRECTINFORMATIONTRANSFER_STRUCT             ENBDirectInformationTransfer;
  LIBLTE_S1AP_MESSAGE_DOWNLINKUEASSOCIATEDLPPATRANSPORT_STRUCT        DownlinkUEAssociatedLPPaTransport;
  LIBLTE_S1AP_MESSAGE_E_RABRELEASECOMMAND_STRUCT                      E_RABReleaseCommand;
  LIBLTE_S1AP_MESSAGE_NASNONDELIVERYINDICATION_STRUCT                 NASNonDeliveryIndication;
  LIBLTE_S1AP_MESSAGE_ENBCONFIGURATIONUPDATE_STRUCT                   ENBConfigurationUpdate;
  LIBLTE_S1AP_MESSAGE_UPLINKUEASSOCIATEDLPPATRANSPORT_STRUCT          UplinkUEAssociatedLPPaTransport;
  LIBLTE_S1AP_MESSAGE_INITIALUEMESSAGE_STRUCT                         InitialUEMessage;
  LIBLTE_S1AP_MESSAGE_E_RABMODIFYREQUEST_STRUCT                       E_RABModifyRequest;
  LIBLTE_S1AP_MESSAGE_UECONTEXTMODIFICATIONREQUEST_STRUCT             UEContextModificationRequest;
  LIBLTE_S1AP_MESSAGE_E_RABSETUPREQUEST_STRUCT                        E_RABSetupRequest;
  LIBLTE_S1AP_MESSAGE_RESET_STRUCT                                    Reset;
  LIBLTE_S1AP_MESSAGE_OVERLOADSTART_STRUCT                            OverloadStart;
  LIBLTE_S1AP_MESSAGE_E_RABRELEASEINDICATION_STRUCT                   E_RABReleaseIndication;
  LIBLTE_S1AP_MESSAGE_LOCATIONREPORTINGFAILUREINDICATION_STRUCT       LocationReportingFailureIndication;
  LIBLTE_S1AP_MESSAGE_DEACTIVATETRACE_STRUCT                          DeactivateTrace;
  LIBLTE_S1AP_MESSAGE_PATHSWITCHREQUEST_STRUCT                        PathSwitchRequest;
  LIBLTE_S1AP_MESSAGE_HANDOVERREQUEST_STRUCT                          HandoverRequest;
  LIBLTE_S1AP_MESSAGE_DOWNLINKNONUEASSOCIATEDLPPATRANSPORT_STRUCT     DownlinkNonUEAssociatedLPPaTransport;
  LIBLTE_S1AP_MESSAGE_OVERLOADSTOP_STRUCT                             OverloadStop;
  LIBLTE_S1AP_MESSAGE_PAGING_STRUCT                                   Paging;
  LIBLTE_S1AP_MESSAGE_HANDOVERNOTIFY_STRUCT                           HandoverNotify;
  LIBLTE_S1AP_MESSAGE_PWSRESTARTINDICATION_STRUCT                     PWSRestartIndication;
  LIBLTE_S1AP_MESSAGE_UECONTEXTRELEASEREQUEST_STRUCT                  UEContextReleaseRequest;
  LIBLTE_S1AP_MESSAGE_UPLINKNASTRANSPORT_STRUCT                       UplinkNASTransport;
  LIBLTE_S1AP_MESSAGE_ENBCONFIGURATIONTRANSFER_STRUCT                 ENBConfigurationTransfer;
  LIBLTE_S1AP_MESSAGE_MMESTATUSTRANSFER_STRUCT                        MMEStatusTransfer;
  LIBLTE_S1AP_MESSAGE_CELLTRAFFICTRACE_STRUCT                         CellTrafficTrace;
  LIBLTE_S1AP_MESSAGE_UECONTEXTRELEASECOMMAND_STRUCT                  UEContextReleaseCommand;
  LIBLTE_S1AP_MESSAGE_KILLREQUEST_STRUCT                              KillRequest;
  LIBLTE_S1AP_MESSAGE_PRIVATEMESSAGE_STRUCT                           PrivateMessage;
  LIBLTE_S1AP_MESSAGE_ENBSTATUSTRANSFER_STRUCT                        ENBStatusTransfer;
  LIBLTE_S1AP_MESSAGE_ERRORINDICATION_STRUCT                          ErrorIndication;
}LIBLTE_S1AP_INITIATINGMESSAGE_CHOICE_UNION;

typedef struct{
  uint8_t                                               procedureCode;
  LIBLTE_S1AP_CRITICALITY_ENUM                          criticality;
  LIBLTE_S1AP_INITIATINGMESSAGE_CHOICE_UNION            choice;
  LIBLTE_S1AP_INITIATINGMESSAGE_CHOICE_ENUM             choice_type;
}LIBLTE_S1AP_INITIATINGMESSAGE_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_initiatingmessage(
  LIBLTE_S1AP_INITIATINGMESSAGE_STRUCT *msg,
  uint8_t                             **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_initiatingmessage(
  uint8_t                             **ptr,
  LIBLTE_S1AP_INITIATINGMESSAGE_STRUCT *msg);

/*******************************************************************************
/* UnsuccessfulOutcome CHOICE
********************************************************************************/
typedef enum{
  LIBLTE_S1AP_UNSUCCESSFULOUTCOME_CHOICE_S1SETUPFAILURE,
  LIBLTE_S1AP_UNSUCCESSFULOUTCOME_CHOICE_PATHSWITCHREQUESTFAILURE,
  LIBLTE_S1AP_UNSUCCESSFULOUTCOME_CHOICE_UECONTEXTMODIFICATIONFAILURE,
  LIBLTE_S1AP_UNSUCCESSFULOUTCOME_CHOICE_INITIALCONTEXTSETUPFAILURE,
  LIBLTE_S1AP_UNSUCCESSFULOUTCOME_CHOICE_ENBCONFIGURATIONUPDATEFAILURE,
  LIBLTE_S1AP_UNSUCCESSFULOUTCOME_CHOICE_HANDOVERPREPARATIONFAILURE,
  LIBLTE_S1AP_UNSUCCESSFULOUTCOME_CHOICE_HANDOVERFAILURE,
  LIBLTE_S1AP_UNSUCCESSFULOUTCOME_CHOICE_MMECONFIGURATIONUPDATEFAILURE,
  LIBLTE_S1AP_UNSUCCESSFULOUTCOME_CHOICE_N_ITEMS,
}LIBLTE_S1AP_UNSUCCESSFULOUTCOME_CHOICE_ENUM;
static const char liblte_s1ap_unsuccessfuloutcome_choice_text[LIBLTE_S1AP_UNSUCCESSFULOUTCOME_CHOICE_N_ITEMS][50] = {
  "S1SetupFailure",
  "PathSwitchRequestFailure",
  "UEContextModificationFailure",
  "InitialContextSetupFailure",
  "ENBConfigurationUpdateFailure",
  "HandoverPreparationFailure",
  "HandoverFailure",
  "MMEConfigurationUpdateFailure",
};

typedef union{
  LIBLTE_S1AP_MESSAGE_S1SETUPFAILURE_STRUCT                           S1SetupFailure;
  LIBLTE_S1AP_MESSAGE_PATHSWITCHREQUESTFAILURE_STRUCT                 PathSwitchRequestFailure;
  LIBLTE_S1AP_MESSAGE_UECONTEXTMODIFICATIONFAILURE_STRUCT             UEContextModificationFailure;
  LIBLTE_S1AP_MESSAGE_INITIALCONTEXTSETUPFAILURE_STRUCT               InitialContextSetupFailure;
  LIBLTE_S1AP_MESSAGE_ENBCONFIGURATIONUPDATEFAILURE_STRUCT            ENBConfigurationUpdateFailure;
  LIBLTE_S1AP_MESSAGE_HANDOVERPREPARATIONFAILURE_STRUCT               HandoverPreparationFailure;
  LIBLTE_S1AP_MESSAGE_HANDOVERFAILURE_STRUCT                          HandoverFailure;
  LIBLTE_S1AP_MESSAGE_MMECONFIGURATIONUPDATEFAILURE_STRUCT            MMEConfigurationUpdateFailure;
}LIBLTE_S1AP_UNSUCCESSFULOUTCOME_CHOICE_UNION;

typedef struct{
  uint8_t                                                 procedureCode;
  LIBLTE_S1AP_CRITICALITY_ENUM                            criticality;
  LIBLTE_S1AP_UNSUCCESSFULOUTCOME_CHOICE_UNION            choice;
  LIBLTE_S1AP_UNSUCCESSFULOUTCOME_CHOICE_ENUM             choice_type;
}LIBLTE_S1AP_UNSUCCESSFULOUTCOME_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_unsuccessfuloutcome(
  LIBLTE_S1AP_UNSUCCESSFULOUTCOME_STRUCT *msg,
  uint8_t                               **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_unsuccessfuloutcome(
  uint8_t                               **ptr,
  LIBLTE_S1AP_UNSUCCESSFULOUTCOME_STRUCT *msg);

/*******************************************************************************
/* SuccessfulOutcome CHOICE
********************************************************************************/
typedef enum{
  LIBLTE_S1AP_SUCCESSFULOUTCOME_CHOICE_HANDOVERREQUESTACKNOWLEDGE,
  LIBLTE_S1AP_SUCCESSFULOUTCOME_CHOICE_UECONTEXTRELEASECOMPLETE,
  LIBLTE_S1AP_SUCCESSFULOUTCOME_CHOICE_UERADIOCAPABILITYMATCHRESPONSE,
  LIBLTE_S1AP_SUCCESSFULOUTCOME_CHOICE_INITIALCONTEXTSETUPRESPONSE,
  LIBLTE_S1AP_SUCCESSFULOUTCOME_CHOICE_E_RABSETUPRESPONSE,
  LIBLTE_S1AP_SUCCESSFULOUTCOME_CHOICE_PATHSWITCHREQUESTACKNOWLEDGE,
  LIBLTE_S1AP_SUCCESSFULOUTCOME_CHOICE_MMECONFIGURATIONUPDATEACKNOWLEDGE,
  LIBLTE_S1AP_SUCCESSFULOUTCOME_CHOICE_RESETACKNOWLEDGE,
  LIBLTE_S1AP_SUCCESSFULOUTCOME_CHOICE_ENBCONFIGURATIONUPDATEACKNOWLEDGE,
  LIBLTE_S1AP_SUCCESSFULOUTCOME_CHOICE_E_RABMODIFYRESPONSE,
  LIBLTE_S1AP_SUCCESSFULOUTCOME_CHOICE_WRITEREPLACEWARNINGRESPONSE,
  LIBLTE_S1AP_SUCCESSFULOUTCOME_CHOICE_S1SETUPRESPONSE,
  LIBLTE_S1AP_SUCCESSFULOUTCOME_CHOICE_KILLRESPONSE,
  LIBLTE_S1AP_SUCCESSFULOUTCOME_CHOICE_UECONTEXTMODIFICATIONRESPONSE,
  LIBLTE_S1AP_SUCCESSFULOUTCOME_CHOICE_HANDOVERCOMMAND,
  LIBLTE_S1AP_SUCCESSFULOUTCOME_CHOICE_HANDOVERCANCELACKNOWLEDGE,
  LIBLTE_S1AP_SUCCESSFULOUTCOME_CHOICE_E_RABRELEASERESPONSE,
  LIBLTE_S1AP_SUCCESSFULOUTCOME_CHOICE_N_ITEMS,
}LIBLTE_S1AP_SUCCESSFULOUTCOME_CHOICE_ENUM;
static const char liblte_s1ap_successfuloutcome_choice_text[LIBLTE_S1AP_SUCCESSFULOUTCOME_CHOICE_N_ITEMS][50] = {
  "HandoverRequestAcknowledge",
  "UEContextReleaseComplete",
  "UERadioCapabilityMatchResponse",
  "InitialContextSetupResponse",
  "E-RABSetupResponse",
  "PathSwitchRequestAcknowledge",
  "MMEConfigurationUpdateAcknowledge",
  "ResetAcknowledge",
  "ENBConfigurationUpdateAcknowledge",
  "E-RABModifyResponse",
  "WriteReplaceWarningResponse",
  "S1SetupResponse",
  "KillResponse",
  "UEContextModificationResponse",
  "HandoverCommand",
  "HandoverCancelAcknowledge",
  "E-RABReleaseResponse",
};

typedef union{
  LIBLTE_S1AP_MESSAGE_HANDOVERREQUESTACKNOWLEDGE_STRUCT               HandoverRequestAcknowledge;
  LIBLTE_S1AP_MESSAGE_UECONTEXTRELEASECOMPLETE_STRUCT                 UEContextReleaseComplete;
  LIBLTE_S1AP_MESSAGE_UERADIOCAPABILITYMATCHRESPONSE_STRUCT           UERadioCapabilityMatchResponse;
  LIBLTE_S1AP_MESSAGE_INITIALCONTEXTSETUPRESPONSE_STRUCT              InitialContextSetupResponse;
  LIBLTE_S1AP_MESSAGE_E_RABSETUPRESPONSE_STRUCT                       E_RABSetupResponse;
  LIBLTE_S1AP_MESSAGE_PATHSWITCHREQUESTACKNOWLEDGE_STRUCT             PathSwitchRequestAcknowledge;
  LIBLTE_S1AP_MESSAGE_MMECONFIGURATIONUPDATEACKNOWLEDGE_STRUCT        MMEConfigurationUpdateAcknowledge;
  LIBLTE_S1AP_MESSAGE_RESETACKNOWLEDGE_STRUCT                         ResetAcknowledge;
  LIBLTE_S1AP_MESSAGE_ENBCONFIGURATIONUPDATEACKNOWLEDGE_STRUCT        ENBConfigurationUpdateAcknowledge;
  LIBLTE_S1AP_MESSAGE_E_RABMODIFYRESPONSE_STRUCT                      E_RABModifyResponse;
  LIBLTE_S1AP_MESSAGE_WRITEREPLACEWARNINGRESPONSE_STRUCT              WriteReplaceWarningResponse;
  LIBLTE_S1AP_MESSAGE_S1SETUPRESPONSE_STRUCT                          S1SetupResponse;
  LIBLTE_S1AP_MESSAGE_KILLRESPONSE_STRUCT                             KillResponse;
  LIBLTE_S1AP_MESSAGE_UECONTEXTMODIFICATIONRESPONSE_STRUCT            UEContextModificationResponse;
  LIBLTE_S1AP_MESSAGE_HANDOVERCOMMAND_STRUCT                          HandoverCommand;
  LIBLTE_S1AP_MESSAGE_HANDOVERCANCELACKNOWLEDGE_STRUCT                HandoverCancelAcknowledge;
  LIBLTE_S1AP_MESSAGE_E_RABRELEASERESPONSE_STRUCT                     E_RABReleaseResponse;
}LIBLTE_S1AP_SUCCESSFULOUTCOME_CHOICE_UNION;

typedef struct{
  uint8_t                                               procedureCode;
  LIBLTE_S1AP_CRITICALITY_ENUM                          criticality;
  LIBLTE_S1AP_SUCCESSFULOUTCOME_CHOICE_UNION            choice;
  LIBLTE_S1AP_SUCCESSFULOUTCOME_CHOICE_ENUM             choice_type;
}LIBLTE_S1AP_SUCCESSFULOUTCOME_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_successfuloutcome(
  LIBLTE_S1AP_SUCCESSFULOUTCOME_STRUCT *msg,
  uint8_t                             **ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_successfuloutcome(
  uint8_t                             **ptr,
  LIBLTE_S1AP_SUCCESSFULOUTCOME_STRUCT *msg);

/*******************************************************************************
/* S1AP_PDU CHOICE
********************************************************************************/
typedef enum{
  LIBLTE_S1AP_S1AP_PDU_CHOICE_INITIATINGMESSAGE,
  LIBLTE_S1AP_S1AP_PDU_CHOICE_SUCCESSFULOUTCOME,
  LIBLTE_S1AP_S1AP_PDU_CHOICE_UNSUCCESSFULOUTCOME,
  LIBLTE_S1AP_S1AP_PDU_CHOICE_N_ITEMS,
}LIBLTE_S1AP_S1AP_PDU_CHOICE_ENUM;
static const char liblte_s1ap_s1ap_pdu_choice_text[LIBLTE_S1AP_S1AP_PDU_CHOICE_N_ITEMS][50] = {
  "initiatingMessage",
  "successfulOutcome",
  "unsuccessfulOutcome",
};

typedef union{
  LIBLTE_S1AP_INITIATINGMESSAGE_STRUCT                         initiatingMessage;
  LIBLTE_S1AP_SUCCESSFULOUTCOME_STRUCT                         successfulOutcome;
  LIBLTE_S1AP_UNSUCCESSFULOUTCOME_STRUCT                       unsuccessfulOutcome;
}LIBLTE_S1AP_S1AP_PDU_CHOICE_UNION;

typedef struct{
  bool ext;
  LIBLTE_S1AP_S1AP_PDU_CHOICE_UNION choice;
  LIBLTE_S1AP_S1AP_PDU_CHOICE_ENUM  choice_type;
}LIBLTE_S1AP_S1AP_PDU_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_s1ap_pdu(
  LIBLTE_S1AP_S1AP_PDU_STRUCT *s1ap_pdu,
  LIBLTE_BYTE_MSG_STRUCT      *msg);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_s1ap_pdu(
  LIBLTE_BYTE_MSG_STRUCT      *msg,
  LIBLTE_S1AP_S1AP_PDU_STRUCT *s1ap_pdu);
#endif // SRSLTE_LIBLTE_S1AP_H
