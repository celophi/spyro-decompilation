#pragma once

#include <moby.h>
#include <custom_types.h>
#include <shapes.h>

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

/// @brief Calculates the 8-bit difference between the supplied value and a timer, then clamps between 0 and 127.
/// @param value Supplied value.
/// @param timer In most cases, the value of a cyclical timer in the game.
/// @return Calculated value.
int GetClampedDifference(int value, int timer);



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

/// @brief Draws the sky for cutscenes and levels.
/// @details Draws the skybox rendering primitives from the WAD based on camera perspectives.
///     - Address: 0x8004eba8
///     - Hook: DrawSkybox.s
///     - Test: DrawSkyboxTest.c
///     - Test Status: Passing
/// @param index Used for specifying a selection of sky models to render.
/// @param cameraA 
/// @param cameraB 
void DrawSkybox(int index, RotationMatrix *cameraA, RotationMatrix *cameraB);