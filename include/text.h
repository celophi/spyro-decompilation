#pragma once

#include <custom_types.h>
#include <vector.h>
#include <moby.h>

/// @brief Determines the direction to draw a text arrow on the screen.
typedef int DrawTextArrowDirection;
enum
{
    POINT_LEFT = 0,
    POINT_RIGHT = 1,
    POINT_UP = 2
};
_Static_assert(sizeof(DrawTextArrowDirection) == 4);

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

/// @brief Copies generated moby structs to the end of a global list.
/// @details Appears to be something that is called often when working with text.
///     - Address: 0x80018880
///     - Hook: CopyHudToShaded.s
///     - Test: CopyHudToShadedTest.c
///     - Test Status: Passing
void CopyHudToShaded();

/// @brief Draws the demo text on the screen.
/// @note
///     - Address: 0x80018908
///     - Hook: DrawDemoText.s
///     - Test: DrawDemoTextTest.c
///     - Test Status: Passing
void DrawDemoText();

/// @brief Draws a blinking text arrow facing left or right.
/// @note
///     - Address: 0x80018534
///     - Hook: DrawTextArrow.s
///     - Test: DrawTextArrowTest.c
///     - Test Status: Passing
/// @param position Position to draw on the screen.
/// @param timer Timer that counts every frame to be used for the pre-determined 16 frame intervals.
/// @param direction Determines if the arrow points left or right.
void DrawTextArrow(Vec3u32* position, uint timer, DrawTextArrowDirection direction);