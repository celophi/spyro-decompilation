#include <common.h>
#include <moby.h>
#include <vector.h>
#include <custom_types.h>
#include <memory.h>

/** @ingroup reveresed_functions
 *  @{
 */


bool IsCharacterQuestionMark(const unsigned int character) {
    return character == 63;
}

bool IsCharacterPeriod(const unsigned int character) {
    return character == 46;
}

bool IsUppercaseAlpha(const unsigned int character) {
    return character - 65 < 26;
}

bool IsCharacterNumberOrSymbol(const unsigned int character) {
    return character - 48 < 10;
}

bool IsCharacterExclamationPoint(const unsigned int character) {
    return character == 33;
}

bool IsCharacterComma(const unsigned int character) {
    return character == 44;
}

bool IsCharacterPercent(const unsigned int character) {
    return character == 37;
}

bool IsCharacterCaret(const unsigned int character) {
    return character == 94;
}

bool IsCharacterPlus(const unsigned int character) {
    return character == 43;
}

bool IsCharacterForwardSlash(const unsigned int character) {
    return character == 47;
}

bool IsCharacterSpace(const unsigned int character) {
    return character == 32;
}

void SetTextAreaFormatAndColor(byte color) {

    _ptr_hudMobys->requiredHUD1 = 0x7F;
    _ptr_hudMobys->color = color;
    _ptr_hudMobys->requiredHUD2 = 0xFF;
}

/// @brief Grows the global text area list and copies the Point3D to it. TextArea grows backwards.
/// @param point 
void GrowTextArea(Vec3u32* position) {
    _ptr_hudMobys = _ptr_hudMobys - 1;
    memset(_ptr_hudMobys, 0, sizeof(Moby));
    CopyVector3D(&_ptr_hudMobys->position, position);
}


/// @brief Formats text in a way similar to title casing, but the first letter is just a different size than the rest.
/// @param text String to render
/// @param leadingDimensions The dimensions to apply to the first character in each word.
///     X = Screen offset from the left.
///     Y = Screen offset from the top.
///     Z = Distance away from the camera.
/// @param followingDimensions
///     X = Spacing between minor characters.
///     Y = Offset from the "Y" position of the leadingDimensions.
///     Z = Distance away from the camera.
/// @param spacing Spacing away from major characters to start drawing minor ones.
/// @param color Color of the text
/// @return 
Moby* DrawVaryingText(char* text, Vec3u32* leadingDimensions, Vec3u32* followingDimensions, byte spacing, byte color) {
    bool bVar1;
    int horizontalAdjustment;
    unsigned int currentCharacter;
    Moby* textArea;
    char textPtr;

    bVar1 = 1;
    textPtr = *text;

    while (textPtr != 0) {
        if (IsCharacterSpace(textPtr)) {
            horizontalAdjustment = followingDimensions->X * 3;
            bVar1 = true;

            if (horizontalAdjustment < 0) {
                horizontalAdjustment = horizontalAdjustment + 3;
            }

            leadingDimensions->X = leadingDimensions->X + (horizontalAdjustment >> 2);
        }
        else {
            GrowTextArea(leadingDimensions);

            textArea = _ptr_hudMobys;
            // if character is ! or ?
            if ((IsCharacterExclamationPoint(*text)) || (IsCharacterQuestionMark(*text))) {
                bVar1 = true;
            }

            if (!bVar1) {
                (_ptr_hudMobys->position).Y = (_ptr_hudMobys->position).Y + followingDimensions->Y;
                (textArea->position).Z = followingDimensions->Z;
            }

            textArea = _ptr_hudMobys;
            textPtr = *text;
            currentCharacter = (unsigned int)textPtr;

            // if the character is a number or symbol
            if (IsCharacterNumberOrSymbol(currentCharacter)) {
                _ptr_hudMobys->type = textPtr + 212;
            }
            else {
                if (IsUppercaseAlpha(currentCharacter)) {
                    _ptr_hudMobys->type = textPtr + 361;
                }
                else {
                    if (IsCharacterExclamationPoint(currentCharacter)) {
                        _ptr_hudMobys->type = 75;
                    }
                    else {
                        if (IsCharacterComma(currentCharacter)) {
                            _ptr_hudMobys->type = 76;
                        }
                        else {
                            if (IsCharacterQuestionMark(currentCharacter)) {
                                _ptr_hudMobys->type = 278;
                            }
                            else if (IsCharacterPeriod(currentCharacter)) {
                                _ptr_hudMobys->type = 327;
                            }
                            else {

                                // apostraphe's in all other cases (which is actually the comma but moved upward!)
                                _ptr_hudMobys->type = 76;
                                (textArea->position).Y = (textArea->position).Y - (followingDimensions->X << 1) / 3;
                            }
                        }
                    }
                }
            }

            SetTextAreaFormatAndColor(color);

            if (bVar1) {
                horizontalAdjustment = leadingDimensions->X + spacing;
            }
            else {
                horizontalAdjustment = leadingDimensions->X + followingDimensions->X;
            }

            leadingDimensions->X = horizontalAdjustment;
            bVar1 = *text - 48 < 10;
        }

        text = text + 1;
        textPtr = *text;
    }

    // In newer decompilations from Ghidra there is some extra stuff here????
    // Might be missing it...

    return _ptr_hudMobys;
}




void TranslateCharacter(char* text) {
    
    if (IsCharacterNumberOrSymbol(*text)) {
        _ptr_hudMobys->type = *text + 212;
        return;
    }
    
    if (IsUppercaseAlpha(*text)) {
        _ptr_hudMobys->type = *text + 361;
        return;
    }

    if (IsCharacterForwardSlash(*text)) {
        _ptr_hudMobys->type = 277;
        return;
    }
    
    if (IsCharacterQuestionMark(*text)) {
        _ptr_hudMobys->type = 278;
        return;
    }
    
    if (IsCharacterPercent(*text)) {
        _ptr_hudMobys->type = 272;
        return;
    }

    if (IsCharacterCaret(*text)) {
        _ptr_hudMobys->type = 321;
        return;
    }

    if (IsCharacterPlus(*text)) {
        _ptr_hudMobys->type = 317;
        return;
    }

    _ptr_hudMobys->type = 327;
}

/// @brief Draws text in all capitals.
/// @param text String of text to draw.
/// @param position Position on the screen to start drawing.
/// @param spacing Horizontal space between each character. 
/// @param color Color of the text.
/// @return Pointer to the list of Mobys to draw.
Moby* DrawCapitalText(char* text, Vec3u32* position, int spacing, byte color) 
{
    while (*text != 0) 
    {
        if (!IsCharacterSpace(*text)) 
        {
            GrowTextArea(position);
            TranslateCharacter(text);
            SetTextAreaFormatAndColor(color);
        }

        position->X += spacing;
        text += 1;
    }

    return _ptr_hudMobys;
}









/**
 * @brief Copies generated moby structs to the end of a global list.
 * @details Appears to be something that is called often when working with text.
 
 * @note Function: CopyHudToShaded \n
   Original Address: 0x80018880 \n
   Hook File: copy_hud_to_shaded.s \n
   Prototype: draw_text.h \n
   Amount of instructions: More (https://decomp.me/scratch/rbY0g) \n
  * @see CopyHudToShaded()
*/
void CopyHudToShaded() {
  Moby **entry = &_ptr_hudMobysHead;
  Moby **ref = &_ptrTextUnk;

  while (*ref != 0) {                                                           // iterate to get to the end of the list.
    ref = entry;
    entry = ref + 1;
  }

  while (_ptr_hudMobys != _ptr_hudMobysQueue) {                                 // append all of the new mobys
    *ref = _ptr_hudMobys;
    _ptr_hudMobys += 1;
    ref += 1;
  }

  *ref = 0;                                                                     // make sure to set the end of the list to zero for iteration to work on the next call to this function. 
}

/**
 * @brief Draws the demo text on the screen.
 * @details Creates HUD mobys for displaying the text "Demo Mode" with varying size text.
 
 * @note Function: DrawDemoText \n
   Original Address: 0x80018908 \n
   Hook File: draw_demo_text.s \n
   Prototype: draw_text.h \n
   Amount of instructions: More (https://decomp.me/scratch/vU390) \n
  * @see DrawDemoText()
*/
void DrawDemoText() {
  Vec3u32 capitalTextInfo = { .X = 199, .Y = 200, .Z = 4352 };
  Vec3u32 lowercaseTextInfo = { .X = 16, .Y = 1, .Z = 5120 };
  const byte spacing = 18;
  const char color = 2;
  const int sinArrayIncrement = 12;

  Moby* mobyPtr = _ptr_hudMobys;
  unsigned int sinArrayIndex = 0;
  
  DrawVaryingText("DEMO MODE", &capitalTextInfo, &lowercaseTextInfo, spacing, color);
  mobyPtr--;
  
  while (_ptr_hudMobys <= mobyPtr) {
    mobyPtr->rotation.z = _sinArray[_levelTimer_60fps * 4 + sinArrayIndex & 0xFF] >> 7;
    mobyPtr--;
    sinArrayIndex += sinArrayIncrement;
  }

  CopyHudToShaded();
}
/** @} */ // end of reveresed_functions