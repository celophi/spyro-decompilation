#include <tests/framework/TestHarness.h>
#include <text.h>
#include <common.h>
#include <symbols.h>

/// @brief Function signature of the system under test.
typedef void (*func)(Vec3u32* position, uint timer, DrawTextArrowDirection direction);

/// @brief Address of the new function to invoke.
static func decompiledFunction = (func) &DrawTextArrow;

/// @brief Address of the original function to invoke.
extern int OG_DrawTextArrow;
static func originalFunction = (func) &OG_DrawTextArrow;

static unsigned int hits = 0;
static unsigned int total = 0;

/// @brief Test assertion code that runs in place of the original function address.
/// @note The arguments to this should be exactly the same as the new and old functions.
/// @param vg 
/// @param parameter 
/// @param rgb
static void Tester(Vec3u32* position, uint timer, DrawTextArrowDirection direction)
{
    EnterCriticalSection();
    func originalFunctionRef = (func) GetOriginalFunction();

    BackupStartingState();

    // Invoke both old and new functions.
    decompiledFunction(position, timer, direction);

    BackupNewFunctionState();
    LoadStartingState();

    originalFunctionRef(position, timer, direction);
    
    // Assert equivalency.
    total++;
    if (IsStateEquivalent())
    {
        hits++;
    }

    // Record results.
    printf("DrawTextArrow Test Result: %d/%d\n", hits, total);
    
    LeaveCriticalSection();
}

/// @brief Hook installation entry point.
void InstallDrawTextArrowTest()
{
    InstallHook((void*)&Tester, (void*)originalFunction, 0);
}