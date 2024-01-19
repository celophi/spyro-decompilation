#include <tests/framework/TestHarness.h>
#include <wad.h>
#include <common.h>
#include <symbols.h>

/// @brief Function signature of the system under test.
typedef int* (*func)(WA4S2 *wa4s2, int option);

/// @brief Address of the new function to invoke.
static func decompiledFunction = (func) &ReadWA4S2;

/// @brief Address of the original function to invoke.
extern int OG_ReadWA4S2;
static func originalFunction = (func) &OG_ReadWA4S2;

static unsigned int hits = 0;
static unsigned int total = 0;

/// @brief Test assertion code that runs in place of the original function address.
/// @note The arguments to this should be exactly the same as the new and old functions.
static int* Tester(WA4S2 *wa4s2, int option)
{
    EnterCriticalSection();
    func originalFunctionRef = (func) GetOriginalFunction();

    BackupStartingState();

    // Invoke both old and new functions.
    decompiledFunction(wa4s2, option);

    BackupNewFunctionState();
    LoadStartingState();

    int* result = originalFunctionRef(wa4s2, option);
    BackupOldFunctionState();
    
    // Assert equivalency.
    total++;
    if (IsStateEquivalent())
    {
        hits++;
    }

    // Record results.
    printf("ReadWA4S2 Test Result: %d/%d\n", hits, total);
    
    LeaveCriticalSection();
    return result;
}

/// @brief Hook installation entry point.
void InstallReadWA4S2Test()
{
    InstallHook((void*)&Tester, (void*)originalFunction, 0);
}