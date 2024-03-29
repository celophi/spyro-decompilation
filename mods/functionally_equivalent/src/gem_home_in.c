#include <custom_types.h>
#include <common.h>
#include <shapes.h>
#include <moby.h>
#include <vector.h>
#include <spyro.h>




//*********************************************
// ** Parent Function: Artisans_MobyUpdateLoop
// ** Scope Replacing: If-Else for Gem Home In
// ** Address of Scope Replacing: 0x8008013c
// ** Need to redirect after return?: Yes - 80080218
// ** Hook File: gem_home_in_hook.s 
//*********************************************
/*
 * Part of function that handles the gem home in
*/
void GemHomeIn()
{
    //Define Registers as variables
    register void* s1 asm("$s1");
    register void* s3 asm("$s3");

    Vec3u32 gemToSpyroDistanceVec = {0};
    unsigned int gemOriginToSpyroLength = 0;

    //* Since we are replacing a branch (if), this is that equivalent branch
    if (*((char*)s1 + 0xf) < 0x20)                                                                          //! If gem is homing but not finished
    {
        Vec3Subtract(&gemToSpyroDistanceVec, &_spyro.position, (Vec3u32*)s1);                                  // Subtract spyro's position from gem origin to get distance vec
        Vec3ScaleDownByPowerOfTwo(&gemToSpyroDistanceVec, 5);                                               // Scale down result by 32
        const int FLAG_3D = 1;
        gemOriginToSpyroLength = Vec3CalculateLengthE(&gemToSpyroDistanceVec, FLAG_3D);                     // Calculate length of distance vector
        
        const int MINIMUM_DISTANCE_FROM_GEM = 0x1e1;

        if (gemOriginToSpyroLength < MINIMUM_DISTANCE_FROM_GEM) 
        {                                                             
        Vec3Scale(&gemToSpyroDistanceVec,&gemToSpyroDistanceVec, *((char*)s1 + 0xf));
        AddVector3D(&((Moby*)s3)->position, s1, &gemToSpyroDistanceVec);                                        // Add origin to distance vec to get new location from 0,0,0
        const int SINE_WAVE_SPEED = 4   ;                                                                        // 4 is the speed required to get 1 half circle (128 steps through the 256 array)
        ((Moby*)s3)->position.Z = ((Moby*)s3)->position.Z + (_sinArray[*((char*)s1 + 0xf) * SINE_WAVE_SPEED] / 12);
        }

        else 
        {
        CopyVector3D(&((Moby*)s3)->position, &_spyro.position);                                                 // Set gem position to spyro's position
        *((char*)s1 + 0xf) = 0x20;                                                                          // Set gem hoam timer to near end
        }
    }

    else
    {
        CopyVector3D(&((Moby*)s3)->position, &_spyro.position);   
    }
    return;
}
