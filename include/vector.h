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

void ScaleVector(Vec3u32* vector, int denominator, int numerator);
extern void OriginalScaleVector(Vec3u32* vector, int denominator, int numerator);