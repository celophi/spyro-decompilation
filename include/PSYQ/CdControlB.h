#pragma once

#include <PSYQ/SYS/TYPES.h>

typedef struct {
    u_char com;
    u_char param;
    u_char *result;

    // CdlFilter
    u_char file;
    u_char chan;
    

    // CdlLOC
    u_char minute;
    u_char second;
    u_char sector;
    u_char track;
} CdControlBArgument;

int CdControlB(u_char com, u_char *param, u_char *result);
int GetCdControlBInvocationCount();
void MockCdControlB();
void UnMockCdControlB();
CdControlBArgument* GetCdControlBInvocation(int index);