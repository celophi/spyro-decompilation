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
    P_TAG tag;
    short x0;
    short y0;
    u_char u0;
    u_char v0;
    u_short	clut;
    short x1;
    short y1;
    u_char u1;
    u_char v1;
    u_short	tpage;
    short x2;
    short y2;
    u_char u2;
    u_char v2;
    u_short	pad1;
    short x3;
    short y3;
    u_char u3;
    u_char v3;
    u_short	pad2;
} POLY_FT4;

typedef struct
{
    P_TAG tag;
    short x0;
    short y0;
    u_char r1;
    u_char g1;
    u_char b1;
    u_char pad1;
    short x1;
    short y1;
    u_char r2;
    u_char g2;
    u_char b2;
    u_char pad2;
    short x2;
    short y2;
} POLY_G3;

typedef struct
{
    P_TAG tag;
    short x0;
    short y0;
    short x1;
    short y1;
    short x2;
    short y2;
} POLY_F3;

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

/// @brief Transfer data to a frame buffer.
/// @param recp Pointer to destination rectangular area.
/// @param p Pointer to main memory address of source of transmission.
/// @return Position of this command in the libgpu command queue.
extern int LoadImage(RECT *recp, u_long *p);