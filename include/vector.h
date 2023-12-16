#pragma once


//~~~~~~~
//Structs
//~~~~~~~

typedef struct Vector3D
{
    int X;
    int Y;
    int Z;
} Vector3D;

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

void Vec3Subtract(Vector3D* destVector, Vector3D* a, Vector3D* b);

void Vec3Scale(Vector3D* result, Vector3D* vector, int multiplier);                 //Scales the s

void Vec3ScaleDownByPowerOfTwo(Vector3D* vector, unsigned int pot);

void Vec3ScaleDownBy4(Vec3_u16 result, Vector3D* source);                        //Scales down the source vector by 4, then stores it in a short vector. 

void Vec3ScaleByFractionPowerOfTwo(Vector3D* param_1, int numerator, unsigned int denominatorExponent);               //80017614     
void Vec3ScaleByFraction(Vector3D* param_1, int denominator, int numerator);

unsigned int Vec3CalculateLengthE(Vector3D* vector, bool flag3D);

void Vec3ApplyRotationMatrix(int* rotationMatrix, unsigned int* inputVector, int* outputVector);

void MatrixTranspose(unsigned int *dest, unsigned int *src);

void Vec3Swap(int *param_1,int *param_2);

/// @brief Copies the source vector to the destination vector.
/// @param destination Destination vector to get copied to.
/// @param source Source vector to copy from.
void CopyVector3D(Vector3D* destination, Vector3D* source);