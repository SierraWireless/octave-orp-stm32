/**
  ******************************************************************************
  * @file    com_mdm.c
  * @author  MCD Application Team
  * @brief   This file implements communication with International Circuit Card
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */

#if defined(USE_COM_MDM)

/* Includes ------------------------------------------------------------------*/

#include "com_mdm.h"

#include <string.h>
#include <stdlib.h>

#include "rtosal.h"

#include "com_err.h"
#include "com_sockets_net_compat.h"
#include "com_trace.h"


#include "cellular_service_os.h"

#include "dc_common.h"
#include "cellular_control_api.h"
#include "cellular_service_datacache.h"

/* Private defines -----------------------------------------------------------*/

/* Private typedef -----------------------------------------------------------*/

typedef enum
{
  COM_MDM_NOT_USED = 0,
  COM_MDM_USED
} com_mdm_handle_state_t;

typedef struct
{
  com_mdm_handle_state_t         handle;
} com_mdm_desc_t;

/* Private macros ------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Descriptor of com_mdm status */
static com_mdm_desc_t com_mdm_desc;
/* storage of registered application callback for MDM URC */
static com_mdm_urc_callback_t com_mdm_app_cb;

/* Global variables ----------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

/* Local callback for MDM URC */
void com_mdm_cb(CS_comMdm_status_t comMdmd_event_infos);

/* Private function Definition -----------------------------------------------*/

void com_mdm_cb(CS_comMdm_status_t comMdmd_event_infos)
{
  UNUSED(comMdmd_event_infos);
  if (com_mdm_app_cb != NULL)
  {
    /* if the application MDM URC call back is registered/defined, then call it */
    com_mdm_app_cb();
  }
}

/* Functions Definition ------------------------------------------------------*/

/**
  * @brief  register callback for mdm urc
  * @note   register a function as callback for mdm urc receive from modem
  * @param  callback - The call back to be registered. May be NULL to unregister callback
  * @retval -
  * @note   the provided call back function should execute a minimum of code.
  *         Application should create an event or message to trigger a receive of a message to be treated later
  */
com_err_t com_mdm_subscribe_event(uint8_t handle, com_mdm_urc_callback_t callback)
{
  com_err_t error = COM_ERR_GENERAL;
  CS_Status_t cs_return;

  if ((handle < 1U) && (com_mdm_desc.handle == COM_MDM_USED))
  {
    /* handle parameter is valid :          */
    /*  -not bigger than number session max */
    /*  -handle is actually used            */

    /* Subscribe com_mdm callback to MDM URC event */
    cs_return = osCS_ComMdm_subscribe_event(com_mdm_cb);
    if (cs_return == CELLULAR_OK)
    {
      /* store the application callback given in parameter */
      com_mdm_app_cb = callback;
      error = COM_ERR_OK;
    }
    else
    {
      error = COM_ERR_GENERAL;
    }
  }
  return (error);
}

/**
  * @brief  open mdm session, and return mdm handle
  * @note   com_mdm_open has to be called before using any com_mdm_* function
  * @param  -
  * @retval - the mdm handle
  * @note   a handle value different from COM_MDM_HANDLE_ERROR is a valid handle value
  *         a handle value equals COM_MDM_HANDLE_ERROR represents an error : no more handle availables.
  *         Using a handle COM_MDM_HANDLE_ERROR in other com_mdm_* functions will result as an error.
  */
uint8_t com_mdm_open(void)
{
  uint8_t   i = 0;
  uint8_t   handle = COM_MDM_HANDLE_ERROR;

  if (com_mdm_desc.handle == COM_MDM_NOT_USED)
  {
    /* An available handle exists, use it */
    com_mdm_desc.handle = COM_MDM_USED;
    handle = i;
  }

  /* Return the found available handle, or COM_MDM_HANDLE_ERROR if no available handle found */
  return (handle);
}

/**
  * @brief  sends a MDM command to the Modem.
  * @note
  * @param[in]  handle           - the mdm handle to use, given by com_mdm_open
  * @param[in]  command          - the string command to be send
  * @param[in]  length           - the length of the command tyo be send
  * @param[out] command_err_code - the error code returned by the command
  * @retval - error code
  * @note   command sent correctly when error code is COM_ERR_OK
  *         command not sent correctly when error code is COM_ERR_GENERAL
  *         handle is unknown when error code is COM_ERR_DESCRIPTOR
  */
com_err_t com_mdm_send(uint8_t handle, com_char_t *command, uint32_t length, int32_t *command_err_code)
{
  com_err_t error = COM_ERR_DESCRIPTOR;
  CS_Status_t cs_return;
  CS_Tx_Buffer_t cs_tx_buffer;

  if ((handle < 1U) && (com_mdm_desc.handle == COM_MDM_USED))
  {
    cs_tx_buffer.p_buffer = command;
    cs_tx_buffer.buffer_size = length;

    cs_return = osCS_ComMdm_send(&cs_tx_buffer, command_err_code);
    if (cs_return == CELLULAR_OK)
    {
      error = COM_ERR_OK;
    }
    else
    {
      error = COM_ERR_GENERAL;
    }
  }
  return (error);
}

/**
  * @brief  initiate a full transaction (send + answer receive) for a MDM command to the Modem.
  * @note
  * @param[in]  handle           - the mdm handle to use, given by com_mdm_open
  * @param[in]  cmd_buff         - the string command to be send
  * @param[in]  cmd_length       - the length of the command to be send
  * @param[in]  rsp_buff         - the string response received
  * @param[in]  rsp_length       - the length of rsp_buff
  * @param[out] command_err_code - the error code returned by the command
  * @retval - error code
  * @note   command sent correctly when error code is COM_ERR_OK
  *         command not sent correctly when error code is COM_ERR_GENERAL
  *         handle is unknown when error code is COM_ERR_DESCRIPTOR
  */
com_err_t com_mdm_transaction(uint8_t handle, com_char_t *cmd_buff, uint32_t cmd_length,
                              com_char_t *rsp_buf, uint32_t rsp_length, int32_t *command_err_code)
{
  com_err_t error = COM_ERR_DESCRIPTOR;
  CS_Status_t cs_return;
  CS_Tx_Buffer_t cs_tx_buffer;
  CS_Rx_Buffer_t cs_rx_buffer;

  if ((handle < 1U) && (com_mdm_desc.handle == COM_MDM_USED))
  {
    cs_tx_buffer.p_buffer = cmd_buff;
    cs_tx_buffer.buffer_size = cmd_length;

    cs_rx_buffer.p_buffer = rsp_buf;
    cs_rx_buffer.max_buffer_size = rsp_length;
    cs_return = osCS_ComMdm_transaction(&cs_tx_buffer, &cs_rx_buffer, command_err_code);

    if (cs_return == CELLULAR_OK)
    {
      error = COM_ERR_OK;
    }
    else
    {
      error = COM_ERR_GENERAL;
    }
  }
  return (error);
}

/**
  * @brief  read message from modem to the rsp buffer provided by the application.
  * @note
  * @param[in]  handle           - the mdm handle to use, given by com_mdm_open
  * @param[in]  resp             - the string response received
  * @param[in]  length           - the length of the resp buffer
  * @param[out] command_err_code - the error code returned by the command
  * @retval - error code
  * @note   command sent correctly when error code is COM_ERR_OK
  *         message is not receive correctly when error code is COM_ERR_GENERAL
  *         handle is unknown when error code is COM_ERR_DESCRIPTOR
  */
com_err_t com_mdm_receive(uint8_t handle, com_char_t *resp, uint32_t length, int32_t *command_err_code)
{
  com_err_t error = COM_ERR_DESCRIPTOR;
  CS_Status_t cs_return;
  CS_Rx_Buffer_t cs_rx_buffer;

  if ((handle < 1U) && (com_mdm_desc.handle == COM_MDM_USED))
  {
    cs_rx_buffer.p_buffer = resp;
    cs_rx_buffer.max_buffer_size = length;

    cs_return = osCS_ComMdm_receive(&cs_rx_buffer, command_err_code);

    if (cs_return == CELLULAR_OK)
    {
      error = COM_ERR_OK;
    }
    else
    {
      error = COM_ERR_GENERAL;
    }
  }
  return (error);
}

/**
  * @brief  close mdm session, and release mdm handle
  * @note   after a call to com_mdm_close no call to any com_mdm_* function should be done
  * @param  - the mdm handle to be released
  * @retval - error code
  * @note   Handle is correctly released when error code is COM_ERR_OK
  *         handle can't be reseased when error code is COM_ERR_DESCRIPTOR
  */
com_err_t com_mdm_close(uint8_t handle)
{
  com_err_t error = COM_ERR_DESCRIPTOR;
  CS_Status_t cs_return;

  if ((handle < 1U) && (com_mdm_desc.handle == COM_MDM_USED))
  {
    /* handle parameter is valid :          */
    /*  -not bigger than number session max */
    /*  -handle is actually used            */

    /* set handle status to not used */
    com_mdm_desc.handle = COM_MDM_NOT_USED;
    if (com_mdm_app_cb != NULL)
    {
      /* a callback is registered, unsubscribe com_mdm callback to MDM URC event */
      cs_return = osCS_ComMdm_subscribe_event(NULL);
      if (cs_return == CELLULAR_OK)
      {
        /* reset the stored application callback */
        com_mdm_app_cb = NULL;
        error = COM_ERR_OK;
      }
      else
      {
        error = COM_ERR_GENERAL;
      }
    }
    else
    {
      error = COM_ERR_OK;
    }
  }
  return (error);
}

/*** Component Initialization/Start *******************************************/
/*** Used by com_core module - Not an User Interface **************************/

/**
  * @brief  Component initialization
  * @note   must be called only one time :
  *         - before using any other functions of com_mdm*
  * @param  -
  * @retval bool      - true/false init ok/nok
  */
void com_mdm_init(void)
{
  /* Initialize com_mdm descriptor */
  com_mdm_desc.handle = COM_MDM_NOT_USED;
  /* Initialize registered application callback */
  com_mdm_app_cb = NULL;
}

/**
  * @brief  Component start
  * @note   must be called only one time:
            - after com_mdm_init
            - and before using any other functions of com_mdm_*
  * @param  -
  * @retval -
  */
void com_mdm_start(void)
{
  __NOP();
}

#endif /* defined(USE_COM_MDM) */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
