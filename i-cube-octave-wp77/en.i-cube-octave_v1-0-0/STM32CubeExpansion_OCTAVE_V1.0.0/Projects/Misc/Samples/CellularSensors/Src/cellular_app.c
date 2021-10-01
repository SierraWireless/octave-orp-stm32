/**
  ******************************************************************************
  * @file    cellular_app.c
  * @author  MCD Application Team
  * @brief   Cellular Application :
  *          - Create and Manage 1 instance of SensorsClt
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

/* Includes ------------------------------------------------------------------*/
#include "plf_config.h"

#if (USE_CELLULAR_APP == 1)
#include <string.h>
#include <stdbool.h>

#include "cellular_app.h"
#include "cellular_app_trace.h"

#if (USE_SENSORS == 1)
#include "cellular_app_sensorsclient.h"
#endif /* USE_SENSORS == 1 */

#include "rtosal.h"

#include "cellular_control_api.h"
#include "cellular_runtime_custom.h"

/* Private typedef -----------------------------------------------------------*/

/* Private defines -----------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Trace shortcut */
static const uint8_t *p_cellular_app_trace;

/* Status of Modem */
static bool cellular_app_data_is_ready; /* false/true: data is not ready/data is ready */

/* Global variables ----------------------------------------------------------*/

#if defined(USE_COM_MDM)
uint8_t currentHandle;
#endif /* defined(USE_COM_MDM) */

/* String used to display application type */
const uint8_t *cellular_app_type_string[CELLULAR_APP_TYPE_MAX] =
{
  (uint8_t *)"CellularApp",
  (uint8_t *)"Sensorsclt"
};

/* Private functions prototypes ----------------------------------------------*/
/* Callback for IP info: if (IP !=0U) then data is ready */
static void cellular_app_ip_info_cb(ca_event_type_t event_type, const cellular_ip_info_t *const p_ip_info,
                                    void *const p_callback_ctx);

static void cellular_app_propagate_info(ca_event_type_t event_type);

/* Public  functions  prototypes ---------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/**
  * @brief  Callback called when IP info changed
  * @note   Used to know when IP info change => provide when Modem is not ready/ready to transmit
  * @param  event_type     - Event that happened: CA_IP_INFO_EVENT.
  * @param  p_ip_info      - The new IP information.
  * @param  p_callback_ctx - The p_callback_ctx parameter in cellular_ip_info_changed_registration function.
  * @retval -
  */
static void cellular_app_ip_info_cb(ca_event_type_t event_type, const cellular_ip_info_t *const p_ip_info,
                                    void *const p_callback_ctx)
{
  UNUSED(p_callback_ctx);

  /* Event to know Modem status ? */
  if ((event_type == CA_IP_INFO_EVENT)  && (p_ip_info != NULL))
  {
    /* If IP address is not null then it means Data is ready */
    if (p_ip_info->ip_addr.addr != 0U)
    {
      /* Data is ready */
      if (cellular_app_is_data_ready() == false)
      {
        /* Modem is ready */
        PRINT_FORCE("%s: Modem ready to transmit data", p_cellular_app_trace)
        cellular_app_data_is_ready = true;
        cellular_app_propagate_info(CA_IP_INFO_EVENT);
      }
      /* else: nothing to do because data ready status already known */
    }
    else
    {
      if (cellular_app_is_data_ready() == true)
      {
        /* Modem is not ready */
        PRINT_FORCE("%s: Modem NOT ready to transmit data!", p_cellular_app_trace)
        cellular_app_data_is_ready = false;
      }
      /* else: nothing to do because data ready status already known */
    }
  }
}

/**
  * @brief  Send to all applications a message
  * @param  event_type - event type
  * @retval -
  */
static void cellular_app_propagate_info(ca_event_type_t event_type)
{
  UNUSED(event_type);
  uint32_t queue_msg = 0U;

  /* if (event_type == CA_IP_INFO_EVENT) */
  SET_CELLULAR_APP_MSG_TYPE(queue_msg, CELLULAR_APP_PROCESS_MSG);
  SET_CELLULAR_APP_MSG_ID(queue_msg, CELLULAR_APP_MODEM_CHANGE_ID);
#if (USE_SENSORS == 1)
  (void)cellular_app_sensorsclient_send_msg(queue_msg);
#endif /* USE_SENSORS == 1 */
}

/* Functions Definition ------------------------------------------------------*/
/**
  * @brief  Get data status
  * @retval bool - false/true - data is not ready to transmit / data is ready
  */
bool cellular_app_is_data_ready(void)
{
  return (cellular_app_data_is_ready);
}

/**
  * @brief  Initialize all needed structures to support CellularApp features and call Cellular init
  * @param  -
  * @retval -
  */
void application_init(void)
{
  /**** BEGIN Cellular Application initialization ****/
  /* Initialize trace shortcut */
  p_cellular_app_trace = cellular_app_type_string[CELLULAR_APP_TYPE_CELLULARAPP];

  /* Initialize data ready status to false */
  cellular_app_data_is_ready = false;

#if (USE_SENSORS == 1)
  /* SensorsClt initialization */
  cellular_app_sensorsclient_init();
#endif /* USE_SENSORS == 1 */
  /**** END   Cellular Application initialization ****/

  /**** BEGIN Cellular initialization ****/
  cellular_init();
  /**** END   Cellular initialization ****/

#if defined(USE_COM_MDM)
  com_err_t com_err;
  currentHandle = orp_open();
  com_err = orp_subscribe_event(currentHandle,orp_callback);
  //com_err = com_mdm_subscribe_event(currentHandle, application_callback);
  PRINT_FORCE("Subscription to ORP callback handler returned %ld",com_err)
#endif /* defined(USE_COM_MDM) */
}

/**
  * @brief  Start all threads needed to activate CellularApp features and call Cellular start
  * @param  -
  * @retval -
  */
void application_start(void)
{
  /**** BEGIN Cellular Application start ****/
  /* Cellular initialization already done - Registration to services is OK */
  /* Registration to Cellular: only needs to know when IP is obtained */
  if (cellular_ip_info_cb_registration(cellular_app_ip_info_cb, (void *) NULL) != CELLULAR_SUCCESS)
  {
    CELLULAR_APP_ERROR(CELLULAR_APP_ERROR_CELLULARAPP, ERROR_FATAL)
  }

#if (USE_SENSORS == 1)
  /* SensorsClt start */
  cellular_app_sensorsclient_start();
#endif /* USE_SENSORS == 1 */
  /**** END   Cellular Application start ****/

  /**** BEGIN Cellular start ****/
  cellular_start();
  /**** END   Cellular start ****/

}

#endif /* USE_CELLULAR_APP == 1 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
