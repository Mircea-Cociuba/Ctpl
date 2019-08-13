/*
 * Can.h
 *
 *  Created on: Jul 23, 2019
 *      Author: uia94406
 */


void Can_Transmit(void);

uint32_t Can_CheckForRx(void);
uint8_t Can_SetTxData(uint8_t* buff);
uint8_t Can_GetRxData(uint8_t* outBuffer);
