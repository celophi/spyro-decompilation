#include <PSYQ/LIBCD.h>
#include <PSYQ/CdControlB.h>

static CdControlBArgument History[1000];
static int InvocationCount = 0;
static int IsMocked = 0;

extern int OG_CdControlB(u_char com, u_char *param, u_char *result);

int CdControlB(u_char com, u_char *param, u_char *result)
{
    if (IsMocked)
    {
        
        History[InvocationCount].com = com;
        History[InvocationCount].param = param;
        History[InvocationCount].result = result;
        InvocationCount++;
    }
    else
    {
        return OG_CdControlB(com, param, result);
    }
}

int GetCdControlBInvocationCount()
{
    return InvocationCount;
}

void MockCdControlB() 
{
    IsMocked = 1;
    InvocationCount = 0;
}

void UnMockCdControlB()
{
    IsMocked = 0;
}

CdControlBArgument* GetCdControlBInvocation(int index)
{
    return &History[index];
}