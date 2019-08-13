/*
 * main.c
 *
 *  Created on: 2019 Jul 22 16:29:42
 *  Author: uia94406
 */




#include <DAVE.h>                 //Declarations from DAVE Code Generation (includes SFR declaration)
#include "os.h"
#include "Can.h"
#include "Ctpl.h"
#include "appl.h"
/**

 * @brief main() - Application entry point
 *
 * <b>Details of function</b><br>
 * This routine is the application entry point. It is invoked by the device startup code. It is responsible for
 * invoking the APP initialization dispatcher routine - DAVE_Init() and hosting the place-holder for user application
 * code.
 */



int main(void)
{
	DAVE_STATUS_t status;

	status = DAVE_Init();           /* Initialization of DAVE APPs  */

	if(status != DAVE_STATUS_SUCCESS)
	{
		/* Placeholder for error handler code. The while loop below can be replaced with an user error handler. */
		XMC_DEBUG("DAVE APPs initialization failed\n");

		while(1U)
		{

		}
	}

	Os_Init();
	Ctpl_Init();
	Appl_Init();

	/* Placeholder for user application code. The while loop below can be replaced with user application code. */
	while(1U)
	{
		(void)Can_CheckForRx();
	}
}
