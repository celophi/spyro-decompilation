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

/// @brief Defines flags that represent whether a vector is outside of the visibility of the camera.
enum OffscreenStatus
{
    OFFSCREEN_BOTTOM = 1,
    OFFSCREEN_TOP = 2,
    OFFSCREEN_LEFT = 4,
    OFFSCREEN_RIGHT = 8,
};
typedef byte OffscreenStatus;
_Static_assert(sizeof(OffscreenStatus) == 1);

/// @brief Represents a 2D vector that is upscaled in order to hold metadata about its visibiility.
/// @details The flags member requires 5 bits of space.
typedef union 
{
    uint vector;
    OffscreenStatus flags;
} BoundedVertex;
_Static_assert(sizeof(BoundedVertex) == 4);


/// @brief Scratchpad start in RAM used for storing skybox vectors.
/// @note Address: 0x1f800000
extern BoundedVertex _ScratchpadDrawSkybox;

typedef struct
{
    uint Y : 10;
    uint X : 11;
    uint Z : 11;
} PackedVertex;
_Static_assert(sizeof(PackedVertex) == 4);

typedef struct
{
    uint VertexIndexEnd : 13;
    uint U2 : 1;
    uint VertexIndexStart : 18;
} PackedCount;
_Static_assert(sizeof(PackedCount) == 4);

typedef struct
{
    PackedVertex PV1;
    int U2;
    short GX;
    short GY;
    short VertexCount;
    short GZ;
    PackedCount PC;
    int U6;
} SkyboxU0;
_Static_assert(sizeof(SkyboxU0) == 24);

typedef struct
{
    uint Color1 : 10;
    uint Color2 : 10;
    uint Color3 : 12;

} PolygonColors;
_Static_assert(sizeof(PolygonColors) == 4);

typedef struct
{
    uint XY2 : 10;
    uint XY1 : 10;
    uint XY0 : 12;

} VertexIndex;
_Static_assert(sizeof(VertexIndex) == 4);

/// @brief Describes a structure that contains offsets to vertex and color information for building polygon primitives.
typedef struct
{
    VertexIndex PU;
    PolygonColors Ins;
} PolyMeta;
_Static_assert(sizeof(PolyMeta) == 8);

typedef struct
{
    byte Red;
    byte Green;
    byte Blue;
    byte Padding;
} RGBu8P;
_Static_assert(sizeof(RGBu8P) == 4);

typedef struct
{
    int XY;
    short Clip;
    short Z;
} SkyVertex;

/// @brief Sky models that have been selected to draw primitives from.
/// @details Models that are in view of the camera that we can draw primitives from.
/// @note Address: 0x8006fcf4
extern SkyVertex* _SkyboxModelsToRender;

/// @brief Sky models that are loaded from the WAD.
/// @note Address: 0x80078a44
extern SkyVertex** _SkyboxWadModels;

/// @brief WAD archive 4, Section 2, Unknown 7
/// @note Address: 0x80078a4c
extern byte** _WA4S2_U7;


/// @brief Number of sky models that exist within a loaded section of the WAD.
/// @note Address: 0x80077780
extern int _SkyboxWadModelCount;

/// @brief Load a camera rotation matrix and translation vector.
/// @param camera Rotation matrix.
static void LoadCameraMatrix(RotationMatrix* camera)
{
    gte_ldR11R12(camera->R11R12);
    gte_ldR13R21(camera->R13R21);
    gte_ldR22R23(camera->R22R23);
    gte_ldR31R32(camera->R31R32);
    gte_ldR33(camera->R33);
}

/// @brief Converts a bounded vector to a normal 2D vector.
/// @details Restores a vector that was scaled up (in order to hold additional metadata) back to it's original value.
/// @param boundedVector Vector that contains metadata about its visibility.
/// @return Restored 2D vector.
static Vec2s16 ConvertToVector(BoundedVertex* boundedVector)
{
    int originalValue = (int)boundedVector->vector >> 5;
    return *(Vec2s16*)&originalValue;
}

/// @brief Converts a vector to a bounded vector with metadata about the offscreen visibility.
/// @param vector 2D vector that is scaled up by 32x in order to hold metadata about the offscreen coordinate positions.
/// @return Vector with out of bounds information.
static BoundedVertex ConvertToBoundedVector(Vec2s16* vector)
{
    // Scale the vector by 32 in or to make 5 bits to hold metadata about which coordinates are outside the camera range.
    BoundedVertex boundedVector = { .vector = *(uint*)vector << 5 };

    if (vector->Y <= 0)
    {
        boundedVector.flags |= OFFSCREEN_BOTTOM;
    }

    if (vector->Y >= 256)
    {
        boundedVector.flags |= OFFSCREEN_TOP;
    }

    if (vector->X <= 0)
    {
        boundedVector.flags |= OFFSCREEN_LEFT;
    }

    if (vector->X >= 512)
    {
        boundedVector.flags |= OFFSCREEN_RIGHT;
    }

    return boundedVector;
}



static byte RunPerspectiveTransformations(SkyboxU0* skybox)
{
    uint vertexCount = skybox->VertexCount;
    PackedVertex* packs = (PackedVertex*)(skybox + 1);

    BoundedVertex *storedVectors = &_ScratchpadDrawSkybox;
    byte endFlags = 0x0F;

    // Iterate through all vertices and calculate RTPS.
    // Something to think about, maybe doing RTPT would be faster.
    for (uint i = 0; i <= vertexCount; i++)
    {
        PackedVertex* pv = &packs[i];

        // Calculate the X, Y, Z component from global values.
        uint xComp = skybox->GX - pv->X;
        uint yComp = skybox->GY - pv->Y;
        uint zComp = skybox->GZ + pv->Z;

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

        uint badGuy = 0;
        gte_stSXY2(badGuy);

        // Convert this vector to a structure that has out of bounds information.
        BoundedVertex bv = ConvertToBoundedVector((Vec2s16*)&badGuy);
            
        // Set flags based on the boundary metadata to make sure that a primitive can be drawn.
        endFlags = endFlags & bv.flags;

        // Save calculation to the scratchpad for later.
        *storedVectors++ = bv;
    }

    return endFlags;
}

static void thing(int* polygon, P_TAG* ptagB, uint ptagMask, P_TAG* ptagA)
{
    int tail = (int)_PrimitiveList;
    PrimitiveLinkedList* stageStart = _PrimitiveStagingStart;
    _PrimitiveList = polygon;

    if ((P_TAG *)polygon != ptagB) 
    {
        ptagA = _PrimitiveStagingStart[0x7ff].Tail;
        *(uint *)tail = ptagMask ^ 0x80000000;
        stageStart[0x7FF].Tail = (P_TAG *)tail;

        if (ptagA == NULL) 
        {
            stageStart[0x7FF].Head = ptagB;
        }
        else 
        {
            ptagA->addr = (u_long)ptagB;
        }
    }
}


static void HandleInner(RotationMatrix *cameraB, SkyVertex** unkStorage)
{
    *unkStorage = 0;
    LoadCameraMatrix(cameraB);

    P_TAG *ptagA;
    uint *vertexEnd;

    
    PackedCount* packedCount;

    SkyVertex** skyVertexStorage = &_SkyboxModelsToRender;
    P_TAG *ptagB = (P_TAG *)((int)_PrimitiveList + 4);
    uint ptagMask = 0;
    int *polygon = (int *)ptagB;

    while (true)
    {
        while (true) 
        {
            SkyboxU0 *skybox = (SkyboxU0 *)*skyVertexStorage;
            skyVertexStorage++;

            if (skybox == NULL) 
            {
                thing(polygon, ptagB, ptagMask, ptagA);
                return;
            }

            byte endFlags = RunPerspectiveTransformations(skybox);

            if ((endFlags & 0xF) == 0)
            {
                packedCount = &skybox->PC;
                PackedVertex* packs = (PackedVertex*)(skybox + 1);
                vertexEnd = (uint*)(packs + skybox->VertexCount);
                break;
            }
        }

        // Get the address of the end of all the vertices.
        PolyMeta* skyStart = (PolyMeta*)((int)vertexEnd + packedCount->VertexIndexStart);
        PolyMeta* skyCursor = skyStart;

        while(skyCursor != (PolyMeta*)(((uint*)skyStart) + packedCount->VertexIndexEnd * 2)) 
        {
            int szCoords = _DrawSkyboxU4 - 0x400;
            if (szCoords - (int)polygon < 1) 
            {
                _DrawSkyboxU3 = 1;
                thing(polygon, ptagB, ptagMask, ptagA);
                return;
            }

            // The vertex indexes XY1 and XY2 have the last two bits cleared in the original code to prevent accessing on an unaligned address.
            // This is removed, but can be added back if needed: XY1 & 0xFFFFFFFC, and XY2 & 0xFFFFFFFC.
            VertexIndex* pu = &skyCursor->PU;
            BoundedVertex* scaledXy0 = &_ScratchpadDrawSkybox + (pu->XY0 / sizeof(uint));
            BoundedVertex* scaledXy1 = &_ScratchpadDrawSkybox + (pu->XY1 / sizeof(uint));
            BoundedVertex* scaledXy2 = &_ScratchpadDrawSkybox + (pu->XY2 / sizeof(uint));

            // The color indexes 1 and 2 have the last two bits cleared in the original code to prevent accessing on an unaligned address.
            // This is removed, but can be added back if needed: Color1 & 0xFFFFFFFC, and Color2 & 0xFFFFFFFC.
            PolygonColors* ins = &skyCursor->Ins;
            uint color1 = ins->Color1;
            uint color2 = ins->Color2;
            uint color3 = ins->Color3;

            // only checks lower 5 bits, Should make triangle?
            // Don't draw a triangle if all of three vertices exist off screen.
            if ((scaledXy0->flags & scaledXy1->flags & scaledXy2->flags & 0x1F) == 0) 
            {
                *(uint *)_PrimitiveList = ptagMask ^ (uint)polygon;
                _PrimitiveList = polygon;

                Vec2s16 xy0 = ConvertToVector(scaledXy0);
                Vec2s16 xy1 = ConvertToVector(scaledXy1);
                Vec2s16 xy2 = ConvertToVector(scaledXy2);

                // These are assigned here instead of the conditional because otherwise, test fails with a difference in memory.
                // It's probably a mistake from the developers where in the case of POLY_F3, additional bytes are added to the end.
                POLY_G3* polyG3 = (POLY_G3*)polygon;

                polyG3->x0 = xy0.X;
                polyG3->y0 = xy0.Y;

                polyG3->x1 = xy1.X;
                polyG3->y1 = xy1.Y;
                
                polyG3->x2 = xy2.X;
                polyG3->y2 = xy2.Y;

                // POLY_F3 and POLY_G3 have overlapping memory, so this can be set once here.
                RGBu8P rgb = *(RGBu8P*)(vertexEnd + (color3 / sizeof(RGBu8P)));
                polyG3->tag.r0 = rgb.Red;
                polyG3->tag.b0 = rgb.Blue;
                polyG3->tag.g0 = rgb.Green;
                polyG3->tag.code = rgb.Padding;

                // If all colors are the same draw POLY_F3.
                if ((color3 == color2) && (color3 == color1)) 
                {
                    POLY_F3* polyF3 = (POLY_F3*)polygon;

                    ptagMask = 0x84000000;

                    // I am not sure why this is necessary. What is going on here??
                    polyF3->tag.code -= 0x10;
                    
                    polyF3->x0 = xy0.X;
                    polyF3->y0 = xy0.Y;

                    polyF3->x1 = xy1.X;
                    polyF3->y1 = xy1.Y;

                    polyF3->x2 = xy2.X;
                    polyF3->y2 = xy2.Y;

                    polygon = (int*)(polyF3 + 1);
                }
                else 
                {
                    ptagMask = 0x86000000;

                    rgb = *(RGBu8P*)(vertexEnd + (color2 / sizeof(RGBu8P)));
                    polyG3->r1 = rgb.Red;
                    polyG3->b1 = rgb.Blue;
                    polyG3->g1 = rgb.Green;
                    polyG3->pad1 = rgb.Padding;

                    rgb = *(RGBu8P*)(vertexEnd + (color1 / sizeof(RGBu8P)));
                    polyG3->r2 = rgb.Red;
                    polyG3->b2 = rgb.Blue;
                    polyG3->g2 = rgb.Green;
                    polyG3->pad2 = rgb.Padding;

                    polygon = (int*)(polyG3 + 1);
                }
            }

            skyCursor++;
        }
    }
}

static void GetNextVertex()
{

}

void DrawSkybox(int index, RotationMatrix *cameraA, RotationMatrix *cameraB)
{   
    // Store registers. (This doesn't seem to have an effect on the game working, but it's needed for testing.)
    storeRegisters(&_RegisterStorage);

    LoadCameraMatrix(cameraA);

    // Clear the translation vector.
    gte_ldtr(0, 0, 0);

    SkyVertex** storage = &_SkyboxModelsToRender;
    SkyVertex** tableStart = _SkyboxWadModels;

    SkyVertex** tableEnd = NULL;
    byte* tableIndex = NULL;

    if (index < 0)
    {
        tableEnd = _SkyboxWadModels + _SkyboxWadModelCount;
    }
    else 
    {
        tableIndex = _WA4S2_U7[index];
    }

    while (true) 
    {
        SkyVertex *vertex;

        if (index < 0) 
        {
            vertex = *tableStart; // first entry

            if (tableStart == tableEnd) 
            {
                break;
            }

            tableStart++;
        }
        else 
        {
            if (*tableIndex == 0xFF)
            {
                break;
            }

            vertex = tableStart[*tableIndex];
            tableIndex++;
        }

        gte_ldVXY0(vertex->XY); //x = 0x03ec, y = 0x0072
        gte_ldVZ0(vertex->Z); // z = 0xff5b

        // coordinate transformation and perspective transformation
        gte_rtps_b();

        // store 3 screen coordinates to non-continuous addresses. Store screen z 0, 1, 2
        int result;

        // SSZ = TRZ + R31*VX0 + R32*VY0 + R33*VZ0; <3> = MAC3
        gte_stMAC3(result); // 0x38d

        // This could be far/near plane clipping?
        // The SZ3 result needs to be greater than zero to draw; otherwise, clipping and distortion might happen?
        if (result - vertex->Clip > 0) // 0x01FF
        {
            *storage++ = vertex; // addr of the entry, place it unknown storage?
        }
    }

    HandleInner(cameraB, storage);
}