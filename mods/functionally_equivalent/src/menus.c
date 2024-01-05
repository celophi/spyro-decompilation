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

void ExitPauseMenu(int resumeMusic)
{
    RECT rect;

    rect.x = 512;
    rect.w = 256;
    rect.y = 0;
    rect.h = 225;

    LoadImage(&rect, _FrameBufferStorage - 28800);
    DrawSync(0);

    _GameState = 0;
    ResetHudTimers();

    _HudChestState = UpTransition;
    _HudDragonState = UpTransition;
    _HudLivesState = UpTransition;
    _HudEggsState = UpTransition;
    _HudKeyState = UpTransition;
    _HudChestAnimationFrame = 13;
    _HudDragonAnimationFrame = 13;
    _HudLivesAnimationFrame = 13;
    _HudEggsAnimationFrame = 13;
    _HudKeyAnimationFrame = 13;

    if (resumeMusic != 0) {
        PlayMusicTrack(_MusicFile,8);
    }
}

/// @brief Handles the required steps for exiting the inventory menu.
/// @note Original Address: 0x8002c7bc
void ExitInventoryMenu()
{
    RECT rect = { .x = 512, .y = 0, .w = 256, .h = 225 };
    
    LoadImage(&rect, _FrameBufferStorage - 28800);
    DrawSync(0);

    _GameState = 0;

    ResetHudTimers();

    _HudChestState = Hidden;
    _HudDragonState = Hidden;
    _HudLivesState = Hidden;
    _HudEggsState = Hidden;
    _HudKeyState = Hidden;

    PlayMusicTrack(_MusicFile, 8);
}