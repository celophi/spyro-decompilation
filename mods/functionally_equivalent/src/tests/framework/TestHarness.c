#include <tests/framework/TestHarness.h>
#include <common.h>
#include <gte.h>

// MIPS Instructions
#define J(addr) ((((unsigned int) addr & 0xFFFFFF) >> 2) | 0x08000000)
#define JR_RA 0x03E00008
#define NOP 0x00000000

unsigned int * ram = (unsigned int *) 0x80010000;
unsigned int * ram_4mb = (unsigned int *) 0x80410000;
unsigned int * ram_6mb = (unsigned int *) 0x80610000;
const unsigned int ramSize = (0x1FF800 - 0x10000) / 4;
unsigned int * scratchpad = (unsigned int *) 0x1F800000;
unsigned int * scratchpadBackup_4mb = (unsigned int *) 0x80400000;
unsigned int * scratchpadBackup_6mb = (unsigned int *) 0x80600000;
const unsigned int scratchpadSize = 0x400 / 4;
unsigned int * gteBackup_4mb = (unsigned int *) 0x80408000;
unsigned int * gteBackup_6mb = (unsigned int *) 0x80608000;
const unsigned int gteSize = 64;


/// @brief Address of where to run the tester installation code.
extern unsigned int OG_Initialize;
unsigned int * hookAddress = &OG_Initialize;
unsigned int instructions[4];

void memCopy(unsigned int * dest, unsigned int * src, unsigned int size)
{
    for (unsigned int i = 0; i < size; i++)
    {
        dest[i] = src[i];
    }
}

int isEqual(unsigned int * dest, unsigned int * src, unsigned int size)
{
    int ret = 1;
    for (unsigned int i = 0; i < size; i++)
    {
        if (dest[i] != src[i])
        {
            printf("Diff at %x\n", &dest[i]);
            ret = 0;
        }
    }
    return ret;
}

void BackupStartingState()
{
    // Backup the program state before calling the decomp function
    memCopy(ram_4mb, ram, ramSize);
    memCopy(scratchpadBackup_4mb, scratchpad, scratchpadSize);
    gte_saveContext(gteBackup_4mb);
}

void BackupNewFunctionState()
{
    // Backup result of decomp function
    memCopy(ram_6mb, ram, ramSize);
    memCopy(scratchpadBackup_6mb, scratchpad, scratchpadSize);
    gte_saveContext(gteBackup_6mb);
}

void LoadStartingState()
{
    // Load state
    memCopy(ram, ram_4mb, ramSize);
    memCopy(scratchpad, scratchpadBackup_4mb, scratchpadSize);
    gte_loadContext(gteBackup_4mb);
}

void BackupOldFunctionState()
{
    gte_saveContext(gteBackup_4mb);
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
    if (!isEqual(gteBackup_4mb, gteBackup_6mb, gteSize))
    {
        return 0;
    }

    if (!isEqual(scratchpad, scratchpadBackup_6mb, scratchpadSize))
    {
        return 0;
    }

    if (!isEqual(ram, ram_6mb, ramSize))
    {
        return 0;
    }

    return 1;
}

void* GetOriginalFunction()
{
    return &instructions[0];
}

void InstallHook(void *tester, void* oldFunc)
{
    unsigned int * oldFuncAddr = (unsigned int *) oldFunc;
    instructions[0] = oldFuncAddr[0];
    instructions[1] = oldFuncAddr[1];
    instructions[2] = J(&oldFuncAddr[2]);
    instructions[3] = NOP;

    oldFuncAddr[0] = J(tester);
    oldFuncAddr[1] = NOP;

    *hookAddress = JR_RA;
}