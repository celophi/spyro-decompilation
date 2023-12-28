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

/// @brief Flat Quadrangle
typedef struct 
{
	P_TAG Tag;
	short x0;
    short y0;
	short x1;
    short y1;
	short x2;
    short y2;
	short x3;
    short y3;
} POLY_F4;

typedef struct {
	u_long	tag;
	u_long	code[15];
} DR_ENV;

typedef struct {
    RECT clip;
    short ofs[2];
    RECT tw;
    ushort tpage;
    u_char dtd;
    u_char dfe;
    u_char isbg;
    u_char r0;
    u_char g0;
    u_char b0;
    DR_ENV	dr_env;
} DRAWENV;

typedef struct {
    RECT disp;
    RECT screen;
    u_char isinter;
    u_char isrgb24;
    u_char pad0;
    u_char pad1;
} DISPENV;

extern void SetDrawMode(DR_MODE *p, int dfe, int dtd, int tpage, RECT *tw);

/// @brief Set and cancel display mask.
/// @note Original Address: 0x8005f6c8
/// @param mask 
extern void SetDispMask(int mask);

/// @brief Initialize drawing engine.
/// @note Original Address: 0x8005f2a4
/// @param mode 
/// @return 
extern int ResetGraph(int mode);

/// @brief Set debugging level.
/// @note Original Address: 0x8005f53c
/// @param level 
/// @return 
extern int SetGraphDebug(int level);

/// @brief Set standard drawing environment structure.
/// @note Original Address: 0x8005ea94
/// @param env 
/// @param x 
/// @param y 
/// @param w 
/// @param h 
/// @return 
extern DRAWENV *SetDefDrawEnv(DRAWENV *env, int x, int y, int w, int h);

/// @brief Set display environment structure members and screen display area.
/// @note Original Address: 0x8005eb4c
/// @param env 
/// @param x 
/// @param y 
/// @param w 
/// @param h 
/// @return 
extern DISPENV *SetDefDispEnv(DISPENV *env, int x, int y, int w, int h);

/// @brief Clear Frame Buffer at high speed.
/// @note Original Address: 0x8005f8f8
/// @param rect 
/// @param r 
/// @param g 
/// @param b 
/// @return 
extern int ClearImage(RECT *rect, u_char r, u_char g, u_char b);

/// @brief Wait for all drawing to terminate.
/// @note Original Address: 0x8005f764
/// @param mode 
/// @return 
extern int DrawSync(int mode);