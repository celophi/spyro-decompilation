#pragma once

#include <custom_types.h>

typedef struct
{
    int X;
    int Y;
    int Z;
} Vec3u32;

typedef struct
{
    ushort X;
    ushort Y;
} Vec2u16;
_Static_assert(sizeof(Vec2u16) == 4);

typedef struct
{
    short X;
    short Y;
} Vec2s16;
_Static_assert(sizeof(Vec2s16) == 4);

typedef struct
{
    byte X;
    byte Y;
    byte Z;
} Vec3u8;
_Static_assert(sizeof(Vec3u8) == 3);

//~~~~~~~
//Structs
//~~~~~~~



typedef struct Vec2
{
    int x;
    int y;

}Vec2;

typedef struct Vec3Padding
{
    int x;
    int y;
    int z;
    int padding;
}Vec3Padding;

typedef struct Vec3_u16
{
    unsigned short x;
    unsigned short y;
    unsigned short z;
}Vec3_u16;

typedef struct Vec3_u8
{
    unsigned char x;
    unsigned char y;
    unsigned char z;
}Vec3_u8;


typedef struct Vec2_u16
{
    unsigned short x;
    unsigned short y;
}Vec2_u16;

typedef struct Vec2_u8
{
    unsigned char x;
    unsigned char y;
}Vec2_u8;

typedef struct Angle
{
    int yaw;
    int pitch;
    int roll;
}Angle;

typedef struct Angle_u16
{
    short yaw;
    short pitch;
    short roll;
}Angle_u16;

typedef struct SpyroHeadAngle
{
    short roll;
    short pad1;

    short pitch;
    short pad2;

    short yaw;
    short pad3;
}SpyroHeadAngle;;

typedef struct CameraAngle
{
    short roll;
    short pitch;
    short yaw;
}CameraAngle;

typedef struct Angle_u8
{
    char yaw;
    char pitch;
    char roll;
}Angle_u8;

typedef struct MATRIX3BY3
{

    short matrix[3][3];

}MATRIX3BY3;

/// @brief Matrix where each value in the structure may hold two coefficients.
/// @details The bits 0-15 align to one value, and 16-31 the other.
typedef struct 
{
    int R11R12;
    int R13R21;
    int R22R23;
    int R31R32;
    int R33;
} RotationMatrix;

/**
 * @brief Scales a vector by a ratio.
 * @details For example, the vector (0, 1511, -686) scaled by denominator 1659 and numerator 1024 results in (1024/1659) * 1511 = 932 and (1024/1659) * -686 = -424.
 * @note 
 *     - Address: 0x800175b8
 *     - Hook: ScaleVector.s
 *     - Test: ScaleVectorTest.c
 * @param vector Input vector to be scaled.
 * @param denominator Bottom half of the ratio.
 * @param numerator Top half of the ratio.
 * @return Scaled vector.
 */
void ScaleVector(Vec3u32* vector, int denominator, int numerator);

/**
 * @brief Calculates the length of a 2D or 3D vector using an adaptation of Newton's iteration using a lookup table.
 * @details Please refer to:
 *      - https://github.com/python/cpython/blob/main/Modules/mathmodule.c
 *      - https://stackoverflow.com/questions/65986056/is-there-a-non-looping-unsigned-32-bit-integer-square-root-function-c
 *      - https://stackoverflow.com/questions/15390807/integer-square-root-in-python
 * @note 
 *     - Address: 0x800171fc
 *     - Hook: GetVectorLength.s
 *     - Test: GetVectorLengthTest.c
 * @param vector Input vector to calculate length.
 * @param includeZ When true, indicates that the vector is 3D and the 'Z' coordinate should be included.
 * @return Length of the vector.
*/
uint GetVectorLength(const Vec3u32* vector, const int includeZ);

/**
 * @brief Calculates the arctan of a vector to get the estimated angle.
 * @details The returned value is a number where 64 = 90 degrees, and 256 = 360 degrees.
 * @note
 *     - Address: 0x800169ac
 *     - Hook: GetAngle.s
 *     - Test: GetAngleTest.c
 * @param x Position of the X component of a vertex.
 * @param y Position of the Y component of a vertex.
 * @return Angle based on a number aligned with 64 = 90 degrees.
*/
int GetAngle(const int x, const int y);

/**
 * @brief Rotates a vector by a matrix.
 * @note
 *      - Address: 0x80017048
 *      - Hook: RotateVectorByMatrix.s
 *      - Test: RotateVectorByMatrixTest.c
 * @param matrix Rotational matrix to apply to the vector.
 * @param input Vector to apply the rotation to.
 * @param output Resulting vector after rotation is applied.
*/
void RotateVectorByMatrix(const RotationMatrix* matrix, Vec3u32* input, Vec3u32* output);

//~~~~~~~~~
//In Game Functions
//~~~~~~~~~

void Vec3Subtract(Vec3u32* destVector, Vec3u32* a, Vec3u32* b);

void Vec3Scale(Vec3u32* result, Vec3u32* vector, int multiplier);                 //Scales the s

void Vec3ScaleDownByPowerOfTwo(Vec3u32* vector, unsigned int pot);

void Vec3ScaleDownBy4(Vec3_u16 result, Vec3u32* source);                        //Scales down the source vector by 4, then stores it in a short vector. 

void Vec3ScaleByFractionPowerOfTwo(Vec3u32* param_1, int numerator, unsigned int denominatorExponent);               //80017614     
void Vec3ScaleByFraction(Vec3u32* param_1, int denominator, int numerator);

unsigned int Vec3CalculateLengthE(Vec3u32* vector, bool flag3D);

void Vec3ApplyRotationMatrix(int* rotationMatrix, unsigned int* inputVector, int* outputVector);

void MatrixTranspose(unsigned int *dest, unsigned int *src);

void Vec3Swap(int *param_1,int *param_2);

/// @brief Adds two vectors together and puts the result in the destination.
/// @param destination Resulting vector.
/// @param first First vector as part of the sum.
/// @param second Second vector as part of the sum.
void AddVector3D(Vec3u32* destination, Vec3u32* first, Vec3u32* second);

/// @brief Copies the source vector to the destination vector.
/// @param destination Destination vector to get copied to.
/// @param source Source vector to copy from.
void CopyVector3D(Vec3u32* destination, Vec3u32* source);

