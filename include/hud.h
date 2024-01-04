#pragma once

#include <vector.h>
#include <moby.h>

typedef struct 
{
    Vec2u16 V1;
    Vec2u16 V2;
} HudLifeRelated_U0;

typedef struct
{
    byte u0;
    byte v0;
    short clut;
    byte u2;
    byte v2;
    short tpage;
} HudDouble;

typedef struct
{
    // 10 could be for each of the mobys?
    HudDouble U1[10];
    int Padding;
} TextureRelatedUnk;

typedef struct
{
    int R;
    int G;
    int B;
} RGBu32;

typedef struct
{
    NewMoby Mobys[12];
} PauseMenuState;

/**
 * @brief Draws an ellipsoid/ovoid on the HUD.
 * @details Used for drawing the life orbs and the eggs.
 * @note
 *      - Address: 0x8001919c
 *      - Hook: DrawHudOval.s
 *      - Test: DrawHudOvalTest.c
 * @param vg Unknown. Something with vertices.
 * @param parameter Unknown. Something to do with textures.
 * @param rgb Colors to set on the primitive.
*/
void DrawHudOval(const HudLifeRelated_U0* vg, const TextureRelatedUnk* parameter, const RGBu32* rgb);

void DisplayHudRelated();