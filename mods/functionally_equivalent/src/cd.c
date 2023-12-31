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


void PlayMusicTrackA()
{
    if ((_CdUnknownFlags & 0x10U) == 0) 
        {
            if ((((_MusicFlags != 0) || (_CdUnknownCommand != 0)) && (_MusicFlags != 4)) && (_MusicFlags != 2)) 
            {
                _MusicFlags = 2;
            }
        }
        else if (_SpuCommonAttr.cd.volume.left == 0) 
        {
            _CdUnknownFlags = 0x40;
        } 
        else 
        {
            _VolumeChange = -(int) _SpuCommonAttr.cd.volume.left >> 3;
            _CdUnknownCommand = 9;
            _InitializeSoundU3 = 0;
            _CdUnknownFlags = 0x200;
        }
}


 //brief Plays/Continues a music track \n Address: 0x800567f4
 //details param_1 is the track to play. param_2 is the flags/mode. For param_2, 1 is to start at the beginning of the track, 8 is continue where it left off assuming it was saved.

void PlayMusicTrack(int track, int flags)
{
    char cVar1;
    int musicPlaying;
    u_char mode;
    CdlLOC cdLoc;
    CdlFILTER filter;

    if ((_CdUnknownFlags & 0x80U) != 0) 
    {
        return;
    }

    if (flags == 2) 
    {
        PlayMusicTrackA();
    }
    else 
    {
        cVar1 = (char) track;
        if (flags < 3) 
        {
            if (flags != 1) 
            {
                return;
            }

            if (((_CdUnknownFlags & 0x40U) == 0) || (_InitializeSoundU4 == 0)) 
            {
                musicPlaying = 1;

                if (_MusicFlags != 0)
                {
                    //goto PlayMusicTrack_B;

                    if (_MusicFlags == musicPlaying) 
                    {
                        return;
                    }

                    _MusicFlags = musicPlaying;
                    return;
                    //goto PlayMusicTrack_B;
                }

                musicPlaying = 1;

                if (_CdUnknownCommand == 0) 
                {
                    return;
                }

                if (_MusicFlags == musicPlaying) 
                {
                    return;
                }
                
                _MusicFlags = musicPlaying;
                return;
            }

            mode = 0xc8;
            CdControlB(CdlSetmode, & mode, (u_char * ) 0);
            filter.file = 1;
            musicPlaying = track;

            if (track < 0) 
            {
                musicPlaying = track + 7;
            }

            filter.chan = cVar1 + (char)(musicPlaying >> 3) * -8;
            CdControlB(CdlSetfilter, & filter.file, (u_char * ) 0);
            CdIntToPos(_SoundFiles[0].Files[track].Sector, & cdLoc);
            musicPlaying = CdControlB(CdlReadS, & cdLoc.minute, (u_char * ) 0);
        } 
        else 
        {
            if (flags == 4)
            {
                PlayMusicTrackA();
            }
            
            if (flags != 8) 
            {
                return;
            }

            if (((_CdUnknownFlags & 0x40U) == 0) || (_InitializeSoundU4 == 0)) 
            {
                musicPlaying = 8;

                if (_MusicFlags != 0)
                {
                    //goto PlayMusicTrack_B;
                    if (_MusicFlags == musicPlaying) 
                    {
                        return;
                    }

                    _MusicFlags = musicPlaying;
                    return;

                    //goto PlayMusicTrack_B;
                }

                musicPlaying = 8;

                //goto joined_r0x80056af8;
                if (_CdUnknownCommand == 0) 
                {
                    return;
                }

                if (_MusicFlags == musicPlaying) 
                {
                    return;
                }

                _MusicFlags = musicPlaying;
                return;

                //goto joined_r0x80056af8;
            }

            mode = 0xc8;
            CdControlB(CdlSetmode, & mode, (u_char * ) 0);
            filter.file = 1;

            if (track < 0) 
            {
                track = track + 7;
            }

            filter.chan = cVar1 + (char)(track >> 3) * -8;
            CdControlB(CdlSetfilter, & filter.file, (u_char * ) 0);
            CdIntToPos(_MusicSectorNumberA, & cdLoc);
            musicPlaying = CdControlB(CdlReadS, & cdLoc.minute, (u_char * ) 0);
        }

        _MusicFlags = flags;

        if (musicPlaying != 0) 
        {
            _CdUnknownCommand = 0;
            _MusicFlags = 0;
            _CdUnknownFlags = 0x10;
            _VolumeChange = _InitializeSoundU2 - _SpuCommonAttr.cd.volume.left >> 3;
            _InitializeSoundU3 = _InitializeSoundU2;
        }
    }
    return;
}