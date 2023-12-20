#include <common.h>
#include <custom_types.h>
#include <moby.h>
#include <shapes.h>
#include <memory.h>
#include <symbols.h>

/// @brief Calculates an offset for applying a shine/glimmer to a drawn line.
/// @param x Position of the X component of a vertex.
/// @param y Position of the Y component of a vertex.
/// @return Offset for applying glimmer.
/// @note Original Address: 0x800169ac
int GetLineGlimmerOffset(const short X, const short Y)
{
    // Get the absolute values of both X and Y.
    const short absX = X < 0 ? -X : X;
    const short absY = Y < 0 ? -Y : Y;

    // If ABS(X) > ABS(Y) then swap the index and divisor.
    const short index = absX > absY ? absY : absX;
    short divisor = absX > absY ? absX : absY;

    // Make sure divisor is not zero to avoid division by zero.
    divisor = divisor == 0 ? 1 : divisor;

    uint result = (uint)(&_GlimmerArray)[(index << 6) / divisor];

    // Determine an offset multiplier based on the sign value of both X and Y.
    int multiplier = (Y < 0)
        ? ((X < 0) ? 2 : 3)
        : ((X < 0) ? 1 : 0);

    // Determine the result from the glimmer array needs to be inverted.
    bool invert = absX >= absY;
    
    // Handle a specific case when both X and Y values are negative.
    if (X < 0 && Y < 0)
    {
        invert = absX < absY;
    }

    // When inverted, the multiplier also gets adjusted forward.
    if (invert) 
    {
        multiplier++;
        result *= -1;
    }

    // Adjust the result by a calculated offset.
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
void AddPrimitiveToList(PrimU0 *primitive)
{
    PrimitiveLinkedList *list = _PrimitiveLinkedList;
    PrimU0 *head = _PrimitiveLinkedList->Head;
    _PrimitiveLinkedList->Head = primitive;

    if (head != 0) 
    {
        head->Tag.addr = (u_long)primitive;
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
    int offset = GetLineGlimmerOffset(x - 256, y - 120);
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
    primitive->Tag.code = 0x50;

    primitive->x0 = X1;
    primitive->y0 = Y1;
    primitive->x1 = X2;
    primitive->y1 = Y2;

    SetVertexColor((RGBu8*)&primitive->Tag.r0, X1, Y1);
    SetVertexColor((RGBu8*)&primitive->r1, X2, Y2);

    AddPrimitiveToList((P_TAG *)primitive);
    _PrimitiveList = primitive + 1;
}











/// @brief Draws a blinking text arrow facing left or right.
/// @param position Position to draw on the screen.
/// @param timer Timer that counts every frame to be used for the pre-determined 16 frame intervals.
/// @param leftOrRight Determines if the arrow points left or right.
void DrawTextArrow(Vec3u32 *position, uint timer, int leftOrRight)
{
    Vec3u32 *destination;

    // Alternate from being visible, and not every 16 frames
    if ((timer & 31) < 16) {
        _ptr_hudMobys = _ptr_hudMobys + -1;

        memset(_ptr_hudMobys, 0, sizeof(Moby));

        destination = &_ptr_hudMobys->position;

        // Set Moby Type to the ascii number 1 for an arrow (clever)
        _ptr_hudMobys->type = 261;

        CopyVector3D(destination, position);

        if (leftOrRight < 2) {
            _ptr_hudMobys->rotation.x = 64;
            _ptr_hudMobys->rotation.z = (char)(leftOrRight << 7);
        }
                    /* Possible PsyQ macro: setSprt16() + setSemiTrans(sprt16, 1) + setShadeTex(sprt16, 1) */
        _ptr_hudMobys->requiredHUD1 = 0x7F;
        _ptr_hudMobys->color = 11;
        _ptr_hudMobys->requiredHUD2 = 0xFF;
    }
}


/**
 * @brief Draws a black textbox with a gold border. You can specify the bounds
 * @details Calls Draw primitive with the interesting hack they use for the alpha channel to achieve the black background, and also 4 draw lines for the border.

 * @param int xBound1 - the first x boundry of the box
 * @param int xBound2 - the second x boundry of the box
 * @param int yBound1 - the first y boundry of the box
 * @param int yBound2 - the second y boundry of the box

 * @note Function: DrawTextbox \n
 * Original Address: 0x8001860C \n
 * Hook File: draw_textbox.s \n
 * Prototype: draw_stuff.h \n
 * Amount of instructions: MORE IN MODDERN GCC (https://decomp.me/scratch/MrfVL) \n
*/
void DrawTextbox(int xBound1,int xBound2,int yBound1,int yBound2)
{
  Poly4FPadded* ptr_prim = (Poly4FPadded*)_ptr_primitivesArray;          

  SetDrawMode((DR_MODE*)_ptr_primitivesArray, 1, 0, 0x40, 0);
    

  AddPrimitiveToList((int)ptr_prim);
  ptr_prim->tag = 0x5000000;
  ptr_prim->code = POLY4F_TRANSPARENT;
  ptr_prim->point1Pos.x = xBound1;
  ptr_prim->point2Pos.x = xBound2;
  ptr_prim->point3Pos.x = xBound1;
  ptr_prim->point4Pos.x = xBound2;
  ptr_prim->point1Pos.y = yBound1;
  ptr_prim->point2Pos.y = yBound1;
  ptr_prim->point3Pos.y = yBound2;
  ptr_prim->point4Pos.y = yBound2;
  ptr_prim->color.R = 0x70;
  ptr_prim->color.G = 0x70;
  ptr_prim->color.B = 0x70;
  AddPrimitiveToList((int)((byte*)ptr_prim + 0xC));
  _ptr_primitivesArray = (int*)((byte*)ptr_prim + 0x24);            // Make space in the array of primitives for the next one

  // Outline of textbox
    DrawLine(xBound1,yBound1,xBound2,yBound1);
    DrawLine(xBound2,yBound1,xBound2,yBound2);
    DrawLine(xBound2,yBound2,xBound1,yBound2);
    DrawLine(xBound1,yBound2,xBound1,yBound1);
}
