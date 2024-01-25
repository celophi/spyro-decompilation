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

/// @brief Not entirely sure on this, but I think it is some kind of coordinate information that is used to determine if a model should be ignored.
/// @details The XYZ values are used to perform RTPS and get a Z value that is compared to the cutoff member. The result of which determines if model should be processed.
typedef struct
{
    int XY;
    short Cutoff;
    short Z;
} ClipCalculation;
_Static_assert(sizeof(ClipCalculation) == 8);

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

/// @brief This is the header or start of an entire model for drawing sky primitives.
/// @note Following this structure immediately adjacent in memory are a VertexIndex array, ColorIndex array, and then a PolyMeta array.
typedef struct
{
    ClipCalculation ClipCalculation;
    short GX;
    short GY;
    short VertexCount;
    short GZ;
    PackedCount PC;
    int U6;
} SkyboxModel;
_Static_assert(sizeof(SkyboxModel) == 24);

/// @brief Represents a structure that indexes where primitive RGB colors should be located.
/// @details These are byte offsets into the WAD to find colors. They come immediately after vertex information.
typedef struct
{
    uint Color2 : 10;
    uint Color1 : 10;
    uint Color0 : 12;
} ColorIndex;
_Static_assert(sizeof(ColorIndex) == 4);

/// @brief Indexes that show where to find vertex information for primitives.
/// @details These are byte offsets into the WAD to find vertices. They come immediately after SkyboxModel data.
typedef struct
{
    uint XY2 : 10;
    uint XY1 : 10;
    uint XY0 : 12;
} VertexIndex;
_Static_assert(sizeof(VertexIndex) == 4);

/// @brief Describes a structure that contains offsets to vertex and color information for building polygon primitives.
/// @details These are structures in the WAD that come after color index information.
typedef struct
{
    VertexIndex PU;
    ColorIndex ColorIndex;
} PolyMeta;
_Static_assert(sizeof(PolyMeta) == 8);

/// @brief RGB structure that also includes padding.
typedef struct
{
    byte Red;
    byte Green;
    byte Blue;
    byte Padding;
} RGBu8P;
_Static_assert(sizeof(RGBu8P) == 4);

/// @brief Sky models that are loaded from the WAD.
/// @note Address: 0x80078a44
extern SkyboxModel** _SkyboxWadModels;

/// @brief Pointers that can be used to reference skybox WAD models directly instead of starting at the beginning of the WAD section.
/// @note Address: 0x80078a4c
extern byte** _SkyboxWadModelPointers;

/// @brief Number of sky models that exist within a loaded section of the WAD.
/// @note Address: 0x80077780
extern int _SkyboxWadModelCount;

/// @brief Sky models that have been selected to draw primitives from.
/// @details Models that are in view of the camera that we can draw primitives from.
/// @note Address: 0x8006fcf4
extern SkyboxModel* _SkyboxModelsToRender;

/// @brief Scratchpad start in RAM used for storing skybox vectors.
/// @note Address: 0x1f800000
extern BoundedVertex _ScratchpadDrawSkybox;

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

    if (vector->Y <= 0 || vector->X == 0 && vector->Y == 1)
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

static byte RunPerspectiveTransformations(SkyboxModel* skybox)
{
    uint vertexCount = skybox->VertexCount;
    PackedVertex* packs = (PackedVertex*)(skybox + 1);

    byte endFlags = 0x0F;
    BoundedVertex *storedVectors = &_ScratchpadDrawSkybox;

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
        uint xy2 = 0;
        gte_rtps_b();
        gte_stSXY2(xy2);

        // Convert this vector to a structure that has out of bounds information.
        BoundedVertex bv = ConvertToBoundedVector((Vec2s16*)&xy2);
            
        // Set flags based on the boundary metadata to make sure that a primitive can be drawn.
        endFlags = endFlags & bv.flags;

        // Save calculation to the scratchpad for later.
        *storedVectors++ = bv;
    }

    return endFlags;
}

/// @brief Adds primitives to a staging list.
/// @param polygon Primitive that gets added to the list.
/// @param current Unknown.
/// @param ptagMask Mask that needs to be applied to set the length of the primitive correctly.
static void StagePrimitive(P_TAG* polygon, P_TAG* current, uint ptagMask)
{
    P_TAG* head = _PrimitiveList;
    _PrimitiveList = polygon;

    if (polygon == current)
    {
        return;
    }

    // This is just setting the P_TAG length of the primitive and making certain that it doesn't clear the address.
    *(uint *)head = ptagMask ^ 0x80000000;

    // Add the primitive onto the staging linked list.
    P_TAG* tail = _PrimitiveStagingStart[0x7ff].Tail;
    if (tail == NULL) 
    {
        _PrimitiveStagingStart[0x7FF].Head = current;
    }
    else 
    {
        tail->addr = (u_long)current;
    }

    _PrimitiveStagingStart[0x7FF].Tail = head;
}

/// @brief Draws either a POLY_F3 or POLY_G3 triangle if vertices exist on screen.
/// @param metadata 
/// @param ptagMask 
/// @param polygon 
/// @param vertexEnd 
static void DrawTriangle(PolyMeta* metadata, uint* ptagMask, P_TAG** polygon, uint* vertexEnd)
{
    // The vertex indexes XY1 and XY2 have the last two bits cleared in the original code to prevent accessing on an unaligned address.
    // This is removed, but can be added back if needed: XY1 & 0xFFFFFFFC, and XY2 & 0xFFFFFFFC.
    VertexIndex* pu = &metadata->PU;
    BoundedVertex* scaledXy0 = &_ScratchpadDrawSkybox + (pu->XY0 / sizeof(uint));
    BoundedVertex* scaledXy1 = &_ScratchpadDrawSkybox + (pu->XY1 / sizeof(uint));
    BoundedVertex* scaledXy2 = &_ScratchpadDrawSkybox + (pu->XY2 / sizeof(uint));

    // Skip drawing a triangle if all of three vertices exist off screen.
    if ((scaledXy0->flags & scaledXy1->flags & scaledXy2->flags & 0x1F) != 0)
    {
        return;
    }
    
    // Restore the vectors back to the unscaled version that does not have embedded screen boundary data.
    Vec2s16 xy0 = ConvertToVector(scaledXy0);
    Vec2s16 xy1 = ConvertToVector(scaledXy1);
    Vec2s16 xy2 = ConvertToVector(scaledXy2);

    // The color indexes 1 and 2 have the last two bits cleared in the original code to prevent accessing on an unaligned address.
    // This is removed, but can be added back if needed: Color1 & 0xFFFFFFFC, and Color2 & 0xFFFFFFFC.
    ColorIndex* colorIndex = &metadata->ColorIndex;
    uint color0 = colorIndex->Color0;
    uint color1 = colorIndex->Color1;
    uint color2 = colorIndex->Color2;

    // The mask includes '8' in order to clear the address of the polygon.
    *(uint *)_PrimitiveList = *ptagMask ^ (uint)*polygon;
    _PrimitiveList = *polygon;

    // These are assigned here instead of the conditional because otherwise, test fails with a difference in memory.
    // It's probably a mistake from the developers where in the case of POLY_F3, additional bytes are added to the end.
    POLY_G3* polyG3 = (POLY_G3*)*polygon;

    polyG3->x0 = xy0.X;
    polyG3->y0 = xy0.Y;

    polyG3->x1 = xy1.X;
    polyG3->y1 = xy1.Y;
    
    polyG3->x2 = xy2.X;
    polyG3->y2 = xy2.Y;

    // POLY_F3 and POLY_G3 have overlapping memory, so this can be set once here.
    RGBu8P rgb = *(RGBu8P*)(vertexEnd + (color0 / sizeof(RGBu8P)));
    polyG3->tag.r0 = rgb.Red;
    polyG3->tag.b0 = rgb.Blue;
    polyG3->tag.g0 = rgb.Green;
    polyG3->tag.code = rgb.Padding;

    // If all colors are the same draw POLY_F3.
    if ((color0 == color1) && (color0 == color2)) 
    {
        POLY_F3* polyF3 = (POLY_F3*)*polygon;

        // The length of the primitive is 4.
        *ptagMask = 0x84000000;

        // I am not sure why this is necessary. What is going on here??
        polyF3->tag.code -= 0x10;
        
        polyF3->x0 = xy0.X;
        polyF3->y0 = xy0.Y;

        polyF3->x1 = xy1.X;
        polyF3->y1 = xy1.Y;

        polyF3->x2 = xy2.X;
        polyF3->y2 = xy2.Y;

        *polygon = (P_TAG*)(polyF3 + 1);
    }
    else 
    {
        // The length of the primitive is 6.
        *ptagMask = 0x86000000;

        rgb = *(RGBu8P*)(vertexEnd + (color1 / sizeof(RGBu8P)));

        polyG3->r1 = rgb.Red;
        polyG3->b1 = rgb.Blue;
        polyG3->g1 = rgb.Green;
        polyG3->pad1 = rgb.Padding;

        rgb = *(RGBu8P*)(vertexEnd + (color2 / sizeof(RGBu8P)));

        polyG3->r2 = rgb.Red;
        polyG3->b2 = rgb.Blue;
        polyG3->g2 = rgb.Green;
        polyG3->pad2 = rgb.Padding;

        *polygon = (P_TAG*)(polyG3 + 1);
    }
}


static void CreatePolygons()
{
    uint *vertexEnd;
    PackedCount* packedCount;
    SkyboxModel** modelsToRender = &_SkyboxModelsToRender;

    P_TAG *ptagB = (P_TAG *)((int)_PrimitiveList + 4);
    P_TAG* polygon = ptagB;
    uint ptagMask = 0;

    while (true)
    {
        while (true) 
        {
            SkyboxModel *model = *modelsToRender++;

            if (model == NULL) 
            {
                StagePrimitive(polygon, ptagB, ptagMask);
                return;
            }

            // Perform RTPS and if it results in at least some set of vertices that are visible, then move on to building polygons.
            // The comparison of 0xF is to handle out of bounds flags that can be set.
            if ((RunPerspectiveTransformations(model) & 0xF) == 0)
            {
                packedCount = &model->PC;
                PackedVertex* packs = (PackedVertex*)(model + 1);
                vertexEnd = (uint*)(packs + model->VertexCount);
                break;
            }
        }

        // Get the address of the end of all the vertices.
        PolyMeta* skyStart = (PolyMeta*)((int)vertexEnd + packedCount->VertexIndexStart);
        PolyMeta* skyCursor = skyStart;

        while(skyCursor != (PolyMeta*)(((uint*)skyStart) + packedCount->VertexIndexEnd * 2)) 
        {
            // Not sure what this is. Could it be prioritizing HUD primitives and making sure sky ones appear behind them?
            int unkPtag = _DrawSkyboxU4 - 0x400;
            if (unkPtag - (int)polygon < 1) 
            {
                _DrawSkyboxU3 = 1;
                StagePrimitive(polygon, ptagB, ptagMask);
                return;
            }

            DrawTriangle(skyCursor, &ptagMask, &polygon, vertexEnd);
            skyCursor++;
        }
    }
}

/// @brief Performs RTPS and stores sky models to a list for rendering if they are not clipped from view.
/// @param modelsToRender List of sky models that will need to be transformed into primitives.
/// @param model Sky model to process.
static void CalculateModelsToRender(SkyboxModel*** modelsToRender, SkyboxModel* model)
{
    // store 3 screen coordinates to non-continuous addresses. Store screen z 0, 1, 2
    int SZ3;

    gte_ldVXY0(model->ClipCalculation.XY);
    gte_ldVZ0(model->ClipCalculation.Z);
    gte_rtps_b();
    gte_stMAC3(SZ3);

    // This could be far/near plane clipping?
    // The SZ3 result needs to be greater than zero to draw; otherwise, clipping and distortion might happen?
    if (SZ3 - model->ClipCalculation.Cutoff > 0)
    {
        *(*modelsToRender)++ = model;
    }
}

/// @brief Draws the sky for cutscenes and levels.
/// @param index Used for specifying a selection of sky models to render.
/// @param cameraA 
/// @param cameraB 
void DrawSkybox(int index, RotationMatrix *cameraA, RotationMatrix *cameraB)
{   
    // Store registers. (This doesn't seem to have an effect on the game working, but it's needed for testing.)
    storeRegisters(&_RegisterStorage);

    // Load the camera and clear the translation vector to do clip calculations.
    LoadCameraMatrix(cameraA);
    gte_ldtr(0, 0, 0);

    // Create a list of models that will need to be rendered depending on the camera state.
    SkyboxModel** modelsToRender = &_SkyboxModelsToRender;
    
    if (index < 0)
    {
        SkyboxModel** cursor = _SkyboxWadModels;

        // Iterate through all models loaded from the WAD.
        while (cursor != (_SkyboxWadModels + _SkyboxWadModelCount))
        {
            SkyboxModel *model = *cursor++;
            CalculateModelsToRender(&modelsToRender, model);
        }
    }
    else 
    {
        byte* tableIndex = _SkyboxWadModelPointers[index];

        // Iterate through a select number of models loaded based on some kind of index.
        while (*tableIndex != 0xFF)
        {
            SkyboxModel *model = _SkyboxWadModels[*tableIndex];
            CalculateModelsToRender(&modelsToRender, model);
            tableIndex++;
        }
    }

    // Explitly set the end of the list of models to render.
    *modelsToRender = 0;
    LoadCameraMatrix(cameraB);
    CreatePolygons();
}