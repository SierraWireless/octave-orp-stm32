/**
  ******************************************************************************
  * @file    cellular_app_sensorsclient.c
  * @author  MCD Application Team
  * @author  Sierra Wireless Team
  * @brief   SenorsClt Cellular Application :
  *          - Manage Sensors (initialization an periodical read)
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

#if (USE_SENSORS ==1)
#include <string.h>
#include <stdbool.h>

#include <math.h>

#include "cellular_app_sensorsclient.h"
#include "cellular_app.h"
#include "cellular_app_trace.h"
#include "cellular_app_sensors.h"

#include "cellular_control_api.h"

#include "rtosal.h"


/* Private typedef -----------------------------------------------------------*/
/* Timer State */
typedef uint8_t cellular_app_sensorsclient_timer_state_t;
#define CELLULAR_APP_SENSORSCLIENT_TIMER_INVALID       (cellular_app_sensorsclient_timer_state_t)0
#define CELLULAR_APP_SENSORSCLIENT_TIMER_IDLE          (cellular_app_sensorsclient_timer_state_t)1
#define CELLULAR_APP_SENSORSCLIENT_TIMER_RUN           (cellular_app_sensorsclient_timer_state_t)2

/* Private defines -----------------------------------------------------------*/
#define SENSORSCLIENT_CELLULAR_MSG                     ((CELLULAR_APP_VALUE_MAX_MSG) + (cellular_app_msg_type_t)1)
#define SENSORSCLIENT_TIMER_MSG                        ((CELLULAR_APP_VALUE_MAX_MSG) + (cellular_app_msg_type_t)2)
#define SENSORSCLIENT_TIMER_UPDATE_MSG                 ((CELLULAR_APP_VALUE_MAX_MSG) + (cellular_app_msg_type_t)3)
/* MSG id when MSG is SENSORSCLIENT_CELLULAR_MSG */
/* MSG id is Cellular info                   */
#define SENSORSCLIENT_CELLULAR_INFO_CHANGE_ID          ((CELLULAR_APP_VALUE_MAX_ID) + (cellular_app_msg_id_t)1)
/* MSG id when MSG is SENSORSCLIENT_TIMER_MSG    */
/* MSG id is Sensors read                   */
#define SENSORSCLIENT_SENSORS_READ_ID                  ((CELLULAR_APP_VALUE_MAX_ID) + (cellular_app_msg_id_t)2)
/* MSG id when MSG is SENSORSCLIENT_SENSORS_TIMER_UPDATE    */
/* MSG id is Sensors read                   */
#define SENSORSCLIENT_SENSORS_TIMER_UPDATE             ((CELLULAR_APP_VALUE_MAX_ID) + (cellular_app_msg_id_t)3)

#define ORP_IS_CONNECTED 							   "orp_isConnected" /*DONT MODIFY - Resource to check the status of ORP connection */

/* ORP Sensor Name (absolute path) declaration */
#define ORP_RESOURCE_SENSOR_PERIODICITY                "settings/period"
#define ORP_RESOURCE_SENSOR_JSON_ROOT                  "telemetry"
#define ORP_RESOURCE_SENSOR_TEMPERATURE                "telemetry/Temperature"
#define ORP_RESOURCE_SENSOR_PRESSURE                   "telemetry/Pressure"
#define ORP_RESOURCE_SENSOR_HUMIDITY                   "telemetry/Humidity"
#define ORP_RESOURCE_SENSOR_ACCELEROMETER_ROOT		   "telemetry/Accelerometer"
#define ORP_RESOURCE_SENSOR_ACCELEROMETER_AXIS_X       "telemetry/Accelerometer/AXIS_X"
#define ORP_RESOURCE_SENSOR_ACCELEROMETER_AXIS_Y       "telemetry/Accelerometer/AXIS_Y"
#define ORP_RESOURCE_SENSOR_ACCELEROMETER_AXIS_Z       "telemetry/Accelerometer/AXIS_Z"
#define ORP_RESOURCE_BOOLEAN_TEST				       "telemetry/boolean"

/* Private variables ---------------------------------------------------------*/
uint32_t SENSORSCLIENT_SENSORS_READ_TIMER = 10000; /* Unit: in ms. */
/* Trace shortcut */
static const uint8_t *p_cellular_app_sensorsclient_trace;
#if defined(USE_COM_MDM)
com_char_t orp_rspbuf[ORP_MAX_RSP_SIZE] = {0};
bool orp_defineResource = true;
bool orp_defineHandler = true;
bool orp_pushUpdate = false;
/* If the resource type to create is of JSON type */
bool orp_pushJSONUpdate = false;

/* If the resource type to create is of Boolean type */
bool orp_pushBoolUpdate = false;

int32_t orp_error_code;
com_err_t com_err;
/*ORP initialized to not connected */
bool orpReady = false;

#endif /* defined(USE_COM_MDM) */
/* SensorsClt application descriptor */
static cellular_app_desc_t     cellular_app_sensorsclient;
/* Timer to read periodically the Sensors */
static osTimerId cellular_app_sensorsclient_sensors_timer_id;
static cellular_app_sensorsclient_timer_state_t cellular_app_sensorsclient_sensors_timer_state;

static uint8_t cellular_app_sensorsclient_string[250];

/* Private macro -------------------------------------------------------------*/

/* Global variables ----------------------------------------------------------*/

/* Private functions prototypes ----------------------------------------------*/
/* Callback for CellularInfo status */
static void cellular_app_sensorsclient_cellular_info_status_cb(ca_event_type_t event_type,
                                                               const cellular_info_t *const p_cellular_info,
                                                               void *const p_callback_ctx);

/* Callback called when Sensors Timer is raised */
static void cellular_app_sensorsclient_sensors_timer_cb(void *p_argument);

static void sensorsclient_update_sensors_info(void);

static void sensorsclient_thread(void *p_argument);

/* Public  functions  prototypes ---------------------------------------------*/

/* Private functions ---------------------------------------------------------*/
/**
  * @brief  Callback called when a value in cellular changed
  * @param  event_type      - Event that happened: CA_CELLULAR_INFO_EVENT.
  * @param  p_cellular_info - The new cellular information.
  * @param  p_callback_ctx  - The p_callback_ctx parameter in cellular_info_changed_registration function.
  * @retval -
  */
static void cellular_app_sensorsclient_cellular_info_status_cb(ca_event_type_t event_type,
                                                               const cellular_info_t *const p_cellular_info,
                                                               void *const p_callback_ctx)
{
  UNUSED(p_callback_ctx);
  uint32_t msg_queue = 0U;
  rtosalStatus status;

  /* Event to know Modem state ? */
  if ((event_type == CA_CELLULAR_INFO_EVENT) && (p_cellular_info != NULL))
  {
    SET_CELLULAR_APP_MSG_TYPE(msg_queue, SENSORSCLIENT_CELLULAR_MSG);
    SET_CELLULAR_APP_MSG_ID(msg_queue, SENSORSCLIENT_CELLULAR_INFO_CHANGE_ID);
    /* Send the message */
    status = rtosalMessageQueuePut(cellular_app_sensorsclient.queue_id, msg_queue, 0U);
    if (status != osOK)
    {
      PRINT_FORCE("%s: ERROR CellularInfo Msg Put Type:%d Id:%d - status:%d!", p_cellular_app_sensorsclient_trace,
                  GET_CELLULAR_APP_MSG_TYPE(msg_queue), GET_CELLULAR_APP_MSG_ID(msg_queue), status)
    }
  }
}

/**
  * @brief  Callback called when Sensors Timer raised
  * @param  p_argument - UNUSED
  * @retval -
  */
static void cellular_app_sensorsclient_sensors_timer_cb(void *p_argument)
{
  UNUSED(p_argument);
  uint32_t msg_queue = 0U;
  rtosalStatus status;

  if (cellular_app_sensorsclient_sensors_timer_state == CELLULAR_APP_SENSORSCLIENT_TIMER_RUN)
  {
    SET_CELLULAR_APP_MSG_TYPE(msg_queue, SENSORSCLIENT_TIMER_MSG);
    SET_CELLULAR_APP_MSG_ID(msg_queue, SENSORSCLIENT_SENSORS_READ_ID);

    /* A message has to be send */
    status = rtosalMessageQueuePut(cellular_app_sensorsclient.queue_id, msg_queue, 0U);
    if (status != osOK)
    {
      PRINT_FORCE("%s: ERROR Sensors Msg Put Type:%d Id:%d - status:%d!", p_cellular_app_sensorsclient_trace,
                  GET_CELLULAR_APP_MSG_TYPE(msg_queue), GET_CELLULAR_APP_MSG_ID(msg_queue), status)
    }
  }
}

/**
  * @brief  Update status according to new cellular info received
  * @retval -
  */
static void sensorsclient_update_cellular_info(void)
{
  bool sensors_timer = false; /* false : sensors timer stopped, true: sensors timer started */
  uint8_t  operator_name_start;
  uint32_t operator_name_length;
  uint8_t  *p_string1;
  uint8_t  *p_string2;

  static uint8_t *sim_string[3]      = {((uint8_t *)"SIM:?  "), \
                                        ((uint8_t *)"SIMCard"), \
                                        ((uint8_t *)"SIMSold")
                                       };
  static uint8_t *state_string[10]   = {((uint8_t *)"   Unknown"), \
                                        ((uint8_t *)"      Init"), \
                                        ((uint8_t *)" SimConctd"), \
                                        ((uint8_t *)" NwkSearch"), \
                                        ((uint8_t *)" NwkAttach"), \
                                        ((uint8_t *)" DataReady"), \
                                        ((uint8_t *)"FlightMode"), \
                                        ((uint8_t *)"    Reboot"), \
                                        ((uint8_t *)"  Updating"), \
                                        ((uint8_t *)"  ModemOff")
                                       };
  static uint8_t *operator_string[1] = {((uint8_t *)"Operator:?")};

  static uint8_t *ip_string[2] = {((uint8_t *)"IP:?"), ((uint8_t *)"IP:")};

  cellular_sim_info_t sensors_sim_info;
  cellular_info_t     sensors_cellular_info;

  /* Read SIM information */
  cellular_get_sim_info(&sensors_sim_info);

  /* Read Cellular information to know modem state */
  cellular_get_cellular_info(&sensors_cellular_info);

  /* [eSIM|SIM |    ] */
  if (sensors_sim_info.sim_status[sensors_sim_info.sim_index] == CA_SIM_READY)
  {
    /* ToDo: Make the difference between SIM and eSIM */
    switch (sensors_sim_info.sim_slot_type[sensors_sim_info.sim_index])
    {
      case CA_SIM_REMOVABLE_SLOT :
        p_string1 = sim_string[1];
        break;
      case CA_SIM_EXTERNAL_MODEM_SLOT :
      case CA_SIM_INTERNAL_MODEM_SLOT :
        p_string1 = sim_string[2];
        break;
      default :
        p_string1 = sim_string[0];
        break;
    }
  }
  else
  {
    p_string1 = sim_string[0];
  }

  /* [State] */
  switch (sensors_cellular_info.modem_state)
  {
    case CA_MODEM_STATE_POWERED_ON:
      p_string2 = state_string[1];
      break;
    case CA_MODEM_STATE_SIM_CONNECTED:
      sensors_timer = true;
      p_string2 = state_string[2];
      break;
    case CA_MODEM_NETWORK_SEARCHING:
      sensors_timer = true;
      p_string2 = state_string[3];
      break;
    case CA_MODEM_NETWORK_REGISTERED:
      sensors_timer = true;
      p_string2 = state_string[4];
      break;
    case CA_MODEM_STATE_DATAREADY:
      sensors_timer = true;
      p_string2 = state_string[5];
      break;
    case CA_MODEM_IN_FLIGHTMODE:
      sensors_timer = true;
      p_string2 = state_string[6];
      break;
    case CA_MODEM_REBOOTING:
      p_string2 = state_string[7];
      break;
    case CA_MODEM_FOTA_INPROGRESS:
      p_string2 = state_string[8];
      break;
    case CA_MODEM_POWER_OFF:
      p_string2 = state_string[9];
      break;
    default:
      p_string2 = state_string[0];
      PRINT_INFO("%s: Modem state Unknown: %d!!!", p_cellular_app_sensorsclient_trace,
                 sensors_cellular_info.modem_state)
      break;
  }

  /* Display information about Sim and Modem status */
  PRINT_FORCE("%s: %s %s", p_cellular_app_sensorsclient_trace, p_string1, p_string2)

  /* Sensors timer start/stop management according to modem state */
  if (sensors_timer == true)
  {
    /* Start periodical Sensors Timer only if not already started */
    if (cellular_app_sensorsclient_sensors_timer_state == CELLULAR_APP_SENSORSCLIENT_TIMER_IDLE)
    {
      if (rtosalTimerStart(cellular_app_sensorsclient_sensors_timer_id, SENSORSCLIENT_SENSORS_READ_TIMER) == osOK)
      {
        cellular_app_sensorsclient_sensors_timer_state = CELLULAR_APP_SENSORSCLIENT_TIMER_RUN;
        PRINT_INFO("%s: Sensors timer started", p_cellular_app_sensorsclient_trace)
      }
    }
  }
  else
  {
    /* Stop periodical Sensors Timer only if already started */
    if (cellular_app_sensorsclient_sensors_timer_state == CELLULAR_APP_SENSORSCLIENT_TIMER_RUN)
    {
      if (rtosalTimerStop(cellular_app_sensorsclient_sensors_timer_id) == osOK)
      {
        cellular_app_sensorsclient_sensors_timer_state = CELLULAR_APP_SENSORSCLIENT_TIMER_IDLE;
        PRINT_INFO("%s: Sensors timer stopped", p_cellular_app_sensorsclient_trace)
      }
    }
  }

  /* [Operator] */
  operator_name_length = sensors_cellular_info.mno_name.len;
  if (sensors_cellular_info.mno_name.value[0] == (uint8_t)'"')
  {
    operator_name_start = 1U;
    operator_name_length = operator_name_length - 2U; /* by-pass '"' at string begin and end */
  }
  else
  {
    operator_name_start = 0U;
  }
  if (operator_name_length > 0U)
  {
    /* Point on mobile network mobile operator name */
    p_string1 = &sensors_cellular_info.mno_name.value[operator_name_start];
  }
  else
  {
    /* Point on mobile network mobile operator unknown*/
    p_string1 = operator_string[0];
    operator_name_length = 10U;
  }

  /* Display information about Operator status */
  PRINT_FORCE("%s: %.*s", p_cellular_app_sensorsclient_trace, (int16_t)operator_name_length, p_string1)

  /* Is IP known ? */
  if (sensors_cellular_info.ip_addr.addr != 0U)
  {
    p_string1 = ip_string[1];
    (void)sprintf((CRC_CHAR_t *)&cellular_app_sensorsclient_string[0], "%u.%u.%u.%u",
                  (uint8_t)(COM_IP4_ADDR1(&sensors_cellular_info.ip_addr)),
                  (uint8_t)(COM_IP4_ADDR2(&sensors_cellular_info.ip_addr)),
                  (uint8_t)(COM_IP4_ADDR3(&sensors_cellular_info.ip_addr)),
                  (uint8_t)(COM_IP4_ADDR4(&sensors_cellular_info.ip_addr)));

    p_string2 = cellular_app_sensorsclient_string;
  }
  else
  {
    p_string1 = ip_string[0];
    p_string2 = NULL;
  }
  PRINT_FORCE("%s: %s %s", p_cellular_app_sensorsclient_trace, p_string1, p_string2)
}

/**
  * @brief  Update status according to new sensors info read
  * @retval -
  */
static void sensorsclient_update_sensors_info(void)
{
  static cellular_app_sensors_data_t sensor_humidity;
  static cellular_app_sensors_data_t sensor_pressure;
  static cellular_app_sensors_data_t sensor_temperature;
  static cellular_app_sensors_data_t accelerometer_info;
  static orp_numeric_resource_update_t orp_update;
  static orp_json_resource_update_t orp_json_update;
  orp_start();
  /* Read Humidity sensor */
  if (cellular_app_sensors_read(CELLULAR_APP_SENSOR_TYPE_HUMIDITY, &sensor_humidity) == true)
  {
	/* Push sensor data to Octave */
	if(orpReady == true && orp_pushUpdate == true && orp_pushJSONUpdate == false)
	{
	  /* Resource member declaration */
	  strcpy((char *)orp_update.resource_name,(const char *)ORP_RESOURCE_SENSOR_HUMIDITY);
	  orp_update.resource_value = sensor_humidity.float_data;
	  (void) memset((void *)orp_rspbuf, 0, ORP_MAX_RSP_SIZE);
	  com_err = orp_set_numeric_resource(currentHandle,&orp_update,orp_rspbuf,&orp_error_code);
	  PRINT_INFO("The Update of Humidity to Octave is %ld :",com_err)
	}
  }
  else
  {
	sensor_humidity.float_data = (float_t)0;
  }
  /* Read Pressure sensor */
  if (cellular_app_sensors_read(CELLULAR_APP_SENSOR_TYPE_PRESSURE, &sensor_pressure) == true)
  {
  	/* Push sensor data to Octave */
  	if(orpReady == true && orp_pushUpdate == true && orp_pushJSONUpdate == false)
  	{
  	  /* Resource member declaration */
  	  strcpy((char *)orp_update.resource_name,(const char *)ORP_RESOURCE_SENSOR_PRESSURE);
  	  orp_update.resource_value = sensor_pressure.float_data;
  	  (void) memset((void *)orp_rspbuf, 0, ORP_MAX_RSP_SIZE);
  	  com_err = orp_set_numeric_resource(currentHandle,&orp_update,orp_rspbuf,&orp_error_code);
  	  PRINT_INFO("The Update of Pressure to Octave is %ld :",com_err)
  	}
  }
  else
  {
    sensor_pressure.float_data = (float_t)0;
  }

  /* Read Temperature sensor */
  if (cellular_app_sensors_read(CELLULAR_APP_SENSOR_TYPE_TEMPERATURE, &sensor_temperature) == true)
  {
	/* Push sensor data to Octave */
	if(orpReady == true && orp_pushUpdate == true && orp_pushJSONUpdate == false)
	{
      /* Resource member declaration */
      strcpy((char *)orp_update.resource_name,(const char *)ORP_RESOURCE_SENSOR_TEMPERATURE);
      orp_update.resource_value = sensor_temperature.float_data;
      (void) memset((void *)orp_rspbuf, 0, ORP_MAX_RSP_SIZE);
      com_err = orp_set_numeric_resource(currentHandle,&orp_update,orp_rspbuf,&orp_error_code);
      PRINT_INFO("The Update of Temperature to Octave is %ld :",com_err)
	}
  }
  else
  {
	sensor_temperature.float_data = (float_t)0;
  }
  /* Read Accelerometer sensor */
  if (cellular_app_sensors_read(CELLULAR_APP_SENSOR_TYPE_ACCELEROMETER, &accelerometer_info) == true)
  {
	/* Push sensor data to Octave */
	if(orpReady == true && orp_pushUpdate == true && orp_pushJSONUpdate == false)
	{
/*	   Resource member declaration
	  strcpy((char *)orp_update.resource_name,(const char *)ORP_RESOURCE_SENSOR_ACCELEROMETER_AXIS_X);
	  orp_update.resource_value = accelerometer_info.AXIS_X;
	  (void) memset((void *)orp_rspbuf, 0, ORP_MAX_RSP_SIZE);
	  com_err = orp_set_numeric_resource(currentHandle,&orp_update,orp_rspbuf,&orp_error_code);
	  PRINT_INFO("The Update of ACCELEROMETER_AXIS_X to Octave is %ld :",com_err)
	  if(com_err == 0)
	  {
		 Resource member declaration
		strcpy((char *)orp_update.resource_name,(const char *)ORP_RESOURCE_SENSOR_ACCELEROMETER_AXIS_Y);
		orp_update.resource_value = accelerometer_info.AXIS_Y;
		(void) memset((void *)orp_rspbuf, 0, ORP_MAX_RSP_SIZE);
		com_err = orp_set_numeric_resource(currentHandle,&orp_update,orp_rspbuf,&orp_error_code);
		PRINT_INFO("The Update of ACCELEROMETER_AXIS_Y to Octave is %ld :",com_err)
		if(com_err == 0)
		{
		   Resource member declaration
		  strcpy((char *)orp_update.resource_name,(const char *)ORP_RESOURCE_SENSOR_ACCELEROMETER_AXIS_Z);
		  orp_update.resource_value = accelerometer_info.AXIS_Z;
		  (void) memset((void *)orp_rspbuf, 0, ORP_MAX_RSP_SIZE);
		  com_err = orp_set_numeric_resource(currentHandle,&orp_update,orp_rspbuf,&orp_error_code);
		  PRINT_INFO("The Update of ACCELEROMETER_AXIS_Z to Octave is %ld :",com_err)
		}
	  }
*/
		/* Resource member declaration */
		strcpy((char *)orp_json_update.resource_name,(const char *)ORP_RESOURCE_SENSOR_ACCELEROMETER_ROOT);
		char ORP_json_Update[ORP_MAX_RESOURCE_VALUE] = {0} ;
		sprintf(ORP_json_Update,"\\x22AXIS_X\\x22:%d,\\x22AXIS_Y\\x22:%d,\\x22AXIS_Z\\x22:%d",accelerometer_info.AXIS_X,accelerometer_info.AXIS_Y,accelerometer_info.AXIS_Z);
		orp_json_update.resource_value = (com_char_t *) ORP_json_Update;
		(void) memset((void *)orp_rspbuf, 0, ORP_MAX_RSP_SIZE);
		com_err = orp_set_json_resource(currentHandle,&orp_json_update,orp_rspbuf,&orp_error_code);
		PRINT_INFO("The Update action of ACCELEROMETER_JSON to Octave is %ld :",com_err)

		/* Resource member declaration */
		/*strcpy((char *)orp_json_update.resource_name,(const char *)ORP_RESOURCE_SENSOR_JSON_ROOT);
		char ORP_json_Update[ORP_MAX_RESOURCE_VALUE] = {0} ;
		sprintf(ORP_json_Update,"\\x22Temperature\\x22:%4.1f,\\x22Pressure\\x22:%6.1f,\\x22Humidity\\x22:%4.1f,\\x22Accelerometer/AXIS_X\\x22:%d,\\x22Accelerometer/AXIS_Y\\x22:%d,\\x22Accelerometer/AXIS_Z\\x22:%d",
				sensor_temperature.float_data,sensor_pressure.float_data,sensor_humidity.float_data, accelerometer_info.AXIS_X,accelerometer_info.AXIS_Y,accelerometer_info.AXIS_Z);
		orp_json_update.resource_value = (com_char_t *) ORP_json_Update;
		(void) memset((void *)orp_rspbuf, 0, ORP_MAX_RSP_SIZE);
		com_err = orp_set_json_resource(currentHandle,&orp_json_update,orp_rspbuf,&orp_error_code);
		PRINT_INFO("The Update of Sensor JSON data to Cloud to Octave is %ld :",com_err)*/
	}
  }
  else
  {
	accelerometer_info.AXIS_X  =  (int16_t)0;
	accelerometer_info.AXIS_Y  =  (int16_t)0;
	accelerometer_info.AXIS_Z  =  (int16_t)0;
  }

  (void)sprintf((CRC_CHAR_t *)cellular_app_sensorsclient_string, "Temperature:%4.1fC Humidity:%4.1f%% Pressure:%6.1fP AxisX:%d AxisY:%d AxisZ:%d",
                sensor_temperature.float_data, sensor_humidity.float_data, sensor_pressure.float_data,accelerometer_info.AXIS_X,accelerometer_info.AXIS_Y,accelerometer_info.AXIS_Z);
  PRINT_INFO("The value of orpReady is %d and orp_pushUpdate is %d",orpReady,orp_pushUpdate)

  /* Display information about Sensors data */
  PRINT_FORCE("%s: %s", p_cellular_app_sensorsclient_trace, cellular_app_sensorsclient_string)
  if(orpReady == true && orp_pushUpdate == true && orp_pushJSONUpdate == true)
  {
	/* Resource member declaration */
	strcpy((char *)orp_update.resource_name,(const char *)ORP_RESOURCE_SENSOR_JSON_ROOT);
	char ORP_json_Update[ORP_MAX_RESOURCE_VALUE] = {0} ;
	sprintf(ORP_json_Update,"\\x22Temperature\\x22:%4.1f,\\x22Pressure\\x22:%6.1f,\\x22Humidity\\x22:%4.1f,\\x22Accelerometer/AXIS_X\\x22:%d,\\x22Accelerometer/AXIS_Y\\x22:%d,\\x22Accelerometer/AXIS_Z\\x22:%d",
			sensor_temperature.float_data,sensor_pressure.float_data,sensor_humidity.float_data, accelerometer_info.AXIS_X,accelerometer_info.AXIS_Y,accelerometer_info.AXIS_Z);
	orp_json_update.resource_value = (com_char_t *) ORP_json_Update;
	(void) memset((void *)orp_rspbuf, 0, ORP_MAX_RSP_SIZE);
	com_err = orp_set_json_resource(currentHandle,&orp_json_update,orp_rspbuf,&orp_error_code);
	PRINT_INFO("The Update of Sensor JSON data to Cloud to Octave is %ld :",com_err)
  }
}

/**
  * @brief  SensorsClt thread
  * @note   Infinite loop SensorsClt body
  * @param  p_argument - unused
  * @retval -
  */
static void sensorsclient_thread(void *p_argument)
{
  UNUSED(p_argument);
  uint32_t msg_queue;
  uint8_t msg_type;  /* Msg type received from the queue */
  uint8_t msg_id;    /* Msg id received from the queue   */

  for (;;)
  {
    msg_queue = 0U; /* Re-initialize msg_queue to impossible value */
    /* Wait a notification to do something */
    (void)rtosalMessageQueueGet(cellular_app_sensorsclient.queue_id, &msg_queue, RTOSAL_WAIT_FOREVER);
    /* Analyze message */
    if (msg_queue != 0U)
    {
      msg_type = GET_CELLULAR_APP_MSG_TYPE(msg_queue);
      msg_id   = GET_CELLULAR_APP_MSG_ID(msg_queue);

      switch (msg_type)
      {
        case CELLULAR_APP_PROCESS_MSG :
          if (msg_id == CELLULAR_APP_MODEM_CHANGE_ID)
          {
            /* SensorClt already updated by internal callback */
            /* sensorsclient_update_cellular_info(); */
            __NOP();
          }
          else /* Should not happen */
          {
            __NOP();
          }
          break;

        case SENSORSCLIENT_CELLULAR_MSG :
          if (msg_id == SENSORSCLIENT_CELLULAR_INFO_CHANGE_ID)
          {
            /* Update ORP Init Status */
            (void) memset((void *)orp_rspbuf, 0, ORP_MAX_RSP_SIZE);
            com_err = orp_get_resource(currentHandle,(uint8_t *) ORP_IS_CONNECTED,
            		orp_rspbuf,&orp_error_code);
            //PRINT_INFO("orp_get_resource returned %ld",com_err)
            if(strcmp((const char *)(orp_rspbuf),"true") == 0)
            {
              orpReady = true;
            }
            else
            {
              /* reset flag */
              orpReady = false;
            }
            sensorsclient_update_cellular_info();
          }
          else /* Should not happen */
          {
            __NOP();
          }
          break;

        case SENSORSCLIENT_TIMER_MSG :
          if (msg_id == SENSORSCLIENT_SENSORS_READ_ID)
          {
            //PRINT_INFO("orpReady value is %d",orpReady)
            /* Default value of orpReady is 0 */
            if(orpReady == false)
            {
              /* Check ORP Init Status */
              (void) memset((void *)orp_rspbuf, 0, ORP_MAX_RSP_SIZE);
              com_err = orp_get_resource(currentHandle,(uint8_t *) ORP_IS_CONNECTED,
            			orp_rspbuf,&orp_error_code);
              //PRINT_INFO("orp_get_resource returned %ld",com_err)
              if(strcmp((const char *)(orp_rspbuf),"true") == 0)
              {
                orpReady = true;
              }
              else
              {
            	/* reset flag */
                orpReady = false;
              }
            }
            sensorsclient_update_sensors_info();
          }
          break;
        case SENSORSCLIENT_TIMER_UPDATE_MSG :
          if (msg_id == SENSORSCLIENT_SENSORS_TIMER_UPDATE)
          {
        	/* Stop periodical Sensors Timer only if already started */
        	if (cellular_app_sensorsclient_sensors_timer_state == CELLULAR_APP_SENSORSCLIENT_TIMER_RUN)
        	{
        	  if (rtosalTimerStop(cellular_app_sensorsclient_sensors_timer_id) == osOK)
        	  {
        	  	cellular_app_sensorsclient_sensors_timer_state = CELLULAR_APP_SENSORSCLIENT_TIMER_IDLE;
        	  }
        	}
        	/* Start periodical Sensors Timer only if not already started */
        	if (cellular_app_sensorsclient_sensors_timer_state == CELLULAR_APP_SENSORSCLIENT_TIMER_IDLE)
        	{
        	  if (rtosalTimerStart(cellular_app_sensorsclient_sensors_timer_id, SENSORSCLIENT_SENSORS_READ_TIMER) == osOK)
        	  {
        	  	cellular_app_sensorsclient_sensors_timer_state = CELLULAR_APP_SENSORSCLIENT_TIMER_RUN;
        	  	PRINT_INFO("%s: Sensors timer started with updated periodicity", p_cellular_app_sensorsclient_trace)
        	  }
        	}
          }
          break;
        default : /* Should not happen */
          __NOP();
          break;
      }
    }
  }
}

/* Functions Definition ------------------------------------------------------*/
/**
  * @brief  Send a message to SensorsClt
  * @param  queue_msg - Message to send
  * @retval -
  */
bool cellular_app_sensorsclient_send_msg(uint32_t queue_msg)
{
  bool result = true;
  rtosalStatus status;

  /* A message has to be send */
  status = rtosalMessageQueuePut(cellular_app_sensorsclient.queue_id, queue_msg, 0U);
  if (status != osOK)
  {
    result = false;
    PRINT_FORCE("%s: ERROR CellularApi Msg Put Type:%d Id:%d - status:%d!", p_cellular_app_sensorsclient_trace,
                GET_CELLULAR_APP_MSG_TYPE(queue_msg), GET_CELLULAR_APP_MSG_ID(queue_msg), status)
  }

  return (result);
}

/**
  * @brief  Initialize all needed structures to support SensorsClt feature
  * @param  -
  * @retval -
  */
void cellular_app_sensorsclient_init(void)
{
  /* Initialize trace shortcut */
  p_cellular_app_sensorsclient_trace = cellular_app_type_string[CELLULAR_APP_TYPE_SENSORSCLIENT];

  /* Application Initialization */
  cellular_app_sensorsclient.app_id = 0U;
  /* Process Status Initialization */
  cellular_app_sensorsclient.process_status = true;
  /* Process Period Initialization */
  cellular_app_sensorsclient.process_period = 0U; /* Unused */
  /* Thread Id Initialization */
  cellular_app_sensorsclient.thread_id = NULL;
  /* Queue Id Initialization/Creation */
  cellular_app_sensorsclient.queue_id = rtosalMessageQueueNew(NULL, CELLULAR_APP_QUEUE_SIZE);

  /* Specific Initialization */

  /* Need a timer to read periodically the Sensors */
  cellular_app_sensorsclient_sensors_timer_id = rtosalTimerNew(NULL,
                                                               (os_ptimer)cellular_app_sensorsclient_sensors_timer_cb,
                                                               osTimerPeriodic, NULL);
  cellular_app_sensorsclient_sensors_timer_state = CELLULAR_APP_SENSORSCLIENT_TIMER_IDLE;

  /* Check Initialization is ok */
  if (cellular_app_sensorsclient.queue_id == NULL)
  {
    CELLULAR_APP_ERROR(CELLULAR_APP_ERROR_SENSORSCLIENT, ERROR_FATAL)
  }
  if (cellular_app_sensorsclient_sensors_timer_id == NULL)
  {
    cellular_app_sensorsclient_sensors_timer_state = CELLULAR_APP_SENSORSCLIENT_TIMER_INVALID;
    CELLULAR_APP_ERROR((CELLULAR_APP_ERROR_SENSORSCLIENT + 1), ERROR_FATAL)
  }
}

/**
  * @brief  Start SensorsClt thread
  * @param  -
  * @retval -
  */
void cellular_app_sensorsclient_start(void)
{
  uint8_t thread_name[CELLULAR_APP_THREAD_NAME_MAX];
  uint32_t len;

  /* Cellular initialization already done - Registration to services is OK */
  /* Registration to CellularInfo: needs to know all cellular status modification*/
  if (cellular_info_cb_registration(cellular_app_sensorsclient_cellular_info_status_cb, (void *) NULL)
      != CELLULAR_SUCCESS)
  {
    CELLULAR_APP_ERROR((CELLULAR_APP_ERROR_SENSORSCLIENT + 2), ERROR_FATAL)
  }

  if (cellular_app_sensorsclient_sensors_timer_state == CELLULAR_APP_SENSORSCLIENT_TIMER_IDLE)
  {
    /* Initialization Sensors modules */
    cellular_app_sensors_init();
    /* Initialization Sensor Humidity */
    if (cellular_app_sensors_initialize(CELLULAR_APP_SENSOR_TYPE_HUMIDITY) == false)
    {
      PRINT_FORCE("%s: Humidity sensors init NOK!", p_cellular_app_sensorsclient_trace)
    }
    /* Initialization Sensor Pressure */
    if (cellular_app_sensors_initialize(CELLULAR_APP_SENSOR_TYPE_PRESSURE) == false)
    {
      PRINT_FORCE("%s: Pressure sensor init NOK!", p_cellular_app_sensorsclient_trace)
    }
    /* Initialization Sensors Temperature */
    if (cellular_app_sensors_initialize(CELLULAR_APP_SENSOR_TYPE_TEMPERATURE) == false)
    {
      PRINT_FORCE("%s: Temperature sensor init NOK!", p_cellular_app_sensorsclient_trace)
    }
    /* Initialization Sensors Temperature */
    if (cellular_app_sensors_initialize(CELLULAR_APP_SENSOR_TYPE_ACCELEROMETER) == false)
    {
    	PRINT_FORCE("%s: Accelerometer sensor init NOK!", p_cellular_app_sensorsclient_trace)
    }
  }

  /* Thread Name Generation */
  len = crs_strlen((const uint8_t *)"SensorsClt");
  /* '+1' to copy '\0' */
  (void)memcpy(thread_name, "SensorsClt", CELLULAR_APP_MIN((len + 1U), CELLULAR_APP_THREAD_NAME_MAX));

  /* Thread Creation */
  cellular_app_sensorsclient.thread_id = rtosalThreadNew((const rtosal_char_t *)thread_name,
                                                         (os_pthread)sensorsclient_thread,
                                                         SENSORSCLIENT_THREAD_PRIO, SENSORSCLIENT_THREAD_STACK_SIZE,
                                                         NULL);
  /* Check Creation is ok */
  if (cellular_app_sensorsclient.thread_id == NULL)
  {
    CELLULAR_APP_ERROR((CELLULAR_APP_ERROR_SENSORSCLIENT + 4), ERROR_FATAL)
  }
}
#if defined(USE_COM_MDM)
void orp_callback(void)
{
	/* TODO STM: If a URC is received before subscription,
	 * the state machine freezes as its not able to extract/free the buffers
	 */
	(void) memset((void *)orp_rspbuf, 0, ORP_MAX_RSP_SIZE);
	orp_response_t * orpURCRsp;
    PRINT_DBG(" *** start of call_orp_receive to read rx buffer***")
    com_err =  orp_receive(currentHandle, orp_rspbuf, ORP_MAX_RSP_SIZE, &orp_error_code);
    orpURCRsp = orpResponseDecode(orp_rspbuf,(size_t)orp_rspbuf);
    PRINT_INFO("An URC on the resource: %s received with value %s",orpURCRsp->resource_name,(orpURCRsp->resource_value))
	if(strcmp(orpURCRsp->resource_name,ORP_RESOURCE_SENSOR_PERIODICITY) == 0)
	{
	  uint32_t msg_queue = 0U;
	  rtosalStatus status;
	  PRINT_INFO("The Sensor periodicity is changed to %ld seconds",(crs_atoi)(orpURCRsp->resource_value))
	  /* Converting timer value to seconds*/
	  SENSORSCLIENT_SENSORS_READ_TIMER=(uint32_t)(crs_atoi)(orpURCRsp->resource_value)*1000;
	  SET_CELLULAR_APP_MSG_TYPE(msg_queue, SENSORSCLIENT_TIMER_UPDATE_MSG);
	  SET_CELLULAR_APP_MSG_ID(msg_queue, SENSORSCLIENT_SENSORS_TIMER_UPDATE);
	  /* Send the message */
	  status = rtosalMessageQueuePut(cellular_app_sensorsclient.queue_id, msg_queue, 0U);
	  if (status != osOK)
	  {
	    PRINT_FORCE("%s: ERROR CellularInfo Msg Put Type:%d Id:%d - status:%d!", p_cellular_app_sensorsclient_trace,
	                    GET_CELLULAR_APP_MSG_TYPE(msg_queue), GET_CELLULAR_APP_MSG_ID(msg_queue), status)
	  }
	}
	else
	{
	  PRINT_INFO("Unidentified sensor value is changed to %s",(orpURCRsp->resource_value))
	}

}

void orp_start (void)
{
  static orp_resource_create_t sPeriodicity;
  if (orpReady == true && orp_defineResource == true)
  {
	/* Resource member declaration */
	strcpy((char *)sPeriodicity.resource_name,(const char *)ORP_RESOURCE_SENSOR_PERIODICITY);
	sPeriodicity.res_dir = 'O';
	sPeriodicity.res_type = 'N';
	com_err = orp_create_resource(currentHandle,&sPeriodicity,&orp_error_code);
	if(com_err == 0)
	{
	  /* Resource member declaration */
	  strcpy((char *)sPeriodicity.resource_name,(const char *)ORP_RESOURCE_SENSOR_TEMPERATURE);
	  sPeriodicity.res_dir = 'I';
	  sPeriodicity.res_type = 'N';
	  com_err = orp_create_resource(currentHandle,&sPeriodicity,&orp_error_code);
	  if(com_err == 0)
	  {
		/* Resource member declaration */
		strcpy((char *)sPeriodicity.resource_name,(const char *)ORP_RESOURCE_SENSOR_PRESSURE);
		sPeriodicity.res_dir = 'I';
		sPeriodicity.res_type = 'N';
		com_err = orp_create_resource(currentHandle,&sPeriodicity,&orp_error_code);
	    if(com_err == 0)
	    {
		  /* Resource member declaration */
		  strcpy((char *)sPeriodicity.resource_name,(const char *)ORP_RESOURCE_SENSOR_HUMIDITY);
		  sPeriodicity.res_dir = 'I';
		  sPeriodicity.res_type = 'N';
		  com_err = orp_create_resource(currentHandle,&sPeriodicity,&orp_error_code);
	      if(com_err == 0)
	      {

	    	 /* Resource member declaration */
	    	 strcpy((char *)sPeriodicity.resource_name,(const char *)ORP_RESOURCE_SENSOR_ACCELEROMETER_ROOT);
	    	 sPeriodicity.res_dir = 'I';
	    	 sPeriodicity.res_type = 'J';
	    	 com_err = orp_create_resource(currentHandle,&sPeriodicity,&orp_error_code);

		    /* Resource member declaration */
		    strcpy((char *)sPeriodicity.resource_name,(const char *)ORP_RESOURCE_SENSOR_ACCELEROMETER_AXIS_X);
		    sPeriodicity.res_dir = 'I';
		    sPeriodicity.res_type = 'J';
		    com_err = orp_create_resource(currentHandle,&sPeriodicity,&orp_error_code);
		    if(com_err == 0)
		    {
		      /* Resource member declaration */
		  	  strcpy((char *)sPeriodicity.resource_name,(const char *)ORP_RESOURCE_SENSOR_ACCELEROMETER_AXIS_Y);
		  	  sPeriodicity.res_dir = 'I';
		  	  sPeriodicity.res_type = 'J';
		  	  com_err = orp_create_resource(currentHandle,&sPeriodicity,&orp_error_code);
		  	  if(com_err == 0)
		  	  {
		  	    /* Resource member declaration */
		  	    strcpy((char *)sPeriodicity.resource_name,(const char *)ORP_RESOURCE_SENSOR_ACCELEROMETER_AXIS_Z);
		  	    sPeriodicity.res_dir = 'I';
		  	    sPeriodicity.res_type = 'J';
		  	    com_err = orp_create_resource(currentHandle,&sPeriodicity,&orp_error_code);
		  	    /* Set the ORP defined resource flag to False to notify successful creation of all resources *
		  	     * ORP wont throw error if resource with same name is created as no action will be done by ORP if done so */
		  	    orp_defineResource = false;
		  	    /* Set ORP Push Update flag to true to enable data push */
		  	    orp_pushUpdate = true;
		  	  }
		    }
	      }
	    }
	  }
	}
  }
  if (orpReady == true && orp_defineHandler == true)
  {
    /* Resource member declaration */
	strcpy((char *)sPeriodicity.resource_name,(const char *)ORP_RESOURCE_SENSOR_PERIODICITY);
	//strcpy((char *)sPeriodicity.resource_name,(const char *)ORP_RESOURCE_BOOLEAN_TEST);
	com_err = orp_create_handler(currentHandle,&sPeriodicity,&orp_error_code);
    PRINT_DBG(" Create Handler return %ld", com_err)
	orp_defineHandler = false;
  }
}

#endif /* defined(USE_COM_MDM) */

#endif /* USE_SENSORS == 1 */

/************************ (C) COPYRIGHT STMicroelectronics and Sierra Wireless *****END OF FILE****/
