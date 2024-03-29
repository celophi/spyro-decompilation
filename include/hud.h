#pragma once

#include <vector.h>
#include <moby.h>
#include <custom_types.h>

/// @brief Indicates the animation state of Moby objects on the HUD.
typedef byte HudAnimation;
enum
{
    /// @brief Moby is not visible and not displayed.
    HIDDEN = 0,

    /// @brief Moby is coming into view moving downward.
    DOWN_TRANSITION = 1,

    /// @brief Moby has stopped moving down and is now rotating.
    ROTATING = 2,

    /// @brief Moby is moving upward out of display.
    UP_TRANSITION = 3,

    /// @brief Specific state that displays the treasure once 100% completion is reached in the level.
    ALL_COLLECTIBLES_OBTAINED = 4,
};
_Static_assert(sizeof(HudAnimation) == 1);

typedef struct 
{
    Vec2u16 V1;
    Vec2u16 V2;
} HudOvalVertex;

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
 *      - Test Status: Passing
 * @param vg Unknown. Something with vertices.
 * @param parameter Unknown. Something to do with textures.
 * @param rgb Colors to set on the primitive.
*/
void DrawHudOval(const HudOvalVertex* vg, const TextureRelatedUnk* parameter, const RGBu32* rgb);

/**
 * @brief Draws Mobys on the HUD.
 * @details Mobys for the treasure, lives, life orbs, eggs, dragons, are drawn.
 * @note
 *      - Address: 0x80019300
 *      - Hook: DrawHudMobys.s
 *      - Test: DrawHudMobysTest.c
 *      - Test Status: Passing
*/
void DrawHudMobys();