#include <common.h>
#include <custom_types.h>
#include <moby.h>
#include <shapes.h>
#include <memory.h>
#include <symbols.h>
#include <PSYQ/LIBGPU.h>
#include <PSYQ/LIBGTE.h>
#include <vector.h>
#include <draw_stuff.h>


/// @brief Defines flags that represent whether a vector is outside of the visibility of the camera.
enum OffscreenStatus
{
    OFFSCREEN_BOTTOM = 1,
    OFFSCREEN_TOP = 2,
    OFFSCREEN_LEFT = 4,
    OFFSCREEN_RIGHT = 8,
};
typedef byte OffscreenStatus;
_Static_assert(sizeof(OffscreenStatus) == 1);

/// @brief Represents a 2D vector that is upscaled in order to hold metadata about its visibiility.
/// @details The flags member requires 5 bits of space.
typedef union 
{
    uint vector;
    OffscreenStatus flags;
} BoundedVertex;
_Static_assert(sizeof(BoundedVertex) == 4);

/// @brief Not entirely sure on this, but I think it is some kind of coordinate information that is used to determine if a model should be ignored.
/// @details The XYZ values are used to perform RTPS and get a Z value that is compared to the cutoff member. The result of which determines if model should be processed.
typedef struct
{
    int XY;
    short Cutoff;
    short Z;
} ClipCalculation;
_Static_assert(sizeof(ClipCalculation) == 8);

/// @brief Vector structure that gets transformed for a perspective.
typedef struct
{
    uint Y : 10;
    uint X : 11;
    uint Z : 11;
} PackedVertex;
_Static_assert(sizeof(PackedVertex) == 4);

/// @brief Packed vertex count.
typedef struct
{
    uint VertexIndexEnd : 13;
    uint Unused : 1;
    uint VertexIndexStart : 18;
} PackedCount;
_Static_assert(sizeof(PackedCount) == 4);

/// @brief This is the header or start of an entire model for drawing sky primitives.
/// @note Following this structure immediately adjacent in memory are a VertexIndex array, ColorIndex array, and then a PolyMeta array.
typedef struct
{
    ClipCalculation ClipCalculation;
    short GX;
    short GY;
    short VertexCount;
    short GZ;
    PackedCount PC;
    int U6;
} SkyboxModel;
_Static_assert(sizeof(SkyboxModel) == 24);

/// @brief Represents a structure that indexes where primitive RGB colors should be located.
/// @details These are byte offsets into the WAD to find colors. They come immediately after vertex information.
typedef struct
{
    uint Color2 : 10;
    uint Color1 : 10;
    uint Color0 : 12;
} ColorIndex;
_Static_assert(sizeof(ColorIndex) == 4);

/// @brief Indexes that show where to find vertex information for primitives.
/// @details These are byte offsets into the WAD to find vertices. They come immediately after SkyboxModel data.
typedef struct
{
    uint XY2 : 10;
    uint XY1 : 10;
    uint XY0 : 12;
} VertexIndex;
_Static_assert(sizeof(VertexIndex) == 4);

/// @brief Describes a structure that contains offsets to vertex and color information for building polygon primitives.
/// @details These are structures in the WAD that come after color index information.
typedef struct
{
    VertexIndex PU;
    ColorIndex ColorIndex;
} PolyMeta;
_Static_assert(sizeof(PolyMeta) == 8);

/// @brief RGB structure that also includes padding.
typedef struct
{
    byte Red;
    byte Green;
    byte Blue;
    byte Padding;
} RGBu8P;
_Static_assert(sizeof(RGBu8P) == 4);

/// @brief Sky models that are loaded from the WAD.
/// @note Address: 0x80078a44
extern SkyboxModel** _SkyboxWadModels;

/// @brief Pointers that can be used to reference skybox WAD models directly instead of starting at the beginning of the WAD section.
/// @note Address: 0x80078a4c
extern byte** _SkyboxWadModelPointers;

/// @brief Number of sky models that exist within a loaded section of the WAD.
/// @note Address: 0x80077780
extern int _SkyboxWadModelCount;

/// @brief Sky models that have been selected to draw primitives from.
/// @details Models that are in view of the camera that we can draw primitives from.
/// @note Address: 0x8006fcf4
extern SkyboxModel* _SkyboxModelsToRender;

/// @brief Scratchpad start in RAM used for storing skybox vectors.
/// @note Address: 0x1f800000
extern byte _ScratchpadDrawSkybox;


/// @brief TESTS PASSING 32/32 frames
/// @param  
void DrawFadeInSkybox(void)

{
    PrimitiveLinkedList *pPVar1;
    uint uVar2;
    int depth;
    uint uVar3;
    uint iVar4;
    P_TAG *pPVar4;
    uint uVar5;
    int *piVar6;
    uint uVar7;
    int uVar9;
    uint uVar8;
    int *cursor;
    uint uVar10;
    uint *puVar11;
    int pSVar18;
    int iVar12;
    int iVar13;
    uint *puVar14;
    uint *puVar15;
    int iVar16;
    byte *pbVar17;
    uint uVar18;
    int *pDVar19;
    P_TAG *pPVar19;
    int *pDVar23;
    
                    /* Store the state of all registers */
                    // Store registers. (This doesn't seem to have an effect on the game working, but it's needed for testing.)
    register int s0 asm("$16");
    register int s1 asm("$17");
    register int s2 asm("$18");
    register int s3 asm("$19");
    register int s4 asm("$20");
    register int s5 asm("$21");
    register int s6 asm("$22");
    register int s7 asm("$23");
    register int gp asm("$28");
    register int sp asm("$29");
    register int fp asm("$30");
    register int ra asm("$31");

    _RegisterStorage.S0 = s0;
    _RegisterStorage.S1 = s1;
    _RegisterStorage.S2 = s2;
    _RegisterStorage.S3 = s3;
    _RegisterStorage.S4 = s4;
    _RegisterStorage.S5 = s5;
    _RegisterStorage.S6 = s6;
    _RegisterStorage.S7 = s7;
    _RegisterStorage.SP = sp;
    _RegisterStorage.FP = fp;
    _RegisterStorage.RA = ra;
    _RegisterStorage.GP = gp;

                    /* Load camera and translation vector */
    gte_ldR11R12(_CameraA.R11R12);
    gte_ldR13R21(_CameraA.R13R21);
    gte_ldR22R23(_CameraA.R22R23);
    gte_ldR31R32(_CameraA.R31R32);
    gte_ldR33(_CameraA.R33);
    gte_ldtr(0,0,0);

                    /* Create variables to track the models to render, the start and end */
    SkyboxModel** modelsToRender = &_SkyboxModelsToRender;
    SkyboxModel** wadModelEnd = _SkyboxWadModels + _SkyboxWadModelCount;
    SkyboxModel** wadModelStart = _SkyboxWadModels;

                    /* Calculate the clipping and determine if the primitive needs to be drawn or not */
    while (cursor = (int *)*wadModelStart, wadModelStart != wadModelEnd) {
        wadModelStart = wadModelStart + 1;
        gte_ldVXY0(*cursor);
        gte_ldVZ0(cursor[1] >> 0x10);
        gte_rtps_b();
        gte_stSZ3(depth);
        gte_stMAC3(depth);

        if (0 < depth - (short)cursor[1]) {
            *modelsToRender = (SkyboxModel *)cursor;
            modelsToRender = modelsToRender + 1;
        }
    }

    *modelsToRender = (SkyboxModel *)0x0;
    gte_ldR11R12(_CameraB.R11R12);
    gte_ldR13R21(_CameraB.R13R21);
    gte_ldR22R23(_CameraB.R22R23);
    gte_ldR31R32(_CameraB.R31R32);
    gte_ldR33(_CameraB.R33);
    wadModelStart = &_SkyboxModelsToRender;
    pPVar19 = (P_TAG *)((int)_PrimitiveList + 4);
    uVar18 = 0;
    depth = _DrawSkyboxU4 + -0x400;
    pDVar23 = (int *)pPVar19;

    do 
    {
        do 
        {
            pSVar18 = (int)*wadModelStart;
            wadModelStart = wadModelStart + 1;
            pDVar19 = (int *)_PrimitiveList;

            if (pSVar18 == 0) 
            {
DrawFadeInSkybox_A:
                pPVar1 = _PrimitiveStagingStart;
                _PrimitiveList = pDVar23;

                if ((P_TAG *)pDVar23 != pPVar19)
                {
                    pPVar4 = _PrimitiveStagingStart[0x7ff].Tail;
                    *pDVar19 = uVar18 ^ 0x80000000;
                    pPVar1[0x7ff].Tail = (P_TAG *)pDVar19;

                    if (pPVar4 == (P_TAG *)0x0) 
                    {
                        pPVar1[0x7ff].Head = pPVar19;
                    }
                    else 
                    {
                        pPVar4->addr = (u_long)pPVar19;
                    }
                }

                return;
            }

            iVar16 = *(int *)(pSVar18 + 8) >> 0x10;
            iVar13 = (int)(short)*(int *)(pSVar18 + 8);
            iVar12 = (int)*(uint *)(pSVar18 + 0xc) >> 0x10;
            puVar14 = (uint *)(pSVar18 + 0x18) + (*(uint *)(pSVar18 + 0xc) & 0xffff);
            uVar8 = *(uint *)(pSVar18 + 0x10);
            pbVar17 = &_ScratchpadDrawSkybox;
            uVar10 = 0xffffffff;
            piVar6 = &_DrawFadeInSkybox_U0;

            gte_ldBFC((_DrawFadeInSkybox_U1 >> 0x10 & 0xff) << 4);
            gte_ldRFC((_DrawFadeInSkybox_U1 & 0xff) << 4);
            gte_ldGFC((_DrawFadeInSkybox_U1 >> 8 & 0xff) << 4);
            gte_ldIR0(_DrawFadeInSkybox_U2);
            puVar11 = puVar14 + 1;
            gte_ldRGB(*puVar14);

            do 
            {
                gte_dpcs_b();
                uVar2 = *puVar11;
                gte_stRGB2(uVar9);
                puVar11 = puVar11 + 1;
                *piVar6 = uVar9;
                gte_ldRGB(uVar2);
                piVar6 = piVar6 + 1;
            } while ((uint *)((int)puVar14 + (uVar8 >> 0xe) + 4) != puVar11);

            uVar2 = *(uint *)(pSVar18 + 0x18);
            gte_ldVZ0((uVar2 >> 0x15) + iVar12);
            gte_ldVXY0((iVar13 - (uVar2 >> 10 & 0x7ff)) + (iVar16 - (uVar2 & 0x3ff)) * 0x10000);
            uVar2 = *(uint *)(pSVar18 + 0x1c);
            puVar11 = (uint *)(pSVar18 + 0x20);

            do 
            {
                gte_rtps_b();
                uVar3 = uVar2 >> 0x15;
                uVar5 = uVar2 >> 10;
                uVar7 = uVar2 & 0x3ff;
                uVar2 = *puVar11;
                gte_ldVZ0(uVar3 + iVar12);
                gte_stSXY2(iVar4);
                gte_ldVXY0((iVar13 - (uVar5 & 0x7ff)) + (iVar16 - uVar7) * 0x10000);
                uVar3 = iVar4 * 0x20;

                if ((int)(iVar4 - 0x10000) < 1) 
                {
                    uVar3 = uVar3 + 1;
                }
                if (-1 < (int)(iVar4 - 0x1000000)) 
                {
                    uVar3 = uVar3 + 2;
                }
                if ((int)(iVar4 * 0x10000) < 1) 
                {
                    uVar3 = uVar3 + 4;
                }
                puVar11 = puVar11 + 1;
                if (-1 < (int)(iVar4 * 0x10000 + -0x2000000)) 
                {
                    uVar3 = uVar3 + 8;
                }

                uVar10 = uVar10 & uVar3;
                *(uint *)pbVar17 = uVar3;
                pbVar17 = (byte *)((int)pbVar17 + 4);

            } while (puVar11 != puVar14 + 2);
        } while ((uVar10 & 0xf) != 0);

        puVar14 = (uint *)((uVar8 >> 0xe) + (int)puVar14);
        puVar11 = puVar14;

        while( true ) 
        {
            uVar10 = *puVar11;
            puVar15 = puVar11 + 2;

            if (puVar11 == puVar14 + (uVar8 & 0x1fff) * 2)
            {
                break;
            }

            if (depth - (int)pDVar23 < 1) 
            {
                _DrawSkyboxU3 = 1;
                pDVar19 = (int *)_PrimitiveList;
                goto DrawFadeInSkybox_A;
            }

            uVar2 = *(uint *)((uVar10 >> 0x14) + 0x1f800000);
            uVar3 = *(uint *)((uVar10 >> 10 & 0x3fc) + 0x1f800000);
            uVar10 = *(uint *)((uVar10 & 0x3fc) + 0x1f800000);
            uVar5 = puVar11[1];
            puVar11 = puVar15;

            if ((uVar2 & uVar3 & uVar10 & 0x1f) == 0) 
            {
                *(uint *)_PrimitiveList = uVar18 ^ (uint)pDVar23;
                iVar12 = (int)uVar2 >> 5;
                iVar13 = (int)uVar3 >> 5;
                iVar16 = (int)uVar10 >> 5;
                pDVar23[2] = iVar12;
                pDVar23[4] = iVar13;
                pDVar23[6] = iVar16;
                uVar10 = uVar5 >> 0x14;
                uVar18 = uVar5 >> 10 & 0x3fc;
                _PrimitiveList = pDVar23;

                if ((uVar10 == uVar18) && (uVar10 == (uVar5 & 0x3fc))) 
                {
                    uVar18 = 0x84000000;
                    pDVar23[1] = *(int *)((int)&_DrawFadeInSkybox_U0 + uVar10) + -0x10000000;
                    pDVar23[2] = iVar12;
                    pDVar23[3] = iVar13;
                    pDVar23[4] = iVar16;
                    pDVar23 = pDVar23 + 5;
                }
                else 
                {
                    iVar12 = *(int *)((int)&_DrawFadeInSkybox_U0 + uVar18);
                    iVar13 = *(int *)((int)&_DrawFadeInSkybox_U0 + (uVar5 & 0x3fc));
                    uVar18 = 0x86000000;
                    pDVar23[1] = *(int *)((int)&_DrawFadeInSkybox_U0 + uVar10);
                    pDVar23[3] = iVar12;
                    pDVar23[5] = iVar13;
                    pDVar23 = pDVar23 + 7;
                }
            }
        }
    } while( true );
}
