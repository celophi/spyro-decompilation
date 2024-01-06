#include <tests/framework/TestHarness.h>
#include <symbols.h>
#include <text.h>

/// @brief Function signature of the system under test.
typedef NewMoby* (*func)(char* text, Vec3u32* position, int spacing, byte color);

/// @brief Address of the new function to invoke.
static func decompiledFunction = (func) &DrawCapitalText;

/// @brief Address of the original function to invoke.
extern int OG_DrawCapitalText;
static func originalFunction = (func) &OG_DrawCapitalText;

static unsigned int hits = 0;
static unsigned int total = 0;

/// @brief Test assertion code that runs in place of the original function address.
/// @note The arguments to this should be exactly the same as the new and old functions.
static NewMoby* Tester(char* text, Vec3u32* position, int spacing, byte color)
{
    EnterCriticalSection();
    func originalFunctionRef = (func) GetOriginalFunction();

    BackupStartingState();
    Vec3u32 backupPosition = {.X = position->X, .Y = position->Y, .Z = position->Z };

    // Invoke both old and new functions.
    NewMoby* decompiledMobyList = decompiledFunction(text, position, spacing, color);

    BackupNewFunctionState();
    LoadStartingState();

    NewMoby* originalMobyList = originalFunctionRef(text, &backupPosition, spacing, color);
    
    // Assert equivalency.
    total++;
    if (IsStateEquivalent())
    {
        hits++;
    }

    // Record results.
    printf("DrawCapitalText Test Result: %d/%d\n", hits, total);
    
    LeaveCriticalSection();
}

/// @brief Hook installation entry point.
void InstallDrawCapitalTextTest()
{
    InstallHook((void*)&Tester, (void*)originalFunction, 0);
}



