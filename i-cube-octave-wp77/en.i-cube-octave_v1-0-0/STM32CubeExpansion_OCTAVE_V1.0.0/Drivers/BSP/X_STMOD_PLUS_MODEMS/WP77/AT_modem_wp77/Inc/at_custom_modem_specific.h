/**
  ******************************************************************************
  * @file    at_custom_modem_specific.h
  * @author  Sierra Wireless Inc. and Affiliates
  * @brief   Header for at_custom_modem_specific.h module for WP77
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 Sierra Wireless Inc. and Affiliates
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by Sierra Wireless Inc. under the BSD 3-Clause license, the "License";
  *  You may not use this file except in compliance with the License. You may obtain a copy of the License at:
  *  opensource.org/licenses/BSD-3-Clause
  *
  * Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
  * 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * 3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
  * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
  * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
  * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef AT_CUSTOM_MODEM_WP77_H
#define AT_CUSTOM_MODEM_WP77_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "at_core.h"
#include "at_parser.h"
#include "at_modem_api.h"
#include "at_modem_signalling.h"
#include "cellular_service.h"
#include "cellular_service_int.h"
#include "ipc_common.h"

/* Exported constants --------------------------------------------------------*/
/* device specific parameters */
#define MODEM_MAX_SOCKET_TX_DATA_SIZE   CONFIG_MODEM_MAX_SOCKET_TX_DATA_SIZE /* cf AT+QISEND */
#define MODEM_MAX_SOCKET_RX_DATA_SIZE   CONFIG_MODEM_MAX_SOCKET_RX_DATA_SIZE /* cf AT+QIRD */
#define WP77_ACTIVATE_PING_REPORT       (1)

/* Exported types ------------------------------------------------------------*/

enum
{
  /* modem specific commands */
  CMD_AT_SELRAT = (CMD_AT_LAST_GENERIC + 1U), /*RAT selection / Query */
  CMD_AT_BAND,                           /* Band selection / Query */
  CMD_AT_SELACQ,                         /* Check RAT scan sequence */
  CMD_AT_WDSI,                           /* Check Air Vantage Connection settings */
  CMD_AT_KSREP,							 /* Modem Enable Boot Status */
  CMD_AT_KSUP,							 /* Modem Boot Status */
  /* WP77 specific TCP/IP commands */

  CMD_AT_SOCKET_PROMPT,                  /* when sending socket data : prompt = "> " */
  CMD_AT_SEND_OK,                        /* socket send data OK */
  CMD_AT_SEND_FAIL,                      /* socket send data problem */
  CMD_AT_CCID,                           /* show ICCID */
  CMD_AT_KCELL,                          /* query and report signal strength */
  CMD_AT_GSTATUS,                        /* check modem and RF status */

  /* modem specific events (URC, BOOT, ...) */
  CMD_AT_WAIT_EVENT,
  CMD_AT_BOOT_EVENT,
  CMD_AT_SIMREADY_EVENT,
  CMD_AT_RDY_EVENT,
  CMD_AT_APP_RDY_EVENT,
  CMD_AT_POWERED_DOWN_EVENT,
  CMD_AT_PSM_POWER_DOWN_EVENT,
 #if defined(USE_COM_MDM)
  CMD_AT_ORP,
 #endif /* defined(USE_COM_MDM) */

};

/* device specific parameters */
typedef enum
{
  QIURC_UNKNOWN,
  QIURC_CLOSED,
  QIURC_RECV,
  QIURC_INCOMING_FULL,
  QIURC_INCOMING,
  QIURC_PDPDEACT,
  QIURC_DNSGIP,
} ATCustom_WP77_QIURC_function_t;

typedef enum
{
  QCFG_unknown,
  QCFG_gprsattach,
  QCFG_nwscanseq,
  QCFG_nwscanmode,
  QCFG_iotopmode,
  QCFG_roamservice,
  QCFG_band,
  QCFG_servicedomain,
  QCFG_sgsn,
  QCFG_msc,
  QCFG_PDP_DuplicateChk,
  QCFG_urc_ri_ring,
  QCFG_urc_ri_smsincoming,
  QCFG_urc_ri_other,
  QCFG_signaltype,
  QCFG_urc_delay,
  QCFG_urc_psm,
} ATCustom_WP77_QCFG_function_t;

typedef enum
{
  QINDCFG_unknown,
  QINDCFG_all,
  QINDCFG_csq,
  QINDCFG_smsfull,
  QINDCFG_ring,
  QINDCFG_smsincoming,
} ATCustom_WP77_QINDCFG_function_t;

typedef enum
{
  QCSQ_unknown,
  QCSQ_noService,
  QCSQ_gsm,
  QCSQ_catM1,
  QCSQ_catNB1,
} ATCustom_WP77_QCSQ_sysmode_t;

/* QCINITSTAT defines */
#define QCINITSTAT_INITIALSTATE    ((uint32_t) 0x0)
#define QCINITSTAT_CPINREADY       ((uint32_t) 0x1)
#define QCINITSTAT_SMSINITCOMPLETE ((uint32_t) 0x2)
#define QCINITSTAT_PBINITCOMPLETE  ((uint32_t) 0x4)

typedef uint32_t ATCustom_WP77_QCFGbandGSM_t;
#define QCFGBANDGSM_NOCHANGE ((ATCustom_WP77_QCFGbandGSM_t) 0x0)
#define QCFGBANDGSM_900      ((ATCustom_WP77_QCFGbandGSM_t) 0x1)
#define QCFGBANDGSM_1800     ((ATCustom_WP77_QCFGbandGSM_t) 0x2)
#define QCFGBANDGSM_850      ((ATCustom_WP77_QCFGbandGSM_t) 0x4)
#define QCFGBANDGSM_1900     ((ATCustom_WP77_QCFGbandGSM_t) 0x8)
#define QCFGBANDGSM_ANY      ((ATCustom_WP77_QCFGbandGSM_t) 0xF)

typedef uint32_t ATCustom_WP77_QCFGbandCatM1_t;
/* CatM1 band bitmap is 64 bits long. We consider it as two 32 bits values.
*  0xMMMMMMMM.LLLLLLLL:
*     MMMMMMMM is the MSB part of the bitmap
*     LLLLLLLL is the LSB part of the bitmap
*/
/* LSB bitmap part */
#define QCFGBANDCATM1_B1_MSB       ((ATCustom_WP77_QCFGbandCatM1_t) 0x0)
#define QCFGBANDCATM1_B1_LSB       ((ATCustom_WP77_QCFGbandCatM1_t) 0x1)
#define QCFGBANDCATM1_B2_MSB       ((ATCustom_WP77_QCFGbandCatM1_t) 0x0)
#define QCFGBANDCATM1_B2_LSB       ((ATCustom_WP77_QCFGbandCatM1_t) 0x2)
#define QCFGBANDCATM1_B3_MSB       ((ATCustom_WP77_QCFGbandCatM1_t) 0x0)
#define QCFGBANDCATM1_B3_LSB       ((ATCustom_WP77_QCFGbandCatM1_t) 0x4)
#define QCFGBANDCATM1_B4_MSB       ((ATCustom_WP77_QCFGbandCatM1_t) 0x0)
#define QCFGBANDCATM1_B4_LSB       ((ATCustom_WP77_QCFGbandCatM1_t) 0x8)
#define QCFGBANDCATM1_B5_MSB       ((ATCustom_WP77_QCFGbandCatM1_t) 0x0)
#define QCFGBANDCATM1_B5_LSB       ((ATCustom_WP77_QCFGbandCatM1_t) 0x10)
#define QCFGBANDCATM1_B8_MSB       ((ATCustom_WP77_QCFGbandCatM1_t) 0x0)
#define QCFGBANDCATM1_B8_LSB       ((ATCustom_WP77_QCFGbandCatM1_t) 0x80)
#define QCFGBANDCATM1_B12_MSB      ((ATCustom_WP77_QCFGbandCatM1_t) 0x0)
#define QCFGBANDCATM1_B12_LSB      ((ATCustom_WP77_QCFGbandCatM1_t) 0x800)
#define QCFGBANDCATM1_B13_MSB      ((ATCustom_WP77_QCFGbandCatM1_t) 0x0)
#define QCFGBANDCATM1_B13_LSB      ((ATCustom_WP77_QCFGbandCatM1_t) 0x1000)
#define QCFGBANDCATM1_B18_MSB      ((ATCustom_WP77_QCFGbandCatM1_t) 0x0)
#define QCFGBANDCATM1_B18_LSB      ((ATCustom_WP77_QCFGbandCatM1_t) 0x20000)
#define QCFGBANDCATM1_B19_MSB      ((ATCustom_WP77_QCFGbandCatM1_t) 0x0)
#define QCFGBANDCATM1_B19_LSB      ((ATCustom_WP77_QCFGbandCatM1_t) 0x40000)
#define QCFGBANDCATM1_B20_MSB      ((ATCustom_WP77_QCFGbandCatM1_t) 0x0)
#define QCFGBANDCATM1_B20_LSB      ((ATCustom_WP77_QCFGbandCatM1_t) 0x80000)
#define QCFGBANDCATM1_B26_MSB      ((ATCustom_WP77_QCFGbandCatM1_t) 0x0)
#define QCFGBANDCATM1_B26_LSB      ((ATCustom_WP77_QCFGbandCatM1_t) 0x2000000)
#define QCFGBANDCATM1_B28_MSB      ((ATCustom_WP77_QCFGbandCatM1_t) 0x0)
#define QCFGBANDCATM1_B28_LSB      ((ATCustom_WP77_QCFGbandCatM1_t) 0x8000000)
#define QCFGBANDCATM1_NOCHANGE_MSB ((ATCustom_WP77_QCFGbandCatM1_t) 0x0)
#define QCFGBANDCATM1_NOCHANGE_LSB ((ATCustom_WP77_QCFGbandCatM1_t) 0x40000000)
/* MSB bitmap part */
#define QCFGBANDCATM1_B39_MSB      ((ATCustom_WP77_QCFGbandCatM1_t) 0x40)
#define QCFGBANDCATM1_B39_LSB      ((ATCustom_WP77_QCFGbandCatM1_t) 0x0)
/* ALL M1 bands bitmaps */
#define QCFGBANDCATM1_ANY_MSB  ((ATCustom_WP77_QCFGbandCatM1_t) 0x40)
#define QCFGBANDCATM1_ANY_LSB  ((ATCustom_WP77_QCFGbandCatM1_t) 0x0a0e189f)

typedef uint32_t ATCustom_WP77_QCFGbandCatNB1_t;
/* CatNB1 band bitmap is 64 bits long. We consider it as two 32 bits values.
*  0xMMMMMMMM.LLLLLLLL:
*     MMMMMMMM is the MSB part of the bitmap
*     LLLLLLLL is the LSB part of the bitmap
*/
/* LSB bitmap part */
#define QCFGBANDCATNB1_B1_MSB   ((ATCustom_WP77_QCFGbandCatNB1_t) 0x0)
#define QCFGBANDCATNB1_B1_LSB   ((ATCustom_WP77_QCFGbandCatNB1_t) 0x1)
#define QCFGBANDCATNB1_B2_MSB   ((ATCustom_WP77_QCFGbandCatNB1_t) 0x0)
#define QCFGBANDCATNB1_B2_LSB   ((ATCustom_WP77_QCFGbandCatNB1_t) 0x2)
#define QCFGBANDCATNB1_B3_MSB   ((ATCustom_WP77_QCFGbandCatNB1_t) 0x0)
#define QCFGBANDCATNB1_B3_LSB   ((ATCustom_WP77_QCFGbandCatNB1_t) 0x4)
#define QCFGBANDCATNB1_B4_MSB   ((ATCustom_WP77_QCFGbandCatNB1_t) 0x0)
#define QCFGBANDCATNB1_B4_LSB   ((ATCustom_WP77_QCFGbandCatNB1_t) 0x8)
#define QCFGBANDCATNB1_B5_MSB   ((ATCustom_WP77_QCFGbandCatNB1_t) 0x0)
#define QCFGBANDCATNB1_B5_LSB   ((ATCustom_WP77_QCFGbandCatNB1_t) 0x10)
#define QCFGBANDCATNB1_B8_MSB   ((ATCustom_WP77_QCFGbandCatNB1_t) 0x0)
#define QCFGBANDCATNB1_B8_LSB   ((ATCustom_WP77_QCFGbandCatNB1_t) 0x80)
#define QCFGBANDCATNB1_B12_MSB  ((ATCustom_WP77_QCFGbandCatNB1_t) 0x0)
#define QCFGBANDCATNB1_B12_LSB  ((ATCustom_WP77_QCFGbandCatNB1_t) 0x800)
#define QCFGBANDCATNB1_B13_MSB  ((ATCustom_WP77_QCFGbandCatNB1_t) 0x0)
#define QCFGBANDCATNB1_B13_LSB  ((ATCustom_WP77_QCFGbandCatNB1_t) 0x1000)
#define QCFGBANDCATNB1_B18_MSB  ((ATCustom_WP77_QCFGbandCatNB1_t) 0x0)
#define QCFGBANDCATNB1_B18_LSB  ((ATCustom_WP77_QCFGbandCatNB1_t) 0x20000)
#define QCFGBANDCATNB1_B19_MSB  ((ATCustom_WP77_QCFGbandCatNB1_t) 0x0)
#define QCFGBANDCATNB1_B19_LSB  ((ATCustom_WP77_QCFGbandCatNB1_t) 0x40000)
#define QCFGBANDCATNB1_B20_MSB  ((ATCustom_WP77_QCFGbandCatNB1_t) 0x0)
#define QCFGBANDCATNB1_B20_LSB  ((ATCustom_WP77_QCFGbandCatNB1_t) 0x80000)
#define QCFGBANDCATNB1_B26_MSB  ((ATCustom_WP77_QCFGbandCatNB1_t) 0x0)
#define QCFGBANDCATNB1_B26_LSB  ((ATCustom_WP77_QCFGbandCatNB1_t) 0x2000000)
#define QCFGBANDCATNB1_B28_MSB  ((ATCustom_WP77_QCFGbandCatNB1_t) 0x0)
#define QCFGBANDCATNB1_B28_LSB  ((ATCustom_WP77_QCFGbandCatNB1_t) 0x8000000)
#define QCFGBANDCATNB1_NOCHANGE_MSB ((ATCustom_WP77_QCFGbandCatNB1_t) 0x0)
#define QCFGBANDCATNB1_NOCHANGE_LSB ((ATCustom_WP77_QCFGbandCatNB1_t) 0x40000000)
/* MSB bitmap part - not used for the moment */
/* ALL NB1 bands bitmaps */
#define QCFGBANDCATNB1_ANY_MSB  ((ATCustom_WP77_QCFGbandCatNB1_t) 0x0)
#define QCFGBANDCATNB1_ANY_LSB  ((ATCustom_WP77_QCFGbandCatNB1_t) 0xA0E189F)

typedef uint32_t ATCustom_WP77_QCFGiotopmode_t;
#define QCFGIOTOPMODE_CATM1       ((ATCustom_WP77_QCFGiotopmode_t) 0x0)
#define QCFGIOTOPMODE_CATNB1      ((ATCustom_WP77_QCFGiotopmode_t) 0x1)
#define QCFGIOTOPMODE_CATM1CATNB1 ((ATCustom_WP77_QCFGiotopmode_t) 0x2)

typedef uint32_t ATCustom_WP77_QCFGscanmode_t;
#define SCANMODE_AUTO     ((ATCustom_WP77_QCFGscanmode_t) 0x0)
#define SCANMODE_UMTS     ((ATCustom_WP77_QCFGscanmode_t) 0x1)
#define SCANMODE_GSMONLY  ((ATCustom_WP77_QCFGscanmode_t) 0x2)
#define SCANMODE_UMTS_GSM  ((ATCustom_WP77_QCFGscanmode_t) 0x5)
#define SCANMODE_LTEONLY  ((ATCustom_WP77_QCFGscanmode_t) 0x6)
#define SCANMODE_LTE_UMTS_GSM  ((ATCustom_WP77_QCFGscanmode_t) 0x7)
#define SCANMODE_UMTS_LTE  ((ATCustom_WP77_QCFGscanmode_t) 0x11)
#define SCANMODE_GSM_LTE  ((ATCustom_WP77_QCFGscanmode_t) 0x12)
#define QCFGSCANMODE_AUTO     ((ATCustom_WP77_QCFGscanmode_t) 0x13)
#define QCFGSCANMODE_GSMONLY  ((ATCustom_WP77_QCFGscanmode_t) 0x14)
#define QCFGSCANMODE_LTEONLY  ((ATCustom_WP77_QCFGscanmode_t) 0x15)



typedef uint32_t ATCustom_WP77_QCFGscanseq_t;
/*individual values*/
#define  QCFGSCANSEQ_AUTO        ((ATCustom_WP77_QCFGscanseq_t) 0x0)
#define  QCFGSCANSEQ_GSM         ((ATCustom_WP77_QCFGscanseq_t) 0x1)
#define  QCFGSCANSEQ_LTECATM1    ((ATCustom_WP77_QCFGscanseq_t) 0x2)
#define  QCFGSCANSEQ_LTECATNB1   ((ATCustom_WP77_QCFGscanseq_t) 0x3)
/*combined values*/
#define  QCFGSCANSEQ_GSM_M1_NB1 ((ATCustom_WP77_QCFGscanseq_t) 0x010203)
#define  QCFGSCANSEQ_GSM_NB1_M1 ((ATCustom_WP77_QCFGscanseq_t) 0x010302)
#define  QCFGSCANSEQ_M1_GSM_NB1 ((ATCustom_WP77_QCFGscanseq_t) 0x020103)
#define  QCFGSCANSEQ_M1_NB1_GSM ((ATCustom_WP77_QCFGscanseq_t) 0x020301)
#define  QCFGSCANSEQ_NB1_GSM_M1 ((ATCustom_WP77_QCFGscanseq_t) 0x030102)
#define  QCFGSCANSEQ_NB1_M1_GSM ((ATCustom_WP77_QCFGscanseq_t) 0x030201)


typedef enum
{
  HOST_LP_STATE_IDLE,               /* state 0 */
  HOST_LP_STATE_LP_REQUIRED,        /* state 1 */
  HOST_LP_STATE_LP_ONGOING,         /* state 2 */
  HOST_LP_STATE_WAKEUP_REQUIRED,    /* state 3 */
  HOST_LP_STATE_ERROR,              /* state 4 */
} ATCustom_WP77_Host_LP_state_t;

typedef enum
{
  MDM_LP_STATE_IDLE,             /* state 0 */
  MDM_LP_STATE_PSM,              /* state 1 */
  MDM_LP_STATE_ERROR,            /* state 2 */
} ATCustom_WP77_Modem_LP_state_t;

#if (ENABLE_WP77_LOW_POWER_MODE != 0U)
typedef enum
{
  EVENT_LP_HOST_SLEEP_REQ,        /* event 0 */
  EVENT_LP_HOST_SLEEP_CANCEL,     /* event 1 */
  EVENT_LP_HOST_SLEEP_COMPLETE,   /* event 2 */
  EVENT_LP_HOST_WAKEUP_REQ,       /* event 3 */
  EVENT_LP_MDM_WAKEUP_REQ,        /* event 4 */
  EVENT_LP_MDM_ENTER_PSM,         /* event 5 */
  EVENT_LP_MDM_LEAVE_PSM,         /* event 6 */

} ATCustom_WP77_LP_event_t;
#endif /* ENABLE_WP77_LOW_POWER_MODE != 0U */

typedef struct
{
  ATCustom_WP77_QCFGscanseq_t    nw_scanseq;
  ATCustom_WP77_QCFGscanmode_t   nw_scanmode;
  ATCustom_WP77_QCFGiotopmode_t  iot_op_mode;
  ATCustom_WP77_QCFGbandGSM_t    gsm_bands;
  AT_CHAR_t 					 band[50];
  ATCustom_WP77_QCFGbandCatM1_t  CatM1_bands_MsbPart;
  ATCustom_WP77_QCFGbandCatM1_t  CatM1_bands_LsbPart;
  ATCustom_WP77_QCFGbandCatNB1_t CatNB1_bands_MsbPart;
  ATCustom_WP77_QCFGbandCatNB1_t CatNB1_bands_LsbPart;
} ATCustom_WP77_mode_band_config_t;

/*typedef struct
{
  ATCustom_WP77_scanseq_t    nw_scanseq;
  ATCustom_WP77_QCFGscanmode_t   nw_scanmode;
  ATCustom_WP77_QCFGiotopmode_t  iot_op_mode;
  AT_CHAR_t band[50];  = Band string
  ATCustom_WP77_QCFGbandGSM_t    gsm_bands;
  ATCustom_WP77_QCFGbandCatM1_t  CatM1_bands_MsbPart;
  ATCustom_WP77_QCFGbandCatM1_t  CatM1_bands_LsbPart;
  ATCustom_WP77_QCFGbandCatNB1_t CatNB1_bands_MsbPart;
  ATCustom_WP77_QCFGbandCatNB1_t CatNB1_bands_LsbPart;
} ATCustom_WP77_mode_band_config_t;*/

/* Engineering Mode - Cell Type information */
#define QENG_CELLTYPE_SERVINGCELL    (uint8_t)(0x0U) /* get 2G or 3G serving cell information */
#define QENG_CELLTYPE_NEIGHBOURCELL  (uint8_t)(0x1U) /* get 2G or 3G neighbour cell information */
#define QENG_CELLTYPE_PSINFO         (uint8_t)(0x2U) /* get 2G or 3G cell information during packet switch connected */

typedef struct
{
  at_bool_t finished;
  at_bool_t wait_header; /* indicate we are waiting for +QIURC: "dnsgip",<err>,<IP_count>,<DNS_ttl> */
  uint32_t  error;
  uint32_t  ip_count;
  uint32_t  ttl;
  AT_CHAR_t hostIPaddr[MAX_SIZE_IPADDR]; /* = host_name parameter from CS_DnsReq_t */
} WP77_qiurc_dnsgip_t;

typedef struct
{
  ATCustom_WP77_mode_band_config_t  mode_and_bands_config;  /* memorize current WP77 mode and bands configuration */
  ATCustom_WP77_QCFG_function_t     QCFG_command_param;
  at_bool_t                         QCFG_command_write;
  ATCustom_WP77_QINDCFG_function_t  QINDCFG_command_param;
  at_bool_t                         QIOPEN_waiting;         /* memorize if waiting for QIOPEN */
  uint8_t                           QIOPEN_current_socket_connected;
  at_bool_t                  QICGSP_config_command;  /* memorize if QICSGP write command is a config or a query cmd */
  WP77_qiurc_dnsgip_t        QIURC_dnsgip_param;     /* memorize infos received in the URC +QIURC:"dnsgip" */
  at_bool_t                  QINISTAT_error;         /* memorize AT+QINISTAT has reported an error  */
  uint8_t                    WP77_sim_status_retries; /* memorize number of attempts to access SIM (with QINISTAT) */
#if (USE_SOCKETS_TYPE == USE_SOCKETS_MODEM)
  at_bool_t                  pdn_already_active; /* check if request PDN to activate is already active (QIACT) */
#endif /* USE_SOCKETS_TYPE */
  ATCustom_WP77_Host_LP_state_t   host_lp_state;         /* to manage automaton Host Low Power state */
  ATCustom_WP77_Modem_LP_state_t  modem_lp_state;        /* to manage automaton Modem Low Power state */
  at_bool_t                       modem_resume_from_PSM; /* indicates that modem has just leave PSM */
  at_bool_t                  avms_connection_status;     /* check if avms connection status */
} WP77_shared_variables_t;

/* External variables --------------------------------------------------------*/
extern WP77_shared_variables_t WP77_shared;

/* Exported macros -----------------------------------------------------------*/

/* Exported functions ------------------------------------------------------- */

/* Exported functions ------------------------------------------------------- */
void        ATCustom_WP77_init(atparser_context_t *p_atp_ctxt);
uint8_t     ATCustom_WP77_checkEndOfMsgCallback(uint8_t rxChar);
at_status_t ATCustom_WP77_getCmd(at_context_t *p_at_ctxt, uint32_t *p_ATcmdTimeout);
at_endmsg_t ATCustom_WP77_extractElement(atparser_context_t *p_atp_ctxt,
                                         const IPC_RxMessage_t *p_msg_in,
                                         at_element_info_t *element_infos);
at_action_rsp_t ATCustom_WP77_analyzeCmd(at_context_t *p_at_ctxt,
                                         const IPC_RxMessage_t *p_msg_in,
                                         at_element_info_t *element_infos);
at_action_rsp_t ATCustom_WP77_analyzeParam(at_context_t *p_at_ctxt,
                                           const IPC_RxMessage_t *p_msg_in,
                                           at_element_info_t *element_infos);
at_action_rsp_t ATCustom_WP77_terminateCmd(atparser_context_t *p_atp_ctxt, at_element_info_t *element_infos);

at_status_t ATCustom_WP77_get_rsp(atparser_context_t *p_atp_ctxt, at_buf_t *p_rsp_buf);
at_status_t ATCustom_WP77_get_urc(atparser_context_t *p_atp_ctxt, at_buf_t *p_rsp_buf);
at_status_t ATCustom_WP77_get_error(atparser_context_t *p_atp_ctxt, at_buf_t *p_rsp_buf);
at_status_t ATCustom_WP77_hw_event(sysctrl_device_type_t deviceType, at_hw_event_t hwEvent, GPIO_PinState gstate);

#ifdef __cplusplus
}
#endif

#endif /* AT_CUSTOM_MODEM_WP77_H */

/************************ (C) COPYRIGHT Sierra Wireless *****END OF FILE****/
