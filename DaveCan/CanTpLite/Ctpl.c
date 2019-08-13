#include "stdint.h"
#include "Can.h"
#include "Ctpl.h"
#include "appl.h"
#include "..\Dave\Generated\SYSTIMER\systimer.h"
#include <stdbool.h>



/* local defines */
#define CTPL_FLOW_CONTROL_CTS  (0x30u)
#define CTPL_FLOW_CONTROL_WAIT (0x31u)
#define CTPL_FLOW_CONTROL_OVF  (0x32u)
#define CTPL_SINFLE_FRAME_NIBLE       (0x0)
#define CTPL_MULTIPLE_FRAME_NIBLE     (0x1)
#define CTPL_CONSECUTIVE_FRAME_NIBLE  (0x2)
#define CTPL_FLOW_CONTROL_NIBLE       (0x3)
#define CTPL_CF_TIMEOUT_200MS  (200000)

/* type definitions */
typedef enum
{
	CTPL_STATE_NONE, //Ctpl not initialized yet
	CTPL_STATE_INIT, //Ctpl has been initialized

	CTPL_DEFAULT
} CtplState_t;


/* local variables */

uint8_t* DataCtpl;//fac pointer
uint16_t PozDataBuffer;
uint8_t message[8];
CtplState_t state;
static bool rxNotif; // flag pt ctpl rx_notif cu care verificam daca avem date
uint8_t NumbersOfCF;
uint16_t LengthOfFF;
static bool txNotif;
uint8_t SeqNrCnt;

/* global variables */


/* local function declarations */
static void Ctpl_SingleFrameHandler(void);
static void Ctpl_MultiframeHandler(void);
static void Ctpl_GetRxData(void);
static uint8_t Ctpl_SendFlowControl(void);
static bool Ctpl_ConsecutiveReading(bool isFirstFrame);
uint8_t Ctpl_CnsFrmChecking(void);
uint32_t Ctpl_LastFrameTimeOut;
uint32_t Ctpl_CurrentTime;
static bool Cptl_MultiFrameRxOnGoing;

/* global function definitions */
void Ctpl_Init(void)
{
	uint16_t i;
	rxNotif = false;
	txNotif = false;

	state = CTPL_STATE_NONE;
	for (i=0; i<sizeof(message); i++)
	{
		message[i] = 0;
	}
	PozDataBuffer=0;

	DataCtpl=Appl_GetDataPointer();
	SeqNrCnt = 0x21;
	Ctpl_LastFrameTimeOut=0;
	Cptl_MultiFrameRxOnGoing =false;

}

//data has been received
void Ctpl_RxNotification(void)
{
	rxNotif=true;
}

//data has been send succesfuly
void Ctpl_TxNotification(void)
{
	txNotif=true;
}

void Ctpl_MainFunction(void)
{
	 Ctpl_CurrentTime = SYSTIMER_GetTime();

	 if(rxNotif==true){
		 Ctpl_GetRxData();   // am copiat date date din can in cptl
		 if(message[0] < (0x10))
		 {
			 Ctpl_SingleFrameHandler();
		 }
		 else
		 {
			Ctpl_MultiframeHandler();
		 }
	 }

	 rxNotif=false;
//
//	 if(((Ctpl_CurrentTime - Ctpl_LastFrameTimeOut) > CTPL_CF_TIMEOUT_200MS) && (Cptl_MultiFrameRxOnGoing==true) && (Ctpl_LastFrameTimeOut))
//	 {
//		 //eror CF transmit
//		Appl_ErrorNotification();
//
//	 }

}


//Function is called by the Application to send data, SF, or MF over CAN
void Ctpl_SendData(uint8_t* buffer, uint16_t LengthOfFF)
{

	uint8_t i;
	uint8_t dat[8]={ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};


	dat[0]=LengthOfFF;


	for(i=0;i<4;i++)
	{
		dat[i+1]= buffer[i];
	}
	Can_SetTxData(&dat[0]);
	Can_Transmit();

}

/* local function definitions */
/* Functia pt SF*/
void Ctpl_SingleFrameHandler(void)
{
	if(message[0]>7)
	{
		//handler eroare
	}
	else
	{
		uint8_t i;
		for(i=0;i<message[0];i++)
		{
			DataCtpl[i]=message[i+1];
		}
		Appl_RxNotification();
	}
}

/*Functie de MF si de impartire a frame-urilor*/
void Ctpl_MultiframeHandler(void)
{
	static bool isFirstFrame = true;

	if (isFirstFrame)
	{
		LengthOfFF = ((message[0] & (0x000f)) << 8 )| message[1]; //lungimea mesajului
		NumbersOfCF = LengthOfFF/7;  // cate CF sunt
		if (Ctpl_SendFlowControl() != 0) // verifica daca a fost trimis FC
		{
			/* Handle flow control error */
			Appl_ErrorNotification();
		}
		Cptl_MultiFrameRxOnGoing=true;
	}
	else
	{   /*Verifica daca a trimis in ordine CF*/
		if(SeqNrCnt== message[0])
		{
			SeqNrCnt++;
			if(SeqNrCnt > 0x2f)
			{
				SeqNrCnt=0x20;
			}
		}
		else
		{
			Appl_ErrorNotification();
		}
	}

	Ctpl_LastFrameTimeOut = Ctpl_CurrentTime;


	if(Ctpl_ConsecutiveReading(isFirstFrame)==true)
	{
		isFirstFrame=true;
	}
	else
	{
		isFirstFrame = false;
	}
}

/*citeste cate 8 baiti din CAN*/
void Ctpl_GetRxData(void)
{
	Can_GetRxData((uint8_t*)&message);
}

/*citim consecutiv frame`urile*/
static bool Ctpl_ConsecutiveReading(bool isFirstFrame)
{
	uint8_t i = 1;
	bool endOfMessage = false;

	if(isFirstFrame)
	{
		i = 2;
	}
	for(i;i<8;i++)
	{
		DataCtpl[PozDataBuffer]=message[i];
		PozDataBuffer++;
		if(PozDataBuffer == LengthOfFF)
		{
			Appl_RecFinishNotification(); // notific aplicatia ca a fost trimis tot mesajul
			Appl_RecStartNotification(PozDataBuffer); // trimitem lungimea bufferului catre aplicatie
			endOfMessage = true;
			PozDataBuffer = 0;
			Cptl_MultiFrameRxOnGoing=false;
			Appl_RxNotification();
			break;
		}
	}
	return endOfMessage;
}

/*Trimitem FC(30.00.00..),ca si o verificare pentru a continua transmisia*/
uint8_t Ctpl_SendFlowControl(void)
{
	uint8_t retVal;
	uint8_t dat[8]={ 0x30, 0x00, 0x00, 0x55, 0x55, 0x55, 0x55, 0x55};
	retVal = Can_SetTxData(dat);
	if (0 == retVal)
	{
		Can_Transmit();
		/* To implement a timeout handler instead of a simple while() */
		while(!txNotif)
		{
			// Do nothing; wait for object transmission success.
			txNotif = false;
		}
	}
	return retVal;
}
