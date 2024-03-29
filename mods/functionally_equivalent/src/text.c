#include <text.h>
#include <memory.h>
#include <symbols.h>
#include <custom_types.h>

/// @brief Returns true if the character specified is a capital letter.
/// @param character 
/// @return Boolean indicating result.
static bool IsUppercaseAlpha(char character)
{
    return character >= 'A' && character <= 'Z';
}

/// @brief Returns true if the character specified is a number.
/// @param character 
/// @return Boolean indicating result.
static bool IsNumber(char character)
{
    return character >= '0' && character <= '9';
}

/// @brief Sets additional properties on the text Moby.
/// @param moby Moby to update.
/// @param color Color that the text moby should have.
/// @param position Position to draw the text moby.
static void SetTextMobyProperties(NewMoby* moby, byte color, Vec3u32* position) 
{
    moby->U13 = 0x7F;
    moby->U3 = 0xFF;
    moby->Color = color;
    CopyVector3D(&moby->Position, position);
}

/// @brief Creates space for a new Moby intended to be used with text.
/// @return Pointer to the new moby created.
static NewMoby* CreateTextMoby() 
{
    _MobyList--;
    memset(_MobyList, 0, sizeof(NewMoby));
    return _MobyList;
}

/// @brief Gets the appropriate moby type given the text character.
/// @note Different from GetVaryingTextMobyType because game crashes on those characters.
/// @param text Character to translate to type.
/// @return Moby type. Defaults to periods if not found.
static short GetCapitalTextMobyType(char* text) 
{
    if (IsNumber(*text)) 
    {
        return *text + 212;
    }
    
    if (IsUppercaseAlpha(*text)) 
    {
        return *text + 361;
    }

    switch (*text) 
    {
        case '/':
            return 277;
        case '?':
            return 278;
        case '%':
            return 272;
        case '^':
            return 321;
        case '+':
            return 317;
        default:
            return 327;
    }
}

/// @brief Gets the appropriate moby type given the text character
/// @note Different from GetCapitalTextMobyType because game crashes on those characters.
/// @param text Character to translate to type.
/// @return Moby type. Defaults to periods if not found.
static short GetVaryingTextMobyType(char* text)
{
    if (IsNumber(*text))
    {
        return *text + 212;
    }
    
    if (IsUppercaseAlpha(*text)) 
    {
        return *text + 361;
    }

    switch (*text)
    {
        case '!':
            return 75;
        case ',':
            return 76;
        case '?':
            return 278;
        case '.':
            return 327;
        default:
            return -1;
    }
}

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
NewMoby* DrawCapitalText(char* text, Vec3u32* position, int spacing, byte color)
{
    while (*text) 
    {
        if (*text != ' ')
        {
            NewMoby* moby = CreateTextMoby();

            // Default the type to a period if not found.
            short type = GetCapitalTextMobyType(text);
            moby->Type = type == -1 ? 327 : type;
            
            SetTextMobyProperties(moby, color, position);
        }

        position->X += spacing;
        text += 1;
    }

    return _MobyList;
}

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
NewMoby* DrawVaryingText(char *text, Vec3u32 *capitalPosition, Vec3u32 *lowercasePosition, int spacing, byte color)
{   
    bool useSpacing = true;
    
    for (; *text; text++) 
    {
        if (*text == ' ')
        {
            useSpacing = true;
            int adjustment = lowercasePosition->X * 3;
            adjustment = (adjustment < 0) ? adjustment + 3 : adjustment;
            capitalPosition->X += (adjustment / 4);
            continue;
        }

        NewMoby* moby = CreateTextMoby();
        SetTextMobyProperties(moby, color, capitalPosition);

        if ((*text == '!') || (*text == '?')) 
        {
            useSpacing = true;
        }

        if (!useSpacing) 
        {
            (moby->Position).Y += lowercasePosition->Y;
            (moby->Position).Z = lowercasePosition->Z;
        }

        int horizontalAdjustment = useSpacing ? spacing : lowercasePosition->X;
        capitalPosition->X += horizontalAdjustment;
        useSpacing = IsNumber(*text);

        moby->Type = GetVaryingTextMobyType(text);

        // If the type was not found, default to an apostraphe.
        if (moby->Type == -1)
        {
            moby->Type = 76;
            (moby->Position).Y -= (lowercasePosition->X * 2) / 3;
        }
    }

    return _MobyList;
}

/// @brief Copies generated moby structs to the end of a global list.
/// @details Appears to be something that is called often when working with text.
///     - Address: 0x80018880
///     - Hook: CopyHudToShaded.s
///     - Test: CopyHudToShadedTest.c
///     - Test Status: Passing
void CopyHudToShaded() 
{
    NewMoby **entry = &_HudMobyListHead;
    NewMoby **ref = &_DSM_U2;

    // Iterate to get to the end of the list.
    while (*ref != 0) 
    {
        ref = entry;
        entry = ref + 1;
    }

    // Append all of the new mobys
    while (_MobyList != _HudMobyQueuePtr)
    {
        *ref = _MobyList;
        _MobyList += 1;
        ref += 1;
    }

    // Make sure to set the end of the list to zero for iteration to work on the next call to this function. 
    *ref = 0;
}

/// @brief Draws the demo text on the screen.
/// @note
///     - Address: 0x80018908
///     - Hook: DrawDemoText.s
///     - Test: DrawDemoTextTest.c
///     - Test Status: Passing
void DrawDemoText()
{
    Vec3u32 capitalTextInfo = { .X = 199, .Y = 200, .Z = 4352 };
    Vec3u32 lowercaseTextInfo = { .X = 16, .Y = 1, .Z = 5120 };

    const byte spacing = 18;
    const byte color = 2;
    int sinArrayIndex = 0;

    // Keep a pointer to the head of the list before drawing text to be used for applying rotation.
    NewMoby *mobyPtr = _MobyList;
    DrawVaryingText("DEMO MODE", &capitalTextInfo, &lowercaseTextInfo, spacing, color);
    mobyPtr--;

    while (_MobyList <= mobyPtr)
    {
            mobyPtr->Rotation.Z = (byte)(_SinArray[(_LevelTimerFPS * 4 + sinArrayIndex & 0xFFU) + 64] / 128);
            sinArrayIndex += 12;
            mobyPtr--;
    }

    CopyHudToShaded();
}

/// @brief Draws a blinking text arrow facing left or right.
/// @note
///     - Address: 0x80018534
///     - Hook: DrawTextArrow.s
///     - Test: DrawTextArrowTest.c
///     - Test Status: Passing
/// @param position Position to draw on the screen.
/// @param timer Timer that counts every frame to be used for the pre-determined 16 frame intervals.
/// @param direction Determines if the arrow points left or right.
void DrawTextArrow(Vec3u32* position, uint timer, DrawTextArrowDirection direction)
{
    // Alternate from being visible, and not every 16 frames
    if ((timer & 31) >= 16)
    {
        return;
    }

    NewMoby* moby = CreateTextMoby();
    
    // Set Moby Type to the ascii number 1 for an arrow (clever)
    moby->Type = NUMERIC_ONE;
    CopyVector3D(&moby->Position, position);

    if (direction < POINT_UP) 
    {
        moby->Rotation.X = 64;
        moby->Rotation.Z = (byte)(direction * 128);
    }

    moby->Color = 11;
    moby->U13 = 0x7F;
    moby->U3 = 0xFF;
}

/// @brief Draws text that appears when rescuing a dragon.
/// @note
///     - Address: 0x80018728
///     - Hook: DrawDragonRescuedText.s
///     - Test: DrawDragonRescuedTextTest.c
///     - Test Status: Passing
void DrawDragonRescuedText()
{
    char* text =  _DragonNames[(*_RescuedDragonMoby)->DragonNameIndex];
    int textLength = GetStringLength(text);
    int horizontalPos = (textLength - 1) * -13 + 176;

    NewMoby* moby = _MobyList;

    Vec3u32 position = { .X = horizontalPos, .Y = 200, .Z = 4096 };
    DrawCapitalText("RESCUED", &position, 20, 2);

    position.X = horizontalPos + 160;
    position.Y = 200;
    position.Z = 3072;
    DrawCapitalText(text, &position, 26, 2);

    moby--;
    int loop = 0;

    while (_MobyList <= moby)
    {
        (moby->Rotation).Z = (byte)(_SinArray[(_DragonRescued_U0 * 2 + loop & 0xFFU) + 64] / 128);
        moby--;
        loop += 12;
    }
}
