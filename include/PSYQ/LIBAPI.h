#pragma once

/// @brief Initialize Memory Card BIOS.
/// @note Original Address: 0x80068958
/// @param val 
extern void InitCARD(long val);

/// @brief Start Memory Card BIOS.
/// @note Original Address: 0x800689b0
/// @param  
/// @return 
extern long StartCARD(void);

/// @brief Initialize Memory Card system.
/// @note Original Address: 0x80068494
/// @param  
extern void _bu_init(void);