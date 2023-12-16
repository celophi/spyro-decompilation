#pragma once

#include <PSYQ/LIBCD.h>

/// @brief This is the pause/inventory menu timer. It is only running in those menus. \n Address: 0x8007588c
extern int _LoadingTimer;

/// @brief 0x80076bb8
extern int _CdReadCallback_U0;

/// @brief 800756e0
extern int _CdReading;

/// @brief 80076b98
extern CdlLOC _CdlLOC;

/// @brief 80076b94
extern int _CdReadSectorsParameter;

/// @brief 80076b9c
extern u_long* _CdReadBuffer;