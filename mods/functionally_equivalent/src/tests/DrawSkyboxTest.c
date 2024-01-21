#include <tests/framework/TestHarness.h>
#include <symbols.h>
#include <draw_stuff.h>

/// @brief Function signature of the system under test.
typedef void (*func)(int option, RotationMatrix *cameraA, RotationMatrix *cameraB);

/// @brief Address of the new function to invoke.
static func decompiledFunction = (func) &DrawSkybox;

/// @brief Address of the original function to invoke.
extern int OG_DrawSkybox;
static func originalFunction = (func) &OG_DrawSkybox;

static unsigned int hits = 0;
static unsigned int total = 0;

static unsigned int cooldown = 0;

/// @brief Test assertion code that runs in place of the original function address.
/// @note The arguments to this should be exactly the same as the new and old functions.
static void Tester(int option, RotationMatrix *cameraA, RotationMatrix *cameraB)
{
    EnterCriticalSection();
    func originalFunctionRef = (func) GetOriginalFunction();

    if (cooldown < 10)
    {
        originalFunctionRef(option, cameraA, cameraB);
        cooldown++;
        LeaveCriticalSection();
        return;
    }
    else
    {
        cooldown = 0;
    }

RESTART:

    BackupStartingState();

    // Invoke both old and new functions.
    decompiledFunction(option, cameraA, cameraB);

    BackupNewFunctionState();
    LoadStartingState();

    originalFunctionRef(option, cameraA, cameraB);

    BackupOldFunctionState();
    
    // Assert equivalency.
    total++;
    if (IsStateEquivalent())
    {
        hits++;
    }
    else
    {
        LoadStartingState();
        goto RESTART;
    }

    // Record results.
    printf("DrawSkybox Test Result: %d/%d\n", hits, total);
    
    LeaveCriticalSection();
}

/// @brief Hook installation entry point.
void InstallDrawSkyboxTest()
{
    InstallHook((void*)&Tester, (void*)originalFunction, 0);
}