/**
  ******************************************************************************
  * @file    plf_cellular_app_sensors_thread_config.h
  * @author  MCD Application Team
  * @brief   Cellular Application Sensors thread configuration
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
#ifndef PLF_CELLULAR_APP_SENSORS_THREAD_CONFIG_H
#define PLF_CELLULAR_APP_SENSORS_THREAD_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif


/* Includes ------------------------------------------------------------------*/
#include "plf_features.h"

/* Exported constants --------------------------------------------------------*/
/* ======================================= */
/* BEGIN -  CellularApp specific defines   */
/* ======================================= */

#if (USE_CELLULAR_APP == 1)

/* SensorsClt parameters */
#if (USE_SENSORS == 1)
#define SENSORSCLIENT_THREAD_NUMBER                   (1U)   /* Number of ui client created = 1U */
#define SENSORSCLIENT_THREAD_STACK_SIZE               (512U) /* Thread stack size per sensors client instance */
#define SENSORSCLIENT_THREAD_PRIO                     osPriorityNormal
#else /* USE_SENSORS == 0 */
#define SENSORSCLIENT_THREAD_NUMBER                   (0U)   /* Number of ui client created = 1U */
#define SENSORSCLIENT_THREAD_STACK_SIZE               (0U) /* Thread stack size per ui client instance */
#endif /* USE_SENSORS == 1 */

/* CellularApp queue size per queue */
#define CELLULAR_APP_QUEUE_SIZE                  (5U)

/* Number of threads created by CellularApp */
#define APPLICATION_THREAD_NUMBER                (SENSORSCLIENT_THREAD_NUMBER)

/* Application thread stack size: define the stack size needed by CellularApp */
#define APPLICATION_THREAD_STACK_SIZE            ((SENSORSCLIENT_THREAD_STACK_SIZE) * (SENSORSCLIENT_THREAD_NUMBER))

/* Application partial heap size: define the partial heap size needed by CellularApp */
/*
 * Partial Heap used for: RTOS Timer/Mutex/Semaphore/Message objects and extra pvPortMalloc call
 * Mutex/Semaphore # 88 bytes
 * Queue           # 96 bytes + (max nb of elements * sizeof(uint32_t))
 * Thread          #104 bytes
 * Timer           # 56 bytes
 */
/* CellularApp Partial Heap:
 * 1 Mutex/Semaphore
 * 1 Queue per APPLICATION_THREAD_STACK_SIZE - for each queue : CELLULAR_APP_QUEUE_SIZE elements
 * APPLICATION_THREAD_NUMBER Threads
 * 0 Timer
 * APPLICATION_PARTIAL_HEAP_SIZE :
 * 88 * 1                                                             # 100
 * + (96 + (CELLULAR_APP_QUEUE_SIZE * 4U))* APPLICATION_THREAD_NUMBER # 350
 * + (104 * APPLICATION_THREAD_NUMBER)                                # 350
 * + (56 * 0)                                                         #   0
 *                                                                    # 800
 */

#define APPLICATION_PARTIAL_HEAP_SIZE     (100U \
                                           +((100U + (CELLULAR_APP_QUEUE_SIZE * 4U)) * APPLICATION_THREAD_NUMBER) \
                                           +(110U * APPLICATION_THREAD_NUMBER))


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

#endif /* PLF_CELLULAR_APP_SENSORS_THREAD_CONFIG_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
