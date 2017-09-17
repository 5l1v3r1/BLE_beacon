/**************************************************************************************************
 *                                            INCLUDES
 **************************************************************************************************/
 
 /* Application */
#include "OSAL_simpleBLECentral.h"
#include "OSAL_simpleBLEPeripheral.h"
#include "simpleBLEUart.h"
 /*********************************************************************
  * GLOBAL VARIABLES
  */

 uint8 tasksCnt;
 uint16 *tasksEvents;
 pTaskEventHandlerFn tasksArr[MAX_TASKS];

 /*********************************************************************
  * FUNCTIONS
  *********************************************************************/
 
 void osalInitTasks(void)
 {
#if (CURRENT_ROLE == ROLE_CENT)
    central_osalInitTasks();
#elif (CURRENT_ROLE == ROLE_PERI)
    peripheral_osalInitTasks();
#endif
 }
 
 /*********************************************************************
 *********************************************************************/
 