/**
 * Note that this test doesn't work properly.
 * I was able to verify one execution, but only by comparing just RAM (not the scratchpad or GTE).
 * After the first comparison succeeds, the emulator complains about a RA load without an active stack.
 * I'm not sure what the cause of this is. Maybe it's because this function is a callback?
*/
#include <tests/framework/TestHarness.h>
#include <cd.h>
#include <PSYQ/CdControlB.h>

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

} State;

void Backup()
{

}

/// @brief Test assertion code that runs in place of the original function address.
/// @note The arguments to this should be exactly the same as the new and old functions.
/// @param track 
/// @param flags 
void Tester(int track, int flags)
{
    func originalFunctionRef = (func) GetOriginalFunction();

    MockCdControlB();
    decompiledFunction(track, flags);

    int count = GetCdControlBInvocationCount();
    for (int i = 0; i < count; i++)
    {
        DecompiledHistory[i] = *(CdControlBArgument*)GetCdControlBInvocation(i);
    }

    MockCdControlB();
    originalFunctionRef(track, flags);

    count = GetCdControlBInvocationCount();
    int result = 1;
    for (int i = 0; i < count; i++)
    {
        if (!IsMemoryEquivalent(&DecompiledHistory[i], GetCdControlBInvocation(i), sizeof(CdControlBArgument)))
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