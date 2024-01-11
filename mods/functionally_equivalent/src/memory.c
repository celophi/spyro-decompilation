#include <common.h>

/// @brief Initializes memory to a value
/// @param destination Address of memory to be written
/// @param value Value to write
/// @param size Number of bytes to write
/// @note Without specifying another library, GCC will want to use this for actual 'memset' functionality. (Not just where referenced in modding)
void memset(void* destination, byte value, uint size) 
{
    for (uint i = 0; i < size; i++) 
    {
        ((byte*)(destination))[i] = value;
    }
}

/// @brief Copies memory from the source to the destination.
/// @param destination Destination to receive copied bytes.
/// @param source Source to supply bytes to copy.
/// @param size Number of bytes to copy.
/// @note Without specifying another library, GCC will want to use this for actual 'memcpy' functionality. (Not just where referenced in modding)
void memcpy(void *destination, void *source, uint size) 
{
    for (uint i = 0; i < size; i++) 
    {
        ((byte*)(destination))[i] = ((byte*)(source))[i];
    }
}

/// @brief Calculates the length of a string.
/// @note
///     - Address: 0x8006276c
///     - Hook: GetStringLength.s
///     - Test: GetStringLengthTest.c
///     - Test Status: In Progress (Results in CPU stalling and pointing to an invalid address.)
/// @param text String to get the length of.
/// @return Length of the string.
int GetStringLength(char* text)
{
    char* cursor = text;

    while (*cursor)
    {
        cursor++;
    }

    return cursor - text;
}