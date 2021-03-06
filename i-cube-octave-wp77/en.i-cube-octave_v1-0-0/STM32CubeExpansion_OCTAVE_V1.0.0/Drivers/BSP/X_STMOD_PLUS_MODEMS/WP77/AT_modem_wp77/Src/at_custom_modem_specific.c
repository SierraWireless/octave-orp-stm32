/**
  ******************************************************************************
  * @file    at_custom_modem_specific.c
  * @author  Sierra Wireless Inc. and Affiliates
  * @brief   This file provides all the specific code to the
  *          WP77 Sierra Wireless modem: LTE-cat-M1 or LTE-cat.NB1(=NB-IOT) or GSM
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

/* WP77 COMPILATION FLAGS to define in project option if needed:*/

/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include "at_modem_api.h"
#include "at_modem_common.h"
#include "at_modem_signalling.h"
#include "at_modem_socket.h"
#include "at_custom_modem_specific.h"
#include "at_custom_modem_signalling.h"
#include "at_datapack.h"
#include "at_util.h"
#include "sysctrl_specific.h"
#include "cellular_runtime_standard.h"
#include "cellular_runtime_custom.h"
#include "plf_config.h"
#include "plf_modem_config.h"
#include "error_handler.h"

#if defined(USE_MODEM_WP77)
#if defined(HWREF_B_CELL_WP77_V2)
#else
#error Hardware reference not specified
#endif /* HWREF_B_CELL_WP77_V2 */
#endif /* USE_MODEM_WP77 */

/* Private typedef -----------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/
#if (USE_TRACE_ATCUSTOM_SPECIFIC == 1U)
#if (USE_PRINTF == 0U)
#include "trace_interface.h"
#define PRINT_INFO(format, args...) TRACE_PRINT(DBG_CHAN_ATCMD, DBL_LVL_P0, "WP77:" format "\n\r", ## args)
#define PRINT_DBG(format, args...)  TRACE_PRINT(DBG_CHAN_ATCMD, DBL_LVL_P1, "WP77:" format "\n\r", ## args)
#define PRINT_API(format, args...)  TRACE_PRINT(DBG_CHAN_ATCMD, DBL_LVL_P2, "WP77 API:" format "\n\r", ## args)
#define PRINT_ERR(format, args...)  TRACE_PRINT(DBG_CHAN_ATCMD, DBL_LVL_ERR, "WP77 ERROR:" format "\n\r", ## args)
#else
#define PRINT_INFO(format, args...)  (void) printf("WP77:" format "\n\r", ## args);
#define PRINT_DBG(...) __NOP(); /* Nothing to do */
#define PRINT_API(...) __NOP(); /* Nothing to do */
#define PRINT_ERR(format, args...)   (void) printf("WP77 ERROR:" format "\n\r", ## args);
#endif /* USE_PRINTF */
#else
#define PRINT_INFO(...) __NOP(); /* Nothing to do */
#define PRINT_DBG(...)  __NOP(); /* Nothing to do */
#define PRINT_API(...)  __NOP(); /* Nothing to do */
#define PRINT_ERR(...)  __NOP(); /* Nothing to do */
#endif /* USE_TRACE_ATCUSTOM_SPECIFIC */

/* Private defines -----------------------------------------------------------*/
#if (ENABLE_WP77_LOW_POWER_MODE == 1U)
/* add a pre-step */
#define CHECK_STEP(stepval) ((p_atp_ctxt->step) == ((stepval)+1U))
#define CHECK_STEP_EXCEEDS(stepval) (p_atp_ctxt->step >= ((stepval)+1U))
#define CHECK_STEP_BETWEEN(low_step, high_step) ((p_atp_ctxt->step >= ((low_step)+1U)) &&\
                                                 (p_atp_ctxt->step <= ((high_step)+1U)))
#else
#define CHECK_STEP(stepval) (p_atp_ctxt->step == stepval)
#define CHECK_STEP_EXCEEDS(stepval) (p_atp_ctxt->step >= stepval)
#define CHECK_STEP_BETWEEN(low_step, high_step) ((p_atp_ctxt->step >= low_step) && (p_atp_ctxt->step <= high_step))
#endif /* ENABLE_WP77_LOW_POWER_MODE == 1U */

/* ###########################  START CUSTOMIZATION PART  ########################### */
#define WP77_DEFAULT_TIMEOUT  ((uint32_t)15000U)
#define WP77_RDY_TIMEOUT      ((uint32_t)30000U)
#define WP77_APP_RDY_TIMEOUT  ((uint32_t)10000U)
#define WP77_SIMREADY_TIMEOUT ((uint32_t)3000U)
#define WP77_ESCAPE_TIMEOUT   ((uint32_t)1000U)  /* maximum time allowed to receive a response to an Escape command */
#define WP77_COPS_TIMEOUT     ((uint32_t)180000U) /* 180 sec */
#define WP77_CGATT_TIMEOUT    ((uint32_t)140000U) /* 140 sec */
#define WP77_CGACT_TIMEOUT    ((uint32_t)150000U) /* 150 sec */
#define WP77_ATH_TIMEOUT      ((uint32_t)90000U) /* 90 sec */
#define WP77_AT_TIMEOUT       ((uint32_t)1000U)  /* timeout for AT */
#define WP77_SOCKET_PROMPT_TIMEOUT ((uint32_t)10000U)
#define WP77_QIOPEN_TIMEOUT   ((uint32_t)150000U) /* 150 sec */
#define WP77_QICLOSE_TIMEOUT  ((uint32_t)150000U) /* 150 sec */
#define WP77_QIACT_TIMEOUT    ((uint32_t)150000U) /* 150 sec */
#define WP77_QIDEACT_TIMEOUT  ((uint32_t)40000U) /* 40 sec */
#define WP77_QNWINFO_TIMEOUT  ((uint32_t)1000U) /* 1000ms */
#define WP77_QIDNSGIP_TIMEOUT ((uint32_t)60000U) /* 60 sec */
#define WP77_QPING_TIMEOUT    ((uint32_t)150000U) /* 150 sec */
#define WP77_CPSMS_TIMEOUT    ((uint32_t)60000)
#define WP77_CEDRX_TIMEOUT    ((uint32_t)60000)

#define WP77_MODEM_SYNCHRO_AT_MAX_RETRIES ((uint8_t)30U)
#define WP77_MAX_SIM_STATUS_RETRIES       ((uint8_t)20U) /* maximum number of AT+QINISTAT retries to wait SIM ready
                                                          * multiply by WP77_SIMREADY_TIMEOUT to compute global
                                                          * timeout value
                                                          */

#if !defined(WP77_OPTION_NETWORK_INFO)
/* set default value */
#define WP77_OPTION_NETWORK_INFO (0)
#endif /* WP77_OPTION_NETWORK_INFO */

#if !defined(WP77_OPTION_ENGINEERING_MODE)
/* set default value */
#define WP77_OPTION_ENGINEERING_MODE (0)
#endif /* WP77_OPTION_ENGINEERING_MODE */

/* Global variables ----------------------------------------------------------*/
/* WP77 Modem device context */
static atcustom_modem_context_t WP77_ctxt;

/* shared variables specific to WP77 */
WP77_shared_variables_t WP77_shared =
{
  .QCFG_command_param              = QCFG_unknown,
  .QCFG_command_write              = AT_FALSE,
  .QINDCFG_command_param           = QINDCFG_unknown,
  .QIOPEN_waiting                  = AT_FALSE,
  .QIOPEN_current_socket_connected = 0U,
  .QICGSP_config_command           = AT_TRUE,
  .WP77_sim_status_retries         = 0U,
#if (USE_SOCKETS_TYPE == USE_SOCKETS_MODEM)
  .pdn_already_active              = false,
#endif /* USE_SOCKETS_TYPE */
  .host_lp_state                   = HOST_LP_STATE_IDLE,
  .modem_lp_state                  = MDM_LP_STATE_IDLE,
  .modem_resume_from_PSM           = false,
  .avms_connection_status          = 0U,
};

/* Private variables ---------------------------------------------------------*/

/* Socket Data receive: to analyze size received in data header */
static AT_CHAR_t SocketHeaderDataRx_Buf[4];
static uint8_t SocketHeaderDataRx_Cpt;
static uint8_t SocketHeaderDataRx_Cpt_Complete;

/* ###########################  END CUSTOMIZATION PART  ########################### */

/* Private function prototypes -----------------------------------------------*/
/* ###########################  START CUSTOMIZATION PART  ########################### */
static void WP77_modem_init(atcustom_modem_context_t *p_modem_ctxt);
static void WP77_modem_reset(atcustom_modem_context_t *p_modem_ctxt);
//static void reinitSyntaxAutomaton_WP77(void);
static void reset_variables_WP77(void);
//static void init_WP77_qiurc_dnsgip(void);
static void socketHeaderRX_reset(void);
static void SocketHeaderRX_addChar(CRC_CHAR_t *rxchar);
static uint16_t SocketHeaderRX_getSize(void);


#if (ENABLE_WP77_LOW_POWER_MODE == 1U)
static at_bool_t init_WP77_low_power(atcustom_modem_context_t *p_modem_ctxt);
static at_bool_t set_WP77_low_power(atcustom_modem_context_t *p_modem_ctxt);
static void low_power_event(ATCustom_WP77_LP_event_t event);
static bool is_modem_in_PSM_state(void);
static bool is_waiting_modem_wakeup(void);

#endif /* ENABLE_WP77_LOW_POWER_MODE == 1U */

/* ###########################  END CUSTOMIZATION PART  ########################### */

/* Functions Definition ------------------------------------------------------*/
void ATCustom_WP77_init(atparser_context_t *p_atp_ctxt)
{
  /* Commands Look-up table */
  static const atcustom_LUT_t ATCMD_WP77_LUT[] =
  {
    /* cmd enum - cmd string - cmd timeout (in ms) - build cmd ftion - analyze cmd ftion */
    {CMD_AT,             "",             WP77_AT_TIMEOUT,       fCmdBuild_NoParams,   fRspAnalyze_None},
    {CMD_AT_OK,          "OK",           WP77_DEFAULT_TIMEOUT,  fCmdBuild_NoParams,   fRspAnalyze_None},
    {CMD_AT_CONNECT,     "CONNECT",      WP77_DEFAULT_TIMEOUT,  fCmdBuild_NoParams,   fRspAnalyze_None},
    {CMD_AT_RING,        "RING",         WP77_DEFAULT_TIMEOUT,  fCmdBuild_NoParams,   fRspAnalyze_None},
    {CMD_AT_NO_CARRIER,  "NO CARRIER",   WP77_DEFAULT_TIMEOUT,  fCmdBuild_NoParams,   fRspAnalyze_None},
    {CMD_AT_ERROR,       "ERROR",        WP77_DEFAULT_TIMEOUT,  fCmdBuild_NoParams,   fRspAnalyze_Error_WP77},
    {CMD_AT_NO_DIALTONE, "NO DIALTONE",  WP77_DEFAULT_TIMEOUT,  fCmdBuild_NoParams,   fRspAnalyze_None},
    {CMD_AT_BUSY,        "BUSY",         WP77_DEFAULT_TIMEOUT,  fCmdBuild_NoParams,   fRspAnalyze_None},
    {CMD_AT_NO_ANSWER,   "NO ANSWER",    WP77_DEFAULT_TIMEOUT,  fCmdBuild_NoParams,   fRspAnalyze_None},
    {CMD_AT_CME_ERROR,   "+CME ERROR",   WP77_DEFAULT_TIMEOUT,  fCmdBuild_NoParams,   fRspAnalyze_Error_WP77},
    {CMD_AT_CMS_ERROR,   "+CMS ERROR",   WP77_DEFAULT_TIMEOUT,  fCmdBuild_NoParams,   fRspAnalyze_CmsErr},

    /* GENERIC MODEM commands */
    {CMD_AT_CGMI,        "+CGMI",        WP77_DEFAULT_TIMEOUT,  fCmdBuild_NoParams,   fRspAnalyze_CGMI},
    {CMD_AT_CGMM,        "+CGMM",        WP77_DEFAULT_TIMEOUT,  fCmdBuild_NoParams,   fRspAnalyze_CGMM},
    {CMD_AT_CGMR,        "+CGMR",        WP77_DEFAULT_TIMEOUT,  fCmdBuild_NoParams,   fRspAnalyze_CGMR},
    {CMD_AT_CGSN,        "+CGSN",        WP77_DEFAULT_TIMEOUT,  fCmdBuild_CGSN_WP77,  fRspAnalyze_CGSN},
    {CMD_AT_GSN,         "+GSN",         WP77_DEFAULT_TIMEOUT,  fCmdBuild_NoParams,   fRspAnalyze_GSN},
    {CMD_AT_CIMI,        "+CIMI",        WP77_DEFAULT_TIMEOUT,  fCmdBuild_NoParams,   fRspAnalyze_CIMI},
    {CMD_AT_CEER,        "+CEER",        WP77_DEFAULT_TIMEOUT,  fCmdBuild_NoParams,   fRspAnalyze_CEER},
    {CMD_AT_CMEE,        "+CMEE",        WP77_DEFAULT_TIMEOUT,  fCmdBuild_CMEE,       fRspAnalyze_None},
    {CMD_AT_CPIN,        "+CPIN",        WP77_DEFAULT_TIMEOUT,  fCmdBuild_CPIN,       fRspAnalyze_CPIN},
    {CMD_AT_CFUN,        "+CFUN",        WP77_DEFAULT_TIMEOUT,  fCmdBuild_CFUN,       fRspAnalyze_CFUN_WP77},
    {CMD_AT_COPS,        "+COPS",        WP77_COPS_TIMEOUT,     fCmdBuild_COPS,  	  fRspAnalyze_COPS},
    {CMD_AT_CNUM,        "+CNUM",        WP77_DEFAULT_TIMEOUT,  fCmdBuild_NoParams,   fRspAnalyze_CNUM},
    {CMD_AT_CGATT,       "+CGATT",       WP77_CGATT_TIMEOUT,    fCmdBuild_CGATT,      fRspAnalyze_CGATT},
    {CMD_AT_CGPADDR,     "+CGPADDR",     WP77_DEFAULT_TIMEOUT,  fCmdBuild_CGPADDR,    fRspAnalyze_CGPADDR},
    {CMD_AT_CEREG,       "+CEREG",       WP77_DEFAULT_TIMEOUT,  fCmdBuild_CEREG,      fRspAnalyze_CEREG},
    {CMD_AT_CREG,        "+CREG",        WP77_DEFAULT_TIMEOUT,  fCmdBuild_CREG,       fRspAnalyze_CREG},
    {CMD_AT_CGREG,       "+CGREG",       WP77_DEFAULT_TIMEOUT,  fCmdBuild_CGREG,      fRspAnalyze_CGREG},
    {CMD_AT_CSQ,         "+CSQ",         WP77_DEFAULT_TIMEOUT,  fCmdBuild_NoParams,   fRspAnalyze_CSQ},
    {CMD_AT_CGDCONT,     "+CGDCONT",     WP77_DEFAULT_TIMEOUT,  fCmdBuild_CGDCONT_WP77,    fRspAnalyze_None},
    {CMD_AT_CGACT,       "+CGACT",       WP77_CGACT_TIMEOUT,    fCmdBuild_CGACT,      fRspAnalyze_None},
    {CMD_AT_CGDATA,      "+CGDATA",      WP77_DEFAULT_TIMEOUT,  fCmdBuild_CGDATA,     fRspAnalyze_None},
    {CMD_AT_CGEREP,      "+CGEREP",      WP77_DEFAULT_TIMEOUT,  fCmdBuild_CGEREP,     fRspAnalyze_None},
    {CMD_AT_CGEV,        "+CGEV",        WP77_DEFAULT_TIMEOUT,  fCmdBuild_NoParams,   fRspAnalyze_CGEV},
    {CMD_ATD,            "D",            WP77_DEFAULT_TIMEOUT,  fCmdBuild_ATD_WP77,   fRspAnalyze_None},
    {CMD_ATE,            "E",            WP77_DEFAULT_TIMEOUT,  fCmdBuild_ATE,        fRspAnalyze_None},
    {CMD_ATH,            "H",            WP77_ATH_TIMEOUT,      fCmdBuild_NoParams,   fRspAnalyze_None},
    {CMD_ATO,            "O",            WP77_DEFAULT_TIMEOUT,  fCmdBuild_NoParams,   fRspAnalyze_None},
    {CMD_ATV,            "V",            WP77_DEFAULT_TIMEOUT,  fCmdBuild_ATV,        fRspAnalyze_None},
    {CMD_ATX,            "X",            WP77_DEFAULT_TIMEOUT,  fCmdBuild_ATX,        fRspAnalyze_None},
    {CMD_AT_ESC_CMD,     "+++",          WP77_ESCAPE_TIMEOUT,   fCmdBuild_ESCAPE_CMD, fRspAnalyze_None},
    {CMD_AT_IPR,         "+IPR",         WP77_DEFAULT_TIMEOUT,  fCmdBuild_IPR,        fRspAnalyze_IPR},
    {CMD_AT_IFC,         "+IFC",         WP77_DEFAULT_TIMEOUT,  fCmdBuild_IFC,        fRspAnalyze_None},
    {CMD_AT_AND_W,       "&W",           WP77_DEFAULT_TIMEOUT,  fCmdBuild_NoParams,   fRspAnalyze_None},
    {CMD_AT_AND_D,       "&D",           WP77_DEFAULT_TIMEOUT,  fCmdBuild_AT_AND_D,   fRspAnalyze_None},
    {CMD_AT_DIRECT_CMD,  "",             WP77_DEFAULT_TIMEOUT,  fCmdBuild_DIRECT_CMD, fRspAnalyze_DIRECT_CMD},
    {CMD_AT_CSIM,        "+CSIM",        WP77_DEFAULT_TIMEOUT,  fCmdBuild_CSIM,       fRspAnalyze_CSIM},
	{CMD_AT_CCID,        "+CCID",        WP77_DEFAULT_TIMEOUT,  fCmdBuild_NoParams,   fRspAnalyze_CCID_WP77},

    /* MODEM SPECIFIC COMMANDS */
	{CMD_AT_SELRAT,      "!SELRAT",      WP77_DEFAULT_TIMEOUT,  fCmdBuild_NoParams,   fRspAnalyze_SELRAT_WP77},
	{CMD_AT_WDSI,        "+WDSI",        WP77_DEFAULT_TIMEOUT,  fCmdBuild_WDSI_WP77,  fRspAnalyze_WDSI_WP77},
	{CMD_AT_KSUP,        "+KSUP",        WP77_DEFAULT_TIMEOUT,  fCmdBuild_NoParams,   fRspAnalyze_None},
	{CMD_AT_KSREP,       "+KSREP",       WP77_DEFAULT_TIMEOUT,  fCmdBuild_KSREP_WP77, fRspAnalyze_None},
	{CMD_AT_BAND,        "!BAND",        WP77_DEFAULT_TIMEOUT,  fCmdBuild_NoParams,   fRspAnalyze_BAND_WP77},
	{CMD_AT_SELACQ,      "!SELACQ",      WP77_DEFAULT_TIMEOUT,  fCmdBuild_NoParams,   fRspAnalyze_SELACQ_WP77},
	{CMD_AT_GSTATUS,     "!GSTATUS",     WP77_DEFAULT_TIMEOUT,  fCmdBuild_NoParams,   fRspAnalyze_GSTATUS_WP77},
    {CMD_AT_SOCKET_PROMPT, "> ",         WP77_SOCKET_PROMPT_TIMEOUT,  fCmdBuild_NoParams,   fRspAnalyze_None},
    {CMD_AT_SEND_OK,      "SEND OK",     WP77_DEFAULT_TIMEOUT,  fCmdBuild_NoParams,   fRspAnalyze_None},
    {CMD_AT_SEND_FAIL,    "SEND FAIL",   WP77_DEFAULT_TIMEOUT,  fCmdBuild_NoParams,   fRspAnalyze_None},
    {CMD_AT_KCELL,       "+KCELL",       WP77_DEFAULT_TIMEOUT,  fCmdBuild_KCELL_WP77, fRspAnalyze_KCELL_WP77},
    {CMD_AT_CPSMS,       "+CPSMS",       WP77_CPSMS_TIMEOUT,    fCmdBuild_CPSMS,  fRspAnalyze_CPSMS},
    {CMD_AT_CEDRXS,      "+CEDRXS",      WP77_CEDRX_TIMEOUT,    fCmdBuild_CEDRXS, fRspAnalyze_CEDRXS},
    {CMD_AT_CEDRXP,      "+CEDRXP",      WP77_CEDRX_TIMEOUT,    fCmdBuild_NoParams,   fRspAnalyze_CEDRXP},
    {CMD_AT_CEDRXRDP,    "+CEDRXRDP",    WP77_CEDRX_TIMEOUT,    fCmdBuild_NoParams,   fRspAnalyze_CEDRXRDP},

    /* MODEM SPECIFIC EVENTS */
    {CMD_AT_WAIT_EVENT,     "",          WP77_DEFAULT_TIMEOUT,        fCmdBuild_NoParams,   fRspAnalyze_None},
    {CMD_AT_BOOT_EVENT,     "",          WP77_RDY_TIMEOUT,            fCmdBuild_NoParams,   fRspAnalyze_None},
    {CMD_AT_RDY_EVENT,      "RDY",       WP77_RDY_TIMEOUT,            fCmdBuild_NoParams,   fRspAnalyze_None},
    {CMD_AT_APP_RDY_EVENT,  "APP RDY",   WP77_APP_RDY_TIMEOUT,        fCmdBuild_NoParams,   fRspAnalyze_None},
    {CMD_AT_POWERED_DOWN_EVENT,     "POWERED DOWN", WP77_RDY_TIMEOUT, fCmdBuild_NoParams,   fRspAnalyze_None},
    {CMD_AT_PSM_POWER_DOWN_EVENT, "PSM POWER DOWN", WP77_RDY_TIMEOUT, fCmdBuild_NoParams,   fRspAnalyze_None},
#if defined(USE_COM_MDM)
    {CMD_AT_ORP,     "+ORP",     WP77_DEFAULT_TIMEOUT,  fCmdBuild_ORP, fRspAnalyze_ORP},
#endif /* defined(USE_COM_MDM) */
  };
#define SIZE_ATCMD_WP77_LUT ((uint16_t) (sizeof (ATCMD_WP77_LUT) / sizeof (atcustom_LUT_t)))

  /* common init */
  WP77_modem_init(&WP77_ctxt);

  /* ###########################  START CUSTOMIZATION PART  ########################### */
  WP77_ctxt.modem_LUT_size = SIZE_ATCMD_WP77_LUT;
  WP77_ctxt.p_modem_LUT = (const atcustom_LUT_t *)ATCMD_WP77_LUT;

  /* override default termination string for AT command: <CR> */
  (void) sprintf((CRC_CHAR_t *)p_atp_ctxt->endstr, "\r");

  /* ###########################  END CUSTOMIZATION PART  ########################### */
}

uint8_t ATCustom_WP77_checkEndOfMsgCallback(uint8_t rxChar)
{
  uint8_t last_char = 0U;

  /* static variables */
  static const uint8_t QIRD_string[] = "+QIRD";
  static uint8_t QIRD_Counter = 0U;

  /*---------------------------------------------------------------------------------------*/
  if (WP77_ctxt.state_SyntaxAutomaton == WAITING_FOR_INIT_CR)
  {
    /* waiting for first valid <CR>, char received before are considered as trash */
    if ((AT_CHAR_t)('\r') == rxChar)
    {
      /* current     : xxxxx<CR>
      *  command format : <CR><LF>xxxxxxxx<CR><LF>
      *  waiting for : <LF>
      */
      WP77_ctxt.state_SyntaxAutomaton = WAITING_FOR_LF;
    }
  }
  /*---------------------------------------------------------------------------------------*/
  else if (WP77_ctxt.state_SyntaxAutomaton == WAITING_FOR_CR)
  {
    if ((AT_CHAR_t)('\r') == rxChar)
    {
      /* current     : xxxxx<CR>
      *  command format : <CR><LF>xxxxxxxx<CR><LF>
      *  waiting for : <LF>
      */
      WP77_ctxt.state_SyntaxAutomaton = WAITING_FOR_LF;
    }
  }
  /*---------------------------------------------------------------------------------------*/
  else if (WP77_ctxt.state_SyntaxAutomaton == WAITING_FOR_LF)
  {
    /* waiting for <LF> */
    if ((AT_CHAR_t)('\n') == rxChar)
    {
      /*  current        : <CR><LF>
      *   command format : <CR><LF>xxxxxxxx<CR><LF>
      *   waiting for    : x or <CR>
      */
      WP77_ctxt.state_SyntaxAutomaton = WAITING_FOR_FIRST_CHAR;
      last_char = 1U;
      QIRD_Counter = 0U;
    }
  }
  /*---------------------------------------------------------------------------------------*/
  else if (WP77_ctxt.state_SyntaxAutomaton == WAITING_FOR_FIRST_CHAR)
  {
    if (WP77_ctxt.socket_ctxt.socket_RxData_state == SocketRxDataState_waiting_header)
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
          WP77_ctxt.socket_ctxt.socket_RxData_state = SocketRxDataState_receiving_header;
        }
      }
    }

    /* NOTE:
    * if we are in socket_RxData_state = SocketRxDataState_waiting_header, we are waiting for +QIRD (test above)
    * but if we receive another message, we need to evacuate it without modifying socket_RxData_state
    * That's why we are nt using "else if" here, if <CR> if received before +QIND, it means that we have received
    * something else
    */
    if (WP77_ctxt.socket_ctxt.socket_RxData_state == SocketRxDataState_receiving_header)
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
        if (WP77_ctxt.socket_ctxt.socket_rx_expected_buf_size != size_from_header)
        {
          /* update buffer size received - should not happen */
          WP77_ctxt.socket_ctxt.socket_rx_expected_buf_size = size_from_header;
        }
        WP77_ctxt.state_SyntaxAutomaton = WAITING_FOR_LF;
        WP77_ctxt.socket_ctxt.socket_RxData_state = SocketRxDataState_receiving_data;
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
    else if (WP77_ctxt.socket_ctxt.socket_RxData_state == SocketRxDataState_receiving_data)
    {
      /* receiving socket data: do not analyze char, just count expected size
      *
      * +QIRD: 522<CR><LF>HTTP/1.1 200 OK<CR><LF><CR><LF>Date: Wed, 21 Feb 2018 14:56:54 GMT<CR><LF><CR><LF>Serve...
      *.                  ^- start to read data: count char
      */
      WP77_ctxt.socket_ctxt.socket_rx_count_bytes_received++;
      WP77_ctxt.state_SyntaxAutomaton = WAITING_FOR_SOCKET_DATA;
      /* check if full buffer has been received */
      if (WP77_ctxt.socket_ctxt.socket_rx_count_bytes_received == WP77_ctxt.socket_ctxt.socket_rx_expected_buf_size)
      {
        WP77_ctxt.socket_ctxt.socket_RxData_state = SocketRxDataState_data_received;
        WP77_ctxt.state_SyntaxAutomaton = WAITING_FOR_CR;
      }
    }
    /* waiting for <CR> or x */
    else if ((AT_CHAR_t)('\r') == rxChar)
    {
      /*   current        : <CR>
      *   command format : <CR><LF>xxxxxxxx<CR><LF>
      *   waiting for    : <LF>
      */
      WP77_ctxt.state_SyntaxAutomaton = WAITING_FOR_LF;
    }
    else {/* nothing to do */ }
  }
  /*---------------------------------------------------------------------------------------*/
  else if (WP77_ctxt.state_SyntaxAutomaton == WAITING_FOR_SOCKET_DATA)
  {
    WP77_ctxt.socket_ctxt.socket_rx_count_bytes_received++;
    /* check if full buffer has been received */
    if (WP77_ctxt.socket_ctxt.socket_rx_count_bytes_received == WP77_ctxt.socket_ctxt.socket_rx_expected_buf_size)
    {
      WP77_ctxt.socket_ctxt.socket_RxData_state = SocketRxDataState_data_received;
      WP77_ctxt.state_SyntaxAutomaton = WAITING_FOR_CR;
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
    /* WP77 special cases
    *
    *  SOCKET MODE: when sending DATA using AT+QISEND, we are waiting for socket prompt "<CR><LF>> "
    *               before to send DATA. Then we should receive "OK<CR><LF>".
    */

    if (WP77_ctxt.socket_ctxt.socket_send_state != SocketSendState_No_Activity)
    {
      switch (WP77_ctxt.socket_ctxt.socket_send_state)
      {
        case SocketSendState_WaitingPrompt1st_greaterthan:
        {
          /* detecting socket prompt first char: "greater than" */
          if ((AT_CHAR_t)('>') == rxChar)
          {
            WP77_ctxt.socket_ctxt.socket_send_state = SocketSendState_WaitingPrompt2nd_space;
          }
          break;
        }

        case SocketSendState_WaitingPrompt2nd_space:
        {
          /* detecting socket prompt second char: "space" */
          if ((AT_CHAR_t)(' ') == rxChar)
          {
            WP77_ctxt.socket_ctxt.socket_send_state = SocketSendState_Prompt_Received;
            last_char = 1U;
          }
          else
          {
            /* if char iommediatly after "greater than" is not a "space", reinit state */
            WP77_ctxt.socket_ctxt.socket_send_state = SocketSendState_WaitingPrompt1st_greaterthan;
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

at_status_t ATCustom_WP77_getCmd(at_context_t *p_at_ctxt, uint32_t *p_ATcmdTimeout)
{
  /* static variables */
  /* memorize number of SIM status query retries (static) */

  /* local variables */
  at_status_t retval = ATSTATUS_OK;
  atparser_context_t *p_atp_ctxt = &(p_at_ctxt->parser);
  at_msg_t curSID = p_atp_ctxt->current_SID;

  PRINT_API("enter ATCustom_WP77_getCmd() for SID %d", curSID)

  /* retrieve parameters from SID command (will update SID_ctxt) */
  if (atcm_retrieve_SID_parameters(&WP77_ctxt, p_atp_ctxt) != ATSTATUS_OK)
  {
    retval = ATSTATUS_ERROR;
    goto exit_ATCustom_WP77_getCmd;
  }

  /* new command: reset command context */
  atcm_reset_CMD_context(&WP77_ctxt.CMD_ctxt);

  /* For each SID, the sequence of AT commands to send id defined (it can be dynamic)
  * Determine and prepare the next command to send for this SID
  */

#if (ENABLE_WP77_LOW_POWER_MODE == 1U)
  /* WP77 specific
   * WP77 modem enters in PSM autonomously and suspends its uart.
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
      atcm_program_WAIT_EVENT(p_atp_ctxt, WP77_APP_RDY_TIMEOUT, INTERMEDIATE_CMD);
    }
    else
    {
      /* check if modem is resuming from PSM. In this case, disable echo */
      if (WP77_shared.modem_resume_from_PSM)
      {
        /* disable echo */
        WP77_ctxt.CMD_ctxt.command_echo = AT_FALSE;
        atcm_program_AT_CMD(&WP77_ctxt, p_atp_ctxt, ATTYPE_EXECUTION_CMD, (CMD_ID_t) CMD_ATE, INTERMEDIATE_CMD);
        WP77_shared.modem_resume_from_PSM = false;
      }
      else
      {
        /* nothing to do, skip the command */
        atcm_program_SKIP_CMD(p_atp_ctxt);
      }
    }

    goto proceed_ATCustom_WP77_buildCmd;
  }
#endif /* ENABLE_WP77_LOW_POWER_MODE == 1U */

  /* ###########################  START CUSTOMIZATION PART  ########################### */
  if (curSID == (at_msg_t) SID_CS_CHECK_CNX)
  {
    if CHECK_STEP((0U))
    {
      atcm_program_AT_CMD(&WP77_ctxt, p_atp_ctxt, ATTYPE_EXECUTION_CMD, (CMD_ID_t) CMD_AT, FINAL_CMD);
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
      atcm_program_AT_CMD(&WP77_ctxt, p_atp_ctxt, ATTYPE_EXECUTION_CMD, (CMD_ID_t) CMD_AT, FINAL_CMD);
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
    uint8_t common_start_sequence_step = WP77_MODEM_SYNCHRO_AT_MAX_RETRIES + 1U;
    /* POWER_ON and RESET are almost the same, specific differences are managed case by case */
    /* for reset, only HW reset is supported */
    if ((curSID == (at_msg_t) SID_CS_RESET) &&
        (WP77_ctxt.SID_ctxt.reset_type != CS_RESET_HW))
    {
      PRINT_ERR("Reset type (%d) not supported", WP77_ctxt.SID_ctxt.reset_type)
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
        reset_variables_WP77();

        /* reinit modem at ready status */
        WP77_ctxt.persist.modem_at_ready = AT_FALSE;

        /* in case of RESET, reset all the contexts to start from a fresh state */
        if (curSID == (at_msg_t) SID_CS_RESET)
        {
          WP77_modem_reset(&WP77_ctxt);
        }

        /* force requested flow control */
        atcm_program_AT_CMD(&WP77_ctxt, p_atp_ctxt, ATTYPE_WRITE_CMD, (CMD_ID_t) CMD_AT_IFC, INTERMEDIATE_CMD);
      }
      else if (CHECK_STEP_BETWEEN((1U), (WP77_MODEM_SYNCHRO_AT_MAX_RETRIES - 1U)))
      {
        /* start a loop to wait for modem : send AT commands */
        if (WP77_ctxt.persist.modem_at_ready == AT_FALSE)
        {
          /* use optional as we are not sure to receive a response from the modem: this allows to avoid to return
            * an error to upper layer
            */
          PRINT_DBG("test connection [try number %d] ", p_atp_ctxt->step)
          atcm_program_AT_CMD_ANSWER_OPTIONAL(&WP77_ctxt, p_atp_ctxt,
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
      else if CHECK_STEP((WP77_MODEM_SYNCHRO_AT_MAX_RETRIES))
      {
        /* if we fall here and the modem is not ready, we have a communication problem */
        if (WP77_ctxt.persist.modem_at_ready == AT_FALSE)
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
        atcm_program_AT_CMD(&WP77_ctxt, p_atp_ctxt, ATTYPE_WRITE_CMD, (CMD_ID_t) CMD_AT_IFC, INTERMEDIATE_CMD);
      }
      else if CHECK_STEP((common_start_sequence_step + 1U))
      {
        /* disable echo */
        WP77_ctxt.CMD_ctxt.command_echo = AT_FALSE;
        atcm_program_AT_CMD(&WP77_ctxt, p_atp_ctxt, ATTYPE_EXECUTION_CMD, (CMD_ID_t) CMD_ATE, INTERMEDIATE_CMD);
      }
      else if CHECK_STEP((common_start_sequence_step + 2U))
      {
        /* request detailed error report */
        atcm_program_AT_CMD(&WP77_ctxt, p_atp_ctxt, ATTYPE_WRITE_CMD, (CMD_ID_t) CMD_AT_CMEE, INTERMEDIATE_CMD);
      }
      else if CHECK_STEP((common_start_sequence_step + 3U))
      {
        /* enable full response format */
        WP77_ctxt.CMD_ctxt.dce_full_resp_format = AT_TRUE;
        atcm_program_AT_CMD(&WP77_ctxt, p_atp_ctxt, ATTYPE_EXECUTION_CMD, (CMD_ID_t) CMD_ATV, INTERMEDIATE_CMD);
      }
      else if CHECK_STEP((common_start_sequence_step + 4U))
      {
        /* deactivate DTR */
        atcm_program_AT_CMD(&WP77_ctxt, p_atp_ctxt, ATTYPE_EXECUTION_CMD, (CMD_ID_t) CMD_AT_AND_D, INTERMEDIATE_CMD);
      }
      else if CHECK_STEP((common_start_sequence_step + 5U))
      {
        /* Read FW revision */
        atcm_program_AT_CMD(&WP77_ctxt, p_atp_ctxt, ATTYPE_EXECUTION_CMD, (CMD_ID_t) CMD_AT_CGMR, INTERMEDIATE_CMD);
      }
      else if CHECK_STEP((common_start_sequence_step + 6U))
      {
        /* power on with AT+CFUN=0 */
        WP77_ctxt.CMD_ctxt.cfun_value = 0U;
        atcm_program_AT_CMD(&WP77_ctxt, p_atp_ctxt, ATTYPE_WRITE_CMD, (CMD_ID_t) CMD_AT_CFUN, INTERMEDIATE_CMD);
      }
      else if CHECK_STEP((common_start_sequence_step + 7U))
      {
        /* get RAT details */
        atcm_program_AT_CMD(&WP77_ctxt, p_atp_ctxt, ATTYPE_READ_CMD, (CMD_ID_t) CMD_AT_SELRAT, INTERMEDIATE_CMD);
      }
      else if CHECK_STEP((common_start_sequence_step + 8U))
      {
        /* get BAND details */
        atcm_program_AT_CMD(&WP77_ctxt, p_atp_ctxt, ATTYPE_READ_CMD, (CMD_ID_t) CMD_AT_BAND, INTERMEDIATE_CMD);
      }
      else if CHECK_STEP((common_start_sequence_step + 9U))
      {
    	/* get Scan Sequence details */
    	atcm_program_AT_CMD(&WP77_ctxt, p_atp_ctxt, ATTYPE_READ_CMD, (CMD_ID_t) CMD_AT_SELACQ, INTERMEDIATE_CMD);
      }
      else if CHECK_STEP((common_start_sequence_step + 10U))
      {
    	  /* Enable Boot Event notification */
          atcm_program_AT_CMD(&WP77_ctxt, p_atp_ctxt, ATTYPE_WRITE_CMD, (CMD_ID_t) CMD_AT_KSREP, INTERMEDIATE_CMD);
      }
      else if CHECK_STEP((common_start_sequence_step + 11U))
      {
        /* force to disable PSM in case modem was switched off with PSM enabled */
        WP77_ctxt.SID_ctxt.set_power_config.psm_present = CELLULAR_TRUE;
        WP77_ctxt.SID_ctxt.set_power_config.psm_mode = PSM_MODE_DISABLE;
        /******Ashu Modification******/
        //Changed to make CPSMS as final command
        atcm_program_AT_CMD(&WP77_ctxt, p_atp_ctxt, ATTYPE_WRITE_CMD, (CMD_ID_t) CMD_AT_CPSMS, FINAL_CMD);
      }

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
      /* since Sierra Wireless WP77 Hardware Design V1.4
        * Power Off sequence is done using GPIO
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
      atcm_program_AT_CMD(&WP77_ctxt, p_atp_ctxt, ATTYPE_WRITE_CMD, (CMD_ID_t) CMD_AT_CFUN, INTERMEDIATE_CMD);
      WP77_shared.WP77_sim_status_retries = 0U;
      WP77_shared.QINISTAT_error = AT_FALSE;
    }
    else if CHECK_STEP((1U))
    {
      if (WP77_ctxt.SID_ctxt.modem_init.init == CS_CMI_MINI)
      {
        /* minimum functionality selected, no SIM access => leave now */
        atcm_program_NO_MORE_CMD(p_atp_ctxt);
      }
      else
      {
        atcm_program_AT_CMD(&WP77_ctxt, p_atp_ctxt, ATTYPE_EXECUTION_CMD, (CMD_ID_t) CMD_AT_CCID, INTERMEDIATE_CMD);
      }

      /* reset SIM ready status, we will check it later */
      WP77_ctxt.persist.modem_sim_ready  = AT_FALSE;
    }
    else if CHECK_STEP((2U))
    {
      if (WP77_shared.WP77_sim_status_retries > WP77_MAX_SIM_STATUS_RETRIES)
      {
        if (WP77_shared.QINISTAT_error == AT_FALSE)
        {
          /* error, max sim status retries reached */
          atcm_program_NO_MORE_CMD(p_atp_ctxt);
          retval = ATSTATUS_ERROR;
        }
        else
        {

          WP77_ctxt.persist.modem_sim_ready  = AT_TRUE; /* assume that SIM is ready now */
          atcm_program_AT_CMD(&WP77_ctxt, p_atp_ctxt, ATTYPE_EXECUTION_CMD, (CMD_ID_t) CMD_AT_CGMR, INTERMEDIATE_CMD);
        }
      }
      else
      {
        /* if CPIN indicates that SIM is not ready, may be it's because there is a PIN code
          *  => check PIN code status regularly
          */
        if ((WP77_shared.WP77_sim_status_retries != 0U) &&
            ((WP77_shared.WP77_sim_status_retries % 3U) == 0U))
        {
          /* check PIN code status */
          atcm_program_AT_CMD(&WP77_ctxt, p_atp_ctxt, ATTYPE_READ_CMD, (CMD_ID_t) CMD_AT_CPIN, INTERMEDIATE_CMD);
        }
        else
        {

          /* request SIM status */
                    atcm_program_AT_CMD(&WP77_ctxt, p_atp_ctxt,
                    		ATTYPE_READ_CMD, (CMD_ID_t) CMD_AT_CPIN, INTERMEDIATE_CMD);
        }
      }
    }
    else if CHECK_STEP((3U))
    {
      if (WP77_ctxt.persist.modem_sim_ready == AT_FALSE)
      {
        if (WP77_ctxt.persist.sim_state == CS_SIMSTATE_SIM_PIN_REQUIRED)
        {
          /* SIM is ready but need PIN code, go to next step */
          atcm_program_SKIP_CMD(p_atp_ctxt);
          PRINT_INFO("SIM is ready, unlock sequence")
        }
        else
        {
          /* SIM not ready yet, wait before retry */
          atcm_program_TEMPO(p_atp_ctxt, WP77_SIMREADY_TIMEOUT, INTERMEDIATE_CMD);
          /* go back to previous step */
          p_atp_ctxt->step = p_atp_ctxt->step - 2U;
          PRINT_INFO("SIM not ready yet")
          WP77_shared.WP77_sim_status_retries++;
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
      /* reset WP77_sim_status_retries */
      WP77_shared.WP77_sim_status_retries = 0U;
      /* check is CPIN is requested */
      atcm_program_AT_CMD(&WP77_ctxt, p_atp_ctxt, ATTYPE_READ_CMD, (CMD_ID_t) CMD_AT_CPIN, INTERMEDIATE_CMD);
    }
    else if CHECK_STEP((5U))
    {
      if (WP77_ctxt.persist.sim_pin_code_ready == AT_FALSE)
      {
        if (strlen((const CRC_CHAR_t *)&WP77_ctxt.SID_ctxt.modem_init.pincode.pincode) != 0U)
        {
          /* send PIN value */
          PRINT_INFO("CPIN required, we send user value to modem")
          atcm_program_AT_CMD(&WP77_ctxt, p_atp_ctxt, ATTYPE_WRITE_CMD, (CMD_ID_t) CMD_AT_CPIN, INTERMEDIATE_CMD);
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
      atcm_program_AT_CMD(&WP77_ctxt, p_atp_ctxt, ATTYPE_READ_CMD, (CMD_ID_t) CMD_AT_CGDCONT, FINAL_CMD);
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
      switch (WP77_ctxt.SID_ctxt.device_info->field_requested)
      {
        case CS_DIF_MANUF_NAME_PRESENT:
          atcm_program_AT_CMD(&WP77_ctxt, p_atp_ctxt, ATTYPE_EXECUTION_CMD, (CMD_ID_t) CMD_AT_CGMI, FINAL_CMD);
          break;

        case CS_DIF_MODEL_PRESENT:
          atcm_program_AT_CMD(&WP77_ctxt, p_atp_ctxt, ATTYPE_EXECUTION_CMD, (CMD_ID_t) CMD_AT_CGMM, FINAL_CMD);
          break;

        case CS_DIF_REV_PRESENT:
          atcm_program_AT_CMD(&WP77_ctxt, p_atp_ctxt, ATTYPE_EXECUTION_CMD, (CMD_ID_t) CMD_AT_CGMR, FINAL_CMD);
          break;

        case CS_DIF_SN_PRESENT:
          WP77_ctxt.CMD_ctxt.cgsn_write_cmd_param = CGSN_SN;
          atcm_program_AT_CMD(&WP77_ctxt, p_atp_ctxt, ATTYPE_EXECUTION_CMD, (CMD_ID_t) CMD_AT_CGSN, FINAL_CMD);
          break;

        case CS_DIF_IMEI_PRESENT:
          atcm_program_AT_CMD(&WP77_ctxt, p_atp_ctxt, ATTYPE_EXECUTION_CMD, (CMD_ID_t) CMD_AT_GSN, FINAL_CMD);
          break;

        case CS_DIF_IMSI_PRESENT:
          atcm_program_AT_CMD(&WP77_ctxt, p_atp_ctxt, ATTYPE_EXECUTION_CMD, (CMD_ID_t) CMD_AT_CIMI, FINAL_CMD);
          break;

        case CS_DIF_PHONE_NBR_PRESENT:
          /* not AT+CNUM not supported by WP77 */
          retval = ATSTATUS_ERROR;
          break;

        case CS_DIF_ICCID_PRESENT:
          atcm_program_AT_CMD(&WP77_ctxt, p_atp_ctxt, ATTYPE_EXECUTION_CMD, (CMD_ID_t) CMD_AT_CCID, FINAL_CMD);
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
      /* get signal strength - Only enable for Debug */
      //atcm_program_AT_CMD(&WP77_ctxt, p_atp_ctxt, ATTYPE_READ_CMD, (CMD_ID_t) CMD_AT_GSTATUS, INTERMEDIATE_CMD );
      /* skip this step */
      atcm_program_SKIP_CMD(p_atp_ctxt);
    }
    else if CHECK_STEP((1U))
    {
      atcm_program_AT_CMD(&WP77_ctxt, p_atp_ctxt, ATTYPE_EXECUTION_CMD, (CMD_ID_t) CMD_AT_CSQ, FINAL_CMD );
    }
    /*else if CHECK_STEP((2U))
    {
       get cell details
      atcm_program_AT_CMD(&WP77_ctxt, p_atp_ctxt, ATTYPE_WRITE_CMD, (CMD_ID_t) CMD_AT_KCELL, FINAL_CMD);
    }*/
/*    else if CHECK_STEP((3U))
    {
#if (WP77_OPTION_NETWORK_INFO == 1)
      atcm_program_AT_CMD(&WP77_ctxt, p_atp_ctxt, ATTYPE_READ_CMD, (CMD_ID_t) CMD_AT_SELRAT,FINAL_CMD);
#else
      atcm_program_SKIP_CMD(p_atp_ctxt);
#endif  WP77_OPTION_NETWORK_INFO
    }*/
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
      atcm_program_AT_CMD(&WP77_ctxt, p_atp_ctxt, ATTYPE_READ_CMD, (CMD_ID_t) CMD_AT_CGATT, FINAL_CMD);
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
      atcm_program_AT_CMD(&WP77_ctxt, p_atp_ctxt, ATTYPE_READ_CMD, (CMD_ID_t) CMD_AT_COPS, INTERMEDIATE_CMD);
    }
    else if CHECK_STEP((1U))
    {
#if 0 /* check actual registration status */
      /* check if actual registration status is the expected one */
      CS_OperatorSelector_t *operatorSelect = &(WP77_ctxt.SID_ctxt.write_operator_infos);
      if (WP77_ctxt.SID_ctxt.read_operator_infos.mode != operatorSelect->mode)
      {
        /* write registration status */
        atcm_program_AT_CMD(&WP77_ctxt, p_atp_ctxt, ATTYPE_WRITE_CMD, (CMD_ID_t) CMD_AT_COPS, INTERMEDIATE_CMD);
      }
      else
      {
        /* skip this step */
        atcm_program_SKIP_CMD(p_atp_ctxt);
      }
#else
      /* due to problem observed on simu: does not register after reboot */
      atcm_program_AT_CMD(&WP77_ctxt, p_atp_ctxt, ATTYPE_WRITE_CMD, (CMD_ID_t) CMD_AT_COPS, INTERMEDIATE_CMD);
#endif /* 0 */
    }
    else if CHECK_STEP((2U))
    {
      /* read registration status */
      atcm_program_AT_CMD(&WP77_ctxt, p_atp_ctxt, ATTYPE_READ_CMD, (CMD_ID_t) CMD_AT_CEREG, INTERMEDIATE_CMD);
    }
    else if CHECK_STEP((3U))
    {
      /* read registration status */
      atcm_program_AT_CMD(&WP77_ctxt, p_atp_ctxt, ATTYPE_READ_CMD, (CMD_ID_t) CMD_AT_CREG, INTERMEDIATE_CMD);
    }
    else if CHECK_STEP((4U))
    {
      /* read registration status */
      atcm_program_AT_CMD(&WP77_ctxt, p_atp_ctxt, ATTYPE_READ_CMD, (CMD_ID_t) CMD_AT_CGREG, FINAL_CMD);
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
      atcm_program_AT_CMD(&WP77_ctxt, p_atp_ctxt, ATTYPE_READ_CMD, (CMD_ID_t) CMD_AT_CEREG, INTERMEDIATE_CMD);
    }
    else if CHECK_STEP((1U))
    {
      /* read registration status */
      atcm_program_AT_CMD(&WP77_ctxt, p_atp_ctxt, ATTYPE_READ_CMD, (CMD_ID_t) CMD_AT_CREG, INTERMEDIATE_CMD);
    }
    else if CHECK_STEP((2U))
    {
      /* read registration status */
      atcm_program_AT_CMD(&WP77_ctxt, p_atp_ctxt, ATTYPE_READ_CMD, (CMD_ID_t) CMD_AT_CGREG, INTERMEDIATE_CMD);
    }
    else if CHECK_STEP((3U))
    {
      /* read registration status */
      atcm_program_AT_CMD(&WP77_ctxt, p_atp_ctxt, ATTYPE_READ_CMD, (CMD_ID_t) CMD_AT_COPS, FINAL_CMD);
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
      CS_UrcEvent_t urcEvent = WP77_ctxt.SID_ctxt.urcEvent;

      /* is an event linked to CREG, CGREG or CEREG ? */
      if ((urcEvent == CS_URCEVENT_EPS_NETWORK_REG_STAT) || (urcEvent == CS_URCEVENT_EPS_LOCATION_INFO) ||
          (urcEvent == CS_URCEVENT_GPRS_NETWORK_REG_STAT) || (urcEvent == CS_URCEVENT_GPRS_LOCATION_INFO) ||
          (urcEvent == CS_URCEVENT_CS_NETWORK_REG_STAT) || (urcEvent == CS_URCEVENT_CS_LOCATION_INFO))
      {
        (void) atcm_subscribe_net_event(&WP77_ctxt, p_atp_ctxt);
      }
      else if (urcEvent == CS_URCEVENT_SIGNAL_QUALITY)
      {
        /* if signal quality URC not yet suscbribe */
        if (WP77_ctxt.persist.urc_subscript_signalQuality == CELLULAR_FALSE)
        {
          /* set event as subscribed */
          WP77_ctxt.persist.urc_subscript_signalQuality = CELLULAR_TRUE;

          /* request the URC we want */
          WP77_shared.QINDCFG_command_param = QINDCFG_csq;
          //atcm_program_AT_CMD(&WP77_ctxt, p_atp_ctxt, ATTYPE_WRITE_CMD, (CMD_ID_t) CMD_AT_QINDCFG, FINAL_CMD);
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
      CS_UrcEvent_t urcEvent = WP77_ctxt.SID_ctxt.urcEvent;

      /* is an event linked to CREG, CGREG or CEREG ? */
      if ((urcEvent == CS_URCEVENT_EPS_NETWORK_REG_STAT) || (urcEvent == CS_URCEVENT_EPS_LOCATION_INFO) ||
          (urcEvent == CS_URCEVENT_GPRS_NETWORK_REG_STAT) || (urcEvent == CS_URCEVENT_GPRS_LOCATION_INFO) ||
          (urcEvent == CS_URCEVENT_CS_NETWORK_REG_STAT) || (urcEvent == CS_URCEVENT_CS_LOCATION_INFO))
      {
        (void) atcm_unsubscribe_net_event(&WP77_ctxt, p_atp_ctxt);
      }
      else if (urcEvent == CS_URCEVENT_SIGNAL_QUALITY)
      {
        /* if signal quality URC suscbribed */
        if (WP77_ctxt.persist.urc_subscript_signalQuality == CELLULAR_TRUE)
        {
          /* set event as unsuscribed */
          WP77_ctxt.persist.urc_subscript_signalQuality = CELLULAR_FALSE;

          /* request the URC we don't want */
          WP77_shared.QINDCFG_command_param = QINDCFG_csq;
          //atcm_program_AT_CMD(&WP77_ctxt, p_atp_ctxt, ATTYPE_WRITE_CMD, (CMD_ID_t) CMD_AT_QINDCFG, FINAL_CMD);
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
      if (WP77_ctxt.persist.urc_subscript_pdn_event == CELLULAR_FALSE)
      {
        /* set event as subscribed */
        WP77_ctxt.persist.urc_subscript_pdn_event = CELLULAR_TRUE;

        /* request PDN events */
        atcm_program_AT_CMD(&WP77_ctxt, p_atp_ctxt, ATTYPE_WRITE_CMD, (CMD_ID_t) CMD_AT_CGEREP, INTERMEDIATE_CMD);
      }
      else
      {
        atcm_program_NO_MORE_CMD(p_atp_ctxt);
      }
    }
    else if CHECK_STEP((1U))
    {
      /* Write WDSI Indication mask */
      atcm_program_AT_CMD(&WP77_ctxt, p_atp_ctxt, ATTYPE_WRITE_CMD, (CMD_ID_t) CMD_AT_WDSI, FINAL_CMD);
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
      if (WP77_ctxt.persist.urc_subscript_pdn_event == CELLULAR_TRUE)
      {
        /* set event as unsuscribed */
        WP77_ctxt.persist.urc_subscript_pdn_event = CELLULAR_FALSE;

        /* request PDN events */
        atcm_program_AT_CMD(&WP77_ctxt, p_atp_ctxt, ATTYPE_WRITE_CMD, (CMD_ID_t) CMD_AT_CGEREP, FINAL_CMD);
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
      WP77_ctxt.CMD_ctxt.cgatt_write_cmd_param = CGATT_ATTACHED;
      /******Ashu Modification******/
      //Changed from Write to read as this functionality can trigger CGATT even when modem is not registered. Hence its a XCC Issue
      atcm_program_AT_CMD(&WP77_ctxt, p_atp_ctxt, ATTYPE_READ_CMD, (CMD_ID_t) CMD_AT_CGATT, FINAL_CMD);
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
      WP77_ctxt.CMD_ctxt.cgatt_write_cmd_param = CGATT_DETACHED;
      /******Ashu Modification******/
      /*Not necessary to perform detach from PDP before rebooting. This SID is called when the device detects error or modem freeze,
      so if modem is froozen,the  as there is risk of sending this command to already frozen modem*/
      //atcm_program_AT_CMD(&WP77_ctxt, p_atp_ctxt, ATTYPE_WRITE_CMD, (CMD_ID_t) CMD_AT_CGATT, FINAL_CMD);
      atcm_program_NO_MORE_CMD(p_atp_ctxt);
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
      WP77_shared.pdn_already_active = AT_FALSE;
      atcm_program_AT_CMD(&WP77_ctxt, p_atp_ctxt, ATTYPE_WRITE_CMD, (CMD_ID_t) CMD_AT_CGPADDR, INTERMEDIATE_CMD);

    }
    else if CHECK_STEP((1U))
    {
      AT_CHAR_t line[32] = {0};
      (void) memcpy((void *)&line[0],&WP77_ctxt.persist.modem_cid_table->ip_addr_infos.ip_addr_value,
    		  strlen((const CRC_CHAR_t *)&WP77_ctxt.persist.modem_cid_table->ip_addr_infos.ip_addr_value));
      if (((AT_CHAR_t *) strcmp((const CRC_CHAR_t *)&line[0], "0.0.0.0") != NULL) &&
    		  ((AT_CHAR_t *) strcmp((const CRC_CHAR_t *)&line[0], "") != NULL))
      {
        /* PDN already active - exit */
    	PRINT_INFO("Skip PDN activation (already active)")
        atcm_program_NO_MORE_CMD(p_atp_ctxt);
      }
      else
      {
        /* request PDN activation */
    	atcm_program_AT_CMD(&WP77_ctxt, p_atp_ctxt, ATTYPE_WRITE_CMD, (CMD_ID_t) CMD_AT_CGACT, INTERMEDIATE_CMD);
      }
    }
    else if CHECK_STEP((2U))
    {
      /* ckeck PDN state */
      atcm_program_AT_CMD(&WP77_ctxt, p_atp_ctxt, ATTYPE_WRITE_CMD, (CMD_ID_t) CMD_AT_CGPADDR, FINAL_CMD);
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
      atcm_program_AT_CMD(&WP77_ctxt, p_atp_ctxt, ATTYPE_WRITE_CMD, (CMD_ID_t) CMD_AT_CGPADDR, INTERMEDIATE_CMD);
    }
    else if CHECK_STEP((1U))
    {
      atcm_program_AT_CMD(&WP77_ctxt, p_atp_ctxt, ATTYPE_EXECUTION_CMD, (CMD_ID_t) CMD_ATX, INTERMEDIATE_CMD);
    }
    else if CHECK_STEP((2U))
    {
      atcm_program_AT_CMD(&WP77_ctxt, p_atp_ctxt, ATTYPE_EXECUTION_CMD, (CMD_ID_t) CMD_ATD, FINAL_CMD);
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
      WP77_shared.QICGSP_config_command = AT_TRUE;
      atcm_program_AT_CMD(&WP77_ctxt, p_atp_ctxt, ATTYPE_WRITE_CMD, (CMD_ID_t) CMD_AT_CGDCONT, FINAL_CMD);
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
      atcm_program_AT_CMD(&WP77_ctxt, p_atp_ctxt, ATTYPE_WRITE_CMD, (CMD_ID_t) CMD_AT_CGDCONT, FINAL_CMD);
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
    atcm_program_AT_CMD(&WP77_ctxt, p_atp_ctxt, ATTYPE_WRITE_CMD, (CMD_ID_t) CMD_AT_CGPADDR, FINAL_CMD);
#else
    /* DATA MODE*/
    atcm_program_AT_CMD(&WP77_ctxt, p_atp_ctxt, ATTYPE_WRITE_CMD, (CMD_ID_t) CMD_AT_CGPADDR, FINAL_CMD);
#endif /* USE_SOCKETS_TYPE */
  }

#if (ENABLE_WP77_LOW_POWER_MODE == 1U)
  else if (curSID == (at_msg_t) SID_CS_INIT_POWER_CONFIG)
  {
    if CHECK_STEP((0U))
    {
      /* Init parameters are available into SID_ctxt.init_power_config
        * SID_ctxt.init_power_config is used to build AT+CPSMS and AT+CEDRX commands
        * Build it from SID_ctxt.init_power_config and modem specificities
        */
      if (init_WP77_low_power(&WP77_ctxt) == AT_FALSE)
      {
        /* Low Power not enabled, stop here the SID */
        atcm_program_NO_MORE_CMD(p_atp_ctxt);
      }
      else
      {
        /* Low Power enabled, continue to next step
          * CEREG 4 not requested for WP77
          */
        atcm_program_SKIP_CMD(p_atp_ctxt);
      }
    }
    else if CHECK_STEP((1U))
    {
      /* read EDRX params */
      atcm_program_AT_CMD(&WP77_ctxt, p_atp_ctxt, ATTYPE_READ_CMD, (CMD_ID_t) CMD_AT_CEDRXS, INTERMEDIATE_CMD);
    }
    else if CHECK_STEP((2U))
    {
      /* read PSM params */
      atcm_program_AT_CMD(&WP77_ctxt, p_atp_ctxt, ATTYPE_READ_CMD, (CMD_ID_t) CMD_AT_CPSMS, INTERMEDIATE_CMD);
    }
    else if CHECK_STEP((3U))
    {
      /* set EDRX params (default) */
      atcm_program_AT_CMD(&WP77_ctxt, p_atp_ctxt, ATTYPE_WRITE_CMD, (CMD_ID_t) CMD_AT_CEDRXS, INTERMEDIATE_CMD);
    }
    else if CHECK_STEP((4U))
    {
      /* enable URC output */
      atcm_program_AT_CMD(&WP77_ctxt, p_atp_ctxt, ATTYPE_WRITE_CMD, (CMD_ID_t) CMD_AT_QURCCFG, INTERMEDIATE_CMD);
    }
    else if CHECK_STEP((5U))
    {
      /* request for +QPSMTIMER urc */
      WP77_shared.QCFG_command_write = AT_TRUE;
      WP77_shared.QCFG_command_param = QCFG_urc_psm;
      atcm_program_AT_CMD(&WP77_ctxt, p_atp_ctxt, ATTYPE_WRITE_CMD, (CMD_ID_t) CMD_AT_QCFG, INTERMEDIATE_CMD);
    }
    else if CHECK_STEP((6U))
    {
      /* set PSM params (default) */
      atcm_program_AT_CMD(&WP77_ctxt, p_atp_ctxt, ATTYPE_WRITE_CMD, (CMD_ID_t) CMD_AT_CPSMS, INTERMEDIATE_CMD);
    }
    else if CHECK_STEP((7U))
    {
      /* configure advanced PSM parameters */
      atcm_program_AT_CMD(&WP77_ctxt, p_atp_ctxt,
                          ATTYPE_READ_CMD, (CMD_ID_t) CMD_AT_QPSMEXTCFG, INTERMEDIATE_CMD);
    }
    else if CHECK_STEP((8U))
    {
      /* configure advanced PSM parameters */
      atcm_program_AT_CMD(&WP77_ctxt, p_atp_ctxt,
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
      (void) set_WP77_low_power(&WP77_ctxt);
      atcm_program_SKIP_CMD(p_atp_ctxt);
    }
    else if CHECK_STEP((1U))
    {
      /* set EDRX params (if available) */
      atcm_program_AT_CMD(&WP77_ctxt, p_atp_ctxt, ATTYPE_WRITE_CMD, (CMD_ID_t) CMD_AT_CEDRXS, INTERMEDIATE_CMD);
    }
    else if CHECK_STEP((2U))
    {
      /* set PSM params (if available) */
      atcm_program_AT_CMD(&WP77_ctxt, p_atp_ctxt, ATTYPE_WRITE_CMD, (CMD_ID_t) CMD_AT_CPSMS, INTERMEDIATE_CMD);
    }
    else if CHECK_STEP((3U))
    {
      /* eDRX Read Dynamix Parameters */
      atcm_program_AT_CMD(&WP77_ctxt, p_atp_ctxt,
                          ATTYPE_EXECUTION_CMD, (CMD_ID_t) CMD_AT_CEDRXRDP, INTERMEDIATE_CMD);
    }
    else if CHECK_STEP((4U))
    {
      atcm_program_AT_CMD(&WP77_ctxt, p_atp_ctxt,
                          ATTYPE_READ_CMD, (CMD_ID_t) CMD_AT_QPSMS, INTERMEDIATE_CMD);
    }
    else if CHECK_STEP((5U))
    {
      atcm_program_AT_CMD(&WP77_ctxt, p_atp_ctxt,
                          ATTYPE_READ_CMD, (CMD_ID_t) CMD_AT_QPSMCFG, INTERMEDIATE_CMD);
    }
    else if CHECK_STEP((6U))
    {
      atcm_program_AT_CMD(&WP77_ctxt, p_atp_ctxt,
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
      if (atcm_modem_event_received(&WP77_ctxt, CS_MDMEVENT_LP_ENTER) == AT_TRUE)
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
      if (WP77_ctxt.SID_ctxt.wakeup_origin == HOST_WAKEUP)
      {
        low_power_event(EVENT_LP_HOST_WAKEUP_REQ);

        if (is_modem_in_PSM_state() == true)
        {
          PRINT_INFO("this is a host wake up and modem is in PSM state")
          /* a non null delay is used in case wake-up is called just after PSM POWER DOWN indication,
            *  otherwise this wake up request is not taken into account by the modem
            */
          (void) SysCtrl_WP77_wakeup_from_PSM(1000U);
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
      atcm_program_WAIT_EVENT(p_atp_ctxt, WP77_APP_RDY_TIMEOUT, INTERMEDIATE_CMD);

    }
    else if CHECK_STEP((2U))
    {
      /* disable echo */
      WP77_ctxt.CMD_ctxt.command_echo = AT_FALSE;
      atcm_program_AT_CMD(&WP77_ctxt, p_atp_ctxt, ATTYPE_EXECUTION_CMD, (CMD_ID_t) CMD_ATE, INTERMEDIATE_CMD);
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
#endif /* ENABLE_WP77_LOW_POWER_MODE == 1U */

  else if (curSID == (at_msg_t) SID_CS_SUSBCRIBE_MODEM_EVENT)
  {
    /* nothing to do here
      * Indeed, default modem events subscribed havebeen saved automatically during analysis of SID command
      * cf function: atcm_retrieve_SID_parameters()
      */
    atcm_program_NO_MORE_CMD(p_atp_ctxt);
  }

  else if (curSID == (at_msg_t) SID_CS_DIRECT_CMD)
  {
    if CHECK_STEP((0U))
    {
      atcm_program_AT_CMD(&WP77_ctxt, p_atp_ctxt, ATTYPE_RAW_CMD, (CMD_ID_t) CMD_AT_DIRECT_CMD, FINAL_CMD);
      atcm_program_CMD_TIMEOUT(&WP77_ctxt, p_atp_ctxt, WP77_ctxt.SID_ctxt.direct_cmd_tx->cmd_timeout);
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
      if (atcm_select_hw_simslot(WP77_ctxt.persist.sim_selected) != ATSTATUS_OK)
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
      atcm_program_AT_CMD(&WP77_ctxt, p_atp_ctxt, ATTYPE_WRITE_CMD, (CMD_ID_t) CMD_AT_CSIM, FINAL_CMD);
    }
    else
    {
      /* error, invalid step */
      retval = ATSTATUS_ERROR;
    }
  }
  #if defined(USE_COM_MDM)
  else if (curSID == (at_msg_t) SID_CS_COM_MDM_TRANSACTION)
  {
    if CHECK_STEP((0U))
    {
      if (WP77_ctxt.SID_ctxt.com_mdm_data.transaction_type == CS_COMMDM_RECEIVE)
      {
        /* COM_MDM receive command */
        atcm_program_NO_MORE_CMD(p_atp_ctxt);

        /* try to retrieve stored message */
        if (orp_storage_get_msg(WP77_ctxt.SID_ctxt.com_mdm_data.rxBuffer.p_buffer,
                            WP77_ctxt.SID_ctxt.com_mdm_data.rxBuffer.max_buffer_size,
                            &(WP77_ctxt.SID_ctxt.com_mdm_data.rxBuffer.buffer_size)) < 0)
        {
          /* error when retrieving message */
          retval = ATSTATUS_ERROR;
        }

      }
      else
      {
        /* COM_MDM transaction or send command */
        atcm_program_AT_CMD(&WP77_ctxt, p_atp_ctxt, ATTYPE_WRITE_CMD, (CMD_ID_t) CMD_AT_ORP, FINAL_CMD);
      }
    }
    else
    {
      /* error, invalid step */
      retval = ATSTATUS_ERROR;
    }
  }
#endif /* defined(USE_COM_MDM) */

  /* ###########################  END CUSTOMIZATION PART  ########################### */
  else
  {
    PRINT_ERR("Error, invalid command ID %d", curSID)
    retval = ATSTATUS_ERROR;
  }

#if (ENABLE_WP77_LOW_POWER_MODE == 1U)
proceed_ATCustom_WP77_buildCmd:
#endif /* (ENABLE_WP77_LOW_POWER_MODE == 1U) */

  /* if no error, build the command to send */
  if (retval == ATSTATUS_OK)
  {
    retval = atcm_modem_build_cmd(&WP77_ctxt, p_atp_ctxt, p_ATcmdTimeout);
  }

exit_ATCustom_WP77_getCmd:
  return (retval);
}

at_endmsg_t ATCustom_WP77_extractElement(atparser_context_t *p_atp_ctxt,
                                         const IPC_RxMessage_t *p_msg_in,
                                         at_element_info_t *element_infos)
{
  at_endmsg_t retval_msg_end_detected = ATENDMSG_NO;
  bool exit_loop;
  uint16_t idx;
  uint16_t start_idx;
  uint16_t *p_parseIndex = &(element_infos->current_parse_idx);

  PRINT_API("enter ATCustom_WP77_extractElement()")
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
      /*if ((p_atp_ctxt->current_atcmd.id == (CMD_ID_t) CMD_AT_QIRD) &&
          (WP77_ctxt.socket_ctxt.socket_receive_state == SocketRcvState_RequestData_Payload) &&
          (WP77_ctxt.socket_ctxt.socket_RxData_state != SocketRxDataState_finished))
      {
        PRINT_DBG("receiving socket data (real size=%d)", SocketHeaderRX_getSize())
        element_infos->str_start_idx = 0U;
        element_infos->str_end_idx = (uint16_t) WP77_ctxt.socket_ctxt.socket_rx_count_bytes_received;
        element_infos->str_size = (uint16_t) WP77_ctxt.socket_ctxt.socket_rx_count_bytes_received;
        WP77_ctxt.socket_ctxt.socket_RxData_state = SocketRxDataState_finished;
        retval_msg_end_detected = ATENDMSG_YES;
        exit_loop = true;
      }*/
    }

    /* check if end of message has been detected */
    if (retval_msg_end_detected == ATENDMSG_YES)
    {
      goto exit_ATCustom_WP77_extractElement;
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
    goto exit_ATCustom_WP77_extractElement;
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
    	/* if the current received AT command is +ORP, ignore those characters
    	 * the ORP message is considered as one block, terminated by <CR><LF>
    	 */
    	if (element_infos->cmd_id_received != CMD_AT_ORP)
    	{
    		exit_loop = true;
    	}
    	else
    	{
    		/* increment end position */
    		element_infos->str_end_idx = *p_parseIndex;
    		element_infos->str_size++;
    	}
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

exit_ATCustom_WP77_extractElement:
  return (retval_msg_end_detected);
}

at_action_rsp_t ATCustom_WP77_analyzeCmd(at_context_t *p_at_ctxt,
                                         const IPC_RxMessage_t *p_msg_in,
                                         at_element_info_t *element_infos)
{
  atparser_context_t *p_atp_ctxt = &(p_at_ctxt->parser);
  at_action_rsp_t retval;

  PRINT_API("enter ATCustom_WP77_analyzeCmd()")

  /* Analyze data received from the modem and
    * search in LUT the ID corresponding to command received
  */
  if (ATSTATUS_OK != atcm_searchCmdInLUT(&WP77_ctxt, p_atp_ctxt, p_msg_in, element_infos))
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
    retval = atcm_check_text_line_cmd(&WP77_ctxt, p_at_ctxt, p_msg_in, element_infos);


    if (retval == ATACTION_RSP_NO_ACTION)
    {
      switch (p_atp_ctxt->current_atcmd.id)
      {
        /* ###########################  START CUSTOMIZED PART  ########################### */


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
            WP77_ctxt.persist.modem_at_ready = AT_TRUE;
          }
          if (p_atp_ctxt->current_atcmd.id == (CMD_ID_t) CMD_ATE)
          {
            PRINT_DBG("Echo successfully disabled")
          }
        }
        if (p_atp_ctxt->current_SID == (at_msg_t) SID_CS_PING_IP_ADDRESS)
        {
          /* PING requests for WP77 are asynchronous.
          * If the PING request is valid (no other ongoing ping request for example), the
          * modem will answer OK.
          * At this point, initialize the ping response structure.
          */
          PRINT_DBG("this is a valid PING request")
          atcm_validate_ping_request(&WP77_ctxt);
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
        WP77_ctxt.persist.modem_at_ready = AT_TRUE;
        if ((p_atp_ctxt->current_SID == (at_msg_t) SID_CS_POWER_ON) ||
            (p_atp_ctxt->current_SID == (at_msg_t) SID_CS_RESET))
        {
          /* ignore the RDY event during POWER ON or RESET */
          retval = ATACTION_RSP_URC_IGNORED;
        }
#if (ENABLE_WP77_LOW_POWER_MODE == 1U)
        else  if (is_modem_in_PSM_state() == true)
        {
          /* ignore this event when modem is in PSM, we will act on APP RDY instead
           * with APP RDY, all modem services should be ready
           */
          retval = ATACTION_RSP_URC_IGNORED;
        }
#endif /* ENABLE_WP77_LOW_POWER_MODE == 1U */
        else
        {
          /* if event is received in other states, it's an unexpected modem reboot */
          if (atcm_modem_event_received(&WP77_ctxt, CS_MDMEVENT_BOOT) == AT_TRUE)
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
#if (ENABLE_WP77_LOW_POWER_MODE == 1U)
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
#endif /* ENABLE_WP77_LOW_POWER_MODE == 1U */
        break;
      }

      case CMD_AT_POWERED_DOWN_EVENT:
      {
        PRINT_DBG("MODEM POWERED DOWN EVENT DETECTED")
        if (atcm_modem_event_received(&WP77_ctxt, CS_MDMEVENT_POWER_DOWN) == AT_TRUE)
        {
          retval = ATACTION_RSP_URC_FORWARDED;
        }
        else
        {
          retval = ATACTION_RSP_URC_IGNORED;
        }
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

      case CMD_AT_CFUN:
        retval = ATACTION_RSP_URC_IGNORED;
        break;

      case CMD_AT_CPIN:
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

      case CMD_AT_PSM_POWER_DOWN_EVENT:
#if (ENABLE_WP77_LOW_POWER_MODE == 1U)
        low_power_event(EVENT_LP_MDM_ENTER_PSM);
#endif /* ENABLE_WP77_LOW_POWER_MODE == 1U */
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
        retval = fRspAnalyze_Error_WP77(p_at_ctxt, &WP77_ctxt, p_msg_in, element_infos);
        break;

      case CMD_AT_CME_ERROR:
      case CMD_AT_CMS_ERROR:
        /* do the analyze here because will not be called by parser if the command
        * has no parameters (+CME ERROR only without parameters)
        */
        retval = fRspAnalyze_Error_WP77(p_at_ctxt, &WP77_ctxt, p_msg_in, element_infos);
        break;
      case CMD_AT_CGACT:
      case CMD_AT_GSTATUS:
      case CMD_AT_SELRAT:
      case CMD_AT_BAND:
      case CMD_AT_KCELL:
      case CMD_AT_SELACQ:
      case CMD_AT_ORP:
      case CMD_AT_CSQ:
      case CMD_AT_WDSI:
      case CMD_AT_COPS:
        retval = ATACTION_RSP_INTERMEDIATE;
        break;
      case CMD_AT_KSUP:
    	PRINT_INFO("KSUP URC Received")
        retval = ATACTION_RSP_IGNORED;
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

at_action_rsp_t ATCustom_WP77_analyzeParam(at_context_t *p_at_ctxt,
                                           const IPC_RxMessage_t *p_msg_in,
                                           at_element_info_t *element_infos)
{
  at_action_rsp_t retval;
  PRINT_API("enter ATCustom_WP77_analyzeParam()")

  /* analyse parameters of the command we received:
  * call the corresponding function from the LUT
  */
  retval = (atcm_get_CmdAnalyzeFunc(&WP77_ctxt, element_infos->cmd_id_received))(p_at_ctxt,
                                                                                 &WP77_ctxt,
                                                                                 p_msg_in,
                                                                                 element_infos);

  return (retval);
}

/* function called to finalize an AT command */
at_action_rsp_t ATCustom_WP77_terminateCmd(atparser_context_t *p_atp_ctxt, at_element_info_t *element_infos)
{
  at_action_rsp_t retval = ATACTION_RSP_IGNORED;
  PRINT_API("enter ATCustom_WP77_terminateCmd()")

  /* ###########################  START CUSTOMIZATION PART  ########################### */
  /* additional tests */
  if (WP77_ctxt.socket_ctxt.socket_send_state != SocketSendState_No_Activity)
  {
    /* special case for SID_CS_SEND_DATA
    * indeed, this function is called when an AT cmd is finished
    * but for AT+QISEND, it is called a 1st time when prompt is received
    * and a second time when data have been sent.
    */
    if (p_atp_ctxt->current_SID != (at_msg_t) SID_CS_SEND_DATA)
    {
      /* reset socket_send_state */
      WP77_ctxt.socket_ctxt.socket_send_state = SocketSendState_No_Activity;
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
at_status_t ATCustom_WP77_get_rsp(atparser_context_t *p_atp_ctxt, at_buf_t *p_rsp_buf)
{
  at_status_t retval;
  PRINT_API("enter ATCustom_WP77_get_rsp()")

  /* prepare response for a SID - common part */
  retval = atcm_modem_get_rsp(&WP77_ctxt, p_atp_ctxt, p_rsp_buf);

  /* ###########################  START CUSTOMIZATION PART  ########################### */
  /* prepare response for a SID
  *  all specific behaviors for SID which are returning data in rsp_buf have to be implemented here
  */
  switch (p_atp_ctxt->current_SID)
  {
    /*case SID_CS_DNS_REQ:*/
      /* PACK data to response buffer */
      /*if (DATAPACK_writeStruct(p_rsp_buf,
                               (uint16_t) CSMT_DNS_REQ,
                               (uint16_t) sizeof(WP77_shared.QIURC_dnsgip_param.hostIPaddr),
                               (void *)WP77_shared.QIURC_dnsgip_param.hostIPaddr) != DATAPACK_OK)
      {
        retval = ATSTATUS_OK;
      }
      break;*/

    case SID_CS_POWER_ON:
    case SID_CS_RESET:
      break;

    case SID_CS_POWER_OFF:
      /* reinit context for power off case */
      WP77_modem_reset(&WP77_ctxt);
      break;

#if (WP77_ACTIVATE_PING_REPORT == 1)
    case SID_CS_PING_IP_ADDRESS:
    {
      /* SID_CS_PING_IP_ADDRESS is waiting for a ping structure
       * For this modem, PING will be received later (as URC).
       * Just indicate that no ping report is available for now.
       */
      PRINT_DBG("Ping no report available yet - use PING_INVALID_INDEX")
      (void) memset((void *)&WP77_ctxt.persist.ping_resp_urc, 0, sizeof(CS_Ping_response_t));
      WP77_ctxt.persist.ping_resp_urc.index = PING_INVALID_INDEX;
      if (DATAPACK_writeStruct(p_rsp_buf,
                               (uint16_t) CSMT_URC_PING_RSP,
                               (uint16_t) sizeof(CS_Ping_response_t),
                               (void *)&WP77_ctxt.persist.ping_resp_urc) != DATAPACK_OK)
      {
        retval = ATSTATUS_OK;
      }
      break;
    }
#endif /* (WP77_ACTIVATE_PING_REPORT == 1) */

    default:
      break;
  }
  /* ###########################  END CUSTOMIZATION PART  ########################### */

  /* reset SID context */
  atcm_reset_SID_context(&WP77_ctxt.SID_ctxt);

  /* reset socket context */
  atcm_reset_SOCKET_context(&WP77_ctxt);

  return (retval);
}

at_status_t ATCustom_WP77_get_urc(atparser_context_t *p_atp_ctxt, at_buf_t *p_rsp_buf)
{
  at_status_t retval;
  PRINT_API("enter ATCustom_WP77_get_urc()")

  /* prepare response for an URC - common part */
  retval = atcm_modem_get_urc(&WP77_ctxt, p_atp_ctxt, p_rsp_buf);

  /* ###########################  START CUSTOMIZATION PART  ########################### */
  /* prepare response for an URC
  *  all specific behaviors for an URC have to be implemented here
  */

  /* ###########################  END CUSTOMIZATION PART  ########################### */

  return (retval);
}

at_status_t ATCustom_WP77_get_error(atparser_context_t *p_atp_ctxt, at_buf_t *p_rsp_buf)
{
  at_status_t retval;
  PRINT_API("enter ATCustom_WP77_get_error()")

  /* prepare response when an error occurred - common part */
  retval = atcm_modem_get_error(&WP77_ctxt, p_atp_ctxt, p_rsp_buf);

  /* ###########################  START CUSTOMIZATION PART  ########################### */
  /*  prepare response when an error occurred
  *  all specific behaviors for an error have to be implemented here
  */

  /* ###########################  END CUSTOMIZATION PART  ########################### */

  return (retval);
}

at_status_t ATCustom_WP77_hw_event(sysctrl_device_type_t deviceType, at_hw_event_t hwEvent, GPIO_PinState gstate)
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

/* WP77 modem init function
*  call common init function and then do actions specific to this modem
*/
static void WP77_modem_init(atcustom_modem_context_t *p_modem_ctxt)
{
  PRINT_API("enter WP77_modem_init")

  /* common init function (reset all contexts) */
  atcm_modem_init(p_modem_ctxt);

  /* modem specific actions if any */
}

/* WP77 modem reset function
*  call common reset function and then do actions specific to this modem
*/
static void WP77_modem_reset(atcustom_modem_context_t *p_modem_ctxt)
{
  PRINT_API("enter WP77_modem_reset")

  /* common reset function (reset all contexts except SID) */
  atcm_modem_reset(p_modem_ctxt);

  /* modem specific actions if any */
}

/*static void reinitSyntaxAutomaton_WP77(void)
{
  WP77_ctxt.state_SyntaxAutomaton = WAITING_FOR_INIT_CR;
}*/

static void reset_variables_WP77(void)
{
  /* Set default values of WP77 specific variables after SWITCH ON or RESET */
  WP77_shared.mode_and_bands_config.nw_scanseq = 0xFFFFFFFFU;
  WP77_shared.mode_and_bands_config.nw_scanmode = 0xFFFFFFFFU;
  WP77_shared.mode_and_bands_config.iot_op_mode = 0xFFFFFFFFU;
  WP77_shared.mode_and_bands_config.gsm_bands = 0xFFFFFFFFU;
  WP77_shared.mode_and_bands_config.CatM1_bands_MsbPart = 0xFFFFFFFFU;
  WP77_shared.mode_and_bands_config.CatM1_bands_LsbPart = 0xFFFFFFFFU;
  WP77_shared.mode_and_bands_config.CatNB1_bands_MsbPart = 0xFFFFFFFFU;
  WP77_shared.mode_and_bands_config.CatNB1_bands_LsbPart = 0xFFFFFFFFU;

  /* other values */
  WP77_shared.host_lp_state = HOST_LP_STATE_IDLE;
  WP77_shared.modem_lp_state = MDM_LP_STATE_IDLE;
  WP77_shared.modem_resume_from_PSM = false;
}

/*static void init_WP77_qiurc_dnsgip(void)
{
  WP77_shared.QIURC_dnsgip_param.finished = AT_FALSE;
  WP77_shared.QIURC_dnsgip_param.wait_header = AT_TRUE;
  WP77_shared.QIURC_dnsgip_param.error = 0U;
  WP77_shared.QIURC_dnsgip_param.ip_count = 0U;
  WP77_shared.QIURC_dnsgip_param.ttl = 0U;
  (void) memset((void *)WP77_shared.QIURC_dnsgip_param.hostIPaddr, 0, MAX_SIZE_IPADDR);
}*/

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

#if (ENABLE_WP77_LOW_POWER_MODE == 1U)
/**
  * @brief  Set initial PSM and DRX states.
  *         Called in SID_CS_INIT_POWER_CONFIG
  * @param  none
  * @retval at_bool_t Indicates if low power is enabled.
  */
static at_bool_t init_WP77_low_power(atcustom_modem_context_t *p_modem_ctxt)
{
  at_bool_t lp_enabled;

  if (p_modem_ctxt->SID_ctxt.init_power_config.low_power_enable == CELLULAR_TRUE)
  {
    /* this parameter is used in CGREG/CEREG to enable or not PSM urc.
     * WP77 specific case: CEREG returns ERROR or UNKNOWN when CEREG=4 and CPSMS=0.
     * To avoid this, never use CEREG=4.
     */
    p_modem_ctxt->persist.psm_urc_requested = AT_FALSE;

    /* PSM and EDRX parameters: need to populate SID_ctxt.set_power_config which is the structure
     * used to build PSM and EDRX AT commands.
     * Provide psm and edrx default parameters.
     *
     * WP77 specific: always set psm_mode to PSM_MODE_DISABLE, will be set to enable only after a sleep request.
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

static at_bool_t set_WP77_low_power(atcustom_modem_context_t *p_modem_ctxt)
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

static void low_power_event(ATCustom_WP77_LP_event_t event)
{
  uint8_t host_state_error = 1U;
  uint8_t mdm_state_error = 1U;

  /* ##### manage HOST LP state ##### */
  if (WP77_shared.host_lp_state == HOST_LP_STATE_IDLE)
  {
    /* Host events */
    if (event == EVENT_LP_HOST_SLEEP_REQ)
    {
      WP77_shared.host_lp_state = HOST_LP_STATE_LP_REQUIRED;
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
      WP77_shared.host_lp_state = HOST_LP_STATE_WAKEUP_REQUIRED;
      (void) SysCtrl_WP77_wakeup_from_PSM(1000U);
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
  else if (WP77_shared.host_lp_state == HOST_LP_STATE_LP_REQUIRED)
  {
    /* Host events */
    if (event == EVENT_LP_HOST_SLEEP_COMPLETE)
    {
      WP77_shared.host_lp_state = HOST_LP_STATE_LP_ONGOING;
      host_state_error = 0U;
    }
    else if (event == EVENT_LP_HOST_SLEEP_CANCEL)
    {
      WP77_shared.host_lp_state = HOST_LP_STATE_IDLE;
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
  else if (WP77_shared.host_lp_state == HOST_LP_STATE_LP_ONGOING)
  {
    /* Host events */
    if (event == EVENT_LP_HOST_WAKEUP_REQ)
    {
      /* Host Wakeup request, check if modem is in PSM */
      if (WP77_shared.modem_lp_state == MDM_LP_STATE_PSM)
      {
        /* waiting for modem leaving PSM confirmation */
        WP77_shared.host_lp_state = HOST_LP_STATE_WAKEUP_REQUIRED;
        host_state_error = 0U;
      }
      else
      {
        /* modem not in PSM, no need to wait for modem confirmation*/
        WP77_shared.host_lp_state = HOST_LP_STATE_IDLE;
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
  else if (WP77_shared.host_lp_state == HOST_LP_STATE_WAKEUP_REQUIRED)
  {
    /* Modem events */
    if (event == EVENT_LP_MDM_LEAVE_PSM)
    {
      WP77_shared.host_lp_state = HOST_LP_STATE_IDLE;
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
  if (WP77_shared.modem_lp_state == MDM_LP_STATE_IDLE)
  {
    if (event == EVENT_LP_MDM_ENTER_PSM)
    {
      WP77_shared.modem_lp_state = MDM_LP_STATE_PSM;
      mdm_state_error = 0U;
    }
    else
    {
      /* unexpected event in this state */
    }
  }
  else if (WP77_shared.modem_lp_state == MDM_LP_STATE_PSM)
  {
    if (event == EVENT_LP_MDM_LEAVE_PSM)
    {
      WP77_shared.modem_resume_from_PSM = true;
      WP77_shared.modem_lp_state = MDM_LP_STATE_IDLE;
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
    PRINT_INFO("LP HOST AUTOMATON new state=%d ", WP77_shared.host_lp_state)
  }
  else if (host_state_error == 1U)
  {
    PRINT_INFO("LP HOST AUTOMATON ignore event %d in state %d", event, WP77_shared.host_lp_state)
  }
  else
  {
    PRINT_INFO("LP HOST AUTOMATON ERROR: unexpected state %d", WP77_shared.host_lp_state)
  }

  if (mdm_state_error == 0U)
  {
    PRINT_INFO("LP MODEM AUTOMATON new state=%d ", WP77_shared.modem_lp_state)
  }
  else if (mdm_state_error == 1U)
  {
    PRINT_INFO("LP MODEM AUTOMATON ignore event %d in state %d", event, WP77_shared.modem_lp_state)
  }
  else
  {
    PRINT_INFO("LP MODEM AUTOMATON ERROR: unexpected state %d", WP77_shared.modem_lp_state)
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
  return (WP77_shared.modem_lp_state == MDM_LP_STATE_PSM);
}

static bool is_waiting_modem_wakeup(void)
{
  return (WP77_shared.host_lp_state == HOST_LP_STATE_WAKEUP_REQUIRED);
}

#endif /* ENABLE_WP77_LOW_POWER_MODE == 1U */

/************************ (C) COPYRIGHT Sierra Wireless *****END OF FILE****/

