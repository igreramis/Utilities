/**
  ******************************************************************************
  * @file    main.c
  * @author  Ac6
  * @version V1.0
  * @date    01-December-2013
  * @brief   Default main function.
  ******************************************************************************
*/

#include "main.h"
#include "stm32l1xx.h"
#include "stm32l1xx_nucleo.h"
//#include "FreeRTOS.h"
//#include "timers.h"
//#include "task.h"
#include "projdefs.h"
#include "cmsis_os.h"

#include "app_mems-library.h"
//AL
#include "flash.h"
#include <string.h>
static char dataOut[FLASH_PAGE_SIZE];
static char dataIn[FLASH_PAGE_SIZE];

//AL

/* USER CODE BEGIN Includes */
/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
void MX_USART2_UART_Init(void);

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/

/* USER CODE END PFP */

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  *
  * @retval None
  */
void vTimerCallback( TimerHandle_t xTimer );
typedef enum
{
  THREAD_1 = 0,
  THREAD_2
} Thread_TypeDef;
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
osThreadId LEDThread1Handle, LEDThread2Handle;
#define USERBUTTON_TASK_STACK_SIZE        ( configMINIMAL_STACK_SIZE * 4 )
void vTaskUserButton();
extern volatile uint8_t PushButtonDetected;
static volatile uint8_t StartDataCollection=0;
extern DEMO_FIFO_STATUS_t DemoFifoStatus;
extern uint8_t SendOrientationRequest;
/* Private function prototypes -----------------------------------------------*/
//AL
#include "core_cm3.h"
uint32_t Address = 0;
uint32_t PageError = 0;
uint32_t ProtectedPAGE = 0x0;
__IO TestStatus MemoryProgramStatus = PASSED;
FLASH_EraseInitTypeDef EraseInitStruct;
/*Variable used to handle the Options Bytes*/
FLASH_OBProgramInitTypeDef OptionsBytesStruct;
void test_flash();
//void print(char *);
//AL

void get_user_input()
{
  (void)snprintf(dataOut, FLASH_PAGE_SIZE, "Press USER button to start the DEMO or press 'r' to retrieve saved data...\r\n");
  (void)MX_APP_UART_Transmit((uint8_t *)dataOut, (uint16_t)strlen(dataOut), 5000);

  /*See if user wants to see sensor data printed on the console */
  if(MX_APP_UART_Receive((uint8_t *)dataIn, 1, 1000)==HAL_OK)
  {
    if(dataIn[0]=='r'||dataIn[0]=='R')
    {
      print_data();
    }
  }
}

int main(void)
{
  /* MCU Configuration----------------------------------------------------------*/
  BaseType_t xReturned;
  TaskHandle_t xUserButtonHandle = NULL;


  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  SystemClock_Config();

  /* Initialize LED */
  MX_GPIO_Init();
  MX_MEMS_Library_Init();

  TimerHandle_t xTimer;

  xReturned = xTaskCreate(
            vTaskUserButton,       /* Function that implements the task. */
            "UserButton",          /* Text name for the task. */
          USERBUTTON_TASK_STACK_SIZE,      /* Stack size in words, not bytes. */
            NULL,    /* Parameter passed into the task. */
            tskIDLE_PRIORITY,/* Priority at which the task is created. */
            &xUserButtonHandle );      /* Used to pass out the created task's handle. */

  if( xReturned != pdPASS )
  {
    return -1;
  }

  xTimer=xTimerCreate("Timer",500,pdTRUE,(void *)0,vTimerCallback);
  if(xTimer==NULL)
  {
  }
  else
  {
    if(xTimerStart(xTimer,0)!=pdPASS)
    {
      return -1;
    }
  }
  // test_flash();
  test_flash_structs();

  get_user_input();

  osKernelStart();

  for (;;);

}

//void vTaskInitFlash(void)
//{
//  erase_pages(ADDR_FLASH_PAGE_33,(ADDR_FLASH_PAGE_34-ADDR_FLASH_PAGE_33)/FLASH_PAGE_SIZE);
//  while(1);
//}

//what should be the size of the buffer?
//how much data do you need to put out?

typedef struct 
{
  int array[2];
}test_flash_struct;

void test_flash_structs()
{
  test_flash_struct st_data_write = {-1300, 1400};
  test_flash_struct st_data_read;
  
  erase_pages(ADDR_FLASH_PAGE_293,(ADDR_FLASH_PAGE_294-ADDR_FLASH_PAGE_293)/FLASH_PAGE_SIZE);
  write_page_without_erase(&st_data_write,ADDR_FLASH_PAGE_293, sizeof(test_flash_struct));
  if(MemoryProgramStatus == FAILED)
  {
    throw_error_via_led();
  }

  read_page(&st_data_read, ADDR_FLASH_PAGE_293, sizeof(test_flash_struct));
  if(MemoryProgramStatus == FAILED)
  {
    throw_error_via_led();
  }
  if (memcmp(&st_data_write, &st_data_read, sizeof(test_flash_struct))!=0)
  {
    throw_error_via_led();
  }

  erase_pages(ADDR_FLASH_PAGE_294,(ADDR_FLASH_PAGE_294-ADDR_FLASH_PAGE_293)/FLASH_PAGE_SIZE);
  write_page_without_erase(&st_data_write,ADDR_FLASH_PAGE_294, sizeof(test_flash_struct));
  if(MemoryProgramStatus == FAILED)
  {
    throw_error_via_led();
  }

  read_page(&st_data_read, ADDR_FLASH_PAGE_294, sizeof(test_flash_struct));
  if(MemoryProgramStatus == FAILED)
  {
    throw_error_via_led();
  }
  if (memcmp(&st_data_write, &st_data_read, sizeof(test_flash_struct))!=0)
  {
    throw_error_via_led();
  }  
}

void test_flash()
{
#define MAX_BUF_SIZE 256
  char dataOut[MAX_BUF_SIZE];
  uint32_t bytes_written=0, itr,start_addr=ADDR_FLASH_PAGE_293;
  uint32_t val = DATA_32;
  uint32_t buffer[FLASH_PAGE_SIZE/sizeof(uint32_t)];
  uint32_t buffer_read[FLASH_PAGE_SIZE/sizeof(uint32_t)];


  erase_pages(ADDR_FLASH_PAGE_293,(ADDR_FLASH_PAGE_294-ADDR_FLASH_PAGE_293)/FLASH_PAGE_SIZE);
  write_page_without_erase(buffer,ADDR_FLASH_PAGE_293, FLASH_PAGE_SIZE);
  if(MemoryProgramStatus == FAILED)
  {
    throw_error_via_led();
  }

  read_page(buffer_read, ADDR_FLASH_PAGE_293, FLASH_PAGE_SIZE);
  if(MemoryProgramStatus == FAILED)
  {
    throw_error_via_led();
  }
  if (memcmp(buffer_read, buffer, FLASH_PAGE_SIZE)!=0)
  {
    throw_error_via_led();
  }
}

void vTaskUserButton(void)
{
  while(1)
  {
    if (PushButtonDetected != 0U)
    {
      PushButtonDetected = 0U;
      /* Change this variable only if DemoFifoStatus is STATUS_IDLE */
//      if (DemoFifoStatus == STATUS_IDLE)
//      {
         DemoFifoStatus = STATUS_SET_FIFO_MODE;
//      }
      SendOrientationRequest  = 1;

      StartDataCollection=(StartDataCollection==1)?0U:1U;
      if (!StartDataCollection)
      {
        get_user_input();
      }
    }
  }
}

void vTimerCallback( TimerHandle_t xTimer )
{
  static uint32_t num_samples=0;
  if(StartDataCollection)
  {
    BSP_LED_Toggle(LED2);
    MX_MEMS_Library_Process();
//    MX_MEMS_Library_Process(nine_axes);
    num_samples++;
    (void)snprintf(dataOut, FLASH_PAGE_SIZE, "Samples collected: %d\r\n", num_samples);
    (void)MX_APP_UART_Transmit((uint8_t *)dataOut, (uint16_t)strlen(dataOut), 5000);
  }
  else
  {
    /*See if user wants to see sensor data printed on the console */
    if(MX_APP_UART_Receive((uint8_t *)dataIn, 1, 10)==HAL_OK)
    {
      if(dataIn[0]=='r'||dataIn[0]=='R')
      {
        print_data();
      }
    }
  }
}

//void print(char *str, char *strII)
//{
//  char dataOut[MAX_BUF_SIZE];
//  (void)snprintf(dataOut, MAX_BUF_SIZE, str, strII);
//  (void)MX_APP_UART_Transmit((uint8_t *)dataOut, (uint16_t)strlen(dataOut), 5000);
//}
/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;

  /**Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /**Initializes the CPU, AHB and APB busses clocks
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = 16;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL6;
  RCC_OscInitStruct.PLL.PLLDIV = RCC_PLL_DIV3;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  /**Initializes the CPU, AHB and APB busses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
                                | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  /**Configure the Systick interrupt time
  */
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq() / 1000);

  /**Configure the Systick
  */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/* USART2 init function */
void MX_USART2_UART_Init(void)
{

  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/** Configure pins as
        * Analog
        * Input
        * Output
        * EVENT_OUT
        * EXTI
*/
static void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct;

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : LD2_Pin */
  GPIO_InitStruct.Pin = LD2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LD2_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : PB5 */
  GPIO_InitStruct.Pin = GPIO_PIN_5;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  file: The file name as string.
  * @param  line: The line in file as a number.
  * @retval None
  */
void _Error_Handler(char *file, int line)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
