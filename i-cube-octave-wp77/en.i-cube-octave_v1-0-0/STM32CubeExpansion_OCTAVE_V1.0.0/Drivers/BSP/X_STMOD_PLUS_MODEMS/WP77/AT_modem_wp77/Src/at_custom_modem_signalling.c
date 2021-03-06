/**
  ******************************************************************************
  * @file    at_custom_modem_signalling.c
  * @author  Sierra Wireless Inc. and Affiliates
  * @brief   This file provides all the 'signalling' code to the
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

/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include "at_modem_api.h"
#include "at_modem_common.h"
#include "at_modem_signalling.h"
#include "at_modem_socket.h"
#include "at_custom_modem_signalling.h"
#include "at_custom_modem_specific.h"
#include "at_datapack.h"
#include "at_util.h"
#include "cellular_runtime_standard.h"
#include "cellular_runtime_custom.h"
#include "plf_config.h"
#include "plf_modem_config.h"
#include "error_handler.h"

/* Private typedef -----------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/
#if (USE_TRACE_ATCUSTOM_SPECIFIC == 1U)
#if (USE_PRINTF == 0U)
#include "trace_interface.h"
#define PRINT_INFO(format, args...) TRACE_PRINT(DBG_CHAN_ATCMD, DBL_LVL_P0, "WP77:" format "\n\r", ## args)
#define PRINT_DBG(format, args...)  TRACE_PRINT(DBG_CHAN_ATCMD, DBL_LVL_P1, "WP77:" format "\n\r", ## args)
#define PRINT_API(format, args...)  TRACE_PRINT(DBG_CHAN_ATCMD, DBL_LVL_P2, "WP77 API:" format "\n\r", ## args)
#define PRINT_ERR(format, args...)  TRACE_PRINT(DBG_CHAN_ATCMD, DBL_LVL_ERR, "WP77 ERROR:" format "\n\r", ## args)
#define PRINT_BUF(pbuf, size)       TRACE_PRINT_BUF_CHAR(DBG_CHAN_ATCMD, DBL_LVL_P0, (const CRC_CHAR_t *)pbuf, size);
#else
#define PRINT_INFO(format, args...)  (void) printf("WP77:" format "\n\r", ## args);
#define PRINT_DBG(...)  __NOP(); /* Nothing to do */
#define PRINT_API(...)  __NOP(); /* Nothing to do */
#define PRINT_ERR(format, args...)   (void) printf("WP77 ERROR:" format "\n\r", ## args);
#define PRINT_BUF(...)  __NOP(); /* Nothing to do */
#endif /* USE_PRINTF */
#else
#define PRINT_INFO(...)  __NOP(); /* Nothing to do */
#define PRINT_DBG(...)   __NOP(); /* Nothing to do */
#define PRINT_API(...)   __NOP(); /* Nothing to do */
#define PRINT_ERR(...)   __NOP(); /* Nothing to do */
#define PRINT_BUF(...)   __NOP(); /* Nothing to do */
#endif /* USE_TRACE_ATCUSTOM_SPECIFIC */

/* START_PARAM_LOOP and END_PARAM_LOOP macros are used to loop on all fields
*  received in a message.
*  Only non-null length fields are analysed.
*  End the analyze when the end of the message or an error has been detected.
*/
#define START_PARAM_LOOP()  uint8_t exitcode = 0U;\
  do {\
    if (atcc_extractElement(p_at_ctxt, p_msg_in, element_infos) != ATENDMSG_NO) {exitcode = 1U;}\
    if (element_infos->str_size != 0U)\
    {\

#define END_PARAM_LOOP()  }\
  if (retval == ATACTION_RSP_ERROR) {exitcode = 1U;}\
  } while (exitcode == 0U);

/* Private defines -----------------------------------------------------------*/
/*
List of bands parameters (cf .h file to see the list of enum values for each parameter)
  - WP77_BAND_GSM    : hexadecimal value that specifies the GSM frequency band (cf AT+QCFG="band")
  - WP77_BAND_CAT_M1 : hexadecimal value that specifies the LTE Cat.M1 frequency band (cf AT+QCFG="band")
                       64bits bitmap split in two 32bits bitmaps  (MSB and LSB parts)
  - WP77_BAND_CAT_NB1: hexadecimal value that specifies the LTE Cat.NB1 frequency band (cf AT+QCFG="band")
                       64bits bitmap split in two 32bits bitmaps  (MSB and LSB parts)
  - WP77_IOTOPMODE   : network category to be searched under LTE network (cf AT+QCFG="iotopmode")
  - WP77_SCANSEQ     : network search sequence (GSM, Cat.M1, Cat.NB1) (cf AT+QCFG="nwscanseq")
  - WP77_SCANMODE    : network to be searched (cf AT+QCFG="nwscanmode")

Below are define default band values that will be used if calling write form of AT+QCFG
(mainly for test purposes)
*/
#define WP77_BAND_GSM          ((ATCustom_WP77_QCFGbandGSM_t)    QCFGBANDGSM_ANY)
#define WP77_BAND_CAT_M1_MSB   ((ATCustom_WP77_QCFGbandCatM1_t)  QCFGBANDCATM1_ANY_MSB)
#define WP77_BAND_CAT_M1_LSB   ((ATCustom_WP77_QCFGbandCatM1_t)  QCFGBANDCATM1_ANY_LSB)
#define WP77_BAND_CAT_NB1_MSB  ((ATCustom_WP77_QCFGbandCatNB1_t) QCFGBANDCATNB1_ANY_MSB)
#define WP77_BAND_CAT_NB1_LSB  ((ATCustom_WP77_QCFGbandCatNB1_t) QCFGBANDCATNB1_ANY_LSB)
#define WP77_IOTOPMODE         ((ATCustom_WP77_QCFGiotopmode_t)  QCFGIOTOPMODE_CATM1CATNB1)
#define WP77_SCANSEQ           ((ATCustom_WP77_QCFGscanseq_t)    QCFGSCANSEQ_M1_NB1_GSM)
#define WP77_SCANMODE          ((ATCustom_WP77_QCFGscanmode_t)   QCFGSCANMODE_AUTO)

#define WP77_PDP_DUPLICATECHK_ENABLE ((uint8_t)0U) /* parameter of AT+QCFG="PDP/DuplicateChk":
                                                    * 0 to refuse, 1 to allow
                                                    */

#define WP77_PSM_URC_ENABLE ((uint8_t)1U) /* parameter of AT+QCFG="psm/urc":
                                           * 0 to disable QPSMTIMER URC report
                                           * 1 to enable QPSMTIMER URC report
                                           */

/* Global variables ----------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

/* Functions Definition ------------------------------------------------------*/
/* Build command functions ------------------------------------------------------- */
at_status_t fCmdBuild_ATD_WP77(atparser_context_t *p_atp_ctxt, atcustom_modem_context_t *p_modem_ctxt)
{
  at_status_t retval = ATSTATUS_OK;
  PRINT_API("enter fCmdBuild_ATD_WP77()")

  /* only for execution command, set parameters */
  if (p_atp_ctxt->current_atcmd.type == ATTYPE_EXECUTION_CMD)
  {
    CS_PDN_conf_id_t current_conf_id = atcm_get_cid_current_SID(p_modem_ctxt);
    uint8_t modem_cid = atcm_get_affected_modem_cid(&p_modem_ctxt->persist, current_conf_id);
    PRINT_INFO("Activate PDN (user cid = %d, modem cid = %d)", (uint8_t)current_conf_id, modem_cid)

    (void) sprintf((CRC_CHAR_t *)p_atp_ctxt->current_atcmd.params, "*99***%d#", modem_cid);

    /* (void) sprintf((CRC_CHAR_t *)p_atp_ctxt->current_atcmd.params , "*99#" ); */
  }
  return (retval);
}

at_status_t fCmdBuild_CGSN_WP77(atparser_context_t *p_atp_ctxt, atcustom_modem_context_t *p_modem_ctxt)
{
  UNUSED(p_modem_ctxt);
  at_status_t retval = ATSTATUS_OK;
  PRINT_API("enter fCmdBuild_CGSN_WP77()")

  /* only for write command, set parameters */
  if (p_atp_ctxt->current_atcmd.type == ATTYPE_WRITE_CMD)
  {
    /* WP77 only supports EXECUTION form of CGSN */
    retval = ATSTATUS_ERROR;
  }
  return (retval);
}



at_status_t fCmdBuild_CGDCONT_WP77(atparser_context_t *p_atp_ctxt, atcustom_modem_context_t *p_modem_ctxt)
{
  at_status_t retval;
  PRINT_API("enter fCmdBuild_CGDCONT_WP77()")

  /* normal case */
  retval = fCmdBuild_CGDCONT(p_atp_ctxt, p_modem_ctxt);

  return (retval);
}


at_status_t fCmdBuild_COPS_WP77(atparser_context_t *p_atp_ctxt, atcustom_modem_context_t *p_modem_ctxt)
{
  UNUSED(p_modem_ctxt);
  at_status_t retval;
  PRINT_API("enter fCmdBuild_COPS_WP77()")

  /* WP77 does not follow 3GPP TS 27.007 values for <AcT> parameter in AT+COPS
   * Encapsulate generic COPS build function to manage this difference
   */

  CS_OperatorSelector_t *operatorSelect = &(p_modem_ctxt->SID_ctxt.write_operator_infos);
  if (operatorSelect->AcT_present == CELLULAR_TRUE)
  {
    if (operatorSelect->AcT == CS_ACT_E_UTRAN)
    {
      /* WP77 AcT = 8 means cat.M1
      *  3GPP AcT = 8 means CS_ACT_EC_GSM_IOT, cat.M1 value is 9
      *  convert 9 to 8 for WP77
      */
      operatorSelect->AcT = CS_ACT_EC_GSM_IOT;
    }
  }

  /* finally call the common COPS function */
  retval = fCmdBuild_COPS(p_atp_ctxt, p_modem_ctxt);

  return (retval);
}

at_status_t fCmdBuild_KCELL_WP77(atparser_context_t *p_atp_ctxt, atcustom_modem_context_t *p_modem_ctxt)
{
	UNUSED(p_modem_ctxt);
	at_status_t retval = ATSTATUS_OK;
	PRINT_API("enter fCmdBuild_KCELL_WP77()")

	/* only for write command, set parameters */
	if (p_atp_ctxt->current_atcmd.type == ATTYPE_WRITE_CMD)
	{
	  /* Normal Power Down */
	  (void) sprintf((CRC_CHAR_t *)p_atp_ctxt->current_atcmd.params, "0");
	}
	return (retval);
}

at_status_t fCmdBuild_WDSI_WP77(atparser_context_t *p_atp_ctxt, atcustom_modem_context_t *p_modem_ctxt)
{
	UNUSED(p_modem_ctxt);
	at_status_t retval = ATSTATUS_OK;
	PRINT_API("enter fCmdBuild_WDSI_WP77()")

	/* only for write command, set parameters */
	if (p_atp_ctxt->current_atcmd.type == ATTYPE_WRITE_CMD)
	{
	  /* Normal Power Down */
	  (void) sprintf((CRC_CHAR_t *)p_atp_ctxt->current_atcmd.params, "4096");
	}
	return (retval);
}

at_status_t fCmdBuild_KSREP_WP77(atparser_context_t *p_atp_ctxt, atcustom_modem_context_t *p_modem_ctxt)
{
  at_status_t retval = ATSTATUS_OK;
  PRINT_API("enter fCmdBuild_KSREP_WP77()")

  /* only for write command, set parameters */
  if (p_atp_ctxt->current_atcmd.type == ATTYPE_WRITE_CMD)
  {
	  (void) sprintf((CRC_CHAR_t *)p_atp_ctxt->current_atcmd.params, "%d",
	                     1);
  }
  return (retval);
}

/* Analyze command functions ------------------------------------------------------- */

at_action_rsp_t fRspAnalyze_Error_WP77(at_context_t *p_at_ctxt, atcustom_modem_context_t *p_modem_ctxt,
                                       const IPC_RxMessage_t *p_msg_in, at_element_info_t *element_infos)
{
  atparser_context_t *p_atp_ctxt = &(p_at_ctxt->parser);
  at_action_rsp_t retval = ATACTION_RSP_ERROR;
  PRINT_API("enter fRspAnalyze_Error_WP77()")

  switch (p_atp_ctxt->current_SID)
  {
    case SID_CS_DIAL_COMMAND:
      /* in case of error during socket connection,
      * release the modem CID for this socket_handle
      */
      (void) atcm_socket_release_modem_cid(p_modem_ctxt, p_modem_ctxt->socket_ctxt.socket_info->socket_handle);
      break;

    default:
      /* nothing to do */
      break;
  }

  /* analyze Error for WP77 */
  switch (p_atp_ctxt->current_atcmd.id)
  {
    case CMD_AT_CREG:
    case CMD_AT_CGREG:
    case CMD_AT_CEREG:
    case CMD_AT_CSQ:
      /* error is ignored */
      retval = ATACTION_RSP_FRC_END;
      break;

    case CMD_AT_CPSMS:
    case CMD_AT_CEDRXS:
      /* error is ignored */
      retval = ATACTION_RSP_FRC_END;
      break;

    case CMD_AT_CGDCONT:
      if (p_atp_ctxt->current_SID == (at_msg_t) SID_CS_INIT_MODEM)
      {
        /* error is ignored in this case because this cmd is only informative */
        retval = ATACTION_RSP_FRC_END;
      }
      break;

    case CMD_AT_CPIN:
      /* error is ignored when WP77_sim_status_retries is not null
       *
       */
      if (WP77_shared.WP77_sim_status_retries != 0U)
      {
        PRINT_INFO("error ignored (waiting for SIM ready)")
        retval = ATACTION_RSP_FRC_CONTINUE;
      }
      else
      {
        retval = fRspAnalyze_Error(p_at_ctxt, p_modem_ctxt, p_msg_in, element_infos);
      }
      break;
    case CMD_AT_KCELL:
    case CMD_AT_SELRAT:
    case CMD_AT_BAND:
    case CMD_AT_SELACQ:
      /* error is ignored */
      retval = ATACTION_RSP_FRC_END;
      break;
	#if defined(USE_COM_MDM)
    case CMD_AT_ORP:
      /* try to get +CME ERROR code only for +CME ERROR (indeed, same function is used also for ERROR only)*/
      if (element_infos->cmd_id_received == (uint32_t) CMD_AT_CME_ERROR)
      {
        /* by default in this function, retval is returning an error. To avoid leave parsing immedialty after
         * command analysis (rparam_rank 1), we change default returned value in this case to ignored.
         * Then, when CME ERROR code will be retrieved, return no error (error code will be inside structure returned).
         */
        retval = ATACTION_RSP_IGNORED;
        START_PARAM_LOOP()
        if (element_infos->param_rank == 2U)
        {
          csint_ComMdm_t *p_mdm_com = &(p_modem_ctxt->SID_ctxt.com_mdm_data);
          /* retrieve CME ERROR code */
          uint32_t CmeErrorCode = ATutil_convertStringToInt(&p_msg_in->buffer[element_infos->str_start_idx],
                                                            element_infos->str_size);
          p_mdm_com->errorCode = (int32_t) CmeErrorCode;
          /*
          PRINT_INFO("CMD_AT_ORP param 2: (size=%d) value=%d", element_infos->str_size, CmeErrorCode)
          PRINT_BUF((const uint8_t *)&p_msg_in->buffer[element_infos->str_start_idx], element_infos->str_size)
          */
          /* if CME ERROR code is present, it is reported in errorCode field and */
          retval = ATACTION_RSP_FRC_END;
        }
        END_PARAM_LOOP()
      }
      break;
#endif /* defined(USE_COM_MDM) */
    default:
      retval = fRspAnalyze_Error(p_at_ctxt, p_modem_ctxt, p_msg_in, element_infos);
      break;
  }

  return (retval);
}

at_action_rsp_t fRspAnalyze_CPIN_WP77(at_context_t *p_at_ctxt, atcustom_modem_context_t *p_modem_ctxt,
                                      const IPC_RxMessage_t *p_msg_in, at_element_info_t *element_infos)
{
  atparser_context_t *p_atp_ctxt = &(p_at_ctxt->parser);
  at_action_rsp_t retval = ATACTION_RSP_IGNORED;
  PRINT_API("enter fRspAnalyze_CPIN_WP77()")


  /*  Sierra Wireless WP77 AT Commands Manual V1.0
  *   analyze parameters for +CPIN
  *
  *   if +CPIN is not received after AT+CPIN request, it's an URC
  */
  if (p_atp_ctxt->current_atcmd.id == (CMD_ID_t) CMD_AT_CPIN)
  {
    retval = fRspAnalyze_CPIN(p_at_ctxt, p_modem_ctxt, p_msg_in, element_infos);
  }
  else
  {
    /* this is an URC */
    START_PARAM_LOOP()
    if (element_infos->param_rank == 2U)
    {
      PRINT_DBG("URC +CPIN received")
      PRINT_BUF((const uint8_t *)&p_msg_in->buffer[element_infos->str_start_idx], element_infos->str_size)
    }
    END_PARAM_LOOP()
  }

  return (retval);
}

at_action_rsp_t fRspAnalyze_CFUN_WP77(at_context_t *p_at_ctxt, atcustom_modem_context_t *p_modem_ctxt,
                                      const IPC_RxMessage_t *p_msg_in, at_element_info_t *element_infos)
{
  UNUSED(p_modem_ctxt);
  at_action_rsp_t retval = ATACTION_RSP_IGNORED;
  PRINT_API("enter fRspAnalyze_CFUN_WP77()")

  /*  Sierra Wireless WP77 AT Commands Manual V1.0
  *   analyze parameters for +CFUN
  *
  *   if +CFUN is received, it's an URC
  */

  /* this is an URC */
  START_PARAM_LOOP()
  if (element_infos->param_rank == 2U)
  {
    PRINT_DBG("URC +CFUN received")
    PRINT_BUF((const uint8_t *)&p_msg_in->buffer[element_infos->str_start_idx], element_infos->str_size)

  }
  END_PARAM_LOOP()

  return (retval);
}


at_action_rsp_t fRspAnalyze_SELRAT_WP77(at_context_t *p_at_ctxt, atcustom_modem_context_t *p_modem_ctxt,
                                       const IPC_RxMessage_t *p_msg_in, at_element_info_t *element_infos)
{
  atparser_context_t *p_atp_ctxt = &(p_at_ctxt->parser);
  at_action_rsp_t retval =  ATACTION_RSP_IGNORED;
  PRINT_DBG("enter fRspAnalyze_SELRAT_WP77()")
  if (p_atp_ctxt->current_atcmd.type == ATTYPE_READ_CMD)
  {
    /* analyze parameters for !SELRAT Read Request*/
	START_PARAM_LOOP()
	if (element_infos->param_rank == 2U)
	{
	  uint8_t rat = ATutil_convertStringToInt(&p_msg_in->buffer[element_infos->str_start_idx],
							element_infos->str_size);
	  WP77_shared.mode_and_bands_config.nw_scanmode = rat;
	  if(rat == 0)
	  {
	    PRINT_INFO("Modem Access Technology is set to Automatic (All RATs")
	  }
	  else if(rat == 1)
	  {
	    PRINT_INFO("Modem Access Technology is UMTS 3G only")
	  }
	  else if(rat == 2)
	  {
	    PRINT_INFO("Modem Access Technology is 2G Only")
	  }
	  else if(rat == 54)
	  {
		PRINT_INFO("Modem Access Technology is UMTS and GSM Only")
	  }
	  else if(rat == 6)
	  {
		PRINT_INFO("Modem Access Technology is LTE Only")
	  }
	  else if(rat == 7)
	  {
		PRINT_INFO("Modem Access Technology is LTE,UMTS,2G Only")
      }
	  else if(rat == 11)
	  {
		PRINT_INFO("Modem Access Technology is LTE and UMTS Only")
	  }
	  else if(rat == 12)
	  {
		PRINT_INFO("Modem Access Technology is LTE and GSM Only")
	  }
	}
	END_PARAM_LOOP()
  }
  return (retval);
}

at_action_rsp_t fRspAnalyze_BAND_WP77(at_context_t *p_at_ctxt, atcustom_modem_context_t *p_modem_ctxt,
        const IPC_RxMessage_t *p_msg_in, at_element_info_t *element_infos)
{
at_action_rsp_t retval = ATACTION_RSP_IGNORED;
PRINT_DBG("enter fRspAnalyze_BAND_WP77()")
PRINT_BUF((const uint8_t *)&p_msg_in->buffer[element_infos->str_start_idx], element_infos->str_size)
(void) memcpy((void *)(WP77_shared.mode_and_bands_config.band),(const void *) & (p_msg_in->buffer[element_infos->str_start_idx]),
        (size_t) element_infos->str_size);
return (retval);
}

at_action_rsp_t fRspAnalyze_SELACQ_WP77(at_context_t *p_at_ctxt, atcustom_modem_context_t *p_modem_ctxt,
        const IPC_RxMessage_t *p_msg_in, at_element_info_t *element_infos)
{
at_action_rsp_t retval = ATACTION_RSP_IGNORED;
PRINT_DBG("enter fRspAnalyze_SELACQ_WP77()")
PRINT_BUF((const uint8_t *)&p_msg_in->buffer[element_infos->str_start_idx], element_infos->str_size)
return (retval);
}

at_action_rsp_t fRspAnalyze_GSTATUS_WP77(at_context_t *p_at_ctxt, atcustom_modem_context_t *p_modem_ctxt,
                                       const IPC_RxMessage_t *p_msg_in, at_element_info_t *element_infos)
{
	at_action_rsp_t retval = ATACTION_RSP_IGNORED;
	PRINT_API("enter fRspAnalyze_GSTATUS_WP77()")
	PRINT_BUF((const uint8_t *)&p_msg_in->buffer[element_infos->str_start_idx], element_infos->str_size)
	return (retval);
}


at_action_rsp_t fRspAnalyze_CCID_WP77(at_context_t *p_at_ctxt, atcustom_modem_context_t *p_modem_ctxt,
                                       const IPC_RxMessage_t *p_msg_in, at_element_info_t *element_infos)
{
  /*UNUSED(p_at_ctxt);*/
  at_action_rsp_t retval = ATACTION_RSP_INTERMEDIATE; /* received a valid intermediate answer */
  PRINT_API("enter fRspAnalyze_QCCID_WP77()")

  /* analyze parameters for +QCCID */
  START_PARAM_LOOP()
  if (element_infos->param_rank == 2U)
  {
    PRINT_DBG("ICCID:")
    PRINT_BUF((const uint8_t *)&p_msg_in->buffer[element_infos->str_start_idx], element_infos->str_size)

    /* WP77 specific treatment:
     *  ICCID reported by the modem includes a blank character (space, code=0x20) at the beginning
     *  remove it if this is the case
     */
    uint16_t src_idx = element_infos->str_start_idx;
    size_t ccid_size = element_infos->str_size;
    if ((p_msg_in->buffer[src_idx] == 0x20U) &&
        (ccid_size >= 2U))
    {
      ccid_size -= 1U;
      src_idx += 1U;
    }

    /* copy ICCID */
    (void) memcpy((void *) & (p_modem_ctxt->SID_ctxt.device_info->u.iccid),
                  (const void *)&p_msg_in->buffer[src_idx],
                  (size_t)ccid_size);
  }
  else
  {
    /* other parameters ignored */
    __NOP(); /* to avoid warning */
  }
  END_PARAM_LOOP()

  return (retval);
}



at_action_rsp_t fRspAnalyze_KCELL_WP77(at_context_t *p_at_ctxt, atcustom_modem_context_t *p_modem_ctxt,
                                      const IPC_RxMessage_t *p_msg_in, at_element_info_t *element_infos)
{
	at_action_rsp_t retval = ATACTION_RSP_IGNORED;
    PRINT_API("enter fRspAnalyze_KCELL_HL78()")
	PRINT_BUF((const uint8_t *)&p_msg_in->buffer[element_infos->str_start_idx], element_infos->str_size)
	return (retval);
}

at_action_rsp_t fRspAnalyze_WDSI_WP77(at_context_t *p_at_ctxt, atcustom_modem_context_t *p_modem_ctxt,
                                      const IPC_RxMessage_t *p_msg_in, at_element_info_t *element_infos)
{
  UNUSED(p_at_ctxt);
  UNUSED(p_modem_ctxt);
  at_action_rsp_t retval = ATACTION_RSP_IGNORED;
  PRINT_API("enter fRspAnalyze_WDSI_WP77()")
  uint32_t wdsi_urc = 0;
  uint8_t avms_connection_status;
  /* analyze parameters for +WDSI
   * this is an URC
   */
  START_PARAM_LOOP()
  if (element_infos->param_rank == 2U)
  {
	  wdsi_urc= ATutil_convertStringToInt(&p_msg_in->buffer[element_infos->str_start_idx],element_infos->str_size);
  }
  else if (element_infos->param_rank == 3U)
  {
	  if(wdsi_urc == 23)
	  {
		  avms_connection_status = ATutil_convertStringToInt(&p_msg_in->buffer[element_infos->str_start_idx],element_infos->str_size);
		  WP77_shared.avms_connection_status = avms_connection_status;
		  PRINT_INFO(" Air Vantage connection Status changed to %d",avms_connection_status)
	  }
  }
  else
  {
    /* parameter ignored */
    __NOP(); /* to avoid warning */
  }

  END_PARAM_LOOP()

  return (retval);

}

at_action_rsp_t fRspAnalyze_CGMR_WP77(at_context_t *p_at_ctxt, atcustom_modem_context_t *p_modem_ctxt,
                                      const IPC_RxMessage_t *p_msg_in, at_element_info_t *element_infos)
{
  atparser_context_t *p_atp_ctxt = &(p_at_ctxt->parser);
  at_action_rsp_t retval = ATACTION_RSP_IGNORED;
  PRINT_API("enter fRspAnalyze_CGMR_WP77()")

  /* analyze parameters for +CGMR */
  /* only for execution command, set parameters */
  if (p_atp_ctxt->current_atcmd.type == ATTYPE_EXECUTION_CMD)
  {
    PRINT_DBG("Revision:")
    PRINT_BUF((const uint8_t *)&p_msg_in->buffer[element_infos->str_start_idx], element_infos->str_size)

    (void) memcpy((void *) & (p_modem_ctxt->SID_ctxt.device_info->u.revision),
                  (const void *)&p_msg_in->buffer[element_infos->str_start_idx],
                  (size_t)element_infos->str_size);
  }

  return (retval);
}



at_action_rsp_t fRspAnalyze_COPS_WP77(at_context_t *p_at_ctxt, atcustom_modem_context_t *p_modem_ctxt,
                                      const IPC_RxMessage_t *p_msg_in, at_element_info_t *element_infos)
{
  at_action_rsp_t retval;
  PRINT_API("enter fRspAnalyze_COPS_WP77()")

  /* WP77 does not follow 3GPP TS 27.007 values for <AcT> parameter in AT+COPS
  * Encapsulate generic COPS build function to manage this difference
  */
  retval = fRspAnalyze_COPS(p_at_ctxt, p_modem_ctxt, p_msg_in, element_infos);

  if (retval != ATACTION_RSP_ERROR)
  {
    if ((p_modem_ctxt->SID_ctxt.read_operator_infos.optional_fields_presence & CS_RSF_ACT_PRESENT) != 0U)
    {
      if (p_modem_ctxt->SID_ctxt.read_operator_infos.AcT == CS_ACT_EC_GSM_IOT)
      {
        /* WP77 AcT = 8 means cat.M1
        *  3GPP AcT = 8 means CS_ACT_EC_GSM_IOT, cat.M1 value if 9
        *  convert 8 to 9 for upper layers
        */
        p_modem_ctxt->SID_ctxt.read_operator_infos.AcT = CS_ACT_E_UTRAN;
      }
    }
  }

  return (retval);
}
#if defined(USE_COM_MDM)

#define ORP_MSG_MAX_SIZE ((uint32_t) 250U)
typedef struct
{
  uint8_t      free;
  uint8_t      data[ORP_MSG_MAX_SIZE];
  uint32_t     size;
} orp_msg_t;

static orp_msg_t orp_msg_urc = { .free = 1U };

static int8_t orp_storage_add_msg(const uint8_t *pbuf, uint32_t size);
static bool orpMsgIsAnURC(const uint8_t *pbuf, uint32_t size);

static int8_t orp_storage_add_msg(const uint8_t *pbuf, uint32_t size)
{
  int8_t status;

  /* TODO:
   * actual implementation : - support storage of only 1 ORP message
   *                         - a new msg is stored only if previous msg consumed by client, otherwise new msg
   *                           is ignored
   *
   * evolution: implement a FIFO if required, or keep only last msg, ... => customer choice.
   */
  if (orp_msg_urc.free == 1U)
  {
    if (size <= ORP_MSG_MAX_SIZE)
    {
      PRINT_INFO("ORP URC message: stored")
      (void) memcpy(orp_msg_urc.data, pbuf, size);
      orp_msg_urc.size = size;
      orp_msg_urc.free = 0U;
      status = 0;
    }
    else
    {
      /* msg size exceed maximum size */
      PRINT_INFO("ORP URC message: ERROR, exceed maximum size")
      status = -1;
    }
  }
  else
  {
    /* no more free space */
    PRINT_INFO("ORP URC message: ERROR, no free slot")
    status = -1;
  }

  return(status);
}

int8_t orp_storage_get_msg(const uint8_t *pbuf, uint32_t max_size, uint32_t *size)
{
  int8_t status;

  if (orp_msg_urc.free == 0U)
  {
    /* an ORP message is available */
    if (orp_msg_urc.size <= max_size)
    {
     PRINT_INFO("ORP URC message: copy to client buffer")
     (void) memcpy((void *)pbuf, (void *)orp_msg_urc.data, orp_msg_urc.size);
     *size = orp_msg_urc.size;
     orp_msg_urc.free = 1U;
     status = 0;
    }
    else
    {
      /* buffer provided by client is too small  */
      PRINT_INFO("ORP URC message: ERROR, client buffer size is too small")
      orp_msg_urc.free = 1U; /* free message to avoid blocking cases */
      status = -1;
    }
  }
  else
  {
    /* no message available */
    PRINT_INFO("ORP URC message: ERROR, no msg available")
    status = -1;
  }

  return(status);
}

at_status_t fCmdBuild_ORP(atparser_context_t *p_atp_ctxt, atcustom_modem_context_t *p_modem_ctxt)
{
  UNUSED(p_modem_ctxt);
  at_status_t retval = ATSTATUS_OK;
  PRINT_INFO("enter fCmdBuild_ORP()")

  /* only for write command, set parameters */
  if (p_atp_ctxt->current_atcmd.type == ATTYPE_WRITE_CMD)
  {
    csint_ComMdm_t *p_mdm_com = &(p_modem_ctxt->SID_ctxt.com_mdm_data);

    PRINT_INFO("COM DATA transaction_type = 0x%x",
               p_mdm_com->transaction_type);

    /* debug infos
    PRINT_INFO("COM DATA txBuffer: ptr=%p size=%d",
               p_mdm_com->txBuffer.p_buffer,
               p_mdm_com->txBuffer.buffer_size);

    PRINT_INFO("COM DATA rxBuffer: ptr=%p size=%d maxSize=%d",
               p_mdm_com->rxBuffer.p_buffer,
               p_mdm_com->rxBuffer.buffer_size,
               p_mdm_com->rxBuffer.max_buffer_size);

    PRINT_INFO("COM DATA error_code = %d",
               p_mdm_com->errorCode);
    */

    /* add opening quote */
    (void) sprintf((CRC_CHAR_t *)p_atp_ctxt->current_atcmd.params, "\"");

    /* now copy the buffer to send */
    uint16_t cmd_params_size = (uint16_t) strlen((CRC_CHAR_t *)&p_atp_ctxt->current_atcmd.params);
    (void) memcpy((void *) &p_atp_ctxt->current_atcmd.params[cmd_params_size ],
                  (const CS_CHAR_t *)p_mdm_com->txBuffer.p_buffer,
                  (size_t) p_mdm_com->txBuffer.buffer_size);
    cmd_params_size += p_mdm_com->txBuffer.buffer_size;

    /* add closing quote */
    (void) memcpy((void *) &p_atp_ctxt->current_atcmd.params[cmd_params_size],
                  (const CS_CHAR_t *)"\"",
                  (size_t) 1);

    retval = ATSTATUS_OK;
  }
  return (retval);
}

at_action_rsp_t fRspAnalyze_ORP(at_context_t *p_at_ctxt, atcustom_modem_context_t *p_modem_ctxt,
                                      const IPC_RxMessage_t *p_msg_in, at_element_info_t *element_infos)
{
  atparser_context_t *p_atp_ctxt = &(p_at_ctxt->parser);
  at_action_rsp_t retval = ATACTION_RSP_IGNORED;
  PRINT_INFO("enter fRspAnalyze_ORP()")
  if (p_atp_ctxt->current_atcmd.id == (CMD_ID_t) CMD_AT_ORP)
  {
    START_PARAM_LOOP()
    if (element_infos->param_rank == 2U)
    {
      /* check crossing-case : ORP URC received during transaction (URC msg starts with c@) */
      if (orpMsgIsAnURC(&p_msg_in->buffer[element_infos->str_start_idx],  element_infos->str_size))
      {
        PRINT_INFO("ORP urc received, crossing case")
		csint_ComMdm_t *p_mdm_com = &(p_modem_ctxt->SID_ctxt.com_mdm_data);
		char *strFound;
        char rcvBuf[p_mdm_com->rxBuffer.max_buffer_size];

        strncpy(rcvBuf,(const char*)&p_msg_in->buffer[element_infos->str_start_idx],element_infos->str_size);
        strFound = strchr(rcvBuf,',');
        if(strFound)
        {
        	int loc = strFound - rcvBuf + 1;
        	element_infos->str_start_idx = element_infos->str_start_idx + loc;
        	element_infos->str_size = element_infos->str_size - loc;
        }
        /* try to store received ORP URC */
        if (orp_storage_add_msg(&p_msg_in->buffer[element_infos->str_start_idx],  element_infos->str_size) == 0)
        {
          /* increase number of ORP messages stored */
          p_modem_ctxt->persist.urc_avail_commdm_event_count++;
          retval = ATACTION_RSP_URC_FORWARDED;
        }
      }
      else
      {
        PRINT_INFO("ORP message received")
        csint_ComMdm_t *p_mdm_com = &(p_modem_ctxt->SID_ctxt.com_mdm_data);
        uint32_t bufSize;
        /* a response from modem is expected only in CS_COMMDM_TRANSACTION type */
        if (p_mdm_com->transaction_type == CS_COMMDM_TRANSACTION)
        {
          char * strFound;
          char rcvBuf[p_mdm_com->rxBuffer.max_buffer_size];
          strncpy(rcvBuf,(const char*)&p_msg_in->buffer[element_infos->str_start_idx],element_infos->str_size);
          PRINT_BUF((const uint8_t *)&p_msg_in->buffer[element_infos->str_start_idx], element_infos->str_size)
          strFound = strchr(rcvBuf,'D');
          if(strFound)
          {
        	int loc = strFound - rcvBuf + 1;
        	element_infos->str_start_idx = element_infos->str_start_idx + loc;
        	bufSize = element_infos->str_size - loc;
          }
          else
          {
            bufSize = element_infos->str_size;
          }
          /* check that received buffer size doesn't exceed rx buffer provided by user */
          if (bufSize <= p_mdm_com->rxBuffer.max_buffer_size)
          {
             /* re copy ORP response to client buffer */
            (void) memcpy((void *) p_mdm_com->rxBuffer.p_buffer,
                        (const void *)&p_msg_in->buffer[element_infos->str_start_idx],
                        (size_t)bufSize);
            p_mdm_com->rxBuffer.buffer_size = bufSize;
            p_mdm_com->errorCode = 0;
          }
          else
          {
            PRINT_INFO("ORP message received, size error !")
            retval = ATACTION_RSP_ERROR;
          }
        }
      }
    }
    END_PARAM_LOOP()
  }
  else
  {
    /* no ongoing ORP command, this is an URC */
    PRINT_INFO("possible ORP urc detected")

    START_PARAM_LOOP()
    if (element_infos->param_rank == 2U)
    {
      /* check if ORP message is an URC : TODO do we really need to test this ? */
      if (orpMsgIsAnURC(&p_msg_in->buffer[element_infos->str_start_idx],  element_infos->str_size))
      {
        PRINT_INFO("ORP urc confirmed")
		const char *strFound;
        csint_ComMdm_t *p_mdm_com = &(p_modem_ctxt->SID_ctxt.com_mdm_data);
        char rcvBuf[p_mdm_com->rxBuffer.max_buffer_size];
        strncpy(rcvBuf,(const char*)&p_msg_in->buffer[element_infos->str_start_idx],element_infos->str_size);
        PRINT_BUF((const uint8_t *)&p_msg_in->buffer[element_infos->str_start_idx], element_infos->str_size)
        strFound = strchr(rcvBuf,',');
        if(strFound)
        {
        	int loc = strFound - rcvBuf + 1;
        	element_infos->str_start_idx = element_infos->str_start_idx + loc;
        	element_infos->str_size = element_infos->str_size - loc;
        }

        /* try to store received ORP URC */
        if (orp_storage_add_msg(&p_msg_in->buffer[element_infos->str_start_idx],  element_infos->str_size) == 0)
        {
          /* increase number of ORP messages stored */
          p_modem_ctxt->persist.urc_avail_commdm_event_count++;
          retval = ATACTION_RSP_URC_FORWARDED;
        }
      }
      else
      {
        PRINT_INFO("this ORP is not an URC")
      }
    }
    END_PARAM_LOOP()

  }

  return (retval);
}

static bool orpMsgIsAnURC(const uint8_t *pbuf, uint32_t size)
{
  bool isAnURC = false;

  /* parse msg content until first condition is met: first occurence of "@" (0x40) is found, end of message
   * if "@" is found, check if previous char is "c" and indicates an URC
   */
  for (uint32_t i=1U; i<size; i++)
  {
    /* search for first '@' (ascii code = 0x40) */
    if(*(pbuf+i) == 0x40U)
    {
      /* if previous char is 'c' or 'C' (ascii code = 0x43 or 0x63), it is an URC */
      if((*(pbuf+i-1) == 0x43U) ||(*(pbuf+i-1) == 0x63U))
      {
        PRINT_INFO("ORP msg analyze: URC confirmed")
        isAnURC = true;
        break;
      }
    }
  }

  return(isAnURC);
}

#endif /* defined(USE_COM_MDM) */
/************************ (C) COPYRIGHT Sierra Wireless *****END OF FILE****/

