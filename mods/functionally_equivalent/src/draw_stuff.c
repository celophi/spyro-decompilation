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

/**
 * @brief Adds a primitive to a linked list.
 * @details Seems to be a staging area that gets turned into an ordering table later.
 * @note
 *      - Address: 0x800168dc
 *      - Hook: AddPrimitiveToList.s
 *      - Test: AddPrimitiveToListTest.c
 * @param primitive Reference to a primitive to stage.
*/
void AddPrimitiveToList(P_TAG *primitive)
{
    PrimitiveLinkedList2 *list = _PrimitiveLinkedList;
    P_TAG *head = _PrimitiveLinkedList->Head;
    _PrimitiveLinkedList->Head = primitive;

    if (head != 0) 
    {
        head->addr = (u_long)primitive;
        return;
    }

    list->Tail = primitive;
}

/// @brief Calculates the 8-bit difference between the supplied value and a timer, then clamps between 0 and 127.
/// @param value Supplied value.
/// @param timer In most cases, the value of a cyclical timer in the game.
/// @return Calculated value.
int GetClampedDifference(int value, int timer)
{
    // Keep only the lower 8 bits.
    byte result = (byte)(value - timer);

    // Clamp the result between 0 and 127.
    return (result >= 128) ? (256 - result) : result;
}

/// @brief Sets the color of a vertex calculating a difference in contrast based on the position.
/// @param rgb Reference to a pixel to color
/// @param x X coordinate
/// @param y Y coordinate
void SetVertexColor(RGBu8* rgb, ushort x, ushort y)
{
    int offset = GetAngle(x - 256, y - 120);
    offset = GetClampedDifference(offset, _CyclingTimer);

    byte calculated = -(byte)offset + 224;
    rgb->R = calculated;
    rgb->G = calculated;
    rgb->B = -(byte)offset + 128;
}

/// @brief Draws a golden line on the screen. This can be seen in menus, and some text boxes.
/// @param X1 Left X coordinate.
/// @param Y1 Left Y coordinate.
/// @param X2 Right X coordinate.
/// @param Y2 Right Y coordinate.
void DrawLine(ushort X1,ushort Y1,ushort X2,ushort Y2)
{   
    LINE_G2* primitive = (LINE_G2*)_PrimitiveList;

    // Create a LINE_G2. Note, should create a real method for this.
    primitive->Tag.len = 4;
    primitive->Tag.addr = 0;
    primitive->Tag.code = LINE_OPAQUE_SHADED;

    primitive->x0 = X1;
    primitive->y0 = Y1;
    primitive->x1 = X2;
    primitive->y1 = Y2;

    SetVertexColor((RGBu8*)&primitive->Tag.r0, X1, Y1);
    SetVertexColor((RGBu8*)&primitive->r1, X2, Y2);

    AddPrimitiveToList((P_TAG *)primitive);
    _PrimitiveList = primitive + 1;
}

/// @brief Draws a black textbox with a gold border. You can specify the bounds
/// @param xBound1 Top-left X coordinate.
/// @param xBound2 Bottom-right X coordinate.
/// @param yBound1 Top-Left Y coordinate.
/// @param yBound2 Bottom-left Y coordinate.
void DrawTextbox(int xBound1,int xBound2,int yBound1,int yBound2)
{
    Poly4FPadded* ptr_prim = (Poly4FPadded*)_PrimitiveList;
    DR_MODE* mode = (DR_MODE*)_PrimitiveList;

    SetDrawMode(mode, 1, 0, 64, 0);
    AddPrimitiveToList((P_TAG*)mode);

    POLY_F4* polyF4 = (POLY_F4*)(mode + 1);

    // setPolyF4 + setSemiTrans
    polyF4->Tag.len = 5;
    polyF4->Tag.addr = 0;
    polyF4->Tag.code = POLY4F_TRANSPARENT;

    polyF4->x0 = xBound1;
    polyF4->x1 = xBound2;
    polyF4->x2 = xBound1;
    polyF4->x3 = xBound2;
    polyF4->y0 = yBound1;
    polyF4->y1 = yBound1;
    polyF4->y2 = yBound2;
    polyF4->y3 = yBound2;

    polyF4->Tag.r0 = 112;
    polyF4->Tag.g0 = 112;
    polyF4->Tag.b0 = 112;
    
    AddPrimitiveToList((P_TAG*)polyF4);
    _PrimitiveList = polyF4 + 1;

    DrawLine(xBound1, yBound1, xBound2, yBound1);
    DrawLine(xBound2, yBound1, xBound2, yBound2);
    DrawLine(xBound2, yBound2, xBound1, yBound2);
    DrawLine(xBound1, yBound2, xBound1, yBound1);
}


/// @brief Creates an ordering table by linking all primitives within a section of memory.
/// @param count Number of links(?) double pointers that exist.
/// @note Original Address: 0x80016784 
/// @return ordering table
P_TAG * CreateOrderingTable(int count)
{
    // Starting from the end of the memory block, iterate backwards until finding the first non-null entry.
    PrimitiveLinkedList* cursor = _PrimitiveStagingStart + count;

    // Find the first entry, starting from the end, that is not null.
    P_TAG* entry = NULL;
    while (!entry && cursor != _PrimitiveStagingStart)
    {
        cursor--;
        entry = cursor->Tail;
    }

    // If the entire OT was iterated over without finding a non-null entry, reset.
    if (cursor == _PrimitiveStagingStart)
    {
        // Make a copy of the head and tail of the list.
        P_TAG* currentHead = _PrimitiveLinkedList->Head;
        P_TAG* currentTail = _PrimitiveLinkedList->Tail;

        // Reset the head.
        _PrimitiveLinkedList->Head = NULL;

        // If the current head isn't reset, then reset the tail, and the previous head (before reset) needs to point to the data section start.
        if (currentHead) 
        {
            _PrimitiveLinkedList->Tail = NULL;
            currentHead->addr = (u_long)&_DataSectionStart;
            return currentTail;
        }

        return (P_TAG *)&_DataSectionStart;
    }

    // The head of the first linked list is the start of the ordering table.
    P_TAG* orderingTable = cursor->Head;

    // Link all of the primitives to each other iteratively.
    while (cursor != _PrimitiveStagingStart)
    {
        cursor--;
        
        if (cursor->Head) 
        {
            // link the tail of the previous list to this newly found list's head.
            entry->addr = (u_long)cursor->Head;

            // The newly found list now becomes the list to be linked with on the next iteration.
            entry = cursor->Tail;
        }
    }

    // Reset the staging area.
    memset(_PrimitiveStagingStart, 0, count * sizeof(PrimitiveLinkedList));

    // If the OT->Tail is not reset
    if (_PrimitiveLinkedList->Tail) 
    {
        entry->addr = (u_long)_PrimitiveLinkedList->Tail;
        
        // Reset the OT.
        _PrimitiveLinkedList->Tail = NULL;
        _PrimitiveLinkedList->Head = NULL;
        _PrimitiveLinkedList->Head->addr = (u_long)&_DataSectionStart;
        return orderingTable;
    }

    entry->addr = (u_long)&_DataSectionStart;
    return orderingTable;
}

/// @brief Adds a primitive to the end of the staging area.
/// @param primitive Primitive to add to the tail.
/// @param offset Offset from the beginning of the staging area to find the current tail.
void StagePrimitive(P_TAG* primitive, int offset)
{
    // Calculate the end of the staging area.
    PrimitiveLinkedList* end = _PrimitiveStagingStart + offset;

    // Get a reference to the last primitive.
    P_TAG* tail = end->Tail;

    // Link the last primitive to the input argument.
    end->Tail = primitive;

    // If the old tail is not null, attach head using the address of the P_TAG.
    if (tail != NULL)
    {
        tail->addr = (u_long)primitive;
        return;
    }

    // Link the last primitive head to be the input argument.
    end->Head = primitive;
}

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


void DrawSkybox(int option, RotationMatrix *cameraA, RotationMatrix *cameraB)
{
    uint ds02;
    int szCoords;
    uint scratchData;
    P_TAG *ptagA;
    uint ds09;
    uint ds13;
    int ds14;
    int *ds15;
    int ds16;
    int ds17;
    uint scratchEndFlags;
    int ds19;
    int ds20;
    uint *ds21;
    SkyboxU0 *ds22;
    int ds23;
    uint ds24;
    int ds25;
    int ds27;
    int ds28;
    int ds29;
    int ds30;
    uint *ds31;
    uint *puVar1;
    uint *ds32;
    int ds33;
    int ds34;
    byte *scratch;
    uint ds37;
    P_TAG *ptagB;
    int *ds41;
    int *ds43;
    

    // Store registers (this is only needed for testing, but we can still do it)
    storeRegisters(&_RegisterStorage);

    // Load a camera rotation matrix and translation vector
    gte_ldR11R12(cameraA->R11R12);
    gte_ldR13R21(cameraA->R13R21);
    gte_ldR22R23(cameraA->R22R23);
    gte_ldR31R32(cameraA->R31R32);
    gte_ldR33(cameraA->R33);
    gte_ldtr(0, 0, 0);


    ds43 = &_UnknownStorageU0;
    ds14 = (int)&_UnknownStorageU0;
    int tableStart = _WA4S2_Table2Start;


    int tableEnd;
    if (option < 0) //true
    {
        tableEnd = _WA4S2_Table2Start + _WA4S2_Table2Count * 4;
        ds43 = (int *)ds14; // empty storage?
    }
    else 
    {
        tableEnd = *(int *)(_WA4S2_U7 + option * 4);
    }

    do 
    {
        if (option < 0) 
        {
            ds15 = *(int **)tableStart; // first entry
            if (tableStart == tableEnd) 
            {

DrawSkybox_A:
                *ds43 = 0;

                gte_ldR11R12(cameraB->R11R12);
                gte_ldR13R21(cameraB->R13R21);
                gte_ldR22R23(cameraB->R22R23);
                gte_ldR31R32(cameraB->R31R32);
                gte_ldR33(cameraB->R33);

                ds43 = &_UnknownStorageU0;
                ptagB = (P_TAG *)((int)_PrimitiveList + 4);
                ds37 = 0;
                szCoords = _DrawSkyboxU4 + -0x400;
                ds41 = (int *)ptagB;

                do 
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

                            return;
                        }

                        ds34 = SBu0->U3 >> 16;
                        ds30 = (int)(short)SBu0->U3;
                        ds28 = SBu0->U4 >> 0x10;
                        ds21 = &SBu0->U7;
                        ds24 = SBu0->U4 & 0xffff;
                        ds13 = SBu0->U5;
                        scratch = &_ScratchpadStart;
                        scratchEndFlags = 0xffffffff;
                        ds02 = *ds21;

                        gte_ldVZ0((ds02 >> 0x15) + ds28);
                        gte_ldVXY0((ds30 - (ds02 >> 10 & 0x7ff)) + (ds34 - (ds02 & 0x3ff)) * 0x10000);

                        ds02 = SBu0->U8;
                        ds22 = SBu0 + 1;

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
                            
                            scratchData = Confused(badGuy);


                            ds22 = (SkyboxU0 *)&ds22->U2;
                            scratchEndFlags = scratchEndFlags & scratchData;
                            *(uint *)scratch = scratchData;
                            scratch = (byte *)((int)scratch + 4);

                        } while (ds22 != (SkyboxU0 *)(ds21 + ds24 + 2));

                    } while ((scratchEndFlags & 0xf) != 0);


                    ds31 = (uint *)((int)ds21 + (ds13 >> 0xe) + ds24 * 4);
                    puVar1 = ds31;

                    while( true ) 
                    {
                        ds02 = *puVar1;
                        ds32 = puVar1 + 2;

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

                            return;
                        }

                        scratchEndFlags = *(uint *)((ds02 >> 0x14) + 0x1f800000);
                        scratchData = *(uint *)((ds02 >> 10 & 0x3fc) + 0x1f800000);
                        ds02 = *(uint *)((ds02 & 0x3fc) + 0x1f800000);
                        ds09 = puVar1[1];
                        puVar1 = ds32;

                        if ((scratchEndFlags & scratchData & ds02 & 0x1f) == 0) 
                        {
                            *(uint *)_PrimitiveList = ds37 ^ (uint)ds41;
                            ds28 = (int)scratchEndFlags >> 5;
                            ds30 = (int)scratchData >> 5;
                            ds34 = (int)ds02 >> 5;
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
                                ds16 = *(int *)((int)ds21 + ds37 + ds24 * 4);
                                ds19 = *(int *)((int)ds21 + (ds09 & 0x3fc) + ds24 * 4);
                                ds37 = 0x86000000;
                                ds41[1] = *(int *)((int)ds21 + ds02 + ds24 * 4);
                                ds41[3] = ds16;
                                ds41[5] = ds19;
                                ds41 = ds41 + 7;
                            }
                        }
                    }
                } while( true );
            }
            tableStart = tableStart + 4;
        }
        else 
        {
            if (*(byte *)tableEnd == 0xff)
            {
                goto DrawSkybox_A;
            }

            ds15 = *(int **)(tableStart + (uint)*(byte *)tableEnd * 4);
            tableEnd = tableEnd + 1;
        }

        gte_ldVXY0(*ds15); //x = 0x03ec, y = 0x0072
        gte_ldVZ0(ds15[1] >> 16); // z = 0xff5b

        // coordinate transformation and perspective transformation
        gte_rtps_b();

        // store 3 screen coordinates to non-continuous addresses. Store screen z 0, 1, 2
        gte_stSZ3(szCoords);
        gte_stMAC3(szCoords); // 0x38d

        if (0 < szCoords - (short)ds15[1]) // 0x01FF
        {
            *ds43 = (int)ds15; // addr of the entry
            ds43 = (int *)((int **)ds43 + 1); // place it unknown storage?
        }

    } while( true );
}