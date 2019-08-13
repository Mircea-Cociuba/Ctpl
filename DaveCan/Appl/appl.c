/*
 * appl.c
 *
 *  Created on: Jul 23, 2019
 *      Author: uia94406
 */

#include "appl.h"
#include "Ctpl.h"


uint8_t data[BUFFER_SIZE];//bufferu de salvare

uint32_t len;

uint8_t dataReceived;
uint8_t status_notification;
static uint32_t table[256];
static int have_table ;
uint8_t ok;
uint32_t rc_crc32(uint32_t crc, const uint8_t *buf, uint32_t len)
{
	uint8_t octet;
	const uint8_t *p, *q;
	crc = ~crc;
	q = buf + len;
	for (p = buf; p < q; p++) {
		octet = p;  /* Cast to unsigned octet. */
		crc = (crc >> 8) ^ table[(crc & 0xff) ^ octet];
	}
	return ~crc;
}

void Appl_Init(void)
{
	uint32_t rem;
	int i, j;
	len=BUFFER_SIZE;
	dataReceived=0;
	status_notification=0;
	have_table = 0;
	ok=0;
	for(i=0;i<BUFFER_SIZE;i++)
	{
		data[i]=0;
	}

	/* This check is not thread safe; there is no mutex. */
	if (have_table == 0) {
		/* Calculate CRC table. */
		for (i = 0; i < 256; i++) {
			rem = i;  /* remainder from polynomial division */
			for (j = 0; j < 8; j++) {
				if (rem & 1) {
					rem >>= 1;
					rem ^= 0xedb88320;
				} else
					rem >>= 1;
			}
			table[i] = rem;
		}
		have_table = 1;
	}
}

void Appl_MainFunction(void)
{
	uint32_t volatile crc=0;
	uint8_t tempData[8]={0};
	if(dataReceived == 1u)
	{
		crc=rc_crc32(0,data,len);
		dataReceived=0u;

		//data should be put on the CAN now
		//copy crc to tempData
		uint8_t i=3;
		while(crc!=0)
		{
			tempData[i]=crc;
			crc = crc >> 8;
			i--;
		}

		Ctpl_SendData(&tempData,4);
	}

	if(status_notification==1)
	{
		Appl_RxNotification();
	}


}

uint8_t* Appl_GetDataPointer(void)
{
	return &data[0];
}

void Appl_RecStartNotification(uint32_t receivedLenght)
{
	//should check if lenght is the same
	len=receivedLenght;
}

void Appl_RecFinishNotification(void)
{
	dataReceived=1;
}

void Appl_ErrorNotification(uint8_t errorCode)
{
	ok=1;
	//stop everything - reset to default
}

void Appl_RxNotification(void)
{
	status_notification=1;
}

void Appl_HandelRXData(void)//reseteaza "data" si pune statusul pe 0
{
	uint8_t i;
	for(i=0;i<BUFFER_SIZE;i++)
		{
			data[i]=0;
		}
	status_notification=0;

}
