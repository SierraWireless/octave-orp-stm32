/**
  ******************************************************************************
  * @file    com_icc.h
  * @author  MCD Application Team
  * @brief   Header for com_icc.c module
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
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef COM_MDM_H
#define COM_MDM_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "plf_config.h"

#if defined(USE_COM_MDM)

#include "com_common.h"
#include "com_err.h"

/** @addtogroup COM
  * @{
  */

/**
  ******************************************************************************
  @verbatim
  ==============================================================================
                    ##### How to use Com MDM interface #####
  ==============================================================================
  Allows to communicate with the Octave

  in Application, call:
  1) com_icc to obtain a icc session handle
  2) com_icc_generic_access for each command to send to the ICC
  3) com_closeicc to close the icc session

  At the moment only one session at a time is possible.
  This mechanism prevent concurrent access to the ICC
  e.g:
  Application1 request a Select on Master File, then a Select on a 1st level Dedicated File
  but before to have time to interact on a Elementary File in this Dedicated File,
  Application2 request a Select on a different Dedicated File

  To use this module define USE_COM_ICC must be set to 1

  @endverbatim

  */

/** @defgroup COM_MDM
  * @{
  */


/* Exported constants --------------------------------------------------------*/

#define COM_MDM_HANDLE_ERROR   0xFFU /* MDM error handle */

/* Exported types ------------------------------------------------------------*/

typedef void (* com_mdm_urc_callback_t)(void);

/* External variables --------------------------------------------------------*/
/* None */

/* Exported macros -----------------------------------------------------------*/
/* None */

/* Exported functions ------------------------------------------------------- */
/**
  ******************************************************************************
  @verbatim
  ==============================================================================
                    ##### How to use Com MDM interface #####
  ==============================================================================
  Allows to communicate with the ICC

  in Application, call:
  1) com_icc to obtain a icc session handle
  2) com_icc_generic_access for each command to send to the ICC
  3) com_closeicc to close the icc session

  At the moment only one session at a time is possible.
  This mechanism prevent concurrent access to the ICC
  e.g:
  Application1 request a Select on Master File, then a Select on a 1st level Dedicated File
  but before to have time to interact on a Elementary File in this Dedicated File,
  Application2 request a Select on a different Dedicated File

  To use this module define USE_COM_ICC must be set to 1

  @endverbatim

  */

/** @defgroup COM_MDM_Functions Functions
  * @{
  */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/** @addtogroup COM_MDM_Functions
  * @{
  */


/*** MDM functionalities ****************************************************/

/**
  * @brief  register callback for mdm urc
  * @note   register a function as callback for mdm urc receive from modem
  * @param  callback - The call back to be registered
  * @retval -
  * @note   the provided call back function should execute a minimum of code.
  *         Application should create an event or message to trigger a receive of a message to be treated later
  */
com_err_t com_mdm_subscribe_event(uint8_t handle, com_mdm_urc_callback_t callback);

/**
  * @brief  open mdm session, and return mdm handle
  * @note   com_mdm_open has to be called before using any com_mdm_* function
  * @param  -
  * @retval - the mdm handle
  * @note   a handle value >=0 is a valid handle value
  *         a handle value <0 represents an error : no more handle availables. Using a handle <0 in other com_mdm_*
  *         functions will result as an error.
  */
uint8_t com_mdm_open(void);

/**
  * @brief  sends a MDM command to the Modem.
  * @note
  * @param[in]  handle           - the mdm handle to use, given by com_mdm_open
  * @param[in]  command          - the string command to be send
  * @param[in]  length           - the length of the command to be send
  * @param[out] command_err_code - the error code returned by the command
  * @retval - error code
  * @note   command sent correctly when error code is COM_ERR_OK
  *         command not sent correctly when error code is COM_ERR_GENERAL
  *         handle is unknown when error code is COM_ERR_DESCRIPTOR
  */
com_err_t com_mdm_send(uint8_t handle, com_char_t *command, uint32_t length, int32_t *command_err_code);

/**
  * @brief  initiate a full transaction (send + answer receive) for a MDM command to the Modem.
  * @note
  * @param[in]  handle           - the mdm handle to use, given by com_mdm_open
  * @param[in]  cmd_buff         - the string command to be send
  * @param[in]  cmd_length       - the length of the command to be send
  * @param[in]  rsp_buff         - the string response received
  * @param[in]  rsp_length       - the length of the response received
  * @param[out] command_err_code - the error code returned by the command
  * @retval - error code
  * @note   command sent correctly when error code is COM_ERR_OK
  *         command not sent correctly when error code is COM_ERR_GENERAL
  *         handle is unknown when error code is COM_ERR_DESCRIPTOR
  */
com_err_t com_mdm_transaction(uint8_t handle, com_char_t *cmd_buff, uint32_t cmd_length,
                              com_char_t *rsp_buf, uint32_t rsp_length, int32_t *command_err_code);

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
com_err_t com_mdm_receive(uint8_t handle, com_char_t *resp, uint32_t length, int32_t *command_err_code);

/**
  * @brief  close mdm session, and release mdm handle
  * @note   after a call to com_mdm_close no call to any com_mdm_* function should be done
  * @param  - the mdm handle to be released
  * @retval - error code
  * @note   Handle is correctly released when error code is COM_ERR_OK
  *         handle can't be reseased when error code is COM_ERR_DESCRIPTOR
  */
com_err_t com_mdm_close(uint8_t handle);

/*** Component Initialization/Start *******************************************/
/*** Used by com_core module - Not an User Interface **************************/

/**
  * @brief  Component initialization
  * @note   must be called only one time :
  *         - before using any other functions of com_mdm*
  * @param  -
  * @retval bool      - true/false init ok/nok
  */
void com_mdm_init(void);

/**
  * @brief  Component start
  * @note   must be called only one time:
            - after com_mdm_init
            - and before using any other functions of com_mdm_*
  * @param  -
  * @retval -
  */
void com_mdm_start(void);

#endif /* defined(USE_COM_MDM) */

#ifdef __cplusplus
}
#endif

#endif /* COM_MDM_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
