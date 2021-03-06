/**
  ******************************************************************************
  * @file    orp.h
  * @author  Sierra Wireless Inc. and Affiliates
  * @brief   Header for orp.c module for Sierra Wireless Octave Modules
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
#ifndef ORP_H_
#define ORP_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "plf_config.h"

#if defined(USE_COM_MDM)

#include "com_common.h"
#include "com_err.h"
#include "com_mdm.h"

/* Exported constants --------------------------------------------------------*/

#define ORP_HANDLE_ERROR   0xFFU /* ORP error handle */
#define ORP_MAX_CMD_SIZE   512U  /* Max Command Tx buffer */
#define ORP_MAX_RSP_SIZE   250U  /* Max Command Response buffer */
#define ORP_MAX_RESOURCE_NAME   100U  /* Max Resource data update buffer */
#define ORP_MAX_RESOURCE_VALUE   350U  /* Max Resource data update buffer */

typedef void (* orp_urc_callback_t)(void);

/* To maintain Cloud connectivity status at application level */
typedef enum
{
  ORP_disconnected = 0,
  ORP_Connected  = 1,
} orp_status_t;

/* Octave resource declaration structure */
typedef struct
{
  com_char_t   resource_name[ORP_MAX_RESOURCE_NAME]; /* Max Final Command size is limited to 250 bytes, hence limiting resource name by 10 */
  com_char_t   res_dir;
  com_char_t   res_type;
} orp_resource_create_t;

/* Octave JSON resource declaration structure */
typedef struct
{
  com_char_t   resource_name[ORP_MAX_RESOURCE_NAME]; /* Max Final Command size is limited to 250 bytes, hence limiting resource name by 10 */
  com_char_t   * resource_value;
} orp_json_resource_update_t;

/* Octave Numeric resource declaration structure */
typedef struct
{
  com_char_t   resource_name[ORP_MAX_RESOURCE_NAME]; /* Max Final Command size is limited to 250 bytes, hence limiting resource name by 10 */
  float   resource_value;
} orp_numeric_resource_update_t;

/* Octave Boolean resource declaration structure */
typedef struct
{
  com_char_t   resource_name[ORP_MAX_RESOURCE_NAME]; /* Max Final Command size is limited to 250 bytes, hence limiting resource name by 10 */
  bool         resource_value;
} orp_bool_resource_update_t;

/* ORP response declaration structure */
typedef struct
{
  char     resource_name[ORP_MAX_RESOURCE_NAME]; /* Max Final Command size is limited to 250 bytes, hence limiting resource name by 10 */
  uint8_t  resource_value[ORP_MAX_RESOURCE_VALUE];
} orp_response_t;

orp_response_t orpResponse;
/*** ORP functionalities ****************************************************/

/**
  * @brief  register callback for orp urc
  * @note   register a function as callback for orp urc receive from modem
  * @param  callback - The call back to be registered
  * @retval -
  * @note   the provided call back function should execute a minimum of code.
  *         Application should create an event or message to trigger a receive of a message to be treated later
  */
com_err_t orp_subscribe_event(uint8_t handle, orp_urc_callback_t callback);

/**
  * @brief  open orp session, and return orp handle
  * @note   com_mdm_open has to be called before using any com_mdm_* function
  * @param  -
  * @retval - the orp handle
  * @note   a handle value >=0 is a valid handle value
  *         a handle value <0 represents an error : no more handle available. Using a handle <0 in other com_mdm_*
  *         functions will result as an error.
  */
uint8_t orp_open(void);

/**
  * @brief  sends a orp command to the Modem.
  * @note
  * @param[in]  handle           - the orp handle to use, given by orp_open
  * @param[in]  resource         - Pointer that holds the resource parameters like name, direction and type.
  * @param[out] command_err_code - the error code returned by the command
  * @retval - error code
  * @note   command sent correctly when error code is COM_ERR_OK
  *         command not sent correctly when error code is COM_ERR_GENERAL
  *         handle is unknown when error code is COM_ERR_DESCRIPTOR
  */
com_err_t orp_create_resource(uint8_t handle, orp_resource_create_t * resource, int32_t *command_err_code);

/**
  * @brief  sends a orp create handler command to the Modem.
  * @note
  * @param[in]  handle           - the orp handle to use, given by orp_open
  * @param[in]  res_path         - the path of resource in Octave to be set
  * @param[out] command_err_code - the error code returned by the command
  * @retval - error code
  * @note   command sent correctly when error code is COM_ERR_OK
  *         command not sent correctly when error code is COM_ERR_GENERAL
  *         handle is unknown when error code is COM_ERR_DESCRIPTOR
  */
com_err_t orp_create_handler(uint8_t handle,
		orp_resource_create_t * sensor, int32_t *command_err_code);

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
com_err_t orp_get_resource(uint8_t handle, com_char_t *res_path,
        com_char_t *rsp_buf, int32_t *command_err_code);

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
com_err_t orp_set_numeric_resource(uint8_t handle, orp_numeric_resource_update_t * resource, com_char_t *rsp_buf, int32_t *command_err_code);

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
com_err_t orp_set_json_resource(uint8_t handle, orp_json_resource_update_t * resource, com_char_t *rsp_buf, int32_t *command_err_code);
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
com_err_t orp_set_bool_resource(uint8_t handle, orp_bool_resource_update_t * resource, com_char_t *rsp_buf, int32_t *command_err_code);
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
com_err_t orp_receive(uint8_t handle, com_char_t *resp, uint32_t length, int32_t *command_err_code);

/**
  * @brief  close orp session, and release orp handle
  * @note   after a call to com_mdm_close no call to any com_mdm_* function should be done
  * @param  - the orp handle to be released
  * @retval - error code
  * @note   Handle is correctly released when error code is COM_ERR_OK
  *         handle can't be released when error code is COM_ERR_DESCRIPTOR
  */
com_err_t orp_close(uint8_t handle);


/**
  * @brief  Decode the ORP URC.
  * @note
  * @param[in]  pbuf             - the orp URC buffer
  * @param[in]  length           - the length of the URC buffer
  * @retval - Decoded URC message structure
  *
  */
orp_response_t * orpResponseDecode(com_char_t *pbuf, uint32_t size);

#endif /* defined(USE_COM_MDM) */

#ifdef __cplusplus
}
#endif

#endif /* COM_MDM_H */
/************************ (C) COPYRIGHT Sierra Wireless *****END OF FILE****/
