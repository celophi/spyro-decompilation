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
    _CdUnknownCommand = 0;
    _MusicFlags = 0;

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

/// @brief Plays music from soundtrack files.
/// @param track Track of music to play.
/// @param flags Certain flags that control how playback happens.
void PlayMusicTrack(int track, int flags)
{
    // Must be unset in order to play music.
    if ((_CdUnknownFlags & 0x80U) != 0) 
    {
        return;
    }

    // starting new track or continuing?
    if (flags == 1 || flags == 8)
    {
        if (((_CdUnknownFlags & 0x40U) == 0) || (_InitializeSoundU4 == 0)) 
        {
            if (_MusicFlags == 0 && _CdUnknownCommand == 0)
            {   
                return;
            }

            _MusicFlags = flags;
            return;
        }

        u_char mode = CdlModeSF | CdlModeRT | CdlModeSpeed;
        CdControlB(CdlSetmode, &mode, NULL);

        int channelValue = track;

        if (track < 0) 
        {
            channelValue = track + 7;
        }
        
        CdlFILTER filter;
        filter.file = 1;
        filter.chan = (char) track + (char)(channelValue >> 3) * -8;
        CdControlB(CdlSetfilter, (u_char *)&filter, NULL);

        int sector = flags == 1 ? _SoundFiles[0].Files[track].Sector : _MusicSectorNumberA;

        CdlLOC cdLoc;
        CdIntToPos(sector, &cdLoc);
        CdControlB(CdlReadS, (u_char *)&cdLoc, NULL);

        _CdUnknownCommand = 0;
        _MusicFlags = 0;
        _CdUnknownFlags = 0x10;

        // _InitializeSoundU2 usually is 0x5000
        _VolumeChange = _InitializeSoundU2 - _SpuCommonAttr.cd.volume.left >> 3;
        _InitializeSoundU3 = _InitializeSoundU2;

        return;
    }

    // pause? seems like turning the volume all the way down also pauses.
    if (flags == 2 || flags == 4) 
    {
        // bitfield 0x10 is set during play/continue, so maybe this is making sure it is in that state.
        if ((_CdUnknownFlags & 0x10U) == 0) 
        {
            if ((((_MusicFlags != 0) || (_CdUnknownCommand != 0)) && (_MusicFlags != 4)) && (_MusicFlags != 2)) 
            {
                _MusicFlags = 2;
            }

            return;
        }
        
        if (_SpuCommonAttr.cd.volume.left == 0) 
        {
            _CdUnknownFlags = 0x40;
            return;
        } 
        
        _VolumeChange = -(int) _SpuCommonAttr.cd.volume.left >> 3;
        _CdUnknownCommand = CdlPause;
        _InitializeSoundU3 = 0;
        _CdUnknownFlags = 0x200;

        return;
    }

    _MusicFlags = flags;
}