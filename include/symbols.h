#pragma once

#include <PSYQ/LIBCD.h>
#include <PSYQ_cd.h>
#include "shapes.h"
#include <hud.h>

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

/// @brief Set to '1' when Spyro is in a flying level; '0' otherwise.
extern int _IsInFlyingLevel;

/// @brief 0x800720f4
extern NewMoby* _DSM_U2;

/// @brief 0x800720f8
extern NewMoby* _DisplayHudRelated_U0;

/// @brief 0x80077fec
extern PauseMenuState _PauseMenuState;

/// @brief 0x80077fc8
extern int _HudLevelTreasure;

/// @brief 0x80077fd8
extern int _DisplayHudRelated_U4;

/// @brief 0x80077fe8
extern int _HudChestHeadSpinTimer;

/// @brief 0x8007850c
extern TextureRelatedUnk _HudUnk1;

/// @brief 0x8007840c
extern HudOvalVertex _HudEggVertices[12];

/// @brief 0x80077fd4
extern int _HudEggs;

/// @brief 0x80077fe0
extern int _HudLifeOrbs;

/// @brief 0x8006cbf8
extern ushort _SinArray[256];

/// @brief 0x8007846c
extern HudOvalVertex _HudLifeOrbVertices[20];

/// @brief 0x80075710
extern NewMoby* _MobyList;

/// @brief 0x800755b4
extern char _TreatureObtainedRatio;

/// @brief 0x80077fb4
extern int _HudChestTransitionTimer;

/**
 * @brief Pointer to a list of Mobys that seem to be related to text.
 * @note Address: 0x80075710
*/
extern NewMoby* _MobyList;

/**
 * @brief Animation transition state for the chest icon on the HUD.
 * @note Address: 0x80077fa8
*/
extern HudAnimation _HudChestState;

/**
 * @brief Animation transition state for dragon count on the HUD.
 * @note Address: 0x80077fa9
*/
extern HudAnimation _HudDragonState;

/**
 * @brief Animation transition state for life count on the HUD.
 * @note Address: 0x80077faa
*/
extern HudAnimation _HudLivesState;

/**
 * @brief Animation transition state for egg count on the HUD.
 * @note Address: 0x80077fab
*/
extern HudAnimation _HudEggsState;

/**
 * @brief Animation transition state for any keys found on the HUD.
 * @note Address: 0x80077fac
*/
extern HudAnimation _HudKeyState;

/**
 * @brief Unknown
 * @note Address: 0x80077fad
*/
extern byte _HudChestAnimationFrame;

/**
 * @brief Unknown
 * @note Address: 0x80077fae
*/
extern byte _HudDragonAnimationFrame;

/**
 * @brief Unknown
 * @note Address: 0x80077faf
*/
extern byte _HudLivesAnimationFrame;

/**
 * @brief Unknown
 * @note Address: 0x80077fb0
*/
extern byte _HudEggsAnimationFrame;

/**
 * @brief Unknown
 * @note Address: 0x80077fb1
*/
extern byte _HudKeyAnimationFrame;
