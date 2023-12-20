#pragma once

#include <custom_types.h>

typedef struct {
    unsigned addr: 24;
    unsigned len: 8;
    byte r0;
    byte g0;
    byte b0;
    byte code;
} P_TAG;

typedef struct {
    u_long tag;
    u_long code[2];
} DR_MODE;

typedef struct {
	short x;
    short y;
	short w;
    short h;
} RECT;

/// @brief Unconnected Gouraud Line
typedef struct 
{
	P_TAG Tag;
	short x0;
    short y0;
	byte r1;
    byte g1;
    byte b1;
    byte p1;
	short x1;
    short y1;
} LINE_G2;

extern void SetDrawMode(DR_MODE *p, int dfe, int dtd, int tpage, RECT *tw);