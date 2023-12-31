/*******************************************************************************
* Title                 :   Utilities for Ethernet Interface
* Filename              :   user_ethernet.c
* Author                :   Hrishikesh Limaye
* Origin Date           :   08/03/2022
* Version               :   1.0.0
* Compiler              :
* Target                :   STM32F437VI - Tor4Eth
* Notes                 :   None
*
* Copyright (c) by KloudQ Technologies Limited.

  This software is copyrighted by and is the sole property of KloudQ
  Technologies Limited.
  All rights, title, ownership, or other interests in the software remain the
  property of  KloudQ Technologies Limited. This software may only be used in
  accordance with the corresponding license agreement. Any unauthorized use,
  duplication, transmission, distribution, or disclosure of this software is
  expressly forbidden.

  This Copyright notice may not be removed or modified without prior written
  consent of KloudQ Technologies Limited.

  KloudQ Technologies Limited reserves the right to modify this software
  without notice.
*
*
*******************************************************************************/
/*************** FILE REVISION LOG *****************************************
*
*    Date    Version   Author         	  Description
*  08/03/22   1.0.0    Hrishikesh Limaye   Initial Release.
*
*******************************************************************************/

/** @file  user_ethernet.c
 *  @brief Ethernet Implementation
 *
 */
/******************************************************************************
* Includes
*******************************************************************************/

#include "applicationdefines.h"
#include "externs.h"
#include "user_ethernet.h"
#include "lwip.h"
/******************************************************************************
* Module Preprocessor Constants
*******************************************************************************/
/**
 * Doxygen tag for documenting variables and constants
 */

/******************************************************************************
* Module Preprocessor Macros
*******************************************************************************/

/******************************************************************************
* Module Typedefs
*******************************************************************************/

/******************************************************************************
* Module Variable Definitions
*******************************************************************************/

uint32_t gu32EthLinkStatus = 0;
uint32_t gu32EthLinkAlert = 2;  /* 2 -  Not yet connected , 1 - Down , 0 - Up */
uint32_t gu32EthLastConnectedSuccess = 1;
uint32_t gu32PowerOnDisconnected = 1;
/******************************************************************************
* Function Prototypes
*******************************************************************************/

/******************************************************************************
* Function Definitions
*******************************************************************************/
/******************************************************************************
* Function : updateEthLinkStatus()
*//**
* \b Description:
*
* This function is used to Poll for Ethernet Link status
* IMP :
  Change LAN8742A_PHY_ADDRESS = 0 // Tor Rev 2
        LAN8742A_PHY_ADDRESS = 1 // Tor Rev 3 boards
*
*
* PRE-CONDITION: Enable Eth interface
*
* POST-CONDITION: None
*
* @return 		None.
*
* \b Example Example:
* @code
*
* 	updateEthLinkStatus()
*
* @endcode
*
* @see
*
* <br><b> - HISTORY OF CHANGES - </b>
*
* <table align="left" style="width:800px">
* <tr><td> Date       </td><td> Software Version </td><td> Initials </td><td> Description </td></tr>
* <tr><td> 08/03/2022 </td><td> 0.0.1            </td><td> HL100133 </td><td> Interface Created </td></tr>
* </table><br><br>
* <hr>
*
*******************************************************************************/
uint32_t updateEthLinkStatus(void)
{
//	HAL_ETH_ReadPHYRegister(&heth, PHY_BSR, &gu32EthLinkStatus);
	if((gu32EthLinkStatus & PHY_LINKED_STATUS) != PHY_LINKED_STATUS)
	{
		HAL_GPIO_TogglePin(LED_1_GPIO_Port, LED_1_Pin);
		gu32EthLinkAlert = 1;
		if(gu32EthLastConnectedSuccess)
			gu32EthLastConnectedSuccess = 0;
		/* Added on 23-12-22 */
		//tcp_connection_terminate();
		return 1;
	}
	else
	{
		/*Added on 23-12-22 */
		if(gu32EthLastConnectedSuccess == 0)
		{
			MX_LWIP_Init();
			gu32EthLastConnectedSuccess = 1;
		}
		MX_LWIP_Process();
		gu32EthLinkAlert = 0;
		HAL_GPIO_WritePin(LED_1_GPIO_Port, LED_1_Pin, GPIO_PIN_SET);	// LED OFF
		return 0;
	}
}

/* Updated by 100229 */
uint32_t updateEthLinkStatus_1(void)
{
//	HAL_ETH_ReadPHYRegister(&heth, PHY_BSR, &gu32EthLinkStatus);
	if((gu32EthLinkStatus & PHY_LINKED_STATUS) != PHY_LINKED_STATUS)
	{
		gu32EthLinkAlert = 1;
		HAL_GPIO_WritePin(LED_1_GPIO_Port, LED_1_Pin, GPIO_PIN_RESET);	// LED ON
		gu32PowerOnDisconnected = 0;
//		tcp_connection_terminate();
		if(gu32EthLastConnectedSuccess)
			gu32EthLastConnectedSuccess = 0;
		return 1;
	}
	else
	{
		/*Added on 23-12-22 */
		if(gu32EthLastConnectedSuccess == 0)
		{
			tcp_modbusclient_connect();
			if(gu32PowerOnDisconnected)
			{
				MX_LWIP_Init();
				gu32PowerOnDisconnected = 0;
			}
			gu32EthLastConnectedSuccess = 1;
		}
		MX_LWIP_Process();
		gu32EthLinkAlert = 0;
		HAL_GPIO_WritePin(LED_1_GPIO_Port, LED_1_Pin, GPIO_PIN_SET);	// LED OFF
		return 0;
	}
}


//***************************************** End of File ********************************************************
