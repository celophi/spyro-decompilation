#include <common.h>
#include <symbols.h>
#include <PSYQ/LIBCD.h>
#include <PSYQ/LIBGPU.h>

/// @brief Callback to execute after reading from the CD-ROM.
/// @param status Result of the CD read operation.
/// @param result Unused
/// @note Original Address: 0x80016490
void CdReadCallbackFunc(u_char status, u_char* result) 
{
    if (status == CdlComplete)
    {
        _CdReadCallback_U0 = 0;
        _LoadingTimer = 0;
        _CdReading = 0;
        return;
    }

    CdControl(CdlSetloc, (u_char *)&_CdlLOC, 0);
    CdRead(_CdReadSectorsParameter, _CdReadBuffer, CdlModeSpeed);
}

/// @brief Initializes the CD-ROM, sets the read callback, reads sectors.
/// @note Original Address: 0x800163e4
void CdReadSectors()
{
    if (_CdReading == 0 || _CdReading >= _LoadingTimer)
    {
        return;
    }

    CdInit();
    CdReadCallback(CdReadCallbackFunc);

    _CdReadSectors_U0 = 64;
    _CdReadSectors_U1 = 0;
    _CdReadSectors_U2 = 0;

    int status = CdlNoIntr;
    while (status != CdlComplete)
    {
        status = CdSync(1, 0);
    }

    _LoadingTimer = 0;
    CdRead(_CdReadSectorsParameter, _CdReadBuffer, CdlModeSpeed);
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