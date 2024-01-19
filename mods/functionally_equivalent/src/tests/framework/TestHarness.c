#include <tests/framework/TestHarness.h>
#include <common.h>
#include <symbols.h>
#include <gte.h>

// MIPS Instructions
#define J(addr) ((((unsigned int) addr & 0xFFFFFF) >> 2) | 0x08000000)
#define JR_RA 0x03E00008
#define NOP 0x00000000

unsigned int * ram = (unsigned int *) 0x80010000;
unsigned int * initialRamState = (unsigned int *) 0x80410000;
unsigned int * decompRamState = (unsigned int *) 0x80610000;

/// @brief The RAM size does not extend to the full 2MB because some space is reserved for the stack. In most games this is 0x800 bytes which can be found in main().
const unsigned int ramSize = (0x1FF800 - 0x10000) / 4;

unsigned int * scratchpad = (unsigned int *) 0x1F800000;
unsigned int * initialScratchpadState = (unsigned int *) 0x80400000;
unsigned int * decompScratchpadState = (unsigned int *) 0x80600000;
const unsigned int scratchpadSize = 0x400 / 4;

unsigned int * initialGteState = (unsigned int *) 0x80408000;
unsigned int * decompGteState = (unsigned int *) 0x80608000;
unsigned int * originalGteState = (unsigned int *) 0x80609000;
const unsigned int gteSize = 64;

/// @brief Specific addresses to exclude from tests.
/// @details There are specific addresses which cannot be under test because of the nature of running the test code, decompiled code, and original code.
///     * RAM sections that are used for extra register storage specifically the SP and RA fields.
const void* exclusions[] = {
    &_RegisterStorage_SP,
    &_RegisterStorage_RA
};

/// @brief Length of the exclusions list.
const int exclusionsLength = sizeof(exclusions) / sizeof(exclusions[0]);

/// @brief Address of where to run the tester installation code.
extern unsigned int OG_Initialize;
unsigned int * hookAddress = &OG_Initialize;
unsigned int instructions[100];

void memCopy(unsigned int * dest, unsigned int * src, unsigned int size)
{
    for (unsigned int i = 0; i < size; i++)
    {
        dest[i] = src[i];
    }
}

/// @brief Returns true if the address is excluded from equality checks.
/// @param address Address of memory to verify if excluded.
/// @return boolean indicating excluded or not.
int isExcluded(unsigned int* address)
{
    for (int i = 0; i < exclusionsLength; i++)
    {
        if (exclusions[i] == (void*)address)
        {
            return 1;
        }
    }

    return 0;
}

int isEqual(unsigned int * dest, unsigned int * src, unsigned int size)
{
    int ret = 1;
    for (unsigned int i = 0; i < size; i++)
    {
        if (dest[i] != src[i])
        {
            if (isExcluded(&dest[i]) == 0)
            {
                printf("Diff at %x\n", &dest[i]);
                ret = 0;
            }
        }
    }
    return ret;
}

void BackupStartingState()
{
    // Backup the program state before calling the decomp function
    memCopy(initialRamState, ram, ramSize);
    memCopy(initialScratchpadState, scratchpad, scratchpadSize);
    gte_saveContext(initialGteState);
}

void BackupNewFunctionState()
{
    // Backup result of decomp function
    memCopy(decompRamState, ram, ramSize);
    memCopy(decompScratchpadState, scratchpad, scratchpadSize);
    gte_saveContext(decompGteState);
}

void LoadStartingState()
{
    // Load state
    memCopy(ram, initialRamState, ramSize);
    memCopy(scratchpad, initialScratchpadState, scratchpadSize);
    gte_loadContext(initialGteState);
}

void BackupOldFunctionState()
{
    gte_saveContext(originalGteState);
}

int IsMemoryEquivalent(void* destination, void* source, unsigned int size)
{
    int result = 1;
    for (unsigned int i = 0; i < size; i++)
    {
        if (((char*)destination)[i] != ((char*)source)[i])
        {
            printf("Failure. Difference at %d position in the structure.\n", i);
            result = 0;
        }
    }

    return result;
}

int IsStateEquivalent()
{
    if (!isEqual(ram, decompRamState, ramSize))
    {
        return 0;
    }

    if (!isEqual(originalGteState, decompGteState, gteSize))
    {
        return 0;
    }

    if (!isEqual(scratchpad, decompScratchpadState, scratchpadSize))
    {
        return 0;
    }

    return 1;
}

void* GetOriginalFunction()
{
    return &instructions[0];
}

/// @brief Creates the installation hook to run the test code.
/// @param tester 
/// @param oldFunc 
/// @param extraNops When specified other than '0', creates additional NOP instructions. This is to handle certain functions that have branches at the beginning.
void InstallHook(void *tester, void* oldFunc, int extraNops)
{
    unsigned int * oldFuncAddr = (unsigned int *) oldFunc;
    instructions[0] = oldFuncAddr[0];
    instructions[1] = oldFuncAddr[1];
    
    for (int i = 0; i < extraNops; i++)
    {
        instructions[i + 2] = oldFuncAddr[i + 2];
    }

    instructions[extraNops + 2] = J(&oldFuncAddr[extraNops + 2]);
    instructions[extraNops + 3] = NOP;

    oldFuncAddr[0] = J(tester);
    oldFuncAddr[1] = NOP;

    *hookAddress = JR_RA;
}