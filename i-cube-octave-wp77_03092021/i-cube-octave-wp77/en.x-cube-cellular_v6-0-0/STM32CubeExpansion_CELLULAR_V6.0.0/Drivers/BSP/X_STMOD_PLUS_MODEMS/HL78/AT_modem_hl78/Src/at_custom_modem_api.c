/**
  ******************************************************************************
  * @file    at_custom_modem_api.c
  * @author  Sierra Wireless Inc. and Affiliates
  * @brief   This file provides all the specific code to the
  *          HL78 Sierra Wireless modem: LTE-cat-M1 or LTE-cat.NB1(=NB-IOT) or GSM
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
#include <string.h>
#include "at_custom_modem_api.h"
#include "at_custom_modem_specific.h"
#include "sysctrl_specific.h"
#include "plf_config.h"

/* HL78 COMPILATION FLAGS to define in project option if needed:
*
*/

/* Private typedef -----------------------------------------------------------*/

/* Private defines -----------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Global variables ----------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

/* Functions Definition ------------------------------------------------------*/
void atcma_init_at_func_ptrs(atcustom_funcPtrs_t *funcPtrs)
{
#if defined(USE_MODEM_HL78)
  /* init function pointers with HL78 functions */
  funcPtrs->f_init = ATCustom_HL78_init;
  funcPtrs->f_checkEndOfMsgCallback = ATCustom_HL78_checkEndOfMsgCallback;
  funcPtrs->f_getCmd = ATCustom_HL78_getCmd;
  funcPtrs->f_extractElement = ATCustom_HL78_extractElement;
  funcPtrs->f_analyzeCmd = ATCustom_HL78_analyzeCmd;
  funcPtrs->f_analyzeParam = ATCustom_HL78_analyzeParam;
  funcPtrs->f_terminateCmd = ATCustom_HL78_terminateCmd;
  funcPtrs->f_get_rsp = ATCustom_HL78_get_rsp;
  funcPtrs->f_get_urc = ATCustom_HL78_get_urc;
  funcPtrs->f_get_error = ATCustom_HL78_get_error;
  funcPtrs->f_hw_event = ATCustom_HL78_hw_event;
#else
#error AT custom does not match with selected modem
#endif /* USE_MODEM_HL78 */
}

void atcma_init_sysctrl_func_ptrs(sysctrl_funcPtrs_t *funcPtrs)
{
#if defined(USE_MODEM_HL78)
  /* init function pointers with HL78 functions */
  funcPtrs->f_getDeviceDescriptor = SysCtrl_HL78_getDeviceDescriptor;
  funcPtrs->f_open_channel =  SysCtrl_HL78_open_channel;
  funcPtrs->f_close_channel =  SysCtrl_HL78_close_channel;
  funcPtrs->f_power_on =  SysCtrl_HL78_power_on;
  funcPtrs->f_power_off = SysCtrl_HL78_power_off;
  funcPtrs->f_reset_device = SysCtrl_HL78_reset;
  funcPtrs->f_sim_select = SysCtrl_HL78_sim_select;
#else
#error SysCtrl does not match with selected modem
#endif /* USE_MODEM_HL78 */
}

/************************ (C) COPYRIGHT Sierra Wireless *****END OF FILE****/
