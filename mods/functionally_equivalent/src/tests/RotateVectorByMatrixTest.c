#include <tests/framework/TestHarness.h>
#include <vector.h>
#include <common.h>

/// @brief Function signature of the system under test.
typedef void (*func)(const RotationMatrix* matrix, Vec3u32* input, Vec3u32* output);

/// @brief Address of the new function to invoke.
static func decompiledFunction = (func) &RotateVectorByMatrix;

/// @brief Address of the original function to invoke.
extern int OG_RotateVectorByMatrix;
static func originalFunction = (func) &OG_RotateVectorByMatrix;

static unsigned int hits = 0;
static unsigned int total = 0;

/**
 * @brief Asserts that the decompiled function is equivalent with the real function.
 * @note Performing a backup of the input vector is necessary. Some code uses the same address for both the input and output vector.
*/
static void Tester(const RotationMatrix* matrix, Vec3u32* input, Vec3u32* output)
{
    EnterCriticalSection();
    func originalFunctionRef = (func) GetOriginalFunction();

    // Create a backup of the input vector. Some invocations use the same address for both input and output.
    Vec3u32 inputBackup = {.X = input->X, .Y = input->Y, .Z = input->Z };

    // Invoke both old and new functions.
    decompiledFunction(matrix, input, output);

    // Backup the result output vector
    Vec3u32 decompiledResult = {.X = output->X, .Y = output->Y, .Z = output->Z };

    originalFunctionRef(matrix, &inputBackup, output);
    
    // Assert equivalency.
    total++;
    if (IsMemoryEquivalent(&decompiledResult, output, sizeof(Vec3u32)))
    {
        hits++;
    }

    // Record results.
    printf("RotateVectorByMatrix Test Result: %d/%d\n", hits, total);
    
    LeaveCriticalSection();
}

/// @brief Hook installation entry point.
void InstallRotateVectorByMatrixTest()
{
    InstallHook((void*)&Tester, (void*)originalFunction, 0);
}