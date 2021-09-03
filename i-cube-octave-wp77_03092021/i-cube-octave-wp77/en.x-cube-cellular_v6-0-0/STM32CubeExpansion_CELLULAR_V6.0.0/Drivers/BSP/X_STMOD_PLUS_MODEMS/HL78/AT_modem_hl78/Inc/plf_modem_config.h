/**
  ******************************************************************************
  * @file    plf_modem_config.h
  * @author  Sierra Wireless Inc. and Affiliates
  * @brief   Header for plf_modem_config.h module for HL78
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
#ifndef PLF_MODEM_CONFIG_H
#define PLF_MODEM_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/
/* You can specify in project configuration the modem and hardware interface used.
*  If this is not specified, default configuration is specified below.
*/
#if defined(HWREF_B_CELL_HL78_V2)
/* already explicitly defined:
 * using HWREF_B_CELL_HL78_V2 directly on STMOD+ connector
 */
#else
/* set default config */
#define HWREF_B_CELL_HL78_V2
#endif /* HWREF_B_CELL_HL78_V2 */

/* Low Power */
#define ENABLE_HL78_LOW_POWER_MODE  USE_LOW_POWER

/* MODEM parameters */
#define USE_MODEM_HL78
#define CONFIG_MODEM_UART_BAUDRATE (115200U)
#define CONFIG_MODEM_USE_STMOD_CONNECTOR

#define UDP_SERVICE_SUPPORTED                (1U)
#define CONFIG_MODEM_UDP_SERVICE_CONNECT_IP  ((uint8_t *)"127.0.0.1")
#define CONFIG_MODEM_MAX_SOCKET_TX_DATA_SIZE ((uint32_t)1460U)
#define CONFIG_MODEM_MAX_SOCKET_RX_DATA_SIZE ((uint32_t)1500U)
#define CONFIG_MODEM_MAX_SIM_GENERIC_ACCESS_CMD_SIZE ((uint32_t)1460U)
#define CONFIG_MODEM_MIN_SIM_GENERIC_ACCESS_RSP_SIZE ((uint32_t)4U)

/* Ping URC received before or after Reply */
#define PING_URC_RECEIVED_AFTER_REPLY        (1U)

/* UART flow control settings */
#if defined(USER_FLAG_MODEM_FORCE_NO_FLOW_CTRL)

#define CONFIG_MODEM_UART_RTS_CTS  (0)
#elif defined(USER_FLAG_MODEM_FORCE_HW_FLOW_CTRL)
#define CONFIG_MODEM_UART_RTS_CTS  (1)
#else /* default FLOW CONTROL setting for HL78 */
#define CONFIG_MODEM_UART_RTS_CTS  (1)
#endif /* user flag for modem flow control */

/* At the end of the modem power on, this parameter defines whether we apply the theoretical delay to let the
 * modem start or if we try to establish communication immediately.
 *
 * HL78_FASTEST_POWER_ON : not supported by HL78
 */

/* Network Information */
#define HL78_OPTION_NETWORK_INFO    (1)  /* 1 if enabled, 0 if disabled */

/* Engineering Mode */
#define HL78_OPTION_ENGINEERING_MODE    (0)  /* 1 if enabled, 0 if disabled */

#ifdef __cplusplus
}
#endif

#endif /*_PLF_MODEM_CONFIG_H */

/************************ (C) COPYRIGHT Sierra Wireless *****END OF FILE****/
