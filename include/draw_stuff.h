#pragma once

#include <moby.h>
#include <custom_types.h>

/**
 * @brief Draws a ps1 primitive to the screen. 
 * @details Places a primitive struct ptr into the array of primitives to be drawn every frame (Somewhat unsure)
 * 
 * @param void* hudMobyInfo - pointer to basic info about the arrow you want to draw (x, y, size)

 * @note Function: DrawPrimitve \n
 * Original Address: 0x800168dc \n
 * Hook File: draw_primitive.s \n 
 * Prototype: draw_stuff.h \n
 * Amount of instructions: Same Amount (https://decomp.me/scratch/wd3wE) \n 
*/
void DrawPrimitive(int primitive);



/// @brief Draws a blinking text arrow facing left or right.
/// @param position Position to draw on the screen.
/// @param timer Timer that counts every frame to be used for the pre-determined 16 frame intervals.
/// @param leftOrRight Determines if the arrow points left or right.
void DrawTextArrow(Vector3D *position, uint timer, int leftOrRight);

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

