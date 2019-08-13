/*
 * Can.c
 *
 *  Created on: Jul 23, 2019
 *      Author: uia94406
 */


#include <Dave.h>
#include "Can.h"
#include "Ctpl.h"

const CAN_NODE_t *HandlePtr1 = &CAN_NODE_0;//prima adresa din CAN

uint8_t can_data_arr_tx[8] = {0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08};//array de trimis

uint8_t can_data_arr_rx[8] = {0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08};//array de primit

void Can_Transmit(void)//functie de transmitere pe can
{

	CAN_NODE_STATUS_t mo_tranmit_status;//statusu mesajelor transmise
	CAN_NODE_STATUS_t status_can;//statusu can
	XMC_CAN_MO_t *MO_Ptr;//pointer la msj

	MO_Ptr = HandlePtr1->lmobj_ptr[0]->mo_ptr;

	// update data for the MO to transmit

	(void)CAN_NODE_MO_UpdateData(HandlePtr1->lmobj_ptr[0],&can_data_arr_tx[0]);

	mo_tranmit_status = CAN_NODE_MO_Transmit(HandlePtr1->lmobj_ptr[0]);//Function to transmit the can MO frame

	if (mo_tranmit_status == CAN_NODE_STATUS_SUCCESS)

	{
		//message object transmission success.

		// read the TXOK status bit
		status_can = CAN_NODE_MO_GetStatus(HandlePtr1->lmobj_ptr[0]);

		if (status_can & XMC_CAN_MO_STATUS_TX_PENDING);
		{
			//Clear the transmit OK flag

			XMC_CAN_MO_ResetStatus(MO_Ptr,XMC_CAN_MO_RESET_STATUS_TX_PENDING);

			Ctpl_TxNotification();
		}

	}
	else
	{
		// message object failed to transmit.
	}

}

uint32_t Can_CheckForRx(void)
{
	CAN_NODE_STATUS_t mo_receive_status;
	XMC_CAN_MO_t *MO_Ptr_Receive;//locuri pt trimitere de msj(MO)
	uint8_t i=0;

	MO_Ptr_Receive = HandlePtr1->lmobj_ptr[1]->mo_ptr;

	mo_receive_status = CAN_NODE_MO_GetStatus(HandlePtr1->lmobj_ptr[1]);
	//Check receive pending status

	if ( mo_receive_status & XMC_CAN_MO_STATUS_RX_PENDING)//verifica daca a primit
	{

		// Clear the flag

		XMC_CAN_MO_ResetStatus(MO_Ptr_Receive,XMC_CAN_MO_RESET_STATUS_RX_PENDING);

		// Read the received Message object

		mo_receive_status = CAN_NODE_MO_Receive(HandlePtr1->lmobj_ptr[1]);

		//todo: must check only for configured RX id - 710 - all others should not work

		if (mo_receive_status == CAN_NODE_STATUS_SUCCESS)//verifica daca-i totu cu succes
		{

			for(; i<8u ; i++)
			{
				can_data_arr_rx[i]=MO_Ptr_Receive->can_data_byte[i];
			}

			//notify CanTpLite
			Ctpl_RxNotification();
		}

	}

	return (uint32_t)mo_receive_status;
}

uint8_t Can_SetTxData(uint8_t* buff)//copiaza datele primite in array_tx
{
	uint8_t i=0;
	if(buff != NULL)
	{
		//copy from buff to can_data_arr_tx
		for(; i<8u ; i++)
		{
			can_data_arr_tx[i]=buff[i];
		}
	}

	return 0;
}

// copiaza datele in buffer de trimitere
uint8_t Can_GetRxData(uint8_t* outBuffer)
{

	uint8_t i=0;
	if(outBuffer != NULL)
	{
		for(; i<8u ; i++)
		{
			outBuffer[i]=can_data_arr_rx[i];
		}


		//copy contents of can_data_arr_rx to the pointer outBuffer
	}

	return 0;
}
