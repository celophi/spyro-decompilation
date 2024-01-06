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