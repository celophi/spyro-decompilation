#pragma once

#include <custom_types.h>
#include <vector.h>
#include <moby.h>

/**
 * @brief Draws the demo text on the screen.
 * @details Creates HUD mobys for displaying the text "Demo Mode" with varying size text.

 * @note Function: DrawDemoText \n
 * Original Address: 0x80018908 \n
 * Hook File: draw_demo_text.s \n
 * Prototype: draw_text.h \n
 * Amount of instructions: MORE IN MODERN GCC (https://decomp.me/scratch/vU390) \n
*/
void DrawDemoText();

/**
 * @brief Appends generated text objects to the list of HUD mobys.
 * @details
 * @note Function: CopyHudToShaded \n
 * Original Address: 0x80018880 \n
 * Hook File: copy_hud_to_shaded.s \n 
 * Prototype: draw_text.h \n
 * Amount of instructions: Same Amount (https://decomp.me/scratch/rbY0g) \n 
*/
void CopyHudToShaded();