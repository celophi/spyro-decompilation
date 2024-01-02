#include <tests/framework/TestHarness.h>
#include <vector.h>

/// @brief Function signature of the system under test.
typedef void (*func)(Vec3u32* vector, int denominator, int numerator);

/// @brief Address of the new function to invoke.
func decompiledFunction = (func) &ScaleVector;

/// @brief Address of the original function to invoke.
extern int OG_ScaleVector;
func originalFunction = (func) &OG_ScaleVector;

unsigned int hits = 0;
unsigned int total = 0;

/// @brief Test assertion code that runs in place of the original function address.
/// @note The arguments to this should be exactly the same as the new and old functions.
/// @param vector 
/// @param denominator 
/// @param numerator 
void Tester(Vec3u32* vector, int denominator, int numerator)
{
    EnterCriticalSection();
    func originalFunctionRef = (func) GetOriginalFunction();

    // Perform any backup of state necessary.
    Vec3u32 backupVector = { .X = vector->X, .Y = vector->Y, .Z = vector->Z };

    // Invoke both old and new functions.
    decompiledFunction(vector, denominator, numerator);
    originalFunctionRef(&backupVector, denominator, numerator);
    
    // Assert equivalency.
    total++;
    if (IsMemoryEquivalent(vector, &backupVector, sizeof(Vec3u32)))
    {
        hits++;
    }

    // Record results.
    printf("ScaleVector Test Result: %d/%d\n", hits, total);
    
    LeaveCriticalSection();
}

/// @brief Hook installation entry point.
void InstallScaleVectorTest()
{
    InstallHook((void*)&Tester, (void*)originalFunction);
}