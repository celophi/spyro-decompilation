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

/// @brief Handles the required steps for exiting the inventory menu.
/// @note Original Address: 0x8002c7bc
void ExitInventoryMenu()
{
    RECT rect = { .x = 512, .y = 0, .w = 256, .h = 225 };
    
    LoadImage(&rect, _ExitInventoryMenuU0 - 0x7080);
    DrawSync(0);

    _GameState = 0;

    ResetHudTimers();

    _HudChestState = Hidden;
    _HudDragonState = 0;
    _HudLivesState = 0;
    _HudEggsState = 0;
    _HudAnimationState = 0;

    PlayMusicTrack(_MusicFile, 8);
}