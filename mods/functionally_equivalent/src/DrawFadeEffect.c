#include <custom_types.h>
#include <symbols.h>
#include <PSYQ/LIBGPU.h>
#include <draw_stuff.h>

typedef struct 
{
    DR_MODE DR_MODE;
    POLY_F4 POLY_F4;
} SemiTransPolyF4;

/// @brief Draws a primitive with opacity to simulate a fade in / fade out effect.
/// @details Adds POLY_F4 with transparency and DRAW_MODE primitives to the screen.
///     - Address: 0x800190d4
///     - Hook: DrawFadeEffect.s
///     - Test: DrawFadeEffectTest.c
///     - Test Status: Passing
/// @param tpage
/// @param red 
/// @param green
/// @param blue
void DrawFadeEffect(int tpage, byte red, byte green, byte blue)
{
    SemiTransPolyF4* primitive = (SemiTransPolyF4*)_PrimitiveList;
    int scaledTpage = tpage << 5;

    SetDrawMode(&primitive->DR_MODE, 1, 0, scaledTpage, NULL);
    AddPrimitiveToList((P_TAG*)primitive);

    primitive->POLY_F4.Tag.len = 5;
    primitive->POLY_F4.Tag.addr = 0;
    primitive->POLY_F4.Tag.code = 42;
    primitive->POLY_F4.Tag.r0 = red;
    primitive->POLY_F4.Tag.g0 = green;
    primitive->POLY_F4.Tag.b0 = blue;

    primitive->POLY_F4.x0 = 0;
    primitive->POLY_F4.y0 = 8;

    primitive->POLY_F4.x1 = 512;
    primitive->POLY_F4.y1 = 8;

    primitive->POLY_F4.x2 = 0;
    primitive->POLY_F4.y2 = 232;

    primitive->POLY_F4.x3 = 512;
    primitive->POLY_F4.y3 = 232;
    
    AddPrimitiveToList((P_TAG *)&primitive->POLY_F4);
    _PrimitiveList = primitive + 1;
}