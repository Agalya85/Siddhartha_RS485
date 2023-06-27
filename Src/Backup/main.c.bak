/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/*
 * Update History : 09-02-2023 by UT
 * Firmware version - BSL_Prod_2_0_3_080223_PortExit_Phy1
 * Changes - Time stamp is written in EEPROM before 30 sec of internal watchdog restart.
 * Time stamp is read from EEPROM at the power on.
 *
 *
 */

/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "dma.h"
#include "i2c.h"
#include "iwdg.h"
#include "lwip.h"
#include "rtc.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "applicationdefines.h"
#include "externs.h"
#include <string.h>
#include "user_adc.h"
#include "user_rtc.h"
#include "user_eeprom.h"
#include "user_timer.h"
#include "deviceinfo.h"
#include "modbus_rs485.h"
#include "modbus_tcp.h"
#include "gsmSim7600.h"
#include "payload.h"
#include "queue.h"
#include "user_ethernet.h"
#include "remote_configuration.h"
#include "serial_flash_w25_fsm.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint32_t gu32OnstartEthernetStatusAlert = 0;
extern strctQUEUE gsmPayload;
extern strctGSM gsmInstance;
uint32_t u32MBCycleCompCouter = 0;
_Bool bitFlagRTCBackupUpdated = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void SystemClock_Config_Ext(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
	uint8_t year = 0;
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */
  /* Check if configuration is present in the memory */
  getLastKnownConfiguration();
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_IWDG_Init();
  MX_TIM7_Init();
  MX_UART4_Init();
  MX_RTC_Init();
  MX_USART3_UART_Init();
  MX_ADC1_Init();
  MX_LWIP_Init();
  MX_I2C1_Init();
  MX_SPI1_Init();
  /* USER CODE BEGIN 2 */
//  HAL_ETH_WritePHYRegister(&heth, PHY_AUTONEGOTIATION, 1);

  updateDeviceSignature();
	HAL_GPIO_WritePin(MB_DE_GPIO_Port,MB_DE_Pin,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(MB_RE_GPIO_Port,MB_RE_Pin,GPIO_PIN_SET);
	HAL_GPIO_WritePin(LED_1_GPIO_Port, LED_1_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LED_2_GPIO_Port, LED_2_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LED_3_GPIO_Port, LED_3_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GSM_PWR_KEY_GPIO_Port,GSM_PWR_KEY_Pin,GPIO_PIN_RESET);

	setupModbus();
	setupModbusRTU();

	if(gu32UploadTimeSupply == 0)
		gu32UploadTimeSupply = ONE_MIN;//THIRTY_SEC;//TWO_MIN;

	if(gu32UploadTimeBatt == 0)
		gu32UploadTimeBatt = FIVE_MIN;

	LL_USART_EnableIT_RXNE(USART3);
	LL_USART_EnableIT_RXNE(UART4);
	if(HAL_TIM_Base_Start_IT(&htim7)!= HAL_OK)
	{
		Error_Handler();
	}
	/*Added on 28-1-22 . APN from configuration*/
	updateNetworkAPN();
	gu32TempPayloadQueueEnqueue = ONE_MIN;
	memset(gau8TempMBPayloadString,0x00,sizeof(char) * gu32MBDataByteCounterLimit);
	memset(gau8TempMBRTUPayloadString,0x00,sizeof(char) * gu32MBDataByteCounterLimitRTU);

	/*Flash Erase for Testing Purpose*/
//	Flash_Erase();

#if(ENABLEMEMORYSPI == TRUE)
	if(HAL_I2C_Mem_Read_IT(&hi2c1, EEPROM_ADDRESS,292,I2C_MEMADD_SIZE_16BIT,(uint8_t *)&strI2cEeprom.pu8SFlashReadMemLocAddress, 4) != HAL_OK)
	{
//		assertError(enmTORERRORS_MEM_I2C,enmERRORSTATE_ACTIVE);
	}
	else
	{
//		assertError(enmTORERRORS_MEM_I2C,enmERRORSTATE_NOERROR);
		HAL_Delay(10);
		strI2cEeprom.u32RdSFlashReadMemLocAddress = strI2cEeprom.pu8SFlashReadMemLocAddress[3] << 24;
		strI2cEeprom.u32RdSFlashReadMemLocAddress |= strI2cEeprom.pu8SFlashReadMemLocAddress[2] << 16;
		strI2cEeprom.u32RdSFlashReadMemLocAddress |= strI2cEeprom.pu8SFlashReadMemLocAddress[1] << 8;
		strI2cEeprom.u32RdSFlashReadMemLocAddress |= strI2cEeprom.pu8SFlashReadMemLocAddress[0];
		if((strI2cEeprom.u32RdSFlashReadMemLocAddress == 0xFFFFFFFF) || (strI2cEeprom.u32RdSFlashReadMemLocAddress >= SFLASH_SIZE))
			strI2cEeprom.u32RdSFlashReadMemLocAddress = 0;
		FlashStr.u32FlashReadAddress = strI2cEeprom.u32RdSFlashReadMemLocAddress;

		// Data lost due to reset is handled
		if(FlashStr.u32FlashReadAddress != 0 && FlashStr.u32FlashReadAddress > SFLASH_UPLOAD_LENGTH)
			FlashStr.u32FlashReadAddress -= SFLASH_UPLOAD_LENGTH;
	}
	if(HAL_I2C_Mem_Read_IT(&hi2c1, EEPROM_ADDRESS,288,I2C_MEMADD_SIZE_16BIT,(uint8_t *)&strI2cEeprom.pu8SFlashWriteMemLocAddress, 4) != HAL_OK)
	{
//		assertError(enmTORERRORS_MEM_I2C,enmERRORSTATE_ACTIVE);
	}
	else
	{
//		assertError(enmTORERRORS_MEM_I2C,enmERRORSTATE_NOERROR);
		HAL_Delay(10);
		strI2cEeprom.u32RdSFlashWriteMemLocAddress = strI2cEeprom.pu8SFlashWriteMemLocAddress[3] << 24;
		strI2cEeprom.u32RdSFlashWriteMemLocAddress |= strI2cEeprom.pu8SFlashWriteMemLocAddress[2] << 16;
		strI2cEeprom.u32RdSFlashWriteMemLocAddress |= strI2cEeprom.pu8SFlashWriteMemLocAddress[1] << 8;
		strI2cEeprom.u32RdSFlashWriteMemLocAddress |= strI2cEeprom.pu8SFlashWriteMemLocAddress[0];
		if((strI2cEeprom.u32RdSFlashWriteMemLocAddress == 0xFFFFFFFF) || (strI2cEeprom.u32RdSFlashWriteMemLocAddress >= SFLASH_SIZE))
		{
			strI2cEeprom.u32RdSFlashWriteMemLocAddress = 0;
		}
		FlashStr.u32FlashWriteAddress = strI2cEeprom.u32RdSFlashWriteMemLocAddress;
	}
	FlashStr.u32SerialFlashDelay = FIVEHUNDRED_MS;
	FlashStr.bitFlagQueueDataWrite = 0;
#endif
/* 	I2C function to write flash read & write pointer in eeprom */
/*	HAL_I2C_Mem_Write_IT(&hi2c1, EEPROM_ADDRESS, 292,I2C_MEMADD_SIZE_16BIT,(uint8_t *)&strI2cEeprom.pu8SFlashReadMemLocAddress,4);
	HAL_Delay(10);
	HAL_I2C_Mem_Write_IT(&hi2c1, EEPROM_ADDRESS, 288,I2C_MEMADD_SIZE_16BIT,(uint8_t *)&strI2cEeprom.pu8SFlashWriteMemLocAddress,4);
	HAL_Delay(10);*/

	// RTC time stamp data read from EEPROM
#if(ENABLE_RTC_EEPROM_BACKUP == TRUE)
	if(HAL_I2C_Mem_Read_IT(&hi2c1, EEPROM_ADDRESS,296,I2C_MEMADD_SIZE_16BIT,(uint8_t *)&strI2cEeprom.pu8STimeStampBackup, 6) != HAL_OK)
	{
//		assertError(enmTORERRORS_MEM_I2C,enmERRORSTATE_ACTIVE);
	}
	else
	{
		HAL_Delay(10);
		STime1.Hours = DecimalToBCD(strI2cEeprom.pu8STimeStampBackup[3]);
		STime1.Minutes = DecimalToBCD(strI2cEeprom.pu8STimeStampBackup[4]);
		STime1.Seconds = DecimalToBCD(strI2cEeprom.pu8STimeStampBackup[5]);
/*		if(STime1.Hours > 24)
			STime1.Hours = 0;
		if(STime1.Minutes > 60)
			STime1.Minutes = 0;
		if(STime1.Seconds > 60)
			STime1.Seconds = 0;*/
		HAL_RTC_SetTime(&hrtc, &STime1, RTC_FORMAT_BCD);

		year = DecimalToBCD(strI2cEeprom.pu8STimeStampBackup[2]);
		SDate1.WeekDay = RTC_WEEKDAY_MONDAY;
		SDate1.Month = DecimalToBCD(strI2cEeprom.pu8STimeStampBackup[1]);
		SDate1.Date = DecimalToBCD(strI2cEeprom.pu8STimeStampBackup[0]);
		SDate1.Year = year;
/*		if(SDate1.Date > 31)
			SDate1.Date = 1;
		if(SDate1.Month > 12)
			SDate1.Month = 1;
		if((SDate1.Year < 23) || (SDate1.Year = 255))
			SDate1.Year = 0;*/
		HAL_RTC_SetDate(&hrtc, &SDate1, RTC_FORMAT_BCD);
	}
#endif
	enqueue(&gsmPayload,(char *)getSystemConfig());
	HAL_GPIO_WritePin(LED_1_GPIO_Port, LED_1_Pin, GPIO_PIN_SET);	// LED OFF
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	/********************** Main Application Program ***************************/

	/***** Ethernet  ****
	  updateEthLinkStatus_1();*/
//	MX_LWIP_Process();

#if(ENABLEMEMORYSPI == TRUE)
	  /* Serial Flash operations */
	  SerialFlashFSM();
	  if(gu32SystemResetTimer < THIRTY_SEC && FlashStr.bitFlagQueueDataWrite == 0)
	  {
		  FlashStr.bitFlagQueueDataWrite = 1;
		  GSMQueueFullFlag = 1;
		  FlashStr.u8QueueWriteCounter = 0;
	  }
#endif

#if(ENABLE_RTC_EEPROM_BACKUP == TRUE)
	  if(gu32SystemResetTimer < THIRTY_SEC &&  bitFlagRTCBackupUpdated == 0)
	  {
		  bitFlagRTCBackupUpdated = 1;
		  memset(strI2cEeprom.pu8STimeStampBackup, 0x0, sizeof(strI2cEeprom.pu8STimeStampBackup));
		  /* Get the RTC current Time */
		  HAL_RTC_GetTime(&hrtc, &STime1, RTC_FORMAT_BIN);
		  /* Get the RTC current Date */
		  HAL_RTC_GetDate(&hrtc, &SDate1, RTC_FORMAT_BIN);
		  strI2cEeprom.pu8STimeStampBackup[0] = SDate1.Date;
		  strI2cEeprom.pu8STimeStampBackup[1] = SDate1.Month;
		  strI2cEeprom.pu8STimeStampBackup[2] = SDate1.Year;
		  strI2cEeprom.pu8STimeStampBackup[3] = STime1.Hours;
		  strI2cEeprom.pu8STimeStampBackup[4] = STime1.Minutes;
		  strI2cEeprom.pu8STimeStampBackup[5] = STime1.Seconds;
		  HAL_I2C_Mem_Write_IT(&hi2c1, EEPROM_ADDRESS, 296, I2C_MEMADD_SIZE_16BIT, (uint8_t *)&strI2cEeprom.pu8STimeStampBackup, 6);
		  HAL_Delay(10);
	  }
#endif
	/*Execute Modbus TCP Poll
	if(gu32OperateModbusTCP == 1)
	{
		mbTCPPoll();
		updateModbusPayload();
	}*/
	/* RTU Poll */
	if(gu32OperateModbus485 == 1)
	{
		modbusQueryFSM();
		uploadSlaveData();
	}

	/***** Internal ADC  *****/
	operateadc();

	/***** Payload Generation  *****/
	if(gu32TempPayloadQueueEnqueue == 0)
	{
		if(u32MB485FirstCycleComplete == 0 || u32MBTCPFirstCycleComplete == 0)
		{
			/* Keep checking every two sec for modbus cycle completion */
			gu32TempPayloadQueueEnqueue = TWO_SEC;
			++u32MBCycleCompCouter;
			if(u32MBCycleCompCouter > 20)
			{
				u32MB485FirstCycleComplete = 1;
				u32MBTCPFirstCycleComplete = 1;
			}
		}
		else
		{
			enqueue(&gsmPayload,(char *)getSystemDataString());
//			gu32TempPayloadQueueEnqueue = gu32UploadTimeSupply;
			if(gu32InputSupplySwitchedAlert == TRUE)
				gu32TempPayloadQueueEnqueue = gu32UploadTimeBatt; //TEN_SEC;//
			else
				gu32TempPayloadQueueEnqueue = gu32UploadTimeSupply;
			if(gu32TempPayloadQueueEnqueue < ONE_MIN)
				gu32TempPayloadQueueEnqueue = ONE_MIN;
		}
	}

	/***** GSM Module Operation  *****/
	if(gsmInstance.u32GSMTimer > FIVE_MIN)
		gsmInstance.u32GSMTimer = FOUR_SEC;

	if(gu32GSMHangTimer == 0)
		initGSMSIM868();

	if(gu32GSMRestartTimer == 0)
		initGSMSIM868();

	operateGSMSIM868();

	/***** LED Indication  *****/
	if(gu32LedTimer == 0)
	{
		HAL_GPIO_TogglePin(LED_2_GPIO_Port, LED_2_Pin);
		HAL_GPIO_TogglePin(HEARTBEAT_GPIO_Port,HEARTBEAT_Pin);
		gu32LedTimer = ONE_SEC; // FIVE_SEC; // HUNDRED_MS; //
		if(gu32InputSupplySwitchedAlert == FALSE)
			gu32LedTimer = ONE_SEC;
		else
			gu32LedTimer = FIVE_SEC;
	}
	if(gu32SystemResetTimer == 0)
	{
		gu32ExtWatchDogResetTimer = TWO_MIN;
		while(1) // Forced Reset after 30 Mins or unable to upload data
		{
			HAL_IWDG_Refresh(&hiwdg);
			if(gu32ExtWatchDogResetTimer == 0)
			{
				while(1) // Alternate for external watch dog
				{

				}
			}
		}
	}

	/***** Remote Configuration Updates *****/
	verifyRemoteConfiguration();
	parseRemoteconfig();

	/***** Internal Watchdog *****/
	/* Use of Internal WatchDog is mandatory irrespective of logic being implemented
	* Can be explicitly disabled whenever required . */
	/* Refresh IWDT */
	getRTCCalender();

	HAL_IWDG_Refresh(&hiwdg);
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_LSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = 25;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 180;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Activate the Over-Drive mode
  */
  if (HAL_PWREx_EnableOverDrive() != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

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
