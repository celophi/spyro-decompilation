#pragma once

/*
    The credit for the first two functions goes to Nicolas Noble,
    author of the openbios project. I'm calling these functions to
    disable interrupts, so that the PSX RAM is guaranteed to not change
    during all our function calls.
    https://github.com/grumpycoders/pcsx-redux/blob/main/src/mips/common/syscalls/syscalls.h#L39-L57
*/
static __attribute__((always_inline)) int EnterCriticalSection() 
{
    register int n asm("a0") = 1;
    register int r asm("v0");
    __asm__ volatile("syscall\n"
                     : "=r"(n), "=r"(r)
                     : "r"(n)
                     : "at", "v1", "a1", "a2", "a3", "t0", "t1", "t2", "t3", "t4", "t5", "t6", "t7", "t8", "t9",
                       "memory");
    return r;
}

static __attribute__((always_inline)) void LeaveCriticalSection() 
{
    register int n asm("a0") = 2;
    __asm__ volatile("syscall\n"
                     : "=r"(n)
                     : "r"(n)
                     : "at", "v0", "v1", "a1", "a2", "a3", "t0", "t1", "t2", "t3", "t4", "t5", "t6", "t7", "t8", "t9",
                       "memory");
}


void memCopy(unsigned int * dest, unsigned int * src, unsigned int size);
int isEqual(unsigned int * dest, unsigned int * src, unsigned int size);
void BackupStartingState();
void BackupNewFunctionState();
void LoadStartingState();
void BackupOldFunctionState();
int IsMemoryEquivalent(void* destination, void* source, unsigned int size);
int IsStateEquivalent();
void* GetOriginalFunction();
void InstallHook(void *tester, void* oldFunc, int extraNops);