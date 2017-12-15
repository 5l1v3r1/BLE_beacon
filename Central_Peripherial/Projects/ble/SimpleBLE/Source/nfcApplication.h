#ifndef __NFC_APPLICATION_H_
#define __NFC_APPLICATION_H_

#include "simpleble.h"
#include "OSAL.h"

void nfcAppInit(uint8 task_id);
uint16 nfcAppProcessEvent(uint8 task_id, uint16 events);


void nfcWorkAsInitiator(uint16 timeout);
void nfcWorkAsTarget(uint16 timeout);
void nfcWorkAsCard(void);

#endif