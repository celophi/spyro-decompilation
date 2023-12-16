#include <common.h>

/// @brief Initializes memory to a value
/// @param destination Address of memory to be written
/// @param value Value to write
/// @param size Number of bytes to write
void Memset(void* destination, byte value, uint size) {
    for (uint i = 0; i < size; i++) {
        ((byte*)(destination))[i] = value;
    }
}

/// @brief Copies memory from the source to the destination.
/// @param destination Destination to receive copied bytes.
/// @param source Source to supply bytes to copy.
/// @param size Number of bytes to copy.
void Memcpy(void *destination, void *source, uint size) {
    for (uint i = 0; i < size; i++) {
        ((byte*)(destination))[i] = ((byte*)(source))[i];
    }
}