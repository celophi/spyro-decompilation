/**
 * In order to run this test, you will need to mock the behavior of the PlayMusicTrack function for now.
 * CD interrupts cause the testing functionality to not work as intended. 
*/
#include <tests/framework/TestHarness.h>
#include <common.h>
#include <symbols.h>
#include <menus.h>

/// @brief Function signature of the system under test.
typedef void (*func)(int resumeMusic);

/// @brief Address of the new function to invoke.
static func decompiledFunction = (func) &ExitPauseMenu;

/// @brief Address of the original function to invoke.
extern int OG_ExitPauseMenu;
static func originalFunction = (func) &OG_ExitPauseMenu;

static unsigned int hits = 0;
static unsigned int total = 0;

/// @brief Test assertion code that runs in place of the original function address.
/// @note The arguments to this should be exactly the same as the new and old functions.
static void Tester(int resumeMusic)
{
    EnterCriticalSection();
    func originalFunctionRef = (func) GetOriginalFunction();

    BackupStartingState();

    // Invoke both old and new functions.
    decompiledFunction(resumeMusic);

    BackupNewFunctionState();
    LoadStartingState();

    originalFunctionRef(resumeMusic);
    
    // Assert equivalency.
    total++;
    if (IsStateEquivalent())
    {
        hits++;
    }

    // Record results.
    printf("ExitPauseMenu Test Result: %d/%d\n", hits, total);
    
    LeaveCriticalSection();
}

/// @brief Hook installation entry point.
void InstallExitPauseMenuTest()
{
    InstallHook((void*)&Tester, (void*)originalFunction, 0);
}