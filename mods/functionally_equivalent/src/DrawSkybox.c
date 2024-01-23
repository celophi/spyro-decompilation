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

typedef union 
{
    uint vector;
    byte flags;
} BoundedVertex;
_Static_assert(sizeof(BoundedVertex) == 4);


/// @brief Scratchpad start in RAM used for storing skybox vectors.
/// @note Address: 0x1f800000
extern BoundedVertex _ScratchpadDrawSkybox;

typedef struct
{
    uint Y : 10;
    uint X : 11;
    uint Z : 11;
} PackedVertex;
_Static_assert(sizeof(PackedVertex) == 4);

typedef struct
{
    uint U3 : 13;
    uint U2 : 1;
    uint U1 : 18;
} PackedCount;
_Static_assert(sizeof(PackedCount) == 4);

typedef struct
{
    PackedVertex PV1;
    int U2;
    short GX;
    short GY;
    short VertexCount;
    short GZ;
    PackedCount PC;
    int U6;
} SkyboxU0;
_Static_assert(sizeof(SkyboxU0) == 24);

typedef struct
{
    uint Unused1 : 2;
    uint U1 : 8;
    uint Unused2 : 2;
    uint U5 : 8;
    uint U3 : 12;

} PolygonColors;
_Static_assert(sizeof(PolygonColors) == 4);

typedef struct
{
    uint Unused1 : 2;
    uint U1 : 8;
    uint Unused2 : 2;
    uint U5 : 8;
    uint U3 : 12;

} PolyUnk;
_Static_assert(sizeof(PolyUnk) == 4);

typedef struct
{
    PolyUnk PU;
    PolygonColors Ins;
} Sky3;

/// @brief Converts a bounded vector to a normal 2D vector.
/// @details Restores a vector that was scaled up (in order to hold additional metadata) back to it's original value.
/// @param boundedVector Vector that contains metadata about its visibility.
/// @return Restored 2D vector.
static Vec2s16 ConvertToVector(BoundedVertex* boundedVector)
{
    int originalValue = (int)boundedVector->vector >> 5;
    return *(Vec2s16*)&originalValue;
}

/// @brief Converts a vector to a bounded vector with metadata about the offscreen visibility.
/// @param vector 2D vector that is scaled up by 32x in order to hold metadata about the offscreen coordinate positions.
/// @return Vector with out of bounds information.
BoundedVertex ConvertToBoundedVector(Vec2s16* vector)
{
    // Scale the vector by 32 in or to make 5 bits to hold metadata about which coordinates are outside the camera range.
    BoundedVertex boundedVector = { .vector = *(uint*)vector << 5 };

    if (vector->Y <= 0)
    {
        boundedVector.flags |= OFFSCREEN_BOTTOM;
    }

    if (vector->Y >= 256)
    {
        boundedVector.flags |= OFFSCREEN_TOP;
    }

    if (vector->X <= 0)
    {
        boundedVector.flags |= OFFSCREEN_LEFT;
    }

    if (vector->X >= 512)
    {
        boundedVector.flags |= OFFSCREEN_RIGHT;
    }

    return boundedVector;
}

/// @brief Restores the vector back to its original value without the flags.
/// @param value Shifted vector.
/// @return Vec2s16
static Vec2s16 UnboundVector(uint value)
{
    int shiftedValue = (int)value >> 5;
    return *(Vec2s16*)&shiftedValue;
}





static int HandleInner(RotationMatrix *cameraB)
{
    P_TAG *ptagA;
    uint *vertexEnd;

    uint vertexCount;
    PackedCount* packedCount;
    
    byte endFlags;

    gte_ldR11R12(cameraB->R11R12);
    gte_ldR13R21(cameraB->R13R21);
    gte_ldR22R23(cameraB->R22R23);
    gte_ldR31R32(cameraB->R31R32);
    gte_ldR33(cameraB->R33);

    SkyVertex** skyVertexStorage = &_SkyVertexStorage;
    P_TAG *ptagB = (P_TAG *)((int)_PrimitiveList + 4);
    uint ds37 = 0;
    int szCoords = _DrawSkyboxU4 - 0x400;
    int *polygon = (int *)ptagB;

    while (true)
    {
        do 
        {
            SkyboxU0 *SBu0 = (SkyboxU0 *)*skyVertexStorage;
            skyVertexStorage++;

            int tail = (int)_PrimitiveList;

            if (SBu0 == NULL) 
            {
                PrimitiveLinkedList* stageStart = _PrimitiveStagingStart;
                _PrimitiveList = polygon;

                if ((P_TAG *)polygon != ptagB) 
                {
                    ptagA = _PrimitiveStagingStart[0x7ff].Tail;
                    *(uint *)tail = ds37 ^ 0x80000000;
                    stageStart[0x7FF].Tail = (P_TAG *)tail;

                    if (ptagA == NULL) 
                    {
                        stageStart[0x7FF].Head = ptagB;
                    }
                    else 
                    {
                        ptagA->addr = (u_long)ptagB;
                    }
                }

                return 1;
            }

            vertexCount = SBu0->VertexCount;
            packedCount = &SBu0->PC;

            PackedVertex* packs = (PackedVertex*)(SBu0 + 1);
            vertexEnd = (uint*)(packs + vertexCount);

            uint *scratch = &_ScratchpadStart;
            endFlags = 0x0F;

            // Iterate through all vertices and calculate RTPS.
            for (uint i = 0; i <= vertexCount; i++)
            {
                PackedVertex* pv = &packs[i];

                // Calculate the X, Y, Z component from global values.
                uint xComp = SBu0->GX - pv->X;
                uint yComp = SBu0->GY - pv->Y;
                uint zComp = SBu0->GZ + pv->Z;

                gte_ldVZ0(zComp);
                gte_ldVXY0(xComp + yComp * 0x10000);

                // This is here to satisfy testing requirements.
                // There is an extra load of VXYZ0 which is not necessary in the original code and gets discarded.
                // To ignore testing restrictions, remove this condition and modify the loop such that `i < vertexCount` instead of `i <= vertexCount`.
                if (i == vertexCount)
                {
                    break;
                }

                // coordinate transformation and perspective transformation
                gte_rtps_b();

                uint badGuy = 0;
                gte_stSXY2(badGuy);

                BoundedVertex bv = ConvertToBoundedVector((Vec2s16*)&badGuy);
                 

                endFlags = endFlags & bv.flags;

                // Save calculation to the scratchpad for later.
                *scratch++ = bv.vector;
            }

        } while ((endFlags & 0xF) != 0);

        // Get the address of the end of all the vertices.
        Sky3* skyStart = (Sky3*)((int)vertexEnd + packedCount->U1);
        Sky3* skyCursor = skyStart;

        while(skyCursor != (Sky3*)(((uint*)skyStart) + packedCount->U3 * 2)) 
        {
            if (szCoords - (int)polygon < 1) 
            {
                _DrawSkyboxU3 = 1;
                int tail = (int)_PrimitiveList;
                PrimitiveLinkedList* stageStart = _PrimitiveStagingStart;
                _PrimitiveList = polygon;

                if ((P_TAG *)polygon != ptagB) 
                {
                    ptagA = _PrimitiveStagingStart[0x7ff].Tail;
                    *(uint *)tail = ds37 ^ 0x80000000;
                    stageStart[0x7ff].Tail = (P_TAG *)tail;

                    if (ptagA == NULL) 
                    {
                        stageStart[0x7ff].Head = ptagB;
                    }
                    else 
                    {
                        ptagA->addr = (u_long)ptagB;
                    }
                }

                return 1;
            }

            // vertex indexes?
            PolyUnk* pu = &skyCursor->PU;
            uint u1 = pu->U1 << 2;
            uint u3 = pu->U3;
            uint u5 = pu->U5 << 2;

            BoundedVertex* scaledXy0 = &_ScratchpadDrawSkybox + (u3 / sizeof(uint));
            BoundedVertex* scaledXy1 = &_ScratchpadDrawSkybox + (u5 / sizeof(uint));
            BoundedVertex* scaledXy2 = &_ScratchpadDrawSkybox + (u1 / sizeof(uint));

            PolygonColors* ins = &skyCursor->Ins;
            uint test = ins->U1 << 2;
            uint U12 = ins->U3;
            uint U13 = ins->U5 << 2;

            // only checks lower 5 bits, Should make triangle?
            if ((scaledXy0->flags & scaledXy1->flags & scaledXy2->flags & 0x1F) == 0) 
            {
                *(uint *)_PrimitiveList = ds37 ^ (uint)polygon;
                _PrimitiveList = polygon;

                Vec2s16 xy0 = ConvertToVector(scaledXy0);
                Vec2s16 xy1 = ConvertToVector(scaledXy1);
                Vec2s16 xy2 = ConvertToVector(scaledXy2);

                // These are assigned here instead of the conditional because otherwise, test fails with a difference in memory.
                // It's probably a mistake from the developers where in the case of POLY_F3, additional bytes are added to the end.
                POLY_G3* polyG3 = (POLY_G3*)polygon;

                polyG3->x0 = xy0.X;
                polyG3->y0 = xy0.Y;

                polyG3->x1 = xy1.X;
                polyG3->y1 = xy1.Y;
                
                polyG3->x2 = xy2.X;
                polyG3->y2 = xy2.Y;

                if ((U12 == U13) && (U12 == test)) 
                {
                    POLY_F3* polyF3 = (POLY_F3*)polygon;

                    ds37 = 0x84000000;
                    polygon[1] = *(int *)((int)vertexEnd + U12) + -0x10000000;
                    
                    polyF3->x0 = xy0.X;
                    polyF3->y0 = xy0.Y;

                    polyF3->x1 = xy1.X;
                    polyF3->y1 = xy1.Y;

                    polyF3->x2 = xy2.X;
                    polyF3->y2 = xy2.Y;

                    polygon = (int*)(polyF3 + 1);
                }
                else 
                {
                    ds37 = 0x86000000;
                    polygon[1] = *(int *)((int)vertexEnd + U12);
                    polygon[3] = *(int *)((int)vertexEnd + U13);
                    polygon[5] = *(int *)((int)vertexEnd + test);

                    polygon = (int*)(polyG3 + 1);
                }
            }

            skyCursor++;
        }
    }

    return 0;
}


void DrawSkybox(int option, RotationMatrix *cameraA, RotationMatrix *cameraB)
{   
    // Store registers (this is only needed for testing, but we can still do it)
    storeRegisters(&_RegisterStorage);

    // Load a camera rotation matrix and translation vector
    gte_ldR11R12(cameraA->R11R12);
    gte_ldR13R21(cameraA->R13R21);
    gte_ldR22R23(cameraA->R22R23);
    gte_ldR31R32(cameraA->R31R32);
    gte_ldR33(cameraA->R33);
    gte_ldtr(0, 0, 0);

    SkyVertex** storage = &_SkyVertexStorage;
    SkyVertex** tableStart = _WA4S2_Table2Start;

    byte* tableEnd;
    if (option < 0) //true
    {
        tableEnd = (byte*)(_WA4S2_Table2Start + _WA4S2_Table2Count);
    }
    else 
    {
        tableEnd = *(byte**)(_WA4S2_U7 + option * 4);
    }

    while (true) 
    {
        SkyVertex *vertex;

        if (option < 0) 
        {
            vertex = *tableStart; // first entry
            if (tableStart == (SkyVertex**)tableEnd) 
            {

DrawSkybox_A:
                *storage = 0;
                int result = HandleInner(cameraB);
                if (result == 1)
                {
                    return;
                }
            }
            tableStart++;
        }
        else 
        {
            if (*tableEnd == 0xFF)
            {
                goto DrawSkybox_A;
            }

            vertex = tableStart[*tableEnd];
            tableEnd++;
        }

        gte_ldVXY0(vertex->XY); //x = 0x03ec, y = 0x0072
        gte_ldVZ0(vertex->Z); // z = 0xff5b

        // coordinate transformation and perspective transformation
        gte_rtps_b();

        // store 3 screen coordinates to non-continuous addresses. Store screen z 0, 1, 2
        int result;
        gte_stMAC3(result); // 0x38d

        if (result - vertex->u0 > 0) // 0x01FF
        {
            *storage++ = vertex; // addr of the entry, place it unknown storage?
        }
    }
}