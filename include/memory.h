#pragma once

#include <custom_types.h>

/// @brief Initializes memory to a value
/// @param destination Address of memory to be written
/// @param value Value to write
/// @param size Number of bytes to write
void memset(void* destination, byte value, unsigned int size);

/// @brief Copies memory from the source to the destination.
/// @param destination Destination to receive copied bytes.
/// @param source Source to supply bytes to copy.
/// @param size Number of DWORDs to copy.
void memcpy(void *destination, void *source, int size);