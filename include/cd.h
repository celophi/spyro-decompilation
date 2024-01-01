#pragma once

#include <common.h>

/// @brief Callback to execute after reading from the CD-ROM.
/// @param status Result of the CD read operation.
/// @note Original Address: 0x80016490
void CdReadCallbackFunc(char status);

/// @brief Initializes the CD-ROM, sets the read callback, reads sectors.
/// @note Original Address: 0x800163e4
void CdReadSectors();


byte CdStatus(void);

void PlayMusicTrack(int track,int flags);
