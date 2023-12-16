#include <common.h>
#include <symbols.h>
#include <PSYQ/LIBCD.h>

/// @brief Callback to execute after reading from the CD-ROM.
/// @param status Result of the CD read operation.
/// @note Original Address: 0x80016490
void CdReadCallbackFunc(char status) 
{
    if (status == CdlComplete)
    {
        _CdReadCallback_U0 = 0;
        _LoadingTimer = 0;
        _CdReading = 0;
    }
    else
    {
        CdControl(CdlSetloc, (u_char *)&_CdlLOC, 0);
        CdRead(_CdReadSectorsParameter, _CdReadBuffer, CdlModeSpeed);
    }
}

/**
 * @brief Returns the status of the CD Drive.

 * @return byte - The CD drive status.

 * @note Function: CdStatus \n
 * Original Address: 0x80063a8c \n
 * Hook File: cd_status.s \n
 * Prototype: cd.h \n
 * Amount of instructions: Same Amount (https://decomp.me/scratch/C9fPW) \n

*/
byte CdStatus(void)

{
  return _cdStatus;
}