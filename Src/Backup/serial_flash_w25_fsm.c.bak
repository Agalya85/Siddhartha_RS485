/*
 * serial_flash_fsm.c
 *
 *  Created on: 01-Nov-2021
 *      Author: Admin
 */


#include "serial_flash_w25_fsm.h"
#include "main.h"
#include "stm32f4xx_hal.h"
#include "applicationDefines.h"
#include <string.h>
#include <stdint.h>
#include <math.h>
#include <stdlib.h>
#include "externs.h"
#include "user_timer.h"

/* Variable declarations */
extern SPI_HandleTypeDef hspi1;

StructSFlash FlashStr;
uint32_t gu32BufferCounter = 1;

//uint32_t SFLASH_SIZE = 0;
_Bool GSMQueueFullFlag = 0;

char *payload_separate;
char *token_1;
char *token_2;
/****************************************************************************
 Function SerialFlashFSM()
 Purpose: SerialFlashFSM
 Input:	None.
 Return value: None.


 Note(s)(if-any) :

 Change History:
 Author           	Date                Remarks
 KloudQ Team      03-05-2021			100229
******************************************************************************/
void SerialFlashFSM(void)
{
	static uint16_t u16writeDataSize = 0;
//	static uint16_t u16writeDataSizeString=0;
	uint16_t u16tempVar = 0;
	uint8_t u8Flash_Busy_status = 0;
	uint8_t DataToWrite[5];

	if(FlashStr.u32SerialFlashDelay != 0)
		return;

	switch(FlashStr.enmCurrentOperation)
	{
		case enmFLASH_MEMORY_IDEAL:
			FlashStr.u32SerialFlashDelay = FIVEHUNDRED_MS;
			if(isQueueEmpty(&gsmPayload) == 1 )
			{
				if((FlashStr.u32FlashReadAddress < FlashStr.u32FlashWriteAddress))// && (FlashStr.u32FlashWriteAddress != 0)
				{
					if((FlashStr.u32FlashReadAddress + SFLASH_UPLOAD_LENGTH) < SFLASH_SIZE)
					{
						FlashStr.enmCurrentOperation = enmFLASH_MEMORY_READ;
						FlashStr.enmCurrentReadState = enmFlashReadStatusReg;
					}
					else
					{
						SFlash_Read(FlashStr.u32FlashReadAddress, (char *)FlashStr.cSFlashTransmitData, (SFLASH_SIZE - FlashStr.u32FlashReadAddress));
						FlashStr.u32FlashReadAddress = SFLASH_SIZE;
						FlashStr.enmCurrentOperation = enmFLASH_MEMORY_READ;
						FlashStr.enmCurrentReadState = enmFlashReadArrangeData;
					}
				}
				else
				{
					/* Reading is complete */
					if(FlashStr.u32FlashReadAddress != 0 || FlashStr.u32FlashWriteAddress != 0)
					{
//						FlashStr.enmCurrentOperation = enmFLASH_MEMORY_ERASE;
					}
				}
			}
			/* Memory Write states */
			else if(GSMQueueFullFlag == 1)
			{
				if((FlashStr.u32FlashWriteAddress + GSM_PAYLOAD_MAX_SIZE_FLASH) < SFLASH_SIZE)
				{
					GSMQueueFullFlag = 0;
					FlashStr.enmCurrentOperation = enmFLASH_MEMORY_WRITE;
					FlashStr.enmCurrentWriteState = enmFlashWriteStatusReg;
				}
				else
				{
					/* Memory Full */
					FlashStr.bitFlagMemoryFull = 1;
					FlashStr.enmCurrentOperation = enmFLASH_MEMORY_IDEAL;
				}
			}
			else if(((FlashStr.u32FlashReadAddress != 0) && (FlashStr.u32FlashWriteAddress != 0)) && ((FlashStr.u32FlashReadAddress >= FlashStr.u32FlashWriteAddress) || (FlashStr.u32FlashReadAddress >= SFLASH_SIZE) || (FlashStr.u32FlashWriteAddress >= SFLASH_SIZE)))
			{
				FlashStr.enmCurrentOperation = enmFLASH_MEMORY_ERASE;
			}
			break;

		case enmFLASH_MEMORY_WRITE:
			switch(FlashStr.enmCurrentWriteState)
			{
				case  enmFlashWriteStatusReg:
					if(FlashStr.bitFlagQueueDataWrite == 1)
					{
						memset((char *)FlashStr.cSFlashWriteData, 0x00, (sizeof(FlashStr.cSFlashWriteData) * sizeof(char)));
						GSMQueueFullFlag = 1;
						if(gsmPayload.data[FlashStr.u8QueueWriteCounter] != NULL)
						{
							strcpy((char *)FlashStr.cSFlashWriteData, (char *)gsmPayload.data[FlashStr.u8QueueWriteCounter]);
							free(gsmPayload.data[FlashStr.u8QueueWriteCounter]);
							gsmPayload.data[FlashStr.u8QueueWriteCounter] = NULL;
						}
						else
						{
							++FlashStr.u8QueueWriteCounter;
							GSMQueueFullFlag = 1;
							if(FlashStr.u8QueueWriteCounter >= MAX_QUEUE_SIZE)
							{
								FlashStr.u8QueueWriteCounter = 0;
								GSMQueueFullFlag = 0;
								FlashStr.bitFlagQueueDataWrite = 0;
							}
						}
					}
/*					else
						strcpy((char *)FlashStr.cSFlashWriteData, (char *)gsmPayload.data[gsmPayload.head]);*/
					u16writeDataSize = strlen((const char *)FlashStr.cSFlashWriteData);
					if(u16writeDataSize > 200)
					{
						// 9 is added in both packets
						char * lptrsof = NULL;
/*						FlashStr.cSFlashWriteData[92] = '9';
						lptrsof = strstr((char *)FlashStr.cSFlashWriteData, "#*");
						if(lptrsof != NULL)
							lptrsof[93] = '9';*/
						FlashStr.cSFlashWriteData[u16writeDataSize - 3] = '9';
						lptrsof = strstr((char *)FlashStr.cSFlashWriteData, "#*");
						if(lptrsof != NULL)
							*(lptrsof - 2) = '9';
#if(ENABLE_DEBUG == TRUE)
						strcat(DebugBuff,"\r\nWrite_Data\r\n");
						strcat(DebugBuff,(const char *)FlashStr.cSFlashWriteData);
						strcat(DebugBuff,"\r\n");
#endif
						u16writeDataSize = strlen((const char *)FlashStr.cSFlashWriteData);
						FlashStr.u16FlashWriteIndex = 0;
						u8Flash_Busy_status = SFlash_Read_Status_Register();
						if(u8Flash_Busy_status == 1)
						{
							/* Flash is busy */
							FlashStr.u32SerialFlashDelay = TWENTY_MS; //FIVEHUNDRED_MS;
							FlashStr.enmCurrentWriteState = enmFlashWriteStatusReg;
						}
						else
						{
							FlashStr.enmCurrentWriteState = enmFlashWriteEnable;
						}
					}
					else
						FlashStr.enmCurrentOperation = enmFLASH_MEMORY_IDEAL;
					break;

				case enmFlashWriteEnable:
					SFlash_Write_Enable();
					FlashStr.enmCurrentWriteState = enmFlashWriteEnableCheck;
					FlashStr.u32SerialFlashDelay = TWENTY_MS;
					break;

				case enmFlashWriteEnableCheck:
					u8Flash_Busy_status = SFlash_Read_Status_Register();
					if(u8Flash_Busy_status == 2)
						FlashStr.enmCurrentWriteState = enmFlashWriteData;
					else
						FlashStr.enmCurrentWriteState = enmFlashWriteEnable;
					FlashStr.u32SerialFlashDelay = TWENTY_MS;
					break;

				case enmFlashWriteData:
					u16tempVar = FlashStr.u32FlashWriteAddress / 256;
					u16tempVar *= 256;
					u16tempVar = FlashStr.u32FlashWriteAddress - u16tempVar;
					FlashStr.u16FlashPageEmptyByte = 256 - u16tempVar;
					if(u16writeDataSize > FlashStr.u16FlashPageEmptyByte)
					{
						// Multiple write cycles
						FlashStr.u16FlashByteWriten = FlashStr.u16FlashPageEmptyByte;
					}
					else
					{
						// Last write cycle
						FlashStr.u16FlashByteWriten = u16writeDataSize;
						FlashStr.enmCurrentOperation = enmFLASH_MEMORY_IDEAL;
					}

					memset(DataToWrite, 0, 5);
					DataToWrite[0] = SFLASH_WRITE_PAGE_CODE;
					DataToWrite[1] = (uint8_t)(FlashStr.u32FlashWriteAddress >> 16);
					DataToWrite[2] = (uint8_t)(FlashStr.u32FlashWriteAddress >> 8);
					DataToWrite[3] = (uint8_t)(FlashStr.u32FlashWriteAddress >> 0);
					SFlash_Chip_Select(ENABLE);
				  	HAL_SPI_Transmit_IT(&hspi1, (uint8_t *) DataToWrite, 4);
				  	HAL_SPI_Transmit_IT(&hspi1, (uint8_t *) &FlashStr.cSFlashWriteData[FlashStr.u16FlashWriteIndex], FlashStr.u16FlashByteWriten);
					HAL_Delay(20);

					SFlash_Chip_Select(DISABLE);
					FlashStr.u32FlashWriteAddress += FlashStr.u16FlashByteWriten;
					FlashStr.u16FlashWriteIndex += FlashStr.u16FlashByteWriten;
					FlashStr.u16FlashPageEmptyByte -= FlashStr.u16FlashByteWriten;
					u16writeDataSize -= FlashStr.u16FlashByteWriten;
					if(FlashStr.u16FlashPageEmptyByte >= u16writeDataSize)	// Write is over
					{
						FlashStr.enmCurrentOperation = enmFLASH_MEMORY_IDEAL;
						memset((char *)FlashStr.cSFlashWriteData, 0x00, (sizeof(FlashStr.cSFlashWriteData) * sizeof(char)));
						/*	Writing SFlash Write address into EEPROM*/
						memset(strI2cEeprom.pu8SFlashWriteMemLocAddress, 0x0, 5);
						strI2cEeprom.pu8SFlashWriteMemLocAddress[3] = (uint8_t)(FlashStr.u32FlashWriteAddress >> 24);
						strI2cEeprom.pu8SFlashWriteMemLocAddress[2] = (uint8_t)(FlashStr.u32FlashWriteAddress >> 16);
						strI2cEeprom.pu8SFlashWriteMemLocAddress[1] = (uint8_t)(FlashStr.u32FlashWriteAddress >> 8);
						strI2cEeprom.pu8SFlashWriteMemLocAddress[0] = (uint8_t)(FlashStr.u32FlashWriteAddress);
//						writeParametertoMemory(I2C_MEM_FLASH_WRITE); replaced by below block
						HAL_I2C_Mem_Write_IT(&hi2c1, EEPROM_ADDRESS, 288,I2C_MEMADD_SIZE_16BIT,(uint8_t *)&strI2cEeprom.pu8SFlashWriteMemLocAddress,4);
						HAL_Delay(10);
						if(FlashStr.bitFlagQueueDataWrite == 1)
						{
							GSMQueueFullFlag = 1;
							++FlashStr.u8QueueWriteCounter;
							if(FlashStr.u8QueueWriteCounter >= MAX_QUEUE_SIZE)
							{
								FlashStr.u8QueueWriteCounter = 1;
								GSMQueueFullFlag = 0;
								FlashStr.bitFlagQueueDataWrite = 0;
							}
						}
					}
					else
					{
						FlashStr.enmCurrentWriteState = enmFlashWriteEnable;
					}
					break;
				default:
					FlashStr.enmCurrentOperation = enmFLASH_MEMORY_IDEAL;
					break;
			}
			break;

		case enmFLASH_MEMORY_READ:
			switch(FlashStr.enmCurrentReadState)
			{
				case enmFlashReadStatusReg:
					u8Flash_Busy_status = SFlash_Read_Status_Register();
					if(u8Flash_Busy_status == 1)
					{
						/* Flash is busy */
						FlashStr.u32SerialFlashDelay = TWENTY_MS;
						FlashStr.enmCurrentReadState = enmFlashReadStatusReg;
					}
					else
					{
						FlashStr.enmCurrentReadState = enmFlashReadData;
					}
					break;

				case enmFlashReadData:
					FlashStr.bitFlagReadInProgress = 1;
					memset((char *)FlashStr.cSFlashTransmitData, 0, sizeof(FlashStr.cSFlashTransmitData));
					if(FlashStr.u32FlashWriteAddress < (FlashStr.u32FlashReadAddress + SFLASH_UPLOAD_LENGTH))
					{
						SFlash_Read(FlashStr.u32FlashReadAddress, (char *)FlashStr.cSFlashTransmitData, FlashStr.u32FlashWriteAddress - FlashStr.u32FlashReadAddress);
#if(ENABLE_DEBUG == TRUE)
						strcat(DebugBuff,"\r\nRead_Data\r\n");
						strcat(DebugBuff,(char *)FlashStr.cSFlashTransmitData);
						strcat(DebugBuff,"\r\n");
#endif
						FlashStr.u32FlashReadAddress += SFLASH_UPLOAD_LENGTH;
					}
					else
					{
						SFlash_Read(FlashStr.u32FlashReadAddress, (char *)FlashStr.cSFlashTransmitData, SFLASH_UPLOAD_LENGTH);
#if(ENABLE_DEBUG == TRUE)
						strcat(DebugBuff,"\r\nRead_Data\r\n");
						strcat(DebugBuff,(char *)FlashStr.cSFlashTransmitData);
						strcat(DebugBuff,"\r\n");
#endif
						FlashStr.u32FlashReadAddress += SFLASH_UPLOAD_LENGTH;
						FlashStr.enmCurrentReadState = enmFlashReadArrangeData;
					}
					FlashStr.enmCurrentReadState = enmFlashReadArrangeData;
					break;

				case enmFlashReadArrangeData:
					SFlash_Arrange_Read_Data();
					if(strlen((char *)FlashStr.cSFlashTransmitData) > 10)
					{
						/* To separate individual payload data as max upload size is 1024 bytes	*/
						char ctempUploadArray1[GSM_PAYLOAD_MAX_SIZE_FLASH];	//[7][500]
						FlashStr.u8BufferQueueCounts = 0;
						enqueue(&gsmPayload,(char *)getSFlashDataString((char *)FlashStr.cSFlashTransmitData, strlen(FlashStr.cSFlashTransmitData) + 10));
						FlashStr.bitFlagSFlashData = 1;
#if(ENABLE_DEBUG == TRUE)
						strcat(DebugBuff,"\r\n Read Arrange Data\r\n");
						memset(ctempUploadArray1, 0, sizeof(ctempUploadArray1));
						strcpy(ctempUploadArray1, FlashStr.cSFlashTransmitData);
						strcat(DebugBuff,(char *)ctempUploadArray1);
						strcat(DebugBuff,"\r\n");
#endif
					}
					else
					{
#if(ENABLE_DEBUG == TRUE)
						strcat(DebugBuff,"\r\n Read Arrange Data size less than 10 \r\n");
#endif
					}
					if(FlashStr.u32FlashReadAddress >= FlashStr.u32FlashWriteAddress)
					{
					  if(strlen((char *)FlashStr.cSFlashHalfReadData) != 0)
					  {
						  enqueue(&gsmPayload,(char *)getSFlashDataString((char *)FlashStr.cSFlashHalfReadData, strlen((char *)FlashStr.cSFlashHalfReadData)));
#if(ENABLE_DEBUG == TRUE)
						    strcat(DebugBuff,"\r\n Read Arrange Data\r\n");
							strcat(DebugBuff,(char *)FlashStr.cSFlashHalfReadData);
							strcat(DebugBuff,"\r\n");
#endif
						  FlashStr.bitFlagSFlashData = 1;
						  FlashStr.u8BufferQueueCounts++;
					  }
					}
					FlashStr.enmCurrentOperation = enmFLASH_MEMORY_IDEAL;
					FlashStr.enmCurrentReadState = enmFlashReadStatusReg;
					break;

				default:
					FlashStr.enmCurrentOperation = enmFLASH_MEMORY_IDEAL;
					break;
			}
			break;

		case enmFLASH_MEMORY_ERASE:
			  FlashStr.bitFlagReadInProgress = 0;
			  SFlash_Chip_Erase();
			  FlashStr.bitFlagMemoryFull = 0;
			  FlashStr.u32FlashWriteAddress = 0;
			  /*	Writing SFlash address into EEPROM*/
			  memset(strI2cEeprom.pu8SFlashWriteMemLocAddress, 0x0, 5);
			  strI2cEeprom.pu8SFlashWriteMemLocAddress[3] = (uint8_t)(FlashStr.u32FlashWriteAddress >> 24);
			  strI2cEeprom.pu8SFlashWriteMemLocAddress[2] |= (uint8_t)(FlashStr.u32FlashWriteAddress >> 16);
			  strI2cEeprom.pu8SFlashWriteMemLocAddress[1] |= (uint8_t)(FlashStr.u32FlashWriteAddress >> 8);
			  strI2cEeprom.pu8SFlashWriteMemLocAddress[0] = (uint8_t)(FlashStr.u32FlashWriteAddress);
//			  writeParametertoMemory(I2C_MEM_FLASH_WRITE);
			  HAL_I2C_Mem_Write_IT(&hi2c1, EEPROM_ADDRESS, 288,I2C_MEMADD_SIZE_16BIT,(uint8_t *)&strI2cEeprom.pu8SFlashWriteMemLocAddress,4);
			  HAL_Delay(100);

			  FlashStr.u32FlashReadAddress = 0;
			  memset(strI2cEeprom.pu8SFlashReadMemLocAddress, 0, sizeof(strI2cEeprom.pu8SFlashReadMemLocAddress)*sizeof(char));
			  strI2cEeprom.pu8SFlashReadMemLocAddress[3] = (uint8_t)(FlashStr.u32FlashReadAddress >> 24);
			  strI2cEeprom.pu8SFlashReadMemLocAddress[2] = (uint8_t)(FlashStr.u32FlashReadAddress >> 16);
			  strI2cEeprom.pu8SFlashReadMemLocAddress[1] = (uint8_t)(FlashStr.u32FlashReadAddress >> 8);
			  strI2cEeprom.pu8SFlashReadMemLocAddress[0] = (uint8_t)(FlashStr.u32FlashReadAddress);
//			  writeParametertoMemory(I2C_MEM_FLASH_READ);
			  HAL_I2C_Mem_Write_IT(&hi2c1, EEPROM_ADDRESS, 292,I2C_MEMADD_SIZE_16BIT,(uint8_t *)&strI2cEeprom.pu8SFlashReadMemLocAddress,4);
			  HAL_Delay(10);
			  FlashStr.u32SerialFlashDelay = TWO_MIN;
			  FlashStr.enmCurrentOperation = enmFLASH_MEMORY_IDEAL;

#if(ENABLE_DEBUG == TRUE)
				strcat(DebugBuff,(char*)"\r\n Erase \r\n");
#endif
			  break;

		default:
			FlashStr.enmCurrentOperation = enmFLASH_MEMORY_IDEAL;
			break;
	}
}

/****************************************************************************
 Function
 Purpose:
 Input:	None.
 Return value: None.


 Note(s)(if-any) :

 Change History:
 Author           	Date                Remarks
 KloudQ Team      03-05-2021			100229
******************************************************************************/
uint8_t SFlash_Read_Status_Register(void)
{
	uint8_t tx_var[3], rx_var[3];
	SFlash_Chip_Select(DISABLE);

	tx_var[0] = SFLASH_STATUS_REG_READ_CODE;
	SFlash_Chip_Select(ENABLE);
	HAL_SPI_TransmitReceive_IT(&hspi1, tx_var, rx_var, 2);
	/* wait till operation complete */
	HAL_Delay(20);
	SFlash_Chip_Select(DISABLE);
	return rx_var[1];
}

/**********************************************************************************
 Function Name: SFlash_Write_Enable
 Purpose: To make Serial flash Write enable, Making Status Register - 0x02
 Input: None.
 Return value: None.

 Change History:
 Author           Date                Remarks
 100229			23-03-2020
************************************************************************************/
void SFlash_Write_Enable(void)
{
	uint8_t tx_var[2];
	tx_var[0] = SFLASH_WRITE_EN_CODE;	//0x06;
	SFlash_Chip_Select(ENABLE);
	HAL_SPI_Transmit_IT(&hspi1, (uint8_t *)tx_var, 1);
	SFlash_Chip_Select(DISABLE);
//	HAL_Delay(20);

}

/**********************************************************************************
 Function Name: SFlash_Write_Disable
 Purpose: To make Serial flash Write Disable & protection for all blocks
 	 	  i.e. Making Status Register - 0x1C
 Input:	None.
 Return value: None.

 Change History:
 Author           Date                Remarks
 100229			23-03-2020
************************************************************************************/
void SFlash_Write_Disable(void)
{
	uint8_t tx_var[2];
	tx_var[0] = SFLASH_WRITE_DIS_CODE;	//0x04;
	SFlash_Chip_Select(ENABLE);
	HAL_SPI_Transmit_IT(&hspi1, (uint8_t *) tx_var,1);
	SFlash_Chip_Select(DISABLE);
	HAL_Delay(20);

}

/**********************************************************************************
 Function Name: SFlash_Chip_Select
 Purpose: Enable or Disable chip select pin of Serial flash
 Input:	  uint8_t state - ENABLE/DISABLE
 Return value: None.
 Note(s)(if-any) :
					ENABLE  - To make chip select pin LOW
					DISABLE - To make chip select pin HIGH

 Change History:
 Author           Date                Remarks
 100229			03-05-2021
************************************************************************************/
void SFlash_Chip_Select(uint8_t state)
{
	if(state == ENABLE)
	{
		HAL_GPIO_WritePin(SPI_CE_GPIO_Port, SPI_CE_Pin, GPIO_PIN_RESET);		// CE LOW
	}
	else if(state == DISABLE)
	{
		HAL_GPIO_WritePin(SPI_CE_GPIO_Port, SPI_CE_Pin, GPIO_PIN_SET);		// CE High
	}
}

/****************************************************************************
 Function SFlash_Read
 Purpose: To read data from serial flash
 Input:	address - memory address
  	  	*ptr_read_data - pointer where to copy read data
  	  	size - size of data to be read
 Return value: None.


 Note(s)(if-any) :

 Change History:
 Author           	Date                Remarks
 KloudQ Team      03-05-2021			100229
******************************************************************************/
void SFlash_Read(uint32_t address, char * ptr_read_data, uint16_t size)
{
	char tx_var[10];
	memset(tx_var, 0, sizeof(tx_var));
	tx_var[0] = SFLASH_READ_HSPEED_CODE;
	tx_var[1] = (uint8_t)(address >> 16);
	tx_var[2] = (uint8_t)(address >> 8);
	tx_var[3] = (uint8_t)(address >> 0);
	tx_var[4] = 0;
	SFlash_Chip_Select(ENABLE);
	HAL_SPI_Transmit_IT(&hspi1, (uint8_t *)tx_var, 5);
	HAL_SPI_Receive_IT(&hspi1, (uint8_t *)ptr_read_data, size);
	HAL_Delay(100);

	SFlash_Chip_Select(DISABLE);
}

/****************************************************************************
 Function SFlash_Arrange_Read_Data()
 Purpose: Extract complete pay-load data from read data/ separate out half pay-load data
 Input:	None.
 Return value: None.


 Note(s)(if-any) :

 Change History:
 Author           	Date                Remarks
 KloudQ Team      04-05-2021			100229
******************************************************************************/
void SFlash_Arrange_Read_Data(void)
{
	char temparray[3000];
	const char endstart[] = "#*";
	char *ptrchar = FlashStr.cSFlashHalfReadData;
	char *ptrendchar;
	uint16_t size = 0;

	if(strlen(FlashStr.cSFlashHalfReadData) == 0)
	{
		memset(FlashStr.cSFlashHalfReadData, 0, sizeof(FlashStr.cSFlashHalfReadData) * sizeof(char));
		memcpy(FlashStr.cSFlashHalfReadData, FlashStr.cSFlashTransmitData, sizeof(FlashStr.cSFlashTransmitData));
	}
	else
		strcat(FlashStr.cSFlashHalfReadData, FlashStr.cSFlashTransmitData);

	if(strstr(FlashStr.cSFlashHalfReadData, endstart) != NULL)
	{
		ptrendchar = ptrchar;
		while( ptrchar != NULL)
		{
			ptrchar = strstr(ptrchar, endstart);
			if(ptrchar != NULL)
			{
				ptrendchar = ptrchar;
				++ptrchar;
			}
		}
		++ptrendchar;
		size = ptrendchar - FlashStr.cSFlashHalfReadData;
		if(size > sizeof(FlashStr.cSFlashHalfReadData))
			size = sizeof (FlashStr.cSFlashHalfReadData) - 1;
		memset(FlashStr.cSFlashTransmitData, 0x00, sizeof(FlashStr.cSFlashTransmitData));
		memcpy(FlashStr.cSFlashTransmitData, FlashStr.cSFlashHalfReadData, size);
		memset(temparray, 0, sizeof(temparray));
		strcpy(temparray, ptrendchar);
		memset(FlashStr.cSFlashHalfReadData, 0, sizeof(FlashStr.cSFlashHalfReadData));
		strcpy(FlashStr.cSFlashHalfReadData, temparray);
	}
	else
	{
		memset(FlashStr.cSFlashHalfReadData, 0, sizeof(FlashStr.cSFlashHalfReadData));
	}
	/* Single payload is copied in queue */
}

/**********************************************************************************
 Function Name: getSFlashDataString
 Purpose: to store flash read data into dynamic memory for uploading
 Input:	  uint8_t *ptr_read_data - Pointer to array in which read data is copied.

 Return value: char * chrptr - Dynamic mem address
 Note(s)(if-any) :

 Change History:
 Author           Date                Remarks
 100229			03-05-2021
************************************************************************************/
char * getSFlashDataString(char * chrptr, uint16_t memory_size)
{
//	char * sflashPayload = malloc(sizeof(char) * SFLASH_PAYLOAD_MAX_SIZE);
	char * sflashPayload = malloc(sizeof(char) * memory_size);
	if(sflashPayload != NULL)
	{
		memset((char *)sflashPayload, 0x00, (sizeof(sflashPayload) * sizeof(char)));
		strcpy((char *)sflashPayload, (char *)chrptr);
		return sflashPayload;
	}
	return NULL;
}

/****************************************************************************
 Function SFlash_Chip_Erase()
 Purpose: To erase serial flash memory
 Input:	None.
 Return value: None.

 Note(s)(if-any) :

 Change History:
 Author           	Date                Remarks
 KloudQ Team      03-05-2021			100229
******************************************************************************/
void SFlash_Chip_Erase(void)
{
	uint8_t var = SFLASH_CHIP_ERASE_CODE;
	SFlash_Write_Enable();
	SFlash_Chip_Select(ENABLE);
	HAL_SPI_Transmit_IT(&hspi1, &var, 1);
	SFlash_Chip_Select(DISABLE);
	HAL_Delay(200);		// Time to erase full chip 100 ms
	SFlash_Write_Disable();
}

/****************************************************************************
 Function SFlash_Read_Device_ID()
 Purpose: To read device id to set flash write limit
 Input:	None.
 Return value: None.

 Note(s)(if-any) :

 Change History:
 Author           	Date                Remarks
 KloudQ Team      03-05-2021			100229
******************************************************************************/
void SFlash_Read_Device_ID(void)
{
	uint8_t tx_var[10], rx_var[10];
	uint32_t address = 0;
	//uint8_t read_status = 0;

	//read_status = SFlash_Read_Status_Register();
	 SFlash_Read_Status_Register();
	memset(tx_var, 0, sizeof(tx_var));
	memset(rx_var, 0, sizeof(rx_var));
	tx_var[0] = SFLASH_READ_DEVICE_ID;	//0x9F;	//
//	tx_var[1] = 0;
//	tx_var[2] = 0;
//	tx_var[3] = 0;
	tx_var[1] = (uint8_t)(address >> 16);
	tx_var[2] = (uint8_t)(address >> 8);
	tx_var[3] = (uint8_t)(address >> 0);
	SFlash_Chip_Select(ENABLE);
	HAL_SPI_Transmit_IT(&hspi1, (uint8_t *)tx_var, 4);
	HAL_SPI_Receive_IT(&hspi1, (uint8_t *)rx_var, 2);
//	HAL_SPI_TransmitReceive_IT(&hspi1, tx_var, rx_var, 4);
	HAL_Delay(10);

	SFlash_Chip_Select(DISABLE);
	switch(rx_var[1])
	{
//		case 0x16:
//			SFLASH_SIZE = SFLASH_SIZE_64Mbit; /* 64 Mbit memory */
//			break;
//		case 0x17:
//			SFLASH_SIZE = SFLASH_SIZE_128Mbit; /* 128 Mbit memory */
//			break;
//		default:
//			SFLASH_SIZE = 0; /* Chip problem or absent */
//			break;
	}
}

/****************************************************************************
 Function
 Purpose:
 Input:	None.
 Return value: None.

 Note(s)(if-any) :

 Change History:
 Author           	Date                Remarks
 KloudQ Team      03-05-2021			100229
******************************************************************************/
/****************************************************************************
 Function
 Purpose:
 Input:	None.
 Return value: None.

 Note(s)(if-any) :

 Change History:
 Author           	Date                Remarks
 KloudQ Team      03-05-2021			100229
******************************************************************************/

/******Replacing Ethernet Status parameter with '9' which specifies buffer data******/
void Buffer_Identifier(char * token)
{
	int count=0;
	for(int loopcounter = 0; loopcounter < strlen(token); loopcounter++)
	{
		if(token[loopcounter]==',')
		{
			count++;
			if(count==6)
			{
				token[loopcounter-1]='9';
				break;
			}loopcounter++;
		}
	}
}
/**************************************** Flash erase for testing **********************************************/

void Flash_Erase(void)
{
	SFlash_Chip_Erase();
	memset(strI2cEeprom.pu8SFlashReadMemLocAddress, 0, 4);
	memset(strI2cEeprom.pu8SFlashWriteMemLocAddress, 0, 4);
	HAL_I2C_Mem_Write_IT(&hi2c1, EEPROM_ADDRESS, 292,I2C_MEMADD_SIZE_16BIT,
					(uint8_t *)&strI2cEeprom.pu8SFlashReadMemLocAddress,4);
	HAL_Delay(100);
	HAL_I2C_Mem_Write_IT(&hi2c1, EEPROM_ADDRESS, 288,I2C_MEMADD_SIZE_16BIT,
					(uint8_t *)&strI2cEeprom.pu8SFlashWriteMemLocAddress,4);
	HAL_Delay(100);
}
