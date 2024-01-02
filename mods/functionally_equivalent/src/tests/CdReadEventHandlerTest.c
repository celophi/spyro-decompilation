/**
 * Note that this test doesn't work properly.
 * I was able to verify one execution, but only by comparing just RAM (not the scratchpad or GTE).
 * After the first comparison succeeds, the emulator complains about a RA load without an active stack.
 * I'm not sure what the cause of this is. Maybe it's because this function is a callback?
*/
#include <tests/framework/TestHarness.h>
#include <cd.h>

/// @brief Function signature of the system under test.
typedef void (*func)(const u_char status, const u_char* result);

/// @brief Address of the new function to invoke.
static func decompiledFunction = (func) &CdReadEventHandler;

/// @brief Address of the original function to invoke.
extern int OG_CdReadEventHandler;
static func originalFunction = (func) &OG_CdReadEventHandler;

static unsigned int hits = 0;
static unsigned int total = 0;

/// @brief Test assertion code that runs in place of the original function address.
/// @note The arguments to this should be exactly the same as the new and old functions.
/// @param status 
/// @param result 
static void Tester(const u_char status, const u_char* result)
{
    EnterCriticalSection();
    func originalFunctionRef = (func) GetOriginalFunction();

    BackupStartingState();
    decompiledFunction(status, result);

    BackupNewFunctionState();
    LoadStartingState();
    originalFunctionRef(status, result);
    
    // Assert equivalency.
    total++;
    if (IsStateEquivalent())
    {
        hits++;
    }

    // Record results.
    printf("CdReadEventHandler Test Result: %d/%d\n", hits, total);
    
    LeaveCriticalSection();
}

/// @brief Hook installation entry point.
void InstallCdReadEventHandlerTest()
{
    InstallHook((void*)&Tester, (void*)originalFunction, 0);
}