/**
  ******************************************************************************
  * @file    sysctrl_specific.c
  * @author  Sierra Wireless Inc. and Affiliates
  * @brief   This file provides all the specific code for System control of
  *          WP77 Sierra Wireless modem
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
#include "sysctrl.h"
#include "sysctrl_specific.h"
#include "ipc_common.h"
#include "plf_config.h"

/* Private typedef -----------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/
#if (USE_TRACE_SYSCTRL == 1U)
#if (USE_PRINTF == 0U)
#include "trace_interface.h"
#define PRINT_FORCE(format, args...) TRACE_PRINT_FORCE(DBG_CHAN_ATCMD, DBL_LVL_P0, format "\n\r", ## args)
#define PRINT_INFO(format, args...) TRACE_PRINT(DBG_CHAN_ATCMD, DBL_LVL_P0, "SysCtrl_WP77:" format "\n\r", ## args)
#define PRINT_ERR(format, args...)  TRACE_PRINT(DBG_CHAN_ATCMD, DBL_LVL_ERR,\
                                                "SysCtrl_WP77 ERROR:" format "\n\r", ## args)
#else
#include <stdio.h>
#define PRINT_FORCE(format, args...) (void) printf(format "\n\r", ## args);
#define PRINT_INFO(format, args...)  (void) printf("SysCtrl_WP77:" format "\n\r", ## args);
#define PRINT_ERR(format, args...)   (void) printf("SysCtrl_WP77 ERROR:" format "\n\r", ## args);
#endif /* USE_PRINTF */

#else
#if (USE_PRINTF == 0U)
#include "trace_interface.h"
#define PRINT_FORCE(format, args...) TRACE_PRINT_FORCE(DBG_CHAN_ATCMD, DBL_LVL_P0, format "\n\r", ## args)
#else
#include <stdio.h>
#define PRINT_FORCE(format, args...) (void) printf(format "\n\r", ## args);
#endif /* USE_PRINTF */
#define PRINT_INFO(...) __NOP(); /* Nothing to do */
#define PRINT_ERR(...)  __NOP(); /* Nothing to do */
#endif /* USE_TRACE_SYSCTRL */

/* Private defines -----------------------------------------------------------*/
#define WP77_BOOT_TIME (30000U) /* Time in ms allowed to complete the modem boot procedure
                                *  according to spec, time = 20 sec
                                *  but practically, it seems that about 25 sec is acceptable
                                */

/* Private variables ---------------------------------------------------------*/

/* Global variables ----------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
static sysctrl_status_t SysCtrl_WP77_setup(void);

/* Functions Definition ------------------------------------------------------*/
sysctrl_status_t SysCtrl_WP77_getDeviceDescriptor(sysctrl_device_type_t type, sysctrl_info_t *p_devices_list)
{
  sysctrl_status_t retval;

  if (p_devices_list == NULL)
  {
    retval = SCSTATUS_ERROR;
  }
  else
  {
    /* check type */
    if (type == DEVTYPE_MODEM_CELLULAR)
    {
#if defined(USE_MODEM_WP77)
      p_devices_list->type          = DEVTYPE_MODEM_CELLULAR;
      p_devices_list->ipc_device    = USER_DEFINED_IPC_DEVICE_MODEM;
      p_devices_list->ipc_interface = IPC_INTERFACE_UART;

      (void) IPC_init(p_devices_list->ipc_device, p_devices_list->ipc_interface, &MODEM_UART_HANDLE);
      retval = SCSTATUS_OK;
#else
      retval = SCSTATUS_ERROR
#endif /* USE_MODEM_WP77 */
    }
    else
    {
      retval = SCSTATUS_ERROR;
    }
  }

  if (retval != SCSTATUS_ERROR)
  {
    (void) SysCtrl_WP77_setup();
  }

  return (retval);
}


sysctrl_status_t SysCtrl_WP77_open_channel(sysctrl_device_type_t type)
{
  UNUSED(type);
  sysctrl_status_t retval = SCSTATUS_OK;

  /* UART configuration */
  MODEM_UART_HANDLE.Instance = MODEM_UART_INSTANCE;
  MODEM_UART_HANDLE.Init.BaudRate = MODEM_UART_BAUDRATE;
  MODEM_UART_HANDLE.Init.WordLength = MODEM_UART_WORDLENGTH;
  MODEM_UART_HANDLE.Init.StopBits = MODEM_UART_STOPBITS;
  MODEM_UART_HANDLE.Init.Parity = MODEM_UART_PARITY;
  MODEM_UART_HANDLE.Init.Mode = MODEM_UART_MODE;
  MODEM_UART_HANDLE.Init.HwFlowCtl = MODEM_UART_HWFLOWCTRL;
  MODEM_UART_HANDLE.Init.OverSampling = UART_OVERSAMPLING_16;
  MODEM_UART_HANDLE.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  /* do not activate autobaud (not compatible with current implementation) */
  MODEM_UART_HANDLE.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;

  /* UART initialization */
  if (HAL_UART_Init(&MODEM_UART_HANDLE) != HAL_OK)
  {
    PRINT_ERR("HAL_UART_Init error")
    retval = SCSTATUS_ERROR;
  }

  /* Enable the UART IRQn */
  HAL_NVIC_EnableIRQ(MODEM_UART_IRQN);

  return (retval);
}

sysctrl_status_t SysCtrl_WP77_close_channel(sysctrl_device_type_t type)
{
  UNUSED(type);
  sysctrl_status_t retval = SCSTATUS_OK;

  /* Disable the UART IRQn */
  HAL_NVIC_DisableIRQ(MODEM_UART_IRQN);

  /* UART deinitialization */
  if (HAL_UART_DeInit(&MODEM_UART_HANDLE) != HAL_OK)
  {
    PRINT_ERR("HAL_UART_DeInit error")
    retval = SCSTATUS_ERROR;
  }

  return (retval);
}

sysctrl_status_t SysCtrl_WP77_power_on(sysctrl_device_type_t type)
{
  UNUSED(type);
  sysctrl_status_t retval = SCSTATUS_OK;
  PRINT_INFO("!!! Modem Powering On !!!")

  /* Reference: Sierra Wireless WP77 Hardware Design V1.4
  *  PWRKEY   connected to MODEM_PWR_EN (inverse pulse)
  *  RESET_N  connected to MODEM_RST    (inverse)
  */

  /* Set PWR_EN to 1  */
  HAL_GPIO_WritePin(MODEM_PWR_EN_GPIO_PORT, MODEM_PWR_EN_PIN, GPIO_PIN_SET);


  /*Set RST pin to Low for 200 Milliseconds and then to high*/
  HAL_GPIO_WritePin(MODEM_RST_GPIO_PORT, MODEM_RST_PIN, GPIO_PIN_RESET);
  SysCtrl_delay(200U);

  /* Set RST_PIN to 1  */
  HAL_GPIO_WritePin(MODEM_RST_GPIO_PORT, MODEM_RST_PIN, GPIO_PIN_SET);

  /* wait for Modem to complete its booting procedure */
  PRINT_INFO("Waiting %d millisec for modem running...", WP77_BOOT_TIME)

  SysCtrl_delay(WP77_BOOT_TIME);
  PRINT_INFO("...done")

  return (retval);
}

sysctrl_status_t SysCtrl_WP77_power_off(sysctrl_device_type_t type)
{
  UNUSED(type);
  sysctrl_status_t retval = SCSTATUS_OK;
  PRINT_INFO("!!! Modem Power Off !!!")

  /* Reference: Sierra Wireless WP77 Hardware Design V1.4
  *  PWRKEY   connected to MODEM_PWR_EN (inverse pulse)
  *  RESET_N  connected to MODEM_RST    (inverse)
  *
  * Turn OFF module sequence
  *
  *                PWRKEY  PWR_EN  modem_state
  * init             1       0      RUNNING
  * T=0 ms           0       1      RUNNING
  * T1=650 ms        1       0      POWER-DOWN PROCEDURE
  * T2=T1+2000 ms    1       0      OFF
  *
  * It is also safe to use AT+QPOWD command which is similar to turning off the modem
  * via PWRKEY pin.
  */

  /* Set MODEM_RST_PIN to 0  */
  HAL_GPIO_WritePin(MODEM_PWR_EN_GPIO_PORT, MODEM_RST_PIN, GPIO_PIN_RESET);

  /* wait at least 5s before powering it Off */
  SysCtrl_delay(5000U);

  /* Set PWR_EN to 0  */
  HAL_GPIO_WritePin(MODEM_PWR_EN_GPIO_PORT, MODEM_PWR_EN_PIN, GPIO_PIN_RESET);

  return (retval);
}

sysctrl_status_t SysCtrl_WP77_reset(sysctrl_device_type_t type)
{
  UNUSED(type);
  sysctrl_status_t retval = SCSTATUS_OK;

  /* Reference: Sierra Wireless WP77 Hardware Design V1.4
  *  PWRKEY   connected to MODEM_PWR_EN (inverse pulse)
  *  RESET_N  connected to MODEM_RST    (inverse)
  *
  * Reset module sequence
  *
  * Can be done using RESET_N pin to low voltage for 100ms minimum
  *
  *          RESET_N  modem_state
  * init       1        RUNNING
  * T=0        0        OFF
  * T=150      1        BOOTING
  * T>=460     1        RUNNING
  */
  PRINT_INFO("!!! Hardware Reset triggered !!!")
  /* set RST to 1 for a time between 150ms and 460ms (200) */
  HAL_GPIO_WritePin(MODEM_RST_GPIO_PORT, MODEM_RST_PIN, GPIO_PIN_RESET);
  SysCtrl_delay(200U);
  HAL_GPIO_WritePin(MODEM_RST_GPIO_PORT, MODEM_RST_PIN, GPIO_PIN_SET);

  /* wait for Modem to complete its restart procedure */
  PRINT_INFO("Waiting %d millisec for modem running...", WP77_BOOT_TIME)
  SysCtrl_delay(WP77_BOOT_TIME);
  PRINT_INFO("...done")

  return (retval);
}

sysctrl_status_t SysCtrl_WP77_sim_select(sysctrl_device_type_t type, sysctrl_sim_slot_t sim_slot)
{
  UNUSED(type);
  sysctrl_status_t retval = SCSTATUS_OK;

  switch (sim_slot)
  {
    case SC_MODEM_SIM_SOCKET_0:
      HAL_GPIO_WritePin(MODEM_SIM_SELECT_0_GPIO_PORT, MODEM_SIM_SELECT_0_PIN, GPIO_PIN_RESET);
      HAL_GPIO_WritePin(MODEM_SIM_SELECT_1_GPIO_PORT, MODEM_SIM_SELECT_1_PIN, GPIO_PIN_RESET);
      PRINT_INFO("MODEM SIM SOCKET SELECTED")
      break;

    case SC_MODEM_SIM_ESIM_1:
      HAL_GPIO_WritePin(MODEM_SIM_SELECT_0_GPIO_PORT, MODEM_SIM_SELECT_0_PIN, GPIO_PIN_SET);
      HAL_GPIO_WritePin(MODEM_SIM_SELECT_1_GPIO_PORT, MODEM_SIM_SELECT_1_PIN, GPIO_PIN_RESET);
      PRINT_INFO("MODEM SIM ESIM SELECTED")
      break;

    case SC_STM32_SIM_2:
      HAL_GPIO_WritePin(MODEM_SIM_SELECT_0_GPIO_PORT, MODEM_SIM_SELECT_0_PIN, GPIO_PIN_RESET);
      HAL_GPIO_WritePin(MODEM_SIM_SELECT_1_GPIO_PORT, MODEM_SIM_SELECT_1_PIN, GPIO_PIN_SET);
      PRINT_INFO("STM32 SIM SELECTED")
      break;

    default:
      PRINT_ERR("Invalid SIM %d selected", sim_slot)
      retval = SCSTATUS_ERROR;
      break;
  }

  return (retval);
}

/* Private function Definition -----------------------------------------------*/
static sysctrl_status_t SysCtrl_WP77_setup(void)
{
  sysctrl_status_t retval = SCSTATUS_OK;

  GPIO_InitTypeDef GPIO_InitStruct;

  /* GPIO config
   * Initial pins state:
   *  PWR_EN initial state = 1 : used to power-on/power-off the board
   *  RST initial state = 0 : used to reset the board
   *  DTR initial state = 0 ; not used
   */
  HAL_GPIO_WritePin(MODEM_PWR_EN_GPIO_PORT, MODEM_PWR_EN_PIN, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(MODEM_RST_GPIO_PORT, MODEM_RST_PIN, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(MODEM_DTR_GPIO_PORT, MODEM_DTR_PIN, GPIO_PIN_RESET);

  /* Init GPIOs - common parameters */
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

  /* Init GPIOs - RESET pin */
  GPIO_InitStruct.Pin = MODEM_RST_PIN;
  HAL_GPIO_Init(MODEM_RST_GPIO_PORT, &GPIO_InitStruct);

  /* Init GPIOs - DTR pin */
  GPIO_InitStruct.Pin = MODEM_DTR_PIN;
  HAL_GPIO_Init(MODEM_DTR_GPIO_PORT, &GPIO_InitStruct);

  /* Init GPIOs - PWR_EN pin */
  GPIO_InitStruct.Pin = MODEM_PWR_EN_PIN;
  HAL_GPIO_Init(MODEM_PWR_EN_GPIO_PORT, &GPIO_InitStruct);

  PRINT_FORCE("WP77 UART config: BaudRate=%d / HW flow ctrl=%d", MODEM_UART_BAUDRATE,
              ((MODEM_UART_HWFLOWCTRL == UART_HWCONTROL_NONE) ? 0 : 1))

  return (retval);
}

/**
  * @brief  Request Modem to wake up from PSM.
  * @param  delay Delay (in ms) to apply before to request wake up (for some usecases).
  * @retval sysctrl_status_t
  */
sysctrl_status_t SysCtrl_WP77_wakeup_from_PSM(uint32_t delay)
{
  sysctrl_status_t retval = SCSTATUS_OK;

  /* Reference: Sierra Wireless WP77 Hardware Design V1.4
  *  PWRKEY   connected to MODEM_PWR_EN (inverse pulse)
  *  RESET_N  connected to MODEM_RST    (inverse)
  *
  * wake up from PSM sequence : Drive PWRKEY pin to low level will wake up the module.
  *                             ie drive MODEM_PWR_EN to high level
  */

  SysCtrl_delay(delay);
  HAL_GPIO_WritePin(MODEM_PWR_EN_GPIO_PORT, MODEM_PWR_EN_PIN, GPIO_PIN_SET);
  SysCtrl_delay(200U);
  HAL_GPIO_WritePin(MODEM_PWR_EN_GPIO_PORT, MODEM_PWR_EN_PIN, GPIO_PIN_RESET);

  return (retval);
}

/************************ (C) COPYRIGHT Sierra Wireless *****END OF FILE****/

