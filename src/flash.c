#include "flash.h"

/*Variable used for Erase procedure*/
FLASH_EraseInitTypeDef EraseInitStruct;
/*Variable used to handle the Options Bytes*/
FLASH_OBProgramInitTypeDef OptionsBytesStruct;
//extern uint32_t Address;
uint32_t PageError;
extern uint32_t ProtectedPAGE;
extern __IO TestStatus MemoryProgramStatus;
//extern void print(char *);

void erase_pages(uint32_t start_page_addr, uint32_t num_pages)
{
  HAL_FLASH_Unlock();

  /* Unlock the Options Bytes *************************************************/
//  HAL_FLASH_OB_Unlock();
//
//  /* Get pages write protection status ****************************************/
//  HAL_FLASHEx_OBGetConfig(&OptionsBytesStruct);

//  if((OptionsBytesStruct.WRPSector0To31 & FLASH_PAGE_TO_BE_PROTECTED) == FLASH_PAGE_TO_BE_PROTECTED)
//  {
//	  /* Error occurred while options bytes programming. **********************/
//	  while (1)
//	  {
//		/* Make LED2 blink (100ms on, 2s off) to indicate error in OB programming operation */
//		BSP_LED_On(LED2);
//		HAL_Delay(100);
//		BSP_LED_Off(LED2);
//		HAL_Delay(2000);
//	  }
//  }
  //  /* Lock the Options Bytes */
//  HAL_FLASH_OB_Lock();

  /* The selected pages are not write protected *******************************/
  if ((OptionsBytesStruct.WRPSector0To31 & FLASH_PAGE_TO_BE_PROTECTED) == 0x00)
  {
	/* Fill EraseInit structure************************************************/
	EraseInitStruct.TypeErase   = FLASH_TYPEERASE_PAGES;
	EraseInitStruct.PageAddress = start_page_addr;
	EraseInitStruct.NbPages     = num_pages;

//	if(HAL_FLASHEx_OBProgram(&OptionsBytesStruct) != HAL_OK)
//	    {
//	      /* Error occurred while options bytes programming. **********************/
//	      while (1)
//	      {
//	        /* Make LED2 blink (100ms on, 2s off) to indicate error in OB programming operation */
//	        BSP_LED_On(LED2);
//	        HAL_Delay(100);
//	        BSP_LED_Off(LED2);
//	        HAL_Delay(2000);
//	      }
//	    }
//	    /* Generate System Reset to load the new option byte values ***************/
//	    HAL_FLASH_OB_Launch();
//    }

	if (HAL_FLASHEx_Erase(&EraseInitStruct, &PageError) != HAL_OK)
	{
	  /*
		Error occurred while page erase.
		User can add here some code to deal with this error.
		PageError will contain the faulty page and then to know the code error on this page,
		user can call function 'HAL_FLASH_GetError()'
	  */
	  while (1)
	  {
		/* Make LED2 blink (100ms on, 2s off) to indicate error in Erase operation */
		BSP_LED_On(LED2);
		HAL_Delay(100);
		BSP_LED_Off(LED2);
		HAL_Delay(2000);
	  }
	}
  }
  /* Lock the Options Bytes */
//  HAL_FLASH_OB_Lock();

  HAL_FLASH_Lock();
}

void throw_error_via_led()
{
	while (1)
	{
	  /* Make LED2 blink (100ms on, 2s off) to indicate error in Write operation */
	  BSP_LED_On(LED2);
	  HAL_Delay(100);
	  BSP_LED_Off(LED2);
	  HAL_Delay(2000);
	}

}

void write_page_without_erase(void * buffer,uint32_t page_st_addr, size_t size)
{
	HAL_FLASH_Unlock();
	uint32_t bytes_written=0, start_addr=page_st_addr, *buffer_uint32=(uint32_t *)buffer;
	while (bytes_written<size)//while (bytes_written<FLASH_PAGE_SIZE)
	{
		if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, start_addr, *buffer_uint32) == HAL_OK)//if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, start_addr, *((uint32_t*)buffer)) == HAL_OK)
		{
			start_addr+=4;
			buffer_uint32+=1;
			bytes_written+=4;
		}
		else
		{
		/* Error occurred while writing data in Flash memory.
		   User can add here some code to deal with this error */
		   throw_error_via_led();
		}
	}

//	bytes_written = 0;
//	while (bytes_written < FLASH_PAGE_SIZE-size)
//	{
//		if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, start_addr, DATA_32) == HAL_OK)//if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, start_addr, *((uint32_t*)buffer)) == HAL_OK)
//		{
//			start_addr+=4;
//			bytes_written+=4;
//		}
//		else
//		{
//		/* Error occurred while writing data in Flash memory.
//		   User can add here some code to deal with this error */
//		   throw_error_via_led();
//		}
//	}

	start_addr=page_st_addr;
	buffer_uint32=(uint32_t *)buffer;
	/* Check the correctness of written data */
	while (start_addr < page_st_addr+size)
	{
	  if((*(__IO uint32_t*) start_addr) != *buffer_uint32)
	  {
		MemoryProgramStatus = FAILED;
	  }
	  start_addr += 4;
	  buffer_uint32+=1;
	}

	HAL_FLASH_Lock();
}

void read_page(void * buffer, uint32_t page_st_addr, size_t size)
{
	HAL_FLASH_Unlock();
	uint32_t bytes_written=0, start_addr=page_st_addr, *buffer_uint32=(uint32_t *)buffer;
//	uint32_t buffer_fuck[FLASH_PAGE_SIZE];
//	uint32_t *pt_buffer_fuck = buffer_fuck;

	while (start_addr < page_st_addr+size)
	{
	  *buffer_uint32 = *(__IO uint32_t*) start_addr;

	  start_addr += 4;
	  buffer_uint32+=1;
	}

//	start_addr = page_st_addr+size;
//	while (start_addr < page_st_addr+FLASH_PAGE_SIZE)
//	{
//	  *pt_buffer_fuck = *(__IO uint32_t*) start_addr;
//
//	  start_addr += 4;
//	  pt_buffer_fuck+=1;
//	}

	HAL_FLASH_Lock();
}
