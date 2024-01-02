#pragma once

#include <common.h>
#include <PSYQ/LIBCD.h>

/// @brief Initializes the CD-ROM, sets the read callback, reads sectors.
/// @note Original Address: 0x800163e4
void CdReadSectors();


byte CdStatus(void);

void PlayMusicTrack(int track,int flags);
