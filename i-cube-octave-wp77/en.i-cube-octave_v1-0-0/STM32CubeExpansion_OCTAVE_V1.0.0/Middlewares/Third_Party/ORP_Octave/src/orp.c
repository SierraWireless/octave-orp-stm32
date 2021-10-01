/**
  ******************************************************************************
  * @file    orp.c
  * @author  Sierra Wireless Inc. and Affiliates
  * @brief   Middleware component for processing of ORP  for Sierra Wireless Octave Modules
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
#include "plf_config.h"

/* Includes ------------------------------------------------------------------*/

#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

#include <math.h>
#include "orp.h"

#include "cellular_control_api.h"

#include "rtosal.h"

/* Private defines -----------------------------------------------------------*/

/* Private typedef -----------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
#if (USE_TRACE_ATCUSTOM_SPECIFIC == 1U)
#if (USE_PRINTF == 0U)
#include "trace_interface.h"
#define PRINT_INFO(format, args...) TRACE_PRINT(DBG_CHAN_ATCMD, DBL_LVL_P0, "WP77:" format "\n\r", ## args)
#define PRINT_DBG(format, args...)  TRACE_PRINT(DBG_CHAN_ATCMD, DBL_LVL_P1, "WP77:" format "\n\r", ## args)
#define PRINT_API(format, args...)  TRACE_PRINT(DBG_CHAN_ATCMD, DBL_LVL_P2, "WP77 API:" format "\n\r", ## args)
#define PRINT_ERR(format, args...)  TRACE_PRINT(DBG_CHAN_ATCMD, DBL_LVL_ERR, "WP77 ERROR:" format "\n\r", ## args)
#define PRINT_BUF(pbuf, size)       TRACE_PRINT_BUF_CHAR(DBG_CHAN_ATCMD, DBL_LVL_P1, (const CRC_CHAR_t *)pbuf, size);
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
/* Private variables ---------------------------------------------------------*/

static uint8_t orp_cmd_buffer[ORP_MAX_CMD_SIZE];
static com_char_t tx_command[ORP_MAX_CMD_SIZE] = {0};
/* Global variables ----------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

/* Private function Definition -----------------------------------------------*/


/* Functions Definition ------------------------------------------------------*/

/**
  * @brief  open orp session, and return orp handle
  * @note   com_mdm_open has to be called before using any com_mdm_* function
  * @param  -
  * @retval - the orp handle
  * @note   a handle value >=0 is a valid handle value
  *         a handle value <0 represents an error : no more handle available. Using a handle <0 in other com_mdm_*
  *         functions will result as an error.
  */
uint8_t orp_open(void)
{
	uint8_t   orpHandle = ORP_HANDLE_ERROR;
	orpHandle = com_mdm_open();
	return orpHandle;
}

/**
  * @brief  register callback for orp urc
  * @note   register a function as callback for orp urc receive from modem
  * @param  callback - The call back to be registered
  * @retval -
  * @note   the provided call back function should execute a minimum of code.
  *         Application should create an event or message to trigger a receive of a message to be treated later
  */
com_err_t orp_subscribe_event(uint8_t handle, orp_urc_callback_t callback)
{

	com_err_t com_err;
	com_err = com_mdm_subscribe_event( handle, callback);
	return com_err;
}

/**
  * @brief  sends a orp command to the Modem.
  * @note
  * @param[in]  handle           - the orp handle to use, given by orp_open
  * @param[in]  resource         - the resource structure
  * @param[out] command_err_code - the error code returned by the command
  * @retval - error code
  * @note   command sent correctly when error code is COM_ERR_OK
  *         command not sent correctly when error code is COM_ERR_GENERAL
  *         handle is unknown when error code is COM_ERR_DESCRIPTOR
  */
com_err_t orp_create_resource(uint8_t handle, orp_resource_create_t * resource, int32_t *command_err_code)
{
	com_err_t com_err;
	(void) memset((com_char_t *)tx_command, 0, ORP_MAX_CMD_SIZE);
	(void)sprintf((CRC_CHAR_t *)orp_cmd_buffer, "%c%c00P%s",resource->res_dir,resource->res_type,(com_char_t *)resource->resource_name);
	/* recopy ORP response to command buffer */
	(void) memcpy((void *) tx_command,(const void *)orp_cmd_buffer,sizeof (orp_cmd_buffer));
	com_err = com_mdm_send(handle, tx_command, strlen((CRC_CHAR_t *)tx_command), command_err_code);

	return com_err;
}

/**
  * @brief  sends a orp create handler command to the Modem.
  * @note
  * @param[in]  handle           - the orp handle to use, given by orp_open
  * @param[in]  resource         - the resource structure
  * @param[out] command_err_code - the error code returned by the command
  * @retval - error code
  * @note   command sent correctly when error code is COM_ERR_OK
  *         command not sent correctly when error code is COM_ERR_GENERAL
  *         handle is unknown when error code is COM_ERR_DESCRIPTOR
  */
com_err_t orp_create_handler(uint8_t handle,
		orp_resource_create_t * resource, int32_t *command_err_code)
{
	com_err_t com_err;
	(void) memset((com_char_t *)tx_command, 0, ORP_MAX_CMD_SIZE);
	(void)sprintf((CRC_CHAR_t *)orp_cmd_buffer, "H.00P%s",(com_char_t *)resource->resource_name);
	/* recopy ORP response to command buffer */
	(void) memcpy((void *) tx_command,(const void *)orp_cmd_buffer,sizeof (orp_cmd_buffer));
	com_err = com_mdm_send(handle, tx_command, strlen((CRC_CHAR_t *)tx_command), command_err_code);
	return com_err;
}
/**
  * @brief  initiate a get ORP on a particular resource to the Modem.
  * @note
  * @param[in]  handle           - the orp handle to use, given by orp_open
  * @param[in]  res_path         - the path of resource in Octave to be set
  * @param[in]  rsp_buff         - the string response received
  * @param[out] command_err_code - the error code returned by the command
  * @retval - error code
  * @note   command sent correctly when error code is COM_ERR_OK
  *         command not sent correctly when error code is COM_ERR_GENERAL
  *         handle is unknown when error code is COM_ERR_DESCRIPTOR
  */
com_err_t orp_get_resource(uint8_t handle, com_char_t *res_path, com_char_t *rsp_buf, int32_t *command_err_code)
{
	com_err_t com_err;
	(void) memset((com_char_t *)tx_command, 0, ORP_MAX_CMD_SIZE);
	if(strcmp((const char *)res_path,"orp_isConnected") == 0)
	{
	  com_char_t orp_isConnected_get[32] = "cloudInterface/connected/value";
	  (void)sprintf((CRC_CHAR_t *)orp_cmd_buffer, "G.00P/%s",(com_char_t *)orp_isConnected_get);
	}
	else
	{
	  (void)sprintf((CRC_CHAR_t *)orp_cmd_buffer, "G.00P/%s",(com_char_t *)res_path);
	}
	/* recopy ORP response to command buffer */
	(void) memcpy((void *) tx_command,(const void *)orp_cmd_buffer,sizeof (orp_cmd_buffer));
	com_err =  com_mdm_transaction(handle, tx_command, strlen((CRC_CHAR_t *)tx_command),
			rsp_buf,ORP_MAX_RSP_SIZE, command_err_code);
	return com_err;
}

/**
  * @brief  initiate a SET ORP on a particular numeric resource to the Modem.
  * @note
  * @param[in]  handle           - the orp handle to use, given by orp_open
  * @param[in]  resource         - the resource structure
  * @param[in]  rsp_buff         - the string response received
  * @param[out] command_err_code - the error code returned by the command
  * @retval - error code
  * @note   command sent correctly when error code is COM_ERR_OK
  *         command not sent correctly when error code is COM_ERR_GENERAL
  *         handle is unknown when error code is COM_ERR_DESCRIPTOR
  */
com_err_t orp_set_numeric_resource(uint8_t handle, orp_numeric_resource_update_t * resource, com_char_t *rsp_buf, int32_t *command_err_code)
{
	com_err_t com_err;
	(void) memset((com_char_t *)tx_command, 0, ORP_MAX_CMD_SIZE);
	(void)sprintf((CRC_CHAR_t *)orp_cmd_buffer, "PN00P%s,D%f",(com_char_t *)resource->resource_name,(resource->resource_value));

	/* recopy ORP response to command buffer */
	(void) memcpy((void *) tx_command,(const void *)orp_cmd_buffer,sizeof (orp_cmd_buffer));
	com_err =  com_mdm_transaction(handle, tx_command, strlen((CRC_CHAR_t *)tx_command),
			rsp_buf,ORP_MAX_RSP_SIZE, command_err_code);
	return com_err;

}

/**
  * @brief  initiate a SET ORP on a particular json resource to the Modem.
  * @note
  * @param[in]  handle           - the orp handle to use, given by orp_open
  * @param[in]  resource         - the resource structure
  * @param[in]  rsp_buff         - the string response received
  * @param[out] command_err_code - the error code returned by the command
  * @retval - error code
  * @note   command sent correctly when error code is COM_ERR_OK
  *         command not sent correctly when error code is COM_ERR_GENERAL
  *         handle is unknown when error code is COM_ERR_DESCRIPTOR
  */
com_err_t orp_set_json_resource(uint8_t handle, orp_json_resource_update_t * resource, com_char_t *rsp_buf, int32_t *command_err_code)
{
	com_err_t com_err;
	(void) memset((com_char_t *)tx_command, 0, ORP_MAX_CMD_SIZE);
	(void)sprintf((CRC_CHAR_t *)orp_cmd_buffer, "PJ00P%s,D{%s}",(com_char_t *)resource->resource_name,(com_char_t *)(resource->resource_value));
	/* recopy ORP response to command buffer */
	(void) memcpy((void *) tx_command,(const void *)orp_cmd_buffer,sizeof (orp_cmd_buffer));
	com_err =  com_mdm_transaction(handle, tx_command, strlen((CRC_CHAR_t *)tx_command),
			rsp_buf,ORP_MAX_RSP_SIZE, command_err_code);
	return com_err;
}

/**
  * @brief  initiate a SET ORP on a particular boolean resource to the Modem.
  * @note
  * @param[in]  handle           - the orp handle to use, given by orp_open
  * @param[in]  resource         - the resource structure
  * @param[in]  rsp_buff         - the string response received
  * @param[out] command_err_code - the error code returned by the command
  * @retval - error code
  * @note   command sent correctly when error code is COM_ERR_OK
  *         command not sent correctly when error code is COM_ERR_GENERAL
  *         handle is unknown when error code is COM_ERR_DESCRIPTOR
  */
com_err_t orp_set_bool_resource(uint8_t handle, orp_bool_resource_update_t * resource, com_char_t *rsp_buf, int32_t *command_err_code)
{
	com_err_t com_err;
	(void) memset((com_char_t *)tx_command, 0, ORP_MAX_CMD_SIZE);
	if((resource->resource_value) == false)
	{
	  (void)sprintf((CRC_CHAR_t *)orp_cmd_buffer, "PB00P%s,Dfalse",(com_char_t *)resource->resource_name);
	}
	else
	{
	  (void)sprintf((CRC_CHAR_t *)orp_cmd_buffer, "PB00P%s,Dtrue",(com_char_t *)resource->resource_name);
	}
	/* recopy ORP response to command buffer */
	(void) memcpy((void *) tx_command,(const void *)orp_cmd_buffer,sizeof (orp_cmd_buffer));
	com_err =  com_mdm_transaction(handle, tx_command, strlen((CRC_CHAR_t *)tx_command),
			rsp_buf,ORP_MAX_RSP_SIZE, command_err_code);
	return com_err;
}
/**
  * @brief  read message from modem to the rsp buffer provided by the application.
  * @note
  * @param[in]  handle           - the orp handle to use, given by orp_open
  * @param[in]  resp             - the string response received
  * @param[in]  length           - the length of the resp buffer
  * @param[out] command_err_code - the error code returned by the command
  * @retval - error code
  * @note   command sent correctly when error code is COM_ERR_OK
  *         message is not receive correctly when error code is COM_ERR_GENERAL
  *         handle is unknown when error code is COM_ERR_DESCRIPTOR
  */
com_err_t orp_receive(uint8_t handle, com_char_t *resp, uint32_t length, int32_t *command_err_code)
{
	com_err_t com_err;
	com_err =  com_mdm_receive(handle,resp,length,command_err_code);
	return com_err;
}

/**
  * @brief  Decode the ORP URC.
  * @note
  * @param[in]  pbuf             - the orp URC buffer
  * @param[in]  length           - the length of the URC buffer
  * @retval - Decoded URC message structure
  *
  */
orp_response_t * orpResponseDecode(com_char_t *pbuf, uint32_t size)
{
  /* parse message content to split the Resource name and value and pass it back to the calling function
   */
  const char *orpStr1;
  const char *orpStr2;
  orpStr1 = strchr((const void *)pbuf,'P');
  orpStr2 = strchr((const void *)pbuf,'D');
  strncpy((char *)orpResponse.resource_name,orpStr1+1,(int)(orpStr2-orpStr1-2));
  strcpy((char *)orpResponse.resource_value,orpStr2+1);
  return(&orpResponse);
}
