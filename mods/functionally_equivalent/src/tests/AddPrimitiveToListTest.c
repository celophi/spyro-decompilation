#include <tests/framework/TestHarness.h>
#include <draw_stuff.h>
#include <common.h>
#include <symbols.h>

/// @brief Function signature of the system under test.
typedef void (*func)(P_TAG *primitive);

/// @brief Address of the new function to invoke.
static func decompiledFunction = (func) &AddPrimitiveToList;

/// @brief Address of the original function to invoke.
extern int OG_AddPrimitiveToList;
static func originalFunction = (func) &OG_AddPrimitiveToList;

static unsigned int hits = 0;
static unsigned int total = 0;

/// @brief Test assertion code that runs in place of the original function address.
/// @note The arguments to this should be exactly the same as the new and old functions.
/// @param primitive 
static void Tester(P_TAG *primitive)
{
    EnterCriticalSection();
    func originalFunctionRef = (func) GetOriginalFunction();

    BackupStartingState();

    // Invoke both old and new functions.
    decompiledFunction(primitive);

    BackupNewFunctionState();
    LoadStartingState();

    originalFunctionRef(primitive);
    
    // Assert equivalency.
    total++;
    if (IsStateEquivalent())
    {
        hits++;
    }

    // Record results.
    printf("AddPrimitiveToList Test Result: %d/%d\n", hits, total);
    
    LeaveCriticalSection();
}

/// @brief Hook installation entry point.
void InstallAddPrimitiveToListTest()
{
    InstallHook((void*)&Tester, (void*)originalFunction, 0);
}