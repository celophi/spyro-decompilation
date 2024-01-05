#include <common.h>
#include <cd.h>
#include <symbols.h>

/// @brief Resets some timers maybe?
void ResetHudTimers()
{
    _TimerA = -2517;
    _TimerB = -2458;
    _TimerC = -2848;
    _TimerD = -2517;
    _TimerE = -2458;
    _TimerF = -2848;
}

/**
 * @brief Performs game play restoration logic when exiting the pause menu.
 * @note
 *      - Address: 0x8002c534
 *      - Hook: ExitPauseMenu.s
 *      - Test: ExitPauseMenu.c
 *      - Test Status: Mostly Passing (Note that this only passes if you mock PlayMusicTrack behavior)
 * @param resumeMusic When 'true', resumes a previously paused music track.
*/
void ExitPauseMenu(int resumeMusic)
{
    RECT rect = { .x = 512, .y = 0, .w = 256, .h = 225 };
    LoadImage(&rect, _FrameBufferStorage - 28800);
    
    DrawSync(0);
    ResetHudTimers();

    _GameState = GAMEPLAY;
    _HudChestState = UP_TRANSITION;
    _HudDragonState = UP_TRANSITION;
    _HudLivesState = UP_TRANSITION;
    _HudEggsState = UP_TRANSITION;
    _HudKeyState = UP_TRANSITION;
    _HudChestAnimationFrame = 13;
    _HudDragonAnimationFrame = 13;
    _HudLivesAnimationFrame = 13;
    _HudEggsAnimationFrame = 13;
    _HudKeyAnimationFrame = 13;

    if (resumeMusic)
    {
        PlayMusicTrack(_MusicFile, 8);
    }
}

/// @brief Handles the required steps for exiting the inventory menu.
/// @note Original Address: 0x8002c7bc
void ExitInventoryMenu()
{
    RECT rect = { .x = 512, .y = 0, .w = 256, .h = 225 };
    
    LoadImage(&rect, _FrameBufferStorage - 28800);
    DrawSync(0);

    _GameState = GAMEPLAY;

    ResetHudTimers();

    _HudChestState = HIDDEN;
    _HudDragonState = HIDDEN;
    _HudLivesState = HIDDEN;
    _HudEggsState = HIDDEN;
    _HudKeyState = HIDDEN;

    PlayMusicTrack(_MusicFile, 8);
}