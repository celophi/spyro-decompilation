#pragma once

#include <common.h>
#include <PSYQ/LIBCD.h>

/**
 * @brief Callback to execute after reading from the CD-ROM.
 * @param status Result of the CD read operation.
 * @param result Unused in this function.
 * @note
 *      - Address: 0x80016490
 *      - Hook: CdReadEventHandler.s
 *      - Test: CdReadEventHandlerTest.c
*/
void CdReadEventHandler(const u_char status, const u_char* result);

/// @brief Initializes the CD-ROM, sets the read callback, reads sectors.
/// @note Original Address: 0x800163e4
void CdReadSectors();


byte CdStatus(void);

void PlayMusicTrack(int track,int flags);