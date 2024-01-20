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

static int HandleInner(int* ds43, RotationMatrix *cameraB)
{
    P_TAG *ptagA;
    uint *ds21;
    uint ds24;
    uint ds13;
    byte *scratch;
    uint scratchEndFlags;
    uint ds02;
    
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

            int ds34 = SBu0->U3 >> 16;
            int ds30 = (int)(short)SBu0->U3;
            int ds28 = SBu0->U4 >> 0x10;
            ds21 = &SBu0->U7;
            ds24 = SBu0->U4 & 0xffff;
            ds13 = SBu0->U5;
            scratch = &_ScratchpadStart;
            scratchEndFlags = 0xffffffff;
            ds02 = *ds21;

            gte_ldVZ0((ds02 >> 0x15) + ds28);
            gte_ldVXY0((ds30 - (ds02 >> 10 & 0x7ff)) + (ds34 - (ds02 & 0x3ff)) * 0x10000);

            ds02 = SBu0->U8;
            SkyboxU0 *ds22 = SBu0 + 1;

            do 
            {
                // coordinate transformation and perspective transformation
                gte_rtps_b();

                uint other = ds02 >> 0x15;
                uint inner1 = ds02 >> 10;
                uint inner2 = ds02 & 0x3ff;
                ds02 = ds22->U1;

                gte_ldVZ0(other + ds28);

                int badGuy = 0;
                gte_stSXY2(badGuy);
                gte_ldVXY0((ds30 - (inner1 & 0x7ff)) + (ds34 - inner2) * 0x10000);
                
                uint scratchData = Confused(badGuy);


                ds22 = (SkyboxU0 *)&ds22->U2;
                scratchEndFlags = scratchEndFlags & scratchData;
                *(uint *)scratch = scratchData;
                scratch = (byte *)((int)scratch + 4);

            } while (ds22 != (SkyboxU0 *)(ds21 + ds24 + 2));

        } while ((scratchEndFlags & 0xF) != 0);


        uint *ds31 = (uint *)((int)ds21 + (ds13 >> 0xe) + ds24 * 4);
        uint *puVar1 = ds31;

        while( true ) 
        {
            ds02 = *puVar1;
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
                    ds41[1] = *(int *)((int)ds21 + ds02 + ds24 * 4) + -0x10000000;
                    ds41[2] = ds28;
                    ds41[3] = ds30;
                    ds41[4] = ds34;
                    ds41 = ds41 + 5;
                }
                else 
                {
                    int ds16 = *(int *)((int)ds21 + ds37 + ds24 * 4);
                    int ds19 = *(int *)((int)ds21 + (ds09 & 0x3fc) + ds24 * 4);
                    ds37 = 0x86000000;
                    ds41[1] = *(int *)((int)ds21 + ds02 + ds24 * 4);
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