/**
  ******************************************************************************
  * @file    at_custom_modem_signalling.c
  * @author  Sierra Wireless Inc. and Affiliates
  * @brief   This file provides all the 'signalling' code to the
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
#define PRINT_INFO(format, args...) TRACE_PRINT(DBG_CHAN_ATCMD, DBL_LVL_P0, "HL78:" format "\n\r", ## args)
#define PRINT_DBG(format, args...)  TRACE_PRINT(DBG_CHAN_ATCMD, DBL_LVL_P1, "HL78:" format "\n\r", ## args)
#define PRINT_API(format, args...)  TRACE_PRINT(DBG_CHAN_ATCMD, DBL_LVL_P2, "HL78 API:" format "\n\r", ## args)
#define PRINT_ERR(format, args...)  TRACE_PRINT(DBG_CHAN_ATCMD, DBL_LVL_ERR, "HL78 ERROR:" format "\n\r", ## args)
#define PRINT_BUF(pbuf, size)       TRACE_PRINT_BUF_CHAR(DBG_CHAN_ATCMD, DBL_LVL_P1, (const CRC_CHAR_t *)pbuf, size);
#else
#define PRINT_INFO(format, args...)  (void) printf("HL78:" format "\n\r", ## args);
#define PRINT_DBG(...)  __NOP(); /* Nothing to do */
#define PRINT_API(...)  __NOP(); /* Nothing to do */
#define PRINT_ERR(format, args...)   (void) printf("HL78 ERROR:" format "\n\r", ## args);
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
  - HL78_BAND_GSM    : hexadecimal value that specifies the GSM frequency band (cf AT+QCFG="band")
  - HL78_BAND_CAT_M1 : hexadecimal value that specifies the LTE Cat.M1 frequency band (cf AT+QCFG="band")
                       64bits bitmap split in two 32bits bitmaps  (MSB and LSB parts)
  - HL78_BAND_CAT_NB1: hexadecimal value that specifies the LTE Cat.NB1 frequency band (cf AT+QCFG="band")
                       64bits bitmap split in two 32bits bitmaps  (MSB and LSB parts)
  - HL78_IOTOPMODE   : network category to be searched under LTE network (cf AT+QCFG="iotopmode")
  - HL78_SCANSEQ     : network search sequence (GSM, Cat.M1, Cat.NB1) (cf AT+QCFG="nwscanseq")
  - HL78_SCANMODE    : network to be searched (cf AT+QCFG="nwscanmode")

Below are define default band values that will be used if calling write form of AT+QCFG
(mainly for test purposes)
*/
#define HL78_BAND_GSM          ((ATCustom_HL78_QCFGbandGSM_t)    QCFGBANDGSM_ANY)
#define HL78_BAND_CAT_M1_MSB   ((ATCustom_HL78_QCFGbandCatM1_t)  QCFGBANDCATM1_ANY_MSB)
#define HL78_BAND_CAT_M1_LSB   ((ATCustom_HL78_QCFGbandCatM1_t)  QCFGBANDCATM1_ANY_LSB)
#define HL78_BAND_CAT_NB1_MSB  ((ATCustom_HL78_QCFGbandCatNB1_t) QCFGBANDCATNB1_ANY_MSB)
#define HL78_BAND_CAT_NB1_LSB  ((ATCustom_HL78_QCFGbandCatNB1_t) QCFGBANDCATNB1_ANY_LSB)
#define HL78_IOTOPMODE         ((ATCustom_HL78_QCFGiotopmode_t)  QCFGIOTOPMODE_CATM1CATNB1)
#define HL78_SCANSEQ           ((ATCustom_HL78_QCFGscanseq_t)    QCFGSCANSEQ_M1_NB1_GSM)
#define HL78_SCANMODE          ((ATCustom_HL78_QCFGscanmode_t)   QCFGSCANMODE_AUTO)

#define HL78_PDP_DUPLICATECHK_ENABLE ((uint8_t)0U) /* parameter of AT+QCFG="PDP/DuplicateChk":
                                                    * 0 to refuse, 1 to allow
                                                    */

#define HL78_PSM_URC_ENABLE ((uint8_t)1U) /* parameter of AT+QCFG="psm/urc":
                                           * 0 to disable QPSMTIMER URC report
                                           * 1 to enable QPSMTIMER URC report
                                           */

/* Global variables ----------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

/* Functions Definition ------------------------------------------------------*/
/* Build command functions ------------------------------------------------------- */
at_status_t fCmdBuild_ATD_HL78(atparser_context_t *p_atp_ctxt, atcustom_modem_context_t *p_modem_ctxt)
{
  at_status_t retval = ATSTATUS_OK;
  PRINT_API("enter fCmdBuild_ATD_HL78()")

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

at_status_t fCmdBuild_CGSN_HL78(atparser_context_t *p_atp_ctxt, atcustom_modem_context_t *p_modem_ctxt)
{
  UNUSED(p_modem_ctxt);
  at_status_t retval = ATSTATUS_OK;
  PRINT_API("enter fCmdBuild_CGSN_HL78()")

  /* only for write command, set parameters */
  if (p_atp_ctxt->current_atcmd.type == ATTYPE_WRITE_CMD)
  {
    /* HL78 only supports EXECUTION form of CGSN */
    retval = ATSTATUS_ERROR;
  }
  return (retval);
}

at_status_t fCmdBuild_QPOWD_HL78(atparser_context_t *p_atp_ctxt, atcustom_modem_context_t *p_modem_ctxt)
{
  UNUSED(p_modem_ctxt);
  at_status_t retval = ATSTATUS_OK;
  PRINT_API("enter fCmdBuild_QPOWD_HL78()")

  /* only for write command, set parameters */
  if (p_atp_ctxt->current_atcmd.type == ATTYPE_WRITE_CMD)
  {
    /* Normal Power Down */
    (void) sprintf((CRC_CHAR_t *)p_atp_ctxt->current_atcmd.params, "1");
  }

  return (retval);
}

at_status_t fCmdBuild_QCFG_HL78(atparser_context_t *p_atp_ctxt, atcustom_modem_context_t *p_modem_ctxt)
{
  UNUSED(p_modem_ctxt);
  /* Commands Look-up table for AT+QCFG
   * important: it has to be aligned with ATCustom_HL78_QCFG_function_t
   */
  static const AT_CHAR_t HL78_QCFG_LUT[][32] =
  {
    {"unknown"}, /* QCFG_unknown */
    {"gprsattach"}, /* QCFG_gprsattach */
    {"nwscanseq"}, /* QCFG_nwscanseq */
    {"nwscanmode"}, /* QCFG_nwscanmode */
    {"iotopmode"}, /* QCFG_iotopmode */
    {"roamservice"}, /* QCFG_roamservice */
    {"band"}, /* QCFG_band */
    {"servicedomain"}, /* QCFG_servicedomain */
    {"sgsn"}, /* QCFG_sgsn */
    {"msc"}, /* QCFG_msc */
    {"pdp/duplicatechk"}, /* QCFG_PDP_DuplicateChk */
    {"urc/ri/ring"}, /* QCFG_urc_ri_ring */
    {"urc/ri/smsincoming"}, /* QCFG_urc_ri_smsincoming */
    {"urc/ri/other"}, /* QCFG_urc_ri_other */
    {"signaltype"}, /* QCFG_signaltype */
    {"urc/delay"}, /* QCFG_urc_delay */
    {"psm/urc"}, /* QCFG_urc_psm */
  };

  at_status_t retval = ATSTATUS_OK;
  PRINT_API("enter fCmdBuild_QCFG_HL78()")

  /* only for write command, set parameters */
  if (p_atp_ctxt->current_atcmd.type == ATTYPE_WRITE_CMD)
  {
    AT_CHAR_t cmd_param1[16] = "\0";
    AT_CHAR_t cmd_param2[16] = "\0";
    AT_CHAR_t cmd_param3[16] = "\0";
    AT_CHAR_t cmd_param4[16] = "\0";
    AT_CHAR_t cmd_param5[16] = "\0";
    uint8_t  cmd_nb_params = 0U;

    if (HL78_shared.QCFG_command_write == AT_TRUE)
    {
      /* HL78_AT_Commands_Manual_V2.0 */
      switch (HL78_shared.QCFG_command_param)
      {
        case QCFG_gprsattach:
          /* cmd_nb_params = 1U; */
          /* NOT IMPLEMENTED */
          break;
        case QCFG_nwscanseq:
          cmd_nb_params = 2U;
          /* param 1 = scanseq */
          (void) sprintf((CRC_CHAR_t *)&cmd_param1, "0%lx",
                         HL78_SCANSEQ);  /* print as hexa but without prefix, need to add 1st digit = 0*/
          /* param 2 = effect */
          (void) sprintf((CRC_CHAR_t *)&cmd_param2, "%d", 1);  /* 1 means take effect immediately */
          break;
        case QCFG_nwscanmode:
          cmd_nb_params = 2U;
          /* param 1 = scanmode */
          (void) sprintf((CRC_CHAR_t *)&cmd_param1, "%ld", HL78_SCANMODE);
          /* param 2 = effect */
          (void) sprintf((CRC_CHAR_t *)&cmd_param2, "%d", 1);  /* 1 means take effect immediately */
          break;
        case QCFG_iotopmode:
          cmd_nb_params = 2U;
          /* param 1 = iotopmode */
          (void) sprintf((CRC_CHAR_t *)&cmd_param1, "%ld", HL78_IOTOPMODE);
          /* param 2 = effect */
          (void) sprintf((CRC_CHAR_t *)&cmd_param2, "%d", 1);  /* 1 means take effect immediately */
          break;
        case QCFG_roamservice:
          /* cmd_nb_params = 2U; */
          /* NOT IMPLEMENTED */
          break;
        case QCFG_band:
          cmd_nb_params = 4U;
          /* param 1 = gsmbandval */
          (void) sprintf((CRC_CHAR_t *)&cmd_param1, "%lx", HL78_BAND_GSM);
          /* param 2 = catm1bandval */
          (void) sprintf((CRC_CHAR_t *)&cmd_param2, "%lx%lx", HL78_BAND_CAT_M1_MSB, HL78_BAND_CAT_M1_LSB);
          /* param 3 = catnb1bandval */
          (void) sprintf((CRC_CHAR_t *)&cmd_param3, "%lx%lx", HL78_BAND_CAT_NB1_MSB, HL78_BAND_CAT_NB1_LSB);
          /* param 4 = effect */
          (void) sprintf((CRC_CHAR_t *)&cmd_param4, "%d", 1);  /* 1 means take effect immediately */
          break;
        case QCFG_servicedomain:
          /* cmd_nb_params = 2U; */
          /* NOT IMPLEMENTED */
          break;
        case QCFG_sgsn:
          /* cmd_nb_params = 1U; */
          /* NOT IMPLEMENTED */
          break;
        case QCFG_msc:
          /* cmd_nb_params = 1U; */
          /* NOT IMPLEMENTED */
          break;
        case QCFG_PDP_DuplicateChk:
          cmd_nb_params = 1U;
          /* param 1 = enable */
          (void) sprintf((CRC_CHAR_t *)&cmd_param1, "%d", HL78_PDP_DUPLICATECHK_ENABLE);
          break;
        case QCFG_urc_ri_ring:
          cmd_nb_params = 5U;
          /* NOT IMPLEMENTED */
          break;
        case QCFG_urc_ri_smsincoming:
          /* cmd_nb_params = 2U; */
          /* NOT IMPLEMENTED */
          break;
        case QCFG_urc_ri_other:
          /* cmd_nb_params = 2U; */
          /* NOT IMPLEMENTED */
          break;
        case QCFG_signaltype:
          /* cmd_nb_params = 1U; */
          /* NOT IMPLEMENTED */
          break;
        case QCFG_urc_delay:
          /* cmd_nb_params = 1U; */
          /* NOT IMPLEMENTED */
          break;
        case QCFG_urc_psm:
          cmd_nb_params = 1U;
          /* param 1 = iotopmode */
          (void) sprintf((CRC_CHAR_t *)&cmd_param1, "%d", HL78_PSM_URC_ENABLE);
          break;
        default:
          break;
      }
    }

    if (cmd_nb_params == 5U)
    {
      /* command has 5 parameters (this is a WRITE command) */
      (void) sprintf((CRC_CHAR_t *)p_atp_ctxt->current_atcmd.params, "\"%s\",%s,%s,%s,%s,%s",
                     HL78_QCFG_LUT[HL78_shared.QCFG_command_param],
                     cmd_param1, cmd_param2, cmd_param3, cmd_param4, cmd_param5);
    }
    else if (cmd_nb_params == 4U)
    {
      /* command has 4 parameters (this is a WRITE command) */
      (void) sprintf((CRC_CHAR_t *)p_atp_ctxt->current_atcmd.params, "\"%s\",%s,%s,%s,%s",
                     HL78_QCFG_LUT[HL78_shared.QCFG_command_param], cmd_param1, cmd_param2, cmd_param3, cmd_param4);
    }
    /* this case never happen until now
    * else if (cmd_nb_params == 3U)
    * {
    *   command has 3 parameters (this is a WRITE command)
    *   (void) sprintf((CRC_CHAR_t *)p_atp_ctxt->current_atcmd.params, "\"%s\",%s,%s,%s",
    *                  HL78_QCFG_LUT[HL78_shared.QCFG_command_param], cmd_param1, cmd_param2, cmd_param3);
    * }
    */
    else if (cmd_nb_params == 2U)
    {
      /* command has 2 parameters (this is a WRITE command) */
      (void) sprintf((CRC_CHAR_t *)p_atp_ctxt->current_atcmd.params, "\"%s\",%s,%s",
                     HL78_QCFG_LUT[HL78_shared.QCFG_command_param], cmd_param1, cmd_param2);
    }
    else if (cmd_nb_params == 1U)
    {
      /* command has 1 parameters (this is a WRITE command) */
      (void) sprintf((CRC_CHAR_t *)p_atp_ctxt->current_atcmd.params, "\"%s\",%s",
                     HL78_QCFG_LUT[HL78_shared.QCFG_command_param], cmd_param1);
    }
    else
    {
      /* command has 0 parameters (this is a READ command) */
      (void) sprintf((CRC_CHAR_t *)p_atp_ctxt->current_atcmd.params, "\"%s\"",
                     HL78_QCFG_LUT[HL78_shared.QCFG_command_param]);
    }
  }

  return (retval);
}

#define APN_EMPTY_STRING ""
at_status_t fCmdBuild_QICSGP_HL78(atparser_context_t *p_atp_ctxt, atcustom_modem_context_t *p_modem_ctxt)
{
  at_status_t retval = ATSTATUS_OK;
  PRINT_API("enter fCmdBuild_QICSGP_HL78()")

  /* only for write command, set parameters */
  if (p_atp_ctxt->current_atcmd.type == ATTYPE_WRITE_CMD)
  {
    /* cf HL78 TCP/IP AT commands manual v1.0
    *  AT+QICSGP: Configure parameters of a TCP/IP context
    *  AT+QICSGP=<cid>[,<context_type>,<APN>[,<username>,<password>)[,<authentication>]]]
    *  - cid: context id (rang 1 to 16)
    *  - context_type: 1 for IPV4, 2 for IPV6
    *  - APN: string for access point name
    *  - username: string
    *  - password: string
    *  - authentication: 0 for NONE, 1 for PAP, 2 for CHAP, 3 for PAP or CHAP
    *
    * example: AT+QICSGP=1,1,"UNINET","","",1
    */

    if (HL78_shared.QICGSP_config_command == AT_TRUE)
    {
      CS_CHAR_t *p_apn;
      /* Write command is a config command */
      CS_PDN_conf_id_t current_conf_id = atcm_get_cid_current_SID(p_modem_ctxt);
      uint8_t modem_cid = atcm_get_affected_modem_cid(&p_modem_ctxt->persist, current_conf_id);
      PRINT_INFO("user cid = %d, modem cid = %d", (uint8_t)current_conf_id, modem_cid)

      uint8_t context_type_value;
      uint8_t authentication_value;

      /* convert context type to numeric value */
      switch (p_modem_ctxt->persist.pdp_ctxt_infos[current_conf_id].pdn_conf.pdp_type)
      {
        case CS_PDPTYPE_IP:
          context_type_value = 1U;
          break;
        case CS_PDPTYPE_IPV6:
        case CS_PDPTYPE_IPV4V6:
          context_type_value = 2U;
          break;

        default :
          context_type_value = 1U;
          break;
      }

      /*  authentication : 0,1,2 or 3 - cf modem AT cmd manuel - Use 0 for None */
      if (p_modem_ctxt->persist.pdp_ctxt_infos[current_conf_id].pdn_conf.username[0] == 0U)
      {
        /* no username => no authentication */
        authentication_value = 0U;
      }
      else
      {
        /* username => PAP or CHAP authentication type */
        authentication_value = 3U;
      }



      /* build command */
      if (p_modem_ctxt->persist.pdp_ctxt_infos[current_conf_id].apn_present == CELLULAR_TRUE)
      {
        /* use the APN explicitly providedby user */
        p_apn = (CS_CHAR_t *) &p_modem_ctxt->persist.pdp_ctxt_infos[current_conf_id].apn;
      }
      else
      {
        /* no APN provided by user: use empty APN string (HL78 specific) to force the network to
        *  select the appropriate APN.
        */
        p_apn = (CS_CHAR_t *) &APN_EMPTY_STRING;
      }

      /* use the APN value given by user */
      (void) sprintf((CRC_CHAR_t *)p_atp_ctxt->current_atcmd.params, "%d,%d,\"%s\",\"%s\",\"%s\",%d",
                     modem_cid,
                     context_type_value,
                     p_apn,
                     p_modem_ctxt->persist.pdp_ctxt_infos[current_conf_id].pdn_conf.username,
                     p_modem_ctxt->persist.pdp_ctxt_infos[current_conf_id].pdn_conf.password,
                     authentication_value
                    );
    }
    else
    {
      /* Write command is a query command */
      CS_PDN_conf_id_t current_conf_id = atcm_get_cid_current_SID(p_modem_ctxt);
      uint8_t modem_cid = atcm_get_affected_modem_cid(&p_modem_ctxt->persist, current_conf_id);
      PRINT_INFO("user cid = %d, modem cid = %d", (uint8_t)current_conf_id, modem_cid)
      (void) sprintf((CRC_CHAR_t *)p_atp_ctxt->current_atcmd.params, "%d", modem_cid);
    }

  }

  return (retval);
}

at_status_t fCmdBuild_CGDCONT_HL78(atparser_context_t *p_atp_ctxt, atcustom_modem_context_t *p_modem_ctxt)
{
  at_status_t retval;
  PRINT_API("enter fCmdBuild_CGDCONT_HL78()")

  /* normal case */
  retval = fCmdBuild_CGDCONT(p_atp_ctxt, p_modem_ctxt);

  return (retval);
}

at_status_t fCmdBuild_QICFG_HL78(atparser_context_t *p_atp_ctxt, atcustom_modem_context_t *p_modem_ctxt)
{
  UNUSED(p_modem_ctxt);

  at_status_t retval = ATSTATUS_OK;
  PRINT_API("enter fCmdBuild_QICFG_HL78()")

  /* only for write command, set parameters */
  if (p_atp_ctxt->current_atcmd.type == ATTYPE_WRITE_CMD)
  {
    retval = ATSTATUS_ERROR;
  }

  return (retval);
}

at_status_t fCmdBuild_QINDCFG_HL78(atparser_context_t *p_atp_ctxt, atcustom_modem_context_t *p_modem_ctxt)
{
  UNUSED(p_modem_ctxt);

  at_status_t retval = ATSTATUS_OK;
  PRINT_API("enter fCmdBuild_QINDCFG_HL78()")

  /* only for write command, set parameters */
  if (p_atp_ctxt->current_atcmd.type == ATTYPE_WRITE_CMD)
  {
    switch (HL78_shared.QINDCFG_command_param)
    {
      case QINDCFG_csq:
        if (p_atp_ctxt->current_SID == (at_msg_t) SID_CS_SUSBCRIBE_NET_EVENT)
        {
          /* subscribe to CSQ URC event, do not save to nvram */
          (void) sprintf((CRC_CHAR_t *)p_atp_ctxt->current_atcmd.params, "\"csq\",1,0");
        }
        else
        {
          /* unsubscribe to CSQ URC event, do not save to nvram */
          (void) sprintf((CRC_CHAR_t *)p_atp_ctxt->current_atcmd.params, "\"csq\",0,0");
        }
        break;

      case QINDCFG_all:
      case QINDCFG_smsfull:
      case QINDCFG_ring:
      case QINDCFG_smsincoming:
      default:
        /* not implemented yet or error */
        break;
    }
  }

  return (retval);
}

at_status_t fCmdBuild_QENG_HL78(atparser_context_t *p_atp_ctxt, atcustom_modem_context_t *p_modem_ctxt)
{
  UNUSED(p_modem_ctxt);

  /* Commands Look-up table for AT+QENG */
  static const AT_CHAR_t HL78_QENG_LUT[][32] =
  {
    {"servingcell"}, /* servingcell */
    {"neighbourcell"}, /* neighbourcell */
    {"psinfo"}, /* psinfo */
  };

  at_status_t retval = ATSTATUS_OK;
  PRINT_API("enter fCmdBuild_QENG_HL78()")

  /* only for write command, set parameters */
  if (p_atp_ctxt->current_atcmd.type == ATTYPE_WRITE_CMD)
  {
    /* cf addendum for description of QENG AT command (for debug only)
     * AT+QENG=<celltype>
     * Engineering mode used to report information of serving cells, neighbouring cells and packet
     * switch parameters.
     */

    /* select param: QENG_CELLTYPE_SERVINGCELL
     *               QENG_CELLTYPE_NEIGHBOURCELL
     *               QENG_CELLTYPE_PSINFO
     */
    uint8_t HL78_QENG_command_param = QENG_CELLTYPE_SERVINGCELL;

    (void) sprintf((CRC_CHAR_t *)p_atp_ctxt->current_atcmd.params, "\"%s\"", HL78_QENG_LUT[HL78_QENG_command_param]);
  }

  return (retval);
}

at_status_t fCmdBuild_QURCCFG_HL78(atparser_context_t *p_atp_ctxt, atcustom_modem_context_t *p_modem_ctxt)
{
  UNUSED(p_modem_ctxt);
  at_status_t retval = ATSTATUS_OK;
  PRINT_API("enter fCmdBuild_QURCCFG_HL78()")

  /* only for write command, set parameters */
  if (p_atp_ctxt->current_atcmd.type == ATTYPE_WRITE_CMD)
  {
    /* set URC output port to UART */
    (void) sprintf((CRC_CHAR_t *)p_atp_ctxt->current_atcmd.params, "\"urcport\",\"uart1\"");
  }

  return (retval);
}

at_status_t fCmdBuild_QPSMEXTCFG(atparser_context_t *p_atp_ctxt, atcustom_modem_context_t *p_modem_ctxt)
{
  UNUSED(p_modem_ctxt);
  at_status_t retval = ATSTATUS_OK;
  PRINT_API("enter fCmdBuild_QPSMEXTCFG()")

  /* only for write command, set parameters */
  if (p_atp_ctxt->current_atcmd.type == ATTYPE_WRITE_CMD)
  {
    /* Normal Power Down */
    (void) sprintf((CRC_CHAR_t *)p_atp_ctxt->current_atcmd.params, "0,,,,,3");
  }

  return (retval);
}

at_status_t fCmdBuild_COPS_HL78(atparser_context_t *p_atp_ctxt, atcustom_modem_context_t *p_modem_ctxt)
{
  UNUSED(p_modem_ctxt);
  at_status_t retval;
  PRINT_API("enter fCmdBuild_COPS_HL78()")

  /* HL78 does not follow 3GPP TS 27.007 values for <AcT> parameter in AT+COPS
   * Encapsulate generic COPS build function to manage this difference
   */

  CS_OperatorSelector_t *operatorSelect = &(p_modem_ctxt->SID_ctxt.write_operator_infos);
  if (operatorSelect->AcT_present == CELLULAR_TRUE)
  {
    if (operatorSelect->AcT == CS_ACT_E_UTRAN)
    {
      /* HL78 AcT = 8 means cat.M1
      *  3GPP AcT = 8 means CS_ACT_EC_GSM_IOT, cat.M1 value is 9
      *  convert 9 to 8 for HL78
      */
      operatorSelect->AcT = CS_ACT_EC_GSM_IOT;
    }
  }

  /* finally call the common COPS function */
  retval = fCmdBuild_COPS(p_atp_ctxt, p_modem_ctxt);

  return (retval);
}

at_status_t fCmdBuild_KCELL_HL78(atparser_context_t *p_atp_ctxt, atcustom_modem_context_t *p_modem_ctxt)
{
	UNUSED(p_modem_ctxt);
	at_status_t retval = ATSTATUS_OK;
	PRINT_API("enter fCmdBuild_KCELL_HL78()")

	/* only for write command, set parameters */
	if (p_atp_ctxt->current_atcmd.type == ATTYPE_WRITE_CMD)
	{
	  /* Normal Power Down */
	  (void) sprintf((CRC_CHAR_t *)p_atp_ctxt->current_atcmd.params, "0");
	}
	return (retval);
}

at_status_t fCmdBuild_AT_AND_K(atparser_context_t *p_atp_ctxt,
		atcustom_modem_context_t *p_modem_ctxt) {
	UNUSED(p_modem_ctxt);

	at_status_t retval = ATSTATUS_OK;
	PRINT_API("enter fCmdBuild_AT_AND_K()")

	/* only for execution command, set parameters */
	if (p_atp_ctxt->current_atcmd.type == ATTYPE_EXECUTION_CMD) {
		if (p_modem_ctxt->CMD_ctxt.flow_control_cts_rts == AT_FALSE) {
			/* No flow control */
			(void) sprintf((CRC_CHAR_t*) p_atp_ctxt->current_atcmd.params,
					"0");
		} else {
			/* CTS/RTS activated */
			(void) sprintf((CRC_CHAR_t*) p_atp_ctxt->current_atcmd.params,
					"3");
				}
	}

	return (retval);
}

/* Analyze command functions ------------------------------------------------------- */

at_action_rsp_t fRspAnalyze_Error_HL78(at_context_t *p_at_ctxt, atcustom_modem_context_t *p_modem_ctxt,
                                       const IPC_RxMessage_t *p_msg_in, at_element_info_t *element_infos)
{
  atparser_context_t *p_atp_ctxt = &(p_at_ctxt->parser);
  at_action_rsp_t retval = ATACTION_RSP_ERROR;
  PRINT_API("enter fRspAnalyze_Error_HL78()")

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

  /* analyze Error for HL78 */
  switch (p_atp_ctxt->current_atcmd.id)
  {
    case CMD_AT_CREG:
    case CMD_AT_CGREG:
    case CMD_AT_CEREG:
      /* error is ignored */
      retval = ATACTION_RSP_FRC_END;
      break;

    case CMD_AT_CPSMS:
    case CMD_AT_QPSMCFG:
    case CMD_AT_QPSMEXTCFG:
    case CMD_AT_CEDRXS:
    case CMD_AT_QNWINFO:
    case CMD_AT_QENG:
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

    case CMD_AT_QINISTAT:
      /* error is ignored: this command is supported from HL78 modem FW
       *  needed to avoid blocking errors with previous modem FW versions
       */
      HL78_shared.QINISTAT_error = AT_TRUE;
      retval = ATACTION_RSP_FRC_CONTINUE;
      break;

    case CMD_AT_CPIN:
      /* error is ignored when HL78_sim_status_retries is not null
       *
       */
      if (HL78_shared.HL78_sim_status_retries != 0U)
      {
        PRINT_INFO("error ignored (waiting for SIM ready)")
        retval = ATACTION_RSP_FRC_CONTINUE;
      }
      else
      {
        retval = fRspAnalyze_Error(p_at_ctxt, p_modem_ctxt, p_msg_in, element_infos);
      }
      break;

    default:
      retval = fRspAnalyze_Error(p_at_ctxt, p_modem_ctxt, p_msg_in, element_infos);
      break;
  }

  return (retval);
}

at_action_rsp_t fRspAnalyze_CPIN_HL78(at_context_t *p_at_ctxt, atcustom_modem_context_t *p_modem_ctxt,
                                      const IPC_RxMessage_t *p_msg_in, at_element_info_t *element_infos)
{
  atparser_context_t *p_atp_ctxt = &(p_at_ctxt->parser);
  at_action_rsp_t retval = ATACTION_RSP_IGNORED;
  PRINT_API("enter fRspAnalyze_CPIN_HL78()")


  /*  Sierra Wireless HL78 AT Commands Manual V1.0
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

at_action_rsp_t fRspAnalyze_CFUN_HL78(at_context_t *p_at_ctxt, atcustom_modem_context_t *p_modem_ctxt,
                                      const IPC_RxMessage_t *p_msg_in, at_element_info_t *element_infos)
{
  UNUSED(p_modem_ctxt);
  at_action_rsp_t retval = ATACTION_RSP_IGNORED;
  PRINT_API("enter fRspAnalyze_CFUN_HL78()")

  /*  Sierra Wireless HL78 AT Commands Manual V1.0
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

at_action_rsp_t fRspAnalyze_QIND_HL78(at_context_t *p_at_ctxt, atcustom_modem_context_t *p_modem_ctxt,
                                      const IPC_RxMessage_t *p_msg_in, at_element_info_t *element_infos)
{
  UNUSED(p_at_ctxt);

  at_action_rsp_t retval = ATACTION_RSP_IGNORED;
  PRINT_API("enter fRspAnalyze_QIND_HL78()")

  at_bool_t HL78_current_qind_is_csq = AT_FALSE;
  /* FOTA infos */
  at_bool_t HL78_current_qind_is_fota = AT_FALSE;
  uint8_t HL78_current_qind_fota_action = 0U; /* 0: ignored FOTA action , 1: FOTA start, 2: FOTA end  */

  /*  Sierra Wireless HL78 AT Commands Manual V1.0
  *   analyze parameters for +QIND
  *
  *   it's an URC
  */

  /* this is an URC */
  START_PARAM_LOOP()
  if (element_infos->param_rank == 2U)
  {
    AT_CHAR_t line[32] = {0};

    /* init param received info */
    HL78_current_qind_is_csq = AT_FALSE;

    PRINT_DBG("URC +QIND received")
    PRINT_BUF((const uint8_t *)&p_msg_in->buffer[element_infos->str_start_idx], element_infos->str_size)

    /* copy element to line for parsing */
    if (element_infos->str_size <= 32U)
    {
      (void) memcpy((void *)&line[0],
                    (const void *) & (p_msg_in->buffer[element_infos->str_start_idx]),
                    (size_t) element_infos->str_size);

      /* extract value and compare it to expected value */
      if ((AT_CHAR_t *) strstr((const CRC_CHAR_t *)&line[0], "csq") != NULL)
      {
        PRINT_DBG("SIGNAL QUALITY INFORMATION")
        HL78_current_qind_is_csq = AT_TRUE;
      }
      else if ((AT_CHAR_t *) strstr((const CRC_CHAR_t *)&line[0], "FOTA") != NULL)
      {
        PRINT_DBG("URC FOTA infos received")
        HL78_current_qind_is_fota = AT_TRUE;
      }
      else
      {
        retval = ATACTION_RSP_URC_IGNORED;
        PRINT_DBG("QIND info not managed: urc ignored")
      }
    }
    else
    {
      PRINT_ERR("param ignored (exceed maximum size)")
      retval = ATACTION_RSP_IGNORED;
    }
  }
  else if (element_infos->param_rank == 3U)
  {
    if (HL78_current_qind_is_csq == AT_TRUE)
    {
      uint32_t rssi = ATutil_convertStringToInt(&p_msg_in->buffer[element_infos->str_start_idx],
                                                element_infos->str_size);
      PRINT_DBG("+CSQ rssi=%ld", rssi)
      p_modem_ctxt->persist.urc_avail_signal_quality = AT_TRUE;
      p_modem_ctxt->persist.signal_quality.rssi = (uint8_t)rssi;
      p_modem_ctxt->persist.signal_quality.ber = 99U; /* in case ber param is not present */
    }
    else if (HL78_current_qind_is_fota == AT_TRUE)
    {
      AT_CHAR_t line[32] = {0};
      /* copy element to line for parsing */
      if (element_infos->str_size <= 32U)
      {
        (void) memcpy((void *)&line[0],
                      (const void *) & (p_msg_in->buffer[element_infos->str_start_idx]),
                      (size_t) element_infos->str_size);
        /* WARNING KEEP ORDER UNCHANGED (END comparison has to be after HTTPEND and FTPEND) */
        if ((AT_CHAR_t *) strstr((const CRC_CHAR_t *)&line[0], "FTPSTART") != NULL)
        {
          HL78_current_qind_fota_action = 0U; /* ignored */
          retval = ATACTION_RSP_URC_IGNORED;
        }
        else if ((AT_CHAR_t *) strstr((const CRC_CHAR_t *)&line[0], "FTPEND") != NULL)
        {
          HL78_current_qind_fota_action = 0U; /* ignored */
          retval = ATACTION_RSP_URC_IGNORED;
        }
        else if ((AT_CHAR_t *) strstr((const CRC_CHAR_t *)&line[0], "HTTPSTART") != NULL)
        {
          PRINT_INFO("URC FOTA start detected !")
          HL78_current_qind_fota_action = 1U;
          if (atcm_modem_event_received(p_modem_ctxt, CS_MDMEVENT_FOTA_START) == AT_TRUE)
          {
            retval = ATACTION_RSP_URC_FORWARDED;
          }
          else
          {
            retval = ATACTION_RSP_URC_IGNORED;
          }
        }
        else if ((AT_CHAR_t *) strstr((const CRC_CHAR_t *)&line[0], "HTTPEND") != NULL)
        {
          HL78_current_qind_fota_action = 0U; /* ignored */
          retval = ATACTION_RSP_URC_IGNORED;
        }
        else if ((AT_CHAR_t *) strstr((const CRC_CHAR_t *)&line[0], "START") != NULL)
        {
          HL78_current_qind_fota_action = 0U; /* ignored */
          retval = ATACTION_RSP_URC_IGNORED;
        }
        else if ((AT_CHAR_t *) strstr((const CRC_CHAR_t *)&line[0], "END") != NULL)
        {
          PRINT_INFO("URC FOTA end detected !")
          HL78_current_qind_fota_action = 2U;
          if (atcm_modem_event_received(p_modem_ctxt, CS_MDMEVENT_FOTA_END) == AT_TRUE)
          {
            retval = ATACTION_RSP_URC_FORWARDED;
          }
          else
          {
            retval = ATACTION_RSP_URC_IGNORED;
          }
        }
        else
        {
          HL78_current_qind_fota_action = 0U; /* ignored */
          retval = ATACTION_RSP_URC_IGNORED;
        }
      }
      else
      {
        PRINT_ERR("param ignored (exceed maximum size)")
        retval = ATACTION_RSP_IGNORED;
      }
    }
    else
    {
      /* ignore */
    }

  }
  else if (element_infos->param_rank == 4U)
  {
    if (HL78_current_qind_is_csq == AT_TRUE)
    {
      uint32_t ber = ATutil_convertStringToInt(&p_msg_in->buffer[element_infos->str_start_idx],
                                               element_infos->str_size);
      PRINT_DBG("+CSQ ber=%ld", ber)
      p_modem_ctxt->persist.signal_quality.ber = (uint8_t)ber;
    }
    else if (HL78_current_qind_is_fota == AT_TRUE)
    {
      if (HL78_current_qind_fota_action == 1U)
      {
        /* FOTA END status
         * parameter ignored for the moment
         */
        retval = ATACTION_RSP_URC_IGNORED;
      }
    }
    else
    {
      /* ignored */
      __NOP(); /* to avoid warning */
    }
  }
  else
  {
    /* other parameters ignored */
    __NOP(); /* to avoid warning */
  }
  END_PARAM_LOOP()

  return (retval);
}

at_action_rsp_t fRspAnalyze_KSRAT_HL78(at_context_t *p_at_ctxt, atcustom_modem_context_t *p_modem_ctxt,
                                       const IPC_RxMessage_t *p_msg_in, at_element_info_t *element_infos)
{
	atparser_context_t *p_atp_ctxt = &(p_at_ctxt->parser);
	at_action_rsp_t retval =  ATACTION_RSP_FRC_END; /* received a valid intermediate answer */
	PRINT_API("enter fRspAnalyze_KSRAT_HL78()")

	if (p_atp_ctxt->current_atcmd.type == ATTYPE_READ_CMD)
	{
		/* analyze parameters for +KSRAT Read Request*/
		START_PARAM_LOOP()
		if (element_infos->param_rank == 2U)
		{
			uint32_t rat = ATutil_convertStringToInt(
										&p_msg_in->buffer[element_infos->str_start_idx],
										element_infos->str_size);
			if(rat == 0)
			{
				PRINT_DBG("Modem Access Technology is set to CATM1")
				PRINT_INFO("Modem Access Technology is set to CATM1")
			}
			else if(rat == 1)
			{
				PRINT_DBG("Modem Access Technology is set to NB1")
		        PRINT_INFO("Modem Access Technology is set to NB1")
			}
			else if(rat == 2)
			{
				PRINT_DBG("Modem Access Technology is set to GSM")
		        PRINT_INFO("Modem Access Technology is set to GSM")
		    }

		}
		END_PARAM_LOOP()
	}
	return (retval);

}

at_action_rsp_t fRspAnalyze_KSELACQ_HL78(at_context_t *p_at_ctxt, atcustom_modem_context_t *p_modem_ctxt,
                                       const IPC_RxMessage_t *p_msg_in, at_element_info_t *element_infos)
{
	atparser_context_t *p_atp_ctxt = &(p_at_ctxt->parser);
	at_action_rsp_t retval =  ATACTION_RSP_INTERMEDIATE; /* received a valid intermediate answer */
	PRINT_API("enter fRspAnalyze_KSELACQ_HL78()")

	if (p_atp_ctxt->current_atcmd.type == ATTYPE_READ_CMD)
	{
		PRINT_BUF((const uint8_t *)&p_msg_in->buffer[element_infos->str_start_idx], element_infos->str_size)
	}
	return (retval);

}

at_action_rsp_t fRspAnalyze_KBNDCFG_HL78(at_context_t *p_at_ctxt, atcustom_modem_context_t *p_modem_ctxt,
                                       const IPC_RxMessage_t *p_msg_in, at_element_info_t *element_infos)
{
	atparser_context_t *p_atp_ctxt = &(p_at_ctxt->parser);
	at_action_rsp_t retval =  ATACTION_RSP_INTERMEDIATE;
	PRINT_API("enter fRspAnalyze_KBNDCFG_HL78()")

	if (p_atp_ctxt->current_atcmd.type == ATTYPE_READ_CMD)
	{
		PRINT_BUF((const uint8_t *)&p_msg_in->buffer[element_infos->str_start_idx], element_infos->str_size)
	}
	return (retval);

}

at_action_rsp_t fRspAnalyze_QCFG_HL78(at_context_t *p_at_ctxt, atcustom_modem_context_t *p_modem_ctxt,
                                      const IPC_RxMessage_t *p_msg_in, at_element_info_t *element_infos)
{
  UNUSED(p_modem_ctxt);

  at_action_rsp_t retval = ATACTION_RSP_IGNORED;
  PRINT_API("enter fRspAnalyze_QCFG_HL78()")

  /* memorize which is current QCFG command received */
  ATCustom_HL78_QCFG_function_t HL78_current_qcfg_cmd = QCFG_unknown;

  START_PARAM_LOOP()
  if (element_infos->param_rank == 2U)
  {
    AT_CHAR_t line[32] = {0};

    /* init param received info */
    HL78_current_qcfg_cmd = QCFG_unknown;

    PRINT_BUF((const uint8_t *)&p_msg_in->buffer[element_infos->str_start_idx], element_infos->str_size)

    /* copy element to line for parsing */
    if (element_infos->str_size <= 32U)
    {
      (void) memcpy((void *)&line[0],
                    (const void *) & (p_msg_in->buffer[element_infos->str_start_idx]),
                    (size_t) element_infos->str_size);

      /* extract value and compare it to expected value */
      if ((AT_CHAR_t *) strstr((const CRC_CHAR_t *)&line[0], "nwscanseq") != NULL)
      {
        PRINT_DBG("+QCFG nwscanseq infos received")
        HL78_current_qcfg_cmd = QCFG_nwscanseq;
      }
      else if ((AT_CHAR_t *) strstr((const CRC_CHAR_t *)&line[0], "nwscanmode") != NULL)
      {
        PRINT_DBG("+QCFG nwscanmode infos received")
        HL78_current_qcfg_cmd = QCFG_nwscanmode;
      }
      else if ((AT_CHAR_t *) strstr((const CRC_CHAR_t *)&line[0], "iotopmode") != NULL)
      {
        PRINT_DBG("+QCFG iotopmode infos received")
        HL78_current_qcfg_cmd = QCFG_iotopmode;
      }
      else if ((AT_CHAR_t *) strstr((const CRC_CHAR_t *)&line[0], "band") != NULL)
      {
        PRINT_DBG("+QCFG band infos received")
        HL78_current_qcfg_cmd = QCFG_band;
      }
      else
      {
        PRINT_ERR("+QCFDG field not managed")
      }
    }
    else
    {
      PRINT_ERR("param ignored (exceed maximum size)")
      retval = ATACTION_RSP_IGNORED;
    }
  }
  else if (element_infos->param_rank == 3U)
  {
    switch (HL78_current_qcfg_cmd)
    {
      case QCFG_nwscanseq:
        HL78_shared.mode_and_bands_config.nw_scanseq =
          (ATCustom_HL78_QCFGscanseq_t) ATutil_convertHexaStringToInt32(
            &p_msg_in->buffer[element_infos->str_start_idx],
            element_infos->str_size);
        break;
      case QCFG_nwscanmode:
        HL78_shared.mode_and_bands_config.nw_scanmode =
          (ATCustom_HL78_QCFGscanmode_t) ATutil_convertStringToInt(
            &p_msg_in->buffer[element_infos->str_start_idx],
            element_infos->str_size);
        break;
      case QCFG_iotopmode:
        HL78_shared.mode_and_bands_config.iot_op_mode =
          (ATCustom_HL78_QCFGiotopmode_t) ATutil_convertStringToInt(
            &p_msg_in->buffer[element_infos->str_start_idx],
            element_infos->str_size);
        break;
      case QCFG_band:
        HL78_shared.mode_and_bands_config.gsm_bands =
          (ATCustom_HL78_QCFGbandGSM_t) ATutil_convertHexaStringToInt32(
            &p_msg_in->buffer[element_infos->str_start_idx],
            element_infos->str_size);
        break;
      default:
        break;
    }
  }
  else if (element_infos->param_rank == 4U)
  {
    switch (HL78_current_qcfg_cmd)
    {
      case QCFG_band:
        (void) ATutil_convertHexaStringToInt64(&p_msg_in->buffer[element_infos->str_start_idx],
                                               element_infos->str_size,
                                               &HL78_shared.mode_and_bands_config.CatM1_bands_MsbPart,
                                               &HL78_shared.mode_and_bands_config.CatM1_bands_LsbPart);
        break;
      default:
        break;
    }
  }
  else if (element_infos->param_rank == 5U)
  {
    switch (HL78_current_qcfg_cmd)
    {
      case QCFG_band:
        (void) ATutil_convertHexaStringToInt64(&p_msg_in->buffer[element_infos->str_start_idx],
                                               element_infos->str_size,
                                               &HL78_shared.mode_and_bands_config.CatNB1_bands_MsbPart,
                                               &HL78_shared.mode_and_bands_config.CatNB1_bands_LsbPart);
        break;
      default:
        break;
    }
  }
  else
  {
    /* other parameters ignored */
    __NOP(); /* to avoid warning */
  }
  END_PARAM_LOOP()

  return (retval);
}

at_action_rsp_t fRspAnalyze_QIURC_HL78(at_context_t *p_at_ctxt, atcustom_modem_context_t *p_modem_ctxt,
                                       const IPC_RxMessage_t *p_msg_in, at_element_info_t *element_infos)
{
  atparser_context_t *p_atp_ctxt = &(p_at_ctxt->parser);
  at_action_rsp_t retval = ATACTION_RSP_IGNORED;
  PRINT_API("enter fRspAnalyze_QIURC_HL78()")

  /* memorize which is current QIURC received */
  ATCustom_HL78_QIURC_function_t HL78_current_qiurc_ind = QIURC_UNKNOWN;

  /*IP AT Commands manual - LTE Module Series - V1.0
  * URC
  * +QIURC:"closed",<connectID> : URC of connection closed
  * +QIURC:"recv",<connectID> : URC of incoming data
  * +QIURC:"incoming full" : URC of incoming connection full
  * +QIURC:"incoming",<connectID> ,<serverID>,<remoteIP>,<remote_port> : URC of incoming connection
  * +QIURC:"pdpdeact",<contextID> : URC of PDP deactivation
  *
  * for DNS request:
  * header: +QIURC: "dnsgip",<err>,<IP_count>,<DNS_ttl>
  * infos:  +QIURC: "dnsgip",<hostIPaddr>]
  */

  /* this is an URC */
  START_PARAM_LOOP()
  if (element_infos->param_rank == 2U)
  {
    AT_CHAR_t line[32] = {0};

    /* init param received info */
    HL78_current_qiurc_ind = QIURC_UNKNOWN;

    PRINT_BUF((const uint8_t *)&p_msg_in->buffer[element_infos->str_start_idx], element_infos->str_size)

    /* copy element to line for parsing */
    if (element_infos->str_size <= 32U)
    {
      (void) memcpy((void *)&line[0],
                    (const void *) & (p_msg_in->buffer[element_infos->str_start_idx]),
                    (size_t) element_infos->str_size);

      /* extract value and compare it to expected value */
      if ((AT_CHAR_t *) strstr((const CRC_CHAR_t *)&line[0], "closed") != NULL)
      {
        PRINT_DBG("+QIURC closed info received")
        HL78_current_qiurc_ind = QIURC_CLOSED;
      }
      else if ((AT_CHAR_t *) strstr((const CRC_CHAR_t *)&line[0], "recv") != NULL)
      {
        PRINT_DBG("+QIURC recv info received")
        HL78_current_qiurc_ind = QIURC_RECV;
      }
      else if ((AT_CHAR_t *) strstr((const CRC_CHAR_t *)&line[0], "incoming full") != NULL)
      {
        PRINT_DBG("+QIURC incoming full info received")
        HL78_current_qiurc_ind = QIURC_INCOMING_FULL;
      }
      else if ((AT_CHAR_t *) strstr((const CRC_CHAR_t *)&line[0], "incoming") != NULL)
      {
        PRINT_DBG("+QIURC incoming info received")
        HL78_current_qiurc_ind = QIURC_INCOMING;
      }
      else if ((AT_CHAR_t *) strstr((const CRC_CHAR_t *)&line[0], "pdpdeact") != NULL)
      {
        PRINT_DBG("+QIURC pdpdeact info received")
        HL78_current_qiurc_ind = QIURC_PDPDEACT;
      }
      else if ((AT_CHAR_t *) strstr((const CRC_CHAR_t *)&line[0], "dnsgip") != NULL)
      {
        PRINT_DBG("+QIURC dnsgip info received")
        HL78_current_qiurc_ind = QIURC_DNSGIP;
        if (p_atp_ctxt->current_SID != (at_msg_t) SID_CS_DNS_REQ)
        {
          /* URC not expected */
          retval = ATACTION_RSP_URC_IGNORED;
        }
      }
      else
      {
        PRINT_ERR("+QIURC field not managed")
      }
    }
    else
    {
      PRINT_ERR("param ignored (exceed maximum size)")
      retval = ATACTION_RSP_IGNORED;
    }
  }
  else if (element_infos->param_rank == 3U)
  {
    uint32_t connectID;
    socket_handle_t sockHandle;

    switch (HL78_current_qiurc_ind)
    {
      case QIURC_RECV:
        /* <connectID> */
        connectID = ATutil_convertStringToInt(&p_msg_in->buffer[element_infos->str_start_idx], element_infos->str_size);
        sockHandle = atcm_socket_get_socket_handle(p_modem_ctxt, connectID);
        (void) atcm_socket_set_urc_data_pending(p_modem_ctxt, sockHandle);
        PRINT_DBG("+QIURC received data for connId=%ld (socket handle=%ld)", connectID, sockHandle)
        /* last param */
        retval = ATACTION_RSP_URC_FORWARDED;
        break;

      case QIURC_CLOSED:
        /* <connectID> */
        connectID = ATutil_convertStringToInt(&p_msg_in->buffer[element_infos->str_start_idx], element_infos->str_size);
        sockHandle = atcm_socket_get_socket_handle(p_modem_ctxt, connectID);
        (void) atcm_socket_set_urc_closed_by_remote(p_modem_ctxt, sockHandle);
        PRINT_DBG("+QIURC closed for connId=%ld (socket handle=%ld)", connectID, sockHandle)
        /* last param */
        retval = ATACTION_RSP_URC_FORWARDED;
        break;

      case QIURC_INCOMING:
        /* <connectID> */
        PRINT_DBG("+QIURC incoming for connId=%ld",
                  ATutil_convertStringToInt(&p_msg_in->buffer[element_infos->str_start_idx], element_infos->str_size))
        break;

      case QIURC_PDPDEACT:
#if 0
        /* we do not process this event as it is also reported by modem when it enters in PSM.
         * So we do support only +CGEV to detect PDP deactivation
         */

        /* <contextID> */
        uint32_t contextID = ATutil_convertStringToInt(
                               &p_msg_in->buffer[element_infos->str_start_idx],
                               element_infos->str_size);
        PRINT_DBG("+QIURC pdpdeact for contextID=%ld", contextID)
        /* Need to inform  upper layer if pdn event URC has been subscribed
         * apply same treatment than CGEV NW PDN DEACT
        */
        p_modem_ctxt->persist.pdn_event.conf_id = atcm_get_configID_for_modem_cid(&p_modem_ctxt->persist,
                                                                                  (uint8_t)contextID);
        /* Indicate that an equivalent to +CGEV URC has been received */
        p_modem_ctxt->persist.pdn_event.event_origine = CGEV_EVENT_ORIGINE_NW;
        p_modem_ctxt->persist.pdn_event.event_scope = CGEV_EVENT_SCOPE_PDN;
        p_modem_ctxt->persist.pdn_event.event_type = CGEV_EVENT_TYPE_DEACTIVATION;
        p_modem_ctxt->persist.urc_avail_pdn_event = AT_TRUE;
        /* last param */
        retval = ATACTION_RSP_URC_FORWARDED;
#else
        retval = ATACTION_RSP_IGNORED;
#endif  /* 0 */
        break;

      case QIURC_DNSGIP:
        /* <err> or <hostIPaddr>]> */
        if (HL78_shared.QIURC_dnsgip_param.wait_header == AT_TRUE)
        {
          /* <err> expected */
          HL78_shared.QIURC_dnsgip_param.error =
            ATutil_convertStringToInt(&p_msg_in->buffer[element_infos->str_start_idx], element_infos->str_size);
          PRINT_DBG("+QIURC dnsgip with error=%ld", HL78_shared.QIURC_dnsgip_param.error)
          if (HL78_shared.QIURC_dnsgip_param.error != 0U)
          {
            /* Error when trying to get host address */
            HL78_shared.QIURC_dnsgip_param.finished = AT_TRUE;
            retval = ATACTION_RSP_ERROR;
          }
        }
        else
        {
          /* <hostIPaddr> expected
          *  with the current implementation, in case of many possible host IP address, we use
          *  the last one received
          */
          (void) memcpy((void *)HL78_shared.QIURC_dnsgip_param.hostIPaddr,
                        (const void *) & (p_msg_in->buffer[element_infos->str_start_idx]),
                        (size_t) element_infos->str_size);
          PRINT_DBG("+QIURC dnsgip Host address #%ld =%s", HL78_shared.QIURC_dnsgip_param.ip_count,
                    HL78_shared.QIURC_dnsgip_param.hostIPaddr)
          HL78_shared.QIURC_dnsgip_param.ip_count--;
          if (HL78_shared.QIURC_dnsgip_param.ip_count == 0U)
          {
            /* all expected URC have been reecived */
            HL78_shared.QIURC_dnsgip_param.finished = AT_TRUE;
            /* last param */
            retval = ATACTION_RSP_FRC_END;
          }
          else
          {
            retval = ATACTION_RSP_IGNORED;
          }
        }
        break;

      case QIURC_INCOMING_FULL:
      default:
        /* no parameter expected */
        PRINT_ERR("parameter not expected for this URC message")
        break;
    }
  }
  else if (element_infos->param_rank == 4U)
  {
    switch (HL78_current_qiurc_ind)
    {
      case QIURC_INCOMING:
        /* <serverID> */
        PRINT_DBG("+QIURC incoming for serverID=%ld",
                  ATutil_convertStringToInt(&p_msg_in->buffer[element_infos->str_start_idx], element_infos->str_size))
        break;

      case QIURC_DNSGIP:
        /* <IP_count> */
        if (HL78_shared.QIURC_dnsgip_param.wait_header == AT_TRUE)
        {
          /* <QIURC_dnsgip_param.ip_count> expected */
          HL78_shared.QIURC_dnsgip_param.ip_count =
            ATutil_convertStringToInt(&p_msg_in->buffer[element_infos->str_start_idx], element_infos->str_size);
          PRINT_DBG("+QIURC dnsgip IP count=%ld", HL78_shared.QIURC_dnsgip_param.ip_count)
          if (HL78_shared.QIURC_dnsgip_param.ip_count == 0U)
          {
            /* No host address available */
            HL78_shared.QIURC_dnsgip_param.finished = AT_TRUE;
            retval = ATACTION_RSP_ERROR;
          }
        }
        break;

      case QIURC_RECV:
      case QIURC_CLOSED:
      case QIURC_PDPDEACT:
      case QIURC_INCOMING_FULL:
      default:
        /* no parameter expected */
        PRINT_ERR("parameter not expected for this URC message")
        break;
    }
  }
  else if (element_infos->param_rank == 5U)
  {
    AT_CHAR_t remoteIP[32] = {0};

    switch (HL78_current_qiurc_ind)
    {
      case QIURC_INCOMING:
        /* <remoteIP> */
        (void) memcpy((void *)&remoteIP[0],
                      (const void *) & (p_msg_in->buffer[element_infos->str_start_idx]),
                      (size_t) element_infos->str_size);
        PRINT_DBG("+QIURC remoteIP for remoteIP=%s", remoteIP)
        break;

      case QIURC_DNSGIP:
        /* <DNS_ttl> */
        if (HL78_shared.QIURC_dnsgip_param.wait_header == AT_TRUE)
        {
          /* <QIURC_dnsgip_param.ttl> expected */
          HL78_shared.QIURC_dnsgip_param.ttl =
            ATutil_convertStringToInt(&p_msg_in->buffer[element_infos->str_start_idx], element_infos->str_size);
          PRINT_DBG("+QIURC dnsgip time-to-live=%ld", HL78_shared.QIURC_dnsgip_param.ttl)
          /* no error, now waiting for URC with IP address */
          HL78_shared.QIURC_dnsgip_param.wait_header = AT_FALSE;
        }
        break;

      case QIURC_RECV:
      case QIURC_CLOSED:
      case QIURC_PDPDEACT:
      case QIURC_INCOMING_FULL:
      default:
        /* no parameter expected */
        PRINT_ERR("parameter not expected for this URC message")
        break;
    }
  }
  else if (element_infos->param_rank == 6U)
  {
    switch (HL78_current_qiurc_ind)
    {
      case QIURC_INCOMING:
        /* <remote_port> */
        PRINT_DBG("+QIURC incoming for remote_port=%ld",
                  ATutil_convertStringToInt(&p_msg_in->buffer[element_infos->str_start_idx], element_infos->str_size))
        /* last param */
        retval = ATACTION_RSP_URC_FORWARDED;
        break;

      case QIURC_RECV:
      case QIURC_CLOSED:
      case QIURC_PDPDEACT:
      case QIURC_INCOMING_FULL:
      case QIURC_DNSGIP:
      default:
        /* no parameter expected */
        PRINT_ERR("parameter not expected for this URC message")
        break;
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

at_action_rsp_t fRspAnalyze_CCID_HL78(at_context_t *p_at_ctxt, atcustom_modem_context_t *p_modem_ctxt,
                                       const IPC_RxMessage_t *p_msg_in, at_element_info_t *element_infos)
{
  /*UNUSED(p_at_ctxt);*/
  at_action_rsp_t retval = ATACTION_RSP_INTERMEDIATE; /* received a valid intermediate answer */
  PRINT_API("enter fRspAnalyze_QCCID_HL78()")

  /* analyze parameters for +QCCID */
  START_PARAM_LOOP()
  if (element_infos->param_rank == 2U)
  {
    PRINT_DBG("ICCID:")
    PRINT_BUF((const uint8_t *)&p_msg_in->buffer[element_infos->str_start_idx], element_infos->str_size)

    /* HL78 specific treatment:
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

at_action_rsp_t fRspAnalyze_QINISTAT_HL78(at_context_t *p_at_ctxt, atcustom_modem_context_t *p_modem_ctxt,
                                          const IPC_RxMessage_t *p_msg_in, at_element_info_t *element_infos)
{
  /*UNUSED(p_at_ctxt);*/
  at_action_rsp_t retval = ATACTION_RSP_INTERMEDIATE; /* received a valid intermediate answer */
  PRINT_API("enter fRspAnalyze_QINISTAT_HL78()")

  /* analyze parameters for +QINISTAT */
  START_PARAM_LOOP()
  if (element_infos->param_rank == 2U)
  {
    PRINT_DBG("QINISTAT:")
    PRINT_BUF((const uint8_t *)&p_msg_in->buffer[element_infos->str_start_idx], element_infos->str_size)

    uint32_t sim_status = ATutil_convertStringToInt(&p_msg_in->buffer[element_infos->str_start_idx],
                                                    element_infos->str_size);
    /* check is CPIN is ready */
    if ((sim_status & QCINITSTAT_CPINREADY) != 0U)
    {
      p_modem_ctxt->persist.modem_sim_ready = AT_TRUE;
      PRINT_INFO("Modem SIM is ready")
    }
    else
    {
      p_modem_ctxt->persist.modem_sim_ready = AT_FALSE;
      PRINT_INFO("Modem SIM not ready yet...")
    }
  }
  else
  {
    /* other parameters ignored */
    __NOP(); /* to avoid warning */
  }
  END_PARAM_LOOP()

  return (retval);
}

at_action_rsp_t fRspAnalyze_KCELL_HL78(at_context_t *p_at_ctxt, atcustom_modem_context_t *p_modem_ctxt,
                                      const IPC_RxMessage_t *p_msg_in, at_element_info_t *element_infos)
{
  UNUSED(p_modem_ctxt);
  //atparser_context_t *p_atp_ctxt = &(p_at_ctxt->parser);
  at_action_rsp_t retval = ATACTION_RSP_INTERMEDIATE;
  PRINT_API("enter fRspAnalyze_KCELL_HL78()")
  //AT_CHAR_t line[32] = {0};
  PRINT_BUF((const uint8_t *)&p_msg_in->buffer[element_infos->str_start_idx], element_infos->str_size)

  /******Ashu Modification******/
  //TODO Some more indepth extraction of AT command responses

  /* memorize sysmode for current QCSQ */
  //ATCustom_HL78_QCSQ_sysmode_t HL78_current_qcsq_sysmode = QCSQ_unknown;

  /* analyze parameters for QCSQ */
  /*if (p_atp_ctxt->current_atcmd.type == ATTYPE_EXECUTION_CMD)
  {

    *  format: +QCSQ: <sysmode>,sysmode>,[,<value1>[,<value2>[,<value3>[,<value4>]]]]
    *
    *  <sysmode> "NOSERVICE", "GSM", "CAT-M1" or "CAT-NB1"
    *
    * if <sysmode> = "NOSERVICE"
    *    no values
    * if <sysmode> = "GSM"
    *    <value1> = <gsm_rssi>
    * if <sysmode> = "CAT-M1" or "CAT-NB1"
    *    <value1> = <lte_rssi> / <value2> = <lte_rssp> / <value3> = <lte_sinr> / <value4> = <lte_rsrq>


    START_PARAM_LOOP()
    if (element_infos->param_rank == 2U)
    {
      AT_CHAR_t line[32] = {0};

       init param received info
      HL78_current_qcsq_sysmode = QCSQ_unknown;

      PRINT_BUF((const uint8_t *)&p_msg_in->buffer[element_infos->str_start_idx], element_infos->str_size)

       copy element to line for parsing
      if (element_infos->str_size <= 32U)
      {
        (void) memcpy((void *)&line[0],
                      (const void *) & (p_msg_in->buffer[element_infos->str_start_idx]),
                      (size_t) element_infos->str_size);

         extract value and compare it to expected value
        if ((AT_CHAR_t *) strstr((const CRC_CHAR_t *)&line[0], "NOSERVICE") != NULL)
        {
          PRINT_DBG("+QCSQ sysmode=NOSERVICE")
          HL78_current_qcsq_sysmode = QCSQ_noService;
        }
        else if ((AT_CHAR_t *) strstr((const CRC_CHAR_t *)&line[0], "GSM") != NULL)
        {
          PRINT_DBG("+QCSQ sysmode=GSM")
          HL78_current_qcsq_sysmode = QCSQ_gsm;
        }
        else if ((AT_CHAR_t *) strstr((const CRC_CHAR_t *)&line[0], "CAT-M1") != NULL)
        {
          PRINT_DBG("+QCSQ sysmode=CAT-M1")
          HL78_current_qcsq_sysmode = QCSQ_catM1;
        }
        else if ((AT_CHAR_t *) strstr((const CRC_CHAR_t *)&line[0], "CAT-NB1") != NULL)
        {
          PRINT_DBG("+QCSQ sysmode=CAT-NB1")
          HL78_current_qcsq_sysmode = QCSQ_catNB1;
        }
        else
        {
          PRINT_ERR("+QCSQ field not managed")
        }
      }
      else
      {
        PRINT_ERR("param ignored (exceed maximum size)")
        retval = ATACTION_RSP_IGNORED;
      }
    }
    else if (element_infos->param_rank == 3U)
    {
       <value1>
      switch (HL78_current_qcsq_sysmode)
      {
        case QCSQ_gsm:
           <gsm_rssi>
          PRINT_DBG("<gsm_rssi> = %s%ld",
                    (ATutil_isNegative(&p_msg_in->buffer[element_infos->str_start_idx],
                                       element_infos->str_size) == 1U) ? "-" : " ",
                    ATutil_convertStringToInt(&p_msg_in->buffer[element_infos->str_start_idx],
                                              element_infos->str_size))
          break;

        case QCSQ_catM1:
        case QCSQ_catNB1:
           <lte_rssi>
          PRINT_DBG("<lte_rssi> = %s%ld",
                    (ATutil_isNegative(&p_msg_in->buffer[element_infos->str_start_idx],
                                       element_infos->str_size) == 1U) ? "-" : " ",
                    ATutil_convertStringToInt(&p_msg_in->buffer[element_infos->str_start_idx],
                                              element_infos->str_size))
          break;

        default:
           parameter ignored
          break;
      }
    }
    else if (element_infos->param_rank == 4U)
    {
       <value2>
      switch (HL78_current_qcsq_sysmode)
      {
        case QCSQ_catM1:
        case QCSQ_catNB1:
           <lte_rsrp>
           rsrp range is -44 dBm to -140 dBm
          PRINT_INFO("<lte_rsrp> = %s%ld dBm",
                     (ATutil_isNegative(&p_msg_in->buffer[element_infos->str_start_idx],
                                        element_infos->str_size) == 1U) ? "-" : " ",
                     ATutil_convertStringToInt(&p_msg_in->buffer[element_infos->str_start_idx],
                                               element_infos->str_size))
          break;

        default:
           parameter ignored
          break;
      }
    }
    else if (element_infos->param_rank == 5U)
    {
       <value3>
      switch (HL78_current_qcsq_sysmode)
      {
        case QCSQ_catM1:
        case QCSQ_catNB1:
           <lte_sinr>
          PRINT_DBG("<lte_sinr> = %s%ld",
                    (ATutil_isNegative(&p_msg_in->buffer[element_infos->str_start_idx],
                                       element_infos->str_size) == 1U) ? "-" : " ",
                    ATutil_convertStringToInt(&p_msg_in->buffer[element_infos->str_start_idx],
                                              element_infos->str_size))
          break;

        default:
           parameter ignored
          break;
      }
    }
    else if (element_infos->param_rank == 6U)
    {
       <value4>
      switch (HL78_current_qcsq_sysmode)
      {
        case QCSQ_catM1:
        case QCSQ_catNB1:
           <lte_rsrq>
          PRINT_DBG("<lte_rsrq> = %s%ld",
                    (ATutil_isNegative(&p_msg_in->buffer[element_infos->str_start_idx],
                                       element_infos->str_size) == 1U) ? "-" : " ",
                    ATutil_convertStringToInt(&p_msg_in->buffer[element_infos->str_start_idx],
                                              element_infos->str_size))
          break;

        default:
           parameter ignored
          break;
      }
    }
    else
    {
       parameter ignored
      __NOP();  to avoid warning
    }

    END_PARAM_LOOP()
  }*/

  return (retval);
}

at_action_rsp_t fRspAnalyze_CGMR_HL78(at_context_t *p_at_ctxt, atcustom_modem_context_t *p_modem_ctxt,
                                      const IPC_RxMessage_t *p_msg_in, at_element_info_t *element_infos)
{
  atparser_context_t *p_atp_ctxt = &(p_at_ctxt->parser);
  at_action_rsp_t retval = ATACTION_RSP_IGNORED;
  PRINT_API("enter fRspAnalyze_CGMR_HL78()")

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

at_action_rsp_t fRspAnalyze_QPSMTIMER_HL78(at_context_t *p_at_ctxt, atcustom_modem_context_t *p_modem_ctxt,
                                           const IPC_RxMessage_t *p_msg_in, at_element_info_t *element_infos)
{
  UNUSED(p_at_ctxt);
  UNUSED(p_modem_ctxt);
  at_action_rsp_t retval = ATACTION_RSP_IGNORED;
  PRINT_API("enter fRspAnalyze_QPSMTIMER_HL78()")

  /* analyze parameters for +QPSMTIMER
   * this is an URC
   */
  START_PARAM_LOOP()
  if (element_infos->param_rank == 2U)
  {
    uint32_t t3412_nwk_value = ATutil_convertStringToInt(&p_msg_in->buffer[element_infos->str_start_idx],
                                                         element_infos->str_size);
    PRINT_INFO("URC +QPSMTIMER received: TAU_duration (T3412) = %ld sec", t3412_nwk_value)

    if (t3412_nwk_value != p_modem_ctxt->persist.low_power_status.nwk_periodic_TAU)
    {
      p_modem_ctxt->persist.low_power_status.nwk_periodic_TAU = t3412_nwk_value;
      p_modem_ctxt->persist.urc_avail_lp_status = AT_TRUE;
      PRINT_DBG("New T3412 value detected")
    }
  }
  else if (element_infos->param_rank == 3U)
  {
    uint32_t t3324_nwk_value = ATutil_convertStringToInt(&p_msg_in->buffer[element_infos->str_start_idx],
                                                         element_infos->str_size);
    PRINT_INFO("URC +QPSMTIMER received: Active_duration (T3324) = %ld sec", t3324_nwk_value)

    if (t3324_nwk_value != p_modem_ctxt->persist.low_power_status.nwk_active_time)
    {
      p_modem_ctxt->persist.low_power_status.nwk_active_time = t3324_nwk_value;
      p_modem_ctxt->persist.urc_avail_lp_status = AT_TRUE;
      PRINT_DBG("New T3324 value detected")
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

at_action_rsp_t fRspAnalyze_COPS_HL78(at_context_t *p_at_ctxt, atcustom_modem_context_t *p_modem_ctxt,
                                      const IPC_RxMessage_t *p_msg_in, at_element_info_t *element_infos)
{
  at_action_rsp_t retval;
  PRINT_API("enter fRspAnalyze_COPS_HL78()")

  /* HL78 does not follow 3GPP TS 27.007 values for <AcT> parameter in AT+COPS
  * Encapsulate generic COPS build function to manage this difference
  */
  retval = fRspAnalyze_COPS(p_at_ctxt, p_modem_ctxt, p_msg_in, element_infos);

  if (retval != ATACTION_RSP_ERROR)
  {
    if ((p_modem_ctxt->SID_ctxt.read_operator_infos.optional_fields_presence & CS_RSF_ACT_PRESENT) != 0U)
    {
      if (p_modem_ctxt->SID_ctxt.read_operator_infos.AcT == CS_ACT_EC_GSM_IOT)
      {
        /* HL78 AcT = 8 means cat.M1
        *  3GPP AcT = 8 means CS_ACT_EC_GSM_IOT, cat.M1 value if 9
        *  convert 8 to 9 for upper layers
        */
        p_modem_ctxt->SID_ctxt.read_operator_infos.AcT = CS_ACT_E_UTRAN;
      }
    }
  }

  return (retval);
}

/************************ (C) COPYRIGHT Sierra Wireless *****END OF FILE****/

