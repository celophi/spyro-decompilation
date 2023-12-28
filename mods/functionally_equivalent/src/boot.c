#include <PSYQ/LIBGPU.h>
#include <PSYQ/LIBAPI.h>
#include <graphics.h>
#include <common.h>

/// @brief Initializes the display and graphics environments.
/// @note Original Address: 0x800122a8
void InitializeDisplay()
{
    SetDispMask(0);
    ResetGraph(0);
    SetGraphDebug(0);

    SetDefDrawEnv(&_GfxEnvA.DrawEnv, 0, 8, 512, 224);
    SetDefDrawEnv(&_GfxEnvB.DrawEnv, 0, 248, 512, 224);
    SetDefDispEnv(&_GfxEnvA.DispEnv, 0, 240, 512, 240);
    SetDefDispEnv(&_GfxEnvB.DispEnv, 0, 0, 512, 240);

    _GfxEnvB.DrawEnv.ofs[1] = 240;
    _GfxEnvA.DrawEnv.isbg = 1;
    _GfxEnvB.DrawEnv.isbg = 1;
    _GfxEnvA.DrawEnv.dtd = 1;
    _GfxEnvB.DrawEnv.dtd = 1;
    _GfxEnvA.DrawEnv.ofs[1] = 0;

    RECT rect = { .x = 0, .y = 0, .w = 512, .h = 512 };

    ClearImage(&rect, 0, 0, 0);
    DrawSync(0);
    SetDispMask(1);
}

/// @brief Initializes the memory cards.
/// @note Original Address: 0x80012460
void InitializeMemoryCards()
{
    _InitializeMemoryCards_u0 = 0;
    _InitializeMemoryCards_u1 = 0;

    InitCARD(0);
    StartCARD();
    _bu_init();
}