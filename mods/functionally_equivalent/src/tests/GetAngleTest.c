#include <tests/framework/TestHarness.h>
#include <vector.h>
#include <common.h>

/// @brief Function signature of the system under test.
typedef int (*func)(const int x, const int y);

/// @brief Address of the new function to invoke.
static func decompiledFunction = (func) &GetAngle;

/// @brief Address of the original function to invoke.
extern int OG_GetAngle;
static func originalFunction = (func) &OG_GetAngle;

static unsigned int hits = 0;
static unsigned int total = 0;

/// @brief Test assertion code that runs in place of the original function address.
/// @note The arguments to this should be exactly the same as the new and old functions.
/// @param x 
/// @param y 
static void Tester(const int x, const int y)
{
    EnterCriticalSection();
    func originalFunctionRef = (func) GetOriginalFunction();

    // Invoke both old and new functions.
    int decompiledResult = decompiledFunction(x, y);
    int originalResult = originalFunctionRef(x, y);
    
    // Assert equivalency.
    total++;
    if (decompiledResult == originalResult)
    {
        hits++;
    }

    // Record results.
    printf("GetAngle Test Result: %d/%d\n", hits, total);
    
    LeaveCriticalSection();
}

/// @brief Hook installation entry point.
void InstallGetAngleTest()
{
    InstallHook((void*)&Tester, (void*)originalFunction, 1);
}