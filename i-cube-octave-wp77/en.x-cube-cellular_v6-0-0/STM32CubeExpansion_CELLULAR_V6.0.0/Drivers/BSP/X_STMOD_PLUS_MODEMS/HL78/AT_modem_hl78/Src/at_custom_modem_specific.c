/**
  ******************************************************************************
  * @file    at_custom_modem_specific.c
  * @author  Sierra Wireless Inc. and Affiliates
  * @brief   This file provides all the specific code to the
  *          HL78 Sierra Wireless modem: LTE-cat-M1 or LTE-cat.NB1(=NB-IOT) or GSM
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

/* AT commands format
 * AT+<X>=?    : TEST COMMAND
 * AT+<X>?     : READ COMMAND
 * AT+<X>=...  : WRITE COMMAND
 * AT+<X>      : EXECUTION COMMAND
*/

/* HL78 COMPILATION FLAGS to define in project option if needed:*/

/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include "at_modem_api.h"
#include "at_modem_common.h"
#include "at_modem_signalling.h"
#include "at_modem_socket.h"
#include "at_custom_modem_specific.h"
#include "at_custom_modem_signalling.h"
#include "at_custom_modem_socket.h"
#include "at_datapack.h"
#include "at_util.h"
#include "sysctrl_specific.h"
#include "cellular_runtime_standard.h"
#include "cellular_runtime_custom.h"
#include "plf_config.h"
#include "plf_modem_config.h"
#include "error_handler.h"

#if defined(USE_MODEM_HL78)
#if defined(HWREF_B_CELL_HL78_V2)
#else
#error Hardware reference not specified
#endif /* HWREF_B_CELL_HL78_V2 */
#endif /* USE_MODEM_HL78 */

/* Private typedef -----------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/
#if (USE_TRACE_ATCUSTOM_SPECIFIC == 1U)
#if (USE_PRINTF == 0U)
#include "trace_interface.h"
#define PRINT_INFO(format, args...) TRACE_PRINT(DBG_CHAN_ATCMD, DBL_LVL_P0, "HL78:" format "\n\r", ## args)
#define PRINT_DBG(format, args...)  TRACE_PRINT(DBG_CHAN_ATCMD, DBL_LVL_P1, "HL78:" format "\n\r", ## args)
#define PRINT_API(format, args...)  TRACE_PRINT(DBG_CHAN_ATCMD, DBL_LVL_P2, "HL78 API:" format "\n\r", ## args)
#define PRINT_ERR(format, args...)  TRACE_PRINT(DBG_CHAN_ATCMD, DBL_LVL_ERR, "HL78 ERROR:" format "\n\r", ## args)
#else
#define PRINT_INFO(format, args...)  (void) printf("HL78:" format "\n\r", ## args);
#define PRINT_DBG(...) __NOP(); /* Nothing to do */
#define PRINT_API(...) __NOP(); /* Nothing to do */
#define PRINT_ERR(format, args...)   (void) printf("HL78 ERROR:" format "\n\r", ## args);
#endif /* USE_PRINTF */
#else
#define PRINT_INFO(...) __NOP(); /* Nothing to do */
#define PRINT_DBG(...)  __NOP(); /* Nothing to do */
#define PRINT_API(...)  __NOP(); /* Nothing to do */
#define PRINT_ERR(...)  __NOP(); /* Nothing to do */
#endif /* USE_TRACE_ATCUSTOM_SPECIFIC */

/* Private defines -----------------------------------------------------------*/
#if (ENABLE_HL78_LOW_POWER_MODE == 1U)
/* add a pre-step */
#define CHECK_STEP(stepval) ((p_atp_ctxt->step) == ((stepval)+1U))
#define CHECK_STEP_EXCEEDS(stepval) (p_atp_ctxt->step >= ((stepval)+1U))
#define CHECK_STEP_BETWEEN(low_step, high_step) ((p_atp_ctxt->step >= ((low_step)+1U)) &&\
                                                 (p_atp_ctxt->step <= ((high_step)+1U)))
#else
#define CHECK_STEP(stepval) (p_atp_ctxt->step == stepval)
#define CHECK_STEP_EXCEEDS(stepval) (p_atp_ctxt->step >= stepval)
#define CHECK_STEP_BETWEEN(low_step, high_step) ((p_atp_ctxt->step >= low_step) && (p_atp_ctxt->step <= high_step))
#endif /* ENABLE_HL78_LOW_POWER_MODE == 1U */

/* ###########################  START CUSTOMIZATION PART  ########################### */
#define HL78_DEFAULT_TIMEOUT  ((uint32_t)15000U)
#define HL78_RDY_TIMEOUT      ((uint32_t)30000U)
#define HL78_APP_RDY_TIMEOUT  ((uint32_t)10000U)
#define HL78_SIMREADY_TIMEOUT ((uint32_t)3000U)
#define HL78_ESCAPE_TIMEOUT   ((uint32_t)1000U)  /* maximum time allowed to receive a response to an Escape command */
#define HL78_COPS_TIMEOUT     ((uint32_t)180000U) /* 180 sec */
#define HL78_CGATT_TIMEOUT    ((uint32_t)140000U) /* 140 sec */
#define HL78_CGACT_TIMEOUT    ((uint32_t)150000U) /* 150 sec */
#define HL78_ATH_TIMEOUT      ((uint32_t)90000U) /* 90 sec */
#define HL78_AT_TIMEOUT       ((uint32_t)1000U)  /* timeout for AT */
#define HL78_SOCKET_PROMPT_TIMEOUT ((uint32_t)10000U)
#define HL78_QIOPEN_TIMEOUT   ((uint32_t)150000U) /* 150 sec */
#define HL78_QICLOSE_TIMEOUT  ((uint32_t)150000U) /* 150 sec */
#define HL78_QIACT_TIMEOUT    ((uint32_t)150000U) /* 150 sec */
#define HL78_QIDEACT_TIMEOUT  ((uint32_t)40000U) /* 40 sec */
#define HL78_QNWINFO_TIMEOUT  ((uint32_t)1000U) /* 1000ms */
#define HL78_QIDNSGIP_TIMEOUT ((uint32_t)60000U) /* 60 sec */
#define HL78_QPING_TIMEOUT    ((uint32_t)150000U) /* 150 sec */
#define HL78_CPSMS_TIMEOUT    ((uint32_t)60000)
#define HL78_CEDRX_TIMEOUT    ((uint32_t)60000)

#define HL78_MODEM_SYNCHRO_AT_MAX_RETRIES ((uint8_t)30U)
#define HL78_MAX_SIM_STATUS_RETRIES       ((uint8_t)20U) /* maximum number of AT+QINISTAT retries to wait SIM ready
                                                          * multiply by HL78_SIMREADY_TIMEOUT to compute global
                                                          * timeout value
                                                          */

#if !defined(HL78_OPTION_NETWORK_INFO)
/* set default value */
#define HL78_OPTION_NETWORK_INFO (0)
#endif /* HL78_OPTION_NETWORK_INFO */

#if !defined(HL78_OPTION_ENGINEERING_MODE)
/* set default value */
#define HL78_OPTION_ENGINEERING_MODE (0)
#endif /* HL78_OPTION_ENGINEERING_MODE */

/* Global variables ----------------------------------------------------------*/
/* HL78 Modem device context */
static atcustom_modem_context_t HL78_ctxt;

/* shared variables specific to HL78 */
HL78_shared_variables_t HL78_shared =
{
  .QCFG_command_param              = QCFG_unknown,
  .QCFG_command_write              = AT_FALSE,
  .QINDCFG_command_param           = QINDCFG_unknown,
  .QIOPEN_waiting                  = AT_FALSE,
  .QIOPEN_current_socket_connected = 0U,
  .QICGSP_config_command           = AT_TRUE,
  .HL78_sim_status_retries         = 0U,
#if (USE_SOCKETS_TYPE == USE_SOCKETS_MODEM)
  .pdn_already_active              = false,
#endif /* USE_SOCKETS_TYPE */
  .host_lp_state                   = HOST_LP_STATE_IDLE,
  .modem_lp_state                  = MDM_LP_STATE_IDLE,
  .modem_resume_from_PSM           = false,
};

/* Private variables ---------------------------------------------------------*/

/* Socket Data receive: to analyze size received in data header */
static AT_CHAR_t SocketHeaderDataRx_Buf[4];
static uint8_t SocketHeaderDataRx_Cpt;
static uint8_t SocketHeaderDataRx_Cpt_Complete;

/* ###########################  END CUSTOMIZATION PART  ########################### */

/* Private function prototypes -----------------------------------------------*/
/* ###########################  START CUSTOMIZATION PART  ########################### */
static void HL78_modem_init(atcustom_modem_context_t *p_modem_ctxt);
static void HL78_modem_reset(atcustom_modem_context_t *p_modem_ctxt);
static void reinitSyntaxAutomaton_HL78(void);
static void reset_variables_HL78(void);
static void init_HL78_qiurc_dnsgip(void);
static void socketHeaderRX_reset(void);
static void SocketHeaderRX_addChar(CRC_CHAR_t *rxchar);
static uint16_t SocketHeaderRX_getSize(void);

static void display_decoded_GSM_bands(uint32_t gsm_bands);
static void display_decoded_CatM1_bands(uint32_t CatM1_bands_MsbPart, uint32_t CatM1_bands_LsbPart);
static void display_decoded_CatNB1_bands(uint32_t CatNB1_bands_MsbPart, uint32_t CatNB1_bands_LsbPart);
static void display_user_friendly_mode_and_bands_config(void);
static uint8_t display_if_active_band(ATCustom_HL78_QCFGscanseq_t scanseq,
                                      uint8_t rank, uint8_t catM1_on, uint8_t catNB1_on, uint8_t gsm_on);

#if (ENABLE_HL78_LOW_POWER_MODE == 1U)
static at_bool_t init_HL78_low_power(atcustom_modem_context_t *p_modem_ctxt);
static at_bool_t set_HL78_low_power(atcustom_modem_context_t *p_modem_ctxt);
static void low_power_event(ATCustom_HL78_LP_event_t event);
static bool is_modem_in_PSM_state(void);
static bool is_waiting_modem_wakeup(void);

#endif /* ENABLE_HL78_LOW_POWER_MODE == 1U */

/* ###########################  END CUSTOMIZATION PART  ########################### */

/* Functions Definition ------------------------------------------------------*/
void ATCustom_HL78_init(atparser_context_t *p_atp_ctxt)
{
  /* Commands Look-up table */
  static const atcustom_LUT_t ATCMD_HL78_LUT[] =
  {
    /* cmd enum - cmd string - cmd timeout (in ms) - build cmd ftion - analyze cmd ftion */
    {CMD_AT,             "",             HL78_AT_TIMEOUT,       fCmdBuild_NoParams,   fRspAnalyze_None},
    {CMD_AT_OK,          "OK",           HL78_DEFAULT_TIMEOUT,  fCmdBuild_NoParams,   fRspAnalyze_None},
    {CMD_AT_CONNECT,     "CONNECT",      HL78_DEFAULT_TIMEOUT,  fCmdBuild_NoParams,   fRspAnalyze_None},
    {CMD_AT_RING,        "RING",         HL78_DEFAULT_TIMEOUT,  fCmdBuild_NoParams,   fRspAnalyze_None},
    {CMD_AT_NO_CARRIER,  "NO CARRIER",   HL78_DEFAULT_TIMEOUT,  fCmdBuild_NoParams,   fRspAnalyze_None},
    {CMD_AT_ERROR,       "ERROR",        HL78_DEFAULT_TIMEOUT,  fCmdBuild_NoParams,   fRspAnalyze_Error_HL78},
    {CMD_AT_NO_DIALTONE, "NO DIALTONE",  HL78_DEFAULT_TIMEOUT,  fCmdBuild_NoParams,   fRspAnalyze_None},
    {CMD_AT_BUSY,        "BUSY",         HL78_DEFAULT_TIMEOUT,  fCmdBuild_NoParams,   fRspAnalyze_None},
    {CMD_AT_NO_ANSWER,   "NO ANSWER",    HL78_DEFAULT_TIMEOUT,  fCmdBuild_NoParams,   fRspAnalyze_None},
    {CMD_AT_CME_ERROR,   "+CME ERROR",   HL78_DEFAULT_TIMEOUT,  fCmdBuild_NoParams,   fRspAnalyze_Error_HL78},
    {CMD_AT_CMS_ERROR,   "+CMS ERROR",   HL78_DEFAULT_TIMEOUT,  fCmdBuild_NoParams,   fRspAnalyze_CmsErr},

    /* GENERIC MODEM commands */
    {CMD_AT_CGMI,        "+CGMI",        HL78_DEFAULT_TIMEOUT,  fCmdBuild_NoParams,   fRspAnalyze_CGMI},
    {CMD_AT_CGMM,        "+CGMM",        HL78_DEFAULT_TIMEOUT,  fCmdBuild_NoParams,   fRspAnalyze_CGMM},
    {CMD_AT_CGMR,        "+CGMR",        HL78_DEFAULT_TIMEOUT,  fCmdBuild_NoParams,   fRspAnalyze_CGMR},
    {CMD_AT_CGSN,        "+CGSN",        HL78_DEFAULT_TIMEOUT,  fCmdBuild_CGSN_HL78,  fRspAnalyze_CGSN},
    {CMD_AT_GSN,         "+GSN",         HL78_DEFAULT_TIMEOUT,  fCmdBuild_NoParams,   fRspAnalyze_GSN},
    {CMD_AT_CIMI,        "+CIMI",        HL78_DEFAULT_TIMEOUT,  fCmdBuild_NoParams,   fRspAnalyze_CIMI},
    {CMD_AT_CEER,        "+CEER",        HL78_DEFAULT_TIMEOUT,  fCmdBuild_NoParams,   fRspAnalyze_CEER},
    {CMD_AT_CMEE,        "+CMEE",        HL78_DEFAULT_TIMEOUT,  fCmdBuild_CMEE,       fRspAnalyze_None},
    {CMD_AT_CPIN,        "+CPIN",        HL78_DEFAULT_TIMEOUT,  fCmdBuild_CPIN,       fRspAnalyze_CPIN},
    {CMD_AT_CFUN,        "+CFUN",        HL78_DEFAULT_TIMEOUT,  fCmdBuild_CFUN,       fRspAnalyze_CFUN_HL78},
    {CMD_AT_COPS,        "+COPS",        HL78_COPS_TIMEOUT,     fCmdBuild_COPS,  	  fRspAnalyze_COPS},
    {CMD_AT_CNUM,        "+CNUM",        HL78_DEFAULT_TIMEOUT,  fCmdBuild_NoParams,   fRspAnalyze_CNUM},
    {CMD_AT_CGATT,       "+CGATT",       HL78_CGATT_TIMEOUT,    fCmdBuild_CGATT,      fRspAnalyze_CGATT},
    {CMD_AT_CGPADDR,     "+CGPADDR",     HL78_DEFAULT_TIMEOUT,  fCmdBuild_CGPADDR,    fRspAnalyze_CGPADDR},
    {CMD_AT_CEREG,       "+CEREG",       HL78_DEFAULT_TIMEOUT,  fCmdBuild_CEREG,      fRspAnalyze_CEREG},
    {CMD_AT_CREG,        "+CREG",        HL78_DEFAULT_TIMEOUT,  fCmdBuild_CREG,       fRspAnalyze_CREG},
    {CMD_AT_CGREG,       "+CGREG",       HL78_DEFAULT_TIMEOUT,  fCmdBuild_CGREG,      fRspAnalyze_CGREG},
    {CMD_AT_CSQ,         "+CSQ",         HL78_DEFAULT_TIMEOUT,  fCmdBuild_NoParams,   fRspAnalyze_CSQ},
    {CMD_AT_CGDCONT,     "+CGDCONT",     HL78_DEFAULT_TIMEOUT,  fCmdBuild_CGDCONT_HL78,    fRspAnalyze_None},
    {CMD_AT_CGACT,       "+CGACT",       HL78_CGACT_TIMEOUT,    fCmdBuild_CGACT,      fRspAnalyze_None},
    {CMD_AT_CGDATA,      "+CGDATA",      HL78_DEFAULT_TIMEOUT,  fCmdBuild_CGDATA,     fRspAnalyze_None},
    {CMD_AT_CGEREP,      "+CGEREP",      HL78_DEFAULT_TIMEOUT,  fCmdBuild_CGEREP,     fRspAnalyze_None},
    {CMD_AT_CGEV,        "+CGEV",        HL78_DEFAULT_TIMEOUT,  fCmdBuild_NoParams,   fRspAnalyze_CGEV},
    {CMD_ATD,            "D",            HL78_DEFAULT_TIMEOUT,  fCmdBuild_ATD_HL78,   fRspAnalyze_None},
    {CMD_ATE,            "E",            HL78_DEFAULT_TIMEOUT,  fCmdBuild_ATE,        fRspAnalyze_None},
    {CMD_ATH,            "H",            HL78_ATH_TIMEOUT,      fCmdBuild_NoParams,   fRspAnalyze_None},
    {CMD_ATO,            "O",            HL78_DEFAULT_TIMEOUT,  fCmdBuild_NoParams,   fRspAnalyze_None},
    {CMD_ATV,            "V",            HL78_DEFAULT_TIMEOUT,  fCmdBuild_ATV,        fRspAnalyze_None},
    {CMD_ATX,            "X",            HL78_DEFAULT_TIMEOUT,  fCmdBuild_ATX,        fRspAnalyze_None},
    {CMD_AT_ESC_CMD,     "+++",          HL78_ESCAPE_TIMEOUT,   fCmdBuild_ESCAPE_CMD, fRspAnalyze_None},
    {CMD_AT_IPR,         "+IPR",         HL78_DEFAULT_TIMEOUT,  fCmdBuild_IPR,        fRspAnalyze_IPR},
    {CMD_AT_IFC,         "+IFC",         HL78_DEFAULT_TIMEOUT,  fCmdBuild_IFC,        fRspAnalyze_None},
    {CMD_AT_AND_W,       "&W",           HL78_DEFAULT_TIMEOUT,  fCmdBuild_NoParams,   fRspAnalyze_None},
    {CMD_AT_AND_D,       "&D",           HL78_DEFAULT_TIMEOUT,  fCmdBuild_AT_AND_D,   fRspAnalyze_None},
    {CMD_AT_DIRECT_CMD,  "",             HL78_DEFAULT_TIMEOUT,  fCmdBuild_DIRECT_CMD, fRspAnalyze_DIRECT_CMD},
    {CMD_AT_CSIM,        "+CSIM",        HL78_DEFAULT_TIMEOUT,  fCmdBuild_CSIM,       fRspAnalyze_CSIM},
	{CMD_AT_CCID,        "+CCID",        HL78_DEFAULT_TIMEOUT,  fCmdBuild_NoParams,   fRspAnalyze_CCID_HL78},

    /* MODEM SPECIFIC COMMANDS */
	{CMD_AT_KSRAT,       "+KSRAT",       HL78_DEFAULT_TIMEOUT,  fCmdBuild_NoParams,   fRspAnalyze_KSRAT_HL78},
	{CMD_AT_AND_K,       "&K",           HL78_SIMREADY_TIMEOUT,  fCmdBuild_AT_AND_K,  fRspAnalyze_None},
	{CMD_AT_KSELACQ,     "+KSELACQ",     HL78_DEFAULT_TIMEOUT,  fCmdBuild_NoParams,   fRspAnalyze_KSELACQ_HL78},
	{CMD_AT_KBNDCFG,     "+KBNDCFG",     HL78_DEFAULT_TIMEOUT,  fCmdBuild_NoParams,   fRspAnalyze_KBNDCFG_HL78},
    {CMD_AT_QPOWD,       "+QPOWD",       HL78_DEFAULT_TIMEOUT,  fCmdBuild_QPOWD_HL78, fRspAnalyze_None},
    {CMD_AT_QCFG,        "+QCFG",        HL78_DEFAULT_TIMEOUT,  fCmdBuild_QCFG_HL78,  fRspAnalyze_QCFG_HL78},
    {CMD_AT_QIND,        "+QIND",        HL78_DEFAULT_TIMEOUT,  fCmdBuild_NoParams,   fRspAnalyze_QIND_HL78},
    {CMD_AT_QICSGP,      "+QICSGP",      HL78_DEFAULT_TIMEOUT,  fCmdBuild_QICSGP_HL78, fRspAnalyze_None},
    {CMD_AT_QIURC,       "+QIURC",       HL78_DEFAULT_TIMEOUT,  fCmdBuild_NoParams, fRspAnalyze_QIURC_HL78},
    {CMD_AT_SOCKET_PROMPT, "> ",         HL78_SOCKET_PROMPT_TIMEOUT,  fCmdBuild_NoParams,   fRspAnalyze_None},
    {CMD_AT_SEND_OK,      "SEND OK",     HL78_DEFAULT_TIMEOUT,  fCmdBuild_NoParams,   fRspAnalyze_None},
    {CMD_AT_SEND_FAIL,    "SEND FAIL",   HL78_DEFAULT_TIMEOUT,  fCmdBuild_NoParams,   fRspAnalyze_None},
    {CMD_AT_QIDNSCFG,     "+QIDNSCFG",   HL78_DEFAULT_TIMEOUT,  fCmdBuild_QIDNSCFG_HL78, fRspAnalyze_None},
    {CMD_AT_QIDNSGIP,     "+QIDNSGIP",   HL78_QIDNSGIP_TIMEOUT, fCmdBuild_QIDNSGIP_HL78, fRspAnalyze_None},
    {CMD_AT_QICFG,       "+QICFG",       HL78_DEFAULT_TIMEOUT,  fCmdBuild_QICFG_HL78, fRspAnalyze_None},
    {CMD_AT_QINDCFG,     "+QINDCFG",     HL78_DEFAULT_TIMEOUT,  fCmdBuild_QINDCFG_HL78,  fRspAnalyze_QINDCFG_HL78},
    {CMD_AT_QINISTAT,    "+QINISTAT",    HL78_DEFAULT_TIMEOUT,  fCmdBuild_NoParams, fRspAnalyze_QINISTAT_HL78},
    {CMD_AT_KCELL,       "+KCELL",       HL78_DEFAULT_TIMEOUT,  fCmdBuild_KCELL_HL78, fRspAnalyze_KCELL_HL78},
    {CMD_AT_QUSIM,       "+QUSIM",       HL78_DEFAULT_TIMEOUT,  fCmdBuild_NoParams,   fRspAnalyze_None},
    {CMD_AT_CPSMS,       "+CPSMS",       HL78_CPSMS_TIMEOUT,    fCmdBuild_CPSMS,  fRspAnalyze_CPSMS},
    {CMD_AT_CEDRXS,      "+CEDRXS",      HL78_CEDRX_TIMEOUT,    fCmdBuild_CEDRXS, fRspAnalyze_CEDRXS},
    {CMD_AT_CEDRXP,      "+CEDRXP",      HL78_CEDRX_TIMEOUT,    fCmdBuild_NoParams,   fRspAnalyze_CEDRXP},
    {CMD_AT_CEDRXRDP,    "+CEDRXRDP",    HL78_CEDRX_TIMEOUT,    fCmdBuild_NoParams,   fRspAnalyze_CEDRXRDP},
    {CMD_AT_QNWINFO,     "+QNWINFO",     HL78_QNWINFO_TIMEOUT,  fCmdBuild_NoParams, fRspAnalyze_None},
    {CMD_AT_QENG,        "+QENG",        HL78_DEFAULT_TIMEOUT,  fCmdBuild_QENG_HL78, fRspAnalyze_None},
    {CMD_AT_QGMR,        "+CGMR",        HL78_DEFAULT_TIMEOUT,  fCmdBuild_NoParams, fRspAnalyze_CGMR_HL78},
    {CMD_AT_QPSMS,       "+QPSMS",       HL78_DEFAULT_TIMEOUT,  fCmdBuild_NoParams, fRspAnalyze_None},
    {CMD_AT_QPSMCFG,     "+QPSMCFG",     HL78_DEFAULT_TIMEOUT,  fCmdBuild_NoParams, fRspAnalyze_None},
    {CMD_AT_QPSMEXTCFG,  "+QPSMEXTCFG",  HL78_DEFAULT_TIMEOUT,  fCmdBuild_QPSMEXTCFG, fRspAnalyze_None},
    {CMD_AT_QURCCFG,     "+QURCCFG",     HL78_DEFAULT_TIMEOUT,  fCmdBuild_QURCCFG_HL78, fRspAnalyze_None},

    /* MODEM SPECIFIC COMMANDS USED FOR SOCKET MODE */
    {CMD_AT_QIACT,       "+QIACT",       HL78_QIACT_TIMEOUT,    fCmdBuild_QIACT_HL78,   fRspAnalyze_QIACT_HL78},
    {CMD_AT_QIOPEN,      "+QIOPEN",      HL78_QIOPEN_TIMEOUT,   fCmdBuild_QIOPEN_HL78,  fRspAnalyze_QIOPEN_HL78},
    {CMD_AT_QICLOSE,     "+QICLOSE",     HL78_QICLOSE_TIMEOUT,  fCmdBuild_QICLOSE_HL78, fRspAnalyze_None},
    {CMD_AT_QISEND,      "+QISEND",      HL78_DEFAULT_TIMEOUT,  fCmdBuild_QISEND_HL78,  fRspAnalyze_None},
    {CMD_AT_QISEND_WRITE_DATA,  "",      HL78_DEFAULT_TIMEOUT,  fCmdBuild_QISEND_WRITE_DATA_HL78, fRspAnalyze_None},
    {CMD_AT_QIRD,        "+QIRD",        HL78_DEFAULT_TIMEOUT,  fCmdBuild_QIRD_HL78,    fRspAnalyze_QIRD_HL78},
    {CMD_AT_QISTATE,     "+QISTATE",     HL78_DEFAULT_TIMEOUT,  fCmdBuild_QISTATE_HL78, fRspAnalyze_QISTATE_HL78},
    {CMD_AT_QPING,        "+QPING",      HL78_QPING_TIMEOUT,    fCmdBuild_QPING_HL78,   fRspAnalyze_QPING_HL78},

    /* MODEM SPECIFIC EVENTS */
    {CMD_AT_WAIT_EVENT,     "",          HL78_DEFAULT_TIMEOUT,        fCmdBuild_NoParams,   fRspAnalyze_None},
    {CMD_AT_BOOT_EVENT,     "",          HL78_RDY_TIMEOUT,            fCmdBuild_NoParams,   fRspAnalyze_None},
    {CMD_AT_RDY_EVENT,      "RDY",       HL78_RDY_TIMEOUT,            fCmdBuild_NoParams,   fRspAnalyze_None},
    {CMD_AT_APP_RDY_EVENT,  "APP RDY",   HL78_APP_RDY_TIMEOUT,        fCmdBuild_NoParams,   fRspAnalyze_None},
    {CMD_AT_POWERED_DOWN_EVENT,     "POWERED DOWN", HL78_RDY_TIMEOUT, fCmdBuild_NoParams,   fRspAnalyze_None},
    {CMD_AT_PSM_POWER_DOWN_EVENT, "PSM POWER DOWN", HL78_RDY_TIMEOUT, fCmdBuild_NoParams,   fRspAnalyze_None},
    {CMD_AT_QPSMTIMER_EVENT, "+QPSMTIMER", HL78_RDY_TIMEOUT, fCmdBuild_NoParams,   fRspAnalyze_QPSMTIMER_HL78},
  };
#define SIZE_ATCMD_HL78_LUT ((uint16_t) (sizeof (ATCMD_HL78_LUT) / sizeof (atcustom_LUT_t)))

  /* common init */
  HL78_modem_init(&HL78_ctxt);

  /* ###########################  START CUSTOMIZATION PART  ########################### */
  HL78_ctxt.modem_LUT_size = SIZE_ATCMD_HL78_LUT;
  HL78_ctxt.p_modem_LUT = (const atcustom_LUT_t *)ATCMD_HL78_LUT;

  /* override default termination string for AT command: <CR> */
  (void) sprintf((CRC_CHAR_t *)p_atp_ctxt->endstr, "\r");

  /* ###########################  END CUSTOMIZATION PART  ########################### */
}

uint8_t ATCustom_HL78_checkEndOfMsgCallback(uint8_t rxChar)
{
  uint8_t last_char = 0U;

  /* static variables */
  static const uint8_t QIRD_string[] = "+QIRD";
  static uint8_t QIRD_Counter = 0U;

  /*---------------------------------------------------------------------------------------*/
  if (HL78_ctxt.state_SyntaxAutomaton == WAITING_FOR_INIT_CR)
  {
    /* waiting for first valid <CR>, char received before are considered as trash */
    if ((AT_CHAR_t)('\r') == rxChar)
    {
      /* current     : xxxxx<CR>
      *  command format : <CR><LF>xxxxxxxx<CR><LF>
      *  waiting for : <LF>
      */
      HL78_ctxt.state_SyntaxAutomaton = WAITING_FOR_LF;
    }
  }
  /*---------------------------------------------------------------------------------------*/
  else if (HL78_ctxt.state_SyntaxAutomaton == WAITING_FOR_CR)
  {
    if ((AT_CHAR_t)('\r') == rxChar)
    {
      /* current     : xxxxx<CR>
      *  command format : <CR><LF>xxxxxxxx<CR><LF>
      *  waiting for : <LF>
      */
      HL78_ctxt.state_SyntaxAutomaton = WAITING_FOR_LF;
    }
  }
  /*---------------------------------------------------------------------------------------*/
  else if (HL78_ctxt.state_SyntaxAutomaton == WAITING_FOR_LF)
  {
    /* waiting for <LF> */
    if ((AT_CHAR_t)('\n') == rxChar)
    {
      /*  current        : <CR><LF>
      *   command format : <CR><LF>xxxxxxxx<CR><LF>
      *   waiting for    : x or <CR>
      */
      HL78_ctxt.state_SyntaxAutomaton = WAITING_FOR_FIRST_CHAR;
      last_char = 1U;
      QIRD_Counter = 0U;
    }
  }
  /*---------------------------------------------------------------------------------------*/
  else if (HL78_ctxt.state_SyntaxAutomaton == WAITING_FOR_FIRST_CHAR)
  {
    if (HL78_ctxt.socket_ctxt.socket_RxData_state == SocketRxDataState_waiting_header)
    {
      /* Socket Data RX - waiting for Header: we are waiting for +QIRD
      *
      * +QIRD: 522<CR><LF>HTTP/1.1 200 OK<CR><LF><CR><LF>Date: Wed, 21 Feb 2018 14:56:54 GMT<CR><LF><CR><LF>Serve...
      *    ^- waiting this string
      */
      if (rxChar == QIRD_string[QIRD_Counter])
      {
        QIRD_Counter++;
        if (QIRD_Counter == (uint8_t) strlen((const CRC_CHAR_t *)QIRD_string))
        {
          /* +QIRD detected, next step */
          socketHeaderRX_reset();
          HL78_ctxt.socket_ctxt.socket_RxData_state = SocketRxDataState_receiving_header;
        }
      }
    }

    /* NOTE:
    * if we are in socket_RxData_state = SocketRxDataState_waiting_header, we are waiting for +QIRD (test above)
    * but if we receive another message, we need to evacuate it without modifying socket_RxData_state
    * That's why we are nt using "else if" here, if <CR> if received before +QIND, it means that we have received
    * something else
    */
    if (HL78_ctxt.socket_ctxt.socket_RxData_state == SocketRxDataState_receiving_header)
    {
      /* Socket Data RX - Header received: we are waiting for second <CR>
      *
      * +QIRD: 522<CR><LF>HTTP/1.1 200 OK<CR><LF><CR><LF>Date: Wed, 21 Feb 2018 14:56:54 GMT<CR><LF><CR><LF>Serve...
      *         ^- retrieving this size
      *             ^- waiting this <CR>
      */
      if ((AT_CHAR_t)('\r') == rxChar)
      {
        /* second <CR> detected, we have received data header
        *  now waiting for <LF>, then start to receive socket data
        *  Verify that size received in header is the expected one
        */
        uint16_t size_from_header = SocketHeaderRX_getSize();
        if (HL78_ctxt.socket_ctxt.socket_rx_expected_buf_size != size_from_header)
        {
          /* update buffer size received - should not happen */
          HL78_ctxt.socket_ctxt.socket_rx_expected_buf_size = size_from_header;
        }
        HL78_ctxt.state_SyntaxAutomaton = WAITING_FOR_LF;
        HL78_ctxt.socket_ctxt.socket_RxData_state = SocketRxDataState_receiving_data;
      }
      else if ((rxChar >= (AT_CHAR_t)('0')) && (rxChar <= (AT_CHAR_t)('9')))
      {
        /* receiving size of data in header */
        SocketHeaderRX_addChar((CRC_CHAR_t *)&rxChar);
      }
      else if (rxChar == (AT_CHAR_t)(','))
      {
        /* receiving data field separator in header: +QIRD: 4,"10.7.76.34",7678
        *  data size field has been received, now ignore all chars until <CR><LF>
        *  additional fields (remote IP address and port) will be analyzed later
        */
        SocketHeaderDataRx_Cpt_Complete = 1U;
      }
      else {/* nothing to do */ }
    }
    else if (HL78_ctxt.socket_ctxt.socket_RxData_state == SocketRxDataState_receiving_data)
    {
      /* receiving socket data: do not analyze char, just count expected size
      *
      * +QIRD: 522<CR><LF>HTTP/1.1 200 OK<CR><LF><CR><LF>Date: Wed, 21 Feb 2018 14:56:54 GMT<CR><LF><CR><LF>Serve...
      *.                  ^- start to read data: count char
      */
      HL78_ctxt.socket_ctxt.socket_rx_count_bytes_received++;
      HL78_ctxt.state_SyntaxAutomaton = WAITING_FOR_SOCKET_DATA;
      /* check if full buffer has been received */
      if (HL78_ctxt.socket_ctxt.socket_rx_count_bytes_received == HL78_ctxt.socket_ctxt.socket_rx_expected_buf_size)
      {
        HL78_ctxt.socket_ctxt.socket_RxData_state = SocketRxDataState_data_received;
        HL78_ctxt.state_SyntaxAutomaton = WAITING_FOR_CR;
      }
    }
    /* waiting for <CR> or x */
    else if ((AT_CHAR_t)('\r') == rxChar)
    {
      /*   current        : <CR>
      *   command format : <CR><LF>xxxxxxxx<CR><LF>
      *   waiting for    : <LF>
      */
      HL78_ctxt.state_SyntaxAutomaton = WAITING_FOR_LF;
    }
    else {/* nothing to do */ }
  }
  /*---------------------------------------------------------------------------------------*/
  else if (HL78_ctxt.state_SyntaxAutomaton == WAITING_FOR_SOCKET_DATA)
  {
    HL78_ctxt.socket_ctxt.socket_rx_count_bytes_received++;
    /* check if full buffer has been received */
    if (HL78_ctxt.socket_ctxt.socket_rx_count_bytes_received == HL78_ctxt.socket_ctxt.socket_rx_expected_buf_size)
    {
      HL78_ctxt.socket_ctxt.socket_RxData_state = SocketRxDataState_data_received;
      HL78_ctxt.state_SyntaxAutomaton = WAITING_FOR_CR;
    }
  }
  /*---------------------------------------------------------------------------------------*/
  else
  {
    /* should not happen */
  }

  /* ###########################  START CUSTOMIZATION PART  ######################### */
  /* if modem does not use standard syntax or has some specificities, replace previous
  *  function by a custom function
  */
  if (last_char == 0U)
  {
    /* HL78 special cases
    *
    *  SOCKET MODE: when sending DATA using AT+QISEND, we are waiting for socket prompt "<CR><LF>> "
    *               before to send DATA. Then we should receive "OK<CR><LF>".
    */

    if (HL78_ctxt.socket_ctxt.socket_send_state != SocketSendState_No_Activity)
    {
      switch (HL78_ctxt.socket_ctxt.socket_send_state)
      {
        case SocketSendState_WaitingPrompt1st_greaterthan:
        {
          /* detecting socket prompt first char: "greater than" */
          if ((AT_CHAR_t)('>') == rxChar)
          {
            HL78_ctxt.socket_ctxt.socket_send_state = SocketSendState_WaitingPrompt2nd_space;
          }
          break;
        }

        case SocketSendState_WaitingPrompt2nd_space:
        {
          /* detecting socket prompt second char: "space" */
          if ((AT_CHAR_t)(' ') == rxChar)
          {
            HL78_ctxt.socket_ctxt.socket_send_state = SocketSendState_Prompt_Received;
            last_char = 1U;
          }
          else
          {
            /* if char iommediatly after "greater than" is not a "space", reinit state */
            HL78_ctxt.socket_ctxt.socket_send_state = SocketSendState_WaitingPrompt1st_greaterthan;
          }
          break;
        }

        default:
          break;
      }
    }
  }

  /* ###########################  END CUSTOMIZATION PART  ########################### */

  return (last_char);
}

at_status_t ATCustom_HL78_getCmd(at_context_t *p_at_ctxt, uint32_t *p_ATcmdTimeout)
{
  /* static variables */
  /* memorize number of SIM status query retries (static) */

  /* local variables */
  at_status_t retval = ATSTATUS_OK;
  atparser_context_t *p_atp_ctxt = &(p_at_ctxt->parser);
  at_msg_t curSID = p_atp_ctxt->current_SID;

  PRINT_API("enter ATCustom_HL78_getCmd() for SID %d", curSID)

  /* retrieve parameters from SID command (will update SID_ctxt) */
  if (atcm_retrieve_SID_parameters(&HL78_ctxt, p_atp_ctxt) != ATSTATUS_OK)
  {
    retval = ATSTATUS_ERROR;
    goto exit_ATCustom_HL78_getCmd;
  }

  /* new command: reset command context */
  atcm_reset_CMD_context(&HL78_ctxt.CMD_ctxt);

  /* For each SID, the sequence of AT commands to send id defined (it can be dynamic)
  * Determine and prepare the next command to send for this SID
  */

#if (ENABLE_HL78_LOW_POWER_MODE == 1U)
  /* HL78 specific
   * HL78 modem enters in PSM autonomously and suspends its uart.
   * If the HOST has not required LowPower state, AT-Custom will wake-up the modem.
   * If an HOST request is received durint this wake-up time, suspend the request until we
   * receive the "APP READY" event from the modem.
  */
  if (p_atp_ctxt->step == 0U)
  {
    if ((is_modem_in_PSM_state() == true) && (is_waiting_modem_wakeup() == true))
    {
      PRINT_INFO("WARNING !!! modem is in PSM state and can not answer")

      /* waiting for RDY indication from modem */
      atcm_program_WAIT_EVENT(p_atp_ctxt, HL78_APP_RDY_TIMEOUT, INTERMEDIATE_CMD);
    }
    else
    {
      /* check if modem is resuming from PSM. In this case, disable echo */
      if (HL78_shared.modem_resume_from_PSM)
      {
        /* disable echo */
        HL78_ctxt.CMD_ctxt.command_echo = AT_FALSE;
        atcm_program_AT_CMD(&HL78_ctxt, p_atp_ctxt, ATTYPE_EXECUTION_CMD, (CMD_ID_t) CMD_ATE, INTERMEDIATE_CMD);
        HL78_shared.modem_resume_from_PSM = false;
      }
      else
      {
        /* nothing to do, skip the command */
        atcm_program_SKIP_CMD(p_atp_ctxt);
      }
    }

    goto proceed_ATCustom_HL78_buildCmd;
  }
#endif /* ENABLE_HL78_LOW_POWER_MODE == 1U */

  /* ###########################  START CUSTOMIZATION PART  ########################### */
  if (curSID == (at_msg_t) SID_CS_CHECK_CNX)
  {
    if CHECK_STEP((0U))
    {
      atcm_program_AT_CMD(&HL78_ctxt, p_atp_ctxt, ATTYPE_EXECUTION_CMD, (CMD_ID_t) CMD_AT, FINAL_CMD);
    }
    else
    {
      /* error, invalid step */
      retval = ATSTATUS_ERROR;
    }
  }
  else if (curSID == (at_msg_t) SID_CS_MODEM_CONFIG)
  {
    if CHECK_STEP((0U))
    {
      atcm_program_AT_CMD(&HL78_ctxt, p_atp_ctxt, ATTYPE_EXECUTION_CMD, (CMD_ID_t) CMD_AT, FINAL_CMD);
    }
    else
    {
      /* error, invalid step */
      retval = ATSTATUS_ERROR;
    }
  }
  else if ((curSID == (at_msg_t) SID_CS_POWER_ON) ||
           (curSID == (at_msg_t) SID_CS_RESET))
  {
    uint8_t common_start_sequence_step = HL78_MODEM_SYNCHRO_AT_MAX_RETRIES + 1U;
    /* POWER_ON and RESET are almost the same, specific differences are managed case by case */
    /* for reset, only HW reset is supported */
    if ((curSID == (at_msg_t) SID_CS_RESET) &&
        (HL78_ctxt.SID_ctxt.reset_type != CS_RESET_HW))
    {
      PRINT_ERR("Reset type (%d) not supported", HL78_ctxt.SID_ctxt.reset_type)
      retval = ATSTATUS_ERROR;
    }
    else
    {
      /****************************************************************************
        * POWER_ON and RESET first steps
        * try to establish the communiction with the modem by sending "AT" commands
        ****************************************************************************/
      if CHECK_STEP((0U))
      {
        /* reset modem specific variables */
        reset_variables_HL78();

        /* reinit modem at ready status */
        HL78_ctxt.persist.modem_at_ready = AT_FALSE;

        /* in case of RESET, reset all the contexts to start from a fresh state */
        if (curSID == (at_msg_t) SID_CS_RESET)
        {
          HL78_modem_reset(&HL78_ctxt);
        }

        /* force requested flow control */
        atcm_program_AT_CMD(&HL78_ctxt, p_atp_ctxt, ATTYPE_EXECUTION_CMD, (CMD_ID_t) CMD_AT_AND_K, INTERMEDIATE_CMD);
        /******Ashu Modification******/
        //TODO: As HL78 octave does not support IFC
        //atcm_program_SKIP_CMD(p_atp_ctxt);
      }
      else if (CHECK_STEP_BETWEEN((1U), (HL78_MODEM_SYNCHRO_AT_MAX_RETRIES - 1U)))
      {
        /* start a loop to wait for modem : send AT commands */
        if (HL78_ctxt.persist.modem_at_ready == AT_FALSE)
        {
          /* use optional as we are not sure to receive a response from the modem: this allows to avoid to return
            * an error to upper layer
            */
          PRINT_DBG("test connection [try number %d] ", p_atp_ctxt->step)
          atcm_program_AT_CMD_ANSWER_OPTIONAL(&HL78_ctxt, p_atp_ctxt,
                                              ATTYPE_EXECUTION_CMD, (CMD_ID_t) CMD_AT, INTERMEDIATE_CMD);
        }
        else
        {
          /* modem has answered to the command AT: it is ready */
          PRINT_INFO("modem synchro established, proceed to normal power sequence")

          /* go to next step: jump to POWER ON sequence step */
          p_atp_ctxt->step = common_start_sequence_step;
          atcm_program_SKIP_CMD(p_atp_ctxt);
        }
      }
      else if CHECK_STEP((HL78_MODEM_SYNCHRO_AT_MAX_RETRIES))
      {
        /* if we fall here and the modem is not ready, we have a communication problem */
        if (HL78_ctxt.persist.modem_at_ready == AT_FALSE)
        {
          /* error, impossible to synchronize with modem */
          PRINT_ERR("Impossible to sync with modem")
          retval = ATSTATUS_ERROR;
        }
        else
        {
          /* continue the boot sequence */
          atcm_program_SKIP_CMD(p_atp_ctxt);
        }
      }
      /********************************************************************
        * common power ON/RESET sequence starts here
        * when communication with modem has been successfully established
        ********************************************************************/
      else if CHECK_STEP((common_start_sequence_step))
      {
        /* force flow control */
        atcm_program_AT_CMD(&HL78_ctxt, p_atp_ctxt, ATTYPE_EXECUTION_CMD, (CMD_ID_t) CMD_AT_AND_K, INTERMEDIATE_CMD);
    	/******Ashu Modification******/
    	//TODO: As HL78 octave does not support IFC
    	atcm_program_SKIP_CMD(p_atp_ctxt);
      }
      else if CHECK_STEP((common_start_sequence_step + 1U))
      {
        /* disable echo */
        HL78_ctxt.CMD_ctxt.command_echo = AT_FALSE;
        atcm_program_AT_CMD(&HL78_ctxt, p_atp_ctxt, ATTYPE_EXECUTION_CMD, (CMD_ID_t) CMD_ATE, INTERMEDIATE_CMD);
      }
      else if CHECK_STEP((common_start_sequence_step + 2U))
      {
        /* request detailed error report */
        atcm_program_AT_CMD(&HL78_ctxt, p_atp_ctxt, ATTYPE_WRITE_CMD, (CMD_ID_t) CMD_AT_CMEE, INTERMEDIATE_CMD);
      }
      else if CHECK_STEP((common_start_sequence_step + 3U))
      {
        /* enable full response format */
        HL78_ctxt.CMD_ctxt.dce_full_resp_format = AT_TRUE;
        atcm_program_AT_CMD(&HL78_ctxt, p_atp_ctxt, ATTYPE_EXECUTION_CMD, (CMD_ID_t) CMD_ATV, INTERMEDIATE_CMD);
      }
      else if CHECK_STEP((common_start_sequence_step + 4U))
      {
        /* deactivate DTR */
        atcm_program_AT_CMD(&HL78_ctxt, p_atp_ctxt, ATTYPE_EXECUTION_CMD, (CMD_ID_t) CMD_AT_AND_D, INTERMEDIATE_CMD);
      }
      else if CHECK_STEP((common_start_sequence_step + 5U))
      {
        /* Read FW revision */
        atcm_program_AT_CMD(&HL78_ctxt, p_atp_ctxt, ATTYPE_EXECUTION_CMD, (CMD_ID_t) CMD_AT_CGMR, INTERMEDIATE_CMD);
      }
      else if CHECK_STEP((common_start_sequence_step + 6U))
      {
        /* power on with AT+CFUN=0 */
        HL78_ctxt.CMD_ctxt.cfun_value = 0U;
        atcm_program_AT_CMD(&HL78_ctxt, p_atp_ctxt, ATTYPE_WRITE_CMD, (CMD_ID_t) CMD_AT_CFUN, INTERMEDIATE_CMD);
      }
      else if CHECK_STEP((common_start_sequence_step + 7U))
      {
        /* force to disable PSM in case modem was switched off with PSM enabled */
        HL78_ctxt.SID_ctxt.set_power_config.psm_present = CELLULAR_TRUE;
        HL78_ctxt.SID_ctxt.set_power_config.psm_mode = PSM_MODE_DISABLE;
        /******Ashu Modification******/
        //Changed to make CPSMS as final command
        atcm_program_AT_CMD(&HL78_ctxt, p_atp_ctxt, ATTYPE_WRITE_CMD, (CMD_ID_t) CMD_AT_CPSMS, INTERMEDIATE_CMD);
      }
      /* ----- start specific power ON sequence here ----
        * HL78_AT_Commands_Manual_V2.0
        */
      /******Ashu Modification******/
      //TODO To implement some HL78 and HL78 specific AT commands like KBNDCFG KSRAT KSELACQ for HL78 hence commenting the below
      /* Check bands parameters */
      else if CHECK_STEP((common_start_sequence_step + 8U))
      {
        atcm_program_AT_CMD(&HL78_ctxt, p_atp_ctxt, ATTYPE_READ_CMD, (CMD_ID_t) CMD_AT_KSRAT, INTERMEDIATE_CMD);
      }
      else if CHECK_STEP((common_start_sequence_step + 9U))
      {
        atcm_program_AT_CMD(&HL78_ctxt, p_atp_ctxt, ATTYPE_READ_CMD, (CMD_ID_t) CMD_AT_KSELACQ, INTERMEDIATE_CMD);
      }
      else if CHECK_STEP((common_start_sequence_step + 10U))
      {
        atcm_program_AT_CMD(&HL78_ctxt, p_atp_ctxt, ATTYPE_READ_CMD, (CMD_ID_t) CMD_AT_KBNDCFG, FINAL_CMD);
      }
      /*else if CHECK_STEP((common_start_sequence_step + 11U))
      {
        HL78_shared.QCFG_command_write = AT_FALSE;
        HL78_shared.QCFG_command_param = QCFG_nwscanmode;
        atcm_program_AT_CMD(&HL78_ctxt, p_atp_ctxt, ATTYPE_WRITE_CMD, (CMD_ID_t) CMD_AT_QCFG, FINAL_CMD);
      }*/
      else if CHECK_STEP_EXCEEDS((common_start_sequence_step + 12U))
      {
        /* error, invalid step */
        retval = ATSTATUS_ERROR;
      }
      else
      {
        /* ignore */
      }
    }
  }
  else if (curSID == (at_msg_t) SID_CS_POWER_OFF)
  {
    if CHECK_STEP((0U))
    {
      /* since Sierra Wireless HL78 Hardware Design V1.4
        * Power Off sequence is done using GPIO
        * The write command CMD_AT_QPOWD is no more used.
        */
      atcm_program_NO_MORE_CMD(p_atp_ctxt);
    }
    else
    {
      /* error, invalid step */
      retval = ATSTATUS_ERROR;
    }
  }
  else if (curSID == (at_msg_t) SID_CS_INIT_MODEM)
  {
    if CHECK_STEP((0U))
    {
      /* CFUN parameters here are coming from user settings in CS_init_modem() */
      atcm_program_AT_CMD(&HL78_ctxt, p_atp_ctxt, ATTYPE_WRITE_CMD, (CMD_ID_t) CMD_AT_CFUN, INTERMEDIATE_CMD);
      HL78_shared.HL78_sim_status_retries = 0U;
      HL78_shared.QINISTAT_error = AT_FALSE;
    }
    else if CHECK_STEP((1U))
    {
      if (HL78_ctxt.SID_ctxt.modem_init.init == CS_CMI_MINI)
      {
        /* minimum functionality selected, no SIM access => leave now */
        atcm_program_NO_MORE_CMD(p_atp_ctxt);
      }
      else
      {
        atcm_program_AT_CMD(&HL78_ctxt, p_atp_ctxt, ATTYPE_EXECUTION_CMD, (CMD_ID_t) CMD_AT_CCID, INTERMEDIATE_CMD);
      }

      /* reset SIM ready status, we will check it later */
      HL78_ctxt.persist.modem_sim_ready  = AT_FALSE;
    }
    else if CHECK_STEP((2U))
    {
      if (HL78_shared.HL78_sim_status_retries > HL78_MAX_SIM_STATUS_RETRIES)
      {
        if (HL78_shared.QINISTAT_error == AT_FALSE)
        {
          /* error, max sim status retries reached */
          atcm_program_NO_MORE_CMD(p_atp_ctxt);
          retval = ATSTATUS_ERROR;
        }
        else
        {
          /* special case: AT+QINISTAT reported an error because this cmd is not
            * supported by the modem FW version. Wait for maximum retries then
            * continue sequance
            */
          PRINT_INFO("warning, modem FW version certainly too old")
          HL78_ctxt.persist.modem_sim_ready  = AT_TRUE; /* assume that SIM is ready now */
          atcm_program_AT_CMD(&HL78_ctxt, p_atp_ctxt, ATTYPE_EXECUTION_CMD, (CMD_ID_t) CMD_AT_CGMR, INTERMEDIATE_CMD);
        }
      }
      else
      {
        /* if CPIN indicates that SIM is not ready, may be it's because there is a PIN code
          *  => check PIN code status regularly
          */
        if ((HL78_shared.HL78_sim_status_retries != 0U) &&
            ((HL78_shared.HL78_sim_status_retries % 3U) == 0U))
        {
          /* check PIN code status */
          atcm_program_AT_CMD(&HL78_ctxt, p_atp_ctxt, ATTYPE_READ_CMD, (CMD_ID_t) CMD_AT_CPIN, INTERMEDIATE_CMD);
        }
        else
        {
          /* request SIM status - Ashu Commented out below original implementation */
          /*atcm_program_AT_CMD(&HL78_ctxt, p_atp_ctxt,
                              ATTYPE_EXECUTION_CMD, (CMD_ID_t) CMD_AT_CPIN, INTERMEDIATE_CMD);*/

          /******Ashu Modification******/
          /* request SIM status */
                    atcm_program_AT_CMD(&HL78_ctxt, p_atp_ctxt,
                    		ATTYPE_READ_CMD, (CMD_ID_t) CMD_AT_CPIN, INTERMEDIATE_CMD);
         /******Ashu Modification******/
        }
      }
    }
    else if CHECK_STEP((3U))
    {
      if (HL78_ctxt.persist.modem_sim_ready == AT_FALSE)
      {
        if (HL78_ctxt.persist.sim_state == CS_SIMSTATE_SIM_PIN_REQUIRED)
        {
          /* SIM is ready but need PIN code, go to next step */
          atcm_program_SKIP_CMD(p_atp_ctxt);
          PRINT_INFO("SIM is ready, unlock sequence")
        }
        else
        {
          /* SIM not ready yet, wait before retry */
          atcm_program_TEMPO(p_atp_ctxt, HL78_SIMREADY_TIMEOUT, INTERMEDIATE_CMD);
          /* go back to previous step */
          p_atp_ctxt->step = p_atp_ctxt->step - 2U;
          PRINT_INFO("SIM not ready yet")
          HL78_shared.HL78_sim_status_retries++;
        }
      }
      else
      {
        /* SIM is ready, go to next step*/
        atcm_program_SKIP_CMD(p_atp_ctxt);
        PRINT_INFO("SIM is ready, unlock sequence")
      }
    }
    else if CHECK_STEP((4U))
    {
      /* reset HL78_sim_status_retries */
      HL78_shared.HL78_sim_status_retries = 0U;
      /* check is CPIN is requested */
      atcm_program_AT_CMD(&HL78_ctxt, p_atp_ctxt, ATTYPE_READ_CMD, (CMD_ID_t) CMD_AT_CPIN, INTERMEDIATE_CMD);
    }
    else if CHECK_STEP((5U))
    {
      if (HL78_ctxt.persist.sim_pin_code_ready == AT_FALSE)
      {
        if (strlen((const CRC_CHAR_t *)&HL78_ctxt.SID_ctxt.modem_init.pincode.pincode) != 0U)
        {
          /* send PIN value */
          PRINT_INFO("CPIN required, we send user value to modem")
          atcm_program_AT_CMD(&HL78_ctxt, p_atp_ctxt, ATTYPE_WRITE_CMD, (CMD_ID_t) CMD_AT_CPIN, INTERMEDIATE_CMD);
        }
        else
        {
          /* no PIN provided by user */
          PRINT_INFO("CPIN required but not provided by user")
          retval = ATSTATUS_ERROR;
        }
      }
      else
      {
        PRINT_INFO("CPIN not required")
        /* no PIN required, skip cmd and go to next step */
        atcm_program_SKIP_CMD(p_atp_ctxt);
      }
    }
    else if CHECK_STEP((6U))
    {
      /* check PDP context parameters */
      atcm_program_AT_CMD(&HL78_ctxt, p_atp_ctxt, ATTYPE_READ_CMD, (CMD_ID_t) CMD_AT_CGDCONT, FINAL_CMD);
    }
    else
    {
      /* error, invalid step */
      retval = ATSTATUS_ERROR;
    }
  }
  else if (curSID == (at_msg_t) SID_CS_GET_DEVICE_INFO)
  {
    if CHECK_STEP((0U))
    {
      switch (HL78_ctxt.SID_ctxt.device_info->field_requested)
      {
        case CS_DIF_MANUF_NAME_PRESENT:
          atcm_program_AT_CMD(&HL78_ctxt, p_atp_ctxt, ATTYPE_EXECUTION_CMD, (CMD_ID_t) CMD_AT_CGMI, FINAL_CMD);
          break;

        case CS_DIF_MODEL_PRESENT:
          atcm_program_AT_CMD(&HL78_ctxt, p_atp_ctxt, ATTYPE_EXECUTION_CMD, (CMD_ID_t) CMD_AT_CGMM, FINAL_CMD);
          break;

        case CS_DIF_REV_PRESENT:
          atcm_program_AT_CMD(&HL78_ctxt, p_atp_ctxt, ATTYPE_EXECUTION_CMD, (CMD_ID_t) CMD_AT_CGMR, FINAL_CMD);
          break;

        case CS_DIF_SN_PRESENT:
          HL78_ctxt.CMD_ctxt.cgsn_write_cmd_param = CGSN_SN;
          atcm_program_AT_CMD(&HL78_ctxt, p_atp_ctxt, ATTYPE_EXECUTION_CMD, (CMD_ID_t) CMD_AT_CGSN, FINAL_CMD);
          break;

        case CS_DIF_IMEI_PRESENT:
          atcm_program_AT_CMD(&HL78_ctxt, p_atp_ctxt, ATTYPE_EXECUTION_CMD, (CMD_ID_t) CMD_AT_GSN, FINAL_CMD);
          break;

        case CS_DIF_IMSI_PRESENT:
          atcm_program_AT_CMD(&HL78_ctxt, p_atp_ctxt, ATTYPE_EXECUTION_CMD, (CMD_ID_t) CMD_AT_CIMI, FINAL_CMD);
          break;

        case CS_DIF_PHONE_NBR_PRESENT:
          /* not AT+CNUM not supported by HL78 */
          retval = ATSTATUS_ERROR;
          break;

        case CS_DIF_ICCID_PRESENT:
          atcm_program_AT_CMD(&HL78_ctxt, p_atp_ctxt, ATTYPE_EXECUTION_CMD, (CMD_ID_t) CMD_AT_CCID, FINAL_CMD);
          break;

        default:
          /* error, invalid step */
          retval = ATSTATUS_ERROR;
          break;
      }
    }
    else
    {
      /* error, invalid step */
      retval = ATSTATUS_ERROR;
    }
  }
  else if (curSID == (at_msg_t) SID_CS_GET_SIGNAL_QUALITY)
  {

    if CHECK_STEP((0U))
    {
      /* get signal strength */
      /******Ashu Modification******/
      //Made Final command until implemented
      atcm_program_AT_CMD(&HL78_ctxt, p_atp_ctxt, ATTYPE_EXECUTION_CMD, (CMD_ID_t) CMD_AT_CSQ,INTERMEDIATE_CMD);
    }
    else if CHECK_STEP((1U))
    {
      /* get signal strength */
      /******Ashu Modification******/
      atcm_program_AT_CMD(&HL78_ctxt, p_atp_ctxt, ATTYPE_WRITE_CMD, (CMD_ID_t) CMD_AT_KCELL, FINAL_CMD);
    }
    else if CHECK_STEP((2U))
    {
#if (HL78_OPTION_NETWORK_INFO == 1)
      /* NB: cmd answer is optional ie no error will be raised if no answer received from modem
        *  indeed, if requested here, it's just a bonus and should not generate an error
        */
       /******Ashu Modification******/
       // TODO To implement SELRAT or KSRAT here plus Band
      /*atcm_program_AT_CMD_ANSWER_OPTIONAL(&HL78_ctxt, p_atp_ctxt, ATTYPE_EXECUTION_CMD, (CMD_ID_t) CMD_AT_QNWINFO,
                                          INTERMEDIATE_CMD);*/
    	atcm_program_SKIP_CMD(p_atp_ctxt);
#else
      atcm_program_SKIP_CMD(p_atp_ctxt);
#endif /* HL78_OPTION_NETWORK_INFO */
    }
    else if CHECK_STEP((3U))
    {
#if (HL78_OPTION_ENGINEERING_MODE == 1)
      atcm_program_AT_CMD(&HL78_ctxt, p_atp_ctxt, ATTYPE_WRITE_CMD, (CMD_ID_t) CMD_AT_QENG, FINAL_CMD);
#else
      atcm_program_NO_MORE_CMD(p_atp_ctxt);
#endif /* HL78_OPTION_ENGINEERING_MODE */
    }
    else
    {
      /* error, invalid step */
      retval = ATSTATUS_ERROR;
    }
  }
  else if (curSID == (at_msg_t) SID_CS_GET_ATTACHSTATUS)
  {
    if CHECK_STEP((0U))
    {
      atcm_program_AT_CMD(&HL78_ctxt, p_atp_ctxt, ATTYPE_READ_CMD, (CMD_ID_t) CMD_AT_CGATT, FINAL_CMD);
    }
    else
    {
      /* error, invalid step */
      retval = ATSTATUS_ERROR;
    }
  }
  else if (curSID == (at_msg_t) SID_CS_REGISTER_NET)
  {
    if CHECK_STEP((0U))
    {
      /* read registration status */
      atcm_program_AT_CMD(&HL78_ctxt, p_atp_ctxt, ATTYPE_READ_CMD, (CMD_ID_t) CMD_AT_COPS, INTERMEDIATE_CMD);
    }
    else if CHECK_STEP((1U))
    {
#if 0 /* check actual registration status */
      /* check if actual registration status is the expected one */
      CS_OperatorSelector_t *operatorSelect = &(HL78_ctxt.SID_ctxt.write_operator_infos);
      if (HL78_ctxt.SID_ctxt.read_operator_infos.mode != operatorSelect->mode)
      {
        /* write registration status */
        atcm_program_AT_CMD(&HL78_ctxt, p_atp_ctxt, ATTYPE_WRITE_CMD, (CMD_ID_t) CMD_AT_COPS, INTERMEDIATE_CMD);
      }
      else
      {
        /* skip this step */
        atcm_program_SKIP_CMD(p_atp_ctxt);
      }
#else
      /* due to problem observed on simu: does not register after reboot */
      atcm_program_AT_CMD(&HL78_ctxt, p_atp_ctxt, ATTYPE_WRITE_CMD, (CMD_ID_t) CMD_AT_COPS, INTERMEDIATE_CMD);
#endif /* 0 */
    }
    else if CHECK_STEP((2U))
    {
      /* read registration status */
      atcm_program_AT_CMD(&HL78_ctxt, p_atp_ctxt, ATTYPE_READ_CMD, (CMD_ID_t) CMD_AT_CEREG, INTERMEDIATE_CMD);
    }
    else if CHECK_STEP((3U))
    {
      /* read registration status */
      atcm_program_AT_CMD(&HL78_ctxt, p_atp_ctxt, ATTYPE_READ_CMD, (CMD_ID_t) CMD_AT_CREG, INTERMEDIATE_CMD);
    }
    else if CHECK_STEP((4U))
    {
      /* read registration status */
      atcm_program_AT_CMD(&HL78_ctxt, p_atp_ctxt, ATTYPE_READ_CMD, (CMD_ID_t) CMD_AT_CGREG, FINAL_CMD);
    }
    else
    {
      /* error, invalid step */
      retval = ATSTATUS_ERROR;
    }
  }
  else if (curSID == (at_msg_t) SID_CS_GET_NETSTATUS)
  {
    if CHECK_STEP((0U))
    {
      /* read registration status */
      atcm_program_AT_CMD(&HL78_ctxt, p_atp_ctxt, ATTYPE_READ_CMD, (CMD_ID_t) CMD_AT_CEREG, INTERMEDIATE_CMD);
    }
    else if CHECK_STEP((1U))
    {
      /* read registration status */
      atcm_program_AT_CMD(&HL78_ctxt, p_atp_ctxt, ATTYPE_READ_CMD, (CMD_ID_t) CMD_AT_CREG, INTERMEDIATE_CMD);
    }
    else if CHECK_STEP((2U))
    {
      /* read registration status */
      atcm_program_AT_CMD(&HL78_ctxt, p_atp_ctxt, ATTYPE_READ_CMD, (CMD_ID_t) CMD_AT_CGREG, INTERMEDIATE_CMD);
    }
    else if CHECK_STEP((3U))
    {
      /* read registration status */
      atcm_program_AT_CMD(&HL78_ctxt, p_atp_ctxt, ATTYPE_READ_CMD, (CMD_ID_t) CMD_AT_COPS, FINAL_CMD);
    }
    else
    {
      /* error, invalid step */
      retval = ATSTATUS_ERROR;
    }
  }
  else if (curSID == (at_msg_t) SID_CS_SUSBCRIBE_NET_EVENT)
  {
    if CHECK_STEP((0U))
    {
      CS_UrcEvent_t urcEvent = HL78_ctxt.SID_ctxt.urcEvent;

      /* is an event linked to CREG, CGREG or CEREG ? */
      if ((urcEvent == CS_URCEVENT_EPS_NETWORK_REG_STAT) || (urcEvent == CS_URCEVENT_EPS_LOCATION_INFO) ||
          (urcEvent == CS_URCEVENT_GPRS_NETWORK_REG_STAT) || (urcEvent == CS_URCEVENT_GPRS_LOCATION_INFO) ||
          (urcEvent == CS_URCEVENT_CS_NETWORK_REG_STAT) || (urcEvent == CS_URCEVENT_CS_LOCATION_INFO))
      {
        (void) atcm_subscribe_net_event(&HL78_ctxt, p_atp_ctxt);
      }
      else if (urcEvent == CS_URCEVENT_SIGNAL_QUALITY)
      {
        /* if signal quality URC not yet suscbribe */
        if (HL78_ctxt.persist.urc_subscript_signalQuality == CELLULAR_FALSE)
        {
          /* set event as subscribed */
          HL78_ctxt.persist.urc_subscript_signalQuality = CELLULAR_TRUE;

          /* request the URC we want */
          HL78_shared.QINDCFG_command_param = QINDCFG_csq;
          atcm_program_AT_CMD(&HL78_ctxt, p_atp_ctxt, ATTYPE_WRITE_CMD, (CMD_ID_t) CMD_AT_QINDCFG, FINAL_CMD);
        }
        else
        {
          atcm_program_NO_MORE_CMD(p_atp_ctxt);
        }
      }
      else
      {
        atcm_program_NO_MORE_CMD(p_atp_ctxt);
      }

    }
    else
    {
      /* error, invalid step */
      retval = ATSTATUS_ERROR;
    }
  }
  else if (curSID == (at_msg_t) SID_CS_UNSUSBCRIBE_NET_EVENT)
  {
    if CHECK_STEP((0U))
    {
      CS_UrcEvent_t urcEvent = HL78_ctxt.SID_ctxt.urcEvent;

      /* is an event linked to CREG, CGREG or CEREG ? */
      if ((urcEvent == CS_URCEVENT_EPS_NETWORK_REG_STAT) || (urcEvent == CS_URCEVENT_EPS_LOCATION_INFO) ||
          (urcEvent == CS_URCEVENT_GPRS_NETWORK_REG_STAT) || (urcEvent == CS_URCEVENT_GPRS_LOCATION_INFO) ||
          (urcEvent == CS_URCEVENT_CS_NETWORK_REG_STAT) || (urcEvent == CS_URCEVENT_CS_LOCATION_INFO))
      {
        (void) atcm_unsubscribe_net_event(&HL78_ctxt, p_atp_ctxt);
      }
      else if (urcEvent == CS_URCEVENT_SIGNAL_QUALITY)
      {
        /* if signal quality URC suscbribed */
        if (HL78_ctxt.persist.urc_subscript_signalQuality == CELLULAR_TRUE)
        {
          /* set event as unsuscribed */
          HL78_ctxt.persist.urc_subscript_signalQuality = CELLULAR_FALSE;

          /* request the URC we don't want */
          HL78_shared.QINDCFG_command_param = QINDCFG_csq;
          atcm_program_AT_CMD(&HL78_ctxt, p_atp_ctxt, ATTYPE_WRITE_CMD, (CMD_ID_t) CMD_AT_QINDCFG, FINAL_CMD);
        }
        else
        {
          atcm_program_NO_MORE_CMD(p_atp_ctxt);
        }
      }
      else
      {
        atcm_program_NO_MORE_CMD(p_atp_ctxt);
      }
    }
    else
    {
      /* error, invalid step */
      retval = ATSTATUS_ERROR;
    }
  }
  else if (curSID == (at_msg_t) SID_CS_REGISTER_PDN_EVENT)
  {
    if CHECK_STEP((0U))
    {
      if (HL78_ctxt.persist.urc_subscript_pdn_event == CELLULAR_FALSE)
      {
        /* set event as subscribed */
        HL78_ctxt.persist.urc_subscript_pdn_event = CELLULAR_TRUE;

        /* request PDN events */
        atcm_program_AT_CMD(&HL78_ctxt, p_atp_ctxt, ATTYPE_WRITE_CMD, (CMD_ID_t) CMD_AT_CGEREP, FINAL_CMD);
      }
      else
      {
        atcm_program_NO_MORE_CMD(p_atp_ctxt);
      }
    }
    else
    {
      /* error, invalid step */
      retval = ATSTATUS_ERROR;
    }
  }
  else if (curSID == (at_msg_t) SID_CS_DEREGISTER_PDN_EVENT)
  {
    if CHECK_STEP((0U))
    {
      if (HL78_ctxt.persist.urc_subscript_pdn_event == CELLULAR_TRUE)
      {
        /* set event as unsuscribed */
        HL78_ctxt.persist.urc_subscript_pdn_event = CELLULAR_FALSE;

        /* request PDN events */
        atcm_program_AT_CMD(&HL78_ctxt, p_atp_ctxt, ATTYPE_WRITE_CMD, (CMD_ID_t) CMD_AT_CGEREP, FINAL_CMD);
      }
      else
      {
        atcm_program_NO_MORE_CMD(p_atp_ctxt);
      }
    }
    else
    {
      /* error, invalid step */
      retval = ATSTATUS_ERROR;
    }
  }
  else if (curSID == (at_msg_t) SID_ATTACH_PS_DOMAIN)
  {
    if CHECK_STEP((0U))
    {
      HL78_ctxt.CMD_ctxt.cgatt_write_cmd_param = CGATT_ATTACHED;
      atcm_program_AT_CMD(&HL78_ctxt, p_atp_ctxt, ATTYPE_WRITE_CMD, (CMD_ID_t) CMD_AT_CGATT, FINAL_CMD);
    }
    else
    {
      /* error, invalid step */
      retval = ATSTATUS_ERROR;
    }
  }
  else if (curSID == (at_msg_t) SID_DETACH_PS_DOMAIN)
  {
    if CHECK_STEP((0U))
    {
      HL78_ctxt.CMD_ctxt.cgatt_write_cmd_param = CGATT_DETACHED;
      atcm_program_AT_CMD(&HL78_ctxt, p_atp_ctxt, ATTYPE_WRITE_CMD, (CMD_ID_t) CMD_AT_CGATT, FINAL_CMD);
    }
    else
    {
      /* error, invalid step */
      retval = ATSTATUS_ERROR;
    }
  }
  else if (curSID == (at_msg_t) SID_CS_ACTIVATE_PDN)
  {
#if (USE_SOCKETS_TYPE == USE_SOCKETS_MODEM)
    /* SOCKET MODE */
    if CHECK_STEP((0U))
    {
      /* ckeck PDN state */
      HL78_shared.pdn_already_active = AT_FALSE;
      /******Ashu Modification******/
      //Use CGPADDR Instead
      //atcm_program_AT_CMD(&HL78_ctxt, p_atp_ctxt, ATTYPE_READ_CMD, (CMD_ID_t) CMD_AT_QIACT, INTERMEDIATE_CMD);
      atcm_program_AT_CMD(&HL78_ctxt, p_atp_ctxt, ATTYPE_WRITE_CMD, (CMD_ID_t) CMD_AT_CGPADDR, INTERMEDIATE_CMD);
    }
    else if CHECK_STEP((1U))
    {
      /* PDN activation */
    	/******Ashu Modification******/
      if(&HL78_ctxt.persist.modem_cid_table->ip_addr_infos != 0U)
      //if (HL78_shared.pdn_already_active == AT_TRUE)
      {
        /* PDN already active - exit */
        PRINT_INFO("Skip PDN activation (already active)")
        atcm_program_NO_MORE_CMD(p_atp_ctxt);
      }
      else
      {
        /* request PDN activation */
        //atcm_program_AT_CMD(&HL78_ctxt, p_atp_ctxt, ATTYPE_WRITE_CMD, (CMD_ID_t) CMD_AT_QIACT, INTERMEDIATE_CMD);
    	  atcm_program_AT_CMD(&HL78_ctxt, p_atp_ctxt, ATTYPE_WRITE_CMD, (CMD_ID_t) CMD_AT_CGACT, INTERMEDIATE_CMD);
      }
    }
    else if CHECK_STEP((2U))
    {
      /* ckeck PDN state */
      //atcm_program_AT_CMD(&HL78_ctxt, p_atp_ctxt, ATTYPE_READ_CMD, (CMD_ID_t) CMD_AT_QIACT, FINAL_CMD);
    	atcm_program_AT_CMD(&HL78_ctxt, p_atp_ctxt, ATTYPE_WRITE_CMD, (CMD_ID_t) CMD_AT_CGPADDR, FINAL_CMD);
    }
    else
    {
      /* error, invalid step */
      retval = ATSTATUS_ERROR;
    }
#else
    /* DATA MODE*/
    if CHECK_STEP((0U))
    {
      /* get IP address */
      atcm_program_AT_CMD(&HL78_ctxt, p_atp_ctxt, ATTYPE_WRITE_CMD, (CMD_ID_t) CMD_AT_CGPADDR, INTERMEDIATE_CMD);
    }
    else if CHECK_STEP((1U))
    {
      atcm_program_AT_CMD(&HL78_ctxt, p_atp_ctxt, ATTYPE_EXECUTION_CMD, (CMD_ID_t) CMD_ATX, INTERMEDIATE_CMD);
    }
    else if CHECK_STEP((2U))
    {
      atcm_program_AT_CMD(&HL78_ctxt, p_atp_ctxt, ATTYPE_EXECUTION_CMD, (CMD_ID_t) CMD_ATD, FINAL_CMD);
    }
    else
    {
      /* error, invalid step */
      retval = ATSTATUS_ERROR;
    }
#endif /* USE_SOCKETS_TYPE */
  }
  else if (curSID == (at_msg_t) SID_CS_DEACTIVATE_PDN)
  {
    /* not implemented yet */
    retval = ATSTATUS_ERROR;
  }
  else if (curSID == (at_msg_t) SID_CS_DEFINE_PDN)
  {
#if (USE_SOCKETS_TYPE == USE_SOCKETS_MODEM)
    /* SOCKET MODE */
    if CHECK_STEP((0U))
    {
      HL78_shared.QICGSP_config_command = AT_TRUE;
      atcm_program_AT_CMD(&HL78_ctxt, p_atp_ctxt, ATTYPE_WRITE_CMD, (CMD_ID_t) CMD_AT_CGDCONT, FINAL_CMD);
    }
    else
    {
      /* error, invalid step */
      retval = ATSTATUS_ERROR;
    }
#else
    /* DATA MODE*/
    if CHECK_STEP((0U))
    {
      atcm_program_AT_CMD(&HL78_ctxt, p_atp_ctxt, ATTYPE_WRITE_CMD, (CMD_ID_t) CMD_AT_CGDCONT, FINAL_CMD);
      /* add AT+CGAUTH for username and password if required */
      /* could also use AT+QICSGP */
    }
    else
    {
      /* error, invalid step */
      retval = ATSTATUS_ERROR;
    }
#endif /* USE_SOCKETS_TYPE */
  }
  else if (curSID == (at_msg_t) SID_CS_SET_DEFAULT_PDN)
  {
    /* nothing to do here
      * Indeed, default PDN has been saved automatically during analysis of SID command
      * cf function: atcm_retrieve_SID_parameters()
      */
    atcm_program_NO_MORE_CMD(p_atp_ctxt);
  }
  else if (curSID == (at_msg_t) SID_CS_GET_IP_ADDRESS)
  {
    /* get IP address */
#if (USE_SOCKETS_TYPE == USE_SOCKETS_MODEM)
    /* SOCKET MODE */
    //atcm_program_AT_CMD(&HL78_ctxt, p_atp_ctxt, ATTYPE_READ_CMD, (CMD_ID_t) CMD_AT_QIACT, FINAL_CMD);
	  atcm_program_AT_CMD(&HL78_ctxt, p_atp_ctxt, ATTYPE_WRITE_CMD, (CMD_ID_t) CMD_AT_CGPADDR, FINAL_CMD);
#else
    /* DATA MODE*/
    atcm_program_AT_CMD(&HL78_ctxt, p_atp_ctxt, ATTYPE_WRITE_CMD, (CMD_ID_t) CMD_AT_CGPADDR, FINAL_CMD);
#endif /* USE_SOCKETS_TYPE */
  }
  else if (curSID == (at_msg_t) SID_CS_DIAL_COMMAND)
  {
    /* SOCKET CONNECTION FOR COMMAND DATA MODE */
    if CHECK_STEP((0U))
    {
      /* reserve a modem CID for this socket_handle */
      HL78_shared.QIOPEN_current_socket_connected = 0U;
      socket_handle_t sockHandle = HL78_ctxt.socket_ctxt.socket_info->socket_handle;
      (void) atcm_socket_reserve_modem_cid(&HL78_ctxt, sockHandle);
      atcm_program_AT_CMD(&HL78_ctxt, p_atp_ctxt, ATTYPE_WRITE_CMD, (CMD_ID_t) CMD_AT_QIOPEN, INTERMEDIATE_CMD);
      PRINT_INFO("For Client Socket Handle=%ld : MODEM CID affected=%d",
                 sockHandle,
                 HL78_ctxt.persist.socket[sockHandle].socket_connId_value)

    }
    else if CHECK_STEP((1U))
    {
      if (HL78_shared.QIOPEN_current_socket_connected == 0U)
      {
        /* Waiting for +QIOPEN urc indicating that socket is open */
        atcm_program_TEMPO(p_atp_ctxt, HL78_QIOPEN_TIMEOUT, INTERMEDIATE_CMD);
      }
      else
      {
        /* socket opened */
        HL78_shared.QIOPEN_waiting = AT_FALSE;
        /* socket is connected */
        (void) atcm_socket_set_connected(&HL78_ctxt, HL78_ctxt.socket_ctxt.socket_info->socket_handle);
        atcm_program_NO_MORE_CMD(p_atp_ctxt);
      }
    }
    else  if CHECK_STEP((2U))
    {
      if (HL78_shared.QIOPEN_current_socket_connected == 0U)
      {
        /* QIOPEN NOT RECEIVED,
          *  cf HL78 TCP/IP AT Commands Manual V1.0, paragraph 2.1.4 - 3/
          *  "if the URC cannot be received within 150 seconds, AT+QICLOSE should be used to close
          *   the socket".
          *
          *  then we will have to return an error to cellular service !!! (see next step)
          */
        HL78_shared.QIOPEN_waiting = AT_FALSE;
        atcm_program_AT_CMD(&HL78_ctxt, p_atp_ctxt, ATTYPE_WRITE_CMD, (CMD_ID_t) CMD_AT_QICLOSE, INTERMEDIATE_CMD);
      }
      else
      {
        /* socket opened */
        HL78_shared.QIOPEN_waiting = AT_FALSE;
        /* socket is connected */
        (void) atcm_socket_set_connected(&HL78_ctxt, HL78_ctxt.socket_ctxt.socket_info->socket_handle);
        atcm_program_NO_MORE_CMD(p_atp_ctxt);
      }
    }
    else  if CHECK_STEP((3U))
    {
      /* if we fall here, it means we have send CMD_AT_QICLOSE on previous step
        *  now inform cellular service that opening has failed => return an error
        */
      /* release the modem CID for this socket_handle */
      (void) atcm_socket_release_modem_cid(&HL78_ctxt, HL78_ctxt.socket_ctxt.socket_info->socket_handle);
      atcm_program_NO_MORE_CMD(p_atp_ctxt);
      retval = ATSTATUS_ERROR;
    }
    else
    {
      /* error, invalid step */
      retval = ATSTATUS_ERROR;
    }
  }
  else if (curSID == (at_msg_t) SID_CS_SEND_DATA)
  {
    if CHECK_STEP((0U))
    {
      /* Check data size to send */
      if (HL78_ctxt.SID_ctxt.socketSendData_struct.buffer_size > MODEM_MAX_SOCKET_TX_DATA_SIZE)
      {
        PRINT_ERR("Data size to send %ld exceed maximum size %ld",
                  HL78_ctxt.SID_ctxt.socketSendData_struct.buffer_size,
                  MODEM_MAX_SOCKET_TX_DATA_SIZE)
        atcm_program_NO_MORE_CMD(p_atp_ctxt);
        retval = ATSTATUS_ERROR;
      }
      else
      {
        HL78_ctxt.socket_ctxt.socket_send_state = SocketSendState_WaitingPrompt1st_greaterthan;
        atcm_program_AT_CMD(&HL78_ctxt, p_atp_ctxt, ATTYPE_WRITE_CMD, (CMD_ID_t) CMD_AT_QISEND, INTERMEDIATE_CMD);
      }
    }
    else if CHECK_STEP((1U))
    {
      /* waiting for socket prompt: "<CR><LF>> " */
      if (HL78_ctxt.socket_ctxt.socket_send_state == SocketSendState_Prompt_Received)
      {
        PRINT_DBG("SOCKET PROMPT ALREADY RECEIVED")
        /* go to next step */
        atcm_program_SKIP_CMD(p_atp_ctxt);
      }
      else
      {
        PRINT_DBG("WAITING FOR SOCKET PROMPT")
        atcm_program_WAIT_EVENT(p_atp_ctxt, HL78_SOCKET_PROMPT_TIMEOUT, INTERMEDIATE_CMD);
      }
    }
    else if CHECK_STEP((2U))
    {
      /* socket prompt received, send DATA */
      atcm_program_AT_CMD(&HL78_ctxt, p_atp_ctxt, ATTYPE_RAW_CMD, (CMD_ID_t) CMD_AT_QISEND_WRITE_DATA, FINAL_CMD);

      /* reinit automaton to receive answer */
      reinitSyntaxAutomaton_HL78();
    }
    else
    {
      /* error, invalid step */
      retval = ATSTATUS_ERROR;
    }
  }
  else if ((curSID == (at_msg_t) SID_CS_RECEIVE_DATA) ||
           (curSID == (at_msg_t) SID_CS_RECEIVE_DATA_FROM))
  {
    if CHECK_STEP((0U))
    {
      HL78_ctxt.socket_ctxt.socket_receive_state = SocketRcvState_RequestSize;
      atcm_program_AT_CMD(&HL78_ctxt, p_atp_ctxt, ATTYPE_WRITE_CMD, (CMD_ID_t) CMD_AT_QIRD, INTERMEDIATE_CMD);
    }
    else if CHECK_STEP((1U))
    {
      /* check that data size to receive is not null */
      if (HL78_ctxt.socket_ctxt.socket_rx_expected_buf_size != 0U)
      {
        /* check that data size to receive does not exceed maximum size
          *  if it's the case, only request maximum size we can receive
          */
        if (HL78_ctxt.socket_ctxt.socket_rx_expected_buf_size >
            HL78_ctxt.socket_ctxt.socketReceivedata.max_buffer_size)
        {
          PRINT_INFO("Data size available (%ld) exceed buffer maximum size (%ld)",
                     HL78_ctxt.socket_ctxt.socket_rx_expected_buf_size,
                     HL78_ctxt.socket_ctxt.socketReceivedata.max_buffer_size)
          HL78_ctxt.socket_ctxt.socket_rx_expected_buf_size = HL78_ctxt.socket_ctxt.socketReceivedata.max_buffer_size;
        }

        /* receive data */
        HL78_ctxt.socket_ctxt.socket_receive_state = SocketRcvState_RequestData_Header;
        atcm_program_AT_CMD(&HL78_ctxt, p_atp_ctxt, ATTYPE_WRITE_CMD, (CMD_ID_t) CMD_AT_QIRD, FINAL_CMD);
      }
      else
      {
        /* no data to receive */
        atcm_program_NO_MORE_CMD(p_atp_ctxt);
      }
    }
    else
    {
      /* error, invalid step */
      retval = ATSTATUS_ERROR;
    }
  }
  else if (curSID == (at_msg_t) SID_CS_SOCKET_CLOSE)
  {
    if CHECK_STEP((0U))
    {
      /* is socket connected ?
        * due to HL78 socket connection mechanism (waiting URC QIOPEN), we can fall here but socket
        * has been already closed if error occurs during connection
        */
      if (atcm_socket_is_connected(&HL78_ctxt, HL78_ctxt.socket_ctxt.socket_info->socket_handle) == AT_TRUE)
      {
        atcm_program_AT_CMD(&HL78_ctxt, p_atp_ctxt, ATTYPE_WRITE_CMD, (CMD_ID_t) CMD_AT_QICLOSE, INTERMEDIATE_CMD);
      }
      else
      {
        /* release the modem CID for this socket_handle */
        (void) atcm_socket_release_modem_cid(&HL78_ctxt, HL78_ctxt.socket_ctxt.socket_info->socket_handle);
        atcm_program_NO_MORE_CMD(p_atp_ctxt);
      }
    }
    else if CHECK_STEP((1U))
    {
      /* release the modem CID for this socket_handle */
      (void) atcm_socket_release_modem_cid(&HL78_ctxt, HL78_ctxt.socket_ctxt.socket_info->socket_handle);
      atcm_program_NO_MORE_CMD(p_atp_ctxt);
    }
    else
    {
      /* error, invalid step */
      retval = ATSTATUS_ERROR;
    }
  }
  else if (curSID == (at_msg_t) SID_CS_DATA_SUSPEND)
  {
    if CHECK_STEP((0U))
    {
      /* wait for 1 second */
      atcm_program_TEMPO(p_atp_ctxt, 1000U, INTERMEDIATE_CMD);
    }
    else if CHECK_STEP((1U))
    {
      /* send escape sequence +++ (RAW command type)
        *  CONNECT expected before 1000 ms
        */
      atcm_program_AT_CMD(&HL78_ctxt, p_atp_ctxt, ATTYPE_RAW_CMD, (CMD_ID_t) CMD_AT_ESC_CMD, FINAL_CMD);
      reinitSyntaxAutomaton_HL78();
    }
    else
    {
      /* error, invalid step */
      retval = ATSTATUS_ERROR;
    }
  }
  else if (curSID == (at_msg_t) SID_CS_DATA_RESUME)
  {
    if CHECK_STEP((0U))
    {
      atcm_program_AT_CMD(&HL78_ctxt, p_atp_ctxt, ATTYPE_EXECUTION_CMD, (CMD_ID_t) CMD_ATX, INTERMEDIATE_CMD);
    }
    else if CHECK_STEP((1U))
    {
      atcm_program_AT_CMD(&HL78_ctxt, p_atp_ctxt, ATTYPE_EXECUTION_CMD, (CMD_ID_t) CMD_ATO, FINAL_CMD);
    }
    else
    {
      /* error, invalid step */
      retval = ATSTATUS_ERROR;
    }
  }
#if (ENABLE_HL78_LOW_POWER_MODE == 1U)
  else if (curSID == (at_msg_t) SID_CS_INIT_POWER_CONFIG)
  {
    if CHECK_STEP((0U))
    {
      /* Init parameters are available into SID_ctxt.init_power_config
        * SID_ctxt.init_power_config is used to build AT+CPSMS and AT+CEDRX commands
        * Build it from SID_ctxt.init_power_config and modem specificities
        */
      if (init_HL78_low_power(&HL78_ctxt) == AT_FALSE)
      {
        /* Low Power not enabled, stop here the SID */
        atcm_program_NO_MORE_CMD(p_atp_ctxt);
      }
      else
      {
        /* Low Power enabled, continue to next step
          * CEREG 4 not requested for HL78
          */
        atcm_program_SKIP_CMD(p_atp_ctxt);
      }
    }
    else if CHECK_STEP((1U))
    {
      /* read EDRX params */
      atcm_program_AT_CMD(&HL78_ctxt, p_atp_ctxt, ATTYPE_READ_CMD, (CMD_ID_t) CMD_AT_CEDRXS, INTERMEDIATE_CMD);
    }
    else if CHECK_STEP((2U))
    {
      /* read PSM params */
      atcm_program_AT_CMD(&HL78_ctxt, p_atp_ctxt, ATTYPE_READ_CMD, (CMD_ID_t) CMD_AT_CPSMS, INTERMEDIATE_CMD);
    }
    else if CHECK_STEP((3U))
    {
      /* set EDRX params (default) */
      atcm_program_AT_CMD(&HL78_ctxt, p_atp_ctxt, ATTYPE_WRITE_CMD, (CMD_ID_t) CMD_AT_CEDRXS, INTERMEDIATE_CMD);
    }
    else if CHECK_STEP((4U))
    {
      /* enable URC output */
      atcm_program_AT_CMD(&HL78_ctxt, p_atp_ctxt, ATTYPE_WRITE_CMD, (CMD_ID_t) CMD_AT_QURCCFG, INTERMEDIATE_CMD);
    }
    else if CHECK_STEP((5U))
    {
      /* request for +QPSMTIMER urc */
      HL78_shared.QCFG_command_write = AT_TRUE;
      HL78_shared.QCFG_command_param = QCFG_urc_psm;
      atcm_program_AT_CMD(&HL78_ctxt, p_atp_ctxt, ATTYPE_WRITE_CMD, (CMD_ID_t) CMD_AT_QCFG, INTERMEDIATE_CMD);
    }
    else if CHECK_STEP((6U))
    {
      /* set PSM params (default) */
      atcm_program_AT_CMD(&HL78_ctxt, p_atp_ctxt, ATTYPE_WRITE_CMD, (CMD_ID_t) CMD_AT_CPSMS, INTERMEDIATE_CMD);
    }
    else if CHECK_STEP((7U))
    {
      /* configure advanced PSM parameters */
      atcm_program_AT_CMD(&HL78_ctxt, p_atp_ctxt,
                          ATTYPE_READ_CMD, (CMD_ID_t) CMD_AT_QPSMEXTCFG, INTERMEDIATE_CMD);
    }
    else if CHECK_STEP((8U))
    {
      /* configure advanced PSM parameters */
      atcm_program_AT_CMD(&HL78_ctxt, p_atp_ctxt,
                          ATTYPE_WRITE_CMD, (CMD_ID_t) CMD_AT_QPSMEXTCFG, INTERMEDIATE_CMD);
    }
    else if CHECK_STEP((9U))
    {
      /* note: keep this as final command (previous command may be skipped if no valid PSM parameters) */
      atcm_program_NO_MORE_CMD(p_atp_ctxt);
    }
    else
    {
      /* error, invalid step */
      retval = ATSTATUS_ERROR;
    }
  }
  else if (curSID == (at_msg_t) SID_CS_SET_POWER_CONFIG)
  {
    if CHECK_STEP((0U))
    {
      (void) set_HL78_low_power(&HL78_ctxt);
      atcm_program_SKIP_CMD(p_atp_ctxt);
    }
    else if CHECK_STEP((1U))
    {
      /* set EDRX params (if available) */
      atcm_program_AT_CMD(&HL78_ctxt, p_atp_ctxt, ATTYPE_WRITE_CMD, (CMD_ID_t) CMD_AT_CEDRXS, INTERMEDIATE_CMD);
    }
    else if CHECK_STEP((2U))
    {
      /* set PSM params (if available) */
      atcm_program_AT_CMD(&HL78_ctxt, p_atp_ctxt, ATTYPE_WRITE_CMD, (CMD_ID_t) CMD_AT_CPSMS, INTERMEDIATE_CMD);
    }
    else if CHECK_STEP((3U))
    {
      /* eDRX Read Dynamix Parameters */
      atcm_program_AT_CMD(&HL78_ctxt, p_atp_ctxt,
                          ATTYPE_EXECUTION_CMD, (CMD_ID_t) CMD_AT_CEDRXRDP, INTERMEDIATE_CMD);
    }
    else if CHECK_STEP((4U))
    {
      atcm_program_AT_CMD(&HL78_ctxt, p_atp_ctxt,
                          ATTYPE_READ_CMD, (CMD_ID_t) CMD_AT_QPSMS, INTERMEDIATE_CMD);
    }
    else if CHECK_STEP((5U))
    {
      atcm_program_AT_CMD(&HL78_ctxt, p_atp_ctxt,
                          ATTYPE_READ_CMD, (CMD_ID_t) CMD_AT_QPSMCFG, INTERMEDIATE_CMD);
    }
    else if CHECK_STEP((6U))
    {
      atcm_program_AT_CMD(&HL78_ctxt, p_atp_ctxt,
                          ATTYPE_READ_CMD, (CMD_ID_t) CMD_AT_QPSMEXTCFG, INTERMEDIATE_CMD);
    }
    else if CHECK_STEP((7U))
    {
      /* note: keep this as final command (previous command may be skipped if no valid PSM parameters) */
      atcm_program_NO_MORE_CMD(p_atp_ctxt);
    }
    else
    {
      /* error, invalid step */
      retval = ATSTATUS_ERROR;
    }
  }
  else if (curSID == (at_msg_t) SID_CS_SLEEP_REQUEST)
  {
    if CHECK_STEP((0U))
    {
      /* update LP automaton states */
      low_power_event(EVENT_LP_HOST_SLEEP_REQ);

      /* Simulate modem ack to enter in low power state
        * (UART is not deactivated yet)
        * check if this event was susbcribed by upper layers
        */
      if (atcm_modem_event_received(&HL78_ctxt, CS_MDMEVENT_LP_ENTER) == AT_TRUE)
      {
        /* trigger an internal event to ATCORE (ie an event not received from IPC)
          * to allow to report an URC to upper layers (URC event = enter in LP)
          */
        AT_internalEvent(DEVTYPE_MODEM_CELLULAR);
      }
      atcm_program_NO_MORE_CMD(p_atp_ctxt);
    }
    else
    {
      /* error, invalid step */
      retval = ATSTATUS_ERROR;
    }
  }
  else if (curSID == (at_msg_t) SID_CS_SLEEP_COMPLETE)
  {
    if CHECK_STEP((0U))
    {
      low_power_event(EVENT_LP_HOST_SLEEP_COMPLETE);
      atcm_program_NO_MORE_CMD(p_atp_ctxt);
    }
    else
    {
      /* error, invalid step */
      retval = ATSTATUS_ERROR;
    }
  }
  else if (curSID == (at_msg_t) SID_CS_SLEEP_CANCEL)
  {
    if CHECK_STEP((0U))
    {
      low_power_event(EVENT_LP_HOST_SLEEP_CANCEL);
      atcm_program_NO_MORE_CMD(p_atp_ctxt);
    }
    else
    {
      /* error, invalid step */
      retval = ATSTATUS_ERROR;
    }
  }
  else if (curSID == (at_msg_t) SID_CS_WAKEUP)
  {
    if CHECK_STEP((0U))
    {
      if (HL78_ctxt.SID_ctxt.wakeup_origin == HOST_WAKEUP)
      {
        low_power_event(EVENT_LP_HOST_WAKEUP_REQ);

        if (is_modem_in_PSM_state() == true)
        {
          PRINT_INFO("this is a host wake up and modem is in PSM state")
          /* a non null delay is used in case wake-up is called just after PSM POWER DOWN indication,
            *  otherwise this wake up request is not taken into account by the modem
            */
          (void) SysCtrl_HL78_wakeup_from_PSM(1000U);
          /* No AT command to send, wait RDY from modem in next step */
          atcm_program_SKIP_CMD(p_atp_ctxt);
        }
        else
        {
          PRINT_INFO("this is a host wake up but modem is not in PSM state")
          /* modem is not in PSM, no need to wait for wake-up confirmation */
          atcm_program_NO_MORE_CMD(p_atp_ctxt);
        }
      }
    }
    else if CHECK_STEP((1U))
    {
      /* waiting for RDY indication from modem */
      atcm_program_WAIT_EVENT(p_atp_ctxt, HL78_APP_RDY_TIMEOUT, INTERMEDIATE_CMD);

    }
    else if CHECK_STEP((2U))
    {
      /* disable echo */
      HL78_ctxt.CMD_ctxt.command_echo = AT_FALSE;
      atcm_program_AT_CMD(&HL78_ctxt, p_atp_ctxt, ATTYPE_EXECUTION_CMD, (CMD_ID_t) CMD_ATE, INTERMEDIATE_CMD);
    }
    else if CHECK_STEP((3U))
    {
      atcm_program_NO_MORE_CMD(p_atp_ctxt);
    }
    else
    {
      /* error, invalid step */
      retval = ATSTATUS_ERROR;
    }
  }
#endif /* ENABLE_HL78_LOW_POWER_MODE == 1U */
  else if (curSID == (at_msg_t) SID_CS_SOCKET_CNX_STATUS)
  {
    if CHECK_STEP((0U))
    {
      atcm_program_AT_CMD(&HL78_ctxt, p_atp_ctxt, ATTYPE_WRITE_CMD, (CMD_ID_t) CMD_AT_QISTATE, FINAL_CMD);
    }
    else
    {
      /* error, invalid step */
      retval = ATSTATUS_ERROR;
    }
  }
  else if (curSID == (at_msg_t) SID_CS_DNS_REQ)
  {
    if CHECK_STEP((0U))
    {
      atcm_program_AT_CMD(&HL78_ctxt, p_atp_ctxt, ATTYPE_WRITE_CMD, (CMD_ID_t) CMD_AT_QIDNSCFG, INTERMEDIATE_CMD);
    }
    else if CHECK_STEP((1U))
    {
      /* initialize +QIURC "dnsgip" parameters */
      init_HL78_qiurc_dnsgip();
      atcm_program_AT_CMD(&HL78_ctxt, p_atp_ctxt, ATTYPE_WRITE_CMD, (CMD_ID_t) CMD_AT_QIDNSGIP, INTERMEDIATE_CMD);
    }
    else if CHECK_STEP((2U))
    {
      /* do we have received a valid DNS response ? */
      if ((HL78_shared.QIURC_dnsgip_param.finished == AT_TRUE) && (HL78_shared.QIURC_dnsgip_param.error == 0U))
      {
        /* yes */
        atcm_program_NO_MORE_CMD(p_atp_ctxt);
      }
      else
      {
        /* not yet, waiting for DNS information */
        atcm_program_TEMPO(p_atp_ctxt, HL78_QIDNSGIP_TIMEOUT, FINAL_CMD);
      }
    }
    else
    {
      /* error, invalid step */
      retval = ATSTATUS_ERROR;
    }
  }
  else if (curSID == (at_msg_t) SID_CS_SUSBCRIBE_MODEM_EVENT)
  {
    /* nothing to do here
      * Indeed, default modem events subscribed havebeen saved automatically during analysis of SID command
      * cf function: atcm_retrieve_SID_parameters()
      */
    atcm_program_NO_MORE_CMD(p_atp_ctxt);
  }
  else if (curSID == (at_msg_t) SID_CS_PING_IP_ADDRESS)
  {
    if CHECK_STEP((0U))
    {
      atcm_program_AT_CMD(&HL78_ctxt, p_atp_ctxt, ATTYPE_WRITE_CMD, (CMD_ID_t) CMD_AT_QPING, FINAL_CMD);
    }
    else
    {
      /* error, invalid step */
      retval = ATSTATUS_ERROR;
    }
  }
  else if (curSID == (at_msg_t) SID_CS_DIRECT_CMD)
  {
    if CHECK_STEP((0U))
    {
      atcm_program_AT_CMD(&HL78_ctxt, p_atp_ctxt, ATTYPE_RAW_CMD, (CMD_ID_t) CMD_AT_DIRECT_CMD, FINAL_CMD);
      atcm_program_CMD_TIMEOUT(&HL78_ctxt, p_atp_ctxt, HL78_ctxt.SID_ctxt.direct_cmd_tx->cmd_timeout);
    }
    else
    {
      /* error, invalid step */
      retval = ATSTATUS_ERROR;
    }
  }
  else if (curSID == (at_msg_t) SID_CS_SIM_SELECT)
  {
    if CHECK_STEP((0U))
    {
      /* select the SIM slot */
      if (atcm_select_hw_simslot(HL78_ctxt.persist.sim_selected) != ATSTATUS_OK)
      {
        retval = ATSTATUS_ERROR;
      }
      atcm_program_NO_MORE_CMD(p_atp_ctxt);
    }
    else
    {
      /* error, invalid step */
      retval = ATSTATUS_ERROR;
    }
  }
  else if (curSID == (at_msg_t) SID_CS_SIM_GENERIC_ACCESS)
  {
    if CHECK_STEP((0U))
    {
      atcm_program_AT_CMD(&HL78_ctxt, p_atp_ctxt, ATTYPE_WRITE_CMD, (CMD_ID_t) CMD_AT_CSIM, FINAL_CMD);
    }
    else
    {
      /* error, invalid step */
      retval = ATSTATUS_ERROR;
    }
  }

  /* ###########################  END CUSTOMIZATION PART  ########################### */
  else
  {
    PRINT_ERR("Error, invalid command ID %d", curSID)
    retval = ATSTATUS_ERROR;
  }

#if (ENABLE_HL78_LOW_POWER_MODE == 1U)
proceed_ATCustom_HL78_buildCmd:
#endif /* (ENABLE_HL78_LOW_POWER_MODE == 1U) */

  /* if no error, build the command to send */
  if (retval == ATSTATUS_OK)
  {
    retval = atcm_modem_build_cmd(&HL78_ctxt, p_atp_ctxt, p_ATcmdTimeout);
  }

exit_ATCustom_HL78_getCmd:
  return (retval);
}

at_endmsg_t ATCustom_HL78_extractElement(atparser_context_t *p_atp_ctxt,
                                         const IPC_RxMessage_t *p_msg_in,
                                         at_element_info_t *element_infos)
{
  at_endmsg_t retval_msg_end_detected = ATENDMSG_NO;
  bool exit_loop;
  uint16_t idx;
  uint16_t start_idx;
  uint16_t *p_parseIndex = &(element_infos->current_parse_idx);

  PRINT_API("enter ATCustom_HL78_extractElement()")
  PRINT_DBG("input message: size=%d ", p_msg_in->size)

  /* if this is beginning of message, check that message header is correct and jump over it */
  if (*p_parseIndex == 0U)
  {
    /* ###########################  START CUSTOMIZATION PART  ########################### */
    /* MODEM RESPONSE SYNTAX:
      * <CR><LF><response><CR><LF>
      *
      */
    start_idx = 0U;
    /* search initial <CR><LF> sequence (for robustness) */
    if ((p_msg_in->buffer[0] == (AT_CHAR_t)('\r')) && (p_msg_in->buffer[1] == (AT_CHAR_t)('\n')))
    {
      /* <CR><LF> sequence has been found, it is a command line */
      PRINT_DBG("cmd init sequence <CR><LF> found - break")
      *p_parseIndex = 2U;
      start_idx = 2U;
    }

    exit_loop = false;
    for (idx = start_idx; (idx < (p_msg_in->size - 1U)) && (exit_loop == false); idx++)
    {
      if ((p_atp_ctxt->current_atcmd.id == (CMD_ID_t) CMD_AT_QIRD) &&
          (HL78_ctxt.socket_ctxt.socket_receive_state == SocketRcvState_RequestData_Payload) &&
          (HL78_ctxt.socket_ctxt.socket_RxData_state != SocketRxDataState_finished))
      {
        PRINT_DBG("receiving socket data (real size=%d)", SocketHeaderRX_getSize())
        element_infos->str_start_idx = 0U;
        element_infos->str_end_idx = (uint16_t) HL78_ctxt.socket_ctxt.socket_rx_count_bytes_received;
        element_infos->str_size = (uint16_t) HL78_ctxt.socket_ctxt.socket_rx_count_bytes_received;
        HL78_ctxt.socket_ctxt.socket_RxData_state = SocketRxDataState_finished;
        retval_msg_end_detected = ATENDMSG_YES;
        exit_loop = true;
      }
    }

    /* check if end of message has been detected */
    if (retval_msg_end_detected == ATENDMSG_YES)
    {
      goto exit_ATCustom_HL78_extractElement;
    }
    /* ###########################  END CUSTOMIZATION PART  ########################### */
  }

  element_infos->str_start_idx = *p_parseIndex;
  element_infos->str_end_idx = *p_parseIndex;
  element_infos->str_size = 0U;

  /* reach limit of input buffer ? (empty message received) */
  if (*p_parseIndex >= p_msg_in->size)
  {
    retval_msg_end_detected = ATENDMSG_YES;
    goto exit_ATCustom_HL78_extractElement;
  }

  /* extract parameter from message */
  exit_loop = false;
  do
  {
    switch (p_msg_in->buffer[*p_parseIndex])
    {
      /* ###########################  START CUSTOMIZATION PART  ########################### */
      /* ----- test separators ----- */
      case ':':
      case ',':
        exit_loop = true;
        break;

      /* ----- test end of message ----- */
      case '\r':
        exit_loop = true;
        retval_msg_end_detected = ATENDMSG_YES;
        break;

      default:
        /* increment end position */
        element_infos->str_end_idx = *p_parseIndex;
        element_infos->str_size++;
        break;
        /* ###########################  END CUSTOMIZATION PART  ########################### */
    }

    /* increment index */
    (*p_parseIndex)++;

    /* reach limit of input buffer ? */
    if (*p_parseIndex >= p_msg_in->size)
    {
      exit_loop = true;
      retval_msg_end_detected = ATENDMSG_YES;
    }
  } while (exit_loop == false);

  /* increase parameter rank */
  element_infos->param_rank = (element_infos->param_rank + 1U);

exit_ATCustom_HL78_extractElement:
  return (retval_msg_end_detected);
}

at_action_rsp_t ATCustom_HL78_analyzeCmd(at_context_t *p_at_ctxt,
                                         const IPC_RxMessage_t *p_msg_in,
                                         at_element_info_t *element_infos)
{
  atparser_context_t *p_atp_ctxt = &(p_at_ctxt->parser);
  at_action_rsp_t retval;

  PRINT_API("enter ATCustom_HL78_analyzeCmd()")

  /* Analyze data received from the modem and
    * search in LUT the ID corresponding to command received
  */
  if (ATSTATUS_OK != atcm_searchCmdInLUT(&HL78_ctxt, p_atp_ctxt, p_msg_in, element_infos))
  {
    /* No command corresponding to a LUT entry has been found.
     * May be we received a text line without command prefix.
     *
     * This is the case for some commands which are using following syntax:
     *    AT+MYCOMMAND
     *    parameters
     *    OK
     *
     * ( usually, command are using the syntax:
     *    AT+MYCOMMAND
     *    +MYCOMMAND=parameters
     *    OK
     *  )
     */

    /* 1st STEP: search in common modems commands
     * (CGMI, CGMM, CGMR, CGSN, GSN, IPR, CIMI, CGPADDR, ...)
     */
    retval = atcm_check_text_line_cmd(&HL78_ctxt, p_at_ctxt, p_msg_in, element_infos);

    /* 2nd STEP: search in specific modems commands if not found at 1st STEP
     *
     * This is the case in socket mode when receiving data.
     * The command is decomposed in 2 lines:
     * The 1st part of the response is analyzed by atcm_searchCmdInLUT:
     *   +QIRD: 522<CR><LF>
     * The 2nd part of the response, corresponding to the data, falls here:
     *   HTTP/1.1 200 OK<CR><LF><CR><LF>Date: Wed, 21 Feb 2018 14:56:54 GMT<CR><LF><CR><LF>Serve...
     */
    if (retval == ATACTION_RSP_NO_ACTION)
    {
      switch (p_atp_ctxt->current_atcmd.id)
      {
        /* ###########################  START CUSTOMIZED PART  ########################### */
        case CMD_AT_QIRD:
          if (fRspAnalyze_QIRD_data_HL78(p_at_ctxt, &HL78_ctxt, p_msg_in, element_infos) != ATACTION_RSP_ERROR)
          {
            /* received a valid intermediate answer */
            retval = ATACTION_RSP_INTERMEDIATE;
          }
          break;

        case CMD_AT_QISTATE:
          (void) fRspAnalyze_QISTATE_HL78(p_at_ctxt, &HL78_ctxt, p_msg_in, element_infos);
          /* received a valid intermediate answer */
          retval = ATACTION_RSP_INTERMEDIATE;
          break;


        /* ###########################  END CUSTOMIZED PART  ########################### */
        default:
          /* this is not one of modem common command, need to check if this is an answer to a modem's specific cmd */
          PRINT_DBG("receive an un-expected line... is it a text line ?")
          retval = ATACTION_RSP_IGNORED;
          break;
      }
    }

    /* we fall here when cmd_id not found in the LUT
    * 2 cases are possible:
    *  - this is a valid line: ATACTION_RSP_INTERMEDIATE
    *  - this is an invalid line: ATACTION_RSP_ERROR
    */
  }
  else
  {
    /* cmd_id has been found in the LUT: determine next action */
    switch (element_infos->cmd_id_received)
    {
      case CMD_AT_OK:
      {
        if (p_atp_ctxt->current_SID == (at_msg_t) SID_CS_DATA_SUSPEND)
        {
          PRINT_INFO("MODEM SWITCHES TO COMMAND MODE")
        }
        if ((p_atp_ctxt->current_SID == (at_msg_t) SID_CS_POWER_ON) ||
            (p_atp_ctxt->current_SID == (at_msg_t) SID_CS_RESET))
        {
          if (p_atp_ctxt->current_atcmd.id == (CMD_ID_t) CMD_AT)
          {
            /* modem is synchronized */
            HL78_ctxt.persist.modem_at_ready = AT_TRUE;
          }
          if (p_atp_ctxt->current_atcmd.id == (CMD_ID_t) CMD_ATE)
          {
            PRINT_DBG("Echo successfully disabled")
          }
        }
        if (p_atp_ctxt->current_SID == (at_msg_t) SID_CS_PING_IP_ADDRESS)
        {
          /* PING requests for HL78 are asynchronous.
          * If the PING request is valid (no other ongoing ping request for example), the
          * modem will answer OK.
          * At this point, initialize the ping response structure.
          */
          PRINT_DBG("this is a valid PING request")
          atcm_validate_ping_request(&HL78_ctxt);
        }
        retval = ATACTION_RSP_FRC_END;
        break;
      }

      case CMD_AT_NO_CARRIER:
      case CMD_AT_NO_ANSWER:
      {
        retval = ATACTION_RSP_ERROR;
        break;
      }

      case CMD_AT_RING:
      case CMD_AT_NO_DIALTONE:
      case CMD_AT_BUSY:
      {
        /* VALUES NOT MANAGED IN CURRENT IMPLEMENTATION BECAUSE NOT EXPECTED */
        retval = ATACTION_RSP_ERROR;
        break;
      }

      case CMD_AT_CONNECT:
      {
        PRINT_INFO("MODEM SWITCHES TO DATA MODE")
        retval = (at_action_rsp_t)(ATACTION_RSP_FLAG_DATA_MODE | ATACTION_RSP_FRC_END);
        break;
      }

      /* ###########################  START CUSTOMIZATION PART  ########################### */
      case CMD_AT_CEREG:
      case CMD_AT_CREG:
      case CMD_AT_CGREG:
      {
        /* check if response received corresponds to the command we have send
        *  if not => this is an URC
        */
        if (element_infos->cmd_id_received == p_atp_ctxt->current_atcmd.id)
        {
          retval = ATACTION_RSP_INTERMEDIATE;
        }
        else
        {
          retval = ATACTION_RSP_URC_FORWARDED;
        }
        break;
      }

      case CMD_AT_RDY_EVENT:
      {
        /* We received RDY event from the modem.
        * If received during Power ON or RESET, it is indicating that the modem is ready.
        * If received in another state, we report to upper layer a modem reboot event.
        */
        HL78_ctxt.persist.modem_at_ready = AT_TRUE;
        if ((p_atp_ctxt->current_SID == (at_msg_t) SID_CS_POWER_ON) ||
            (p_atp_ctxt->current_SID == (at_msg_t) SID_CS_RESET))
        {
          /* ignore the RDY event during POWER ON or RESET */
          retval = ATACTION_RSP_URC_IGNORED;
        }
#if (ENABLE_HL78_LOW_POWER_MODE == 1U)
        else  if (is_modem_in_PSM_state() == true)
        {
          /* ignore this event when modem is in PSM, we will act on APP RDY instead
           * with APP RDY, all modem services should be ready
           */
          retval = ATACTION_RSP_URC_IGNORED;
        }
#endif /* ENABLE_HL78_LOW_POWER_MODE == 1U */
        else
        {
          /* if event is received in other states, it's an unexpected modem reboot */
          if (atcm_modem_event_received(&HL78_ctxt, CS_MDMEVENT_BOOT) == AT_TRUE)
          {
            retval = ATACTION_RSP_URC_FORWARDED;
          }
          else
          {
            retval = ATACTION_RSP_URC_IGNORED;
          }
        }
        break;
      }

      case CMD_AT_APP_RDY_EVENT:
      {
#if (ENABLE_HL78_LOW_POWER_MODE == 1U)
        if (is_waiting_modem_wakeup() == true)
        {
          PRINT_INFO("APP RDY unlock event")
          /* UNLOCK the WAIT EVENT */
          retval = ATACTION_RSP_FRC_END;
        }
        else
        {
          retval = ATACTION_RSP_URC_IGNORED;
        }
        /* update LP automatons */
        low_power_event(EVENT_LP_MDM_LEAVE_PSM);
#else
        retval = ATACTION_RSP_URC_IGNORED;
#endif /* ENABLE_HL78_LOW_POWER_MODE == 1U */
        break;
      }

      case CMD_AT_POWERED_DOWN_EVENT:
      {
        PRINT_DBG("MODEM POWERED DOWN EVENT DETECTED")
        if (atcm_modem_event_received(&HL78_ctxt, CS_MDMEVENT_POWER_DOWN) == AT_TRUE)
        {
          retval = ATACTION_RSP_URC_FORWARDED;
        }
        else
        {
          retval = ATACTION_RSP_URC_IGNORED;
        }
        break;
      }

      case CMD_AT_QPSMTIMER_EVENT:
      {
        PRINT_DBG("QPSMTIMER URC DETECTED")
        /* will forward negotiated PSM timers value if new values detected */
        retval = ATACTION_RSP_URC_FORWARDED;;
        break;
      }

      case CMD_AT_SOCKET_PROMPT:
      {
        PRINT_INFO(" SOCKET PROMPT RECEIVED")
        /* if we were waiting for this event, we can continue the sequence */
        if (p_atp_ctxt->current_SID == (at_msg_t) SID_CS_SEND_DATA)
        {
          /* UNLOCK the WAIT EVENT */
          retval = ATACTION_RSP_FRC_END;
        }
        else
        {
          retval = ATACTION_RSP_URC_IGNORED;
        }
        break;
      }

      case CMD_AT_SEND_OK:
      {
        if (p_atp_ctxt->current_SID == (at_msg_t) SID_CS_SEND_DATA)
        {
          retval = ATACTION_RSP_FRC_END;
        }
        else
        {
          retval = ATACTION_RSP_ERROR;
        }
        break;
      }

      case CMD_AT_SEND_FAIL:
      {
        retval = ATACTION_RSP_ERROR;
        break;
      }

      case CMD_AT_QIURC:
      {
        /* retval will be override in analyze of +QUIRC content
        *  indeed, QIURC can be considered as an URC or a normal msg (for DNS request)
        */
        retval = ATACTION_RSP_INTERMEDIATE;
        break;
      }

      case CMD_AT_QIOPEN:
        /* now waiting for an URC  */
        retval = ATACTION_RSP_INTERMEDIATE;
        break;

      case CMD_AT_QIND:
        retval = ATACTION_RSP_URC_FORWARDED;
        break;

      case CMD_AT_CFUN:
        retval = ATACTION_RSP_URC_IGNORED;
        break;

      case CMD_AT_CPIN:
        retval = ATACTION_RSP_URC_IGNORED;
        break;

      case CMD_AT_QCFG:
        retval = ATACTION_RSP_INTERMEDIATE;
        break;

      case CMD_AT_QUSIM:
        retval = ATACTION_RSP_URC_IGNORED;
        break;

      case CMD_AT_CPSMS:
        retval = ATACTION_RSP_INTERMEDIATE;
        break;

      case CMD_AT_CEDRXS:
        retval = ATACTION_RSP_INTERMEDIATE;
        break;

      case CMD_AT_CGEV:
        retval = ATACTION_RSP_URC_FORWARDED;
        break;

      case CMD_AT_QPING:
        retval = ATACTION_RSP_URC_FORWARDED;
        break;

      case CMD_AT_PSM_POWER_DOWN_EVENT:
#if (ENABLE_HL78_LOW_POWER_MODE == 1U)
        low_power_event(EVENT_LP_MDM_ENTER_PSM);
#endif /* ENABLE_HL78_LOW_POWER_MODE == 1U */
        retval =  ATACTION_RSP_IGNORED;
        break;

      /* ###########################  END CUSTOMIZATION PART  ########################### */

      case CMD_AT:
        retval = ATACTION_RSP_IGNORED;
        break;

      case CMD_AT_INVALID:
        retval = ATACTION_RSP_ERROR;
        break;

      case CMD_AT_ERROR:
        /* ERROR does not contains parameters, so call the analyze function explicitly
        *  otherwise it will not ne called
        */
        retval = fRspAnalyze_Error_HL78(p_at_ctxt, &HL78_ctxt, p_msg_in, element_infos);
        break;

      case CMD_AT_CME_ERROR:
      case CMD_AT_CMS_ERROR:
        /* do the analyze here because will not be called by parser if the command
        * has no parameters (+CME ERROR only without parameters)
        */
        retval = fRspAnalyze_Error_HL78(p_at_ctxt, &HL78_ctxt, p_msg_in, element_infos);
        break;
      case CMD_AT_KCELL:
      case CMD_AT_KSELACQ:
      case CMD_AT_KBNDCFG:
      case CMD_AT_KSRAT:
          retval = ATACTION_RSP_URC_IGNORED;
          break;
      default:
      {
        /* check if response received corresponds to the command we have send
        *  if not => this is an ERROR
        */
        if (element_infos->cmd_id_received == p_atp_ctxt->current_atcmd.id)
        {
          retval = ATACTION_RSP_INTERMEDIATE;
        }
        else if (p_atp_ctxt->current_atcmd.type == ATTYPE_RAW_CMD)
        {
          retval = ATACTION_RSP_IGNORED;
        }
        /******Ashu Modification******/
        else if(p_atp_ctxt->current_atcmd.id == (CMD_ID_t) CMD_AT_CSQ)
        {
          /* Ignore the CME ERROR that is received when SIM is booting up */
          retval = ATACTION_RSP_IGNORED;
        }
        /******Ashu Modification******/
        else
        {

          PRINT_INFO("UNEXPECTED MESSAGE RECEIVED")
          retval = ATACTION_RSP_IGNORED;
        }
        break;
      }
    }
  }
  return (retval);
}

at_action_rsp_t ATCustom_HL78_analyzeParam(at_context_t *p_at_ctxt,
                                           const IPC_RxMessage_t *p_msg_in,
                                           at_element_info_t *element_infos)
{
  at_action_rsp_t retval;
  PRINT_API("enter ATCustom_HL78_analyzeParam()")

  /* analyse parameters of the command we received:
  * call the corresponding function from the LUT
  */
  retval = (atcm_get_CmdAnalyzeFunc(&HL78_ctxt, element_infos->cmd_id_received))(p_at_ctxt,
                                                                                 &HL78_ctxt,
                                                                                 p_msg_in,
                                                                                 element_infos);

  return (retval);
}

/* function called to finalize an AT command */
at_action_rsp_t ATCustom_HL78_terminateCmd(atparser_context_t *p_atp_ctxt, at_element_info_t *element_infos)
{
  at_action_rsp_t retval = ATACTION_RSP_IGNORED;
  PRINT_API("enter ATCustom_HL78_terminateCmd()")

  /* ###########################  START CUSTOMIZATION PART  ########################### */
  /* additional tests */
  if (HL78_ctxt.socket_ctxt.socket_send_state != SocketSendState_No_Activity)
  {
    /* special case for SID_CS_SEND_DATA
    * indeed, this function is called when an AT cmd is finished
    * but for AT+QISEND, it is called a 1st time when prompt is received
    * and a second time when data have been sent.
    */
    if (p_atp_ctxt->current_SID != (at_msg_t) SID_CS_SEND_DATA)
    {
      /* reset socket_send_state */
      HL78_ctxt.socket_ctxt.socket_send_state = SocketSendState_No_Activity;
    }
  }

  if ((p_atp_ctxt->current_atcmd.id == (at_msg_t) CMD_ATD) ||
      (p_atp_ctxt->current_atcmd.id == (at_msg_t) CMD_ATO) ||
      (p_atp_ctxt->current_atcmd.id == (at_msg_t) CMD_AT_CGDATA))
  {
    if (element_infos->cmd_id_received == (at_msg_t) CMD_AT_CONNECT)
    {
      /* force last command (no command can be sent in DATA mode) */
      p_atp_ctxt->is_final_cmd = 1U;
      PRINT_DBG("CONNECT received")
    }
    else
    {
      PRINT_ERR("expected CONNECT not received !!!")
      retval = ATACTION_RSP_ERROR;
    }
  }

  /* ###########################  END CUSTOMIZATION PART  ########################### */
  return (retval);
}

/* function called to finalize a SID */
at_status_t ATCustom_HL78_get_rsp(atparser_context_t *p_atp_ctxt, at_buf_t *p_rsp_buf)
{
  at_status_t retval;
  PRINT_API("enter ATCustom_HL78_get_rsp()")

  /* prepare response for a SID - common part */
  retval = atcm_modem_get_rsp(&HL78_ctxt, p_atp_ctxt, p_rsp_buf);

  /* ###########################  START CUSTOMIZATION PART  ########################### */
  /* prepare response for a SID
  *  all specific behaviors for SID which are returning data in rsp_buf have to be implemented here
  */
  switch (p_atp_ctxt->current_SID)
  {
    case SID_CS_DNS_REQ:
      /* PACK data to response buffer */
      if (DATAPACK_writeStruct(p_rsp_buf,
                               (uint16_t) CSMT_DNS_REQ,
                               (uint16_t) sizeof(HL78_shared.QIURC_dnsgip_param.hostIPaddr),
                               (void *)HL78_shared.QIURC_dnsgip_param.hostIPaddr) != DATAPACK_OK)
      {
        retval = ATSTATUS_OK;
      }
      break;

    case SID_CS_POWER_ON:
    case SID_CS_RESET:
      display_user_friendly_mode_and_bands_config();
      break;

    case SID_CS_POWER_OFF:
      /* reinit context for power off case */
      HL78_modem_reset(&HL78_ctxt);
      break;

#if (HL78_ACTIVATE_PING_REPORT == 1)
    case SID_CS_PING_IP_ADDRESS:
    {
      /* SID_CS_PING_IP_ADDRESS is waiting for a ping structure
       * For this modem, PING will be received later (as URC).
       * Just indicate that no ping report is available for now.
       */
      PRINT_DBG("Ping no report available yet - use PING_INVALID_INDEX")
      (void) memset((void *)&HL78_ctxt.persist.ping_resp_urc, 0, sizeof(CS_Ping_response_t));
      HL78_ctxt.persist.ping_resp_urc.index = PING_INVALID_INDEX;
      if (DATAPACK_writeStruct(p_rsp_buf,
                               (uint16_t) CSMT_URC_PING_RSP,
                               (uint16_t) sizeof(CS_Ping_response_t),
                               (void *)&HL78_ctxt.persist.ping_resp_urc) != DATAPACK_OK)
      {
        retval = ATSTATUS_OK;
      }
      break;
    }
#endif /* (HL78_ACTIVATE_PING_REPORT == 1) */

    default:
      break;
  }
  /* ###########################  END CUSTOMIZATION PART  ########################### */

  /* reset SID context */
  atcm_reset_SID_context(&HL78_ctxt.SID_ctxt);

  /* reset socket context */
  atcm_reset_SOCKET_context(&HL78_ctxt);

  return (retval);
}

at_status_t ATCustom_HL78_get_urc(atparser_context_t *p_atp_ctxt, at_buf_t *p_rsp_buf)
{
  at_status_t retval;
  PRINT_API("enter ATCustom_HL78_get_urc()")

  /* prepare response for an URC - common part */
  retval = atcm_modem_get_urc(&HL78_ctxt, p_atp_ctxt, p_rsp_buf);

  /* ###########################  START CUSTOMIZATION PART  ########################### */
  /* prepare response for an URC
  *  all specific behaviors for an URC have to be implemented here
  */

  /* ###########################  END CUSTOMIZATION PART  ########################### */

  return (retval);
}

at_status_t ATCustom_HL78_get_error(atparser_context_t *p_atp_ctxt, at_buf_t *p_rsp_buf)
{
  at_status_t retval;
  PRINT_API("enter ATCustom_HL78_get_error()")

  /* prepare response when an error occurred - common part */
  retval = atcm_modem_get_error(&HL78_ctxt, p_atp_ctxt, p_rsp_buf);

  /* ###########################  START CUSTOMIZATION PART  ########################### */
  /*  prepare response when an error occurred
  *  all specific behaviors for an error have to be implemented here
  */

  /* ###########################  END CUSTOMIZATION PART  ########################### */

  return (retval);
}

at_status_t ATCustom_HL78_hw_event(sysctrl_device_type_t deviceType, at_hw_event_t hwEvent, GPIO_PinState gstate)
{
  UNUSED(deviceType);
  UNUSED(hwEvent);
  UNUSED(gstate);

  at_status_t retval = ATSTATUS_OK;
  /* IMPORTANT: Do not add traces int this function of in functions called
   * (this function called under interrupt if GPIO event)
   */

  /* ###########################  START CUSTOMIZATION PART  ########################### */
  /* NO GPIO EVENT FOR THIS MODEM IN PSM */
  /* ###########################  END CUSTOMIZATION PART  ########################### */

  return (retval);
}

/* Private function Definition -----------------------------------------------*/

/* HL78 modem init function
*  call common init function and then do actions specific to this modem
*/
static void HL78_modem_init(atcustom_modem_context_t *p_modem_ctxt)
{
  PRINT_API("enter HL78_modem_init")

  /* common init function (reset all contexts) */
  atcm_modem_init(p_modem_ctxt);

  /* modem specific actions if any */
}

/* HL78 modem reset function
*  call common reset function and then do actions specific to this modem
*/
static void HL78_modem_reset(atcustom_modem_context_t *p_modem_ctxt)
{
  PRINT_API("enter HL78_modem_reset")

  /* common reset function (reset all contexts except SID) */
  atcm_modem_reset(p_modem_ctxt);

  /* modem specific actions if any */
}

static void reinitSyntaxAutomaton_HL78(void)
{
  HL78_ctxt.state_SyntaxAutomaton = WAITING_FOR_INIT_CR;
}

static void reset_variables_HL78(void)
{
  /* Set default values of HL78 specific variables after SWITCH ON or RESET */
  HL78_shared.mode_and_bands_config.nw_scanseq = 0xFFFFFFFFU;
  HL78_shared.mode_and_bands_config.nw_scanmode = 0xFFFFFFFFU;
  HL78_shared.mode_and_bands_config.iot_op_mode = 0xFFFFFFFFU;
  HL78_shared.mode_and_bands_config.gsm_bands = 0xFFFFFFFFU;
  HL78_shared.mode_and_bands_config.CatM1_bands_MsbPart = 0xFFFFFFFFU;
  HL78_shared.mode_and_bands_config.CatM1_bands_LsbPart = 0xFFFFFFFFU;
  HL78_shared.mode_and_bands_config.CatNB1_bands_MsbPart = 0xFFFFFFFFU;
  HL78_shared.mode_and_bands_config.CatNB1_bands_LsbPart = 0xFFFFFFFFU;

  /* other values */
  HL78_shared.host_lp_state = HOST_LP_STATE_IDLE;
  HL78_shared.modem_lp_state = MDM_LP_STATE_IDLE;
  HL78_shared.modem_resume_from_PSM = false;
}

static void init_HL78_qiurc_dnsgip(void)
{
  HL78_shared.QIURC_dnsgip_param.finished = AT_FALSE;
  HL78_shared.QIURC_dnsgip_param.wait_header = AT_TRUE;
  HL78_shared.QIURC_dnsgip_param.error = 0U;
  HL78_shared.QIURC_dnsgip_param.ip_count = 0U;
  HL78_shared.QIURC_dnsgip_param.ttl = 0U;
  (void) memset((void *)HL78_shared.QIURC_dnsgip_param.hostIPaddr, 0, MAX_SIZE_IPADDR);
}

static void socketHeaderRX_reset(void)
{
  (void) memset((void *)SocketHeaderDataRx_Buf, 0, 4U);
  SocketHeaderDataRx_Cpt = 0U;
  SocketHeaderDataRx_Cpt_Complete = 0U;
}
static void SocketHeaderRX_addChar(CRC_CHAR_t *rxchar)
{
  if ((SocketHeaderDataRx_Cpt_Complete == 0U) && (SocketHeaderDataRx_Cpt < 4U))
  {
    (void) memcpy((void *)&SocketHeaderDataRx_Buf[SocketHeaderDataRx_Cpt], (void *)rxchar, sizeof(char));
    SocketHeaderDataRx_Cpt++;
  }
}
static uint16_t SocketHeaderRX_getSize(void)
{
  uint16_t retval = (uint16_t) ATutil_convertStringToInt((uint8_t *)SocketHeaderDataRx_Buf,
                                                         (uint16_t)SocketHeaderDataRx_Cpt);
  return (retval);
}

static void display_decoded_GSM_bands(uint32_t gsm_bands)
{
  PRINT_INFO("GSM BANDS config = 0x%lx", gsm_bands)

  if ((gsm_bands & QCFGBANDGSM_900) != 0U)
  {
    PRINT_INFO("GSM_900")
  }
  if ((gsm_bands & QCFGBANDGSM_1800) != 0U)
  {
    PRINT_INFO("GSM_1800")
  }
  if ((gsm_bands & QCFGBANDGSM_850) != 0U)
  {
    PRINT_INFO("GSM_850")
  }
  if ((gsm_bands & QCFGBANDGSM_1900) != 0U)
  {
    PRINT_INFO("GSM_1900")
  }
}

static void display_decoded_CatM1_bands(uint32_t CatM1_bands_MsbPart, uint32_t CatM1_bands_LsbPart)
{
  PRINT_INFO("Cat.M1 BANDS config = 0x%lx%lx", CatM1_bands_MsbPart, CatM1_bands_LsbPart)

  /* LSB bitmap part: ........XXXXXXXX */
  if ((CatM1_bands_LsbPart & (uint32_t) QCFGBANDCATM1_B1_LSB) != 0U)
  {
    PRINT_INFO("CatM1_B1")
  }
  if ((CatM1_bands_LsbPart & (uint32_t) QCFGBANDCATM1_B2_LSB) != 0U)
  {
    PRINT_INFO("CatM1_B2")
  }
  if ((CatM1_bands_LsbPart & (uint32_t) QCFGBANDCATM1_B3_LSB) != 0U)
  {
    PRINT_INFO("CatM1_B3")
  }
  if ((CatM1_bands_LsbPart & (uint32_t) QCFGBANDCATM1_B4_LSB) != 0U)
  {
    PRINT_INFO("CatM1_B4")
  }
  if ((CatM1_bands_LsbPart & (uint32_t) QCFGBANDCATM1_B5_LSB) != 0U)
  {
    PRINT_INFO("CatM1_B5")
  }
  if ((CatM1_bands_LsbPart & (uint32_t) QCFGBANDCATM1_B8_LSB) != 0U)
  {
    PRINT_INFO("CatM1_B8")
  }
  if ((CatM1_bands_LsbPart & (uint32_t) QCFGBANDCATM1_B12_LSB) != 0U)
  {
    PRINT_INFO("CatM1_B12")
  }
  if ((CatM1_bands_LsbPart & (uint32_t) QCFGBANDCATM1_B13_LSB) != 0U)
  {
    PRINT_INFO("CatM1_B13")
  }
  if ((CatM1_bands_LsbPart & (uint32_t) QCFGBANDCATM1_B18_LSB) != 0U)
  {
    PRINT_INFO("CatM1_B18")
  }
  if ((CatM1_bands_LsbPart & (uint32_t) QCFGBANDCATM1_B19_LSB) != 0U)
  {
    PRINT_INFO("CatM1_B19")
  }
  if ((CatM1_bands_LsbPart & (uint32_t) QCFGBANDCATM1_B20_LSB) != 0U)
  {
    PRINT_INFO("CatM1_B20")
  }
  if ((CatM1_bands_LsbPart & (uint32_t) QCFGBANDCATM1_B26_LSB) != 0U)
  {
    PRINT_INFO("CatM1_B26")
  }
  if ((CatM1_bands_LsbPart & (uint32_t) QCFGBANDCATM1_B28_LSB) != 0U)
  {
    PRINT_INFO("CatM1_B28")
  }
  /* MSB bitmap part: XXXXXXXX........ */
  if ((CatM1_bands_MsbPart & (uint32_t) QCFGBANDCATM1_B39_MSB) != 0U)
  {
    PRINT_INFO("CatM1_B39")
  }
}

static void display_decoded_CatNB1_bands(uint32_t CatNB1_bands_MsbPart, uint32_t CatNB1_bands_LsbPart)
{
  UNUSED(CatNB1_bands_MsbPart); /* MSB part of NB1 band bitmap is not used */

  PRINT_INFO("Cat.NB1 BANDS config = 0x%lx%lx", CatNB1_bands_MsbPart, CatNB1_bands_LsbPart)

  /* LSB bitmap part: ........XXXXXXXX */
  if ((CatNB1_bands_LsbPart & (uint32_t) QCFGBANDCATNB1_B1_LSB) != 0U)
  {
    PRINT_INFO("CatNB1_B1")
  }
  if ((CatNB1_bands_LsbPart & (uint32_t) QCFGBANDCATNB1_B2_LSB) != 0U)
  {
    PRINT_INFO("CatNB1_B2")
  }
  if ((CatNB1_bands_LsbPart & (uint32_t) QCFGBANDCATNB1_B3_LSB) != 0U)
  {
    PRINT_INFO("CatNB1_B3")
  }
  if ((CatNB1_bands_LsbPart & (uint32_t) QCFGBANDCATNB1_B4_LSB) != 0U)
  {
    PRINT_INFO("CatNB1_B4")
  }
  if ((CatNB1_bands_LsbPart & (uint32_t) QCFGBANDCATNB1_B5_LSB) != 0U)
  {
    PRINT_INFO("CatNB1_B5")
  }
  if ((CatNB1_bands_LsbPart & (uint32_t) QCFGBANDCATNB1_B8_LSB) != 0U)
  {
    PRINT_INFO("CatNB1_B8")
  }
  if ((CatNB1_bands_LsbPart & (uint32_t) QCFGBANDCATNB1_B12_LSB) != 0U)
  {
    PRINT_INFO("CatNB1_B12")
  }
  if ((CatNB1_bands_LsbPart & (uint32_t) QCFGBANDCATNB1_B13_LSB) != 0U)
  {
    PRINT_INFO("CatNB1_B13")
  }
  if ((CatNB1_bands_LsbPart & (uint32_t) QCFGBANDCATNB1_B18_LSB) != 0U)
  {
    PRINT_INFO("CatNB1_B18")
  }
  if ((CatNB1_bands_LsbPart & (uint32_t) QCFGBANDCATNB1_B19_LSB) != 0U)
  {
    PRINT_INFO("CatNB1_B19")
  }
  if ((CatNB1_bands_LsbPart & (uint32_t) QCFGBANDCATNB1_B20_LSB) != 0U)
  {
    PRINT_INFO("CatNB1_B20")
  }
  if ((CatNB1_bands_LsbPart & (uint32_t) QCFGBANDCATNB1_B26_LSB) != 0U)
  {
    PRINT_INFO("CatNB1_B26")
  }
  if ((CatNB1_bands_LsbPart & (uint32_t) QCFGBANDCATNB1_B28_LSB) != 0U)
  {
    PRINT_INFO("CatNB1_B28")
  }
  /* MSB bitmap part: XXXXXXXX........ */
  /* no bands in MSB bitmap part */
}

//TODO: Ashu
static void display_user_friendly_mode_and_bands_config(void)
{
  uint8_t catM1_on = 0U;
  uint8_t catNB1_on = 0U;
  uint8_t gsm_on = 0U;
  ATCustom_HL78_QCFGscanseq_t scanseq_1st, scanseq_2nd, scanseq_3rd;

#if 0 /* for DEBUG */
  /* display modem raw values */
  display_decoded_CatM1_bands(HL78_shared.mode_and_bands_config.CatM1_bands);
  display_decoded_CatNB1_bands(HL78_shared.mode_and_bands_config.CatNB1_bands);
  display_decoded_GSM_bands(HL78_shared.mode_and_bands_config.gsm_bands);

  PRINT_INFO("nw_scanmode = 0x%x", HL78_shared.mode_and_bands_config.nw_scanmode)
  PRINT_INFO("iot_op_mode = 0x%x", HL78_shared.mode_and_bands_config.iot_op_mode)
  PRINT_INFO("nw_scanseq = 0x%x", HL78_shared.mode_and_bands_config.nw_scanseq)
#endif /* 0, for DEBUG */

  PRINT_INFO(">>>>> HL78 mode and bands configuration <<<<<")
  /* LTE bands */
  if ((HL78_shared.mode_and_bands_config.nw_scanmode == QCFGSCANMODE_AUTO) ||
      (HL78_shared.mode_and_bands_config.nw_scanmode == QCFGSCANMODE_LTEONLY))
  {
    if ((HL78_shared.mode_and_bands_config.iot_op_mode == QCFGIOTOPMODE_CATM1CATNB1) ||
        (HL78_shared.mode_and_bands_config.iot_op_mode == QCFGIOTOPMODE_CATM1))
    {
      /* LTE Cat.M1 active */
      catM1_on = 1U;
    }
    if ((HL78_shared.mode_and_bands_config.iot_op_mode == QCFGIOTOPMODE_CATM1CATNB1) ||
        (HL78_shared.mode_and_bands_config.iot_op_mode == QCFGIOTOPMODE_CATNB1))
    {
      /* LTE Cat.NB1 active */
      catNB1_on = 1U;
    }
  }

  /* GSM bands */
  if ((HL78_shared.mode_and_bands_config.nw_scanmode == QCFGSCANMODE_AUTO) ||
      (HL78_shared.mode_and_bands_config.nw_scanmode == QCFGSCANMODE_GSMONLY))
  {
    /* GSM active */
    gsm_on = 1U;
  }

  /* Search active techno */
  scanseq_1st = ((ATCustom_HL78_QCFGscanseq_t)HL78_shared.mode_and_bands_config.nw_scanseq &
                 (ATCustom_HL78_QCFGscanseq_t)0x00FF0000U) >> 16;
  scanseq_2nd = ((ATCustom_HL78_QCFGscanseq_t)HL78_shared.mode_and_bands_config.nw_scanseq &
                 (ATCustom_HL78_QCFGscanseq_t)0x0000FF00U) >> 8;
  scanseq_3rd = ((ATCustom_HL78_QCFGscanseq_t)HL78_shared.mode_and_bands_config.nw_scanseq &
                 (ATCustom_HL78_QCFGscanseq_t)0x000000FFU);
  PRINT_DBG("decoded scanseq: 0x%lx -> 0x%lx -> 0x%lx", scanseq_1st, scanseq_2nd, scanseq_3rd)

  uint8_t rank = 1U;
  /* display active bands by rank
   * rank is incremented only if a band is really active
   */
  /* 1st band */
  if (1U == display_if_active_band(scanseq_1st, rank, catM1_on, catNB1_on, gsm_on))
  {
    rank++;
  }
  /* 2nd band */
  if (1U == display_if_active_band(scanseq_2nd, rank, catM1_on, catNB1_on, gsm_on))
  {
    rank++;
  }
  /* 3rd band (do not need to increment rank as this is the last band */
  (void) display_if_active_band(scanseq_3rd, rank, catM1_on, catNB1_on, gsm_on);

  PRINT_INFO(">>>>> ................................. <<<<<")
}

static uint8_t display_if_active_band(ATCustom_HL78_QCFGscanseq_t scanseq,
                                      uint8_t rank,
                                      uint8_t catM1_on,
                                      uint8_t catNB1_on,
                                      uint8_t gsm_on)
{
  UNUSED(rank);
  uint8_t retval = 0U;

  if (scanseq == (ATCustom_HL78_QCFGscanseq_t) QCFGSCANSEQ_GSM)
  {
    if (gsm_on == 1U)
    {
      PRINT_INFO("GSM band active (scan rank = %d)", rank)
      display_decoded_GSM_bands(HL78_shared.mode_and_bands_config.gsm_bands);

      retval = 1U;
    }
  }
  else if (scanseq == (ATCustom_HL78_QCFGscanseq_t) QCFGSCANSEQ_LTECATM1)
  {
    if (catM1_on == 1U)
    {
      PRINT_INFO("LTE Cat.M1 band active (scan rank = %d)", rank)
      display_decoded_CatM1_bands(HL78_shared.mode_and_bands_config.CatM1_bands_MsbPart,
                                  HL78_shared.mode_and_bands_config.CatM1_bands_LsbPart);
      retval = 1U;
    }
  }
  else if (scanseq == (ATCustom_HL78_QCFGscanseq_t) QCFGSCANSEQ_LTECATNB1)
  {
    if (catNB1_on == 1U)
    {
      PRINT_INFO("LTE Cat.NB1 band active (scan rank = %d)", rank)
      display_decoded_CatNB1_bands(HL78_shared.mode_and_bands_config.CatNB1_bands_MsbPart,
                                   HL78_shared.mode_and_bands_config.CatNB1_bands_LsbPart);
      retval = 1U;
    }
  }
  else
  {
    /* scanseq value not managed */
  }

  return (retval);
}

#if (ENABLE_HL78_LOW_POWER_MODE == 1U)
/**
  * @brief  Set initial PSM and DRX states.
  *         Called in SID_CS_INIT_POWER_CONFIG
  * @param  none
  * @retval at_bool_t Indicates if low power is enabled.
  */
static at_bool_t init_HL78_low_power(atcustom_modem_context_t *p_modem_ctxt)
{
  at_bool_t lp_enabled;

  if (p_modem_ctxt->SID_ctxt.init_power_config.low_power_enable == CELLULAR_TRUE)
  {
    /* this parameter is used in CGREG/CEREG to enable or not PSM urc.
     * HL78 specific case: CEREG returns ERROR or UNKNOWN when CEREG=4 and CPSMS=0.
     * To avoid this, never use CEREG=4.
     */
    p_modem_ctxt->persist.psm_urc_requested = AT_FALSE;

    /* PSM and EDRX parameters: need to populate SID_ctxt.set_power_config which is the structure
     * used to build PSM and EDRX AT commands.
     * Provide psm and edrx default parameters.
     *
     * HL78 specific: always set psm_mode to PSM_MODE_DISABLE, will be set to enable only after a sleep request.
     */
    p_modem_ctxt->SID_ctxt.set_power_config.psm_present = CELLULAR_TRUE;
    p_modem_ctxt->SID_ctxt.set_power_config.psm_mode = PSM_MODE_ENABLE;
    (void) memcpy((void *) &p_modem_ctxt->SID_ctxt.set_power_config.psm,
                  (void *) &p_modem_ctxt->SID_ctxt.init_power_config.psm,
                  sizeof(CS_PSM_params_t));

    p_modem_ctxt->SID_ctxt.set_power_config.edrx_present = CELLULAR_TRUE;
    p_modem_ctxt->SID_ctxt.set_power_config.edrx_mode = EDRX_MODE_DISABLE;
    (void) memcpy((void *) &p_modem_ctxt->SID_ctxt.set_power_config.edrx,
                  (void *) &p_modem_ctxt->SID_ctxt.init_power_config.edrx,
                  sizeof(CS_EDRX_params_t));

    lp_enabled = AT_TRUE;
  }
  else
  {
    /* this parameter is used in CGREG/CEREG to enable or not PSM urc.  */
    p_modem_ctxt->persist.psm_urc_requested = AT_FALSE;

    /* do not send PSM and EDRX commands */
    lp_enabled = AT_FALSE;
  }

  return (lp_enabled);
}

static at_bool_t set_HL78_low_power(atcustom_modem_context_t *p_modem_ctxt)
{
  at_bool_t lp_set_and_enabled;

  if (p_modem_ctxt->SID_ctxt.set_power_config.psm_present == CELLULAR_TRUE)
  {
    /* the modem info structure SID_ctxt.set_power_config has been already updated */

    /* PSM parameters are present */
    if (p_modem_ctxt->SID_ctxt.set_power_config.psm_mode == PSM_MODE_ENABLE)
    {
      /* PSM is enabled */
      p_modem_ctxt->persist.psm_urc_requested = AT_TRUE;
      lp_set_and_enabled = AT_TRUE;
    }
    else
    {
      /* PSM is explicitly disabled */
      p_modem_ctxt->persist.psm_urc_requested = AT_FALSE;
      lp_set_and_enabled = AT_FALSE;

      /* RESET T3412 and T3324 values reported to upper layer */
      /* T3412, default value (0U means value not available) */
      p_modem_ctxt->persist.low_power_status.nwk_periodic_TAU = 0U;
      /* T3324, default value (0U means value not available) */
      p_modem_ctxt->persist.low_power_status.nwk_active_time = 0U;
      p_modem_ctxt->persist.urc_avail_lp_status = AT_TRUE;
    }
  }
  else
  {
    /* PSM parameters are not present */
    lp_set_and_enabled = AT_FALSE;
  }

  return (lp_set_and_enabled);
}

static void low_power_event(ATCustom_HL78_LP_event_t event)
{
  uint8_t host_state_error = 1U;
  uint8_t mdm_state_error = 1U;

  /* ##### manage HOST LP state ##### */
  if (HL78_shared.host_lp_state == HOST_LP_STATE_IDLE)
  {
    /* Host events */
    if (event == EVENT_LP_HOST_SLEEP_REQ)
    {
      HL78_shared.host_lp_state = HOST_LP_STATE_LP_REQUIRED;
      host_state_error = 0U;
    }
    /* Modem events */
    else if (event == EVENT_LP_MDM_ENTER_PSM)
    {
      /* Host in not in Low Power state, force modem wake up */
      PRINT_INFO("modem is in PSM state but Host is not, force wake up")
      /* a non null delay is used when called just after PSM POWER DOWN indication,
      *  otherwise this wake up request is not taken into account by the modem.
      */
      HL78_shared.host_lp_state = HOST_LP_STATE_WAKEUP_REQUIRED;
      (void) SysCtrl_HL78_wakeup_from_PSM(1000U);
      host_state_error = 0U;
    }
    else if (event == EVENT_LP_MDM_LEAVE_PSM)
    {
      /* host is already in idle state, no need to update host_lp_state */
      host_state_error = 0U;
    }
    else if (event == EVENT_LP_MDM_WAKEUP_REQ)
    {
      /* not implemented yet
       * is needed if UART is disabled by host (in case of STM32 low-power)
       */
    }
    else
    {
      /* unexpected event in this state */
    }
  }
  else if (HL78_shared.host_lp_state == HOST_LP_STATE_LP_REQUIRED)
  {
    /* Host events */
    if (event == EVENT_LP_HOST_SLEEP_COMPLETE)
    {
      HL78_shared.host_lp_state = HOST_LP_STATE_LP_ONGOING;
      host_state_error = 0U;
    }
    else if (event == EVENT_LP_HOST_SLEEP_CANCEL)
    {
      HL78_shared.host_lp_state = HOST_LP_STATE_IDLE;
      host_state_error = 0U;
    }
    /* Modem events */
    else if (event == EVENT_LP_MDM_ENTER_PSM)
    {
      /* Host has required Low Power state, do not force modem wake up */
      PRINT_INFO("modem entered in PSM state")
      host_state_error = 0U;
    }
    else if (event == EVENT_LP_MDM_LEAVE_PSM)
    {
      /* Crossing case.
       * Modem has left PSM state (T3412 expiry) while host is requiring Low Power.
       * Modem will returns automatically in PSM after T3324 expiry.
       */
      host_state_error = 0U;
    }
    else if (event == EVENT_LP_MDM_WAKEUP_REQ)
    {
      /* not implemented yet
       * is needed if UART is disabled by host (in case of STM32 low-power)
       */
    }
    else
    {
      /* unexpected event in this state */
    }
  }
  else if (HL78_shared.host_lp_state == HOST_LP_STATE_LP_ONGOING)
  {
    /* Host events */
    if (event == EVENT_LP_HOST_WAKEUP_REQ)
    {
      /* Host Wakeup request, check if modem is in PSM */
      if (HL78_shared.modem_lp_state == MDM_LP_STATE_PSM)
      {
        /* waiting for modem leaving PSM confirmation */
        HL78_shared.host_lp_state = HOST_LP_STATE_WAKEUP_REQUIRED;
        host_state_error = 0U;
      }
      else
      {
        /* modem not in PSM, no need to wait for modem confirmation*/
        HL78_shared.host_lp_state = HOST_LP_STATE_IDLE;
        host_state_error = 0U;
      }
    }
    /* Modem events */
    else if (event == EVENT_LP_MDM_ENTER_PSM)
    {
      /* Modem enters PSM (T3324 expiry) */
      host_state_error = 0U;
    }
    else if (event == EVENT_LP_MDM_LEAVE_PSM)
    {
      /* Modem has left PSM state while HOST is in Low Power state.
       * Modem will returns in PSM after T3324 expiry.
       */
      host_state_error = 0U;
    }
    else if (event == EVENT_LP_MDM_WAKEUP_REQ)
    {
      /* not implemented yet
       * is needed if UART is disabled by host (in case of STM32 low-power)
       */
    }
    else
    {
      /* unexpected event in this state */
    }
  }
  else if (HL78_shared.host_lp_state == HOST_LP_STATE_WAKEUP_REQUIRED)
  {
    /* Modem events */
    if (event == EVENT_LP_MDM_LEAVE_PSM)
    {
      HL78_shared.host_lp_state = HOST_LP_STATE_IDLE;
      host_state_error = 0U;
    }
    else
    {
      /* unexpected event in this state */
    }
  }
  else /* state equal HOST_LP_STATE_ERROR or unexpected */
  {
    /* error or unexpected state  */
    host_state_error = 2U;
  }

  /* ##### manage MODEM LP state ##### */
  if (HL78_shared.modem_lp_state == MDM_LP_STATE_IDLE)
  {
    if (event == EVENT_LP_MDM_ENTER_PSM)
    {
      HL78_shared.modem_lp_state = MDM_LP_STATE_PSM;
      mdm_state_error = 0U;
    }
    else
    {
      /* unexpected event in this state */
    }
  }
  else if (HL78_shared.modem_lp_state == MDM_LP_STATE_PSM)
  {
    if (event == EVENT_LP_MDM_LEAVE_PSM)
    {
      HL78_shared.modem_resume_from_PSM = true;
      HL78_shared.modem_lp_state = MDM_LP_STATE_IDLE;
      mdm_state_error = 0U;
    }
    else
    {
      /* unexpected event in this state */
    }
  }
  else /* state equal MDM_LP_STATE_ERROR or unexpected */
  {
    /* error or unexpected state  */
    mdm_state_error = 2U;
  }

  /* --- report automaton status --- */
  if (host_state_error == 0U)
  {
    PRINT_INFO("LP HOST AUTOMATON new state=%d ", HL78_shared.host_lp_state)
  }
  else if (host_state_error == 1U)
  {
    PRINT_INFO("LP HOST AUTOMATON ignore event %d in state %d", event, HL78_shared.host_lp_state)
  }
  else
  {
    PRINT_INFO("LP HOST AUTOMATON ERROR: unexpected state %d", HL78_shared.host_lp_state)
  }

  if (mdm_state_error == 0U)
  {
    PRINT_INFO("LP MODEM AUTOMATON new state=%d ", HL78_shared.modem_lp_state)
  }
  else if (mdm_state_error == 1U)
  {
    PRINT_INFO("LP MODEM AUTOMATON ignore event %d in state %d", event, HL78_shared.modem_lp_state)
  }
  else
  {
    PRINT_INFO("LP MODEM AUTOMATON ERROR: unexpected state %d", HL78_shared.modem_lp_state)
  }
  /* error codes: 1 for ignored event, 2 for unexpected state */
  if ((mdm_state_error != 0U) && (host_state_error != 0U))
  {
    PRINT_INFO("LP AUTOMATONS WARNING ignored event %d (mdm_err=%d, host_err=%d)",
               event, mdm_state_error, host_state_error)
  }
}

static bool is_modem_in_PSM_state(void)
{
  return (HL78_shared.modem_lp_state == MDM_LP_STATE_PSM);
}

static bool is_waiting_modem_wakeup(void)
{
  return (HL78_shared.host_lp_state == HOST_LP_STATE_WAKEUP_REQUIRED);
}

#endif /* ENABLE_HL78_LOW_POWER_MODE == 1U */

/************************ (C) COPYRIGHT Sierra Wireless *****END OF FILE****/

