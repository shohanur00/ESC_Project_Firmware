#ifndef DEBUG_H
#define DEBUG_H

#include "main.h"
#include <string.h>


void Debug_Add_Log(const char *fmt, ...);
void Debug_Send_Log(void);
void Debug_Clear_All_Log(void);


#endif
