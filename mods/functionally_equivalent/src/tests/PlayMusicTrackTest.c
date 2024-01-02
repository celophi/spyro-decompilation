/**
 * Note that this test doesn't work properly.
 * I was able to verify one execution, but only by comparing just RAM (not the scratchpad or GTE).
 * After the first comparison succeeds, the emulator complains about a RA load without an active stack.
 * I'm not sure what the cause of this is. Maybe it's because this function is a callback?
*/
#include <tests/framework/TestHarness.h>
#include <cd.h>
#include <PSYQ/CdControlB.h>
#include <symbols.h>

/// @brief Function signature of the system under test.
typedef void (*func)(int track, int flags);

/// @brief Address of the new function to invoke.
func decompiledFunction = (func) &PlayMusicTrack;

/// @brief Address of the original function to invoke.
extern int OG_PlayMusicTrack;
func originalFunction = (func) &OG_PlayMusicTrack;

unsigned int hits = 0;
unsigned int total = 0;

CdControlBArgument DecompiledHistory[1000];
CdControlBArgument OriginalHistory[1000];

typedef struct {
    int MusicFlags;
    int CdUnknownCommand;
    int CdUnknownFlags;
    int VolumeChange;
    int InitializeSoundU3;
} State;

State GlobalState;

void Backup()
{
    BackupStartingState();
    GlobalState.MusicFlags = _MusicFlags;
    GlobalState.CdUnknownCommand = _CdUnknownCommand;
    GlobalState.CdUnknownFlags = _CdUnknownFlags;
    GlobalState.VolumeChange = _VolumeChange;
    GlobalState.InitializeSoundU3 = _InitializeSoundU3;
}

int Compare(CdControlBArgument* actual, CdControlBArgument* expected)
{
    return IsMemoryEquivalent(actual, expected, sizeof(CdControlBArgument));

    /*
    if (actual->com != expected->com)
    {
        return 0;
    }

    if (actual->result != expected->result) 
    {
        return 0;
    }

    if (actual->com == CdlSetmode)
    {
        if (*(u_char*)(actual->param) != *(u_char*)(expected->param))
        {
            return 0;
        }
    }

    if (actual->com == CdlSetfilter)
    {
        CdlFILTER* actualFilter = (CdlFILTER*)actual->param;
        CdlFILTER* expectedFilter = (CdlFILTER*)expected->param;

        if (!IsMemoryEquivalent(actualFilter, expectedFilter, sizeof(CdlFILTER)))
        {
            return 0;
        }
    }

    if (actual->com == CdlReadS)
    {
        CdlLOC* actualLoc = (CdlLOC*)actual->param;
        CdlLOC* expectedLoc = (CdlLOC*)expected->param;

        if (!IsMemoryEquivalent(actualLoc, expectedLoc, sizeof(CdlLOC)))
        {
            return 0;
        }
    } */

//    return 1;
}

/// @brief Test assertion code that runs in place of the original function address.
/// @note The arguments to this should be exactly the same as the new and old functions.
/// @param track 
/// @param flags 
void Tester(int track, int flags)
{
    BackupStartingState();

    func originalFunctionRef = (func) GetOriginalFunction();

    MockCdControlB();
    decompiledFunction(track, flags);

    int count = GetCdControlBInvocationCount();
    for (int i = 0; i < count; i++)
    {
        DecompiledHistory[i] = *(CdControlBArgument*)GetCdControlBInvocation(i);
    }

    LoadStartingState();

    MockCdControlB();
    originalFunctionRef(track, flags);

    count = GetCdControlBInvocationCount();
    for (int i = 0; i < count; i++)
    {
        OriginalHistory[i] = *(CdControlBArgument*)GetCdControlBInvocation(i);
    }

    int result = 1;
    for (int i = 0; i < count; i++)
    {
        if (!Compare(&DecompiledHistory[i], &OriginalHistory[i]))
        {
            result = 0;
        }
    }

    UnMockCdControlB();

    // Record results.
    total++;
    if (result)
    {
        hits++;
    }

    printf("PlayMusicTrack Test Result: %d/%d\n", hits, total);
}

/// @brief Hook installation entry point.
void InstallPlayMusicTrackTest()
{
    InstallHook((void*)&Tester, (void*)originalFunction);
}