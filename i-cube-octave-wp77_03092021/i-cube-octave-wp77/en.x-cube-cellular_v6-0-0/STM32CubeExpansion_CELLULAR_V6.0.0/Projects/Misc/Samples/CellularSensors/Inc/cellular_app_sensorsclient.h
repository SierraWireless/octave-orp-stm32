/**
  ******************************************************************************
  * @file    cellular_app_sensorsclient.h
  * @author  MCD Application Team
  * @brief   Header for cellular_app_sensorsclient.c module
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
#ifndef CELLULAR_APP_SENSORSCLIENT_H
#define CELLULAR_APP_SENSORSCLIENT_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "plf_config.h"

#if (USE_SENSORS == 1)

/* Exported constants --------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* External variables --------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

/**
  * @brief  Send a message to SensorsClt
  * @param  queue_msg - Message to send
  * @retval bool      - false/true - Message not send / Message send
  */
bool cellular_app_sensorsclient_send_msg(uint32_t queue_msg);

/**
  * @brief  Initialize all needed structures to support SensorsClt feature
  * @param  -
  * @retval -
  */
void cellular_app_sensorsclient_init(void);

/**
  * @brief  Start SensorsClt thread
  * @param  -
  * @retval -
  */
void cellular_app_sensorsclient_start(void);

#endif /* USE_SENSORS == 1 */

#ifdef __cplusplus
}
#endif

#endif /* CELLULAR_APP_SENSORSCLIENT_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
