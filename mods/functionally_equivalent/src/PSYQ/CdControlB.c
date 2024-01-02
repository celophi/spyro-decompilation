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
        History[InvocationCount].result = result;

        if (com == CdlSetfilter)
        {
            CdlFILTER *filter = (CdlFILTER*)param;
            History[InvocationCount].chan = filter->chan;
            History[InvocationCount].file = filter->file;
            //History[InvocationCount].pad = filter->pad;
        }
        else if (com == CdlReadS)
        {
            CdlLOC *loc = (CdlLOC*)param;
            History[InvocationCount].track = loc->track;
            History[InvocationCount].minute = loc->minute;
            History[InvocationCount].second = loc->second;
            History[InvocationCount].sector = loc->sector;
        }
        else if (com == CdlSetmode)
        {
            History[InvocationCount].param = *(u_char*)param;
        }

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