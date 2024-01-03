#pragma once

#include <moby.h>
#include <custom_types.h>
#include <shapes.h>

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

/**
 * @brief Adds a primitive to a linked list.
 * @details Seems to be a staging area that gets turned into an ordering table later.
 * @note
 *      - Address: 0x800168dc
 *      - Hook: AddPrimitiveToList.s
 *      - Test: AddPrimitiveToListTest.c
 * @param primitive Reference to a primitive to stage.
*/
void AddPrimitiveToList(P_TAG *primitive);

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

/// @brief Calculates the 8-bit difference between the supplied value and a timer, then clamps between 0 and 127.
/// @param value Supplied value.
/// @param timer In most cases, the value of a cyclical timer in the game.
/// @return Calculated value.
int GetClampedDifference(int value, int timer);

/// @brief Draws a blinking text arrow facing left or right.
/// @param position Position to draw on the screen.
/// @param timer Timer that counts every frame to be used for the pre-determined 16 frame intervals.
/// @param leftOrRight Determines if the arrow points left or right.
void DrawTextArrow(Vec3u32 *position, uint timer, int leftOrRight);

/**
 * @brief Draws a black textbox with a gold border. You can specify the bounds
 * @details Calls Draw primitive with the interesting hack they use for the alpha channel to achieve the black background, and also 4 draw lines for the border.

 * @param int xBound1 - the first x boundry of the box
 * @param int xBound2 - the second x boundry of the box
 * @param int yBound1 - the first y boundry of the box
 * @param int yBound2 - the second y boundry of the box

 * @note Function: DrawTextbox \n
 * Original Address: 0x8001860C \n
 * Hook File: draw_textbox.s \n
 * Prototype: draw_stuff.h \n
 * Amount of instructions: MORE IN MODDERN GCC (https://decomp.me/scratch/MrfVL) \n
*/
void DrawTextbox(int xBound1, int xBound2, int yBound1, int yBound2);
