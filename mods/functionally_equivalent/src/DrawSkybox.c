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

typedef struct
{
    short X;
    short Y;
} SXY;

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
    PackedVertex PV;
    PackedVertex PV8;
} SkyboxU0;
_Static_assert(sizeof(SkyboxU0) == 32);

typedef struct
{
    uint Unused1 : 2;
    uint U1 : 8;
    uint Unused2 : 2;
    uint U5 : 8;
    uint U3 : 12;

} Insanity;
_Static_assert(sizeof(Insanity) == 4);

uint Confused(uint* sxy2)
{
    SXY* vec = (SXY*)sxy2;
    uint scratch = *sxy2 << 5;

    if (vec->Y <= 0)
    {
        scratch += 1;
    }

    if (vec->Y >= 256)
    {
        scratch += 2;
    }

    if (vec->X <= 0)
    {
        scratch += 4;
    }

    if (vec->X >= 512)
    {
        scratch += 8;
    }

    return scratch;
}

static int HandleInner(int* ds43, RotationMatrix *cameraB)
{
    P_TAG *ptagA;
    uint *ds21;
    uint vertexCount;
    PackedCount* packedCount;
    
    uint scratchEndFlags;
    
    *ds43 = 0;

    gte_ldR11R12(cameraB->R11R12);
    gte_ldR13R21(cameraB->R13R21);
    gte_ldR22R23(cameraB->R22R23);
    gte_ldR31R32(cameraB->R31R32);
    gte_ldR33(cameraB->R33);

    ds43 = &_UnknownStorageU0;
    P_TAG *ptagB = (P_TAG *)((int)_PrimitiveList + 4);
    uint ds37 = 0;
    int szCoords = _DrawSkyboxU4 - 0x400;
    int *ds41 = (int *)ptagB;

    while (true)
    {
        do 
        {
            SkyboxU0 *SBu0 = (SkyboxU0 *)*ds43;

            ds43 = (int *)((SkyboxU0 **)ds43 + 1);
            int tail = (int)_PrimitiveList;

            if (SBu0 == NULL) 
            {
                PrimitiveLinkedList* stageStart = _PrimitiveStagingStart;
                _PrimitiveList = ds41;

                if ((P_TAG *)ds41 != ptagB) 
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

            ds21 = (uint*)&SBu0->PV;

            vertexCount = SBu0->VertexCount;
            packedCount = &SBu0->PC;

            uint *scratch = &_ScratchpadStart;
            scratchEndFlags = 0xFFFFFFFF;

            // Iterate through all vertices and calculate RTPS.
            for (uint i = 0; i <= vertexCount; i++)
            {
                PackedVertex* pv = &SBu0->PV + i;

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

                uint scratchData = Confused(&badGuy);
                scratchEndFlags = scratchEndFlags & scratchData;

                // Save calculation to the scratchpad for later.
                *scratch++ = scratchData;
            }

        } while ((scratchEndFlags & 0xF) != 0);

        uint *ds31 = (uint *)((int)ds21 + packedCount->U1 + vertexCount * 4);
        uint *puVar1 = ds31;

        while( true ) 
        {
            uint ds02 = *puVar1;
            uint *ds32 = puVar1 + 2;

            if (puVar1 == ds31 + packedCount->U3 * 2)
            {
                break;
            }

            if (szCoords - (int)ds41 < 1) 
            {
                _DrawSkyboxU3 = 1;
                int tail = (int)_PrimitiveList;
                PrimitiveLinkedList* stageStart = _PrimitiveStagingStart;
                _PrimitiveList = ds41;

                if ((P_TAG *)ds41 != ptagB) 
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

            scratchEndFlags = *(uint *)((ds02 >> 20) + 0x1f800000);
            uint scratchData = *(uint *)((ds02 >> 10 & 0x3FC) + 0x1f800000);
            ds02 = *(uint *)((ds02 & 0x3fc) + 0x1f800000);
            uint ds09 = puVar1[1];
            puVar1 = ds32;

            Insanity* ins = (Insanity*)&ds09;
            uint test = ins->U1 << 2;
            uint U12 = ins->U3;
            uint U13 = ins->U5 << 2;

            if ((scratchEndFlags & scratchData & ds02 & 0x1F) == 0) 
            {
                *(uint *)_PrimitiveList = ds37 ^ (uint)ds41;
                int ds28 = (int)scratchEndFlags >> 5;
                int ds30 = (int)scratchData >> 5;
                int ds34 = (int)ds02 >> 5;
                ds41[2] = ds28;
                ds41[4] = ds30;
                ds41[6] = ds34;
                _PrimitiveList = ds41;

                if ((U12 == U13) && (U12 == test)) 
                {
                    ds37 = 0x84000000;
                    ds41[1] = *(int *)((int)ds21 + U12 + vertexCount * 4) + -0x10000000;
                    ds41[2] = ds28;
                    ds41[3] = ds30;
                    ds41[4] = ds34;
                    ds41 = ds41 + 5;
                }
                else 
                {
                    int ds16 = *(int *)((int)ds21 + U13 + vertexCount * 4);
                    int ds19 = *(int *)((int)ds21 + test + vertexCount * 4);
                    ds37 = 0x86000000;
                    ds41[1] = *(int *)((int)ds21 + U12 + vertexCount * 4);
                    ds41[3] = ds16;
                    ds41[5] = ds19;
                    ds41 = ds41 + 7;
                }
            }
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

    SkyVertex** storage = (SkyVertex**)&_UnknownStorageU0;
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
                int result = HandleInner((int*)storage, cameraB);
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