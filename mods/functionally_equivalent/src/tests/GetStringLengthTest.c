#include <tests/framework/TestHarness.h>
#include <memory.h>
#include <symbols.h>

/// @brief Function signature of the system under test.
typedef int (*func)(char* text);

/// @brief Address of the new function to invoke.
static func decompiledFunction = (func) &GetStringLength;

/// @brief Address of the original function to invoke.
extern int OG_GetStringLength;
static func originalFunction = (func) &OG_GetStringLength;

static unsigned int hits = 0;
static unsigned int total = 0;

/// @brief Test assertion code that runs in place of the original function address.
/// @note The arguments to this should be exactly the same as the new and old functions.
/// @param text
static int Tester(char* text)
{
    EnterCriticalSection();
    func originalFunctionRef = (func) GetOriginalFunction();

    BackupStartingState();

    // Invoke both old and new functions.
    int decompiledLength = decompiledFunction(text);

    BackupNewFunctionState();
    LoadStartingState();

    int originalLength = originalFunctionRef(text);
    
    // Assert equivalency.
    total++;
    if (IsStateEquivalent() && decompiledLength == originalLength)
    {
        hits++;
    }

    // Record results.
    printf("GetStringLength Test Result: %d/%d\n", hits, total);
    
    LeaveCriticalSection();
}

/// @brief Hook installation entry point.
void InstallGetStringLengthTest()
{
    InstallHook((void*)&Tester, (void*)originalFunction, 0);
}