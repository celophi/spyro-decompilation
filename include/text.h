#pragma once

#include <custom_types.h>
#include <vector.h>
#include <moby.h>

/// @brief Draws text in all capitals.
/// @note
///     - Address: 0x80017fe4
///     - Hook: DrawCapitalText.s
///     - Test: DrawCapitalTextTest.c
///     - Test Status: Passing
/// @param text String of text to draw.
/// @param position Position on the screen to start drawing.
/// @param spacing Horizontal space between each character. 
/// @param color Color of the text.
/// @return Pointer to the list of mobys to draw.
NewMoby* DrawCapitalText(char* text, Vec3u32* position, int spacing, byte color);



/// @brief Draws a text string to the screen where the first letter of a word is capitalized and the following are lowercase.
/// @note
///     - Address: 0x800181ac
///     - Hook: DrawVaryingText.s
///     - Test: DrawVaryingTextTest.c
///     - Test Status: In Progress (Passes on spaces and letters. Need more testing on symbols.)
/// @param text String to write on the screen.
/// @param capitalPosition Position where capital letters should begin.
/// @param lowercasePosition Position where lowercase letters should follow.
/// @param spacing Horizontal spacing between characters.
/// @param color Color to apply to each letter.
/// @return Pointer to the text mobys.
NewMoby* DrawVaryingText(char *text, Vec3u32 *capitalPosition, Vec3u32 *lowercasePosition, int spacing, byte color);