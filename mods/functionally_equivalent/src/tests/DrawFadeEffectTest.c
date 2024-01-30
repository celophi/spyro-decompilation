#include <tests/framework/TestHarness.h>
#include <symbols.h>
#include <common.h>
#include <draw_stuff.h>

/// @brief Function signature of the system under test.
typedef void (*func)(int tpage, byte red, byte green, byte blue);

/// @brief Address of the new function to invoke.
static func decompiledFunction = (func) &DrawFadeEffect;

/// @brief Address of the original function to invoke.
extern int OG_DrawFadeEffect;
static func originalFunction = (func) &OG_DrawFadeEffect;

static unsigned int hits = 0;
static unsigned int total = 0;

/// @brief Test assertion code that runs in place of the original function address.
/// @note The arguments to this should be exactly the same as the new and old functions.
static void Tester(int tpage, byte red, byte green, byte blue)
{
    EnterCriticalSection();
    func originalFunctionRef = (func) GetOriginalFunction();

    BackupStartingState();

    // Invoke both old and new functions.
    decompiledFunction(tpage, red, green, blue);

    BackupNewFunctionState();
    LoadStartingState();

    originalFunctionRef(tpage, red, green, blue);

    BackupOldFunctionState();
    
    // Assert equivalency.
    total++;
    if (IsStateEquivalent())
    {
        hits++;
    }

    // Record results.
    printf("DrawFadeEffect Test Result: %d/%d\n", hits, total);
    
    LeaveCriticalSection();
}

/// @brief Hook installation entry point.
void InstallDrawFadeEffectTest()
{
    InstallHook((void*)&Tester, (void*)originalFunction, 0);
}