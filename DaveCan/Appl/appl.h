/*
 * appl.h
 *
 *  Created on: Jul 23, 2019
 *      Author: uia94406
 */


#include "stdint.h"

#define BUFFER_SIZE (0x1000u)


void Appl_MainFunction(void);
void Appl_Init(void);
void Appl_RxNotification(void);
uint8_t* Appl_GetDataPointer(void);
