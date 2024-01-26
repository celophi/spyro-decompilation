#pragma once

#include <custom_types.h>
#include <draw_stuff.h>

// This is a structure starting at WAD Archive 3, Section 3.
typedef struct {
    int Count;
    int FF_1;
    int FF_2;
    int FF_3;
    int FF_4;
    int Padding[8];
    int Offset;
    int ModelOffsets;
    byte R[262084];
} WA4S3_Head;

typedef struct {
    int First;
    int Second;
} WA4S3_TablePair;

typedef struct {
    ushort SubCount;
    short U2;
    byte Option;
    byte U3[3];
    int U10;
    int U11;
    int U12;
    int U13;
    int U14;
    int U15;
    int U16;
} WA4S3_Metadata;

typedef struct
{
    int offset;
    int count;
    int u1;
} WA4S2;

typedef struct
{
    byte U[256];
    int X;
} WA4S2_U0;

typedef struct
{
    int u0;
    int Count;
    int TableStart;
} WA4S2_T1;

typedef struct
{
    int u0;
    RGBu8 RGB;
    byte Padding;
    int Count;
    int TableStart;
} WA4S2_T2;

/// @brief Parses some WAD data and creates absolute addresses.
/// @param section 
/// @return 
WA4S3_Head* WA4S3_MakeAbsoluteAddresses(WA4S3_Head* section);

int* ReadWA4S2(WA4S2 *wa4s2, int option);

extern void OG_WA4S2_Loop(WA4S2_U0 *param_1,int option);