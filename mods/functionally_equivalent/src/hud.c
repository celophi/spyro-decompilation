#include <common.h>
#include <symbols.h>
#include <moby.h>
#include <hud.h>
#include <draw_stuff.h>
#include <draw_text.h>

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
 * @param vg Unknown. Something with vertices.
 * @param parameter Unknown. Something to do with textures.
 * @param rgb Colors to set on the primitive.
*/
void DrawHudOval(const HudLifeRelated_U0* vg, const TextureRelatedUnk* parameter, const RGBu32* rgb)
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


void DisplayHudRelated()

{
  NewMoby **ppNVar1;
  uint count2;
  int counter;
  int *mobyRef;
  RGBu32 rgb;
  Vec3u32 position;
  HudLifeRelated_U0 *hud0;
  NewMoby *mobyPtr;
  
  if (_DisplayHudRelated_U1 == 0) {
    mobyRef = &_DSM_U2;
    if (_DSM_U2 != 0) {
      ppNVar1 = (NewMoby **)&_DisplayHudRelated_U0;
      do {
        mobyRef = (int *)ppNVar1;
        ppNVar1 = (NewMoby **)mobyRef + 1;
      } while ((NewMoby *)*mobyRef != (NewMoby *)0x0);
    }
    if (_HudChestState != 0) {
      count2 = (uint)(_HudChestState == 4) * 4;
      if (count2 < 5) {
        mobyPtr = _PauseMenuState.Mobys + (uint)(_HudChestState == 4) * 4;
        do {
          *mobyRef = (int)mobyPtr;
          mobyRef = (int *)((NewMoby **)mobyRef + 1);
          count2 = count2 + 1;
          mobyPtr = mobyPtr + 1;
        } while ((int)count2 < 5);
      }
      if (_HudChestState == 4) {
        sprintf((char *)&rgb,_TreatureObtainedRatio,_HudLevelTreasure,_HudLevelTreasure);
        mobyPtr = _MobyList;
        position.X = 0x5a;
        position.Y = 0x24;
        position.Z = 0xb40;
        DrawCapitalText(&rgb,&position,28,11);
        mobyPtr = mobyPtr + -1;
        counter = 0;
        if ((int)_MobyList <= (int)mobyPtr) {
          do {
            count2 = _DisplayHudRelated_U2 * 4 + counter;
            counter = counter + 0xc;
            mobyPtr->Rotation = (byte)(_SinArray[(count2 & 0xff) + 0x40] >> 7);
            *mobyRef = (int)mobyPtr;
            mobyPtr = mobyPtr + -1;
            mobyRef = (int *)((NewMoby **)mobyRef + 1);
          } while ((int)_MobyList <= (int)mobyPtr);
        }
      }
    }
    mobyPtr = _PauseMenuState.Mobys + 5;
    if (_HudDragonState != 0) {
      counter = 0;
      do {
        *mobyRef = (int)mobyPtr;
        mobyRef = (int *)((NewMoby **)mobyRef + 1);
        counter = counter + 1;
        mobyPtr = mobyPtr + 1;
      } while (counter < 3);
    }
    if (_HudLivesState != 0) {
      counter = 0;
      mobyPtr = _PauseMenuState.Mobys + 8;
      do {
        *mobyRef = (int)mobyPtr;
        mobyRef = (int *)((NewMoby **)mobyRef + 1);
        counter = counter + 1;
        mobyPtr = mobyPtr + 1;
      } while (counter < 3);
    }
    if ((_HudAnimationState != 0) && (_DisplayHudRelated_U4 == 1)) {
      *mobyRef = (int)(_PauseMenuState.Mobys + 0xb);
      mobyRef = (int *)((NewMoby **)mobyRef + 1);
    }
    *mobyRef = 0;
  }
  if ((_HudLivesState != 0) && (counter = 0, 0 < _HudLifeOrbs)) {
    hud0 = &_HudUnk0;
    do {
      rgb.R = ((int)((uint)_SinArray[(_CyclingTimer - (counter << 8) / 0x14 & 0xffU) + 0x40] << 0x10) >> 0x17) + 0x80;
      rgb.G = rgb.R;
      rgb.B = rgb.R;
      DrawHudOval(hud0,&_HudUnk1,&rgb);
      counter = counter + 1;
      hud0 = hud0 + 1;
    } while (counter < _HudLifeOrbs);
  }
  if ((_HudEggsState != 0) && (counter = 0, 0 < _HudEggs)) {
    hud0 = (HudLifeRelated_U0 *)&_LifeOrbVertices;
    do {
      DrawHudOval(hud0,(TextureRelatedUnk *)(_HudUnk1.U1 + (_HudChestHeadSpinTimer + counter) % 9 + 1),(RGBu32 *)0x0);
      counter = counter + 1;
      hud0 = hud0 + 1;
    } while (counter < _HudEggs);
  }
  return;
}

