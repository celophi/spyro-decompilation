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




uint Confused(int badGuy)
{

    /* compiler in decomp messes up here and creates the wrong kind of branches. */
                            /*
                            scratchData = badGuy * 32;



                            int r = badGuy * 0x10000;

                            if (r < 1) 
                            {
                            scratchData += 4;
                            }

                            if (r > 0x01FFFFFF)
                            {
                            scratchData += 8;
                            }

                            if (badGuy < 0x10001) 
                            {
                            scratchData += 1;
                            }

                            if (badGuy > 0x0FFFFFF) 
                            {
                            scratchData += 2;
                            }
                            */
  uint scratch = 0;
  doScratchCalc(badGuy, scratch);
  return scratch;
}

typedef struct
{
    uint Y : 10;
    uint X : 11;
    uint Z : 11;
} PackedVertex;
_Static_assert(sizeof(PackedVertex) == 4);

typedef struct
{
    PackedVertex PV1;
    int U2;
    int U3;
    short countA;
    short U4;
    int U5;
    int U6;
    PackedVertex PV;
    PackedVertex PV8;
} SkyboxU0;
_Static_assert(sizeof(SkyboxU0) == 32);

typedef struct
{
    short X;
    short Y;
    short u0;
    short Z;
} SkyVertex4;

typedef struct
{
    SkyVertex4 Vertices[4];
} Skybox4;

static int HandleInner(int* ds43, RotationMatrix *cameraB)
{
    P_TAG *ptagA;
    uint *ds21;
    uint vertexCount;
    uint ds13;
    byte *scratch;
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
    int szCoords = _DrawSkyboxU4 + -0x400;
    int *ds41 = (int *)ptagB;

    while (true)
    {
        do 
        {
            SkyboxU0 *SBu0 = (SkyboxU0 *)*ds43;
            // test new structure
            Skybox4* sb4 = (Skybox4*)*ds43;

            ds43 = (int *)((SkyboxU0 **)ds43 + 1);
            int tail = (int)_PrimitiveList;

            if (sb4 == NULL) 
            {
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

            short xPart = sb4->Vertices[1].X;
            short yPart = sb4->Vertices[1].Y;
            short zPart = sb4->Vertices[1].Z;

        
            ds21 = (uint*)&SBu0->PV;
            PackedVertex* start = &SBu0->PV;


            vertexCount = SBu0->countA;
            ds13 = SBu0->U5;
            scratch = &_ScratchpadStart;
            scratchEndFlags = 0xffffffff;

            for (uint i = 0; i <= vertexCount; i++)
            {
                PackedVertex* pv = &SBu0->PV + i;

                uint xComp = xPart - pv->X;
                uint yComp = yPart - pv->Y;
                uint zComp = zPart + pv->Z;

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

                int badGuy = 0;
                gte_stSXY2(badGuy);

                uint scratchData = Confused(badGuy);
                scratchEndFlags = scratchEndFlags & scratchData;
                *(uint *)scratch = scratchData;
                scratch = (byte *)((int)scratch + 4);
            }

        } while ((scratchEndFlags & 0xF) != 0);


        uint *ds31 = (uint *)((int)ds21 + (ds13 >> 0xe) + vertexCount * 4);
        uint *puVar1 = ds31;

        while( true ) 
        {
            uint ds02 = *puVar1;
            uint *ds32 = puVar1 + 2;

            if (puVar1 == ds31 + (ds13 & 0x1fff) * 2)
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

            scratchEndFlags = *(uint *)((ds02 >> 0x14) + 0x1f800000);
            uint scratchData = *(uint *)((ds02 >> 10 & 0x3fc) + 0x1f800000);
            ds02 = *(uint *)((ds02 & 0x3fc) + 0x1f800000);
            uint ds09 = puVar1[1];
            puVar1 = ds32;

            if ((scratchEndFlags & scratchData & ds02 & 0x1F) == 0) 
            {
                *(uint *)_PrimitiveList = ds37 ^ (uint)ds41;
                int ds28 = (int)scratchEndFlags >> 5;
                int ds30 = (int)scratchData >> 5;
                int ds34 = (int)ds02 >> 5;
                ds41[2] = ds28;
                ds41[4] = ds30;
                ds41[6] = ds34;
                ds02 = ds09 >> 0x14;
                ds37 = ds09 >> 10 & 0x3fc;
                _PrimitiveList = ds41;

                if ((ds02 == ds37) && (ds02 == (ds09 & 0x3fc))) 
                {
                    ds37 = 0x84000000;
                    ds41[1] = *(int *)((int)ds21 + ds02 + vertexCount * 4) + -0x10000000;
                    ds41[2] = ds28;
                    ds41[3] = ds30;
                    ds41[4] = ds34;
                    ds41 = ds41 + 5;
                }
                else 
                {
                    int ds16 = *(int *)((int)ds21 + ds37 + vertexCount * 4);
                    int ds19 = *(int *)((int)ds21 + (ds09 & 0x3fc) + vertexCount * 4);
                    ds37 = 0x86000000;
                    ds41[1] = *(int *)((int)ds21 + ds02 + vertexCount * 4);
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