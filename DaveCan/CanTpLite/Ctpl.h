/*
 * Ctpl.h
 *
 *  Created on: Jul 23, 2019
 *      Author: uia94406
 */

#ifndef CANTPLITE_CTPL_H_
#define CANTPLITE_CTPL_H_

/* global function declarations */
void Ctpl_Init(void);
void Ctpl_MainFunction(void);
void Ctpl_RxNotification(void);
void Ctpl_TxNotification(void);
void Ctpl_SendData(uint8_t* buffer, uint16_t size);

#endif /* CANTPLITE_CTPL_H_ */
