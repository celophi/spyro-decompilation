#include <tests/framework/TestHarness.h>
#include <draw_stuff.h>
#include <common.h>
#include <symbols.h>

/// @brief Function signature of the system under test.
typedef void (*func)(const HudOvalVertex* vg, const TextureRelatedUnk* parameter, const RGBu32* rgb);

/// @brief Address of the new function to invoke.
static func decompiledFunction = (func) &DrawHudOval;

/// @brief Address of the original function to invoke.
extern int OG_DrawHudOval;
static func originalFunction = (func) &OG_DrawHudOval;

static unsigned int hits = 0;
static unsigned int total = 0;

/// @brief Test assertion code that runs in place of the original function address.
/// @note The arguments to this should be exactly the same as the new and old functions.
/// @param vg 
/// @param parameter 
/// @param rgb
static void Tester(const HudOvalVertex* vg, const TextureRelatedUnk* parameter, const RGBu32* rgb)
{
    EnterCriticalSection();
    func originalFunctionRef = (func) GetOriginalFunction();

    BackupStartingState();

    // Invoke both old and new functions.
    decompiledFunction(vg, parameter, rgb);

    BackupNewFunctionState();
    LoadStartingState();

    originalFunctionRef(vg, parameter, rgb);
    
    // Assert equivalency.
    total++;
    if (IsStateEquivalent())
    {
        hits++;
    }

    // Record results.
    printf("DrawHudOval Test Result: %d/%d\n", hits, total);
    
    LeaveCriticalSection();
}

/// @brief Hook installation entry point.
void InstallDrawHudOvalTest()
{
    InstallHook((void*)&Tester, (void*)originalFunction, 0);
}