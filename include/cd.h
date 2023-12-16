#pragma once

#include <common.h>

/// @brief Callback to execute after reading from the CD-ROM.
/// @param status Result of the CD read operation.
/// @note Original Address: 0x80016490
void CdReadCallbackFunc(char status);


byte CdStatus(void);
