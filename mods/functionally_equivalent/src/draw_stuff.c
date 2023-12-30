#include <common.h>
#include <custom_types.h>
#include <moby.h>
#include <shapes.h>
#include <memory.h>
#include <symbols.h>
#include <PSYQ/LIBGPU.h>

/// @brief Calculates the arctan of a vector to get the estimated angle. The returned value is a number where 64 = 90 degrees, and 256 = 360 degrees.
/// @param x Position of the X component of a vertex.
/// @param y Position of the Y component of a vertex.
/// @return Angle based on a number aligned with 64 = 90 degrees.
/// @note Original Address: 0x800169ac
int GetAngle(int x, int y)
{
    // Get the absolute values of both X and Y.
    int absX = x < 0 ? -x : x;
    int absY = y < 0 ? -y : y;

    // If ABS(X) > ABS(Y) then swap the index and divisor.
    int numerator = absX > absY ? absY : absX;
    int divisor = absX > absY ? absX : absY;

    // Make sure divisor is not zero to avoid division by zero.
    divisor = divisor == 0 ? 1 : divisor;

    int idx = (numerator << 6) / divisor;
    uint result = (uint) _AngleArray[idx];

    // Determine an offset multiplier based on the sign value of both X and Y.
    int multiplier = (x < 0)
        ? ((y < 0) ? 2 : 1)
        : ((y < 0) ? 3 : 0);

    // Determine the result from the glimmer array needs to be inverted.
    bool invert = absX < absY;

    // Handle a specific case when only one of either X and Y values are negative.
    if ((x < 0 && y >= 0) || (x >= 0 && y < 0))
    {
        invert = absX >=  absY;
    }

    // When inverted, the multiplier also gets adjusted forward.
    if (invert) 
    {
        multiplier++;
        result *= -1;
    }

    int offset = multiplier * 64;
    return offset + result;
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

/// @brief Adds a primitive to a linked list.
/// @param primitive Primitive to draw
/// @note Original Address: 0x800168dc
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

/// @brief Draws a blinking text arrow facing left or right.
/// @param position Position to draw on the screen.
/// @param timer Timer that counts every frame to be used for the pre-determined 16 frame intervals.
/// @param leftOrRight Determines if the arrow points left or right.
void DrawTextArrow(Vec3u32 *position, uint timer, int leftOrRight)
{
    Vec3u32 *destination;

    // Alternate from being visible, and not every 16 frames
    if ((timer & 31) < 16) 
    {
        _ptr_hudMobys = _ptr_hudMobys - 1;

        memset(_ptr_hudMobys, 0, sizeof(Moby));

        destination = &_ptr_hudMobys->position;

        // Set Moby Type to the ascii number 1 for an arrow (clever)
        _ptr_hudMobys->type = 261;

        CopyVector3D(destination, position);

        if (leftOrRight < 2) 
        {
            _ptr_hudMobys->rotation.x = 64;
            _ptr_hudMobys->rotation.z = (char)(leftOrRight << 7);
        }

        _ptr_hudMobys->color = 11;
        _ptr_hudMobys->requiredHUD1 = 0x7F;
        _ptr_hudMobys->requiredHUD2 = 0xFF;
    }
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