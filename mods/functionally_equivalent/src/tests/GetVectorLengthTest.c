#include <tests/framework/TestHarness.h>
#include <vector.h>

/// @brief Function signature of the system under test.
typedef uint (*func)(const Vec3u32 *vector, const int includeZ);

/// @brief Address of the new function to invoke.
func decompiledFunction = (func) &GetVectorLength;

/// @brief Address of the original function to invoke.
extern int OG_GetVectorLength;
func originalFunction = (func) &OG_GetVectorLength;

unsigned int hits = 0;
unsigned int total = 0;

/// @brief Test assertion code that runs in place of the original function address.
/// @note The arguments to this should be exactly the same as the new and old functions.
/// @param vector 
/// @param includeZ 
uint Tester(Vec3u32 *vector, int includeZ)
{
    EnterCriticalSection();
    func originalFunctionRef = (func) GetOriginalFunction();

    // Invoke both old and new functions.
    uint decompResult = decompiledFunction(vector, includeZ);
    uint originalResult = originalFunctionRef(vector, includeZ);
    
    // Assert equivalency.
    total++;
    if (decompResult == originalResult)
    {
        hits++;
    }

    // Record results.
    printf("GetVectorLength Test Result: %d/%d\n", hits, total);
    
    LeaveCriticalSection();

    return originalResult;
}

/// @brief Hook installation entry point.
void InstallGetVectorLengthTest()
{
    InstallHook((void*)&Tester, (void*)originalFunction);
}