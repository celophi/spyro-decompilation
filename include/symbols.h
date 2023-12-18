#pragma once

#include <PSYQ/LIBCD.h>
#include "shapes.h"

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

/// @brief 800774b4
extern int _CdReadSectors_U0;

/// @brief 800776c4
extern int _CdReadSectors_U1;

/// @brief 800776c8
extern int _CdReadSectors_U2;

extern byte _GlimmerArray;

/// @brief Pointer to linked list of primitives to be drawn every frame.
/// @note Address: 0x8007581c
extern PrimitiveList* _PrimitiveList;