#include <tests/framework/TestHarness.h>
#include <common.h>
#include <symbols.h>
#include <hud.h>

/// @brief Function signature of the system under test.
typedef void (*func)();

/// @brief Address of the new function to invoke.
static func decompiledFunction = (func) &DisplayHudRelated;

/// @brief Address of the original function to invoke.
extern int OG_DisplayHudRelated;
static func originalFunction = (func) &OG_DisplayHudRelated;

static unsigned int hits = 0;
static unsigned int total = 0;

/// @brief Test assertion code that runs in place of the original function address.
/// @note The arguments to this should be exactly the same as the new and old functions.
static void Tester()
{
    EnterCriticalSection();
    func originalFunctionRef = (func) GetOriginalFunction();

    BackupStartingState();

    // Invoke both old and new functions.
    decompiledFunction();

    BackupNewFunctionState();
    LoadStartingState();

    originalFunctionRef();
    
    // Assert equivalency.
    total++;
    if (IsStateEquivalent())
    {
        hits++;
    }

    // Record results.
    printf("DisplayHudRelated Test Result: %d/%d\n", hits, total);
    
    LeaveCriticalSection();
}

/// @brief Hook installation entry point.
void InstallDisplayHudRelatedTest()
{
    InstallHook((void*)&Tester, (void*)originalFunction, 0);
}