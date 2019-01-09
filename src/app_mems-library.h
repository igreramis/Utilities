/**
  ******************************************************************************
  * File Name          :  app_mems-library.h
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __INIT_H
#define __INIT_H
#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
/* Exported Functions --------------------------------------------------------*/
void MX_MEMS_Library_Init(void);
void MX_MEMS_Library_Process(void);

void lx_task(void);
//void Get_Orientation(void);
void Get_Orientation(int *);

//AL
typedef enum
{
  STATUS_IDLE,
  STATUS_SET_FIFO_MODE,
  STATUS_FIFO_RUN,
  STATUS_FIFO_DOWNLOAD,
  STATUS_SET_FIFO_BYPASS_MODE
} DEMO_FIFO_STATUS_t;
//AL
#ifdef __cplusplus
}
#endif
#endif /* __INIT_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
