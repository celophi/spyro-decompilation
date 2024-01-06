#include <tests/framework/TestHarness.h>
#include <symbols.h>
#include <text.h>

/// @brief Function signature of the system under test.
typedef NewMoby* (*func)(char *text, Vec3u32 *capitalPosition, Vec3u32 *lowercasePosition, int spacing, byte color);

/// @brief Address of the new function to invoke.
static func decompiledFunction = (func) &DrawVaryingText;

/// @brief Address of the original function to invoke.
extern int OG_DrawVaryingText;
static func originalFunction = (func) &OG_DrawVaryingText;

static unsigned int hits = 0;
static unsigned int total = 0;

/// @brief Test assertion code that runs in place of the original function address.
/// @note The arguments to this should be exactly the same as the new and old functions.
static NewMoby* Tester(char *text, Vec3u32 *capitalPosition, Vec3u32 *lowercasePosition, int spacing, byte color)
{
    EnterCriticalSection();
    func originalFunctionRef = (func) GetOriginalFunction();

    BackupStartingState();
    Vec3u32 backupPosition = { .X = capitalPosition->X, .Y = capitalPosition->Y, .Z = capitalPosition->Z };

    // Invoke both old and new functions.
    NewMoby* decompiledMobyList = decompiledFunction(text, capitalPosition, lowercasePosition, spacing, color);

    BackupNewFunctionState();
    LoadStartingState();

    NewMoby* originalMobyList = originalFunctionRef(text, &backupPosition, lowercasePosition, spacing, color);
    
    // Assert equivalency.
    total++;
    if (IsStateEquivalent())
    {
        hits++;
    }

    // Record results.
    printf("DrawVaryingText Test Result: %d/%d\n", hits, total);
    
    LeaveCriticalSection();
}

/// @brief Hook installation entry point.
void InstallDrawVaryingTextTest()
{
    InstallHook((void*)&Tester, (void*)originalFunction, 0);
}



