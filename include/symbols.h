#pragma once

#include <PSYQ/LIBCD.h>
#include <PSYQ_cd.h>
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
extern int _CdUnknownCommand;

/// @brief 800776c8
extern int _MusicFlags;

extern byte _AngleArray[68];

/// @brief Pointer to linked list of primitives to be drawn every frame.
/// @note Address: 0x8007581c
extern PrimitiveLinkedList2* _PrimitiveLinkedList;

/// @brief 800757b0
extern void* _PrimitiveList;

/// @brief 800770f4
extern int _CyclingTimer;

/// @brief 80074b84
extern short _SquareRootLookupTable[192];

/// @brief 0x80075f08
extern SpuCommonAttr _SpuCommonAttr;

/// @brief 0x80076230
extern int _VolumeChange;

/// @brief 0x800761d8
extern int _InitializeSoundU3;

/// @brief 0x80075748
extern int _InitializeSoundU4;

/// @brief 0x800776b8
extern int _MusicSectorNumberA;

/// @brief 0x80076224
extern int _InitializeSoundU2;