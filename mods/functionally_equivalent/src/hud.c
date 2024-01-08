#include <common.h>
#include <symbols.h>
#include <moby.h>
#include <hud.h>
#include <draw_stuff.h>
#include <text.h>

/**
 * @brief Constructs a new POLY_FT4 with the correct P_TAG information.
 */ 
static POLY_FT4* CreatePOLY_FT4() 
{
    POLY_FT4* poly = _PrimitiveList;

    poly->tag.code = 0x2C;
    poly->tag.len = 9;
    poly->tag.addr = 0;

    return poly;
}

/**
 * @brief Draws an ellipsoid/ovoid on the HUD.
 * @details Used for drawing the life orbs and the eggs.
 * @note
 *      - Address: 0x8001919c
 *      - Hook: DrawHudOval.s
 *      - Test: DrawHudOvalTest.c
 *      - Test Status: Passing
 * @param vg Unknown. Something with vertices.
 * @param parameter Unknown. Something to do with textures.
 * @param rgb Colors to set on the primitive.
*/
void DrawHudOval(const HudOvalVertex* vg, const TextureRelatedUnk* parameter, const RGBu32* rgb)
{
    POLY_FT4* polyFT4 = CreatePOLY_FT4();

    if (rgb) 
    {
        polyFT4->tag.r0 = (u_char)rgb->R;
        polyFT4->tag.g0 = (u_char)rgb->G;
        polyFT4->tag.b0 = (u_char)rgb->B;
    }
    else 
    {
        polyFT4->tag.b0 = 128;
        polyFT4->tag.r0 = 128;
        polyFT4->tag.g0 = 128;
    }

    // Set vertices
    polyFT4->x0 = (vg->V1).X;
    polyFT4->y0 = (vg->V1).Y;

    polyFT4->x1 = (vg->V1).X + (vg->V2).X;
    polyFT4->y1 = (vg->V1).Y;

    polyFT4->x2 = (vg->V1).X;
    polyFT4->y2 = (vg->V1).Y + (vg->V2).Y;
    
    polyFT4->x3 = (vg->V1).X + (vg->V2).X;
    polyFT4->y3 = (vg->V1).Y + (vg->V2).Y;

    // Set texture coordinates.
    polyFT4->u0 = parameter->U1[0].u0;
    polyFT4->v0 = parameter->U1[0].v0;

    /**
     * The line below is removed because it gets overwritten immediately. 
     * I'm not sure if this is a mistake or not, but it seems like the right code might have been:
     * 
     * polyFT4->u1 = parameter->U1[0].u2;
     * polyFT4->u3 = polyFT4->u0 + *(char *)&(vg->V2).X;
     * 
     * However, this results in some diamond shape instead of a circle.
    */

    //polyFT4->u1 = parameter->U1[0].u2;

    polyFT4->u1 = polyFT4->u0 + *(char *)&(vg->V2).X;
    polyFT4->v1 = parameter->U1[0].v2;

    polyFT4->u2 = polyFT4->u0;
    polyFT4->v2 = polyFT4->v0 + *(char *)&(vg->V2).Y;

    polyFT4->u3 = polyFT4->u1;
    polyFT4->v3 = polyFT4->v0 + *(char *)&(vg->V2).Y;

    polyFT4->clut = parameter->U1[0].clut;
    polyFT4->tpage = parameter->U1[0].tpage;

    AddPrimitiveToList((P_TAG *)polyFT4);
    _PrimitiveList = polyFT4 + 1;
}

/**
 * @brief Draws Mobys on the HUD.
 * @details Mobys for the treasure, lives, life orbs, eggs, dragons, are drawn.
 * @note
 *      - Address: 0x80019300
 *      - Hook: DrawHudMobys.s
 *      - Test: DrawHudMobysTest.c
 *      - Test Status: Passing
*/
void DrawHudMobys()
{   
    if (!_IsInFlyingLevel) 
    {
        NewMoby** mobyRef = &_DSM_U2;
    
        if (_DSM_U2) 
        {
            NewMoby **pRef = &_HudMobyListHead;

            while (*mobyRef)
            {
                mobyRef = pRef;
                pRef = mobyRef + 1;
            }
        }

        NewMoby* mobyPtr = _PauseMenuState.Mobys;
            
        // Displays the chest icon and treature / treasure text in upper left.
        if (_HudChestState == ALL_COLLECTIBLES_OBTAINED) 
        {
            mobyPtr +=4;
            *mobyRef++ = mobyPtr++;

            char treatureObtainedText;
            sprintf(&treatureObtainedText, &_TreatureObtainedRatio, _HudLevelTreasure, _HudLevelTreasure);

            NewMoby* reference = _MobyList;

            Vec3u32 position = {.X = 90, .Y = 36, .Z = 2880 };
            DrawCapitalText(&treatureObtainedText, &position, 28, 11);

            reference--;
            int sinStep = 0;

            while (reference >= _MobyList)
            {
                uint sinIndex = _HudChestTransitionTimer * 4 + sinStep;
                sinStep += 12;

                reference->Rotation = (byte)(_SinArray[(sinIndex & 0xFF) + 64] >> 7);
                *mobyRef++ = reference--;
            }
        }
        else if (_HudChestState != HIDDEN)
        {
            for (int i = 0; i < 5; i++)
            {
                *mobyRef++ = mobyPtr++;
            }
        }

        NewMoby* mobyPtr2 = _PauseMenuState.Mobys + 5;

        if (_HudDragonState != 0) 
        {
            for (int i = 0; i < 3; i++)
            {
                *mobyRef++ = mobyPtr2++;
            }
        }

        if (_HudLivesState != 0) 
        {
            mobyPtr2 = _PauseMenuState.Mobys + 8;

            for (int i = 0; i < 3; i++)
            {
                *mobyRef++ = mobyPtr2++;
            }
        }

        if ((_HudKeyState != HIDDEN) && (_DisplayHudRelated_U4 == 1)) 
        {
            *mobyRef = _PauseMenuState.Mobys + 11;
            mobyRef++;
        }

        *mobyRef = 0;
    }

    // Draw all the life orbs on the HUD.
    if (_HudLivesState != 0) 
    {
        HudOvalVertex *vertices = _HudLifeOrbVertices;
        
        for (int i = 0; i < _HudLifeOrbs; i++)
        {
            int color = ((int)((uint)_SinArray[(_CyclingTimer - (i << 8) / 20 & 0xFFU) + 64] << 16) >> 23) + 128;
            RGBu32 rgb = { .R = color, .G = color, .B = color };

            DrawHudOval(vertices, &_HudUnk1, &rgb);
            vertices++;
        }
    }

    // Draw all the eggs on the HUD.
    if (_HudEggsState != 0) 
    {
        HudOvalVertex *vertices = _HudEggVertices;

        for (int i = 0; i < _HudEggs; i++)
        {
            DrawHudOval(vertices, (TextureRelatedUnk *)(_HudUnk1.U1 + (_HudChestHeadSpinTimer + i) % 9 + 1), NULL);
            vertices++;
        }
    }
}

