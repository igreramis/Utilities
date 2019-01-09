/**
  ******************************************************************************
  * File Name          :  app_mems-library.c
  * Description        : This file provides code for the configuration
  *                      of the STMicroelectronics.MEMS-Library.0.0.1 instances.
  ******************************************************************************
  *
  * COPYRIGHT 2018 STMicroelectronics
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software
  * distributed under the License is distributed on an "AS IS" BASIS,
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  ******************************************************************************
  */

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "app_mems-library.h"
#include "app_mems_int_pin_a_interface.h"
//#include "stm32l1xx_hal.h"
//#include "stm32l1xx_hal_exti.h"
//#include "stm32l1_nucleo_l152re.h"
#include "stm32_common_bsp.h"
#include "iks01a2_motion_sensors.h"
#include "iks01a2_motion_sensors_ex.h"
#include "app_uart_interface.h"

/* Private typedef -----------------------------------------------------------*/
/**
 * @brief  Handle DEMO State Machine
 */
extern void _Error_Handler(char *, int);
//typedef enum
//{
//  STATUS_IDLE,
//  STATUS_SET_FIFO_MODE,
//  STATUS_FIFO_RUN,
//  STATUS_FIFO_DOWNLOAD,
//  STATUS_SET_FIFO_BYPASS_MODE
//} DEMO_FIFO_STATUS_t;

/* Private define ------------------------------------------------------------*/
#define MAX_BUF_SIZE 256
#define FIFO_WATERMARK   91 /*!< FIFO size limit */

#define LSM6DSL_SAMPLE_ODR      10.0f /*!< Sample Output Data Rate [Hz] */
#define LSM6DSL_FIFO_MAX_ODR  6600.0f /*!< LSM6DSL FIFO maximum ODR */

#define ENABLE  1 /*!< Enable LSM6DSL FIFO functions */

#define INDICATION_DELAY  100 /* LED is ON for this period [ms]. */

#define PATTERN_GYR_X_AXIS  0 /*!< Pattern of gyro X axis */
#define PATTERN_GYR_Y_AXIS  1 /*!< Pattern of gyro Y axis */
#define PATTERN_GYR_Z_AXIS  2 /*!< Pattern of gyro Z axis */

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
volatile uint8_t MemsEventDetected = 0;
//static volatile uint8_t PushButtonDetected = 0;
volatile uint8_t PushButtonDetected = 0;
DEMO_FIFO_STATUS_t DemoFifoStatus = STATUS_SET_FIFO_BYPASS_MODE;
static char dataOut[MAX_BUF_SIZE];
static uint8_t fifo_full_status = 0;
static uint16_t num_samples = 0;
static uint16_t prev_num_samples = 0;

/* Private function prototypes -----------------------------------------------*/
static void MX_IKS01A2_LSM6DSL_FIFOMode_Init(void);
static void MX_IKS01A2_LSM6DSL_FIFOMode_Process(void);
static int32_t LSM6DSL_FIFO_Set_Bypass_Mode(void);
static int32_t LSM6DSL_FIFO_Set_FIFO_Mode(void);
//static int32_t LSM6DSL_Read_All_FIFO_Data(void);
static int32_t LSM6DSL_Read_All_FIFO_Data(int *three_axes);
//static int32_t LSM6DSL_Read_Single_FIFO_Pattern_Cycle(uint16_t SampleIndex);
static int32_t LSM6DSL_Read_Single_FIFO_Pattern_Cycle(uint16_t SampleIndex, int *three_axes);
static int32_t LSM6DSL_FIFO_Demo_Config(void);

//AL
void MX_IKS01A2_LSM6DSL_9AXES_Init();
//static void MX_IKS01A2_LSM6DSL_9AXESMode_Process(void);
static void MX_IKS01A2_LSM6DSL_9AXESMode_Process(int *nine_axes);
static void Send_Orientation(void);
void DisplayOrientation(int *six_axes);
void Display3Axes(int *three_axes);
uint8_t SendOrientationRequest  = 0;
void Get_Orientation(int *axes);
void debug_printvalues(int *nine_axes);
//AL
/**
  * @brief  Initialize the MEMS application
  * @retval None
  */
void MX_MEMS_Library_Init(void)
{
  /* USER CODE BEGIN SV */

  /* USER CODE END SV */

  /* USER CODE BEGIN MEMS_Library_Init_PreTreatment */

  /* USER CODE END MEMS_Library_Init_PreTreatment */

  /* Initialize the peripherals and the MEMS components */

//  MX_IKS01A2_LSM6DSL_FIFOMode_Init();
  MX_IKS01A2_LSM6DSL_9AXES_Init();

  /* USER CODE BEGIN SV */

  /* USER CODE END SV */

  /* USER CODE BEGIN MEMS_Library_Init_PostTreatment */

  /* USER CODE END MEMS_Library_Init_PostTreatment */
}

void debug_printvalues(int *nine_axes)
{
	DisplayOrientation(nine_axes+0);
	Display3Axes(nine_axes+6);
}
/**
  * @brief  Main process of the MEMS application
  * @retval None
  */
void MX_MEMS_Library_Process(void)
{
	int nine_axes[9];
//  MX_IKS01A2_LSM6DSL_FIFOMode_Process();
	MX_IKS01A2_LSM6DSL_9AXESMode_Process(nine_axes+0);
//	debug_printvalues(nine_axes);
}

/**
  * @brief  Initialize the LSM6DSL FIFO Mode application
  * @retval None
  */
void MX_IKS01A2_LSM6DSL_FIFOMode_Init(void)
{
  /* Initialize LED */
  BSP_LED_Init(LED2);

  /* Initialize button */
  BSP_PB_Init(BUTTON_KEY, BUTTON_MODE_EXTI);

  set_mems_int_pin_a_exti();

  MX_APP_UART_Init();

  (void)IKS01A2_MOTION_SENSOR_Init(IKS01A2_LSM6DSL_0, MOTION_GYRO);

  /* Configure LSM6DSL sensor for the DEMO application */
  if (LSM6DSL_FIFO_Demo_Config() != BSP_ERROR_NONE)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  (void)snprintf(dataOut, MAX_BUF_SIZE, "\r\n------ LSM6DSL FIFO Mode DEMO ------\r\n\r\n");
//  (void)MX_APP_UART_Transmit((uint8_t *)dataOut, (uint16_t)strlen(dataOut), 5000);
  printf("%s",dataOut);
}

void MX_IKS01A2_LSM6DSL_9AXES_Init()
{
  /* Initialize LED */
  BSP_LED_Init(LED2);

  /* Initialize button */
  BSP_PB_Init(BUTTON_KEY, BUTTON_MODE_EXTI);

  set_mems_int_pin_a_exti();

  MX_APP_UART_Init();
//  BSP_COM_Init(COM1);

//  (void)IKS01A2_MOTION_SENSOR_Init(IKS01A2_LSM6DSL_0, MOTION_GYRO);

  (void)IKS01A2_MOTION_SENSOR_Init(IKS01A2_LSM6DSL_0, MOTION_ACCELERO | MOTION_GYRO);
  int i=0,y;
  /* Configure LSM6DSL sensor for the DEMO application */
  if (LSM6DSL_FIFO_Demo_Config() != BSP_ERROR_NONE)
  {
	  y=i;
	  i=y;
//    _Error_Handler(__FILE__, __LINE__);
  }

  (void)IKS01A2_MOTION_SENSOR_Enable_6D_Orientation(IKS01A2_LSM6DSL_0, IKS01A2_MOTION_SENSOR_INT1_PIN);

  (void)snprintf(dataOut, MAX_BUF_SIZE, "\r\n------ LSM6DSL 9AXES ------\r\n\r\n");
  (void)MX_APP_UART_Transmit((uint8_t *)dataOut, (uint16_t)strlen(dataOut), 5000);
//  printf("%s", dataOut);

}

void MX_IKS01A2_LSM6DSL_9AXESMode_Process(int *nine_axes)
{
  IKS01A2_MOTION_SENSOR_Event_Status_t status;

  /* Handle DEMO State Machine */
  switch (DemoFifoStatus)
  {
    case STATUS_IDLE:
      DemoFifoStatus = STATUS_SET_FIFO_MODE;
      break;

    case STATUS_SET_FIFO_MODE:
      if (LSM6DSL_FIFO_Set_FIFO_Mode() != BSP_ERROR_NONE)
      {
        Error_Handler();
      }
      DemoFifoStatus = STATUS_FIFO_RUN;
      break;

    case STATUS_FIFO_RUN:
      /* Get num of unread FIFO samples before reading data */
      if (IKS01A2_MOTION_SENSOR_FIFO_Get_Num_Samples(IKS01A2_LSM6DSL_0, &num_samples) != BSP_ERROR_NONE)
      {
        Error_Handler();
      }
      /* Print dot realtime whenever new data is stored in FIFO */
      if ((num_samples != prev_num_samples))
      {
        prev_num_samples = num_samples;
        (void)snprintf(dataOut, MAX_BUF_SIZE, ".");
//        printf("%s", dataOut);
//        fflush(stdout);
        (void)MX_APP_UART_Transmit((uint8_t *)dataOut, (uint16_t)strlen(dataOut), 5000);
      }
      if (MemsEventDetected == 1U)
      {
        DemoFifoStatus = STATUS_FIFO_DOWNLOAD;
      }
      break;

    case STATUS_FIFO_DOWNLOAD:
      /* Print data if FIFO is full */
      if (IKS01A2_MOTION_SENSOR_FIFO_Get_Full_Status(IKS01A2_LSM6DSL_0, &fifo_full_status) != BSP_ERROR_NONE)
      {
        Error_Handler();
      }
      if (fifo_full_status == 1U)
      {
    	Get_Orientation(nine_axes+0);
        BSP_LED_On(LED2);
        if (LSM6DSL_Read_All_FIFO_Data(nine_axes+6) != BSP_ERROR_NONE)
        {
          Error_Handler();
        }
        HAL_Delay(INDICATION_DELAY);
        BSP_LED_Off(LED2);
        DemoFifoStatus = STATUS_SET_FIFO_BYPASS_MODE;
      }
      break;

    case STATUS_SET_FIFO_BYPASS_MODE:
      if (LSM6DSL_FIFO_Set_Bypass_Mode() != BSP_ERROR_NONE)
      {
        Error_Handler();
      }
      MemsEventDetected = 0;
      num_samples = 0;
      prev_num_samples = 0;
      DemoFifoStatus = STATUS_IDLE;
      break;

    default:
      Error_Handler();
      break;
  }
}

//void MX_IKS01A2_LSM6DSL_9AXESMode_Process(void)
//{
//  IKS01A2_MOTION_SENSOR_Event_Status_t status;
//
////  if (PushButtonDetected != 0U)
////  {
////    PushButtonDetected = 0;
////
////    /* Change this variable only if DemoFifoStatus is STATUS_IDLE */
////    if (DemoFifoStatus == STATUS_IDLE)
////    {
////      DemoFifoStatus = STATUS_SET_FIFO_MODE;
////    }
////    SendOrientationRequest  = 1;
////  }
//
////  if (SendOrientationRequest != 0U)
////  {
////    SendOrientationRequest = 0;
////
////    Send_Orientation();
////  }
//
//
////  if (MemsEventDetected != 0U)
////  {
////    MemsEventDetected = 0;
////
////    if (IKS01A2_MOTION_SENSOR_Get_Event_Status(IKS01A2_LSM6DSL_0, &status) != BSP_ERROR_NONE)
////    {
////      Error_Handler();
////    }
////
////    if (status.D6DOrientationStatus != 0U)
////    {
////      Send_Orientation();
////    }
////  }
//
//  /* Handle DEMO State Machine */
//  switch (DemoFifoStatus)
//  {
//    case STATUS_IDLE:
//      DemoFifoStatus = STATUS_SET_FIFO_MODE;
//      break;
//
//    case STATUS_SET_FIFO_MODE:
//      if (LSM6DSL_FIFO_Set_FIFO_Mode() != BSP_ERROR_NONE)
//      {
//        Error_Handler();
//      }
//      DemoFifoStatus = STATUS_FIFO_RUN;
//      break;
//
//    case STATUS_FIFO_RUN:
//      /* Get num of unread FIFO samples before reading data */
//      if (IKS01A2_MOTION_SENSOR_FIFO_Get_Num_Samples(IKS01A2_LSM6DSL_0, &num_samples) != BSP_ERROR_NONE)
//      {
//        Error_Handler();
//      }
//      /* Print dot realtime whenever new data is stored in FIFO */
//      if ((num_samples != prev_num_samples))
//      {
//        prev_num_samples = num_samples;
//        (void)snprintf(dataOut, MAX_BUF_SIZE, ".");
////        printf("%s", dataOut);
////        fflush(stdout);
//        (void)MX_APP_UART_Transmit((uint8_t *)dataOut, (uint16_t)strlen(dataOut), 5000);
//      }
//      if (MemsEventDetected == 1U)
//      {
//        DemoFifoStatus = STATUS_FIFO_DOWNLOAD;
//      }
//      break;
//
//    case STATUS_FIFO_DOWNLOAD:
//      /* Print data if FIFO is full */
//      if (IKS01A2_MOTION_SENSOR_FIFO_Get_Full_Status(IKS01A2_LSM6DSL_0, &fifo_full_status) != BSP_ERROR_NONE)
//      {
//        Error_Handler();
//      }
//      if (fifo_full_status == 1U)
//      {
//    	Send_Orientation();
//        BSP_LED_On(LED2);
//        if (LSM6DSL_Read_All_FIFO_Data() != BSP_ERROR_NONE)
//        {
//          Error_Handler();
//        }
//        HAL_Delay(INDICATION_DELAY);
//        BSP_LED_Off(LED2);
//        DemoFifoStatus = STATUS_SET_FIFO_BYPASS_MODE;
//      }
//      break;
//
//    case STATUS_SET_FIFO_BYPASS_MODE:
//      if (LSM6DSL_FIFO_Set_Bypass_Mode() != BSP_ERROR_NONE)
//      {
//        Error_Handler();
//      }
//      MemsEventDetected = 0;
//      num_samples = 0;
//      prev_num_samples = 0;
//      DemoFifoStatus = STATUS_IDLE;
//      break;
//
//    default:
//      Error_Handler();
//      break;
//  }
//}
void Get_Orientation(int *axes)
{
  uint8_t xl = 0;
  uint8_t xh = 0;
  uint8_t yl = 0;
  uint8_t yh = 0;
  uint8_t zl = 0;
  uint8_t zh = 0;

  if (IKS01A2_MOTION_SENSOR_Get_6D_Orientation_XL(IKS01A2_LSM6DSL_0, &xl) != BSP_ERROR_NONE)
  {
	(void)snprintf(dataOut, MAX_BUF_SIZE, "Error getting 6D orientation XL axis from LSM6DSL - accelerometer.\r\n");
	printf("%s", dataOut);
	return;
  }
  if (IKS01A2_MOTION_SENSOR_Get_6D_Orientation_XH(IKS01A2_LSM6DSL_0, &xh) != BSP_ERROR_NONE)
  {
	(void)snprintf(dataOut, MAX_BUF_SIZE, "Error getting 6D orientation XH axis from LSM6DSL - accelerometer.\r\n");
	printf("%s", dataOut);
	return;
  }
  if (IKS01A2_MOTION_SENSOR_Get_6D_Orientation_YL(IKS01A2_LSM6DSL_0, &yl) != BSP_ERROR_NONE)
  {
	(void)snprintf(dataOut, MAX_BUF_SIZE, "Error getting 6D orientation YL axis from LSM6DSL - accelerometer.\r\n");
	printf("%s", dataOut);
	return;
  }
  if (IKS01A2_MOTION_SENSOR_Get_6D_Orientation_YH(IKS01A2_LSM6DSL_0, &yh) != BSP_ERROR_NONE)
  {
	(void)snprintf(dataOut, MAX_BUF_SIZE, "Error getting 6D orientation YH axis from LSM6DSL - accelerometer.\r\n");
	printf("%s", dataOut);
	return;
  }
  if (IKS01A2_MOTION_SENSOR_Get_6D_Orientation_ZL(IKS01A2_LSM6DSL_0, &zl) != BSP_ERROR_NONE)
  {
	(void)snprintf(dataOut, MAX_BUF_SIZE, "Error getting 6D orientation ZL axis from LSM6DSL - accelerometer.\r\n");
	printf("%s", dataOut);
	return;
  }
  if (IKS01A2_MOTION_SENSOR_Get_6D_Orientation_ZH(IKS01A2_LSM6DSL_0, &zh) != BSP_ERROR_NONE)
  {
	(void)snprintf(dataOut, MAX_BUF_SIZE, "Error getting 6D orientation ZH axis from LSM6DSL - accelerometer.\r\n");
	printf("%s", dataOut);
	return;
  }
//  *axes=(xl<<0)|(xh<<1)|(yl<<2)|(yh<<3)|(zl<<4)|(zh<<5);
  axes[0]=xl, axes[1]=xh, axes[2]=yl, axes[3]=yh, axes[4]=zl, axes[5]=zh;
}

void DisplayOrientation(int *six_axes)
{
	  uint8_t xl = (uint8_t)six_axes[0];
	  uint8_t xh = (uint8_t)six_axes[1];
	  uint8_t yl = (uint8_t)six_axes[2];
	  uint8_t yh = (uint8_t)six_axes[3];
	  uint8_t zl = (uint8_t)six_axes[4];
	  uint8_t zh = (uint8_t)six_axes[5];
	  if (xl == 0U && yl == 0U && zl == 0U && xh == 0U && yh == 1U && zh == 0U)
	  {
	    (void)snprintf(dataOut, MAX_BUF_SIZE, "\r\n  ________________  " \
	                   "\r\n |                | " \
	                   "\r\n |  *             | " \
	                   "\r\n |                | " \
	                   "\r\n |                | " \
	                   "\r\n |                | " \
	                   "\r\n |                | " \
	                   "\r\n |________________| \r\n");
	  }

	  else if (xl == 1U && yl == 0U && zl == 0U && xh == 0U && yh == 0U && zh == 0U)
	  {
	    (void)snprintf(dataOut, MAX_BUF_SIZE, "\r\n  ________________  " \
	                   "\r\n |                | " \
	                   "\r\n |             *  | " \
	                   "\r\n |                | " \
	                   "\r\n |                | " \
	                   "\r\n |                | " \
	                   "\r\n |                | " \
	                   "\r\n |________________| \r\n");
	  }

	  else if (xl == 0U && yl == 0U && zl == 0U && xh == 1U && yh == 0U && zh == 0U)
	  {
	    (void)snprintf(dataOut, MAX_BUF_SIZE, "\r\n  ________________  " \
	                   "\r\n |                | " \
	                   "\r\n |                | " \
	                   "\r\n |                | " \
	                   "\r\n |                | " \
	                   "\r\n |                | " \
	                   "\r\n |  *             | " \
	                   "\r\n |________________| \r\n");
	  }

	  else if (xl == 0U && yl == 1U && zl == 0U && xh == 0U && yh == 0U && zh == 0U)
	  {
	    (void)snprintf(dataOut, MAX_BUF_SIZE, "\r\n  ________________  " \
	                   "\r\n |                | " \
	                   "\r\n |                | " \
	                   "\r\n |                | " \
	                   "\r\n |                | " \
	                   "\r\n |                | " \
	                   "\r\n |             *  | " \
	                   "\r\n |________________| \r\n");
	  }

	  else if (xl == 0U && yl == 0U && zl == 0U && xh == 0U && yh == 0U && zh == 1U)
	  {
	    (void)snprintf(dataOut, MAX_BUF_SIZE, "\r\n  __*_____________  " \
	                   "\r\n |________________| \r\n");
	  }

	  else if (xl == 0U && yl == 0U && zl == 1U && xh == 0U && yh == 0U && zh == 0U)
	  {
	    (void)snprintf(dataOut, MAX_BUF_SIZE, "\r\n  ________________  " \
	                   "\r\n |________________| " \
	                   "\r\n    *               \r\n");
	  }

	  else
	  {
	    (void)snprintf(dataOut, MAX_BUF_SIZE, "None of the 6D orientation axes is set in LSM6DSL - accelerometer.\r\n");
	  }

	//  printf("%s", dataOut);
	  (void)MX_APP_UART_Transmit((uint8_t *)dataOut, (uint16_t)strlen(dataOut), 5000);
}
//static void Send_Orientation(void)
//{
//  uint8_t xl = 0;
//  uint8_t xh = 0;
//  uint8_t yl = 0;
//  uint8_t yh = 0;
//  uint8_t zl = 0;
//  uint8_t zh = 0;
//
//  if (IKS01A2_MOTION_SENSOR_Get_6D_Orientation_XL(IKS01A2_LSM6DSL_0, &xl) != BSP_ERROR_NONE)
//  {
//    (void)snprintf(dataOut, MAX_BUF_SIZE, "Error getting 6D orientation XL axis from LSM6DSL - accelerometer.\r\n");
//    printf("%s", dataOut);
//    return;
//  }
//  if (IKS01A2_MOTION_SENSOR_Get_6D_Orientation_XH(IKS01A2_LSM6DSL_0, &xh) != BSP_ERROR_NONE)
//  {
//    (void)snprintf(dataOut, MAX_BUF_SIZE, "Error getting 6D orientation XH axis from LSM6DSL - accelerometer.\r\n");
//    printf("%s", dataOut);
//    return;
//  }
//  if (IKS01A2_MOTION_SENSOR_Get_6D_Orientation_YL(IKS01A2_LSM6DSL_0, &yl) != BSP_ERROR_NONE)
//  {
//    (void)snprintf(dataOut, MAX_BUF_SIZE, "Error getting 6D orientation YL axis from LSM6DSL - accelerometer.\r\n");
//    printf("%s", dataOut);
//    return;
//  }
//  if (IKS01A2_MOTION_SENSOR_Get_6D_Orientation_YH(IKS01A2_LSM6DSL_0, &yh) != BSP_ERROR_NONE)
//  {
//    (void)snprintf(dataOut, MAX_BUF_SIZE, "Error getting 6D orientation YH axis from LSM6DSL - accelerometer.\r\n");
//    printf("%s", dataOut);
//    return;
//  }
//  if (IKS01A2_MOTION_SENSOR_Get_6D_Orientation_ZL(IKS01A2_LSM6DSL_0, &zl) != BSP_ERROR_NONE)
//  {
//    (void)snprintf(dataOut, MAX_BUF_SIZE, "Error getting 6D orientation ZL axis from LSM6DSL - accelerometer.\r\n");
//    printf("%s", dataOut);
//    return;
//  }
//  if (IKS01A2_MOTION_SENSOR_Get_6D_Orientation_ZH(IKS01A2_LSM6DSL_0, &zh) != BSP_ERROR_NONE)
//  {
//    (void)snprintf(dataOut, MAX_BUF_SIZE, "Error getting 6D orientation ZH axis from LSM6DSL - accelerometer.\r\n");
//    printf("%s", dataOut);
//    return;
//  }
//
//  if (xl == 0U && yl == 0U && zl == 0U && xh == 0U && yh == 1U && zh == 0U)
//  {
//    (void)snprintf(dataOut, MAX_BUF_SIZE, "\r\n  ________________  " \
//                   "\r\n |                | " \
//                   "\r\n |  *             | " \
//                   "\r\n |                | " \
//                   "\r\n |                | " \
//                   "\r\n |                | " \
//                   "\r\n |                | " \
//                   "\r\n |________________| \r\n");
//  }
//
//  else if (xl == 1U && yl == 0U && zl == 0U && xh == 0U && yh == 0U && zh == 0U)
//  {
//    (void)snprintf(dataOut, MAX_BUF_SIZE, "\r\n  ________________  " \
//                   "\r\n |                | " \
//                   "\r\n |             *  | " \
//                   "\r\n |                | " \
//                   "\r\n |                | " \
//                   "\r\n |                | " \
//                   "\r\n |                | " \
//                   "\r\n |________________| \r\n");
//  }
//
//  else if (xl == 0U && yl == 0U && zl == 0U && xh == 1U && yh == 0U && zh == 0U)
//  {
//    (void)snprintf(dataOut, MAX_BUF_SIZE, "\r\n  ________________  " \
//                   "\r\n |                | " \
//                   "\r\n |                | " \
//                   "\r\n |                | " \
//                   "\r\n |                | " \
//                   "\r\n |                | " \
//                   "\r\n |  *             | " \
//                   "\r\n |________________| \r\n");
//  }
//
//  else if (xl == 0U && yl == 1U && zl == 0U && xh == 0U && yh == 0U && zh == 0U)
//  {
//    (void)snprintf(dataOut, MAX_BUF_SIZE, "\r\n  ________________  " \
//                   "\r\n |                | " \
//                   "\r\n |                | " \
//                   "\r\n |                | " \
//                   "\r\n |                | " \
//                   "\r\n |                | " \
//                   "\r\n |             *  | " \
//                   "\r\n |________________| \r\n");
//  }
//
//  else if (xl == 0U && yl == 0U && zl == 0U && xh == 0U && yh == 0U && zh == 1U)
//  {
//    (void)snprintf(dataOut, MAX_BUF_SIZE, "\r\n  __*_____________  " \
//                   "\r\n |________________| \r\n");
//  }
//
//  else if (xl == 0U && yl == 0U && zl == 1U && xh == 0U && yh == 0U && zh == 0U)
//  {
//    (void)snprintf(dataOut, MAX_BUF_SIZE, "\r\n  ________________  " \
//                   "\r\n |________________| " \
//                   "\r\n    *               \r\n");
//  }
//
//  else
//  {
//    (void)snprintf(dataOut, MAX_BUF_SIZE, "None of the 6D orientation axes is set in LSM6DSL - accelerometer.\r\n");
//  }
//
////  printf("%s", dataOut);
//  (void)MX_APP_UART_Transmit((uint8_t *)dataOut, (uint16_t)strlen(dataOut), 5000);
//}
/**
  * @brief  BSP Push Button callback
  * @param  Button Specifies the pin connected EXTI line
  * @retval None.
  */
void BSP_PB_Callback(Button_TypeDef Button)
{
  /* Wait until the Push Button is released */
  while (BSP_PB_GetState(BUTTON_KEY) == (uint32_t)GPIO_PIN_RESET);

  PushButtonDetected = 1;
}

/**
  * @brief  Process of the LSM6DSL FIFO Mode application
  * @retval None
  */
#if 0
void MX_IKS01A2_LSM6DSL_FIFOMode_Process(void)
{
  if (PushButtonDetected != 0U)
  {
    PushButtonDetected = 0;

    /* Change this variable only if DemoFifoStatus is STATUS_IDLE */
    if (DemoFifoStatus == STATUS_IDLE)
    {
      DemoFifoStatus = STATUS_SET_FIFO_MODE;
    }
  }

  /* Handle DEMO State Machine */
  switch (DemoFifoStatus)
  {
    case STATUS_IDLE:
      break;

    case STATUS_SET_FIFO_MODE:
      if (LSM6DSL_FIFO_Set_FIFO_Mode() != BSP_ERROR_NONE)
      {
        _Error_Handler(__FILE__, __LINE__);
      }
      DemoFifoStatus = STATUS_FIFO_RUN;
      break;

    case STATUS_FIFO_RUN:
      /* Get num of unread FIFO samples before reading data */
      if (IKS01A2_MOTION_SENSOR_FIFO_Get_Num_Samples(IKS01A2_LSM6DSL_0, &num_samples) != BSP_ERROR_NONE)
      {
        _Error_Handler(__FILE__, __LINE__);
      }
      /* Print dot realtime whenever new data is stored in FIFO */
      if (num_samples != prev_num_samples)
      {
        prev_num_samples = num_samples;
        (void)snprintf(dataOut, MAX_BUF_SIZE, ".");
        (void)MX_APP_UART_Transmit((uint8_t *)dataOut, (uint16_t)strlen(dataOut), 5000);
      }
      if (MemsEventDetected == 1U)
      {
        DemoFifoStatus = STATUS_FIFO_DOWNLOAD;
      }
      break;

    case STATUS_FIFO_DOWNLOAD:
      /* Print data if FIFO is full */
      if (IKS01A2_MOTION_SENSOR_FIFO_Get_Full_Status(IKS01A2_LSM6DSL_0, &fifo_full_status) != BSP_ERROR_NONE)
      {
        _Error_Handler(__FILE__, __LINE__);
      }
      if (fifo_full_status == 1U)
      {
        BSP_LED_On(LED2);
        if (LSM6DSL_Read_All_FIFO_Data() != BSP_ERROR_NONE)
        {
          _Error_Handler(__FILE__, __LINE__);
        }
        HAL_Delay(INDICATION_DELAY);
        BSP_LED_Off(LED2);
        DemoFifoStatus = STATUS_SET_FIFO_BYPASS_MODE;
      }
      break;

    case STATUS_SET_FIFO_BYPASS_MODE:
      if (LSM6DSL_FIFO_Set_Bypass_Mode() != BSP_ERROR_NONE)
      {
        _Error_Handler(__FILE__, __LINE__);
      }
      MemsEventDetected = 0;
      num_samples = 0;
      prev_num_samples = 0;
      DemoFifoStatus = STATUS_IDLE;
      break;

    default:
      _Error_Handler(__FILE__, __LINE__);
      break;
  }
}
#endif
/**
  * @brief  Configure FIFO
  * @retval BSP status
  */
static int32_t LSM6DSL_FIFO_Demo_Config(void)
{
  int32_t ret;

  if ((ret = IKS01A2_MOTION_SENSOR_SetOutputDataRate(IKS01A2_LSM6DSL_0, MOTION_GYRO,
                                                     LSM6DSL_SAMPLE_ODR)) != BSP_ERROR_NONE)
  {
    return ret;
  }

  /* Set gyro FIFO decimation */
  if ((ret = IKS01A2_MOTION_SENSOR_FIFO_Set_Decimation(IKS01A2_LSM6DSL_0, MOTION_GYRO,
                                                       (uint8_t)LSM6DSL_FIFO_GY_NO_DEC)) != BSP_ERROR_NONE)
  {
    return ret;
  }

  /* Set FIFO ODR to highest value */
  if ((ret = IKS01A2_MOTION_SENSOR_FIFO_Set_ODR_Value(IKS01A2_LSM6DSL_0, LSM6DSL_FIFO_MAX_ODR)) != BSP_ERROR_NONE)
  {
    return ret;
  }

  /* Set FIFO_FULL on INT1 */
  if ((ret = IKS01A2_MOTION_SENSOR_FIFO_Set_INT1_FIFO_Full(IKS01A2_LSM6DSL_0, ENABLE)) != BSP_ERROR_NONE)
  {
    return ret;
  }

  /* Set FIFO watermark */
  if ((ret = IKS01A2_MOTION_SENSOR_FIFO_Set_Watermark_Level(IKS01A2_LSM6DSL_0, FIFO_WATERMARK)) != BSP_ERROR_NONE)
  {
    return ret;
  }

  /* Set FIFO depth to be limited to watermark threshold level  */
  if ((ret = IKS01A2_MOTION_SENSOR_FIFO_Set_Stop_On_Fth(IKS01A2_LSM6DSL_0, ENABLE)) != BSP_ERROR_NONE)
  {
    return ret;
  }

  return ret;
}

/**
  * @brief  Set FIFO bypass mode
  * @retval BSP status
  */
static int32_t LSM6DSL_FIFO_Set_Bypass_Mode(void)
{
  int32_t ret;

  if ((ret = IKS01A2_MOTION_SENSOR_FIFO_Set_Mode(IKS01A2_LSM6DSL_0, (uint8_t)LSM6DSL_BYPASS_MODE)) != BSP_ERROR_NONE)
  {
    return ret;
  }

  (void)snprintf(dataOut, MAX_BUF_SIZE, "Press USER button to start the DEMO...\r\n");
  (void)MX_APP_UART_Transmit((uint8_t *)dataOut, (uint16_t)strlen(dataOut), 5000);

  return ret;
}

/**
  * @brief  Set FIFO to FIFO mode
  * @retval BSP status
  */
static int32_t LSM6DSL_FIFO_Set_FIFO_Mode(void)
{
  int32_t ret;

  (void)snprintf(dataOut, MAX_BUF_SIZE, "\r\nLSM6DSL starts to store the data into FIFO...\r\n\r\n");
  (void)MX_APP_UART_Transmit((uint8_t *)dataOut, (uint16_t)strlen(dataOut), 5000);

  HAL_Delay(1000);

  /* Set FIFO mode to FIFO */
  if ((ret = IKS01A2_MOTION_SENSOR_FIFO_Set_Mode(IKS01A2_LSM6DSL_0, (uint8_t)LSM6DSL_FIFO_MODE)) != BSP_ERROR_NONE)
  {
    return ret;
  }

  return ret;
}

/**
  * @brief  Read all unread FIFO data in cycle
  * @retval BSP status
  */
static int32_t LSM6DSL_Read_All_FIFO_Data(int *three_axes)
{
  uint16_t samples_to_read = 0;
  uint16_t i;
  int32_t ret;

  /* Get num of unread FIFO samples before reading data */
  if ((ret = IKS01A2_MOTION_SENSOR_FIFO_Get_Num_Samples(IKS01A2_LSM6DSL_0, &samples_to_read)) != BSP_ERROR_NONE)
  {
    return ret;
  }

  /* 'samples_to_read' actually contains number of words in FIFO but each FIFO sample (data set) consists of 3 words
  so the 'samples_to_read' has to be divided by 3 */
  samples_to_read /= 3U;

  (void)snprintf(dataOut, MAX_BUF_SIZE,
                 "\r\n\r\n%d samples in FIFO.\r\n\r\nStarted downloading data from FIFO...\r\n\r\n", samples_to_read);
  (void)MX_APP_UART_Transmit((uint8_t *)dataOut, (uint16_t)strlen(dataOut), 5000);

  HAL_Delay(1000);

  (void)snprintf(dataOut, MAX_BUF_SIZE, "[DATA ##]     GYR_X     GYR_Y     GYR_Z\r\n");
  (void)MX_APP_UART_Transmit((uint8_t *)dataOut, (uint16_t)strlen(dataOut), 5000);

  //AL
  samples_to_read=1;
  //AL
  for (i = 0; i < samples_to_read; i++)
  {
    if ((ret = LSM6DSL_Read_Single_FIFO_Pattern_Cycle(i,three_axes)) != BSP_ERROR_NONE)
    {
      return ret;
    }
  }

  (void)snprintf(dataOut, MAX_BUF_SIZE, "\r\nFIFO download completed.\r\n\r\n");
  (void)MX_APP_UART_Transmit((uint8_t *)dataOut, (uint16_t)strlen(dataOut), 5000);

  return ret;
}
//static int32_t LSM6DSL_Read_All_FIFO_Data(void)
//{
//  uint16_t samples_to_read = 0;
//  uint16_t i;
//  int32_t ret;
//
//  /* Get num of unread FIFO samples before reading data */
//  if ((ret = IKS01A2_MOTION_SENSOR_FIFO_Get_Num_Samples(IKS01A2_LSM6DSL_0, &samples_to_read)) != BSP_ERROR_NONE)
//  {
//    return ret;
//  }
//
//  /* 'samples_to_read' actually contains number of words in FIFO but each FIFO sample (data set) consists of 3 words
//  so the 'samples_to_read' has to be divided by 3 */
//  samples_to_read /= 3U;
//
//  (void)snprintf(dataOut, MAX_BUF_SIZE,
//                 "\r\n\r\n%d samples in FIFO.\r\n\r\nStarted downloading data from FIFO...\r\n\r\n", samples_to_read);
//  (void)MX_APP_UART_Transmit((uint8_t *)dataOut, (uint16_t)strlen(dataOut), 5000);
//
//  HAL_Delay(1000);
//
//  (void)snprintf(dataOut, MAX_BUF_SIZE, "[DATA ##]     GYR_X     GYR_Y     GYR_Z\r\n");
//  (void)MX_APP_UART_Transmit((uint8_t *)dataOut, (uint16_t)strlen(dataOut), 5000);
//
//  //AL
//  samples_to_read=1;
//  //AL
//  for (i = 0; i < samples_to_read; i++)
//  {
//    if ((ret = LSM6DSL_Read_Single_FIFO_Pattern_Cycle(i)) != BSP_ERROR_NONE)
//    {
//      return ret;
//    }
//  }
//
//  (void)snprintf(dataOut, MAX_BUF_SIZE, "\r\nFIFO download completed.\r\n\r\n");
//  (void)MX_APP_UART_Transmit((uint8_t *)dataOut, (uint16_t)strlen(dataOut), 5000);
//
//  return ret;
//}
void Display3Axes(int *three_axes)
{
	  int SampleIndex=1;
	  (void)snprintf(dataOut, MAX_BUF_SIZE, "[DATA %02d]  %8ld  %8ld  %8ld\r\n", SampleIndex + 1U, three_axes[0], three_axes[1], three_axes[2]);
	  (void)MX_APP_UART_Transmit((uint8_t *)dataOut, (uint16_t)strlen(dataOut), 5000);
}
/**
  * @brief  Read single FIFO pattern cycle
  * @param  SampleIndex Current sample index.
  * @retval BSP status
  */
static int32_t LSM6DSL_Read_Single_FIFO_Pattern_Cycle(uint16_t SampleIndex, int *three_axes)
{
  uint16_t pattern = 0;
  int32_t angular_velocity = 0;
  int32_t gyr_x = 0, gyr_y = 0, gyr_z = 0;
  int32_t ret = BSP_ERROR_NONE;
  int i;

  /* Read one whole FIFO pattern cycle. Pattern: Gx, Gy, Gz */
  for (i = 0; i <= 2; i++)
  {
    /* Read FIFO pattern number */
    if ((ret = IKS01A2_MOTION_SENSOR_FIFO_Get_Pattern(IKS01A2_LSM6DSL_0, &pattern)) != BSP_ERROR_NONE)
    {
      return ret;
    }

    /* Read single FIFO data (angular velocity in one axis) */
    if ((ret = IKS01A2_MOTION_SENSOR_FIFO_Get_Axis(IKS01A2_LSM6DSL_0, MOTION_GYRO, &angular_velocity)) != BSP_ERROR_NONE)
    {
      return ret;
    }

    /* Decide which axis has been read from FIFO based on pattern number */
    switch (pattern)
    {
      case PATTERN_GYR_X_AXIS:
        gyr_x = angular_velocity;
        break;

      case PATTERN_GYR_Y_AXIS:
        gyr_y = angular_velocity;
        break;

      case PATTERN_GYR_Z_AXIS:
        gyr_z = angular_velocity;
        break;

      default:
        ret = BSP_ERROR_UNKNOWN_FAILURE;
        break;
    }
  }

  if (ret != BSP_ERROR_NONE)
  {
    return ret;
  }

  if(three_axes)
  {
	  three_axes[0]=gyr_x, three_axes[1]=gyr_y, three_axes[2]=gyr_z;
  }
  else
	  ret=BSP_ERROR_UNKNOWN_FAILURE;
//  (void)snprintf(dataOut, MAX_BUF_SIZE, "[DATA %02d]  %8ld  %8ld  %8ld\r\n", SampleIndex + 1U, gyr_x, gyr_y, gyr_z);
//  (void)MX_APP_UART_Transmit((uint8_t *)dataOut, (uint16_t)strlen(dataOut), 5000);

  return ret;
}
//static int32_t LSM6DSL_Read_Single_FIFO_Pattern_Cycle(uint16_t SampleIndex)
//{
//  uint16_t pattern = 0;
//  int32_t angular_velocity = 0;
//  int32_t gyr_x = 0, gyr_y = 0, gyr_z = 0;
//  int32_t ret = BSP_ERROR_NONE;
//  int i;
//
//  /* Read one whole FIFO pattern cycle. Pattern: Gx, Gy, Gz */
//  for (i = 0; i <= 2; i++)
//  {
//    /* Read FIFO pattern number */
//    if ((ret = IKS01A2_MOTION_SENSOR_FIFO_Get_Pattern(IKS01A2_LSM6DSL_0, &pattern)) != BSP_ERROR_NONE)
//    {
//      return ret;
//    }
//
//    /* Read single FIFO data (angular velocity in one axis) */
//    if ((ret = IKS01A2_MOTION_SENSOR_FIFO_Get_Axis(IKS01A2_LSM6DSL_0, MOTION_GYRO, &angular_velocity)) != BSP_ERROR_NONE)
//    {
//      return ret;
//    }
//
//    /* Decide which axis has been read from FIFO based on pattern number */
//    switch (pattern)
//    {
//      case PATTERN_GYR_X_AXIS:
//        gyr_x = angular_velocity;
//        break;
//
//      case PATTERN_GYR_Y_AXIS:
//        gyr_y = angular_velocity;
//        break;
//
//      case PATTERN_GYR_Z_AXIS:
//        gyr_z = angular_velocity;
//        break;
//
//      default:
//        ret = BSP_ERROR_UNKNOWN_FAILURE;
//        break;
//    }
//  }
//
//  if (ret != BSP_ERROR_NONE)
//  {
//    return ret;
//  }
//
//  (void)snprintf(dataOut, MAX_BUF_SIZE, "[DATA %02d]  %8ld  %8ld  %8ld\r\n", SampleIndex + 1U, gyr_x, gyr_y, gyr_z);
//  (void)MX_APP_UART_Transmit((uint8_t *)dataOut, (uint16_t)strlen(dataOut), 5000);
//
//  return ret;
//}

#ifdef __cplusplus
}
#endif

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
