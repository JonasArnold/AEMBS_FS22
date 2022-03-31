/*
 * mySystemView.c
 *
 *  Created on: 31.03.2022
 *      Author: jonas
 */
#include "mySystemView.h"
#include "McuRTT.h"
#include "McuSystemView.h"
#include "McuWait.h"

void myInterrupt(void)
{
	// record interrupts
#if configUSE_SEGGER_SYSTEM_VIEWER_HOOKS
	McuSystemView_RecordEnterISR();
#endif
/*......*/
#if configUSE_SEGGER_SYSTEM_VIEWER_HOOKS
	McuSystemView_RecordExitISR();
#endif
}

void mySystemView_Test(void)
{
	McuSystemView_Print((const char *)"Printing a message\r\n");
	McuSystemView_Warn((const char *)"Warning message\r\n");
	McuSystemView_Error((const char *)"Error message\r\n");

	// measure runtime
#if configUSE_SEGGER_SYSTEM_VIEWER_HOOKS
	McuSystemView_OnUserStart(1);
#endif
	McuWait_Waitms(10);
#if configUSE_SEGGER_SYSTEM_VIEWER_HOOKS
	McuSystemView_OnUserStop(1);
#endif
}

void mySystemView_Init(void)
{
	McuRTT_Init();
#if configUSE_SEGGER_SYSTEM_VIEWER_HOOKS
	McuSystemView_Init();
#endif
}
