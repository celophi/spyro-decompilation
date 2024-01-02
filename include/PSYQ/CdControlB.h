#pragma once

#include <PSYQ/SYS/TYPES.h>

typedef struct {
    u_char com;
    u_char *param;
    u_char *result;
} CdControlBArgument;

int CdControlB(u_char com, u_char *param, u_char *result);
int GetCdControlBInvocationCount();
void MockCdControlB();
void UnMockCdControlB();
CdControlBArgument* GetCdControlBInvocation(int index);