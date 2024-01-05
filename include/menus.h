#pragma once

/**
 * @brief Performs game play restoration logic when exiting the pause menu.
 * @note
 *      - Address: 0x8002c534
 *      - Hook: ExitPauseMenu.s
 *      - Test: ExitPauseMenu.c
 *      - Test Status: Mostly Passing (Note that this only passes if you mock PlayMusicTrack behavior)
 * @param resumeMusic When 'true', resumes a previously paused music track.
*/
void ExitPauseMenu(int resumeMusic);

/**
 * @brief Handles the required steps for exiting the inventory menu. 
 * @details Places texture back into vram, sets gamestate to gameplay, resets hud timers, continues music, etc

 * @note Function: ExitInventoryMenu \n
 * Original Address: 0x8002c7bc \n
 * Hook File: exit_inventory_menu.s \n 
 * Prototype: CURRENTLY N/A \n
 * Amount of instructions: Same Amount (https://decomp.me/scratch/HuLXz) \n 

*/
void ExitInventoryMenu(void);