#include <common.h>
#include <gte.h>
#include <draw_text.h>
#include <vector.h>

// MIPS Instructions
#define J(addr) ((((unsigned int) addr & 0xFFFFFF) >> 2) | 0x08000000)
#define JR_RA 0x03E00008
#define NOP   0x00000000

typedef void (*func)(Vec3u32* vector, int denominator, int numerator);
func newFunc = (func) &ScaleVector;
int newRes;
func oldFunc = (func) &OriginalScaleVector;
int ogRes;

unsigned int * hookAddress = (unsigned int *) (0x8005b988);
unsigned int instructions[4];


/*
unsigned int * hookAddress = (unsigned int *) (0x8005b988);
unsigned int instructions[4];

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
unsigned int hits = 0;
unsigned int total = 0;

*/

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
unsigned int hits = 0;
unsigned int total = 0;


/*
    The credit for the first two functions goes to Nicolas Noble,
    author of the openbios project. I'm calling these functions to
    disable interrupts, so that the PSX RAM is guaranteed to not change
    during all our function calls.
    https://github.com/grumpycoders/pcsx-redux/blob/main/src/mips/common/syscalls/syscalls.h#L39-L57
*/
static __attribute__((always_inline)) int enterCriticalSection() {
    register int n asm("a0") = 1;
    register int r asm("v0");
    __asm__ volatile("syscall\n"
                     : "=r"(n), "=r"(r)
                     : "r"(n)
                     : "at", "v1", "a1", "a2", "a3", "t0", "t1", "t2", "t3", "t4", "t5", "t6", "t7", "t8", "t9",
                       "memory");
    return r;
}

static __attribute__((always_inline)) void leaveCriticalSection() {
    register int n asm("a0") = 2;
    __asm__ volatile("syscall\n"
                     : "=r"(n)
                     : "r"(n)
                     : "at", "v0", "v1", "a1", "a2", "a3", "t0", "t1", "t2", "t3", "t4", "t5", "t6", "t7", "t8", "t9",
                       "memory");
}

void memCopy(unsigned int * dest, unsigned int * src, unsigned int size)
{
    for (unsigned int i = 0; i < size; i++)
        dest[i] = src[i];
}

int isEqual(unsigned int * dest, unsigned int * src, unsigned int size)
{

    printf("Start %x. End: %x", &dest[0], dest[size-1]);
    int ret = 1;
    for (unsigned int i = 0; i < size; i++)
    {
        if (dest[i] != src[i])
        {
            printf("Diff at %x. Original: %x. New: %x\n", &dest[i], dest[i], src[i]);
            ret = 0;
        }
    }
    return ret;
}

// The Tester() function takes the same signature as the function you're decompiling
void Tester(Vec3u32* vector, int denominator, int numerator)
{
    enterCriticalSection();
    
    printf("Address: %x", &vector);

    // Backup the program state before calling the decomp function
    printf("Backup RAM 1\n");
    memCopy(ram_4mb, ram, ramSize);
    printf("Backup Scratch 1\n");
    memCopy(scratchpadBackup_4mb, scratchpad, scratchpadSize);
    printf("Backup GTE 1\n");
    gte_saveContext(gteBackup_4mb);

    // Call the decomp function
    printf("call new func");
   
    newFunc(vector, denominator, numerator);


    // Backup result of decomp function
    printf("Backup RAM 2\n");
    memCopy(ram_6mb, ram, ramSize);
    printf("Backup Scratch 2\n");
    memCopy(scratchpadBackup_6mb, scratchpad, scratchpadSize);
    printf("Backup GTE 2\n");
    gte_saveContext(gteBackup_6mb);


    // Load state
    printf("Load RAM 1\n");
    memCopy(ram, ram_4mb, ramSize);
    printf("Load Scratch 1\n");
    memCopy(scratchpad, scratchpadBackup_4mb, scratchpadSize);
    printf("Load GTE 1\n");
    gte_loadContext(gteBackup_4mb);

    // Call the original function
    func ogFunc = (func) &instructions[0];
    printf("call old func");
    ogFunc(vector, denominator, numerator);
    gte_saveContext(gteBackup_4mb);

    total++;
    // Comparing the results of the original function and the decomp function
    if ((newRes == ogRes) &&
        (isEqual(gteBackup_4mb, gteBackup_6mb, gteSize)) &&
        (isEqual(scratchpad, scratchpadBackup_6mb, scratchpadSize)) &&
        (isEqual(ram, ram_6mb, ramSize))) {
        hits++;
    }
    printf("Hits/totals: %d/%d\n", hits, total);
    leaveCriticalSection();
}

void InstallHook()
{
    unsigned int * oldFuncAddr = (unsigned int *) oldFunc;
    instructions[0] = oldFuncAddr[0];
    instructions[1] = oldFuncAddr[1];
    instructions[2] = J(&oldFuncAddr[2]);
    instructions[3] = NOP;

    // Rewrite the old func address instructions with jump to tester
    oldFuncAddr[0] = J(&Tester);
    oldFuncAddr[1] = NOP;

    *hookAddress = JR_RA;
}