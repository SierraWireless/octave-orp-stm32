/**
  ******************************************************************************
  * @file    plf_cellular_app_sensors_config.h
  * @author  MCD Application Team
  * @brief   Cellular Application Sensors features configuration
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef PLF_CELLULAR_APP_SENSORS_CONFIG_H
#define PLF_CELLULAR_APP_SENSORS_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/

/* Overwrites X-Cube-Cellular features  */

/* ======================================= */
/* BEGIN -  Miscellaneous functionalities  */
/* ======================================= */

/* To configure some parameters of the software */
#if !defined USE_CMD_CONSOLE
#define USE_CMD_CONSOLE                          (1) /* 0: not activated, 1: activated */
#endif /* !defined USE_CMD_CONSOLE */

/* To include RTC service */
#if !defined USE_RTC
#define USE_RTC                                  (0)   /* 0: not activated, 1: activated */
#endif /* !defined USE_RTC */

#if !defined SW_DEBUG_VERSION
#define SW_DEBUG_VERSION                         (1U)  /** 0 for SW release version (no traces),
                                                         * 1 for SW debug version */


#endif /* !defined SW_DEBUG_VERSION */

/* ======================================= */
/* END   -  Miscellaneous functionalities  */
/* ======================================= */

/* Exported types ------------------------------------------------------------*/
/* External variables --------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

/* ======================================= */
/* BEGIN -  CellularApp specific defines   */
/* ======================================= */

/* if USE_CELLULAR_APP is set then CellularApp is activated
   if not then plf_cellular_app_config.h is just an overwrite of Cellular Platform configuration */
#define USE_CELLULAR_APP                         (1)   /* 0: not activated, 1: activated */

#if (USE_CELLULAR_APP == 1)

/* To use a Terminal to interact with CellularApp through CMD module */
/* 0U: No usage of command module registration - XU: X command module registrations */
#if (USE_CMD_CONSOLE == 1)
#define APPLICATION_CMD_NB                       (0U)  /* no interaction with CellularApp */
#endif /* USE_CMD_CONSOLE == 1 */

/* To register callback in order to receive Cellular status modification by CellularApi */
#define CELLULAR_CONCURENT_REGISTRATION_CB_MAX_NB (1U)  /* 1U: CellularApp : cellular_ip_info
                                                               CellularSensor : cellular_info
                                                               registration to different service so 1U is OK */

/* Because some traces in CellularApp are using PRINT_FORCE, so next define can not be deactivated */
#define USE_DBG_CHAN_APPLICATION                 (1U)  /* To access to cellular system trace */

/* Active or not the debug trace in CellularApp */
#if (SW_DEBUG_VERSION == 1U)
#define USE_TRACE_APPLICATION                    (1U)  /* 1: CellularApp trace activated   */
#else  /* SW_DEBUG_VERSION == 0U */
#define USE_TRACE_APPLICATION                    (0U)  /* 0: CellularApp trace deactivated */
#endif /* SW_DEBUG_VERSION == 1U */

#if !defined USE_DISPLAY
#define USE_DISPLAY    (0)  /* 0: not activated, 1: activated */
#endif /* !defined USE_DISPLAY */
#if !defined USE_ST33
#define USE_ST33       (0)  /* 0: not activated, 1: activated */
#endif /* !defined USE_ST33 */
#if !defined USE_SENSORS
#define USE_SENSORS    (1)  /* 0: not activated, 1: activated */
#endif /* !defined USE_SENSORS */

/* ======================================= */
/* END   -  CellularApp specific defines   */
/* ======================================= */

/* Exported types ------------------------------------------------------------*/
/* External variables --------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

#endif /* USE_CELLULAR_APP == 1 */

#ifdef __cplusplus
}
#endif

#endif /* PLF_CELLULAR_APP_SENSORS_CONFIG_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
