#include <common.h>
#include <moby.h>
#include <vector.h>
#include <custom_types.h>
#include <memory.h>
#include <symbols.h>

/**
 * @brief Copies generated moby structs to the end of a global list.
 * @details Appears to be something that is called often when working with text.
 
 * @note Function: CopyHudToShaded \n
   Original Address: 0x80018880 \n
   Hook File: copy_hud_to_shaded.s \n
   Prototype: draw_text.h \n
   Amount of instructions: More (https://decomp.me/scratch/rbY0g) \n
  * @see CopyHudToShaded()
*/
void CopyHudToShaded() {
  Moby **entry = &_ptr_hudMobysHead;
  Moby **ref = &_ptrTextUnk;

  while (*ref != 0) {                                                           // iterate to get to the end of the list.
    ref = entry;
    entry = ref + 1;
  }

  while (_ptr_hudMobys != _ptr_hudMobysQueue) {                                 // append all of the new mobys
    *ref = _ptr_hudMobys;
    _ptr_hudMobys += 1;
    ref += 1;
  }

  *ref = 0;                                                                     // make sure to set the end of the list to zero for iteration to work on the next call to this function. 
}

/**
 * @brief Draws the demo text on the screen.
 * @details Creates HUD mobys for displaying the text "Demo Mode" with varying size text.
 
 * @note Function: DrawDemoText \n
   Original Address: 0x80018908 \n
   Hook File: draw_demo_text.s \n
   Prototype: draw_text.h \n
   Amount of instructions: More (https://decomp.me/scratch/vU390) \n
  * @see DrawDemoText()
*/
void DrawDemoText() {
  Vec3u32 capitalTextInfo = { .X = 199, .Y = 200, .Z = 4352 };
  Vec3u32 lowercaseTextInfo = { .X = 16, .Y = 1, .Z = 5120 };
  const byte spacing = 18;
  const char color = 2;
  const int sinArrayIncrement = 12;

  Moby* mobyPtr = _ptr_hudMobys;
  unsigned int sinArrayIndex = 0;
  
  DrawVaryingText("DEMO MODE", &capitalTextInfo, &lowercaseTextInfo, spacing, color);
  mobyPtr--;
  
  while (_ptr_hudMobys <= mobyPtr) {
    mobyPtr->rotation.z = _sinArray[_levelTimer_60fps * 4 + sinArrayIndex & 0xFF] >> 7;
    mobyPtr--;
    sinArrayIndex += sinArrayIncrement;
  }

  CopyHudToShaded();
}