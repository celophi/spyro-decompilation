#pragma once

#include <PSYQ/LIBCD.h>
#include <PSYQ_cd.h>
#include "shapes.h"
#include <hud.h>

extern int _InitializeMemoryCards_u0;
extern int _InitializeMemoryCards_u1;

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
extern NewMoby* _HudMobyListHead;

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

/// @brief Timer that cylces according to FPS.
/// @note Address: 0x800758c8
extern int _LevelTimerFPS;

/// @brief Unknown. Seems like some pointer to mobys.
/// @note Address: 0x800756fc
extern NewMoby* _HudMobyQueuePtr;

/// @brief References names of dragons to be rescued.
/// @note Address: 0x8006f694
extern char* _DragonNames[80];

/// @brief Unknown.
/// @note Address: 0x80077080
extern int _DragonRescued_U0;

/// @brief Unknown.
/// @note Address: 0x800770c0
extern NewMoby** _RescuedDragonMoby;

/// @brief Unknown.
/// @note Address: 0x800758b0
extern int _DrawSkyboxU3;

/// @brief Unknown.
/// @note Address: 0x80075780
extern int _DrawSkyboxU4;

/// @brief Starting area in RAM for the PSX scratchpad.
/// @note Address: 0x1f800000
extern uint _ScratchpadStart;

/// @brief Storage area for MIPS registers
/// @note Address: 0x80077dd8
extern RegisterStorage _RegisterStorage;

/// @brief Storage area for the MIPS stack pointer.
/// @note Address: 0x80077dfc
extern int _RegisterStorage_SP;

/// @brief Storage area for the MIPS return address;
/// @note Address: 0x80077e04
extern int _RegisterStorage_RA;

/// @brief Unknown storage area.
/// @note Address: 0x8006fcf4
extern SkyVertex* _SkyVertexStorage;

/// @brief WAD archive 4, Section 2 table 2 start
/// @note Address: 0x80078a44
extern SkyVertex** _WA4S2_Table2Start;

/// @brief WAD archive 4, Section 2 table 2 count
/// @note Address: 0x80078a40
extern int _WA4S2_Table2Count;

/// @brief WAD archive 4, Section 2, Unknown 7
/// @note Address: 0x80078a4c
extern int _WA4S2_U7;

/// @brief WA4S2 Unknown.
/// @note Address: 0x800785c8
extern int _ReadWA4S2_Count;

/// @brief WA4S2 Unknown.
/// @note Address: 0x800785c0
extern int* _ReadWA4S2_U1_Addr;

/// @brief Unknown.
/// @note Address: 0x800785c4
extern int* _ReadWA4S2_EndU;

/// @brief Unknown.
/// @note Address: 0x800785ac
extern int _ReadWA4S2_counter;

/// @brief Unknown.
/// @note Address: 0x800785a8
extern int* _WA4S2_Table1;

/// @brief Unknown.
/// @note Address: 0x800785b4
extern int _ReadWA3S2_u5;

/// @brief Unknown.
/// @note Address: 0x800785b0
extern int _ReadWA3S2_u6;

/// @brief Unknown.
/// @note Address: 0x8007778c
extern int _ReadWA3S2_u7;

/// @brief Unknown.
/// @note Address: 0x80077788
extern int _ReadWA3S2_u8;

/// @brief Unknown.
/// @note Address: 0x800785bc
extern int _ReadWA3S2_u9;

/// @brief Unknown.
/// @note Address: 0x800785b8
extern int _ReadWA3S2_u10;

/// @brief Unknown.
/// @note Address: 0x800785d4
extern int* _ReadWA3S2_u11;

/// @brief Color used for the drawing environment by default. This appears to be a tan colored yellow often.
/// @note Address: 0x80077790
extern RGBu8 _WadDrawEnvRgb;

/// @brief Unknown.
/// @note Address: 0x80077784
extern int* _WA4S2_Table2;

/// @brief Unknown.
/// @note Address: 0x80077780
extern int _WA4S2_Table2Count;

/// @brief Unknown.
/// @note Address: 0x800758bc
extern int _ReadWA3S2_u14;

/// @brief Unknown.
/// @note Address: 0x80078640
extern int _ReadWA3S2_u15;

/// @brief Unknown.
/// @note Address: 0x80076278
extern int _ReadWA3S2_u16;

/// @brief Unknown.
/// @note Address: 0x80076234
extern int _ReadWA3S2_u17;